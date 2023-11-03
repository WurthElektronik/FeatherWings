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
 * @brief ATCommand driver general AT command definitions.
 */

#include <ATCommands.h>
#include <stdio.h>
#include <stdlib.h>

char AT_commandBuffer[AT_MAX_COMMAND_BUFFER_SIZE];

static const char *ATCommand_BooleanValueStrings[ATCommand_BooleanValue_NumberOfValues] =
    {
        "false",
        "true"};

/**
 * @brief Converts an integer to string.
 *
 * @param[out] outString Number converted to string
 * @param[in] number Integer value to convert to string
 * @param[in] intFlags Formatting flags
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_IntToString(char *outString, uint32_t number, uint16_t intFlags)
{
    if ((0 == (intFlags & ATCOMMAND_INTFLAGS_SIGN)) || (0 == (intFlags & ATCOMMAND_INTFLAGS_NOTATION)))
    {
        return false;
    }

    if (ATCOMMAND_INTFLAGS_NOTATION_HEX == (intFlags & ATCOMMAND_INTFLAGS_NOTATION))
    {
        /* HEX */
        sprintf(outString, "0x%lx", number);
    }
    else
    {
        /* DEC */

        if (ATCOMMAND_INTFLAGS_UNSIGNED == (intFlags & ATCOMMAND_INTFLAGS_SIGN))
        {
            /* UNSIGNED */
            sprintf(outString, "%lu", number);
        }
        else
        {
            /* SIGNED */
            sprintf(outString, "%ld", *(int32_t *)&number);
        }
    }

    return true;
}

/**
 * @brief Parses string to integer
 *
 * @param[out] number Parsed integer value
 * @param[in] inString String to be parsed
 * @param[in] intFlags Flags to determine how to parse
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_StringToInt(void *number, const char *inString, uint16_t intFlags)
{
    if ((NULL == inString) || (NULL == number))
    {
        return false;
    }

    bool hex = (0 == strncmp(inString, "0x", 2)) || ((intFlags & ATCOMMAND_INTFLAGS_NOTATION_HEX) != 0);
    bool signedDataType = (intFlags & ATCOMMAND_INTFLAGS_SIGNED) != 0;

    if (signedDataType)
    {
        char *endptr;

        long long longNr = strtoll(inString, &endptr, (hex == true) ? 16 : 10);

        if (endptr == inString || *endptr != ATCOMMAND_STRING_TERMINATE)
        {
            /* endptr did not move to end, thus conversion failed */
            return false;
        }

        if ((intFlags & ATCOMMAND_INTFLAGS_SIZE8) != 0)
        {
            *((int8_t *)number) = longNr;
        }
        else if ((intFlags & ATCOMMAND_INTFLAGS_SIZE16) != 0)
        {
            *((int16_t *)number) = longNr;
        }
        else if ((intFlags & ATCOMMAND_INTFLAGS_SIZE32) != 0)
        {
            *((int32_t *)number) = longNr;
        }
        else if ((intFlags & ATCOMMAND_INTFLAGS_SIZE64) != 0)
        {
            *((int64_t *)number) = longNr;
        }
    }
    else
    {
        char *endptr;

        unsigned long long longNr = strtoull(inString, &endptr, (hex == true) ? 16 : 10);

        if (endptr == inString || *endptr != ATCOMMAND_STRING_TERMINATE)
        {
            /* endptr did not move to end, thus conversion failed */
            return false;
        }

        if ((intFlags & ATCOMMAND_INTFLAGS_SIZE8) != 0)
        {
            *((uint8_t *)number) = longNr;
        }
        else if ((intFlags & ATCOMMAND_INTFLAGS_SIZE16) != 0)
        {
            *((uint16_t *)number) = longNr;
        }
        else if ((intFlags & ATCOMMAND_INTFLAGS_SIZE32) != 0)
        {
            *((uint32_t *)number) = longNr;
        }
        else if ((intFlags & ATCOMMAND_INTFLAGS_SIZE64) != 0)
        {
            *((uint64_t *)number) = longNr;
        }
    }

    return true;
}

/**
 * @brief Appends a byte array argument to the end of an AT command.
 *
 * @param[out] pOutString  AT command after appending argument
 * @param[in] pInArgument Pointer to byte array to be added
 * @param[in] numBytes    Number of bytes to add
 * @param[in] delimiter   Delimiter to append after argument
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_AppendArgumentBytes(char *pOutString,
                                   const char *pInArgument,
                                   uint16_t numBytes,
                                   char delimiter)
{
    if (NULL == pOutString)
    {
        return false;
    }

    size_t strLength = strlen(pOutString);
    if (NULL != pInArgument)
    {
        memcpy(&pOutString[strLength], pInArgument, numBytes);
        strLength += numBytes;
    }

    pOutString[strLength] = delimiter;

    return true;
}

/**
 * @brief Appends a string argument to the end of an AT command.
 *
 * The supplied delimiter is appended after the argument. A null byte ('\0')
 * is appended after the delimiter, if the delimiter itself is not a null byte.
 *
 * @param[out] pOutString AT command after appending argument
 * @param[in] pInArgument Argument to be added
 * @param[in] delimiter Delimiter to append after argument
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_AppendArgumentString(char *pOutString, const char *pInArgument, char delimiter)
{
    if (NULL == pOutString)
    {
        return false;
    }

    size_t outStrLength = strlen(pOutString);
    if (NULL != pInArgument)
    {
        size_t inStrLength = strlen(pInArgument);
        strcpy(&pOutString[outStrLength], pInArgument);
        outStrLength += inStrLength;
    }
    pOutString[outStrLength] = delimiter;
    if (delimiter != '\0')
    {
        pOutString[outStrLength + 1] = '\0';
    }

    return true;
}

/**
 * @brief Appends a string argument to the end of an AT command, which is embedded into quotation marks
 *
 * The supplied delimiter is appended after the argument. A null byte ('\0')
 * is appended after the delimiter, if the delimiter itself is not a null byte.
 * The string will be embedded into quotation marks
 *
 * @param[out] pOutString AT command after appending argument
 * @param[in] pInArgument Argument to be added
 * @param[in] delimiter Delimiter to append after argument
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_AppendArgumentStringQuotationMarks(char *pOutString, const char *pInArgument, char delimiter)
{
    if (NULL == pOutString)
    {
        return false;
    }

    size_t outStrLength = strlen(pOutString);
    if (NULL != pInArgument)
    {
        pOutString[outStrLength++] = '\"';
        size_t inStrLength = strlen(pInArgument);
        strcpy(&pOutString[outStrLength], pInArgument);
        outStrLength += inStrLength;
        pOutString[outStrLength++] = '\"';
    }
    pOutString[outStrLength] = delimiter;
    if (delimiter != '\0')
    {
        pOutString[outStrLength + 1] = '\0';
    }

    return true;
}

/**
 * @brief Appends bitmask strings to the supplied string.
 *
 * @param[out] pOutString AT command after appending argument
 * @param[in] stringList List of strings containing the string representations of the bits in the input bitmask
 * @param[in] numStrings Number of elements in stringList (max. number of bits in input bitmask)
 * @param[in] bitmask Input bitmask
 * @param[in] delimiter Delimiter to append after argument
 * @param[in] maxStringLength Max. length of output string
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_AppendArgumentBitmask(char *pOutString,
                                     const char *stringList[],
                                     uint8_t numStrings,
                                     uint32_t bitmask,
                                     char delimiter,
                                     uint16_t maxStringLength)
{
    size_t outStrLength = strlen(pOutString) + 1;
    bool empty = true;
    for (int i = 0; i < numStrings; i++)
    {
        if (0 != (bitmask & (1 << i)))
        {
            outStrLength += strlen(stringList[i]) + 1;
            if (outStrLength > maxStringLength)
            {
                return false;
            }
            if (!ATCommand_AppendArgumentString(pOutString, stringList[i], ATCOMMAND_BITMASK_DELIM))
            {
                return false;
            }
            empty = false;
        }
    }
    pOutString[outStrLength - (empty ? 1 : 2)] = delimiter;
    pOutString[outStrLength - (empty ? 0 : 1)] = '\0';
    return true;
}

/**
 * @brief Appends a boolean string (true, false) to the supplied string.
 *
 * @param[out] pOutString AT command after appending argument
 * @param[in] inBool Value to append
 * @param[in] delimiter Delimiter to append after argument
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_AppendArgumentBoolean(char *pOutString, bool inBool, char delimiter)
{
    return ATCommand_AppendArgumentString(pOutString, ATCommand_BooleanValueStrings[(inBool == true) ? 1 : 0], delimiter);
}

/**
 * @brief Gets the next string argument from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Argument as string
 * @param[in] delimiter Delimiter which occurs after argument to get
 * @param[in] maxLength Max. length of string to get (including termination character).
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentString(char **pInArguments,
                                     char *pOutArgument,
                                     char delimiter,
                                     uint16_t maxLength)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }

    size_t argumentLength = 0;
    size_t inputStringLength = strlen(*pInArguments);
    size_t quotationCharCount = 0;

    while (true)
    {
        if (argumentLength > maxLength - 1)
        {
            return false;
        }
        else if (((*pInArguments)[argumentLength] == delimiter) && (quotationCharCount % 2 == 0))
        {
            /* If argument list is not empty, copy string to output arguments */
            memcpy(pOutArgument, *pInArguments, argumentLength);

            pOutArgument[argumentLength] = '\0';

            if (argumentLength > 0 || **pInArguments != '\0')
            {
                *pInArguments = &((*pInArguments)[argumentLength + 1]);
            }

            return true;
        }
        else if (((*pInArguments)[argumentLength] == '"'))
        {
            quotationCharCount++;
        }
        else if (argumentLength > inputStringLength)
        {
            return false;
        }
        argumentLength++;
    }
}

/**
 * @brief Gets the next string argument from the supplied AT command and removes the quotation marks
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Argument as string
 * @param[in] delimiter Delimiter which occurs after argument to get
 * @param[in] maxLength Max. length of string to get (including termination character).
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentStringWithoutQuotationMarks(char **pInArguments,
                                                          char *pOutArgument,
                                                          char delimiter,
                                                          uint16_t maxLength)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }

    size_t argumentLength = 0;
    size_t inputStringLength = strlen(*pInArguments);
    size_t quotationCharCount = 0;

    while (true)
    {
        if (argumentLength > maxLength + 1)
        {
            return false;
        }
        else if (((*pInArguments)[argumentLength] == delimiter) && (quotationCharCount % 2 == 0))
        {
            if (((*pInArguments)[0] == '"') &&
                ((*pInArguments)[argumentLength - 1] == '"') &&
                (argumentLength >= 2))
            {
                memcpy(pOutArgument, *pInArguments + 1, argumentLength - 2);
                pOutArgument[argumentLength - 2] = '\0';

                if (argumentLength > 0 || **pInArguments != '\0')
                {
                    *pInArguments = &((*pInArguments)[argumentLength + 1]);
                }

                return true;
            }
            else
            {
                return false;
            }
        }
        else if (((*pInArguments)[argumentLength] == '"'))
        {
            quotationCharCount++;
        }
        else if (argumentLength > inputStringLength)
        {
            return false;
        }
        argumentLength++;
    }
}

/**
 * @brief Appends an integer argument to the end of an AT command
 *
 * @param[out] pOutString AT command after appending argument
 * @param[in] pInValue Argument to be added
 * @param[in] intFlags Integer formatting flags
 * @param[in] delimiter Delimiter to append after argument
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_AppendArgumentInt(char *pOutString, uint32_t pInValue, uint16_t intFlags, char delimiter)
{
    if (NULL == pOutString)
    {
        return false;
    }

    char tempString[12];

    if (ATCommand_IntToString(tempString, pInValue, intFlags))
    {
        return ATCommand_AppendArgumentString(pOutString, tempString, delimiter);
    }

    return false;
}

/**
 * @brief Appends an integer argument to the end of an AT command, which is embedded into quotation marks
 *
 * @param[out] pOutString AT command after appending argument
 * @param[in] pInValue Argument to be added
 * @param[in] intFlags Integer formatting flags
 * @param[in] delimiter Delimiter to append after argument
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_AppendArgumentIntQuotationMarks(char *pOutString, uint32_t pInValue, uint16_t intFlags, char delimiter)
{
    if (NULL == pOutString)
    {
        return false;
    }

    char tempString[12];

    if (ATCommand_IntToString(tempString, pInValue, intFlags))
    {
        return ATCommand_AppendArgumentStringQuotationMarks(pOutString, tempString, delimiter);
    }

    return false;
}

/**
 * @brief Gets the next enumeration argument from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Enumeration value corresponding to the string read from the input AT command
 * @param[in] stringList List of strings containing the string representations of the enumeration's values
 * @param[in] numStrings Number of elements in stringList (number of elements in the enumeration)
 * @param[in] maxStringLength Max. length of individual enumeration value strings
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentEnum(char **pInArguments,
                                   uint8_t *pOutArgument,
                                   const char *stringList[],
                                   uint8_t numStrings,
                                   uint16_t maxStringLength,
                                   char delimiter)
{
    char tempString[maxStringLength];

    if (!ATCommand_GetNextArgumentString(pInArguments,
                                         tempString,
                                         delimiter,
                                         sizeof(tempString)))
    {
        return false;
    }

    bool ok;
    *pOutArgument = ATCommand_FindString(stringList,
                                         numStrings,
                                         tempString,
                                         0,
                                         &ok);
    return ok;
}

/**
 * @brief Gets the next string argument from the supplied AT command.
 *
 * @param[in,out]  pInArguments AT command to get argument from
 * @param[in]  length Length of the argument to copy
 * @param[out] pOutArgument Argument as byte array
 * @param[in] maxLength Max. length of byte array
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentByteArray(char **pInArguments, uint16_t length, uint8_t *pOutArgument, uint16_t maxLength)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }
    if (length > maxLength)
    {
        /* target array to small */
        return false;
    }

    memcpy(pOutArgument, *pInArguments, length);
    *pInArguments += length;
    return true;
}

/**
 * @brief Gets the next bitmask argument from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[in] stringList List of strings containing the string representations of the bits in the output bitmask
 * @param[in] numStrings Number of elements in stringList (max. number of bits in output bitmask)
 * @param[in] maxStringLength Max. length of individual bitmask strings
 * @param[out] bitmask Output bitmask
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentBitmask(char **pInArguments,
                                      const char *stringList[],
                                      uint8_t numStrings,
                                      uint16_t maxStringLength,
                                      uint32_t *bitmask,
                                      char delimiter)
{

    bool ret = false;
    char tempString[maxStringLength];

    *bitmask = 0;

    while (**pInArguments != '\0' &&
           ((ret = ATCommand_GetNextArgumentString(pInArguments, tempString, ATCOMMAND_BITMASK_DELIM, sizeof(tempString))) ||
            (ret = ATCommand_GetNextArgumentString(pInArguments, tempString, delimiter, sizeof(tempString)))))
    {
        bool ok;
        uint8_t flag = ATCommand_FindString(stringList, numStrings, tempString, 0, &ok);
        if (ok)
        {
            *bitmask |= (1 << flag);
        }
    }
    return true;
}

/**
 * @brief Gets the next boolean argument (true, false) from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] outBool Output boolean
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentBoolean(char **pInArguments, bool *outBool, char delimiter)
{
    uint8_t enumValue;
    bool ok = ATCommand_GetNextArgumentEnum(pInArguments,
                                            &enumValue,
                                            ATCommand_BooleanValueStrings,
                                            ATCommand_BooleanValue_NumberOfValues,
                                            6,
                                            delimiter);
    if (ok)
    {
        *outBool = (enumValue == 1) ? true : false;
    }
    return ok;
}

/**
 * @brief Gets the next integer argument from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Argument parsed as integer
 * @param[in] intFlags Flags to determine how to parse
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentInt(char **pInArguments,
                                  void *pOutArgument,
                                  uint16_t intFlags,
                                  char delimiter)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }

    char tempString[40];

    char *tempstartlocation = *pInArguments;

    ATCommand_GetNextArgumentString(pInArguments, tempString, delimiter, sizeof(tempString));

    if (!ATCommand_StringToInt(pOutArgument, tempString, intFlags))
    {
        *pInArguments = tempstartlocation;
        return false;
    }

    return true;
}

/**
 * @brief Gets the next integer argument from the supplied AT command and removes the quotation marks
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Argument parsed as integer
 * @param[in] intFlags Flags to determine how to parse
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentIntWithoutQuotationMarks(char **pInArguments,
                                                       void *pOutArgument,
                                                       uint16_t intFlags,
                                                       char delimiter)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }

    char tempString[40];

    char *tempstartlocation = *pInArguments;

    ATCommand_GetNextArgumentStringWithoutQuotationMarks(pInArguments, tempString, delimiter, sizeof(tempString));

    if (!ATCommand_StringToInt(pOutArgument, tempString, intFlags))
    {
        *pInArguments = tempstartlocation;
        return false;
    }

    return true;
}

/**
 * @param Gets the command name from an AT command.
 *
 * @param[in,out] pInAtCmd AT command to get command name from
 * @param[in] pCmdName Command name as string
 * @param[in] CmdNameLen Length of the provided butter
 * @param[in] delimiters Delimiters which occurs after command name
 * @param[in] number_of_delimiters Number of delimiters
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetCmdName(char **pInAtCmd,
                          char *pCmdName,
                          size_t CmdNameLen,
                          char *delimiters,
                          uint8_t number_of_delimiters)
{
    if ((NULL == pInAtCmd) || (NULL == pCmdName))
    {
        return false;
    }

    size_t argumentLength = 0;

    while (true)
    {
        if (argumentLength >= CmdNameLen)
        {
            return false;
        }
        for (uint8_t i = 0; i < number_of_delimiters; i++)
        {
            if ((*pInAtCmd)[argumentLength] == delimiters[i])
            {
                /* if argument list not empty*/
                /* copy string to output arguments */
                memcpy(pCmdName, *pInAtCmd, argumentLength);

                pCmdName[argumentLength] = '\0';

                if ((*pInAtCmd)[argumentLength] == ATCOMMAND_STRING_TERMINATE)
                {
                    /* string end, nothing follows */
                    *pInAtCmd = &((*pInAtCmd)[argumentLength]);
                }
                else
                {
                    /* more arguments follow */
                    *pInAtCmd = &((*pInAtCmd)[argumentLength + 1]);
                }
                return true;
            }
        }
        if ((*pInAtCmd)[argumentLength] == '\0')
        {
            return false;
        }
        argumentLength++;
    }
}

/**
 * @brief Looks up a string in a list of strings (case insensitive) and returns the
 * index of the string or the supplied default value, if the string is not found.
 *
 * @param[in] stringList List of strings to search in
 * @param[in] numStrings Number of strings in stringList
 * @param[in] str String to look for
 * @param[in] defaultValue Value to return if the string is not found
 * @param[out] ok Is set to true if the string is found. Optional.
 *
 * @return Index of the first occurrence of str in stringList or defaultValue, if string is not found
 */
uint8_t ATCommand_FindString(const char *stringList[],
                             uint8_t numStrings,
                             const char *str,
                             uint8_t defaultValue,
                             bool *ok)
{
    for (uint8_t i = 0; i < numStrings; i++)
    {
        if (0 == strcasecmp(stringList[i], str))
        {
            if (ok)
            {
                *ok = true;
            }
            return i;
        }
    }

    if (ok)
    {
        *ok = false;
    }
    return defaultValue;
}

/**
 * @brief Appends a bits argument to the end of an AT command, which is embedded into quotation marks
 *
 * The supplied delimiter is appended after the argument. A null byte ('\0')
 * is appended after the delimiter, if the delimiter itself is not a null byte.
 * The string will be embedded into quotation marks
 *
 * @param[out] pOutString AT command after appending argument
 * @param[in] pInArgument Argument to be added
 * @param[in] intFlags Integer formatting flags
 * @param[in] delimiter Delimiter to append after argument
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_AppendArgumentBitsQuotationMarks(char *pOutString,
                                                uint32_t pInValue,
                                                uint16_t intFlags,
                                                char delimiter)
{
    if (NULL == pOutString)
    {
        return false;
    }

    char tempString[12];

    if ((intFlags & ATCOMMAND_INTFLAGS_SIZE8) != 0)
    {
        for (int i = 7; i >= 0; i--)
        {
            tempString[i] = pInValue & (1 << (7 - i)) ? '1' : '0';
        }

        tempString[8] = ATCOMMAND_STRING_TERMINATE;
    }
    else
    {
        return false;
    }

    return ATCommand_AppendArgumentStringQuotationMarks(pOutString, tempString, delimiter);
}

/**
 * @brief Gets the next bits argument from the supplied AT command and removes the quotation marks
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Argument parsed as integer
 * @param[in] intFlags Flags to determine how to parse
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentBitsWithoutQuotationMarks(char **pInArguments,
                                                        void *pOutArgument,
                                                        uint16_t intFlags,
                                                        char delimiter)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }

    char tempString[40];

    ATCommand_GetNextArgumentStringWithoutQuotationMarks(pInArguments, tempString, delimiter, sizeof(tempString));

    long longNr = 0;

    if ((intFlags & ATCOMMAND_INTFLAGS_SIZE8) != 0)
    {
        for (int i = 7; i >= 0; i--)
        {
            longNr |= ((tempString[i] - '0') << (7 - i));
        }
        *((uint8_t *)pOutArgument) = longNr;
    }
    else
    {
        return false;
    }

    return true;
}

/**
 * @brief Gets the next enumeration argument from the supplied AT command and removes the quotation marks.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Enumeration value corresponding to the string read from the input AT command
 * @param[in] stringList List of strings containing the string representations of the enumeration's values
 * @param[in] numStrings Number of elements in stringList (number of elements in the enumeration)
 * @param[in] maxStringLength Max. length of individual enumeration value strings
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentEnumWithoutQuotationMarks(char **pInArguments,
                                                        uint8_t *pOutArgument,
                                                        const char *stringList[],
                                                        uint8_t numStrings,
                                                        uint16_t maxStringLength,
                                                        char delimiter)
{
    char tempString[maxStringLength];

    if (!ATCommand_GetNextArgumentStringWithoutQuotationMarks(pInArguments,
                                                              tempString,
                                                              delimiter,
                                                              sizeof(tempString)))
    {
        return false;
    }

    bool ok;
    *pOutArgument = ATCommand_FindString(stringList,
                                         numStrings,
                                         tempString,
                                         0,
                                         &ok);
    return ok;
}

/**
 * @brief Parses string to double
 *
 * @param[out] number Parsed double value
 * @param[in] inString String to be parsed
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_StringToDouble(void *number, const char *inString)
{
    if ((NULL == inString) || (NULL == number))
    {
        return false;
    }

    char *endptr;

    double doubleNr = strtod(inString, &endptr);

    if (endptr == inString || *endptr != ATCOMMAND_STRING_TERMINATE)
    {
        /* endptr did not move to end, thus conversion failed */
        return false;
    }

    *((double *)number) = doubleNr;

    return true;
}

/**
 * @brief Parses string to float
 *
 * @param[out] number Parsed float value
 * @param[in] inString String to be parsed
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_StringToFloat(void *number, const char *inString)
{
    if ((NULL == inString) || (NULL == number))
    {
        return false;
    }

    char *endptr;

    float floatNr = strtof(inString, &endptr);

    if (endptr == inString || *endptr != ATCOMMAND_STRING_TERMINATE)
    {
        /* endptr did not move to end, thus conversion failed */
        return false;
    }

    *((float *)number) = floatNr;

    return true;
}

/**
 * @brief Gets the next double argument from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Argument parsed as double
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentDouble(char **pInArguments,
                                     void *pOutArgument,
                                     char delimiter)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }

    char tempString[40];

    char *tempstartlocation = *pInArguments;

    ATCommand_GetNextArgumentString(pInArguments, tempString, delimiter, sizeof(tempString));

    if (!ATCommand_StringToDouble(pOutArgument, tempString))
    {
        *pInArguments = tempstartlocation;
        return false;
    }

    return true;
}

/**
 * @brief Gets the next float argument from the supplied AT command.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Argument parsed as float
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentFloat(char **pInArguments,
                                    void *pOutArgument,
                                    char delimiter)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }

    char tempString[40];

    char *tempstartlocation = *pInArguments;

    ATCommand_GetNextArgumentString(pInArguments, tempString, delimiter, sizeof(tempString));

    if (!ATCommand_StringToFloat(pOutArgument, tempString))
    {
        *pInArguments = tempstartlocation;
        return false;
    }

    return true;
}

/**
 * @brief Gets the next double argument from the supplied AT command and removes the quotation marks
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Argument parsed as double
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentDoubleWithoutQuotationMarks(char **pInArguments,
                                                          void *pOutArgument,
                                                          char delimiter)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }

    char tempString[40];

    char *tempstartlocation = *pInArguments;

    ATCommand_GetNextArgumentStringWithoutQuotationMarks(pInArguments, tempString, delimiter, sizeof(tempString));

    if (!ATCommand_StringToDouble(pOutArgument, tempString))
    {
        *pInArguments = tempstartlocation;
        return false;
    }

    return true;
}

/**
 * @brief Gets the next float argument from the supplied AT command and removes the quotation marks.
 *
 * @param[in,out] pInArguments AT command to get argument from
 * @param[out] pOutArgument Argument parsed as float
 * @param[in] delimiter Delimiter which occurs after argument to get
 *
 * @return true if successful, false otherwise
 */
bool ATCommand_GetNextArgumentFloatWithoutQuotationMarks(char **pInArguments,
                                                         void *pOutArgument,
                                                         char delimiter)
{
    if ((NULL == pInArguments) || (NULL == pOutArgument))
    {
        return false;
    }

    char tempString[40];

    char *tempstartlocation = *pInArguments;

    ATCommand_GetNextArgumentStringWithoutQuotationMarks(pInArguments, tempString, delimiter, sizeof(tempString));

    if (!ATCommand_StringToFloat(pOutArgument, tempString))
    {
        *pInArguments = tempstartlocation;
        return false;
    }

    return true;
}

/**
 * @brief Count the number of arguments in argument string.
 *
 * @param[in] Argument String to check
 *
 * @return number of arguments in argument string
 */
int ATCommand_CountArgs(char *stringP)
{
    if (stringP == NULL)
    {
        return -1;
    }

    int length = strlen(stringP);

    if (length == 0)
    {
        return 0;
    }

    int quotationmarks = 0;
    int argscount = 1;

    for (int i = 0; i < length; i++)
    {
        if (stringP[i] == '"')
        {
            quotationmarks++;
        }
        else if (quotationmarks % 2 == 0 && stringP[i] == ATCOMMAND_ARGUMENT_DELIM)
        {
            argscount++;
        }
    }

    return argscount;
}
