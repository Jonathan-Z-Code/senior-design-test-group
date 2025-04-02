#include "control_api.h"
#include <Arduino.h>

MotorControl::motor_err_t MotorControl::init(uint8_t left_encoder_pin, uint8_t right_encoder_pin, \
    uint8_t left_motor_pin, uint8_t right_motor_pin) {
        
    // init ISR variables to zero before init IRQ handlers
    _left_count = _right_count = 0;
    
    // establish left and right wheel PWM pins
    _left_motor_pin = left_motor_pin;
    _right_motor_pin = right_motor_pin;

    // IRQ handlers for encoders
    attachInterrupt(left_encoder_pin, left_encoder_update, RISING);
    attachInterrupt(right_encoder_pin, right_encoder_update, RISING);
    
    // Init IRQ Handler for Timer0
    _timer0_cfg = timerBegin(0, 160, true); // count up (500kHz timer)
    timerAttachInterrupt(_timer0_cfg, &timer0_ISR, true);
    timerAlarmWrite(_timer0_cfg, OVERFLOW_VALUE, true); // 250ms overflow condition
    timerAlarmEnable(_timer0_cfg); // enable overflow alarm

    // timestamp init function
    _previous_time = millis();

    return SUCCESS;
}

MotorControl::motor_err_t MotorControl::update_speed() {
    // at the moment, default is 8-bit PWM resolution 
    analogWrite(_right_motor_pin, debug_config->right_wheel);
    analogWrite(_left_motor_pin, debug_config->left_wheel);

    return SUCCESS;
}

MotorControl::motor_err_t MotorControl::motor_algorithm() {
    
    // tweak these values
    const int PWM_STEP_VALUE = 3;
    const int PWM_HIGH_SPEED_COMPENSATION = 1;
    const int ENCODER_DIFFERENCE_THRESHOLD = 1;

    // variables for keeping track of previous counts
    static int prev_left_count = 0 ;
    static int prev_right_count = 0;
    static bool negative_feedback = false;

    // take current time
    unsigned long current_time = millis();
    
    // every UPDATE_INTERVAL milliseconds, run the motor algorithm 
    if(current_time - _previous_time >= UPDATE_INTERVAL) {
      _previous_time = current_time;
        
      // local variables to keep track of deltas
      int delta_left = _left_count - prev_left_count;
      int delta_right = _right_count - prev_right_count;
  
        if(delta_left < debug_config->target_value) {
            debug_config->left_wheel += PWM_STEP_VALUE;
        }
        else if(delta_left > debug_config->target_value) {
            debug_config->left_wheel -= PWM_STEP_VALUE;
            negative_feedback = true;
        }
  
        if(delta_right < debug_config->target_value) {
            debug_config->right_wheel += PWM_STEP_VALUE;
        }
        else if(delta_right > debug_config->target_value) {
            debug_config->right_wheel -= PWM_STEP_VALUE;
            negative_feedback = true;
        }
  
        if(delta_left >= 12 && debug_config->target_value > 12) {
            debug_config->left_wheel += PWM_HIGH_SPEED_COMPENSATION;
        }
  
        int kp = 1;
        int error = delta_left - delta_right;
        int correction_factor = fabs(kp * error);
  
        if(negative_feedback == false) {
            // left wheel is faster
            if(error >= ENCODER_DIFFERENCE_THRESHOLD) {
                debug_config->right_wheel += correction_factor; 
            }
            // right wheel is faster
            if(-error >= ENCODER_DIFFERENCE_THRESHOLD) {
                debug_config->left_wheel += correction_factor;
            }
        }
        else {
            // left wheel is faster
            if(error >= ENCODER_DIFFERENCE_THRESHOLD) {
                debug_config->left_wheel -= correction_factor; 
                debug_config->right_wheel += correction_factor*0.5f;
            }
            // right wheel is faster
            if(-error >= ENCODER_DIFFERENCE_THRESHOLD) {
                debug_config->right_wheel -= correction_factor;
                debug_config->left_wheel += correction_factor*0.5f;
            }
        }
  
        // store previous count values for the next iteration
        prev_left_count = _left_count;
        prev_right_count = _right_count;

        // reset negativeFeedback flag
        negative_feedback = false;    

        // check for edge case 
        if(delta_right == 0 && delta_left == 0 && debug_config->target_value == 0) {
            debug_config->left_wheel = 1;
            debug_config->right_wheel = 1;
        }

        // udpate speed
        update_speed();
    }

    return SUCCESS;
}
