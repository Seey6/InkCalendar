#include "network.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ArduinoJson.h"

WiFiClient wifi_client;
HTTPClient http_client; 

WiFiClientSecure wifi_client_secure;
HTTPClient https_client;

StaticJsonDocument<1024*4> doc;

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
    wifi_client_secure.setInsecure();
    wifi_client_secure.setBufferSizes(1024,1024);
    debug_print("wifi connect\n");
    return RET_OK;
}

ret network_get_time(signed long long &timestamp){
    http_client.begin(wifi_client,"http://tptm.hd.mi.com/gettimestamp");
    if(HTTP_CODE_OK==http_client.GET()){
        timestamp = atoi(http_client.getString().c_str()+15);
    }
    http_client.end();
    debug_print("timestamp:%lld\n",timestamp);
    return timestamp!=0?RET_OK:RET_ERROR;
}

skycon network_weather_skycon(char* skycon){
    if(!strcmp(skycon,"CLEAR_DAY"))return CLEAR_DAY;
    if(!strcmp(skycon,"CLEAR_NIGHT"))return CLEAR_NIGHT;
    if(!strcmp(skycon,"PARTLY_CLOUDY_DAY"))return PARTLY_CLOUDY_DAY;
    if(!strcmp(skycon,"PARTLY_CLOUDY_NIGHT"))return PARTLY_CLOUDY_NIGHT;
    if(!strcmp(skycon,"CLOUDY"))return CLOUDY;
    if(!strcmp(skycon,"LIGHT_HAZE"))return LIGHT_HAZE;
    if(!strcmp(skycon,"MODERATE_HAZE"))return MODERATE_HAZE;
    if(!strcmp(skycon,"HEAVY_HAZE"))return HEAVY_HAZE;
    if(!strcmp(skycon,"LIGHT_RAIN"))return LIGHT_RAIN;
    if(!strcmp(skycon,"MODERATE_RAIN"))return MODERATE_RAIN;
    if(!strcmp(skycon,"HEAVY_RAIN"))return HEAVY_RAIN;
    if(!strcmp(skycon,"STORM_RAIN"))return STORM_RAIN;
    if(!strcmp(skycon,"FOG"))return FOG;
    if(!strcmp(skycon,"LIGHT_SNOW"))return LIGHT_SNOW;
    if(!strcmp(skycon,"MODERATE_SNOW"))return MODERATE_SNOW;
    if(!strcmp(skycon,"HEAVY_SNOW"))return HEAVY_SNOW;
    if(!strcmp(skycon,"STORM_SNOW"))return STORM_SNOW;
    if(!strcmp(skycon,"DUST"))return DUST;
    if(!strcmp(skycon,"SAND"))return SAND;
    if(!strcmp(skycon,"WIND"))return WIND;
    return CLEAR_DAY;
}
ret network_get_weather(unsigned long city_code,weather_t *weather,int day){
    char buf[32];
    skycon skycon_map[100]={
        CLEAR_DAY,CLEAR_NIGHT,CLEAR_DAY,CLEAR_NIGHT,CLOUDY,PARTLY_CLOUDY_DAY,PARTLY_CLOUDY_NIGHT,PARTLY_CLOUDY_DAY,PARTLY_CLOUDY_NIGHT,CLOUDY,//0-9
        LIGHT_RAIN,LIGHT_RAIN,MODERATE_RAIN,LIGHT_RAIN,MODERATE_RAIN,HEAVY_RAIN,STORM_RAIN,STORM_RAIN,STORM_RAIN,LIGHT_SNOW,//10-19
        LIGHT_RAIN,LIGHT_RAIN,LIGHT_RAIN,MODERATE_RAIN,HEAVY_RAIN,STORM_RAIN,DUST,SAND,DUST,SAND,//20-29
        FOG,LIGHT_HAZE,WIND,WIND,WIND,WIND,WIND,LIGHT_SNOW,CLEAR_DAY//30-38
    };
    skycon_map[99]=CLEAR_DAY;
    http_client.begin(wifi_client,"http://api.seniverse.com/v3/weather/daily.json?key=S2tLpnZ0dIPlx8vCH&location=tianjin&language=zh-Hans&unit=c&start=0&days=5");
    int http_ret = http_client.GET();
    if(HTTP_CODE_OK==http_ret){
        DeserializationError error = deserializeJson(doc, http_client.getString());
        if (error){
            debug_print("weather api json error\n");
            return RET_ERROR;
        }
        for (int i = 0; i < (day>3?3:day); i++)
        {
            strlcpy(buf,doc["results"][0]["daily"][i]["code_day"],sizeof(buf));
            weather[i].weather_day=skycon_map[atoi(buf)];
            strlcpy(buf,doc["results"][0]["daily"][i]["code_night"],sizeof(buf));
            weather[i].weather_night=skycon_map[atoi(buf)];
            strlcpy(buf,doc["results"][0]["daily"][i]["high"],sizeof(buf));
            weather[i].temp_max=atoi(buf);
            strlcpy(buf,doc["results"][0]["daily"][i]["low"],sizeof(buf));
            weather[i].temp_min=atoi(buf);
            debug_print("weather api day%d:code_day:%d,high_temp:%d,low_temp:%d\n",i,weather[i].weather_day,weather[i].temp_max,weather[i].temp_min);
        }
        http_client.end();
        return RET_OK;
    }else{
        debug_print("weather api get error:%s\n",http_client.errorToString(http_ret).c_str());
        http_client.end();
        return RET_ERROR;
    }
}

ret network_get_hitokoto(char* buffer){
    https_client.begin(wifi_client_secure,"https://v1.hitokoto.cn/?c=d&c=f&c=j&encode=text&max_length=24");
    int http_ret = https_client.GET();
    if(HTTP_CODE_OK==http_ret){
        strcpy(buffer,https_client.getString().c_str());
        debug_print("hitokoto:%s\n",buffer);
        https_client.end();
        return RET_OK;
    }else{
        debug_print("hitokoto api error:%s\n",https_client.errorToString(http_ret).c_str());
        https_client.end();
        return RET_ERROR;
    }
}
ret network_get_weather_now(char* pos,weather_now_t& weather_now){
    char buf[32];
    http_client.begin(wifi_client,"http://api.caiyunapp.com/v2.6/TAkhjf8d1nlSlspN/117.1735,39.1107/realtime");
    int ret_http = http_client.GET();
    if(HTTP_CODE_OK==ret_http){
        DeserializationError error = deserializeJson(doc, http_client.getString());
        if (error){
            debug_print("weather_now api json error\n");
            return RET_ERROR;
        }
        if(strcmp(doc["status"],"ok")!=0){
            debug_print("weather_now api code error\n");
            return RET_ERROR;
        }
        strlcpy(buf,doc["result"]["realtime"]["skycon"],sizeof(buf));
        weather_now.weather = network_weather_skycon(buf);
        weather_now.humidity = doc["result"]["realtime"]["humidity"].as<float>()*100;
        weather_now.temp = doc["result"]["realtime"]["temperature"].as<float>();
        weather_now.wind_degree = doc["result"]["realtime"]["wind"]["direction"].as<float>();
        weather_now.wind_speed = doc["result"]["realtime"]["wind"]["speed"].as<float>();
        weather_now.temp_apparent = doc["result"]["realtime"]["apparent_temperature"].as<float>();
        strlcpy(weather_now.air_quality,doc["result"]["realtime"]["air_quality"]["description"]["chn"],sizeof(weather_now.air_quality));
        debug_print("weather_now weather:%d,air_quality:%s,temp:%d,temp_apparent:%d,humidity:%d,wind_speed:%d,wind_degree:%d\n",weather_now.weather,weather_now.air_quality,weather_now.temp,weather_now.temp_apparent,weather_now.humidity,weather_now.wind_speed,weather_now.wind_degree);
        http_client.end();
        return RET_OK;
    }else{
        debug_print("weather_now api error:%s\n",https_client.errorToString(ret_http).c_str());
        http_client.end();
        return RET_ERROR;
    }
}