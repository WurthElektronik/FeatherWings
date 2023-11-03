/**
 * \file
 * \brief Main file for the WE-CalypsoWiFiFeatherWing.
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
#include "calypsoBoard.h"
#include "json-builder.h"
#include "sensorBoard.h"
#include <strings.h>

// WiFi access point parameters
#define WI_FI_SSID "AP"
#define WI_FI_PASSWORD "pw"

#define GET_TEMPERATURE_ID "temperature"
#define GET_HUMIDITY_ID "humidity"
#define GET_PRESSURE_ID "pressure"
#define GET_ACCELERATION_ID "acceleration"

#define STR_GREEN "green"
#define STR_BLUE "blue"
#define STR_RED "red"
#define STR_OFF "off"

#define POST_NEOPIXEL_ID "led-color"

#define MAX_RESP_LENGTH 255

#define HIDS_PART_NUMBER 2525020210001

static float PADS_pressure;
static float ITDS_accelX, ITDS_accelY, ITDS_accelZ;
static float TIDS_temp;
static float HIDS_humidity;

// Calypso settings
CalypsoSettings calSettings;

int msgID;

/* Startup State Machine */
typedef enum
{
    Calypso_SM_ModuleUp,
    Calypso_SM_WLAN_Connect,
    Calypso_SM_StartUpFinished,
    Calypso_SM_CustomHTTPPost4LED,
    Calypso_SM_HTTPGet4Sensor,
    Calypso_SM_Idle,
    Calypso_SM_Error
} Calypso_SM_t;

static volatile Calypso_SM_t calypsoApplicationCurrentState =
    Calypso_SM_Idle;

void Calypso_EventCallback(char *eventText);

char CustomHTTPPOSTID[64], CustomHTTPPOSTValue[64];

char HTTPGETID[64];

void setup()
{
    delay(3000);

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    // Wi-Fi settings
    strcpy(calSettings.wifiSettings.SSID, WI_FI_SSID);
    strcpy(calSettings.wifiSettings.securityParams.securityKey, WI_FI_PASSWORD);
    calSettings.wifiSettings.securityParams.securityType =
        ATWLAN_SecurityType_WPA_WPA2;

    if (!sensorBoard_Init())
    {
        WE_DEBUG_PRINT("I2C init failed \r\n");
        calypsoApplicationCurrentState = Calypso_SM_Error;
    }
    // Initialize the sensors in default mode
    if (!PADS_2511020213301_simpleInit())
    {
        WE_DEBUG_PRINT("PADS init failed \r\n");
        calypsoApplicationCurrentState = Calypso_SM_Error;
    }

    if (!ITDS_2533020201601_simpleInit())
    {
        WE_DEBUG_PRINT("ITDS init failed \r\n");
        calypsoApplicationCurrentState = Calypso_SM_Error;
    }

    if (!TIDS_2521020222501_simpleInit())
    {
        WE_DEBUG_PRINT("TIDS init failed \r\n");
        calypsoApplicationCurrentState = Calypso_SM_Error;
    }
#if HIDS_PART_NUMBER == 2525020210001
    if (!HIDS_2525020210001_simpleInit())
    {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
        calypsoApplicationCurrentState = Calypso_SM_Error;
    }
#elif HIDS_PART_NUMBER == 2525020210002
    if (!HIDS_2525020210002_simpleInit())
    {
        WE_DEBUG_PRINT("HIDS init failed \r\n");
        calypsoApplicationCurrentState = Calypso_SM_Error;
    }
#endif

    // Initialize Calypso
    if (!Calypso_simpleInit(&calSettings, &Calypso_EventCallback))
    {
        WE_DEBUG_PRINT("Calypso init failed \r\n");
        calypsoApplicationCurrentState = Calypso_SM_Error;
    }
    while (calypsoApplicationCurrentState != Calypso_SM_StartUpFinished)
    {
        switch (calypsoApplicationCurrentState)
        {
        case Calypso_SM_ModuleUp:
        {
            WE_DEBUG_PRINT("module has started\r\n");
            // check if autoconnect feature will connect fine if not
            // manually connect to chosen ap
            WE_Delay(30);
            if (calypsoApplicationCurrentState == Calypso_SM_ModuleUp)
            {
                calypsoApplicationCurrentState =
                    Calypso_SM_WLAN_Connect;
            }
            break;
        }
        case Calypso_SM_WLAN_Connect:
        {
            if (!Calypso_WLANconnect())
            {
                WE_DEBUG_PRINT("WiFi connect fail\r\n");
                calypsoApplicationCurrentState = Calypso_SM_Error;
            }
            else if (calypsoApplicationCurrentState ==
                     Calypso_SM_WLAN_Connect)
            {
                calypsoApplicationCurrentState = Calypso_SM_Idle;
            }
            break;
        }
        case Calypso_SM_Error:
        {
            WE_DEBUG_PRINT("Error state in start up \r\n");
            return;
        }
        default:
            break;
        }
    }

    WE_DEBUG_PRINT("Startup finished\r\n");
    calypsoApplicationCurrentState = Calypso_SM_Idle;
    // The message ID acts as the packet number
    msgID = 0;
    neopixelInit();
    delay(3000);
}

void loop()
{
    switch (calypsoApplicationCurrentState)
    {
    case Calypso_SM_CustomHTTPPost4LED:
    {
        if (0 == strcasecmp(CustomHTTPPOSTID, POST_NEOPIXEL_ID))
        {
            if (0 == strcasecmp(CustomHTTPPOSTValue, STR_RED))
            {
                neopixelSet(NEO_PIXEL_RED);
            }
            else if (0 == strcasecmp(CustomHTTPPOSTValue, STR_GREEN))
            {
                neopixelSet(NEO_PIXEL_GREEN);
            }
            else if (0 == strcasecmp(CustomHTTPPOSTValue, STR_BLUE))
            {
                neopixelSet(NEO_PIXEL_BLUE);
            }
            else if (0 == strcasecmp(CustomHTTPPOSTValue, STR_OFF))
            {
                neopixelSet(0);
            }
            else
            {
                WE_DEBUG_PRINT("value for custom HTTP POST is not valid\r\n");
            }
        }
        else
        {
            WE_DEBUG_PRINT("Id for custom HTTP POST is not %s\r\n", POST_NEOPIXEL_ID);
        }
        calypsoApplicationCurrentState = Calypso_SM_Idle;
        break;
    }
    case Calypso_SM_HTTPGet4Sensor:
    {
        char responseStr[MAX_RESP_LENGTH];
        if (0 == strcasecmp(HTTPGETID, GET_TEMPERATURE_ID))
        {
            if (TIDS_2521020222501_readSensorData(&TIDS_temp))
            {
                sprintf(responseStr, "%.2f °C", TIDS_temp);
                WE_DEBUG_PRINT("%s\r\n", responseStr);
                Calypso_HTTPCustomResponse(responseStr, strlen(responseStr), true);
            }
        }
        else if (0 == strcasecmp(HTTPGETID, GET_HUMIDITY_ID))
        {
#if HIDS_PART_NUMBER == 2525020210001
            if (HIDS_2525020210001_readSensorData(&HIDS_humidity, NULL))
            {
                sprintf(responseStr, "%.2f %%", HIDS_humidity);
                WE_DEBUG_PRINT("%s\r\n", responseStr);
                Calypso_HTTPCustomResponse(responseStr, strlen(responseStr), true);
            }
#elif HIDS_PART_NUMBER == 2525020210002
            if (HIDS_2525020210002_readSensorData(&HIDS_humidity, NULL))
            {
                sprintf(responseStr, "%.2f %%", HIDS_humidity);
                WE_DEBUG_PRINT("%s\r\n", responseStr);
                Calypso_HTTPCustomResponse(responseStr, strlen(responseStr), true);
            }
#endif
        }
        else if (0 == strcasecmp(HTTPGETID, GET_PRESSURE_ID))
        {
            if (PADS_2511020213301_readSensorData(&PADS_pressure, NULL))
            {
                sprintf(responseStr, "%.3f kPa", PADS_pressure);
                WE_DEBUG_PRINT("%s\r\n", responseStr);
                Calypso_HTTPCustomResponse(responseStr, strlen(responseStr), true);
            }
        }
        else if (0 == strcasecmp(HTTPGETID, GET_ACCELERATION_ID))
        {
            if (ITDS_2533020201601_readSensorData(&ITDS_accelX, &ITDS_accelY,
                                                  &ITDS_accelZ, NULL))
            {
                sprintf(responseStr, "X:%.3f g Y:%.3f g  Z:%.3f g", ITDS_accelX,
                        ITDS_accelY, ITDS_accelZ);
                WE_DEBUG_PRINT("%s\r\n", responseStr);
                Calypso_HTTPCustomResponse(responseStr, strlen(responseStr), true);
            }
        }
        else
        {
            WE_DEBUG_PRINT("Unknown request\r\n");
        }
        calypsoApplicationCurrentState = Calypso_SM_Idle;
        break;
    }
    default:
        break;
    }
}

void Calypso_EventCallback(char *eventText)
{
    ATEvent_t event;
    ATEvent_ParseEventType(&eventText, &event);

    if (ATEvent_Invalid == event)
    {
        return;
    }

    switch (event)
    {
    case ATEvent_Startup:
    {
        ATEvent_Startup_t startUp;
        if (ATEvent_ParseStartUpEvent(&eventText, &startUp))
        {
            calypsoApplicationCurrentState = Calypso_SM_ModuleUp;
        }
        else
        {
            calypsoApplicationCurrentState = Calypso_SM_Error;
        }
        break;
    }
    case ATEvent_NetappIP4Acquired:
    {
        ATEvent_NetappIP4Acquired_t ipAcquired;
        calypsoApplicationCurrentState =
            ATEvent_ParseNetappIP4AcquiredEvent(&eventText, &ipAcquired)
                ? Calypso_SM_StartUpFinished
                : Calypso_SM_Error;
        break;
    }
    case ATEvent_CustomHTTPPost:
    {
        calypsoApplicationCurrentState =
            ATEvent_ParseCustomHTTPPostEvent(&eventText, CustomHTTPPOSTID, CustomHTTPPOSTValue, sizeof(CustomHTTPPOSTID), sizeof(CustomHTTPPOSTValue))
                ? Calypso_SM_CustomHTTPPost4LED
                : Calypso_SM_Error;
        break;
    }
    case ATEvent_HTTPGet:
    {
        calypsoApplicationCurrentState =
            ATEvent_ParseHttpGetEvent(&eventText, HTTPGETID, sizeof(HTTPGETID))
                ? Calypso_SM_HTTPGet4Sensor
                : Calypso_SM_Error;
        break;
    }
    default:
        break;
    }
}
