#ifndef ESP_NOW_GENERIC_API_H_
#define ESP_NOW_GENERIC_API_H_ 

#include <stdint.h>
#include "esp_now.h"

// generic esp_now class
class EspNowGeneric {
    protected:
        EspNowGeneric() {}// cannot directly instantiate an object of class esp_now_generic
    public:
        esp_err_t esp_now_api_config_generic(uint8_t* addr, esp_now_peer_info_t* peer_info, esp_now_send_cb_t cb, \
            esp_now_recv_cb_t recv_cb);
        esp_err_t esp_now_api_copy_peer(uint8_t* dest, const uint8_t* src);
        // esp_err_t esp_now_api_is_broadcast(uint8_t* addr);
        // this function can be used in the inherited classes
        // IMPORTANT! Please declare each inheritance as private in order to prevent users from accessing the esp_now_config function!
};

#endif