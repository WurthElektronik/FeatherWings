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
 * @brief StephanoI driver header file.
 */

#ifndef STEPHANOI_H_INCLUDED
#define STEPHANOI_H_INCLUDED

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <utils/base64.h>

/**
 * @brief Max recommended payload size is 512 bytes.
 */
#define STEPHANOI_MAX_PAYLOAD_SIZE 512

/**
 * @brief Max. length of sent commands and responses from StephanoI.
 */
#define STEPHANOI_LINE_MAX_SIZE 2048

#define STEPHANOI_DEFAULT_BAUDRATE (uint32_t)115200

/**
 * @brief Max. length of response text (size of buffer storing responses received from StephanoI).
 * @see StephanoI_currentResponseText
 */
#define STEPHANOI_MAX_RESPONSE_TEXT_LENGTH STEPHANOI_LINE_MAX_SIZE

#define STEPHANOI_RESPONSE_ERROR "ERROR"          /**< String sent by module if AT command failed */
#define STEPHANOI_SEND_OK "SEND OK"               /**< String sent by module if Socket transmission was OK */
#define STEPHANOI_SEND_CANCELLED "SEND CANCELLED" /**< String sent by module if Socket transmission has been cancelled */
#define STEPHANOI_SEND_FAIL "SEND FAIL"           /**< String sent by module if Socket transmission has been failed */
#define STEPHANOI_READY4DATA_CHAR (char)'>'       /**< Character sent by the module if ready for data reception */
#define STEPHANOI_SET_OK "SET OK"                 /**< String sent by module if http url set was ok */

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct StephanoI_Pins_t
    {
        WE_Pin_t StephanoI_Pin_Reset;
        WE_Pin_t StephanoI_Pin_Wakeup;

    } StephanoI_Pins_t;

    /**
 * @brief AT command confirmation status.
 */
    typedef enum StephanoI_CNFStatus_t
    {
        StephanoI_CNFStatus_Success,
        StephanoI_CNFStatus_Failed,
        StephanoI_CNFStatus_Ready4Data,
        StephanoI_CNFStatus_SendOK,
        StephanoI_CNFStatus_SetOK,
        StephanoI_CNFStatus_Invalid,
        StephanoI_CNFStatus_NumberOfValues
    } StephanoI_CNFStatus_t;

    /**
 * @brief Data format used for transferred data.
 */
    typedef enum StephanoI_DataFormat_t
    {
        StephanoI_DataFormat_Binary,
        StephanoI_DataFormat_NumberOfValues,
    } StephanoI_DataFormat_t;

    /**
 * @brief Timeout categories (for responses to AT commands).
 * @see StephanoI_SetTimeout(), StephanoI_GetTimeout()
 */
    typedef enum StephanoI_Timeout_t
    {
        StephanoI_Timeout_General,
        StephanoI_Timeout_FactoryReset,
        StephanoI_Timeout_WifiScan,
        StephanoI_Timeout_WifiConnect,
        StephanoI_Timeout_SocketOpen,
        StephanoI_Timeout_SocketPing,
        StephanoI_Timeout_MQTTGeneral,
        StephanoI_Timeout_MQTTConnect,
        StephanoI_Timeout_HTTPGeneral,
        StephanoI_Timeout_HTTPGetPost,
        StephanoI_Timeout_NumberOfValues
    } StephanoI_Timeout_t;

    /**
 * @brief Boolean value (true, false).
 */
    typedef enum StephanoI_BooleanValue_t
    {
        StephanoI_BooleanValue_False = 0,
        StephanoI_BooleanValue_True = 1,
        StephanoI_BooleanValue_NumberOfValues
    } StephanoI_BooleanValue_t;

    /**
 * @brief StephanoI event callback.
 * Arguments: Event text
 */
    typedef void (*StephanoI_EventCallback_t)(char*);

    extern bool StephanoI_Init(WE_UART_t* uartP, StephanoI_Pins_t* pinoutP, StephanoI_EventCallback_t eventCallback);
    extern bool StephanoI_Deinit(void);

    extern bool StephanoI_PinReset(void);
    extern bool StephanoI_SetWakeUpPin(bool high);

    extern bool StephanoI_Transparent_Transmit(const uint8_t* data, uint16_t dataLength);
    extern bool StephanoI_SendRequest(char* data);
    extern bool StephanoI_SendRequest_ex(uint8_t* data, size_t dataLength);
    extern bool StephanoI_WaitForConfirm(uint32_t maxTimeMs, StephanoI_CNFStatus_t expectedStatus);
    extern bool StephanoI_WaitForConfirm_ex(uint32_t maxTimeMs, StephanoI_CNFStatus_t expectedStatus, char* pOutResponse, uint16_t responseSize);

    extern bool StephanoI_SetTimingParameters(uint32_t waitTimeStepMicroseconds, uint32_t minCommandIntervalMicroseconds);
    extern void StephanoI_SetTimeout(StephanoI_Timeout_t type, uint32_t timeout);
    extern uint32_t StephanoI_GetTimeout(StephanoI_Timeout_t type);

    extern void StephanoI_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters);

#ifdef __cplusplus
}
#endif

#endif // STEPHANOI_H_INCLUDED
