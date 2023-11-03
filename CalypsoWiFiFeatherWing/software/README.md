![WE Logo](../../assets/WE_Logo_small_t.png)

# Calypso Wi-Fi FeatherWing

## Introduction

The [Calypso Wi-Fi FeatherWing](https://www.we-online.de/katalog/en/CALYPSO_FeatherWing) was designed with rapid prototyping in mind. Being fully compatible with the Adafruit ecosystem, this FeatherWing gives the user the flexibility to choose the preferred host microcontroller. The inherent modularity of the ecosystem allows the FeatherWing to be easily integrated into any project.

Würth Elektronik eiSos provides a Software Development Kit (SDK) with examples to support all WE FeatherWings. Here are the salient features of the WE FeatherWing SDK. 
* The SDK is **open-source** and well documented.
* It uses popular open-source tool chain including an IDE.
* The examples are written in Arduino styled C/C++ for easy understanding.
* The core components of the SDK are written in pure C to enable easy porting to any microcontroller platform.
* Modular structure of the software stack makes it easy to integrate into any project.

### Software components

* **Install IDE**: your favourite development IDE (we recommend [Visual Studio Code](https://code.visualstudio.com/) with [Platform IO](https://platformio.org/) extension.
* **PlatformIO**: is a cross-platform, cross-architecture, multiple framework professional tool for embedded software development. It provides the tool chain necessary for the software development including building, debugging, code-upload and many more. PlatformIO works well on all the modern operating systems and supports a host of development boards including the Feathers from Adafruit. Further details about PlatformIO can be found under [platformio.org](https://platformio.org/).
* **WE SDK**: This is a layer of platform-independent pure C drivers for sensors and wireless connectivity modules from Würth Elektronik eiSos. These drivers implement all the necessary functions to utilize full feature set of the sensors and wireless connectivity modules. More details on the SDK and downloads under [WCS Software](https://we-online.com/wcs-software).
* **Board files**: This layer provides abstraction at a board level and provides functions to configure and control individual FeatherWings from WE.
* **User application**: The SDK currently implements a quick start example for each of the FeatherWings.

### Installing the tools

* Install Visual Studio Code on the platform of your choice following the [instructions](https://code.visualstudio.com/docs).
* Follow the instructions to install the [PlatformIO IDE](https://platformio.org/platformio-ide) extension.
* Download and install [Mosquitto MQTT broker](https://mosquitto.org/download/). Follow the instructions for the installation and configuration on the [Mosquitto MQTT broker main page](https://mosquitto.org/man/mosquitto-8.html).

> **Note**: For more information about usage with the cloud check our [Calypso cloud examples](../../Sensor2CloudConnectivity).


### Quick start example

The quick start example for the Calypso Wi-Fi FeatherWing demonstrates a connection to the Mosquitto MQTT server and data transmission.

1. Prerequisites for running this example:
    * A Wi-Fi access point with WPA2 personal security and a known password.
    * Mosquitto MQTT broker running in the same network. Make sure to note the IP address and port of the Mosquitto server. 

2. Configuration: The following parameters need to be set-up before building the code.
    * Wi-Fi access point credentials.
    * MQTT server parameters
    * SNTP server parameters.
```C
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
```
3. Setup: The debug as well as the Calypso UART interfaces are initialized. The calypso is configured with the given parameters. Further, the module connects to the given Wi-Fi, SNTP server and the Mosquitto MQTT server.

```C
void setup() {
    delay(3000);

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    // Wi-Fi settings
    strcpy(calSettings.wifiSettings.SSID, WI_FI_SSID);
    strcpy(calSettings.wifiSettings.securityParams.securityKey, WI_FI_PASSWORD);
    calSettings.wifiSettings.securityParams.securityType =
        ATWLAN_SecurityType_WPA_WPA2;

    // MQTT Settings - Mosquitto broker(non-secure for demo purposes only)
    strcpy(calSettings.mqttSettings.clientID, MQTT_CLIENT_ID);
    calSettings.mqttSettings.flags = ATMQTT_CreateFlags_URL;
    strcpy(calSettings.mqttSettings.serverInfo.address, MQTT_SERVER_ADDRESS);
    calSettings.mqttSettings.serverInfo.port = MQTT_PORT;
    calSettings.mqttSettings.connParams.protocolVersion =
        ATMQTT_ProtocolVersion_v3_1;
    calSettings.mqttSettings.connParams.blockingSend = 0;
    calSettings.mqttSettings.connParams.format = Calypso_DataFormat_Base64;

    // SNTP settings
    strcpy(calSettings.sntpSettings.timezone, SNTP_TIMEZONE);
    strcpy(calSettings.sntpSettings.server, SNTP_SERVER);

    // Initialize Calypso
    if (!Calypso_simpleInit(&calSettings, &Calypso_EventCallback)) {
        WE_DEBUG_PRINT("Calypso init failed \r\n");
        calypsoStartUpCurrentState = Calypso_StartUp_SM_Error;
    }

    while (calypsoStartUpCurrentState != Calypso_StartUp_SM_Finished) {
        switch (calypsoStartUpCurrentState) {
            case Calypso_StartUp_SM_ModuleUp: {
                WE_DEBUG_PRINT("module has started\r\n");
                // check if autoconnect feature will connect fine if not
                // manually connect to chosen ap
                WE_Delay(30);
                if (calypsoStartUpCurrentState == Calypso_StartUp_SM_ModuleUp) {
                    calypsoStartUpCurrentState =
                        Calypso_StartUp_SM_WLAN_Connect;
                }
                break;
            }
            case Calypso_StartUp_SM_WLAN_Connect: {
                if (!Calypso_WLANconnect()) {
                    WE_DEBUG_PRINT("WiFi connect fail\r\n");
                    calypsoStartUpCurrentState = Calypso_StartUp_SM_Error;
                } else if (calypsoStartUpCurrentState ==
                           Calypso_StartUp_SM_WLAN_Connect) {
                    calypsoStartUpCurrentState = Calypso_StartUp_SM_Idle;
                }
                break;
            }
            case Calypso_StartUp_SM_SNTP_Setup: {
                WE_DEBUG_PRINT("WiFi connected\r\n");
                if (!Calypso_setUpSNTP()) {
                    WE_DEBUG_PRINT("SNTP config fail\r\n");
                    calypsoStartUpCurrentState = Calypso_StartUp_SM_Error;
                } else {
                    calypsoStartUpCurrentState =
                        Calypso_StartUp_SM_MQTT_Connect;
                }
                break;
            }
            case Calypso_StartUp_SM_MQTT_Connect: {
                if (!Calypso_MQTTconnect()) {
                    WE_DEBUG_PRINT("MQTT connect fail\r\n");
                    calypsoStartUpCurrentState = Calypso_StartUp_SM_Error;
                } else if (calypsoStartUpCurrentState ==
                           Calypso_StartUp_SM_MQTT_Connect) {
                    calypsoStartUpCurrentState = Calypso_StartUp_SM_Idle;
                }
                break;
            }
            case Calypso_StartUp_SM_Error: {
                WE_DEBUG_PRINT("Error state in start up \r\n");
                return;
            }
            default:
                break;
        }
    }

    WE_DEBUG_PRINT("Startup State machine finished\r\n");
    calypsoApplicationCurrentState = Calypso_Application_SM_Publish_Data;
    msgID = 0;
}
```
4. In the main application, the module gets the current time from the SNTP server, converts it to UNIX timestamp format and publishes this timestamp along with device ID and a packet number to the MQTT server periodically.
```C
void loop() {
    switch (calypsoApplicationCurrentState) {
        case Calypso_Application_SM_Publish_Data: {
            Timestamp timestamp;
            Timer_initTime(&timestamp);

            msgID++;
            if (msgID == INT_MAX) {
                msgID = 0;
            }

            // Get the current time from calypso
            if (!Calypso_getTimestamp(&timestamp)) {
                WE_DEBUG_PRINT("Get time fail\r\n");
                calypsoApplicationCurrentState = Calypso_Application_SM_Error;
            }

            /* convert to unix timestamp */
            unsigned long long unixTime_ms = Time_ConvertToUnix(&timestamp);

            /*Create a JSON object with the device ID, message ID, and time
             * stamp*/
            json_value *payload = json_object_new(1);
            json_object_push(payload, "deviceId", json_string_new(macAddress));
            json_object_push(payload, "messageId", json_integer_new(msgID));
            json_object_push(payload, "ts", json_integer_new(unixTime_ms));

            buf = (char *)malloc(json_measure(payload));
            json_serialize(buf, payload);
            json_builder_free(payload);

            /*Publish to MQTT topic*/
            if (!Calypso_MQTTPublishData((char *)MQTT_TOPIC, 0, buf,
                                         strlen(buf))) {
                WE_DEBUG_PRINT("Publish failed\n\r");
                calypsoApplicationCurrentState = Calypso_Application_SM_Error;
            }

            WE_DEBUG_PRINT("Data published\n\r");

            calypsoApplicationCurrentState = Calypso_Application_SM_Idle;
            /*Clean-up*/
            free(buf);
            break;
        }
        case Calypso_Application_SM_Publish_Ack: {
            WE_DEBUG_PRINT("Publish Data Ack received\n\r");
            WE_Delay(5000);
            calypsoApplicationCurrentState =
                Calypso_Application_SM_Publish_Data;
            break;
        }
        case Calypso_Application_SM_Error: {
            WE_DEBUG_PRINT("Application Error\n\r");
            return;
        }
        default:
            break;
    }
}
```
5. It is possible to view the messages on another MQTT client in the same network by subscribing to the same topic.

### Running the Example

The quick start examples in the SDK are written to be run on Adafruit Feather M0 express. The hardware setup is as simple as stacking up the FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the code.
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio Code.
3. Build and upload the code from the PlatformIO tab as shown in the Figure below
4. After successful upload, click on **Monitor** in PlatformIO extension tab to view the debug logs in the serial terminal.

   ![Running quick start example](../../assets/VSCode.png)
