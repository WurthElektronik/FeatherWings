/**
 * \file
 * \brief Main file for the WEICLEDFeatherWing.
 *
 * \copyright (c) 2024 Würth Elektronik eiSos GmbH & Co. KG
 *
 * \page License
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 * THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 * KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 * INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 * INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 * REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 * PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY
 * RIGHT RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT
 * IS USED. INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING
 * THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH
 * ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR
 * ENDORSEMENT THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS PACKAGE
 */

#define PROTEUSIIIFEATHERWING false
#define SENSORFEATHERWING false

/**         Includes         */
#include "global.h"
#include "debug.h"
#include "ICLED.h"
#include "ICLED_demos.h"

#if SENSORFEATHERWING == true
#include "sensorBoard.h"
static float ITDS_acceleration_y;
static volatile bool check_orientation = false;
#define HIDS_PART_NUMBER 2525020210002
static float HIDS_humidity;
static float TIDS_temp;

void startTimer(int frequencyHz);
void TC3_Handler();
#endif

#if PROTEUSIIIFEATHERWING == true
#include "ProteusIII.h"
#define PAYLOAD_BUFFER_SIZE 64

static uint8_t payload_buffer[PAYLOAD_BUFFER_SIZE];

static ProteusIII_Pins_t ProteusIII_pins;

/* Callback functions for various indications sent by the Proteus-III. */
static void RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac,
                       int8_t rssi);

#endif

/* Test Modes */
typedef enum
{
    TEST1,
    TEST2,
    TEST3,
    TEST4,
    TEST5,
    TEST6,
    TEST7,
    TEST8,
#if PROTEUSIIIFEATHERWING == true
    TEST9,
#endif
#if SENSORFEATHERWING == true
    TEST10,
    TEST11,
#endif
    TEST_Total_Count
} TestMode;

static volatile TestMode current_mode = TEST1;

static void PROG_ISR_handler();

static unsigned long prog_debounce_time_elapsed = 0;

static bool running_loop = true;

static volatile bool initial_test_run = true;

static bool orientation_changed = false;

void setup()
{

    // Using the USB serial port for debug messages
#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

#if PROTEUSIIIFEATHERWING == true

    ProteusIII_pins.ProteusIII_Pin_SleepWakeUp.pin = 9;
    ProteusIII_pins.ProteusIII_Pin_Mode.pin = 17;

    ProteusIII_CallbackConfig_t callbackConfig = {0};
    callbackConfig.rxCb = RxCallback;

    if (!ProteusIII_Init(&ProteusIII_pins, PROTEUSIII_DEFAULT_BAUDRATE,
                         WE_FlowControl_NoFlowControl,
                         ProteusIII_OperationMode_CommandMode,
                         callbackConfig))
    {
        WE_DEBUG_PRINT("Proteus init failed \r\n");
    }

    uint8_t oldName[32];
    uint16_t oldnameLength;

    if (!ProteusIII_GetDeviceName(oldName, &oldnameLength))
    {
        WE_DEBUG_PRINT("Proteus failed to get name \r\n");
        exit(0);
    }

    char newName[32] = "ICLED";

    if (memcmp(newName, oldName, strlen(newName)) != 0)
    {
        // Set name to ICLED
        if (!ProteusIII_SetDeviceName((uint8_t *)newName, strlen(newName)))
        {
            WE_DEBUG_PRINT("Proteus failed to set name \r\n");
        }
    }

#endif

#if SENSORFEATHERWING == true
    if (!sensorBoard_Init())
    {
        WE_DEBUG_PRINT("I2C init failed \r\n");
    }

    if (!ITDS_2533020201601_simpleInit())
    {
        WE_DEBUG_PRINT("ITDS init failed \r\n");
    }

    if (!TIDS_2521020222501_simpleInit())
    {
        WE_DEBUG_PRINT("TIDS init failed \r\n");
    }

#if HIDS_PART_NUMBER == 2525020210001
    if (!HIDS_2525020210001_simpleInit())
    {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
    }
#elif HIDS_PART_NUMBER == 2525020210002
    if (!HIDS_2525020210002_simpleInit())
    {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
    }
#endif

    startTimer(5);

#endif

    if (!ICLED_Init(RGB, Landscape))
    {
        WE_DEBUG_PRINT("ICLED init failed \r\n");
    }

    pinMode(ICLED_PROG_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ICLED_PROG_PIN), PROG_ISR_handler, FALLING);
}

void loop()
{
#if SENSORFEATHERWING == true
    ICLED_Orientation current_orientation = ICLED_get_orientation();
    if (check_orientation)
    {
        if (!TIDS_2521020222501_readSensorData(&TIDS_temp))
        {
            WE_DEBUG_PRINT("error reading TIDS data \r\n");
        }
#if HIDS_PART_NUMBER == 2525020210001
        if (!HIDS_2525020210001_readSensorData(&HIDS_humidity, NULL))
        {
            WE_DEBUG_PRINT("error reading TIDS data \r\n");
        }
#elif HIDS_PART_NUMBER == 2525020210002
        if (!HIDS_2525020210002_readSensorData(&HIDS_humidity, NULL))
        {
            WE_DEBUG_PRINT("error reading TIDS data \r\n");
        }
#endif
        if (ITDS_2533020201601_readSensorData(NULL, &ITDS_acceleration_y, NULL, NULL))
        {
            ICLED_set_orientation(ITDS_acceleration_y > 0.05 ? Landscape_UpsideDown : Landscape);
        }
        check_orientation = false;
    }
    orientation_changed = (current_orientation != ICLED_get_orientation());
#endif

    ICLED_set_color_system(RGB);

    switch (current_mode)
    {
    case TEST1:
    {
        running_loop = true;
        ICLED_demo_start_show((bool *)&initial_test_run, &running_loop, (uint8_t *)&current_mode);
        break;
    }
    case TEST2:
    {
        if (!initial_test_run && !orientation_changed)
        {
            break;
        }
        ICLED_clear(false);
        initial_test_run = false;
        ICLED_set_pixel(0, 0, 0, 50, 128);
        ICLED_set_screen_pixel(3, 6, 0, 50, 0, 128);
        ICLED_set_pixel(104, 50, 0, 0, 255);
        break;
    }
    case TEST3:
    {
        ICLED_clear(false);
        running_loop = true;
        ICLED_demo_send_alphabet(128, 128, 128, 10, 80, &running_loop);
        break;
    }
    case TEST4:
    {
        ICLED_clear(false);
        running_loop = true;
        uint16_t place = 12;
        char string[] = "Hello World!";
        ICLED_set_string(string, &place, 128, 128, 128, 10, false);
        static uint16_t hello_world_current_column = place;
        ICLED_start_conditional_loop(0, place, 100, &hello_world_current_column, &running_loop);
        break;
    }
    case TEST5:
    {
        ICLED_clear(false);
        running_loop = true;
        ICLED_demo_show_rainbow(8, 35, &running_loop);
        break;
    }
    case TEST6:
    {
        char price[] = "42,69";
        ICLED_clear(false);
        running_loop = true;
        ICLED_demo_show_price(price, 0, 128, 0, 10, 80, &running_loop);
        break;
    }
    case TEST7:
    {
        uint16_t place = 0;
        ICLED_clear(false);
        running_loop = true;
        ICLED_set_emoji(Emoji_Smile_Face, &place, 128, 128, 128, 10, false);
        ICLED_set_emoji(Emoji_Neutral_Face, &place, 128, 128, 128, 10, false);
        ICLED_set_emoji(Emoji_Frown_Face, &place, 128, 128, 128, 10, false);
        ICLED_set_emoji(Emoji_Heart, &place, 128, 0, 0, 15, false);
        static uint16_t emojis_current_column = 0;
        ICLED_start_conditional_loop(0, place, 100, &emojis_current_column, &running_loop);
        break;
    }
    case TEST8:
    {
        if (!initial_test_run && !orientation_changed)
        {
            break;
        }
        ICLED_clear(false);
        initial_test_run = false;
        ICLED_set_color_system(RGB);
        for (uint8_t i = 0; i < ICLED_ROWS; i++)
        {
            ICLED_set_screen_pixel(i, 0, 227, 0, 11, 50);
        }
        ICLED_set_color_system(HSV);
        for (uint8_t i = 0; i < ICLED_ROWS; i++)
        {
            ICLED_set_screen_pixel(i, ICLED_COLUMNS - 1, 357, 100, 50, 50);
        }
        break;
    }
#if PROTEUSIIIFEATHERWING == true
    case TEST9:
    {
        if (initial_test_run)
        {
            strcpy((char *)payload_buffer, "Send BLE Data");
        }
        initial_test_run = false;
        ICLED_clear(false);
        running_loop = true;
        ICLED_demo_show_ble_data((char *)payload_buffer, 0, 0, 255, 25, 80, &running_loop);
        break;
    }
#endif
#if SENSORFEATHERWING == true
    case TEST10:
    {
        ICLED_clear(false);
        running_loop = true;
        ICLED_demo_show_temp_sensor_data(TIDS_temp, 25, 80, &running_loop);
        break;
    }
    case TEST11:
    {
        ICLED_clear(false);
        running_loop = true;
        ICLED_demo_show_hum_sensor_data(HIDS_humidity, 25, 80, &running_loop);
        break;
    }
#endif
    case TEST_Total_Count:
    {
        WE_DEBUG_PRINT("Invalid Test \r\n");
        exit(0);
    }
    }
}

#if PROTEUSIIIFEATHERWING == true

static void RxCallback(uint8_t *payload, uint16_t payloadLength, uint8_t *btMac,
                       int8_t rssi)
{
    if (current_mode != TEST9)
    {
        return;
    }

    if (payloadLength > (PAYLOAD_BUFFER_SIZE - 1))
    {
        return;
    }

    memcpy(payload_buffer, payload, payloadLength);
    payload_buffer[payloadLength] = '\0';
    running_loop = false;
}

#endif

static void PROG_ISR_handler()
{
    if ((millis() - prog_debounce_time_elapsed) < 300)
    {
        return;
    }
    prog_debounce_time_elapsed = millis();
    current_mode = (TestMode)((current_mode + 1) % TEST_Total_Count);
    running_loop = false;
    initial_test_run = true;
}

#if SENSORFEATHERWING == true

#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1024

void startTimer(int frequencyHz)
{
    REG_GCLK_CLKCTRL = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3);
    while (GCLK->STATUS.bit.SYNCBUSY == 1)
        ; // wait for sync

    TcCount16 *TC = (TcCount16 *)TC3;

    TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;
    while (TC->STATUS.bit.SYNCBUSY == 1)
        ; // wait for sync

    // Use the 16-bit timer
    TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
    while (TC->STATUS.bit.SYNCBUSY == 1)
        ; // wait for sync

    // Use match mode so that the timer counter resets when the count matches the compare register
    TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
    while (TC->STATUS.bit.SYNCBUSY == 1)
        ; // wait for sync

    // Set prescaler to 1024
    TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024;
    while (TC->STATUS.bit.SYNCBUSY == 1)
        ; // wait for sync

    int compareValue = (CPU_HZ / (TIMER_PRESCALER_DIV * frequencyHz)) - 1;
    // Make sure the count is in a proportional position to where it was
    // to prevent any jitter or disconnect when changing the compare value.
    TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
    TC->CC[0].reg = compareValue;
    while (TC->STATUS.bit.SYNCBUSY == 1)
        ;
    // Enable the compare interrupt
    TC->INTENSET.reg = 0;
    TC->INTENSET.bit.MC0 = 1;

    NVIC_EnableIRQ(TC3_IRQn);

    TC->CTRLA.reg |= TC_CTRLA_ENABLE;
    while (TC->STATUS.bit.SYNCBUSY == 1)
        ; // wait for sync
}

void TC3_Handler()
{
    TcCount16 *TC = (TcCount16 *)TC3;
    if (TC->INTFLAG.bit.MC0 == 1)
    {
        TC->INTFLAG.bit.MC0 = 1;
        check_orientation = true;
        running_loop = false;
    }
}

#endif
