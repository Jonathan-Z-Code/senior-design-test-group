#ifndef SETTINGS_H_
#define SETTINGS_H_


#define RPWM_OUTPUT      (23) // Arduino PWM output for forward
#define LPWM_OUTPUT      (22) // Arduino PWM output for reverse
#define NINE_BIT_RES     (9)  // config setting for 9-bit resolution ADC

#define DEADZONE_MIN_X (215) // determine min x value for deadzone
#define DEADZONE_MAX_X (235) // determine upper x value for deadzone
#define DEADZONE_MIN_Y (220) // determine min y value for deadzone
#define DEADZONE_MAX_Y (240) // determine upper y value for deadzone

#define X_AXIS_MEASUREMENT (0) 
#define Y_AXIS_MEASUREMENT (1)

#define SUCCESS (1)
#define FAILURE (0)

#define ESP_NOW_TX_DELAY (100) // value is in milliseconds

#define OVERFLOW_VALUE (20000) // sets the overflow trigger for timers

#define NUM_ENCODER_SAMPLES (10) // set the number of samples to average for the wheel encoders

#endif