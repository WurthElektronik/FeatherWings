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
#include <Calypso/ATCommands/ATSocket.h>
#include <Calypso/Calypso.h>
#include <global/global.h>

static const char* ATSocketFamilyString[Calypso_ATSocket_Family_NumberOfValues] = {"INET", "INET6"};

static const char* ATSocketTypeString[Calypso_ATSocket_Type_NumberOfValues] = {"STREAM", "DGRAM"};

static const char* ATSocketProcotolString[Calypso_ATSocket_Protocol_NumberOfValues] = {"TCP", "UDP", "SEC"};

static const char* ATSocketSockOptLevelString[Calypso_ATSocket_SockOptLevel_NumberOfValues] = {"SOCKET", "IP"};

static const char* ATSocketSockOptSocketString[Calypso_ATSocket_SockOptSocket_NumberOfValues] = {
    "KEEPALIVE", "KEEPALIVETIME", "RX_NO_IP_BOUNDARY", "RCVTIMEO", "RCVBUF", "NONBLOCKING", "SECMETHOD", "SECURE_MASK", "SECURE_FILES_CA_FILE_NAME", "SECURE_FILES_PRIVATE_KEY_FILE_NAME", "SECURE_FILES_CERTIFICATE_FILE_NAME", "SECURE_FILES_DH_KEY_FILE_NAME", "SECURE_DOMAIN_NAME_VERIFICATION", "DISABLE_CERTIFICATE_STORE"};

static const char* ATSocketSockOptIPString[Calypso_ATSocket_SockOptIP_NumberOfValues] = {"MULTICAST_TTL", "ADD_MEMBERSHIP", "DROP_MEMBERSHIP"};

static const char* AtSocketSockOptSecMethod[Calypso_ATSocket_SockOptSecMethod_NumberOfValues] = {"SSLV3", "TLSV1", "TLSV1_1", "TLSV1_2", "SSLV3_TLSV1_2"};

static const char* Calypso_ATSocket_CipherStrings[Calypso_ATSocket_Cipher_NumberOfValues] = {"SSL_RSA_WITH_RC4_128_SHA",
                                                                                             "SSL_RSA_WITH_RC4_128_MD5",
                                                                                             "TLS_RSA_WITH_AES_256_CBC_SHA",
                                                                                             "TLS_DHE_RSA_WITH_AES_256_CBC_SHA",
                                                                                             "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA",
                                                                                             "TLS_ECDHE_RSA_WITH_RC4_128_SHA",
                                                                                             "TLS_RSA_WITH_AES_128_CBC_SHA256",
                                                                                             "TLS_RSA_WITH_AES_256_CBC_SHA256",
                                                                                             "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256",
                                                                                             "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA",
                                                                                             "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA",
                                                                                             "TLS_RSA_WITH_AES_128_GCM_SHA256",
                                                                                             "TLS_RSA_WITH_AES_256_GCM_SHA384",
                                                                                             "TLS_DHE_RSA_WITH_AES_128_GCM_SHA256",
                                                                                             "TLS_DHE_RSA_WITH_AES_256_GCM_SHA384",
                                                                                             "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256",
                                                                                             "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384",
                                                                                             "TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256",
                                                                                             "TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384",
                                                                                             "TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256",
                                                                                             "TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256",
                                                                                             "TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256"};

static bool Calypso_ATSocket_AddArgumentsCreate(char* pAtCommand, Calypso_ATSocket_Family_t family, Calypso_ATSocket_Type_t type, Calypso_ATSocket_Protocol_t protocol);
static bool Calypso_ATSocket_AddArgumentsBindConnect(char* pAtCommand, uint8_t socketID, Calypso_ATSocket_Descriptor_t socket);
static bool Calypso_ATSocket_AddArgumentsRecv(char* pAtCommand, uint8_t socketID, Calypso_DataFormat_t format, uint16_t length);
static bool Calypso_ATSocket_AddArgumentsRecvFrom(char* pAtCommand, uint8_t socketID, Calypso_ATSocket_Descriptor_t socketFrom, Calypso_DataFormat_t format, uint16_t length);
static bool Calypso_ATSocket_AddArgumentsSendTo(char* pAtCommand, uint8_t socketID, Calypso_ATSocket_Descriptor_t* remoteSocket, Calypso_DataFormat_t format, uint16_t length, char* data);
static bool Calypso_ATSocket_AddArgumentsSetSockOpt(char* pAtCommand, uint8_t socketID, Calypso_ATSocket_SockOptLevel_t level, uint8_t option, Calypso_ATSocket_Options_t* data);

static bool Calypso_ATSocket_ParseResponseCreate(char** pAtCommand, uint8_t* pOutSocketID);
static bool Calypso_ATSocket_ParseResponseGetOptions(Calypso_ATSocket_SockOptLevel_t level, uint8_t option, char** pAtCommand, Calypso_ATSocket_Options_t* pValues);

/**
 * @brief Creates a socket (using the AT+socket command).
 *
 * @param[in] family Family of the socket. See Calypso_ATSocket_Family_t
 * @param[in] type Type of the socket. See Calypso_ATSocket_Type_t
 * @param[in] protocol Protocol of the socket. See Calypso_ATSocket_Protocol_t
 * @param[out] socketID ID (descriptor) assigned to the new socket. Can be used to access the socket from other functions.
 *
 * @return true if successful, false otherwise
 */

bool Calypso_ATSocket_Create(Calypso_ATSocket_Family_t family, Calypso_ATSocket_Type_t type, Calypso_ATSocket_Protocol_t protocol, uint8_t* socketID)
{

    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+socket=");

    if (!Calypso_ATSocket_AddArgumentsCreate(pRequestCommand, family, type, protocol))
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

    return Calypso_ATSocket_ParseResponseCreate(&pRespondCommand, socketID);
}

/**
 * @brief Closes a socket (using the AT+close command).
 *
 * @param[in] socketID ID of the socket to be closed
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_Close(uint8_t socketID)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+close=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
 * @brief Binds a socket to an IP address and a port (using the AT+bind command).
 *
 * @param[in] socketID ID of the socket to be bound
 * @param[in] socket Socket configuration. See Calypso_ATSocket_Descriptor_t.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_Bind(uint8_t socketID, Calypso_ATSocket_Descriptor_t socket)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+bind=");

    if (!Calypso_ATSocket_AddArgumentsBindConnect(pRequestCommand, socketID, socket))
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
 * Starts listening on the supplied socket (using the AT+listen command).
 *
 * @param[in] socketID ID of the socket on which to start listening
 * @param[in] backlog Max length of connect request queue
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_Listen(uint8_t socketID, uint16_t backlog)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+listen=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pRequestCommand, backlog, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
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
 * @brief Connects to a remote socket (using the AT+connect command).
 *
 * @param[in] socketID ID of the local socket to be connected
 * @param[in] remoteSocket Remote socket to connect to. See Calypso_ATSocket_Descriptor_t
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_Connect(uint8_t socketID, Calypso_ATSocket_Descriptor_t remoteSocket)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+connect=");

    if (!Calypso_ATSocket_AddArgumentsBindConnect(pRequestCommand, socketID, remoteSocket))
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
 * @brief Accepts incoming connections on a socket (using the AT+accept command).
 *
 * @param[in] socketID ID of the local socket to accept connections for
 * @param[in] family Family of connections to accept
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_Accept(uint8_t socketID, Calypso_ATSocket_Family_t family)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+accept=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATSocketFamilyString[family], ATCOMMAND_STRING_TERMINATE))
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
 * @brief Trigger receiving of data via the supplied local socket (using the AT+recv command).
 *
 * Note that any received data is provided asynchronously in events of type Calypso_ATEvent_SocketRcvd.
 *
 * @param[in] socketID ID of the local socket which should receive
 * @param[in] format Format in which the data is to be provided. Setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to encode the data as Base64 before sending it to this device via UART.
 * @param[in] length Max number of bytes to receive
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_Receive(uint8_t socketID, Calypso_DataFormat_t format, uint16_t length)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+recv=");

    if (!Calypso_ATSocket_AddArgumentsRecv(pRequestCommand, socketID, format, length))
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
 * Trigger receiving of data from the supplied remote socket via the supplied local socket (using the AT+recvFrom command).
 *
 * Note that any received data is provided asynchronously in events of type Calypso_ATEvent_SocketRcvdFrom.
 *
 * @param[in] socketID ID of the local socket which should receive
 * @param[in] remoteSocket Remote socket from which the data should be received
 * @param[in] format Format in which the data is to be provided. Setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to encode the data as Base64 before sending it to this device via UART.
 * @param[in] length Max number of bytes to receive
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_ReceiveFrom(uint8_t socketID, Calypso_ATSocket_Descriptor_t remoteSocket, Calypso_DataFormat_t format, uint16_t length)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+recvFrom=");

    if (!Calypso_ATSocket_AddArgumentsRecvFrom(pRequestCommand, socketID, remoteSocket, format, length))
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
 * @brief Sends data via the supplied local socket (using the AT+send command).
 *
 * @param[in] socketID ID of the local socket via which the data should be sent
 * @param[in] format Format in which the data is provided. Note that setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to interpret the data as Base64 - the module will decode the data and
 *                   transmit the decoded data via the socket. If using Calypso_DataFormat_Base64, you either need to
 *                   provide Base64 encoded data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64 Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] length Number of bytes to be sent
 * @param[in] data Data to be sent
 * @param[out] bytesSent The number of bytes that have been sent. Note that in case
 *                       encodeAsBase64 is true, the actual number of (encoded) bytes sent might be higher.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_Send(uint8_t socketID, Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t length, char* data, uint16_t* bytesSent) { return Calypso_ATSocket_SendTo(socketID, NULL, format, encodeAsBase64, length, data, bytesSent); }

/**
 * Sends data via the supplied local socket (using the AT+send or AT+sendTo command).
 *
 * The AT+sendTo command is used if a remote socket is supplied (UDP). Otherwise,
 * the AT+send command is used (TCP).
 *
 * @param[in] socketID ID of the local socket via which the data should be sent
 * @param[in] remoteSocket Remote socket to which the data should be sent (optional)
 * @param[in] format Format in which the data is provided. Note that setting the format to Calypso_DataFormat_Base64
 *                   causes the Calypso module to interpret the data as Base64 - the module will decode the data and
 *                   transmit the decoded data via the socket. If using Calypso_DataFormat_Base64, you either need to
 *                   provide Base64 encoded data or set encodeAsBase64 to true.
 * @param[in] encodeAsBase64 Encode the data in Base64 format before sending it to the Calypso module
 * @param[in] length Number of bytes to be sent
 * @param[in] data Data to be sent
 * @param[out] bytesSent The number of bytes that have been sent. Note that in case
 *                       encodeAsBase64 is true, the actual number of (encoded) bytes sent might be higher.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_SendTo(uint8_t socketID, Calypso_ATSocket_Descriptor_t* remoteSocket, Calypso_DataFormat_t format, bool encodeAsBase64, uint16_t length, char* data, uint16_t* bytesSent)
{
    *bytesSent = 0;

    if (encodeAsBase64)
    {
        /* Base64 encoded data might exceed the max. chunk size. To limit the required buffer size,
		 * the data is encoded in chunks, if necessary. */

        uint16_t maxChunkSize = (((CALYPSO_MAX_PAYLOAD_SIZE - 1) * 3) / 4) - 2;
        uint16_t chunkSize = 0;
        for (uint16_t chunkOffset = 0; chunkOffset < length; chunkOffset += chunkSize)
        {
            chunkSize = length - chunkOffset;
            if (chunkSize > maxChunkSize)
            {
                chunkSize = maxChunkSize;
            }

            /* Encode as Base64 */
            uint32_t lengthEncoded;
            if (!Base64_GetEncBufSize(chunkSize, &lengthEncoded))
            {
                return false;
            }
            char base64Buffer[lengthEncoded];
            if (!Base64_Encode((uint8_t*)data + chunkOffset, chunkSize, (uint8_t*)base64Buffer, &lengthEncoded))
            {
                return false;
            }

            /* Recursively call Calypso_ATSocket_Send() with the encoded binary data (excluding '\0') */
            uint16_t chunkBytesWritten = 0;
            bool ok = Calypso_ATSocket_SendTo(socketID, remoteSocket, format, false, lengthEncoded, base64Buffer, &chunkBytesWritten);
            if (!ok)
            {
                return false;
            }

            *bytesSent += chunkSize;
        }

        return true;
    }

    /* Send data using either AT+send or AT+sendTo, splitting the payload into
	 * chunks of max. CALYPSO_MAX_PAYLOAD_SIZE, if necessary. */
    uint16_t chunkBytesSent = 0;
    for (uint16_t chunkOffset = 0; chunkOffset < length; chunkOffset += chunkBytesSent)
    {
        uint16_t chunkSize = length - chunkOffset;
        if (chunkSize > CALYPSO_MAX_PAYLOAD_SIZE)
        {
            chunkSize = CALYPSO_MAX_PAYLOAD_SIZE;
        }

        char* pRequestCommand = AT_commandBuffer;

        if (NULL == remoteSocket)
        {
            /* Stream / TCP */
            strcpy(pRequestCommand, "AT+send=");
        }
        else
        {
            /* Datagram / UDP */
            strcpy(pRequestCommand, "AT+sendTo=");
        }

        if (!Calypso_ATSocket_AddArgumentsSendTo(pRequestCommand, socketID, remoteSocket, format, chunkSize, data + chunkOffset))
        {
            return false;
        }

        chunkBytesSent = chunkSize;

        if (!Calypso_SendRequest(pRequestCommand))
        {
            return false;
        }
        if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, NULL))
        {
            return false;
        }

        *bytesSent += chunkBytesSent;

    }

    return true;
}

/**
 * @brief Sets socket options (using the AT+setSockOpt command).
 *
 * @param[in] socketID ID of the local socket for which options should be set
 * @param[in] level Option level parameter
 * @param[in] option Option parameter
 * @param[in] pValues Option value. Uses the Calypso_ATSocket_Options_t member corresponding to the option being set.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_SetSocketOption(uint8_t socketID, Calypso_ATSocket_SockOptLevel_t level, uint8_t option, Calypso_ATSocket_Options_t* pValues)
{

    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+setSockOpt=");

    if (!Calypso_ATSocket_AddArgumentsSetSockOpt(pRequestCommand, socketID, level, option, pValues))
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
 * @brief Gets socket options (using the AT+getSockOpt command).
 *
 * @param[in] socketID ID of the local socket for which options should be set
 * @param[in] level Option level parameter
 * @param[in] option Option parameter
 * @param[out] pValues Option value. Uses the Calypso_ATSocket_Options_t member corresponding to the option being retrieved.
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_GetSocketOption(uint8_t socketID, Calypso_ATSocket_SockOptLevel_t level, uint8_t option, Calypso_ATSocket_Options_t* pValues)
{
    char* pRequestCommand = AT_commandBuffer;
    char* pRespondCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+getSockOpt=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, socketID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATSocketSockOptLevelString[level], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (Calypso_ATSocket_SockOptLevel_Socket == level)
    {
        if (option == Calypso_ATSocket_SockOptSocket_DisableCertificateStore)
        {
            /* Not supported */
            return false;
        }
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATSocketSockOptSocketString[option], ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    else if (Calypso_ATSocket_SockOptLevel_IP == level)
    {
        if (!ATCommand_AppendArgumentString(pRequestCommand, ATSocketSockOptIPString[option], ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    else
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
    if (!Calypso_WaitForConfirm(Calypso_GetTimeout(Calypso_Timeout_General), Calypso_CNFStatus_Success, pRespondCommand))
    {
        return false;
    }

    return Calypso_ATSocket_ParseResponseGetOptions(level, option, &pRespondCommand, pValues);
}

/**
 * @brief Adds socket descriptor arguments to the AT command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socket Socket information to append
 * @param[in] lastDelim Delimiter to append after the last argument
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_AppendSocketDescriptor(char* pAtCommand, Calypso_ATSocket_Descriptor_t socket, char lastDelim)
{
    if (socket.family >= Calypso_ATSocket_Family_NumberOfValues)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, ATSocketFamilyString[socket.family], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, socket.port, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, socket.address, lastDelim);
}

/**
 * @brief Adds arguments to the AT+socket command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] family Family of the socket. See Calypso_ATSocket_Family_t
 * @param[in] type Type of the socket. See Calypso_ATSocket_Type_t
 * @param[in] protocol Protocol of the socket. See Calypso_ATSocket_Protocol_t
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATSocket_AddArgumentsCreate(char* pAtCommand, Calypso_ATSocket_Family_t family, Calypso_ATSocket_Type_t type, Calypso_ATSocket_Protocol_t protocol)
{

    if (family >= Calypso_ATSocket_Family_NumberOfValues)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, ATSocketFamilyString[family], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (type >= Calypso_ATSocket_Type_NumberOfValues)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, ATSocketTypeString[type], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (protocol >= Calypso_ATSocket_Protocol_NumberOfValues)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, ATSocketProcotolString[protocol], ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+bind / AT+connect command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socketID ID of the socket to be bound
 * @param[in] socket Socket configuration. See Calypso_ATSocket_Descriptor_t.
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATSocket_AddArgumentsBindConnect(char* pAtCommand, uint8_t socketID, Calypso_ATSocket_Descriptor_t socket)
{

    if (!ATCommand_AppendArgumentInt(pAtCommand, socketID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!Calypso_ATSocket_AppendSocketDescriptor(pAtCommand, socket, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+recv command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socketID ID of the local socket which should receive
 * @param[in] format Format in which the data is to be provided. The module will encode the data and will provide it in this format.
 * @param[in] length Max number of bytes to receive
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATSocket_AddArgumentsRecv(char* pAtCommand, uint8_t socketID, Calypso_DataFormat_t format, uint16_t length)
{

    if (format >= Calypso_DataFormat_NumberOfValues)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, socketID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, length < CALYPSO_MAX_PAYLOAD_SIZE ? length : CALYPSO_MAX_PAYLOAD_SIZE, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+recvFrom command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socketID ID of the local socket which should receive
 * @param[in] socketFrom Remote socket from which the data should be received
 * @param[in] format Format in which the data is to be provided. The module will encode the data and will provide it in this format.
 * @param[in] length Max number of bytes to receive
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATSocket_AddArgumentsRecvFrom(char* pAtCommand, uint8_t socketID, Calypso_ATSocket_Descriptor_t socketFrom, Calypso_DataFormat_t format, uint16_t length)
{

    if (!ATCommand_AppendArgumentInt(pAtCommand, socketID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!Calypso_ATSocket_AppendSocketDescriptor(pAtCommand, socketFrom, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, length < CALYPSO_MAX_PAYLOAD_SIZE ? length : CALYPSO_MAX_PAYLOAD_SIZE, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+send or AT+sendTo command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socketID ID of the local socket via which the data should be sent
 * @param[in] remoteSocket Remote socket to which the data should be sent. Optional (to be used with AT+sendTo).
 * @param[in] format Format in which the data is provided
 * @param[in] length Number of bytes to be sent
 * @param[in] data Data to be sent
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATSocket_AddArgumentsSendTo(char* pAtCommand, uint8_t socketID, Calypso_ATSocket_Descriptor_t* remoteSocket, Calypso_DataFormat_t format, uint16_t length, char* data)
{
    if (format >= Calypso_DataFormat_NumberOfValues)
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, socketID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (NULL != remoteSocket)
    {
        if (!Calypso_ATSocket_AppendSocketDescriptor(pAtCommand, *remoteSocket, ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, format, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentInt(pAtCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentBytes(pAtCommand, data, length, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Adds arguments to the AT+setSockOpt command string.
 *
 * @param[out] pAtCommand The AT command string to add the arguments to
 * @param[in] socketID ID of the local socket for which options should be set
 * @param[in] level Option level parameter
 * @param[in] option Option parameter
 * @param[in] pValues Option value
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATSocket_AddArgumentsSetSockOpt(char* pAtCommand, uint8_t socketID, Calypso_ATSocket_SockOptLevel_t level, uint8_t option, Calypso_ATSocket_Options_t* pValues)
{

    if (!ATCommand_AppendArgumentInt(pAtCommand, socketID, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (!ATCommand_AppendArgumentString(pAtCommand, ATSocketSockOptLevelString[level], ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    if (Calypso_ATSocket_SockOptLevel_Socket == level)
    {
        if (!ATCommand_AppendArgumentString(pAtCommand, ATSocketSockOptSocketString[option], ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        switch (option)
        {
            case Calypso_ATSocket_SockOptSocket_KeepAlive:
                if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->keepAlive, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            case Calypso_ATSocket_SockOptSocket_KeepAliveTime:
                if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->keepAliveTimeSeconds, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            case Calypso_ATSocket_SockOptSocket_RXNoIPBoundary:
                if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->rxNoIpBoundary, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            case Calypso_ATSocket_SockOptSocket_RCVTimeout:
                if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->rcvTimeout.seconds, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }
                if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->rcvTimeout.microseconds, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            case Calypso_ATSocket_SockOptSocket_RCVBuf:
                if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->rcvBuf, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            case Calypso_ATSocket_SockOptSocket_NonBlocking:
                if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->nonBlocking, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            case Calypso_ATSocket_SockOptSocket_SecMethod:
                if (!ATCommand_AppendArgumentString(pAtCommand, AtSocketSockOptSecMethod[pValues->secMethod], ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            case Calypso_ATSocket_SockOptSocket_SecureMask:
                if (!Calypso_ATSocket_AppendCipherMask(pAtCommand, pValues->secureMask))
                {
                    return false;
                }
                break;

            case Calypso_ATSocket_SockOptSocket_SecureFilesCAFileName:
            case Calypso_ATSocket_SockOptSocket_SecureFilesPrivateKeyFileName:
            case Calypso_ATSocket_SockOptSocket_SecureFilesCertificateFileName:
            case Calypso_ATSocket_SockOptSocket_SecureFilesDHKeyFileName:
                /* File name strings to append */
                if (!ATCommand_AppendArgumentString(pAtCommand, pValues->fileName, ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            case Calypso_ATSocket_SockOptSocket_SecureDomainNameVerification:
                if (!ATCommand_AppendArgumentString(pAtCommand, pValues->secureDomainNameVerification, ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            case Calypso_ATSocket_SockOptSocket_DisableCertificateStore:
                if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->disableCertificateStore, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            default:
            {
                /* Invalid option or not implemented */
                return false;
            }
        }
    }
    else if (Calypso_ATSocket_SockOptLevel_IP == level)
    {
        if (!ATCommand_AppendArgumentString(pAtCommand, ATSocketSockOptIPString[option], ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }

        switch (option)
        {
            case Calypso_ATSocket_SockOptIP_MulticastTTL:
                if (!ATCommand_AppendArgumentInt(pAtCommand, pValues->multicastTTL, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            case Calypso_ATSocket_SockOptIP_AddMembership:
            case Calypso_ATSocket_SockOptIP_DropMembership:
                if (!ATCommand_AppendArgumentString(pAtCommand, pValues->multicastGroup.ipv4Address, ATCOMMAND_ARGUMENT_DELIM))
                {
                    return false;
                }
                if (!ATCommand_AppendArgumentString(pAtCommand, pValues->multicastGroup.interfaceAddress, ATCOMMAND_STRING_TERMINATE))
                {
                    return false;
                }
                break;

            default:
                /* Invalid option or not implemented */
                return false;
        }
    }
    else
    {
        return false;
    }

    return ATCommand_AppendArgumentString(pAtCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the response to the AT+socket command.
 *
 * @param[in,out] pAtCommand The string received in response to the AT+socket command
 * @param[out] pOutSocketID The parsed socket ID
 *
 * @return true if successful, false otherwise
 */
static bool Calypso_ATSocket_ParseResponseCreate(char** pAtCommand, uint8_t* pOutSocketID)
{
    const char* cmd = "+socket:";
    const size_t cmdLength = strlen(cmd);

    /* Check if response is for this command */
    if (0 != strncmp(*pAtCommand, cmd, cmdLength))
    {
        return false;
    }
    *pAtCommand += cmdLength;
    return ATCommand_GetNextArgumentInt(pAtCommand, pOutSocketID, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

static bool Calypso_ATSocket_ParseResponseGetOptions(Calypso_ATSocket_SockOptLevel_t level, uint8_t option, char** pAtCommand, Calypso_ATSocket_Options_t* pValues)
{

    const char* expectedCmd = "+getsockopt:";
    const size_t cmdLength = strlen(expectedCmd);

    char tempString[50];

    /* Check if response is for getSockOpt */
    if (0 != strncmp(*pAtCommand, expectedCmd, cmdLength))
    {
        return false;
    }

    *pAtCommand += cmdLength;

    switch (level)
    {
        case Calypso_ATSocket_SockOptLevel_Socket:
        {
            switch (option)
            {
                case Calypso_ATSocket_SockOptSocket_KeepAlive:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->keepAlive, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATSocket_SockOptSocket_KeepAliveTime:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->keepAliveTimeSeconds, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATSocket_SockOptSocket_RXNoIPBoundary:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->rxNoIpBoundary, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATSocket_SockOptSocket_RCVTimeout:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->rcvTimeout.seconds, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
                    {
                        return false;
                    }
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->rcvTimeout.microseconds, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATSocket_SockOptSocket_RCVBuf:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->rcvBuf, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATSocket_SockOptSocket_NonBlocking:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->nonBlocking, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATSocket_SockOptSocket_SecMethod:
                {
                    bool ret;
                    if (!ATCommand_GetNextArgumentString(pAtCommand, tempString, ATCOMMAND_STRING_TERMINATE, sizeof(tempString)))
                    {
                        return false;
                    }
                    pValues->secMethod = ATCommand_FindString(AtSocketSockOptSecMethod, Calypso_ATSocket_SockOptSecMethod_NumberOfValues, tempString, Calypso_ATSocket_SockOptSecMethod_SSLv3, &ret);
                    if (!ret)
                    {
                        return false;
                    }
                    break;
                }

                case Calypso_ATSocket_SockOptSocket_SecureMask:
                    if (!ATCommand_GetNextArgumentBitmask(pAtCommand, Calypso_ATSocket_CipherStrings, Calypso_ATSocket_Cipher_NumberOfValues, 50, &pValues->secureMask, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATSocket_SockOptSocket_SecureFilesCAFileName:
                case Calypso_ATSocket_SockOptSocket_SecureFilesPrivateKeyFileName:
                case Calypso_ATSocket_SockOptSocket_SecureFilesCertificateFileName:
                case Calypso_ATSocket_SockOptSocket_SecureFilesDHKeyFileName:
                    if (!ATCommand_GetNextArgumentString(pAtCommand, pValues->fileName, ATCOMMAND_STRING_TERMINATE, sizeof(pValues->fileName)))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATSocket_SockOptSocket_SecureDomainNameVerification:
                    if (!ATCommand_GetNextArgumentString(pAtCommand, pValues->secureDomainNameVerification, ATCOMMAND_STRING_TERMINATE, sizeof(pValues->secureDomainNameVerification)))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATSocket_SockOptSocket_DisableCertificateStore:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->disableCertificateStore, ATCOMMAND_INTFLAGS_SIZE8 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                default:
                    return false;
            }
            break;
        }

        case Calypso_ATSocket_SockOptLevel_IP:
            switch (option)
            {
                case Calypso_ATSocket_SockOptIP_MulticastTTL:
                    if (!ATCommand_GetNextArgumentInt(pAtCommand, &pValues->multicastTTL, ATCOMMAND_INTFLAGS_SIZE32 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE))
                    {
                        return false;
                    }
                    break;

                case Calypso_ATSocket_SockOptIP_AddMembership:
                case Calypso_ATSocket_SockOptIP_DropMembership:
                    if (!ATCommand_GetNextArgumentString(pAtCommand, pValues->multicastGroup.ipv4Address, ATCOMMAND_ARGUMENT_DELIM, sizeof(pValues->multicastGroup.ipv4Address)))
                    {
                        return false;
                    }
                    if (!ATCommand_GetNextArgumentString(pAtCommand, pValues->multicastGroup.interfaceAddress, ATCOMMAND_STRING_TERMINATE, sizeof(pValues->multicastGroup.interfaceAddress)))
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
 * @brief Parses a string to Calypso_ATSocket_Family_t.
 *
 * @param[in] familyString String representing the socket family
 * @param[out] pOutFamily The parsed socket family
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_ParseSocketFamily(const char* familyString, Calypso_ATSocket_Family_t* pOutFamily)
{
    bool ok;
    *pOutFamily = ATCommand_FindString(ATSocketFamilyString, Calypso_ATSocket_Family_NumberOfValues, familyString, Calypso_ATSocket_Family_INET, &ok);
    return ok;
}

/**
 * @brief Returns the string representation of the supplied socket family.
 *
 * @param[in] family Socket family
 * @param[out] pOutFamilyStr String representation of the supplied socket family
 *
 * @return true if successful, false otherwise
 */
bool Calypso_ATSocket_GetSocketFamilyString(Calypso_ATSocket_Family_t family, char* pOutFamilyStr)
{
    if (family >= Calypso_ATSocket_Family_NumberOfValues)
    {
        return false;
    }
    strcpy(pOutFamilyStr, ATSocketFamilyString[family]);
    return true;
}

/**
 * @brief Appends cipher mask flags to the supplied string.
 *
 * @param[out] pOutStr The string to which the flags should be added (must be null terminated)
 * @param[in] cipherMask Cipher mask flags (see Calypso_ATSocket_Cipher_t)
 */
bool Calypso_ATSocket_AppendCipherMask(char* pOutStr, uint32_t cipherMask) { return ATCommand_AppendArgumentBitmask(pOutStr, Calypso_ATSocket_CipherStrings, Calypso_ATSocket_Cipher_NumberOfValues, cipherMask, ATCOMMAND_STRING_TERMINATE, AT_MAX_COMMAND_BUFFER_SIZE); }
