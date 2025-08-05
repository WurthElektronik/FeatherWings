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
 * @brief StephanoI driver source file.
 */

#include <StephanoI/StephanoI.h>
#include <global/ATCommands.h>
#include <global/global.h>
#include <stdio.h>
#include <stdlib.h>

static void StephanoI_HandleRxByte(uint8_t* dataP, size_t size);
static void StephanoI_HandleRxLine(char* rxPacket, uint16_t rxLength);
static WE_UART_HandleRxByte_t byteRxCallback = StephanoI_HandleRxByte;

/**
 * @brief Timeouts for responses to AT commands (milliseconds).
 * Initialization is done in StephanoI_Init().
 */
static uint32_t StephanoI_timeouts[StephanoI_Timeout_NumberOfValues] = {0};
/**
 * @brief Is set to true when sending an AT command and is reset to false when the response has been received.
 */
static bool StephanoI_requestPending = false;

/**
 * @brief Name of the command last sent using StephanoI_SendRequest() (without prefix "AT+").
 */
static char StephanoI_pendingCommandName[64] = {0};

/**
 * @brief Length of StephanoI_pendingCommandName.
 */
static size_t StephanoI_pendingCommandNameLength = 0;

/**
 * @brief Buffer used for current response text.
 */
static char* StephanoI_currentResponseText = NULL;
static size_t StephanoI_currentResponseLength = 0;
static size_t StephanoI_currentResponseAvailableLength = 0;

/**
 * @brief Confirmation status of the current (last issued) command.
 */
static StephanoI_CNFStatus_t StephanoI_cmdConfirmStatus;

/**
 * @brief Data buffer for received data.
 */
static char StephanoI_rxBuffer[STEPHANOI_LINE_MAX_SIZE];

/**
 * @brief Number of bytes in receive buffer.
 * @see StephanoI_rxBuffer
 */
static uint16_t StephanoI_rxByteCounter = 0;

/**
 * @brief Is set to true when the first EOL character (default: carriage return)
 * has been found in the current response text.
 */
static bool StephanoI_eolChar1Found = false;

/**
 * @brief First EOL character expected for responses received from StephanoI.
 * @see StephanoI_eolChar2, StephanoI_twoEolCharacters, StephanoI_SetEolCharacters()
 */
static uint8_t StephanoI_eolChar1 = '\r';

/**
 * @brief Second EOL character expected for responses received from StephanoI.
 * Only applicable if StephanoI_twoEolCharacters is true.
 * @see StephanoI_eolChar1, StephanoI_twoEolCharacters, StephanoI_SetEolCharacters()
 */
static uint8_t StephanoI_eolChar2 = '\n';

/**
 * @brief Controls whether a line of text received from StephanoI is considered complete after one or two EOL characters.
 * @see StephanoI_eolChar1, StephanoI_eolChar2, StephanoI_SetEolCharacters()
 */
static bool StephanoI_twoEolCharacters = true;

/**
 * @brief Pin configuration struct pointer.
 */
static StephanoI_Pins_t* StephanoI_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* StephanoI_uartP = NULL;

/**
 * @brief Time step (microseconds) when waiting for responses from StephanoI.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see StephanoI_SetTimingParameters
 */
static uint32_t StephanoI_waitTimeStepUsec = 5 * 1000;

/**
 * @brief Minimum interval (microseconds) between subsequent commands sent to StephanoI
 * (more precisely the minimum interval between the confirmation of the previous command
 * and the sending of the following command).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see StephanoI_SetTimingParameters
 */
static uint32_t StephanoI_minCommandIntervalUsec = 3 * 1000;

/**
 * @brief Time (microseconds) of last confirmation received from StephanoI (if any).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 */
static uint32_t StephanoI_lastConfirmTimeUsec = 0;

/**
 * @brief Callback function for events.
 */
StephanoI_EventCallback_t StephanoI_eventCallback;

/**
 * @bief Is set to true if currently executing a (custom) event handler.
 */
static bool StephanoI_executingEventCallback = false;

/**
 * @brief Initializes the serial communication with the module
 *
 * @param[in] uartP:         definition of the uart connected to the module
 * @param[in] pinoutP:        definition of the gpios connected to the module
 * @param[in] eventCallback  Function pointer to event handler (optional)

 * @return true if successful, false otherwise
 */
bool StephanoI_Init(WE_UART_t* uartP, StephanoI_Pins_t* pinoutP, StephanoI_EventCallback_t eventCallback)
{
    StephanoI_requestPending = false;

    /* Callbacks */
    StephanoI_eventCallback = eventCallback;

    if (pinoutP == NULL)
    {
        return false;
    }
    else if (uartP == NULL)
    {
        return false;
    }
    else if (uartP->uartInit == NULL)
    {
        return false;
    }
    else if (uartP->uartDeinit == NULL)
    {
        return false;
    }
    else if (uartP->uartTransmit == NULL)
    {
        return false;
    }

    /* initialize the pins */
    StephanoI_pinsP = pinoutP;
    StephanoI_pinsP->StephanoI_Pin_Reset.type = WE_Pin_Type_Output;
    StephanoI_pinsP->StephanoI_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    StephanoI_pinsP->StephanoI_Pin_Wakeup.type = WE_Pin_Type_Output;
    StephanoI_pinsP->StephanoI_Pin_Wakeup.initial_value.output = WE_Pin_Level_Low;

    WE_Pin_t pins[sizeof(StephanoI_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &StephanoI_pinsP->StephanoI_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &StephanoI_pinsP->StephanoI_Pin_Wakeup, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    StephanoI_uartP = uartP;
    if (false == StephanoI_uartP->uartInit(StephanoI_uartP->baudrate, StephanoI_uartP->flowControl, StephanoI_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    /* Set response timeouts */
    StephanoI_timeouts[StephanoI_Timeout_General] = 1000;
    StephanoI_timeouts[StephanoI_Timeout_FactoryReset] = 5000;
    StephanoI_timeouts[StephanoI_Timeout_WifiScan] = 5000;
    StephanoI_timeouts[StephanoI_Timeout_WifiConnect] = 10000;
    StephanoI_timeouts[StephanoI_Timeout_SocketOpen] = 2500;
    StephanoI_timeouts[StephanoI_Timeout_SocketPing] = 5000;
    StephanoI_timeouts[StephanoI_Timeout_MQTTGeneral] = 10000;
    StephanoI_timeouts[StephanoI_Timeout_MQTTConnect] = 15000;
    StephanoI_timeouts[StephanoI_Timeout_HTTPGeneral] = 1000;
    StephanoI_timeouts[StephanoI_Timeout_HTTPGetPost] = 10000;

    return true;
}

/**
 * @brief Deinitializes the serial communication with the module.
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_Deinit(void)
{
    WE_Pin_t pins[sizeof(StephanoI_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &StephanoI_pinsP->StephanoI_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &StephanoI_pinsP->StephanoI_Pin_Wakeup, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    StephanoI_eventCallback = NULL;

    StephanoI_rxByteCounter = 0;
    StephanoI_eolChar1Found = 0;
    StephanoI_requestPending = false;
    StephanoI_currentResponseLength = 0;

    return StephanoI_uartP->uartDeinit();
}

/**
 * @brief Performs a reset of the module using the reset pin.
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_PinReset(void)
{
    if (!WE_SetPin(StephanoI_pinsP->StephanoI_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(5);
    return WE_SetPin(StephanoI_pinsP->StephanoI_Pin_Reset, WE_Pin_Level_High);
}

/**
 * @brief Sets the wake-up pin low or high
 *
 * @param[in] high If 'true' then high, low otherwise
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_SetWakeUpPin(bool high) { return WE_SetPin(StephanoI_pinsP->StephanoI_Pin_Wakeup, high ? WE_Pin_Level_High : WE_Pin_Level_Low); }

bool StephanoI_Transparent_Transmit(const uint8_t* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }
    return StephanoI_uartP->uartTransmit((uint8_t*)data, dataLength);
}

/**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data AT command to send. Note that the command has to end with "\r\n\0".
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_SendRequest(char* data) { return StephanoI_SendRequest_ex((uint8_t*)data, strlen(data)); }

/**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data AT command to send. Note that the command has to end with "\r\n\0".
 * @param[in] dataLength Length of the command 
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_SendRequest_ex(uint8_t* data, size_t dataLength)
{
    if (StephanoI_executingEventCallback)
    {
        /* Don't allow sending AT commands from event handlers, as this will
		 * mess up send/receive states and buffers. */
        return false;
    }

    StephanoI_requestPending = true;
    StephanoI_currentResponseLength = 0;

    /* Make sure that the time between the last confirmation received from the module
	 * and the next command sent to the module is not shorter than StephanoI_minCommandIntervalUsec */
    uint32_t t = WE_GetTickMicroseconds() - StephanoI_lastConfirmTimeUsec;
    if (t < StephanoI_minCommandIntervalUsec)
    {
        WE_DelayMicroseconds(StephanoI_minCommandIntervalUsec - t);
    }

    /* Get command name from request string (remove prefix "AT+" and parameters) */
    StephanoI_pendingCommandName[0] = '\0';
    StephanoI_pendingCommandNameLength = 0;
    if (dataLength > 3 && (data[0] == 'a' || data[0] == 'A') && (data[1] == 't' || data[1] == 'T') && data[2] == '+')
    {
        char* pData = (char*)(data + 3);
        char delimiters[] = {ATCOMMAND_COMMAND_DELIM, '?', '\r'};
        if (ATCommand_GetCmdName(&pData, StephanoI_pendingCommandName, sizeof(StephanoI_pendingCommandName), delimiters, sizeof(delimiters)))
        {
            StephanoI_pendingCommandNameLength = strlen(StephanoI_pendingCommandName);
        }
    }

    WE_DEBUG_PRINT_DEBUG("> %.*s", dataLength, data);
    if ((data[dataLength - 2] != '\r') && (data[dataLength - 1] != '\n'))
    {
        WE_DEBUG_PRINT_DEBUG("\r\n");
    }

    StephanoI_Transparent_Transmit(data, dataLength);

    return true;
}

/**
 * @brief Waits for the response from the module after a request.
 *
 * @param[in]  maxTimeMs Maximum wait time in milliseconds
 * @param[in]  expectedStatus Status to wait for
 * @param[out] pOutResponse Received response text (if any) will be written to this buffer (optional)
 * @param[in]  responseSize Size of the provided response buffer
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_WaitForConfirm_ex(uint32_t maxTimeMs, StephanoI_CNFStatus_t expectedStatus, char* pOutResponse, uint16_t responseSize)
{
    StephanoI_cmdConfirmStatus = StephanoI_CNFStatus_Invalid;

    if ((NULL != pOutResponse) && (responseSize != 0))
    {
        StephanoI_currentResponseText = pOutResponse;
        StephanoI_currentResponseAvailableLength = responseSize;
        StephanoI_currentResponseLength = 0;
    }
    else
    {
        StephanoI_currentResponseText = NULL;
        StephanoI_currentResponseAvailableLength = 0;
        StephanoI_currentResponseLength = 0;
    }

    uint32_t t0 = WE_GetTick();

    while (1)
    {
        if (StephanoI_CNFStatus_Invalid != StephanoI_cmdConfirmStatus)
        {
            /* Store current time to enable check for min. time between received confirm and next command. */
            StephanoI_lastConfirmTimeUsec = WE_GetTickMicroseconds();
            StephanoI_requestPending = false;

            bool ret = false;
            if (StephanoI_cmdConfirmStatus == expectedStatus)
            {
                if (NULL != StephanoI_currentResponseText)
                {
                    StephanoI_currentResponseText[StephanoI_currentResponseLength] = '\0';
                }

                ret = true;
            }

            StephanoI_currentResponseText = NULL;
            StephanoI_currentResponseAvailableLength = 0;
            StephanoI_currentResponseLength = 0;
            return ret;
        }

        uint32_t now = WE_GetTick();
        if (now - t0 > maxTimeMs)
        {
            /* Timeout */
            break;
        }

        if (StephanoI_waitTimeStepUsec > 0)
        {
            WE_DelayMicroseconds(StephanoI_waitTimeStepUsec);
        }
    }

    /* Timeout occurred */
    StephanoI_requestPending = false;
    StephanoI_currentResponseText = NULL;
    StephanoI_currentResponseAvailableLength = 0;
    StephanoI_currentResponseLength = 0;
    return false;
}

/**
 * @brief Waits for the response from the module.
 *
 * @param[in]  maxTimeMs Maximum wait time in milliseconds
 * @param[in]  expectedStatus Status to wait for
 *
 * @return true if successful, false otherwise
 */
inline bool StephanoI_WaitForConfirm(uint32_t maxTimeMs, StephanoI_CNFStatus_t expectedStatus) { return StephanoI_WaitForConfirm_ex(maxTimeMs, expectedStatus, NULL, 0); }

/**
 * @brief Set timing parameters used by the StephanoI driver.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] waitTimeStepUsec Time step (microseconds) when waiting for responses from StephanoI.
 * @param[in] minCommandIntervalUsec Minimum interval (microseconds) between subsequent commands sent to StephanoI.
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_SetTimingParameters(uint32_t waitTimeStepUsec, uint32_t minCommandIntervalUsec)
{
    StephanoI_waitTimeStepUsec = waitTimeStepUsec;
    StephanoI_minCommandIntervalUsec = minCommandIntervalUsec;
    return true;
}

/**
 * @brief Sets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 * @param[in] timeout Timeout in milliseconds
 */
void StephanoI_SetTimeout(StephanoI_Timeout_t type, uint32_t timeout) { StephanoI_timeouts[type] = timeout; }

/**
 * @brief Gets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 *
 * @return Timeout in milliseconds
 */
uint32_t StephanoI_GetTimeout(StephanoI_Timeout_t type) { return StephanoI_timeouts[type]; }

/**
 * @brief Default byte received callback.
 *
 * Is called when a single byte has been received.
 *
 * @param[in] receivedByte The received byte.
 */
static void StephanoI_HandleRxByte(uint8_t* dataP, size_t size)
{
    uint8_t receivedByte;
    for (; size > 0; size--, dataP++)
    {
        receivedByte = *dataP;

        /* Interpret received byte */
        if (StephanoI_rxByteCounter == 0)
        {
            if (STEPHANOI_READY4DATA_CHAR == receivedByte)
            {
                StephanoI_rxBuffer[StephanoI_rxByteCounter] = receivedByte;
                StephanoI_rxByteCounter++;
                StephanoI_rxBuffer[StephanoI_rxByteCounter] = '\0';
                StephanoI_rxByteCounter++;
                StephanoI_HandleRxLine(StephanoI_rxBuffer, StephanoI_rxByteCounter);
                StephanoI_eolChar1Found = false;
                StephanoI_rxByteCounter = 0;
            }
            else if (('\n' != receivedByte) && ('\r' != receivedByte))
            {
                StephanoI_rxBuffer[StephanoI_rxByteCounter] = receivedByte;
                StephanoI_rxByteCounter++;
            }
        }
        else
        {
            if (StephanoI_rxByteCounter >= STEPHANOI_LINE_MAX_SIZE)
            {
                StephanoI_rxByteCounter = 0;
                StephanoI_eolChar1Found = false;
                return;
            }

            if (receivedByte == StephanoI_eolChar1)
            {
                StephanoI_eolChar1Found = true;

                if (!StephanoI_twoEolCharacters)
                {
                    /* Interpret it now */
                    StephanoI_rxBuffer[StephanoI_rxByteCounter] = '\0';
                    StephanoI_rxByteCounter++;
                    StephanoI_HandleRxLine(StephanoI_rxBuffer, StephanoI_rxByteCounter);
                    StephanoI_eolChar1Found = false;
                    StephanoI_rxByteCounter = 0;
                }
            }
            else if (StephanoI_eolChar1Found)
            {
                if (receivedByte == StephanoI_eolChar2)
                {
                    /* Interpret it now */
                    StephanoI_rxBuffer[StephanoI_rxByteCounter] = '\0';
                    StephanoI_rxByteCounter++;
                    StephanoI_HandleRxLine(StephanoI_rxBuffer, StephanoI_rxByteCounter);
                    StephanoI_eolChar1Found = false;
                    StephanoI_rxByteCounter = 0;
                }
            }
            else
            {
                StephanoI_rxBuffer[StephanoI_rxByteCounter++] = receivedByte;
            }
        }
    }
}

/**
 * @brief Is called when a complete line has been received.
 *
 * @param[in] rxPacket Received text
 * @param[in] rxLength Received text length
 */
static void StephanoI_HandleRxLine(char* rxPacket, uint16_t rxLength)
{
    WE_DEBUG_PRINT_DEBUG("< %s\r\n", rxPacket);

    /* confirmations */
    if (StephanoI_requestPending)
    {
        /* AT command was sent to module. Waiting for response. */
        if (STEPHANOI_READY4DATA_CHAR == rxPacket[0])
        {
            StephanoI_cmdConfirmStatus = StephanoI_CNFStatus_Ready4Data;
        }
        else if (0 == strncmp(&rxPacket[0], ATCOMMAND_RESPONSE_OK, strlen(ATCOMMAND_RESPONSE_OK)))
        {
            StephanoI_cmdConfirmStatus = StephanoI_CNFStatus_Success;
        }
        else if (0 == strncmp(&rxPacket[0], STEPHANOI_RESPONSE_ERROR, strlen(STEPHANOI_RESPONSE_ERROR)))
        {
            StephanoI_cmdConfirmStatus = StephanoI_CNFStatus_Failed;
        }
        else if (0 == strncmp(&rxPacket[0], STEPHANOI_SEND_OK, strlen(STEPHANOI_SEND_OK)))
        {
            StephanoI_cmdConfirmStatus = StephanoI_CNFStatus_SendOK;
        }
        else if (0 == strncmp(&rxPacket[0], STEPHANOI_SET_OK, strlen(STEPHANOI_SET_OK)))
        {
            StephanoI_cmdConfirmStatus = StephanoI_CNFStatus_SetOK;
        }
        else if (0 == strncmp(&rxPacket[0], STEPHANOI_SEND_FAIL, strlen(STEPHANOI_SEND_FAIL)))
        {
            StephanoI_cmdConfirmStatus = StephanoI_CNFStatus_Failed;
        }
        else if (0 == strncmp(&rxPacket[0], STEPHANOI_SEND_CANCELLED, strlen(STEPHANOI_SEND_CANCELLED)))
        {
            StephanoI_cmdConfirmStatus = StephanoI_CNFStatus_Success; //STEPHANOI_SEND_CANCELLED is a success message when closing the data transmission
        }
        else
        {
            /* Doesn't start with o or e - copy to response text buffer, if the start
			 * of the response matches the pending command name preceded by '+' */
            if ((StephanoI_currentResponseText != NULL) && (rxLength > 1) && (StephanoI_rxBuffer[0] == '+') && (StephanoI_pendingCommandName[0] != '\0') && (0 == strncmp(StephanoI_pendingCommandName, StephanoI_rxBuffer + 1, StephanoI_pendingCommandNameLength)))
            { /* Copy to response text buffer, taking care not to exceed buffer size */
                uint16_t chunkLength = rxLength;

                /* on first copy operation remove command to only copy the arguments */
                if (StephanoI_currentResponseLength == 0)
                {
                    /* cut off command, '+', and ':' */
                    if (chunkLength > (StephanoI_pendingCommandNameLength + 2))
                    {
                        chunkLength -= (StephanoI_pendingCommandNameLength + 2);
                    }
                }

                if (StephanoI_currentResponseLength + chunkLength > StephanoI_currentResponseAvailableLength)
                {
                    /* shorten chunk length to not overflow the buffer size */
                    chunkLength = StephanoI_currentResponseAvailableLength - StephanoI_currentResponseLength;
                }

                if (StephanoI_currentResponseLength == 0)
                {
                    /* cut off command, '+', and ':' */
                    memcpy(&StephanoI_currentResponseText[StephanoI_currentResponseLength], &StephanoI_rxBuffer[1 /*+*/ + StephanoI_pendingCommandNameLength /* command name */ + 1 /*:*/], chunkLength);
                }
                else
                {
                    memcpy(&StephanoI_currentResponseText[StephanoI_currentResponseLength], &StephanoI_rxBuffer[0], chunkLength);
                }
                StephanoI_currentResponseLength += chunkLength;
            }
        }
    }

    /* indications */
    if (('+' == rxPacket[0]) || (0 == strcmp(rxPacket, "ready")) || (0 == strncmp(rxPacket, "WIFI ", 5)) || (0 == strcmp(strrchr(rxPacket, ATCOMMAND_STRING_TERMINATE) - 7, "CONNECT")) || //check last 7 chars
        (0 == strcmp(strrchr(rxPacket, ATCOMMAND_STRING_TERMINATE) - 6, "CLOSED")) ||                                                                                                      //check last 6 chars
        (0 == strncmp(rxPacket, "SEND ", 5)) || (0 == strcmp(rxPacket, "NO CERT FOUND")) || (0 == strcmp(rxPacket, "NO PRVT_KEY FOUND")) || (0 == strcmp(rxPacket, "NO CA FOUND")) || (0 == strcmp(rxPacket, "busy p...")) || (0 == strcmp(rxPacket, "Will force to restart!!!")) || (0 == strncmp(rxPacket, "ERR CODE:", 9)))
    { /* An event occurred. Execute callback (if specified). */
        if (NULL != StephanoI_eventCallback)
        {
            StephanoI_executingEventCallback = true;
            StephanoI_eventCallback(StephanoI_rxBuffer);
            StephanoI_executingEventCallback = false;
        }
    }
}

/**
 * @brief Sets EOL character(s) used for interpreting responses from StephanoI.
 *
 * @param[in] eol1 First EOL character
 * @param[in] eol2 Second EOL character (is only used if twoEolCharacters is true)
 * @param[in] twoEolCharacters Controls whether the two EOL characters eol1 and eol2 (true) or only eol1 (false) is used
 */
void StephanoI_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters)
{
    StephanoI_eolChar1 = eol1;
    StephanoI_eolChar2 = eol2;
    StephanoI_twoEolCharacters = twoEolCharacters;
}
