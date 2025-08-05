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
 * @brief Adrastea driver header file.
 */

#ifndef ADRASTEAI_H_INCLUDED
#define ADRASTEAI_H_INCLUDED

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Max recommended payload size is 1460 bytes.
 */
#define ADRASTEAI_MAX_PAYLOAD_SIZE 1460

/**
 * @brief Max. length of sent commands and responses from Adrastea.
 */
#define ADRASTEAI_LINE_MAX_SIZE 2048

/**
 * @brief Max. length of response text (size of buffer storing responses received from ADRASTEA).
 * @see AdrasteaI_currentResponseText
 */
#define ADRASTEAI_MAX_RESPONSE_TEXT_LENGTH ADRASTEAI_LINE_MAX_SIZE

#define ADRASTEAI_RESPONSE_OK "OK"       /**< String sent by module if AT command was successful */
#define ADRASTEAI_RESPONSE_ERROR "ERROR" /**< String sent by module if AT command failed */
#define ADRASTEAI_SMS_ERROR "+CMS ERROR" /**< String sent by module if SMS AT command failed */

#define ADRASTEAI_MCU_EVENT "MCU menu -- PowerManager"  /**< String sent by module on boot up before access to at commands */
#define ADRASTEAI_MAPCLIOPEN_EVENT "Open MAP CLI"       /**< String sent by module after module is ready to receive at commands */
#define ADRASTEAI_POWERMODECHANGE_EVENT "Configured to" /**< String sent by module after mcu changed power mode */
#define ADRASTEAI_SLEEPSET_EVENT "Sleep -"              /**< String sent by module after mcu changed sleep */
#define ADRASTEAI_MAPCLICLOSE_EVENT "MAP CLI Closed"    /**< String sent by module when map cli is closed*/

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief AT command confirmation status.
 */
    typedef enum AdrasteaI_CNFStatus_t
    {
        AdrasteaI_CNFStatus_Success,
        AdrasteaI_CNFStatus_Failed,
        AdrasteaI_CNFStatus_Invalid,
        AdrasteaI_CNFStatus_NumberOfValues
    } AdrasteaI_CNFStatus_t;

    /**
 * @brief Timeout categories (for responses to AT commands).
 * @see AdrasteaI_SetTimeout(), AdrasteaI_GetTimeout()
 */
    typedef enum AdrasteaI_Timeout_t
    {
        AdrasteaI_Timeout_General,
        AdrasteaI_Timeout_Device,
        AdrasteaI_Timeout_GNSS,
        AdrasteaI_Timeout_HTTP,
        AdrasteaI_Timeout_MQTT,
        AdrasteaI_Timeout_NetService,
        AdrasteaI_Timeout_PacketDomain,
        AdrasteaI_Timeout_Proprietary,
        AdrasteaI_Timeout_SIM,
        AdrasteaI_Timeout_SMS,
        AdrasteaI_Timeout_Socket,
        AdrasteaI_Timeout_Power,
        AdrasteaI_Timeout_NumberOfValues
    } AdrasteaI_Timeout_t;

    /**
 * @brief Current ATMode of the module
 */
    typedef enum AdrasteaI_ATMode_t
    {
        AdrasteaI_ATMode_Off,   //no AT mode
        AdrasteaI_ATMode_Map,   //AT mode on but not fully booted
        AdrasteaI_ATMode_Ready, //AT mode on and ready
    } AdrasteaI_ATMode_t;

    /**
 * @brief Pins used by this driver.
 */
    typedef struct AdrasteaI_Pins_t
    {
        WE_Pin_t AdrasteaI_Pin_Reset;
        WE_Pin_t AdrasteaI_Pin_WakeUp;
    } AdrasteaI_Pins_t;

    /**
 * @brief Hold how many lines if the incoming response.
 */
    typedef struct AdrasteaI_Response_Complete_t
    {
        uint8_t lineskip;
        char delim;
    } AdrasteaI_Response_Complete_t;

    /**
 * @brief Adrastea event callback.
 *
 * Arguments: Event text
 */
    typedef void (*AdrasteaI_EventCallback_t)(char*);

    extern uint8_t AdrasteaI_optionalParamsDelimCount;

    extern bool AdrasteaI_Init(WE_UART_t* uartP, AdrasteaI_Pins_t* pinoutP, AdrasteaI_EventCallback_t eventCallback);
    extern bool AdrasteaI_Deinit(void);

    extern bool AdrasteaI_PinReset(void);
    extern bool AdrasteaI_PinWakeUp(void);

    extern bool AdrasteaI_SendRequest(char* data);
    extern bool AdrasteaI_WaitForConfirm(uint32_t maxTimeMs, AdrasteaI_CNFStatus_t expectedStatus, char* pOutResponse);

    extern bool AdrasteaI_SetTimingParameters(uint32_t waitTimeStepMicroseconds, uint32_t minCommandIntervalMicroseconds);
    extern void AdrasteaI_SetTimeout(AdrasteaI_Timeout_t type, uint32_t timeout);
    extern uint32_t AdrasteaI_GetTimeout(AdrasteaI_Timeout_t type);

    extern bool AdrasteaI_Transparent_Transmit(const char* data, uint16_t dataLength);
    extern void AdrasteaI_SetEolCharacters(uint8_t eol1, uint8_t eol2, bool twoEolCharacters);
    extern AdrasteaI_ATMode_t AdrasteaI_CheckATMode();

#ifdef __cplusplus
}
#endif

#endif // ADRASTEAI_H_INCLUDED
