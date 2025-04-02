#ifndef CONTROL_API_H_
#define CONTROL_API_H_

#include "esp32-hal-timer.h"

class MotorControl {
    
    private:

    // counting rising edges from the encoder output
    static volatile uint16_t _left_count;
    static void left_encoder_update(void) {
        _left_count++;
    }
  
    // counting rising edges from the encoder output
    static volatile uint16_t _right_count;
    static void right_encoder_update(void) {
        _right_count++;
    }
  
    static void timer0_ISR(void) {
        // timer automatically resets auto-reload value into timer0 register
    }

    // struct to define debug configuration settings for PWM
    typedef struct Config {
        uint8_t left_wheel;
        uint8_t right_wheel;
        int target_value;
    } Config;
    
    // private variables to store user-provided data
    Config _motor_config = {0};
    hw_timer_t* _timer0_cfg;
    uint8_t _right_motor_pin;
    uint8_t _left_motor_pin;

    // variables and constants for timekeeping
    const uint32_t OVERFLOW_VALUE = 125000;
    const uint16_t UPDATE_INTERVAL = 30;
    unsigned long _previous_time;
    
    public:
    
    enum motor_err_t {
        SUCCESS = 0,
        FAIL,
    };

    motor_err_t init(uint8_t left_encoder_pin, uint8_t right_encoder_pin, uint8_t left_motor_pin, uint8_t right_motor_pin);
    motor_err_t update_speed();
    motor_err_t motor_algorithm();

    // public-facing ptr to _motor_config (for debug purposes)
    // I am using this debug_config pointer in the current motor_algorithm in order to change PWM targets on the fly
    Config* debug_config = &_motor_config;

};

#endif