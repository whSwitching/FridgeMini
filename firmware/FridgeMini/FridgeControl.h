#pragma once

#ifndef _FRIDGECONTROL_h
#define _FRIDGECONTROL_h

#include <arduino.h>
#include <esp32-hal-ledc.h>
#include <esp32-hal-log.h>
#include <esp_err.h>
#include <esp_timer.h>
#include <math.h>
#include <ArduPID.h>
#include "TFTDisplay.h"
#include "WireConfig.h"
#include "Utils.h"

#define TEC1_OVERHEAT_TEMP 50	// maximum 67 for TEC1 12708
#define TEC1_OVERHEATING_TEMP 40
#define TEC1_NORMAL_TEMP 30

/// <summary>
/// I am control of the system
/// </summary>
class FridgeControl
{
	const int32_t CONTROL_INTERVENTION_INTERVAL_NS = 2000000; // microseconds
	int64_t control_last_intervene = 0;

	const int pin_output_limitH = 1000;
	bool pid_enabled = false;
	ArduPID pid;	
	double pid_input = 0;
	double pid_output = 0;
	double pid_setpoint = 0;
	double pid_p = 10;
	double pid_i = 1;
	double pid_d = 0.5;

	uint32_t _freq = 0;
	uint32_t _dutyMax = 0;
	int _channel = -1;
	int _pin = -1;
	bool _ledcAttached = false;
	float _power_precent = 0;
public:
	~FridgeControl();
	// default 10k Hz, 10bit (0~1023)
	void Begin(bool enablePID = false, int pin = TEC1_PWM_PIN, int channel = TEC1_PWM_CHANNEL, uint32_t freq = 1000, uint8_t resBits = 10);
	bool RequireAutoControl();
	void AutoControl(display_param_t* v, TFTDisplay* tft);
	float GetPowerPercent();
	// precent = [0, 1]
	void SetPowerPercent(float precent);	
	void SetPowerOnOff(bool on);
};

#endif
