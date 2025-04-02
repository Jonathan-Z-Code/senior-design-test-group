
/* ------------------------------------- GPS UNIT TESTING INSTRUCTIONS --------------------------------------------

    Software pre-reqs: Unity framework, gps_api header file
    Hardware pre-reqs: Must have a ESP32 unit, and a BE-880 GPS module connected together (follow the tx and rx pin assignments)

    Step 1: Flash this unit test to the esp32 (on platformio the command should be "pio test -e recv -f test_gps")
    Step 2: Observe the serial monitor in order to verify tests have passed or not
    Step 3: If the unit tests all pass, throw a party :) 


   ----------------------------------------------------------------------------------------------------------------
*/

#include "gps_api.h"
#include "unity.h"

GPS gps;

gps_config_t config = {
    .baud_rate = 38400,                     /* default baud rate for BE-880 GPS module */
    .data_period_ms = DATA_PERIOD_1000MS,   /* default data period for BE-880 GPS module */
    .tx_pin = 17,                           /* default UART2 TX pin for UART2 */
    .rx_pin = 16,                           /* default UART2 RX pin for UART2 */
};

/* test if object pointer actually contains object data */
void test_gps_config(void) {
    /* initialize the gps serial */
    bool retval = gps.init(&config);
    TEST_ASSERT(retval);
}

/* test if gps data is coming in */
void test_gps_recv_data(void) {
    bool retval = gps.recv_data();
    TEST_ASSERT(retval);
}

/* descriptions of tests are located where the test is defined */
int run_unity_tests(void) {
    /* configure unity framework*/
    UNITY_BEGIN();
    RUN_TEST(test_gps_config);
    RUN_TEST(test_gps_recv_data);
    return UNITY_END();
}

void setup(void) {
    /* configure UART0 port for debug logs */
    Serial.begin(115200);
    delay(2000); /* Platformio says wait around 2s for serial monitor to connect with Unity */
    Serial.println("installation done.");

    /* run unity test framework! */
    run_unity_tests();
}

void loop(void) {}