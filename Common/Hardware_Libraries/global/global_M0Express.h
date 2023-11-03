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
 * @brief M0Express platform driver.
 */

#ifndef GLOBAL_M0EXPRESS_H_INCLUDED
#define GLOBAL_M0EXPRESS_H_INCLUDED

#ifdef M0Express

#include <stdbool.h>

#include "global_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(UART_RXPin0_TXPin1)

    void WE_UART_RXPin0_TXPin1_Init(uint32_t baudrate,
                              WE_FlowControl_t fc,
                              WE_Parity_t par);
    void WE_UART_RXPin0_TXPin1_DeInit();

    void WE_UART_RXPin0_TXPin1_Transmit(const char *data, uint16_t length);

    void WE_UART_RXPin0_TXPin1_HandleRxByte(uint8_t receivedByte);

#endif /* UART_RXPin0_TXPin1 */

#if defined(UART_RXPin11_TXPin10)

    void WE_UART_RXPin11_TXPin10_Init(uint32_t baudrate,
                              WE_FlowControl_t fc,
                              WE_Parity_t par);
    void WE_UART_RXPin11_TXPin10_DeInit();

    void WE_UART_RXPin11_TXPin10_Transmit(const char *data, uint16_t length);

    void WE_UART_RXPin11_TXPin10_HandleRxByte(uint8_t receivedByte);

#endif /* UART_RXPin11_TXPin10 */

#ifdef __cplusplus
}
#endif

#endif /* M0Express */

#endif /* GLOBAL_M0EXPRESS_H_INCLUDED */
