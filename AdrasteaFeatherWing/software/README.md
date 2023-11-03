![WE Logo](../../assets/WE_Logo_small_t.png)

# Adrastea FeatherWing

## Introduction

The [Adrastea FeatherWing](https://www.we-online.de/katalog/en/ADRASTEA_FeatherWing) was designed with rapid prototyping in mind. Being fully compatible with the Adafruit ecosystem, this FeatherWing gives the user the flexibility to choose the preferred host microcontroller. The inherent modularity of the ecosystem allows the FeatherWing to be easily integrated into any project.

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

### Quick start example

The quick start example for the Adrastea FeatherWing demonstrates a connection to an unencrypted, unauthenticated Mosquitto MQTT server running on port 1883 and data transmission.

1. Prerequisites for running this example:
    * Mosquitto MQTT broker running in the same network. Make sure to note the IP address and the the broker is unencrypted, unauthenticated  and on port 1883. 

2. Configuration: The following parameters need to be set-up before building the code.
    * MQTT server parameters
```C
/*MQTT settings - Mosquitto server*/
#define MQTT_CLIENT_ID "adrastea"
#define MQTT_SERVER_ADDRESS "127.0.0.1"
#define MQTT_TOPIC "adrtopic"
```
3. Setup: The debug as well as the Adrastea UART interfaces are initialized. The Adrastea is configured with the given parameters. Further, the module waits for the network to be registere and connects to the Mosquitto MQTT server.

```C
void setup() {
    delay(3000);

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    Adrastea_pins.Adrastea_Pin_WakeUp.pin = 6;

    if (!Adrastea_Init(&Adrastea_pins, 115200, WE_FlowControl_NoFlowControl,
                       WE_Parity_None, &Adrastea_EventCallback)) {
        WE_DEBUG_PRINT("failed to initialize\r\n");
    }

    WE_DEBUG_PRINT("Initialized\r\n");

    adrasteaApplicationCurrentState =
        Adrastea_Application_SM_Wait4NetworkRegister;

    while (adrasteaApplicationCurrentState !=
           Adrastea_Application_SM_MQTT_PublishData) {
        switch (adrasteaApplicationCurrentState) {
            case Adrastea_Application_SM_Wait4NetworkRegister: {
                if (!ATPacketDomain_SetNetworkRegistrationResultCode(
                        ATPacketDomain_Network_Registration_Result_Code_Enable)) {
                    WE_DEBUG_PRINT(
                        "failed to set network registration result code\r\n");
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Error;
                    break;
                }

                // read current network to mcheck if already registered
                if (!ATPacketDomain_ReadNetworkRegistrationStatus((
                        ATPacketDomain_Network_Registration_Status_t*)&status)) {
                    WE_DEBUG_PRINT(
                        "failed to read network registration status\r\n");
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Error;
                    break;
                } else if (
                    status.state ==
                    ATPacketDomain_Network_Registration_State_Registered_Roaming) {
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_MQTT_Connect;
                } else {
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Idle;
                }
                break;
            }
            case Adrastea_Application_SM_MQTT_Connect: {
                WE_DEBUG_PRINT("Network Registered\r\n");
                if (!ATMQTT_SetMQTTUnsolicitedNotificationEvents(
                        ATMQTT_Event_All, ATCommon_Event_State_Enable)) {
                    WE_DEBUG_PRINT("failed to enable mqtt events\r\n");
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Error;
                }

                if (!ATMQTT_ConfigureNodes(
                        ATMQTT_Conn_ID_Single_Connectivity_Mode,
                        (char*)MQTT_CLIENT_ID, (char*)MQTT_SERVER_ADDRESS, NULL,
                        NULL)) {
                    WE_DEBUG_PRINT("failed to configure node\r\n");
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Error;
                }

                if (!ATMQTT_Disconnect(
                        ATMQTT_Conn_ID_Single_Connectivity_Mode)) {
                    WE_DEBUG_PRINT("failed to disconnect\r\n");
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Error;
                }

                if (!ATMQTT_Connect(ATMQTT_Conn_ID_Single_Connectivity_Mode)) {
                    WE_DEBUG_PRINT("failed to connect\r\n");
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Error;
                } else if (adrasteaApplicationCurrentState ==
                           Adrastea_Application_SM_MQTT_Connect) {
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Idle;
                }
                break;
            }
            case Adrastea_Application_SM_Error: {
                return;
            }
            default:
                break;
        }
    }
}
```
4. In the main application, the module waits for a string to be entered in the terminal ending with \r and publishes this string to the MQTT server.
```C
void loop() {
    while (Serial.available() && adrasteaApplicationCurrentState ==
                                     Adrastea_Application_SM_MQTT_PublishData) {
        char receivedbyte = Serial.read();
        if (receivedbyte != '\r' && receivedbyte != '\n') {
            payload[payloadIndex++] = receivedbyte;
            WE_DEBUG_PRINT("%c", receivedbyte);
        } else if (receivedbyte == '\r') {
            WE_DEBUG_PRINT("\r\n");
            payload[payloadIndex] = '\0';
            if (!ATMQTT_Publish(ATMQTT_Conn_ID_Single_Connectivity_Mode,
                                ATMQTT_QoS_At_Least_Once,
                                ATMQTT_Retain_Not_Retained, (char*)MQTT_TOPIC,
                                payload, payloadIndex)) {
                WE_DEBUG_PRINT("failed to publish\r\n");
                adrasteaApplicationCurrentState = Adrastea_Application_SM_Error;
            }
            payloadIndex = 0;
            WE_DEBUG_PRINT("Data to send: ");
        }
    }
}
```
5. It is possible to view the messages on another MQTT client by subscribing to the same topic.

### Running the Example

The quick start examples in the SDK are written to be run on Adafruit Feather M0 express. The hardware setup is as simple as stacking up the FeatherWing on top of the M0 Feather and powering up the board.

1. Clone or download the code.
2. Open the workspace of interest with the filename `<FeatherWing>.code-workspace` in Visual Studio Code.
3. Build and upload the code from the PlatformIO tab as shown in the Figure below
4. After successful upload, click on **Monitor** in PlatformIO extension tab to view the debug logs in the serial terminal.

   ![Running quick start example](../../assets/VSCode.png)
