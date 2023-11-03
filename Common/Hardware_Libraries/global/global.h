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
 * @brief This is the main header file of the WE Wireless Connectivity SDK for STM32.
 */

#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <global_types.h>
#include "ConfigPlatform.h"
#include "../WeSensorsSDK.h"

#ifdef M0Express
#include "global_M0Express.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(WE_DEBUG)
#include "debug.h"
#endif /* WE_DEBUG */

    /**
     * @brief Initialize GPIO pins.
     *
     * @param[in] pins Array of pins to configure. Entries that have port or pin number set to zero are ignored.
     * @param[in] numPins Number of elements in pins array.
     * @return true if request succeeded, false otherwise
     */
    extern bool WE_InitPins(WE_Pin_t pins[], uint8_t numPins);

    /**
     * @brief Deinitialize a pin.
     *
     * @param[in] pin Pin to be deinitialized
     * @return true if request succeeded, false otherwise
     */
    extern bool WE_DeinitPin(WE_Pin_t pin);

    /**
     * @brief Switch pin to output high/low
     *
     * @param[in] pin Output pin to be set
     * @param[in] out Output level to be set
     * @return true if request succeeded, false otherwise
     */
    extern bool WE_SetPin(WE_Pin_t pin, WE_Pin_Level_t out);

    /**
     * @brief Read current pin level.
     *
     * @param[in] pin Pin to be read
     * @return Current level of pin
     */
    extern WE_Pin_Level_t WE_GetPinLevel(WE_Pin_t pin);

    /**
     * @brief Sleep function.
     *
     * @param[in] sleepForMs Delay in milliseconds
     */
    extern void WE_Delay(uint16_t sleepForMs);

    /**
     * @brief Sleep function.
     *
     * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
     *
     * @param[in] sleepForUsec Delay in microseconds
     */
    extern void WE_DelayMicroseconds(uint32_t sleepForUsec);

    /**
     * @brief Returns current tick value (in milliseconds).
     *
     * @return Current tick value (in milliseconds)
     */
    extern uint32_t WE_GetTick();

    /**
     * @brief Returns current tick value (in microseconds).
     *
     * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
     *
     * @return Current tick value (in microseconds)
     */
    extern uint32_t WE_GetTickMicroseconds();

    extern int8_t WE_ReadReg(WE_sensorInterface_t *interface,
                             uint8_t regAdr,
                             uint16_t numBytesToRead,
                             uint8_t *data);

    extern int8_t WE_WriteReg(WE_sensorInterface_t *interface,
                              uint8_t regAdr,
                              uint16_t numBytesToWrite,
                              uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif /* GLOBAL_H_INCLUDED */
