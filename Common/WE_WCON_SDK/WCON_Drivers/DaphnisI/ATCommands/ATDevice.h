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
 * @brief Device commands for DaphnisI header file.
 */

#ifndef DAPHNIS_COMMANDS_DEVICE_H_
#define DAPHNIS_COMMANDS_DEVICE_H_

#include <DaphnisI/DaphnisI.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Tests communication with the device (using the AT command).
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_Test();

    /**
 * @brief Reboots the device (using the ATZ command).
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_Reset();

    /**
 * @brief Sleep device (using the AT+SLEEP command).
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_Sleep();

    /**
 * @brief Get the battery level (using the AT+BAT command)
 *
 * @param[out] batteryLevelP: Pointer to the battery level in mV.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetBatteryLevel(uint16_t* batteryLevelP);

    typedef struct DaphnisI_FW_Version_t
    {
        uint8_t Major;
        uint8_t Minor;
        uint8_t Patch;
    } DaphnisI_FW_Version_t;

    typedef struct DaphnisI_LoRaWAN_LL_Version_t
    {
        uint8_t Major;
        uint8_t Minor;
        uint8_t Patch;
    } DaphnisI_LoRaWAN_LL_Version_t;

    typedef struct DaphnisI_LoRaWAN_RP_Version_t
    {
        uint8_t Label;
        uint8_t Major;
        uint8_t Minor;
        uint8_t Patch;
    } DaphnisI_LoRaWAN_RP_Version_t;

    /**
 * @brief Get the generic version (using the AT+VER command)
 *
 * @param[out] firmware_VersionP: Pointer to the firmware version. See DaphnisI_FW_Version_t.
 *
 * @param[out] ll_VersionP: Pointer to the LoRaWAN link layer version. See DaphnisI_LoRaWAN_LL_Version_t.
 *
 * @param[out] rp_VersionP: Pointer to the LoRaWAN regional parameters version. See DaphnisI_LoRaWAN_RP_Version_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetGenericVersion(DaphnisI_FW_Version_t* firmware_VersionP, DaphnisI_LoRaWAN_LL_Version_t* ll_VersionP, DaphnisI_LoRaWAN_RP_Version_t* rp_VersionP);

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

    typedef DaphnisI_Device_Address_t DaphnisI_SerialNumber_t;

    /**
 * @brief Get the serial number (using the AT+SN command)
 *
 * @param[out] serialNumberP: Pointer to the serial number (LSB First). See DaphnisI_SerialNumber_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetSerialNumber(DaphnisI_SerialNumber_t* serialNumberP);

    /**
 * @brief Get the firmware version (using the AT+FWVER command)
 *
 * @param[out] firmware_VersionP: Pointer to the firmware version. See DaphnisI_FW_Version_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetFirmwareVersion(DaphnisI_FW_Version_t* firmware_VersionP);

#endif /* DAPHNISI_MIN_FW_VER */

#ifdef __cplusplus
}
#endif

#endif /* DAPHNIS_COMMANDS_DEVICE_H_ */
