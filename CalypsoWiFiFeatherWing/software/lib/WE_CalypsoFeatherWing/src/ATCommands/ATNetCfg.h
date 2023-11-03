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
 * @brief AT commands for network configuration.
 */

#ifndef AT_NETCFG_H_INCLUDED
#define AT_NETCFG_H_INCLUDED

#include <ATCommands.h>
#include <Calypso.h>
#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Network interface mode flags used by ATNetCfg_SetInterfaceModes().
 */
typedef enum ATNetCfg_InterfaceMode_t
{
    ATNetCfg_InterfaceMode_None                 = 0,            /**< None (all disabled) */
    ATNetCfg_InterfaceMode_IPv6StationLocal     = 1 << 0,       /**< Enable IPv6 local */
    ATNetCfg_InterfaceMode_IPv6StationGlobal    = 1 << 1,       /**< Enable IPv6 global */
    ATNetCfg_InterfaceMode_DisableIPv4Dhcp      = 1 << 2,       /**< Disable IPv4 DHCP */
    ATNetCfg_InterfaceMode_IPv6LocalStatic      = 1 << 3,       /**< Enable IPv6 local static */
    ATNetCfg_InterfaceMode_IPv6LocalStateless   = 1 << 4,       /**< Enable IPv6 local stateless */
    ATNetCfg_InterfaceMode_IPv6LocalStateful    = 1 << 5,       /**< Enable IPv6 local stateful */
    ATNetCfg_InterfaceMode_IPv6GlobalStatic     = 1 << 6,       /**< Enable IPv6 global static */
    ATNetCfg_InterfaceMode_IPv6GlobalStateful   = 1 << 7,       /**< Enable IPv6 global stateful */
    ATNetCfg_InterfaceMode_DisableIPv4Lla       = 1 << 8,       /**< Disable LLA feature */
    ATNetCfg_InterfaceMode_EnableDhcpRelease    = 1 << 9,       /**< Enable DHCP release */
    ATNetCfg_InterfaceMode_IPv6GlobalStateless  = 1 << 10,      /**< Enable IPv6 global stateless */
    ATNetCfg_InterfaceMode_DisableFastRenew     = 1 << 11,      /**< Fast renew disabled */
    ATNetCfg_InterfaceMode_NumberOfValues       = 12            /**< Number of flags in this enumeration */
} ATNetCfg_InterfaceMode_t;

/**
 * @brief Network configuration IPv4 method
 */
typedef enum ATNetCfg_IPv4Method_t
{
    ATNetCfg_IPv4Method_Unknown,
    ATNetCfg_IPv4Method_Static,
    ATNetCfg_IPv4Method_Dhcp,
    ATNetCfg_IPv4Method_DhcpLla,
    ATNetCfg_IPv4Method_ReleaseIPSet,
    ATNetCfg_IPv4Method_ReleaseIPOff,
    ATNetCfg_IPv4Method_NumberOfValues
} ATNetCfg_IPv4Method_t;

/**
 * @brief Network configuration IPv6 method
 */
typedef enum ATNetCfg_IPv6Method_t
{
    ATNetCfg_IPv6Method_Unknown,
    ATNetCfg_IPv6Method_Static,
    ATNetCfg_IPv6Method_Stateless,
    ATNetCfg_IPv6Method_Stateful,
    ATNetCfg_IPv6Method_NumberOfValues
} ATNetCfg_IPv6Method_t;

/**
 * @brief IPv4 configuration structure
 */
typedef struct ATNetCfg_IPv4Config_t
{
    ATNetCfg_IPv4Method_t method;
    char ipAddress[15];
    char subnetMask[15];
    char gatewayAddress[15];
    char dnsAddress[15];
} ATNetCfg_IPv4Config_t;

/**
 * @brief IPv6 configuration structure
 */
typedef struct ATNetCfg_IPv6Config_t
{
    ATNetCfg_IPv6Method_t method;
    char ipAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];
    char dnsAddress[CALYPSO_MAX_IP_ADDRESS_LENGTH];
} ATNetCfg_IPv6Config_t;

extern bool ATNetCfg_SetInterfaceModes(uint16_t modes);

extern bool ATNetCfg_GetMacAddress(uint8_t macAddress[6]);
extern bool ATNetCfg_SetMacAddress(uint8_t macAddress[6]);

extern bool ATNetCfg_GetIPv4AddressStation(ATNetCfg_IPv4Config_t *ipConfig);
extern bool ATNetCfg_SetIPv4AddressStation(ATNetCfg_IPv4Config_t *ipConfig);

extern bool ATNetCfg_GetIPv4AddressAP(ATNetCfg_IPv4Config_t *ipConfig);
extern bool ATNetCfg_SetIPv4AddressAP(ATNetCfg_IPv4Config_t *ipConfig);

extern bool ATNetCfg_GetIPv6AddressLocal(ATNetCfg_IPv6Config_t *ipConfig);
extern bool ATNetCfg_SetIPv6AddressLocal(ATNetCfg_IPv6Config_t *ipConfig);

extern bool ATNetCfg_GetIPv6AddressGlobal(ATNetCfg_IPv6Config_t *ipConfig);
extern bool ATNetCfg_SetIPv6AddressGlobal(ATNetCfg_IPv6Config_t *ipConfig);

extern bool ATNetCfg_DisconnectApStation(uint8_t macAddress[6]);

extern bool ATNetCfg_GetIPv4DnsClient(char dns2ndServerAddress[16]);
extern bool ATNetCfg_SetIPv4DnsClient(const char dns2ndServerAddress[16]);

#ifdef __cplusplus
}
#endif

#endif /* AT_NETCFG_H_INCLUDED */
