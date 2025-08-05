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
 * @brief AT commands for GPIO functionality.
 */

#ifndef CALYPSO_AT_GPIO_H_INCLUDED
#define CALYPSO_AT_GPIO_H_INCLUDED
#include <Calypso/Calypso.h>
#include <global/ATCommands.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief GPIOs that are available on the Calypso module.
 */
    typedef enum Calypso_ATGPIO_GPIOId_t
    {
        Calypso_ATGPIO_GPIOId_0,
        Calypso_ATGPIO_GPIOId_1,
        Calypso_ATGPIO_GPIOId_2,
        Calypso_ATGPIO_GPIOId_3,
        Calypso_ATGPIO_GPIOId_NumberOfValues
    } Calypso_ATGPIO_GPIOId_t;

    /**
 * @brief Types of functions that can be assigned to GPIOs.
 */
    typedef enum Calypso_ATGPIO_GPIOType_t
    {
        Calypso_ATGPIO_GPIOType_Unused,
        Calypso_ATGPIO_GPIOType_Input,
        Calypso_ATGPIO_GPIOType_Output,
        Calypso_ATGPIO_GPIOType_PWM,
        Calypso_ATGPIO_GPIOType_NumberOfValues
    } Calypso_ATGPIO_GPIOType_t;

    /**
 * @brief GPIO input pull types.
 */
    typedef enum Calypso_ATGPIO_PullType_t
    {
        Calypso_ATGPIO_PullType_NoPull,
        Calypso_ATGPIO_PullType_PullUp,
        Calypso_ATGPIO_PullType_PullDown,
        Calypso_ATGPIO_PullType_NumberOfValues
    } Calypso_ATGPIO_PullType_t;

    /**
 * @brief GPIO input/output state.
 */
    typedef enum Calypso_ATGPIO_GPIOState_t
    {
        Calypso_ATGPIO_GPIOState_Low = 0,
        Calypso_ATGPIO_GPIOState_High = 1,
        Calypso_ATGPIO_GPIOState_NumberOfValues = 2
    } Calypso_ATGPIO_GPIOState_t;

    /**
 * @brief Input parameters and/or state (depending on usage).
 */
    typedef struct Calypso_ATGPIO_Input_t
    {
        /**
	 * @brief Input level
	 */
        Calypso_ATGPIO_GPIOState_t state;

        /**
	 * @brief Input pull type
	 */
        Calypso_ATGPIO_PullType_t pullType;
    } Calypso_ATGPIO_Input_t;

    /**
 * @brief Output parameters and/or state (depending on usage).
 */
    typedef struct Calypso_ATGPIO_Output_t
    {
        /**
	 * @brief Output level
	 */
        Calypso_ATGPIO_GPIOState_t state;
    } Calypso_ATGPIO_Output_t;

    /**
 * @brief PWM parameters and/or state (depending on usage).
 */
    typedef struct Calypso_ATGPIO_PWM_t
    {
        /**
	 * @brief PWM Period in milliseconds
	 */
        uint16_t period;

        /**
	 * @brief PWM ratio in percent
	 */
        uint8_t ratio;
    } Calypso_ATGPIO_PWM_t;

    /**
 * @brief GPIO parameters and/or state (depending on usage).
 *
 * Provide this union as an argument to Calypso_ATGPIO_Get() or Calypso_ATGPIO_Set() and use
 * the element of the union that corresponds to the used GPIO type.
 *
 * @see Calypso_ATGPIO_Get(), Calypso_ATGPIO_Set()
 */
    typedef union Calypso_ATGPIO_Parameters_t
    {
        Calypso_ATGPIO_Input_t input;
        Calypso_ATGPIO_Output_t output;
        Calypso_ATGPIO_PWM_t pwm;
    } Calypso_ATGPIO_Parameters_t;

    /**
 * @brief GPIO configuration and/or state (depending on usage).
 *
 * @see Calypso_ATGPIO_Get(), Calypso_ATGPIO_Set()
 */
    typedef struct Calypso_ATGPIO_GPIO_t
    {
        /**
	 * @brief GPIO ID
	 */
        Calypso_ATGPIO_GPIOId_t id;

        /**
	 * @brief Function assigned to the GPIO.
	 */
        Calypso_ATGPIO_GPIOType_t type;

        /**
	 * @brief Parameters of the GPIO.
	 * Use the element of the Calypso_ATGPIO_Parameters_t union that matches the selected GPIO type.
	 */
        Calypso_ATGPIO_Parameters_t parameters;
    } Calypso_ATGPIO_GPIO_t;

    extern bool Calypso_ATGPIO_Get(Calypso_ATGPIO_GPIOId_t id, bool defaultSetting, Calypso_ATGPIO_GPIO_t* gpio);
    extern bool Calypso_ATGPIO_Set(Calypso_ATGPIO_GPIO_t* gpio, bool save);
    extern bool Calypso_ATGPIO_IsFunctionSupported(Calypso_ATGPIO_GPIOId_t id, Calypso_ATGPIO_GPIOType_t type);

#ifdef __cplusplus
}
#endif

#endif /* CALYPSO_AT_GPIO_H_INCLUDED */
