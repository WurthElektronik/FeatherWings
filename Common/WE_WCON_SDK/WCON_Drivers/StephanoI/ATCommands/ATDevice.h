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
 * @brief AT commands for basic device functionality.
 */

#ifndef STEPHANOI_DEVICE_H_INCLUDED
#define STEPHANOI_DEVICE_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ATDEVICE_DEEPSLEEP_TIME_MAX (uint32_t)2147483647 /**< Maximum timeout for sleep command */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief UART parity
 */
    typedef enum StephanoI_ATDevice_UartParity_t
    {
        StephanoI_ATDevice_UartParity_None = 0,
        StephanoI_ATDevice_UartParity_Odd = 1,
        StephanoI_ATDevice_UartParity_Even = 2,
    } StephanoI_ATDevice_UartParity_t;

    /**
 * @brief UART flow control
 */
    typedef enum StephanoI_ATDevice_UartFlowControl_t
    {
        StephanoI_ATDevice_UartFlowControl_None = 0,
        StephanoI_ATDevice_UartFlowControl_RTS = 1,
        StephanoI_ATDevice_UartFlowControl_CTS = 2,
        StephanoI_ATDevice_UartFlowControl_RTSnCTS = 3,
    } StephanoI_ATDevice_UartFlowControl_t;

    /**
 * @brief UART configuration
 */
    typedef struct StephanoI_ATDevice_UART_t
    {
        uint32_t baudrate;
        StephanoI_ATDevice_UartParity_t parity;
        StephanoI_ATDevice_UartFlowControl_t flow_control;
    } StephanoI_ATDevice_UART_t;

    /**
 * @brief Sleep mode
 */
    typedef enum StephanoI_ATDevice_SleepMode_t
    {
        StephanoI_ATDevice_SleepMode_Disable = 0,
        StephanoI_ATDevice_SleepMode_Modem = 1,
        StephanoI_ATDevice_SleepMode_Light = 2,
        StephanoI_ATDevice_SleepMode_ModemInterval = 3,
    } StephanoI_ATDevice_SleepMode_t;

    /**
 * @brief Wake-up source
 */
    typedef enum StephanoI_ATDevice_WakeupSource_t
    {
        StephanoI_ATDevice_WakeupSource_Reserved0 = 0,
        StephanoI_ATDevice_WakeupSource_Reserved1 = 1,
        StephanoI_ATDevice_WakeupSource_GPIO = 2,
    } StephanoI_ATDevice_WakeupSource_t;

    /**
 * @brief Wake-up pin number
 */
    typedef enum StephanoI_ATDevice_WakeupPin_t
    {
        StephanoI_ATDevice_WakeupPin_GPIO3 = 3,
        StephanoI_ATDevice_WakeupPin_GPIO10 = 10
    } StephanoI_ATDevice_WakeupPin_t;

    /**
 * @brief Bluetooth LE power value
 */
    typedef enum StephanoI_ATDevice_BluetoothLEPower_t
    {
        StephanoI_ATDevice_BluetoothLEPower_None = -1,
        StephanoI_ATDevice_BluetoothLEPower_Minus27 = 0,
        StephanoI_ATDevice_BluetoothLEPower_Minus24 = 1,
        StephanoI_ATDevice_BluetoothLEPower_Minus21 = 2,
        StephanoI_ATDevice_BluetoothLEPower_Minus18 = 3,
        StephanoI_ATDevice_BluetoothLEPower_Minus15 = 4,
        StephanoI_ATDevice_BluetoothLEPower_Minus12 = 5,
        StephanoI_ATDevice_BluetoothLEPower_Minus9 = 6,
        StephanoI_ATDevice_BluetoothLEPower_Minus6 = 7,
        StephanoI_ATDevice_BluetoothLEPower_Minus3 = 8,
        StephanoI_ATDevice_BluetoothLEPower_0 = 9,
        StephanoI_ATDevice_BluetoothLEPower_Plus3 = 10,
        StephanoI_ATDevice_BluetoothLEPower_Plus6 = 11,
        StephanoI_ATDevice_BluetoothLEPower_Plus9 = 12,
        StephanoI_ATDevice_BluetoothLEPower_Plus12 = 13,
        StephanoI_ATDevice_BluetoothLEPower_Plus15 = 14,
        StephanoI_ATDevice_BluetoothLEPower_Plus18 = 15,
    } StephanoI_ATDevice_BluetoothLEPower_t;

    /**
 * @brief Wifi power value
 */
    typedef enum StephanoI_ATDevice_WifiPower_t
    {
        StephanoI_ATDevice_WifiPower_Plus10 = 40,
        StephanoI_ATDevice_WifiPower_Plus11 = 44,
        StephanoI_ATDevice_WifiPower_Plus12 = 48,
        StephanoI_ATDevice_WifiPower_Plus13 = 52,
        StephanoI_ATDevice_WifiPower_Plus14 = 56,
        StephanoI_ATDevice_WifiPower_Plus15 = 60,
        StephanoI_ATDevice_WifiPower_Plus16 = 64,
        StephanoI_ATDevice_WifiPower_Plus17 = 68,
        StephanoI_ATDevice_WifiPower_Plus18 = 72,
        StephanoI_ATDevice_WifiPower_Plus19 = 76,
        StephanoI_ATDevice_WifiPower_Plus20 = 80,
    } StephanoI_ATDevice_WifiPower_t;

    /**
 * @brief Manufacturer data type
 */
    typedef enum StephanoI_ATDevice_SYSMFG_data_t
    {
        StephanoI_ATDevice_SYSMFG_data_u8 = 1,
        StephanoI_ATDevice_SYSMFG_data_i8 = 2,
        StephanoI_ATDevice_SYSMFG_data_u16 = 3,
        StephanoI_ATDevice_SYSMFG_data_i16 = 4,
        StephanoI_ATDevice_SYSMFG_data_u32 = 5,
        StephanoI_ATDevice_SYSMFG_data_i32 = 6,
        StephanoI_ATDevice_SYSMFG_data_string = 7,
        StephanoI_ATDevice_SYSMFG_data_binary = 8
    } StephanoI_ATDevice_SYSMFG_data_t;

    /**
 * @brief Manufacturer data
 */
    typedef struct StephanoI_ATDevice_SYSMFG_t
    {
        char namespace[20];
        char key[20];
        StephanoI_ATDevice_SYSMFG_data_t type;
        uint16_t length;
        uint8_t* value;
    } StephanoI_ATDevice_SYSMFG_t;

    /**
 * @brief File system read
 */
    typedef struct StephanoI_ATDevice_FileSystemRead_t
    {
        uint16_t length;
        uint8_t* value;
    } StephanoI_ATDevice_FileSystemRead_t;

    extern bool StephanoI_ATDevice_Test();
    extern bool StephanoI_ATDevice_GetVersion();
    extern bool StephanoI_ATDevice_Reset();
    extern bool StephanoI_ATDevice_Restore();
    extern bool StephanoI_ATDevice_EchoOn(bool on);
    extern bool StephanoI_ATDevice_SetCurrentUART(StephanoI_ATDevice_UART_t t);
    extern bool StephanoI_ATDevice_SetDefaultUART(StephanoI_ATDevice_UART_t t);
    extern bool StephanoI_ATDevice_GetDefaultUART(StephanoI_ATDevice_UART_t* t);
    extern bool StephanoI_ATDevice_SetSleep(StephanoI_ATDevice_SleepMode_t mode);
    extern bool StephanoI_ATDevice_GetSleep(StephanoI_ATDevice_SleepMode_t* modeP);
    extern bool StephanoI_ATDevice_SetDeepSleep(uint32_t time_ms);
    extern bool StephanoI_ATDevice_SetWakeUpSource(StephanoI_ATDevice_WakeupPin_t pin_number, bool active_high);
    extern bool StephanoI_ATDevice_SetTXPower(StephanoI_ATDevice_WifiPower_t wifi_power, StephanoI_ATDevice_BluetoothLEPower_t ble_power);
    extern bool StephanoI_ATDevice_GetTXPower(StephanoI_ATDevice_WifiPower_t* wifi_powerP, StephanoI_ATDevice_BluetoothLEPower_t* ble_powerP);
    extern bool StephanoI_ATDevice_GetManufacturingUserPartitions();
    extern bool StephanoI_ATDevice_EraseManufacturingUserPartitions(char* namespace, char* key);
    extern bool StephanoI_ATDevice_ReadManufacturingUserPartitions(char* namespace, char* key, uint16_t offset, uint16_t length, StephanoI_ATDevice_SYSMFG_t* t);
    extern bool StephanoI_ATDevice_WriteManufacturingUserPartitions(char* namespace, char* key, StephanoI_ATDevice_SYSMFG_data_t type, uint16_t length, uint8_t* dataP);
    extern bool StephanoI_ATDevice_SetSystemTimestamp(uint32_t time);
    extern bool StephanoI_ATDevice_GetSystemTimestamp(uint32_t* timeP);

    extern bool StephanoI_ATDevice_SetSystemStoremode(bool enable);
    extern bool StephanoI_ATDevice_GetSystemStoremode(bool* enableP);

    extern bool StephanoI_ATDevice_FileSystemSetMount(bool enable);
    extern bool StephanoI_ATDevice_FileSystemGetFiles();
    extern bool StephanoI_ATDevice_FileSystemDelete(char* filename);
    extern bool StephanoI_ATDevice_FileSystemRead(char* filename, uint16_t offset, uint16_t length, StephanoI_ATDevice_FileSystemRead_t* t);
    extern bool StephanoI_ATDevice_FileSystemGetFileSize(char* filename, uint16_t* sizeP);
    extern bool StephanoI_ATDevice_FileSystemWrite(char* filename, uint16_t offset, uint16_t length, uint8_t* dataP);

    extern bool StephanoI_ATDevice_GetSystemTemp(float* tempP);

    extern bool StephanoI_ATDevice_ParseManufacturingUserPartitions(char* EventArgumentsP, StephanoI_ATDevice_SYSMFG_t* t);
    extern bool StephanoI_ATDevice_ParseSystemTimestamp(char* EventArgumentsP, uint32_t* t);
    extern bool StephanoI_ATDevice_ParseSystemStoremode(char* EventArgumentsP, bool* t);
    extern bool StephanoI_ATDevice_ParseSystemTemp(char* EventArgumentsP, float* t);
    extern bool StephanoI_ATDevice_ParseFileSystemGetFilesize(char* EventArgumentsP, uint16_t* t);
    extern bool StephanoI_ATDevice_ParseFileSystemRead(char* EventArgumentsP, StephanoI_ATDevice_FileSystemRead_t* t);
    extern bool StephanoI_ATDevice_ParseSleep(char* EventArgumentsP, StephanoI_ATDevice_SleepMode_t* t);

#ifdef __cplusplus
}
#endif

#endif /* STEPHANOI_DEVICE_H_INCLUDED */
