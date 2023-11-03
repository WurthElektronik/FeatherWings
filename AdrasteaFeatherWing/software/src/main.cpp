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

#include "Adrastea.h"

#include "ATCommands/ATEvent.h"
#include "ATCommands/ATMQTT.h"
#include "ATCommands/ATPacketDomain.h"

void Adrastea_EventCallback(char* eventText);

static volatile ATPacketDomain_Network_Registration_Status_t status;

Adrastea_Pins_t Adrastea_pins;

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
#define MQTT_SERVER_ADDRESS "127.0.0.1"
#define MQTT_TOPIC "adrtopic"

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
            WE_DEBUG_PRINT("Enter the data to send: ");
        }
    }
}

void Adrastea_EventCallback(char* eventText) {
    ATEvent_t event;
    ATEvent_ParseEventType(&eventText, &event);

    switch (event) {
        case ATEvent_PacketDomain_Network_Registration_Status: {
            if (!ATPacketDomain_ParseNetworkRegistrationStatusEvent(
                    eventText,
                    (ATPacketDomain_Network_Registration_Status_t*)&status)) {
                adrasteaApplicationCurrentState = Adrastea_Application_SM_Error;
            } else if (
                status.state ==
                ATPacketDomain_Network_Registration_State_Registered_Roaming) {
                adrasteaApplicationCurrentState =
                    Adrastea_Application_SM_MQTT_Connect;
            }
            break;
        }
        case ATEvent_MQTT_Connection_Confirmation: {
            ATMQTT_Connection_Result_t connResult;
            if (!ATMQTT_ParseConnectionConfirmationEvent(eventText,
                                                         &connResult)) {
                adrasteaApplicationCurrentState = Adrastea_Application_SM_Error;
            } else if (connResult.resultCode ==
                       ATMQTT_Event_Result_Code_Success) {
                WE_DEBUG_PRINT("MQTT Connected\r\n");
                WE_DEBUG_PRINT("Enter the data to send: ");
                adrasteaApplicationCurrentState =
                    Adrastea_Application_SM_MQTT_PublishData;
            }
            break;
        }
        default:
            break;
    }
}
