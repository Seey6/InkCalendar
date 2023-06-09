#ifndef DEVICE_H_
#define DEVICE_H_
#include "def.h"
#include "Arduino.h"
#include "user_interface.h"
ret device_get_battery(int &battery_precent);

ret device_save_time(signed long long timestamp);
ret device_get_time(signed long long *timestamp);

ret device_set_init_finish(uint32_t flag);
ret device_get_init_finish(uint32_t* flag);

ret device_set_tomato_past(uint32_t min);
ret device_get_tomato_past(uint32_t* min);
ret device_set_tomato_total(uint32_t min);
ret device_get_tomato_total(uint32_t* min);

#endif