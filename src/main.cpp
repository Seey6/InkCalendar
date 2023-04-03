#include <Arduino.h>
#include "LittleFS.h"

#include "def.h"
#include "gui.h"
#include "network.h"
#include "device.h"

#include "time.h"
uint32_t time_buf[4];

void mode_normal(){
  signed long long ts = 0;
  struct tm* time_struct = 0;
  struct weather_t weather[3];
  struct weather_now_t weather_now;
  char buf[100];
  int battery = 0;
  uint32_t init_flag = 0;
  int32_t sleep_time = 0;
  device_get_init_finish(&init_flag);
  if(init_flag!=1){
    gui_draw_ui();
    if(RET_OK != network_init("tjuwlan")){
      gui_draw_text(0,0,"网络初始化失败");
      ESP.deepSleep(0);
    }
    if(RET_OK == network_get_time(ts)){
      time_buf[0] = system_get_time();
      time_struct = gmtime((time_t*)&ts);
      gui_draw_calendar(88,16,time_struct->tm_mon,time_struct->tm_mday);
      gui_draw_time(8,0,time_struct->tm_hour,time_struct->tm_min);
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
      gui_draw_weather_day(92,216,weather);
    }
    if(RET_OK == device_get_battery(battery)){
      gui_draw_battery(8,152,battery);
    }
    device_set_init_finish(1);

    time_buf[1] = system_get_time();
    sleep_time = 60 - ts%60 - ((time_buf[1]-time_buf[0])/1e6);
    while(sleep_time<=0){
      sleep_time+=60;
    }
    ts+=((time_buf[1]-time_buf[0])/1e6);
    device_save_time(ts);
    debug_print("sleep time:%d\n",sleep_time);
    debug_print("timestamp before sleep:%lld\n",ts);
    ESP.deepSleep(sleep_time*1e6);
  }else{
    device_get_time(&ts);
    ts+=60;
    ts-=(ts%60);
    debug_print("timestamp after reboot:%lld\n",ts);
    time_struct = gmtime((time_t*)&ts);
    
    gui_draw_time(8,0,time_struct->tm_hour,time_struct->tm_min);
    if(time_struct->tm_hour==0 && time_struct->tm_min==0){
      gui_draw_calendar(88,16,time_struct->tm_mon,time_struct->tm_mday);
    }
    
    if(time_struct->tm_min==0 && time_struct->tm_hour >= 6){
      if(RET_OK != network_init("tjuwlan")){
        time_buf[1] = system_get_time();
        ts += ((time_buf[1]-time_buf[3])/1e6);
        sleep_time = 60e6 - (time_buf[1]-time_buf[3]);
        device_save_time(ts);
        ESP.deepSleep(sleep_time);
      }
    }
    if(time_struct->tm_hour==6&&time_struct->tm_min==0){
      gui_draw_ui();
      gui_draw_calendar(88,16,time_struct->tm_mon,time_struct->tm_mday);
      if(RET_OK == network_get_weather(0,weather,3)){
        gui_draw_weather_day(92,216,weather);
      }
      if(RET_OK == device_get_battery(battery)){
        gui_draw_battery(8,152,battery);
      }
    }
    if(time_struct->tm_min==0 && time_struct->tm_hour >= 6){
      if(RET_OK == network_get_hitokoto(buf)){
        gui_draw_hitokoto(299-20,0,buf);
      }
      if(RET_OK==network_get_weather_now(0,weather_now)){
        gui_draw_weather_now(8,216,weather_now);
      }
      if(RET_OK == network_get_time(ts)){
        time_buf[3] = system_get_time();
        time_struct = gmtime((time_t*)&ts);
        gui_draw_time(8,0,time_struct->tm_hour,time_struct->tm_min);
      }
      time_buf[0] = system_get_time();
      sleep_time = 60e6 - (ts%60)*1e6 - ((time_buf[0]-time_buf[3]));
      while(sleep_time<=0){
        sleep_time+=60e6;
      }
      ts+=((time_buf[0]-time_buf[3])/1e6);
    }else{
      time_buf[1] = system_get_time();
      ts += ((time_buf[1]-time_buf[3])/1e6);
      sleep_time = 60e6 - (time_buf[1]-time_buf[3]);
    }
    device_save_time(ts);
    ESP.deepSleep(sleep_time);
  }
}
void mode_tomato(){
  int choice = 0;
  int time_map[] = {25,30,45,60};
  // int time_map[] = {1,2,3,4};
  uint32_t has_init = 0,past=0,total=0;
  int32_t sleep_time = 0;

  debug_print("tomato clock mode\n");
  device_get_init_finish(&has_init);
  device_get_tomato_past(&past);
  device_get_tomato_total(&total);
  if(has_init!=2){
    EPD.EPD_init_Full();
    EPD.clearbuffer();
    EPD.EPD_Dis_Full(EPD.EPDbuffer,1);

    gui_draw_tomato_menu(80,144,choice);
    pinMode(0,INPUT);
    while (1)
    {
      if(digitalRead(5)==0){
        delay(5);
        if(digitalRead(5)==0){
          choice = ((choice+1)&0x3);
          pinMode(0,OUTPUT);
          digitalWrite(0,HIGH);
          gui_draw_tomato_menu(80,144,choice);
          pinMode(0,INPUT);
        }
      }
      
      if(digitalRead(0)==0){
        delay(5);
        if(digitalRead(0)==0){
          pinMode(0,OUTPUT);
          digitalWrite(0,HIGH);
          break;
        }
      }
      delay(30);
    }
    time_buf[3] = system_get_time();
    EPD.EPD_init_Full();
    EPD.clearbuffer();
    EPD.EPD_Dis_Full(EPD.EPDbuffer,1);
    gui_draw_tomato_clock(104,72,time_map[choice],0);

    device_set_tomato_past(0);
    device_set_init_finish(2);
    device_set_tomato_total(time_map[choice]);
    sleep_time = 60e6-(system_get_time()-time_buf[3]);
    while(sleep_time<0)sleep_time+=60e6;
    ESP.deepSleep(sleep_time);
  }else{
    past+=1;
    if(past>=total){
      EPD.EPD_init_Full();
      EPD.clearbuffer();
      EPD.fontscale=1;
      EPD.SetFont(FONT16);
      EPD.drawXbm(80,152,96,96,(uint8_t*)image_task_finish);
      EPD.DrawUTF(192,176,"已完成");
      EPD.EPD_Dis_Full(EPD.EPDbuffer,1);
      EPD.deepsleep();
      ESP.deepSleep(0);
    }
    gui_draw_tomato_clock(104,72,total,past);
    device_set_tomato_past(past);
    sleep_time = 60e6-(system_get_time()-time_buf[3]);
    while(sleep_time<0)sleep_time+=60e6;
    ESP.deepSleep(sleep_time);
  }
}

void setup() {
  time_buf[3] = system_get_time();
  time_buf[2] = time_buf[3];
  uint32_t mode = 0;
  serial_init();
  debug_print("init start\n");
  LittleFS.begin();
  gui_init(&LittleFS,OPM42);
  pinMode(5, INPUT_PULLUP);
  device_get_init_finish(&mode);
  if (digitalRead(5) == 0 || mode==2)
  {
    mode_tomato();
  }else{
    mode_normal();
  }
}

void loop() {
  delay(5000);
  debug_print("HELLO!\n");
}