/**
 * \file
 * \brief Main file for the WE-CalypsoWiFiFeatherWing.
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
#include "calypsoBoard.h"
#include "json-builder.h"
#include "debug.h"

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

// Calypso settings
CalypsoSettings calSettings;

int msgID;

/* Startup State Machine */
typedef enum {
    Calypso_StartUp_SM_ModuleUp,
    Calypso_StartUp_SM_WLAN_Connect,
    Calypso_StartUp_SM_SNTP_Setup,
    Calypso_StartUp_SM_MQTT_Connect,
    Calypso_StartUp_SM_Idle,
    Calypso_StartUp_SM_Finished,
    Calypso_StartUp_SM_Error
} Calypso_StartUp_SM_t;

static volatile Calypso_StartUp_SM_t calypsoStartUpCurrentState =
    Calypso_StartUp_SM_Idle;

/* Application State Machine */
typedef enum {
    Calypso_Application_SM_Publish_Data,
    Calypso_Application_SM_Publish_Ack,
    Calypso_Application_SM_Idle,
    Calypso_Application_SM_Error
} Calypso_Applciation_SM_t;

static volatile Calypso_Applciation_SM_t calypsoApplicationCurrentState =
    Calypso_Application_SM_Idle;

void Calypso_EventCallback(char *eventText);

char *buf;

char macAddress[18];

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
                WE_Delay(1000);
                ATWLAN_SetMode_t mode;

                if (!Calypso_getWLANMode(&mode)) {
                    WE_DEBUG_PRINT("get wlan mode failed\r\n");
                    calypsoStartUpCurrentState = Calypso_StartUp_SM_Error;
                    break;
                } else if (mode == ATWLAN_SetMode_Station) {
                    if (calypsoStartUpCurrentState ==
                        Calypso_StartUp_SM_ModuleUp) {
                        calypsoStartUpCurrentState =
                            Calypso_StartUp_SM_WLAN_Connect;
                        break;
                    }
                    break;
                }

                if (!Calypso_setWLANMode(ATWLAN_SetMode_Station)) {
                    WE_DEBUG_PRINT("set wlan mode failed\r\n");
                    calypsoStartUpCurrentState = Calypso_StartUp_SM_Error;
                    break;
                }

                calypsoStartUpCurrentState = Calypso_StartUp_SM_WLAN_Connect;
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

void Calypso_EventCallback(char *eventText) {
    ATEvent_t event;
    ATEvent_ParseEventType(&eventText, &event);

    if (ATEvent_Invalid == event) {
        return;
    }

    char eventName[32];
    ATEvent_GetEventName(event, eventName);
    switch (event) {
        case ATEvent_Startup: {
            ATEvent_Startup_t startUp;
            if (ATEvent_ParseStartUpEvent(&eventText, &startUp)) {
                strcpy(macAddress, startUp.MACAddress);
                calypsoStartUpCurrentState = Calypso_StartUp_SM_ModuleUp;
            } else {
                calypsoStartUpCurrentState = Calypso_StartUp_SM_Error;
            }
            break;
        }
        case ATEvent_NetappIP4Acquired: {
            ATEvent_NetappIP4Acquired_t ipAcquired;
            calypsoStartUpCurrentState =
                ATEvent_ParseNetappIP4AcquiredEvent(&eventText, &ipAcquired)
                    ? Calypso_StartUp_SM_SNTP_Setup
                    : Calypso_StartUp_SM_Error;
            break;
        }
        case ATEvent_MQTTConnack: {
            ATEvent_MQTTConnack_t mqttConnack;
            if (!ATEvent_ParseMQTTConnackEvent(&eventText, &mqttConnack) ||
                (mqttConnack.returnCode != MQTTConnack_Return_Code_Accepted)) {
                calypsoStartUpCurrentState = Calypso_StartUp_SM_Error;
            } else {
                calypsoStartUpCurrentState = Calypso_StartUp_SM_Finished;
            }
            break;
        }
        case ATEvent_MQTTPuback: {
            calypsoApplicationCurrentState = Calypso_Application_SM_Publish_Ack;
            break;
        }
        default:
            break;
    }
}
