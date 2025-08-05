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
 * @brief AT commands for basic device functionality.
 */

#ifndef CALYPSO_AT_DEVICE_H_INCLUDED
#define CALYPSO_AT_DEVICE_H_INCLUDED
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ATDEVICE_SLEEP_MIN_TIMEOUT (uint32_t)1     /**< Minimum timeout for sleep command */
#define ATDEVICE_SLEEP_MAX_TIMEOUT (uint32_t)86400 /**< Maximum timeout for sleep command */
#define ATDEVICE_STOP_MAX_TIMEOUT (uint16_t)0xFFFF /**< Maximum timeout for stop command */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief ID parameter for AT+get=[ID] command.
 */
    typedef enum Calypso_ATDevice_GetId_t
    {
        Calypso_ATDevice_GetId_Status,
        Calypso_ATDevice_GetId_General,
        Calypso_ATDevice_GetId_IOT,
        Calypso_ATDevice_GetId_UART,
        Calypso_ATDevice_GetId_TransparentMode,
        Calypso_ATDevice_GetId_GPIO,
        Calypso_ATDevice_GetId_NumberOfValues
    } Calypso_ATDevice_GetId_t;

    /**
 * @brief Option parameter for AT+get=status,[Option] command.
 */
    typedef enum Calypso_ATDevice_GetStatus_t
    {
        Calypso_ATDevice_GetStatus_Device,
        Calypso_ATDevice_GetStatus_WLAN,
        Calypso_ATDevice_GetStatus_BSD,
        Calypso_ATDevice_GetStatus_NetApp,
        Calypso_ATDevice_GetStatus_NumberOfValues
    } Calypso_ATDevice_GetStatus_t;

    /**
 * @brief Option parameter for AT+get=general,[Option] command.
 */
    typedef enum Calypso_ATDevice_GetGeneral_t
    {
        Calypso_ATDevice_GetGeneral_Version,
        Calypso_ATDevice_GetGeneral_Time,
        Calypso_ATDevice_GetGeneral_Persistent,
        Calypso_ATDevice_GetGeneral_NumberOfValues
    } Calypso_ATDevice_GetGeneral_t;

    /**
 * @brief Option parameter for AT+get=IOT,[Option] command.
 */
    typedef enum Calypso_ATDevice_GetIot_t
    {
        Calypso_ATDevice_GetIot_UDID,
        Calypso_ATDevice_GetIot_NumberOfValues
    } Calypso_ATDevice_GetIot_t;

    /**
 * @brief Option parameter for AT+get=UART,[Option] command.
 */
    typedef enum Calypso_ATDevice_GetUart_t
    {
        Calypso_ATDevice_GetUart_Baudrate,
        Calypso_ATDevice_GetUart_Parity,
        Calypso_ATDevice_GetUart_FlowControl,
        Calypso_ATDevice_GetUart_TransparentTrigger,
        Calypso_ATDevice_GetUart_TransparentTimeout,
        Calypso_ATDevice_GetUart_TransparentETX,
        Calypso_ATDevice_GetUart_NumberOfValues
    } Calypso_ATDevice_GetUart_t;

    /**
 * @brief Option parameter for AT+get=transparent_mode,[Option] command.
 *
 * Minimum firmware version required: 2.0.0
 */
    typedef enum Calypso_ATDevice_GetTransparentMode_t
    {
        Calypso_ATDevice_GetTransparentMode_RemoteAddress,
        Calypso_ATDevice_GetTransparentMode_RemotePort,
        Calypso_ATDevice_GetTransparentMode_LocalPort,
        Calypso_ATDevice_GetTransparentMode_SocketType,
        Calypso_ATDevice_GetTransparentMode_SecureMethod,
        Calypso_ATDevice_GetTransparentMode_PowerSave,
        Calypso_ATDevice_GetTransparentMode_SkipDateVerify,
        Calypso_ATDevice_GetTransparentMode_DisableCertificateStore,
        Calypso_ATDevice_GetTransparentMode_NumberOfValues
    } Calypso_ATDevice_GetTransparentMode_t;

    /**
 * @brief Option parameter for AT+get=GPIO,[Option] command.
 *
 * Minimum firmware version required: 2.0.0
 */
    typedef enum Calypso_ATDevice_GetGPIO_t
    {
        Calypso_ATDevice_GetGPIO_RemoteLock,
        Calypso_ATDevice_GetGPIO_NumberOfValues
    } Calypso_ATDevice_GetGPIO_t;

    /**
 * @brief UART parity parameter.
 */
    typedef enum Calypso_ATDevice_UartParity_t
    {
        Calypso_ATDevice_UartParity_None = 0,
        Calypso_ATDevice_UartParity_Even = 1,
        Calypso_ATDevice_UartParity_Odd = 2,
    } Calypso_ATDevice_UartParity_t;

    /**
 * @brief Socket types for communicating with peer in transparent mode.
 *
 * Minimum firmware version required: 2.0.0
 */
    typedef enum Calypso_ATDevice_TransparentModeSocketType_t
    {
        Calypso_ATDevice_TransparentModeSocketType_UDP,       /**< Send/receive data to/from a UDP device */
        Calypso_ATDevice_TransparentModeSocketType_TCPServer, /**< Create a TCP server and send/receive data to/from the first peer device that connects */
        Calypso_ATDevice_TransparentModeSocketType_TCPClient, /**< Create a TCP connection to a TCP server and send/receive data to/from it */
        Calypso_ATDevice_TransparentModeSocketType_NumberOfValues
    } Calypso_ATDevice_TransparentModeSocketType_t;

    /**
 * @brief Socket secure method types for communicating with peer in transparent mode.
 *
 * Minimum firmware version required: 2.0.0
 */
    typedef enum Calypso_ATDevice_TransparentModeSecureMethod_t
    {
        Calypso_ATDevice_TransparentModeSecureMethod_None,
        Calypso_ATDevice_TransparentModeSecureMethod_SSLv3,
        Calypso_ATDevice_TransparentModeSecureMethod_TLSv1,
        Calypso_ATDevice_TransparentModeSecureMethod_TLSv1_1,
        Calypso_ATDevice_TransparentModeSecureMethod_TLSv1_2,
        Calypso_ATDevice_TransparentModeSecureMethod_SSLv3_TLSv1_2,
        Calypso_ATDevice_TransparentModeSecureMethod_NumberOfValues,
    } Calypso_ATDevice_TransparentModeSecureMethod_t;

    typedef enum Calypso_ATDevice_TransparentModeUartTrigger_t
    {
        Calypso_ATDevice_TransparentModeUartTrigger_Timer = (1 << 0),       /**< Data is transmitted if a pause of configurable length is detected in the data stream */
        Calypso_ATDevice_TransparentModeUartTrigger_OneETX = (1 << 1),      /**< Data is transmitted if the first ETX character has been received */
        Calypso_ATDevice_TransparentModeUartTrigger_TwoETX = (1 << 2),      /**< Data is transmitted if the full ETX (two characters) has been received */
        Calypso_ATDevice_TransparentModeUartTrigger_TransmitETX = (1 << 3), /**< If this option is set and OneETX or TwoETX is set, the ETX characters are not removed from the data stream but are forwarded to the socket */
        Calypso_ATDevice_TransparentModeUartTrigger_NumberOfValues = 4
    } Calypso_ATDevice_TransparentModeUartTrigger_t;

    /**
 * @brief Status flags returned when calling Calypso_ATDevice_Get() with Calypso_ATDevice_GetId_Status.
 */
    typedef enum Calypso_ATDevice_StatusFlags_t
    {
        Calypso_ATDevice_StatusFlags_GeneralError = (1 << 0),
        Calypso_ATDevice_StatusFlags_WlanAsyncConnectedResponse = (1 << 1),
        Calypso_ATDevice_StatusFlags_WlanAsyncDisconnectedResponse = (1 << 2),
        Calypso_ATDevice_StatusFlags_StationConnected = (1 << 3),
        Calypso_ATDevice_StatusFlags_StationDisconnected = (1 << 4),
        Calypso_ATDevice_StatusFlags_P2PDeviceFound = (1 << 5),
        Calypso_ATDevice_StatusFlags_ConnectionFailed = (1 << 6),
        Calypso_ATDevice_StatusFlags_P2PNegativeRequestReceived = (1 << 7),
        Calypso_ATDevice_StatusFlags_RxFilters = (1 << 8),
        Calypso_ATDevice_StatusFlags_WlanStationConnected = (1 << 9),
        Calypso_ATDevice_StatusFlags_TxFailed = (1 << 10),
        Calypso_ATDevice_StatusFlags_IPAcquired = (1 << 11),
        Calypso_ATDevice_StatusFlags_IPAcquiredV6 = (1 << 12),
        Calypso_ATDevice_StatusFlags_IPLeased = (1 << 13),
        Calypso_ATDevice_StatusFlags_IPReleased = (1 << 14),
        Calypso_ATDevice_StatusFlags_IPv4Lost = (1 << 15),
        Calypso_ATDevice_StatusFlags_DHCPAcquireTimeout = (1 << 16),
        Calypso_ATDevice_StatusFlags_IPCollision = (1 << 17),
        Calypso_ATDevice_StatusFlags_IPv6Lost = (1 << 18),
        Calypso_ATDevice_StatusFlags_NumberOfValues = 19
    } Calypso_ATDevice_StatusFlags_t;

    /**
 * @brief Structure used for passing date/time data.
 */
    typedef struct Calypso_ATDevice_Time_t
    {
        uint8_t hour;   /**< Hour of day (0..23) */
        uint8_t minute; /**< Minute (0..59) */
        uint8_t second; /**< Second (0..59) */
        uint8_t day;    /**< Day (0..31) */
        uint8_t month;  /**< Month (1..12) */
        uint16_t year;  /**< Year */
    } Calypso_ATDevice_Time_t;

    /**
 * @brief Device version info.
 */
    typedef struct Calypso_ATDevice_Version_t
    {
        char chipId[32];                 /**< Chip ID */
        char MACVersion[32];             /**< MAC version (x.x.x.x) */
        char PHYVersion[32];             /**< PHY version (x.x.x.x) */
        char NWPVersion[32];             /**< NWP version (x.x.x.x) */
        char ROMVersion[32];             /**< ROM version (x) */
        char calypsoFirmwareVersion[32]; /**< Calypso firmware version (x.x.x). Only returned by firmware versions >= 1.9.0 */
    } Calypso_ATDevice_Version_t;

    /**
 * @brief General parameters (used by Calypso_ATDevice_Get() and Calypso_ATDevice_Set() with Calypso_ATDevice_GetId_General).
 */
    typedef union Calypso_ATDevice_Value_General_t
    {
        Calypso_ATDevice_Version_t version; /**< Version info, read only */
        Calypso_ATDevice_Time_t time;       /**< Current time */
        uint8_t persistent;                 /**< Persistent (1=enable, 0=disable) */
    } Calypso_ATDevice_Value_General_t;

    /**
 * @brief IOT parameters (used by Calypso_ATDevice_Get() and Calypso_ATDevice_Set() with Calypso_ATDevice_GetId_IOT).
 */
    typedef union Calypso_ATDevice_Value_IOT_t
    {
        char udid[36]; /**< 16 byte UDID, read only */
    } Calypso_ATDevice_Value_IOT_t;

    /**
 * @brief UART parameters (used by Calypso_ATDevice_Get() and Calypso_ATDevice_Set() with Calypso_ATDevice_GetId_UART).
 */
    typedef union Calypso_ATDevice_Value_UART_t
    {
        uint32_t baudrate;                    /**< UART baud rate */
        Calypso_ATDevice_UartParity_t parity; /**< UART parity 0=none, 1=even, 2=odd) */
        bool flowControl;                     /**< UART flow control false=flow control disabled, true=flow control enabled */
        uint8_t transparentTrigger;           /**< Bitmask defining UART trigger used in transparent mode (see Calypso_ATDevice_TransparentModeUartTrigger_t) */
        uint16_t transparentTimeoutMs;        /**< Timeout used for triggering transmission in transparent mode (see Calypso_ATDevice_TransparentModeUartTrigger_Timer) */
        char transparentETX[2];               /**< Two byte ETX used in transparent mode (see Calypso_ATDevice_TransparentModeUartTrigger_t)*/
    } Calypso_ATDevice_Value_UART_t;

    /**
 * @brief Transparent mode parameters (used by Calypso_ATDevice_Get() and Calypso_ATDevice_Set() with Calypso_ATDevice_GetId_TransparentMode).
 *
 * Minimum firmware version required: 2.0.0
 */
    typedef union Calypso_ATDevice_Value_TransparentMode_t
    {
        char remoteAddress[32];                                      /**< IP of the peer device */
        uint16_t remotePort;                                         /**< Port of the peer device */
        uint16_t localPort;                                          /**< Local port */
        Calypso_ATDevice_TransparentModeSocketType_t socketType;     /**< Type of socket used to communicate with peer */
        Calypso_ATDevice_TransparentModeSecureMethod_t secureMethod; /**< Socket security method */
        bool powerSave;                                              /**< Enable power save mode */
        bool skipDateVerify;                                         /**< Skip certificate date verification (only applicable if using secure sockets) */
        bool disableCertificateStore;                                /**< Disable verification of root CA using the module's certificate store (only applicable if using secure sockets) */
    } Calypso_ATDevice_Value_TransparentMode_t;

    /**
 * @brief GPIO parameters (used by Calypso_ATDevice_Get() and Calypso_ATDevice_Set() with Calypso_ATDevice_GetId_GPIO).
 *
 * Minimum firmware version required: 2.0.0
 */
    typedef union Calypso_ATDevice_Value_GPIO_t
    {
        bool remoteLock;
    } Calypso_ATDevice_Value_GPIO_t;

    /**
 * @brief Used as parameter Calypso_ATDevice_Get() and Calypso_ATDevice_Set().
 * The member that is written to or read from depends on the ID and option parameters.
 *
 * @see Calypso_ATDevice_Get(), Calypso_ATDevice_Set(), Calypso_ATDevice_GetId_t
 */
    typedef union Calypso_ATDevice_Value_t
    {
        uint32_t status;                                          /**< Status flags (response to getting device status, see Calypso_ATDevice_StatusFlags_t) */
        Calypso_ATDevice_Value_General_t general;                 /**< General device parameters, used with Calypso_ATDevice_GetId_General */
        Calypso_ATDevice_Value_IOT_t iot;                         /**< IOT device parameters, used with Calypso_ATDevice_GetId_IOT */
        Calypso_ATDevice_Value_UART_t uart;                       /**< UART device parameters, used with Calypso_ATDevice_GetId_UART */
        Calypso_ATDevice_Value_TransparentMode_t transparentMode; /**< Transparent mode parameters, used with Calypso_ATDevice_GetId_TransparentMode. Minimum firmware version required: 2.0.0 */
        Calypso_ATDevice_Value_GPIO_t gpio;                       /**< GPIO parameters */
    } Calypso_ATDevice_Value_t;

    extern bool Calypso_ATDevice_Test();
    extern bool Calypso_ATDevice_Start();
    extern bool Calypso_ATDevice_Stop(uint32_t timeoutMs);
    extern bool Calypso_ATDevice_Restart(uint32_t timeoutMs);
    extern bool Calypso_ATDevice_Reboot();
    extern bool Calypso_ATDevice_FactoryReset();
    extern bool Calypso_ATDevice_Sleep(uint32_t timeoutSeconds);
    extern bool Calypso_ATDevice_PowerSave();
    extern bool Calypso_ATDevice_Get(Calypso_ATDevice_GetId_t id, uint8_t option, Calypso_ATDevice_Value_t* pValue);
    extern bool Calypso_ATDevice_Set(Calypso_ATDevice_GetId_t id, uint8_t option, Calypso_ATDevice_Value_t* pValue);
    extern bool Calypso_ATDevice_StartProvisioning();
    extern bool Calypso_ATDevice_PrintStatusFlags(uint32_t flags, char* pOutStr, size_t maxLength);

#ifdef __cplusplus
}
#endif

#endif /* CALYPSO_AT_DEVICE_H_INCLUDED */
