#pragma once

#ifndef _WIRECONFIG_h
#define _WIRECONFIG_h

#include <hal/ledc_types.h>
#include <driver/pcnt.h>

#define LOCAL_HOSTNAME "FridgeMini"
#define LOCAL_AP "FridgeMini"
#define LOCAL_APPWD "FridgeMini0123"
#define ON HIGH
#define OFF LOW

#define TEMPSENSOR_MIN -20
#define TEMPSENSOR_ERR -124
#define TEMPSENSOR_CRC -125
#define TEMPSENSOR_BADDATA -126
#define TEMPSENSOR_DISCONNECT -127
#define TEMPSENSOR_DRIVER -128


// 用来读本地室温 非必须
//#define QWeather_location "101200408"  // locationID 查询 https://raw.githubusercontent.com/qwd/LocationList/master/China-City-List-latest.csv
//#define QWeather_key "00000"  // api key 注册 https://id.qweather.com/#/register

// 天猫精灵 精灵技能 非必须
//#define Aligenie_AuthenticationFile_Path "/aligenie/???.txt"  // 认证文件 (精灵服务器认证我)
//#define Aligenie_AuthenticationFile_Content "???"  // 认证文件内容 (精灵服务器认证我)
//#define Aligenie_Authorization_Header "???"  // 请求头 (我认证精灵服务器)


// DS18B20地址
// 制冷片冷面温度 // cool
#define DS_a_ADDR 0xDB177DD4454DB028ull;  
// 制冷片热面温度 // heat
#define DS_b_ADDR 0xCD72E9D445BD3B28ull;  
// 箱内温度 // box 
#define DS_c_ADDR 0xDF3DC5D4459B9328ull;  
// 所有温度传感器连一根线
#define DS_DATA_PIN 32

// 箱内循环风扇转速
#define BOX_TACH_PIN 36
#define BOX_TACH_PCNT_UNIT PCNT_UNIT_6
// 散热器风扇转速
#define HEAT_TACH_PIN 39
#define HEAT_TACH_PCNT_UNIT PCNT_UNIT_5
// 水泵转速
#define PUMP_TACH_PIN 34
#define PUMP_TACH_PCNT_UNIT PCNT_UNIT_4


// 制冷片PWM控制
#define TEC1_PWM_PIN 22
#define TEC1_PWM_CHANNEL LEDC_CHANNEL_4
// 散热器风扇PWM控制
#define HEAT_PWM_PIN 17
#define HEAT_PWM_CHANNEL LEDC_CHANNEL_5
// 箱内循环风扇PWM控制
#define BOX_PWM_PIN 16
#define BOX_PWM_CHANNEL LEDC_CHANNEL_6


// 冷藏温度设定 [-10, 10] or idle
#define TEMP_SETTING_LLIM -10
#define TEMP_SETTING_HLIM 10
#define TEMP_SETTING_IDLE 20
#define FILE_FRIDGETEMP "/fridgetemp.txt"
// WIFI模式设定 1=STA, 2=AP, 3=APSTA
#define FILE_WIFIMODE "/wifimode.txt"
// 字体文件
#define FT_CN_16 "msyh_82@16pix"
#define FT_WETH_16 "qweather_46@16pix"
#define FT_NUM_24 "dsDigii_19@24pix"
#define FT_NUM_32 "dsDigii_19@32pix"
#define FT_NUM_40 "dsDigii_19@40pix"


#endif

/*
 "                               ┌─── ───┐ \n" +
 "                               │             │ \n" +
 "                          EN - │EN        D23│ ~ GPIO23 / VSPI_MOSI  ---> TFT SDA \n" +
 "   BOX_TACH_PIN <---  GPIO36 - │VP        D22│ ~ GPIO22              ---> TEC1_PWM_PIN \n" +
 "  HEAT_TACH_PIN <---  GPIO39 - │VN        TX0│ ~ GPIO1 \n" +
 "  PUMP_TACH_PIN <---  GPIO34 - │D34       RX0│ ~ GPIO3 \n" +
 "                      GPIO35 - │D35       D21│ ~ GPIO21              ---> TFT BLK \n" +
 "    DS_DATA_PIN <---  GPIO32 ~ │D32       D19│ ~ GPIO19 / VSPI_MISO  ---> TFT DC \n" +
 "                      GPIO33 ~ │D33       D18│ ~ GPIO18 / VSPI_CLK   ---> TFT SCK \n" +
 "                      GPIO25 ~ │D25        D5│ ~ GPIO5  / VSPI_SS \n" +
 "                      GPIO26 ~ │D26       TX2│ ~ GPIO17              ---> HEAT_PWM_PIN \n" +
 "                      GPIO27 ~ │D27       RX2│ ~ GPIO16              ---> BOX_PWM_PIN \n" +
 "          HSPI_CLK  / GPIO14 ~ │D14        D4│ ~ GPIO4               ---> TFT RES \n" +
 "          HSPI_MISO / GPIO12 ~ │D12        D2│ ~ GPIO2  / LED_BUILTIN \n" +
 "          HSPI_MOSI / GPIO13 ~ │D13       D15│ ~ GPIO15 / HSPI_SS \n" +
 "                         GND - │GND       GND│ - GND                 ---> TFT GND \n" +
 "                          5v - │VIN       3V3│ - 3v3                 ---> TFT VCC \n" +
 "                               │             │ \n" +
 "                               │[RST]  [BOOT]│ \n" +
 "                               └──[USB]──┘ \n"
*/

/*
Bit resolution | Min Frequency [Hz] | Max Frequency [Hz]
             1 |                489 |           40078277
             2 |                245 |           20039138
             3 |                123 |           10019569
             4 |                 62 |            5009784
             5 |                 31 |            2504892
             6 |                 16 |            1252446
             7 |                  8 |             626223
             8 |                  4 |             313111
             9 |                  2 |             156555
            10 |                  1 |              78277
            11 |                  1 |              39138
            12 |                  1 |              19569
            13 |                  1 |               9784
            14 |                  1 |               4892
            15 |                  2 |               2446
            16 |                  1 |               1223
            17 |                  1 |                611
            18 |                  2 |                305
            19 |                  1 |                152
            20 |                  1 |                 76
*/
