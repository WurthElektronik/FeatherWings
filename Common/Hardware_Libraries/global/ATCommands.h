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

#ifndef AT_COMMMANDS_H_INCLUDED
#define AT_COMMMANDS_H_INCLUDED


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Size of buffer used for commands sent to the wireless module and the responses
 * received from the module.
 *
 * May be adopted to required file/data sizes.
 */
#define AT_MAX_COMMAND_BUFFER_SIZE 2048


#define ATCOMMAND_COMMAND_PREFIX  "AT+"                       /**< Prefix for AT commands */
#define ATCOMMAND_COMMAND_DELIM   (char)'='                   /**< Character delimiting AT command and parameters */
#define ATCOMMAND_CONFIRM_PREFIX  (char)'+'                   /**< Prefix for received confirmations */
#define ATCOMMAND_CONFIRM_DELIM   (char)':'                   /**< Character delimiting AT command and returned parameters in received confirmations */
#define ATCOMMAND_EVENT_PREFIX    (char)'+'                   /**< Prefix for received event notifications */
#define ATCOMMAND_EVENT_DELIM     (char)':'                   /**< Character delimiting event name and parameters in received notifications */
#define ATCOMMAND_ARGUMENT_DELIM  (char)','                   /**< Character delimiting parameters in AT commands */
#define ATCOMMAND_BITMASK_DELIM   (char)'|'                   /**< Character delimiting elements in bitmask parameters */
#define ATCOMMAND_CRLF            "\r\n"                      /**< Newline string indicating end of command (carriage return and line feed) */

#define ATCOMMAND_RESPONSE_OK     "OK"                        /**< String sent by module if AT command was successful */
#define ATCOMMAND_RESPONSE_ERROR  "error"                     /**< String sent by module if AT command failed */

#define ATCOMMAND_STRING_TERMINATE '\0'                       /**< End of string character */
#define ATCOMMAND_STRING_EMPTY     ""                         /**< Empty string */


#define ATCOMMAND_INTFLAGS_SIZE   (uint16_t)(0x0F)               /**< Mask for integer conversion flags concerning size */
#define ATCOMMAND_INTFLAGS_SIZE8  (uint16_t)(1)               /**< 8 bit integer conversion flag */
#define ATCOMMAND_INTFLAGS_SIZE16  (uint16_t)(2)               /**< 8 bit integer conversion flag */
#define ATCOMMAND_INTFLAGS_SIZE32 (uint16_t)(4)               /**< 16 bit integer conversion flag */
#define ATCOMMAND_INTFLAGS_SIZE64 (uint16_t)(8)               /**< 32 bit integer conversion flag */

#define ATCOMMAND_INTFLAGS_SIGN       (uint16_t)(0x30)        /**< Mask for integer conversion flags concerning sign */
#define ATCOMMAND_INTFLAGS_SIGNED     (uint16_t)(0x10)        /**< Signed integer conversion flag */
#define ATCOMMAND_INTFLAGS_UNSIGNED   (uint16_t)(0x20)        /**< Unsigned integer conversion flag */

#define ATCOMMAND_INTFLAGS_NOTATION        (uint16_t)(0xC0)   /**< Mask for integer conversion flags concerning notation */
#define ATCOMMAND_INTFLAGS_NOTATION_HEX    (uint16_t)(0x40)   /**< Hexadecimal notation */
#define ATCOMMAND_INTFLAGS_NOTATION_DEC    (uint16_t)(0x80)   /**< Decimal notation */

/**
 * @brief Boolean value (true, false).
 */
typedef enum ATCommand_BooleanValue_t
{
    ATCommand_BooleanValue_False = 0,
    ATCommand_BooleanValue_True = 1,
    ATCommand_BooleanValue_NumberOfValues
} ATCommand_BooleanValue_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Buffer used for commands sent to the wireless module and the responses
 * received from the module.
 */
extern char AT_commandBuffer[AT_MAX_COMMAND_BUFFER_SIZE];

extern bool ATCommand_IntToString(char *outString, uint32_t number, uint16_t intFlags);
extern bool ATCommand_StringToInt(void *pOutInt, const char *pInString, uint16_t intFlags);


extern bool ATCommand_AppendArgumentBytes(char *pOutString,
                                        const char *pInArgument,
                                        uint16_t numBytes,
                                        char delimiter);

extern bool ATCommand_AppendArgumentString(char *pOutString,
                                         const char *pInArgument,
                                         char delimiter);

extern bool ATCommand_AppendArgumentStringQuotationMarks(char *pOutString,
                                         const char *pInArgument,
                                         char delimiter);

extern bool ATCommand_AppendArgumentBitmask(char *pOutString,
                                          const char *stringList[],
                                          uint8_t numStrings,
                                          uint32_t bitmask,
                                          char delimiter,
                                          uint16_t maxStringLength);

extern bool ATCommand_AppendArgumentBoolean(char *pOutString,
                                          bool inBool,
                                          char delimiter);

extern bool ATCommand_AppendArgumentBitsQuotationMarks(char *pOutString,
										  uint32_t pInValue,
										  uint16_t intFlags,
                                          char delimiter);

extern bool ATCommand_GetNextArgumentString(char **pInArguments,
                                          char *pOutArgument,
                                          char delimiter,
                                          uint16_t maxLength);

extern bool ATCommand_GetNextArgumentStringWithoutQuotationMarks(char **pInArguments,
                                                  char *pOutArgument,
                                                  char delimiter,
                                                  uint16_t maxLength);

extern bool ATCommand_GetNextArgumentByteArray(char **pInArguments, 
											uint16_t length, 
											uint8_t *pOutArgument, 
											uint16_t maxLength);

extern bool ATCommand_AppendArgumentInt(char *pOutString,
                                      uint32_t pInValue,
                                      uint16_t intFlags,
                                      char delimiter);

extern bool ATCommand_AppendArgumentIntQuotationMarks(char *pOutString,
                                      uint32_t pInValue,
                                      uint16_t intFlags,
                                      char delimiter);

extern bool ATCommand_GetNextArgumentIntWithoutQuotationMarks(char **pInArguments,
                                       	   	   	   void *pOutArgument,
												   uint16_t intFlags,
												   char delimiter);

extern bool ATCommand_GetNextArgumentBitsWithoutQuotationMarks(char **pInArguments,
                                       	   	   	   void *pOutArgument,
												   uint16_t intFlags,
												   char delimiter);

extern bool ATCommand_GetNextArgumentInt(char **pInArguments,
                                       void *pOutArgument,
                                       uint16_t intFlags,
                                       char delimiter);


extern bool ATCommand_GetNextArgumentEnum(char **pInArguments,
                                        uint8_t *pOutArgument,
                                        const char *stringList[],
                                        uint8_t numStrings,
                                        uint16_t maxStringLength,
                                        char delimiter);
extern bool ATCommand_GetNextArgumentBitmask(char **pInArguments,
                                           const char *stringList[],
                                           uint8_t numStrings,
                                           uint16_t maxStringLength,
                                           uint32_t *bitmask,
                                           char delimiter);
extern bool ATCommand_GetNextArgumentBoolean(char **pInArguments,
                                    bool *outBool,
                                    char delimiter);

extern bool ATCommand_GetCmdName(char **pInAtCmd,
                               char *pCmdName,
							   size_t CmdNameLen,
                               char* delimiters,
							   uint8_t number_of_delimiters);

extern uint8_t ATCommand_FindString(const char *stringList[],
                                  uint8_t numStrings,
                                  const char *str,
                                  uint8_t defaultValue,
                                  bool *ok);

extern bool ATCommand_GetNextArgumentEnumWithoutQuotationMarks(char **pInArguments,
                                 uint8_t *pOutArgument,
                                 const char *stringList[],
                                 uint8_t numStrings,
                                 uint16_t maxStringLength,
                                 char delimiter);

extern bool ATCommand_StringToDouble(void *number, const char *inString);

extern bool ATCommand_StringToFloat(void *number, const char *inString);

extern bool ATCommand_GetNextArgumentDouble(char **pInArguments,
                                void *pOutArgument,
                                char delimiter);

extern bool ATCommand_GetNextArgumentFloat(char **pInArguments,
                                void *pOutArgument,
                                char delimiter);

extern bool ATCommand_GetNextArgumentDoubleWithoutQuotationMarks(char **pInArguments,
                                void *pOutArgument,
                                char delimiter);

extern bool ATCommand_GetNextArgumentFloatWithoutQuotationMarks(char **pInArguments,
                                void *pOutArgument,
                                char delimiter);
								
extern int ATCommand_CountArgs(char *stringP);

#ifdef __cplusplus
}
#endif

#endif /* AT_COMMMANDS_H_INCLUDED */
