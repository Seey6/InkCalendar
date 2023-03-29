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
void gui_draw_text(int x,int y,char* text);

#endif