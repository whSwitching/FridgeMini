#pragma once

#ifndef _UTILS_h
#define _UTILS_h

#include <esp_system.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <TFT_eSPI.h>
#include "WireConfig.h"

typedef struct display_param_t
{
	String weather;
	float tempAir;
	float tempCool;
	float tempHeat;
	float tempBox;
	int wifiModeSetting;
	int tempSetting;
	uint16_t rpmPump;
	float pwPump;
	uint16_t rpmSink;
	float pwSink;
	uint16_t rpmBox;
	float pwBox;
	float pwTEC;
	int8_t rssi;
	String localhost;
	tm* time;
	uint32_t upTimeMs;
	esp_reset_reason_t resetReason;
};

typedef struct sys_info_t
{
	uint32_t chipID;
	uint8_t chipCores;
	const char* chipModel;
	uint8_t chipRevision;
	const char* sdkVersion;
	uint32_t cpuFreqMHz;
	String flashChipMode;
	uint32_t flashChipFreqMHz;
	float flashChipSizeKB;
	float sketchSizeKB;
	float freeSketchSpaceKB;
	float heapSizeKB;
	float freeHeapSizeKB;
	uint32_t maxAllocHeapB;
	float psramSizeKB;
	float freePsramSizeKB;
	float maxAllocPsramKB;
	float fsSizeKB;
	float usedFSSizeKB;
	esp_reset_reason_t resetReason;
};

typedef void(*get_sensorvalues_delegate)(display_param_t* val);

typedef String(*get_now_string_callback)(void);

static String wifi_rssiIndicate(int8_t rssi)
{
    if (rssi > -55)
        return "■■■■■";
    else if (rssi > -66)
        return "■■■■□";
    else if (rssi > -77)
        return "■■■□□";
    else if (rssi > -88)
        return "■■□□□";
    else if (rssi > -100)
        return "■□□□□";
    else
        return "□□□□□";
}

static uint16_t wifi_rssiIndicateColor(int8_t rssi)
{
	if (rssi > -55)
		return TFT_GREEN;
	else if (rssi > -66)
		return TFT_OLIVE;
	else if (rssi > -77)
		return TFT_CYAN;
	else if (rssi > -88)
		return TFT_GOLD;
	else if (rssi > -100)
		return TFT_YELLOW;
	else
		return TFT_RED;
}

static bool temp_value_valid(float temp)
{
	return temp != TEMPSENSOR_ERR && 
		temp != TEMPSENSOR_CRC && 
		temp != TEMPSENSOR_BADDATA &&
		temp != TEMPSENSOR_DISCONNECT && 
		temp != TEMPSENSOR_DRIVER;
}

static uint16_t temp_color(float temp, float thresholdL = 10, float thresholdH = 38)
{
	if (temp_value_valid(temp))
	{
		float step = (thresholdH - thresholdL) / 5;
		if (temp >= thresholdH)
			return TFT_PURPLE;
		else if (temp >= thresholdH - step)
			return TFT_RED;
		else if (temp >= thresholdH - step * 2)
			return TFT_YELLOW;
		else if (temp >= thresholdH - step * 4)
			return TFT_GREENYELLOW;
		else if (temp >= thresholdL)
			return TFT_WHITE;
		else
			return TFT_CYAN;
	}
	else
	{
		if (temp == TEMPSENSOR_DISCONNECT)
			return TFT_YELLOW;
		else if (temp == TEMPSENSOR_BADDATA)
			return TFT_ORANGE;
		else
			return TFT_RED;
	}
}

static uint16_t rpm_color(uint16_t rpm)
{
	if (rpm >= 2000)
		return TFT_YELLOW;
	else if (rpm >= 1000)
		return TFT_GREENYELLOW;
	else
		return TFT_WHITE;
}

static uint16_t pow_color(float pow)
{
	if (pow >= 0.8f)
		return TFT_YELLOW;
	else if (pow >= 0.2f)
		return TFT_GREENYELLOW;
	else
		return TFT_WHITE;
}

static String wifi_authModeString(uint8_t networkItem)
{
	uint8_t authMode = uint8_t(WiFi.encryptionType(networkItem));
	if (authMode == 0)
		return "OPEN";
	else if (authMode == 1)
		return "WEP";
	else if (authMode == 2)
		return "WPA";
	else if (authMode == 3)
		return "WPA2";
	else if (authMode == 4)
		return "WPA_WPA2";
	else if (authMode == 5)
		return "WPA2_ENT";
	else if (authMode == 6)
		return "WPA3";
	else if (authMode == 7)
		return "WPA2_WPA3";
	else if (authMode == 8)
		return  "WAPI";
	else
		return "Unknown";
}

static String list_file_node(const char* name, uint8_t depth)
{
	String html = "";
	for (uint8_t i = 0; i < depth; i++)
	{
		html += "    ";
	}
	html += "+-- " + String(name) + "\n";
	return html;
}

static String list_fs_dir(const char* dirPath, uint8_t depth)
{
	String html = "";
	fs::File dir = SPIFFS.open(dirPath);
	if (dir)
	{
		if (dir.isDirectory())
		{
			if (strlen(dir.name()) > 0)
				html += list_file_node(dir.name(), depth);
			else
				html += list_file_node(dirPath, depth);
			fs::File file = dir.openNextFile();
			while (file)
			{
				if (file.isDirectory())
					html += list_fs_dir(file.path(), depth + 1);
				else
					html += list_file_node(file.name(), depth + 1);
				file.close();
				file = dir.openNextFile();
			}
		}
		dir.close();
	}
	return html;
}

static String reset_reason(esp_reset_reason_t reason)
{
	switch (reason)
	{
	case ESP_RST_POWERON: return "PwOn";
	case ESP_RST_EXT: return "ExPn";
	case ESP_RST_SW: return "Soft";
	case ESP_RST_PANIC: return "Panic";
	case ESP_RST_INT_WDT: return "IDog";
	case ESP_RST_TASK_WDT: return "TDog";
	case ESP_RST_WDT: return "Dog";
	case ESP_RST_DEEPSLEEP: return "DSlep";
	case ESP_RST_BROWNOUT: return "Vdd";
	case ESP_RST_SDIO: return "SDIO";
	default: return "Unkwn";
	}
}

static String reset_reason_verbose(esp_reset_reason_t reason)
{
	switch (reason)
	{
	case ESP_RST_POWERON: return "due to power-on event";
	case ESP_RST_EXT: return "by external pin";
	case ESP_RST_SW: return "software via esp_restart";
	case ESP_RST_PANIC: return "software due to exception/panic";
	case ESP_RST_INT_WDT: return "due to interrupt watchdog";
	case ESP_RST_TASK_WDT: return "due to task watchdog";
	case ESP_RST_WDT: return "due to other watchdogs";
	case ESP_RST_DEEPSLEEP: return "after exiting deep sleep mode";
	case ESP_RST_BROWNOUT: return "Vdd Brownout";
	case ESP_RST_SDIO: return "over SDIO";
	default: return "reason can not be determined";
	}
}


#endif
