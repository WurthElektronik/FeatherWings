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
 * @brief AT commands for socket functionality.
 */

#ifndef STEPHANOI_SOCKET_H_INCLUDED
#define STEPHANOI_SOCKET_H_INCLUDED

#include <StephanoI/ATCommands/ATEvent.h>
#include <StephanoI/ATCommands/ATWifi.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SOCKET_DATALEN_MAX (1024 * 16)
#define SOCKET_TYPE_LEN (5 + 2 + 1 + 1)

    /**
 * @brief Parameters of Socket get state Event
 */
    typedef struct StephanoI_ATSocket_State_t
    {
        uint8_t link_ID;
        char type[SOCKET_TYPE_LEN];
        char remote_ip[WIFI_IPv6_STRINGLEN];
        uint32_t remote_port;
        uint32_t local_port;
        uint8_t tetype;
    } StephanoI_ATSocket_State_t;

    /**
 * @brief Parameters of Socket ping Event
 */
    typedef uint32_t StephanoI_ATSocket_Ping_t;

    typedef char StephanoI_ATSocket_DomainResolved_t[WIFI_IPv6_STRINGLEN];

    typedef struct StephanoI_ATSocket_SendState_t
    {
        uint32_t had_sent_len;
        uint32_t port_rcv_len;
    } StephanoI_ATSocket_SendState_t;

    typedef struct StephanoI_ATSocket_Receive_t
    {
        uint8_t link_ID;
        uint32_t length;
        uint8_t data[SOCKET_DATALEN_MAX];
    } StephanoI_ATSocket_Receive_t;

    typedef struct StephanoI_ATSocket_ReceiveData_t
    {
        uint32_t actual_len;
        char remote_ip[WIFI_IPv6_STRINGLEN];
        uint32_t remote_port;
        uint8_t data[SOCKET_DATALEN_MAX];
    } StephanoI_ATSocket_ReceiveData_t;

    typedef struct StephanoI_ATSocket_ReceiveLen_t
    {
        uint16_t datalen_link0;
        uint16_t datalen_link1;
        uint16_t datalen_link2;
        uint16_t datalen_link3;
        uint16_t datalen_link4;
    } StephanoI_ATSocket_ReceiveLen_t;

    typedef enum StephanoI_ATSocket_ReceiveMode_t
    {
        StephanoI_ATSocket_ReceiveMode_Active = 0,
        StephanoI_ATSocket_ReceiveMode_Passive
    } StephanoI_ATSocket_ReceiveMode_t;

    /**
 * @brief Wifi socket type
 */
    typedef enum StephanoI_ATSocket_Type_t
    {
        StephanoI_ATSocket_Type_TCP,
        StephanoI_ATSocket_Type_TCPv6,
        StephanoI_ATSocket_Type_UDP,
        StephanoI_ATSocket_Type_UDPv6,
        StephanoI_ATSocket_Type_SSL,
        StephanoI_ATSocket_Type_SSLv6
    } StephanoI_ATSocket_Type_t;

    /**
 * @brief Wifi time zone type
 */
    typedef enum StephanoI_ATSocket_TimeZone_t
    {
        StephanoI_ATSocket_TimeZone_UTCminus12 = -12,
        StephanoI_ATSocket_TimeZone_UTCminus11 = -11,
        StephanoI_ATSocket_TimeZone_UTCminus10 = -10,
        StephanoI_ATSocket_TimeZone_UTCminus9 = -9,
        StephanoI_ATSocket_TimeZone_UTCminus8 = -8,
        StephanoI_ATSocket_TimeZone_UTCminus7 = -7,
        StephanoI_ATSocket_TimeZone_UTCminus6 = -6,
        StephanoI_ATSocket_TimeZone_UTCminus5 = -5,
        StephanoI_ATSocket_TimeZone_UTCminus4 = -4,
        StephanoI_ATSocket_TimeZone_UTCminus3 = -3,
        StephanoI_ATSocket_TimeZone_UTCminus2 = -2,
        StephanoI_ATSocket_TimeZone_UTCminus1 = -1,
        StephanoI_ATSocket_TimeZone_UTC = 0,
        StephanoI_ATSocket_TimeZone_UTCPlus1 = 1,
        StephanoI_ATSocket_TimeZone_UTCPlus2 = 2,
        StephanoI_ATSocket_TimeZone_UTCPlus3 = 3,
        StephanoI_ATSocket_TimeZone_UTCPlus4 = 4,
        StephanoI_ATSocket_TimeZone_UTCPlus5 = 5,
        StephanoI_ATSocket_TimeZone_UTCPlus6 = 6,
        StephanoI_ATSocket_TimeZone_UTCPlus7 = 7,
        StephanoI_ATSocket_TimeZone_UTCPlus8 = 8,
        StephanoI_ATSocket_TimeZone_UTCPlus9 = 9,
        StephanoI_ATSocket_TimeZone_UTCPlus10 = 10,
        StephanoI_ATSocket_TimeZone_UTCPlus11 = 11,
        StephanoI_ATSocket_TimeZone_UTCPlus12 = 12,
        StephanoI_ATSocket_TimeZone_UTCPlus13 = 13,
        StephanoI_ATSocket_TimeZone_UTCPlus14 = 14,
    } StephanoI_ATSocket_TimeZone_t;

    /**
 * @brief Wifi SSL auth mode
 */
    typedef enum StephanoI_ATSocket_SSLAuthMode_t
    {
        StephanoI_ATSocket_SSLAuthMode_NoAuth = 0,
        StephanoI_ATSocket_SSLAuthMode_ClientHasCert = 1,
        StephanoI_ATSocket_SSLAuthMode_ClientRequestsCert = 2,
        StephanoI_ATSocket_SSLAuthMode_Mutual = 3,
    } StephanoI_ATSocket_SSLAuthMode_t;

    typedef char StephanoI_ATSocket_SNTP_Time_t[64];

    typedef struct StephanoI_ATSocket_SSLClient_t
    {
        uint8_t link_ID;
        StephanoI_ATSocket_SSLAuthMode_t auth_mode;
        uint8_t pki_number;
        uint8_t ca_number;
    } StephanoI_ATSocket_SSLClient_t;

    typedef struct StephanoI_ATSocket_SSLClientName_t
    {
        uint8_t link_ID;
        char common_name[64 + 2];
    } StephanoI_ATSocket_SSLClientName_t;

    typedef struct StephanoI_ATSocket_SSLClientServerNameIndication_t
    {
        uint8_t link_ID;
        char sni[64 + 2];
    } StephanoI_ATSocket_SSLClientServerNameIndication_t;

    typedef struct StephanoI_ATSocket_SSLClientALPN_t
    {
        uint8_t link_ID;
        uint8_t counts;
        char alpn[64 + 2];
        char alpn1[64 + 2];
        char alpn2[64 + 2];
        char alpn3[64 + 2];
        char alpn4[64 + 2];
        char alpn5[64 + 2];
    } StephanoI_ATSocket_SSLClientALPN_t;

    typedef struct StephanoI_ATSocket_SSLClientPSK_t
    {
        uint8_t link_ID;
        char psk[32 + 2];
        char hint[32 + 2];
    } StephanoI_ATSocket_SSLClientPSK_t;

    extern bool StephanoI_ATSocket_SetEnableIPv6(bool enable);
    extern bool StephanoI_ATSocket_GetEnableIPv6(bool* enable);
    extern bool StephanoI_ATSocket_SetMultiple(bool multiple);
    extern bool StephanoI_ATSocket_GetMultiple(bool* multipleP);

    extern bool StephanoI_ATSocket_OpenTCPSocket(bool multiple_connections, uint8_t link_ID, StephanoI_ATSocket_Type_t type, char* remote_IP, uint32_t remote_port);
    extern bool StephanoI_ATSocket_OpenUDPSocket(bool multiple_connections, uint8_t link_ID, StephanoI_ATSocket_Type_t type, char* remote_IP, uint32_t remote_port, uint32_t local_port);
    extern bool StephanoI_ATSocket_OpenSocketEx(StephanoI_ATSocket_Type_t type, char* remote_IP, uint32_t remote_port);
    extern bool StephanoI_ATSocket_CloseSocket(bool multiple_connections, uint8_t link_ID);
    extern bool StephanoI_ATSocket_GetState(StephanoI_ATSocket_State_t* t);
    extern bool StephanoI_ATSocket_Ping(char* host, StephanoI_ATSocket_Ping_t* t);
    extern bool StephanoI_ATSocket_ResolveDomain(char* domain_name, StephanoI_ATSocket_DomainResolved_t* domainP);
    extern bool StephanoI_ATSocket_Send(bool multiple_connections, uint8_t link_id, char* remote_host, uint32_t remote_port, uint8_t* data, uint32_t length);
    extern bool StephanoI_ATSocket_SendConfiguration(uint32_t report_size, uint32_t transmit_size);
    extern bool StephanoI_ATSocket_EnableTCPSSLServer(bool multiple_connections, bool mode, uint16_t param2, StephanoI_ATSocket_Type_t type, bool CA_enable);
    extern bool StephanoI_ATSocket_SetTCPSSLServerMaxConnections(uint8_t max_num);
    extern bool StephanoI_ATSocket_GetTCPSSLServerMaxConnections(uint8_t* max_num);
    extern bool StephanoI_ATSocket_SetTCPSSLServerTimeout(uint16_t time);
    extern bool StephanoI_ATSocket_GetTCPSSLServerTimeout(uint16_t* time);
    extern bool StephanoI_ATSocket_EnableSNTPServer(bool enable, StephanoI_ATSocket_TimeZone_t timezone, char* server1);
    extern bool StephanoI_ATSocket_ReadSNTPTime(StephanoI_ATSocket_SNTP_Time_t* t);
    extern bool StephanoI_ATSocket_SetSNTPTimeInterval(uint32_t time);
    extern bool StephanoI_ATSocket_GetSNTPTimeInterval(uint32_t* time);

    extern bool StephanoI_ATSocket_SetSSLClient(bool multiple_connections, StephanoI_ATSocket_SSLClient_t client);
    extern bool StephanoI_ATSocket_GetSSLClient();
    extern bool StephanoI_ATSocket_SetSSLClientName(bool multiple_connections, StephanoI_ATSocket_SSLClientName_t name);
    extern bool StephanoI_ATSocket_GetSSLClientName(StephanoI_ATSocket_SSLClientName_t* name);
    extern bool StephanoI_ATSocket_SetSSLClientServerNameIndication(bool multiple_connections, StephanoI_ATSocket_SSLClientServerNameIndication_t name);
    extern bool StephanoI_ATSocket_GetSSLClientServerNameIndication(StephanoI_ATSocket_SSLClientServerNameIndication_t* name);
    extern bool StephanoI_ATSocket_SetSSLClientALPN(bool multiple_connections, StephanoI_ATSocket_SSLClientALPN_t alpn);
    extern bool StephanoI_ATSocket_GetSSLClientALPN(StephanoI_ATSocket_SSLClientALPN_t* alpn);
    extern bool StephanoI_ATSocket_SetSSLClientPSK(bool multiple_connections, StephanoI_ATSocket_SSLClientPSK_t psk);
    extern bool StephanoI_ATSocket_GetSSLClientPSK(StephanoI_ATSocket_SSLClientPSK_t* psk);
    extern bool StephanoI_ATSocket_SetDNSServerInformation(bool manual, char* DNS_server);

    extern bool StephanoI_ATSocket_SetReceiveMode(StephanoI_ATSocket_ReceiveMode_t mode);
    extern bool StephanoI_ATSocket_GetReceiveMode(StephanoI_ATSocket_ReceiveMode_t* modeP);
    extern bool StephanoI_ATSocket_GetReceiveLength(StephanoI_ATSocket_ReceiveLen_t* t);
    extern bool StephanoI_ATSocket_GetReceivedData(bool multiple_connections, uint8_t link_ID, uint16_t len, StephanoI_ATSocket_ReceiveData_t* dataP);

    extern bool StephanoI_ATSocket_ParseState(char* EventArgumentsP, StephanoI_ATSocket_State_t* t);
    extern bool StephanoI_ATSocket_ParsePing(char* EventArgumentsP, StephanoI_ATSocket_Ping_t* t);
    extern bool StephanoI_ATSocket_ParseDomainResolved(char* EventArgumentsP, StephanoI_ATSocket_DomainResolved_t* t);
    extern bool StephanoI_ATSocket_ParseSendState(char* EventArgumentsP, StephanoI_ATSocket_SendState_t* t);
    extern bool StephanoI_ATSocket_ParseReceive(char* EventArgumentsP, bool multiple_connections, StephanoI_ATSocket_Receive_t* t);
    extern bool StephanoI_ATSocket_ParseReceiveData(char* EventArgumentsP, StephanoI_ATSocket_ReceiveData_t* t);
    extern bool StephanoI_ATSocket_ParseReceiveLen(char* EventArgumentsP, StephanoI_ATSocket_ReceiveLen_t* t);
    extern bool StephanoI_ATSocket_ParseGetMultiple(char* EventArgumentsP, bool* b);
    extern bool StephanoI_ATSocket_ParseSNTPTime(char* EventArgumentsP, StephanoI_ATSocket_SNTP_Time_t* t);
    extern bool StephanoI_ATSocket_ParseSNTPTimeInterval(char* EventArgumentsP, uint32_t* t);
    extern bool StephanoI_ATSocket_ParseReceiveMode(char* EventArgumentsP, StephanoI_ATSocket_ReceiveMode_t* b);
    extern bool StephanoI_ATSocket_ParseEnableIPv6(char* EventArgumentsP, bool* t);
    extern bool StephanoI_ATSocket_ParseTCPSSLServerMaxConnections(char* EventArgumentsP, uint8_t* t);
    extern bool StephanoI_ATSocket_ParseTCPSSLServerTimeout(char* EventArgumentsP, uint16_t* t);
    extern bool StephanoI_ATSocket_ParseSSLClient(char* EventArgumentsP, StephanoI_ATSocket_SSLClient_t* t);
    extern bool StephanoI_ATSocket_ParseSSLClientName(char* EventArgumentsP, StephanoI_ATSocket_SSLClientName_t* t);
    extern bool StephanoI_ATSocket_ParseSSLClientServerNameIndication(char* EventArgumentsP, StephanoI_ATSocket_SSLClientServerNameIndication_t* t);
    extern bool StephanoI_ATSocket_ParseSSLALPN(char* EventArgumentsP, StephanoI_ATSocket_SSLClientALPN_t* t);
    extern bool StephanoI_ATSocket_ParseSSLPSK(char* EventArgumentsP, StephanoI_ATSocket_SSLClientPSK_t* t);

#ifdef __cplusplus
}
#endif

#endif /* STEPHANOI_SOCKET_H_INCLUDED */
