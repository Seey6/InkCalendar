#ifndef NETWORK_H_
#define NETWORK_H_
#include "def.h"
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

typedef struct weather_t {
    char weather_day[20];//白天天气
    char weather_night[20];//夜晚天气

    uint8_t weather_code_day;//白天天气code
    uint8_t weather_code_night;//夜晚天气code

    int temp_max;//最高气温
    int temp_min;//最低气温

    int wind_degree;//风向
    int wind_level;//风级

    int humidity;//湿度
}weather_t;

ret network_init(const char* ssid,const char* pswd = nullptr);
ret network_get_time(signed long long &timestamp);
ret network_get_weather(unsigned long city_code,weather_t *weather,int day);
#endif