#ifndef GPS_API_H_
#define GPS_API_H_

#include <stdint.h>
#include "HardwareSerial.h"

// Utilizing UART0 for non-serial monitor activities is not allowed in this GPS context
typedef enum {
    UART_PORT_2=2,
} port_num_t;

// different values permissible for this GPS model (BE-880)
typedef enum {
    DATA_PERIOD_1000MS,
    DATA_PERIOD_2000MS,
    DATA_PERIOD_3000MS,
    DATA_PERIOD_4000MS,
    DATA_PERIOD_5000MS
} data_period_t;

// gps configuration struct
typedef struct {
    port_num_t port_num;            /* Which UART port will the GPS communicate with? */
    unsigned long baud_rate;             /* Desired baud rate (Check GPS datasheet for max/min ratings) */
    uint8_t i2c_addr;               /* Assuming GPS model has a integrated compass IC, what is the i2c address? */
    data_period_t data_period_ms;   /* What is the time interval for GPS data (in milliseconds)*/
    int8_t tx_pin;                 /* TX pin for gps UART */
    int8_t rx_pin;                 /* RX pin for gps UART */
} gps_config_t;


class GPS {

    /* --------------------------------- PRIVATE MEMBERS -----------------------------*/
    private:
    
    // init recv buffer for GPS data
    static const uint8_t BUF_LEN = 100;
    char gps_data[BUF_LEN] = {0};

    // callback function for gps data
    static void gps_data_recv_cb(void);

    /* --------------------------------- PUBLIC MEMBERS -----------------------------*/
    public:

    // constructor
    GPS(void);

    // initialization function
    bool init(gps_config_t* config);

    // test recv function
    bool recv_data(void);
};


#endif /* GPS_API_H_ */