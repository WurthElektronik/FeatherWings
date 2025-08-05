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
 * @brief base64 encoder/decoder source file.
 */

#include "base64.h"

/**
 * @brief Base64 encoding table
 */
static const uint8_t base64EncTable[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

/**
 * @brief Base64 decoding table
 */
static const uint8_t base64DecTable[123] = {0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, /* 0-9 */
                                            0, 0, 0, 0, 0, 0, 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,                                                                                        /* A-Z */
                                            0, 0, 0, 0, 0, 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};                                                                                           /* a-z */

/**
 * @brief Get Base64 decoded buffer size.
 *
 * This routine calculates the expected raw data buffer size
 * for the given Base64 buffer and buffer size.
 *
 * @param[in] inputData Source buffer holding the base64 data
 * @param[in] inputLength Length of base64 data without termination character
 * @param[out] outputLengthP Pointer to calculated length of decoded data and the termination character
 *
 * @return true if successful, false otherwise.
 */
bool Base64_GetDecBufSize(uint8_t* inputData, uint32_t inputLength, uint32_t* outputLengthP)
{
    if ((inputData == NULL) || (outputLengthP == NULL))
    {
        return false;
    }

    *outputLengthP = inputLength / 4 * 3;

    if (*outputLengthP == 0)
    {
        return false;
    }

    if (inputData[inputLength - 1] == '=')
    {
        *outputLengthP = *outputLengthP - 1;
    }
    if (inputData[inputLength - 2] == '=')
    {
        *outputLengthP = *outputLengthP - 1;
    }

    return true;
}

/**
 * @brief Get Base64 encoded buffer size.
 *
 * This routine calculates the expected Base64 buffer size
 * for the given raw data size.
 *
 * @param[in] inputLength Length of the raw data
 * @param[out] outputLengthP Pointer to calculated length of encoded data and the termination character
 *
 * @return true if successful, false otherwise.
 */
bool Base64_GetEncBufSize(uint32_t inputLength, uint32_t* outputLengthP)
{
    if (outputLengthP == NULL)
    {
        return false;
    }

    /* Data size without additional string termination character */
    *outputLengthP = (4 * ((inputLength + 2) / 3));

    return true;
}

/**
 * @brief Decode Base64 data.
 *
 * This routine decodes the supplied data in Base64 format to raw data
 * and writes it to the output buffer (outputData, must be pre-allocated).
 * The size of the output buffer is also returned.
 *
 * @param[in] inputData Source buffer (holding the Base64 data to be decoded)
 * @param[in] inputLength Source buffer size
 * @param[out] outputData Destination buffer (will contain the decoded data)
 * @param[in,out] outputLength As input it is the size of the buffer allocated for outputData and it will
 * 				  be updated with the actual output length
 *
 * @return true if successful, false otherwise
 */
bool Base64_Decode(uint8_t* inputData, uint32_t inputLength, uint8_t* outputData, uint32_t* outputLength)
{
    if ((inputData == NULL) || (outputData == NULL) || (outputLength == NULL))
    {
        return false;
    }

    uint32_t decode_value;
    uint32_t nibble6_1, nibble6_2, nibble6_3, nibble6_4;
    uint32_t i, j;

    if (inputLength % 4 != 0)
    {
        return false;
    }

    uint32_t decoded_buffer_length;

    if (!Base64_GetDecBufSize(inputData, inputLength, &decoded_buffer_length))
    {
        return false;
    }

    if (*outputLength < decoded_buffer_length)
    {
        return false;
    }

    *outputLength = decoded_buffer_length;

    for (i = 0, j = 0; i < inputLength;)
    {
        nibble6_1 = inputData[i] == '=' ? 0 & i++ : base64DecTable[inputData[i++]];
        nibble6_2 = inputData[i] == '=' ? 0 & i++ : base64DecTable[inputData[i++]];
        nibble6_3 = inputData[i] == '=' ? 0 & i++ : base64DecTable[inputData[i++]];
        nibble6_4 = inputData[i] == '=' ? 0 & i++ : base64DecTable[inputData[i++]];

        decode_value = (nibble6_1 << 3 * 6) + (nibble6_2 << 2 * 6) + (nibble6_3 << 1 * 6) + (nibble6_4 << 0 * 6);

        if (j < *outputLength)
        {
            outputData[j++] = (decode_value >> 2 * 8) & 0xFF;
        }
        if (j < *outputLength)
        {
            outputData[j++] = (decode_value >> 1 * 8) & 0xFF;
        }
        if (j < *outputLength)
        {
            outputData[j++] = (decode_value >> 0 * 8) & 0xFF;
        }
    }

    return true;
}

/**
 * @brief Encodes raw data to Base64 format.
 *
 * This routine encodes the supplied raw data to base64 format and writes it to the
 * output buffer (outputData, must be pre-allocated).
 * The size of the output buffer is also returned.
 *
 * Note that the data is returned as a null terminated string, the destination
 * buffer must have size Base64_GetEncBufSize.
 *
 * @param[in] inputData Source buffer holding the raw data
 * @param[in] inputLength Source buffer size
 * @param[out] outputData Destination buffer (will contain the Base64 encoded data)
 * @param[in,out] outputLength As input it is the size of the buffer allocated for outputData and it will
 * 			   	  be updated with the actual output length
 *
 * @return true if successful, false otherwise
 */
bool Base64_Encode(uint8_t* inputData, uint32_t inputLength, uint8_t* outputData, uint32_t* outputLength)
{
    if ((inputData == NULL) || (outputData == NULL) || (outputLength == NULL))
    {
        return false;
    }

    uint32_t encodeValue;
    uint32_t nibble6_1, nibble6_2, nibble6_3;
    uint32_t i, j;

    uint32_t encoded_buffer_length;

    if (!Base64_GetEncBufSize(inputLength, &encoded_buffer_length))
    {
        return false;
    }

    if (*outputLength < encoded_buffer_length)
    {
        return false;
    }

    *outputLength = encoded_buffer_length;

    for (i = 0, j = 0; i < inputLength;)
    {
        nibble6_1 = i < inputLength ? inputData[i++] : 0;
        nibble6_2 = i < inputLength ? inputData[i++] : 0;
        nibble6_3 = i < inputLength ? inputData[i++] : 0;

        encodeValue = (nibble6_1 << 0x10) + (nibble6_2 << 0x08) + nibble6_3;

        outputData[j++] = base64EncTable[(encodeValue >> 3 * 6) & 0x3F];
        outputData[j++] = base64EncTable[(encodeValue >> 2 * 6) & 0x3F];
        outputData[j++] = base64EncTable[(encodeValue >> 1 * 6) & 0x3F];
        outputData[j++] = base64EncTable[(encodeValue >> 0 * 6) & 0x3F];
    }

    if (inputLength % 3 >= 1)
    {
        outputData[*outputLength - 1] = '=';
    }
    if (inputLength % 3 == 1)
    {
        outputData[*outputLength - 2] = '=';
    }

    return true;
}
