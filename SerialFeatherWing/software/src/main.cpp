/**
 * \file
 * \brief Main file for the SerialFeatherWing.
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
#include <ConfigPlatform.h>
#include <global/global.h>
#include <global_platform_types.h>

#include <we_print.h>

#define CalypsoWiFiFeatherWing 0
#define ThyoneWirelessFeatherWing 1

// Wireless module selection
#define FeatherWing ThyoneWirelessFeatherWing

#define RS232 0
#define RS485 1

#define SerialCommunicationStandard RS232

#define BufferSize 256

char RXModulebuffer[BufferSize], RXRSbuffer[BufferSize];
uint16_t RXRSlength = 0;
/* Sensor2BLE State Machine */
typedef enum {
    SerialFeatherWing_SM_Idle,
#if FeatherWing == CalypsoWiFiFeatherWing
    SerialFeatherWing_SM_Calypso_StartUp,
    SerialFeatherWing_SM_Wlan_Connect,
    SerialFeatherWing_SM_MQTT_Connect,
    SerialFeatherWing_SM_MQTT_Subscribe,
#endif
    SerialFeatherWing_SM_Module_Send,
    SerialFeatherWing_SM_RS_Send
} SerialFeatherWing_SM_t;

static volatile SerialFeatherWing_SM_t currentState = SerialFeatherWing_SM_Idle;

#if FeatherWing == CalypsoWiFiFeatherWing
#include <calypsoBoard.h>

bool Gateway_SubscribeToTopics();

// WiFi access point parameters
#define WI_FI_SSID "AP"
#define WI_FI_PASSWORD "pw"

/*MQTT settings - Mosquitto server*/
#define MQTT_CLIENT_ID "calypso"
#define MQTT_SERVER_ADDRESS "127.0.0.1"
#define MQTT_PORT 1883
#define MQTT_Subscribe_Topic "CalypsoCommand"
#define MQTT_Publish_Topic "CalypsoResponse"

// Calypso settings
CalypsoSettings calSettings;

void Calypso_EventCallback(char *eventText);

static void WE_UART_RXPin11_TXPin10_HandleRxByte(uint8_t *bytes,
                                                 size_t bytes_length);

#elif FeatherWing == ThyoneWirelessFeatherWing
#include <ThyoneI/ThyoneI.h>

#define SETEBOS_MODE_PIN 14

#define THYONE_DEFAULT_RF_CHANNEL 21
#define THYONE_DEFAULT_RF_PROFILE 0

/*Connect the pins 1 and 2 on JP1 to use the Wake-up feature on Pin GPIO9*/
static const WE_Arduino_Pin_t pin_SleepWakeUp = WE_ARDUINO_PIN(9);

/*Connect the pins 3 and 4 on JP1 to use the mode change feature on Pin
 * GPIOA3/Pin17*/
static const WE_Arduino_Pin_t pin_Mode = WE_ARDUINO_PIN(17);

static ThyoneI_Pins_t ThyoneI_pins = {
    .ThyoneI_Pin_Reset = WE_PIN_UNDEFINED(),
    .ThyoneI_Pin_SleepWakeUp = WE_PIN((void *)&pin_SleepWakeUp),
    .ThyoneI_Pin_Boot = WE_PIN_UNDEFINED(),
    .ThyoneI_Pin_Mode = WE_PIN((void *)&pin_Mode),
    .ThyoneI_Pin_Busy = WE_PIN_UNDEFINED(),
};

static WE_UART_t ThyoneI_uart = {
    .uartInit = WE_UART_RXPin11_TXPin10_Init,
    .uartDeinit = WE_UART_RXPin11_TXPin10_DeInit,
    .uartTransmit = WE_UART_RXPin11_TXPin10_Transmit,
    .baudrate = THYONEI_DEFAULT_BAUDRATE,
    .flowControl = WE_FlowControl_NoFlowControl,
    .parity = WE_Parity_None,
};

static void ThyoneRxCallback(uint8_t *payload, uint16_t payload_length,
                             uint32_t sourceAddress, int8_t rssi);

static void WE_UART_RXPin0_TXPin1_HandleRxByte(uint8_t *bytes,
                                               size_t bytes_length);
#endif

void setup() {
#if ((WE_DEBUG_PRINT_LEVEL > WE_Debug_Print_Level_Off) || \
     defined(WE_APP_PRINT_ENABLED))
    WE_Print_Init();
#endif

#if SerialCommunicationStandard == RS485
    pinMode(12, OUTPUT);
#endif

    delay(5000);

#if FeatherWing == CalypsoWiFiFeatherWing

    WE_UART_RXPin11_TXPin10_Init(
        115200, WE_FlowControl_NoFlowControl, WE_Parity_None,
        (WE_UART_HandleRxByte_t *)&WE_UART_RXPin11_TXPin10_HandleRxByte);

    // Wi-Fi settings
    strcpy(calSettings.wifiSettings.SSID, WI_FI_SSID);
    strcpy(calSettings.wifiSettings.securityParams.securityKey, WI_FI_PASSWORD);
    calSettings.wifiSettings.securityParams.securityType =
        Calypso_ATWLAN_SecurityType_WPA_WPA2;

    // MQTT Settings - Mosquitto broker(non-secure for demo purposes only)
    strcpy(calSettings.mqttSettings.clientID, MQTT_CLIENT_ID);
    calSettings.mqttSettings.flags = Calypso_ATMQTT_CreateFlags_URL;
    strcpy(calSettings.mqttSettings.serverInfo.address, MQTT_SERVER_ADDRESS);
    calSettings.mqttSettings.serverInfo.port = MQTT_PORT;
    calSettings.mqttSettings.connParams.protocolVersion =
        Calypso_ATMQTT_ProtocolVersion_v3_1;
    calSettings.mqttSettings.connParams.blockingSend = 0;
    calSettings.mqttSettings.connParams.format = Calypso_DataFormat_Base64;
    strcpy(calSettings.mqttSettings.userOptions.userName, "\0");
    strcpy(calSettings.mqttSettings.userOptions.passWord, "\0");

    // Initialize Calypso
    if (!Calypso_simpleInit(&calSettings, &Calypso_EventCallback)) {
        WE_APP_PRINT("Calypso init failed \r\n");
    }

    while (currentState != SerialFeatherWing_SM_Calypso_StartUp) {
    }

    // to wait for calypso auto connect
    WE_Delay(1000);
    Calypso_ATWLAN_SetMode_t mode;

    if (!Calypso_getWLANMode(&mode)) {
        WE_APP_PRINT("get wlan mode failed\r\n");
        return;
    } else if (mode == Calypso_ATWLAN_SetMode_Station) {
        if (currentState == SerialFeatherWing_SM_Calypso_StartUp) {
            currentState = SerialFeatherWing_SM_Wlan_Connect;
            return;
        }
    }

    if (!Calypso_setWLANMode(Calypso_ATWLAN_SetMode_Station)) {
        WE_APP_PRINT("set wlan mode failed\r\n");
        return;
    }

    currentState = SerialFeatherWing_SM_Wlan_Connect;

#elif FeatherWing == ThyoneWirelessFeatherWing
    uint8_t serialNrThy[4] = {0};

    WE_UART_RXPin0_TXPin1_Init(
        115200, WE_FlowControl_NoFlowControl, WE_Parity_None,
        (WE_UART_HandleRxByte_t *)&WE_UART_RXPin0_TXPin1_HandleRxByte);

    SetPinMode(SETEBOS_MODE_PIN, OUTPUT);
    WritePin(SETEBOS_MODE_PIN, HIGH);

    if (!ThyoneI_Init(&ThyoneI_uart, &ThyoneI_pins,
                      ThyoneI_OperationMode_CommandMode, ThyoneRxCallback)) {
        WE_APP_PRINT("Thyone init failed \r\n");
    }

    if (ThyoneI_GetSerialNumber(serialNrThy)) {
        WE_APP_PRINT("Thyone-I default source address %02X%02X%02X%02X \r\n",
                     serialNrThy[3], serialNrThy[2], serialNrThy[1],
                     serialNrThy[0]);
    }
#endif
}

void loop() {
    switch (currentState) {
#if FeatherWing == CalypsoWiFiFeatherWing
        case SerialFeatherWing_SM_Wlan_Connect: {
            if (!Calypso_WLANconnect()) {
                WE_APP_PRINT("WiFi connect fail\r\n");
                currentState = SerialFeatherWing_SM_Idle;
            } else if (currentState == SerialFeatherWing_SM_Wlan_Connect) {
                currentState = SerialFeatherWing_SM_Idle;
            }
            break;
        }
        case SerialFeatherWing_SM_MQTT_Connect: {
            if (!Calypso_MQTTconnect()) {
                WE_APP_PRINT("MQTT connect fail\r\n");
                currentState = SerialFeatherWing_SM_Idle;
            } else if (currentState == SerialFeatherWing_SM_MQTT_Connect) {
                currentState = SerialFeatherWing_SM_Idle;
            }
            break;
        }
        case SerialFeatherWing_SM_MQTT_Subscribe: {
            Calypso_ATMQTT_SubscribeTopic_t nodeCommand = {
                .QoS = Calypso_ATMQTT_QoS_QoS1};

            strcpy(nodeCommand.topic, MQTT_Subscribe_Topic);

            if (!Calypso_subscribe(0, 1, &nodeCommand)) {
                WE_APP_PRINT("MQTT subscription fail\r\n");
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
                WE_APP_PRINT("Not able to send broadcast\r\n");
            }
#elif FeatherWing == ThyoneWirelessFeatherWing
            if (ThyoneI_TransmitBroadcast((uint8_t *)RXRSbuffer,
                                          strlen(RXRSbuffer))) {
                WE_APP_PRINT("Broadcast sent! \r\n");
            }
#endif
            currentState = SerialFeatherWing_SM_Idle;
            break;
        }
        case SerialFeatherWing_SM_RS_Send: {
#if FeatherWing == CalypsoWiFiFeatherWing
#if SerialCommunicationStandard == RS232
            WE_UART_RXPin11_TXPin10_Transmit((const uint8_t *)RXModulebuffer,
                                             strlen(RXModulebuffer));
#elif SerialCommunicationStandard == RS485
            digitalWrite(12, HIGH);
            WE_UART_RXPin11_TXPin10_Transmit(RXModulebuffer,
                                             strlen(RXModulebuffer));
            digitalWrite(12, LOW);
#endif
#elif FeatherWing == ThyoneWirelessFeatherWing
#if SerialCommunicationStandard == RS232
            WE_UART_RXPin0_TXPin1_Transmit((const uint8_t *)RXModulebuffer,
                                           strlen(RXModulebuffer));
#elif SerialCommunicationStandard == RS485
            digitalWrite(12, HIGH);
            WE_UART_RXPin0_TXPin1_Transmit(RXModulebuffer,
                                           strlen(RXModulebuffer));
            digitalWrite(12, LOW);
#endif
#endif
            WE_APP_PRINT("Data sent! \r\n");
            currentState = SerialFeatherWing_SM_Idle;
            break;
        }
        default:
            break;
    }
}

#if FeatherWing == CalypsoWiFiFeatherWing

void Calypso_EventCallback(char *eventText) {
    Calypso_ATEvent_t event;
    Calypso_ATEvent_ParseEventType(&eventText, &event);

    if (Calypso_ATEvent_Invalid == event) {
        return;
    }

    switch (event) {
        case Calypso_ATEvent_Startup: {
            Calypso_ATEvent_Startup_t startUp;
            if (Calypso_ATEvent_ParseStartUpEvent(&eventText, &startUp)) {
                currentState = SerialFeatherWing_SM_Calypso_StartUp;
            }
            break;
        }
        case Calypso_ATEvent_NetappIP4Acquired: {
            Calypso_ATEvent_NetappIP4Acquired_t ipAcquired;
            if (Calypso_ATEvent_ParseNetappIP4AcquiredEvent(&eventText,
                                                            &ipAcquired)) {
                currentState = SerialFeatherWing_SM_MQTT_Connect;
            }
            break;
        }
        case Calypso_ATEvent_WlanDisconnect: {
            currentState = SerialFeatherWing_SM_Wlan_Connect;
            break;
        }
        case Calypso_ATEvent_MQTTConnack: {
            Calypso_ATEvent_MQTTConnack_t mqttConnack;
            if (Calypso_ATEvent_ParseMQTTConnackEvent(&eventText,
                                                      &mqttConnack) &&
                (mqttConnack.returnCode == MQTTConnack_Return_Code_Accepted)) {
                currentState = SerialFeatherWing_SM_MQTT_Subscribe;
            }
            break;
        }
        case Calypso_ATEvent_MQTTSuback: {
            currentState = SerialFeatherWing_SM_Idle;
            break;
        }
        case Calypso_ATEvent_MQTTRecv: {
            Calypso_ATEvent_MQTTRcvd_t mqttReceived;
            if (Calypso_ATEvent_ParseSocketMQTTRcvdEvent(&eventText,
                                                         &mqttReceived) &&
                (strcmp(mqttReceived.topic, MQTT_Subscribe_Topic) == 0)) {
                memcpy(RXModulebuffer, (const char *)mqttReceived.data,
                       mqttReceived.dataLength);
                RXModulebuffer[mqttReceived.dataLength] = '\0';
                currentState = SerialFeatherWing_SM_RS_Send;
            }
            break;
        }
        default:
            break;
    }
}

static void WE_UART_RXPin11_TXPin10_HandleRxByte(uint8_t *bytes,
                                                 size_t bytes_length) {
#if SerialCommunicationStandard == RS485
    if (currentState != SerialFeatherWing_SM_RS_Send) {
#endif
        for (size_t i = 0; i++; i < bytes_length) {
            RXRSbuffer[RXRSlength] = bytes[i];
            RXRSlength++;
            if (RXRSlength >= 2 && RXRSbuffer[RXRSlength - 2] == '\r' &&
                RXRSbuffer[RXRSlength - 1] == '\n') {
                RXRSbuffer[RXRSlength - 2] = '\0';
                currentState = SerialFeatherWing_SM_Module_Send;
                RXRSlength = 0;
            }
        }
#if SerialCommunicationStandard == RS485
    }
#endif
}

#elif FeatherWing == ThyoneWirelessFeatherWing

/* callback for data reception */
static void ThyoneRxCallback(uint8_t *payload, uint16_t payload_length,
                             uint32_t sourceAddress, int8_t rssi) {
    memcpy(RXModulebuffer, (const char *)payload, payload_length);
    RXModulebuffer[payload_length] = '\0';
    currentState = SerialFeatherWing_SM_RS_Send;
}

static void WE_UART_RXPin0_TXPin1_HandleRxByte(uint8_t *bytes,
                                               size_t bytes_length) {
#if SerialCommunicationStandard == RS485
    if (currentState != SerialFeatherWing_SM_RS_Send) {
#endif
        for (size_t i = 0; i++; i < bytes_length) {
            RXRSbuffer[RXRSlength] = bytes[i];
            RXRSlength++;
            if (RXRSlength >= 2 && RXRSbuffer[RXRSlength - 2] == '\r' &&
                RXRSbuffer[RXRSlength - 1] == '\n') {
                RXRSbuffer[RXRSlength - 2] = '\0';
                currentState = SerialFeatherWing_SM_Module_Send;
                RXRSlength = 0;
            }
        }
#if SerialCommunicationStandard == RS485
    }
#endif
}

#endif
