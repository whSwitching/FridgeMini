// 
// 
// 

#include "FridgeControl.h"

FridgeControl::~FridgeControl()
{
    if (_ledcAttached)
    {
        ledcDetachPin(_pin);
        log_i("ledcDetachPin %d", _pin);
    }
}

void FridgeControl::Begin(bool enablePID, int pin, int channel, uint32_t freq, uint8_t resBits)
{
    pid_enabled = enablePID;
    _pin = pin;
    _channel = channel;
    _freq = freq;
    _dutyMax = pow(2, resBits) - 1;
    if (_channel >= 0 && _pin > 0)
    {
        esp_err_t ret = ledcSetup(_channel, _freq, resBits);
        log_i("ledcSetup PIN %d on Channel %d at %d Hz %d bits return 0x%x: %s", pin, channel, freq, resBits, ret, esp_err_to_name(ret));
        //if (ret == ESP_OK)
        {
            ledcAttachPin(_pin, _channel);
            _ledcAttached = true;
            SetPowerOnOff(false);
        }
    }
    
    pid.begin(&pid_input, &pid_output, &pid_setpoint, pid_p, pid_i, pid_d);
    pid.reverse();
    pid.setOutputLimits(0, pin_output_limitH);
    pid.setWindUpLimits(-1, 1);
    pid.stop();
}

bool FridgeControl::RequireAutoControl()
{
    int64_t now = esp_timer_get_time();
    return now - control_last_intervene >= CONTROL_INTERVENTION_INTERVAL_NS;
}

void FridgeControl::AutoControl(display_param_t* v, TFTDisplay* tft)
{
    control_last_intervene = esp_timer_get_time();
    // if temp sensors invalid, turn it off
    if (!temp_value_valid(v->tempCool))
    {
        v->pwTEC = 0;
        v->pwBox = 0;
        log_e("TEC1 cool side DS a reading invalid");
        tft->ShowError("DS a Error");
        pid.stop();
        return;
    }
    if (!temp_value_valid(v->tempHeat))
    {
        v->pwTEC = 0;
        v->pwBox = 0;
        pid.stop();
        log_e("TEC1 heat side DS b reading invalid");
        tft->ShowError("DS b Error");
        return;
    }
    if (!temp_value_valid(v->tempBox))
    {
        v->pwTEC = 0;
        v->pwBox = 0;
        pid.stop();
        log_e("Box DS c reading invalid");
        tft->ShowError("DS c Error");
        return;
    }
    // clear error
    tft->ShowError(emptyString);
    // check TEC1 heat/cool sides temp diff
    float tec1TempDiff = v->tempHeat - v->tempCool;
    // check box air circulation
    float boxTempDiff = v->tempBox - v->tempCool;
    // check setting temp diff
    float settingTempDiff = v->tempCool - v->tempSetting;
    // temp setting is idle
    if (v->tempSetting == TEMP_SETTING_IDLE)
    {
        v->pwTEC = 0;
        v->pwSink = boxTempDiff / TEC1_NORMAL_TEMP * 0.7f; // 0~0.7 (power) map to 0~TEC1_NORMAL_TEMP (tec1TempDiff)
        v->pwBox = boxTempDiff / 10 * 0.7f;                // 0~0.7 (power) map to 0~10 (boxTempDiff)
        log_i("temp setting idel, turn everyting off");
        pid.stop();
        return;
    }
    // if pump/heatsink fan invalid, show warning
    if (v->rpmPump == 0 && v->pwPump > 0.1f)
    {
        log_e("Pump fan rpm reading 0 but power is %.0f", v->pwPump);
        tft->ShowWarning("pump invalid");
    }
    if (v->rpmSink == 0 && v->pwSink > 0.1f)
    {
        log_e("Heatsink fan rpm reading 0 but power is %.0f", v->pwSink);
        tft->ShowWarning("heatsink invalid");
    }
    if (tec1TempDiff >= TEC1_OVERHEAT_TEMP) // overheated, turn it off
    {
        v->pwTEC = 0;
        v->pwSink = 1;
        v->pwBox = 1;
        tft->ShowWarning("overheated");
        pid.stop();
        return;
    }
    else if (tec1TempDiff >= TEC1_OVERHEATING_TEMP) // go overheating, turn it down
    {
        v->pwTEC = 0.05; // lower output, MOS pwm not work linearly
        v->pwSink = 1;
        v->pwBox = 1;
        tft->ShowWarning("overheating");
        pid.stop();
        return;
    }
    // clear warning
    tft->ShowWarning(emptyString);
    if (pid_enabled)
    {
        // pid control
        pid_setpoint = v->tempSetting;
        pid_input = v->tempCool;
        pid.start();
        pid.compute();
        v->pwTEC = (float)(pid_output / pin_output_limitH);
    }
    else
    {
        // full power when distance to setting > 5
        if (settingTempDiff > 5)
        {
            v->pwTEC = 1;
        }
        else
        {
            // map 0.05~1 power to 0~5
            v->pwTEC = 0.05f + settingTempDiff / 5 * 0.95f;
        }
    }
    // safe
    if (tec1TempDiff >= TEC1_NORMAL_TEMP)
    {
        v->pwSink = 0.7;
    }
    else // lower than normal, even safer, trun heatsink fan down linearly, 0.3~0.7 map to 0~TEC1_NORMAL_TEMP
    {
        v->pwSink = 0.3f + tec1TempDiff / TEC1_NORMAL_TEMP * 0.4f;
    }
    // inbox air circulation not good, box fan full power
    if (boxTempDiff >= 10)
    {
        v->pwBox = 1;
    }
    else // lower than 10, trun box fan down linearly, 0~0.7 map to 0~10
    {
        v->pwBox = boxTempDiff / 10 * 0.7f;;
    }
    if (pid_enabled)
    {
        // for serial chart
        Serial.println("Temp_Box Temp_Cool Temp_Heat Pow_Box Pow_Tec Pow_Sink");
        Serial.printf("%.1f %.1f %.1f %.1f %.1f %.1f\n", v->tempBox, v->tempCool, v->tempHeat, v->pwBox, v->pwTEC, v->pwSink);
    }
    else
    {
        log_i("Temp_Box=%.1f, Temp_Cool=%.1f, Temp_Heat=%.1f, Pow_Box=%.1f, Pow_Tec=%.1f, Pow_Sink=%.1f\nTEC1 Diff=%.1f, Box Diff=%.1f, Setting Diff=%.1f", v->tempBox, v->tempCool, v->tempHeat, v->pwBox, v->pwTEC, v->pwSink, tec1TempDiff, boxTempDiff, settingTempDiff);
    }
    return;
}

float FridgeControl::GetPowerPercent()
{
    return _power_precent;
}

void FridgeControl::SetPowerPercent(float precent)
{
    if (_ledcAttached)
    {
        _power_precent = precent;
        if (precent < 0.01f)
            _power_precent = 0;
        if (precent > 0.99f)
            _power_precent = 1;
        int duty = _power_precent * _dutyMax;
        ledcWrite(_channel, duty);
        log_d("SET %.2f ledcWrite channel %d duty = %d", _power_precent, _channel, duty);
    }
    else
        log_e("ERROR: channel %d not attached pin %d", _channel, _pin);
}

void FridgeControl::SetPowerOnOff(bool on)
{
    if (_ledcAttached)
    {
        _power_precent = on ? 1 : 0;
        ledcWrite(_channel, on ? _dutyMax : 0);
        log_d("SET OnOff = %d", on);
    }
}
