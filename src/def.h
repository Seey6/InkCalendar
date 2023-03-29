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
#endif