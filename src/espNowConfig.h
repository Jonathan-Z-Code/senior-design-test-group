#ifndef ESPNOWCONFIG_H_
#define ESPNOWCONFIG_H_

#include "masterIncludes.h"

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

bool espNowConfiguration(void);


// This data packet structure must be the EXACT same in the receiever and transmitter code!!
typedef struct transmit_message {
    uint16_t x_value;
    uint16_t y_value;
} transmit_message;


// data packet will be sent from the car to the controller
typedef struct data_message {
    int targetValue;
    int rightMotorValue;
    int leftMotorValue;
    int deltaLeft;
    int deltaRight;
} data_message;


#endif 