#include <Arduino.h>
#include "LittleFS.h"

#include "def.h"
#include "gui.h"
#include "network.h"
#include "device.h"

#include "time.h"


void setup() {
  signed long long ts = 0;
  struct tm* time_struct = 0;
  struct weather_t weather;
  char buf[100];
  int battery = 0;

  serial_init();
  debug_print("init start\n");
  LittleFS.begin();
  gui_init(&LittleFS,OPM42);
  network_init("tjuwlan");

  gui_draw_ui();
  if(RET_OK == network_get_time(ts)){
    time_struct = gmtime((time_t*)&ts);
    gui_draw_calendar(80,16,time_struct->tm_mon,time_struct->tm_mday);
    gui_draw_time(4,0,time_struct->tm_hour+8,time_struct->tm_min);
  }
  if(RET_OK== device_get_battery(battery)){
    gui_draw_battery(4,152,battery);
  }
  if(RET_OK == network_get_hitokoto(buf)){
    gui_draw_hitokoto(299-20,0,buf);
  }
  
  // delay(5000);
  // if(RET_OK == network_get_weather(0,&weather,1)){
  //   sprintf(buf,"白天天气:%s 夜间天气:%s",weather.weather_day,weather.weather_night);
  //   gui_draw_text(78,0,buf);
  //   sprintf(buf,"最高气温:%dC 最低气温:%dC",weather.temp_max,weather.temp_min);
  //   gui_draw_text(104,0,buf);
  //   sprintf(buf,"风向:%d %d级",weather.wind_degree,weather.wind_level);
  //   gui_draw_text(136,0,buf);
  // }
  debug_print("setup finish\n");


}

void loop() {
  delay(5000);
  debug_print("HELLO!\n");
}