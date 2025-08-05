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
 * @brief DaphnisI driver header file.
 */

#ifndef DAPHNISI_H_INCLUDED
#define DAPHNISI_H_INCLUDED

#include <global/global_types.h>
#include <stdbool.h>

/**
 * @brief Minimum firmware version of the module that is supported by the driver.
 */
#ifndef DAPHNISI_MIN_FW_VER
#define DAPHNISI_MIN_FW_VER FW(1, 4, 0)
#endif

#define DAPHNISI_DEFAULT_BAUDRATE (uint32_t)9600

#define DAPHNISI_STARTUP_TIME (uint32_t)1400

#define DAPHNISI_ADDRESS_LENGTH 4
typedef uint8_t DaphnisI_Device_Address_t[DAPHNISI_ADDRESS_LENGTH];

/**
 * @brief Max. length of sent commands and responses from DaphnisI.
 */
#define DAPHNISI_LINE_MAX_SIZE 512

#define DAPHNISI_RESPONSE_OK "OK"     /**< String sent by module if AT command was successful */
#define DAPHNISI_RESPONSE_ERROR "AT_" /**< String sent by module if AT command failed */

/**
 * @brief Max. length of response text (size of buffer storing responses received from DaphnisI).
 * @see DaphnisI_currentResponseText
 */
#define DAPHNISI_MAX_RESPONSE_TEXT_LENGTH DAPHNISI_LINE_MAX_SIZE

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct DaphnisI_Pins_t
    {
        WE_Pin_t DaphnisI_Pin_Reset;
        WE_Pin_t DaphnisI_Pin_WakeUp;
        WE_Pin_t DaphnisI_Pin_Boot;
    } DaphnisI_Pins_t;

    /**
 * @brief AT command confirmation status.
 */
    typedef enum DaphnisI_CNFStatus_t
    {
        DaphnisI_CNFStatus_Success,
        DaphnisI_CNFStatus_Failed,
        DaphnisI_CNFStatus_Invalid,
        DaphnisI_CNFStatus_NumberOfValues
    } DaphnisI_CNFStatus_t;

    /**
 * @brief AT command error messages.
 */
    typedef enum DaphnisI_ErrorMessage_t
    {
        DaphnisI_ErrorMessage_ERROR,
        DaphnisI_ErrorMessage_PARAM_ERROR,
        DaphnisI_ErrorMessage_BUSY_ERROR,
        DaphnisI_ErrorMessage_TEST_PARAM_OVERFLOW,
        DaphnisI_ErrorMessage_NO_NET_JOINED,
        DaphnisI_ErrorMessage_RX_ERROR,
        DaphnisI_ErrorMessage_DUTYCYCLE_RESTRICTED,
        DaphnisI_ErrorMessage_CRYPTO_ERROR,
#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)
        DaphnisI_ErrorMessage_INVALID_MODE,
        DaphnisI_ErrorMessage_INVALID_ROLE,
#endif
        DaphnisI_ErrorMessage_Count,
        DaphnisI_ErrorMessage_Invalid
    } DaphnisI_ErrorMessage_t;

    /**
 * @brief Timeout categories (for responses to AT commands).
 * @see DaphnisI_SetTimeout(), DaphnisI_GetTimeout()
 */
    typedef enum DaphnisI_Timeout_t
    {
        DaphnisI_Timeout_General,
#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)
        DaphnisI_Timeout_P2P_Data,
        DaphnisI_Timeout_P2P_Remote_GPIO,
#endif
        DaphnisI_Timeout_NumberOfValues
    } DaphnisI_Timeout_t;

    /**
 * @brief AT event IDs.
 */
    typedef enum DaphnisI_ATEvent_t
    {
        DaphnisI_ATEvent_Invalid = (uint16_t)0,

        // General events
        DaphnisI_ATEvent_StartUp,

        // LoRaWAN events
        DaphnisI_ATEvent_LoRaWAN_RxInfo,
        DaphnisI_ATEvent_LoRaWAN_RxData,
        DaphnisI_ATEvent_LoRaWAN_TxConf,
        DaphnisI_ATEvent_LoRaWAN_Class,
        DaphnisI_ATEvent_LoRaWAN_BeaconInfo,
        DaphnisI_ATEvent_LoRaWAN_BeaconLost,
        DaphnisI_ATEvent_LoRaWAN_BeaconNotReceived,
        DaphnisI_ATEvent_LoRaWAN_ContextState_Stored,
        DaphnisI_ATEvent_LoRaWAN_ContextState_Restored,
        DaphnisI_ATEvent_LoRaWAN_JoinState_Success,
        DaphnisI_ATEvent_LoRaWAN_JoinState_Fail,

#if DAPHNISI_MIN_FW_VER >= FW(1, 4, 0)
        // P2P events
        DaphnisI_ATEvent_P2P_TxConf_Success,
        DaphnisI_ATEvent_P2P_TxConf_Error,
        DaphnisI_ATEvent_P2P_TxConf_Busy,
        DaphnisI_ATEvent_P2P_TxTime,
        DaphnisI_ATEvent_P2P_TxResp_Success,
        DaphnisI_ATEvent_P2P_TxResp_Fail_Timeout,
        DaphnisI_ATEvent_P2P_TxResp_Fail_NACK_Received,
        DaphnisI_ATEvent_P2P_TxResp_Fail_Internal_Error,
        DaphnisI_ATEvent_P2P_RxData,
        DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Set_Response,
        DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Get_Response,
        DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Set_Response,
        DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Get_Response,
        DaphnisI_ATEvent_P2P_GPIO_Remote_Cfg_Changed,
        DaphnisI_ATEvent_P2P_GPIO_Remote_Value_Changed,
#endif

        DaphnisI_ATEvent_NumberOfValues,
        DaphnisI_ATEvent_Max = UINT16_MAX
    } DaphnisI_ATEvent_t;

    /**
 * @brief DaphnisI event callback.
 * Arguments: Event text
 */
    typedef void (*DaphnisI_EventCallback_t)(DaphnisI_ATEvent_t event_type, char* event_data);

    /**
 * @brief Initializes the serial communication with the module
 *
 * @param[in] uartP:          definition of the uart connected to the module
 * @param[in] pinoutP:        definition of the gpios connected to the module
 * @param[in] eventCallback  Function pointer to event handler (optional)

 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_Init(WE_UART_t* uartP, DaphnisI_Pins_t* pinoutP, DaphnisI_EventCallback_t eventCallback);

    /**
 * @brief Deinitializes the serial communication with the module.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_Deinit(void);

    /**
 * @brief Performs a reset of the module using the reset pin.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_PinReset(void);

    /**
 * @brief Wakes the module up from power save mode using the wake up pin.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_PinWakeUp(void);

    /**
 * @brief Sets pin level to high or low.
 *
 * @param[in] pin Output pin to be set
 * @param[in] level Output level to be set
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetPin(WE_Pin_t pin, WE_Pin_Level_t level);

    /**
 * @brief Sends the supplied AT command to the module
 *
 * @param[in] data AT command to send. Note that the command has to end with "\r\n\0".
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SendRequest(char* data);

    /**
 * @brief Waits for the response from the module after a request.
 *
 * @param[in] maxTimeMs Maximum wait time in milliseconds
 * @param[in] expectedStatus Status to wait for
 * @param[out] pOutResponse Received response text (if any) will be written to this buffer (optional)
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_WaitForConfirm(uint32_t maxTimeMs, DaphnisI_CNFStatus_t expectedStatus, char* pOutResponse);

    /**
 * @brief Waits for an event from the module.
 *
 * @param[in] maxTimeMs Maximum wait time in milliseconds
 * @param[in] expectedEvent Events to wait for
 * @param[in] eventsLength Length of events to wait for
 * @param[in] resetEventsBuffer boolean to indicate if the internal events buffer should reset or not
 *
 * @return the event if successful, DaphnisI_ATEvent_Invalid otherwise
 */
    extern DaphnisI_ATEvent_t DaphnisI_WaitForEvents(uint32_t maxTimeMs, DaphnisI_ATEvent_t* expectedEvents, uint8_t eventsLength, bool resetEventsBuffer);

    /**
 * @brief Returns the code of the last error (if any).
 *
 * @param[out] lastError value of last error (if any). See DaphnisI_ErrorMessage_t.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_GetLastError(DaphnisI_ErrorMessage_t* lastError);

    /**
 * @brief Set timing parameters used by the DaphnisI driver.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] waitTimeStepUsec Time step (microseconds) when waiting for responses from DaphnisI.
 * @param[in] minCommandIntervalUsec Minimum interval (microseconds) between subsequent commands sent to DaphnisI.
 *
 * @return true if successful, false otherwise
 */
    extern bool DaphnisI_SetTimingParameters(uint32_t waitTimeStepMicroseconds, uint32_t minCommandIntervalMicroseconds);

    /**
 * @brief Sets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 * @param[in] timeout Timeout in milliseconds
 */
    extern void DaphnisI_SetTimeout(DaphnisI_Timeout_t type, uint32_t timeout);

    /**
 * @brief Gets the timeout for responses to AT commands of the given type.
 *
 * @param[in] type Timeout (i.e. command) type
 *
 * @return Timeout in milliseconds
 */
    extern uint32_t DaphnisI_GetTimeout(DaphnisI_Timeout_t type);

#ifdef __cplusplus
}
#endif

#endif // DAPHNISI_H_INCLUDED
