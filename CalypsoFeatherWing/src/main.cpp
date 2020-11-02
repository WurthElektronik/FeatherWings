/**
 * \file
 * \brief Main file for the WE-CalypsoFeatherWing.
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

// WiFi access point parameters
#define WI_FI_SSID "AP"
#define WI_FI_PASSWORD "pw"

/*MQTT settings - Mosquitto server*/
#define MQTT_CLIENT_ID "calypso"
#define MQTT_SERVER_ADDRESS "127.0.0.1"
#define MQTT_PORT 1883
#define MQTT_TOPIC "timeStamp"

// SNTP settings
#define SNTP_TIMEZONE "+60"
#define SNTP_SERVER "0.de.pool.ntp.org"

// USB-Serial debug Interface
TypeSerial *SerialDebug;

// Calypso settings
CalypsoSettings calSettings;

// Calypso object
CALYPSO *calypso;

// Serial communication port for Calypso
TypeHardwareSerial *SerialCalypso;

void setup() {
    delay(5000);
    // Using the USB serial port for debug messages
    SerialDebug = SSerial_create(&Serial);
    SSerial_begin(SerialDebug, 115200);

    SerialCalypso = HSerial_create(&Serial1);

    // Create serial port for Calypso FeatherWing
    HSerial_beginP(SerialCalypso, 921600,
                   (uint8_t)((0x10ul) | (0x1ul) | (0x400ul)));
    // Wi-Fi settings
    strcpy(calSettings.wifiSettings.SSID, WI_FI_SSID);
    strcpy(calSettings.wifiSettings.securityParams.securityKey, WI_FI_PASSWORD);
    calSettings.wifiSettings.securityParams.securityType =
        ATWLAN_SECURITY_TYPE_WPA_WPA2;

    // MQTT Settings
    strcpy(calSettings.mqttSettings.clientID, MQTT_CLIENT_ID);
    calSettings.mqttSettings.flags = ATMQTT_CREATE_FLAGS_IP4;
    strcpy(calSettings.mqttSettings.serverInfo.address, MQTT_SERVER_ADDRESS);
    calSettings.mqttSettings.serverInfo.port = MQTT_PORT;
    calSettings.mqttSettings.connParams.protocolVersion = ATMQTT_PROTOCOL_v3_1;
    calSettings.mqttSettings.connParams.blockingSend = 0;
    calSettings.mqttSettings.connParams.format = Calypso_DataFormat_Binary;
    strcpy(calSettings.mqttSettings.userOptions.userName, MQTT_CLIENT_ID);

    // SNTP settings
    strcpy(calSettings.sntpSettings.timezone, SNTP_TIMEZONE);
    strcpy(calSettings.sntpSettings.server, SNTP_SERVER);

    calypso = Calypso_Create(SerialDebug, SerialCalypso, &calSettings);

    // Initialize Calypso
    if (!Calypso_simpleInit(calypso)) {
        SSerial_printf(SerialDebug, "Calypso init failed \r\n");
    }

    // Connect Calypso FeatherWing to the Wi-Fi access point
    if (!Calypso_WLANconnect(calypso)) {
        SSerial_printf(SerialDebug, "WiFi connect fail\r\n");
        return;
    }

    delay(3000);

    // Set up SNTP client on Calypso
    if (!Calypso_setUpSNTP(calypso)) {
        SSerial_printf(SerialDebug, "SNTP config fail\r\n");
    }

    // Connect to Mosquitto MQTT server
    if (!Calypso_MQTTconnect(calypso)) {
        SSerial_printf(SerialDebug, "MQTT connect fail\r\n");
    }
}

void loop() {
    Timestamp timestamp;
    Timer_initTime(&timestamp);

    // Get the current time from calypso
    if (!Calypso_getTimestamp(calypso, &timestamp)) {
        SSerial_printf(SerialDebug, "Get time fail\r\n");
    }

    /* convert to unix timestamp */
    unsigned long long unixTime_ms = Time_ConvertToUnix(&timestamp);

    /*Create a JSON object with the time stamp*/
    json_value *timeStamp = json_object_new(1);
    json_object_push(timeStamp, "ts", json_integer_new(unixTime_ms));
    char *buf = (char *)malloc(json_measure(timeStamp));
    json_serialize(buf, timeStamp);

    /*Publish to MQTT topic*/
    if (!Calypso_MQTTPublishData(calypso, MQTT_TOPIC, 0, buf, strlen(buf),
                                 false)) {
        SSerial_printf(SerialDebug, "Publish failed\n\r");
    }

    /*Clean-up*/
    json_builder_free(timeStamp);
    free(buf);

    delay(5000);
}