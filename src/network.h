#ifndef NETWORK_H_
#define NETWORK_H_
#include "def.h"
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;


ret network_init(const char* ssid,const char* pswd = nullptr);
ret network_get_time(signed long long &timestamp);
ret network_get_weather(unsigned long city_code,weather_t *weather,int day);
ret network_get_hitokoto(char* buffer);
ret network_get_weather_now(char* pos,weather_now_t &weather_now);
ret network_get_poetry(char* content,char* title);
#endif