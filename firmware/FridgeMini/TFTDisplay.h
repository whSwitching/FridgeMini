#pragma once
// TFTDisplay.h

#ifndef _TFTDISPLAY_h
#define _TFTDISPLAY_h

#include <esp_timer.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "WireConfig.h"
#include "Utils.h"


class TFTDisplay
{
	const int32_t DISPLAY_UPDATE_INTERVAL_NS = 1000000; // microseconds
	int64_t display_last_update = 0;

	// background print or not
	bool layer0 = false;
	// warn area reset or not
	bool warnArea = false;
	int m = -1;
	int d = -1;
	int hh = -1;
	int mm = -1;
	String weather = emptyString;
	String rssiInd = emptyString;
	String errorMsg = emptyString;
	String warnMsg = emptyString;
	TFT_eSPI tft = TFT_eSPI();
	int32_t warnMsgY = 0;
	int32_t warnMsgH = 0;
	// background and titles, will not change
	void printLayer0(display_param_t* v);
	// values may change
	void printLayer1(display_param_t* v);
	// values will change everytime updated
	void printLayer2(display_param_t* v);
	void printLayerErr();
	void printTempTitle(int32_t tlx, int32_t tly, int32_t w, int32_t h, const char* title);
	void printTitle(int32_t tlx, int32_t tly, int32_t w, int32_t h, const char* title, const char* unit);
	void printTemp(int32_t tlx, int32_t tly, int32_t w, int32_t h, float val, float color_thresholdL, float color_thresholdH);
	void printFan(int32_t tlx, int32_t tly, int32_t w, int32_t h, uint16_t rpm);
	String dateWeekdayTimeString(tm* ptm)
	{
		char date[11];
		sprintf(date, "%4d-%02d-%02d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);

		String weekDays[] = { "\u5468\u65e5", "\u5468\u4e00", "\u5468\u4e8c", "\u5468\u4e09", "\u5468\u56db", "\u5468\u4e94", "\u5468\u516d" };

		char time[6];
		sprintf(time, "%02d:%02d", ptm->tm_hour, ptm->tm_min);

		return String(date) + " " + weekDays[ptm->tm_wday] + " " + String(time);
	}
	String uptimeString(uint32_t uptimeMs)
	{
		uint32_t min = uptimeMs / 60000 % 60;
		uint32_t hr = uptimeMs / 60000 / 60;
		char time[8];
		sprintf(time, "%d:%02d", hr, min);
		return String(time);
	}
public:
	void Begin();
	void ScreenTest();
	bool RequireUpdateScreen();
	void UpdateScreen(display_param_t* v);
	void ShowError(String err);
	void ShowWarning(String warn);
};

#endif
