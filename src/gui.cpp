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
        "","January","February","March","April","May","June","July","August","September","October","November","December"
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
        EPD.DrawUTF(x,y+4,time);
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
char* gui_weather_get_direction_degree(uint16_t degree){
    if(degree>337||degree<=22){
        return "北";
    }else if(degree>22&&degree<=67){
        return "东北";
    }else if(degree>67&&degree<=112){
        return "东";
    }else if(degree>112&&degree<=157){
        return "东南";
    }else if(degree>157||degree<=202){
        return "南";
    }else if(degree>202||degree<=247){
        return "西南";
    }else if(degree>247||degree<=292){
        return "西";
    }else if(degree>292||degree<=337){
        return "西北";
    }else{
        return "北";
    }
}
int gui_weather_get_level_speed(int speed){
    if(speed <= 1)return 0;
    if(speed <= 5)return 1;
    if(speed <= 11)return 2;
    if(speed <= 19)return 3;
    if(speed <= 28)return 4;
    if(speed <= 38)return 5;
    if(speed <= 49)return 6;
    if(speed <= 61)return 7;
    if(speed <= 74)return 8;
    if(speed <= 88)return 9;
    if(speed <= 102)return 10;
    if(speed <= 117)return 11;
    if(speed <= 133)return 12;
    if(speed <= 149)return 13;
    if(speed <= 166)return 14;
    if(speed <= 182)return 15;
    if(speed <= 201)return 16;
    if(speed >= 202)return 17;
    return 0;
}
void gui_draw_weather_now(int x,int y,weather_now_t weather_now){
    char *buf;
    unsigned char icon_buf[2];
    char skycon_text[][13] = {"晴","晴","多云","多云","阴","轻度雾霾","中度雾霾","重度雾霾","小雨","中雨","大雨","暴雨","雾","小雪","中雪","大雪","暴雪","浮尘","沙尘","大风"};
    buf = (char*)malloc(32);
    EPD.EPD_init_Part();
    EPD.clearbuffer();
    EPD.fontscale=1;
    EPD.SetFont(FONT32);
    itoa(weather_now.temp,buf,10);
    if(weather_now.temp>=0&&weather_now.temp<=9){
        EPD.DrawUTF(x+2,y+4+16,buf);
    }else{
        EPD.DrawUTF(x+2,y+4,buf);
    }
    EPD.SetFont(FONT16);
    EPD.DrawUTF(x,y+4+36+4,"°C");
    EPD.DrawUTF(x+16,y+4+36+4,skycon_text[weather_now.weather]);
    EPD.SetFont(ICON32);
    icon_buf[0]=0;
    icon_buf[1]=weather_now.weather+'a';
    EPD.DrawUnicodeChar(x+2,y+136,32,32,(uint8_t*)icon_buf);

    EPD.SetFont(FONT16);
    
    sprintf(buf,"体感: %d °C",weather_now.temp_apparent);
    EPD.DrawUTF(x+40,y+4,buf);
    sprintf(buf,"湿度: %d",weather_now.humidity);
    EPD.DrawUTF(x+40,y+4+100,buf);
    if(strcmp("良",weather_now.air_quality)==0||strcmp("优",weather_now.air_quality)==0){
        sprintf(buf,"空气质量%s",weather_now.air_quality);
        EPD.DrawUTF(x+40+20,y+4+100-16,buf);
    }else{
        strlcpy(buf,weather_now.air_quality,sizeof(buf));
        EPD.DrawUTF(x+40+20,y+4+100,buf);
    }
    // if(gui_get_text_len(buf)>11){
    //     if(gui_weather_get_level_speed(weather_now.wind_speed)==0){
    //         EPD.DrawUTF(x+40+20+20,y+8,"无风");
    //     }else{
    //         sprintf(buf,"%s风%d级",gui_weather_get_direction_degree(weather_now.wind_degree),gui_weather_get_level_speed(weather_now.wind_speed));
    //         EPD.DrawUTF(x+40+20+20,y+8,buf);
    //     }
    // }else{
        if(gui_weather_get_level_speed(weather_now.wind_speed)==0){
            EPD.DrawUTF(x+40+20,y+4,"无风");
        }else{
            sprintf(buf,"%s风%d级",gui_weather_get_direction_degree(weather_now.wind_degree),gui_weather_get_level_speed(weather_now.wind_speed));
            EPD.DrawUTF(x+40+20,y+4,buf);
        }
    // }

    EPD.EPD_Dis_Part(x,x+79,y,y+183,(uint8_t*)EPD.EPDbuffer,1);
    EPD.deepsleep();
    free(buf);

}

void gui_draw_weather_day(int x,int y,weather_t* weather){
    unsigned char icon_buf[2];
    char buf[40];
    signed char min=0,max=0,i=0;


    uint8_t skycon = 0;
    uint8_t day = 0;

    for (int i = 0; i < 3; i++)
    {
        if(weather[i].weather_day>=8 && weather[i].weather_day<=11){
            day = (day|(1<<i));
            skycon = (skycon|(1<<0));//display
            skycon = (skycon|(1<<1));//rain
        }
        if(weather[i].weather_day>=13 && weather[i].weather_day<=16){
            day = (day|(1<<i));
            skycon = (skycon|(1<<0));//display
            skycon = (skycon|(1<<2));//snow
        }
        if(weather[i].weather_night>=8 && weather[i].weather_night<=11){
            day = (day|(1<<i));
            skycon = (skycon|(1<<0));//display
            skycon = (skycon|(1<<1));//rain
        }
        if(weather[i].weather_night>=13 && weather[i].weather_night<=16){
            day = (day|(1<<i));
            skycon = (skycon|(1<<0));//display
            skycon = (skycon|(1<<2));//snow
        }
    }
    
    EPD.EPD_init_Part();
    EPD.clearbuffer();
    EPD.SetFont(FONT16);
    EPD.fontscale=1;

    if(skycon&0x1){
        sprintf(buf,"%s%s%s天有%s%s,记得带伞",(day>>0)&0x1?"今":"",(day>>1)&0x1?"明":"",(day>>2)&0x1?"后":"",(skycon>>1)&0x1?"雨":"",(skycon>>2)&0x1?"雪":"");
        EPD.DrawUTF(x,y+4,buf);
    }

    EPD.DrawUTF(x+24,4+y,"今天");
    EPD.DrawUTF(x+24,4+y+64,"明天");
    EPD.DrawUTF(x+24,4+y+128,"后天");

    EPD.SetFont(ICON32);
    icon_buf[0]=0;
    for (i = 0; i < 3; i++)
    {
        icon_buf[1]=weather[i].weather_day+'a';
        EPD.DrawUnicodeChar(x+24+24,4+y+i*64,32,32,icon_buf);
    }
    EPD.SetFont(FONT16);
    for (i = 0; i < 3; i++)
    {
        sprintf(buf,"%d°C",weather[i].temp_max);
        EPD.DrawUTF(x+24+4+32+24,4+y+i*64,buf);
        sprintf(buf,"%d°C",weather[i].temp_min);
        EPD.DrawUTF(x+24+24+8+32+48+4+20,4+y+i*64,buf);
    }
    max = weather[0].temp_max;
    min = weather[0].temp_min;
    for (i = 0; i < 3; i++)
    {
        if(weather[i].temp_min>max)max = weather[i].temp_min;
        if(weather[i].temp_max>max)max = weather[i].temp_max;
        if(weather[i].temp_min<min)min = weather[i].temp_min;
        if(weather[i].temp_max<min)min = weather[i].temp_max;
    }
    debug_print("min:%d,max:%d\n",min,max);
    for (i = 0; i < 2; i++)
    {
        EPD.DrawLine(24+24+8+32+20+x+(max-weather[i].temp_max)*48/(max-min),4+16+y+i*64,24+24+8+32+20+x+(max-weather[i+1].temp_max)*48/(max-min),64+4+16+y+i*64);
    }
    for (i = 0; i < 2; i++)
    {
        debug_print("dis1:%d,dis2:%d\n",(max-weather[i].temp_min),(max-weather[i+1].temp_min));
        EPD.DrawLine(24+24+8+32+20+x+(max-weather[i].temp_min)*48/(max-min),4+16+y+i*64,24+24+8+32+20+x+(max-weather[i+1].temp_min)*48/(max-min),64+4+16+y+i*64);
    }
    
    

    EPD.EPD_Dis_Part(x,x+159+16,y,y+183,(uint8_t*)EPD.EPDbuffer,1);
    EPD.deepsleep();


}



void gui_draw_tomato_menu(int x,int y,int choice){
    EPD.EPD_init_Part();
    EPD.clearbuffer();
    EPD.SetFont(FONT32);
    EPD.fontscale=1;
    EPD.DrawUTF(x,y+16,"25 Min");
    EPD.DrawUTF(x+36,y+16,"30 Min");
    EPD.DrawUTF(x+72,y+16,"45 Min");
    EPD.DrawUTF(x+108,y+16,"60 Min");
    EPD.DrawCircle(x+16+36*choice,y+8,4,1);
    EPD.EPD_Dis_Part(x,x+143,y,y+199,(uint8_t*)EPD.EPDbuffer,1);
    EPD.deepsleep();
}

// void gui_draw_ring(int x,int y,int r,int thick,int degree){
//     int tan[361]={0};
//     int dis_2 = 0;
//     int r_20 = (r-thick)*(r-thick);
//     int r_21 = (r+thick)*(r+thick);
//     for (size_t i = x-r; i <= x+r; i++)
//     {
//         for (size_t j = y-r; j <= y+r; j++)
//         {
//             dis_2 = (i-x)*(i-x)+(j-y)*(j-y);
//             if(dis_2 >= r_20 && dis_2 <= r_21){
//                 if(degree>=0 && degree < 90 && (x<0 || y<0 || (j-y)*100/(i-x)>tan[degree])){
//                     //draw
                    
                    
//                 }
//                 if(degree>=90 && degree <180 && (y<0 || (j-y)*100/(i-x)<tan[degree])){

//                 }
//                 if(degree>=180 && degree <270 && ((y<0&&x>0)||(j-y)*100/(i-x)<tan[degree])){

//                 }
//                 if(degree>=270 && degree <360 && ((j-y)*100/(i-x)>tan[degree])){

//                 }
//             }
//         }
        
//     }
    
// }

void gui_draw_tomato_clock(int x,int y,int time,int past){
    char buf[] ={'0','0',0};
    EPD.EPD_init_Part();
    EPD.clearbuffer();
    EPD.SetFont(FONT70);
    EPD.fontscale=1;

    // EPD.DrawBox()  
    
    if(time-past<10){
        itoa(time-past,buf+1,10);
    }else{
        itoa(time-past,buf,10);
    }
    EPD.DrawUTF(x+0,3+y+60,buf);
    EPD.DrawEmptyBox(x+100,3+y,250,12);
    EPD.DrawBox(x+100,3+y,(past)*250/time,12);

    EPD.EPD_Dis_Part(x,x+119,y,y+255,(uint8_t*)EPD.EPDbuffer,1);
    EPD.deepsleep();
}