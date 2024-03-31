
#include "Network.h"


void Network::wifi_BeginSTA(const char* remoteAp_ssid, const char* remoteAp_password, IPAddress my_ip, IPAddress remoteAp_gateway, IPAddress remoteAp_subnet, IPAddress dns1, IPAddress dns2)
{
	log_i("WIFI Station Init...");
	WiFi.config(my_ip, remoteAp_gateway, remoteAp_subnet, dns1, dns2);
	WiFi.mode(WIFI_STA);
	WiFi.setHostname(LOCAL_HOSTNAME);
	WiFi.begin(remoteAp_ssid, remoteAp_password);
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
	while (WiFi.status() != WL_CONNECTED)
	{
		log_i(".");
		digitalWrite(LED_BUILTIN, ON);
		delay(200);
		digitalWrite(LED_BUILTIN, OFF);
		delay(500);
	}
	digitalWrite(LED_BUILTIN, OFF);
	log_i("WIFI Station Setup, STA: %s, IP: %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

	ntpClient.begin();
}

void Network::wifi_BeginAP(const char* localAp_ssid, const char* localAp_password, IPAddress localAp_ip, IPAddress localAp_gateway, IPAddress localAp_subnet)
{
	log_i("WIFI AP Init...");
	WiFi.softAPConfig(localAp_ip, localAp_gateway, localAp_subnet);
	WiFi.mode(WIFI_AP);
	WiFi.softAPsetHostname(LOCAL_HOSTNAME);
	WiFi.softAP(localAp_ssid, localAp_password, 11);
	log_i("WIFI AP Setup, IP: %s, SubnetMask: %s", WiFi.softAPIP().toString().c_str(), WiFi.softAPSubnetMask().toString().c_str());
}

void Network::wifi_BeginAPSTA(const char* remoteAp_ssid, const char* remoteAp_password, IPAddress my_ip, IPAddress remoteAp_gateway, IPAddress remoteAp_subnet, IPAddress dns1, IPAddress dns2, const char* localAp_ssid, const char* localAp_password, IPAddress localAp_ip, IPAddress localAp_gateway, IPAddress localAp_subnet)
{
	log_i("WIFI AP Init...");
	WiFi.softAPConfig(localAp_ip, localAp_gateway, localAp_subnet);
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAPsetHostname(LOCAL_HOSTNAME);
	WiFi.softAP(localAp_ssid, localAp_password, 11);
	log_i("WIFI AP Setup, SSID: %s, IP: %s, SubnetMask: %s", WiFi.softAPSSID().c_str(), WiFi.softAPIP().toString().c_str(), WiFi.softAPSubnetMask().toString().c_str());

	log_i("WIFI Station Init...");
	WiFi.config(my_ip, remoteAp_gateway, remoteAp_subnet, dns1, dns2);
	WiFi.setHostname(LOCAL_HOSTNAME);
	WiFi.begin(remoteAp_ssid, remoteAp_password);
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
	int timeout = 5;
	while (WiFi.status() != WL_CONNECTED && timeout > 0)
	{
		log_i(".");
		digitalWrite(LED_BUILTIN, ON);
		delay(500);
		digitalWrite(LED_BUILTIN, OFF);
		delay(500);
		timeout -= 1;
	}
	digitalWrite(LED_BUILTIN, OFF);

	if (timeout > 0)
	{
		log_i("WIFI Station Setup, STA: %s, IP: %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
		ntpClient.begin();
	}
	else
	{
		log_i("WIFI Station Setup, connect to %s timeout, use local AP IP: %s", remoteAp_ssid, WiFi.softAPIP().toString().c_str());
	}
}

void Network::Wifi_Begin(const char* remoteAp_ssid, const char* remoteAp_password)
{
	_wifiModeSetting = (wifi_mode_t)fs_read_firstline(FILE_WIFIMODE);
	_fridgeTempSetting = fs_read_firstline(FILE_FRIDGETEMP);

	if (_wifiModeSetting == WIFI_STA)
		wifi_BeginSTA(remoteAp_ssid, remoteAp_password);
	else 	if (_wifiModeSetting == WIFI_AP)
		wifi_BeginAP();
	else
		wifi_BeginAPSTA(remoteAp_ssid, remoteAp_password);
}

bool Network::Wifi_Connected()
{
	if ((WiFi.getMode() & WIFI_MODE_STA) > 0)
		return WiFi.status() == WL_CONNECTED;
	return false;
}

int8_t Network::Wifi_GetRSSI()
{
	if (Wifi_Connected())
		return WiFi.RSSI();
	return TEMPSENSOR_DISCONNECT;
}

String Network::Wifi_GetLocalIP()
{
	WiFiMode_t mode = WiFi.getMode();
	if ((mode & WIFI_MODE_STA) > 0)
	{
		if (WiFi.status() == WL_CONNECTED)
			return WiFi.localIP().toString();
	}
	if ((mode & WIFI_MODE_AP) > 0)
	{
		return WiFi.softAPIP().toString();
	}
	return LOCAL_HOSTNAME;
}

bool Network::Ntp_Update()
{
	if (Wifi_Connected())
		return ntpClient.update();
	return false;
}

bool Network::Ntp_TimeSet()
{
	return ntpClient.isTimeSet();
}

tm* Network::Ntp_GetTime()
{
	if (Wifi_Connected())
	{
		unsigned long unixEpoch = ntpClient.getEpochTime(); // seconds since 1970-1-1 00:00 + TimeZoneOffset
		time_t rawtime = unixEpoch;
		tm* gmt = gmtime(&rawtime);
		if (gmt->tm_year + 1900 >= 2024)
			return gmt;
	}
	return nullptr;
}

String Network::Ntp_GetTimeString(bool date, bool time, bool weekday, bool weekdayCN)
{
	tm* ptm = Ntp_GetTime();
	if (ptm)
	{
		// yyyy-MM-dd HH:mm
		char ret[20];
		int idx = 0;
		if (date)
		{
			sprintf(ret, "%4d-%02d-%02d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
			idx = 10;
		}
		if (time)
		{
			if (idx > 0)
			{
				ret[idx] = ' ';
				idx++;
			}
			sprintf(&ret[idx], "%02d:%02d", ptm->tm_hour, ptm->tm_min);
			idx += 5;
		}
		if (weekday)
		{
			if (idx > 0)
			{
				ret[idx] = ' ';
				idx++;
				ret[idx] = '\0';
			}
			if (weekdayCN)
			{
				String weekDays[] = { "\u5468\u65e5", "\u5468\u4e00", "\u5468\u4e8c", "\u5468\u4e09", "\u5468\u56db", "\u5468\u4e94", "\u5468\u516d" };
				return String(ret) + weekDays[ptm->tm_wday];
			}
			else
			{
				String weekDays[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
				return String(ret) + weekDays[ptm->tm_wday];
			}
		}
		else
			return String(ret);
	}
	else
	{
		String ret = "0000-00-00 00:00";
		unsigned int s = 0;
		unsigned int len = 0;
		if (date)
		{
			s = 0;
			len += 10;
		}
		if (time)
		{
			s = date ? 0 : 12;
			len += (date ? 6 : 5);
		}
		if (weekday)
			return len > 0 ? (ret.substring(s, s + len) + " ddd") : "ddd";
		else
			return ret.substring(s, s + len);
	}
}

WiFiMode_t Network::Get_WifiModeSetting()
{
	return _wifiModeSetting;
}

int Network::Set_WifiModeSetting(WiFiMode_t mode)
{
	int writeSize = fs_write_firstline(FILE_WIFIMODE, (int)mode);
	if (writeSize > 0)
		_wifiModeSetting = mode;
	return writeSize;
}

int Network::Get_FridgeTempSetting()
{
	return _fridgeTempSetting;
}

int Network::Set_FridgeTempSetting(int temp)
{
	int writeSize = fs_write_firstline(FILE_FRIDGETEMP, temp);
	if (writeSize > 0)
		_fridgeTempSetting = temp;
	return writeSize;
}

int Network::Weather_GetNow(String& weather, float& temp)
{
	if (Wifi_Connected())
	{
		return qweather.GetNowWeather(weather, temp);
	}
	else
	{
		weather = "";
		temp = TEMPSENSOR_DISCONNECT;
		return -11;
	}
}
