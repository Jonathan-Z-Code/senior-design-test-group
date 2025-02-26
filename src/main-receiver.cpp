#include "masterIncludes.h"
#include "espNowConfig.h"
#include "settings.h"

// stores x-axis value for joystick
uint16_t x_axis = 0;

// stores y-axis value for joystick
uint16_t y_axis = 0;

// retrieve the MAC address of the ESP32 receiver
extern uint8_t controllerAddress[];

transmit_message recvData; 
data_message outgoingData;

// Create a type to store peerInfo (the ESP32 receiver module)
extern esp_now_peer_info_t peerInfo;

// boolean to determine when to update target based on joystick ADC reading
volatile bool updateTarget = false;

// timer 0 configuration variable
hw_timer_t* Timer0_cfg = NULL;

// IMPORTANT NOTE:
// esp32 dev changed the function "analogWrite" (no needed for PinMode for PWM)
// 1kHz default freq
// callback function that will be executed when data is received

// pin names for the encoder readings
#define LEFT_ENCODER (36)
#define RIGHT_ENCODER (3)


// test variables for encoder readings
int prevLeftCount = 0;
int prevRightCount = 0;
volatile int leftCount = 0;
volatile int rightCount = 0;
volatile int targetValue = 0;
// end of test variables for encoder readings

// counting rising edges from the encoder output
void leftEncoderUpdate(void) {
  leftCount++;
}

// counting rising edges from the encoder output
void rightEncoderUpdate(void) {
  rightCount++;
}

// I'm currently not using this timer peripheral, maybe we can 
// use this as a watchdog timer???
void Timer0_ISR(void) {
  // timer automatically resets auto-reload value into timer0 register
}


// esp_now callback function when as esp_now packet is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  // copy contents of incoming packet to the transmit_message
  memcpy(&recvData, incomingData, sizeof(transmit_message));

  // debug print out received values
  //Serial.println("IMCOMING PACKET:");
  //Serial.print("x val: ");
  //Serial.println(IMU_data.x_value);
  //Serial.print("y val: ");
  //Serial.println(IMU_data.y_value);
  //Serial.println(); // end with newline to seperate different incoming packets

  // limit the target value between 0-28
  targetValue = (recvData.y_value >> 4) - 4;
  if(targetValue < 0) {
    targetValue = 0;
  }

  updateTarget = true;

}


void setup() {

  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
    
  // Register peer into peerInfo struct
  memcpy(peerInfo.peer_addr, controllerAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Attempt to add peer to ESP_NOW memory
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));


  // test code for encoders
  attachInterrupt(LEFT_ENCODER, leftEncoderUpdate, RISING);
  attachInterrupt(RIGHT_ENCODER, rightEncoderUpdate, RISING);
  
  Timer0_cfg = timerBegin(0, 160, true); // count up (500kHz timer)
  timerAttachInterrupt(Timer0_cfg, &Timer0_ISR, true);
  timerAlarmWrite(Timer0_cfg, OVERFLOW_VALUE, true); // 250ms overflow condition
  timerAlarmEnable(Timer0_cfg); // enable overflow alarm
  // end of test code for encoders

}
 
#define UPDATE_INTERVAL (30) // in milliseconds
#define ENCODER_DIFFERENCE_THRESHOLD (1)
#define PWM_HIGH_SPEED_COMPENSATION (1)
#define PWM_STEP_VALUE (3) // assume 8-bit PWM
unsigned long currentTime = 0;
unsigned long previousTime = 0;
float kp = 1;

int rightMotorValue = 0;
int leftMotorValue = 0;
bool negativeFeedback = false;

#define LEFT_MOTOR_PWM (23)
#define RIGHT_MOTOR_PWM (22)


void loop() {
  
  currentTime = millis();

  // every UPDATE_INTERVAL milliseconds, run the motor algorithm 
  if(currentTime - previousTime >= UPDATE_INTERVAL) {
    previousTime = currentTime;
  
    int deltaLeft = leftCount - prevLeftCount;
    int deltaRight = rightCount - prevRightCount;

    if(deltaLeft < targetValue) {
			leftMotorValue += PWM_STEP_VALUE;
		}
		else if(deltaLeft > targetValue) {
			leftMotorValue -= PWM_STEP_VALUE;
      negativeFeedback = true;
		}

		if(deltaRight < targetValue) {
			rightMotorValue += PWM_STEP_VALUE;
		}
    else if(deltaRight > targetValue) {
			rightMotorValue -= PWM_STEP_VALUE;
      negativeFeedback = true;
		}

    if(deltaLeft >= 12 && targetValue > 12) {
      leftMotorValue += PWM_HIGH_SPEED_COMPENSATION;
    }

    int error = deltaLeft - deltaRight;
    int correctionFactor = fabs(kp * error);

    if(negativeFeedback == false) {
      // left wheel is faster
      if(error >= ENCODER_DIFFERENCE_THRESHOLD) {
        rightMotorValue += correctionFactor; 
      }
      // right wheel is faster
      if(-error >= ENCODER_DIFFERENCE_THRESHOLD) {
        leftMotorValue += correctionFactor;
      }
    }
    else {
      // left wheel is faster
      if(error >= ENCODER_DIFFERENCE_THRESHOLD) {
        leftMotorValue -= correctionFactor; 
        rightMotorValue += correctionFactor*0.5f;
      }
      // right wheel is faster
      if(-error >= ENCODER_DIFFERENCE_THRESHOLD) {
        rightMotorValue -= correctionFactor;
        leftMotorValue += correctionFactor*0.5f;
      }
    }

    // store previous count values for the next iteration
    prevLeftCount = leftCount;
    prevRightCount = rightCount;

    // limit PWM values
    if(rightMotorValue > 255) {
      rightMotorValue = 255;
    }
    if(leftMotorValue > 255) {
      leftMotorValue = 255;
    }
    if(rightMotorValue < 0) {
      rightMotorValue = 0;
    }
    if(leftMotorValue < 0) {
      leftMotorValue = 0;
    }

    // reset negativeFeedback flag
    negativeFeedback = false;    

    // check for edge case 
    if(deltaRight == 0 && deltaLeft == 0 && targetValue == 0) {
      leftMotorValue = 10;
      rightMotorValue = 10;
    }

    // verify correct pinout
    analogWrite(LEFT_MOTOR_PWM, leftMotorValue);
    analogWrite(RIGHT_MOTOR_PWM, rightMotorValue);
    
    //Serial.print("TV ");
    //Serial.print(targetValue);
    //Serial.print(" | LM ");
    //Serial.print(leftMotorValue);
    //Serial.print(" | RM ");
    //Serial.print(rightMotorValue);
    //Serial.print(" | DL ");
    //Serial.print(deltaLeft);
    //Serial.print(" | DR ");
    //Serial.print(deltaRight);
    //Serial.println();
    
    // store the car's data into the data_message struct
    outgoingData.deltaLeft = deltaLeft;
    outgoingData.deltaRight = deltaRight;
    outgoingData.targetValue = targetValue;
    outgoingData.leftMotorValue = leftMotorValue;
    outgoingData.rightMotorValue = rightMotorValue;

    // send the resulting data OTA (over-the-air) to the transmitter ESP32
    esp_err_t result = esp_now_send(controllerAddress, (uint8_t *) &outgoingData, sizeof(outgoingData));
    if(result == ESP_OK) {
      Serial.print("success! \n");
    }
  }

}

