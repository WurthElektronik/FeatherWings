/*
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
 */

/**
 * @file
 * @brief DaphnisI driver source file.
 */

#include <DaphnisI/ATCommands/ATDevice.h>
#include <DaphnisI/DaphnisI.h>
#include <global/ATCommands.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

static void DaphnisI_HandleRxByte(uint8_t* dataP, size_t size);
static void DaphnisI_HandleRxLine(char* rxPacket, uint16_t rxLength);
static bool DaphnisI_Transparent_Transmit(const char* data, uint16_t dataLength);
static WE_UART_HandleRxByte_t byteRxCallback = DaphnisI_HandleRxByte;
static bool DaphnisI_ATEvent_ParseEventType(char** pAtCommand, DaphnisI_ATEvent_t* pEvent);

/**
 * @brief Timeouts for responses to AT commands (milliseconds).
 */
static uint32_t DaphnisI_timeouts[DaphnisI_Timeout_NumberOfValues] = {
	[DaphnisI_Timeout_General] = 1000,
#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)
    [DaphnisI_Timeout_P2P_Data] = 10000,
    [DaphnisI_Timeout_P2P_Remote_GPIO] = 3000,
#endif
};

/**
 * @brief Is set to true when sending an AT command and is reset to false when the response has been received.
 */
static bool DaphnisI_requestPending = false;

/**
 * @brief Name of the command last sent using DaphnisI_SendRequest() (without prefix "AT+").
 */
static char DaphnisI_pendingCommandName[64] = {0};

/**
 * @brief Length of DaphnisI_pendingCommandName.
 */
static size_t DaphnisI_pendingCommandNameLength = 0;

/**
 * @brief Buffer used for current response text.
 */
static char DaphnisI_currentResponseText[DAPHNISI_MAX_RESPONSE_TEXT_LENGTH];

/**
 * @brief Length of text in current response text buffer.
 * @see DaphnisI_currentResponseText
 */
static size_t DaphnisI_currentResponseLength = 0;

/**
 * @brief Last error text (if any).
 */
static DaphnisI_ErrorMessage_t DaphnisI_lastError = DaphnisI_ErrorMessage_Invalid;

/**
 * @brief Confirmation status of the current (last issued) command.
 */
static volatile DaphnisI_CNFStatus_t DaphnisI_cmdConfirmStatus;

#define EVENT_BUFFER_LENGTH 3

/**
 * @brief Last received events.
 */
static volatile DaphnisI_ATEvent_t DaphnisI_events[EVENT_BUFFER_LENGTH];

/**
 * @brief Data buffer for received data.
 */
static char DaphnisI_rxBuffer[DAPHNISI_LINE_MAX_SIZE];

/**
 * @brief Number of bytes in receive buffer.
 * @see DaphnisI_rxBuffer
 */
static uint16_t DaphnisI_rxByteCounter = 0;

/**
 * @brief Is set to true when the first EOL character (default: carriage return)
 * has been found in the current response text.
 */
static bool DaphnisI_eolChar1Found = false;

/**
 * @brief First EOL character expected for responses received from DaphnisI.
 * @see DaphnisI_eolChar2, DaphnisI_twoEolCharacters, DaphnisI_SetEolCharacters()
 */
static const uint8_t DaphnisI_eolChar1 = '\r';

/**
 * @brief Second EOL character expected for responses received from DaphnisI.
 * Only applicable if DaphnisI_twoEolCharacters is true.
 * @see DaphnisI_eolChar1, DaphnisI_twoEolCharacters, DaphnisI_SetEolCharacters()
 */
static const uint8_t DaphnisI_eolChar2 = '\n';

/**
 * @brief Controls whether a line of text received from DaphnisI is considered complete after one or two EOL characters.
 * @see DaphnisI_eolChar1, DaphnisI_eolChar2, DaphnisI_SetEolCharacters()
 */
static const bool DaphnisI_twoEolCharacters = true;

/**
 * @brief Pin configuration struct pointer.
 */
static DaphnisI_Pins_t* DaphnisI_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* DaphnisI_uartP = NULL;

/**
 * @brief Time step (microseconds) when waiting for responses from DaphnisI.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see DaphnisI_SetTimingParameters
 */
static uint32_t DaphnisI_waitTimeStepUsec = 1000;

/**
 * @brief Minimum interval (microseconds) between subsequent commands sent to DaphnisI
 * (more precisely the minimum interval between the confirmation of the previous command
 * and the sending of the following command).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see DaphnisI_SetTimingParameters
 */
static uint32_t DaphnisI_minCommandIntervalUsec = 3 * 1000;

/**
 * @brief Time (microseconds) of last confirmation received from DaphnisI (if any).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 */
static uint32_t DaphnisI_lastConfirmTimeUsec = 0;

/**
 * @brief Callback function for events.
 */
DaphnisI_EventCallback_t DaphnisI_eventCallback;

/**
 * @bief Is set to true if currently executing a (custom) event handler.
 */
static bool DaphnisI_executingEventCallback = false;

/**
 * @brief  Array corresponding to the description of each possible AT Error
 */
static const char* DaphnisI_ErrorMessage_Strings[DaphnisI_ErrorMessage_Count] = {"ERROR",        "PARAM_ERROR", "BUSY_ERROR", "TEST_PARAM_OVERFLOW", "NO_NETWORK_JOINED", "RX_ERROR", "DUTYCYCLE_RESTRICTED", "CRYPTO_ERROR",
#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)
                                                                                 "INVALID_MODE", "INVALID_ROLE"
#endif
};

static const ATCommand_Event_t LoRaWANContextSubEvents[] = {EVENTENTRY("NVM_DATA_STORED", DaphnisI_ATEvent_LoRaWAN_ContextState_Stored) LASTEVENTENTRY("NVM_DATA_RESTORED", DaphnisI_ATEvent_LoRaWAN_ContextState_Restored)};

static const ATCommand_Event_t LoRaWANJoinSubEvents[] = {EVENTENTRY("JOINED", DaphnisI_ATEvent_LoRaWAN_JoinState_Success) LASTEVENTENTRY("JOIN_FAILED", DaphnisI_ATEvent_LoRaWAN_JoinState_Fail)};

static const ATCommand_Event_t SysnotfSubEvents[] = {LASTEVENTENTRY("READY", DaphnisI_ATEvent_StartUp)};

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)
static const ATCommand_Event_t P2PTxConfSubEvents[] = {EVENTENTRY("SUCCESS", DaphnisI_ATEvent_P2P_TxConf_Success) EVENTENTRY("ERROR", DaphnisI_ATEvent_P2P_TxConf_Error) LASTEVENTENTRY("BUSY", DaphnisI_ATEvent_P2P_TxConf_Busy)};

static const ATCommand_Event_t P2PTxRespSubEvents[] = {EVENTENTRY("SENT", DaphnisI_ATEvent_P2P_TxResp_Success) EVENTENTRY("ACK", DaphnisI_ATEvent_P2P_TxResp_Success) EVENTENTRY("NACK,0", DaphnisI_ATEvent_P2P_TxResp_Fail_Timeout) EVENTENTRY("NACK,1", DaphnisI_ATEvent_P2P_TxResp_Fail_NACK_Received) LASTEVENTENTRY("NACK,2", DaphnisI_ATEvent_P2P_TxResp_Fail_Internal_Error)};
#endif

static const ATCommand_Event_t moduleMainEvents[] = {PARENTEVENTENTRY("+CS", LoRaWANContextSubEvents, ATCOMMAND_STRING_TERMINATE) PARENTEVENTENTRY("+JOIN", LoRaWANJoinSubEvents, ATCOMMAND_STRING_TERMINATE) PARENTEVENTENTRY("+SYSNOTF", SysnotfSubEvents, ATCOMMAND_STRING_TERMINATE)
#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)
                                                         PARENTEVENTENTRY("+P2PTXCONF", P2PTxConfSubEvents, ATCOMMAND_STRING_TERMINATE) EVENTENTRY("+P2PTXTIME", DaphnisI_ATEvent_P2P_TxTime) PARENTEVENTENTRY("+P2PTXRESP", P2PTxRespSubEvents, ATCOMMAND_STRING_TERMINATE) EVENTENTRY("+P2PRXDATA", DaphnisI_ATEvent_P2P_RxData)
                                                             EVENTENTRY("+P2PGPIORCFGSET", DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Set_Response) EVENTENTRY("+P2PGPIORCFGGET", DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Get_Response) EVENTENTRY("+P2PGPIORVALUESET", DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Set_Response) EVENTENTRY("+P2PGPIORVALUEGET", DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Get_Response)
                                                                 EVENTENTRY("+P2PGPIORCFGCHANGED", DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Changed) EVENTENTRY("+P2PGPIORVALUECHANGED", DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Changed)
#endif
                                                                     EVENTENTRY("+RXINFO", DaphnisI_ATEvent_LoRaWAN_RxInfo) EVENTENTRY("+RXDATA", DaphnisI_ATEvent_LoRaWAN_RxData) EVENTENTRY("+TXCONF", DaphnisI_ATEvent_LoRaWAN_TxConf) EVENTENTRY("+CLASSUPDATE", DaphnisI_ATEvent_LoRaWAN_Class) EVENTENTRY("+BEACONINFO", DaphnisI_ATEvent_LoRaWAN_BeaconInfo)
                                                                         EVENTENTRY("+BEACONLOST", DaphnisI_ATEvent_LoRaWAN_BeaconLost) LASTEVENTENTRY("+BEACONNOTRECEIVED", DaphnisI_ATEvent_LoRaWAN_BeaconNotReceived)};

/**
 * @brief Is called when a complete line has been received.
 *
 * @param[in] rxPacket Received text
 * @param[in] rxLength Received text length
 */
static void DaphnisI_HandleRxLine(char* rxPacket, uint16_t rxLength)
{
    WE_DEBUG_PRINT_DEBUG("< %s\r\n", rxPacket);

    if (DaphnisI_requestPending)
    {
        /* AT command was sent to module. Waiting for response. */

        if (DAPHNISI_RESPONSE_OK[0] == rxPacket[0])
        {
            if (0 == strncasecmp(&rxPacket[0], DAPHNISI_RESPONSE_OK, strlen(DAPHNISI_RESPONSE_OK)))
            {
                DaphnisI_cmdConfirmStatus = DaphnisI_CNFStatus_Success;
            }
            else
            {
                DaphnisI_cmdConfirmStatus = DaphnisI_CNFStatus_Failed;
            }
        }
        else if (DAPHNISI_RESPONSE_ERROR[0] == rxPacket[0])
        {
            if (!(0 == strncasecmp(&rxPacket[0], DAPHNISI_RESPONSE_ERROR, strlen(DAPHNISI_RESPONSE_ERROR))))
            {
                DaphnisI_cmdConfirmStatus = DaphnisI_CNFStatus_Success;
            }
            else
            {
                DaphnisI_cmdConfirmStatus = DaphnisI_CNFStatus_Failed;
                char* rxError = rxPacket + strlen(DAPHNISI_RESPONSE_ERROR);
                if (!ATCommand_GetNextArgumentEnum(&rxError, (uint8_t*)&DaphnisI_lastError, DaphnisI_ErrorMessage_Strings, DaphnisI_ErrorMessage_Count, 32, ATCOMMAND_STRING_TERMINATE))
                {
                    DaphnisI_lastError = DaphnisI_ErrorMessage_ERROR;
                }
            }
        }
        else
        {
            /* Copy to response text buffer, if the start
			 * of the response matches the pending command name preceded by '+' */
            if (rxLength < DAPHNISI_LINE_MAX_SIZE && rxLength > 1 && DaphnisI_rxBuffer[0] == '+' && DaphnisI_pendingCommandName[0] != '\0' && 0 == strncasecmp(DaphnisI_pendingCommandName, DaphnisI_rxBuffer + 1, DaphnisI_pendingCommandNameLength))
            {
                /* Copy to response text buffer, taking care not to exceed buffer size */
                uint16_t chunkLength = rxLength;
                if (DaphnisI_currentResponseLength + chunkLength >= DAPHNISI_MAX_RESPONSE_TEXT_LENGTH)
                {
                    chunkLength = DAPHNISI_MAX_RESPONSE_TEXT_LENGTH - DaphnisI_currentResponseLength;
                }
                memcpy(&DaphnisI_currentResponseText[DaphnisI_currentResponseLength], DaphnisI_rxBuffer, chunkLength);
                DaphnisI_currentResponseLength += chunkLength;
            }
        }
    }

    if ('+' == rxPacket[0])
    {
        DaphnisI_ATEvent_t event;
        char* bufferP = DaphnisI_rxBuffer;
        if (!DaphnisI_ATEvent_ParseEventType(&bufferP, &event))
        {
            return;
        }

        for (uint8_t i = 0; i < EVENT_BUFFER_LENGTH; i++)
        {
            if (DaphnisI_events[i] == DaphnisI_ATEvent_Invalid)
            {
                DaphnisI_events[i] = event;
                break;
            }
        }

        /* An event occurred. Execute callback (if specified). */
        if (NULL != DaphnisI_eventCallback)
        {
            DaphnisI_executingEventCallback = true;
            DaphnisI_eventCallback(event, bufferP);
            DaphnisI_executingEventCallback = false;
        }
    }
}

/**
 * @brief Default byte received callback.
 *
 * Is called when a single byte has been received.
 *
 * @param[in] receivedByte The received byte.
 */
static void DaphnisI_HandleRxByte(uint8_t* dataP, size_t size)
{
    uint8_t receivedByte;
    for (; size > 0; size--, dataP++)
    {
        receivedByte = *dataP;

        /* Interpret received byte */
        if (DaphnisI_rxByteCounter == 0)
        {
            /* This is the first character - i.e. the start of a new line */
            /* Possible responses: OK, AT_, +[event], +[cmdResponse] */
            if (('O' == receivedByte) || ('A' == receivedByte) || ('+' == receivedByte))
            {
                DaphnisI_rxBuffer[DaphnisI_rxByteCounter] = receivedByte;
                DaphnisI_rxByteCounter++;
            }
        }
        else
        {
            if (DaphnisI_rxByteCounter >= DAPHNISI_LINE_MAX_SIZE)
            {
                DaphnisI_rxByteCounter = 0;
                DaphnisI_eolChar1Found = false;
                return;
            }

            if (receivedByte == DaphnisI_eolChar1)
            {
                DaphnisI_eolChar1Found = true;

                if (!DaphnisI_twoEolCharacters)
                {
                    /* Interpret it now */
                    DaphnisI_rxBuffer[DaphnisI_rxByteCounter] = '\0';
                    DaphnisI_rxByteCounter++;
                    DaphnisI_HandleRxLine(DaphnisI_rxBuffer, DaphnisI_rxByteCounter);
                    DaphnisI_eolChar1Found = false;
                    DaphnisI_rxByteCounter = 0;
                }
            }
            else if (DaphnisI_eolChar1Found)
            {
                if (receivedByte == DaphnisI_eolChar2)
                {
                    /* Interpret it now */
                    DaphnisI_rxBuffer[DaphnisI_rxByteCounter] = '\0';
                    DaphnisI_rxByteCounter++;
                    DaphnisI_HandleRxLine(DaphnisI_rxBuffer, DaphnisI_rxByteCounter);
                    DaphnisI_eolChar1Found = false;
                    DaphnisI_rxByteCounter = 0;
                }
            }
            else
            {
                DaphnisI_rxBuffer[DaphnisI_rxByteCounter++] = receivedByte;
            }
        }
    }
}

bool DaphnisI_Init(WE_UART_t* uartP, DaphnisI_Pins_t* pinoutP, DaphnisI_EventCallback_t eventCallback)
{
    DaphnisI_requestPending = false;

    /* Callbacks */
    DaphnisI_eventCallback = eventCallback;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    /* initialize the pins */
    DaphnisI_pinsP = pinoutP;
    DaphnisI_pinsP->DaphnisI_Pin_Boot.type = WE_Pin_Type_Output;
    DaphnisI_pinsP->DaphnisI_Pin_Boot.initial_value.output = WE_Pin_Level_Low;
    DaphnisI_pinsP->DaphnisI_Pin_WakeUp.type = WE_Pin_Type_Output;
    DaphnisI_pinsP->DaphnisI_Pin_WakeUp.initial_value.output = WE_Pin_Level_Low;
    DaphnisI_pinsP->DaphnisI_Pin_Reset.type = WE_Pin_Type_Output;
    DaphnisI_pinsP->DaphnisI_Pin_Reset.initial_value.output = WE_Pin_Level_High;

    WE_Pin_t pins[sizeof(DaphnisI_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &DaphnisI_pinsP->DaphnisI_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &DaphnisI_pinsP->DaphnisI_Pin_WakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &DaphnisI_pinsP->DaphnisI_Pin_Reset, sizeof(WE_Pin_t));

    for (uint8_t i = 0; i < EVENT_BUFFER_LENGTH; i++)
    {
        DaphnisI_events[i] = DaphnisI_ATEvent_Invalid;
    }

    if (!WE_InitPins(pins, pin_count))
    {
        /* Error */
        return false;
    }

    DaphnisI_uartP = uartP;
    if (false == DaphnisI_uartP->uartInit(DaphnisI_uartP->baudrate, DaphnisI_uartP->flowControl, DaphnisI_uartP->parity, &byteRxCallback))
    {
        return false;
    }

    WE_Delay(10);

    /* reset module */
    if (!(IS_WE_PIN_UNDEFINED(DaphnisI_pinsP->DaphnisI_Pin_Reset) ? DaphnisI_Reset() : DaphnisI_PinReset()))
    {
        WE_DEBUG_PRINT_INFO("Reset failed.\r\n");
        DaphnisI_Deinit();
        return false;
    }

    DaphnisI_FW_Version_t fw;
    DaphnisI_LoRaWAN_LL_Version_t ll;
    DaphnisI_LoRaWAN_RP_Version_t rp;

    if (!DaphnisI_GetGenericVersion(&fw, &ll, &rp))
    {
        return false;
    }

    if (FW(fw.Major, fw.Minor, fw.Patch) < DAPHNISI_MIN_FW_VER)
    {
        WE_DEBUG_PRINT_INFO("Please update the module to the selected version.\r\n");
        return false;
    }

    return true;
}

bool DaphnisI_Deinit(void)
{
    WE_Pin_t pins[sizeof(DaphnisI_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &DaphnisI_pinsP->DaphnisI_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &DaphnisI_pinsP->DaphnisI_Pin_WakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &DaphnisI_pinsP->DaphnisI_Pin_Reset, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    DaphnisI_eventCallback = NULL;

    DaphnisI_rxByteCounter = 0;
    DaphnisI_eolChar1Found = 0;
    DaphnisI_requestPending = false;
    DaphnisI_currentResponseLength = 0;

    return DaphnisI_uartP->uartDeinit();
}

/**
 * @brief Sends raw data to Daphnis via UART.
 *
 * This function sends data immediately without any processing and is used
 * internally for sending AT commands to Daphnis.
 *
 * @param[in] data Pointer to data buffer (data to be sent)
 * @param[in] dataLength Number of bytes to be sent
 *
 * @return true if successful, false otherwise
 */
static bool DaphnisI_Transparent_Transmit(const char* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }
    return DaphnisI_uartP->uartTransmit((uint8_t*)data, dataLength);
}

bool DaphnisI_PinReset(void)
{
    if (!WE_SetPin(DaphnisI_pinsP->DaphnisI_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }

    WE_Delay(5);

    if (!WE_SetPin(DaphnisI_pinsP->DaphnisI_Pin_Reset, WE_Pin_Level_High))
    {
        return false;
    }

    DaphnisI_ATEvent_t startUpEvent = DaphnisI_ATEvent_StartUp;

    return (DaphnisI_WaitForEvents(DAPHNISI_STARTUP_TIME, &startUpEvent, 1, true) == DaphnisI_ATEvent_StartUp);
}

bool DaphnisI_PinWakeUp(void)
{
    if (!WE_SetPin(DaphnisI_pinsP->DaphnisI_Pin_WakeUp, WE_Pin_Level_High))
    {
        return false;
    }
    WE_Delay(5);
    return WE_SetPin(DaphnisI_pinsP->DaphnisI_Pin_WakeUp, WE_Pin_Level_Low);
}

bool DaphnisI_SetPin(WE_Pin_t pin, WE_Pin_Level_t level) { return WE_SetPin(pin, level); }

bool DaphnisI_SendRequest(char* data)
{
    if (DaphnisI_executingEventCallback)
    {
        /* Don't allow sending AT commands from event handlers, as this will
		 * mess up send/receive states and buffers. */
        return false;
    }

    DaphnisI_requestPending = true;
    DaphnisI_currentResponseLength = 0;
    DaphnisI_lastError = DaphnisI_ErrorMessage_Invalid;

    /* Make sure that the time between the last confirmation received from the module
	 * and the next command sent to the module is not shorter than DaphnisI_minCommandIntervalUsec */
    uint32_t t = WE_GetTickMicroseconds() - DaphnisI_lastConfirmTimeUsec;
    if (t < DaphnisI_minCommandIntervalUsec)
    {
        WE_DelayMicroseconds(DaphnisI_minCommandIntervalUsec - t);
    }

    size_t dataLength = strlen(data);

    /* Get command name from request string (remove prefix "AT+" and parameters) */
    DaphnisI_pendingCommandName[0] = '\0';
    DaphnisI_pendingCommandNameLength = 0;
    if (dataLength > 3 && (data[0] == 'a' || data[0] == 'A') && (data[1] == 't' || data[1] == 'T') && data[2] == '+')
    {
        char* pData = data + 3;
        char delimiters[] = {ATCOMMAND_COMMAND_DELIM, '\r'};
        if (ATCommand_GetCmdName(&pData, DaphnisI_pendingCommandName, sizeof(DaphnisI_pendingCommandName), delimiters, sizeof(delimiters)))
        {
            DaphnisI_pendingCommandNameLength = strlen(DaphnisI_pendingCommandName);
        }
    }

    WE_DEBUG_PRINT_DEBUG("> %s", data);

    DaphnisI_Transparent_Transmit(data, dataLength);

    return true;
}

bool DaphnisI_WaitForConfirm(uint32_t maxTimeMs, DaphnisI_CNFStatus_t expectedStatus, char* pOutResponse)
{
    DaphnisI_cmdConfirmStatus = DaphnisI_CNFStatus_Invalid;

    uint32_t t0 = WE_GetTick();

    while (1)
    {
        if (DaphnisI_CNFStatus_Invalid != DaphnisI_cmdConfirmStatus)
        {
            /* Store current time to enable check for min. time between received confirm and next command. */
            DaphnisI_lastConfirmTimeUsec = WE_GetTickMicroseconds();

            DaphnisI_requestPending = false;

            if (DaphnisI_cmdConfirmStatus == expectedStatus)
            {
                if (NULL != pOutResponse)
                {
                    /* Copy response for further processing */
                    memcpy(pOutResponse, DaphnisI_currentResponseText, DaphnisI_currentResponseLength);
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        uint32_t now = WE_GetTick();
        if (now - t0 > maxTimeMs)
        {
            /* Timeout */
            break;
        }

        if (DaphnisI_waitTimeStepUsec > 0)
        {
            WE_DelayMicroseconds(DaphnisI_waitTimeStepUsec);
        }
    }

    DaphnisI_requestPending = false;
    return false;
}

DaphnisI_ATEvent_t DaphnisI_WaitForEvents(uint32_t maxTimeMs, DaphnisI_ATEvent_t* expectedEvents, uint8_t eventsLength, bool resetEventsBuffer)
{

    if (resetEventsBuffer)
    {
        for (uint8_t i = 0; i < EVENT_BUFFER_LENGTH; i++)
        {
            DaphnisI_events[i] = DaphnisI_ATEvent_Invalid;
        }
    }

    uint32_t t0 = WE_GetTick();

    while (1)
    {

        for (uint8_t i = 0; i < EVENT_BUFFER_LENGTH; i++)
        {
            for (uint8_t j = 0; j < eventsLength; j++)
            {
                if (DaphnisI_events[i] == expectedEvents[j])
                {
                    return expectedEvents[j];
                }
            }
        }

        uint32_t now = WE_GetTick();
        if (now - t0 > maxTimeMs)
        {
            /* Timeout */
            break;
        }

        if (DaphnisI_waitTimeStepUsec > 0)
        {
            WE_DelayMicroseconds(DaphnisI_waitTimeStepUsec);
        }
    }

    return DaphnisI_ATEvent_Invalid;
}

bool DaphnisI_GetLastError(DaphnisI_ErrorMessage_t* lastError)
{
    if (NULL == lastError || DaphnisI_ErrorMessage_Invalid == DaphnisI_lastError)
    {
        return false;
    }

    *lastError = DaphnisI_lastError;

    return true;
}

bool DaphnisI_SetTimingParameters(uint32_t waitTimeStepUsec, uint32_t minCommandIntervalUsec)
{
    DaphnisI_waitTimeStepUsec = waitTimeStepUsec;
    DaphnisI_minCommandIntervalUsec = minCommandIntervalUsec;
    return true;
}

void DaphnisI_SetTimeout(DaphnisI_Timeout_t type, uint32_t timeout) { DaphnisI_timeouts[type] = timeout; }

uint32_t DaphnisI_GetTimeout(DaphnisI_Timeout_t type) { return DaphnisI_timeouts[type]; }

/**
 * @brief Parses the received AT command and returns the corresponding DaphnisI_ATEvent_t.
 *
 * @param[in,out] pAtCommand AT command starting with '+'
 * @param[out] pEvent DaphnisI_ATEvent_t representing the event
 *
 * @return true if parsed successfully, false otherwise
 */
static bool DaphnisI_ATEvent_ParseEventType(char** pAtCommand, DaphnisI_ATEvent_t* pEvent)
{
    char delimiters[] = {ATCOMMAND_EVENT_DELIM, ATCOMMAND_STRING_TERMINATE};

    if (!ATCommand_ParseEventType(pAtCommand, moduleMainEvents, delimiters, sizeof(delimiters), (uint16_t*)pEvent))
    {
        *pEvent = DaphnisI_ATEvent_Invalid;
        return false;
    }

    return true;
}
