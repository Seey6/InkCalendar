#ifndef NETWORK_H_
#define NETWORK_H_
#include "def.h"
ret network_init(const char* ssid,const char* pswd = nullptr);
ret network_get_time(signed long long &timestamp);
#endif