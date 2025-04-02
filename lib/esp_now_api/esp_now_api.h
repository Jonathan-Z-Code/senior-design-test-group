#ifndef ESP_NOW_API_H_
#define ESP_NOW_API_H_ 

#include <stdint.h>
#include "esp_now_generic_enums.h"
#include "esp_now_generic_api.h"

// esp_now receiver class
class EspNowRecv: private EspNowGeneric {

    public:

    EspNowRecv() : EspNowGeneric() {}

    // struct to store motor values and specs
    typedef struct recv_message {
        int targetValue;
        int rightMotorValue;
        int leftMotorValue;
        int deltaLeft;
        int deltaRight;
    } recv_message;
    
    // init esp_now protocol and register callback functions and peer mac address
    esp_err_t init(const uint8_t* addr, esp_now_send_cb_t sent_cb, esp_now_recv_cb_t recv_cb) {
        esp_now_api_copy_peer(_peer_address, addr);
        return esp_now_api_config_generic(_peer_address, &_peer_info, sent_cb, recv_cb); 
    }

    // send the data OTA via esp_now (data sent to peer address)
    esp_err_t send() {
        return esp_now_send(_peer_address, (uint8_t *)debug_data, sizeof(debug_data));
    }

    // public pointer to a recv_message struct
    recv_message* debug_data = &_recv_data;

    private:
    
    recv_message _recv_data = {0};    
    uint8_t _peer_address[MAC_ADDR_LEN] = {0};
    esp_now_peer_info_t _peer_info;

};


// esp_now controller class
class EspNowController : private EspNowGeneric {

    public:
    
    // REMINDER: If you inherit an abstract class, you have to reference the constructor 
    EspNowController() : EspNowGeneric() {}

    // struct used to store user inputs
    typedef struct controller_message {
        uint16_t x_value;
        uint16_t y_value;
    } controller_message;

    // init esp_now protocol and register callback functions and peer mac address
    esp_err_t init(const uint8_t* addr, esp_now_send_cb_t sent_cb, esp_now_recv_cb_t recv_cb) {
        esp_now_api_copy_peer(_peer_address, addr);
        return esp_now_api_config_generic(_peer_address, &_peer_info, sent_cb, recv_cb); 
    }

    // send the data OTA via esp_now (data sent to peer address
    esp_err_t send() {
        return esp_now_send(_peer_address, (uint8_t *)joystick_data, sizeof(joystick_data));
    }

    // public pointer to a controller_message struct
    controller_message* joystick_data = &_controller_data;

    private:

    controller_message _controller_data = {0};
    uint8_t _peer_address[MAC_ADDR_LEN] = {0}; 
    esp_now_peer_info_t _peer_info;

};

#endif