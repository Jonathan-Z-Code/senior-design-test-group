#include "esp_now_generic_enums.h"
#include "esp_now_generic_api.h"

// espressif provided header file
#include "esp_now.h"
#include "esp_wifi.h"

// arduino provided header files
#include <Arduino.h>

// generic esp_now config function
esp_err_t EspNowGeneric::esp_now_api_config_generic(uint8_t* addr, esp_now_peer_info_t* peer_info, esp_now_send_cb_t sent_cb, \
    esp_now_recv_cb_t recv_cb) {

    // Set device as a Wi-Fi Station 
    // (esp_wifi.h library is slimmer than wifi.h library)
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_err_t wifi_result = esp_wifi_start();
    if(wifi_result == ESP_OK) {
        Serial.println("Wifi mode STA success!");
    }
    else {
        Serial.println("Error in Wifi init STA");
        return wifi_result;
    }
 
    // Init ESP-NOW peripheral
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return ESP_ERR_ESPNOW_INTERNAL;
    }
    else {
        Serial.println("Success init ESP-NOW");
    }

    // Once ESPNow is successfully initialized, we will register the callback function to
    // get the status of transmitted packet
    esp_err_t cb_result = esp_now_register_send_cb(sent_cb);
    if(cb_result == ESP_OK) {
        Serial.println("Success registering cb function");
    }
    else {
        Serial.println("Couldn't register cb function");
        return cb_result;
    }

    // register the recv callback function
    esp_err_t recv_cb_result = esp_now_register_recv_cb(esp_now_recv_cb_t(recv_cb));
    if(recv_cb_result == ESP_OK) {
        Serial.println("Success registering recv cb function");
    }
    else {
        Serial.println("Couldn't register recv cb function");
        return recv_cb_result;
    }

    // Register peer into peerInfo struct
    memcpy(peer_info->peer_addr, addr, 6);
    peer_info->channel = 0;  
    peer_info->encrypt = false;

    // Attempt to add peer to ESP_NOW memory
    esp_err_t peer_result = esp_now_add_peer(peer_info);
    if(peer_result == ESP_OK) {
        Serial.println("Success adding peer to esp_now registry!");
    }
    else {     
    Serial.println("Failed to add peer");
        return peer_result;
    }

    // if it passes all tests, function should return success
    Serial.println("------ PASSED ESP_NOW CONFIGURATION -----");
    return ESP_OK;
} 

// function to manually add the user-provided mac address to the respective class _peer_address
esp_err_t EspNowGeneric::esp_now_api_copy_peer(uint8_t* dest, const uint8_t* src) {
    memcpy(dest, src, 6*sizeof(uint8_t));
    return 0;
}