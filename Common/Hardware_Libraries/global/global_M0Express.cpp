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

#include "global_M0Express.h"

#ifdef M0Express

#include "global.h"
#include "ConfigPlatform.h"

#if defined(UART_RXPin0_TXPin1)

Uart *moduleUARTSERCOM2;
TypeHardwareSerial *serialModuleSERCOM2;

void SERCOM2_Handler()
{
    moduleUARTSERCOM2->IrqHandler();

    while (HSerial_available(serialModuleSERCOM2))
    {
        uint8_t receivedbyte = (uint8_t)HSerial_read(serialModuleSERCOM2);
        WE_UART_RXPin0_TXPin1_HandleRxByte(receivedbyte);
    }
}

void WE_UART_RXPin0_TXPin1_Init(uint32_t baudrate,
                          WE_FlowControl_t fc,
                          WE_Parity_t par)
{
    if (fc != WE_FlowControl_NoFlowControl)
    {
        WE_DEBUG_PRINT("Flow Control isnt supported \r\n");
    }
    moduleUARTSERCOM2 = new Uart(&sercom2, PIN_SERIAL1_RX, PIN_SERIAL1_TX, PAD_SERIAL1_RX,
                                 PAD_SERIAL1_TX);
    serialModuleSERCOM2 = HSerial_create(moduleUARTSERCOM2);
    HSerial_beginP(serialModuleSERCOM2, baudrate, (uint16_t)(HARDSER_STOP_BIT_1 | par | HARDSER_DATA_8));
    pinPeripheral(PIN_SERIAL1_RX, PIO_SERCOM_ALT);
    pinPeripheral(PIN_SERIAL1_TX, PIO_SERCOM_ALT);
}

void WE_UART_RXPin0_TXPin1_DeInit()
{
    HSerial_end(serialModuleSERCOM2);
    free(moduleUARTSERCOM2);
}

void WE_UART_RXPin0_TXPin1_Transmit(const char *data, uint16_t length)
{
    HSerial_writeB(serialModuleSERCOM2, data, length);
}

#endif /* UART_RXPin0_TXPin1 */

#if defined(UART_RXPin11_TXPin10)

Uart *moduleUARTSERCOM1;
TypeHardwareSerial *serialModuleSERCOM1;

void SERCOM1_Handler()
{
    moduleUARTSERCOM1->IrqHandler();

    while (HSerial_available(serialModuleSERCOM1))
    {
        uint8_t receivedbyte = (uint8_t)HSerial_read(serialModuleSERCOM1);
        WE_UART_RXPin11_TXPin10_HandleRxByte(receivedbyte);
    }
}

void WE_UART_RXPin11_TXPin10_Init(uint32_t baudrate,
                          WE_FlowControl_t fc,
                          WE_Parity_t par)
{
    if (fc != WE_FlowControl_NoFlowControl)
    {
        WE_DEBUG_PRINT("Flow Control isnt supported \r\n");
    }
    moduleUARTSERCOM1 = new Uart(&sercom1, 11, 10, SERCOM_RX_PAD_0,
                                 UART_TX_PAD_2);
    serialModuleSERCOM1 = HSerial_create(moduleUARTSERCOM1);
    HSerial_beginP(serialModuleSERCOM1, baudrate, (uint16_t)(HARDSER_STOP_BIT_1 | par | HARDSER_DATA_8));
    pinPeripheral(11, PIO_SERCOM);
    pinPeripheral(10, PIO_SERCOM);
}

void WE_UART_RXPin11_TXPin10_DeInit()
{
    HSerial_end(serialModuleSERCOM1);
    free(moduleUARTSERCOM1);
}

void WE_UART_RXPin11_TXPin10_Transmit(const char *data, uint16_t length)
{
    HSerial_writeB(serialModuleSERCOM1, data, length);
}
#endif /* UART_RXPin11_TXPin10 */

#endif
