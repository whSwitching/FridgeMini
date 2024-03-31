#pragma once
// HttpService.h

#ifndef _HTTPSERVICE_h
#define _HTTPSERVICE_h

#define ON HIGH
#define OFF LOW

#include <WebServer.h>
#include <StreamString.h>
#include <Update.h>
#include "WireConfig.h"
#include "SysInfo.h"
#include "Utils.h"
#include "Network.h"

class HttpService
{
	WebServer webserver;
	String wifiscanResult = emptyString;
	String _otaUsername = emptyString;
	String _otaPassword = emptyString;
	String _otaError = emptyString;
	bool _otaAuthenticated = false;
	get_sensorvalues_delegate _get_sensorvalues = NULL;
	Network* _net = NULL;
public:
	void Http_Begin(Network* net, get_sensorvalues_delegate get_sensorvalues);
	void Http_Loop()
	{
		webserver.handleClient();
	}
private:	
	void http_root();
	// invoke from proxy
	void http_control();
#ifdef Aligenie_AuthenticationFile_Path
#ifdef Aligenie_AuthenticationFile_Content
#ifdef Aligenie_Authorization_Header
	// invoke from aligenie
	void http_aligenie_auth();
	// invoke from aligenie
	void http_aligenie_serivce();
#endif
#endif
#endif
	void http_ota();
	void http_ota_upload();
	String http_root_controlpanel(display_param_t* v);
	String root_temp_sensor(display_param_t* v);
	String root_fan_sensor(display_param_t* v);
	String root_setting();
	String info_table();
	String info_table_board();
	String info_table_wifi();
	String info_table_fs();
	String fs_info();
	String wifi_scan();

  String html_header()
  {
    return "<html><head><meta charset=\"utf-8\"><title>Home</title><style>span { color:blue;}</style></head><body><h1>" + String(LOCAL_HOSTNAME) + "</h1><hr/>";
  }
  String board_pinout()
  {
/*
	"                                           ┌─── ───┐ \n" +
	"                                           │             │ \n" +
	"                                      EN - │EN        D23│ ~ GPIO23 / SPI3_MOSI \n" +
	"                         ADC1_0 / GPIO36 - │VP        D22│ ~ GPIO22 / SPI3_WP \n" +
	"                         ADC1_3 / GPIO39 - │VN        TX0│ ~ GPIO1  / I2S0_CLK \n" +
	"                         ADC1_6 / GPIO34 - │D34       RX0│ ~ GPIO3  / I2S1_CLK \n" +
	"                         ADC1_7 / GPIO35 - │D35       D21│ ~ GPIO21 / SPI3_HD \n" +
	"               TOUCH_9 / ADC1_4 / GPIO32 ~ │D32       D19│ ~ GPIO19 / SPI3_MISO \n" +
	"               TOUCH_8 / ADC1_5 / GPIO33 ~ │D33       D18│ ~ GPIO18 / SPI3_CLK \n" +
	"                 DAC_1 / ADC2_8 / GPIO25 ~ │D25        D5│ ~ GPIO5  / SPI3_CS0 \n" +
	"                 DAC_2 / ADC2_9 / GPIO26 ~ │D26       TX2│ ~ GPIO17 \n" +
	"               TOUCH_7 / ADC2_7 / GPIO27 ~ │D27       RX2│ ~ GPIO16 \n" +
	"   SPI2_CLK  / TOUCH_6 / ADC2_6 / GPIO14 ~ │D14        D4│ ~ GPIO4  / ADC2_0 / SPI2_HD  / TOUCH_0 \n" +
	"   SPI2_MISO / TOUCH_5 / ADC2_5 / GPIO12 ~ │D12        D2│ ~ GPIO2  / ADC2_2 / SPI2_WP  / TOUCH_2 / BUILTIN_LED \n" +
	"   SPI2_MOSI / TOUCH_4 / ADC2_4 / GPIO13 ~ │D13       D15│ ~ GPIO15 / ADC2_3 / SPI2_CS0 / TOUCH_3 \n" +
	"                                     GND - │GND       GND│ - GND \n" +
	"                                      5v - │VIN       3V3│ - 3v3 \n" +
	"                                           │             │ \n" +
	"                                           │[RST]  [BOOT]│ \n" +
	"                                           └──[USB]──┘ \n"
*/
	  //return "<pre>\n                                          ┌─── ───┐\n                                          │             │\n                                     EN - │EN        D23│ ~ GPIO23 / SPI3_MOSI\n                        ADC1_0 / GPIO36 - │VP        D22│ ~ GPIO22 / SPI3_WP\n                        ADC1_3 / GPIO39 - │VN        TX0│ ~ GPIO1  / I2S0_CLK\n                        ADC1_6 / GPIO34 - │D34       RX0│ ~ GPIO3  / I2S1_CLK\n                        ADC1_7 / GPIO35 - │D35       D21│ ~ GPIO21 / SPI3_HD\n              TOUCH_9 / ADC1_4 / GPIO32 ~ │D32       D19│ ~ GPIO19 / SPI3_MISO\n              TOUCH_8 / ADC1_5 / GPIO33 ~ │D33       D18│ ~ GPIO18 / SPI3_CLK\n                DAC_1 / ADC2_8 / GPIO25 ~ │D25        D5│ ~ GPIO5  / SPI3_CS0\n                DAC_2 / ADC2_9 / GPIO26 ~ │D26       TX2│ ~ GPIO17\n              TOUCH_7 / ADC2_7 / GPIO27 ~ │D27       RX2│ ~ GPIO16\n  SPI2_CLK  / TOUCH_6 / ADC2_6 / GPIO14 ~ │D14        D4│ ~ GPIO4  / ADC2_0 / SPI2_HD  / TOUCH_0\n  SPI2_MISO / TOUCH_5 / ADC2_5 / GPIO12 ~ │D12        D2│ ~ GPIO2  / ADC2_2 / SPI2_WP  / TOUCH_2 / BUILTIN_LED\n  SPI2_MOSI / TOUCH_4 / ADC2_4 / GPIO13 ~ │D13       D15│ ~ GPIO15 / ADC2_3 / SPI2_CS0 / TOUCH_3\n                                    GND - │GND       GND│ - GND\n                                     5v - │VIN       3V3│ - 3v3\n                                          │             │\n                                          │[RST]  [BOOT]│\n                                          └──[USB]──┘\n</pre><hr/>";
	  
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
 "          HSPI_MISO / GPIO12 ~ │D12        D2│ ~ GPIO2  / BUILTIN_LED \n" +
 "          HSPI_MOSI / GPIO13 ~ │D13       D15│ ~ GPIO15 / HSPI_SS \n" +
 "                         GND - │GND       GND│ - GND                 ---> TFT GND \n" +
 "                          5v - │VIN       3V3│ - 3v3                 ---> TFT VCC \n" +
 "                               │             │ \n" +
 "                               │[RST]  [BOOT]│ \n" +
 "                               └──[USB]──┘ \n"
*/
	  return "<pre>\n                               ┌─── ───┐ \n                               │             │ \n                          EN - │EN        D23│ ~ GPIO23 / VSPI_MOSI  ---> TFT SDA \n   BOX_TACH_PIN <---  GPIO36 - │VP        D22│ ~ GPIO22              ---> TEC1_PWM_PIN \n  HEAT_TACH_PIN <---  GPIO39 - │VN        TX0│ ~ GPIO1 \n  PUMP_TACH_PIN <---  GPIO34 - │D34       RX0│ ~ GPIO3 \n                      GPIO35 - │D35       D21│ ~ GPIO21              ---> TFT BLK \n    DS_DATA_PIN <---  GPIO32 ~ │D32       D19│ ~ GPIO19 / VSPI_MISO  ---> TFT DC \n                      GPIO33 ~ │D33       D18│ ~ GPIO18 / VSPI_CLK   ---> TFT SCK \n                      GPIO25 ~ │D25        D5│ ~ GPIO5  / VSPI_SS \n                      GPIO26 ~ │D26       TX2│ ~ GPIO17              ---> HEAT_PWM_PIN \n                      GPIO27 ~ │D27       RX2│ ~ GPIO16              ---> BOX_PWM_PIN \n          HSPI_CLK  / GPIO14 ~ │D14        D4│ ~ GPIO4               ---> TFT RES \n          HSPI_MISO / GPIO12 ~ │D12        D2│ ~ GPIO2  / BUILTIN_LED \n          HSPI_MOSI / GPIO13 ~ │D13       D15│ ~ GPIO15 / HSPI_SS \n                         GND - │GND       GND│ - GND                 ---> TFT GND \n                          5v - │VIN       3V3│ - 3v3                 ---> TFT VCC \n                               │             │ \n                               │[RST]  [BOOT]│ \n                               └──[USB]──┘ \n</pre><hr/>";
  }
	void http_notfound()
	{
		String html = html_header() + "<p>" + ((webserver.method() == HTTP_POST) ? "POST " : "GET ") + webserver.uri() + "</p><h2>Page not found</h2></body></html>";
		webserver.send(404, "text/html", html);
	}
	void set_otaError()
	{
		Update.printError(Serial);
		StreamString str;
		Update.printError(str);
		_otaError = str.c_str();
	}
};

#endif
