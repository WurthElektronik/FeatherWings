![WE Logo](../../assets/WE_Logo_small_t.png)

# Adrastea Thyone Bridge

## Introduction
This example implements the functionality of a bridge between the proprietary network (Thyone-I FeatherWing) and the MQTT (Adrastea-I FeatherWing). A typical IoT application consists of multiple sensor nodes that could communicate using any local network protocol to a Gateway. The Gateway inturn connects to the cloud over the internet. This example implements a gateway. On boot-up, the Adrastea-I is configured to connect to the MQTT broker over the internet. Further, the Thyone-I is configured in the receive mode. Any data that comes in via the Thyone-I module is published to a predefined topic using the Adrastea-I module.
 
### Quick start example

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
3. Setup: The debug, Adrastea and Thyone UART interfaces are initialized. The Adrastea is configured with the given parameters. Further, the module waits for the network to be registered and connects to the Mosquitto MQTT server.

```C
void setup() {
    delay(3000);

#ifdef WE_DEBUG
    WE_Debug_Init();
#endif

    WE_DEBUG_PRINT("Starting initialization of Adrastea...\r\n");
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

    /*Initialize the Thyone-I FeatherWing*/
    WE_DEBUG_PRINT("Starting initialization of Thyone...\r\n");

    /*Connect the pins 1 and 2 on JP1 to use the Wake-up feature on Pin GPIO9*/
    ThyoneI_pins.ThyoneI_Pin_SleepWakeUp.pin = 9;

    /*Connect the pins 3 and 4 on JP1 to use the mode change feature on Pin
     * GPIOA3/Pin17*/
    ThyoneI_pins.ThyoneI_Pin_Mode.pin = 17;

    if (!ThyoneI_Init(&ThyoneI_pins, THYONEI_DEFAULT_BAUDRATE,
                      WE_FlowControl_NoFlowControl,
                      ThyoneI_OperationMode_CommandMode, RxCallback)) {
        WE_DEBUG_PRINT("Thyone init failed \r\n");
    }
}
```
4. In the main application, the module waits for incoming packets from the Thyone using a callback. Once a packet arrives, it is published to the broker.
```C
/* callback for data reception */
static void RxCallback(uint8_t* payload, uint16_t payload_length,
                       uint32_t sourceAddress, int8_t rssi) {
    int i = 0;

    WE_DEBUG_PRINT("\r\n");
    for (i = 0; i < payload_length; i++) {
        WE_DEBUG_PRINT("%c", *(payload + i));
    }
    WE_DEBUG_PRINT("\r\n");
    /*Publish the packet received via Adrastea-I FeatherWing*/
    payload[payload_length] = '\0';
    if (!ATMQTT_Publish(ATMQTT_Conn_ID_Single_Connectivity_Mode,
                        ATMQTT_QoS_At_Least_Once, ATMQTT_Retain_Not_Retained,
                        (char*)MQTT_TOPIC, (char*)payload, payload_length)) {
        WE_DEBUG_PRINT("failed to publish\r\n");
        adrasteaApplicationCurrentState = Adrastea_Application_SM_Error;
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
