/**
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
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
#include <CordeliaI/ATCommands/ATEvent.h>
#include <CordeliaI/CordeliaI.h>
#include <global/ATCommands.h>

static const ATCommand_Event_t generalSubEvents[] = {EVENTENTRY("reset_request", CordeliaI_ATEvent_GeneralResetRequest) LASTEVENTENTRY("error", CordeliaI_ATEvent_GeneralError)};

static const ATCommand_Event_t wlanSubEvents[] = {EVENTENTRY("connect", CordeliaI_ATEvent_WlanConnect) EVENTENTRY("disconnect", CordeliaI_ATEvent_WlanDisconnect) EVENTENTRY("sta_added", CordeliaI_ATEvent_WlanStaAdded) LASTEVENTENTRY("sta_removed", CordeliaI_ATEvent_WlanStaRemoved)};

static const ATCommand_Event_t socketSubEvents[] = {EVENTENTRY("tx_failed", CordeliaI_ATEvent_SocketTxFailed) LASTEVENTENTRY("async_event", CordeliaI_ATEvent_SocketAsyncEvent)};

static const ATCommand_Event_t netAppSubEvents[] = {LASTEVENTENTRY("ipv4_acquired", CordeliaI_ATEvent_NetappIP4Acquired)};

static const ATCommand_Event_t mqttSubEvents[] = {EVENTENTRY("info", CordeliaI_ATEvent_MQTTInfo) EVENTENTRY("recv", CordeliaI_ATEvent_MQTTRecv) LASTEVENTENTRY("error", CordeliaI_ATEvent_MQTTError)};

static const ATCommand_Event_t iotSubEvents[] = {EVENTENTRY("info", CordeliaI_ATEvent_IoTInfo) LASTEVENTENTRY("error", CordeliaI_ATEvent_IoTError)};

static const ATCommand_Event_t fatalErrorSubEvents[] = {EVENTENTRY("device_abort", CordeliaI_ATEvent_FatalErrorDeviceAbort) EVENTENTRY("driver_abort", CordeliaI_ATEvent_FatalErrorDriverAbort) EVENTENTRY("sync_loss", CordeliaI_ATEvent_FatalErrorSyncLost) EVENTENTRY("no_cmd_ack", CordeliaI_ATEvent_FatalErrorNoCmdAck) LASTEVENTENTRY("cmd_timout", CordeliaI_ATEvent_FatalErrorCmdTimeout)};

static const ATCommand_Event_t moduleMainEvents[] = {PARENTEVENTENTRY("+eventgeneral", generalSubEvents, ATCOMMAND_ARGUMENT_DELIM) PARENTEVENTENTRY("+eventwlan", wlanSubEvents, ATCOMMAND_ARGUMENT_DELIM) PARENTEVENTENTRY("+eventsock", socketSubEvents, ATCOMMAND_ARGUMENT_DELIM) PARENTEVENTENTRY("+eventnetapp", netAppSubEvents, ATCOMMAND_ARGUMENT_DELIM)
                                                         PARENTEVENTENTRY("+eventmqtt", mqttSubEvents, ATCOMMAND_ARGUMENT_DELIM) PARENTEVENTENTRY("+eventfatalerror", fatalErrorSubEvents, ATCOMMAND_ARGUMENT_DELIM) PARENTEVENTENTRY("+eventiot", iotSubEvents, ATCOMMAND_ARGUMENT_DELIM) LASTEVENTENTRY("+eventstartup", CordeliaI_ATEvent_Startup)};

/**
 * @brief Parses the received AT command and returns the corresponding CordeliaI_ATEvent_t.
 *
 * @param[in,out] pAtCommand AT command starting with '+'
 * @param[out] pEvent CordeliaI_ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
bool CordeliaI_ATEvent_ParseEventType(char** pAtCommand, CordeliaI_ATEvent_t* pEvent)
{
    char delimiters[] = {ATCOMMAND_EVENT_DELIM, ATCOMMAND_STRING_TERMINATE};

    if (!ATCommand_ParseEventType(pAtCommand, moduleMainEvents, delimiters, sizeof(delimiters), (uint16_t*)pEvent))
    {
        *pEvent = CordeliaI_ATEvent_Invalid;
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the startup event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] CordeliaI_Examples_startupEvent The parsed startup event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool CordeliaI_ATEvent_ParseStartUpEvent(char** pEventArguments, CordeliaI_ATEvent_Startup_t* CordeliaI_Examples_startupEvent)
{

    if (!ATCommand_GetNextArgumentString(pEventArguments, CordeliaI_Examples_startupEvent->articleNr, ATCOMMAND_ARGUMENT_DELIM, sizeof(CordeliaI_Examples_startupEvent->articleNr)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, CordeliaI_Examples_startupEvent->chipID, ATCOMMAND_ARGUMENT_DELIM, sizeof(CordeliaI_Examples_startupEvent->chipID)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, CordeliaI_Examples_startupEvent->MACAddress, ATCOMMAND_ARGUMENT_DELIM, sizeof(CordeliaI_Examples_startupEvent->MACAddress)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(CordeliaI_Examples_startupEvent->firmwareVersion[0]), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(CordeliaI_Examples_startupEvent->firmwareVersion[1]), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, '.'))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(CordeliaI_Examples_startupEvent->firmwareVersion[2]), ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the IoT event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command.
 * @param[out] CordeliaI_Examples_IoTEvent The parsed IoT event data.
 *
 * @return true if parsed successfully, false otherwise.
 */
bool CordeliaI_ATEvent_ParseIoTEvent(char** pEventArguments, CordeliaI_ATEvent_IoT_t* CordeliaI_Examples_IoTEvent)
{
    if ((pEventArguments == NULL) || (CordeliaI_Examples_IoTEvent == NULL))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, CordeliaI_Examples_IoTEvent->statusMessage, ATCOMMAND_ARGUMENT_DELIM, sizeof(CordeliaI_Examples_IoTEvent->statusMessage)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(CordeliaI_Examples_IoTEvent->statusCode), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the MQTT event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command.
 * @param[out] CordeliaI_Examples_MQTTEvent The parsed MQTT event data.
 *
 * @return true if parsed successfully, false otherwise.
 */
bool CordeliaI_ATEvent_ParseMQTTEvent(char** pEventArguments, CordeliaI_ATEvent_MQTT_t* CordeliaI_Examples_MQTTEvent)
{
    if ((pEventArguments == NULL) || (CordeliaI_Examples_MQTTEvent == NULL))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, CordeliaI_Examples_MQTTEvent->statusMessage, ATCOMMAND_ARGUMENT_DELIM, sizeof(CordeliaI_Examples_MQTTEvent->statusMessage)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentInt(pEventArguments, &(CordeliaI_Examples_MQTTEvent->statusCode), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the MQTT data received event.
 *
 * @param[in,out] pEventArguments String containing arguments of the event
 * @param[out] rcvdEvent The parsed mqtt received data from event
 *
 * @return true if parsed successfully, false otherwise
 */
bool CordeliaI_ATEvent_ParseSocketMQTTRcvdEvent(char** pEventArguments, CordeliaI_ATEvent_MQTTRecv_t* CordeliaI_Examples_MQTTRecvEvent)
{
    if ((pEventArguments == NULL) || (CordeliaI_Examples_MQTTRecvEvent == NULL))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, CordeliaI_Examples_MQTTRecvEvent->topic, ATCOMMAND_ARGUMENT_DELIM, sizeof(CordeliaI_Examples_MQTTRecvEvent->topic)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, CordeliaI_Examples_MQTTRecvEvent->qos, ATCOMMAND_ARGUMENT_DELIM, sizeof(CordeliaI_Examples_MQTTRecvEvent->qos)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(pEventArguments, CordeliaI_Examples_MQTTRecvEvent->message, ATCOMMAND_STRING_TERMINATE, sizeof(CordeliaI_Examples_MQTTRecvEvent->message)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Parses the values of the IPv4 acquired event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] ipv4Event The parsed IPv4 acquired event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool CordeliaI_ATEvent_ParseNetappIP4AcquiredEvent(char** pEventArguments, CordeliaI_ATEvent_NetappIP4Acquired_t* ipv4Event)
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
