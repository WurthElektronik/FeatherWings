/**
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
 *
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 *THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 *KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 *INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 *INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 *REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 *PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT
 *RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED.
 *INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS
 *OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH ELEKTRONIK EISOS TO USE
 *SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 **/

/**
 * @file
 * @brief AT event definitions.
 */

#include "ATEvent.h"

#include "../Calypso.h"

static const char *ATEvent_Strings[ATEvent_NumberOfValues] = {
    "invalid",
    "eventstartup",
    "eventwakeup",
    "ping",
    "reset_request",
    "error",
    "connect",
    "disconnect",
    "sta_added",
    "sta_removed",
    "p2p_connect",
    "p2p_disconnect",
    "p2p_client_added",
    "p2p_client_removed",
    "p2p_devfound",
    "p2p_request",
    "p2p_connectfail",
    "provisioning_status",
    "provisioning_profile_added",
    "tx_failed",
    "async_event",
    "connect",
    "accept",
    "recv",
    "recvfrom",
    "ipv4_acquired",
    "ipv6_acquired",
    "ip_collision",
    "dhcpv4_leased",
    "dhcpv4_released",
    "ipv4_lost",
    "dhcp_ipv4_acquire_timeout",
    "ipv6_lost",
    "operation",
    "recv",
    "disconnect",
    "connack",
    "puback",
    "suback",
    "unsuback",
    "filegetfilelist",
    "eventhttpget",
    "eventcustomgpio",
    "eventcustomhttppost",
    "device_abort",
    "driver_abort",
    "sync_loss",
    "no_cmd_ack",
    "cmd_timout"};

static bool ATEvent_ParseEventSubType(const char *eventSubTypeString,
                                      ATEvent_t eventMainType,
                                      ATEvent_t *pEventSubType);

/**
 * @brief Parses the received AT command and returns the corresponding
 * ATEvent_t.
 *
 * @param[in,out] pAtCommand AT command starting with '+'
 * @param[out] pEvent ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseEventType(char **pAtCommand, ATEvent_t *pEvent) {
    bool ret = false;
    char cmdName[32];
    char option[64];

    *pEvent = ATEvent_Invalid;

    char delimiters[] = {ATCOMMAND_EVENT_DELIM, ATCOMMAND_STRING_TERMINATE};
    ret = ATCommand_GetCmdName(pAtCommand, cmdName, sizeof(cmdName), delimiters,
                               sizeof(delimiters));
    if (ret) {
        if (0 == strcasecmp(cmdName, "+eventgeneral")) {
            ret = ATCommand_GetNextArgumentString(
                pAtCommand, option, ATCOMMAND_ARGUMENT_DELIM, sizeof(option));
            if (ret) {
                ATEvent_ParseEventSubType(option, ATEvent_General, pEvent);
            }
        } else if (0 == strcasecmp(cmdName, "+eventwlan")) {
            ret = ATCommand_GetNextArgumentString(
                pAtCommand, option, ATCOMMAND_ARGUMENT_DELIM, sizeof(option));
            if (ret) {
                ATEvent_ParseEventSubType(option, ATEvent_Wlan, pEvent);
            }
        } else if (0 == strcasecmp(cmdName, "+eventsocket")) {
            ret = ATCommand_GetNextArgumentString(
                pAtCommand, option, ATCOMMAND_ARGUMENT_DELIM, sizeof(option));
            if (ret) {
                ATEvent_ParseEventSubType(option, ATEvent_Socket, pEvent);
            }
        } else if (0 == strcasecmp(cmdName, "+eventnetapp")) {
            ret = ATCommand_GetNextArgumentString(
                pAtCommand, option, ATCOMMAND_ARGUMENT_DELIM, sizeof(option));
            if (ret) {
                ATEvent_ParseEventSubType(option, ATEvent_Netapp, pEvent);
            }
        } else if (0 == strcasecmp(cmdName, "+eventmqtt")) {
            if (!ATCommand_GetNextArgumentString(pAtCommand, option,
                                                 ATCOMMAND_ARGUMENT_DELIM,
                                                 sizeof(option))) {
                return false;
            }

            if (!ATEvent_ParseEventSubType(option, ATEvent_MQTT, pEvent)) {
                return false;
            }

            if (*pEvent == ATEvent_MQTTOperation) {
                uint8_t mqttOperationID;
                if (!ATCommand_GetNextArgumentEnum(
                        pAtCommand, &mqttOperationID,
                        &ATEvent_Strings[ATEvent_MQTTConnack], 4, 10,
                        ATCOMMAND_ARGUMENT_DELIM)) {
                    return false;
                }
                *pEvent = ATEvent_MQTTConnack + mqttOperationID;
            }

            ret = true;

        } else if (0 == strcasecmp(cmdName, "+eventfatalerror")) {
            ret = ATCommand_GetNextArgumentString(
                pAtCommand, option, ATCOMMAND_ARGUMENT_DELIM, sizeof(option));
            if (ret) {
                ATEvent_ParseEventSubType(option, ATEvent_FatalError, pEvent);
            }
        } else if (0 == strcasecmp(cmdName, "+eventstartup")) {
            *pEvent = ATEvent_Startup;
        } else if (0 == strcasecmp(cmdName, "+eventwakeup")) {
            *pEvent = ATEvent_WakeUp;
        } else if (0 == strcasecmp(cmdName, "+recv")) {
            *pEvent = ATEvent_SocketRcvd;
        } else if (0 == strcasecmp(cmdName, "+recvfrom")) {
            *pEvent = ATEvent_SocketRcvdFrom;
        } else if (0 == strcasecmp(cmdName, "+connect")) {
            *pEvent = ATEvent_SocketTCPConnect;
        } else if (0 == strcasecmp(cmdName, "+accept")) {
            *pEvent = ATEvent_SocketTCPAccept;
        } else if (0 == strcasecmp(cmdName, "+eventhttpget")) {
            *pEvent = ATEvent_HTTPGet;
        } else if (0 == strcasecmp(cmdName, "+eventcustom")) {
            uint8_t customEventId;
            ret = ATCommand_GetNextArgumentInt(
                pAtCommand, &customEventId,
                ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED |
                    ATCOMMAND_INTFLAGS_NOTATION_DEC,
                ATCOMMAND_ARGUMENT_DELIM);
            if (ret) {
                switch (customEventId) {
                    case ATEvent_CustomEventID_GPIO:
                        *pEvent = ATEvent_CustomGPIO;
                        break;

                    case ATEvent_CustomEventID_HTTPPost:
                        *pEvent = ATEvent_CustomHTTPPost;
                        break;

                    default:
                        ret = false;
                }
            }
        } else if (0 == strcasecmp(cmdName, "+netappping")) {
            *pEvent = ATEvent_Ping;
        } else if (0 == strcasecmp(cmdName, "+filegetfilelist")) {
            *pEvent = ATEvent_FileListEntry;
        } else {
            ret = false;
        }
    }
    return ret;
}

/**
 * @brief Returns the name of an event.
 *
 * @param[in] event Type of event
 * @param[out] pEventName Event name
 *
 * @return true if successful, false otherwise
 */
bool ATEvent_GetEventName(ATEvent_t event, char *pEventName) {
    strcpy(pEventName, ATEvent_Strings[event]);
    return true;
}

/**
 * @brief Parses the values of the startup event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] Calypso_Examples_startupEvent The parsed startup event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseStartUpEvent(
    char **pEventArguments, ATEvent_Startup_t *Calypso_Examples_startupEvent) {
    bool ret = false;

    ret = ATCommand_GetNextArgumentString(
        pEventArguments, Calypso_Examples_startupEvent->articleNr,
        ATCOMMAND_ARGUMENT_DELIM,
        sizeof(Calypso_Examples_startupEvent->articleNr));

    if (ret) {
        ret = ATCommand_GetNextArgumentString(
            pEventArguments, Calypso_Examples_startupEvent->chipID,
            ATCOMMAND_ARGUMENT_DELIM,
            sizeof(Calypso_Examples_startupEvent->chipID));
    }

    if (ret) {
        ret = ATCommand_GetNextArgumentString(
            pEventArguments, Calypso_Examples_startupEvent->MACAddress,
            ATCOMMAND_ARGUMENT_DELIM,
            sizeof(Calypso_Examples_startupEvent->MACAddress));
    }

    if (ret) {
        ret = ATCommand_GetNextArgumentInt(
            pEventArguments,
            &(Calypso_Examples_startupEvent->firmwareVersion[0]),
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '.');
    }

    if (ret) {
        ret = ATCommand_GetNextArgumentInt(
            pEventArguments,
            &(Calypso_Examples_startupEvent->firmwareVersion[1]),
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '.');
    }

    if (ret) {
        ret = ATCommand_GetNextArgumentInt(
            pEventArguments,
            &(Calypso_Examples_startupEvent->firmwareVersion[2]),
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Parses the values of the ping event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] pingEvent The parsed ping event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParsePingEvent(char **pEventArguments, ATEvent_Ping_t *pingEvent) {
    bool ret = false;

    ret = ATCommand_GetNextArgumentInt(
        pEventArguments, &(pingEvent->packetsSent),
        ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_NOTATION_DEC |
            ATCOMMAND_INTFLAGS_UNSIGNED,
        ATCOMMAND_ARGUMENT_DELIM);

    if (ret) {
        ret = ATCommand_GetNextArgumentInt(
            pEventArguments, &(pingEvent->packetsReceived),
            ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_NOTATION_DEC |
                ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret) {
        ret = ATCommand_GetNextArgumentInt(
            pEventArguments, &(pingEvent->roundTripTimeMs),
            ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_NOTATION_DEC |
                ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_STRING_TERMINATE);
    }

    return ret;
}

/**
 * @brief Parses the values of the TCP connect event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] connectEvent The parsed TCP connect event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseSocketTCPConnectEvent(
    char **pEventArguments, ATEvent_SocketTCPConnect_t *connectEvent) {
    bool ret = ATCommand_GetNextArgumentInt(
        pEventArguments, &(connectEvent->serverPort),
        ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
        ATCOMMAND_ARGUMENT_DELIM);
    if (ret) {
        ret = ATCommand_GetNextArgumentString(
            pEventArguments, connectEvent->serverAddress,
            ATCOMMAND_STRING_TERMINATE, sizeof(connectEvent->serverAddress));
    }
    return ret;
}

/**
 * @brief Parses the values of the TCP accept event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] acceptEvent The parsed TCP accept event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseSocketTCPAcceptEvent(char **pEventArguments,
                                       ATEvent_SocketTCPAccept_t *acceptEvent) {
    char temp[12];

    bool ret = ATCommand_GetNextArgumentInt(
        pEventArguments, &(acceptEvent->socketID),
        ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
        ATCOMMAND_ARGUMENT_DELIM);

    if (ret) {
        ret = ATCommand_GetNextArgumentString(
            pEventArguments, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp));
        if (ret) {
            ret = ATSocket_ParseSocketFamily(temp, &(acceptEvent->family));
        }
    }

    if (ret) {
        ret = ATCommand_GetNextArgumentInt(
            pEventArguments, &(acceptEvent->clientPort),
            ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM);
    }

    if (ret) {
        ret = ATCommand_GetNextArgumentString(
            pEventArguments, acceptEvent->clientAddress,
            ATCOMMAND_STRING_TERMINATE, sizeof(acceptEvent->clientAddress));
    }

    return ret;
}

/**
 * @brief Parses the values of the socket receive / receive from event
 * arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[in] decodeBase64 Enables decoding of received Base64 data
 * @param[out] rcvdEvent The parsed socket receive / receive from event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseSocketRcvdEvent(char **pEventArguments, bool decodeBase64,
                                  ATEvent_SocketRcvd_t *rcvdEvent) {
    if (!ATCommand_GetNextArgumentInt(
            pEventArguments, &(rcvdEvent->socketID),
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            pEventArguments, &(rcvdEvent->format),
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            pEventArguments, &(rcvdEvent->length),
            ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (decodeBase64) {
        uint32_t decodedSize = Calypso_GetBase64DecBufSize(
            (uint8_t *)*pEventArguments, rcvdEvent->length);
        if (decodedSize - 1 > sizeof(rcvdEvent->data)) {
            return false;
        }

        if (!Calypso_DecodeBase64((uint8_t *)*pEventArguments,
                                  rcvdEvent->length, (uint8_t *)rcvdEvent->data,
                                  &decodedSize)) {
            return false;
        }
        rcvdEvent->length = decodedSize - 1;
        return true;
    }

    /* Received bytes count must not exceed buffer size */
    if (rcvdEvent->length >= sizeof(rcvdEvent->data) - 1) {
        return false;
    }
    return ATCommand_GetNextArgumentString(pEventArguments, rcvdEvent->data,
                                           ATCOMMAND_STRING_TERMINATE,
                                           sizeof(rcvdEvent->data));
}

/**
 * @brief Parses the values of the MQTT from event.
 *
 * @param[in,out] pEventArguments String containing arguments of the event
 * @param[out] rcvdEvent The parsed mqtt received data from event
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseSocketMQTTRcvdEvent(char **pEventArguments,
                                      ATEvent_MQTTRcvd_t *rcvdEvent) {
    if (!ATCommand_GetNextArgumentString(pEventArguments, rcvdEvent->topic,
                                         ATCOMMAND_ARGUMENT_DELIM,
                                         sizeof(rcvdEvent->topic))) {
        return false;
    }

    if (!ATCommand_GetNextArgumentEnum(
            pEventArguments, &(rcvdEvent->qos), ATMQTT_QoSStrings,
            ATMQTT_QoS_NumberOfValues, 5, ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            pEventArguments, &(rcvdEvent->retain),
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            pEventArguments, &(rcvdEvent->duplicate),
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            pEventArguments, (uint8_t *)&(rcvdEvent->dataFormat),
            ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(
            pEventArguments, &(rcvdEvent->dataLength),
            ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_ARGUMENT_DELIM)) {
        return false;
    }

    if (ATCommand_GetNextArgumentString(pEventArguments, rcvdEvent->data,
                                        ATCOMMAND_STRING_TERMINATE,
                                        sizeof(rcvdEvent->data))) {
        if (rcvdEvent->dataFormat == Calypso_DataFormat_Base64) {
            /*Decode the base64 encoded data*/
            uint32_t elen = 0;
            char out[CALYPSO_LINE_MAX_SIZE];
            Calypso_DecodeBase64((uint8_t *)rcvdEvent->data,
                                 rcvdEvent->dataLength, (uint8_t *)out, &elen);
            strcpy(rcvdEvent->data, out);
            rcvdEvent->dataLength = elen;
        }
        return true;
    }

    return false;
}

/**
 * @brief Parses the values of the IPv4 acquired event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] ipv4Event The parsed IPv4 acquired event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseNetappIP4AcquiredEvent(
    char **pEventArguments, ATEvent_NetappIP4Acquired_t *ipv4Event) {
    bool ret = ATCommand_GetNextArgumentString(
        pEventArguments, ipv4Event->address, ATCOMMAND_ARGUMENT_DELIM,
        sizeof(ipv4Event->address));

    if (ret) {
        ret = ATCommand_GetNextArgumentString(
            pEventArguments, ipv4Event->gateway, ATCOMMAND_ARGUMENT_DELIM,
            sizeof(ipv4Event->gateway));
    }

    if (ret) {
        ret = ATCommand_GetNextArgumentString(pEventArguments, ipv4Event->DNS,
                                              ATCOMMAND_STRING_TERMINATE,
                                              sizeof(ipv4Event->DNS));
    }

    return ret;
}

/**
 * @brief Parses the values of the MQTT connect event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT
 * command
 * @param[out] connackEvent The parsed MQTT connack event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseMQTTConnackEvent(char **pEventArguments,
                                   ATEvent_MQTTConnack_t *connackEvent) {
    if (!pEventArguments || !connackEvent) {
        return false;
    }

    uint16_t value2;

    if (!ATCommand_GetNextArgumentInt(
            pEventArguments, &value2,
            ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED,
            ATCOMMAND_STRING_TERMINATE)) {
        return false;
    }

    connackEvent->ackFlags = (uint8_t)(0xFF & (value2 >> 8));
    connackEvent->returnCode = (MQTTConnack_Return_Code_t)(0x00FF & value2);

    return true;
}

/**
 * @brief Retrieves the HTTP GET event ID argument.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] id The HTTP GET event ID
 * @param[in] maxIdLength Max. length of ID (including string termination
 * character).
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseHttpGetEvent(char **pEventArguments, char *id,
                               uint16_t maxIdLength) {
    return ATCommand_GetNextArgumentString(
        pEventArguments, id, ATCOMMAND_STRING_TERMINATE, maxIdLength);
}

/**
 * @brief Parses the values of the file list entry event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] fileListEntry The parsed file list entry data
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseFileListEntryEvent(char **pEventArguments,
                                     ATFile_FileListEntry_t *fileListEntry) {
    return ATFile_ParseFileListEntry(pEventArguments, fileListEntry);
}

/**
 * @brief Parses the values of the custom GPIO event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] gpioId ID of the GPIO that has been configured via the module's
 * REST API.
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseCustomGPIOEvent(char **pEventArguments, uint8_t *gpioId) {
    return ATCommand_GetNextArgumentInt(pEventArguments, gpioId,
                                        ATCOMMAND_INTFLAGS_SIZE8 |
                                            ATCOMMAND_INTFLAGS_UNSIGNED |
                                            ATCOMMAND_INTFLAGS_NOTATION_DEC,
                                        ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the values of the custom HTTP POST event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] id HTTP POST event id
 * @param[out] value HTTP POST event value
 * @param[in] maxIdLength Max. length of ID (including string termination
 * character)
 * @param[in] maxValueLength Max. length of value (including string termination
 * character)
 *
 * @return true if parsed successfully, false otherwise
 */
bool ATEvent_ParseCustomHTTPPostEvent(char **pEventArguments, char *id,
                                      char *value, uint16_t maxIdLength,
                                      uint16_t maxValueLength) {
    if (!ATCommand_GetNextArgumentString(
            pEventArguments, id, ATCOMMAND_ARGUMENT_DELIM, maxIdLength)) {
        return false;
    }
    return ATCommand_GetNextArgumentString(
        pEventArguments, value, ATCOMMAND_STRING_TERMINATE, maxValueLength);
}

/**
 * @brief Parses an event sub type string (first argument of received event
 * string) to ATEvent_t.
 *
 * @param[in] eventSubTypeString String containing the event's ID
 * @param[in] eventMainType Main event type category (ATEvent_General,
 * ATEvent_Wlan, ATEvent_Socket, ATEvent_Netapp, ATEvent_MQTT or
 * ATEvent_FatalError)
 * @param[out] pEventSubType ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
static bool ATEvent_ParseEventSubType(const char *eventSubTypeString,
                                      ATEvent_t eventMainType,
                                      ATEvent_t *pEventSubType) {
    uint8_t typeCount = 0;

    switch (eventMainType) {
        case ATEvent_General:
            typeCount = ATEvent_General_NumberOfValues;
            break;

        case ATEvent_Wlan:
            typeCount = ATEvent_WLAN_NumberOfValues;
            break;

        case ATEvent_Socket:
            typeCount = ATEvent_Socket_NumberOfValues;
            break;

        case ATEvent_Netapp:
            typeCount = ATEvent_Netapp_NumberOfValues;
            break;

        case ATEvent_MQTT:
            typeCount = ATEvent_MQTT_NumberOfValues;
            break;

        case ATEvent_FatalError:
            typeCount = ATEvent_FatalError_NumberOfValues;
            break;

        default:
            break;
    }

    *pEventSubType = ATEvent_Invalid;

    for (int i = 0; i < typeCount; i++) {
        ATEvent_t event = eventMainType + (ATEvent_t)i;
        if (0 == strcasecmp(eventSubTypeString, ATEvent_Strings[event])) {
            *pEventSubType = event;
            return true;
        }
    }

    return false;
}
