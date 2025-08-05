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
 * @brief AT commands for basic device functionality.
 */

#ifndef CORDELIAI_AT_DEVICE_H_INCLUDED
#define CORDELIAI_AT_DEVICE_H_INCLUDED
#include <CordeliaI/CordeliaI.h>
#include <global/ATCommands.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ATDEVICE_SLEEP_MIN_TIMEOUT (uint32_t)1     /**< Minimum timeout for sleep command */
#define ATDEVICE_SLEEP_MAX_TIMEOUT (uint32_t)86400 /**< Maximum timeout for sleep command */
#define ATDEVICE_STOP_MAX_TIMEOUT (uint16_t)0xFFFF /**< Maximum timeout for stop command */
#define CordeliaI_MQTT_PUBLISH_MAX_LENGTH 512

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief ID parameter for AT+get=[ID],[Option] command.
 */
    typedef enum CordeliaI_ATDevice_GetId_t
    {
        CordeliaI_ATDevice_GetId_Status,
        CordeliaI_ATDevice_GetId_General,
        CordeliaI_ATDevice_GetId_IOT,
        CordeliaI_ATDevice_GetId_UART,
        CordeliaI_ATDevice_GetId_CSR,
        CordeliaI_ATDevice_GetId_MQTT,
        CordeliaI_ATDevice_GetId_SubTopic0,
        CordeliaI_ATDevice_GetId_SubTopic1,
        CordeliaI_ATDevice_GetId_SubTopic2,
        CordeliaI_ATDevice_GetId_SubTopic3,
        CordeliaI_ATDevice_GetId_PubTopic0,
        CordeliaI_ATDevice_GetId_PubTopic1,
        CordeliaI_ATDevice_GetId_PubTopic2,
        CordeliaI_ATDevice_GetId_PubTopic3,
        CordeliaI_ATDevice_GetId_Quarklink,
        CordeliaI_ATDevice_GetId_OTA,
        CordeliaI_ATDevice_GetId_NumberOfValues
    } CordeliaI_ATDevice_GetId_t;

    /**
 * @brief Option parameter for AT+get=mqtt,[Option] command.
 */
    typedef enum CordeliaI_ATDevice_GetMQTT_t
    {
        CordeliaI_ATDevice_GetMQTT_IotHubEndpoint,
        CordeliaI_ATDevice_GetMQTT_IotHubPort,
        CordeliaI_ATDevice_GetMQTT_ClientCertPath,
        CordeliaI_ATDevice_GetMQTT_RootCAPath,
        CordeliaI_ATDevice_GetMQTT_ClientPrivateKey,
        CordeliaI_ATDevice_GetMQTT_ClientId,
        CordeliaI_ATDevice_GetMQTT_Flags,
        CordeliaI_ATDevice_GetMQTT_TLS1_3,
        CordeliaI_ATDevice_GetMQTT_Base64,
        CordeliaI_ATDevice_GetMQTT_WillTopic,
        CordeliaI_ATDevice_GetMQTT_WillMessage,
        CordeliaI_ATDevice_GetMQTT_WillQos,
        CordeliaI_ATDevice_GetMQTT_WillRetain,
        CordeliaI_ATDevice_GetMQTT_UserName,
        CordeliaI_ATDevice_GetMQTT_Password,
        CordeliaI_ATDevice_GetMQTT_KeepAliveTimeOut,
        CordeliaI_ATDevice_GetMQTT_CleanConnect,
        CordeliaI_ATDevice_GetMQTT_NumberOfValues // Total number of MQTT parameters
    } CordeliaI_ATDevice_GetMQTT_t;

    /**
 * @brief Option parameter for AT+get=quarklink,[Option] command.
 */
    typedef enum CordeliaI_ATDevice_GetQuarklink_t
    {
        CordeliaI_ATDevice_GetQuarklink_Hostname,
        CordeliaI_ATDevice_GetQuarklink_Port,
        CordeliaI_ATDevice_GetQuarklink_RootCAPath,
        CordeliaI_ATDevice_GetQuarklink_NumberOfValues // Total number of MQTT parameters
    } CordeliaI_ATDevice_GetQuarklink_t;

    /**
 * @brief Option parameter for AT+get=ota,[Option] command.
 */
    typedef enum CordeliaI_ATDevice_GetOTA_t
    {
        CordeliaI_ATDevice_GetOTA_URL,
        CordeliaI_ATDevice_GetOTA_RootCAPath,
        CordeliaI_ATDevice_GetOTA_FirmwareVersion,
        CordeliaI_ATDevice_GetOTA_NumberOfValues // Total number of MQTT parameters
    } CordeliaI_ATDevice_GetOTA_t;

    /**
 * @brief Option parameter for AT+get=csr,[Option] command.
 */
    typedef enum CordeliaI_ATDevice_GetCSR_t
    {
        CordeliaI_ATDevice_GetCSR_Country,
        CordeliaI_ATDevice_GetCSR_State,
        CordeliaI_ATDevice_GetCSR_Locality,
        CordeliaI_ATDevice_GetCSR_Surname,
        CordeliaI_ATDevice_GetCSR_Organization,
        CordeliaI_ATDevice_GetCSR_Unit,
        CordeliaI_ATDevice_GetCSR_Email,
        CordeliaI_ATDevice_GetCSR_NumberOfValues
    } CordeliaI_ATDevice_GetCSR_t;

    /**
 * @brief Option parameter for AT+get=pubtopic,[Option] command.
 */
    typedef enum CordeliaI_ATDevice_GetPubTopic_t
    {
        CordeliaI_ATDevice_GetPubTopic_Name,
        CordeliaI_ATDevice_GetPubTopic_Qos,
        CordeliaI_ATDevice_GetPubTopic_Retain,
        CordeliaI_ATDevice_GetPubTopic_NumberOfValues
    } CordeliaI_ATDevice_GetPubTopic_t;

    /**
 * @brief Option parameter for AT+get=subtopic,[Option] command.
 */
    typedef enum CordeliaI_ATDevice_GetSubTopic_t
    {
        CordeliaI_ATDevice_GetSubTopic_Name,
        CordeliaI_ATDevice_GetSubTopic_Qos,
        CordeliaI_ATDevice_GetSubTopic_NumberOfValues
    } CordeliaI_ATDevice_GetSubTopic_t;

    /**
 * @brief Option parameter for AT+get=general,[Option] command.
 */
    typedef enum CordeliaI_ATDevice_GetGeneral_t
    {
        CordeliaI_ATDevice_GetGeneral_Version,
        CordeliaI_ATDevice_GetGeneral_Time,
        CordeliaI_ATDevice_GetGeneral_Persistent,
        CordeliaI_ATDevice_GetGeneral_NumberOfValues
    } CordeliaI_ATDevice_GetGeneral_t;

    /**
 * @brief Option parameter for AT+get=IOT,[Option] command.
 */
    typedef enum CordeliaI_ATDevice_GetIot_t
    {
        CordeliaI_ATDevice_GetIot_UDID,
        CordeliaI_ATDevice_GetIot_DeviceID,
        CordeliaI_ATDevice_GetIot_NumberOfValues
    } CordeliaI_ATDevice_GetIot_t;

    /**
 * @brief Option parameter for AT+get=UART,[Option] command.
 */
    typedef enum CordeliaI_ATDevice_GetUart_t
    {
        CordeliaI_ATDevice_GetUart_Baudrate,
        CordeliaI_ATDevice_GetUart_Parity,
        CordeliaI_ATDevice_GetUart_FlowControl,
        CordeliaI_ATDevice_GetUart_TransparentTrigger,
        CordeliaI_ATDevice_GetUart_TransparentTimeout,
        CordeliaI_ATDevice_GetUart_TransparentETX,
        CordeliaI_ATDevice_GetUart_NumberOfValues
    } CordeliaI_ATDevice_GetUart_t;

    /**
 * @brief Structure for MQTT publish arguments.

 */
    typedef struct CordeliaI_MQTT_PublishArguments_t
    {
        char Message[CordeliaI_MQTT_PUBLISH_MAX_LENGTH];
        uint8_t topicNo;
    } CordeliaI_MQTT_PublishArguments_t;

    /**
 * @brief UART parity parameter.
 */
    typedef enum CordeliaI_ATDevice_UartParity_t
    {
        CordeliaI_ATDevice_UartParity_None = 0,
        CordeliaI_ATDevice_UartParity_Even = 1,
        CordeliaI_ATDevice_UartParity_Odd = 2,
    } CordeliaI_ATDevice_UartParity_t;

    /**
 * @brief Transparent mode trigger parameters
 */
    typedef enum CordeliaI_ATDevice_TransparentModeUartTrigger_t
    {
        CordeliaI_ATDevice_TransparentModeUartTrigger_Timer = (1 << 0),       /**< Data is transmitted if a pause of configurable length is detected in the data stream */
        CordeliaI_ATDevice_TransparentModeUartTrigger_OneETX = (1 << 1),      /**< Data is transmitted if the first ETX character has been received */
        CordeliaI_ATDevice_TransparentModeUartTrigger_TwoETX = (1 << 2),      /**< Data is transmitted if the full ETX (two characters) has been received */
        CordeliaI_ATDevice_TransparentModeUartTrigger_TransmitETX = (1 << 3), /**< If this option is set and OneETX or TwoETX is set, the ETX characters are not removed from the data stream but are forwarded to the socket */
        CordeliaI_ATDevice_TransparentModeUartTrigger_NumberOfValues = 4
    } CordeliaI_ATDevice_TransparentModeUartTrigger_t;

    /**
 * @brief Status flags returned when calling CordeliaI_ATDevice_Get() with CordeliaI_ATDevice_GetId_Status.
 */
    typedef enum CordeliaI_ATDevice_StatusFlags_t
    {
        CordeliaI_ATDevice_StatusFlags_GeneralError = (1 << 0),
        CordeliaI_ATDevice_StatusFlags_WlanAsyncConnectedResponse = (1 << 1),
        CordeliaI_ATDevice_StatusFlags_WlanAsyncDisconnectedResponse = (1 << 2),
        CordeliaI_ATDevice_StatusFlags_StationConnected = (1 << 3),
        CordeliaI_ATDevice_StatusFlags_StationDisconnected = (1 << 4),
        CordeliaI_ATDevice_StatusFlags_P2PDeviceFound = (1 << 5),
        CordeliaI_ATDevice_StatusFlags_ConnectionFailed = (1 << 6),
        CordeliaI_ATDevice_StatusFlags_P2PNegativeRequestReceived = (1 << 7),
        CordeliaI_ATDevice_StatusFlags_RxFilters = (1 << 8),
        CordeliaI_ATDevice_StatusFlags_WlanStationConnected = (1 << 9),
        CordeliaI_ATDevice_StatusFlags_TxFailed = (1 << 10),
        CordeliaI_ATDevice_StatusFlags_IPAcquired = (1 << 11),
        CordeliaI_ATDevice_StatusFlags_IPAcquiredV6 = (1 << 12),
        CordeliaI_ATDevice_StatusFlags_IPLeased = (1 << 13),
        CordeliaI_ATDevice_StatusFlags_IPReleased = (1 << 14),
        CordeliaI_ATDevice_StatusFlags_IPv4Lost = (1 << 15),
        CordeliaI_ATDevice_StatusFlags_DHCPAcquireTimeout = (1 << 16),
        CordeliaI_ATDevice_StatusFlags_IPCollision = (1 << 17),
        CordeliaI_ATDevice_StatusFlags_IPv6Lost = (1 << 18),
        CordeliaI_ATDevice_StatusFlags_NumberOfValues = 19
    } CordeliaI_ATDevice_StatusFlags_t;

    /**
 * @brief Structure used for passing date/time data.
 */
    typedef struct CordeliaI_ATDevice_Time_t
    {
        uint8_t hour;   /**< Hour of day (0..23) */
        uint8_t minute; /**< Minute (0..59) */
        uint8_t second; /**< Second (0..59) */
        uint8_t day;    /**< Day (0..31) */
        uint8_t month;  /**< Month (1..12) */
        uint16_t year;  /**< Year */
    } CordeliaI_ATDevice_Time_t;

    /**
 * @brief Device version info.
 */
    typedef struct CordeliaI_ATDevice_Version_t
    {
        char chipId[32];                   /**< Chip ID */
        char MACVersion[32];               /**< MAC version (x.x.x.x) */
        char PHYVersion[32];               /**< PHY version (x.x.x.x) */
        char NWPVersion[32];               /**< NWP version (x.x.x.x) */
        char ROMVersion[32];               /**< ROM version (x) */
        char cordeliaiFirmwareVersion[32]; /**< CordeliaI firmware version (x.x.x). Only returned by firmware versions >= 1.9.0 */
    } CordeliaI_ATDevice_Version_t;

    /**
 * @brief General parameters (used by CordeliaI_ATDevice_Get() and CordeliaI_ATDevice_Set() with CordeliaI_ATDevice_GetId_General).
 */
    typedef union CordeliaI_ATDevice_Value_General_t
    {
        CordeliaI_ATDevice_Version_t version; /**< Version info, read only */
        CordeliaI_ATDevice_Time_t time;       /**< Current time */
        uint8_t persistent;                   /**< Persistent (1=enable, 0=disable) */
    } CordeliaI_ATDevice_Value_General_t;

    /**
 * @brief IOT parameters (used by CordeliaI_ATDevice_Get() and CordeliaI_ATDevice_Set() with CordeliaI_ATDevice_GetId_IOT).
 */
    typedef union CordeliaI_ATDevice_Value_IOT_t
    {
        char udid[37];     /**< 16 byte UDID plus a byte for null terminator, read only */
        char deviceId[65]; /**< 16 byte UDID plus a byte for null terminator, read only */
    } CordeliaI_ATDevice_Value_IOT_t;

    /**
 * @brief CSR parameters (used by CordeliaI_ATDevice_Get() and CordeliaI_ATDevice_Set() with CordeliaI_ATDevice_GetId_CSR).
 */
    typedef union CordeliaI_ATDevice_Value_CSR_t
    {
        char country[3];       /**< 2 byte country string plus a byte for null terminator */
        char state[129];       /**< 128 byte state string plus a byte for null terminator */
        char locality[129];    /**< 128 byte locality string plus a byte for null terminator */
        char surname[65];      /**< 64 byte surname string plus a byte for null terminator */
        char organization[65]; /**< 64 byte organization string plus a byte for null terminator */
        char unit[65];         /**< 64 byte unit string plus a byte for null terminator */
        char email[256];       /**< 255 byte email string plus a byte for null terminator */
    } CordeliaI_ATDevice_Value_CSR_t;

    /**
 * @brief UART parameters (used by CordeliaI_ATDevice_Get() and CordeliaI_ATDevice_Set() with CordeliaI_ATDevice_GetId_UART).
 */
    typedef union CordeliaI_ATDevice_Value_UART_t
    {
        uint32_t baudrate;                      /**< UART baud rate */
        CordeliaI_ATDevice_UartParity_t parity; /**< UART parity 0=none, 1=even, 2=odd) */
        bool flowControl;                       /**< UART flow control false=flow control disabled, true=flow control enabled */
        uint8_t transparentTrigger;             /**< Bitmask defining UART trigger used in transparent mode (see CordeliaI_ATDevice_TransparentModeUartTrigger_t) */
        uint16_t transparentTimeoutMs;          /**< Timeout used for triggering transmission in transparent mode (see CordeliaI_ATDevice_TransparentModeUartTrigger_Timer) */
        char transparentETX[2];                 /**< Two byte ETX used in transparent mode (see CordeliaI_ATDevice_TransparentModeUartTrigger_t)*/
    } CordeliaI_ATDevice_Value_UART_t;

    /**
 * @brief Used as parameter CordeliaI_ATDevice_Get() and CordeliaI_ATDevice_Set().
 * The member that is written to or read from depends on the ID and option parameters.
 *
 * @see CordeliaI_ATDevice_Get(), CordeliaI_ATDevice_Set(), CordeliaI_ATDevice_GetId_t
 */
    typedef union CordeliaI_ATDevice_Value_t
    {
        uint32_t status;                            /**< Status flags (response to getting device status, see CordeliaI_ATDevice_StatusFlags_t) */
        CordeliaI_ATDevice_Value_General_t general; /**< General device parameters, used with CordeliaI_ATDevice_GetId_General */
        CordeliaI_ATDevice_Value_IOT_t iot;         /**< IOT device parameters, used with CordeliaI_ATDevice_GetId_IOT */
        CordeliaI_ATDevice_Value_CSR_t csr;         /**< IOT device parameters, used with CordeliaI_ATDevice_Value_CSR_t */
        CordeliaI_ATDevice_Value_UART_t uart;       /**< UART device parameters, used with CordeliaI_ATDevice_GetId_UART */
    } CordeliaI_ATDevice_Value_t;

    extern bool CordeliaI_ATDevice_Test();
    extern bool CordeliaI_ATDevice_SendATCommand(char* atCommand);
    extern bool CordeliaI_Get_DeviceID();
    extern bool CordeliaI_ATDevice_Start();
    extern bool CordeliaI_ATDevice_Stop(uint32_t timeoutMs);
    extern bool CordeliaI_ATDevice_Restart(uint32_t timeoutMs);
    extern bool CordeliaI_ATDevice_Reboot();
    extern bool CordeliaI_ATDevice_FactoryReset();
    extern bool CordeliaI_ATDevice_Hibernate(uint32_t timeoutSeconds);
    extern bool CordeliaI_ATDevice_Get(CordeliaI_ATDevice_GetId_t id, uint8_t option, void* pValue);
    extern bool CordeliaI_ATDevice_Set(CordeliaI_ATDevice_GetId_t id, uint8_t option, void* pValue);
    extern bool CordeliaI_MQTT_Publish(CordeliaI_MQTT_PublishArguments_t publishArgs);
    extern bool CordeliaI_IoT_Enrol();
    extern bool CordeliaI_IoT_Connect();
    extern bool CordeliaI_IoT_Disconnect();
    extern bool CordeliaI_ATDevice_StartProvisioning();
    extern bool CordeliaI_ATDevice_PrintStatusFlags(uint32_t flags, char* pOutStr, size_t maxLength);

#ifdef __cplusplus
}
#endif

#endif /* CORDELIAI_AT_DEVICE_H_INCLUDED */
