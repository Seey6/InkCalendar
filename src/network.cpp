#include "network.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ArduinoJson.h"

WiFiClient wifi_client;
HTTPClient http_client; 

StaticJsonDocument<4096> doc;

ret network_init(const char* ssid,const char* pswd){
    int count = 0;
    if(pswd!=nullptr){
        WiFi.begin(ssid,pswd);
    }else{
        WiFi.begin(ssid);
    }
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if(count++ > 30*2){
            return RET_TIMEOUT;
        }
    }
    debug_print("wifi connect\n");
    return RET_OK;
}

ret network_get_time(signed long long &timestamp){
    http_client.begin(wifi_client,"http://tptm.hd.mi.com/gettimestamp");
    if(HTTP_CODE_OK==http_client.GET()){
        timestamp = atoi(http_client.getString().c_str()+15);
    }
    http_client.end();
    debug_print("timestamp:%ld\n",timestamp);
    return timestamp!=0?RET_OK:RET_ERROR;
}

ret network_get_weather(unsigned long city_code,weather_t *weather,int day){
    http_client.begin(wifi_client,"http://api.seniverse.com/v3/weather/daily.json?key=S2tLpnZ0dIPlx8vCH&location=tianjin&language=zh-Hans&unit=c&start=0&days=5");
    int http_ret = http_client.GET();
    if(HTTP_CODE_OK==http_ret){
        DeserializationError error = deserializeJson(doc, http_client.getString());
        if (error){
            debug_print("weather api json error\n");
            return RET_ERROR;
        }
        for (size_t i = 0; i < (day>3?3:day); i++)
        {
            strlcpy(weather[i].weather_day,doc["results"][0]["daily"][0]["text_day"],sizeof(weather[i].weather_day));
            strlcpy(weather[i].weather_night,doc["results"][0]["daily"][0]["text_night"],sizeof(weather[i].weather_night));
            weather[i].weather_code_day = atoi(doc["results"][0]["daily"][0]["code_day"]);
            weather[i].weather_code_night = atoi(doc["results"][0]["daily"][0]["code_night"]);
            weather[i].temp_max = atoi(doc["results"][0]["daily"][0]["high"]);
            weather[i].temp_min = atoi(doc["results"][0]["daily"][0]["low"]);
            weather[i].wind_degree = atoi(doc["results"][0]["daily"][0]["wind_direction_degree"]);
            weather[i].wind_level = atoi(doc["results"][0]["daily"][0]["wind_scale"]);
            weather[i].humidity = atoi(doc["results"][0]["daily"][0]["humidity"]);
        }
        return RET_OK;
    }else{
        debug_print("weather api get error:%s\n",http_client.errorToString(http_ret).c_str());
        return RET_ERROR;
    }
}