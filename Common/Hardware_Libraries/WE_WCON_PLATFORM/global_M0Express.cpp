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

#include "global/global.h"
#include "ConfigPlatform.h"
#include <stdint.h>
#include <stdbool.h>

static bool WE_UART_Conf_To_Arduino_UART_Conf(WE_FlowControl_t fc,
                                              WE_Parity_t par, uint16_t *ardunio_uart_conf)
{
    if (ardunio_uart_conf == NULL)
    {
        return false;
    }

    if (fc != WE_FlowControl_NoFlowControl)
    {
        WE_DEBUG_PRINT_ERROR("Flow Control isnt supported.\r\n");
        return false;
    }

    switch (par)
    {
    case WE_Parity_None:
    {
        *ardunio_uart_conf = SERIAL_8N1;
        break;
    }
    case WE_Parity_Odd:
    {
        *ardunio_uart_conf = SERIAL_8O1;

        break;
    }
    case WE_Parity_Even:
    {
        *ardunio_uart_conf = SERIAL_8E1;
        break;
    }
    default:
    {
        WE_DEBUG_PRINT_ERROR("Unrecognized parity.\r\n");
        return false;
    }
    }

    return true;
}

#if defined(UART_RXPin0_TXPin1)

Uart *moduleUARTSERCOM2;
TypeHardwareSerial *serialModuleSERCOM2;

WE_UART_HandleRxByte_t *Sercom2RXByteHandlerP;

void SERCOM2_Handler()
{
    moduleUARTSERCOM2->IrqHandler();

    while (HSerial_available(serialModuleSERCOM2))
    {
        uint8_t receivedbyte = (uint8_t)HSerial_read(serialModuleSERCOM2);
        (*Sercom2RXByteHandlerP)(&receivedbyte, 1);
    }
}

bool WE_UART_RXPin0_TXPin1_Init(uint32_t baudrate,
                                WE_FlowControl_t fc,
                                WE_Parity_t par, WE_UART_HandleRxByte_t *rxByteHandlerP)
{
    uint16_t ardunio_uart_conf;

    if (!WE_UART_Conf_To_Arduino_UART_Conf(fc, par, &ardunio_uart_conf))
    {
        return false;
    }

    moduleUARTSERCOM2 = new Uart(&sercom2, PIN_SERIAL1_RX, PIN_SERIAL1_TX, PAD_SERIAL1_RX,
                                 PAD_SERIAL1_TX);
    serialModuleSERCOM2 = HSerial_create(moduleUARTSERCOM2);
    HSerial_beginP(serialModuleSERCOM2, baudrate, ardunio_uart_conf);
    pinPeripheral(PIN_SERIAL1_RX, PIO_SERCOM_ALT);
    pinPeripheral(PIN_SERIAL1_TX, PIO_SERCOM_ALT);

    Sercom2RXByteHandlerP = rxByteHandlerP;

    return true;
}

bool WE_UART_RXPin0_TXPin1_DeInit()
{
    HSerial_end(serialModuleSERCOM2);
    free(moduleUARTSERCOM2);
    Sercom2RXByteHandlerP = NULL;
    return true;
}

bool WE_UART_RXPin0_TXPin1_Transmit(const uint8_t *data, uint16_t length)
{
    HSerial_writeB(serialModuleSERCOM2, (const char *)data, length);
    return true;
}

#endif /* UART_RXPin0_TXPin1 */

#if defined(UART_RXPin11_TXPin10)

Uart *moduleUARTSERCOM1;
TypeHardwareSerial *serialModuleSERCOM1;

WE_UART_HandleRxByte_t *Sercom1RXByteHandlerP;

void SERCOM1_Handler()
{
    moduleUARTSERCOM1->IrqHandler();

    while (HSerial_available(serialModuleSERCOM1))
    {
        uint8_t receivedbyte = (uint8_t)HSerial_read(serialModuleSERCOM1);
        (*Sercom1RXByteHandlerP)(&receivedbyte, 1);
    }
}

bool WE_UART_RXPin11_TXPin10_Init(uint32_t baudrate,
                                  WE_FlowControl_t fc,
                                  WE_Parity_t par, WE_UART_HandleRxByte_t *rxByteHandlerP)
{
    uint16_t ardunio_uart_conf;

    if (!WE_UART_Conf_To_Arduino_UART_Conf(fc, par, &ardunio_uart_conf))
    {
        return false;
    }

    moduleUARTSERCOM1 = new Uart(&sercom1, 11, 10, SERCOM_RX_PAD_0,
                                 UART_TX_PAD_2);
    serialModuleSERCOM1 = HSerial_create(moduleUARTSERCOM1);
    HSerial_beginP(serialModuleSERCOM1, baudrate, ardunio_uart_conf);
    pinPeripheral(11, PIO_SERCOM);
    pinPeripheral(10, PIO_SERCOM);

    Sercom1RXByteHandlerP = rxByteHandlerP;

    return true;
}

bool WE_UART_RXPin11_TXPin10_DeInit()
{
    HSerial_end(serialModuleSERCOM1);
    free(moduleUARTSERCOM1);
    Sercom1RXByteHandlerP = NULL;
    return true;
}

bool WE_UART_RXPin11_TXPin10_Transmit(const uint8_t *data, uint16_t length)
{
    HSerial_writeB(serialModuleSERCOM1, (const char *)data, length);
    return true;
}
#endif /* UART_RXPin11_TXPin10 */

#endif
