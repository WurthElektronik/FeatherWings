/**
 * \file
 * \brief Aduino platform drivers for Adafruit M0 feather.
 *
 * This code is abstraction of arduino peripheral drivers for Adafruit feather MO board.
 *
 * \copyright (c) 2020 Würth Elektronik eiSos GmbH & Co. KG
 *
 * \page License
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
 * IN THE ROOT DIRECTORY OF THIS PACKAGE
 */

#ifndef ARDUINOPLATFORM_H
#define ARDUINOPLATFORM_H

/**         Includes         */

#include <stdint.h>
#include <Arduino.h>
#include <wiring_private.h>

#define WE_SUCCESS 0
#define WE_FAIL 1
#define MAX_PRINT_LEN 1280
#define I2C_CLOCK_SPEED_FAST 400000
#define I2C_CLOCK_SPEED_STANDARD 100000

#define NEO_PIXEL_RED ((uint32_t)(50 << 16) + (uint32_t)(0 << 8) + (uint32_t)0)
#define NEO_PIXEL_BLUE ((uint32_t)(0 << 16) + (uint32_t)(0 << 8) + (uint32_t)50)
#define NEO_PIXEL_ORANGE ((uint32_t)(50 << 16) + (uint32_t)(15 << 8) + (uint32_t)0)
#define NEO_PIXEL_GREEN ((uint32_t)(0 << 16) + (uint32_t)(50 << 8) + (uint32_t)0)
#define NEO_PIXEL_OFF (uint32_t)0

#define BTN_LONG_PRESS_DURATION_MS 2000

/**         Functions definition         */

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        void *obj;
    } TypeSerial;

    void soft_reset();
    void SSerial_destroy(TypeSerial *m);
    TypeSerial *SSerial_create(void *ser);

    size_t SSerial_write(TypeSerial *m, uint8_t byte);
    size_t SSerial_writeB(TypeSerial *m, const char *buffer, size_t size);
    void SSerial_begin(TypeSerial *m, uint32_t baud_count);
    void SSerial_beginP(TypeSerial *m, uint32_t baud_count, uint8_t parameter);
    int SSerial_available(TypeSerial *m);
    void SSerial_flush(TypeSerial *m);
    void SSerial_printf(TypeSerial *m, const char format[], ...);
    int SSerial_read(TypeSerial *m);

    typedef struct
    {
        void *obj;
    } TypeHardwareSerial;

    void HSerial_destroy(TypeHardwareSerial *m);
    TypeHardwareSerial *HSerial_create(void *ser);

    size_t HSerial_write(TypeHardwareSerial *m, uint8_t byte);
    size_t HSerial_writeB(TypeHardwareSerial *m, const char *buffer, size_t size);
    void HSerial_begin(TypeHardwareSerial *m, uint32_t baud_count);
    void HSerial_beginP(TypeHardwareSerial *m, uint32_t baud_count,
                        uint8_t parameter);
    void HSerial_end(TypeHardwareSerial *m);
    int HSerial_available(TypeHardwareSerial *m);
    int HSerial_availableForWrite(TypeHardwareSerial *m);
    void HSerial_flush(TypeHardwareSerial *m);
    int HSerial_read(TypeHardwareSerial *m);

    void I2CSetAddress(int address);
    int8_t I2CInit(int address);
    void I2CSetClock(uint32_t baudrate);
    int8_t ReadReg(uint8_t RegAdr, int NumByteToRead, uint8_t *Data);
    int8_t WriteReg(int RegAdr, int NumByteToWrite, uint8_t *Data);
    int8_t I2CReceive(uint8_t *data, int datalen);
    int8_t I2CSend(uint8_t *data, int datalen);

    void neopixelInit();
    void neopixelSet(uint32_t color);

    void buttonInit(uint8_t pin, void (*OnBtnPress)(), void (*OnBtnLongPress)());
    void buttonUpdate();

#ifdef __cplusplus
}
#endif

#endif /* ARDUINOPLATFORM_H */