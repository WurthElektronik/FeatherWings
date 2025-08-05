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
 * @brief Adrastea driver source file.
 */
#include <AdrasteaI/ATCommands/ATDevice.h>
#include <AdrasteaI/ATCommands/ATEvent.h>
#include <AdrasteaI/AdrasteaI.h>
#include <global/ATCommands.h>
#include <global/global.h>
#include <stdio.h>
#include <stdlib.h>

static void AdrasteaI_HandleRxByte(uint8_t* dataP, size_t size);
static void AdrasteaI_HandleRxLine(char* rxPacket, uint16_t rxLength);
static WE_UART_HandleRxByte_t byteRxCallback = AdrasteaI_HandleRxByte;

/**
 * @brief Timeouts for responses to AT commands (milliseconds).
 * Initialization is done in AdrasteaI_Init().
 */
static uint32_t AdrasteaI_timeouts[AdrasteaI_Timeout_NumberOfValues] = {
	[AdrasteaI_Timeout_General] = 1000,
	[AdrasteaI_Timeout_Device] = 3000,
	[AdrasteaI_Timeout_GNSS] = 10000,
	[AdrasteaI_Timeout_HTTP] = 3000,
	[AdrasteaI_Timeout_MQTT] = 3000,
	[AdrasteaI_Timeout_NetService] = 10000,
	[AdrasteaI_Timeout_PacketDomain] = 3000,
	[AdrasteaI_Timeout_Proprietary] = 10000,
	[AdrasteaI_Timeout_SIM] = 3000,
	[AdrasteaI_Timeout_SMS] = 20000,
	[AdrasteaI_Timeout_Socket] = 3000,
	[AdrasteaI_Timeout_Power] = 3000,
};

#define ADRASTEAI_OPEN_MAP_CLI()		AdrasteaI_Transparent_Transmit("map\r\n", 5);

#define ADRASTEAI_RESET_ALL_CLI()		AdrasteaI_Transparent_Transmit("reset all\r\n", 11)

/**
 * @brief Is set to true when sending an AT command and is reset to false when the response has been received.
 */
static bool AdrasteaI_requestPending = false;

/**
 * @brief Struct to hold current command response expected lines to skip
 */
static AdrasteaI_Response_Complete_t AdrasteaI_responseSkip = {.delim = '\0', .lineskip = 0};

/**
 * @brief Is set to false when a command is received and response lines checked.
 */
static bool AdrasteaI_shouldCheckResponseSkip = true;

/**
 * @brief Name of the command last sent using AdrasteaI_SendRequest() (without prefix "AT+").
 */
static char AdrasteaI_pendingCommandName[64] = {0};

/**
 * @brief Length of AdrasteaI_pendingCommandName.
 */
static size_t AdrasteaI_pendingCommandNameLength = 0;

/**
 * @brief Buffer used for current response text.
 */
static char AdrasteaI_currentResponseText[ADRASTEAI_MAX_RESPONSE_TEXT_LENGTH];

/**
 * @brief Length of text in current response text buffer.
 * @see AdrasteaI_currentResponseText
 */
static size_t AdrasteaI_currentResponseLength = 0;

/**
 * @brief Confirmation status of the current (last issued) command.
 */
static AdrasteaI_CNFStatus_t AdrasteaI_cmdConfirmStatus;

/**
 * @brief Data buffer for received data.
 */
static char AdrasteaI_rxBuffer[ADRASTEAI_LINE_MAX_SIZE];

/**
 * @brief Number of bytes in receive buffer.
 * @see AdrasteaI_rxBuffer
 */
static uint16_t AdrasteaI_rxByteCounter = 0;

/**
 * @brief Is set to true when the first EOL character (default: carriage return)
 * has been found in the current response text.
 */
static bool AdrasteaI_eolChar1Found = false;

/**
 * @brief First EOL character expected for responses received from Adrastea.
 * @see AdrasteaI_eolChar2, AdrasteaI_twoEolCharacters, AdrasteaI_SetEolCharacters()
 */
static uint8_t AdrasteaI_eolChar1 = '\r';

/**
 * @brief Second EOL character expected for responses received from Adrastea.
 * Only applicable if AdrasteaI_twoEolCharacters is true.
 * @see AdrasteaI_eolChar1, AdrasteaI_twoEolCharacters, AdrasteaI_SetEolCharacters()
 */
static uint8_t AdrasteaI_eolChar2 = '\n';

/**
 * @brief Controls whether a line of text received from Adrastea is considered complete after one or two EOL characters.
 * @see AdrasteaI_eolChar1, AdrasteaI_eolChar2, AdrasteaI_SetEolCharacters()
 */
static bool AdrasteaI_twoEolCharacters = true;

/**
 * @brief Is set to true after the echoed command is received and set to false when the response is received.
 */
static bool AdrasteaI_responseincoming = false;

/**
 * @brief See description of AdrasteaI_ATMode_t
 */
static volatile AdrasteaI_ATMode_t AdrasteaI_ATMode = AdrasteaI_ATMode_Off;

/**
 * @brief Time step (microseconds) when waiting for responses from Adrastea.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see AdrasteaI_SetTimingParameters
 */
static uint32_t AdrasteaI_waitTimeStepUsec = 5 * 1000;

/**
 * @brief Minimum interval (microseconds) between subsequent commands sent to Adrastea
 * (more precisely the minimum interval between the confirmation of the previous command
 * and the sending of the following command).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @see AdrasteaI_SetTimingParameters
 */
static uint32_t AdrasteaI_minCommandIntervalUsec = 3 * 1000;

/**
 * @brief Time (microseconds) of last confirmation received from Adrastea (if any).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 */
static uint32_t AdrasteaI_lastConfirmTimeUsec = 0;

/**
 * @brief Callback function for events.
 */
AdrasteaI_EventCallback_t AdrasteaI_eventCallback;

/**
 * @bief Is set to true if currently executing a (custom) event handler.
 */
static bool AdrasteaI_executingEventCallback = false;

/**
 * @brief Number of optional parameters delimiters so far (used by at commands).
 *
 */
uint8_t AdrasteaI_optionalParamsDelimCount = 0;

/**
 * @brief bool to indicate if the MCU is currently in the process of waking up ffrom sleep.
 *
 */
static bool AdrasteaI_wakingUp = false;

/**
 * @brief Major firmware version of the driver.
 */
uint8_t AdrasteaI_firmwareVersionMajor = 6;

/**
 * @brief Minor firmware version of the driver.
 */
uint16_t AdrasteaI_firmwareVersionMinor = 6;

/**
 * @brief Pin configuration struct pointer.
 */
static AdrasteaI_Pins_t* AdrasteaI_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* AdrasteaI_uartP = NULL;

/**
 * @brief Initializes the serial communication with the module
 *
 * @param[in] uartP:          definition of the uart connected to the module
 * @param[in] pinoutP:        definition of the gpios connected to the module
 * @param[in] eventCallback  Function pointer to event handler (optional)

 * @return true if successful, false otherwise
 */
bool AdrasteaI_Init(WE_UART_t* uartP, AdrasteaI_Pins_t* pinoutP, AdrasteaI_EventCallback_t eventCallback)
{
    AdrasteaI_requestPending = false;

    /* Callbacks */
    AdrasteaI_eventCallback = eventCallback;

    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    AdrasteaI_pinsP = pinoutP;
    AdrasteaI_pinsP->AdrasteaI_Pin_Reset.type = WE_Pin_Type_Output;
    AdrasteaI_pinsP->AdrasteaI_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    AdrasteaI_pinsP->AdrasteaI_Pin_WakeUp.type = WE_Pin_Type_Output;
    AdrasteaI_pinsP->AdrasteaI_Pin_WakeUp.initial_value.output = WE_Pin_Level_Low;

    WE_Pin_t pins[sizeof(AdrasteaI_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &AdrasteaI_pinsP->AdrasteaI_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &AdrasteaI_pinsP->AdrasteaI_Pin_WakeUp, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    AdrasteaI_uartP = uartP;
    if (false == AdrasteaI_uartP->uartInit(AdrasteaI_uartP->baudrate, AdrasteaI_uartP->flowControl, AdrasteaI_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    /* reset module */
    if(IS_WE_PIN_UNDEFINED(AdrasteaI_pinsP->AdrasteaI_Pin_Reset))
    {
    	/* To clear the buffer of the module */
    	(void)AdrasteaI_ATDevice_Test();

		if(!AdrasteaI_ATDevice_Reset())
		{
			ADRASTEAI_RESET_ALL_CLI();
		}
    }
    else
    {
    	if(!AdrasteaI_PinReset())
    	{
			WE_DEBUG_PRINT_INFO("Pin reset failed\r\n");
			AdrasteaI_Deinit();
			return false;
    	}
    }

	WE_Delay(10);

	while (AdrasteaI_CheckATMode() != AdrasteaI_ATMode_Ready)
	{
		WE_Delay(10);
	}


    AdrasteaI_ATDevice_Revision_Identity_t revisionIdentity;
    if (!AdrasteaI_ATDevice_RequestRevisionIdentity(&revisionIdentity))
    {
        return false;
    }

    //check if firmware of connected module matches version of the driver.
    if (revisionIdentity.major != AdrasteaI_firmwareVersionMajor || revisionIdentity.minor != AdrasteaI_firmwareVersionMinor)
    {
        WE_DEBUG_PRINT_INFO("Driver version and module firmware version mismatch, driver might not behave as expected\r\n");
    }

    return true;
}

/**
 * @brief Deinitializes the serial communication with the module.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_Deinit(void)
{

    WE_Pin_t pins[sizeof(AdrasteaI_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &AdrasteaI_pinsP->AdrasteaI_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &AdrasteaI_pinsP->AdrasteaI_Pin_WakeUp, sizeof(WE_Pin_t));

    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    AdrasteaI_eventCallback = NULL;

    AdrasteaI_rxByteCounter = 0;
    AdrasteaI_eolChar1Found = 0;
    AdrasteaI_requestPending = false;
    AdrasteaI_currentResponseLength = 0;

    return AdrasteaI_uartP->uartDeinit();
}

/**
 * @brief Performs a reset of the module using the reset pin.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_PinReset(void)
{
    if (!WE_SetPin(AdrasteaI_pinsP->AdrasteaI_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }
    WE_Delay(100);
    return WE_SetPin(AdrasteaI_pinsP->AdrasteaI_Pin_Reset, WE_Pin_Level_High);
}

/**
 * @brief Wakes the module up from power save mode using the wake up pin.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_PinWakeUp(void)
{
    AdrasteaI_wakingUp = true;
    if (!WE_SetPin(AdrasteaI_pinsP->AdrasteaI_Pin_WakeUp, WE_Pin_Level_High))
    {
        return false;
    }

    WE_Delay(100);

    if (!WE_SetPin(AdrasteaI_pinsP->AdrasteaI_Pin_WakeUp, WE_Pin_Level_Low))
    {
        return false;
    }

    if (!AdrasteaI_SendRequest("sleepSet disable\r\n"))
    {
        return false;
    }

    if (!AdrasteaI_WaitForConfirm(AdrasteaI_GetTimeout(AdrasteaI_Timeout_Power), AdrasteaI_CNFStatus_Success, NULL))
    {
        return false;
    }

    ADRASTEAI_OPEN_MAP_CLI();
    AdrasteaI_wakingUp = false;
    return true;
}

/**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data AT command to send. Note that the command has to end with "\r\n\0".
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_SendRequest(char* data)
{
    if (AdrasteaI_executingEventCallback)
    {
        /* Don't allow sending AT commands from event handlers, as this will
		 * mess up send/receive states and buffers. */
        return false;
    }

    AdrasteaI_requestPending = true;
    AdrasteaI_currentResponseLength = 0;

    /* Make sure that the time between the last confirmation received from the module
	 * and the next command sent to the module is not shorter than AdrasteaI_minCommandIntervalUsec */
    uint32_t t = WE_GetTickMicroseconds() - AdrasteaI_lastConfirmTimeUsec;
    if (t < AdrasteaI_minCommandIntervalUsec)
    {
        WE_DelayMicroseconds(AdrasteaI_minCommandIntervalUsec - t);
    }

    size_t dataLength = strlen(data);

    char delimiters[] = {ATCOMMAND_COMMAND_DELIM, '?', '\r'};

    /* Get command name from request string (remove prefix "AT+" and parameters) */
    AdrasteaI_pendingCommandName[0] = '\0';
    AdrasteaI_pendingCommandNameLength = 0;
    if (dataLength > 2 && data[0] == 'A' && data[1] == 'T')
    {
        char* pData;
        if (dataLength > 3 && (data[2] == '+' || data[2] == '%'))
        {
            pData = data + 3;
        }
        else
        {
            pData = data + 2;
        }
        if (ATCommand_GetCmdName(&pData, AdrasteaI_pendingCommandName, sizeof(AdrasteaI_pendingCommandName), delimiters, sizeof(delimiters)))
        {
            AdrasteaI_pendingCommandNameLength = strlen(AdrasteaI_pendingCommandName);
        }
    }

    WE_DEBUG_PRINT_DEBUG("> %s", data);

    AdrasteaI_Transparent_Transmit(data, dataLength);

    return true;
}

/**
 * @brief Sends raw data to Adrastea via UART.
 *
 * This function sends data immediately without any processing and is used
 * internally for sending AT commands to Adrastea.
 *
 * @param[in] data Pointer to data buffer (data to be sent)
 * @param[in] dataLength Number of bytes to be sent
 */
bool AdrasteaI_Transparent_Transmit(const char* data, uint16_t dataLength)
{
    if ((data == NULL) || (dataLength == 0))
    {
        return false;
    }
    return AdrasteaI_uartP->uartTransmit((uint8_t*)data, dataLength);
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
bool AdrasteaI_WaitForConfirm(uint32_t maxTimeMs, AdrasteaI_CNFStatus_t expectedStatus, char* pOutResponse)
{
    AdrasteaI_cmdConfirmStatus = AdrasteaI_CNFStatus_Invalid;

    uint32_t t0 = WE_GetTick();

    while (1)
    {
        if (AdrasteaI_CNFStatus_Invalid != AdrasteaI_cmdConfirmStatus)
        {
            /* Store current time to enable check for min. time between received confirm and next command. */
            AdrasteaI_lastConfirmTimeUsec = WE_GetTickMicroseconds();
            AdrasteaI_requestPending = false;
            AdrasteaI_responseincoming = false;
            if (AdrasteaI_cmdConfirmStatus == expectedStatus)
            {
                if (NULL != pOutResponse)
                {
                    /* Copy response for further processing */
                    memcpy(pOutResponse, AdrasteaI_currentResponseText, AdrasteaI_currentResponseLength);
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

        if (AdrasteaI_waitTimeStepUsec > 0)
        {
            WE_DelayMicroseconds(AdrasteaI_waitTimeStepUsec);
        }
    }

    AdrasteaI_responseincoming = false;
    AdrasteaI_requestPending = false;
    return false;
}

/**
 * @brief Check if the response of the command is expected to be more the one line and fill AdrasteaI_responseSkip accordingly.
 */
void AdrasteaI_CheckResponseComplete()
{
    AdrasteaI_responseSkip.lineskip = 0;
    AdrasteaI_responseSkip.delim = '\0';

    if (('+' != AdrasteaI_rxBuffer[0]) && ('%' != AdrasteaI_rxBuffer[0]))
    {
        return;
    }

    AdrasteaI_ATEvent_t event;
    char* tmp = AdrasteaI_rxBuffer;
    if (!AdrasteaI_ATEvent_ParseEventType(&tmp, &event))
    {
        return;
    }

    switch (event)
    {
        case AdrasteaI_ATEvent_Ready:
        {
            AdrasteaI_ATMode = AdrasteaI_ATMode_Ready;
        }
        break;
        case AdrasteaI_ATEvent_MQTT_Publication_Received:
        {
            AdrasteaI_responseSkip.lineskip = 1;
            AdrasteaI_responseSkip.delim = ',';
        }
        break;
        case AdrasteaI_ATEvent_SMS_Read_Message:
        {
            AdrasteaI_responseSkip.lineskip = 1;
            AdrasteaI_responseSkip.delim = ',';
        }
        break;
        case AdrasteaI_ATEvent_SMS_List_Messages:
        {
            AdrasteaI_responseSkip.lineskip = 1;
            AdrasteaI_responseSkip.delim = ',';
        }
        break;
        default:
            break;
    }
}

/**
 * @brief Set timing parameters used by the Adrastea driver.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] waitTimeStepUsec Time step (microseconds) when waiting for responses from Adrastea.
 * @param[in] minCommandIntervalUsec Minimum interval (microseconds) between subsequent commands sent to Adrastea.
 *
 * @return true if successful, false otherwise
 */
bool AdrasteaI_SetTimingParameters(uint32_t waitTimeStepUsec, uint32_t minCommandIntervalUsec)
{
    AdrasteaI_waitTimeStepUsec = waitTimeStepUsec;
    AdrasteaI_minCommandIntervalUsec = minCommandIntervalUsec;
    return true;
}

/**
 * @brief Sets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 * @param[in] timeout Timeout in milliseconds
 */
void AdrasteaI_SetTimeout(AdrasteaI_Timeout_t type, uint32_t timeout) { AdrasteaI_timeouts[type] = timeout; }

/**
 * @brief Gets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 *
 * @return Timeout in milliseconds
 */
uint32_t AdrasteaI_GetTimeout(AdrasteaI_Timeout_t type) { return AdrasteaI_timeouts[type]; }

/**
 * @brief Default byte received callback.
 *
 * Is called when a single byte has been received.
 *
 * @param[in] receivedByte The received byte.
 */
static void AdrasteaI_HandleRxByte(uint8_t* dataP, size_t size)
{
    uint8_t receivedByte;
    for (; size > 0; size--, dataP++)
    {
        receivedByte = *dataP;
        if (receivedByte == AdrasteaI_eolChar1 && AdrasteaI_rxByteCounter == 0)
        {
            continue;
        }

        if (receivedByte == AdrasteaI_eolChar2 && !AdrasteaI_eolChar1Found)
        {
            continue;
        }

        if (AdrasteaI_rxByteCounter >= ADRASTEAI_LINE_MAX_SIZE)
        {
            AdrasteaI_rxByteCounter = 0;
            AdrasteaI_eolChar1Found = false;
            return;
        }

        if (receivedByte == AdrasteaI_eolChar1)
        {
            AdrasteaI_eolChar1Found = true;

            if (AdrasteaI_shouldCheckResponseSkip)
            {
                AdrasteaI_CheckResponseComplete();
                AdrasteaI_shouldCheckResponseSkip = false;
            }

            if (!AdrasteaI_twoEolCharacters)
            {
                if (AdrasteaI_responseSkip.lineskip == 0)
                {
                    AdrasteaI_rxBuffer[AdrasteaI_rxByteCounter] = '\0';
                    AdrasteaI_rxByteCounter++;
                    AdrasteaI_HandleRxLine(AdrasteaI_rxBuffer, AdrasteaI_rxByteCounter);
                    AdrasteaI_eolChar1Found = false;
                    AdrasteaI_rxByteCounter = 0;
                    AdrasteaI_shouldCheckResponseSkip = true;
                }
                else
                {
                    AdrasteaI_responseSkip.lineskip -= 1;
                    AdrasteaI_eolChar1Found = false;
                    AdrasteaI_rxBuffer[AdrasteaI_rxByteCounter] = AdrasteaI_responseSkip.delim;
                    AdrasteaI_rxByteCounter++;
                }
            }
        }
        else if (AdrasteaI_eolChar1Found)
        {
            if (receivedByte == AdrasteaI_eolChar2)
            {
                if (AdrasteaI_responseSkip.lineskip == 0)
                {
                    /* Interpret it now */
                    AdrasteaI_rxBuffer[AdrasteaI_rxByteCounter] = '\0';
                    AdrasteaI_rxByteCounter++;
                    AdrasteaI_HandleRxLine(AdrasteaI_rxBuffer, AdrasteaI_rxByteCounter);
                    AdrasteaI_eolChar1Found = false;
                    AdrasteaI_rxByteCounter = 0;
                    AdrasteaI_shouldCheckResponseSkip = true;
                }
                else
                {
                    AdrasteaI_responseSkip.lineskip -= 1;
                    AdrasteaI_eolChar1Found = false;
                    AdrasteaI_rxBuffer[AdrasteaI_rxByteCounter] = AdrasteaI_responseSkip.delim;
                    AdrasteaI_rxByteCounter++;
                }
            }
        }
        else
        {
            AdrasteaI_rxBuffer[AdrasteaI_rxByteCounter++] = receivedByte;
        }
    }
}

/**
 * @brief Is called when a complete line has been received.
 *
 * @param[in] rxPacket Received text
 * @param[in] rxLength Received text length
 */
static void AdrasteaI_HandleRxLine(char* rxPacket, uint16_t rxLength)
{
    WE_DEBUG_PRINT_DEBUG("< %s\r\n", rxPacket);

    if (AdrasteaI_requestPending)
    {
        if (0 == strncmp(&rxPacket[0], ADRASTEAI_RESPONSE_OK, strlen(ADRASTEAI_RESPONSE_OK)))
        {
            AdrasteaI_responseincoming = false;
            AdrasteaI_cmdConfirmStatus = AdrasteaI_CNFStatus_Success;
        }
        else if (0 == strncmp(&rxPacket[0], ADRASTEAI_RESPONSE_ERROR, strlen(ADRASTEAI_RESPONSE_ERROR)))
        {
            AdrasteaI_responseincoming = false;
            AdrasteaI_cmdConfirmStatus = AdrasteaI_CNFStatus_Failed;
        }
        else if (0 == strncmp(&rxPacket[0], ADRASTEAI_SMS_ERROR, strlen(ADRASTEAI_SMS_ERROR)))
        {
            AdrasteaI_responseincoming = false;
            AdrasteaI_cmdConfirmStatus = AdrasteaI_CNFStatus_Failed;
        }
        else if (0 == strncmp(&rxPacket[0], ADRASTEAI_POWERMODECHANGE_EVENT, strlen(ADRASTEAI_POWERMODECHANGE_EVENT)))
        {
            AdrasteaI_responseincoming = false;
            AdrasteaI_cmdConfirmStatus = AdrasteaI_CNFStatus_Success;
        }
        else if (0 == strncmp(&rxPacket[0], ADRASTEAI_SLEEPSET_EVENT, strlen(ADRASTEAI_SLEEPSET_EVENT)))
        {
            AdrasteaI_responseincoming = false;
            AdrasteaI_cmdConfirmStatus = AdrasteaI_CNFStatus_Success;
        }
        else
        {
            if (AdrasteaI_responseincoming)
            {
                /* Copy to response text buffer, taking care not to exceed buffer size */
                uint16_t chunkLength = rxLength;
                bool isevent = false;

                if (AdrasteaI_currentResponseLength == 0 && rxLength > (AdrasteaI_pendingCommandNameLength + 2) && (0 == strncmp(AdrasteaI_pendingCommandName, rxPacket + 1, AdrasteaI_pendingCommandNameLength)))
                {
                    isevent = true;
                    chunkLength -= (AdrasteaI_pendingCommandNameLength + 2);
                }

                if (AdrasteaI_currentResponseLength + chunkLength >= ADRASTEAI_MAX_RESPONSE_TEXT_LENGTH)
                {
                    chunkLength = ADRASTEAI_MAX_RESPONSE_TEXT_LENGTH - AdrasteaI_currentResponseLength;
                }

                if (AdrasteaI_currentResponseLength == 0)
                {
                    if (isevent)
                    {
                        memcpy(&AdrasteaI_currentResponseText[AdrasteaI_currentResponseLength], AdrasteaI_rxBuffer + AdrasteaI_pendingCommandNameLength + 2, chunkLength);
                    }
                    else
                    {
                        memcpy(&AdrasteaI_currentResponseText[AdrasteaI_currentResponseLength], AdrasteaI_rxBuffer, chunkLength);
                    }
                }
                else
                {
                    memcpy(&AdrasteaI_currentResponseText[AdrasteaI_currentResponseLength], AdrasteaI_rxBuffer, chunkLength);
                }
                AdrasteaI_currentResponseLength += chunkLength;
            }
            else if (rxLength < ADRASTEAI_LINE_MAX_SIZE && rxLength > 2 && AdrasteaI_pendingCommandName[0] != '\0' && 'A' == rxPacket[0] && 'T' == rxPacket[1] &&
                     ((('+' == rxPacket[2] || '%' == rxPacket[2]) && (0 == strncmp(AdrasteaI_pendingCommandName, AdrasteaI_rxBuffer + 3, AdrasteaI_pendingCommandNameLength))) || (0 == strncmp(AdrasteaI_pendingCommandName, AdrasteaI_rxBuffer + 2, AdrasteaI_pendingCommandNameLength))))
            {
                //if true command echo detected store response for lines after
                AdrasteaI_responseincoming = true;
            }
        }
    }

    if (0 == strncmp(&rxPacket[0], ADRASTEAI_MCU_EVENT, strlen(ADRASTEAI_MCU_EVENT)))
    {
        AdrasteaI_ATMode = AdrasteaI_ATMode_Off;
        if (!AdrasteaI_wakingUp)
        {
        	ADRASTEAI_OPEN_MAP_CLI();
        }
    }
    else if (0 == strncmp(&rxPacket[0], ADRASTEAI_MAPCLICLOSE_EVENT, strlen(ADRASTEAI_MAPCLICLOSE_EVENT)))
    {
        AdrasteaI_ATMode = AdrasteaI_ATMode_Off;
    }
    else if (0 == strncmp(&rxPacket[0], ADRASTEAI_MAPCLIOPEN_EVENT, strlen(ADRASTEAI_MAPCLIOPEN_EVENT)))
    {
        AdrasteaI_ATMode = AdrasteaI_ATMode_Map;
    }
    else if (('+' == rxPacket[0]) || ('%' == rxPacket[0]))
    {
        if (NULL != AdrasteaI_eventCallback)
        {
            /* Execute callback (if specified). */
            AdrasteaI_executingEventCallback = true;
            AdrasteaI_eventCallback(AdrasteaI_rxBuffer);
            AdrasteaI_executingEventCallback = false;
        }
    }
}

/**
 * @brief Sets EOL character(s) used for interpreting responses from Adrastea.
 *
 * @param[in] eol1 First EOL character
 * @param[in] eol2 Second EOL character (is only used if twoEolCharacters is true)
 * @param[in] twoEolCharacters Controls whether the two EOL characters eol1 and eol2 (true) or only eol1 (false) is used
 */
void AdrasteaI_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters)
{
    AdrasteaI_eolChar1 = eol1;
    AdrasteaI_eolChar2 = eol2;
    AdrasteaI_twoEolCharacters = twoEolCharacters;
}

/**
 * @brief Check if module is in at command mode
 *
 * @return boolean to indicate mode.
 */
AdrasteaI_ATMode_t AdrasteaI_CheckATMode() { return AdrasteaI_ATMode; }
