#include "gps_api.h"
#include "Arduino.h"
#include "HardwareSerial.h"

/* ptr to a gps HardwareSerial port */
static HardwareSerial* gps = new HardwareSerial(UART_PORT_2);   

/* index and process buffer for parsing gps data */
volatile static uint16_t idx = 0;
static char process_buffer[128] = {0};

/*  IMPORTANT NOTE: 
    This callback function is marked as STATIC in the gps_api.h defintion.
    The reason why is because if I set a callback function
    to a non-static function in a class, there will be errors!!!

    CONTROL FLOW EXPLANATION:
    1. Read the amount of chars recv from GPS UART
    2. Store chars inside of a process buffer until a newline appears
    3. Once this newline has been reached, examine the GPS data header
    4. Does this header equal "$GNGLL" ? If not, then ignore
    5. If the GPS data header does equal "$GNGLL", then we have to parse the location data
*/

void GPS::gps_data_recv_cb(void) {
    int num_bytes = gps->available();                                   /* retrieve the num of bytes available */
    Serial.printf("num bytes: %d \n", num_bytes);                       /* debug purposes */
    while(num_bytes--) {                                                /* iterate though all gps data bytes */
        process_buffer[idx++] = (char)gps->read();                      /* pop chars off the ring buffer */
        if(process_buffer[idx-1] == '\n') {                             /* if prev char read was newline (this is a delimiter) */
            if(strncmp("$GNGLL",process_buffer,6) == 0) {               /* if gps data header matches desired command ($GNGLL) */
                process_buffer[idx] = '\0';                             /* terminate desired command string */
                Serial.printf("HEADER FOUND!: %s", process_buffer);     /* debug purposes print this out to test */
            }
            idx = 0; /* reset index every time a command is parsed*/
        }
    }
    // may or may not have to manually flush buffer yourself
    //gps->flush(false);
}

GPS::GPS(void) {
    /* Default constructor (perhaps move the init function into this constructor ?)*/ 
}

bool GPS::init(gps_config_t* config) {
    gps->setTimeout(10);        /* 10 complete symbols (100 bits) worth of timeout is used */
    gps->setRxBufferSize(1024); /* set RX buf length */
    gps->setTxBufferSize(128);  /* set TX buf length */
    
    /* create a callback function that triggers when receiving gps data */
    gps->onReceive(gps_data_recv_cb, true);
    gps->begin(config->baud_rate, SERIAL_8N1, config->rx_pin, config->tx_pin);
 
    /* sanity check the object creation */
    if(gps != NULL) {
        return true;
    }
    else {
        return false;
    }
}

bool GPS::recv_data(void) {

    /* configure timeout as desired (in milliseconds)*/
    const unsigned long TIMEOUT = 1100;
    
    /* configure the reference point for a timeout condition */
    unsigned long startTime = millis();

    /* block until gps has available data OR timeout occurs */
    while(gps->available() <= 0 && millis() - startTime < TIMEOUT);

    /* if the while loop exited because serial data is available, then return true */
    if(gps->available() > 0) return true;
    else return false; /* timeout condition occured*/

}