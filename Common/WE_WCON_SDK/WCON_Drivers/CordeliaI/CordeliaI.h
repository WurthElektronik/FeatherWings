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
 * @brief CordeliaI driver header file.
 */

#ifndef CORDELIAI_H_INCLUDED
#define CORDELIAI_H_INCLUDED
#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <utils/base64.h>

/**
 * @brief Max. length of sent commands and responses from CordeliaI.
 */
#define CORDELIAI_LINE_MAX_SIZE 2048

/**
 * @brief Max. length of response text (size of buffer storing responses received from CordeliaI).
 * @see CordeliaI_currentResponseText
 */
#define CORDELIAI_MAX_RESPONSE_TEXT_LENGTH CORDELIAI_LINE_MAX_SIZE

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief AT command confirmation status.
 */
    typedef enum CordeliaI_CNFStatus_t
    {
        CordeliaI_CNFStatus_Success,
        CordeliaI_CNFStatus_Failed,
        CordeliaI_CNFStatus_Invalid,
        CordeliaI_CNFStatus_NumberOfValues
    } CordeliaI_CNFStatus_t;

    /**
 * @brief Data format used for transferred data.
 */
    typedef enum CordeliaI_DataFormat_t
    {
        CordeliaI_DataFormat_Binary,
        CordeliaI_DataFormat_Base64,
        CordeliaI_DataFormat_NumberOfValues,
    } CordeliaI_DataFormat_t;

    /**
 * @brief Timeout categories (for responses to AT commands).
 * @see CordeliaI_SetTimeout(), CordeliaI_GetTimeout()
 */
    typedef enum CordeliaI_Timeout_t
    {
        CordeliaI_Timeout_General,
        CordeliaI_Timeout_FactoryReset,
        CordeliaI_Timeout_WlanAddProfile,
        CordeliaI_Timeout_WlanScan,
        CordeliaI_Timeout_FileIO,
        CordeliaI_Timeout_OTAVersion,
        CordeliaI_Timeout_NumberOfValues
    } CordeliaI_Timeout_t;

    /**
 * @brief Application modes. Used with CordeliaI_SetApplicationModePins().
 */
    typedef enum CordeliaI_ApplicationMode_t
    {
        CordeliaI_ApplicationMode_ATCommandMode = 0,
        CordeliaI_ApplicationMode_OTAUpdate = 1,
        CordeliaI_ApplicationMode_Provisioning = 2,
        CordeliaI_ApplicationMode_TransparentMode = 3
    } CordeliaI_ApplicationMode_t;

    /**
 * @brief Pins used by this driver.
 */
    typedef struct CordeliaI_Pins_t
    {
        WE_Pin_t CordeliaI_Pin_Reset;
        WE_Pin_t CordeliaI_Pin_WakeUp;
        WE_Pin_t CordeliaI_Pin_Boot;
        WE_Pin_t CordeliaI_Pin_AppMode0;
        WE_Pin_t CordeliaI_Pin_AppMode1;
        WE_Pin_t CordeliaI_Pin_StatusInd0;
        WE_Pin_t CordeliaI_Pin_StatusInd1;
    } CordeliaI_Pins_t;

    /**
 * @brief CordeliaI event callback.
 * Arguments: Event text
 */
    typedef void (*CordeliaI_EventCallback_t)(char*);

    /**
 * @brief CordeliaI line received callback.
 *
 * Can be used to intercept responses from CordeliaI.
 *
 * Arguments: Line text, length of text
 * Returns: true if the line should not be processed by the driver,
 *          false if the driver should process the line as usual
 *
 * @see CordeliaI_SetLineRxCallback()
 */
    typedef bool (*CordeliaI_LineRxCallback_t)(char*, uint16_t);

    extern bool CordeliaI_Init(WE_UART_t* uartP, CordeliaI_Pins_t* pinoutP, CordeliaI_EventCallback_t eventCallback);
    extern bool CordeliaI_Deinit(void);

    extern bool CordeliaI_SetApplicationModePins(CordeliaI_ApplicationMode_t appMode);
    extern bool CordeliaI_PinReset(void);
    extern bool CordeliaI_PinWakeUp(void);
    extern bool CordeliaI_GetPinLevel(WE_Pin_t pin, WE_Pin_Level_t* pin_levelP);

    extern bool CordeliaI_SendRequest(char* data);
    extern bool CordeliaI_WaitForConfirm(uint32_t maxTimeMs, CordeliaI_CNFStatus_t expectedStatus, char* pOutResponse);

    extern int32_t CordeliaI_GetLastError(char* lastErrorText);

    extern bool CordeliaI_SetTimingParameters(uint32_t waitTimeStepMicroseconds, uint32_t minCommandIntervalMicroseconds);
    extern void CordeliaI_SetTimeout(CordeliaI_Timeout_t type, uint32_t timeout);
    extern uint32_t CordeliaI_GetTimeout(CordeliaI_Timeout_t type);

    extern bool CordeliaI_Transparent_Transmit(const char* data, uint16_t dataLength);
    extern void CordeliaI_SetByteRxCallback(WE_UART_HandleRxByte_t callback);
    extern void CordeliaI_SetLineRxCallback(CordeliaI_LineRxCallback_t callback);
    extern void CordeliaI_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters);

#ifdef __cplusplus
}
#endif

#endif // CORDELIAI_H_INCLUDED
