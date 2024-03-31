
#include "TempSensor.h"

void TempSensor::Begin(uint8_t pin, uint8_t tx, uint8_t rx, bool parasitePower)
{
    _pin = pin;
    _parasite = parasitePower;
    _ds = new OneWire32(pin, tx, rx, parasitePower ? 1u : 0u);
    _ds->reset();
}

void TempSensor::RequestTemp()
{
    uint32_t now = millis();
    if (now - _lastRead >= READ_INTERVAL)
    {
        _ds->request();
        vTaskDelay(750 / portTICK_PERIOD_MS);

        uint8_t errC = _ds->getTemp(_cool, _tempCool);
        if (errC)
            _tempCool = TEMPSENSOR_ERR - errC;

        uint8_t errH = _ds->getTemp(_heat, _tempHeat);
        if (errH)
            _tempHeat = TEMPSENSOR_ERR - errH;

        uint8_t errB = _ds->getTemp(_box, _tempBox);
        if (errB)
            _tempBox = TEMPSENSOR_ERR - errB;

        if (!errC && !errH && !errB)
            _lastRead = now;
    }
}

void TempSensor::PrintAddresses()
{
    uint64_t addr[3];
    uint8_t devices = _ds->search(addr, 3);
    for (uint8_t i = 0; i < devices; i += 1)
        Serial.printf("Temp Sensor %d Addr: 0x%llx,\n", i, addr[i]);
}

float TempSensor::GetTempBox()
{
    return _tempBox;
}

float TempSensor::GetTempCool()
{
    return _tempCool;
}

float TempSensor::GetTempHeat()
{
    return _tempHeat;
}
