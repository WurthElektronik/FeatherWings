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
 * @brief Device commands for DaphnisI header file.
 */

#ifndef DAPHNIS_COMMON_H_
#define DAPHNIS_COMMON_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define BYTEARRAY_TO_KEYADDR_LENGTH(length) ((length * 2) + (length - 1) + 1)
#define BYTEARRAY_TO_HEXSTRING_LENGTH(length) ((length * 2) + 1)

    extern bool DaphnisI_ByteArrayToKeyAddr(uint8_t* byteArray, uint16_t byteArrayLength, char* keyAddr);
    extern bool DaphnisI_ByteArrayToHexString(uint8_t* byteArray, uint16_t byteArrayLength, char* hexString);
    extern bool DaphnisI_KeyAddrToByteArray(char* keyAddr, uint16_t keyAddrLength, uint8_t* byteArray);
    extern bool DaphnisI_HexStringToByteArray(char* hexString, uint16_t hexStringLength, uint8_t* byteArray);

#ifdef __cplusplus
}
#endif

#endif /* DAPHNIS_COMMON_H_ */
