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
 * @brief This is the main header file of the WE Wireless Connectivity SDK.
 */

#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "global_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define WE_PIN(PIN_DEF_POINTER)                                                                                                                                                                                                                                                                                                                                                                                \
    ((WE_Pin_t){                                                                                                                                                                                                                                                                                                                                                                                               \
        .pin_def = PIN_DEF_POINTER,                                                                                                                                                                                                                                                                                                                                                                            \
    })

#define WE_PIN_UNDEFINED()                                                                                                                                                                                                                                                                                                                                                                                     \
    ((WE_Pin_t){                                                                                                                                                                                                                                                                                                                                                                                               \
        .pin_def = NULL,                                                                                                                                                                                                                                                                                                                                                                                       \
    })

#define IS_WE_PIN_UNDEFINED(PIN) (PIN.pin_def == NULL)

#ifndef UNUSED
#define UNUSED(X) (void)X /* To avoid gcc/g++ warnings */
#endif

/**
 * @brief Driver version
 */
#define WE_WIRELESS_CONNECTIVITY_SDK_VERSION {2, 4, 0}

#ifndef WE_DEBUG_PRINT_LEVEL
#define WE_DEBUG_PRINT_LEVEL WE_Debug_Print_Level_Off // Default level
#endif

#if (WE_DEBUG_PRINT_LEVEL > WE_Debug_Print_Level_Off)
#include "debug.h"
#define WE_DEBUG_PRINT(level, ...)                               \
   do                                                            \
   {                                                             \
      if ((level) <= WE_DEBUG_PRINT_LEVEL)                       \
      {                                                          \
         WE_Debug_Print(level, __FILE__, __func__, __VA_ARGS__); \
      }                                                          \
   } while (0)
#else
#define WE_DEBUG_PRINT(level, ...)
#endif

#define WE_DEBUG_PRINT_ERROR(...)   WE_DEBUG_PRINT(WE_Debug_Print_Level_Error, __VA_ARGS__)
#define WE_DEBUG_PRINT_INFO(...)   	WE_DEBUG_PRINT(WE_Debug_Print_Level_Info, __VA_ARGS__)
#define WE_DEBUG_PRINT_DEBUG(...) 	WE_DEBUG_PRINT(WE_Debug_Print_Level_Debug, __VA_ARGS__)

/**
 * @brief Priority for UART interrupts (used for communicating with radio module)
 */
#define WE_PRIORITY_UART_RX 0

/**
 * @brief Priority for (asynchronous) processing of data received from radio module.
 */
#define WE_PRIORITY_RX_DATA_PROCESSING 1

    /**
 * @brief Request the 3 byte driver version
 *
 * @param[out] version: Pointer to the 3 byte driver version.
 * @return true if request succeeded, false otherwise
 */
    static inline bool WE_GetDriverVersion(uint8_t* version)
    {
        uint8_t help[3] = WE_WIRELESS_CONNECTIVITY_SDK_VERSION;
        memcpy(version, help, 3);
        return true;
    }

    /**
 * @brief Initialize GPIO pins.
 *
 * @param[in] pins: Array of pins to configure. Entries that pin definition as NULL are ignored.
 * @param[in] numPins: Number of elements in pins array.
 * @return true if request succeeded, false otherwise
 */
    extern bool WE_InitPins(WE_Pin_t pins[], uint8_t numPins);

    /**
 * @brief Re-configure a pin.
 *
 * @param[in] pin: Pin to re-configure.
 * @return true if request succeeded, false otherwise
 */
    extern bool WE_Reconfigure(WE_Pin_t pin);

    /**
 * @brief Deinitialize GPIO pins.
 *
 * @param[in] pins: Array of pins to deinitalize. Entries that pin definition as NULL are ignored.
 * @param[in] numPins: Number of elements in pins array.
 * @return true if request succeeded, false otherwise
 */
    extern bool WE_DeinitPins(WE_Pin_t pins[], uint8_t numPins);

    /**
 * @brief Switch pin to output high/low
 *
 * @param[in] pin: Output pin to be set
 * @param[in] out: Output level to be set
 * @return true if request succeeded, false otherwise
 */

    extern bool WE_SetPin(WE_Pin_t pin, WE_Pin_Level_t out);

    /**
 * @brief Gets the pin level
 *
 * @param[in] pin: the pin to be checked
 *
 * @param[out] pin_levelP: the pin level
 *
 * @return true if request succeeded,
 *         false otherwise
 *
 */
    extern bool WE_GetPinLevel(WE_Pin_t pin, WE_Pin_Level_t* pin_levelP);

    /**
 * @brief Delay function.
 *
 * @param[in] delay: Delay in milliseconds
 */
    extern void WE_Delay(uint32_t delay);

    /**
 * @brief Delay function.
 *
 * Note that WE_MICROSECOND_TICK needs to be defined to enable microsecond timer resolution.
 *
 * @param[in] delay: Delay in microseconds
 */
    extern void WE_DelayMicroseconds(uint32_t delay);

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

#ifdef __cplusplus
}
#endif

#endif /* GLOBAL_H_INCLUDED */
