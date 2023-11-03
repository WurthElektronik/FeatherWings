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
 * @brief Contains code related to redirecting printf() output to UART for debug reasons.
 */

#include "debug.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "global.h"

#if defined(WE_DEBUG)

TypeSerial *SerialDebug;

/**
 * @brief Initializes UART2 and connects this interface to printf().
 *
 * Note that for redirection of printf() output to work, there must not be other
 * definitions of system file functions such as _write() (e.g. it might be necessary
 * to exclude the STM32CubeIDE-generated syscalls.c from compilation).
 *
 * Also note that it is not safe to call printf() from different contexts (e.g.
 * inside main and inside ISRs) - there is no guarantee, that the debug output is
 * forwarded correctly in all circumstances.
 *
 * There are two preprocessor defines controlling debug behavior:
 * - WE_DEBUG: Initialize debug UART and enable printing of debug messages in drivers.
 * - WE_DEBUG_INIT: Initialize debug UART but disable printing of debug messages in drivers
 *   (adds support for debugging using printf in user/example code, but drivers don't print
 *   diagnostic/debug info).
 */
void WE_Debug_Init()
{
    SerialDebug = SSerial_create(&Serial);
    SSerial_begin(SerialDebug, 921600);
}

void WE_Debug_Print(const char format[], ...)
{
    va_list ap;
    va_start(ap, format);
    SSerial_vprintf(SerialDebug, format, ap);
    va_end(ap);
}

#endif // WE_DEBUG
