/*
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
 */

/**
 * @file
 * @brief CordeliaI driver source file.
 */
#include <CordeliaI/ATCommands/ATDevice.h>
#include <CordeliaI/ATCommands/ATEvent.h>
#include <CordeliaI/CordeliaI.h>
#include <global/global.h>
#include <stdio.h>
#include <stdlib.h>

static void CordeliaI_HandleRxByte(uint8_t* dataP, size_t size);
static void CordeliaI_HandleRxLine(char* rxPacket, uint16_t rxLength);

/**
 * @brief Timeouts for responses to AT commands (milliseconds).
 * Initialization is done in CordeliaI_Init().
 */
static uint32_t CordeliaI_timeouts[CordeliaI_Timeout_NumberOfValues] = {0};

/**
 * @brief Is set to true when sending an AT command and is reset to false when the response has been received.
 */
static bool CordeliaI_requestPending = false;

/**
 * @brief Name of the command last sent using CordeliaI_SendRequest() (without prefix "AT+").
 */
static char CordeliaI_pendingCommandName[64] = {0};

/**
 * @brief Length of CordeliaI_pendingCommandName.
 */
static size_t CordeliaI_pendingCommandNameLength = 0;

/**
 * @brief Buffer used for current response text.
 */
static char CordeliaI_currentResponseText[CORDELIAI_MAX_RESPONSE_TEXT_LENGTH];

/**
 * @brief Length of text in current response text buffer.
 * @see CordeliaI_currentResponseText
 */
static size_t CordeliaI_currentResponseLength = 0;

/**
 * @brief Last error text (if any).
 */
static char CordeliaI_lastErrorText[32] = {0};

/**
 * @brief Last error code (if any).
 */
static int32_t CordeliaI_lastErrorCode = 0;

/**
 * @brief Confirmation status of the current (last issued) command.
 */
static CordeliaI_CNFStatus_t CordeliaI_cmdConfirmStatus;

/**
 * @brief Data buffer for received data.
 */
static char CordeliaI_rxBuffer[CORDELIAI_LINE_MAX_SIZE];

/**
 * @brief Number of bytes in receive buffer.
 * @see CordeliaI_rxBuffer
 */
static uint16_t CordeliaI_rxByteCounter = 0;

/**
 * @brief Is set to true when the first EOL character (default: carriage return)
 * has been found in the current response text.
 */
static bool CordeliaI_eolChar1Found = false;

/**
 * @brief First EOL character expected for responses received from CordeliaI.
 * @see CordeliaI_eolChar2, CordeliaI_twoEolCharacters, CordeliaI_SetEolCharacters()
 */
static uint8_t CordeliaI_eolChar1 = '\r';

/**
 * @brief Second EOL character expected for responses received from CordeliaI.
 * Only applicable if CordeliaI_twoEolCharacters is true.
 * @see CordeliaI_eolChar1, CordeliaI_twoEolCharacters, CordeliaI_SetEolCharacters()
 */
static uint8_t CordeliaI_eolChar2 = '\n';

/**
 * @brief Controls whether a line of text received from CordeliaI is considered complete after one or two EOL characters.
 * @see CordeliaI_eolChar1, CordeliaI_eolChar2, CordeliaI_SetEolCharacters()
 */
static bool CordeliaI_twoEolCharacters = true;

/**
 * @brief Time step (microseconds) when waiting for responses from CordeliaI.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see CordeliaI_SetTimingParameters
 */
static uint32_t CordeliaI_waitTimeStepUsec = 5 * 1000;

/**
 * @brief Minimum interval (microseconds) between subsequent commands sent to CordeliaI
 * (more precisely the minimum interval between the confirmation of the previous command
 * and the sending of the following command).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see CordeliaI_SetTimingParameters
 */
static uint32_t CordeliaI_minCommandIntervalUsec = 3 * 1000;

/**
 * @brief Time (microseconds) of last confirmation received from CordeliaI (if any).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 */
static uint32_t CordeliaI_lastConfirmTimeUsec = 0;

/**
 * @brief Callback function for events.
 */
CordeliaI_EventCallback_t CordeliaI_eventCallback;

/**
 * @bief Is set to true if currently executing a (custom) event handler.
 */
static bool CordeliaI_executingEventCallback = false;

/**
 * @brief Callback function which is executed if a single byte has been received from CordeliaI.
 * The default callback is CordeliaI_HandleRxByte().
 * @see CordeliaI_SetByteRxCallback()
 */
static WE_UART_HandleRxByte_t byteRxCallback = NULL;

/**
 * @brief Optional callback function which is executed if a line is received from CordeliaI.
 * Can be used to intercept responses from CordeliaI.
 * @see CordeliaI_SetLineRxCallback()
 */
CordeliaI_LineRxCallback_t CordeliaI_lineRxCallback = NULL;

/**
 * @brief Pin configuration struct pointer.
 */
static CordeliaI_Pins_t* CordeliaI_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* CordeliaI_uartP = NULL;

/**
 * @brief Initializes the serial communication with the module
 *
 * @param[in] uartP:          definition of the uart connected to the module
 * @param[in] pinoutP:        definition of the gpios connected to the module
 * @param[in] eventCallback  Function pointer to event handler (optional)

 * @return true if successful, false otherwise
 */
bool CordeliaI_Init(WE_UART_t* uartP, CordeliaI_Pins_t* pinoutP, CordeliaI_EventCallback_t eventCallback)
{
    CordeliaI_requestPending = false;

    /* Callbacks */
    byteRxCallback = CordeliaI_HandleRxByte;
    CordeliaI_lineRxCallback = NULL;
    CordeliaI_eventCallback = eventCallback;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    CordeliaI_pinsP = pinoutP;
    CordeliaI_pinsP->CordeliaI_Pin_Reset.type = WE_Pin_Type_Output;
    CordeliaI_pinsP->CordeliaI_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    CordeliaI_pinsP->CordeliaI_Pin_WakeUp.type = WE_Pin_Type_Output;
    CordeliaI_pinsP->CordeliaI_Pin_WakeUp.initial_value.output = WE_Pin_Level_Low;
    CordeliaI_pinsP->CordeliaI_Pin_Boot.type = WE_Pin_Type_Output;
    CordeliaI_pinsP->CordeliaI_Pin_Boot.initial_value.output = WE_Pin_Level_Low;
    CordeliaI_pinsP->CordeliaI_Pin_AppMode0.type = WE_Pin_Type_Output;
    CordeliaI_pinsP->CordeliaI_Pin_AppMode0.initial_value.output = WE_Pin_Level_Low;
    CordeliaI_pinsP->CordeliaI_Pin_AppMode1.type = WE_Pin_Type_Output;
    CordeliaI_pinsP->CordeliaI_Pin_AppMode1.initial_value.output = WE_Pin_Level_Low;
    CordeliaI_pinsP->CordeliaI_Pin_StatusInd0.type = WE_Pin_Type_Input;
    CordeliaI_pinsP->CordeliaI_Pin_StatusInd0.initial_value.input_pull = WE_Pin_PullType_No;
    CordeliaI_pinsP->CordeliaI_Pin_StatusInd1.type = WE_Pin_Type_Input;
    CordeliaI_pinsP->CordeliaI_Pin_StatusInd1.initial_value.input_pull = WE_Pin_PullType_No;

    WE_Pin_t pins[sizeof(CordeliaI_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_WakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_AppMode0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_AppMode1, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_StatusInd0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_StatusInd1, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    CordeliaI_uartP = uartP;
    if (false == CordeliaI_uartP->uartInit(CordeliaI_uartP->baudrate, CordeliaI_uartP->flowControl, CordeliaI_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    /* Set response timeouts */
    CordeliaI_timeouts[CordeliaI_Timeout_General] = 1000;
    CordeliaI_timeouts[CordeliaI_Timeout_FactoryReset] = 100000;
    CordeliaI_timeouts[CordeliaI_Timeout_WlanAddProfile] = 5000;
    CordeliaI_timeouts[CordeliaI_Timeout_WlanScan] = 1000;
    CordeliaI_timeouts[CordeliaI_Timeout_FileIO] = 2000;
    CordeliaI_timeouts[CordeliaI_Timeout_OTAVersion] = 7000;

    return true;
}

/**
 * @brief Deinitializes the serial communication with the module.
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_Deinit(void)
{
    WE_Pin_t pins[sizeof(CordeliaI_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_WakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_AppMode0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_AppMode1, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_StatusInd0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &CordeliaI_pinsP->CordeliaI_Pin_StatusInd1, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    CordeliaI_eventCallback = NULL;

    CordeliaI_rxByteCounter = 0;
    CordeliaI_eolChar1Found = 0;
    CordeliaI_requestPending = false;
    CordeliaI_currentResponseLength = 0;

    return CordeliaI_uartP->uartDeinit();
}

/**
 * @brief Sets the CordeliaI's application mode pins APP_MODE_0 and APP_MODE_1.
 *
 * @param[in] appMode Application mode to set
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_SetApplicationModePins(CordeliaI_ApplicationMode_t appMode)
{
    if (!WE_SetPin(CordeliaI_pinsP->CordeliaI_Pin_AppMode0, (0 != (appMode & 0x01)) ? WE_Pin_Level_High : WE_Pin_Level_Low))
    {
        return false;
    }
    return WE_SetPin(CordeliaI_pinsP->CordeliaI_Pin_AppMode1, (0 != (appMode & 0x02)) ? WE_Pin_Level_High : WE_Pin_Level_Low);
}

/**
 * @brief Performs a reset of the module using the reset pin.
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_PinReset(void)
{
    if (!WE_SetPin(CordeliaI_pinsP->CordeliaI_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(3000);
    return WE_SetPin(CordeliaI_pinsP->CordeliaI_Pin_Reset, WE_Pin_Level_High);
}

/**
 * @brief Wakes the module up from power save mode using the wake up pin.
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_PinWakeUp(void)
{
    if (!WE_SetPin(CordeliaI_pinsP->CordeliaI_Pin_WakeUp, WE_Pin_Level_High))
    {
        return false;
    }
    WE_Delay(5);
    return WE_SetPin(CordeliaI_pinsP->CordeliaI_Pin_WakeUp, WE_Pin_Level_Low);
}

/**
 * @brief Gets the pin level
 *
 * @param[in] pin: the pin to be checked
 *
 * @param[out] pin_levelP: the pin level
 *
 * @return true if request succeeded,
 *         false otherwise
 *
 */
bool CordeliaI_GetPinLevel(WE_Pin_t pin, WE_Pin_Level_t* pin_levelP) { return WE_GetPinLevel(pin, pin_levelP); }

/**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data AT command to send. Note that the command has to end with "\r\n\0".
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_SendRequest(char* data)
{
    if (CordeliaI_executingEventCallback)
    {
        /* Don't allow sending AT commands from event handlers, as this will
		 * mess up send/receive states and buffers. */
        return false;
    }

    CordeliaI_requestPending = true;
    CordeliaI_currentResponseLength = 0;
    *CordeliaI_lastErrorText = '\0';
    CordeliaI_lastErrorCode = 0;

    /* Make sure that the time between the last confirmation received from the module
	 * and the next command sent to the module is not shorter than CordeliaI_minCommandIntervalUsec */
    uint32_t t = WE_GetTickMicroseconds() - CordeliaI_lastConfirmTimeUsec;
    if (t < CordeliaI_minCommandIntervalUsec)
    {
        WE_DelayMicroseconds(CordeliaI_minCommandIntervalUsec - t);
    }

    size_t dataLength = strlen(data);

    /* Get command name from request string (remove prefix "AT+" and parameters) */
    CordeliaI_pendingCommandName[0] = '\0';
    CordeliaI_pendingCommandNameLength = 0;
    if (dataLength > 3 && (data[0] == 'a' || data[0] == 'A') && (data[1] == 't' || data[1] == 'T') && data[2] == '+')
    {
        char* pData = data + 3;
        char delimiters[] = {ATCOMMAND_COMMAND_DELIM, '\r'};
        if (ATCommand_GetCmdName(&pData, CordeliaI_pendingCommandName, sizeof(CordeliaI_pendingCommandName), delimiters, sizeof(delimiters)))
        {
            CordeliaI_pendingCommandNameLength = strlen(CordeliaI_pendingCommandName);
        }
    }

    WE_DEBUG_PRINT_DEBUG("> %s", data);

    CordeliaI_Transparent_Transmit(data, dataLength);

    return true;
}

/**
 * @brief Sends raw data to CordeliaI via UART.
 *
 * This function sends data immediately without any processing and is used
 * internally for sending AT commands to CordeliaI.
 *
 * @param[in] data Pointer to data buffer (data to be sent)
 * @param[in] dataLength Number of bytes to be sent
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_Transparent_Transmit(const char* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }
    return CordeliaI_uartP->uartTransmit((uint8_t*)data, dataLength);
}

/**
 * @brief Waits for the response from the module after a request.
 *
 * @param[in] maxTimeMs Maximum wait time in milliseconds
 * @param[in] expectedStatus Status to wait for
 * @param[out] pOutResponse Received response text (if any) will be written to this buffer (optional)
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_WaitForConfirm(uint32_t maxTimeMs, CordeliaI_CNFStatus_t expectedStatus, char* pOutResponse)
{
    CordeliaI_cmdConfirmStatus = CordeliaI_CNFStatus_Invalid;

    uint32_t t0 = WE_GetTick();

    while (1)
    {
        if (CordeliaI_CNFStatus_Invalid != CordeliaI_cmdConfirmStatus)
        {
            /* Store current time to enable check for min. time between received confirm and next command. */
            CordeliaI_lastConfirmTimeUsec = WE_GetTickMicroseconds();

            CordeliaI_requestPending = false;

            if (CordeliaI_cmdConfirmStatus == expectedStatus)
            {
                if (NULL != pOutResponse)
                {
                    /* Copy response for further processing */
                    memcpy(pOutResponse, CordeliaI_currentResponseText, CordeliaI_currentResponseLength);
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

        if (CordeliaI_waitTimeStepUsec > 0)
        {
            WE_DelayMicroseconds(CordeliaI_waitTimeStepUsec);
        }
    }

    CordeliaI_requestPending = false;
    return false;
}

/**
 * @brief Returns the code of the last error (if any).
 *
 * @param[out] lastErrorText Text of last error (if any). See CordeliaI_lastErrorText for max. buffer size.
 *
 * @return Last error code (if any)
 */
int32_t CordeliaI_GetLastError(char* lastErrorText)
{
    if (NULL != lastErrorText)
    {
        strcpy(lastErrorText, CordeliaI_lastErrorText);
    }
    return CordeliaI_lastErrorCode;
}

/**
 * @brief Set timing parameters used by the CordeliaI driver.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] waitTimeStepUsec Time step (microseconds) when waiting for responses from CordeliaI.
 * @param[in] minCommandIntervalUsec Minimum interval (microseconds) between subsequent commands sent to CordeliaI.
 *
 * @return true if successful, false otherwise
 */
bool CordeliaI_SetTimingParameters(uint32_t waitTimeStepUsec, uint32_t minCommandIntervalUsec)
{
    CordeliaI_waitTimeStepUsec = waitTimeStepUsec;
    CordeliaI_minCommandIntervalUsec = minCommandIntervalUsec;
    return true;
}

/**
 * @brief Sets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 * @param[in] timeout Timeout in milliseconds
 */
void CordeliaI_SetTimeout(CordeliaI_Timeout_t type, uint32_t timeout) { CordeliaI_timeouts[type] = timeout; }

/**
 * @brief Gets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 *
 * @return Timeout in milliseconds
 */
uint32_t CordeliaI_GetTimeout(CordeliaI_Timeout_t type) { return CordeliaI_timeouts[type]; }

/**
 * @brief Default byte received callback.
 *
 * Is called when a single byte has been received.
 *
 * @param[in] receivedByte The received byte.
 */
static void CordeliaI_HandleRxByte(uint8_t* dataP, size_t size)
{
    uint8_t receivedByte;
    for (; size > 0; size--, dataP++)
    {
        receivedByte = *dataP;

        /* Interpret received byte */
        if (CordeliaI_rxByteCounter == 0)
        {
            /* This is the first character - i.e. the start of a new line */
            /* Possible responses: OK, Error, +[event], +[cmdResponse] */
            if (('O' == receivedByte) || ('o' == receivedByte) || ('E' == receivedByte) || ('e' == receivedByte) || ('+' == receivedByte))
            {
                CordeliaI_rxBuffer[CordeliaI_rxByteCounter] = receivedByte;
                CordeliaI_rxByteCounter++;
            }
        }
        else
        {
            if (CordeliaI_rxByteCounter >= CORDELIAI_LINE_MAX_SIZE)
            {
                CordeliaI_rxByteCounter = 0;
                CordeliaI_eolChar1Found = false;
                return;
            }

            if (receivedByte == CordeliaI_eolChar1)
            {
                CordeliaI_eolChar1Found = true;

                if (!CordeliaI_twoEolCharacters)
                {
                    /* Interpret it now */
                    CordeliaI_rxBuffer[CordeliaI_rxByteCounter] = '\0';
                    CordeliaI_rxByteCounter++;
                    CordeliaI_HandleRxLine(CordeliaI_rxBuffer, CordeliaI_rxByteCounter);
                    CordeliaI_eolChar1Found = false;
                    CordeliaI_rxByteCounter = 0;
                }
            }
            else if (CordeliaI_eolChar1Found)
            {
                if (receivedByte == CordeliaI_eolChar2)
                {
                    /* Interpret it now */
                    CordeliaI_rxBuffer[CordeliaI_rxByteCounter] = '\0';
                    CordeliaI_rxByteCounter++;
                    CordeliaI_HandleRxLine(CordeliaI_rxBuffer, CordeliaI_rxByteCounter);
                    CordeliaI_eolChar1Found = false;
                    CordeliaI_rxByteCounter = 0;
                }
            }
            else
            {
                CordeliaI_rxBuffer[CordeliaI_rxByteCounter++] = receivedByte;
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
static void CordeliaI_HandleRxLine(char* rxPacket, uint16_t rxLength)
{
    WE_DEBUG_PRINT_DEBUG("< %s\r\n", rxPacket);

    /* Check if a custom line rx callback is specified and call it if so */
    if (NULL != CordeliaI_lineRxCallback)
    {
        /* Custom callback returns true if the line has been handled */
        bool handled = CordeliaI_lineRxCallback(rxPacket, rxLength);
        if (handled)
        {
            return;
        }
    }
    if (CordeliaI_requestPending)
    {
        /* AT command was sent to module. Waiting for response. */

        /* If starts with 'O', check if response is "OK\r\n" */
        if (('O' == rxPacket[0]) || ('o' == rxPacket[0]))
        {
            if (0 == strncasecmp(&rxPacket[0], ATCOMMAND_RESPONSE_OK, strlen(ATCOMMAND_RESPONSE_OK)))
            {
                CordeliaI_cmdConfirmStatus = CordeliaI_CNFStatus_Success;
            }
            else
            {
                CordeliaI_cmdConfirmStatus = CordeliaI_CNFStatus_Failed;
            }
        }
        /* If starts with 'E', check if response is "Error:[arguments]\r\n" */
        else if (('E' == rxPacket[0]) || ('e' == rxPacket[0]))
        {
            if (!(0 == strncasecmp(&rxPacket[0], ATCOMMAND_RESPONSE_ERROR, strlen(ATCOMMAND_RESPONSE_ERROR))))
            {

                CordeliaI_cmdConfirmStatus = CordeliaI_CNFStatus_Success;
            }
            else
            {
                char* packetPtr = rxPacket;
                ATCommand_GetNextArgumentString(&packetPtr, CordeliaI_lastErrorText, ':', sizeof(CordeliaI_lastErrorText));

                while (*packetPtr == ' ')
                {
                    packetPtr++;
                }

                ATCommand_GetNextArgumentString(&packetPtr, CordeliaI_lastErrorText, ATCOMMAND_ARGUMENT_DELIM, sizeof(CordeliaI_lastErrorText));
                ATCommand_GetNextArgumentInt(&packetPtr, &CordeliaI_lastErrorCode, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_SIZE32, ATCOMMAND_STRING_TERMINATE);

                CordeliaI_cmdConfirmStatus = CordeliaI_CNFStatus_Failed;
            }
        }
        else
        {
            /* Doesn't start with o or e - copy to response text buffer, if the start
			 * of the response matches the pending command name preceded by '+' */
            if (rxLength < CORDELIAI_LINE_MAX_SIZE && rxLength > 1 && CordeliaI_rxBuffer[0] == '+' && CordeliaI_pendingCommandName[0] != '\0' && 0 == strncasecmp(CordeliaI_pendingCommandName, CordeliaI_rxBuffer + 1, CordeliaI_pendingCommandNameLength))
            {
                /* Copy to response text buffer, taking care not to exceed buffer size */
                uint16_t chunkLength = rxLength;
                if (CordeliaI_currentResponseLength + chunkLength >= CORDELIAI_MAX_RESPONSE_TEXT_LENGTH)
                {
                    chunkLength = CORDELIAI_MAX_RESPONSE_TEXT_LENGTH - CordeliaI_currentResponseLength;
                }
                memcpy(&CordeliaI_currentResponseText[CordeliaI_currentResponseLength], CordeliaI_rxBuffer, chunkLength);
                CordeliaI_currentResponseLength += chunkLength;
            }
        }
    }

    if ('+' == rxPacket[0])
    {
        /* An event occurred. Execute callback (if specified). */
        if (NULL != CordeliaI_eventCallback)
        {
            CordeliaI_executingEventCallback = true;
            CordeliaI_eventCallback(CordeliaI_rxBuffer);
            CordeliaI_executingEventCallback = false;
        }
    }
}

/**
 * @brief Sets the callback function which is executed if a byte has been received from CordeliaI.
 *
 * The default callback is CordeliaI_HandleRxByte().
 *
 * @param[in] callback Pointer to byte received callback function (default callback is used if NULL)
 */
void CordeliaI_SetByteRxCallback(WE_UART_HandleRxByte_t callback) { byteRxCallback = (callback == NULL) ? CordeliaI_HandleRxByte : callback; }

/**
 * @brief Sets an optional callback function which is executed if a line has been received from CordeliaI.
 * Can be used to intercept responses from CordeliaI.
 *
 * The callback function must return true if the line should not be processed by the driver
 * and false if the driver should process the line as usual.
 *
 * @param[in] callback Pointer to line received callback function
 */
void CordeliaI_SetLineRxCallback(CordeliaI_LineRxCallback_t callback) { CordeliaI_lineRxCallback = callback; }

/**
 * @brief Sets EOL character(s) used for interpreting responses from CordeliaI.
 *
 * @param[in] eol1 First EOL character
 * @param[in] eol2 Second EOL character (is only used if twoEolCharacters is true)
 * @param[in] twoEolCharacters Controls whether the two EOL characters eol1 and eol2 (true) or only eol1 (false) is used
 */
void CordeliaI_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters)
{
    CordeliaI_eolChar1 = eol1;
    CordeliaI_eolChar2 = eol2;
    CordeliaI_twoEolCharacters = twoEolCharacters;
}
