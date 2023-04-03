#include "device.h"

uint8_t after_init = 0;
uint64_t system_init_time = 0;

ret device_get_battery(int &battery_precent){
    int i = 0;
    int32_t buf=0;
    float bat;
    pinMode(12, OUTPUT);
    digitalWrite(12, 1);
    while (i++<3){
        buf += analogRead(A0);
        delay(1000);
    }
    bat = buf * 0.0009765625 * 5.607 / 3.0;
    bat = 497.50976 * bat * bat * bat * bat
        - 7442.07254 * bat * bat * bat
        + 41515.70648 * bat * bat
        - 102249.34377 * bat
        + 93770.99821;
    digitalWrite(12, 0);
    if(bat>100){
        battery_precent = 100;
    }else if(bat < 0){
        battery_precent = 3;
    }else{
        battery_precent = bat;
    }
    debug_print("battery:%f\n",bat);
    return RET_OK;
}


ret device_save_time(signed long long timestamp){
    return ESP.rtcUserMemoryWrite(1,(uint32*)&timestamp,sizeof(signed long long))?RET_OK:RET_ERROR;
}
ret device_get_time(signed long long *timestamp){
    if(ESP.rtcUserMemoryRead(1,(uint32_t*)timestamp,sizeof(signed long long))){
        return RET_OK;
    }else{
        *timestamp = 0;
        return RET_ERROR;
    }
}

ret device_set_init_finish(uint32_t flag){
    uint32_t buf = flag;
    return ESP.rtcUserMemoryWrite(0,&buf,4)?RET_OK:RET_ERROR;
}
ret device_get_init_finish(uint32_t *flag){
    return ESP.rtcUserMemoryRead(0,flag,4)?RET_OK:RET_ERROR;
}

ret device_set_tomato_past(uint32_t past){
    uint32_t buf = past;
    return ESP.rtcUserMemoryWrite(2,&buf,4)?RET_OK:RET_ERROR;
}
ret device_get_tomato_past(uint32_t *past){
    return ESP.rtcUserMemoryRead(2,past,4)?RET_OK:RET_ERROR;
}
ret device_set_tomato_total(uint32_t min){
    uint32_t buf = min;
    return ESP.rtcUserMemoryWrite(3,&buf,4)?RET_OK:RET_ERROR;
}
ret device_get_tomato_total(uint32_t *min){
    return ESP.rtcUserMemoryRead(3,min,4)?RET_OK:RET_ERROR;
}