/**
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
 *
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE THAT WÜRTH ELEKTRONIK
 * EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY KIND RELATED TO, BUT NOT LIMITED
 * TO THE NON-INFRINGEMENT OF THIRD PARTIES’ INTELLECTUAL PROPERTY RIGHTS OR THE
 * MERCHANTABILITY OR FITNESS FOR YOUR INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT
 * WARRANT OR REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY PATENT
 * RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT RELATING TO ANY
 * COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED. INFORMATION PUBLISHED BY
 * WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE
 * FROM WÜRTH ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT
 * THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2025 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 **/

/**
 * @file
 * @brief AT event definitions.
 */
#include <Calypso/ATCommands/ATEvent.h>
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>

static const ATCommand_Event_t generalSubEvents[] = {EVENTENTRY("reset_request", Calypso_ATEvent_GeneralResetRequest) LASTEVENTENTRY("error", Calypso_ATEvent_GeneralError)};

static const ATCommand_Event_t wlanSubEvents[] = {EVENTENTRY("connect", Calypso_ATEvent_WlanConnect) EVENTENTRY("disconnect", Calypso_ATEvent_WlanDisconnect) EVENTENTRY("sta_added", Calypso_ATEvent_WlanStaAdded) EVENTENTRY("sta_removed", Calypso_ATEvent_WlanStaRemoved) EVENTENTRY("p2p_connect", Calypso_ATEvent_WlanP2PConnect) EVENTENTRY("p2p_disconnect", Calypso_ATEvent_WlanP2PDisconnect)
                                                      EVENTENTRY("p2p_client_added", Calypso_ATEvent_WlanP2PClientAdded) EVENTENTRY("p2p_client_removed", Calypso_ATEvent_WlanP2PClientRemoved) EVENTENTRY("p2p_devfound", Calypso_ATEvent_WlanP2PDevFound) EVENTENTRY("p2p_request", Calypso_ATEvent_WlanP2PRequest) EVENTENTRY("p2p_connectfail", Calypso_ATEvent_WlanP2PConnectFail)
                                                          EVENTENTRY("provisioning_status", Calypso_ATEvent_WlanProvisioningStatus) LASTEVENTENTRY("provisioning_profile_added", Calypso_ATEvent_WlanProvisioningProfileAdded)};

static const ATCommand_Event_t socketSubEvents[] = {EVENTENTRY("tx_failed", Calypso_ATEvent_SocketTxFailed) LASTEVENTENTRY("async_event", Calypso_ATEvent_SocketAsyncEvent)};

static const ATCommand_Event_t netAppSubEvents[] = {EVENTENTRY("ipv4_acquired", Calypso_ATEvent_NetappIP4Acquired) EVENTENTRY("ipv6_acquired", Calypso_ATEvent_NetappIP6Acquired) EVENTENTRY("ip_collision", Calypso_ATEvent_NetappIPCollision) EVENTENTRY("dhcpv4_leased", Calypso_ATEvent_NetappDHCPv4_leased) EVENTENTRY("dhcpv4_released", Calypso_ATEvent_NetappDHCPv4_released)
                                                        EVENTENTRY("ipv4_lost", Calypso_ATEvent_NetappIPv4Lost) EVENTENTRY("dhcp_ipv4_acquire_timeout", Calypso_ATEvent_NetappDHCPIPv4AcquireTimeout) LASTEVENTENTRY("ipv6_lost", Calypso_ATEvent_NetappIPv6Lost)};

static const ATCommand_Event_t mqttOperationSubEvents[] = {EVENTENTRY("connack", Calypso_ATEvent_MQTTConnack) EVENTENTRY("puback", Calypso_ATEvent_MQTTPuback) EVENTENTRY("suback", Calypso_ATEvent_MQTTSuback) LASTEVENTENTRY("unsuback", Calypso_ATEvent_MQTTUnsuback)};

static const ATCommand_Event_t mqttSubEvents[] = {PARENTEVENTENTRY("operation", mqttOperationSubEvents, ATCOMMAND_ARGUMENT_DELIM) EVENTENTRY("recv", Calypso_ATEvent_MQTTRecv) LASTEVENTENTRY("disconnect", Calypso_ATEvent_MQTTDisconnect)};

static const ATCommand_Event_t fatalErrorSubEvents[] = {EVENTENTRY("device_abort", Calypso_ATEvent_FatalErrorDeviceAbort) EVENTENTRY("driver_abort", Calypso_ATEvent_FatalErrorDriverAbort) EVENTENTRY("sync_loss", Calypso_ATEvent_FatalErrorSyncLost) EVENTENTRY("no_cmd_ack", Calypso_ATEvent_FatalErrorNoCmdAck) LASTEVENTENTRY("cmd_timout", Calypso_ATEvent_FatalErrorCmdTimeout)};

static const ATCommand_Event_t customSubEvents[] = {EVENTENTRY("0", Calypso_ATEvent_CustomGPIO) LASTEVENTENTRY("1", Calypso_ATEvent_CustomHTTPPost)};

static const ATCommand_Event_t moduleMainEvents[] = {PARENTEVENTENTRY("+eventgeneral", generalSubEvents, ATCOMMAND_ARGUMENT_DELIM) PARENTEVENTENTRY("+eventwlan", wlanSubEvents, ATCOMMAND_ARGUMENT_DELIM) PARENTEVENTENTRY("+eventsock", socketSubEvents, ATCOMMAND_ARGUMENT_DELIM) PARENTEVENTENTRY("+eventnetapp", netAppSubEvents, ATCOMMAND_ARGUMENT_DELIM)
                                                         PARENTEVENTENTRY("+eventmqtt", mqttSubEvents, ATCOMMAND_ARGUMENT_DELIM) PARENTEVENTENTRY("+eventfatalerror", fatalErrorSubEvents, ATCOMMAND_ARGUMENT_DELIM) EVENTENTRY("+eventstartup", Calypso_ATEvent_Startup) EVENTENTRY("+eventwakeup", Calypso_ATEvent_WakeUp) EVENTENTRY("+recv", Calypso_ATEvent_SocketRcvd)
                                                             EVENTENTRY("+recvfrom", Calypso_ATEvent_SocketRcvdFrom) EVENTENTRY("+connect", Calypso_ATEvent_SocketTCPConnect) EVENTENTRY("+accept", Calypso_ATEvent_SocketTCPAccept) EVENTENTRY("+select", Calypso_ATEvent_SocketSelect) EVENTENTRY("+eventhttpget", Calypso_ATEvent_HTTPGet)
                                                                 PARENTEVENTENTRY("+eventcustom", customSubEvents, ATCOMMAND_ARGUMENT_DELIM) EVENTENTRY("+netappping", Calypso_ATEvent_Ping) LASTEVENTENTRY("+filegetfilelist", Calypso_ATEvent_FileListEntry)};

/**
 * @brief Parses the received AT command and returns the corresponding Calypso_ATEvent_t.
 *
 * @param[in,out] pAtCommand AT command starting with '+'
 * @param[out] pEvent Calypso_ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATEvent_ParseEventType(char** pAtCommand, Calypso_ATEvent_t* pEvent)
{
    char delimiters[] = {ATCOMMAND_EVENT_DELIM, ATCOMMAND_STRING_TERMINATE};

    if (!ATCommand_ParseEventType(pAtCommand, moduleMainEvents, delimiters, sizeof(delimiters), (uint16_t*)pEvent))
    {
        *pEvent = Calypso_ATEvent_Invalid;
        return false;
    }

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
bool Calypso_ATEvent_ParseStartUpEvent(char** pEventArguments, Calypso_ATEvent_Startup_t* Calypso_Examples_startupEvent)
{

    if (!ATCommand_GetNextArgumentString(pEventArguments, Calypso_Examples_startupEvent->articleNr, ATCOMMAND_ARGUMENT_DELIM, sizeof(Calypso_Examples_startupEvent->articleNr)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, Calypso_Examples_startupEvent->chipID, ATCOMMAND_ARGUMENT_DELIM, sizeof(Calypso_Examples_startupEvent->chipID)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, Calypso_Examples_startupEvent->MACAddress, ATCOMMAND_ARGUMENT_DELIM, sizeof(Calypso_Examples_startupEvent->MACAddress)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(Calypso_Examples_startupEvent->firmwareVersion[0]), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(Calypso_Examples_startupEvent->firmwareVersion[1]), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(Calypso_Examples_startupEvent->firmwareVersion[2]), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the ping event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] pingEvent The parsed ping event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATEvent_ParsePingEvent(char** pEventArguments, Calypso_ATEvent_Ping_t* pingEvent)
{

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pingEvent->packetsSent), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(pingEvent->packetsReceived), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return ATCommand_GetNextArgumentInt(pEventArguments, &(pingEvent->roundTripTimeMs), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the values of the TCP connect event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] connectEvent The parsed TCP connect event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATEvent_ParseSocketTCPConnectEvent(char** pEventArguments, Calypso_ATEvent_SocketTCPConnect_t* connectEvent)
{
    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(connectEvent->serverPort), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return ATCommand_GetNextArgumentString(pEventArguments, connectEvent->serverAddress, ATCOMMAND_STRING_TERMINATE, sizeof(connectEvent->serverAddress));
}

/**
 * @brief Parses the values of the TCP accept event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] acceptEvent The parsed TCP accept event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATEvent_ParseSocketTCPAcceptEvent(char** pEventArguments, Calypso_ATEvent_SocketTCPAccept_t* acceptEvent)
{
    char temp[12];

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(acceptEvent->socketID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, temp, ATCOMMAND_ARGUMENT_DELIM, sizeof(temp)))
    {
        return false;
    }

    if (!Calypso_ATSocket_ParseSocketFamily(temp, &(acceptEvent->family)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(acceptEvent->clientPort), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return ATCommand_GetNextArgumentString(pEventArguments, acceptEvent->clientAddress, ATCOMMAND_STRING_TERMINATE, sizeof(acceptEvent->clientAddress));
}

/**
 * @brief Parses the values of the socket tx failed event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] txFailedEvent The parsed socket tx failed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATEvent_ParseSocketTXFailedEvent(char** pEventArguments, Calypso_ATEvent_SocketTXFailed_t* txFailedtEvent)
{
    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(txFailedtEvent->socketID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return ATCommand_GetNextArgumentInt(pEventArguments, &(txFailedtEvent->errorCode), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the values of the socket receive / receive from event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[in] decodeBase64 Enables decoding of received Base64 data
 * @param[out] rcvdEvent The parsed socket receive / receive from event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATEvent_ParseSocketRcvdEvent(char** pEventArguments, bool decodeBase64, Calypso_ATEvent_SocketRcvd_t* rcvdEvent)
{
    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(rcvdEvent->socketID), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(rcvdEvent->format), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(rcvdEvent->length), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (decodeBase64)
    {
        uint32_t decodedSize;
        if (!Base64_GetDecBufSize((uint8_t*)*pEventArguments, rcvdEvent->length, &decodedSize))
        {
            return false;
        }

        if (decodedSize + 1 > sizeof(rcvdEvent->data))
        {
            /* not enough space */
            return false;
        }

        decodedSize = sizeof(rcvdEvent->data) - 1 /*'\0'*/;
        if (!Base64_Decode((uint8_t*)*pEventArguments, rcvdEvent->length, (uint8_t*)rcvdEvent->data, &decodedSize))
        {
            return false;
        }
        /* add string termination character needed by the Calypso functions */
        rcvdEvent->data[decodedSize] = '\0';

        rcvdEvent->length = decodedSize;
        return true;
    }

    /* Received bytes count must not exceed buffer size */
    if (rcvdEvent->length >= sizeof(rcvdEvent->data) - 1)
    {
        return false;
    }
    return ATCommand_GetNextArgumentString(pEventArguments, rcvdEvent->data, ATCOMMAND_STRING_TERMINATE, sizeof(rcvdEvent->data));
}

/**
 * @brief Parses the values of the MQTT from event.
 *
 * @param[in,out] pEventArguments String containing arguments of the event
 * @param[out] rcvdEvent The parsed mqtt received data from event
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATEvent_ParseSocketMQTTRcvdEvent(char** pEventArguments, Calypso_ATEvent_MQTTRcvd_t* rcvdEvent)
{
    if (!ATCommand_GetNextArgumentString(pEventArguments, rcvdEvent->topic, ATCOMMAND_ARGUMENT_DELIM, sizeof(rcvdEvent->topic)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentEnum(pEventArguments, &(rcvdEvent->qos), Calypso_ATMQTT_QoSStrings, Calypso_ATMQTT_QoS_NumberOfValues, 5, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(rcvdEvent->retain), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(rcvdEvent->duplicate), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, (uint8_t*)&(rcvdEvent->dataFormat), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(rcvdEvent->dataLength), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (ATCommand_GetNextArgumentString(pEventArguments, rcvdEvent->data, ATCOMMAND_STRING_TERMINATE, sizeof(rcvdEvent->data)))
    {
        if (rcvdEvent->dataFormat == Calypso_DataFormat_Base64)
        {
            /* Decode the base64 encoded data */
            uint32_t elen = CALYPSO_LINE_MAX_SIZE;
            char out[CALYPSO_LINE_MAX_SIZE + 1];
            if (!Base64_Decode((uint8_t*)rcvdEvent->data, rcvdEvent->dataLength, (uint8_t*)out, &elen))
            {
                return false;
            }
            /* add string termination character needed by the Calypso functions */
            out[elen] = '\0';

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
bool Calypso_ATEvent_ParseNetappIP4AcquiredEvent(char** pEventArguments, Calypso_ATEvent_NetappIP4Acquired_t* ipv4Event)
{
    if (!ATCommand_GetNextArgumentString(pEventArguments, ipv4Event->address, ATCOMMAND_ARGUMENT_DELIM, sizeof(ipv4Event->address)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(pEventArguments, ipv4Event->gateway, ATCOMMAND_ARGUMENT_DELIM, sizeof(ipv4Event->gateway)))
    {
        return false;
    }
    return ATCommand_GetNextArgumentString(pEventArguments, ipv4Event->DNS, ATCOMMAND_STRING_TERMINATE, sizeof(ipv4Event->DNS));
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
bool Calypso_ATEvent_ParseMQTTConnackEvent(char** pEventArguments, Calypso_ATEvent_MQTTConnack_t* connackEvent)
{
    if (!pEventArguments || !connackEvent)
    {
        return false;
    }

    uint16_t value2;

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &value2, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
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
 * @param[in] maxIdLength Max. length of ID (including string termination character).
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATEvent_ParseHttpGetEvent(char** pEventArguments, char* id, uint16_t maxIdLength) { return ATCommand_GetNextArgumentString(pEventArguments, id, ATCOMMAND_STRING_TERMINATE, maxIdLength); }

/**
 * @brief Parses the values of the file list entry event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] fileListEntry The parsed file list entry data
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATEvent_ParseFileListEntryEvent(char** pEventArguments, Calypso_ATFile_FileListEntry_t* fileListEntry) { return Calypso_ATFile_ParseFileListEntry(pEventArguments, fileListEntry); }

/**
 * @brief Parses the values of the custom GPIO event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] gpioId ID of the GPIO that has been configured via the module's REST API.
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATEvent_ParseCustomGPIOEvent(char** pEventArguments, uint8_t* gpioId) { return ATCommand_GetNextArgumentInt(pEventArguments, gpioId, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE); }

/**
 * @brief Parses the values of the custom HTTP POST event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] id HTTP POST event id
 * @param[out] value HTTP POST event value
 * @param[in] maxIdLength Max. length of ID (including string termination character)
 * @param[in] maxValueLength Max. length of value (including string termination character)
 *
 * @return true if parsed successfully, false otherwise
 */
bool Calypso_ATEvent_ParseCustomHTTPPostEvent(char** pEventArguments, char* id, char* value, uint16_t maxIdLength, uint16_t maxValueLength)
{
    if (!ATCommand_GetNextArgumentString(pEventArguments, id, ATCOMMAND_ARGUMENT_DELIM, maxIdLength))
    {
        return false;
    }
    return ATCommand_GetNextArgumentString(pEventArguments, value, ATCOMMAND_STRING_TERMINATE, maxValueLength);
}
