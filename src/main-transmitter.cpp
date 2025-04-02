/* 
    ---------------------------------------- ESPNOWTRANSMITTER FUNCTIONALITY ----------------------------------------------------------
    
    Pre-reqs: The only thing the user needs to provide are mac addresses and callback functions! No globals! Nice!
    
    What's the catch? Increased over-head from C++ inherited classes and abstract class. :(

    Why Bother? Previous codebase was a mess, time to harness the power of C++ and use encapsulation and abstraction via classes

    Author: Jonathan Zurita (jzuritastem@gmail.com)

    Any suggestions welcome!
    ----------------------------------------------------------------------------------------------------------------------------
*/

#include <Arduino.h>
#include "esp_now_api.h"

// user defined mac addresses
const uint8_t controller_address[MAC_ADDR_LEN] = {0xA0, 0xA3, 0xB3, 0x96, 0x6E, 0x40};
const uint8_t recv_address[MAC_ADDR_LEN] = {0xA0, 0xA3, 0xB3, 0x96, 0x6E, 0xF8};

// create an object from the EspNowRecv class
EspNowController controller;

// callback function for esp_now transmitted data
void data_sent_cb(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAILURE");
    return;
}

// callback function for esp_now data reception
void data_receive_cb(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    // store data into recv_message struct derived from EspNowRecv class 
    static EspNowRecv::recv_message debug_data = {0};

    // copy recv data and print out results
    memcpy(&debug_data, data, sizeof(debug_data));
    Serial.println("PACKET RECEIVED: ");
    Serial.println(debug_data.deltaLeft);
    return;
}

void setup(void) {
 
    Serial.begin(9600);

    // init the recv object with peer mac addr and callback functions
    controller.init(recv_address, data_sent_cb, data_receive_cb);
    
    // store updated joystick_data to the object's struct
    controller.joystick_data->x_value = 1;
    controller.joystick_data->y_value = 1;
 
    // send the joystick_data struct OTA to peer address stated in recv.init()
    controller.send();

}

void loop(void) {
    // do nothing
    Serial.println("INTO MAIN LOOP ...");
    delay(5000);
    controller.send();
}
