#include "device.h"


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