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
 * @brief AT commands for HTTP functionality.
 */

#include <StephanoI/ATCommands/ATHTTP.h>
#include <StephanoI/StephanoI.h>
#include <global/ATCommands.h>

/**
 * @brief HTTP Client
 *
 * @param[in] opt Method
 * @param[in] content Content type
 * @param[in] url URL
 * @param[in] host Host
 * @param[in] path Path
 * @param[in] transport_via_SSL  SSL or TCP
 * @param[in] data  Data
 * @param[in] header  Header
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATHTTP_Client(StephanoI_ATHTTP_Opt_t opt, StephanoI_ATHTTP_Content_t content, char* url, char* host, char* path, bool transport_via_SSL, char* data, char* header)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+HTTPCLIENT=");

    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)opt, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, (uint32_t)content, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, url, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, host, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, path, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, transport_via_SSL ? (uint8_t)2 : (uint8_t)1, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), (opt == StephanoI_ATHTTP_Opt_Post) ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (opt == StephanoI_ATHTTP_Opt_Post)
    {
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, data, (header != NULL) ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
        if (header != NULL)
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, header, ATCOMMAND_STRING_TERMINATE))
            {
                return false;
            }
        }
    }

    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_HTTPGetPost), StephanoI_CNFStatus_Success);
}

/**
 * @brief HTTP Get size
 *
 * @param[in] url URL
 * @param[out] t Pointer to the size of the data

 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATHTTP_GetSize(char* url, StephanoI_ATHTTP_Size_t* t)
{
    char responsebuffer[16];
    char* pRequestCommand = AT_commandBuffer;
    strcpy(pRequestCommand, "AT+HTTPGETSIZE=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, url, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_HTTPGetPost), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATHTTP_ParseGetSize(responsebuffer, t);
}

/**
 * @brief HTTP Get
 *
 * @param[in] url URL
 * @param[out] t Pointer to the data and its length
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATHTTP_Get(char* url, StephanoI_ATHTTP_Get_t* t)
{
    char responsebuffer[7 + t->length];
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+HTTPCGET=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, url, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm_ex(StephanoI_GetTimeout(StephanoI_Timeout_HTTPGetPost), StephanoI_CNFStatus_Success, responsebuffer, sizeof(responsebuffer)))
    {
        return false;
    }

    return StephanoI_ATHTTP_ParseGet(responsebuffer, t);
}

/**
 * @brief HTTP post
 *
 * @param[in] url URL
 * @param[in] data Data
 * @param[in] length Length
 * @param[in] number_of_headers Number of header
 * @param[in] headers Headers
 *
 * @return true if successful, false otherwise
 */
bool StephanoI_ATHTTP_Post(char* url, uint8_t* data, uint32_t length, uint8_t number_of_headers, char** headers)
{
    char* pRequestCommand = AT_commandBuffer;

    strcpy(pRequestCommand, "AT+HTTPCPOST=");

    if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, url, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }
    if (!ATCommand_AppendArgumentInt(pRequestCommand, length, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), (number_of_headers > 0) ? ATCOMMAND_ARGUMENT_DELIM : ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }
    if (number_of_headers > 0)
    {
        if (!ATCommand_AppendArgumentInt(pRequestCommand, number_of_headers, (ATCOMMAND_INTFLAGS_NOTATION_DEC | ATCOMMAND_INTFLAGS_UNSIGNED), ATCOMMAND_ARGUMENT_DELIM))
        {
            return false;
        }
        for (uint8_t i = 0; i < number_of_headers - 1; i++)
        {
            if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, *(headers + i), ATCOMMAND_ARGUMENT_DELIM))
            {
                return false;
            }
        }
        if (!ATCommand_AppendArgumentStringQuotationMarks(pRequestCommand, *(headers + number_of_headers - 1), ATCOMMAND_STRING_TERMINATE))
        {
            return false;
        }
    }
    if (!ATCommand_AppendArgumentString(pRequestCommand, ATCOMMAND_CRLF, ATCOMMAND_STRING_TERMINATE))
    {
        return false;
    }

    if (!StephanoI_SendRequest(pRequestCommand))
    {
        return false;
    }

    if (!StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_HTTPGeneral), StephanoI_CNFStatus_Ready4Data))
    {
        return false;
    }

    if (!StephanoI_SendRequest_ex(data, length))
    {
        return false;
    }
    return StephanoI_WaitForConfirm(StephanoI_GetTimeout(StephanoI_Timeout_HTTPGetPost), StephanoI_CNFStatus_SendOK);
}

/**
 * @brief Parses the values of the HTTP client event arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATHTTP_ParseClient(char* EventArgumentsP, StephanoI_ATHTTP_Client_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->length), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    t->data = malloc(t->length);
    return ATCommand_GetNextArgumentByteArray(&argumentsP, t->length, t->data, t->length);
}

/**
 * @brief Parses the values of the HTTP get size event arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATHTTP_ParseGetSize(char* EventArgumentsP, StephanoI_ATHTTP_Size_t* t)
{
    char* argumentsP = EventArgumentsP;
    return ATCommand_GetNextArgumentInt(&argumentsP, t, ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_STRING_TERMINATE);
}

/**
 * @brief Parses the values of the HTTP get event arguments.
 *
 * @param[in] EventArgumentsP String containing arguments of the AT command
 * @param[out]    t               The parsed event data
 *
 * @return true if parsed successfully, false otherwise
 */
bool StephanoI_ATHTTP_ParseGet(char* EventArgumentsP, StephanoI_ATHTTP_Get_t* t)
{
    char* argumentsP = EventArgumentsP;
    if (!ATCommand_GetNextArgumentInt(&argumentsP, &(t->length), ATCOMMAND_INTFLAGS_SIZE16 | ATCOMMAND_INTFLAGS_UNSIGNED, ATCOMMAND_ARGUMENT_DELIM))
    {
        return false;
    }

    t->data = malloc(t->length);
    return ATCommand_GetNextArgumentByteArray(&argumentsP, t->length, t->data, t->length);
}
