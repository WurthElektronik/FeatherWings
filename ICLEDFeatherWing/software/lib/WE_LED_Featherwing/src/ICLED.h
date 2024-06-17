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

#ifndef ICLED
#define ICLED

//#include <Adafruit_ZeroDMA.h>
#include <SPI.h>
#include "ArduinoPlatform.h"


// Letters are optimized for the WE-WLICLED Featherwing, no guarantee for other
// display sizes and shapes
#define COLUMNS 15    // Columns on screen
#define ROWS 7        // Rows on screen
#define LEDCOUNT 105  // LEDs on screen
#define SCREENSTORUN 45
// Maximum screen width reserved in RAM for scrolling Text
// set to 1 if no scrolling texts wanted --> saves RAM
#define BYTESPERPIXEL 3  // GRB , each is 8bit= 1 Byte
#define BYTESTOTAL                               \
    (LEDCOUNT * BYTESPERPIXEL * 8 * 4 + 7) / 8 + \
        120                // total Bytes needed for spi
#define TEMPCORRECTION -5  // Temperature Correction

#define MAXBRIGHTNESS \
    210  // Max Brightness of all 3 colors summed up ; do only change if you
         // know what you are doing



enum ColSystem { GRB, HSV, invalid };
class WLICLED {
   public:
    WLICLED(ColSystem ColSys);
    ~WLICLED();

    bool begin(void);  // System setup for dma and spi transfer | best practice
                       // for feather m0 (&express)
    // only use this, if you know, what you are doing --> see cpp file
    bool begin(SERCOM *sercom, Sercom *sercomBase, uint8_t dmacID, uint8_t mosi,
               SercomSpiTXPad padTX, EPioType pinFunc);
    void set_pixel(uint16_t pixel, uint16_t G_H, uint8_t R_S, uint8_t B_V,
                   uint8_t Bright = 255);  // sets pixel on given color
    void set_screenPixel(
        uint8_t column, uint8_t row, uint16_t G_H, uint8_t R_S, uint8_t B_V,
        uint8_t Bright = 255);  // sets pixel by coordinates (column,line)
    void set_pixelHSV(
        uint16_t pixel, uint16_t H, uint8_t S, uint8_t V, uint8_t &G,
        uint8_t &R, uint8_t &B,
        uint8_t &Bright);  // sets pixel on given color in HSV coordinates
    void set_all(uint16_t G_H, uint8_t R_S, uint8_t B_V,
                 uint8_t Bright = 255);  // sets all pixels
    void show(uint32_t del = 50);        // starts transfering data to LEDs
    void showLoop(uint32_t del = 50);    // starts transfering data to LEDs and
                                         // until interrupt
    void showRun(uint32_t del = 5, uint16_t end = COLUMNS * (SCREENSTORUN - 1),
                 uint16_t times = 1);  // starts scrolling transfer
    void showRunLoop(
        uint32_t del = 5,
        uint16_t end = COLUMNS *
                       (SCREENSTORUN -
                        1));  // same as above but in loop until interrupt
    uint16_t set_char(char c, uint16_t place, uint16_t G_H, uint8_t R_S,
                      uint8_t B_V,
                      uint8_t Bright = 255);  // writes a letter in LEDBuf
    uint16_t set_string(char c[], uint16_t place, uint16_t G_H, uint8_t R_S,
                        uint8_t B_V,
                        uint8_t Bright = 255);  // writes a string in LEDBuf
    uint16_t set_WElogo(uint16_t place = 1,
                        uint8_t Bright = 10);  // writes the WE-Logo in LEDBuf
    uint16_t set_Wuerthlogo(
        uint16_t place = 1,
        uint8_t Bright = 10);  // Writes the Würth logo in LEDBuf
    void set_charPixel(
        uint16_t column, uint16_t row, uint16_t G_H, uint8_t R_S, uint8_t B_V,
        uint8_t Bright =
            255);      // mostly internal used, sets pixel on expanded screen
    void clearChar();  // clears expanded LEDBuf
    void send_Alphabet(uint16_t G_H, uint8_t R_S, uint8_t B_V,
                       uint8_t Bright = 25);  // shows the Alphabet on screen
    void fill(uint16_t G, uint8_t R, uint8_t B,
              uint8_t Bright = 255);  // fills expanded LEDBuf with given color
    void showPrice(char c[], uint16_t place, uint16_t G_H, uint8_t R_S,
                   uint8_t B_V,
                   uint8_t Bright = 255);  // shows a price with € on screen
    uint16_t set_euro(uint16_t place, uint16_t G, uint8_t R, uint8_t B,
                      uint8_t Bright = 255);  // writes the '€' symbol in LEDBuf
    void testimage(uint16_t G_H, uint8_t R_S,
                   uint8_t B_V);  // shows a test immage on screen
    void showRainbow(
        uint32_t del = 5,
        uint8_t Bright = 255);  // shows scrolling rainbow until interrupt
    
    
    uint16_t set_emoji(
        uint8_t e, int32_t place, uint16_t G_H, uint8_t R_S, uint8_t B_V,
        uint8_t Bright =
            255);  // sets an emoji on screen, for emoji list see WE-Github site
    

    void to_HSV();  // changes color mode to HSV system
    void to_GRB();  // changes color mode to GRB system

    void clear();            // clears LEDBuf
    void init_show();        // starts initialising show
    void init_show_light();  // starts light init show
    void init_concern();     // starts light animation for Commitment meeting
    bool interrupt = false;  // little helper for interrupts, set true to
                             // trigger interrupt and end Loop methodes

    // Serial Debug Console
    TypeSerial *SerialDebug;


#

   protected:
    Adafruit_ZeroDMA dma;  ///< The DMA manager for the SPI class
    SPIClass *spi;         ///< Underlying SPI hardware interface we use to DMA
    inline uint8_t calc(uint8_t color, uint8_t Bright);
    
                       // connected
    
   
    void Temperature_Safety(uint8_t &G, uint8_t &R, uint8_t &B, uint8_t Bright,
                            uint8_t G_H, uint8_t R_S,
                            uint8_t B_V);  // Prevents Board from overheating

    ColSystem ColorSystem = invalid;  // enum to change color System
    bool dmaTrans = false;            // check if dma is activated
    // Sensor checks --> true if sensors ok and connected
    
    // ITDS checks for orientation and changes bool turned
    // set true to turn Board around if no ITDS sensor connected
    bool turned = false;

    uint32_t dmaBuf[LEDCOUNT * BYTESPERPIXEL + 35] = {
        0};  // The raw buffer we write to SPI

    // buffer for LEDs --> will be written into dmaBuf after bit-expansion in
    // show() methode --> used to save some RAM for bigger animations
    struct {
        uint8_t G;
        uint8_t R;
        uint8_t B;

    } LEDBuf[LEDCOUNT * SCREENSTORUN];
};

// 1:4 bit translator for spi signal
// 1 : 1110 (0xe) , 0: 1000 (0x8)
//
static const uint32_t bitExpand[256] = {
    0x88888888, 0x8e888888, 0xe8888888, 0xee888888, 0x888e8888, 0x8e8e8888,
    0xe88e8888, 0xee8e8888, 0x88e88888, 0x8ee88888, 0xe8e88888, 0xeee88888,
    0x88ee8888, 0x8eee8888, 0xe8ee8888, 0xeeee8888, 0x88888e88, 0x8e888e88,
    0xe8888e88, 0xee888e88, 0x888e8e88, 0x8e8e8e88, 0xe88e8e88, 0xee8e8e88,
    0x88e88e88, 0x8ee88e88, 0xe8e88e88, 0xeee88e88, 0x88ee8e88, 0x8eee8e88,
    0xe8ee8e88, 0xeeee8e88, 0x8888e888, 0x8e88e888, 0xe888e888, 0xee88e888,
    0x888ee888, 0x8e8ee888, 0xe88ee888, 0xee8ee888, 0x88e8e888, 0x8ee8e888,
    0xe8e8e888, 0xeee8e888, 0x88eee888, 0x8eeee888, 0xe8eee888, 0xeeeee888,
    0x8888ee88, 0x8e88ee88, 0xe888ee88, 0xee88ee88, 0x888eee88, 0x8e8eee88,
    0xe88eee88, 0xee8eee88, 0x88e8ee88, 0x8ee8ee88, 0xe8e8ee88, 0xeee8ee88,
    0x88eeee88, 0x8eeeee88, 0xe8eeee88, 0xeeeeee88, 0x8888888e, 0x8e88888e,
    0xe888888e, 0xee88888e, 0x888e888e, 0x8e8e888e, 0xe88e888e, 0xee8e888e,
    0x88e8888e, 0x8ee8888e, 0xe8e8888e, 0xeee8888e, 0x88ee888e, 0x8eee888e,
    0xe8ee888e, 0xeeee888e, 0x88888e8e, 0x8e888e8e, 0xe8888e8e, 0xee888e8e,
    0x888e8e8e, 0x8e8e8e8e, 0xe88e8e8e, 0xee8e8e8e, 0x88e88e8e, 0x8ee88e8e,
    0xe8e88e8e, 0xeee88e8e, 0x88ee8e8e, 0x8eee8e8e, 0xe8ee8e8e, 0xeeee8e8e,
    0x8888e88e, 0x8e88e88e, 0xe888e88e, 0xee88e88e, 0x888ee88e, 0x8e8ee88e,
    0xe88ee88e, 0xee8ee88e, 0x88e8e88e, 0x8ee8e88e, 0xe8e8e88e, 0xeee8e88e,
    0x88eee88e, 0x8eeee88e, 0xe8eee88e, 0xeeeee88e, 0x8888ee8e, 0x8e88ee8e,
    0xe888ee8e, 0xee88ee8e, 0x888eee8e, 0x8e8eee8e, 0xe88eee8e, 0xee8eee8e,
    0x88e8ee8e, 0x8ee8ee8e, 0xe8e8ee8e, 0xeee8ee8e, 0x88eeee8e, 0x8eeeee8e,
    0xe8eeee8e, 0xeeeeee8e, 0x888888e8, 0x8e8888e8, 0xe88888e8, 0xee8888e8,
    0x888e88e8, 0x8e8e88e8, 0xe88e88e8, 0xee8e88e8, 0x88e888e8, 0x8ee888e8,
    0xe8e888e8, 0xeee888e8, 0x88ee88e8, 0x8eee88e8, 0xe8ee88e8, 0xeeee88e8,
    0x88888ee8, 0x8e888ee8, 0xe8888ee8, 0xee888ee8, 0x888e8ee8, 0x8e8e8ee8,
    0xe88e8ee8, 0xee8e8ee8, 0x88e88ee8, 0x8ee88ee8, 0xe8e88ee8, 0xeee88ee8,
    0x88ee8ee8, 0x8eee8ee8, 0xe8ee8ee8, 0xeeee8ee8, 0x8888e8e8, 0x8e88e8e8,
    0xe888e8e8, 0xee88e8e8, 0x888ee8e8, 0x8e8ee8e8, 0xe88ee8e8, 0xee8ee8e8,
    0x88e8e8e8, 0x8ee8e8e8, 0xe8e8e8e8, 0xeee8e8e8, 0x88eee8e8, 0x8eeee8e8,
    0xe8eee8e8, 0xeeeee8e8, 0x8888eee8, 0x8e88eee8, 0xe888eee8, 0xee88eee8,
    0x888eeee8, 0x8e8eeee8, 0xe88eeee8, 0xee8eeee8, 0x88e8eee8, 0x8ee8eee8,
    0xe8e8eee8, 0xeee8eee8, 0x88eeeee8, 0x8eeeeee8, 0xe8eeeee8, 0xeeeeeee8,
    0x888888ee, 0x8e8888ee, 0xe88888ee, 0xee8888ee, 0x888e88ee, 0x8e8e88ee,
    0xe88e88ee, 0xee8e88ee, 0x88e888ee, 0x8ee888ee, 0xe8e888ee, 0xeee888ee,
    0x88ee88ee, 0x8eee88ee, 0xe8ee88ee, 0xeeee88ee, 0x88888eee, 0x8e888eee,
    0xe8888eee, 0xee888eee, 0x888e8eee, 0x8e8e8eee, 0xe88e8eee, 0xee8e8eee,
    0x88e88eee, 0x8ee88eee, 0xe8e88eee, 0xeee88eee, 0x88ee8eee, 0x8eee8eee,
    0xe8ee8eee, 0xeeee8eee, 0x8888e8ee, 0x8e88e8ee, 0xe888e8ee, 0xee88e8ee,
    0x888ee8ee, 0x8e8ee8ee, 0xe88ee8ee, 0xee8ee8ee, 0x88e8e8ee, 0x8ee8e8ee,
    0xe8e8e8ee, 0xeee8e8ee, 0x88eee8ee, 0x8eeee8ee, 0xe8eee8ee, 0xeeeee8ee,
    0x8888eeee, 0x8e88eeee, 0xe888eeee, 0xee88eeee, 0x888eeeee, 0x8e8eeeee,
    0xe88eeeee, 0xee8eeeee, 0x88e8eeee, 0x8ee8eeee, 0xe8e8eeee, 0xeee8eeee,
    0x88eeeeee, 0x8eeeeeee, 0xe8eeeeee, 0xeeeeeeee,
};
#endif