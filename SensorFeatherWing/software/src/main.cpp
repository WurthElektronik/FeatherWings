/**
 * \file
 * \brief Main file for the WE-SensorFeatherWing.
 *
 * \copyright (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
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
#include "sensorBoard.h"
#include "debug.h"

float PADS_pressure, PADS_temp;
float ITDS_accelX, ITDS_accelY, ITDS_accelZ, ITDS_temp;
bool ITDS_doubleTapEvent, ITDS_freeFallEvent;
float TIDS_temp;
float HIDS_humidity, HIDS_temp;

#define HIDS_PART_NUMBER 2525020210001

void setup() {
    delay(2000);

    // Using the USB serial port for debug messages
#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    if (!sensorBoard_Init()) {
        WE_DEBUG_PRINT("I2C init failed \r\n");
    }
    // Initialize the sensors in default mode
    if (!PADS_2511020213301_simpleInit()) {
        WE_DEBUG_PRINT("PADS init failed \r\n");
    }

    if (!ITDS_2533020201601_simpleInit()) {
        WE_DEBUG_PRINT("ITDS init failed \r\n");
    }

    if (!TIDS_2521020222501_simpleInit()) {
        WE_DEBUG_PRINT("TIDS init failed \r\n");
    }
#if HIDS_PART_NUMBER == 2525020210001
    if (!HIDS_2525020210001_simpleInit()) {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
    }
#elif HIDS_PART_NUMBER == 2525020210002
    if (!HIDS_2525020210002_simpleInit()) {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
    }
#endif
}

void loop() {
    WE_DEBUG_PRINT("----------------------------------------------------\r\n");
    // Read and print sensor values
    if (PADS_2511020213301_readSensorData(&PADS_pressure, &PADS_temp)) {
        WE_DEBUG_PRINT("WSEN_PADS: Atm. Pres: %f kPa Temp: %f °C\r\n",
                       PADS_pressure, PADS_temp);
    }
    if (ITDS_2533020201601_readSensorData(&ITDS_accelX, &ITDS_accelY,
                                          &ITDS_accelZ, &ITDS_temp)) {
        WE_DEBUG_PRINT(
            "WSEN_ITDS(Acceleration): X:%f g Y:%f g  Z:%f g Temp: "
            "%f °C \r\n",
            ITDS_accelX, ITDS_accelY, ITDS_accelZ, ITDS_temp);
    }
    if (ITDS_2533020201601_readDoubleTapEvent(&ITDS_doubleTapEvent)) {
        WE_DEBUG_PRINT("WSEN_ITDS(DoubleTap): State %s \r\n",
                       ITDS_doubleTapEvent ? "True" : "False");
    }
    if (ITDS_2533020201601_readFreeFallEvent(&ITDS_freeFallEvent)) {
        WE_DEBUG_PRINT("WSEN_ITDS(FreeFall): State %s \r\n",
                       ITDS_freeFallEvent ? "True" : "False");
    }
    if (TIDS_2521020222501_readSensorData(&TIDS_temp)) {
        WE_DEBUG_PRINT("WSEN_TIDS(Temperature): %f °C\r\n", TIDS_temp);
    }
#if HIDS_PART_NUMBER == 2525020210001
    if (HIDS_2525020210001_readSensorData(&HIDS_humidity, &HIDS_temp)) {
        WE_DEBUG_PRINT("WSEN_HIDS: RH: %f %% Temp: %f °C\r\n", HIDS_humidity,
                       HIDS_temp);
    }
#elif HIDS_PART_NUMBER == 2525020210002
    if (HIDS_2525020210002_readSensorData(&HIDS_humidity, &HIDS_temp)) {
        WE_DEBUG_PRINT("WSEN_HIDS: RH: %f %% Temp: %f °C\r\n", HIDS_humidity,
                       HIDS_temp);
    }
#endif

    WE_DEBUG_PRINT("----------------------------------------------------\r\n");
    WE_DEBUG_PRINT("\r\n");
    delay(1000);
}
