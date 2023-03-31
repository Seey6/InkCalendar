#ifndef DEF_H_
#define DEF_H_
#define DEBUG_FLAG

#ifdef DEBUG_FLAG

#define debug_print(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#define serial_init() Serial.begin(115200)

#else

#undef debug_print
#define debug_print(fmt, ...)
#undef serial_init
#define serial_init()

#endif

enum ret {
    RET_TIMEOUT = -2,
    RET_ERROR = -1,
    RET_OK = 0,
};

enum skycon{
    CLEAR_DAY=0,//晴（白天）|100
    CLEAR_NIGHT,//晴（夜间）|150
    PARTLY_CLOUDY_DAY,//多云（白天）|103
    PARTLY_CLOUDY_NIGHT,//多云（夜间）|153
    CLOUDY,//阴|101
    LIGHT_HAZE,//轻度雾霾|502
    MODERATE_HAZE,//中度雾霾|502
    HEAVY_HAZE,//重度雾霾|502
    LIGHT_RAIN,//小雨|305
    MODERATE_RAIN,//中雨|306
    HEAVY_RAIN,//大雨|307
    STORM_RAIN,//暴雨|308
    FOG,//雾|2003
    LIGHT_SNOW,//小雪|400
    MODERATE_SNOW,//中雪|401
    HEAVY_SNOW,//大雪|402
    STORM_SNOW,//暴雪|403
    DUST,//浮尘|504
    SAND,//沙尘|503
    WIND,//大风|1002
};

typedef struct weather_t {
    char weather_day[20];//白天天气
    char weather_night[20];//夜晚天气

    unsigned char weather_code_day;//白天天气code
    unsigned char weather_code_night;//夜晚天气code

    int temp_max;//最高气温
    int temp_min;//最低气温

    int wind_degree;//风向
    int wind_level;//风级

    int humidity;//湿度
}weather_t;
typedef struct weather_now_t {
    unsigned char weather;//天气
    signed char temp;//温度
    unsigned char humidity;//湿度
    unsigned char wind_speed;//风速
    unsigned short wind_degree;//风向
    int temp_apparent;//体表温度
    char air_quality[13];//空气质量
}weather_now_t;
#endif