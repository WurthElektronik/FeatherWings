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
 * @brief AT commands for network application control.
 */

#ifndef AT_NETAPP_H_INCLUDED
#define AT_NETAPP_H_INCLUDED

#include <ATCommands.h>
#include <Calypso.h>
#include <stdbool.h>
#include <stdint.h>

#include "ATSocket.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Network applications (flags).
 * Used to start/stop network applications via ATNetApp_StartApplications() / ATNetApp_StopApplications()
 * as well as for configuring the applications via ATNetApp_Set() / ATNetApp_Get().
 */
typedef enum ATNetApp_Application_t
{
    ATNetApp_Application_None              = 0,
    ATNetApp_Application_HttpServer        = (1 << 0),     /**< HTTP server */
    ATNetApp_Application_DhcpServer        = (1 << 1),     /**< DHCP server */
    ATNetApp_Application_mDns              = (1 << 2),     /**< mDNS (multicast-DNS) */
    ATNetApp_Application_DnsServer         = (1 << 3),     /**< DNS server */
    ATNetApp_Application_SntpClient        = (1 << 4),     /**< SNTP client */
    ATNetApp_Application_DnsClient         = (1 << 5),     /**< DNS client (can't be started/stopped, configuration only) */
    ATNetApp_Application_Device            = (1 << 6),     /**< Device (can't be started/stopped, configuration only) */
    ATNetApp_Application_NumberOfValues    = 7
} ATNetApp_Application_t;

/**
 * @brief HTTP server options.
 * See ATNetApp_Set() / ATNetApp_Get() and ATNetApp_Application_HttpServer.
 */
typedef enum ATNetApp_HttpOption_t
{
    ATNetApp_HttpOption_PrimPortNum,
    ATNetApp_HttpOption_AuthCheck,
    ATNetApp_HttpOption_AuthName,
    ATNetApp_HttpOption_AuthPassword,
    ATNetApp_HttpOption_AuthRealm,
    ATNetApp_HttpOption_RomPagesAccess,
    ATNetApp_HttpOption_SecondPortNum,
    ATNetApp_HttpOption_SecondPortEn,
    ATNetApp_HttpOption_PrimPortSecEn,
    ATNetApp_HttpOption_PrivKeyFile,
    ATNetApp_HttpOption_DevCertFile,
    ATNetApp_HttpOption_CaCertFile,
    ATNetApp_HttpOption_TmpRegisterService,
    ATNetApp_HttpOption_TmpUnregisterService,
    ATNetApp_HttpOption_NumberOfValues
} ATNetApp_HttpOption_t;

/**
 * @brief DHCP server options.
 * See ATNetApp_Set() / ATNetApp_Get() and ATNetApp_Application_DhcpServer.
 */
typedef enum ATNetApp_DhcpOption_t
{
    ATNetApp_DhcpOption_Basic,
    ATNetApp_DhcpOption_NumberOfValues
} ATNetApp_DhcpOption_t;

/**
 * @brief mDNS options.
 * See ATNetApp_Set() / ATNetApp_Get() and ATNetApp_Application_mDns.
 */
typedef enum ATNetApp_mDnsOption_t
{
    ATNetApp_mDnsOption_ContQuery,
    ATNetApp_mDnsOption_QeventMask,
    ATNetApp_mDnsOption_TimingParams,
    ATNetApp_mDnsOption_NumberOfValues
} ATNetApp_mDnsOption_t;

/**
 * @brief mDns event mask (flags).
 * See ATNetApp_Set() / ATNetApp_Get() and ATNetApp_Application_mDns / ATNetApp_mDnsOption_QeventMask.
 */
typedef enum ATNetApp_mDnsEventMask_t
{
    ATNetApp_mDnsEventMask_IPP              = (1 << 0),
    ATNetApp_mDnsEventMask_DeviceInfo       = (1 << 1),
    ATNetApp_mDnsEventMask_HTTP             = (1 << 2),
    ATNetApp_mDnsEventMask_HTTPS            = (1 << 3),
    ATNetApp_mDnsEventMask_Workstation      = (1 << 4),
    ATNetApp_mDnsEventMask_GUID             = (1 << 5),
    ATNetApp_mDnsEventMask_H323             = (1 << 6),
    ATNetApp_mDnsEventMask_NTP              = (1 << 7),
    ATNetApp_mDnsEventMask_Objective        = (1 << 8),
    ATNetApp_mDnsEventMask_RDP              = (1 << 9),
    ATNetApp_mDnsEventMask_Remote           = (1 << 10),
    ATNetApp_mDnsEventMask_RTSP             = (1 << 11),
    ATNetApp_mDnsEventMask_SIP              = (1 << 12),
    ATNetApp_mDnsEventMask_SMB              = (1 << 13),
    ATNetApp_mDnsEventMask_SOAP             = (1 << 14),
    ATNetApp_mDnsEventMask_SSH              = (1 << 15),
    ATNetApp_mDnsEventMask_Telnet           = (1 << 16),
    ATNetApp_mDnsEventMask_TFTP             = (1 << 17),
    ATNetApp_mDnsEventMask_XMPP             = (1 << 18),
    ATNetApp_mDnsEventMask_RAOP             = (1 << 19),
    ATNetApp_mDnsEventMask_NumberOfValues   = 20
} ATNetApp_mDnsEventMask_t;

/**
 * @brief SNTP client options.
 * See ATNetApp_Set() / ATNetApp_Get() and ATNetApp_Application_SntpClient.
 */
typedef enum ATNetApp_SntpOption_t
{
    ATNetApp_SntpOption_Enable,
    ATNetApp_SntpOption_UpdateInterval,
    ATNetApp_SntpOption_TimeZone,
    ATNetApp_SntpOption_Servers,
    ATNetApp_SntpOption_NumberOfValues
} ATNetApp_SntpOption_t;

/**
 * @brief DNS client options
 * See ATNetApp_Set() / ATNetApp_Get() and ATNetApp_Application_DnsClient.
 */
typedef enum ATNetApp_DnsClientOption_t
{
    ATNetApp_DnsClientOption_Time,
    ATNetApp_DnsClientOption_NumberOfValues
} ATNetApp_DnsClientOption_t;

/**
 * @brief Device options
 * See ATNetApp_Set() / ATNetApp_Get() and ATNetApp_Application_Device.
 */
typedef enum ATNetApp_DeviceOption_t
{
    ATNetApp_DeviceOption_URN,
    ATNetApp_DeviceOption_Domain,
    ATNetApp_DeviceOption_NumberOfValues
} ATNetApp_DeviceOption_t;

/**
 * @brief HTTP server option values.
 */
typedef union ATNetApp_HttpOptionValue_t
{
    uint16_t primPortNum;                           /**< Primary port number */
    uint8_t authCheck;                              /**< Authentication check (0=disabled, 1=enabled) */
    char authName[20];                              /**< Authentication name */
    char authPassword[20];                          /**< Authentication password */
    char authRealm[20];                             /**< Authorization realm */
    uint8_t romPagesAccess;                         /**< ROM pages access (0=disabled, 1=enabled) */
    uint16_t secondPortNum;                         /**< Secondary port number */
    uint8_t secondPortEn;                           /**< Secondary port enable (0=disabled, 1=enabled) */
    uint8_t primPortSecEn;                          /**< Primary port security enable (0=disabled, 1=enabled) */
    char privKeyFile[96];                           /**< Private key file */
    char devCertFile[96];                           /**< Device certificate file */
    char caCertFile[96];                            /**< CA certificate file */
    char tmpRegisterService[80];                    /**< Unregister service name for mDNS */
    char tmpUnregisterService[80];                  /**< Register service name for mDNS */
} ATNetApp_HttpOptionValue_t;

/**
 * @brief DHCP server basic option values.
 */
typedef struct ATNetApp_DhcpBasicOption_t
{
    uint32_t leaseTime;                             /**< Lease time in seconds */
    char firstIpAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];  /**< First IP address for allocation */
    char lastIpAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];   /**< Last IP address for allocation */
} ATNetApp_DhcpBasicOption_t;

/**
 * @brief DHCP server option values.
 */
typedef union ATNetApp_DhcpOptionValue_t
{
    ATNetApp_DhcpBasicOption_t basic;               /**< Basic DHCP server parameters */
} ATNetApp_DhcpOptionValue_t;

/**
 * @brief mDNS timing parameter values.
 */
typedef struct ATNetApp_mDnsTimingParams_t
{
    uint32_t period;                                /**< Period in ticks (100 ticks = 1 second) */
    uint32_t repetitions;                           /**< Repetitions */
    uint32_t telescopicFactor;                      /**< Telescopic factor */
    uint32_t retransmissionInterval;                /**< Retransmission interval */
    uint32_t maxPeriodInterval;                     /**< Maximum period interval */
    uint32_t maxTime;                               /**< Maximum time */

} ATNetApp_mDnsTimingParams_t;

/**
 * @brief mDNS option values.
 */
typedef union ATNetApp_mDnsOptionValue_t
{
    char contQuery[81];                             /**< Service name */
    uint32_t qeventMask;                            /**< Event mask */
    ATNetApp_mDnsTimingParams_t timingParams;       /**< Timing parameters */

} ATNetApp_mDnsOptionValue_t;

/**
 * @brief SNTP client option values.
 */
typedef union ATNetApp_SntpOptionValue_t
{
    uint8_t enable;                                 /**< Enables or disables the SNTP client (0=disabled, 1=enabled) */
    uint32_t updateInterval;                        /**< Minimum update interval in seconds */
    int16_t timeZone;                               /**< UTC +/- minutes */
    char servers[3][CALYPSO_MAX_HOST_NAME_LENGTH];       /**< List of three server addresses / URLS */
} ATNetApp_SntpOptionValue_t;

/**
 * @brief DNS client time option values.
 */
typedef struct ATNetApp_DnsClientTime_t
{
    uint32_t maxResponseTime;                       /**< Maximum response time in milliseconds */
    uint16_t numberOfRetries;                       /**< Number of retries */
} ATNetApp_DnsClientTime_t;

/**
 * @brief DNS client option values.
 */
typedef union ATNetApp_DnsClientOptionValue_t
{
    ATNetApp_DnsClientTime_t time;                  /**< Time parameters */
} ATNetApp_DnsClientOptionValue_t;

/**
 * @brief Device option values.
 */
typedef union ATNetApp_DeviceOptionValue_t
{
    char urn[33];                                   /**< Device name */
    char domain[63];                                /**< Domain name */
} ATNetApp_DeviceOptionValue_t;

/**
 * @brief Option value struct.
 * Used by ATNetApp_Set() and ATNetApp_Get().
 */
typedef union ATNetApp_OptionValue_t
{
    ATNetApp_HttpOptionValue_t http;                /**< HTTP server options */
    ATNetApp_DhcpOptionValue_t dhcp;                /**< DHCP server options */
    ATNetApp_mDnsOptionValue_t mDns;                /**< mDNS options */
    ATNetApp_SntpOptionValue_t sntp;                /**< SNTP client options */
    ATNetApp_DnsClientOptionValue_t dnsClient;      /**< DNS client options */
    ATNetApp_DeviceOptionValue_t device;            /**< Device options */
} ATNetApp_OptionValue_t;

/**
 * @brief Ping mode.
 */
typedef enum ATNetApp_PingMode_t
{
    ATNetApp_PingMode_ReportWhenComplete,           /**< Report once all pings are done */
    ATNetApp_PingMode_ReportAfterEveryPing,         /**< Report after every ping */
    ATNetApp_PingMode_StopAfterFirstPing,           /**< Stop after first successful ping */
    ATNetApp_PingMode_NumberOfValues
} ATNetApp_PingMode_t;

/**
 * @brief Host / IP lookup result.
 * @see ATNetApp_GetHostByName()
 */
typedef struct ATNetApp_GetHostByNameResult_t
{
    char hostName[CALYPSO_MAX_HOST_NAME_LENGTH];         /**< Name of looked up host */
    char hostAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];     /**< Address of looked up host */
} ATNetApp_GetHostByNameResult_t;

/**
 * @brief Ping parameters.
 * @see ATNetApp_Ping()
 */
typedef struct ATNetApp_PingParameters_t
{
    ATSocket_Family_t family;                       /**< INET or INET6 */
    char destination[CALYPSO_MAX_IP_ADDRESS_LENGTH];     /**< Destination IP address (0 to stop an ongoing ping) */
    uint16_t size;                                  /**< Size of ping in bytes */
    uint16_t delayMs;                               /**< Delay between pings in milliseconds */
    uint16_t timeoutMs;                             /**< Timeout for each ping in milliseconds */
    uint16_t maxNumberOfPings;                      /**< Number of pings to send (0 = forever) */
    ATNetApp_PingMode_t mode;                       /**< Ping mode. See ATNetApp_PingMode_t. */
} ATNetApp_PingParameters_t;

extern bool ATNetApp_StartApplications(uint8_t apps);
extern bool ATNetApp_StopApplications(uint8_t apps);

extern bool ATNetApp_Set(ATNetApp_Application_t app, uint8_t option, ATNetApp_OptionValue_t *value);
extern bool ATNetApp_Get(ATNetApp_Application_t app, uint8_t option, ATNetApp_OptionValue_t *value);

extern bool ATNetApp_GetHostByName(const char *hostName,
                                   ATSocket_Family_t family,
                                   ATNetApp_GetHostByNameResult_t *lookupResult);

extern bool ATNetApp_Ping(ATNetApp_PingParameters_t *parameters);

extern bool ATNetApp_UpdateTime();

#ifdef __cplusplus
}
#endif

#endif /* AT_NETAPP_H_INCLUDED */
