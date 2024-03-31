#pragma once

#ifndef _FANCONTROL_h
#define _FANCONTROL_h

#include <arduino.h>
#include "driver/pcnt.h"
#include "driver/ledc.h"
#include "esp32-hal-log.h"
#include "esp_err.h"
#include "WireConfig.h"

#define PCNT_COUNTER_LLIM 0
#define PCNT_COUNTER_HLIM 100	// interrupt at 10 r, rpm = 10r * 60000 / duration_ms  (minimal 20 ct/s, 10 r/s, 600 r/m)

typedef struct pcnt_evt_t
{
	pcnt_unit_t unit;
	uint32_t status;
	unsigned long ms;
};

class FanControl
{
	static void IRAM_ATTR pcnt_event_handler(void* arg);

	const unsigned int pcnt_evt_queue_SIZE = 5;
	xQueueHandle pcnt_evt_queue;
	uint32_t Freq = 25000;
	uint32_t _dutyMax = 0;

	int _sensorPin = -1;
	pcnt_unit_t _sensorPCntUnit = PCNT_UNIT_MAX;
	pcnt_channel_t _sensorPCntChannel = PCNT_CHANNEL_MAX;
	
	int _pwmPin = -1;
	ledc_channel_t _pwmChannel = LEDC_CHANNEL_MAX;
	bool _ledcAttached = false;
	float _power_precent = 1;

	volatile uint32_t _sensor_rpm = 0;
	volatile unsigned long _pcnt_start;
public:
	FanControl();
	~FanControl();
	void Begin(pcnt_unit_t pcUnit, pcnt_channel_t pcChannel, int sensorPin, ledc_channel_t pwmChannel = LEDC_CHANNEL_MAX, int pwmPin = -1, uint32_t pwmFreq = 25000, ledc_timer_bit_t pwmResBits = LEDC_TIMER_10_BIT);	// 25k Hz, 10bit (0~1023));
	void CalculateRpm();
	// precent = [0, 1]
	void SetPowerPercent(float precent);
	void SetPowerOnOff(bool on);
	uint16_t GetSpeedRpm()
	{
		return _sensor_rpm;
	}
	float GetPowerPercent()
	{
		return _power_precent;
	}
private:
	static bool ISR_INSTALLED;
};

#endif
