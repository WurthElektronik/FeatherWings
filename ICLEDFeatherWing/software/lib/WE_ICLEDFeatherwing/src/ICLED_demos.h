/**
***************************************************************************************************
* This file is part of ICLED SDK:
*
*
* THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
*THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY KIND
*RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
*INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
*INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR REPRESENT
*THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY PATENT RIGHT,
*COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT RELATING TO
*ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED. INFORMATION
*PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS OR SERVICES
*DOES NOT CONSTITUTE A LICENSE FROM WÜRTH ELEKTRONIK EISOS TO USE SUCH PRODUCTS
*OR SERVICES OR A WARRANTY OR ENDORSEMENT THEREOF
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
*
* COPYRIGHT (c) 2024 Würth Elektronik eiSos GmbH & Co. KG
*
***************************************************************************************************
**/

#ifndef ICLED_DEMOS_H
#define ICLED_DEMOS_H

#include <stdint.h>
#include <stddef.h>

bool ICLED_demo_send_alphabet(uint16_t R_H, uint16_t G_S, uint16_t B_V, uint8_t brightness, uint32_t delay, bool *running = NULL);

bool ICLED_demo_show_rainbow(uint8_t brightness, uint32_t delay, bool *running = NULL);

bool ICLED_set_WElogo(uint16_t *place, uint8_t brightness);

bool ICLED_demo_show_temp_sensor_data(float temperature, uint8_t brightness, uint32_t delay, bool *running);

bool ICLED_demo_show_hum_sensor_data(float humidity, uint8_t brightness, uint32_t delay, bool *running);

bool ICLED_demo_show_ble_data(char *payload_buffer, uint16_t R_H, uint16_t G_S, uint16_t B_V, uint8_t brightness, uint32_t delay, bool *running);

bool ICLED_demo_show_price(char *string, uint16_t R_H, uint16_t G_S, uint16_t B_V, uint8_t brightness, uint32_t delay, bool *running);

bool ICLED_demo_start_show(bool *initial_test_run, bool *running, uint8_t *mode);

#endif
