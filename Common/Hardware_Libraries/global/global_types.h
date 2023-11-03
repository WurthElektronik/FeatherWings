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
 * @brief Contains type definitions used in the Wireless Connectivity SDK.
 */

#ifndef GLOBAL_TYPES_H_INCLUDED
#define GLOBAL_TYPES_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Used to configure a pin as input or output.
     */
    typedef enum WE_Pin_Type_t
    {
        WE_Pin_Type_Input = (uint8_t)0, /**< Input pin */
        WE_Pin_Type_Output = (uint8_t)1 /**< Output pin */
    } WE_Pin_Type_t;

    /**
     * @brief Configuration of one pin.
     */
    typedef struct WE_Pin_t
    {
        uint32_t pin;
        WE_Pin_Type_t type;
    } WE_Pin_t;

    /**
     * @brief Configures pin pull type.
     */
    typedef enum WE_Pin_PullType_t
    {
        WE_Pin_PullType_No = (uint8_t)0,  /**< No pull up / pull down */
        WE_Pin_PullType_Up = (uint8_t)1,  /**< Pull up */
        WE_Pin_PullType_Down = (uint8_t)2 /**< Pull down */
    } WE_Pin_PullType_t;

    /**
     * @brief Pin level (high/low).
     */
    typedef enum WE_Pin_Level_t
    {
        WE_Pin_Level_Low = (uint8_t)0, /**< Pin level LOW */
        WE_Pin_Level_High = (uint8_t)1 /**< Pin level HIGH */
    } WE_Pin_Level_t;

    /**
     * @brief UART flow control configuration.
     */
    typedef enum WE_FlowControl_t
    {
        WE_FlowControl_NoFlowControl = (uint8_t)0, /**< Flow control disabled */
        WE_FlowControl_RTSOnly = (uint8_t)1,       /**< Flow control using RTS only */
        WE_FlowControl_CTSOnly = (uint8_t)2,       /**< Flow control using CTS only */
        WE_FlowControl_RTSAndCTS = (uint8_t)3,     /**< Flow control using RTS and CTS */
    } WE_FlowControl_t;

    /**
     * @brief UART parity configuration.
     */
    typedef enum WE_Parity_t
    {
        WE_Parity_Even = (uint8_t)1, /**< Even parity */
        WE_Parity_Odd = (uint8_t)2,  /**< Odd parity */
        WE_Parity_None = (uint8_t)3  /**< No parity */
    } WE_Parity_t;

#ifdef __cplusplus
}
#endif

#endif /* GLOBAL_TYPES_H_INCLUDED */
