/**
 * \file
 * \brief Main file for the WE-ThyoneWirelessSensors.
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
#include "ThyoneI.h"
#include "sensorBoard.h"

#define THYONE_DEFAULT_RF_CHANNEL 21
#define THYONE_DEFAULT_RF_PROFILE 0

char sendBuffer[224];
char temp[64];

ThyoneI_Pins_t ThyoneI_pins;

float PADS_pressure;
float ITDS_accelX, ITDS_accelY, ITDS_accelZ;
float TIDS_temp;
float HIDS_humidity;

#define HIDS_PART_NUMBER 2525020210001

void setup() {
    delay(5000);

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    ThyoneI_pins.ThyoneI_Pin_SleepWakeUp.pin = 9;
    ThyoneI_pins.ThyoneI_Pin_Mode.pin = 17;

    if (!ThyoneI_Init(&ThyoneI_pins, THYONEI_DEFAULT_BAUDRATE,
                      WE_FlowControl_NoFlowControl,
                      ThyoneI_OperationMode_CommandMode, NULL)) {
        WE_DEBUG_PRINT("Thyone init failed \r\n");
    }

    uint8_t serialNrThy[4] = {0};

    if (ThyoneI_GetSerialNumber(serialNrThy)) {
        WE_DEBUG_PRINT("Thyone-I default source address %02X%02X%02X%02X \r\n",
                       serialNrThy[3], serialNrThy[2], serialNrThy[1],
                       serialNrThy[0]);
    }

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
    sprintf(sendBuffer, "Hello world!\r\n");
    // Read and print sensor values
    if (PADS_2511020213301_readSensorData(&PADS_pressure, NULL)) {
        sprintf(temp, "Pre: %f kPa\r\n", PADS_pressure);
        strcat(sendBuffer, temp);
    }
    if (ITDS_2533020201601_readSensorData(&ITDS_accelX, &ITDS_accelY,
                                          &ITDS_accelZ, NULL)) {
        sprintf(temp, "Acc: X:%f g Y:%f g  Z:%f g\r\n", ITDS_accelX,
                ITDS_accelY, ITDS_accelZ);
        strcat(sendBuffer, temp);
    }
    if (TIDS_2521020222501_readSensorData(&TIDS_temp)) {
        sprintf(temp, "Temp: %f C\r\n", TIDS_temp);
        strcat(sendBuffer, temp);
    }
    if (
#if HIDS_PART_NUMBER == 2525020210001
        HIDS_2525020210001_readSensorData(&HIDS_humidity, NULL)
#elif HIDS_PART_NUMBER == 2525020210002
        HIDS_2525020210002_readSensorData(&HIDS_humidity, NULL)
#else
        false
#endif
    ) {
        sprintf(temp, "RH: %f %%\r\n", HIDS_humidity);
        strcat(sendBuffer, temp);
    }
    WE_DEBUG_PRINT(sendBuffer);

    /*Broadcast Hello message to all the peers in the network*/
    if (ThyoneI_TransmitBroadcast((unsigned char *)sendBuffer,
                                  strlen(sendBuffer))) {
    } else {
        WE_DEBUG_PRINT("Broadcast send failed \r\n");
    }
    delay(5000);
}
