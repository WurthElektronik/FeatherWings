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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief AT commands for basic device functionality.
 */

#ifndef AT_DEVICE_H_INCLUDED
#define AT_DEVICE_H_INCLUDED

#include <ATCommands.h>
#include <Calypso.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#define ATDEVICE_SLEEP_MIN_TIMEOUT (uint32_t) 1               /**< Minimum timeout for sleep command */
#define ATDEVICE_SLEEP_MAX_TIMEOUT (uint32_t) 86400           /**< Maximum timeout for sleep command */
#define ATDEVICE_STOP_MAX_TIMEOUT  (uint16_t) 0xFFFF          /**< Maximum timeout for stop command */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ID parameter for AT+get=[ID] command.
 */
typedef enum ATDevice_GetId_t
{
    ATDevice_GetId_Status,
    ATDevice_GetId_General,
    ATDevice_GetId_IOT,
    ATDevice_GetId_UART,
    ATDevice_GetId_TransparentMode,
    ATDevice_GetId_GPIO,
    ATDevice_GetId_NumberOfValues
} ATDevice_GetId_t;

/**
 * @brief Option parameter for AT+get=status,[Option] command.
 */
typedef enum ATDevice_GetStatus_t
{
    ATDevice_GetStatus_Device,
    ATDevice_GetStatus_WLAN,
    ATDevice_GetStatus_BSD,
    ATDevice_GetStatus_NetApp,
    ATDevice_GetStatus_NumberOfValues
} ATDevice_GetStatus_t;

/**
 * @brief Option parameter for AT+get=general,[Option] command.
 */
typedef enum ATDevice_GetGeneral_t
{
    ATDevice_GetGeneral_Version,
    ATDevice_GetGeneral_Time,
    ATDevice_GetGeneral_Persistent,
    ATDevice_GetGeneral_NumberOfValues
} ATDevice_GetGeneral_t;

/**
 * @brief Option parameter for AT+get=IOT,[Option] command.
 */
typedef enum ATDevice_GetIot_t
{
    ATDevice_GetIot_UDID,
    ATDevice_GetIot_NumberOfValues
} ATDevice_GetIot_t;

/**
 * @brief Option parameter for AT+get=UART,[Option] command.
 */
typedef enum ATDevice_GetUart_t
{
    ATDevice_GetUart_Baudrate,
    ATDevice_GetUart_Parity,
    ATDevice_GetUart_FlowControl,
    ATDevice_GetUart_TransparentTrigger,
    ATDevice_GetUart_TransparentTimeout,
    ATDevice_GetUart_TransparentETX,
    ATDevice_GetUart_NumberOfValues
} ATDevice_GetUart_t;

/**
 * @brief Option parameter for AT+get=transparent_mode,[Option] command.
 *
 * Minimum firmware version required: 2.0.0
 */
typedef enum ATDevice_GetTransparentMode_t
{
    ATDevice_GetTransparentMode_RemoteAddress,
    ATDevice_GetTransparentMode_RemotePort,
    ATDevice_GetTransparentMode_LocalPort,
    ATDevice_GetTransparentMode_SocketType,
    ATDevice_GetTransparentMode_SecureMethod,
    ATDevice_GetTransparentMode_PowerSave,
    ATDevice_GetTransparentMode_SkipDateVerify,
    ATDevice_GetTransparentMode_DisableCertificateStore,
    ATDevice_GetTransparentMode_NumberOfValues
} ATDevice_GetTransparentMode_t;

/**
 * @brief Option parameter for AT+get=GPIO,[Option] command.
 *
 * Minimum firmware version required: 2.0.0
 */
typedef enum ATDevice_GetGPIO_t
{
    ATDevice_GetGPIO_RemoteLock,
    ATDevice_GetGPIO_NumberOfValues
} ATDevice_GetGPIO_t;

/**
 * @brief UART parity parameter.
 */
typedef enum ATDevice_UartParity_t
{
    ATDevice_UartParity_None = 0,
    ATDevice_UartParity_Even = 1,
    ATDevice_UartParity_Odd = 2,
} ATDevice_UartParity_t;

/**
 * @brief Socket types for communicating with peer in transparent mode.
 *
 * Minimum firmware version required: 2.0.0
 */
typedef enum ATDevice_TransparentModeSocketType_t
{
    ATDevice_TransparentModeSocketType_UDP,             /**< Send/receive data to/from a UDP device */
    ATDevice_TransparentModeSocketType_TCPServer,       /**< Create a TCP server and send/receive data to/from the first peer device that connects */
    ATDevice_TransparentModeSocketType_TCPClient,       /**< Create a TCP connection to a TCP server and send/receive data to/from it */
    ATDevice_TransparentModeSocketType_NumberOfValues
} ATDevice_TransparentModeSocketType_t;

/**
 * @brief Socket secure method types for communicating with peer in transparent mode.
 *
 * Minimum firmware version required: 2.0.0
 */
typedef enum ATDevice_TransparentModeSecureMethod_t
{
    ATDevice_TransparentModeSecureMethod_None,
    ATDevice_TransparentModeSecureMethod_SSLv3,
    ATDevice_TransparentModeSecureMethod_TLSv1,
    ATDevice_TransparentModeSecureMethod_TLSv1_1,
    ATDevice_TransparentModeSecureMethod_TLSv1_2,
    ATDevice_TransparentModeSecureMethod_SSLv3_TLSv1_2,
    ATDevice_TransparentModeSecureMethod_NumberOfValues,
} ATDevice_TransparentModeSecureMethod_t;

typedef enum ATDevice_TransparentModeUartTrigger_t
{
    ATDevice_TransparentModeUartTrigger_Timer           = (1 << 0),     /**< Data is transmitted if a pause of configurable length is detected in the data stream */
    ATDevice_TransparentModeUartTrigger_OneETX          = (1 << 1),     /**< Data is transmitted if the first ETX character has been received */
    ATDevice_TransparentModeUartTrigger_TwoETX          = (1 << 2),     /**< Data is transmitted if the full ETX (two characters) has been received */
    ATDevice_TransparentModeUartTrigger_TransmitETX     = (1 << 3),     /**< If this option is set and OneETX or TwoETX is set, the ETX characters are not removed from the data stream but are forwarded to the socket */
    ATDevice_TransparentModeUartTrigger_NumberOfValues  = 4
} ATDevice_TransparentModeUartTrigger_t;

/**
 * @brief Status flags returned when calling ATDevice_Get() with ATDevice_GetId_Status.
 */
typedef enum ATDevice_StatusFlags_t
{
    ATDevice_StatusFlags_GeneralError                       = (1 << 0),
    ATDevice_StatusFlags_WlanAsyncConnectedResponse         = (1 << 1),
    ATDevice_StatusFlags_WlanAsyncDisconnectedResponse      = (1 << 2),
    ATDevice_StatusFlags_StationConnected                   = (1 << 3),
    ATDevice_StatusFlags_StationDisconnected                = (1 << 4),
    ATDevice_StatusFlags_P2PDeviceFound                     = (1 << 5),
    ATDevice_StatusFlags_ConnectionFailed                   = (1 << 6),
    ATDevice_StatusFlags_P2PNegativeRequestReceived         = (1 << 7),
    ATDevice_StatusFlags_RxFilters                          = (1 << 8),
    ATDevice_StatusFlags_WlanStationConnected               = (1 << 9),
    ATDevice_StatusFlags_TxFailed                           = (1 << 10),
    ATDevice_StatusFlags_IPAcquired                         = (1 << 11),
    ATDevice_StatusFlags_IPAcquiredV6                       = (1 << 12),
    ATDevice_StatusFlags_IPLeased                           = (1 << 13),
    ATDevice_StatusFlags_IPReleased                         = (1 << 14),
    ATDevice_StatusFlags_IPv4Lost                           = (1 << 15),
    ATDevice_StatusFlags_DHCPAcquireTimeout                 = (1 << 16),
    ATDevice_StatusFlags_IPCollision                        = (1 << 17),
    ATDevice_StatusFlags_IPv6Lost                           = (1 << 18),
    ATDevice_StatusFlags_NumberOfValues                     = 19
} ATDevice_StatusFlags_t;

/**
 * @brief Structure used for passing date/time data.
 */
typedef struct ATDevice_Time_t
{
    uint8_t hour;           /**< Hour of day (0..23) */
    uint8_t minute;         /**< Minute (0..59) */
    uint8_t second;         /**< Second (0..59) */
    uint8_t day;            /**< Day (0..31) */
    uint8_t month;          /**< Month (1..12) */
    uint16_t year;          /**< Year */
} ATDevice_Time_t;

/**
 * @brief Device version info.
 */
typedef struct ATDevice_Version_t
{
    char chipId[32];                    /**< Chip ID */
    char MACVersion[32];                /**< MAC version (x.x.x.x) */
    char PHYVersion[32];                /**< PHY version (x.x.x.x) */
    char NWPVersion[32];                /**< NWP version (x.x.x.x) */
    char ROMVersion[32];                /**< ROM version (x) */
    char calypsoFirmwareVersion[32];    /**< Calypso firmware version (x.x.x). Only returned by firmware versions >= 1.9.0 */
} ATDevice_Version_t;

/**
 * @brief General parameters (used by ATDevice_Get() and ATDevice_Set() with ATDevice_GetId_General).
 */
typedef union ATDevice_Value_General_t
{
    ATDevice_Version_t version;     /**< Version info, read only */
    ATDevice_Time_t time;           /**< Current time */
    uint8_t persistent;             /**< Persistent (1=enable, 0=disable) */
} ATDevice_Value_General_t;

/**
 * @brief IOT parameters (used by ATDevice_Get() and ATDevice_Set() with ATDevice_GetId_IOT).
 */
typedef union ATDevice_Value_IOT_t
{
    char udid[36];                 /**< 16 byte UDID, read only */
} ATDevice_Value_IOT_t;

/**
 * @brief UART parameters (used by ATDevice_Get() and ATDevice_Set() with ATDevice_GetId_UART).
 */
typedef union ATDevice_Value_UART_t
{
    uint32_t baudrate;              /**< UART baud rate */
    ATDevice_UartParity_t parity;   /**< UART parity 0=none, 1=even, 2=odd) */
    bool flowControl;               /**< UART flow control false=flow control disabled, true=flow control enabled */
    uint8_t transparentTrigger;     /**< Bitmask defining UART trigger used in transparent mode (see ATDevice_TransparentModeUartTrigger_t) */
    uint16_t transparentTimeoutMs;  /**< Timeout used for triggering transmission in transparent mode (see ATDevice_TransparentModeUartTrigger_Timer) */
    char transparentETX[2];         /**< Two byte ETX used in transparent mode (see ATDevice_TransparentModeUartTrigger_t)*/
} ATDevice_Value_UART_t;

/**
 * @brief Transparent mode parameters (used by ATDevice_Get() and ATDevice_Set() with ATDevice_GetId_TransparentMode).
 *
 * Minimum firmware version required: 2.0.0
 */
typedef union ATDevice_Value_TransparentMode_t
{
    char remoteAddress[32];                                 /**< IP of the peer device */
    uint16_t remotePort;                                    /**< Port of the peer device */
    uint16_t localPort;                                     /**< Local port */
    ATDevice_TransparentModeSocketType_t socketType;        /**< Type of socket used to communicate with peer */
    ATDevice_TransparentModeSecureMethod_t secureMethod;    /**< Socket security method */
    bool powerSave;                                         /**< Enable power save mode */
    bool skipDateVerify;                                    /**< Skip certificate date verification (only applicable if using secure sockets) */
    bool disableCertificateStore;                           /**< Disable verification of root CA using the module's certificate store (only applicable if using secure sockets) */
} ATDevice_Value_TransparentMode_t;

/**
 * @brief GPIO parameters (used by ATDevice_Get() and ATDevice_Set() with ATDevice_GetId_GPIO).
 *
 * Minimum firmware version required: 2.0.0
 */
typedef union ATDevice_Value_GPIO_t
{
    bool remoteLock;
} ATDevice_Value_GPIO_t;

/**
 * @brief Used as parameter ATDevice_Get() and ATDevice_Set().
 * The member that is written to or read from depends on the ID and option parameters.
 *
 * @see ATDevice_Get(), ATDevice_Set(), ATDevice_GetId_t
 */
typedef union ATDevice_Value_t
{
    uint32_t status;                                    /**< Status flags (response to getting device status, see ATDevice_StatusFlags_t) */
    ATDevice_Value_General_t general;                   /**< General device parameters, used with ATDevice_GetId_General */
    ATDevice_Value_IOT_t iot;                           /**< IOT device parameters, used with ATDevice_GetId_IOT */
    ATDevice_Value_UART_t uart;                         /**< UART device parameters, used with ATDevice_GetId_UART */
    ATDevice_Value_TransparentMode_t transparentMode;   /**< Transparent mode parameters, used with ATDevice_GetId_TransparentMode. Minimum firmware version required: 2.0.0 */
    ATDevice_Value_GPIO_t gpio;                         /**< GPIO parameters */
} ATDevice_Value_t;

extern bool ATDevice_Test();
extern bool ATDevice_Start();
extern bool ATDevice_Stop(uint32_t timeoutMs);
extern bool ATDevice_Restart(uint32_t timeoutMs);
extern bool ATDevice_Reboot();
extern bool ATDevice_FactoryReset();
extern bool ATDevice_Sleep(uint32_t timeoutSeconds);
extern bool ATDevice_PowerSave();
extern bool ATDevice_Get(ATDevice_GetId_t id, uint8_t option, ATDevice_Value_t *pValue);
extern bool ATDevice_Set(ATDevice_GetId_t id, uint8_t option, ATDevice_Value_t *pValue);
extern bool ATDevice_StartProvisioning();
extern bool ATDevice_PrintStatusFlags(uint32_t flags, char *pOutStr, size_t maxLength);

#ifdef __cplusplus
}
#endif

#endif /* AT_DEVICE_H_INCLUDED */
