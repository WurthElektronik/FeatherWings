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
 * @brief AT commands for GPIO functionality.
 */

#ifndef AT_GPIO_H_INCLUDED
#define AT_GPIO_H_INCLUDED

#include <ATCommands.h>
#include <Calypso.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GPIOs that are available on the Calypso module.
 */
typedef enum ATGPIO_GPIOId_t
{
    ATGPIO_GPIOId_0,
    ATGPIO_GPIOId_1,
    ATGPIO_GPIOId_2,
    ATGPIO_GPIOId_3,
    ATGPIO_GPIOId_NumberOfValues
} ATGPIO_GPIOId_t;

/**
 * @brief Types of functions that can be assigned to GPIOs.
 */
typedef enum ATGPIO_GPIOType_t
{
    ATGPIO_GPIOType_Unused,
    ATGPIO_GPIOType_Input,
    ATGPIO_GPIOType_Output,
    ATGPIO_GPIOType_PWM,
    ATGPIO_GPIOType_NumberOfValues
} ATGPIO_GPIOType_t;

/**
 * @brief GPIO input pull types.
 */
typedef enum ATGPIO_PullType_t
{
    ATGPIO_PullType_NoPull,
    ATGPIO_PullType_PullUp,
    ATGPIO_PullType_PullDown,
    ATGPIO_PullType_NumberOfValues
} ATGPIO_PullType_t;

/**
 * @brief GPIO input/output state.
 */
typedef enum ATGPIO_GPIOState_t
{
    ATGPIO_GPIOState_Low = 0,
    ATGPIO_GPIOState_High = 1,
    ATGPIO_GPIOState_NumberOfValues = 2
} ATGPIO_GPIOState_t;

/**
 * @brief Input parameters and/or state (depending on usage).
 */
typedef struct ATGPIO_Input_t
{
    /**
     * @brief Input level
     */
    ATGPIO_GPIOState_t state;

    /**
     * @brief Input pull type
     */
    ATGPIO_PullType_t pullType;
} ATGPIO_Input_t;

/**
 * @brief Output parameters and/or state (depending on usage).
 */
typedef struct ATGPIO_Output_t
{
    /**
     * @brief Output level
     */
    ATGPIO_GPIOState_t state;
} ATGPIO_Output_t;

/**
 * @brief PWM parameters and/or state (depending on usage).
 */
typedef struct ATGPIO_PWM_t
{
    /**
     * @brief PWM Period in milliseconds
     */
    uint16_t period;

    /**
     * @brief PWM ratio in percent
     */
    uint8_t ratio;
} ATGPIO_PWM_t;

/**
 * @brief GPIO parameters and/or state (depending on usage).
 *
 * Provide this union as an argument to ATGPIO_Get() or ATGPIO_Set() and use
 * the element of the union that corresponds to the used GPIO type.
 *
 * @see ATGPIO_Get(), ATGPIO_Set()
 */
typedef union ATGPIO_Parameters_t
{
    ATGPIO_Input_t input;
    ATGPIO_Output_t output;
    ATGPIO_PWM_t pwm;
} ATGPIO_Parameters_t;

/**
 * @brief GPIO configuration and/or state (depending on usage).
 *
 * @see ATGPIO_Get(), ATGPIO_Set()
 */
typedef struct ATGPIO_GPIO_t
{
    /**
     * @brief GPIO ID
     */
    ATGPIO_GPIOId_t id;

    /**
     * @brief Function assigned to the GPIO.
     */
    ATGPIO_GPIOType_t type;

    /**
     * @brief Parameters of the GPIO.
     * Use the element of the ATGPIO_Parameters_t union that matches the selected GPIO type.
     */
    ATGPIO_Parameters_t parameters;
} ATGPIO_GPIO_t;

extern bool ATGPIO_Get(ATGPIO_GPIOId_t id, bool defaultSetting, ATGPIO_GPIO_t *gpio);
extern bool ATGPIO_Set(ATGPIO_GPIO_t *gpio, bool save);
extern bool ATGPIO_IsFunctionSupported(ATGPIO_GPIOId_t id, ATGPIO_GPIOType_t type);

#ifdef __cplusplus
}
#endif

#endif /* AT_GPIO_H_INCLUDED */
