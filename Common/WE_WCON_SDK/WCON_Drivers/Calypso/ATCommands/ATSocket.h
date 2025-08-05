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

#ifndef CALYPSO_AT_SOCKET_H_INCLUDED
#define CALYPSO_AT_SOCKET_H_INCLUDED
#include "ATFile.h"
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Socket family
 */
    typedef enum Calypso_ATSocket_Family_t
    {
        Calypso_ATSocket_Family_INET,
        Calypso_ATSocket_Family_INET6,
        Calypso_ATSocket_Family_NumberOfValues,
    } Calypso_ATSocket_Family_t;

    /**
 * @brief Socket type
 */
    typedef enum Calypso_ATSocket_Type_t
    {
        Calypso_ATSocket_Type_Stream,
        Calypso_ATSocket_Type_Datagram,
        Calypso_ATSocket_Type_NumberOfValues,
    } Calypso_ATSocket_Type_t;

    /**
 * @brief Socket protocol
 */
    typedef enum Calypso_ATSocket_Protocol_t
    {
        Calypso_ATSocket_Protocol_TCP,
        Calypso_ATSocket_Protocol_UDP,
        Calypso_ATSocket_Protocol_SEC,
        Calypso_ATSocket_Protocol_NumberOfValues,
    } Calypso_ATSocket_Protocol_t;

    /**
 * @brief Level parameter for Calypso_ATSocket_SetSocketOption()
 */
    typedef enum Calypso_ATSocket_SockOptLevel_t
    {
        Calypso_ATSocket_SockOptLevel_Socket,
        Calypso_ATSocket_SockOptLevel_IP,
        Calypso_ATSocket_SockOptLevel_NumberOfValues,
    } Calypso_ATSocket_SockOptLevel_t;

    /**
 * @brief Options for Calypso_ATSocket_SetSocketOption() for level Calypso_ATSocket_SockOptLevel_Socket.
 */
    typedef enum Calypso_ATSocket_SockOptSocket_t
    {
        Calypso_ATSocket_SockOptSocket_KeepAlive,
        Calypso_ATSocket_SockOptSocket_KeepAliveTime,
        Calypso_ATSocket_SockOptSocket_RXNoIPBoundary,
        Calypso_ATSocket_SockOptSocket_RCVTimeout,
        Calypso_ATSocket_SockOptSocket_RCVBuf,
        Calypso_ATSocket_SockOptSocket_NonBlocking,
        Calypso_ATSocket_SockOptSocket_SecMethod,
        Calypso_ATSocket_SockOptSocket_SecureMask,
        Calypso_ATSocket_SockOptSocket_SecureFilesCAFileName,
        Calypso_ATSocket_SockOptSocket_SecureFilesPrivateKeyFileName,
        Calypso_ATSocket_SockOptSocket_SecureFilesCertificateFileName,
        Calypso_ATSocket_SockOptSocket_SecureFilesDHKeyFileName,
        Calypso_ATSocket_SockOptSocket_SecureDomainNameVerification,
        Calypso_ATSocket_SockOptSocket_DisableCertificateStore,
        Calypso_ATSocket_SockOptSocket_NumberOfValues,
    } Calypso_ATSocket_SockOptSocket_t;

    /**
 * @brief Options for Calypso_ATSocket_SetSocketOption() for level Calypso_ATSocket_SockOptLevel_IP.
 */
    typedef enum Calypso_ATSocket_SockOptIP_t
    {
        Calypso_ATSocket_SockOptIP_MulticastTTL,
        Calypso_ATSocket_SockOptIP_AddMembership,
        Calypso_ATSocket_SockOptIP_DropMembership,
        Calypso_ATSocket_SockOptIP_NumberOfValues,
    } Calypso_ATSocket_SockOptIP_t;

    /**
 * @brief Possible values for Calypso_ATSocket_SockOptSocket_SecMethod option used with
 * Calypso_ATSocket_SetSocketOption() for level Calypso_ATSocket_SockOptLevel_Socket.
 */
    typedef enum Calypso_ATSocket_SockOptSecMethod_t
    {
        Calypso_ATSocket_SockOptSecMethod_SSLv3,
        Calypso_ATSocket_SockOptSecMethod_TLSv1,
        Calypso_ATSocket_SockOptSecMethod_TLSv1_1,
        Calypso_ATSocket_SockOptSecMethod_TLSv1_2,
        Calypso_ATSocket_SockOptSecMethod_SSLv3_TLSv1_2,
        Calypso_ATSocket_SockOptSecMethod_NumberOfValues,
    } Calypso_ATSocket_SockOptSecMethod_t;

    /**
 * @brief Socket ciphers (flags)
 */
    typedef enum Calypso_ATSocket_Cipher_t
    {
        Calypso_ATSocket_Cipher_SSL_RSA_WITH_RC4_128_SHA = (1 << 0),
        Calypso_ATSocket_Cipher_SSL_RSA_WITH_RC4_128_MD5 = (1 << 1),
        Calypso_ATSocket_Cipher_TLS_RSA_WITH_AES_256_CBC_SHA = (1 << 2),
        Calypso_ATSocket_Cipher_TLS_DHE_RSA_WITH_AES_256_CBC_SHA = (1 << 3),
        Calypso_ATSocket_Cipher_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA = (1 << 4),
        Calypso_ATSocket_Cipher_TLS_ECDHE_RSA_WITH_RC4_128_SHA = (1 << 5),
        Calypso_ATSocket_Cipher_TLS_RSA_WITH_AES_128_CBC_SHA256 = (1 << 6),
        Calypso_ATSocket_Cipher_TLS_RSA_WITH_AES_256_CBC_SHA256 = (1 << 7),
        Calypso_ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 = (1 << 8),
        Calypso_ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA = (1 << 9),
        Calypso_ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA = (1 << 10),
        Calypso_ATSocket_Cipher_TLS_RSA_WITH_AES_128_GCM_SHA256 = (1 << 11),
        Calypso_ATSocket_Cipher_TLS_RSA_WITH_AES_256_GCM_SHA384 = (1 << 12),
        Calypso_ATSocket_Cipher_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 = (1 << 13),
        Calypso_ATSocket_Cipher_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 = (1 << 14),
        Calypso_ATSocket_Cipher_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 = (1 << 15),
        Calypso_ATSocket_Cipher_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 = (1 << 16),
        Calypso_ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 = (1 << 17),
        Calypso_ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 = (1 << 18),
        Calypso_ATSocket_Cipher_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 = (1 << 19),
        Calypso_ATSocket_Cipher_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 = (1 << 20),
        Calypso_ATSocket_Cipher_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256 = (1 << 21),
        Calypso_ATSocket_Cipher_NumberOfValues = 22
    } Calypso_ATSocket_Cipher_t;

    /**
 * @brief Option value type used for options accepting true/false.
 */
    typedef enum Calypso_ATSocket_OptionState_t
    {
        Calypso_ATSocket_OptionState_Disabled = 0,
        Calypso_ATSocket_OptionState_Enabled = 1
    } Calypso_ATSocket_OptionState_t;

    /**
 * @brief Socket configuration structure.
 */
    typedef struct Calypso_ATSocket_Descriptor_t
    {
        Calypso_ATSocket_Family_t family;
        uint16_t port;
        char address[CALYPSO_MAX_IP_ADDRESS_LENGTH];
    } Calypso_ATSocket_Descriptor_t;

    /**
 * @brief Receive timeout option struct.
 * @see Calypso_ATSocket_SockOptSocket_RCVTimeout
 */
    typedef struct Calypso_ATSocket_ReceiveTimeout_t
    {
        uint32_t seconds;
        uint32_t microseconds;
    } Calypso_ATSocket_ReceiveTimeout_t;

    /**
 * @brief Used as parameter for Calypso_ATSocket_SetSocketOption() when joining/leaving a multicast group.
 * @see Calypso_ATSocket_SockOptIP_AddMembership, Calypso_ATSocket_SockOptIP_DropMembership
 */
    typedef struct Calypso_ATSocket_MulticastGroup_t
    {
        char ipv4Address[32];
        char interfaceAddress[32];
    } Calypso_ATSocket_MulticastGroup_t;

    /**
 * @brief Used to set option values using Calypso_ATSocket_SetSocketOption()
 */
    typedef union Calypso_ATSocket_Options_t
    {
        Calypso_ATSocket_OptionState_t keepAlive;                      /**< Enable/disable TCP keep active message (used with Calypso_ATSocket_SockOptSocket_KeepAlive)*/
        uint32_t keepAliveTimeSeconds;                                 /**< Keep alive timeout (used with Calypso_ATSocket_SockOptSocket_KeepAliveTime) */
        Calypso_ATSocket_OptionState_t rxNoIpBoundary;                 /**< Enable/disable RX IP boundary (used with Calypso_ATSocket_SockOptSocket_RXNoIPBoundary) */
        Calypso_ATSocket_ReceiveTimeout_t rcvTimeout;                  /**< Timeout value that specifies maximum amount of time an input function waits until it completes (used with Calypso_ATSocket_SockOptSocket_RCVTimeout) */
        uint32_t rcvBuf;                                               /**< TCP maximum receive window size (used with Calypso_ATSocket_SockOptSocket_RCVBuf) */
        Calypso_ATSocket_OptionState_t nonBlocking;                    /**< Set socket to non-blocking (used with Calypso_ATSocket_SockOptSocket_NonBlocking) */
        Calypso_ATSocket_SockOptSecMethod_t secMethod;                 /**< Sets security method to socket (used with Calypso_ATSocket_SockOptSocket_SecMethod) */
        uint32_t secureMask;                                           /**< Sets specific ciphers as bitmask (of Calypso_ATSocket_Cipher_t) to TCP secured socket (default value: all ciphers, used with Calypso_ATSocket_SockOptSocket_SecureMask) */
        char fileName[ATFILE_FILENAME_MAX_LENGTH];                     /**< File name used for Calypso_ATSocket_SockOptSocket_SecureFilesCAFileName, Calypso_ATSocket_SockOptSocket_SecureFilesPrivateKeyFileName, Calypso_ATSocket_SockOptSocket_SecureFilesCertificateFileName, Calypso_ATSocket_SockOptSocket_SecureFilesDHKeyFileName */
        char secureDomainNameVerification[ATFILE_FILENAME_MAX_LENGTH]; /**< Set a domain name, to check in SSL client connection (used with Calypso_ATSocket_SockOptSocket_SecureDomainNameVerification) */
        Calypso_ATSocket_OptionState_t disableCertificateStore;        /**< Disables the use of the on-board root CA catalogue */
        uint32_t multicastTTL;                                         /**< Set the time-to-live value of outgoing multicast packets (used with Calypso_ATSocket_SockOptIP_MulticastTTL) */
        Calypso_ATSocket_MulticastGroup_t multicastGroup;              /**< Used for joining/leaving a multicast group (UDP, used with Calypso_ATSocket_SockOptIP_AddMembership, Calypso_ATSocket_SockOptIP_DropMembership) */
    } Calypso_ATSocket_Options_t;

    extern bool Calypso_ATSocket_Create(Calypso_ATSocket_Family_t family, Calypso_ATSocket_Type_t type, Calypso_ATSocket_Protocol_t protocol, uint8_t* socketID);
    extern bool Calypso_ATSocket_Close(uint8_t socketID);
    extern bool Calypso_ATSocket_Bind(uint8_t socketID, Calypso_ATSocket_Descriptor_t socket);
    extern bool Calypso_ATSocket_Listen(uint8_t socketID, uint16_t backlog);
    extern bool Calypso_ATSocket_Connect(uint8_t socketID, Calypso_ATSocket_Descriptor_t remoteSocket);
    extern bool Calypso_ATSocket_Accept(uint8_t socketID, Calypso_ATSocket_Family_t family);
    extern bool Calypso_ATSocket_SetSocketOption(uint8_t socketID, Calypso_ATSocket_SockOptLevel_t level, uint8_t option, Calypso_ATSocket_Options_t* pValues);
    extern bool Calypso_ATSocket_GetSocketOption(uint8_t socketID, Calypso_ATSocket_SockOptLevel_t level, uint8_t option, Calypso_ATSocket_Options_t* pValues);
    extern bool Calypso_ATSocket_Receive(uint8_t socketID, Calypso_DataFormat_t format, uint16_t length);
    extern bool Calypso_ATSocket_ReceiveFrom(uint8_t socketID, Calypso_ATSocket_Descriptor_t remoteSocket, Calypso_DataFormat_t format, uint16_t length);
    extern bool Calypso_ATSocket_Send(uint8_t socketID, Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t length, char* data, uint16_t* bytesSent);
    extern bool Calypso_ATSocket_SendTo(uint8_t socketID, Calypso_ATSocket_Descriptor_t* remoteSocket, Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t length, char* data, uint16_t* bytesSent);

    extern bool Calypso_ATSocket_ParseSocketFamily(const char* familyString, Calypso_ATSocket_Family_t* pOutFamily);
    extern bool Calypso_ATSocket_GetSocketFamilyString(Calypso_ATSocket_Family_t family, char* pOutFamilyStr);

    extern bool Calypso_ATSocket_AppendSocketDescriptor(char* pAtCommand, Calypso_ATSocket_Descriptor_t socket, char lastDelim);
    extern bool Calypso_ATSocket_AppendCipherMask(char* pOutStr, uint32_t cipherMask);

#ifdef __cplusplus
}
#endif

#endif /* CALYPSO_AT_SOCKET_H_INCLUDED */
