
#include "TFTDisplay.h"

void TFTDisplay::Begin()
{
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    log_i("TFT Init...");

    log_i("SPIFFS FILES:");

    if (!SPIFFS.exists("/" + String(FT_CN_16) + ".vlw") ||
        !SPIFFS.exists("/" + String(FT_WETH_16) + ".vlw") ||
        !SPIFFS.exists("/" + String(FT_NUM_24) + ".vlw") ||
        !SPIFFS.exists("/" + String(FT_NUM_32) + ".vlw") ||
        !SPIFFS.exists("/" + String(FT_NUM_40) + ".vlw"))
    {
        log_e("STOP: Font file missing in SPIFFS.");
        while (1)
            yield();
    }
    else
    {
        String filestree = list_fs_dir("/", 0);
        log_i("\n%s", filestree.c_str());
    }

    layer0 = false;
    warnArea = false;
}

void TFTDisplay::ScreenTest()
{
    tft.fillScreen(TFT_BLACK);

    tft.fillRectHGradient(0, 0, 240, 40, TFT_RED, TFT_WHITE);
    tft.fillRectHGradient(0, 40, 240, 40, TFT_GREEN, TFT_WHITE);
    tft.fillRectHGradient(0, 80, 240, 40, TFT_BLUE, TFT_WHITE);
    tft.fillRectHGradient(0, 120, 240, 40, TFT_BLACK, TFT_RED);
    tft.fillRectHGradient(0, 160, 240, 40, TFT_BLACK, TFT_GREEN);
    tft.fillRectHGradient(0, 200, 240, 40, TFT_BLACK, TFT_BLUE);

    tft.setTextSize(2);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("RED", 120, 0);
    tft.drawString("GREEN", 120, 40);
    tft.drawString("BLUE", 120, 80);
    tft.setTextSize(3);
    tft.drawString("RED", 120, 120);
    tft.drawString("GREEN", 120, 160);
    tft.drawString("BLUE", 120, 200);
    tft.setTextSize(1);

    delay(2000);
    layer0 = false;
    warnArea = false;
}

bool TFTDisplay::RequireUpdateScreen()
{
    int64_t now = esp_timer_get_time();
    return now - display_last_update >= DISPLAY_UPDATE_INTERVAL_NS;
}

void TFTDisplay::UpdateScreen(display_param_t* v)
{
    display_last_update = esp_timer_get_time();
    printLayer0(v);
    printLayer1(v);
    printLayer2(v);
    printLayerErr();
}

void TFTDisplay::ShowError(String err)
{
    if (errorMsg != err)
    {
        errorMsg = err;
        if (errorMsg == emptyString)
        {
            layer0 = false;
            rssiInd = emptyString;
        }
    }
}

void TFTDisplay::ShowWarning(String warn)
{
    if (warnMsg != warn)
    {
        warnMsg = warn;
        if (warnMsg == emptyString)
        {
            warnArea = false;
        }
    }
    
}

void TFTDisplay::printLayer0(display_param_t* v)
{
    if (!layer0 || !warnArea)
    {
        tft.loadFont(FT_CN_16);

        if (!layer0)
        {
            tft.fillScreen(TFT_BLACK);
            tft.drawFastHLine(0, 17, 240, TFT_WHITE);

            int32_t y = 25; // table pos
            int32_t h = 38; // column heigh
            int32_t c = 120;// column width

            printTempTitle(0, y, 240 - c, h, "室温");
            printTempTitle(c, y, 240 - c, h, "冷面");
            y += h;
            printTempTitle(0, y, 240 - c, h, "箱内");
            printTempTitle(c, y, 240 - c, h, "热面");
            y += h;
            printTempTitle(0, y, 240 - c, h, "设定");
            printTempTitle(c, y, 240 - c, h, "温差");
            y += h;
            printTitle(0, y, 240 - c, h, "水泵", "rpm");
            printTitle(c, y, 240 - c, h, "散热", "rpm");
            y += h;

            tft.fillRect(0, y, 65, 16, TFT_BLACK);
            tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(TL_DATUM);
            tft.drawString("制冷输出", 0, y);

            tft.fillRect(c, y, 65, 16, TFT_BLACK);
            tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(TL_DATUM);
            tft.drawString("散热输出", c, y);
            y += 16;

            int32_t lastLh = 16; // last line height
            warnMsgY = y;
            warnMsgH = 240 - y - lastLh;
            // warn msg line
            tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(CL_DATUM);
            tft.drawString("已运行", 0, warnMsgY + warnMsgH / 2);

            tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(CL_DATUM);
            tft.drawString("上次重启", 120, warnMsgY + warnMsgH / 2);

            // last line
            tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(CL_DATUM);
            tft.drawString("本机:", 0, 240 - lastLh / 2);

            tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(CR_DATUM);
            tft.drawString("Wifi:", 183, 240 - lastLh / 2);

            layer0 = true;
        }
        else if (!warnArea)
        {
            tft.fillRect(0, warnMsgY, 240, warnMsgH, TFT_BLACK);
            // warn msg line
            tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(CL_DATUM);
            tft.drawString("已运行", 0, warnMsgY + warnMsgH / 2);

            tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(CL_DATUM);
            tft.drawString("上次重启", 120, warnMsgY + warnMsgH / 2);

            warnArea = true;
        }

        tft.unloadFont();

        if (v)
        {
            // last reset
            tft.fillRect(190, warnMsgY, 50, warnMsgH, TFT_BLACK);
            tft.setTextColor(TFT_RED);
            tft.setTextSize(2);
            tft.setTextDatum(CC_DATUM);
            tft.drawString(reset_reason(v->resetReason), 215, warnMsgY + warnMsgH / 2);
        }
    }
}

void TFTDisplay::printLayer1(display_param_t* v)
{
    if (v)
    {
        int32_t y = 0;
        int32_t h = 16;
        int32_t wethStart = 170;

        if (v->weather != weather)
        {
            tft.fillRect(wethStart, y, 240 - wethStart, h, TFT_BLACK);
            // weather
            tft.loadFont(FT_WETH_16);
            tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(TR_DATUM);
            tft.drawString(v->weather, 240, 0);
            tft.unloadFont();
            weather = v->weather;
        }

        String ind = wifi_rssiIndicate(v->rssi);
        bool updateRssi = ind != rssiInd;
        bool updateTime = v->time && (v->time->tm_mday != d || v->time->tm_mon != m || v->time->tm_min != mm || v->time->tm_hour != hh);
        if (updateTime || updateRssi)
        {
            tft.loadFont(FT_CN_16);
            if (updateTime)
            {
                // date
                tft.fillRect(0, y, wethStart, h, TFT_BLACK);
                tft.setTextColor(TFT_WHITE);
                tft.setTextDatum(TL_DATUM);
                tft.drawString(dateWeekdayTimeString(v->time), 1, 0);
                hh = v->time->tm_hour;
                mm = v->time->tm_min;
                m = v->time->tm_mon;
                d = v->time->tm_mday;
            }
            if (updateRssi)
            {
                // ip
                tft.fillRect(40, 224, 110, h, TFT_BLACK);
                tft.setTextColor(TFT_WHITE);
                tft.setTextDatum(CL_DATUM);
                tft.drawString(v->localhost, 40, 232);
                // rssi
                tft.fillRect(185, 224, 55, h, TFT_BLACK);
                tft.setTextColor(wifi_rssiIndicateColor(v->rssi));
                tft.setTextDatum(CR_DATUM);
                tft.drawString(ind, 238, 232);
                rssiInd = ind;
            }

            // uptime
            tft.fillRect(50, warnMsgY, 70, warnMsgH, TFT_BLACK);
            tft.setTextColor(TFT_WHITE);
            tft.setTextDatum(CC_DATUM);
            tft.drawString(uptimeString(v->upTimeMs), 85, warnMsgY + warnMsgH / 2);

            tft.unloadFont();
        }
    }
}

void TFTDisplay::printLayer2(display_param_t* v)
{
    if (v)
    {
        int32_t x = 35; // table pos
        int32_t y = 25; // table pos
        int32_t h = 38; // cell heigh
        int32_t w = 50; // cell width
        tft.loadFont(FT_NUM_40);
        // ari temp
        printTemp(x, y, w, h, v->tempAir, 10, 37);
        // cool temp
        printTemp(120 + x, y, w, h, v->tempCool, -20, 10);
        y += h;
        // box temp
        printTemp(x, y, w, h, v->tempBox, -10, 10);
        // heat temp
        printTemp(120 + x, y, w, h, v->tempHeat, 20, 60);
        y += h;
        // setting
        printTemp(x, y, w, h, v->tempSetting, TEMP_SETTING_LLIM, TEMP_SETTING_HLIM);
        // diff
        if (temp_value_valid(v->tempHeat) && temp_value_valid(v->tempCool))
            printTemp(120 + x, y, w, h, v->tempHeat - v->tempCool, 20, 50);
        else
            printTemp(120 + x, y, w, h, TEMPSENSOR_ERR, 20, 50);
        y += h;
        tft.unloadFont();

        tft.loadFont(FT_NUM_24);
        // pump rpm
        printFan(x, y, w, h, v->rpmPump);
        // heatsink rpm
        printFan(120 + x, y, w, h, v->rpmSink);
        y += h;

        // tec1 power
        tft.fillRect(70, y, 50, 16, TFT_BLACK);
        tft.setTextColor(pow_color(v->pwTEC));
        tft.setCursor(70, y);
        tft.printf("%.0f %%", v->pwTEC * 100);
        // heatsink power
        tft.fillRect(190, y, 50, 16, TFT_BLACK);
        tft.setTextColor(pow_color(v->pwSink));
        tft.setCursor(190, y);
        tft.printf("%.0f %%", v->pwSink * 100);
        y += 16;

        tft.unloadFont();
    }

    if (warnMsg != emptyString)
    {
        tft.fillRect(0, warnMsgY, 240, warnMsgH, TFT_BLACK);
        //tft.drawRect(0, warnMsgY, 240, warnMsgH, TFT_RED);
        tft.setTextColor(TFT_RED);
        tft.setTextSize(2);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(warnMsg, 120, warnMsgY + warnMsgH / 2);
    }
}

void TFTDisplay::printLayerErr()
{
    if (errorMsg != emptyString)
    {
        tft.fillRect(20, 60, 200, 120, TFT_RED);
        tft.fillRect(30, 70, 180, 100, TFT_WHITE);
        tft.setTextColor(TFT_RED);
        tft.setTextSize(3);
        tft.setTextDatum(CC_DATUM);
        tft.drawString(errorMsg, 120, 120);
    }
}

void TFTDisplay::printTempTitle(int32_t tlx, int32_t tly, int32_t w, int32_t h, const char* title)
{
    tft.fillRect(tlx, tly, w, h, TFT_BLACK);

    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(title, tlx, tly);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("\u2103", tlx + 90, tly);
}

void TFTDisplay::printTitle(int32_t tlx, int32_t tly, int32_t w, int32_t h, const char* title, const char* unit)
{
    tft.fillRect(tlx, tly, w, h, TFT_BLACK);

    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(title, tlx, tly);

    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(unit, tlx + 85, tly);
}

void TFTDisplay::printTemp(int32_t tlx, int32_t tly, int32_t w, int32_t h, float val, float color_thresholdL, float color_thresholdH)
{
    tft.fillRect(tlx, tly, w, h, TFT_BLACK);
    //tft.drawRect(tlx, tly, w, h, TFT_RED);

    tft.setTextDatum(TR_DATUM);
    if (val == TEMPSENSOR_DISCONNECT)
    {
        tft.setTextColor(TFT_RED);
        tft.drawString("N", tlx + w, tly);
        //tft.drawString("\u65AD\u5F00", tlx + w, tly);
    }
    else if (!temp_value_valid(val))
    {
        tft.setTextColor(TFT_YELLOW);
        tft.drawString("E", tlx + w, tly);
        //tft.drawString("\u9519\u8BEF", tlx + w, tly);
    }
    else if (color_thresholdL == TEMP_SETTING_LLIM && color_thresholdH == TEMP_SETTING_HLIM && val == TEMP_SETTING_IDLE)
    {
        tft.setTextColor(TFT_WHITE);
        tft.drawString("?", tlx + w, tly);
    }
    else
    {
        tft.setTextColor(temp_color(val, color_thresholdL, color_thresholdH));
        tft.drawFloat(val, 0, tlx + w, tly);
    }
}

void TFTDisplay::printFan(int32_t tlx, int32_t tly, int32_t w, int32_t h, uint16_t rpm)
{
    tft.fillRect(tlx, tly, w, h, TFT_BLACK);
    //tft.drawRect(tlx, tly, w, h, TFT_RED);

    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(rpm_color(rpm));
    tft.drawNumber(rpm, tlx + w/2, tly);
}
