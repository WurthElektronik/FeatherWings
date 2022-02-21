/**
 * \file
 * \brief Main file for the WE-CalypsoWiFiFeatherWing.
 *
 * \copyright (c) 2020 Würth Elektronik eiSos GmbH & Co. KG
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
// USB-Serial debug Interface
TypeSerial *SerialDebug;

// Calypso settings
CalypsoSettings calSettings;

// Calypso object
CALYPSO *calypso;

// Serial communication port for Calypso
TypeHardwareSerial *SerialCalypso;

// Sensors
// WE absolute pressure sensor object
PADS *sensorPADS;
// WE 3-axis acceleration sensor object
ITDS *sensorITDS;
// WE Temperature sensor object
TIDS *sensorTIDS;
// WE humidity sensor object
HIDS *sensorHIDS;

int msgID;

char *serializeData();
void setup()
{
    delay(7000);
    // Using the USB serial port for debug messages
    SerialDebug = SSerial_create(&Serial);
    SSerial_begin(SerialDebug, 115200);

    SerialCalypso = HSerial_create(&Serial1);

    // Create serial port for Calypso WiFi FeatherWing baud 921600, 8E1
    HSerial_beginP(SerialCalypso, 921600, (uint8_t)SERIAL_8E1);
    // Wi-Fi settings
    strcpy(calSettings.wifiSettings.SSID, WI_FI_SSID);
    strcpy(calSettings.wifiSettings.securityParams.securityKey, WI_FI_PASSWORD);
    calSettings.wifiSettings.securityParams.securityType =
        ATWLAN_SECURITY_TYPE_WPA_WPA2;

    // Create sensor objects
    sensorPADS = PADSCreate(SerialDebug);
    sensorITDS = ITDSCreate(SerialDebug);
    sensorTIDS = TIDSCreate(SerialDebug);
    sensorHIDS = HIDSCreate(SerialDebug);

    // Initialize the sensors in default mode
    if (!PADS_simpleInit(sensorPADS))
    {
        SSerial_printf(SerialDebug, "PADS init failed \r\n");
    }

    if (!ITDS_simpleInit(sensorITDS))
    {
        SSerial_printf(SerialDebug, "ITDS init failed \r\n");
    }
    if (!TIDS_simpleInit(sensorTIDS))
    {
        SSerial_printf(SerialDebug, "TIDS init failed \r\n");
    }
    if (!HIDS_simpleInit(sensorHIDS))
    {
        SSerial_printf(SerialDebug, "HIDS init failed \r\n");
    }

    calypso = Calypso_Create(SerialDebug, SerialCalypso, &calSettings);

    // Initialize Calypso
    if (!Calypso_simpleInit(calypso))
    {
        SSerial_printf(SerialDebug, "Calypso init failed \r\n");
    }
    // Connect Calypso WiFi FeatherWing to the Wi-Fi access point
    if (!Calypso_WLANconnect(calypso))
    {
        SSerial_printf(SerialDebug, "WiFi connect fail\r\n");
        return;
    }
    neopixelInit();
    delay(3000);
}

void loop()
{
    char responseStr[MAX_RESP_LENGTH];
    if (Calypso_waitForEvent(calypso) == true)
    {
        if (calypso->lastEvent == ATEvent_Custom)
        {
            char *ptr;
            ptr = strtok(calypso->bufferCalypso.data, ",");
            if (0 == strcasecmp(ptr, POST_NEOPIXEL_ID))
            {
                ptr = strtok(NULL, "\0");
                if (0 == strcasecmp(ptr, STR_RED))
                {
                    neopixelSet(NEO_PIXEL_RED);
                }
                else if (0 == strcasecmp(ptr, STR_GREEN))
                {
                    neopixelSet(NEO_PIXEL_GREEN);
                }
                else if (0 == strcasecmp(ptr, STR_BLUE))
                {
                    neopixelSet(NEO_PIXEL_BLUE);
                }
                else if (0 == strcasecmp(ptr, STR_OFF))
                {
                    neopixelSet(0);
                }
                else
                {
                    SSerial_printf(SerialDebug, "unknown color\r\n");
                }
            }
        }
        else if (calypso->lastEvent == ATEvent_HTTPGet)
        {
            if (0 == strcasecmp(calypso->bufferCalypso.data, GET_TEMPERATURE_ID))
            {
                if (TIDS_readSensorData(sensorTIDS))
                {
                    sprintf(responseStr, "%.2f °C", sensorTIDS->data[tidsTemperature]);
                    SSerial_printf(SerialDebug, "%s\r\n", responseStr);
                    Calypso_HTTPCustomResponse(calypso, responseStr, strlen(responseStr), true);
                }
            }
            else if (0 == strcasecmp(calypso->bufferCalypso.data, GET_HUMIDITY_ID))
            {
                if (HIDS_readSensorData(sensorHIDS))
                {
                    sprintf(responseStr, "%.2f %%", sensorHIDS->data[hidsRelHumidity]);
                    SSerial_printf(SerialDebug, "%s\r\n", responseStr);
                    Calypso_HTTPCustomResponse(calypso, responseStr, strlen(responseStr), true);
                }
            }
            else if (0 == strcasecmp(calypso->bufferCalypso.data, GET_PRESSURE_ID))
            {
                if (PADS_readSensorData(sensorPADS))
                {
                    sprintf(responseStr, "%.3f kPa", sensorPADS->data[padsPressure]);
                    SSerial_printf(SerialDebug, "%s\r\n", responseStr);
                    Calypso_HTTPCustomResponse(calypso, responseStr, strlen(responseStr), true);
                }
            }
            else if (0 == strcasecmp(calypso->bufferCalypso.data, GET_ACCELERATION_ID))
            {
                if (ITDS_readSensorData(sensorITDS))
                {
                    sprintf(responseStr, "X:%.3f g Y:%.3f g  Z:%.3f g", sensorITDS->data[itdsXAcceleration],
                            sensorITDS->data[itdsYAcceleration], sensorITDS->data[itdsZAcceleration]);
                    SSerial_printf(SerialDebug, "%s\r\n", responseStr);
                    Calypso_HTTPCustomResponse(calypso, responseStr, strlen(responseStr), true);
                }
            }
            else
            {
                SSerial_printf(SerialDebug, "Unknown request\r\n");
            }
        }
    }
}