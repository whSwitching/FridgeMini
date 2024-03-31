#pragma once

#ifndef _NETWORK_H
#define _NETWORK_H

#include <esp32-hal-log.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "WireConfig.h"
#include "WeatherAPI.h"

class Network
{
	const int TimeZoneOffset = 8;
	WiFiUDP ntpUDP = WiFiUDP();
	NTPClient ntpClient = NTPClient(ntpUDP, "ntp.ntsc.ac.cn", TimeZoneOffset * 3600, 600000); // timezone= +8, ntp sync interval = 10 min
	WeatherAPI qweather = WeatherAPI();
	WiFiMode_t _wifiModeSetting;
	int _fridgeTempSetting;
private:
	void wifi_BeginSTA(const char* remoteAp_ssid, const char* remoteAp_password, IPAddress my_ip = IPAddress(192, 168, 2, 31), IPAddress remoteAp_gateway = IPAddress(192, 168, 2, 1), IPAddress remoteAp_subnet = IPAddress(255, 255, 255, 0), IPAddress dns1 = IPAddress(180, 76, 76, 76), IPAddress dns2 = IPAddress(8, 8, 8, 8));
	void wifi_BeginAP(const char* localAp_ssid = LOCAL_AP, const char* localAp_password = LOCAL_APPWD, IPAddress localAp_ip = IPAddress(192, 168, 0, 31), IPAddress localAp_gateway = IPAddress(192, 168, 0, 31), IPAddress localAp_subnet = IPAddress(255, 255, 255, 0));
	void wifi_BeginAPSTA(const char* remoteAp_ssid, const char* remoteAp_password, IPAddress my_ip = IPAddress(192, 168, 2, 31), IPAddress remoteAp_gateway = IPAddress(192, 168, 2, 1), IPAddress remoteAp_subnet = IPAddress(255, 255, 255, 0), IPAddress dns1 = IPAddress(180, 76, 76, 76), IPAddress dns2 = IPAddress(8, 8, 8, 8),
		                 const char* localAp_ssid = LOCAL_AP, const char* localAp_password = LOCAL_APPWD, IPAddress localAp_ip = IPAddress(192, 168, 0, 31), IPAddress localAp_gateway = IPAddress(192, 168, 0, 31), IPAddress localAp_subnet = IPAddress(255, 255, 255, 0));
	int fs_read_firstline(const char* name)
	{
		int t = 0;
		File f = SPIFFS.open(name, "r");
		if (f)
		{
			t = f.parseInt();
			f.close();
			return t;
		}
		if (name == FILE_WIFIMODE)
			return WIFI_MODE_APSTA;
		else if (name == FILE_FRIDGETEMP)
			return TEMP_SETTING_IDLE;
		else
			return 0;
	}
	int fs_write_firstline(const char* name, int value)
	{
		File f = SPIFFS.open(name, "w");
		if (f)
		{
			size_t wt = f.printf("%d\r\n", value);
			f.close();
			return wt;
		}
		return -1;
	}
public:
	void Wifi_Begin(const char* remoteAp_ssid, const char* remoteAp_password);
	bool Wifi_Connected();
	int8_t Wifi_GetRSSI();
	String Wifi_GetLocalIP();
	bool Ntp_Update();
	bool Ntp_TimeSet();
	struct tm* Ntp_GetTime();
	String Ntp_GetTimeString(bool date = true, bool time = true, bool weekday = true, bool weekdayCN = true);
	WiFiMode_t Get_WifiModeSetting();
	int Set_WifiModeSetting(WiFiMode_t mode);
	int Get_FridgeTempSetting();
	int Set_FridgeTempSetting(int temp);
	/// <returns>
	/// 0: updated and got updated data, &gt;0: not update, got cached data, &lt;0: updated but failed, got cached data
	/// </returns>
	int Weather_GetNow(String& weather, float& temp);
};


#endif
