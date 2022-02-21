/**
 * \file
 * \brief File for the calypso of the WE IoT design kit.
 *
 * \copyright (c) 2020 Würth Elektronik eiSos GmbH & Co. KG
 *
 * \page License
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 * THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 * KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 * INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 * INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 * REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 * PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY
 * RIGHT RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT
 * IS USED. INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING
 * THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH
 * ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR
 * ENDORSEMENT THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS PACKAGE
 */
#include "calypso.h"

static const char *ATWLAN_SecurityTypeStrings[] =
    {
        ATWLAN_SECURITY_TYPE(GENERATE_STRING)};

static const char *ATWLAN_SecurityEAPStrings[] =
    {
        ATWLAN_SECURITY_EAP(GENERATE_STRING)};

static const char *ATSocketFamilyString[] =
    {
        "INET",
        "INET6",
};

static const char *ATMQTT_SecurityMethodsStrings[] =
    {
        ATMQTT_SECURITY_METHOD(GENERATE_STRING)};

static const char *ATMQTT_CipherStrings[] =
    {
        ATMQTT_CIPHER(GENERATE_STRING)};

static const char *ATMQTT_ProtocolStrings[] =
    {
        ATMQTT_PROTOCOL(GENERATE_STRING)};

static const char *ATMQTT_QoSStrings[] =
    {
        ATMQTT_QOS(GENERATE_STRING)};

static const char *ATMQTT_SetOptionStrings[] =
    {
        ATMQTT_SET_OPTION(GENERATE_STRING)};

static const char *ATMQTT_CreateFlagsStrings[] =
    {
        "ip4", "ip6", "url", "sec", "skip_domain_verify", "skip_cert_verify", "skip_date_verify"};

static uint8_t Calypso_base64EncTable[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                             'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                             'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                             'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                             'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                             'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                             'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                             '4', '5', '6', '7', '8', '9', '+', '/'};

static uint8_t Calypso_base64DecTable[123] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,
                                              52, 53, 54, 55, 56, 57, 58, 59, 60, 61, /* 0-9 */
                                              0, 0, 0, 0, 0, 0, 0,
                                              0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, /* A-Z */
                                              0, 0, 0, 0, 0, 0,
                                              26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}; /* a-z */

static const char *ATFile_OpenOptions_Strings[] =
    {
        "CREATE", "READ", "WRITE", "OVERWRITE", "CREATE_FAILSAFE", "CREATE_SECURE", "CREATE_NOSIGNITURE", "CREATE_STATIC_TOKEN", "CREATE_VENDOR_TOKEN", "CREATE_PUBLIC_WRITE", "CREATE_PUBLIC_READ"};

bool Calypso_appendArgumentInt(char *pOutString, uint32_t pInValue, uint16_t intflags, char delimeter);
bool Calypso_parseInt(char *pOutString, uint32_t pInInt, uint16_t intFlags);
uint32_t Calypso_getBase64EncBufSize(uint32_t inputLength);
uint32_t Calypso_getBase64DecBufSize(uint8_t *inputData, uint32_t inputLength);

/**
 * Get Base64 decoded buffer size
 *
 * This routine calculate the expected raw data buffer size
 * by given base64 buffer and buffer size
 *
 * input:
 * -   inputData       -  source buffer which hold the base64 data
 * -   inputLength     -  length of base64 data
 *
 * return         function shall return expected size.
*/
uint32_t Calypso_getBase64DecBufSize(uint8_t *inputData, uint32_t inputLength)
{
    uint32_t outputLength = inputLength / 4 * 3;

    if (outputLength == 0)
    {
        return 0;
    }
    if (inputData[inputLength - 1] == '=')
        outputLength--;
    if (inputData[inputLength - 2] == '=')
        outputLength--;

    return outputLength;
}

/**
 * Decode Base64 data to raw data
 *
 * This routine decode a given data in Base64 format to raw data,
 * and return it into a given buffer - outputData (which should be already allocated).
 * size of the outputData buffer also be returned
 *
 * input;
 *  - inputData    source buffer which hold the Base64 data
 *  - inputLength  source buffer size
 *
 * output:
 * - outputData    destination buffer which hold the raw data
 * - outputLength  destination buffer size
 *
 * return         true if successfull, false otherwise
*/
bool Calypso_decodeBase64(uint8_t *inputData, uint32_t inputLength, uint8_t *outputData, uint32_t *outputLength)
{
    uint32_t decode_value;
    uint32_t nibble6_1, nibble6_2, nibble6_3, nibble6_4;
    uint32_t i, j;

    if (inputLength % 4 != 0)
    {
        return false;
    }

    if (outputData == NULL)
    {
        return false;
    }

    *outputLength = Calypso_getBase64DecBufSize(inputData, inputLength);

    for (i = 0, j = 0; i < inputLength;)
    {
        nibble6_1 = inputData[i] == '=' ? 0 & i++ : Calypso_base64DecTable[inputData[i++]];
        nibble6_2 = inputData[i] == '=' ? 0 & i++ : Calypso_base64DecTable[inputData[i++]];
        nibble6_3 = inputData[i] == '=' ? 0 & i++ : Calypso_base64DecTable[inputData[i++]];
        nibble6_4 = inputData[i] == '=' ? 0 & i++ : Calypso_base64DecTable[inputData[i++]];

        decode_value = (nibble6_1 << 3 * 6) + (nibble6_2 << 2 * 6) + (nibble6_3 << 1 * 6) + (nibble6_4 << 0 * 6);

        if (j < *outputLength)
            outputData[j++] = (decode_value >> 2 * 8) & 0xFF;
        if (j < *outputLength)
            outputData[j++] = (decode_value >> 1 * 8) & 0xFF;
        if (j < *outputLength)
            outputData[j++] = (decode_value >> 0 * 8) & 0xFF;
    }
    outputData[j] = 0;

    return true;
}
/**
 * @brief  Encode data using base64 encoding
 * @param  inputData Pointer to the input data.
 * @param  inputLength Length of the input data
 * @param  outputData Pointer to output data
 * @param  outputLength Pointer to output data length
 * @retval true if successful false in case of failure
 */
bool Calypso_encodeBase64(uint8_t *inputData, uint32_t inputLength, uint8_t *outputData, uint32_t *outputLength)
{
    uint32_t encodeValue;
    uint32_t nibble6_1, nibble6_2, nibble6_3;
    uint32_t i, j;

    *outputLength = Calypso_getBase64EncBufSize(inputLength);

    if (outputData == NULL)
    {
        return false;
        ;
    }

    for (i = 0, j = 0; i < inputLength;)
    {

        nibble6_1 = i < inputLength ? inputData[i++] : 0;
        nibble6_2 = i < inputLength ? inputData[i++] : 0;
        nibble6_3 = i < inputLength ? inputData[i++] : 0;

        encodeValue = (nibble6_1 << 0x10) + (nibble6_2 << 0x08) + nibble6_3;

        outputData[j++] = Calypso_base64EncTable[(encodeValue >> 3 * 6) & 0x3F];
        outputData[j++] = Calypso_base64EncTable[(encodeValue >> 2 * 6) & 0x3F];
        outputData[j++] = Calypso_base64EncTable[(encodeValue >> 1 * 6) & 0x3F];
        outputData[j++] = Calypso_base64EncTable[(encodeValue >> 0 * 6) & 0x3F];
    }

    if (inputLength % 3 >= 1)
        outputData[*outputLength - 1] = '=';
    if (inputLength % 3 == 1)
        outputData[*outputLength - 2] = '=';

    outputData[*outputLength] = 0;

    return true;
}

/**
 * @brief  Get the length of base64 encoded data
 * @param  inputLength Length of the input data
 * @retval Length of base64 encoded data
 */
uint32_t Calypso_getBase64EncBufSize(uint32_t inputLength)
{
    return (4 * ((inputLength + 2) / 3));
}

/**
* @brief Parses int to string
*
* @param pOutString Pointer to parsed string
* @param pInInt Pointer to int value to parse
* @param intFlags flags to determine how to parse
*
* @RetVal true if successful, false otherwise
*/
bool Calypso_parseInt(char *pOutString, uint32_t pInInt, uint16_t intFlags)
{
    if ((0 == (intFlags & INTFLAGS_SIGN)) || (0 == (intFlags & INTFLAGS_NOTATION)))
    {
        return false;
    }

    /* HEX*/
    if (INTFLAGS_NOTATION_HEX == (intFlags & INTFLAGS_NOTATION))
    {
        sprintf(pOutString, "0x%lx", pInInt);
    }
    /* DEC */
    else
    {

        /* UNSIGNED */
        if (INTFLAGS_UNSIGNED == (intFlags & INTFLAGS_SIGN))
        {
            sprintf(pOutString, "%lu", pInInt);
        }
        /* SIGNED */
        else
        {
            sprintf(pOutString, "%ld", pInInt);
        }
    }

    return true;
}

/**
* @brief Appends a string argument to the end of the at command
*
* @param pOutString Pointer to at-command to append argument
* @param pInArgument String of argument to add
* @param delimeter delimeter to append after argument
*
* return true if successful, false otherwise
*/
bool Calypso_appendArgumentString(char *pOutString, const char *pInArgument, char delimeter)
{
    if ((NULL == pOutString))
    {
        return false;
    }

    if (NULL != pInArgument)
    {
        strcpy(&pOutString[strlen(pOutString)], pInArgument);
    }
    pOutString[strlen(pOutString)] = delimeter;

    return true;
}

/**
* Appends an int argument to the end of the at command
*
* @param pOutString Pointer to at-command to append argument
* @param pInValue String of argument to add
* @param intFlags flags to determine how to parse
* @param delimeter delimeter to append after argument
*
* return true if successful, false otherwise
*/
bool Calypso_appendArgumentInt(char *pOutString, uint32_t pInValue, uint16_t intflags, char delimeter)
{
    if ((NULL == pOutString))
    {
        return false;
    }

    char tempString[12];

    if (Calypso_parseInt(tempString, pInValue, intflags))
    {

        return Calypso_appendArgumentString(pOutString, tempString, delimeter);
    }

    return false;
}

/**
* @brief Gets next string argument from at-command.
*
* @param pOutargument Pointer to argument as string
* @param pInArguments Pointer to string containtin at-command to get argument from
* @param delimeter delimeter which occurs after argument to get
*
* return true if successful, false otherwise
*/
bool Calypso_getNextArgumentString(char **pInArguments, char *pOutargument, char delim)
{
    if ((NULL == pInArguments) || (NULL == pOutargument))
    {
        return false;
    }

    size_t argumentLength = 0;
    size_t inputStringLength = strlen(*pInArguments);

    while (true)
    {
        if (((*pInArguments)[argumentLength] == delim))
        {
            /* if argument list not empty*/
            /* copy string to output arguments */
            memcpy(pOutargument, *pInArguments, argumentLength);
            memset(&pOutargument[argumentLength], '\0', 1);

            *pInArguments = &((*pInArguments)[argumentLength + 1]);
            return true;
        }
        else if (argumentLength > inputStringLength)
        {
            return false;
        }
        argumentLength++;
    }
}

/**
* @brief Gets command name from at-command.
*
* @param pCmdName Pointer to command name as string
* @param pInArguments Pointer to at-command to get command name from
* @param delimeter delimetet which occurs after command name
*
*
* @RetVal true if successful, false otherwise
*/
bool Calypso_getCmdName(char **pInAtCmd, char *pCmdName, char delim)
{
    if ((NULL == pInAtCmd) || (NULL == pCmdName))
    {
        return false;
    }

    size_t argumentLength = 0;
    size_t inputStringLength = strlen(*pInAtCmd);

    while (true)
    {
        if (((*pInAtCmd)[argumentLength] == delim))
        {
            /* if argument list not empty*/
            /* copy string to output arguments */
            memcpy(pCmdName, *pInAtCmd, argumentLength);
            memset(&pCmdName[argumentLength], '\0', 1);

            *pInAtCmd = &((*pInAtCmd)[argumentLength + 1]);
            return true;
        }
        else if (argumentLength > inputStringLength)
        {
            return false;
        }
        argumentLength++;
    }
}

/**
* @brief Gets next int argument from at-command.
*
* @param pOutargument argument as parsed as int
* @param pInArguments Pointer to at-command to get argument from
* @param intFlags flags to determine how to parse
* @param delimeter delimetet which occurs after argument to get
* *
* @RetVal true if successful, false otherwise
*/
bool Calypso_getNextArgumentInt(char **pInArguments, void *pOutargument, uint16_t intflags, char delim)
{
    if ((NULL == pInArguments) || (NULL == pOutargument))
    {
        return false;
    }

    char tempString[12];

    Calypso_getNextArgumentString(pInArguments, tempString, delim);
    return Calypso_StringToInt(pOutargument, tempString, intflags);
}

/**
* @brief Parses int string to int
*
* @param pOutInt Pointer to parsed int value
* @param pInString int string to parse
* @param intFlags flags to determine how to parse
*
* @RetVal true if successful, false otherwise
*/
bool Calypso_StringToInt(void *pOutInt, const char *pInString, uint16_t intFlags)
{
    uint32_t resultUnsigned;
    int32_t resultSigned;
    uint8_t figure;
    size_t argumentLength = 0;
    char currentChar = '0';
    bool isNegative = false;

    if ((NULL == pInString) || (NULL == pOutInt))
    {
        return false;
    }

    argumentLength = strlen(pInString);

    resultUnsigned = 0;
    resultSigned = 0;
    figure = 0;
    /* Hex notation starts with 0x */
    if (0 == strncmp(pInString, "0x", 2))
    {
        pInString += 2;
        argumentLength -= 2;
        for (int i = 0; i < argumentLength; i++)
        {
            currentChar = pInString[i];

            if ((currentChar >= '0') && (currentChar <= '9'))
            {
                figure = (0x0F & (uint8_t)(currentChar - '0'));
            }
            else if ((currentChar >= 'A') && (currentChar <= 'F'))
            {
                figure = (0x0F & ((uint8_t)(currentChar - 'A') + 10));
            }
            else if ((currentChar >= 'a') && (currentChar <= 'f'))
            {
                figure = (0x0F & ((uint8_t)(currentChar - 'a') + 10));
            }

            resultUnsigned = (resultUnsigned << 4) + figure;
        }

        memcpy(pOutInt, &resultUnsigned, (intFlags & INTFLAGS_SIZE));
    }
    else
    {
        if ('-' == pInString[0])
        {
            isNegative = true;
            pInString++;
            argumentLength--;
        }

        for (int i = 0; i < argumentLength; i++)
        {
            figure = (uint8_t)(pInString[i] - '0');
            resultUnsigned = (resultUnsigned * 10) + figure;
        }

        if (isNegative)
        {
            resultSigned = 0 - resultUnsigned;
            memcpy(pOutInt, &resultSigned, (intFlags & INTFLAGS_SIZE));
        }
        else
        {
            memcpy(pOutInt, &resultUnsigned, (intFlags & INTFLAGS_SIZE));
        }
    }

    return true;
}

/**
* @brief Adds the arguments to the request command string
*
* @param pOutString the request command string to add the arguments to
* @param connectionArgs the arguments of the command. See ATWLAN_ConnectionArguments_t
* @param lastDelim the delimeter after the last argument.
*
* @RetVal true if arguments were added successful, false otherwise
*/
bool ATWLAN_addConnectionArguments(char *pOutString, ATWLAN_ConnectionArguments_t connectionArgs, char lastDelim)
{
    bool ret = false;

    ret = Calypso_appendArgumentString(pOutString, connectionArgs.SSID, ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_appendArgumentString(pOutString, connectionArgs.BSSID, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pOutString, ATWLAN_SecurityTypeStrings[connectionArgs.securityParams.securityType], ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pOutString, connectionArgs.securityParams.securityKey, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pOutString, connectionArgs.securityExtParams.extUser, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pOutString, connectionArgs.securityExtParams.extAnonUser, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pOutString, ATWLAN_SecurityEAPStrings[connectionArgs.securityExtParams.eapMethod], lastDelim);
    }

    return ret;
}

/**
* @brief Parses a string to ATSocket_Family_t
*
* @param pOutFamily ATSocket_Family_t representing the socket family
* @param familyString string representing the socket family
*
* @RetVal true if parsed successful, false otherwise
*/
bool ATSocket_parseSocketFamily(const char *familyString, ATSocket_Family_t *pOutFamily)
{
    for (int i = 0; i < ATSocket_Family_NumberOfValues; i++)
    {
        if (0 == strcasecmp(familyString, ATSocketFamilyString[i]))
        {
            *pOutFamily = (ATSocket_Family_t)i;
            return true;
        }
    }

    return false;
}

/**
* @brief Adds the arguments to the request command string
*
* @param pAtCommand the request command string to add the arguments to
* @param clientID: client ID of the new
* @param flags:
* @param serverInfo: server address and server ip. See ATMQTT_ServerInfo_t.
* @param securityParams: security parameters. See securityMethod.
* @param connectionParams: connection parameters. See ATMQTT_connectionParams_t.
*
* @RetVal true if successful, false otherwise
*/
bool ATMQTT_addArgumentsCreate(char *pAtCommand, char *clientID, uint32_t flags, ATMQTT_ServerInfo_t serverInfo, ATMQTT_securityParams_t securityParams, ATMQTT_connectionParams_t connectionParams)
{

    bool ret = false;

    ret = Calypso_appendArgumentString(pAtCommand, clientID, ARGUMENT_DELIM);

    if (ret)
    {
        char optionString[128];
        memset(optionString, 0, 128);

        uint32_t bitmask;
        for (int i = 0; i < ATMQTT_CREATE_FLAGS_NUM_OF_BITS; i++)
        {
            bitmask = (1 << i);
            if (0 != (bitmask & flags))
            {
                Calypso_appendArgumentString(optionString, ATMQTT_CreateFlagsStrings[i], BITMASK_DELIM);
            }
        }
        /* delete last '|' */
        optionString[strlen(optionString) - 1] = '\0';
        ret = Calypso_appendArgumentString(pAtCommand, optionString, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, serverInfo.address, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, serverInfo.port, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, ATMQTT_SecurityMethodsStrings[securityParams.securityMethod], ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, ATMQTT_CipherStrings[securityParams.cipher], ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, securityParams.privateKeyFile, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, securityParams.certificateFile, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, securityParams.CAFile, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, securityParams.DHKey, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, ATMQTT_ProtocolStrings[connectionParams.protocolVersion], ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, connectionParams.blockingSend, (INTFLAGS_UNSIGNED | INTFLAGS_NOTATION_DEC), ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, connectionParams.format, (INTFLAGS_UNSIGNED | INTFLAGS_NOTATION_DEC), STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, CRLF, STRING_TERMINATE);
    }

    return ret;
}

/**
* @brief Adds the arguments to the request command string
*
* @param pAtCommand the request command string to add the arguments to
* @param index index of MQTT client to to publish.
* @param topicString topic to publish to
* @param retain retain the message(0) or not (1)
* @param messageLength length of the message
* @param pMessage message to publish
*
* @RetVal true if successful, false otherwise
*/
bool ATMQTT_addArgumentsSet(char *pAtCommand, uint8_t index, uint8_t option, void *pValues)
{
    bool ret = false;

    ret = Calypso_appendArgumentInt(pAtCommand, index, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);

    if (ret && (option < ATMQTT_SET_OPTION_NumberOfValues))
    {
        ret = Calypso_appendArgumentString(pAtCommand, ATMQTT_SetOptionStrings[option], ARGUMENT_DELIM);
    }
    else
    {
        ret = false;
    }

    if (ret)
    {
        switch (option)
        {
        case ATMQTT_SET_OPTION_user:
        case ATMQTT_SET_OPTION_password:
        {
            ret = Calypso_appendArgumentString(pAtCommand, pValues, STRING_TERMINATE);
            break;
        }

        case ATMQTT_SET_OPTION_will:
        {
            ATMQTT_setWillParams_t *pWillValues = pValues;

            ret = Calypso_appendArgumentString(pAtCommand, pWillValues->topic, ARGUMENT_DELIM);

            if (ret)
            {
                ret = Calypso_appendArgumentString(pAtCommand, ATMQTT_QoSStrings[pWillValues->QoS], ARGUMENT_DELIM);
            }

            if (ret)
            {
                ret = Calypso_appendArgumentInt(pAtCommand, pWillValues->retain, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
            }

            if (ret)
            {
                ret = Calypso_appendArgumentInt(pAtCommand, pWillValues->messageLength, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
            }

            if (ret)
            {
                ret = Calypso_appendArgumentString(pAtCommand, pWillValues->message, STRING_TERMINATE);
            }

            break;
        }

        case ATMQTT_SET_OPTION_keepAlive:
        {
            uint16_t *pKeepAliveValue = pValues;
            ret = Calypso_appendArgumentInt(pAtCommand, *pKeepAliveValue, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), STRING_TERMINATE);
            break;
        }

        case ATMQTT_SET_OPTION_clean:
        {
            uint8_t *pCleanValue = pValues;
            ret = Calypso_appendArgumentInt(pAtCommand, *pCleanValue, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), STRING_TERMINATE);

            break;
        }

        default:
        {
            ret = false;
        }
        }
    }

    return ret;
}

/**
* @brief Adds the arguments to the request command string
*
* @param pAtCommand the request command string to add the arguments to
* @param fileName: file to open
* @param options: option flags for the file.
* @param fileSize: maximum size of the file. Will be allocate on create
*
* @RetVal true if arguments were added successful
* false otherwise
*/
bool ATMQTT_addArgumentsPublish(char *pAtCommand, uint8_t index, char *topicString, ATMQTT_QoS_t QoS, uint8_t retain, uint16_t messageLength, char *pMessage)
{
    bool ret = false;

    ret = Calypso_appendArgumentInt(pAtCommand, index, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, topicString, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, ATMQTT_QoSStrings[QoS], ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, retain, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, messageLength, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, pMessage, STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, CRLF, STRING_TERMINATE);
    }

    return ret;
}

/**
 *Adds the arguments to the request command string
 *
 *input:
 * -pAtCommand  the request command string to add the arguments to
 * -index           index of mqttclient to to subscribe.
 * -numOfTopics     number of topics to which subscribe to
 * -pTopics         Topics to subscribe to. See ATMQTT_subscribeTopic_t
*/
bool ATMQTT_addArgumentsSubscribe(char *pAtCommand, uint8_t index, uint8_t numOfTopics, ATMQTT_subscribeTopic_t *pTopics)
{
    bool ret = false;

    ret = Calypso_appendArgumentInt(pAtCommand, index, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, numOfTopics, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
    }

    for (int i = 0; i < numOfTopics; i++)
    {
        if (ret)
        {
            Calypso_appendArgumentString(pAtCommand, pTopics[i].topicString, ARGUMENT_DELIM);
        }

        if (ret)
        {
            Calypso_appendArgumentString(pAtCommand, ATMQTT_QoSStrings[pTopics[i].QoS], ARGUMENT_DELIM);
        }

        if (ret)
        {
            Calypso_appendArgumentString(pAtCommand, STRING_EMPTY, ARGUMENT_DELIM);
        }
    }

    /* add empty, unused topics*/
    for (int i = numOfTopics; i < 5; i++)
    {
        if (ret)
        {
            Calypso_appendArgumentString(pAtCommand, ",,", ARGUMENT_DELIM);
        }
    }

    pAtCommand[strlen(pAtCommand)] = STRING_TERMINATE;

    if (ret)
    {
        Calypso_appendArgumentString(pAtCommand, CRLF, STRING_TERMINATE);
    }

    return ret;
}

/**
* @brief Adds the arguments to the request command string
*
* @param fileID file id of opened file. Is returned by ATFile_open
* @param certFileName: full path to certificate (optional). Can be NULL if not used.
* @param signature: SHA1 signature (optional). Can be NULL if not used.
*
* @RetVal true if arguments were added successful
* false otherwise
*/
bool ATFile_AddArgumentsFileOpen(char *pAtCommand, const char *fileName, uint32_t options, uint16_t fileSize)
{
    bool ret = false;

    if ((NULL == pAtCommand) || (NULL == fileName))
    {
        return false;
    }

    if (strlen(fileName) <= FILENAME_MAX_LENGTH)
    {
        ret = Calypso_appendArgumentString(pAtCommand, fileName, ARGUMENT_DELIM);
    }

    if (ret)
    {
        char optionString[128];
        memset(optionString, 0, 128);

        uint32_t bitmask;
        for (int i = 0; i < ATFILE_OPEN_NUM_OF_BITS; i++)
        {
            bitmask = (1 << i);
            if (0 != (bitmask & options))
            {
                Calypso_appendArgumentString(optionString, ATFile_OpenOptions_Strings[i], BITMASK_DELIM);
            }
        }
        /* delete last '|' */
        optionString[strlen(optionString) - 1] = '\0';
        ret = Calypso_appendArgumentString(pAtCommand, optionString, ARGUMENT_DELIM);
    }

    if (FILE_MIN_SIZE <= fileSize)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, fileSize, (INTFLAGS_UNSIGNED | INTFLAGS_NOTATION_DEC), STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, CRLF, STRING_TERMINATE);
    }

    return ret;
}

/**
 *Adds the arguments to the request command string
 *
 *input:
 * -fileID:       file id of opened file. Is returned by ATFile_open
 * -certFileName: full path to certificate (optional). Can be NULL if not used.
 * -signature:    SHA1 signature (optional). Can be NULL if not used.
 *
 *return true if arguments were added successful
 *       false otherwise
 */
bool ATFile_AddArgumentsFileClose(char *pAtCommand, uint32_t fileID, const char *certName, const char *signature)
{
    bool ret = false;

    if (NULL == pAtCommand)
    {
        return false;
    }

    ret = Calypso_appendArgumentInt(pAtCommand, fileID, (INTFLAGS_UNSIGNED | INTFLAGS_NOTATION_DEC), ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, certName, ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, signature, STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, CRLF, STRING_TERMINATE);
    }

    return ret;
}

/**
* @brief Adds the arguments to the request command string
*
* @param fileName name of the file to delete
* @param secureToken: secure token returned by ATFile_open() (optional)
*
* @RetVal true if arguments were added successful
* false otherwise
*/
bool ATFile_AddArgumentsFileDel(char *pAtCommand, const char *fileName, uint32_t secureToken)
{
    bool ret = false;

    if (NULL == pAtCommand)
    {
        return false;
    }

    ret = Calypso_appendArgumentString(pAtCommand, fileName, ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, secureToken, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, CRLF, STRING_TERMINATE);
    }

    return ret;
}

/**
* @brief Adds the arguments to the request command string
*
* @param fileID: ID of file to read. Is returned by ATFile_open
* @param offset: offset of the read operation
* @param format: format of the output data.
* @param bytesToRead: number of bytes to read
*
* @RetVal true if arguments were added successful
* false otherwise
*/
bool ATFile_AddArgumentsFileRead(char *pAtCommand, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, uint16_t bytesToRead)
{
    bool ret = false;

    if (NULL == pAtCommand)
    {
        return false;
    }

    ret = Calypso_appendArgumentInt(pAtCommand, fileID, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, offset, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, format, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, bytesToRead, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), STRING_TERMINATE);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, CRLF, STRING_TERMINATE);
    }

    return ret;
}

/**
* @brief Adds the arguments to the request command string
*
* @param pAtCommand Pointer to At command to add arguments to
* @param fileID ID of file to write. Is returned by ATFile_open
* @param offset offset of the write operation
* @param format format of the input data.
* @param bytestoWrite number of bytes to write
* @param encodeToBase64 boolean use base64 encoding
* @param pData Data to be written
*
* @RetVal true if arguments were added successful
* false otherwise
*/
bool ATFile_AddArgumentsFileWrite(char *pAtCommand, uint32_t fileID, uint16_t offset, Calypso_DataFormat_t format, bool encodeToBase64, uint16_t bytesToWrite, char *pData)
{
    bool ret = false;

    if (NULL == pAtCommand)
    {
        return false;
    }

    ret = Calypso_appendArgumentInt(pAtCommand, fileID, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, offset, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
    }

    if (ret)
    {
        ret = Calypso_appendArgumentInt(pAtCommand, format, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
    }

    if (encodeToBase64)
    {

        uint32_t lengthEncoded;
        lengthEncoded = Calypso_getBase64EncBufSize(bytesToWrite);

        char base64Buffer[lengthEncoded];
        Calypso_encodeBase64((uint8_t *)pData, bytesToWrite, (uint8_t *)base64Buffer, &lengthEncoded);

        if (ret)
        {
            ret = Calypso_appendArgumentInt(pAtCommand, lengthEncoded, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
        }

        if (ret)
        {
            ret = Calypso_appendArgumentString(pAtCommand, base64Buffer, STRING_TERMINATE);
        }
    }
    else
    {
        if (ret)
        {
            ret = Calypso_appendArgumentInt(pAtCommand, bytesToWrite, (INTFLAGS_NOTATION_DEC | INTFLAGS_UNSIGNED), ARGUMENT_DELIM);
        }

        if (ret)
        {
            ret = Calypso_appendArgumentString(pAtCommand, pData, STRING_TERMINATE);
        }
    }
    if (ret)
    {
        ret = Calypso_appendArgumentString(pAtCommand, CRLF, STRING_TERMINATE);
    }

    return ret;
}

/**
* @brief Parses the response of a AT+fileopen command
*
* @param pAtCommand Pointer to the string response by the module
* @param fileID ID of the opened file. Can be used for further file functions
* @param secureToken secure token of the opened file.
*
* @RetVal true if arguments were added successful
* false otherwise
*/
bool ATFile_ParseResponseFileOpen(char **pAtCommand, uint32_t *fileID, uint32_t *secureToken)
{
    bool ret = false;
    const char *cmd = "+fileopen:";
    const size_t cmdLength = strlen(cmd);

    ret = (0 == strncmp(*pAtCommand, cmd, cmdLength));
    if (ret)
    {
        *pAtCommand += cmdLength;
        ret = Calypso_getNextArgumentInt(pAtCommand, fileID, INTFLAGS_SIZE32, ARGUMENT_DELIM);

        if (ret)
        {
            ret = Calypso_getNextArgumentInt(pAtCommand, secureToken, INTFLAGS_SIZE32, STRING_TERMINATE);
        }
    }

    return ret;
}

/**
* @brief Parses the response of a AT+fileread command
*
* @param pAtCommand Pointer to the string reponse by the module
* @param pOutFormat Pointer to the format of the output data
* @param byteRead number of bytes which had been read
* @param data Pointer to file content which had been read out
*
* @RetVal true if arguments were added successful
* false otherwise
*/
bool ATFile_ParseResponseFileRead(char **pAtCommand, Calypso_DataFormat_t *pOutFormat, uint16_t *bytesRead, char *data)
{
    bool ret = false;
    const char *cmd = "+fileread:";
    const size_t cmdLength = strlen(cmd);

    ret = (0 == strncmp(*pAtCommand, cmd, cmdLength));
    if (ret)
    {
        *pAtCommand += cmdLength;

        ret = Calypso_getNextArgumentInt(pAtCommand, pOutFormat, INTFLAGS_SIZE8, ARGUMENT_DELIM);

        if (ret)
        {
            ret = Calypso_getNextArgumentInt(pAtCommand, bytesRead, INTFLAGS_SIZE16, ARGUMENT_DELIM);
        }

        if (ret)
        {
            ret = Calypso_getNextArgumentString(pAtCommand, data, STRING_TERMINATE);
        }
    }

    return ret;
}

/**
* @brief Parses the response of a AT+filewrite command
*
* @param pAtCommand Pointer to the string reponse by the module
* @param writtenBytes number of bytes which has been written
*
* @RetVal true if arguments were added successful
* false otherwise
*/
bool ATFile_ParseResponseFileWrite(char **pAtCommand, uint16_t *bytesWritten)
{
    bool ret = false;
    const char *cmd = "+filewrite:";
    const size_t cmdLength = strlen(cmd);

    ret = (0 == strncmp(*pAtCommand, cmd, cmdLength));
    if (ret)
    {
        *pAtCommand += cmdLength;
        ret = Calypso_getNextArgumentInt(pAtCommand, bytesWritten, INTFLAGS_SIZE16, STRING_TERMINATE);
    }

    return ret;
}