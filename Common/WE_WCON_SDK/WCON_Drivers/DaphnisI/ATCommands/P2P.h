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
 * @brief LoRaWAN commands for DaphnisI header file.
 */

#ifndef DAPHNIS_COMMANDS_P2P_H_
#define DAPHNIS_COMMANDS_P2P_H_

#include <DaphnisI/DaphnisI.h>

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct DaphnisI_P2P_RxData_t
    {
        DaphnisI_Device_Address_t source_address;
        int16_t RSSI;
        uint8_t data_length;
        uint8_t* data;
    } DaphnisI_P2P_RxData_t;

/**
 * @brief Max payload size.
 */
#define DAPHNISI_P2P_MAX_PAYLOAD_SIZE 224

    /**
 * @brief Transmit a payload using broadcast addressing (using the AT+P2PBROADCASTTX command)
 *
 * @param[in] payloadP: Pointer to payload.
 *
 * @param[in] payload_length: Payload length.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_P2P_TransmitBroadcast(uint8_t* payloadP, uint16_t payload_length);

    /**
 * @brief Transmit a payload using unicast addressing (using the AT+P2PUNICASTTX command)
 *
 * @param[in] payloadP: Pointer to payload.
 *
 * @param[in] payload_length: Payload length.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_P2P_TransmitUnicast(uint8_t* payloadP, uint16_t payload_length);

    /**
 * @brief Transmit a payload using multicast addressing (using the AT+P2PMULTICASTTX command)
 *
 * @param[in] payloadP: Pointer to payload.
 *
 * @param[in] payload_length: Payload length.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_P2P_TransmitMulticast(uint8_t* payloadP, uint16_t payload_length);

    /**
 * @brief Transmit a payload using unicast addressing to the specified destination address (using the AT+P2PUNICASTEXTX command)
 *
 * @param[in] dest_address: Destination address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @param[in] payloadP: Pointer to payload.
 *
 * @param[in] payload_length: Payload length.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_P2P_TransmitUnicastExtended(DaphnisI_Device_Address_t dest_address, uint8_t* payloadP, uint16_t payload_length);

    /**
 * @brief Transmit a payload using mutlicast addressing to the specified group id (using the AT+P2PMULTICASTEXTX command)
 *
 * @param[in] group_id: Group ID.
 *
 * @param[in] payloadP: Pointer to payload.
 *
 * @param[in] payload_length: Payload length.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_P2P_TransmitMulticastExtended(uint8_t group_id, uint8_t* payloadP, uint16_t payload_length);

    /**
 * @brief Set RX enabled state (using the AT+P2PRX command)
 *
 * @param[in] rx_state: State of RX.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_P2P_SetRXEnabled(bool rx_state);

    /**
 * @brief Get RX enabled state (using the AT+P2PRX command)
 *
 * @param[out] rx_stateP: Pointer to state of RX.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_P2P_GetRXEnabled(bool* rx_stateP);

    /* P2P Event Parsers */

    /**
 * @brief Parse the value of TX time event (using the +P2PTXTIME event)
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command.
 *
 * @param[out] timeonairP: Pointer to time on air of sent payload.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_P2P_ParseTXTimeEvent(char** pEventArguments, uint32_t* timeonairP);

    /**
 * @brief Parse the value of RX data event (using the +P2PRXDATA event)
 *
 * @param[in,out] pEventArguments: String containing arguments of the AT command.
 *
 * @param[out] rxDataP: Pointer to received data struct. See DaphnisI_P2P_RxData_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_P2P_ParseRXDataEvent(char** pEventArguments, DaphnisI_P2P_RxData_t* rxDataP);

#ifdef __cplusplus
}
#endif

#endif /* DAPHNISI_MIN_FW_VER */

#endif /* DAPHNIS_COMMANDS_P2P_H_ */
