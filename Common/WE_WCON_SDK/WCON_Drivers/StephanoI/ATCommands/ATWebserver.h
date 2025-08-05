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
 * @brief AT commands for Web server functionality.
 */

#ifndef STEPHANOI_WEBSERVER_H_INCLUDED
#define STEPHANOI_WEBSERVER_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Webserver Response type
 */
    typedef enum StephanoI_ATWebserver_Response_t
    {
        StephanoI_ATWebserver_Response_ReceivedWifiInformation = 1,
        StephanoI_ATWebserver_Response_StationConnected = 2,
        StephanoI_ATWebserver_Response_OTAReceiveDataBegin = 3,
        StephanoI_ATWebserver_Response_OTAReceiveDataSuccess = 4,
        StephanoI_ATWebserver_Response_OTAReceiveDataFailed = 5,
    } StephanoI_ATWebserver_Response_t;

    extern bool StephanoI_ATWebserver_Enable(bool enable, uint16_t server_port, uint8_t connection_timeout);

    extern bool StephanoI_ATWebserver_ParseResponse(char* EventArgumentsP, StephanoI_ATWebserver_Response_t* t);

#ifdef __cplusplus
}
#endif

#endif /* STEPHANOI_WEBSERVER_H_INCLUDED */
