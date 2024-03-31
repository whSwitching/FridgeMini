#pragma once

#ifndef _WEATHERAPI_h
#define _WEATHERAPI_h

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ArduinoUZlib.h>
#include <math.h>
#include <esp_timer.h>

#include "WireConfig.h"

typedef struct {
	const char* text;
	const char* temp;
	short icon;
	const char* windDir;
	const char* windScale;
	const char* humidity;
	const char* precip;
	short airLevel;
	const char* airCategory;
	const char* airPm2p5;
	const char* airPm10;
} WeatherNow;

typedef void (*Weather_Update_Callback_t)(StaticJsonDocument<2048>& data);

class WeatherAPI
{
private:
	const char* ca = \
		"-----BEGIN CERTIFICATE-----\n" \
		"MIIGEzCCA/ugAwIBAgIQfVtRJrR2uhHbdBYLvFMNpzANBgkqhkiG9w0BAQwFADCB\n" \
		"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n" \
		"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n" \
		"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTgx\n" \
		"MTAyMDAwMDAwWhcNMzAxMjMxMjM1OTU5WjCBjzELMAkGA1UEBhMCR0IxGzAZBgNV\n" \
		"BAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEYMBYGA1UE\n" \
		"ChMPU2VjdGlnbyBMaW1pdGVkMTcwNQYDVQQDEy5TZWN0aWdvIFJTQSBEb21haW4g\n" \
		"VmFsaWRhdGlvbiBTZWN1cmUgU2VydmVyIENBMIIBIjANBgkqhkiG9w0BAQEFAAOC\n" \
		"AQ8AMIIBCgKCAQEA1nMz1tc8INAA0hdFuNY+B6I/x0HuMjDJsGz99J/LEpgPLT+N\n" \
		"TQEMgg8Xf2Iu6bhIefsWg06t1zIlk7cHv7lQP6lMw0Aq6Tn/2YHKHxYyQdqAJrkj\n" \
		"eocgHuP/IJo8lURvh3UGkEC0MpMWCRAIIz7S3YcPb11RFGoKacVPAXJpz9OTTG0E\n" \
		"oKMbgn6xmrntxZ7FN3ifmgg0+1YuWMQJDgZkW7w33PGfKGioVrCSo1yfu4iYCBsk\n" \
		"Haswha6vsC6eep3BwEIc4gLw6uBK0u+QDrTBQBbwb4VCSmT3pDCg/r8uoydajotY\n" \
		"uK3DGReEY+1vVv2Dy2A0xHS+5p3b4eTlygxfFQIDAQABo4IBbjCCAWowHwYDVR0j\n" \
		"BBgwFoAUU3m/WqorSs9UgOHYm8Cd8rIDZsswHQYDVR0OBBYEFI2MXsRUrYrhd+mb\n" \
		"+ZsF4bgBjWHhMA4GA1UdDwEB/wQEAwIBhjASBgNVHRMBAf8ECDAGAQH/AgEAMB0G\n" \
		"A1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjAbBgNVHSAEFDASMAYGBFUdIAAw\n" \
		"CAYGZ4EMAQIBMFAGA1UdHwRJMEcwRaBDoEGGP2h0dHA6Ly9jcmwudXNlcnRydXN0\n" \
		"LmNvbS9VU0VSVHJ1c3RSU0FDZXJ0aWZpY2F0aW9uQXV0aG9yaXR5LmNybDB2Bggr\n" \
		"BgEFBQcBAQRqMGgwPwYIKwYBBQUHMAKGM2h0dHA6Ly9jcnQudXNlcnRydXN0LmNv\n" \
		"bS9VU0VSVHJ1c3RSU0FBZGRUcnVzdENBLmNydDAlBggrBgEFBQcwAYYZaHR0cDov\n" \
		"L29jc3AudXNlcnRydXN0LmNvbTANBgkqhkiG9w0BAQwFAAOCAgEAMr9hvQ5Iw0/H\n" \
		"ukdN+Jx4GQHcEx2Ab/zDcLRSmjEzmldS+zGea6TvVKqJjUAXaPgREHzSyrHxVYbH\n" \
		"7rM2kYb2OVG/Rr8PoLq0935JxCo2F57kaDl6r5ROVm+yezu/Coa9zcV3HAO4OLGi\n" \
		"H19+24rcRki2aArPsrW04jTkZ6k4Zgle0rj8nSg6F0AnwnJOKf0hPHzPE/uWLMUx\n" \
		"RP0T7dWbqWlod3zu4f+k+TY4CFM5ooQ0nBnzvg6s1SQ36yOoeNDT5++SR2RiOSLv\n" \
		"xvcRviKFxmZEJCaOEDKNyJOuB56DPi/Z+fVGjmO+wea03KbNIaiGCpXZLoUmGv38\n" \
		"sbZXQm2V0TP2ORQGgkE49Y9Y3IBbpNV9lXj9p5v//cWoaasm56ekBYdbqbe4oyAL\n" \
		"l6lFhd2zi+WJN44pDfwGF/Y4QA5C5BIG+3vzxhFoYt/jmPQT2BVPi7Fp2RBgvGQq\n" \
		"6jG35LWjOhSbJuMLe/0CjraZwTiXWTb2qHSihrZe68Zk6s+go/lunrotEbaGmAhY\n" \
		"LcmsJWTyXnW0OMGuf1pGg+pRyrbxmRE1a6Vqe8YAsOf4vmSyrcjC8azjUeqkk+B5\n" \
		"yOGBQMkKW+ESPMFgKuOXwIlCypTPRpgSabuY0MLTDXJLR27lk8QyKGOHQ+SwMj4K\n" \
		"00u/I5sUKUErmgQfky3xxzlIPK1aEn8=\n" \
		"-----END CERTIFICATE-----\n";
	String wNow = emptyString;
	const int Max_Unspecify_ContentLength = 2048;
	const int interval = 300 * 1000000; // in microseconds
	int continuous_failed_count = 0;
	int64_t next_request_time = 0;
	float now_temp = TEMPSENSOR_DISCONNECT;
	String now_weather = emptyString;
	/// <returns>
	/// 0: success,
	/// 1: https return error,
	/// 2: deserializeJson error,
	/// 3: unexpected content,
	/// 4: unexpected invoke,
	/// -1: https request failed,
	/// </returns>
	int make_query_now(StaticJsonDocument<1024>& doc);
	// get "now.temp" from json doc
	float jsonget_now_temp(StaticJsonDocument<1024>& doc);
	// get "now.text" from json doc
	String jsonget_now_weather(StaticJsonDocument<1024>& doc);
public:
	WeatherAPI()
	{
#ifdef QWeather_key
#ifdef QWeather_location
		wNow = "https://devapi.qweather.com/v7/weather/now?location=" + String(QWeather_location) + "&key=" + String(QWeather_key);
#endif
#endif
	}
	int64_t NextUpdateDelaySec();
	/// <returns>
	/// 0: updated and got updated data, &gt;0: not update, got cached data, &lt;0: updated but failed, got cached data
	/// </returns>
	int GetNowWeather(String& weather, float& temp);
};

#endif
