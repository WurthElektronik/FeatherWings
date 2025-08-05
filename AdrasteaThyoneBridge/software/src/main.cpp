/**
 * \file
 * \brief Main file for the WE-AdrasteaFeatherWing.
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

#include <AdrasteaI/AdrasteaI.h>
#include <ThyoneI/ThyoneI.h>

#include <AdrasteaI/ATCommands/ATEvent.h>
#include <AdrasteaI/ATCommands/ATMQTT.h>
#include <AdrasteaI/ATCommands/ATPacketDomain.h>

#include <we_print.h>

// Defining variables for Adrastea
void Adrastea_EventCallback(char* eventText);

static volatile AdrasteaI_ATPacketDomain_Network_Registration_Status_t status;

static const WE_Arduino_Pin_t AdrasteaI_pin_SleepWakeUp = WE_ARDUINO_PIN(6);

static AdrasteaI_Pins_t AdrasteaI_pins = {
    .AdrasteaI_Pin_Reset = WE_PIN_UNDEFINED(),
    .AdrasteaI_Pin_WakeUp = WE_PIN((void*)&AdrasteaI_pin_SleepWakeUp),
};

static WE_UART_t AdrasteaI_uart;

/*Connect the pins 1 and 2 on JP1 to use the Wake-up feature on Pin GPIO9*/
static const WE_Arduino_Pin_t ThyoneI_pin_SleepWakeUp = WE_ARDUINO_PIN(9);

/*Connect the pins 3 and 4 on JP1 to use the mode change feature on Pin
 * GPIOA3/Pin17*/
static const WE_Arduino_Pin_t ThyoneI_pin_Mode = WE_ARDUINO_PIN(17);

static ThyoneI_Pins_t ThyoneI_pins = {
    .ThyoneI_Pin_Reset = WE_PIN_UNDEFINED(),
    .ThyoneI_Pin_SleepWakeUp = WE_PIN((void*)&ThyoneI_pin_SleepWakeUp),
    .ThyoneI_Pin_Boot = WE_PIN_UNDEFINED(),
    .ThyoneI_Pin_Mode = WE_PIN((void*)&ThyoneI_pin_Mode),
    .ThyoneI_Pin_Busy = WE_PIN_UNDEFINED()};

static WE_UART_t ThyoneI_uart;

#define SETEBOS_MODE_PIN 14

/* State Machine */
typedef enum {
    Adrastea_Application_SM_Wait4NetworkRegister,
    Adrastea_Application_SM_MQTT_Connect,
    Adrastea_Application_SM_MQTT_PublishData,
    Adrastea_Application_SM_Idle,
    Adrastea_Application_SM_Error
} Adrastea_Application_SM_t;

static volatile Adrastea_Application_SM_t adrasteaApplicationCurrentState =
    Adrastea_Application_SM_Idle;

char payload[64];
int payloadIndex = 0;

/*MQTT settings - Mosquitto server*/
#define MQTT_CLIENT_ID "adrastea"
#define MQTT_SERVER_ADDRESS "test.mosquitto.org"
#define MQTT_TOPIC "adrtopic"

// Defining variables for Thyone
#define Transmitter 0
#define THYONE_DEFAULT_RF_CHANNEL 21
#define THYONE_DEFAULT_RF_PROFILE (ThyoneI_Profile_t) ThyoneI_Profile_125kbit

String msg1;

char sendBuffer[224];
char temp[64];
int counter = 0;

static void RxCallback(uint8_t* payload, uint16_t payload_length,
                       uint32_t sourceAddress, int8_t rssi);

void setup() {
    delay(3000);

#if ((WE_DEBUG_PRINT_LEVEL > WE_Debug_Print_Level_Off) || \
     defined(WE_APP_PRINT_ENABLED))
    WE_Print_Init();
#endif

    WE_APP_PRINT("Starting initialization of Adrastea...\r\n");

    AdrasteaI_uart.baudrate = 115200;
    AdrasteaI_uart.flowControl = WE_FlowControl_NoFlowControl;
    AdrasteaI_uart.parity = WE_Parity_None;
    AdrasteaI_uart.uartInit = WE_UART_RXPin0_TXPin1_Init;
    AdrasteaI_uart.uartDeinit = WE_UART_RXPin0_TXPin1_DeInit;
    AdrasteaI_uart.uartTransmit = WE_UART_RXPin0_TXPin1_Transmit;

    if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins,
                        Adrastea_EventCallback)) {
        WE_APP_PRINT("failed to initialize\r\n");
    }

    WE_APP_PRINT("Initialized\r\n");

    adrasteaApplicationCurrentState =
        Adrastea_Application_SM_Wait4NetworkRegister;

    while (adrasteaApplicationCurrentState !=
           Adrastea_Application_SM_MQTT_PublishData) {
        switch (adrasteaApplicationCurrentState) {
            case Adrastea_Application_SM_Wait4NetworkRegister: {
                if (!AdrasteaI_ATPacketDomain_SetNetworkRegistrationResultCode(
                        AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_Enable)) {
                    WE_APP_PRINT(
                        "failed to set network registration result code\r\n");
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Error;
                    break;
                }

                // read current network to mcheck if already registered
                if (!AdrasteaI_ATPacketDomain_ReadNetworkRegistrationStatus((
                        AdrasteaI_ATPacketDomain_Network_Registration_Status_t*)&status)) {
                    WE_APP_PRINT(
                        "failed to read network registration status\r\n");
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Error;
                    break;
                } else if (
                    status.state ==
                    AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_Roaming) {
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_MQTT_Connect;
                } else {
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Idle;
                }
                break;
            }
            case Adrastea_Application_SM_MQTT_Connect: {
                WE_APP_PRINT("Network Registered\r\n");
                if (!AdrasteaI_ATMQTT_SetMQTTUnsolicitedNotificationEvents(
                        AdrasteaI_ATMQTT_Event_All,
                        AdrasteaI_ATCommon_Event_State_Enable)) {
                    WE_APP_PRINT("failed to enable mqtt events\r\n");
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Error;
                }

                if (!AdrasteaI_ATMQTT_ConfigureNodes(
                        AdrasteaI_ATMQTT_Conn_ID_Single_Connectivity_Mode,
                        (char*)MQTT_CLIENT_ID, (char*)MQTT_SERVER_ADDRESS, NULL,
                        NULL)) {
                    WE_APP_PRINT("failed to configure node\r\n");
                    adrasteaApplicationCurrentState =
                        Adrastea_Application_SM_Error;
                }

                if (!AdrasteaI_ATMQTT_Connect(
                        AdrasteaI_ATMQTT_Conn_ID_Single_Connectivity_Mode)) {
                    WE_APP_PRINT("failed to connect\r\n");
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

    /*Set the mode pin on Setebos to Thyone-I mode*/
    SetPinMode(SETEBOS_MODE_PIN, OUTPUT);
    WritePin(SETEBOS_MODE_PIN, HIGH);

    /*Initialize the Thyone-I FeatherWing*/
    WE_APP_PRINT("Starting initialization of Thyone...\r\n");

    ThyoneI_uart.baudrate = THYONEI_DEFAULT_BAUDRATE;
    ThyoneI_uart.flowControl = WE_FlowControl_NoFlowControl;
    ThyoneI_uart.parity = WE_Parity_None;
    ThyoneI_uart.uartInit = WE_UART_RXPin11_TXPin10_Init;
    ThyoneI_uart.uartDeinit = WE_UART_RXPin11_TXPin10_DeInit;
    ThyoneI_uart.uartTransmit = WE_UART_RXPin11_TXPin10_Transmit;

    if (!ThyoneI_Init(&ThyoneI_uart, &ThyoneI_pins,
                      ThyoneI_OperationMode_CommandMode, RxCallback)) {
        WE_APP_PRINT("Thyone init failed \r\n");
    }
}

void loop() { /*The processing of data is done using call backs */ }

void Adrastea_EventCallback(char* eventText) {
    AdrasteaI_ATEvent_t event;
    AdrasteaI_ATEvent_ParseEventType(&eventText, &event);

    switch (event) {
        case AdrasteaI_ATEvent_PacketDomain_Network_Registration_Status: {
            if (!AdrasteaI_ATPacketDomain_ParseNetworkRegistrationStatusEvent(
                    eventText,
                    (AdrasteaI_ATPacketDomain_Network_Registration_Status_t*)&status)) {
                adrasteaApplicationCurrentState = Adrastea_Application_SM_Error;
            } else if (
                status.state ==
                AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_Roaming) {
                adrasteaApplicationCurrentState =
                    Adrastea_Application_SM_MQTT_Connect;
            }
            break;
        }
        case AdrasteaI_ATEvent_MQTT_Connection_Confirmation: {
            AdrasteaI_ATMQTT_Connection_Result_t connResult;
            if (!AdrasteaI_ATMQTT_ParseConnectionConfirmationEvent(
                    eventText, &connResult)) {
                adrasteaApplicationCurrentState = Adrastea_Application_SM_Error;
            } else if (connResult.resultCode ==
                       AdrasteaI_ATMQTT_Event_Result_Code_Success) {
                WE_APP_PRINT("MQTT Connected\r\n");
                WE_APP_PRINT("Enter the data to send: ");
                adrasteaApplicationCurrentState =
                    Adrastea_Application_SM_MQTT_PublishData;
            }
            break;
        }
        default:
            break;
    }
}

/* callback for data reception */
static void RxCallback(uint8_t* payload, uint16_t payload_length,
                       uint32_t sourceAddress, int8_t rssi) {
    int i = 0;

    WE_APP_PRINT("\r\n");
    for (i = 0; i < payload_length; i++) {
        WE_APP_PRINT("%c", *(payload + i));
    }
    WE_APP_PRINT("\r\n");
    /*Publish the packet received via Adrastea-I FeatherWing*/
    payload[payload_length] = '\0';
    if (!AdrasteaI_ATMQTT_Publish(
            AdrasteaI_ATMQTT_Conn_ID_Single_Connectivity_Mode,
            AdrasteaI_ATMQTT_QoS_At_Least_Once,
            AdrasteaI_ATMQTT_Retain_Not_Retained, (char*)MQTT_TOPIC,
            (char*)payload, payload_length)) {
        WE_APP_PRINT("failed to publish\r\n");
        adrasteaApplicationCurrentState = Adrastea_Application_SM_Error;
    }
}
