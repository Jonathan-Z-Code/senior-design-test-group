#include "masterIncludes.h"  
#include "espNowConfig.h"

// Create a type to store peerInfo (the ESP32 receiver module)
esp_now_peer_info_t peerInfo;


// This is the MAC address of the ESP32 receiver module
uint8_t broadcastAddress[] = {0xA0, 0xA3, 0xB3, 0x96, 0x78, 0x28};
uint8_t controllerAddress[] = {0xA0, 0xA3, 0xB3, 0x96, 0x6E, 0xF8};

// This is the callback function that is invoked when ESP32 receives an ACK or timeout error
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAILURE");
}


// standalone function that configures the ESP-NOW peripheral on the ESP32 unit
bool espNowConfiguration(void) {  
  
  // Init ESP-NOW peripheral
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return false;
  }
  else {
    Serial.println("Success init ESP-NOW");
  }

  // Once ESPNow is successfully initialized, we will register the callback function to
  // get the status of transmitted packet
  esp_err_t cb_result = esp_now_register_send_cb(OnDataSent);
  if(cb_result == ESP_OK) {
    Serial.println("Success registering cb function");
  }
  else {
    Serial.println("Couldn't register cb function");
    return false;
  }
  
  // Register peer into peerInfo struct
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Attempt to add peer to ESP_NOW memory
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return false;
  }

  // if it passes all tests, function should return true
  return true;

}