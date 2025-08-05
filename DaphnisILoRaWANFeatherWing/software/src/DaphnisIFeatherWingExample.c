#include "DaphnisIFeatherWingExample.h"

#include <ConfigPlatform.h>
#include <global/global.h>
#include <global_platform_types.h>
#include <DaphnisI/ATCommands/LoRaWAN.h>

#include <sensorBoard.h>

#include <we_print.h>

// Define HIDS_PART_NUMBER if not already defined
#ifndef HIDS_PART_NUMBER
#define HIDS_PART_NUMBER 2525020210002
#endif

/*
 * See State machine for Daphnis-I sdk example.
 * */
typedef enum {
    DaphnisI_LoRaWAN_SM_Set_Up_Keys,
    DaphnisI_LoRaWAN_SM_Join_Network,
    DaphnisI_LoRaWAN_SM_Send_Data,
    DaphnisI_LoRaWAN_SM_Receive_Data,
    DaphnisI_LoRaWAN_SM_Idle,
    DaphnisI_LoRaWAN_SM_Error
} DaphnisI_LoRaWAN_SM_t;

// TODO BY THE USER - be sure to convert key string to hex byte array WITH LSB
// FIRST!
static DaphnisI_EUI_t appEUI = {0x01, 0x23, 0x45, 0x67,
                                0x89, 0xAB, 0xCD, 0xEF};  // Placeholder
static DaphnisI_Key_t appKey = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB,
                                0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67,
                                0x89, 0xAB, 0xCD, 0xEF};  // Placeholder

static uint8_t rxDataBuffer[DAPHNISI_LORAWAN_MAX_PAYLOAD_SIZE];
static DaphnisI_RxData_t rxData = {.data = rxDataBuffer};
static volatile DaphnisI_LoRaWAN_SM_t currentState = DaphnisI_LoRaWAN_SM_Idle;

float PADS_pressure, PADS_temp;
float ITDS_accelX, ITDS_accelY, ITDS_accelZ, ITDS_temp;
bool ITDS_doubleTapEvent, ITDS_freeFallEvent;
float TIDS_temp;
float HIDS_humidity, HIDS_temp;

bool DaphnisI_Featherwing_SensorSetup(void) {
    if (!sensorBoard_Init()) {
        WE_APP_PRINT("I2C init failed \r\n");
        return false;
    }

    // Initialize the sensors in default mode
    if (!PADS_2511020213301_simpleInit()) {
        WE_APP_PRINT("PADS init failed \r\n");
        return false;
    }

    if (!ITDS_2533020201601_simpleInit()) {
        WE_APP_PRINT("ITDS init failed \r\n");
        return false;
    }

    if (!TIDS_2521020222501_simpleInit()) {
        WE_APP_PRINT("TIDS init failed \r\n");
        return false;
    }

#if HIDS_PART_NUMBER == 2525020210001
    if (!HIDS_2525020210001_simpleInit()) {
        WE_APP_PRINT("HIDS init failed \r\n");
        return false;
    }
#elif HIDS_PART_NUMBER == 2525020210002
    if (!HIDS_2525020210002_simpleInit()) {
        WE_APP_PRINT("HIDS init failed \r\n");
        return false;
    }
#endif

    return true;
}

void Print_SensorValues(void) {
    WE_APP_PRINT("----------------------------------------------------\r\n");

    // Read and print sensor values
    if (PADS_2511020213301_readSensorData(&PADS_pressure, &PADS_temp)) {
        WE_APP_PRINT("WSEN_PADS: Atm. Pres: %f kPa Temp: %f °C\r\n",
                     PADS_pressure, PADS_temp);
    }

    if (ITDS_2533020201601_readSensorData(&ITDS_accelX, &ITDS_accelY,
                                          &ITDS_accelZ, &ITDS_temp)) {
        WE_APP_PRINT(
            "WSEN_ITDS(Acceleration): X:%f g Y:%f g Z:%f g Temp: %f °C \r\n",
            ITDS_accelX, ITDS_accelY, ITDS_accelZ, ITDS_temp);
    }

    if (ITDS_2533020201601_readDoubleTapEvent(&ITDS_doubleTapEvent)) {
        WE_APP_PRINT("WSEN_ITDS(DoubleTap): State %s \r\n",
                     ITDS_doubleTapEvent ? "True" : "False");
    }

    if (ITDS_2533020201601_readFreeFallEvent(&ITDS_freeFallEvent)) {
        WE_APP_PRINT("WSEN_ITDS(FreeFall): State %s \r\n",
                     ITDS_freeFallEvent ? "True" : "False");
    }

    if (TIDS_2521020222501_readSensorData(&TIDS_temp)) {
        WE_APP_PRINT("WSEN_TIDS(Temperature): %f °C\r\n", TIDS_temp);
    }

#if HIDS_PART_NUMBER == 2525020210001
    if (HIDS_2525020210001_readSensorData(&HIDS_humidity, &HIDS_temp)) {
        WE_APP_PRINT("WSEN_HIDS: RH: %f %% Temp: %f °C\r\n", HIDS_humidity,
                     HIDS_temp);
    }
#elif HIDS_PART_NUMBER == 2525020210002
    if (HIDS_2525020210002_readSensorData(&HIDS_humidity, &HIDS_temp)) {
        WE_APP_PRINT("WSEN_HIDS: RH: %f %% Temp: %f °C\r\n", HIDS_humidity,
                     HIDS_temp);
    }
#endif

    WE_APP_PRINT("----------------------------------------------------\r\n");
    WE_APP_PRINT("\r\n");
}

void DaphnisI_Featherwings_ReadAndConvertSensorData(uint8_t *payloadP,
                                                    uint8_t *indexP) {
    // Read pressure and temperature from the PADS sensor
    if (PADS_2511020213301_readSensorData(&PADS_pressure, &PADS_temp)) {
        memcpy(payloadP + *indexP, &PADS_pressure, sizeof(float));
        *indexP += sizeof(float);
        memcpy(payloadP + *indexP, &PADS_temp, sizeof(float));
        *indexP += sizeof(float);
    }

    // Read acceleration and temperature from the ITDS sensor
    if (ITDS_2533020201601_readSensorData(&ITDS_accelX, &ITDS_accelY,
                                          &ITDS_accelZ, &ITDS_temp)) {
        memcpy(payloadP + *indexP, &ITDS_accelX, sizeof(float));
        *indexP += sizeof(float);
        memcpy(payloadP + *indexP, &ITDS_accelY, sizeof(float));
        *indexP += sizeof(float);
        memcpy(payloadP + *indexP, &ITDS_accelZ, sizeof(float));
        *indexP += sizeof(float);
        memcpy(payloadP + *indexP, &ITDS_temp, sizeof(float));
        *indexP += sizeof(float);
    }

    // Read double tap event from the ITDS sensor
    if (ITDS_2533020201601_readDoubleTapEvent(&ITDS_doubleTapEvent)) {
        payloadP[*indexP] = ITDS_doubleTapEvent ? 1 : 0;
        *indexP += 1;
    }

    // Read free fall event from the ITDS sensor
    if (ITDS_2533020201601_readFreeFallEvent(&ITDS_freeFallEvent)) {
        payloadP[*indexP] = ITDS_freeFallEvent ? 1 : 0;
        *indexP += 1;
    }

    // Read temperature from the TIDS sensor
    if (TIDS_2521020222501_readSensorData(&TIDS_temp)) {
        memcpy(payloadP + *indexP, &TIDS_temp, sizeof(float));
        *indexP += sizeof(float);
    }

// Read humidity and temperature from the HIDS sensor (part number
// 2525020210001)
#if HIDS_PART_NUMBER == 2525020210001
    if (HIDS_2525020210001_readSensorData(&HIDS_humidity, &HIDS_temp)) {
        memcpy(payloadP + *indexP, &HIDS_humidity, sizeof(float));
        *indexP += sizeof(float);
        memcpy(payloadP + *indexP, &HIDS_temp, sizeof(float));
        *indexP += sizeof(float);
    }
// Read humidity and temperature from the HIDS sensor (part number
// 2525020210002)
#elif HIDS_PART_NUMBER == 2525020210002
    if (HIDS_2525020210002_readSensorData(&HIDS_humidity, &HIDS_temp)) {
        memcpy(payloadP + *indexP, &HIDS_humidity, sizeof(float));
        *indexP += sizeof(float);
        memcpy(payloadP + *indexP, &HIDS_temp, sizeof(float));
        *indexP += sizeof(float);
    }
#endif
}

void DaphnisI_Featherwing_OTAA_Session(void) {
    bool ret = false;
    while (1) {
        switch (currentState) {
            case DaphnisI_LoRaWAN_SM_Set_Up_Keys: {
                ret = true;

                // Read and print Device EUI for user to copy
                DaphnisI_EUI_t device_eui;
                ret &= DaphnisI_LoRaWAN_GetDeviceEUI(&device_eui);
                WE_APP_PRINT("Device EUI is ");
                for (int8_t i = DAPHNISI_LORAWAN_EUI_LENGTH - 1; i > 0; i--) {
                    WE_APP_PRINT("%02X:", device_eui[i]);
                }
                WE_APP_PRINT("%02X\r\n", device_eui[0]);

                // Setting the keys
                ret &= DaphnisI_LoRaWAN_SetAPPEUI(appEUI);
                ret &= DaphnisI_LoRaWAN_SetKey(DaphnisI_KeyType_APP, appKey);
                currentState = ret ? DaphnisI_LoRaWAN_SM_Join_Network
                                   : DaphnisI_LoRaWAN_SM_Error;
                break;
            }
            case DaphnisI_LoRaWAN_SM_Join_Network: {
                ret = true;
                ret &= DaphnisI_LoRaWAN_Join(DaphnisI_JoinMode_OTAA);
                currentState =
                    ret ? DaphnisI_LoRaWAN_SM_Idle : DaphnisI_LoRaWAN_SM_Error;
                break;
            }
            case DaphnisI_LoRaWAN_SM_Send_Data: {
                ret = true;
                // Set payload size depending on what data shall be transmitted
                uint8_t payload[24];
                uint8_t index = 0;

                // Read sensor data and convert the data to LoRaWAN data payload
                // format
                DaphnisI_Featherwings_ReadAndConvertSensorData(payload, &index);

                // Send the data via LoRaWAN
                ret &= DaphnisI_LoRaWAN_Send(1, payload, index, false);

                currentState = ret ? DaphnisI_LoRaWAN_SM_Send_Data
                                   : DaphnisI_LoRaWAN_SM_Error;

                int transmission_delay = 60000;
#ifdef TRANSMIT_FREQ
                transmission_delay = TRANSMIT_FREQ;
#endif
                WE_Delay(transmission_delay);  // Transmit one package every 60
                                               // seconds by default
                break;
            }
            case DaphnisI_LoRaWAN_SM_Receive_Data: {
                WE_APP_PRINT("%i bytes received on on port %i: %s\r\n",
                             rxData.dataLength, rxData.port, rxData.data);
                currentState = DaphnisI_LoRaWAN_SM_Send_Data;
                break;
            }
            case DaphnisI_LoRaWAN_SM_Error: {
                WE_APP_PRINT("Error occurred\r\n");
                return;
            }
            default:
                break;
        }
    }

    return;
}

void DaphnisI_OTAA_EventCallback(DaphnisI_ATEvent_t event, char *eventText) {
    switch (event) {
        case DaphnisI_ATEvent_StartUp: {
            currentState = DaphnisI_LoRaWAN_SM_Set_Up_Keys;
            break;
        }
        case DaphnisI_ATEvent_LoRaWAN_JoinState_Success: {
            currentState = DaphnisI_LoRaWAN_SM_Send_Data;
            break;
        }
        case DaphnisI_ATEvent_LoRaWAN_JoinState_Fail: {
            currentState =
                DaphnisI_LoRaWAN_SM_Join_Network;  // try joining again ->
                                                   // devnonce could be an issue
            break;
        }
        case DaphnisI_ATEvent_LoRaWAN_RxData: {
            currentState =
                DaphnisI_LoRaWAN_ParseRxDataEvent(&eventText, &rxData)
                    ? DaphnisI_LoRaWAN_SM_Receive_Data
                    : DaphnisI_LoRaWAN_SM_Error;
            break;
        }
        default:
            break;
    }
}