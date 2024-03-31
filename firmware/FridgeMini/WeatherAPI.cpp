
#include "WeatherAPI.h"

int WeatherAPI::make_query_now(StaticJsonDocument<1024>& doc)
{
    if (wNow == emptyString)
        return 4;

    log_i("request url = %s", wNow.c_str());

    WiFiClientSecure sslClient;
    sslClient.setCACert(ca);
    
    HTTPClient https;
    https.addHeader("Accept-Encoding", "gzip");
    
    if (https.begin(sslClient, wNow))
    {
        int httpCode = https.GET();
        // httpCode will be negative on error
        if (httpCode > 0 && (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY))
        {
            continuous_failed_count = 0;

            int contentLength = https.getSize();
            uint8_t* zip_buffer = (uint8_t*)malloc(sizeof(uint8_t) * (contentLength > 0 ? contentLength : Max_Unspecify_ContentLength));
            int readLength = 0;
            log_i("contentLength = %d", contentLength);
            while (https.connected() && (contentLength > 0 || contentLength == -1))
            {
                int canread = sslClient.available();
                if (canread)
                {
                    if (contentLength == -1 && readLength + canread > Max_Unspecify_ContentLength)
                    {
                        log_e("contentLength unspecified, read %d, canRead %d, exceeded %s, terminate", readLength, canread, Max_Unspecify_ContentLength);
                        https.end();
                        sslClient.stop();
                        free(zip_buffer);
                        return 3;
                    }
                    size_t read = sslClient.readBytes(zip_buffer + sizeof(uint8_t) * readLength, canread);
                    readLength += read;
                    if (contentLength > 0)
                        contentLength -= read;
                    log_d("canRead %d, read %d, left %d", canread, read, contentLength);
                }
                delay(1);
            }
            https.end();
            sslClient.stop();

            uint8_t* unzip_buffer = NULL;
            size_t unzip_length = 0;
            //log_d("FreeHeap %d", ESP.getFreeHeap());
            ArduinoUZlib::decompress(zip_buffer, readLength, unzip_buffer, unzip_length);
            log_i("decompressed Length = %d", unzip_length);
            free(zip_buffer);
            log_i("decompressed content = %s", unzip_buffer);
            DeserializationError error = deserializeJson(doc, unzip_buffer, unzip_length);
            if (unzip_buffer)
                free(unzip_buffer);
            //log_d("FreeHeap %d", ESP.getFreeHeap());
            if (error)
            {
                log_e("deserializeJson() failed: %s", error.c_str());
                return 2;
            }
            else
                return 0;
        }
        else
        {
            continuous_failed_count++;
            log_w("response httpCode = %d, continuous failed %d", httpCode, continuous_failed_count);

            https.end();
            sslClient.stop();
            return 1;
        }
    }
    else
    {
        log_w("https.begin failed");
        https.end();
        sslClient.stop();
        return -1;
    }
}

float WeatherAPI::jsonget_now_temp(StaticJsonDocument<1024>& doc)
{
    if (&doc && !doc.isNull())
    {
        const char* code = doc["code"]; // "200"
        if (atoi(code) == 200)
        {
            JsonObject now = doc["now"];
            if (!now.isNull())
            {
                const char* now_temp = now["temp"];
                if (now_temp)
                {
                    return strtof(now_temp, NULL);
                }
            }
            return TEMPSENSOR_BADDATA;
        }
        return TEMPSENSOR_ERR;
    }
    return TEMPSENSOR_DISCONNECT;
}

String WeatherAPI::jsonget_now_weather(StaticJsonDocument<1024>& doc)
{
    if (&doc && !doc.isNull())
    {
        const char* code = doc["code"]; // "200"
        if (atoi(code) == 200)
        {
            JsonObject now = doc["now"];
            if (!now.isNull())
            {
                const char* now_text = now["text"];
                if (now_text)
                {
                    return String(now_text);
                }
            }
        }
    }
    return String("");
}

int64_t WeatherAPI::NextUpdateDelaySec()
{
    if (wNow == emptyString)
        return INT_FAST64_MAX;
    else
        return (next_request_time - esp_timer_get_time()) / 1000000;
}

int WeatherAPI::GetNowWeather(String& weather, float& temp)
{
    if (wNow == emptyString)
    {
        weather = emptyString;
        temp = TEMPSENSOR_DISCONNECT;
        return 20;
    }

    // set cache data to it
    weather = now_weather;
    temp = now_temp;

    int64_t nowns = esp_timer_get_time();
    if (nowns >= next_request_time)
    {
        StaticJsonDocument<1024> wd0;
        int ret = make_query_now(wd0);
        if (ret == 0)
        {
            now_temp = jsonget_now_temp(wd0);
            now_weather = jsonget_now_weather(wd0);
            weather = now_weather;
            temp = now_temp;

            next_request_time = nowns + interval;
            return 0;
        }
        else if (ret > 0)
        {
            // code error, request made
            next_request_time = nowns + pow10(continuous_failed_count) * 1000000;
            return -10;
        }
        else
        {
            // internal error, not request yet
            return ret;
        }
    }
    else
    {
        // interval limit
        return 10;
    }
}
