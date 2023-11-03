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

#include "ATNetApp.h"

#include "Calypso.h"

static const char *ATNetApp_ApplicationStrings[ATNetApp_Application_NumberOfValues] = {
    "HTTP_SERVER",
    "DHCP_SERVER",
    "MDNS",
    "DNS_SERVER",
    "SNTP_CLIENT",
    "DNS_CLIENT",
    "DEVICE"
};

static const char *ATNetApp_HttpOptionStrings[ATNetApp_HttpOption_NumberOfValues] = {
    "PRIM_PORT_NUM",
    "AUTH_CHECK",
    "AUTH_NAME",
    "AUTH_PASSWORD",
    "AUTH_REALM",
    "ROM_PAGES_ACCESS",
    "SECOND_PORT_NUM",
    "SECOND_PORT_EN",
    "PRIM_PORT_SEC_EN",
    "PRIV_KEY_FILE",
    "DEV_CERT_FILE",
    "CA_CERT_FILE",
    "TMP_REGISTER_SERVICE",
    "TMP_UNREGISTER_SERVICE"
};

static const char *ATNetApp_DhcpOptionStrings[ATNetApp_DhcpOption_NumberOfValues] = {
    "BASIC"
};

static const char *ATNetApp_mDnsOptionStrings[ATNetApp_mDnsOption_NumberOfValues] = {
    "CONT_QUERY",
    "QEVETN_MASK",
    "TIMING_PARAMS"
};

static const char *ATNetApp_mDnsEventMaskStrings[ATNetApp_mDnsEventMask_NumberOfValues] = {
    "ipp",
    "deviceinfo",
    "http",
    "https",
    "workstation",
    "guid",
    "h323",
    "ntp",
    "objective",
    "rdp",
    "remote",
    "rtsp",
    "sip",
    "smb",
    "soap",
    "ssh",
    "telnet",
    "tftp",
    "xmpp",
    "raop"
};

static const char *ATNetApp_SntpOptionStrings[ATNetApp_SntpOption_NumberOfValues] = {
    "enable",
    "update_interval",
    "time_zone",
    "server_address"
};

static const char *ATNetApp_DnsClientOptionStrings[ATNetApp_DnsClientOption_NumberOfValues] = {
    "TIME"
};

static const char *ATNetApp_DeviceOptionStrings[ATNetApp_DeviceOption_NumberOfValues] = {
    "URN",
    "DOMAIN"
};

static bool ATNetApp_AddStartStopArguments(char *pOutString, uint8_t apps);

/**
 * @brief Starts one or more networking applications.
 *
 * @param[in] apps Applications (flags) to start (see ATNetApp_Application_t)
 *
 * @return true if successful, false otherwise
 */
bool ATNetApp_StartApplications(uint8_t apps)
{
    if (0 != (apps & ATNetApp_Application_SntpClient))
    {
        /* Special case: SNTP client is started via AT+netAppSet instead of AT+netAppStart */
        ATNetApp_OptionValue_t value;
        memset(&value, 0, sizeof(value));
        value.sntp.enable = true;
        if (!ATNetApp_Set(ATNetApp_Application_SntpClient, ATNetApp_SntpOption_Enable, &value))
        {
            return false;
        }
        apps &= ~ATNetApp_Application_SntpClient;
        if (0 == apps)
        {
            /* No other apps to start */
            return true;
        }
    }

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+netAppStart=");

    if (!ATNetApp_AddStartStopArguments(pRequestCommand, apps))
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
 * @brief Stops one or more networking applications.
 *
 * @param[in] apps Applications (flags) to stop (see ATNetApp_Application_t)
 *
 * @return true if successful, false otherwise
 */
bool ATNetApp_StopApplications(uint8_t apps)
{
    if (0 != (apps & ATNetApp_Application_SntpClient))
    {
        /* Special case: SNTP client is stopped via AT+netAppSet instead of AT+netAppStop */
        ATNetApp_OptionValue_t value;
        memset(&value, 0, sizeof(value));
        value.sntp.enable = false;
        if (!ATNetApp_Set(ATNetApp_Application_SntpClient, ATNetApp_SntpOption_Enable, &value))
        {
            return false;
        }
        apps &= ~ATNetApp_Application_SntpClient;
        if (0 == apps)
        {
            /* No other apps to stop */
            return true;
        }
    }

    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+netAppStop=");

    if (!ATNetApp_AddStartStopArguments(pRequestCommand, apps))
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
 * @brief Sets network applications parameters.
 *
 * @param[in] app Application ID
 * @param[in] option Parameter to be set. Type depends on the application.
 * @param[in] value Value to be set
 *
 * @return true if successful, false otherwise
 */
bool ATNetApp_Set(ATNetApp_Application_t app, uint8_t option, ATNetApp_OptionValue_t *value)
{
    /* ATNetApp_Application_t contains bitmask values - find index of first nonzero bit to use as index */
    uint8_t appIdx = 0;
    for (; appIdx < ATNetApp_Application_NumberOfValues; appIdx++)
    {
        if (0 != (app & (1 << appIdx)))
        {
            break;
        }
    }
    if (appIdx >= ATNetApp_Application_NumberOfValues)
    {
        return false;
    }
    if (ATNetApp_Application_SntpClient == app &&
            ATNetApp_SntpOption_Servers == option)
    {
        /* Special case: The SNTP servers parameter contains up to three servers which
         * have to be set using individual AT commands. */
        for (uint8_t i = 0; i < 3; i++)
        {
            if (0 == strlen(value->sntp.servers[i]))
            {
                /* Empty server address string - skip */
                continue;
            }

            char *pRequestCommand = AT_commandBuffer;
            strcpy(pRequestCommand, "AT+netAppSet=SNTP_CLIENT,server_address,");

            if (!ATCommand_AppendArgumentInt(pRequestCommand, i, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->sntp.servers[i], ATCOMMAND_STRING_TERMINATE))
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
            if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL))
            {
                return false;
            }
        }
        return true;
    }

    char *pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+netAppSet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_ApplicationStrings[appIdx], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (app)
    {
    case ATNetApp_Application_HttpServer:
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_HttpOptionStrings[option], ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        switch (option)
        {
        case ATNetApp_HttpOption_PrimPortNum:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->http.primPortNum, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_AuthCheck:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->http.authCheck, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_AuthName:
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->http.authName, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_AuthPassword:
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->http.authPassword, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_AuthRealm:
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->http.authRealm, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_RomPagesAccess:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->http.romPagesAccess, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_SecondPortNum:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->http.secondPortNum, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_SecondPortEn:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->http.secondPortEn, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_PrimPortSecEn:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->http.primPortSecEn, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_PrivKeyFile:
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->http.privKeyFile, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_DevCertFile:
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->http.devCertFile, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_CaCertFile:
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->http.caCertFile, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_TmpRegisterService:
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->http.tmpRegisterService, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_TmpUnregisterService:
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->http.tmpUnregisterService, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        default:
            return false;
        }
        break;

    case ATNetApp_Application_DhcpServer:
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_DhcpOptionStrings[option], ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        switch (option)
        {
        case ATNetApp_DhcpOption_Basic:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->dhcp.basic.leaseTime, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->dhcp.basic.firstIpAddress, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->dhcp.basic.lastIpAddress, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        default:
            return false;
        }
        break;

    case ATNetApp_Application_mDns:
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_mDnsOptionStrings[option], ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        switch (option)
        {
        case ATNetApp_mDnsOption_ContQuery:
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->mDns.contQuery, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_mDnsOption_QeventMask:
            if (!ATCommand_AppendArgumentBitmask(pRequestCommand,
                                               ATNetApp_mDnsEventMaskStrings,
                                               ATNetApp_mDnsEventMask_NumberOfValues,
                                               value->mDns.qeventMask,
                                               ATCOMMAND_STRING_TERMINATE,
                                               AT_MAX_COMMAND_BUFFER_SIZE))
            {
                return false;
            }
            break;

        case ATNetApp_mDnsOption_TimingParams:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->mDns.timingParams.period, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->mDns.timingParams.repetitions, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->mDns.timingParams.telescopicFactor, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->mDns.timingParams.retransmissionInterval, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->mDns.timingParams.maxPeriodInterval, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->mDns.timingParams.maxTime, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        default:
            return false;
        }
        break;

    case ATNetApp_Application_DnsServer:
        /* No options for DNS server */
        return false;

    case ATNetApp_Application_SntpClient:
    {
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_SntpOptionStrings[option], ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        switch (option)
        {
        case ATNetApp_SntpOption_Enable:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->sntp.enable, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_SntpOption_UpdateInterval:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->sntp.updateInterval, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_SntpOption_TimeZone:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->sntp.timeZone, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        default:
            return false;
        }

        break;
    }

    case ATNetApp_Application_DnsClient:
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_DnsClientOptionStrings[option], ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        switch (option)
        {
        case ATNetApp_DnsClientOption_Time:
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->dnsClient.time.maxResponseTime, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_AppendArgumentInt(pRequestCommand, value->dnsClient.time.numberOfRetries, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        default:
            return false;
        }
        break;

    case ATNetApp_Application_Device:
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_DeviceOptionStrings[option], ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        switch (option)
        {
        case ATNetApp_DeviceOption_URN:
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->device.urn, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_DeviceOption_Domain:
            if (!ATCommand_AppendArgumentString(pRequestCommand, value->device.domain, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        default:
            return false;
        }
        break;

    default:
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
 * @brief Gets network applications parameters.
 *
 * @param[in] app Application ID
 * @param[in] option Parameter to be retrieved. Type depends on the application.
 * @param[out] value The returned value
 *
 * @return true if successful, false otherwise
 */
bool ATNetApp_Get(ATNetApp_Application_t app, uint8_t option, ATNetApp_OptionValue_t *value)
{
    /* ATNetApp_Application_t contains bitmask values - find index of first nonzero bit to use as index */
    uint8_t appIdx = 0;
    for (; appIdx < ATNetApp_Application_NumberOfValues; appIdx++)
    {
        if (0 != (app & (1 << appIdx)))
        {
            break;
        }
    }
    if (appIdx >= ATNetApp_Application_NumberOfValues)
    {
        return false;
    }

    memset(value, 0, sizeof(*value));

    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+netAppGet=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_ApplicationStrings[appIdx], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    switch (app)
    {
    case ATNetApp_Application_HttpServer:
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_HttpOptionStrings[option], ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        break;

    case ATNetApp_Application_DhcpServer:
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_DhcpOptionStrings[option], ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        break;

    case ATNetApp_Application_mDns:
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_mDnsOptionStrings[option], ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        break;

    case ATNetApp_Application_DnsServer:
        /* No options for DNS server */
        return false;

    case ATNetApp_Application_SntpClient:
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_SntpOptionStrings[option], ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        break;

    case ATNetApp_Application_DnsClient:
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_DnsClientOptionStrings[option], ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        break;

    case ATNetApp_Application_Device:
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATNetApp_DeviceOptionStrings[option], ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        break;

    default:
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
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char *expectedCmd = "+netappget:";
    const size_t cmdLength = strlen(expectedCmd);

    /* Check if response is for netappget */
    if (0 != strncmp(pRespondCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;
    switch (app)
    {
    case ATNetApp_Application_HttpServer:
        switch (option)
        {
        case ATNetApp_HttpOption_PrimPortNum:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->http.primPortNum, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_AuthCheck:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->http.authCheck, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_AuthName:
            if (!ATCommand_GetNextArgumentString(&pRespondCommand, value->http.authName, ATCOMMAND_STRING_TERMINATE, sizeof(value->http.authName)))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_AuthPassword:
            if (!ATCommand_GetNextArgumentString(&pRespondCommand, value->http.authPassword, ATCOMMAND_STRING_TERMINATE, sizeof(value->http.authPassword)))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_AuthRealm:
            if (!ATCommand_GetNextArgumentString(&pRespondCommand, value->http.authRealm, ATCOMMAND_STRING_TERMINATE, sizeof(value->http.authRealm)))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_RomPagesAccess:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->http.romPagesAccess, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_SecondPortNum:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->http.secondPortNum, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_SecondPortEn:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->http.secondPortEn, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_PrimPortSecEn:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->http.primPortSecEn, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_HttpOption_PrivKeyFile:
        case ATNetApp_HttpOption_DevCertFile:
        case ATNetApp_HttpOption_CaCertFile:
        case ATNetApp_HttpOption_TmpRegisterService:
        case ATNetApp_HttpOption_TmpUnregisterService:
            /* No getters for the above options */
            return false;

        default:
            return false;
        }
        break;

    case ATNetApp_Application_DhcpServer:
        switch (option)
        {
        case ATNetApp_DhcpOption_Basic:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->dhcp.basic.leaseTime, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentString(&pRespondCommand, value->dhcp.basic.firstIpAddress, ATCOMMAND_ARGUMENT_DELIM, sizeof(value->dhcp.basic.firstIpAddress)))
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentString(&pRespondCommand, value->dhcp.basic.lastIpAddress, ATCOMMAND_STRING_TERMINATE, sizeof(value->dhcp.basic.lastIpAddress)))
            {
                return false;
            }
            break;

        default:
            return false;
        }
        break;

    case ATNetApp_Application_mDns:
        switch (option)
        {
        case ATNetApp_mDnsOption_ContQuery:
            if (!ATCommand_GetNextArgumentString(&pRespondCommand, value->mDns.contQuery, ATCOMMAND_STRING_TERMINATE, sizeof(value->mDns.contQuery)))
            {
                return false;
            }
            break;

        case ATNetApp_mDnsOption_QeventMask:
            if (!ATCommand_GetNextArgumentBitmask(&pRespondCommand,
                                                ATNetApp_mDnsEventMaskStrings,
                                                ATNetApp_mDnsEventMask_NumberOfValues,
                                                200,
                                                &value->mDns.qeventMask,
                                                ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_mDnsOption_TimingParams:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->mDns.timingParams.period, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->mDns.timingParams.repetitions, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->mDns.timingParams.telescopicFactor, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->mDns.timingParams.retransmissionInterval, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->mDns.timingParams.maxPeriodInterval, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->mDns.timingParams.maxTime, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        default:
            return false;
        }
        break;

    case ATNetApp_Application_DnsServer:
        /* No options for DNS server */
        return false;

    case ATNetApp_Application_SntpClient:
    {
        switch (option)
        {
        case ATNetApp_SntpOption_Enable:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->sntp.enable, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_SntpOption_UpdateInterval:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->sntp.updateInterval, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_SntpOption_TimeZone:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->sntp.timeZone, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_SIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        case ATNetApp_SntpOption_Servers:
            for (int i = 0; i < 3; i++)
            {
                char indexStr[10];
                /* Skip "[i] :" */
                if (!ATCommand_GetNextArgumentString(&pRespondCommand, indexStr, ':', sizeof(indexStr)))
                {
                    return false;
                }
                /* Skip extra spaces */
                while (ATCommand_GetNextArgumentString(&pRespondCommand, indexStr, ' ', sizeof(indexStr)))
                {
                }
                /* Get address of server i */
                if (!ATCommand_GetNextArgumentString(&pRespondCommand, value->sntp.servers[i], '\n', 50))
                {
                    return false;
                }
            }
            break;

        default:
            return false;
        }
        break;
    }

    case ATNetApp_Application_DnsClient:
        switch (option)
        {
        case ATNetApp_DnsClientOption_Time:
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->dnsClient.time.maxResponseTime, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
            if (!ATCommand_GetNextArgumentInt(&pRespondCommand, &value->dnsClient.time.numberOfRetries, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED | ATCOMMAND_INTFLAGS_NOTATION_DEC, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
            break;

        default:
            return false;
        }
        break;

    case ATNetApp_Application_Device:
        switch (option)
        {
        case ATNetApp_DeviceOption_URN:
            if (!ATCommand_GetNextArgumentString(&pRespondCommand, value->device.urn, ATCOMMAND_STRING_TERMINATE, sizeof(value->device.urn)))
            {
                return false;
            }
            break;

        case ATNetApp_DeviceOption_Domain:
            if (!ATCommand_GetNextArgumentString(&pRespondCommand, value->device.domain, ATCOMMAND_STRING_TERMINATE, sizeof(value->device.domain)))
            {
                return false;
            }
            break;

        default:
            return false;
        }
        break;

    default:
        return false;
    }

    return true;
}

/**
 * @brief Looks up the IP address for the supplied host name.
 *
 * @param[in] hostName Name of host
 * @param[in] family Network protocol family
 * @param[out] lookupResult The lookup result containing the IP address for the supplied host
 *
 * @return true if successful, false otherwise
 */
bool ATNetApp_GetHostByName(const char *hostName,
                            ATSocket_Family_t family,
                            ATNetApp_GetHostByNameResult_t *lookupResult)
{
    char *pRequestCommand = AT_commandBuffer;
    char *pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+netAppGetHostByName=");

    if (!ATCommand_AppendArgumentString(pRequestCommand, hostName, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    char temp[32];
    if (!ATSocket_GetSocketFamilyString(family, temp))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, temp, ATCOMMAND_STRING_TERMINATE))
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
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_NetAppHostLookUp), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    const char *expectedCmd = "+netappgethostbyname:";
    const size_t cmdLength = strlen(expectedCmd);

    /* Check if response is for netappgethostbyname */
    if (0 != strncmp(pRespondCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    pRespondCommand += cmdLength;

    if (!ATCommand_GetNextArgumentString(&pRespondCommand, lookupResult->hostName, ATCOMMAND_ARGUMENT_DELIM, sizeof(lookupResult->hostName)))
    {
        return false;
    }
    if (!ATCommand_GetNextArgumentString(&pRespondCommand, lookupResult->hostAddress, ATCOMMAND_STRING_TERMINATE, sizeof(lookupResult->hostAddress)))
    {
        return false;
    }
    return true;
}

/**
 * @brief Pings another device.
 *
 * This function starts pinging the supplied host at regular intervals. The value returned
 * by this function indicates if the request was successful or not. The results of the ping
 * command (packets sent/received, roundtrip time) are reported in the form of events. The
 * number of events issued depends on the value of the mode parameter.
 *
 * @param[in] parameters Ping command parameters
 *
 * @return true if ping was requested successfully, false otherwise
 */
bool ATNetApp_Ping(ATNetApp_PingParameters_t *parameters)
{
    char *pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+netAppPing=");

    char temp[32];
    if (!ATSocket_GetSocketFamilyString(parameters->family, temp))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, temp, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, parameters->destination, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, parameters->size, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, parameters->delayMs, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, parameters->timeoutMs, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, parameters->maxNumberOfPings, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, parameters->mode, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
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
 * @brief Updates the device's time using the SNTP client.
 *
 * Note that the SNTP client must be started using ATNetApp_StartApplications(ATNetApp_Application_SntpClient)
 * and configured using ATNetApp_Set(ATNetApp_Application_SntpClient, ...) before calling this function.
 *
 * @return true if successful, false otherwise
 */
bool ATNetApp_UpdateTime()
{
    if (!Calypso_SendRequest("AT+netAppUpdateTime\r\n"))
    {
        return false;
    }
    return Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_NetAppUpdateTime), Calypso_CNFStatus_Success, NULL);
}

/**
 * @brief Adds arguments to the AT+netAppStart or AT+netAppStop command.
 *
 * @param[out] pOutString Output string containing AT command
 * @param[in] apps Applications to be started or stopped (bitmask)
 *
 * @return true if successful, false otherwise
 */
bool ATNetApp_AddStartStopArguments(char *pOutString, uint8_t apps)
{
    if (!ATCommand_AppendArgumentBitmask(pOutString,
                                       ATNetApp_ApplicationStrings,
                                       ATNetApp_Application_NumberOfValues,
                                       apps,
                                       ATCOMMAND_STRING_TERMINATE,
                                       AT_MAX_COMMAND_BUFFER_SIZE))
    {
        return false;
    }
    return ATCommand_AppendArgumentString(pOutString, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}
