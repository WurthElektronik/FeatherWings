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
 * @brief Metis-e driver header file.
 */

#include <global/global_types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef METIS_E_H_INCLUDED
#define METIS_E_H_INCLUDED

#define METIS_E_MAX_USERSETTING_LENGTH 1

#define METIS_E_DEFAULT_BAUDRATE (uint32_t)115200

#define METIS_E_RSSIINVALID (int8_t)-128

/* Masks for FLAGS */
/* RSSI output is indicated by bit 0 of cfg-flags */
/* Timestamp output is indicated by bit 1 of cfg-flags */
#define METIS_E_CFG_RSSI_ENABLE (uint8_t)0x01      /* Bit 0 for rssi enable  */
#define METIS_E_CFG_TIMESTAMP_ENABLE (uint8_t)0x02 /* Bit 1 for timestamp enable */
#define METIS_E_CFG_MASK (uint8_t)(METIS_E_CFG_RSSI_ENABLE | METIS_E_CFG_TIMESTAMP_ENABLE)

    typedef struct MetisE_Pins_t
    {
        WE_Pin_t MetisE_Pin_Reset;
        WE_Pin_t MetisE_Pin_SleepWakeUp;
        WE_Pin_t MetisE_Pin_Boot;
    } MetisE_Pins_t;

    typedef enum MetisE_wmBusMode_t
    {
        MetisE_wmBusMode_S = 0,
        MetisE_wmBusMode_T_Meter,
        MetisE_wmBusMode_T_Other,
        MetisE_wmBusMode_CT_Other,
        MetisE_wmBusMode_C_Meter,
        MetisE_wmBusMode_C_Other,
        MetisE_wmBusMode_MAX,
        MetisE_wmBusMode_Undefined = 0xFF,
    } MetisE_wmBusMode_t;

    typedef enum MetisE_wmBusFrameFormat_t
    {
        MetisE_FrameFormt_A = 1,
        MetisE_FrameFormat_B,
        MetisE_FrameFormat_Undefined = 0xFF
    } MetisE_wmBusFrameFormat_t;

    typedef enum MetisE_wmBusRole_t
    {
        MetisE_wmBusRole_Meter = 1,
        MetisE_wmBusRole_Gateway,
        MetisE_wmBusRole_MAX,
        MetisE_wmBusRole_Undefined,
    } MetisE_wmBusRole_t;

    /**
 * @brief Available user/runtime settings. A subset of the available options
 */
    typedef enum MetisE_UserSettings_t
    {
        MetisE_USERSETTING_WMBUS_RXMODE = 0x03,
        MetisE_USERSETTING_WMBUS_ROLE = 0x07,
        MetisE_USERSETTING_UART_BAUDRATE = 0x0A,
        MetisE_USERSETTING_TXPOWER = 0x0B,
        MetisE_USERSETTING_CFGFLAGS = 0x0C,
        MetisE_USERSETTING_FACTORYSETTINGS = 0x80,
        MetisE_USERSETTING_FWVERSION = 0x81,
    } MetisE_UserSettings_t;

    /**
 * @brief Struct representing a user setting.
 */
    typedef struct MetisE_Configuration_t
    {
        MetisE_UserSettings_t usersetting;             /**< user setting */
        uint8_t value[METIS_E_MAX_USERSETTING_LENGTH]; /**< value */
        uint8_t value_length;                          /**< length of the value */
    } MetisE_Configuration_t;

    typedef struct MetisE_ReceivedData_t
    {
        int8_t rssi;
        uint8_t payloadLength;
        uint32_t timestamp;
        uint8_t* pPayload;
        MetisE_wmBusMode_t wmBusModeRx;
        MetisE_wmBusFrameFormat_t wmBusModeFrameFormat;
    } MetisE_ReceivedData_t;

    extern bool MetisE_Init(WE_UART_t* uartP, MetisE_Pins_t* pinoutP, void (*RXcb)(MetisE_ReceivedData_t));
    extern bool MetisE_Deinit(void);

    extern bool MetisE_PinReset(void);
    extern bool MetisE_Reset(void);

    extern bool MetisE_Transmit(MetisE_wmBusMode_t mode, MetisE_wmBusFrameFormat_t frameFormat, uint8_t* payload, uint8_t length);
    extern bool MetisE_ReceiveStart(void);
    extern bool MetisE_ReceiveStop(void);

    extern bool MetisE_Shutdown(void);
    extern bool MetisE_Standby(void);
    extern bool MetisE_PinWakeup(void);

    /* Read the non-volatile settings */
    extern bool MetisE_GetFirmwareVersion(uint8_t* fw);
    extern bool MetisE_GetSerialNumber(uint8_t* sn);
    extern bool MetisE_GetDefault(MetisE_UserSettings_t us, uint8_t* response, uint8_t* response_length);
    extern bool MetisE_GetDefault_TXPower(uint8_t* txpower);
    extern bool MetisE_GetDefault_wmBusRxMode(MetisE_wmBusMode_t* wmbusRxMode);
    extern bool MetisE_GetDefault_wmBusRole(MetisE_wmBusRole_t* wmbusRole);
    extern bool MetisE_GetDefault_CFGFlags(uint8_t* cfgFlags);

    /* read volatile settings out of RAM */
    extern bool MetisE_GetVolatile_TXPower(uint8_t* txpower);
    extern bool MetisE_GetVolatile_wmBusRxMode(MetisE_wmBusMode_t* wmbusRxMode);
    extern bool MetisE_GetVolatile_wmBusRole(MetisE_wmBusRole_t* wmbusRxRole);
    extern bool MetisE_GetVolatile_CFGFlags(uint8_t* cfgFlags);

    /* Functions that write the non-volatile settings in the flash: After modification of any non-volatile setting,
 * the module must be reset such that the update takes effect.
 * IMPORTANT: Use only in rare cases, since flash can be written to only a limited number of times.
 */
    extern bool MetisE_FactoryReset(void);
    extern bool MetisE_SetDefault(MetisE_UserSettings_t us, uint8_t* value, uint8_t length);
    extern bool MetisE_SetDefault_TXPower(uint8_t txpower);
    extern bool MetisE_SetDefault_wmBusRxMode(MetisE_wmBusMode_t wmbusRxMode);
    extern bool MetisE_SetDefault_wmBusRole(MetisE_wmBusRole_t wmubsRxRole);
    extern bool MetisE_SetDefault_CFGFlags(uint8_t cfgFlags);

    /* write volatile settings into RAM, these settings are lost after a reset */
    extern bool MetisE_SetVolatile_TXPower(uint8_t txpower);
    extern bool MetisE_SetVolatile_wmBusRxMode(MetisE_wmBusMode_t wmbusRxMode);
    extern bool MetisE_SetVolatile_wmBusRole(MetisE_wmBusRole_t wmbusRxRole);
    extern bool MetisE_SetVolatile_CFGFlags(uint8_t cfgFlags);

#endif // METIS_E_H_INCLUDED

#ifdef __cplusplus
}
#endif
