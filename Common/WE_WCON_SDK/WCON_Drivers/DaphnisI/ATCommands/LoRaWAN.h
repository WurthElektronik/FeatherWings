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

#ifndef DAPHNIS_COMMANDS_LORAWAN_H_
#define DAPHNIS_COMMANDS_LORAWAN_H_

#include <DaphnisI/ATCommands/ATDevice.h>
#include <DaphnisI/DaphnisI.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define DAPHNISI_LORAWAN_EUI_LENGTH 8
#define DAPHNISI_LORAWAN_KEY_LENGTH 16
/**
 * @brief Max recommended payload size is 242 bytes.
 */
#define DAPHNISI_LORAWAN_MAX_PAYLOAD_SIZE 242

    typedef uint8_t DaphnisI_EUI_t[DAPHNISI_LORAWAN_EUI_LENGTH];
    typedef uint8_t DaphnisI_Key_t[DAPHNISI_LORAWAN_KEY_LENGTH];

    typedef enum
    {
        DaphnisI_DeviceClass_A = 0,
        DaphnisI_DeviceClass_B,
        DaphnisI_DeviceClass_C,
        DaphnisI_DeviceClass_Count,
    } DaphnisI_DeviceClass_t;

    typedef enum
    {
        DaphnisI_JoinMode_ABP,
        DaphnisI_JoinMode_OTAA,
        DaphnisI_JoinMode_Count,
    } DaphnisI_JoinMode_t;

    typedef enum
    {
        DaphnisI_KeyType_APP,
        DaphnisI_KeyType_NETWORK_SESSION,
        DaphnisI_KeyType_APP_SESSION,
        DaphnisI_KeyType_Count
    } DaphnisI_KeyType_t;

    typedef enum
    {
        DaphnisI_Delay_JoinWindow1,
        DaphnisI_Delay_JoinWindow2,
        DaphnisI_Delay_RX1,
        DaphnisI_Delay_RX2,
        DaphnisI_Delay_Count
    } DaphnisI_Delay_t;

    typedef enum DaphnisI_SlotWindow_t
    {
        DaphnisI_SlotWindow_1,
        DaphnisI_SlotWindow_2,
        DaphnisI_SlotWindow_P,
        DaphnisI_SlotWindow_C,
        DaphnisI_SlotWindow_Count,
    } DaphnisI_SlotWindow_t;

    typedef struct DaphnisI_RxInfo_t
    {
        uint8_t port;
        uint8_t dataRate;
        int8_t RSSI;
        uint8_t SNR;
        uint8_t linkMargin;
        uint8_t gatewaysReached;
        DaphnisI_SlotWindow_t window;
    } DaphnisI_RxInfo_t;

    typedef struct DaphnisI_RxData_t
    {
        uint8_t port;
        uint8_t dataLength;
        uint8_t* data;
    } DaphnisI_RxData_t;

    typedef struct DaphnisI_BeaconInfo_t
    {
        uint8_t dataRate;
        int8_t RSSI;
        uint8_t SNR;
        uint8_t infoDescriptor;
        char info[13];
        uint32_t frequency;
        uint32_t timestamp;
    } DaphnisI_BeaconInfo_t;

    /* Keys, IDs and EUIs */

    /**
 * @brief Set the App EUI
 *
 * @param[in] app_eui: APP_EUI (LSB First). See DaphnisI_EUI_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetAPPEUI(DaphnisI_EUI_t app_eui);

    /**
 * @brief Get the App EUI
 *
 * @param[out] app_euiP: Pointer to APP_EUI (LSB First). See DaphnisI_EUI_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetAPPEUI(DaphnisI_EUI_t* app_euiP);

    /**
 * @brief Set the Device EUI
 *
 * @param[in] device_eui: DEUI (LSB First). See DaphnisI_EUI_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetDeviceEUI(DaphnisI_EUI_t device_eui);

    /**
 * @brief Get the Device EUI
 *
 * @param[out] device_euiP: Pointer to DEUI (LSB First). See DaphnisI_EUI_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetDeviceEUI(DaphnisI_EUI_t* device_euiP);

    /**
 * @brief Set LoRaWAN key
 *
 * @param[in] keyType: Type of key to set. See DaphnisI_KeyType_t.
 *
 * @param[in] key: Key (LSB First). See DaphnisI_KeyType_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetKey(DaphnisI_KeyType_t keyType, DaphnisI_Key_t key);

    /**
 * @brief Set Device Address
 *
 * @param[in] device_address: Device address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetDeviceAddress(DaphnisI_Device_Address_t device_address);

    /**
 * @brief Get Device Address
 *
 * @param[out] device_addressP: Pointer to device address (LSB First). See DaphnisI_Device_Address_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetDeviceAddress(DaphnisI_Device_Address_t* device_addressP);

    /**
 * @brief Set Network ID
 *
 * @param[in] network_id: Network ID.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetNetworkID(uint8_t network_id);

    /**
 * @brief Get Network ID
 *
 * @param[out] network_idP: Pointer to network ID.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetNetworkID(uint8_t* network_idP);

    /* Stack Context */

    /**
 * @brief Stores the LoRaWAN context (using the AT+CS command).
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_StoreStackContext();

    /**
 * @brief Erases the stored LoRaWAN context (using the AT+RFS command).
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_EraseStoredStackContext();

    /* Network */

    /**
 * @brief Sets the device class
 *
 * @param[in] device_class : the device class. See DaphnisI_DeviceClass_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetDeviceClass(DaphnisI_DeviceClass_t device_class);

    /**
 * @brief Gets the device class
 *
 * @param[out] device_classP: pointer to the device class. See DaphnisI_DeviceClass_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetDeviceClass(DaphnisI_DeviceClass_t* device_classP);

    /**
 * @brief Join the network
 *
 * @param join_Mode : which mode to use for join. See DaphnisI_JoinMode_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_Join(DaphnisI_JoinMode_t join_Mode);

    /**
 * @brief Send message to connected network
 *
 * @param[in] port : the port number (range 0-199)
 * @param[in] payload: the payload to send
 * @param[in] length: the length of the payload in bytes
 * @param[in] ack_Requested: ack requested from network
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_Send(uint8_t port, const uint8_t* payload, uint16_t length, bool ack_Requested);

    /**
 * @brief Set delay
 *
 * @param[in] delayType: which delay to set. See DaphnisI_Delay_t.
 *
 * @param[in] delayMS: delay in milliseconds
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetDelay(DaphnisI_Delay_t delayType, uint16_t delayMS);

    /**
 * @brief Get delay
 *
 * @param[in] delayType: which delay to set. See DaphnisI_Delay_t.
 *
 * @param[out] delayMSP: pointer to the delay in milliseconds
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetDelay(DaphnisI_Delay_t delayType, uint16_t* delayMSP);

    /**
 * @brief Set the Rx2 window frequency in Hz
 *
 * @param[in] frequency: The frequency to set
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetRX2Frequency(uint32_t frequency);

    /**
 * @brief Get the Rx2 window frequency in Hz
 *
 * @param[out] frequencyP: The current Rx2 windows frequency
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetRX2Frequency(uint32_t* frequencyP);

    /**
 * @brief Set the Rx2 data rate
 *
 * @param[in] dataRate: The data rate of RX2
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetRX2DataRate(uint8_t dataRate);

    /**
 * @brief Get the Rx2 window data rate
 *
 * @param[out] dataRateP: The current Rx2 data rate
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetRX2DataRate(uint8_t* dataRateP);

    /**
 * @brief Set the ping slot periodicity factor
 *
 * @param[in] factor: The periodicity factor of the ping slot
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetPingSlotPeriodicityFactor(uint8_t factor);

    /**
 * @brief Get the ping slot periodicity factor
 *
 * @param[out] factorP: The periodicity factor of the ping slot
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetPingSlotPeriodicityFactor(uint8_t* factorP);

    /**
 * @brief Sets the Tx power
 *
 * @param[in] power: Tx power.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetTxPower(uint8_t power);

    /**
 * @brief Gets the Tx power
 *
 * @param[out] powerP: Tx power.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetTxPower(uint8_t* powerP);

    /**
 * @brief Sets the adaptive data rate state
 *
 * @param[in] adr: adaptive rate state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetAdaptiveDataRate(bool adr);

    /**
 * @brief Gets the adaptive data rate state
 *
 * @param[out] adrP: Adaptive data rate state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetAdaptiveDataRate(bool* adrP);

    /**
 * @brief Sets the data rate
 *
 * @param[in] dataRate: Data rate.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetDataRate(uint8_t dataRate);

    /**
 * @brief Gets the data rate power
 *
 * @param[out] dataRateP: Data rate.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetDataRate(uint8_t* dataRateP);

    /**
 * @brief Sets the duty cycle restriction state
 *
 * @param[in] dcRestriction: Duty cycle restriction state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_SetDutyCycleRestriction(bool dcRestriction);

    /**
 * @brief Gets the duty cycle restriction state
 *
 * @param[out] dcRestrictionP: Duty cycle restriction state.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetDutyCycleRestriction(bool* dcRestrictionP);

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)
    /* Version */

    /**
 * @brief Get the LoRaWAN link layer version (using the AT+LLVER command)
 *
 * @param[out] ll_VersionP: Pointer to the LoRaWAN link layer version. See DaphnisI_LoRaWAN_LL_Version_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetLinkLayerVersion(DaphnisI_LoRaWAN_LL_Version_t* ll_VersionP);

    /**
 * @brief Get the LoRaWAN regional parameters version (using the AT+RPVER command)
 *
 * @param[out] rp_VersionP: Pointer to the LoRaWAN regional parameters version. See DaphnisI_LoRaWAN_RP_Version_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_GetRegionalParametersrVersion(DaphnisI_LoRaWAN_RP_Version_t* rp_VersionP);
#endif

    /* 	LoRaWAN Events Parsers */

    /**
 * @brief Parses the values of the class change event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] pDevice_class The parsed class parameter
 *
 * @return true if parsed successfully, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_ParseClassEvent(char** pEventArguments, DaphnisI_DeviceClass_t* pDevice_class);

    /**
 * @brief Parses the values of the rx info event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] pRxInfo The parsed rx info event parameters
 *
 * @return true if parsed successfully, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_ParseRxInfoEvent(char** pEventArguments, DaphnisI_RxInfo_t* pRxInfo);

    /**
 * @brief Parses the values of the rx data event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] pRxData The parsed rx data parameters
 *
 * @return true if parsed successfully, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_ParseRxDataEvent(char** pEventArguments, DaphnisI_RxData_t* pRxData);

    /**
 * @brief Parses the values of the ping event arguments.
 *
 * @param[in,out] pEventArguments String containing arguments of the AT command
 * @param[out] pBeaconInfo The parsed beacon info event parameters
 *
 * @return true if parsed successfully, false otherwise
 */
    extern bool DaphnisI_LoRaWAN_ParseBeaconInfoEvent(char** pEventArguments, DaphnisI_BeaconInfo_t* pBeaconInfo);

#ifdef __cplusplus
}
#endif

#endif /* DAPHNIS_COMMANDS_LORAWAN_H_ */
