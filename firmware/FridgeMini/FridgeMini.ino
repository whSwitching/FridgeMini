
#include <rom/rtc.h>
#include "WireConfig.h"
#include "SysInfo.h"
#include "Utils.h"
#include "TFTDisplay.h"
#include "Network.h"
#include "TempSensor.h"
#include "FanControl.h"
#include "FridgeControl.h"
#include "HttpService.h"
//
TFTDisplay display;
Network net;
FridgeControl tec1;
FanControl pumpFan;
FanControl sinkFan;
FanControl boxFan;
TempSensor sensors;
HttpService http;

display_param_t vals;

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS begin failed!");
        while (1)
            yield(); // Stay here
    }
    Serial.println("SPIFFS OK");

    xTaskCreatePinnedToCore(sensors_read_core0task, "sensors_task", 20480, NULL, 1, NULL, PRO_CPU_NUM);

    sensors.Begin();
    tec1.Begin();
    
    display.Begin();
    display.ScreenTest();
    display.UpdateScreen(NULL);

    net.Wifi_Begin("Not-Free", "wanxiao8192");
    http.Http_Begin(&net, update_values);

    //SysInfo::Print_PartitionTable();
    
    //sensors.PrintAddresses();
    
    Serial.printf("last reset: %s\n[SETUP] END\n", reset_reason_verbose(esp_reset_reason()));
}


void sensors_read_core0task(void* parameter)
{
    pumpFan.Begin(PUMP_TACH_PCNT_UNIT, PCNT_CHANNEL_0, PUMP_TACH_PIN);
    sinkFan.Begin(HEAT_TACH_PCNT_UNIT, PCNT_CHANNEL_0, HEAT_TACH_PIN, HEAT_PWM_CHANNEL, HEAT_PWM_PIN);
    boxFan.Begin(BOX_TACH_PCNT_UNIT, PCNT_CHANNEL_0, BOX_TACH_PIN, BOX_PWM_CHANNEL, BOX_PWM_PIN);
    while (true)
    {
        pumpFan.CalculateRpm();
        sinkFan.CalculateRpm();
        boxFan.CalculateRpm();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        //log_i("unused stack size %d on sensors_read_core0task", uxTaskGetStackHighWaterMark(NULL));
    }
}

void loop()
{
    sensors.RequestTemp();

    bool updateCtrl = tec1.RequireAutoControl();
    bool updateScreen = display.RequireUpdateScreen();
    if (updateCtrl || updateScreen)
    {
        update_values(&vals);
        if (updateCtrl)
        {
            tec1.AutoControl(&vals, &display);
            update_control(&vals);
        }
        if (updateScreen)
        {
            display.UpdateScreen(&vals);
        }
    }

    net.Ntp_Update();

    http.Http_Loop();

    //log_i("unused stack size %d on loop", uxTaskGetStackHighWaterMark(NULL));
}

void update_control(display_param_t* v)
{
    tec1.SetPowerPercent(v->pwTEC);
    sinkFan.SetPowerPercent(v->pwSink);
    boxFan.SetPowerPercent(v->pwBox);
}

void update_values(display_param_t* v)
{
    v->wifiModeSetting = net.Get_WifiModeSetting();
    v->tempSetting = net.Get_FridgeTempSetting();
    v->localhost = net.Wifi_GetLocalIP();
    v->rssi = net.Wifi_GetRSSI();
    v->time = net.Ntp_GetTime();
    net.Weather_GetNow(v->weather, v->tempAir);
    //v->weather = "";
    //v->tempAir = TEMPSENSOR_DISCONNECT;
    v->tempCool = sensors.GetTempCool();
    v->tempHeat = sensors.GetTempHeat();
    v->tempBox = sensors.GetTempBox();
    v->rpmPump = pumpFan.GetSpeedRpm();
    v->pwPump = pumpFan.GetPowerPercent();
    v->rpmSink = sinkFan.GetSpeedRpm();
    v->pwSink = sinkFan.GetPowerPercent();
    v->rpmBox = boxFan.GetSpeedRpm();
    v->pwBox = boxFan.GetPowerPercent();
    v->pwTEC = tec1.GetPowerPercent();
    v->upTimeMs = millis();
    v->resetReason = esp_reset_reason();
}
