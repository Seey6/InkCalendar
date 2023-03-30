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
    return count+1;
}
void gui_init(fs::FS* fs,epd_type mode){
    EPD.SetFS(fs);
    EPD.EPD_Set_Model(mode);
}

void gui_demo_full(){
    EPD.EPD_init_Full();
    EPD.clearbuffer();
    EPD.fontscale = 2;
    EPD.SetFont(FONT16);
    EPD.DrawUTF(0, 0, "初始化成功");
    EPD.DrawUTF(31, 0, "Hello World");
    EPD.fontscale = 1;
    debug_print("缓存图像绘制完毕，准备全刷 \n");
    EPD.EPD_Dis_Full((uint8_t *)EPD.EPDbuffer, 1);
    EPD.deepsleep();
}

void gui_demo_part(){
    EPD.EPD_init_Part(); //局刷的初始化
    EPD.clearbuffer();
    EPD.fontscale = 2;
    EPD.SetFont(FONT16);
    EPD.DrawUTF(0, 0, "现在是局刷");
    debug_print("开始局刷 \n");
    EPD.EPD_Dis_Part(0, 31, 0, 199, (uint8_t *)EPD.EPDbuffer, 1);
    debug_print("局刷结束 \n");
    EPD.deepsleep();
}
void gui_draw_text(int x,int y,char* text,FONT font){
    int x_end=0,y_end=0;

    EPD.EPD_init_Part();
    EPD.clearbuffer();
    EPD.fontscale = 1;
    EPD.SetFont(font);
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


void gui_draw_calendar(int x,int y,int month,int day){
    char day_buf[3]={'0'};
    char month_map[][10] = {
        "January","February","March","April","May","June","July","August","September","October","November","December"
    };
    uint8_t month_offset[12] = {
        24,16,32,40,48,40,48,24,0,16,8,8
    };
    if(day>0&&day<=31&&month>0&&month<=12){
        if(day<10){
            itoa(day,day_buf+1,10);
        }else{
            itoa(day,day_buf,10);
        }
        day_buf[2]=0;

        EPD.EPD_init_Part();
        EPD.clearbuffer();
        EPD.fontscale = 1;
        EPD.SetFont(FONT70);
        EPD.DrawUTF(x,y+24,day_buf);
        EPD.DrawLine(x+74,y+4,x+74,y+160);
        EPD.SetFont(FONT32);
        EPD.DrawUTF(x+78,y+month_offset[month+1],month_map[month+1]);
        EPD.EPD_Dis_Part(x,x+111,y,y+175,(uint8_t*)EPD.EPDbuffer,1);
        EPD.deepsleep();
    }    
}
void gui_draw_ui(){
    EPD.EPD_init_Full();
    EPD.clearbuffer();
    EPD.DrawLine(8,199,299-24-8,199);
    EPD.DrawLine(299-24,7,299-24,399-8);
    EPD.EPD_Dis_Full((uint8_t*)EPD.EPDbuffer,1);
    EPD.deepsleep();
}

void gui_draw_time(int x,int y,int hour,int min){
    char time[6] = {'0','0','0','0','0',0};
    int bat_icon_x_start=0,bat_icon_x_end=0,bat_icon_y_start=0,bat_icon_y_end=0;
    if(hour>=0&&hour<=23&&min>=0&&min<=59){
        if(hour<10){
            itoa(hour,time+1,10);
        }else{
            itoa(hour,time,10);
        }
        if(min<10){
            itoa(min,time+1+3,10);
        }else{
            itoa(min,time+3,10);
        }
        time[2]=':';
        time[5]=0;

        EPD.EPD_init_Part();
        EPD.clearbuffer();
        EPD.fontscale=1;
        EPD.SetFont(FONT24);
        EPD.DrawUTF(x,y,time);
        EPD.EPD_Dis_Part(x,x+23,y,y+79,(uint8_t*)EPD.EPDbuffer,1);
        EPD.deepsleep();
    }
}
void gui_draw_battery(int x,int y,int battery){
    int thick = 2;
    int height = 24;
    int width = 40-thick;
    
    EPD.EPD_init_Part();
    EPD.clearbuffer();
    EPD.DrawBox(x,y,width,thick);//上
    EPD.DrawBox(x+height-thick,y,width,thick);//下
    EPD.DrawBox(x,y,thick,height);//左
    EPD.DrawBox(x,y+width-thick,thick,height);//右
    EPD.DrawBox(x+height/2-thick,y+width,thick,thick*2);//电池头
    EPD.DrawBox(x+thick+1,y+thick+1,(width-thick*2)*battery/100-2,height-thick*2-2);
    EPD.EPD_Dis_Part(x,x+23,y,y+39,(uint8_t*)EPD.EPDbuffer,1);
    EPD.deepsleep();
}
void gui_draw_hitokoto(int x,int y,char* buffer){
    EPD.EPD_init_Part();
    EPD.clearbuffer();
    EPD.fontscale=1;
    EPD.SetFont(FONT16);
    int count = gui_get_text_len(buffer);
    
    EPD.DrawUTF(x,199-count*EPD.fontwidth/4,buffer);
    
    EPD.EPD_Dis_Part(x,x+16,0,399,(uint8_t*)EPD.EPDbuffer,1);
    EPD.deepsleep();
    // free(buf_multiline);
}