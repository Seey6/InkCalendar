#ifndef GUI_H_
#define GUI_H_
#include "EPaperDrive.h"
#include "def.h"

#define EPD_CS 15
#define EPD_RST 2
#define EPD_DC 0
#define EPD_BUSY 4
#define EPD_CLK 14
#define EPD_DIN 13

extern EPaperDrive EPD;

void gui_init(fs::FS* fs,epd_type mode);
void gui_demo_full();
void gui_demo_part();
void gui_draw_text(int x,int y,long number);
void gui_draw_text(int x,int y,char* text,FONT font = FONT16);
void gui_draw_calendar(int x,int y,int month,int day);
void gui_draw_ui();
void gui_draw_time(int x,int y,int hour,int min);
void gui_draw_battery(int x,int y,int battery);
void gui_draw_hitokoto(int x,int y,char* text);
void gui_draw_weather_now(int x,int y,weather_now_t weather_now);

#endif