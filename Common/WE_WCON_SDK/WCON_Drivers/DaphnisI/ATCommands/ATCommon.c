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
 * @brief Common functions for DaphnisI source file.
 */
#include <DaphnisI/ATCommands/ATCommon.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

bool DaphnisI_ByteArrayToKeyAddr(uint8_t* byteArray, uint16_t byteArrayLength, char* keyAddr)
{
    if (keyAddr == NULL)
    {
        return false;
    }

    uint16_t i;
    for (i = 0; i < byteArrayLength - 1; i++)
    {
        sprintf(&keyAddr[3 * i], "%02" PRIX32 ":", (uint32_t)byteArray[byteArrayLength - 1 - i]);
    }

    sprintf(&keyAddr[3 * i], "%02" PRIX32, (uint32_t)byteArray[byteArrayLength - 1 - i]);

    keyAddr[(3 * byteArrayLength) - 1] = '\0';

    return true;
}

bool DaphnisI_ByteArrayToHexString(uint8_t* byteArray, uint16_t byteArrayLength, char* hexString)
{
    if (hexString == NULL)
    {
        return false;
    }

    for (uint16_t i = 0; i < byteArrayLength; i++)
    {
        sprintf(&hexString[2 * i], "%02" PRIX32, (uint32_t)byteArray[i]);
    }

    hexString[2 * byteArrayLength] = '\0';

    return true;
}

bool DaphnisI_KeyAddrToByteArray(char* keyAddr, uint16_t keyAddrLength, uint8_t* byteArray)
{
    if (keyAddr == NULL)
    {
        return false;
    }

    if (keyAddr[keyAddrLength] != '\0')
    {
        return false;
    }

    for (uint16_t i = 0; i < keyAddrLength; i += 3)
    {
        uint32_t temp;
        sscanf(&keyAddr[i], "%02" SCNx32, &temp);
        byteArray[(keyAddrLength - 1 - i) / 3] = (uint8_t)(temp & 0xFF);
    }

    return true;
}

bool DaphnisI_HexStringToByteArray(char* hexString, uint16_t hexStringLength, uint8_t* byteArray)
{
    if (hexString == NULL)
    {
        return false;
    }

    if (hexString[hexStringLength] != '\0')
    {
        return false;
    }

    for (uint16_t i = 0; i < hexStringLength; i += 2)
    {
        uint32_t temp;
        sscanf(&hexString[i], "%02" SCNx32, &temp);
        byteArray[i / 2] = (uint8_t)(temp & 0xFF);
    }

    return true;
}
