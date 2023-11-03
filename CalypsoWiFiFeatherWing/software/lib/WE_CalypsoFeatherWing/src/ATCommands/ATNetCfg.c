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

#include "ATNetCfg.h"

#include <string.h>

#include "Calypso.h"

bool ATNetCfg_GetIPv4Address(ATNetCfg_IPv4Config_t *ipConfig, bool ap);
bool ATNetCfg_SetIPv4Address(ATNetCfg_IPv4Config_t *ipConfig, bool ap);

bool ATNetCfg_GetIPv6Address(ATNetCfg_IPv6Config_t *ipConfig, bool global);
bool ATNetCfg_SetIPv6Address(ATNetCfg_IPv6Config_t *ipConfig, bool global);

static const char *ATNetCfg_InterfaceModeStrings[ATNetCfg_InterfaceMode_NumberOfValues] = {
    "IPV6_STA_LOCAL",
    "IPV6_STA_GLOBAL",
    "DISABLE_IPV4_DHCP",
    "IPV6_LOCAL_STATIC",
    "IPV6_LOCAL_STATELESS",
    "IPV6_LOCAL_STATEFUL",
    "IPV6_GLOBAL_STATIC",
    "IPV6_GLOBAL_STATEFUL",
    "DISABLE_IPV4_LLA",
    "ENABLE_DHCP_RELEASE",
    "IPV6_GLOBAL_STATELESS",
    "DISABLE_FAST_RENEW"
};

static const char *ATNetCfg_IPv4MethodStrings[ATNetCfg_IPv4Method_NumberOfValues] = {
    "UNKNOWN",
    "STATIC",
    "DHCP",
    "DHCP_LLA",
    "RELEASE_IP_SET",
    "RELEASE_IP_OFF"
};

static const char *ATNetCfg_IPv6MethodStrings[ATNetCfg_IPv6Method_NumberOfValues] = {
    "UNKNOWN",
    "STATIC",
    "STATELESS",
    "STATEFUL"
};

/**
 * @brief Enables or disables network interface modes.
 *
 * @param[in] modes Flags of modes to enable. See ATNetCfg_InterfaceMode_t. Set to ATNetCfg_InterfaceMode_None to disable all modes.
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_SetInterfaceModes(uint16_t modes)
{
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+netCfgSet=IF,STATE,");

    if (!ATCommand_AppendArgumentBitmask(pRequestCommand,
                                       ATNetCfg_InterfaceModeStrings,
                                       ATNetCfg_InterfaceMode_NumberOfValues,
                                       modes,
                                       ATCOMMAND_STRING_TERMINATE,
                                       AT_MAX_COMMAND_BUFFER_SIZE))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Returns the device's MAC address.
 *
 * @param[out] macAddress The returned MAC address. Must be a byte array of size 6.
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_GetMacAddress(uint8_t macAddress[6])
{
    if (NULL == macAddress)
    {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+netCfgGet=GET_MAC_ADDR");
    ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char *expectedCmd = "+netcfgget:";
    const size_t cmdLength = strlen(expectedCmd);
    if (0 != strncmp(pRespondCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    for (uint8_t i = 0; i < 6; i++)
    {
        if (!ATCommand_GetNextArgumentInt(&pRespondCommand,
                                        macAddress + i,
                                        (ATCOMMAND_INTFLAGS_NOTATION_HEX | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_SIZE8),
                                        i < 5 ? ATCOMMAND_CONFIRM_DELIM : ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief Sets the device's MAC address.
 *
 * @param[in] macAddress The MAC address to be set. Must be a byte array of size 6.
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_SetMacAddress(uint8_t macAddress[6])
{
    if (NULL == macAddress)
    {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+netCfgSet=SET_MAC_ADDR,,");

    for (uint8_t i = 0; i < 6; i++)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                       macAddress[i],
                                       ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX,
                                       i < 5 ? ':' : ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }

    ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Returns the device's IPv4 station address and other IPv4 info (subnet mask, gateway, DNS).
 *
 * This is the configuration used when the device is used as a WLAN station
 * (use ATNetCfg_GetIPv4AddressAP() for getting AP configuration).
 *
 * @param[out] ipConfig The returned IPv4 configuration.
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_GetIPv4AddressStation(ATNetCfg_IPv4Config_t *ipConfig)
{
    return ATNetCfg_GetIPv4Address(ipConfig, false);
}

/**
 * @brief Sets the device's IPv4 station address and other IPv4 info (subnet mask, gateway, DNS).
 *
 * This is the configuration used when the device is used as a WLAN station
 * (use ATNetCfg_SetIPv4AddressAP() for setting AP configuration).
 *
 * @param[in] ipConfig The IPv4 configuration to be set.
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_SetIPv4AddressStation(ATNetCfg_IPv4Config_t *ipConfig)
{
    return ATNetCfg_SetIPv4Address(ipConfig, false);
}

/**
 * @brief Returns the device's IPv4 access point address and other IPv4 info (subnet mask, gateway, DNS).
 *
 * This is the configuration used when the device is used as a WLAN access point
 * (use ATNetCfg_GetIPv4AddressStation() for getting station configuration).
 *
 * @param[out] ipConfig The returned IPv4 configuration.
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_GetIPv4AddressAP(ATNetCfg_IPv4Config_t *ipConfig)
{
    return ATNetCfg_GetIPv4Address(ipConfig, true);
}

/**
 * @brief Sets the device's IPv4 access point address and other IPv4 info (subnet mask, gateway, DNS).
 *
 * This is the configuration used when the device is used as a WLAN access point
 * (use ATNetCfg_SetIPv4AddressStation() for setting station configuration).
 *
 * @param[in] ipConfig The IPv4 configuration to be set.
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_SetIPv4AddressAP(ATNetCfg_IPv4Config_t *ipConfig)
{
    return ATNetCfg_SetIPv4Address(ipConfig, true);
}

/**
 * @brief Returns the device's IPv4 address and other IPv4 info (subnet mask, gateway, DNS)
 * for station or access point mode.
 *
 * @param[out] ipConfig The returned IPv4 configuration.
 * @param[in] ap Return access point configuration (true) or station configuration (false)
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_GetIPv4Address(ATNetCfg_IPv4Config_t *ipConfig, bool ap)
{
    if (NULL == ipConfig)
    {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, ap ? "AT+netCfgGet=IPV4_AP_ADDR" : "AT+netCfgGet=IPV4_STA_ADDR");
    ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);


    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char *expectedCmd = "+netcfgget:";
    const size_t cmdLength = strlen(expectedCmd);
    if (0 != strncmp(pRespondCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    char ipModeStr[9];
    if (!ATCommand_GetNextArgumentString(&pRespondCommand, ipModeStr, ATCOMMAND_ARGUMENT_DELIM, sizeof(ipModeStr)))
    {
        return false;
    }

    char *dhcpMode = "dhcp";
    char *dhcpLlaMode = "dhcp_lla";
    char *staticMode = "static";
    if (0 == strncmp(ipModeStr, dhcpMode, strlen(dhcpMode)))
    {
        ipConfig->method = ATNetCfg_IPv4Method_Dhcp;
    }
    else if (0 == strncmp(ipModeStr, dhcpLlaMode, strlen(dhcpLlaMode)))
    {
        ipConfig->method = ATNetCfg_IPv4Method_DhcpLla;
    }
    else if (0 == strncmp(ipModeStr, staticMode, strlen(staticMode)))
    {
        ipConfig->method = ATNetCfg_IPv4Method_Static;
    }
    else
    {
        ipConfig->method = ATNetCfg_IPv4Method_Unknown;
    }

    if (!ATCommand_GetNextArgumentString(&pRespondCommand, ipConfig->ipAddress, ATCOMMAND_ARGUMENT_DELIM, sizeof(ipConfig->ipAddress)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pRespondCommand, ipConfig->subnetMask, ATCOMMAND_ARGUMENT_DELIM, sizeof(ipConfig->subnetMask)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pRespondCommand, ipConfig->gatewayAddress, ATCOMMAND_ARGUMENT_DELIM, sizeof(ipConfig->gatewayAddress)))
    {
        return false;
    }

    if (!ATCommand_GetNextArgumentString(&pRespondCommand, ipConfig->dnsAddress, ATCOMMAND_STRING_TERMINATE, sizeof(ipConfig->dnsAddress)))
    {
        return false;
    }

    return true;
}

/**
 * @brief Sets the device's IPv4 address and other IPv4 info (subnet mask, gateway, DNS)
 * for station or access point mode.
 *
 * @param[in] ipConfig The IPv4 configuration to be set.
 * @param[in] ap Set access point configuration (true) or station configuration (false)
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_SetIPv4Address(ATNetCfg_IPv4Config_t *ipConfig, bool ap)
{
    if (NULL == ipConfig)
    {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, ap ? "AT+netCfgSet=IPV4_AP_ADDR," : "AT+netCfgSet=IPV4_STA_ADDR,");

    if (ap)
    {
        ipConfig->method = ATNetCfg_IPv4Method_Static;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetCfg_IPv4MethodStrings[ipConfig->method], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (ATNetCfg_IPv4Method_Static == ipConfig->method)
    {
        if (!ATCommand_AppendArgumentString(pRequestCommand, ipConfig->ipAddress, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ipConfig->subnetMask, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ipConfig->gatewayAddress, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ipConfig->dnsAddress, ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }

    ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Returns the device's local IPv6 configuration.
 *
 * @param[out] ipConfig The returned IPv6 configuration
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_GetIPv6AddressLocal(ATNetCfg_IPv6Config_t *ipConfig)
{
    return ATNetCfg_GetIPv6Address(ipConfig, false);
}

/**
 * @brief Sets the device's local IPv6 configuration.
 *
 * @param[in] ipConfig The IPv6 configuration to be set
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_SetIPv6AddressLocal(ATNetCfg_IPv6Config_t *ipConfig)
{
    return ATNetCfg_SetIPv6Address(ipConfig, false);
}

/**
 * @brief Returns the device's global IPv6 configuration.
 *
 * @param[out] ipConfig The returned IPv6 configuration
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_GetIPv6AddressGlobal(ATNetCfg_IPv6Config_t *ipConfig)
{
    return ATNetCfg_GetIPv6Address(ipConfig, true);
}

/**
 * @brief Sets the device's global IPv6 configuration.
 *
 * @param[in] ipConfig The IPv6 configuration to be set
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_SetIPv6AddressGlobal(ATNetCfg_IPv6Config_t *ipConfig)
{
    return ATNetCfg_SetIPv6Address(ipConfig, true);
}

/**
 * @brief Returns the device's local or global IPv6 configuration.
 *
 * @param[out] ipConfig The returned IPv6 configuration
 * @param[in] global Get the global (true) or local (false) IPv6 configuration
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_GetIPv6Address(ATNetCfg_IPv6Config_t *ipConfig, bool global)
{
    if (NULL == ipConfig)
    {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, global ? "AT+netCfgGet=IPV6_ADDR_GLOBAL" : "AT+netCfgGet=IPV6_ADDR_LOCAL");
    ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);


    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char *expectedCmd = "+netcfgget:";
    const size_t cmdLength = strlen(expectedCmd);
    if (0 != strncmp(pRespondCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    char ipModeStr[9];
    if (!ATCommand_GetNextArgumentString(&pRespondCommand, ipModeStr, ATCOMMAND_ARGUMENT_DELIM, sizeof(ipModeStr)))
    {
        return false;
    }

    char *statelessMode = "stateless";
    char *statefulMode = "stateful";
    char *staticMode = "static";
    if (0 == strncmp(ipModeStr, statelessMode, strlen(statelessMode)))
    {
        ipConfig->method = ATNetCfg_IPv6Method_Stateless;
    }
    else if (0 == strncmp(ipModeStr, statefulMode, strlen(statefulMode)))
    {
        ipConfig->method = ATNetCfg_IPv6Method_Stateful;
    }
    else if (0 == strncmp(ipModeStr, staticMode, strlen(staticMode)))
    {
        ipConfig->method = ATNetCfg_IPv6Method_Static;
    }
    else
    {
        ipConfig->method = ATNetCfg_IPv6Method_Unknown;
    }

    return ATCommand_GetNextArgumentString(&pRespondCommand, ipConfig->ipAddress, ATCOMMAND_STRING_TERMINATE, sizeof(ipConfig->ipAddress));
}

/**
 * @brief Sets the device's local or global IPv6 configuration.
 *
 * @param[in] ipConfig The IPv6 configuration to be set
 * @param[in] global Set the global (true) or local (false) IPv6 configuration
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_SetIPv6Address(ATNetCfg_IPv6Config_t *ipConfig, bool global)
{
    if (NULL == ipConfig)
    {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, global ? "AT+netCfgSet=IPV6_ADDR_GLOBAL," : "AT+netCfgSet=IPV6_ADDR_LOCAL,");

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetCfg_IPv6MethodStrings[ipConfig->method], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ipConfig->ipAddress, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ipConfig->dnsAddress, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Disconnects the station with the supplied MAC address. Only works in AP mode.
 *
 * @param[in] macAddress MAC address of the station to disconnect from. Must be a byte array of size 6.
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_DisconnectApStation(uint8_t macAddress[6])
{
    if (NULL == macAddress)
    {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+netCfgSet=AP_STATION_DISCONNECT,,");

    for (uint8_t i = 0; i < 6; i++)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand,
                                       macAddress[i],
                                       ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_HEX,
                                       i < 5 ? ':' : ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }

    ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Returns the secondary DNS server address.
 *
 * @param[out] dns2ndServerAddress The returned secondary DNS server address.
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_GetIPv4DnsClient(char dns2ndServerAddress[16])
{
    if (NULL == dns2ndServerAddress)
    {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+netCfgGet=IPV4_DNS_CLIENT");
    ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);


    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char *expectedCmd = "+netcfgget:";
    const size_t cmdLength = strlen(expectedCmd);
    if (0 != strncmp(pRespondCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    return ATCommand_GetNextArgumentString(&pRespondCommand, dns2ndServerAddress, ATCOMMAND_STRING_TERMINATE, 16);
}

/**
 * @brief Sets the secondary DNS server address.
 *
 * @param[out] dns2ndServerAddress The secondary DNS server address to be set.
 *
 * @return true if successful, false otherwise
 */
bool ATNetCfg_SetIPv4DnsClient(const char dns2ndServerAddress[16])
{
    if (NULL == dns2ndServerAddress)
    {
        return false;
    }

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+netCfgSet=IPV4_DNS_CLIENT,,");

    ATCommand_AppendArgumentString(pRequestCommand, dns2ndServerAddress, ATCOMMAND_STRING_TERMINATE);
    ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);

    if (!Calypso_SendRequest(pRequestCommand))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL);
}
