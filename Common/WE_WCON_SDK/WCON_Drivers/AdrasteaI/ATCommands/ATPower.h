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
 * @brief MCU commands for Power functionality.
 */

#ifndef ADRASTEAI_AT_POWER_H_INCLUDED
#define ADRASTEAI_AT_POWER_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief MCU Power Modes
 */
    typedef enum AdrasteaI_ATPower_Mode_t
    {
        AdrasteaI_ATPower_Mode_Invalid = -1,
        AdrasteaI_ATPower_Mode_Stop,
        AdrasteaI_ATPower_Mode_Standby,
        AdrasteaI_ATPower_Mode_Shutdown,
        AdrasteaI_ATPower_Mode_NumberOfValues
    } AdrasteaI_ATPower_Mode_t;

    typedef uint16_t AdrasteaI_ATPower_Mode_Duration_t;

#define AdrasteaI_ATPower_Mode_Duration_Invalid 0

    extern bool AdrasteaI_ATPower_SetPowerMode(AdrasteaI_ATPower_Mode_t mode, AdrasteaI_ATPower_Mode_Duration_t duration);

    extern bool AdrasteaI_ATPower_EnableSleep();

#ifdef __cplusplus
}
#endif

#endif /* ADRASTEAI_AT_POWER_H_INCLUDED */
