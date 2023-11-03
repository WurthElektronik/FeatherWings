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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Calypso driver header file.
 */

#ifndef CALYPSO_H_INCLUDED
#define CALYPSO_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <global.h>

/**
 * @brief Max recommended payload size is 1460 bytes.
 */
#define CALYPSO_MAX_PAYLOAD_SIZE 1460

/**
 * @brief Default receive buffer size (used when receiving data e.g. via sockets or HTTP requests).
 */
#define CALYPSO_RECEIVE_BUFFER_SIZE 2048

/**
 * @brief Max. length of sent commands and responses from Calypso.
 */
#define CALYPSO_LINE_MAX_SIZE 2048

/**
 * @brief Max. length of IP address strings.
 */
#define CALYPSO_MAX_IP_ADDRESS_LENGTH 44

/**
 * @brief Max. length of host name strings (e.g. URLs or IP addresses).
 */
#define CALYPSO_MAX_HOST_NAME_LENGTH 128

/**
 * @brief Max. length of response text (size of buffer storing responses received from Calypso).
 * @see Calypso_currentResponseText
 */
#define CALYPSO_MAX_RESPONSE_TEXT_LENGTH CALYPSO_LINE_MAX_SIZE

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief AT command confirmation status.
     */
    typedef enum Calypso_CNFStatus_t
    {
        Calypso_CNFStatus_Success,
        Calypso_CNFStatus_Failed,
        Calypso_CNFStatus_Invalid,
        Calypso_CNFStatus_NumberOfValues
    } Calypso_CNFStatus_t;

    /**
     * @brief Data format used for transferred data.
     */
    typedef enum Calypso_DataFormat_t
    {
        Calypso_DataFormat_Binary,
        Calypso_DataFormat_Base64,
        Calypso_DataFormat_NumberOfValues,
    } Calypso_DataFormat_t;

    /**
     * @brief Timeout categories (for responses to AT commands).
     * @see Calypso_SetTimeout(), Calypso_GetTimeout()
     */
    typedef enum Calypso_Timeout_t
    {
        Calypso_Timeout_General,
        Calypso_Timeout_FactoryReset,
        Calypso_Timeout_WlanAddProfile,
        Calypso_Timeout_WlanScan,
        Calypso_Timeout_NetAppUpdateTime,
        Calypso_Timeout_NetAppHostLookUp,
        Calypso_Timeout_HttpConnect,
        Calypso_Timeout_HttpRequest,
        Calypso_Timeout_FileIO,
        Calypso_Timeout_GPIO,
        Calypso_Timeout_NumberOfValues
    } Calypso_Timeout_t;

    /**
     * @brief Application modes. Used with Calypso_SetApplicationModePins().
     */
    typedef enum Calypso_ApplicationMode_t
    {
        Calypso_ApplicationMode_ATCommandMode = 0,
        Calypso_ApplicationMode_OTAUpdate = 1,
        Calypso_ApplicationMode_Provisioning = 2,
        Calypso_ApplicationMode_TransparentMode = 3
    } Calypso_ApplicationMode_t;

    /**
     * @brief Calypso event callback.
     * Arguments: Event text
     */
    typedef void (*Calypso_EventCallback_t)(char *);

    /**
     * @brief Calypso byte received callback.
     *
     * Is used to handle responses (byte per byte) from Calypso.
     *
     * Arguments: Received byte
     *
     * @see Calypso_SetByteRxCallback()
     */
    typedef void (*Calypso_ByteRxCallback_t)(uint8_t);

    /**
     * @brief Calypso line received callback.
     *
     * Can be used to intercept responses from Calypso.
     *
     * Arguments: Line text, length of text
     * Returns: true if the line should not be processed by the driver,
     *          false if the driver should process the line as usual
     *
     * @see Calypso_SetLineRxCallback()
     */
    typedef bool (*Calypso_LineRxCallback_t)(char *, uint16_t);

    extern uint8_t Calypso_firmwareVersionMajor;
    extern uint8_t Calypso_firmwareVersionMinor;
    extern uint8_t Calypso_firmwareVersionPatch;

    extern bool Calypso_Init(
        uint32_t baudrate,
        WE_FlowControl_t flowControl,
        WE_Parity_t parity,
        Calypso_EventCallback_t eventCallback);

    extern bool Calypso_Deinit(void);

    extern bool Calypso_SendRequest(char *data);
    extern bool Calypso_WaitForConfirm(uint32_t maxTimeMs,
                                       Calypso_CNFStatus_t expectedStatus,
                                       char *pOutResponse);

    extern int32_t Calypso_GetLastError(char *lastErrorText);

    extern uint32_t Calypso_GetBase64DecBufSize(uint8_t *inputData, uint32_t inputLength);
    extern uint32_t Calypso_GetBase64EncBufSize(uint32_t inputLength);
    extern bool Calypso_DecodeBase64(uint8_t *inputData,
                                     uint32_t inputLength,
                                     uint8_t *outputData,
                                     uint32_t *outputLength);
    extern bool Calypso_EncodeBase64(uint8_t *inputData,
                                     uint32_t inputLength,
                                     uint8_t *outputData,
                                     uint32_t *outputLength);

    extern bool Calypso_SetTimingParameters(uint32_t waitTimeStepMicroseconds, uint32_t minCommandIntervalMicroseconds);
    extern void Calypso_SetTimeout(Calypso_Timeout_t type, uint32_t timeout);
    extern uint32_t Calypso_GetTimeout(Calypso_Timeout_t type);

    extern void Calypso_Transmit(const char *data, uint16_t dataLength);
    extern void Calypso_SetByteRxCallback(Calypso_ByteRxCallback_t callback);
    extern void Calypso_SetLineRxCallback(Calypso_LineRxCallback_t callback);
    extern void Calypso_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters);

#ifdef __cplusplus
}
#endif

#endif // CALYPSO_H_INCLUDED
