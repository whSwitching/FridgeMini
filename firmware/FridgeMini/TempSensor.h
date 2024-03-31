#pragma once
// TempSensor.h

#ifndef _TEMPSENSOR_h
#define _TEMPSENSOR_h

#include <OneWireESP32.h>
#include "WireConfig.h"

class TempSensor
{
	const uint READ_INTERVAL = 3000; // ms
	uint32_t _lastRead = 0;
    uint64_t _box = DS_c_ADDR;
	uint64_t _cool = DS_a_ADDR;
	uint64_t _heat = DS_b_ADDR;
	float _tempBox = TEMPSENSOR_DISCONNECT;
	float _tempCool = TEMPSENSOR_DISCONNECT;
	float _tempHeat = TEMPSENSOR_DISCONNECT;
	int _pin = -1;
	bool _parasite = false;
	OneWire32* _ds = NULL;
public:
	void Begin(uint8_t pin = DS_DATA_PIN, uint8_t tx = 0u, uint8_t rx = 1u, bool parasitePower = false);
	// request for all (write 0xCC 0x44)
	void RequestTemp();
	void PrintAddresses();
	float GetTempBox();
	float GetTempCool();
	float GetTempHeat();
};

#endif
