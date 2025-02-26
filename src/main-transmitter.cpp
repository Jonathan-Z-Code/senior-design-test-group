#include "masterIncludes.h"
#include "espNowConfig.h"
#include "settings.h"

// stores x-axis value for joystick
uint16_t x_axis = 0;

// stores y-axis value for joystick
uint16_t y_axis = 0;

// boolean to track if data falls into deadzone condition
bool deadzoneCondition_X = false;
bool deadzoneCondition_Y = false;
bool deadzoneCondition = false;

// retrieve the MAC address of the ESP32 receiver
extern uint8_t broadcastAddress[];

// Create a struct_message called myData
transmit_message test_data;
data_message recv_data;

// Create a type to store peerInfo (the ESP32 receiver module)
extern esp_now_peer_info_t peerInfo;

// stores esp_now transmission result
esp_err_t result;


void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  
  memcpy(&recv_data, incomingData, sizeof(recv_data));
  
  // print important values
  Serial.print("TV ");
  Serial.print(recv_data.targetValue);
  Serial.print(" | LM ");
  Serial.print(recv_data.leftMotorValue);
  Serial.print(" | RM ");
  Serial.print(recv_data.rightMotorValue);
  Serial.print(" | DL ");
  Serial.print(recv_data.deltaLeft);
  Serial.print(" | DR ");
  Serial.print(recv_data.deltaRight);
  Serial.println();

}


void setup(void) {
    
  // init serial monitor
  Serial.begin(115200);

  // establish 9-bit ADC resolution
  analogReadResolution(NINE_BIT_RES);  
  
  // establish ADC functionality for GPIO pins
  adcAttachPin(36);
  adcAttachPin(39);

  // Set device as a Wi-Fi Station for ESP-NOW operation
  WiFi.mode(WIFI_STA);

  bool esp_now_config_result = espNowConfiguration();

  if(esp_now_config_result == SUCCESS) {
    Serial.println("ESP_NOW config successful!");
  }
  else {
    Serial.println("ESP_NOW config function failed");
  }
  
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

}


void calculateDeadzone(uint16_t x_value, uint16_t y_value) {

  // initialize deadzone conditions to be false
  deadzoneCondition = false;
  deadzoneCondition_X = false;
  deadzoneCondition_Y = false;

  if(x_value < DEADZONE_MAX_X && x_value > DEADZONE_MIN_X) {
    // controller deadzone condition X satisfied 
    deadzoneCondition_X = true;
  }
  
  if(y_value < DEADZONE_MAX_Y && y_value > DEADZONE_MIN_Y) {
    // controller deadzone condition Y satisfied 
    deadzoneCondition_Y = true;
  }

  // if both X and Y conditions are satisfied, then update global deadzone boolean variable
  if(deadzoneCondition_X == true && deadzoneCondition_Y == true) {
    deadzoneCondition = true;
  }

  return;

}


void loop(void) {

  // IMPORTANT NOTE:
  // esp32 dev changed the function "analogWrite" (no needed for PinMode for PWM)
  // 1kHz default freq

  // scaling the adc readings to 9-bit values
  x_axis = analogRead(39);
  y_axis = analogRead(36);

  calculateDeadzone(x_axis, y_axis);

  //Serial.print("x val = ");
  //Serial.print(x_axis);
  //Serial.print(" y val = ");
  //Serial.print(y_axis);

  // if deadzoneCondition is met, do not transfer data to receiver
  if(deadzoneCondition) {
    // possibly transmit a stop command to receiver
    //Serial.println("; nop");
    test_data.x_value = 0;
    test_data.y_value = 0; 
  }
  else {
    //Serial.println("; ok");
    test_data.x_value = x_axis;
    test_data.y_value = y_axis;  
  }

  // Send message via ESP-NOW
  result = esp_now_send(broadcastAddress, (uint8_t *) &test_data, sizeof(transmit_message));

  // think about graceful error handling
  if (result == ESP_OK) {
    // do nothing
  }
  else {
    Serial.println("OTA error");
  }

  delay(ESP_NOW_TX_DELAY);

}