![WE Logo](../../assets/WE_Logo_small_t.png)

# Calypso Wi-Fi FeatherWing

## Introduction

The [Serial FeatherWing](https://www.we-online.de/katalog/en/Serial_FeatherWing) was designed with rapid prototyping in mind. Being fully compatible with the Adafruit ecosystem, this FeatherWing gives the user the flexibility to choose the preferred host microcontroller. The inherent modularity of the ecosystem allows the FeatherWing to be easily integrated into any project.

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

### Quick start example
This example requires the Serial Bridge FeatherWing connected to the M0 Feather and either the Thyone FeatherWing or the Calypso Wi-Fi FeatherWing. This combination can be used to retrofit a legacy RS232/RS485 device with Wireless connectivity.

1. Configuration: The following parameters need to be set-up before building the code.
```C
#define FeatherWing CalypsoWiFiFeatherWing
#define SerialCommunicationStandard RS232
```
2. Setup: The debug as well as the selected module UART interfaces and the UART interface for the serial featherwing are initialized.

```C
void setup() {
#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

#if SerialCommunicationStandard == RS485
    pinMode(12, OUTPUT);
#endif

    delay(5000);

#if FeatherWing == CalypsoWiFiFeatherWing

    WE_UART_RXPin11_TXPin10_Init(115200, WE_FlowControl_NoFlowControl,
                                 WE_Parity_None);

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
    strcpy(calSettings.mqttSettings.userOptions.userName, "\0");
    strcpy(calSettings.mqttSettings.userOptions.passWord, "\0");

    // Initialize Calypso
    if (!Calypso_simpleInit(&calSettings, &Calypso_EventCallback)) {
        WE_DEBUG_PRINT("Calypso init failed \r\n");
    }

    while (currentState != SerialFeatherWing_SM_Calypso_StartUp) {
    }

    // to wait for calypso auto connect
    WE_Delay(30);

    if (currentState == SerialFeatherWing_SM_Calypso_StartUp) {
        currentState = SerialFeatherWing_SM_Wlan_Connect;
    }

#elif FeatherWing == ThyoneWirelessFeatherWing
    uint8_t serialNrThy[4] = {0};

    WE_UART_RXPin0_TXPin1_Init(115200, WE_FlowControl_NoFlowControl,
                               WE_Parity_None);

    ThyoneI_pins.ThyoneI_Pin_SleepWakeUp.pin = 9;
    ThyoneI_pins.ThyoneI_Pin_Mode.pin = 17;

    if (!ThyoneI_Init(&ThyoneI_pins, THYONEI_DEFAULT_BAUDRATE,
                      WE_FlowControl_NoFlowControl,
                      ThyoneI_OperationMode_CommandMode, ThyoneRxCallback)) {
        WE_DEBUG_PRINT("Thyone init failed \r\n");
    }

    if (ThyoneI_GetSerialNumber(serialNrThy)) {
        WE_DEBUG_PRINT("Thyone-I default source address %02X%02X%02X%02X \r\n",
                       serialNrThy[3], serialNrThy[2], serialNrThy[1],
                       serialNrThy[0]);
    }
#endif
}
```
3. In the main application, if the selected module is calypso then the data is read from the serial featherwing and sent to the MQTT broker and vice versa. Otherwise if the selected module is is thyone then data is read from the serial featherwing and sent to thyone and vice versa.
```C
void loop() {
    switch (currentState) {
#if FeatherWing == CalypsoWiFiFeatherWing
        case SerialFeatherWing_SM_Wlan_Connect: {
            if (!Calypso_WLANconnect()) {
                WE_DEBUG_PRINT("WiFi connect fail\r\n");
                currentState = SerialFeatherWing_SM_Idle;
            } else if (currentState == SerialFeatherWing_SM_Wlan_Connect) {
                currentState = SerialFeatherWing_SM_Idle;
            }
            break;
        }
        case SerialFeatherWing_SM_MQTT_Connect: {
            if (!Calypso_MQTTconnect()) {
                WE_DEBUG_PRINT("MQTT connect fail\r\n");
                currentState = SerialFeatherWing_SM_Idle;
            } else if (currentState == SerialFeatherWing_SM_MQTT_Connect) {
                currentState = SerialFeatherWing_SM_Idle;
            }
            break;
        }
        case SerialFeatherWing_SM_MQTT_Subscribe: {
            ATMQTT_SubscribeTopic_t nodeCommand = {.QoS = ATMQTT_QoS_QoS1};

            strcpy(nodeCommand.topic, MQTT_Subscribe_Topic);

            if (!Calypso_subscribe(0, 1, &nodeCommand)) {
                WE_DEBUG_PRINT("MQTT subscription fail\r\n");
                currentState = SerialFeatherWing_SM_Idle;
            } else if (currentState == SerialFeatherWing_SM_MQTT_Subscribe) {
                currentState = SerialFeatherWing_SM_Idle;
            }
            break;
        }
#endif
        case SerialFeatherWing_SM_Module_Send: {
#if FeatherWing == CalypsoWiFiFeatherWing
            if (!Calypso_MQTTPublishData((char *)MQTT_Publish_Topic, 0,
                                         RXRSbuffer, strlen(RXRSbuffer))) {
                WE_DEBUG_PRINT("Not able to send broadcast\r\n");
            }
#elif FeatherWing == ThyoneWirelessFeatherWing
            if (ThyoneI_TransmitBroadcast((uint8_t *)RXRSbuffer,
                                          strlen(RXRSbuffer))) {
                WE_DEBUG_PRINT("Broadcast sent! \r\n");
            }
#endif
            currentState = SerialFeatherWing_SM_Idle;
            break;
        }
        case SerialFeatherWing_SM_RS_Send: {
#if FeatherWing == CalypsoWiFiFeatherWing
#if SerialCommunicationStandard == RS232
            WE_UART_RXPin11_TXPin10_Transmit(RXModulebuffer,
                                             strlen(RXModulebuffer));
#elif SerialCommunicationStandard == RS485
            digitalWrite(12, HIGH);
            WE_UART_RXPin11_TXPin10_Transmit(RXModulebuffer,
                                             strlen(RXModulebuffer));
            digitalWrite(12, LOW);
#endif
#elif FeatherWing == ThyoneWirelessFeatherWing
#if SerialCommunicationStandard == RS232
            WE_UART_RXPin0_TXPin1_Transmit(RXModulebuffer,
                                           strlen(RXModulebuffer));
#elif SerialCommunicationStandard == RS485
            digitalWrite(12, HIGH);
            WE_UART_RXPin0_TXPin1_Transmit(RXModulebuffer,
                                           strlen(RXModulebuffer));
            digitalWrite(12, LOW);
#endif
#endif
            WE_DEBUG_PRINT("Data sent! \r\n");
            currentState = SerialFeatherWing_SM_Idle;
            break;
        }
        default:
            break;
    }
}
```

### Running the Example

The quick start examples in the SDK are written to be run on Adafruit Feather M0 express. The hardware setup is as simple as stacking up the FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the code.
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio Code.
3. Build and upload the code from the PlatformIO tab as shown in the Figure below
4. After successful upload, click on **Monitor** in PlatformIO extension tab to view the debug logs in the serial terminal.

   ![Running quick start example](../../assets/VSCode.png)
