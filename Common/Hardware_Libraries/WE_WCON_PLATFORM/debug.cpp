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

#include "global/debug.h"
#include "ConfigPlatform.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <we_print.h>

#if (WE_DEBUG_PRINT_LEVEL > WE_Debug_Print_Level_Off)

static const char *we_debug_level_string[] = {
    [WE_Debug_Print_Level_Off] = NULL,
    [WE_Debug_Print_Level_Error] = "ERROR",
    [WE_Debug_Print_Level_Info] = "INFO",
    [WE_Debug_Print_Level_Debug] = "DEBUG",
};

void WE_Debug_Print(WE_Debug_Print_Level_t level, const char *file, const char *func, const char *format, ...)
{
#ifdef WE_APP_PRINT
    WE_APP_PRINT("%s (File: %s, Function: %s): ", we_debug_level_string[level], file, func);
#endif /* WE_APP_PRINT */

#ifdef WE_APP_PRINT_V
    va_list args;
    va_start(args, format);
    WE_APP_PRINT_V(format, args);
    va_end(args);
#endif /* WE_APP_PRINT_V */
}

#endif // WE_DEBUG
