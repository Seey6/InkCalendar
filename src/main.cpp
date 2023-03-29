#include <Arduino.h>
#include "LittleFS.h"

#include "def.h"
#include "gui.h"
#include "network.h"

#include "time.h"


void setup() {
  signed long long ts = 0;
  struct tm* time_struct = 0;
  char buf[100];

  serial_init();
  LittleFS.begin();
  gui_init(&LittleFS,OPM42);
  network_init("五虎上将","11135555");

  gui_demo_full();
  if(RET_OK == network_get_time(ts)){
    gui_draw_text(0,0,ts);
    time_struct = gmtime((time_t*)&ts);
    sprintf(buf,"%d %d %d",1900+time_struct->tm_year,1+time_struct->tm_mon,time_struct->tm_mday);
    gui_draw_text(26,0,buf);
  }


}

void loop() {
  delay(5000);
  debug_print("HELLO!\n");
}