//
//
//

#include "HttpService.h"

static const char OtaPageHtml[] PROGMEM = "<html><head><meta charset='utf-8'><title>OTA</title><style>span { color:blue;}</style></head><body><h1>Firmware Update</h1><form method='POST' action='' enctype='multipart/form-data'><input type='file' accept='.bin' name='firmware'><input type='submit' value='Upload'></form><hr/><h1>FileSystem Upload</h1><form method='POST' action='' enctype='multipart/form-data'><input type='file' accept='.bin' name='filesystem'><input type='submit' value='Upload'></form></body></html>";

static const char OtaSuccessPageHtml[] PROGMEM = "<html><head><meta charset='utf-8'><meta http-equiv='refresh' content='15;URL=/'><title>OTA</title><style>span{color:blue;}</style></head><body><center><h3>Update Success! Rebooting...Please just wait</h3></center></body></html>";

void HttpService::Http_Begin(Network* net, get_sensorvalues_delegate get_sensorvalues)
{
	digitalWrite(LED_BUILTIN, ON);
	log_i("WebServer Init...");
	
	_net = net;
	_get_sensorvalues = get_sensorvalues;

	webserver.on("/", [this]() { http_root(); });
	webserver.on("/ctrl", [this]() { http_control(); });
	webserver.on("/ota", HTTP_GET, [this]() { http_ota(); });
	webserver.on("/ota", HTTP_POST, [this]() { http_ota(); }, [this]() { http_ota_upload(); });
	webserver.onNotFound([this]() { http_notfound(); });

#ifdef Aligenie_AuthenticationFile_Path
#ifdef Aligenie_AuthenticationFile_Content
#ifdef Aligenie_Authorization_Header
	webserver.on("/aligenie/serivce", [this]() { http_aligenie_serivce(); });
	webserver.on(Aligenie_AuthenticationFile_Path, [this]() { http_aligenie_auth(); });
#endif
#endif
#endif

	webserver.begin();
	log_i("WebServer Setup.");
	digitalWrite(LED_BUILTIN, OFF);
}

void HttpService::http_root()
{
	if (webserver.method() == HTTP_POST && webserver.args() > 0)
	{
		String board = webserver.arg("board");
		if (board == "WifiScan")
		{
			wifiscanResult = wifi_scan();
		}
		else if (board == "SetTemp" && webserver.hasArg("temp"))
		{
			String v = webserver.arg("temp");
			if (!v.isEmpty() && _net)
			{
				int t = v.toInt();
				if ((t >= TEMP_SETTING_LLIM && t <= TEMP_SETTING_HLIM) || t == TEMP_SETTING_IDLE)
				{
					_net->Set_FridgeTempSetting(t);
				}
			}
		}
		else if (board == "SetWifiMode" && webserver.hasArg("wifimode"))
		{
			String v = webserver.arg("wifimode");
			if (!v.isEmpty())
			{
				int m = v.toInt();
				if (m == 1 || m == 2 || m == 3)
				{
					if (_net->Set_WifiModeSetting((WiFiMode_t)m) > 2)
					{
						webserver.client().setNoDelay(true);
						webserver.send_P(200, PSTR("text/html"), "<html><head><meta charset='utf-8'><meta http-equiv='refresh' content='5;URL=/'><title>Reboot</title><style>span{color:blue;}</style></head><body><center><h3>Settings Changed, Rebooting...</h3></center></body></html>");
						delay(100);
						webserver.client().stop();
						ESP.restart();
					}
				}
			}
		}
		else if (board == "Restart")
		{
			webserver.client().setNoDelay(true);
			webserver.send_P(200, PSTR("text/html"), "<html><head><meta charset='utf-8'><meta http-equiv='refresh' content='5;URL=/'><title>Reboot</title><style>span{color:blue;}</style></head><body><center><h3>Rebooting...</h3></center></body></html>");
			delay(100);
			webserver.client().stop();
			ESP.restart();
		}
	}

	display_param_t* v = NULL;
	if (_get_sensorvalues)
	{
		v = new display_param_t();
		_get_sensorvalues(v);
	}

	String html = html_header();
	html += board_pinout();
	html += info_table();
	html += wifiscanResult;
	html += http_root_controlpanel(v);
	html += "</body></html>";
	webserver.send(200, "text/html", html);

	if (v)
		delete v;
}

void HttpService::http_control()
{
	if (_get_sensorvalues)
	{
		display_param_t v = {};
		_get_sensorvalues(&v);

		StaticJsonDocument<512> d;
		d["Valid"] = true;
		d["TempSetting"] = v.tempSetting;
		d["TempBox"] = v.tempBox;
		d["TempCool"] = v.tempCool;
		d["TempHeatsink"] = v.tempHeat;
		d["BoxRpm"] = v.rpmBox;
		d["PumpRpm"] = v.rpmPump;
		d["HeatsinkRpm"] = v.rpmSink;
		d["BoxPower"] = v.pwBox;
		d["PumpPower"] = v.pwPump;
		d["HeatsinkPower"] = v.pwSink;
		d["Tec1Power"] = v.pwTEC;
		d["TempSettingBefore"] = v.tempSetting;

		if (webserver.method() == HTTP_GET)
		{
			String json;
			serializeJson(d, json);
			webserver.send(200, "application/json", json);
			return;
		}
		else if (webserver.method() == HTTP_POST && webserver.args() > 0 && webserver.hasArg("temp"))
		{
			String st = webserver.arg("temp");
			if (!st.isEmpty() && _net)
			{
				int t = st.toInt();
				if ((t >= TEMP_SETTING_LLIM && t <= TEMP_SETTING_HLIM) || t == TEMP_SETTING_IDLE)
				{
					_net->Set_FridgeTempSetting(t);
					d["TempSetting"] = _net->Get_FridgeTempSetting();
				}
			}
			String json;
			serializeJson(d, json);
			webserver.send(200, "application/json", json);
			return;
		}
	}
	webserver.send(200, "application/json", "{\"Valid\":false}");
}

#ifdef Aligenie_AuthenticationFile_Path
#ifdef Aligenie_AuthenticationFile_Content
#ifdef Aligenie_Authorization_Header
void HttpService::http_aligenie_auth()
{
	webserver.send(200, "text/plain", Aligenie_AuthenticationFile_Content);
}

void HttpService::http_aligenie_serivce()
{
	if (webserver.header("Authorization") == Aligenie_Authorization_Header)
	{
		DynamicJsonDocument doc(2048);
		DeserializationError deError = deserializeJson(doc, webserver.arg("plain"));
		if (!deError)
		{
			if (doc["intentName"] == "FridgeGet")
			{
				if (_get_sensorvalues)
				{
					display_param_t v = {};
					_get_sensorvalues(&v);

					String thing;
					String action;
					for (JsonObject slot : doc["slotEntities"].as<JsonArray>())
					{
						if (slot["slotName"] == "object:object")
							thing = String(slot["standardValue"].as<JsonString>().c_str());
						else if (slot["slotName"] == "action:action")
							action = String(slot["standardValue"].as<JsonString>().c_str());
					}
					if (thing == "fridge" && action != emptyString)
					{
						if (v.tempSetting != TEMP_SETTING_IDLE)
							webserver.send(200, "application/json", "{\"returnCode\":\"0\",\"returnValue\":{\"reply\":\"当前设定是" + String(v.tempSetting) + "度, 冰箱里是" + String(v.tempBox) + "度\",\"resultType\":\"RESULT\",\"executeCode\":\"SUCCESS\"}}");
						else
							webserver.send(200, "application/json", "{\"returnCode\":\"0\",\"returnValue\":{\"reply\":\"当前在待机, 冰箱里是" + String(v.tempBox) + "度\",\"resultType\":\"RESULT\",\"executeCode\":\"SUCCESS\"}}");
					}
					else
					{
						log_e("aligenie request body failed to read object/action\n");
						webserver.send(200, "application/json", "{\"returnCode\":\"1\",\"returnValue\":{\"reply\":\"好像服务有点问题\",\"resultType\":\"RESULT\",\"executeCode\":\"PARAMS_ERROR\"}}");
					}
				}
				else
					webserver.send(200, "application/json", "{\"returnCode\":\"0\",\"returnValue\":{\"reply\":\"查不到, 冰箱固件有问题吧\",\"resultType\":\"RESULT\",\"executeCode\":\"SUCCESS\"}}");
			}
			else if (doc["intentName"] == "FridgeSet")
			{
				if (_get_sensorvalues && _net)
				{
					display_param_t v = {};
					_get_sensorvalues(&v);

					String thing;
					String tempstr;
					for (JsonObject slot : doc["slotEntities"].as<JsonArray>())
					{
						if (slot["slotName"] == "object:object")
							thing = String(slot["standardValue"].as<JsonString>().c_str());
						else if (slot["slotName"] == "temp:temp")
							tempstr = String(slot["standardValue"].as<JsonString>().c_str());
					}
					if (thing == "fridge" && tempstr != emptyString)
					{
						int temp = v.tempSetting;
						if (tempstr == "up")
							temp += 5;
						else if (tempstr == "down")
							temp -= 5;
						else
							temp = tempstr.toInt();
						if (temp == v.tempSetting)
						{
							// no change
							webserver.send(200, "application/json", "{\"returnCode\":\"0\",\"returnValue\":{\"reply\":\"帮你看了下, 已经是" + String(temp) + "度不用调\",\"resultType\":\"RESULT\",\"executeCode\":\"SUCCESS\"}}");
						}
						else if (temp == TEMP_SETTING_IDLE)
						{
							// turn off
							webserver.send(200, "application/json", "{\"returnCode\":\"0\",\"returnValue\":{\"reply\":\"本来是" + String(v.tempSetting) + "度, 现在待机在\",\"resultType\":\"RESULT\",\"executeCode\":\"SUCCESS\"}}");
						}
						else if (temp > TEMP_SETTING_HLIM)
						{
							// too hight
							webserver.send(200, "application/json", "{\"returnCode\":\"0\",\"returnValue\":{\"reply\":\"已经" + String(v.tempSetting) + "度了再调干脆拿出来\",\"resultType\":\"RESULT\",\"executeCode\":\"SUCCESS\"}}");
						}
						else if (temp < TEMP_SETTING_LLIM)
						{
							// too low
							webserver.send(200, "application/json", "{\"returnCode\":\"0\",\"returnValue\":{\"reply\":\"已经" + String(v.tempSetting) + "度了再低冻成冰块怎么喝\",\"resultType\":\"RESULT\",\"executeCode\":\"SUCCESS\"}}");
						}
						else
						{
							// normal range
							_net->Set_FridgeTempSetting(temp);
							int tempAfter = _net->Get_FridgeTempSetting();
							webserver.send(200, "application/json", "{\"returnCode\":\"0\",\"returnValue\":{\"reply\":\"已经从" + String(v.tempSetting) + "调到" + String(tempAfter) + "度了\",\"resultType\":\"RESULT\",\"executeCode\":\"SUCCESS\"}}");
						}
					}
					else
					{
						log_e("aligenie request body failed to read object/temp\n");
						webserver.send(200, "application/json", "{\"returnCode\":\"1\",\"returnValue\":{\"reply\":\"好像服务有点问题\",\"resultType\":\"RESULT\",\"executeCode\":\"PARAMS_ERROR\"}}");
					}
				}
				else
				{
					log_e("aligenie request but _get_sensorvalues/network delegate null\n");
					webserver.send(200, "application/json", "{\"returnCode\":\"0\",\"returnValue\":{\"reply\":\"查不到, 冰箱固件不对呀\",\"resultType\":\"RESULT\",\"executeCode\":\"SUCCESS\"}}");
				}
			}
			else
			{
				log_e("aligenie request body intentName error: '%s'\n", doc["intentName"].as<JsonString>().c_str());
				webserver.send(200, "application/json", "{\"returnCode\":\"-1\",\"returnValue\":{\"reply\":\"好像参数有点问题\",\"resultType\":\"RESULT\",\"executeCode\":\"PARAMS_ERROR\"}}");
			}
		}
		else
		{
			log_e("aligenie request body Deserialization error '%s'\n", deError.c_str());
			webserver.send(200, "application/json", "{\"returnCode\":\"1\",\"returnValue\":{\"reply\":\"好像设备有点问题\",\"resultType\":\"RESULT\",\"executeCode\":\"EXECUTE_ERROR\"}}");
		}
	}
	else
	{
		webserver.send(200, "application/json", "{\"returnCode\":\"-1\",\"returnValue\":{\"reply\":\"未授权的调用\",\"resultType\":\"RESULT\",\"executeCode\":\"PARAMS_ERROR\"}}");
	}
}
#endif
#endif
#endif

void HttpService::http_ota()
{
	if (webserver.method() == HTTP_GET)
	{
		if (_otaUsername != emptyString && _otaPassword != emptyString && !webserver.authenticate(_otaUsername.c_str(), _otaPassword.c_str()))
			return webserver.requestAuthentication();
		webserver.send_P(200, PSTR("text/html"), OtaPageHtml);
	}
	else if (webserver.method() == HTTP_POST)
	{
		if (!_otaAuthenticated)
			return webserver.requestAuthentication();
		if (Update.hasError())
		{
			webserver.send(200, F("text/html"), String(F("Update error: ")) + _otaError);
		}
		else
		{
			webserver.client().setNoDelay(true);
			webserver.send_P(200, PSTR("text/html"), OtaSuccessPageHtml);
			delay(100);
			webserver.client().stop();
			ESP.restart();
		}
	}
}

void HttpService::http_ota_upload()
{
	//handler for the file upload, get sketch bytes, write through the Update object
	HTTPUpload& upload = webserver.upload();
	if (upload.status == UPLOAD_FILE_START)
	{
		_otaError.clear();

		_otaAuthenticated = (_otaUsername == emptyString || _otaPassword == emptyString || webserver.authenticate(_otaUsername.c_str(), _otaPassword.c_str()));
		if (!_otaAuthenticated)
		{
			log_i("[OTA] Unauthenticated Update");
			return;
		}
		log_i("[OTA] Update: %s", upload.filename.c_str());

		if (upload.name == "filesystem")
		{
			if (!Update.begin(SPIFFS.totalBytes(), U_SPIFFS)) //start with max available size
				Update.printError(Serial);
		}
		else
		{
			uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
			if (!Update.begin(maxSketchSpace, U_FLASH)) //start with max available size
				set_otaError();
		}
	}
	else if (_otaAuthenticated && upload.status == UPLOAD_FILE_WRITE && !_otaError.length())
	{
		log_i("[OTA] .");
		if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
			set_otaError();
	}
	else if (_otaAuthenticated && upload.status == UPLOAD_FILE_END && !_otaError.length())
	{
		if (Update.end(true)) //true to set the size to the current progress
			log_i("[OTA] Update Success: %u\nRebooting...\n", upload.totalSize);
		else
			set_otaError();
	}
	else if (_otaAuthenticated && upload.status == UPLOAD_FILE_ABORTED)
	{
		Update.end();
		log_e("[OTA] Update was aborted");
	}
	delay(0);
}

String HttpService::http_root_controlpanel(display_param_t* v)
{
	String html = "<table width=\"95%\" border=\"1\">";
	html += "<tr><th width=\"35%\"><h3>Temperature</h3></th><th width=\"35%\"><h3>Fan</h3></th><th width=\"30%\"><h3>Setting</h3></th></tr>";
	html += "<tr><td>" + root_temp_sensor(v) + "</td>";
	html += "<td>" + root_fan_sensor(v) + "</td>";
	html += "<td>" + root_setting() + "</td></tr></table><hr/>";
	return html;
}

String HttpService::root_temp_sensor(display_param_t* v)
{
	String weather = v == NULL ? "" : v->weather;
	float air = v == NULL ? TEMPSENSOR_DISCONNECT : v->tempAir;
	float box = v == NULL ? TEMPSENSOR_DISCONNECT : v->tempBox;
	float cool = v == NULL ? TEMPSENSOR_DISCONNECT : v->tempCool;
	float heat = v == NULL ? TEMPSENSOR_DISCONNECT : v->tempHeat;

	String airTemp = "";
	if (air == TEMPSENSOR_DISCONNECT)
		airTemp = "?";
	else
		airTemp = String(air, 0);

	String boxTemp = "";
	if (box > TEMPSENSOR_MIN)
		boxTemp = String(box, 1);
	else if (box != TEMPSENSOR_DISCONNECT)
		boxTemp = "unexpected";

	String coolTemp = "";
	if (cool > TEMPSENSOR_MIN)
		coolTemp = String(cool, 1);
	else if (box != TEMPSENSOR_DISCONNECT)
		coolTemp = "unexpected";

	String heatTemp = "";
	if (heat > TEMPSENSOR_MIN)
		heatTemp = String(heat, 1);
	else if (heat != TEMPSENSOR_DISCONNECT)
		heatTemp = "unexpected";

	String diff = (heat > TEMPSENSOR_MIN && cool > TEMPSENSOR_MIN) ? String(heat - cool, 1) : "";

	return "<p>Weather: " + airTemp + " ℃ " + weather + 
		"</p><p>Storage: " + boxTemp + 
		" ℃</p><p>Refrigerating: " + coolTemp + " / " + heatTemp + 
		" ℃</p><p>Diff: " + diff + " ℃</p>";
}

String HttpService::root_fan_sensor(display_param_t* v)
{
	float pwPump = 0;
	float pwSink = 0;
	float pwBox = 0;
	float pwTec = 0;
	uint16_t pump = 0;
	uint16_t sink = 0;
	uint16_t box = 0;
	if (v)
	{
		pwPump = v->pwPump * 100;
		pwSink = v->pwSink * 100;
		pwBox = v->pwBox * 100;
		pwTec = v->pwTEC * 100;
		pump = v->rpmPump;
		sink = v->rpmSink;
		box = v->rpmBox;
	}
	return "<p>Storage: " + String(pwBox, 0) + " % power @ " + String(box) + " rpm</p><p>Pump: " +
		String(pwPump, 0) + " % power @ " + String(pump) + " rpm</p><p>HeatSink: " +
		String(pwSink, 0) + " % power @ " + String(sink) + " rpm</p><p>Refrigerating: " +
		String(pwTec, 0) + " % power</p>";
}

String HttpService::root_setting()
{
	if (_net)
	{
		int t = _net->Get_FridgeTempSetting();
		String html = "<Form action=\"\" method=\"post\">Thermostatic: <select name=\"temp\">";
		html += "<option value=\"20\" " + String(t > 10 ? "selected" : "") + "> Stop</option>";
		html += "<option value=\"10\" " + String(t == 10 ? "selected" : "") + "> 10 ℃</option>";
		html += "<option value=\"5\" " + String(t == 5 ? "selected" : "") + "> 5 ℃</option>";
		html += "<option value=\"0\" " + String(t == 0 ? "selected" : "") + "> 0 ℃</option>";
		html += "<option value=\"-5\" " + String(t == -5 ? "selected" : "") + "> -5 ℃</option>";
		html += "<option value=\"-10\" " + String(t == -10 ? "selected" : "") + "> -10 ℃</option>";
		html += "</select> <input name=\"board\" type=\"submit\" value=\"SetTemp\"></form>";

		int m = _net->Get_WifiModeSetting();
		html += "<Form action=\"\" method=\"post\">WIFI Mode: <select name=\"wifimode\">";
		html += "<option value=\"1\" " + String(m == 1 ? "selected" : "") + "> STA</option>";
		html += "<option value=\"2\" " + String(m == 2 ? "selected" : "") + "> AP</option>";
		html += "<option value=\"3\" " + String(m == 3 ? "selected" : "") + "> STA+AP</option>";
		html += "</select> <input name=\"board\" type=\"submit\" value=\"SetWifiMode\"></form>";

		return html;
	}
	return "";
}

String HttpService::info_table()
{
	String html = "<table width=\"95%\" border=\"1\">";
	html += "<tr><th width=\"35%\"><h3>Board</h3></th><th width=\"35%\"><h3>Network</h3></th><th width=\"30%\"><h3>File System</h3></th></tr>";
	html += "<tr><td>" + info_table_board() + "</td>";
	html += "<td>" + info_table_wifi() + "</td>";
	html += "<td>" + info_table_fs() + "</td></tr></table><hr/>";
	return html;
}

String HttpService::info_table_board()
{
	sys_info_t st;
	SysInfo::Get_Sysinfo(st);
	String html = "";
	html += "<p>Chip ID: <span>" + String(st.chipID) + "</span></p>";
	html += "<p>Chip Model: <span>" + String(st.chipModel) + " Rev " + String(st.chipRevision) + "</span></p>";
	html += "<p>Chip Cores: <span>" + String(st.chipCores) + "</span></p>";
	html += "<p>CPU Frequency: <span>" + String(st.cpuFreqMHz) + " MHz</span></p>";
	html += "<p>SDK Version: <span>" + String(st.sdkVersion) + "</span></p>";
	html += "<p>Flash Chip Mode: <span>" + st.flashChipMode + "</span></p>";
	html += "<p>Flash Chip Frequency: <span>" + String(st.flashChipFreqMHz) + " MHz</span></p>";
	html += "<p>Flash Chip Size: <span>" + String(st.flashChipSizeKB, 1) + " KBytes</span></p>";
	html += "<p>Sketch Size: <span>" + String(st.sketchSizeKB, 1) + " KBytes</span></p>";
	html += "<p>Free Sketch Space: <span>" + String(st.freeSketchSpaceKB, 1) + " KBytes</span></p>";
	html += "<p>Free Internal RAM: <span>" + String(st.freeHeapSizeKB, 1) + " of " + String(st.heapSizeKB, 1) + " KBytes</span></p>";
	html += "<p>Max Block Internal RAM: <span>" + String(st.maxAllocHeapB) + " Bytes</span></p>";
	html += "<p>Free PSRAM: <span>" + String(st.freePsramSizeKB, 1) + " of " + String(st.psramSizeKB, 1) + " KBytes</span></p>";
	html += "<p>Max Block PSRAM: <span>" + String(st.maxAllocPsramKB, 1) + " KBytes</span></p>";
	html += "<p>Last Reset: <span>" + reset_reason_verbose(st.resetReason) + "</span></p>";
	html += "<p><Form action=\"\" method=\"post\"><input name=\"board\" type=\"submit\" value=\"Restart\" /></Form></p>";
	return html;
}

String HttpService::info_table_wifi()
{
	String html = "<p><span>" + (_net != NULL ? _net->Ntp_GetTimeString(true, true, true, false) : "NTP client unavailable") + "</span></p>";
	
	if (WiFi.getMode() == WIFI_STA)
		html += "<p>Mode: STAtion</p><p>SSID: " + String(WiFi.SSID()) + "</p>";
	else if (WiFi.getMode() == WIFI_AP)
		html += "<p>Mode: Access Point</p><p>AP SSID: " + String(WiFi.softAPSSID()) + "</p>";
	else if (WiFi.getMode() == WIFI_AP_STA)
		html += "<p>Mode: STAtion + Access Point</p><p>SSID: " + String(WiFi.SSID()) + "</p><p>AP SSID: " + String(WiFi.softAPSSID()) + "</p>";
	else
		html += "<p>Mode: OFF</p>";
	//html += "<p>PSK: " + String(WiFi.psk()) + "</p>";
	html += "<p>Gateway: " + WiFi.gatewayIP().toString() + "</p>";
	html += "<p>Subnet Mask: " + WiFi.subnetMask().toString() + "</p>";
	if (WiFi.getMode() == WIFI_STA)
	{
		html += "<p>Local IP: " + WiFi.localIP().toString() + "</p>";
		html += "<p>Local IPv6: " + WiFi.localIPv6().toString() + "</p>";
	}
	else if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA)
	{
		html += "<p>Local AP IP: " + WiFi.softAPIP().toString() + "</p>";
		html += "<p>Local AP IPv6: " + WiFi.softAPIPv6().toString() + "</p>";
	}
	html += "<p>Local HostName: " + String(WiFi.getHostname()) + "</p>";
	html += "<p>MAC: " + WiFi.macAddress() + "</p>";
	html += "<p>DNS 1: " + WiFi.dnsIP().toString() + "</p>";
	html += "<p>DNS 2: " + WiFi.dnsIP(1).toString() + "</p>";
	if (WiFi.getMode() != WIFI_AP)
		html += "<p><Form action=\"\" method=\"post\"><input name=\"board\" type=\"submit\" value=\"WifiScan\"><span> (May take few secs)</span></Form></p>";
	return html;
}

String HttpService::info_table_fs()
{
	return "<p>" + fs_info() + "</p><pre>" + list_fs_dir("/", 0) + "</pre>";
}

String HttpService::fs_info()
{
	float used = SPIFFS.usedBytes() / 1024.0f;
	float total = SPIFFS.totalBytes() / 1024.0f;
	float precent = used * 100.0 / total;
	return "<p>Size: " + String(used, 1) + " / " + String(total, 1) + " KBytes, " + String(precent) + "% used</p>";
}

String HttpService::wifi_scan()
{
	if (WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA)
	{
		String html = "<p><h3>Wifi Scan Results</h3></p><table width=\"60%\" border=\"1\"><tr><th width=\"40%\">SSID</th><th width=\"20%\">Channel</th><th width=\"20%\">Signal Strength</th><th width=\"20%\">Encryption Type</th>";
		int n = WiFi.scanNetworks(false, false, true);
		for (int i = 0; i < n; ++i)
		{
			html += "<tr><td>" + WiFi.SSID(i) +
				"</td><td>" + String(WiFi.channel(i)) +
				"</td><td>" + wifi_rssiIndicate(i) +
				"</td><td>" + wifi_authModeString(i) +
				"</td></tr>";
		}
		delay(10);
		html += "</table><hr/>";
		WiFi.scanDelete();
		return html;
	}
	return "";
}
