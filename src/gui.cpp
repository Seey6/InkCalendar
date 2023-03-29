#include "gui.h"

EPaperDrive EPD(0, EPD_CS, EPD_RST, EPD_DC, EPD_BUSY, EPD_CLK, EPD_DIN);


int gui_get_text_len(char* p){
    int i = 0,count = 0;
    while(p[i]){
        if(p[i]<=0x7f){
            count++;
            i++;
        }else if((p[i]&0xe0)==0xc0){
            count+=2;
            i+=2;
        }else if((p[i]&0xf0)==0xe0){
            count+=2;
            i+=3;
        }
    }
    return count;
}
void gui_init(fs::FS* fs,epd_type mode){
    EPD.SetFS(fs);
    EPD.EPD_Set_Model(mode);
}

void gui_demo_full(){
    EPD.EPD_init_Full();
    EPD.clearbuffer();
    EPD.fontscale = 2;
    EPD.SetFont(FONT12);
    EPD.DrawUTF(0, 0, "初始化成功");
    EPD.DrawUTF(26, 0, "Hello World");
    EPD.fontscale = 1;
    debug_print("缓存图像绘制完毕，准备全刷 \n");
    EPD.EPD_Dis_Full((uint8_t *)EPD.EPDbuffer, 1);
    EPD.deepsleep();
}

void gui_demo_part(){
    EPD.EPD_init_Part(); //局刷的初始化
    EPD.clearbuffer();
    EPD.fontscale = 2;
    EPD.SetFont(FONT12);
    EPD.DrawUTF(0, 0, "现在是局刷");
    debug_print("开始局刷 \n");
    EPD.EPD_Dis_Part(0, 23, 0, 199, (uint8_t *)EPD.EPDbuffer, 1);
    debug_print("局刷结束 \n");
    EPD.deepsleep();
}
void gui_draw_text(int x,int y,char* text){
    int x_end=0,y_end=0;

    EPD.EPD_init_Part();
    EPD.clearbuffer();
    EPD.fontscale = 2;
    EPD.SetFont(FONT12);
    EPD.DrawUTF(x,y,text);
    x_end = x+((EPD.fontscale*EPD.fontheight-1)/8+1)*8-1;
    y_end = y+((EPD.fontscale*EPD.fontwidth*gui_get_text_len(text)/2-1)/8+1)*8-1;
    if(y_end>=400){
        y_end=(400-y)/8*8+y-1;
    }
    if(x_end>=300){
        x_end=(300-x)/8*8+x-1;
    }
    EPD.EPD_Dis_Part(x,x_end,y,y_end,(uint8_t*)EPD.EPDbuffer,1);
    EPD.deepsleep();
}
void gui_draw_text(int x,int y,long number){
    char* buffer = (char*)malloc(20);
    itoa(number,buffer,10);
    gui_draw_text(x,y,buffer);
    free(buffer);
}