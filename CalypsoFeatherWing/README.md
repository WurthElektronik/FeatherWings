![WE Logo](../assets/WE_Logo_small_t.png)

# Calypso FeatherWing

## Introduction

<!--Goals-->
WE WiFi connectivity evaluation board provides an easy to use platform for learning, experimenting and prototyping cloud connectibity applications.

The Calypso FeatherWing offers easy access to the multiple cloud platforms which then provide the data storage, visualization and other advanced data analyses available in your chosen cloud.

In combination with other WE boards and chosen micro controller, it simplifies the creation of secure data pipelines from connected devices and their sensors to online data storage and processing.
A user can use this FeatherWing to collect sensor data, store it in a database and then visualize the data in graphs.





The SDK can be accessed on Github at [eismart FeatherWing GitHub](/../../).

### Necessary Steps

* **Install IDE**: your favourite development IDE (we recommend [Visual Studio Code](https://code.visualstudio.com/) with [Platform IO](https://platformio.org/) extension.
* **PlatformIO**: is a cross-platform, cross-architecture, multiple framework professional tool for embedded software development. It provides the tool chain necessary for the software development including building, debugging, code-upload and many more. PlatformIO works well on all the modern operating systems and supports a host of development boards including the Feathers from Adafruit. Further details about PlatformIO can be found under [platformio.org](https://platformio.org/)
* **WE SDK**: This is a layer of platform-independent pure C drivers for sensors and wireless connectivity modules from Würth Elektronik eiSos. These drivers implement all the necessary functions to utilize full feature set of the sensors and wireless connectivity modules. More details on the SDK and dowloads under [WCS Software](we-online.com/wcs-software)
* **Board files**: This layer provides abstraction at a board level and provides functions to configure and control individual FeatherWings from WE.
* **User application**: The SDK currently implements a quick start example for each of the FeatherWings.

### Installing the tools

* Install Visual Studio Code on the platform of your choice following the [instructions](code.visualstudio.com/docs)
* Follow the instructions under to install [PlatformIO IDE](platformio.org/install/ide?install=vscode) extension.


## Example

The quick start examples in the SDK are written to be run on Adafruit’s Feather M0 express. The hardware setup is as simple as stacking up the FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the [WE FeatherWing SDK](/)
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio code.
3. Build and upload the code from the PlatformIO tab as shown in the Figure below
4. After successful upload, click on **Monitor** in PlatformIO extension tab to view the debug logs in the serial terminal.(See Figure)


![Running quick start example](assets/VSCode.png)


### Source Code

```

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

int msgID;

void setup() {
    delay(5000);
    // Using the USB serial port for debug messages
    SerialDebug = SSerial_create(&Serial);
    SSerial_begin(SerialDebug, 115200);

    SerialCalypso = HSerial_create(&Serial1);

    // Create serial port for Calypso FeatherWing baud 921600, 8E1
    HSerial_beginP(SerialCalypso, 921600, (uint8_t)SERIAL_8E1);
    // Wi-Fi settings
    strcpy(calSettings.wifiSettings.SSID, WI_FI_SSID);
    strcpy(calSettings.wifiSettings.securityParams.securityKey, WI_FI_PASSWORD);
    calSettings.wifiSettings.securityParams.securityType =
        ATWLAN_SECURITY_TYPE_WPA_WPA2;

    // MQTT Settings - Mosquitto broker(non-secure for demo purposes only)
    strcpy(calSettings.mqttSettings.clientID, MQTT_CLIENT_ID);
    calSettings.mqttSettings.flags = ATMQTT_CREATE_FLAGS_IP4;
    strcpy(calSettings.mqttSettings.serverInfo.address, MQTT_SERVER_ADDRESS);
    calSettings.mqttSettings.serverInfo.port = MQTT_PORT;
    calSettings.mqttSettings.connParams.protocolVersion = ATMQTT_PROTOCOL_v3_1;
    calSettings.mqttSettings.connParams.blockingSend = 0;
    calSettings.mqttSettings.connParams.format = Calypso_DataFormat_Base64;
    strcpy(calSettings.mqttSettings.userOptions.userName, MQTT_CLIENT_ID);
    strcpy(calSettings.mqttSettings.userOptions.password, MQTT_CLIENT_ID);

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

    // Connect to MQTT server
    if (!Calypso_MQTTconnect(calypso)) {
        SSerial_printf(SerialDebug, "MQTT connect fail\r\n");
    }
    msgID = 0;
}

void loop() {
    Timestamp timestamp;
    Timer_initTime(&timestamp);

    msgID++;
    if (msgID == INT_MAX) {
        msgID = 0;
    }

    // Get the current time from calypso
    if (!Calypso_getTimestamp(calypso, &timestamp)) {
        SSerial_printf(SerialDebug, "Get time fail\r\n");
    }

    /* convert to unix timestamp */
    unsigned long long unixTime_ms = Time_ConvertToUnix(&timestamp);

    /*Create a JSON object with the device ID, message ID, and time stamp*/
    json_value *payload = json_object_new(1);
    json_object_push(payload, "deviceId", json_string_new(calypso->MAC_ADDR));
    json_object_push(payload, "messageId", json_integer_new(msgID));
    json_object_push(payload, "ts", json_integer_new(unixTime_ms));

    char *buf = (char *)malloc(json_measure(payload));
    json_serialize(buf, payload);

    /*Publish to MQTT topic*/
    if (!Calypso_MQTTPublishData(calypso, MQTT_TOPIC, 0, buf, strlen(buf),
                                 true)) {
        SSerial_printf(SerialDebug, "Publish failed\n\r");
    }

    /*Clean-up*/
    json_builder_free(payload);
    free(buf);

    delay(5000);
}


```