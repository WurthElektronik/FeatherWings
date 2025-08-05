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
 * @brief Skoll-I driver header file.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef SKOLL_I_H_INCLUDED
#define SKOLL_I_H_INCLUDED

#include "EZSerial_Host/ezsapi.h"
#include <global/global_types.h>
#include <stdbool.h>

/* timings */
#define SKOLL_I_BOOT_DURATION (uint16_t)700

#define SKOLL_I_DEFAULT_BAUDRATE (uint32_t)115200

#define SKOLL_I_COMMAND_TIMEOUT_MS (uint32_t)1500

#define WE_BLUETOOTH_COMPANY_IDENTIFIER 0x031A

    /*  Skoll-I pins */
    typedef struct Skoll_I_Pins_t
    {
        WE_Pin_t Skoll_I_Pin_Reset;
        WE_Pin_t Skoll_I_Pin_SPP;
        WE_Pin_t Skoll_I_Pin_Connection;
        WE_Pin_t Skoll_I_Pin_Role;
    } Skoll_I_Pins_t;

    /* Bluetooth Classic roles */
    typedef enum
    {
        Skoll_I_BluetoothClassic_Role_Master,
        Skoll_I_BluetoothClassic_Role_Slave,
    } Skoll_I_BluetoothClassic_Role_t;

    /* Bluetooth LE roles */
    typedef enum
    {
        Skoll_I_BluetoothLE_Role_Peripheral,
        Skoll_I_BluetoothLE_Role_Central,
    } Skoll_I_BluetoothLE_Role_t;

    /**
 * @brief Initialize the Skoll_I interface for serial interface
 *
 * @param[in] uartP:                      definition of the uart connected to the module
 * @param[in] pinoutP:                    definition of the gpios connected to the module
 * @param[in] event_handler_in:           RX event handler function
 * @param[in] transparentdata_handler_in: RX event handler function for the transparenrt data mode
 *
 * @return true if initialization succeeded,
 *         false otherwise
 */
    extern bool Skoll_I_Init(WE_UART_t* uartP, Skoll_I_Pins_t* pinoutP, void (*event_handler_in)(ezs_packet_t* packet), void (*transparentdata_handler_in)(uint8_t* payload, uint16_t payloadLength));

    /**
 * @brief Deinitialize the Skoll_I interface
 *
 * @return true if deinitialization succeeded,
 *         false otherwise
 */
    extern bool Skoll_I_Deinit(void);

    /**
 * @brief Configures the Skoll_I UART interface
 *
 * @details: Disables echo and sets the mode to binary. Furthermore it defines whether the module
 * switches to transparent data mode, when connection is open, or stays in command mode.
 * This function should be run, after each reboot.
 *
 * @return true if configuration succeeded,
 *         false otherwise
 */
    extern bool Skoll_I_Configure(void);

    /**
 * @brief Reset the Skoll_I by pin
 *
 * @return true if reset succeeded,
 *         false otherwise
 */
    extern bool Skoll_I_PinReset(void);

    /**
 * @brief Check pin if Bluetooth connection is open
 *
 * @param[out] connectionOpenStateP: Bluetooth connection open state
 *
 * @return true if request is successful,
 *         false otherwise
 */
    extern bool Skoll_I_PinIsConnectionOpen(bool* connectionOpenStateP);

    /**
 * @brief Check pin if Bluetooth channel is open
 *
 * @param[out] channelOpenStateP: Bluetooth channel open state
 *
 * @return true if request is successful,
 *         false otherwise
 */
    extern bool Skoll_I_PinIsChannelOpen(bool* channelOpenStateP);

    /**
 * @brief Set role of Bluetooth LE interface via module pin
 *
 * @param[in] role: Role
 *
 * @return true if succeeded,
 *         false otherwise
 */
    extern bool Skoll_I_BluetoothLE_PinSetRole(Skoll_I_BluetoothLE_Role_t role);

    /**
 * @brief Disconnects the Bluetooth LE connection via commands
 *
 * @details: Pin is used to switch to command mode, before disconnecting
 *
 * @param[in] conn_handle: Handle of the Bluetooth LE connection
 *
 * @return true if succeeded,
 *         false otherwise
 */
    extern bool Skoll_I_BluetoothLE_PinDisconnect(uint8_t conn_handle);

    /**
 * @brief Transmit data in case the module entered the transparent Bluetooth LE CYSPP data mode
 *
 * @param[in] length: Length of the data
 * @param[in] data: Data to be sent
 *
 * @return true if succeeded,
 *         false otherwise
 */
    extern bool Skoll_I_BluetoothLE_CYSPPTransparentTransmit(uint16_t length, const uint8_t* data);

    /**
 * @brief Disconnects the Bluetooth Classic connection via pin
 *
 * @return true if succeeded,
 *         false otherwise
 */
    extern bool Skoll_I_BluetoothClassic_PinDisconnect(void);

    /**
 * @brief Transmit data in case the module entered the transparent Bluetooth Classic SPP data mode
 *
 * @param[in] length: Length of the data
 * @param[in] data: Data to be sent
 *
 * @return true if succeeded,
 *         false otherwise
 */
    extern bool Skoll_I_BluetoothClassic_SPPTransparentTransmit(uint16_t length, const uint8_t* data);

    extern ezs_packet_t* Skoll_I_Wait4CNF(ezs_packet_type_t type, uint32_t max_time_ms);
#endif // SKOLL_I_H_INCLUDED

#ifdef __cplusplus
}
#endif
