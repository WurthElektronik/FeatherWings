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

#ifndef STEPHANOI_HTTP_H_INCLUDED
#define STEPHANOI_HTTP_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief HTTP option
 */
    typedef enum StephanoI_ATHTTP_Opt_t
    {
        StephanoI_ATHTTP_Opt_Head = 1,
        StephanoI_ATHTTP_Opt_Get = 2,
        StephanoI_ATHTTP_Opt_Post = 3,
        StephanoI_ATHTTP_Opt_Put = 4,
        StephanoI_ATHTTP_Opt_Delete = 5,
    } StephanoI_ATHTTP_Opt_t;

    /**
 * @brief HTTP content type
 */
    typedef enum StephanoI_ATHTTP_Content_t
    {
        StephanoI_ATHTTP_Content_ApplicationURLEncoded = 0,
        StephanoI_ATHTTP_Content_ApplicationJson = 1,
        StephanoI_ATHTTP_Content_MultipartFormdata = 2,
        StephanoI_ATHTTP_Content_textxml = 3,
    } StephanoI_ATHTTP_Content_t;

    /**
 * @brief HTTP client event
 */
    typedef struct StephanoI_ATHTTP_Client_t
    {
        uint16_t length;
        uint8_t* data;
    } StephanoI_ATHTTP_Client_t;

    /**
 * @brief HTTP get size event
 */
    typedef uint16_t StephanoI_ATHTTP_Size_t;

    /**
 * @brief HTTP get event
 */
    typedef struct StephanoI_ATHTTP_Get_t
    {
        uint16_t length;
        uint8_t* data;
    } StephanoI_ATHTTP_Get_t;

    /**
 * @brief HTTP URL
 */
    extern bool StephanoI_ATHTTP_Client(StephanoI_ATHTTP_Opt_t opt, StephanoI_ATHTTP_Content_t content, char* url, char* host, char* path, bool transport_via_SSL, char* data, char* header);
    extern bool StephanoI_ATHTTP_GetSize(char* url, StephanoI_ATHTTP_Size_t* size);
    extern bool StephanoI_ATHTTP_Get(char* url, StephanoI_ATHTTP_Get_t* t);
    extern bool StephanoI_ATHTTP_Post(char* url, uint8_t* data, uint32_t length, uint8_t number_of_headers, char** headers);

    extern bool StephanoI_ATHTTP_ParseClient(char* EventArgumentsP, StephanoI_ATHTTP_Client_t* t);
    extern bool StephanoI_ATHTTP_ParseGetSize(char* EventArgumentsP, StephanoI_ATHTTP_Size_t* t);
    extern bool StephanoI_ATHTTP_ParseGet(char* EventArgumentsP, StephanoI_ATHTTP_Get_t* t);

#ifdef __cplusplus
}
#endif

#endif /* STEPHANOI_HTTP_H_INCLUDED */
