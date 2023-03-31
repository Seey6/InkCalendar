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
  struct weather_t weather[3];
  struct weather_now_t weather_now;
  char buf[100];
  int battery = 0;
  uint32_t init_flag = 0;
  int16_t sleep_time = 0;
  uint32_t time_buf[4];
  time_buf[3] = system_get_time();
  time_buf[2] = time_buf[3];
  serial_init();
  debug_print("init start\n");
  LittleFS.begin();
  gui_init(&LittleFS,OPM42);
  device_get_init_finish(&init_flag);
  if(init_flag!=1){
    gui_draw_ui();
    network_init("tjuwlan");
    if(RET_OK == network_get_time(ts)){
      time_buf[0] = system_get_time();
      time_struct = gmtime((time_t*)&ts);
      gui_draw_calendar(80,16,time_struct->tm_mon,time_struct->tm_mday);
      gui_draw_time(4,0,time_struct->tm_hour+8,time_struct->tm_min);
    }else{
      time_buf[0]=system_get_time();
    }
    if(RET_OK == network_get_hitokoto(buf)){
      gui_draw_hitokoto(299-20,0,buf);
    }
    if(RET_OK==network_get_weather_now(0,weather_now)){
      gui_draw_weather_now(8,216,weather_now);
    }
    if(RET_OK == network_get_weather(0,weather,3)){
      gui_draw_weather_day(112,216,weather);
    }
    if(RET_OK == device_get_battery(battery)){
      gui_draw_battery(4,152,battery);
    }
    device_set_init_finish();

    time_buf[1] = system_get_time();
    sleep_time = 60 - ts%60 - ((time_buf[1]-time_buf[0])/1e6);
    while(sleep_time<=0){
      sleep_time+=60;
    }
    ts+=((time_buf[1]-time_buf[0])/1e6);
    device_save_time(ts);
    debug_print("sleep time:%lld\n",sleep_time);
    debug_print("timestamp before sleep:%lld\n",ts);
    ESP.deepSleep(sleep_time*1e6);
  }else{
    device_get_time(&ts);
    ts+=60;
    ts-=(ts%60);
    debug_print("timestamp after reboot:%lld\n",ts);
    time_struct = gmtime((time_t*)&ts);
    gui_draw_time(4,0,time_struct->tm_hour+8,time_struct->tm_min);
    
    if(time_struct->tm_hour==0&&time_struct->tm_min==0){
      gui_draw_calendar(80,16,time_struct->tm_mon,time_struct->tm_mday);
      if(RET_OK == network_get_weather(0,weather,3)){
        gui_draw_weather_day(112,216,weather);
      }
      if(RET_OK == device_get_battery(battery)){
        gui_draw_battery(4,152,battery);
      }
    }
    if(time_struct->tm_min==0){
      network_init("tjuwlan");
      if(RET_OK == network_get_hitokoto(buf)){
        gui_draw_hitokoto(299-20,0,buf);
      }
      if(RET_OK==network_get_weather_now(0,weather_now)){
        gui_draw_weather_now(8,216,weather_now);
      }
      if(RET_OK == network_get_time(ts)){
        time_buf[3] = system_get_time();
        time_struct = gmtime((time_t*)&ts);
        gui_draw_time(4,0,time_struct->tm_hour+8,time_struct->tm_min);
      }
    }

    time_buf[1] = system_get_time();
    ts += ((time_buf[1]-time_buf[3])/1e6);
    device_save_time(ts);
    ESP.deepSleep(60e6-(time_buf[1]-time_buf[2]));
  }

}

void loop() {
  delay(5000);
  debug_print("HELLO!\n");
}