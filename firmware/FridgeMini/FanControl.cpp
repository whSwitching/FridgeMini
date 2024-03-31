
#include "FanControl.h"

bool FanControl::ISR_INSTALLED = false;

FanControl::FanControl()
{
    pcnt_evt_queue = xQueueCreate(pcnt_evt_queue_SIZE, sizeof(pcnt_evt_t));
}

FanControl::~FanControl()
{
    if ((_sensorPCntUnit >= PCNT_UNIT_0 && _sensorPCntUnit < PCNT_UNIT_MAX) &&
        (_sensorPCntChannel >= PCNT_CHANNEL_0 || _sensorPCntChannel < PCNT_CHANNEL_MAX))
    {
        pcnt_counter_pause(_sensorPCntUnit);
        pcnt_counter_clear(_sensorPCntUnit);
        pcnt_filter_disable(_sensorPCntUnit);
        pcnt_event_disable(_sensorPCntUnit, PCNT_EVT_H_LIM);
        pcnt_isr_handler_remove(_sensorPCntUnit);
    }
    if (_ledcAttached)
    {
        ledcDetachPin(_pwmPin);
    }
}

void FanControl::Begin(pcnt_unit_t pcUnit, pcnt_channel_t pcChannel, int sensorPin, ledc_channel_t pwmChannel, int pwmPin, uint32_t pwmFreq, ledc_timer_bit_t pwmResBits)
{
    _sensorPCntUnit = pcUnit;
    _sensorPCntChannel = pcChannel;
    _sensorPin = sensorPin;
    _pwmChannel = pwmChannel;
    _pwmPin = pwmPin;
    Freq = pwmFreq;
    _dutyMax = pow(2, (int)pwmResBits) - 1;

    if ((_sensorPCntUnit >= PCNT_UNIT_0 && _sensorPCntUnit < PCNT_UNIT_MAX) &&
        (_sensorPCntChannel >= PCNT_CHANNEL_0 || _sensorPCntChannel < PCNT_CHANNEL_MAX) && _sensorPin > 0)
    {
        pcnt_config_t config = {
            .pulse_gpio_num = _sensorPin,
            .ctrl_gpio_num = -1,
            .lctrl_mode = PCNT_MODE_KEEP,	// when control signal is low, keep the primary counter mode
            .hctrl_mode = PCNT_MODE_KEEP,   // when control signal is high, keep the primary counter mode
            .pos_mode = PCNT_COUNT_DIS,     // do nothing on positive edge
            .neg_mode = PCNT_COUNT_INC,     // increase on falling edge
            .counter_h_lim = PCNT_COUNTER_HLIM,
            .counter_l_lim = PCNT_COUNTER_LLIM,
            .unit = _sensorPCntUnit,
            .channel = _sensorPCntChannel
        };
        esp_err_t ret = pcnt_unit_config(&config);
        log_i("pcnt_unit_config unit %d channel %d on PIN %d return 0x%x: %s", _sensorPCntUnit, _sensorPCntChannel, _sensorPin, ret, esp_err_to_name(ret));
        if (ret == ESP_OK)
        {
            pcnt_counter_pause(_sensorPCntUnit);
            pcnt_counter_clear(_sensorPCntUnit);
            pcnt_set_filter_value(_sensorPCntUnit, 100); // 0~1023 filter pulse lasting < 1.25 ms (100 * 12.5us)
            pcnt_filter_enable(_sensorPCntUnit);
            if (!ISR_INSTALLED)
            {
                ret = pcnt_isr_service_install(ESP_INTR_FLAG_EDGE | ESP_INTR_FLAG_LOWMED);
                ISR_INSTALLED = ret == ESP_OK;
                log_i("pcnt_isr_service_install return 0x%x: %s", ret, esp_err_to_name(ret));
            }
            ret = pcnt_isr_handler_add(_sensorPCntUnit, pcnt_event_handler, (void*)this);
            log_i("pcnt_isr_handler_add return 0x%x: %s", ret, esp_err_to_name(ret));
            if (ret == ESP_OK)
            {
                pcnt_intr_enable(_sensorPCntUnit);
                pcnt_event_enable(_sensorPCntUnit, PCNT_EVT_H_LIM);
                pcnt_counter_resume(_sensorPCntUnit);
                _pcnt_start = millis();
            }
        }
    }

    if (_pwmPin > 0 && _pwmChannel < LEDC_CHANNEL_MAX)
    {
        esp_err_t ret = ledcSetup(_pwmChannel, Freq, pwmResBits);
        log_i("ledcSetup PIN %d on Channel %d at %d Hz %d bits return 0x%x: %s", _pwmPin, _pwmChannel, Freq, pwmResBits, ret, esp_err_to_name(ret));
        //if (ret == ESP_OK)
        {
            ledcAttachPin(_pwmPin, _pwmChannel);
            _ledcAttached = true;
            SetPowerOnOff(true);
        }
    }
}

void FanControl::SetPowerPercent(float precent)
{
    if (_ledcAttached)
    {
        _power_precent = precent;
        if (precent < 0.05f)
            _power_precent = 0;
        if (precent > 0.95f)
            _power_precent = 1;
        int duty = _power_precent * _dutyMax;
        ledcWrite(_pwmChannel, duty);
        log_d("SET %.2f ledcWrite channel %d duty = %d", _power_precent, _pwmChannel, duty);
    }
    else
        log_e("ERROR: channel %d not attached pin %d", _pwmChannel, _pwmPin);
}

void FanControl::SetPowerOnOff(bool on)
{
    if (_pwmPin > 0)
    {
        _power_precent = on ? 1 : 0;
        ledcWrite(_pwmChannel, on ? _dutyMax : 0);
        log_d("SET OnOff = %d", on);
    }
}

void FanControl::CalculateRpm()
{
    pcnt_evt_t evt;

    int ret = xQueueReceive(pcnt_evt_queue, &evt, 1000 / portTICK_PERIOD_MS);
    // timeout, queue empty for one sec, or lower than 600 rpm
    if (ret != pdTRUE)
    {
        _sensor_rpm = 0;
        return;
    }
    // consume all data but wait less time
    while (ret == pdTRUE)
    {
        if (evt.unit == _sensorPCntUnit && (evt.status & PCNT_EVT_H_LIM))
        {
            _sensor_rpm = 30000UL * (uint32_t)PCNT_COUNTER_HLIM / evt.ms;
        }
        ret = xQueueReceive(pcnt_evt_queue, &evt, 100 / portTICK_PERIOD_MS);
    }
    return;
}

void IRAM_ATTR FanControl::pcnt_event_handler(void* arg)
{
    unsigned long now = millis();
    FanControl* o = static_cast<FanControl*>(arg);
    if (o && now > o->_pcnt_start)
    {
        uint32_t status = 0;
        if (pcnt_get_event_status(o->_sensorPCntUnit, &status) == ESP_OK)
        {
            pcnt_evt_t evt = { o->_sensorPCntUnit, status, now - o->_pcnt_start };
            o->_pcnt_start = now;
            int ret = xQueueSendFromISR(o->pcnt_evt_queue, &evt, NULL);
            if (ret == errQUEUE_FULL)
            {
                // remove 2 then resend
                isr_log_e("pcnt_evt_queue full");
                pcnt_evt_t dump;
                xQueueReceiveFromISR(o->pcnt_evt_queue, &dump, NULL);
                xQueueReceiveFromISR(o->pcnt_evt_queue, &dump, NULL);
                xQueueSendFromISR(o->pcnt_evt_queue, &evt, NULL);
            }
            isr_log_i("pcnt_evt_queue send");
        }
        else
            isr_log_e("get_event_status = %d", status);
    }
}
