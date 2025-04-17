/*
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
 */

/**
 * @file
 * @brief Adrastea driver source file.
 */

#include "Adrastea.h"

#include <stdio.h>
#include <stdlib.h>

#include "ATCommands/ATDevice.h"
#include <ATCommands.h>
#include "ATCommands/ATEvent.h"

static void Adrastea_HandleRxByte(uint8_t receivedByte);
static void Adrastea_HandleRxLine(char *rxPacket, uint16_t rxLength);

/**
 * @brief Timeouts for responses to AT commands (milliseconds).
 * Initialization is done in Adrastea_Init().
 */
static uint32_t Adrastea_timeouts[Adrastea_Timeout_NumberOfValues] = {0};

/**
 * @brief Is set to true when sending an AT command and is reset to false when
 * the response has been received.
 */
static bool Adrastea_requestPending = false;

/**
 * @brief Struct to hold current command response expected lines to skip
 */
static Adrastea_Response_Complete_t Adrastea_responseSkip = {.delim = '\0',
                                                             .lineskip = 0};

/**
 * @brief Is set to false when a command is received and response lines checked.
 */
static bool Adrastea_shouldCheckResponseSkip = true;

/**
 * @brief Name of the command last sent using Adrastea_SendRequest() (without
 * prefix "AT+").
 */
static char Adrastea_pendingCommandName[64] = {0};

/**
 * @brief Length of Adrastea_pendingCommandName.
 */
static size_t Adrastea_pendingCommandNameLength = 0;

/**
 * @brief Buffer used for current response text.
 */
static char Adrastea_currentResponseText[ADRASTEA_MAX_RESPONSE_TEXT_LENGTH];

/**
 * @brief Length of text in current response text buffer.
 * @see Adrastea_currentResponseText
 */
static size_t Adrastea_currentResponseLength = 0;

/**
 * @brief Confirmation status of the current (last issued) command.
 */
static Adrastea_CNFStatus_t Adrastea_cmdConfirmStatus;

/**
 * @brief Data buffer for received data.
 */
static char Adrastea_rxBuffer[ADRASTEA_LINE_MAX_SIZE];

/**
 * @brief Number of bytes in receive buffer.
 * @see Adrastea_rxBuffer
 */
static uint16_t Adrastea_rxByteCounter = 0;

/**
 * @brief Is set to true when the first EOL character (default: carriage return)
 * has been found in the current response text.
 */
static bool Adrastea_eolChar1Found = false;

/**
 * @brief First EOL character expected for responses received from Adrastea.
 * @see Adrastea_eolChar2, Adrastea_twoEolCharacters,
 * Adrastea_SetEolCharacters()
 */
static uint8_t Adrastea_eolChar1 = '\r';

/**
 * @brief Second EOL character expected for responses received from Adrastea.
 * Only applicable if Adrastea_twoEolCharacters is true.
 * @see Adrastea_eolChar1, Adrastea_twoEolCharacters,
 * Adrastea_SetEolCharacters()
 */
static uint8_t Adrastea_eolChar2 = '\n';

/**
 * @brief Controls whether a line of text received from Adrastea is considered
 * complete after one or two EOL characters.
 * @see Adrastea_eolChar1, Adrastea_eolChar2, Adrastea_SetEolCharacters()
 */
static bool Adrastea_twoEolCharacters = true;

/**
 * @brief Is set to true after the echoed command is received and set to false
 * when the response is received.
 */
static bool Adrastea_responseincoming = false;

/**
 * @brief Is set to true when module is in at mode and set to false otherwise.
 */
static volatile bool Adrastea_ATMode = false;

/**
 * @brief Time step (microseconds) when waiting for responses from Adrastea.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer
 * resolution.
 *
 * @see Adrastea_SetTimingParameters
 */
static uint32_t Adrastea_waitTimeStepUsec = 5 * 1000;

/**
 * @brief Minimum interval (microseconds) between subsequent commands sent to
 * Adrastea (more precisely the minimum interval between the confirmation of the
 * previous command and the sending of the following command).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer
 * resolution.
 *
 * @see Adrastea_SetTimingParameters
 */
static uint32_t Adrastea_minCommandIntervalUsec = 3 * 1000;

/**
 * @brief Time (microseconds) of last confirmation received from Adrastea (if
 * any).
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer
 * resolution.
 */
static uint32_t Adrastea_lastConfirmTimeUsec = 0;

/**
 * @brief Callback function for events.
 */
Adrastea_EventCallback_t Adrastea_eventCallback;

/**
 * @bief Is set to true if currently executing a (custom) event handler.
 */
static bool Adrastea_executingEventCallback = false;

/**
 * @brief Callback function which is executed if a single byte has been received
 * from Adrastea. The default callback is Adrastea_HandleRxByte().
 * @see Adrastea_SetByteRxCallback()
 */
Adrastea_ByteRxCallback_t Adrastea_byteRxCallback = NULL;

/**
 * @brief Optional callback function which is executed if a line is received
 * from Adrastea. Can be used to intercept responses from Adrastea.
 * @see Adrastea_SetLineRxCallback()
 */
Adrastea_LineRxCallback_t Adrastea_lineRxCallback = NULL;

/**
 * @brief Number of optional parameters delimiters so far (used by at commands).
 *
 */
uint8_t Adrastea_optionalParamsDelimCount = 0;

/**
 * @brief bool to indicate if the MCU is currently in the process of waking up
 * ffrom sleep.
 *
 */
static bool Adrastea_wakingUp = false;

/**
 * @brief Major firmware version of the driver.
 */
uint8_t Adrastea_firmwareVersionMajor = 6;

/**
 * @brief Minor firmware version of the driver.
 */
uint16_t Adrastea_firmwareVersionMinor = 6;

static Adrastea_Pins_t *Adrastea_pinsP = NULL;

/**
 * @brief Initializes the serial communication with the module
 *
 * @param[in] pinoutP        Pin definition
 * @param[in] baudrate       Baud rate of the serial communication
 * @param[in] flowControl    Flow control setting for the serial communication
 * @param[in] parity         Parity bit configuration for the serial
 communication
 * @param[in] eventCallback  Function pointer to event handler (optional)

 * @return true if successful, false otherwise
 */
bool Adrastea_Init(Adrastea_Pins_t *pinoutP, uint32_t baudrate,
                   WE_FlowControl_t flowControl, WE_Parity_t parity,
                   Adrastea_EventCallback_t eventCallback) {
    Adrastea_requestPending = false;

    /* Callbacks */
    Adrastea_byteRxCallback = Adrastea_HandleRxByte;
    Adrastea_lineRxCallback = NULL;
    Adrastea_eventCallback = eventCallback;

    /* initialize the pins */
    if (pinoutP == NULL) {
        return false;
    }

    Adrastea_pinsP = pinoutP;
    Adrastea_pinsP->Adrastea_Pin_WakeUp.type = WE_Pin_Type_Output;

    WE_Pin_t pins[sizeof(Adrastea_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &Adrastea_pinsP->Adrastea_Pin_WakeUp,
           sizeof(WE_Pin_t));

    if (false == WE_InitPins(pins, pin_count)) {
        /* error */
        return false;
    }

    /* Set initial pin levels */
    WE_SetPin(Adrastea_pinsP->Adrastea_Pin_WakeUp, WE_Pin_Level_Low);

    WE_UART_RXPin0_TXPin1_Init(baudrate, flowControl,
                               parity); /* Adrastea default: 115200 Baud 8 */
    WE_Delay(10);

    ATDevice_Reset();
    WE_Delay(1000);

    /* Set response timeouts */
    Adrastea_timeouts[Adrastea_Timeout_General] = 1000;
    Adrastea_timeouts[Adrastea_Timeout_Device] = 1000;
    Adrastea_timeouts[Adrastea_Timeout_GNSS] = 10000;
    Adrastea_timeouts[Adrastea_Timeout_HTTP] = 3000;
    Adrastea_timeouts[Adrastea_Timeout_MQTT] = 3000;
    Adrastea_timeouts[Adrastea_Timeout_NetService] = 10000;
    Adrastea_timeouts[Adrastea_Timeout_PacketDomain] = 3000;
    Adrastea_timeouts[Adrastea_Timeout_Proprietary] = 10000;
    Adrastea_timeouts[Adrastea_Timeout_SIM] = 3000;
    Adrastea_timeouts[Adrastea_Timeout_SMS] = 20000;
    Adrastea_timeouts[Adrastea_Timeout_Socket] = 3000;
    Adrastea_timeouts[Adrastea_Timeout_Power] = 3000;

    if (!ATDevice_Test()) {
        Adrastea_Transmit("map\r\n", 5);
        while (Adrastea_CheckATMode() == false) {
        }
    } else {
        Adrastea_ATMode = true;
    }

    ATDevice_Revision_Identity_t revisionIdentity;
    ATDevice_RequestRevisionIdentity(&revisionIdentity);

    // check if firmware of connected module matches version of the driver.
    if (revisionIdentity.major != Adrastea_firmwareVersionMajor ||
        revisionIdentity.minor != Adrastea_firmwareVersionMinor) {
        printf(
            "Driver version and module firmware version mismatch, driver might "
            "not behave as expected\r\n");
    }

    return true;
}

/**
 * @brief Deinitializes the serial communication with the module.
 *
 * @return true if successful, false otherwise
 */
bool Adrastea_Deinit(void) {
    Adrastea_eventCallback = NULL;

    Adrastea_rxByteCounter = 0;
    Adrastea_eolChar1Found = 0;
    Adrastea_requestPending = false;
    Adrastea_currentResponseLength = 0;

    WE_DeinitPin(Adrastea_pinsP->Adrastea_Pin_WakeUp);

    WE_UART_RXPin0_TXPin1_DeInit();

    return true;
}

/**
 * @brief Wakes the module up from power save mode using the wake up pin.
 *
 * @return true if successful, false otherwise
 */
bool Adrastea_PinWakeUp(void) {
    Adrastea_wakingUp = true;
    if (!WE_SetPin(Adrastea_pinsP->Adrastea_Pin_WakeUp, WE_Pin_Level_High)) {
        return false;
    }

    WE_Delay(100);

    if (!WE_SetPin(Adrastea_pinsP->Adrastea_Pin_WakeUp, WE_Pin_Level_Low)) {
        return false;
    }

    if (!Adrastea_SendRequest("sleepSet disable\r\n")) {
        return false;
    }

    if (!Adrastea_WaitForConfirm(Adrastea_GetTimeout(Adrastea_Timeout_Power),
                                 Adrastea_CNFStatus_Success, NULL)) {
        return false;
    }

    Adrastea_Transmit("map\r\n", 5);
    Adrastea_wakingUp = false;
    return true;
}

/**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data AT command to send. Note that the command has to end with
 * "\r\n\0".
 *
 * @return true if successful, false otherwise
 */
bool Adrastea_SendRequest(char *data) {
    if (Adrastea_executingEventCallback) {
        /* Don't allow sending AT commands from event handlers, as this will
         * mess up send/receive states and buffers. */
        return false;
    }

    Adrastea_requestPending = true;
    Adrastea_currentResponseLength = 0;

    /* Make sure that the time between the last confirmation received from the
     * module and the next command sent to the module is not shorter than
     * Adrastea_minCommandIntervalUsec */
    uint32_t t = WE_GetTickMicroseconds() - Adrastea_lastConfirmTimeUsec;
    if (t < Adrastea_minCommandIntervalUsec) {
        WE_DelayMicroseconds(Adrastea_minCommandIntervalUsec - t);
    }

    size_t dataLength = strlen(data);

    char delimiters[] = {ATCOMMAND_COMMAND_DELIM, '?', '\r'};

    /* Get command name from request string (remove prefix "AT+" and parameters)
     */
    Adrastea_pendingCommandName[0] = '\0';
    Adrastea_pendingCommandNameLength = 0;
    if (dataLength > 2 && data[0] == 'A' && data[1] == 'T') {
        char *pData;
        if (dataLength > 3 && (data[2] == '+' || data[2] == '%')) {
            pData = data + 3;
        } else {
            pData = data + 2;
        }
        if (ATCommand_GetCmdName(&pData, Adrastea_pendingCommandName,
                                 sizeof(Adrastea_pendingCommandName),
                                 delimiters, sizeof(delimiters))) {
            Adrastea_pendingCommandNameLength =
                strlen(Adrastea_pendingCommandName);
        }
    }

    WE_DEBUG_PRINT("> %s", data);

    Adrastea_Transmit(data, dataLength);

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
void Adrastea_Transmit(const char *data, uint16_t dataLength) {
    WE_UART_RXPin0_TXPin1_Transmit(data, dataLength);
}

/**
 * @brief Waits for the response from the module after a request.
 *
 * @param[in] maxTimeMs Maximum wait time in milliseconds
 * @param[in] expectedStatus Status to wait for
 * @param[out] pOutResponse Received response text (if any) will be written to
 * this buffer (optional)
 *
 * @return true if successful, false otherwise
 */
bool Adrastea_WaitForConfirm(uint32_t maxTimeMs,
                             Adrastea_CNFStatus_t expectedStatus,
                             char *pOutResponse) {
    Adrastea_cmdConfirmStatus = Adrastea_CNFStatus_Invalid;

    uint32_t t0 = WE_GetTick();

    while (1) {
        if (Adrastea_CNFStatus_Invalid != Adrastea_cmdConfirmStatus) {
            /* Store current time to enable check for min. time between received
             * confirm and next command. */
            Adrastea_lastConfirmTimeUsec = WE_GetTickMicroseconds();
            Adrastea_requestPending = false;
            Adrastea_responseincoming = false;
            if (Adrastea_cmdConfirmStatus == expectedStatus) {
                if (NULL != pOutResponse) {
                    /* Copy response for further processing */
                    memcpy(pOutResponse, Adrastea_currentResponseText,
                           Adrastea_currentResponseLength);
                }
                return true;
            } else {
                return false;
            }
        }

        uint32_t now = WE_GetTick();
        if (now - t0 > maxTimeMs) {
            /* Timeout */
            break;
        }

        if (Adrastea_waitTimeStepUsec > 0) {
            WE_DelayMicroseconds(Adrastea_waitTimeStepUsec);
        }
    }

    Adrastea_responseincoming = false;
    Adrastea_requestPending = false;
    return false;
}

/**
 * @brief Count the number of arguments in argument string.
 *
 * @param[in] Argument String to check
 *
 * @return number of arguments in argument string
 */
int Adrastea_CountArgs(char *stringP) {
    if (stringP == NULL) {
        return -1;
    }

    int length = strlen(stringP);

    if (length == 0) {
        return 0;
    }

    int quotationmarks = 0;
    int argscount = 1;

    for (int i = 0; i < length; i++) {
        if (stringP[i] == '"') {
            quotationmarks++;
        } else if (quotationmarks % 2 == 0 &&
                   stringP[i] == ATCOMMAND_ARGUMENT_DELIM) {
            argscount++;
        }
    }

    return argscount;
}

/**
 * @brief Check if the response of the command is expected to be more the one
 * line and fill Adrastea_responseSkip accordingly.
 */
void Adrastea_CheckResponseComplete() {
    Adrastea_responseSkip.lineskip = 0;
    Adrastea_responseSkip.delim = '\0';

    if (('+' != Adrastea_rxBuffer[0]) && ('%' != Adrastea_rxBuffer[0])) {
        return;
    }

    ATEvent_t event;
    char *tmp = Adrastea_rxBuffer;
    if (!ATEvent_ParseEventType(&tmp, &event)) {
        return;
    }

    switch (event) {
        case ATEvent_MQTT_Publication_Received:
            Adrastea_responseSkip.lineskip = 1;
            Adrastea_responseSkip.delim = ',';
            break;
        case ATEvent_SMS_Read_Message:
            Adrastea_responseSkip.lineskip = 1;
            Adrastea_responseSkip.delim = ',';
            break;
        case ATEvent_SMS_List_Messages:
            Adrastea_responseSkip.lineskip = 1;
            Adrastea_responseSkip.delim = ',';
            break;
        default:
            break;
    }
}

/**
 * @brief Set timing parameters used by the Adrastea driver.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer
 * resolution.
 *
 * @param[in] waitTimeStepUsec Time step (microseconds) when waiting for
 * responses from Adrastea.
 * @param[in] minCommandIntervalUsec Minimum interval (microseconds) between
 * subsequent commands sent to Adrastea.
 *
 * @return true if successful, false otherwise
 */
bool Adrastea_SetTimingParameters(uint32_t waitTimeStepUsec,
                                  uint32_t minCommandIntervalUsec) {
    Adrastea_waitTimeStepUsec = waitTimeStepUsec;
    Adrastea_minCommandIntervalUsec = minCommandIntervalUsec;
    return true;
}

/**
 * @brief Sets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 * @param[in] timeout Timeout in milliseconds
 */
void Adrastea_SetTimeout(Adrastea_Timeout_t type, uint32_t timeout) {
    Adrastea_timeouts[type] = timeout;
}

/**
 * @brief Gets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 *
 * @return Timeout in milliseconds
 */
uint32_t Adrastea_GetTimeout(Adrastea_Timeout_t type) {
    return Adrastea_timeouts[type];
}

void WE_UART_RXPin0_TXPin1_HandleRxByte(uint8_t receivedByte) {
    Adrastea_byteRxCallback(receivedByte);
}

/**
 * @brief Default byte received callback.
 *
 * Is called when a single byte has been received.
 *
 * @param[in] receivedByte The received byte.
 */
static void Adrastea_HandleRxByte(uint8_t receivedByte) {
    if (receivedByte == Adrastea_eolChar1 && Adrastea_rxByteCounter == 0) {
        return;
    }

    if (receivedByte == Adrastea_eolChar2 && !Adrastea_eolChar1Found) {
        return;
    }

    if (Adrastea_rxByteCounter >= ADRASTEA_LINE_MAX_SIZE) {
        Adrastea_rxByteCounter = 0;
        Adrastea_eolChar1Found = false;
        return;
    }

    if (receivedByte == Adrastea_eolChar1) {
        Adrastea_eolChar1Found = true;

        if (Adrastea_shouldCheckResponseSkip) {
            Adrastea_CheckResponseComplete();
            Adrastea_shouldCheckResponseSkip = false;
        }

        if (!Adrastea_twoEolCharacters) {
            if (Adrastea_responseSkip.lineskip == 0) {
                Adrastea_rxBuffer[Adrastea_rxByteCounter] = '\0';
                Adrastea_rxByteCounter++;
                Adrastea_HandleRxLine(Adrastea_rxBuffer,
                                      Adrastea_rxByteCounter);
                Adrastea_eolChar1Found = false;
                Adrastea_rxByteCounter = 0;
                Adrastea_shouldCheckResponseSkip = true;
            } else {
                Adrastea_responseSkip.lineskip -= 1;
                Adrastea_eolChar1Found = false;
                Adrastea_rxBuffer[Adrastea_rxByteCounter] =
                    Adrastea_responseSkip.delim;
                Adrastea_rxByteCounter++;
            }
        }
    } else if (Adrastea_eolChar1Found) {
        if (receivedByte == Adrastea_eolChar2) {
            if (Adrastea_responseSkip.lineskip == 0) {
                /* Interpret it now */
                Adrastea_rxBuffer[Adrastea_rxByteCounter] = '\0';
                Adrastea_rxByteCounter++;
                Adrastea_HandleRxLine(Adrastea_rxBuffer,
                                      Adrastea_rxByteCounter);
                Adrastea_eolChar1Found = false;
                Adrastea_rxByteCounter = 0;
                Adrastea_shouldCheckResponseSkip = true;
            } else {
                Adrastea_responseSkip.lineskip -= 1;
                Adrastea_eolChar1Found = false;
                Adrastea_rxBuffer[Adrastea_rxByteCounter] =
                    Adrastea_responseSkip.delim;
                Adrastea_rxByteCounter++;
            }
        }
    } else {
        Adrastea_rxBuffer[Adrastea_rxByteCounter++] = receivedByte;
    }
}

/**
 * @brief Is called when a complete line has been received.
 *
 * @param[in] rxPacket Received text
 * @param[in] rxLength Received text length
 */
static void Adrastea_HandleRxLine(char *rxPacket, uint16_t rxLength) {
    WE_DEBUG_PRINT("< %s\r\n", rxPacket);

    /* Check if a custom line rx callback is specified and call it if so */
    if (NULL != Adrastea_lineRxCallback) {
        /* Custom callback returns true if the line has been handled */
        bool handled = Adrastea_lineRxCallback(rxPacket, rxLength);
        if (handled) {
            return;
        }
    }
    if (Adrastea_requestPending) {
        if (0 == strncmp(&rxPacket[0], ADRASTEA_RESPONSE_OK,
                         strlen(ADRASTEA_RESPONSE_OK))) {
            Adrastea_responseincoming = false;
            Adrastea_cmdConfirmStatus = Adrastea_CNFStatus_Success;
        } else if (0 == strncmp(&rxPacket[0], ADRASTEA_RESPONSE_ERROR,
                                strlen(ADRASTEA_RESPONSE_ERROR))) {
            Adrastea_responseincoming = false;
            Adrastea_cmdConfirmStatus = Adrastea_CNFStatus_Failed;
        } else if (0 == strncmp(&rxPacket[0], ADRASTEA_SMS_ERROR,
                                strlen(ADRASTEA_SMS_ERROR))) {
            Adrastea_responseincoming = false;
            Adrastea_cmdConfirmStatus = Adrastea_CNFStatus_Failed;
        } else if (0 == strncmp(&rxPacket[0], ADRASTEA_POWERMODECHANGE_EVENT,
                                strlen(ADRASTEA_POWERMODECHANGE_EVENT))) {
            Adrastea_responseincoming = false;
            Adrastea_cmdConfirmStatus = Adrastea_CNFStatus_Success;
        } else if (0 == strncmp(&rxPacket[0], ADRASTEA_SLEEPSET_EVENT,
                                strlen(ADRASTEA_SLEEPSET_EVENT))) {
            Adrastea_responseincoming = false;
            Adrastea_cmdConfirmStatus = Adrastea_CNFStatus_Success;
        } else {
            if (Adrastea_responseincoming) {
                /* Copy to response text buffer, taking care not to exceed
                 * buffer size */
                uint16_t chunkLength = rxLength;
                bool isevent = false;

                if (Adrastea_currentResponseLength == 0 &&
                    rxLength > (Adrastea_pendingCommandNameLength + 2) &&
                    (0 == strncmp(Adrastea_pendingCommandName, rxPacket + 1,
                                  Adrastea_pendingCommandNameLength))) {
                    isevent = true;
                    chunkLength -= (Adrastea_pendingCommandNameLength + 2);
                }

                if (Adrastea_currentResponseLength + chunkLength >=
                    ADRASTEA_MAX_RESPONSE_TEXT_LENGTH) {
                    chunkLength = ADRASTEA_MAX_RESPONSE_TEXT_LENGTH -
                                  Adrastea_currentResponseLength;
                }

                if (Adrastea_currentResponseLength == 0) {
                    if (isevent) {
                        memcpy(&Adrastea_currentResponseText
                                   [Adrastea_currentResponseLength],
                               Adrastea_rxBuffer +
                                   Adrastea_pendingCommandNameLength + 2,
                               chunkLength);
                    } else {
                        memcpy(&Adrastea_currentResponseText
                                   [Adrastea_currentResponseLength],
                               Adrastea_rxBuffer, chunkLength);
                    }
                } else {
                    memcpy(&Adrastea_currentResponseText
                               [Adrastea_currentResponseLength],
                           Adrastea_rxBuffer, chunkLength);
                }
                Adrastea_currentResponseLength += chunkLength;
            } else if (rxLength < ADRASTEA_LINE_MAX_SIZE && rxLength > 2 &&
                       Adrastea_pendingCommandName[0] != '\0' &&
                       'A' == rxPacket[0] && 'T' == rxPacket[1] &&
                       ((('+' == rxPacket[2] || '%' == rxPacket[2]) &&
                         (0 == strncmp(Adrastea_pendingCommandName,
                                       Adrastea_rxBuffer + 3,
                                       Adrastea_pendingCommandNameLength))) ||
                        (0 == strncmp(Adrastea_pendingCommandName,
                                      Adrastea_rxBuffer + 2,
                                      Adrastea_pendingCommandNameLength)))) {
                // if true command echo detected store response for lines after
                Adrastea_responseincoming = true;
            }
        }
    }

    if (0 ==
        strncmp(&rxPacket[0], ADRASTEA_MCU_EVENT, strlen(ADRASTEA_MCU_EVENT))) {
        Adrastea_ATMode = false;
        if (!Adrastea_wakingUp) {
            Adrastea_Transmit("map\r\n", 5);
        }
    } else if (0 == strncmp(&rxPacket[0], ADRASTEA_MAPCLICLOSE_EVENT,
                            strlen(ADRASTEA_MAPCLICLOSE_EVENT))) {
        Adrastea_ATMode = false;
    } else if (0 == strncmp(&rxPacket[0], ADRASTEA_MAPCLIOPEN_EVENT,
                            strlen(ADRASTEA_MAPCLIOPEN_EVENT))) {
        Adrastea_ATMode = true;
    } else if (('+' == rxPacket[0]) || ('%' == rxPacket[0])) {
        if (NULL != Adrastea_eventCallback) {
            /* Execute callback (if specified). */
            Adrastea_executingEventCallback = true;
            Adrastea_eventCallback(Adrastea_rxBuffer);
            Adrastea_executingEventCallback = false;
        }
    }
}

/**
 * @brief Sets the callback function which is executed if a byte has been
 * received from Adrastea.
 *
 * The default callback is Adrastea_HandleRxByte().
 *
 * @param[in] callback Pointer to byte received callback function (default
 * callback is used if NULL)
 */
void Adrastea_SetByteRxCallback(Adrastea_ByteRxCallback_t callback) {
    Adrastea_byteRxCallback =
        (callback == NULL) ? Adrastea_HandleRxByte : callback;
}

/**
 * @brief Sets an optional callback function which is executed if a line has
 * been received from Adrastea. Can be used to intercept responses from
 * Adrastea.
 *
 * The callback function must return true if the line should not be processed by
 * the driver and false if the driver should process the line as usual.
 *
 * @param[in] callback Pointer to line received callback function
 */
void Adrastea_SetLineRxCallback(Adrastea_LineRxCallback_t callback) {
    Adrastea_lineRxCallback = callback;
}

/**
 * @brief Sets EOL character(s) used for interpreting responses from Adrastea.
 *
 * @param[in] eol1 First EOL character
 * @param[in] eol2 Second EOL character (is only used if twoEolCharacters is
 * true)
 * @param[in] twoEolCharacters Controls whether the two EOL characters eol1 and
 * eol2 (true) or only eol1 (false) is used
 */
void Adrastea_SetEolCharacters(uint8_t eol1, uint8_t eol2,
                               bool twoEolCharacters) {
    Adrastea_eolChar1 = eol1;
    Adrastea_eolChar2 = eol2;
    Adrastea_twoEolCharacters = twoEolCharacters;
}

/**
 * @brief Check if module is in at command mode
 *
 * @return boolean to indicate mode.
 */
bool Adrastea_CheckATMode() { return Adrastea_ATMode; }
