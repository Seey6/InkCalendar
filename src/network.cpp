#include "network.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"

WiFiClient wifi_client;
HTTPClient http_client; 

ret network_init(const char* ssid,const char* pswd){
    int count = 0;
    if(pswd!=nullptr){
        WiFi.begin(ssid,pswd);
    }else{
        WiFi.begin(ssid);
    }
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if(count++ > 30*2){
            return RET_TIMEOUT;
        }
    }
    debug_print("wifi connect\n");
    return RET_OK;
}

ret network_get_time(signed long long &timestamp){
    http_client.begin(wifi_client,"http://tptm.hd.mi.com/gettimestamp");
    if(HTTP_CODE_OK==http_client.GET()){
        timestamp = atoi(http_client.getString().c_str()+15);
    }
    debug_print("timestamp:%ld\n",timestamp);
    return timestamp!=0?RET_OK:RET_ERROR;
}