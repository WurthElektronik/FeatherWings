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
 * @brief Calypso driver source file.
 */
#include <Calypso/ATCommands/ATDevice.h>
#include <Calypso/ATCommands/ATEvent.h>
#include <Calypso/Calypso.h>
#include <global/global.h>
#include <stdio.h>
#include <stdlib.h>

static void Calypso_HandleRxByte(uint8_t* dataP, size_t size);
static void Calypso_HandleRxLine(char* rxPacket, uint16_t rxLength);

/**
 * @brief Timeouts for responses to AT commands (milliseconds).
 * Initialization is done in Calypso_Init().
 */
static uint32_t Calypso_timeouts[Calypso_Timeout_NumberOfValues] = {0};

/**
 * @brief Is set to true when sending an AT command and is reset to false when the response has been received.
 */
static bool Calypso_requestPending = false;

/**
 * @brief Name of the command last sent using Calypso_SendRequest() (without prefix "AT+").
 */
static char Calypso_pendingCommandName[64] = {0};

/**
 * @brief Length of Calypso_pendingCommandName.
 */
static size_t Calypso_pendingCommandNameLength = 0;

/**
 * @brief Buffer used for current response text.
 */
static char Calypso_currentResponseText[CALYPSO_MAX_RESPONSE_TEXT_LENGTH];

/**
 * @brief Length of text in current response text buffer.
 * @see Calypso_currentResponseText
 */
static size_t Calypso_currentResponseLength = 0;

/**
 * @brief Last error text (if any).
 */
static char Calypso_lastErrorText[32] = {0};

/**
 * @brief Last error code (if any).
 */
static int32_t Calypso_lastErrorCode = 0;

/**
 * @brief Confirmation status of the current (last issued) command.
 */
static Calypso_CNFStatus_t Calypso_cmdConfirmStatus;

/**
 * @brief Data buffer for received data.
 */
static char Calypso_rxBuffer[CALYPSO_LINE_MAX_SIZE];

/**
 * @brief Number of bytes in receive buffer.
 * @see Calypso_rxBuffer
 */
static uint16_t Calypso_rxByteCounter = 0;

/**
 * @brief Is set to true when the first EOL character (default: carriage return)
 * has been found in the current response text.
 */
static bool Calypso_eolChar1Found = false;

/**
 * @brief First EOL character expected for responses received from Calypso.
 * @see Calypso_eolChar2, Calypso_twoEolCharacters, Calypso_SetEolCharacters()
 */
static uint8_t Calypso_eolChar1 = '\r';

/**
 * @brief Second EOL character expected for responses received from Calypso.
 * Only applicable if Calypso_twoEolCharacters is true.
 * @see Calypso_eolChar1, Calypso_twoEolCharacters, Calypso_SetEolCharacters()
 */
static uint8_t Calypso_eolChar2 = '\n';

/**
 * @brief Controls whether a line of text received from Calypso is considered complete after one or two EOL characters.
 * @see Calypso_eolChar1, Calypso_eolChar2, Calypso_SetEolCharacters()
 */
static bool Calypso_twoEolCharacters = true;

/**
 * @brief Time step (microseconds) when waiting for responses from Calypso.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see Calypso_SetTimingParameters
 */
static uint32_t Calypso_waitTimeStepUsec = 5 * 1000;

/**
 * @brief Minimum interval (microseconds) between subsequent commands sent to Calypso
 * (more precisely the minimum interval between the confirmation of the previous command
 * and the sending of the following command).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see Calypso_SetTimingParameters
 */
static uint32_t Calypso_minCommandIntervalUsec = 3 * 1000;

/**
 * @brief Time (microseconds) of last confirmation received from Calypso (if any).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 */
static uint32_t Calypso_lastConfirmTimeUsec = 0;

/**
 * @brief Major firmware version of the connected module.
 *
 * Note that some functions may behave differently depending on the firmware version.
 *
 * The firmware version can be determined during runtime using
 * Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &version) - this will
 * update the variables Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
 * Calypso_firmwareVersionPatch.
 */
uint8_t Calypso_firmwareVersionMajor = 2;

/**
 * @brief Minor firmware version of the connected module.
 *
 * Note that some functions may behave differently depending on the firmware version.
 *
 * The firmware version can be determined during runtime using
 * Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &version) - this will
 * update the variables Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
 * Calypso_firmwareVersionPatch.
 */
uint8_t Calypso_firmwareVersionMinor = 0;

/**
 * @brief Patch firmware version of the connected module.
 *
 * Note that some functions may behave differently depending on the firmware version.
 *
 * The firmware version can be determined during runtime using
 * Calypso_ATDevice_Get(Calypso_ATDevice_GetId_General, Calypso_ATDevice_GetGeneral_Version, &version) - this will
 * update the variables Calypso_firmwareVersionMajor, Calypso_firmwareVersionMinor and
 * Calypso_firmwareVersionPatch.
 */
uint8_t Calypso_firmwareVersionPatch = 0;

/**
 * @brief Callback function for events.
 */
Calypso_EventCallback_t Calypso_eventCallback;

/**
 * @bief Is set to true if currently executing a (custom) event handler.
 */
static bool Calypso_executingEventCallback = false;

/**
 * @brief Callback function which is executed if a single byte has been received from Calypso.
 * The default callback is Calypso_HandleRxByte().
 * @see Calypso_SetByteRxCallback()
 */
static WE_UART_HandleRxByte_t byteRxCallback = NULL;

/**
 * @brief Optional callback function which is executed if a line is received from Calypso.
 * Can be used to intercept responses from Calypso.
 * @see Calypso_SetLineRxCallback()
 */
Calypso_LineRxCallback_t Calypso_lineRxCallback = NULL;

/**
 * @brief Pin configuration struct pointer.
 */
static Calypso_Pins_t* Calypso_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* Calypso_uartP = NULL;

/**
 * @brief Initializes the serial communication with the module
 *
 * @param[in] uartP:          definition of the uart connected to the module
 * @param[in] pinoutP:        definition of the gpios connected to the module
 * @param[in] eventCallback  Function pointer to event handler (optional)

 * @return true if successful, false otherwise
 */
bool Calypso_Init(WE_UART_t* uartP, Calypso_Pins_t* pinoutP, Calypso_EventCallback_t eventCallback)
{
    Calypso_requestPending = false;

    /* Callbacks */
    byteRxCallback = Calypso_HandleRxByte;
    Calypso_lineRxCallback = NULL;
    Calypso_eventCallback = eventCallback;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    Calypso_pinsP = pinoutP;
    Calypso_pinsP->Calypso_Pin_Reset.type = WE_Pin_Type_Output;
    Calypso_pinsP->Calypso_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    Calypso_pinsP->Calypso_Pin_WakeUp.type = WE_Pin_Type_Output;
    Calypso_pinsP->Calypso_Pin_WakeUp.initial_value.output = WE_Pin_Level_Low;
    Calypso_pinsP->Calypso_Pin_Boot.type = WE_Pin_Type_Output;
    Calypso_pinsP->Calypso_Pin_Boot.initial_value.output = WE_Pin_Level_Low;
    Calypso_pinsP->Calypso_Pin_AppMode0.type = WE_Pin_Type_Output;
    Calypso_pinsP->Calypso_Pin_AppMode0.initial_value.output = WE_Pin_Level_Low;
    Calypso_pinsP->Calypso_Pin_AppMode1.type = WE_Pin_Type_Output;
    Calypso_pinsP->Calypso_Pin_AppMode1.initial_value.output = WE_Pin_Level_Low;
    Calypso_pinsP->Calypso_Pin_StatusInd0.type = WE_Pin_Type_Input;
    Calypso_pinsP->Calypso_Pin_StatusInd0.initial_value.input_pull = WE_Pin_PullType_No;
    Calypso_pinsP->Calypso_Pin_StatusInd1.type = WE_Pin_Type_Input;
    Calypso_pinsP->Calypso_Pin_StatusInd1.initial_value.input_pull = WE_Pin_PullType_No;

    WE_Pin_t pins[sizeof(Calypso_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_WakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_AppMode0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_AppMode1, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_StatusInd0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_StatusInd1, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    Calypso_uartP = uartP;
    if (false == Calypso_uartP->uartInit(Calypso_uartP->baudrate, Calypso_uartP->flowControl, Calypso_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    /* Set response timeouts */
    Calypso_timeouts[Calypso_Timeout_General] = 1000;
    Calypso_timeouts[Calypso_Timeout_FactoryReset] = 100000;
    Calypso_timeouts[Calypso_Timeout_WlanAddProfile] = 5000;
    Calypso_timeouts[Calypso_Timeout_WlanScan] = 1000;
    Calypso_timeouts[Calypso_Timeout_NetAppUpdateTime] = 30000;
    Calypso_timeouts[Calypso_Timeout_NetAppHostLookUp] = 5000;
    Calypso_timeouts[Calypso_Timeout_HttpConnect] = 30000;
    Calypso_timeouts[Calypso_Timeout_HttpRequest] = 30000;
    Calypso_timeouts[Calypso_Timeout_FileIO] = 2000;
    Calypso_timeouts[Calypso_Timeout_GPIO] = 2000;

    return true;
}

/**
 * @brief Deinitializes the serial communication with the module.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_Deinit(void)
{
    WE_Pin_t pins[sizeof(Calypso_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_WakeUp, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_Boot, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_AppMode0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_AppMode1, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_StatusInd0, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Calypso_pinsP->Calypso_Pin_StatusInd1, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    Calypso_eventCallback = NULL;

    Calypso_rxByteCounter = 0;
    Calypso_eolChar1Found = 0;
    Calypso_requestPending = false;
    Calypso_currentResponseLength = 0;

    return Calypso_uartP->uartDeinit();
}

/**
 * @brief Sets the Calypso's application mode pins APP_MODE_0 and APP_MODE_1.
 *
 * @param[in] appMode Application mode to set
 *
 * @return true if successful, false otherwise
 */
bool Calypso_SetApplicationModePins(Calypso_ApplicationMode_t appMode)
{
    if (!WE_SetPin(Calypso_pinsP->Calypso_Pin_AppMode0, (0 != (appMode & 0x01)) ? WE_Pin_Level_High : WE_Pin_Level_Low))
    {
        return false;
    }
    return WE_SetPin(Calypso_pinsP->Calypso_Pin_AppMode1, (0 != (appMode & 0x02)) ? WE_Pin_Level_High : WE_Pin_Level_Low);
}

/**
 * @brief Performs a reset of the module using the reset pin.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_PinReset(void)
{
    if (!WE_SetPin(Calypso_pinsP->Calypso_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(5);
    return WE_SetPin(Calypso_pinsP->Calypso_Pin_Reset, WE_Pin_Level_High);
}

/**
 * @brief Wakes the module up from power save mode using the wake up pin.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_PinWakeUp(void)
{
    if (!WE_SetPin(Calypso_pinsP->Calypso_Pin_WakeUp, WE_Pin_Level_High))
    {
        return false;
    }
    WE_Delay(5);
    return WE_SetPin(Calypso_pinsP->Calypso_Pin_WakeUp, WE_Pin_Level_Low);
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
bool Calypso_GetPinLevel(WE_Pin_t pin, WE_Pin_Level_t* pin_levelP) { return WE_GetPinLevel(pin, pin_levelP); }

/**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data AT command to send. Note that the command has to end with "\r\n\0".
 *
 * @return true if successful, false otherwise
 */
bool Calypso_SendRequest(char* data)
{
    if (Calypso_executingEventCallback)
    {
        /* Don't allow sending AT commands from event handlers, as this will
		 * mess up send/receive states and buffers. */
        return false;
    }

    Calypso_requestPending = true;
    Calypso_currentResponseLength = 0;
    *Calypso_lastErrorText = '\0';
    Calypso_lastErrorCode = 0;

    /* Make sure that the time between the last confirmation received from the module
	 * and the next command sent to the module is not shorter than Calypso_minCommandIntervalUsec */
    uint32_t t = WE_GetTickMicroseconds() - Calypso_lastConfirmTimeUsec;
    if (t < Calypso_minCommandIntervalUsec)
    {
        WE_DelayMicroseconds(Calypso_minCommandIntervalUsec - t);
    }

    size_t dataLength = strlen(data);

    /* Get command name from request string (remove prefix "AT+" and parameters) */
    Calypso_pendingCommandName[0] = '\0';
    Calypso_pendingCommandNameLength = 0;
    if (dataLength > 3 && (data[0] == 'a' || data[0] == 'A') && (data[1] == 't' || data[1] == 'T') && data[2] == '+')
    {
        char* pData = data + 3;
        char delimiters[] = {ATCOMMAND_COMMAND_DELIM, '\r'};
        if (ATCommand_GetCmdName(&pData, Calypso_pendingCommandName, sizeof(Calypso_pendingCommandName), delimiters, sizeof(delimiters)))
        {
            Calypso_pendingCommandNameLength = strlen(Calypso_pendingCommandName);
        }
    }

    WE_DEBUG_PRINT_DEBUG("> %s", data);

    Calypso_Transparent_Transmit(data, dataLength);

    return true;
}

/**
 * @brief Sends raw data to Calypso via UART.
 *
 * This function sends data immediately without any processing and is used
 * internally for sending AT commands to Calypso.
 *
 * @param[in] data Pointer to data buffer (data to be sent)
 * @param[in] dataLength Number of bytes to be sent
 *
 * @return true if successful, false otherwise
 */
bool Calypso_Transparent_Transmit(const char* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }
    return Calypso_uartP->uartTransmit((uint8_t*)data, dataLength);
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
bool Calypso_WaitForConfirm(uint32_t maxTimeMs, Calypso_CNFStatus_t expectedStatus, char* pOutResponse)
{
    Calypso_cmdConfirmStatus = Calypso_CNFStatus_Invalid;

    uint32_t t0 = WE_GetTick();

    while (1)
    {
        if (Calypso_CNFStatus_Invalid != Calypso_cmdConfirmStatus)
        {
            /* Store current time to enable check for min. time between received confirm and next command. */
            Calypso_lastConfirmTimeUsec = WE_GetTickMicroseconds();

            Calypso_requestPending = false;

            if (Calypso_cmdConfirmStatus == expectedStatus)
            {
                if (NULL != pOutResponse)
                {
                    /* Copy response for further processing */
                    memcpy(pOutResponse, Calypso_currentResponseText, Calypso_currentResponseLength);
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

        if (Calypso_waitTimeStepUsec > 0)
        {
            WE_DelayMicroseconds(Calypso_waitTimeStepUsec);
        }
    }

    Calypso_requestPending = false;
    return false;
}

/**
 * @brief Returns the code of the last error (if any).
 *
 * @param[out] lastErrorText Text of last error (if any). See Calypso_lastErrorText for max. buffer size.
 *
 * @return Last error code (if any)
 */
int32_t Calypso_GetLastError(char* lastErrorText)
{
    if (NULL != lastErrorText)
    {
        strcpy(lastErrorText, Calypso_lastErrorText);
    }
    return Calypso_lastErrorCode;
}

/**
 * @brief Set timing parameters used by the Calypso driver.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] waitTimeStepUsec Time step (microseconds) when waiting for responses from Calypso.
 * @param[in] minCommandIntervalUsec Minimum interval (microseconds) between subsequent commands sent to Calypso.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_SetTimingParameters(uint32_t waitTimeStepUsec, uint32_t minCommandIntervalUsec)
{
    Calypso_waitTimeStepUsec = waitTimeStepUsec;
    Calypso_minCommandIntervalUsec = minCommandIntervalUsec;
    return true;
}

/**
 * @brief Sets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 * @param[in] timeout Timeout in milliseconds
 */
void Calypso_SetTimeout(Calypso_Timeout_t type, uint32_t timeout) { Calypso_timeouts[type] = timeout; }

/**
 * @brief Gets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 *
 * @return Timeout in milliseconds
 */
uint32_t Calypso_GetTimeout(Calypso_Timeout_t type) { return Calypso_timeouts[type]; }

/**
 * @brief Default byte received callback.
 *
 * Is called when a single byte has been received.
 *
 * @param[in] receivedByte The received byte.
 */
static void Calypso_HandleRxByte(uint8_t* dataP, size_t size)
{
    uint8_t receivedByte;
    for (; size > 0; size--, dataP++)
    {
        receivedByte = *dataP;

        /* Interpret received byte */
        if (Calypso_rxByteCounter == 0)
        {
            /* This is the first character - i.e. the start of a new line */
            /* Possible responses: OK, Error, +[event], +[cmdResponse] */
            if (('O' == receivedByte) || ('o' == receivedByte) || ('E' == receivedByte) || ('e' == receivedByte) || ('+' == receivedByte))
            {
                Calypso_rxBuffer[Calypso_rxByteCounter] = receivedByte;
                Calypso_rxByteCounter++;
            }
        }
        else
        {
            if (Calypso_rxByteCounter >= CALYPSO_LINE_MAX_SIZE)
            {
                Calypso_rxByteCounter = 0;
                Calypso_eolChar1Found = false;
                return;
            }

            if (receivedByte == Calypso_eolChar1)
            {
                Calypso_eolChar1Found = true;

                if (!Calypso_twoEolCharacters)
                {
                    /* Interpret it now */
                    Calypso_rxBuffer[Calypso_rxByteCounter] = '\0';
                    Calypso_rxByteCounter++;
                    Calypso_HandleRxLine(Calypso_rxBuffer, Calypso_rxByteCounter);
                    Calypso_eolChar1Found = false;
                    Calypso_rxByteCounter = 0;
                }
            }
            else if (Calypso_eolChar1Found)
            {
                if (receivedByte == Calypso_eolChar2)
                {
                    /* Interpret it now */
                    Calypso_rxBuffer[Calypso_rxByteCounter] = '\0';
                    Calypso_rxByteCounter++;
                    Calypso_HandleRxLine(Calypso_rxBuffer, Calypso_rxByteCounter);
                    Calypso_eolChar1Found = false;
                    Calypso_rxByteCounter = 0;
                }
            }
            else
            {
                Calypso_rxBuffer[Calypso_rxByteCounter++] = receivedByte;
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
static void Calypso_HandleRxLine(char* rxPacket, uint16_t rxLength)
{
    WE_DEBUG_PRINT_DEBUG("< %s\r\n", rxPacket);

    /* Check if a custom line rx callback is specified and call it if so */
    if (NULL != Calypso_lineRxCallback)
    {
        /* Custom callback returns true if the line has been handled */
        bool handled = Calypso_lineRxCallback(rxPacket, rxLength);
        if (handled)
        {
            return;
        }
    }
    if (Calypso_requestPending)
    {
        /* AT command was sent to module. Waiting for response. */

        /* If starts with 'O', check if response is "OK\r\n" */
        if (('O' == rxPacket[0]) || ('o' == rxPacket[0]))
        {
            if (0 == strncasecmp(&rxPacket[0], ATCOMMAND_RESPONSE_OK, strlen(ATCOMMAND_RESPONSE_OK)))
            {
                Calypso_cmdConfirmStatus = Calypso_CNFStatus_Success;
            }
            else
            {
                Calypso_cmdConfirmStatus = Calypso_CNFStatus_Failed;
            }
        }
        /* If starts with 'E', check if response is "Error:[arguments]\r\n" */
        else if (('E' == rxPacket[0]) || ('e' == rxPacket[0]))
        {
            if (!(0 == strncasecmp(&rxPacket[0], ATCOMMAND_RESPONSE_ERROR, strlen(ATCOMMAND_RESPONSE_ERROR))))
            {

                Calypso_cmdConfirmStatus = Calypso_CNFStatus_Success;
            }
            else
            {
                char* packetPtr = rxPacket;
                ATCommand_GetNextArgumentString(&packetPtr, Calypso_lastErrorText, ':', sizeof(Calypso_lastErrorText));

                while (*packetPtr == ' ')
                {
                    packetPtr++;
                }

                ATCommand_GetNextArgumentString(&packetPtr, Calypso_lastErrorText, ATCOMMAND_ARGUMENT_DELIM, sizeof(Calypso_lastErrorText));
                ATCommand_GetNextArgumentInt(&packetPtr, &Calypso_lastErrorCode, ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_SIZE32, ATCOMMAND_STRING_TERMINATE);

                Calypso_cmdConfirmStatus = Calypso_CNFStatus_Failed;
            }
        }
        else
        {
            /* Doesn't start with o or e - copy to response text buffer, if the start
			 * of the response matches the pending command name preceded by '+' */
            if (rxLength < CALYPSO_LINE_MAX_SIZE && rxLength > 1 && Calypso_rxBuffer[0] == '+' && Calypso_pendingCommandName[0] != '\0' && 0 == strncasecmp(Calypso_pendingCommandName, Calypso_rxBuffer + 1, Calypso_pendingCommandNameLength))
            {
                /* Copy to response text buffer, taking care not to exceed buffer size */
                uint16_t chunkLength = rxLength;
                if (Calypso_currentResponseLength + chunkLength >= CALYPSO_MAX_RESPONSE_TEXT_LENGTH)
                {
                    chunkLength = CALYPSO_MAX_RESPONSE_TEXT_LENGTH - Calypso_currentResponseLength;
                }
                memcpy(&Calypso_currentResponseText[Calypso_currentResponseLength], Calypso_rxBuffer, chunkLength);
                Calypso_currentResponseLength += chunkLength;
            }
        }
    }

    if ('+' == rxPacket[0])
    {
        /* An event occurred. Execute callback (if specified). */
        if (NULL != Calypso_eventCallback)
        {
            Calypso_executingEventCallback = true;
            Calypso_eventCallback(Calypso_rxBuffer);
            Calypso_executingEventCallback = false;
        }
    }
}

/**
 * @brief Sets the callback function which is executed if a byte has been received from Calypso.
 *
 * The default callback is Calypso_HandleRxByte().
 *
 * @param[in] callback Pointer to byte received callback function (default callback is used if NULL)
 */
void Calypso_SetByteRxCallback(WE_UART_HandleRxByte_t callback) { byteRxCallback = (callback == NULL) ? Calypso_HandleRxByte : callback; }

/**
 * @brief Sets an optional callback function which is executed if a line has been received from Calypso.
 * Can be used to intercept responses from Calypso.
 *
 * The callback function must return true if the line should not be processed by the driver
 * and false if the driver should process the line as usual.
 *
 * @param[in] callback Pointer to line received callback function
 */
void Calypso_SetLineRxCallback(Calypso_LineRxCallback_t callback) { Calypso_lineRxCallback = callback; }

/**
 * @brief Sets EOL character(s) used for interpreting responses from Calypso.
 *
 * @param[in] eol1 First EOL character
 * @param[in] eol2 Second EOL character (is only used if twoEolCharacters is true)
 * @param[in] twoEolCharacters Controls whether the two EOL characters eol1 and eol2 (true) or only eol1 (false) is used
 */
void Calypso_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters)
{
    Calypso_eolChar1 = eol1;
    Calypso_eolChar2 = eol2;
    Calypso_twoEolCharacters = twoEolCharacters;
}
