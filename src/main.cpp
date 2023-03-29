#include <Arduino.h>
#include "LittleFS.h"

#include "def.h"
#include "gui.h"
#include "network.h"

#include "time.h"


void setup() {
  signed long long ts = 0;
  struct tm* time_struct = 0;
  struct weather_t weather;
  char buf[100];

  serial_init();
  debug_print("init start\n");
  LittleFS.begin();
  gui_init(&LittleFS,OPM42);
  network_init("tjuwlan");

  gui_demo_full();
  if(RET_OK == network_get_time(ts)){
    gui_draw_text(0,0,ts);
    time_struct = gmtime((time_t*)&ts);
    sprintf(buf,"%d %d %d",1900+time_struct->tm_year,1+time_struct->tm_mon,time_struct->tm_mday,8+time_struct->tm_hour,time_struct->tm_min,time_struct->tm_sec);
    gui_draw_text(26,0,buf);
  }
  delay(5000);
  if(RET_OK == network_get_weather(0,&weather,1)){
    sprintf(buf,"白天天气:%s 夜间天气:%s",weather.weather_day,weather.weather_night);
    gui_draw_text(52,0,buf);
    sprintf(buf,"最高气温:%dC 最低气温:%dC",weather.temp_max,weather.temp_min);
    gui_draw_text(78,0,buf);
    sprintf(buf,"风向:%d %d级",weather.wind_degree,weather.wind_level);
    gui_draw_text(104,0,buf);
  }
  debug_print("setup finish\n");


}

void loop() {
  delay(5000);
  debug_print("HELLO!\n");
}