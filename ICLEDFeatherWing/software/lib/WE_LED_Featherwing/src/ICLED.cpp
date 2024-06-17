/**
***************************************************************************************************
* This file is part of ICLED SDK:
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
* COPYRIGHT (c) 2024 Würth Elektronik eiSos GmbH & Co. KG
*
***************************************************************************************************
**/




#include "ICLED.h"

/**
 * @brief Constructor --> clears expanded LEDBuf
 *
 * @return None
 */
WLICLED::WLICLED(ColSystem ColSys) {
    // set color System to given Color system
    ColorSystem = ColSys;
    // clear Buffer and set all values to zero
    clearChar();
    // start serial communication
    SerialDebug = SSerial_create(&Serial);
    SSerial_begin(SerialDebug, 115200);

}

/**
 * @brief Deconstructor --> resets dma and spi settings
 *
 * @return None
 */
WLICLED::~WLICLED() {
    // Methode Source: v
    // ,with some changes
    dma.abort();
#ifdef SPI
    if (spi) {
        spi->endTransaction();
        if (spi != &SPI) delete spi;
    }
#endif
};







/**
 * @brief Changes Color mode to HSV, use methodes as usual with HSV Coordinates
 * --> Library calculates the right GRB color by itself
 *
 * @return None
 */
void WLICLED::to_HSV() { ColorSystem = HSV; }

/**
 * @brief Changes Color mode to GRB, use methodes with RGB Coordinates
 *
 * @return None
 */
void WLICLED::to_GRB() { ColorSystem = GRB; }



/**
 * @brief Initialise SPI and DMA transfer --> recommended for feather m0
 * (&express)
 *
 * @return true if succesfull
 */
bool WLICLED::begin(void) {
    // Methode Source: https://github.com/adafruit/Adafruit_NeoPixel_ZeroDMA,
    // with some changes
    return begin(&sercom5, SERCOM5, SERCOM5_DMAC_ID_TX, 6, SPI_PAD_2_SCK_3,
                 PIO_SERCOM);  // Feather m0 express pin 6 by default
    return false;  // something went wrong if executing this, return false
}

// do only use this, if you know, what you are doing!
/*

Source: https://github.com/adafruit/Adafruit_NeoPixel_ZeroDMA


Format: sercom   base     dmacID              mosi  padTX            pinFunc
ADAFRUIT_FEATHER_M0
    Serial1 (TX/RX) is on SERCOM0, do not use
    SERCOM1,2 are 100% in the clear
    I2C is on SERCOM3, do not use
    SPI is on SERCOM4, but OK to use (as SPI MOSI)
    Serial5 is on SERCOM5, but OK to use (Arduino core detritus)
    &sercom1, SERCOM1, SERCOM1_DMAC_ID_TX,   12, SPI_PAD_3_SCK_1, PIO_SERCOM,
    &sercom2, SERCOM2, SERCOM2_DMAC_ID_TX,    5, SPI_PAD_3_SCK_1, PIO_SERCOM,
    &sercom4, SERCOM4, SERCOM4_DMAC_ID_TX, MOSI, SPI_PAD_2_SCK_3,
PIO_SERCOM_ALT, &sercom5, SERCOM5, SERCOM5_DMAC_ID_TX,    6, SPI_PAD_2_SCK_3,
PIO_SERCOM,

ADAFRUIT_FEATHER_M0_EXPRESS
    Serial1 (TX/RX) is on SERCOM0, do not use
    SERCOM1,5 are 100% in the clear
    SPI FLASH is on SERCOM2, do not use
    I2C is on SERCOM3, do not use
    SPI is on SERCOM4, but OK to use (as SPI MOSI)
    &sercom1, SERCOM1, SERCOM1_DMAC_ID_TX, 12, SPI_PAD_3_SCK_1, PIO_SERCOM,
    &sercom4, SERCOM4, SERCOM4_DMAC_ID_TX, MOSI, SPI_PAD_2_SCK_3,
PIO_SERCOM_ALT, &sercom5, SERCOM5, SERCOM5_DMAC_ID_TX, 6, SPI_PAD_2_SCK_3,
PIO_SERCOM,


ADAFRUIT_FEATHER_M4_EXPRESS) --> Not recommended, some changes in library
necessary for SAMD51 MCU! SERCOM0,3,4 are 100% clear to use SPI is on SERCOM1,
but OK to use (as SPI MOSI) I2C is on SERCOM2, do not use Serial1 (TX/RX) is on
SERCOM5, do not use Feather M4 uses QSPI flash, not on a SERCOM &sercom0,
SERCOM0, SERCOM0_DMAC_ID_TX,   A4, SPI_PAD_0_SCK_1, PIO_SERCOM_ALT, &sercom1,
SERCOM1, SERCOM1_DMAC_ID_TX, MOSI, SPI_PAD_3_SCK_1, PIO_SERCOM, &sercom3,
SERCOM3, SERCOM3_DMAC_ID_TX,   12, SPI_PAD_0_SCK_1, PIO_SERCOM, &sercom4,
SERCOM4, SERCOM4_DMAC_ID_TX,   A2, SPI_PAD_0_SCK_1, PIO_SERCOM_ALT,
*/
/**
 * @brief Initialise SPI and DMA transfer, do only use if you know what you are
 * doing!
 *
 * @param[in] Sercom: sercom to use
 * @param[in] Sercom: sercom Base
 * @param[in] dmacID: lID of DMAC
 * @param[in] mosi: mosi pin
 * @param[in] padTX: pinmux set up for SPI SERCOM pin config
 * @param[in] pinFunc: pinmux setup for 'type' of pinmux
 *
 * @return true if succesfull
 */
bool WLICLED::begin(SERCOM *sercom, Sercom *sercomBase, uint8_t dmacID,
                    uint8_t mosi, SercomSpiTXPad padTX, EPioType pinFunc) {

    // Methode Source: https://github.com/adafruit/Adafruit_NeoPixel_ZeroDMA ,
    // with some changes
    int pin = 6;
    if (mosi != pin) return false;  // Invalid pin

    // DMA buffer is 4X the LEDBuf buffer size.  Each bit is expanded
    // 4:1 to allow use of SPI peripheral to generate right timing
    // (0b1000 for a zero bit, 0b1110 for a one bit).  SPI is clocked at
    // 3.2 MHz, the 4:1 sizing then creates 800 KHz bitrate.
    // The extra 120 bytes is the low-level latch at the end of the
    // data stream.  When idle, SPI logic level is normally HIGH, LOW is needed
    // for latch.  There is no invert option.  Various tricks like
    // switching the pin to a normal LOW output at end of data don't quite
    // work, there's still small glitches.  So, solution here is to keep
    // the SPI DMA transfer in an endless loop...it actually issues the
    // data over and over again forever (this doesn't cost us
    // anything, since it's 100% DMA, no CPU use)...and those 120 zero
    // bytes at the end provide the 280 microsecond EOD latch.

    spi = NULL;  // No SPIClass assigned yet,
// check MOSI pin against existing defined SPI SERCOMs...
#if SPI_INTERFACES_COUNT > 0
    if (pin == PIN_SPI_MOSI) {  // If DI pin is main SPI MOSI...
        spi = &SPI;             // Use the existing SPIClass object
        padTX = PAD_SPI_TX;
    }
#endif
#if SPI_INTERFACES_COUNT > 1
    else if (pin == PIN_SPI1_MOSI) {  // If DI pin = secondary SPI MOSI...
        spi = &SPI1;                  // Use the SPI1 SPIClass object
        padTX = PAD_SPI1_TX;
    }
#endif
#if SPI_INTERFACES_COUNT > 2
    else if (pin == PIN_SPI2_MOSI) {  // Ditto, tertiary SPI
        spi = &SPI2;
        padTX = PAD_SPI2_TX;
    }
#endif
#if SPI_INTERFACES_COUNT > 3
    else if (pin == PIN_SPI3_MOSI) {
        spi = &SPI3;
        padTX = PAD_SPI3_TX;
    }
#endif
#if SPI_INTERFACES_COUNT > 4
    else if (pin == PIN_SPI4_MOSI) {
        spi = &SPI4;
        padTX = PAD_SPI4_TX;
    }
#endif
#if SPI_INTERFACES_COUNT > 5
    else if (pin == PIN_SPI5_MOSI) {
        spi = &SPI5;
        padTX = PAD_SPI5_TX;
    }
#endif
    // If DI pin is not an existing SPI SERCOM, allocate a new one.
    if (spi == NULL) {
        /* DIRTY POOL! The SPIClass constructor expects MISO, SCK and MOSI
        pins, in that order. Our library only intends to ever use the MOSI
        output, the others are never even set to SERCOM periph functions.
        We just give the SPI constructor THE SAME PIN NUMBER for all three.
        The SPI lib never checks if they're distinct and valid for each of
        the three. It does set pinPeripheral for each (or in this case,
        the same for the MOSI pin three times)...but no matter, we set our
        own pinPeripheral below. The SPI RX PAD also doesn't matter...we
        always claim it's PAD 1 here, because (by hardware design) the TX
        pad will always be 0, 2 or 3...this might collide with the SCK PAD
        value, but we don't care, neither SCK nor MISO is actually used.
        (This is tested across many SAMD devices and works, but it's
        conceivable that this could fail spectacularly on some unforseen
        future device, if the SERCOM pad assignment becomes hardwarily
        strict.)*/
        spi = new SPIClass(sercom, mosi, mosi, mosi, padTX, SERCOM_RX_PAD_1);
    }
    if (spi) {
        spi->begin();
        pinPeripheral(mosi, pinFunc);
        dma.setTrigger(dmacID);
        dma.setAction(DMA_TRIGGER_ACTON_BEAT);
        if (DMA_STATUS_OK == dma.allocate()) {
            if (dma.addDescriptor(
                    dmaBuf,                               // source adress
                    (void *)(&sercomBase->SPI.DATA.reg),  // target adress
                    BYTESTOTAL,                           // this many...
                    DMA_BEAT_SIZE_BYTE,                   // bytes/hword/words
                    true,      // increment source addr?
                    false)) {  // increment dest addr?
                dma.loop(
                    true);  // DMA transaction loops forever! Latch is built in.
                            // SPI transaction is started BUT NEVER ENDS.  This
                            // is important. 800 khz * 4 = 3.2MHz
                spi->beginTransaction(
                    SPISettings(3200000, MSBFIRST, SPI_MODE0));
                if (DMA_STATUS_OK == dma.startJob()) return true;  // SUCCESS
                // Else various errors, clean up partially-initialized stuff:
                spi->endTransaction();
            }
            dma.free();
        }
        // Delete SPIClass object, UNLESS it's an existing (Arduino-defined) one
#if SPI_INTERFACES_COUNT > 0
        if (spi == &SPI) {
            spi = NULL;
        }
#endif
#if SPI_INTERFACES_COUNT > 1
        else if (spi == &SPI1) {
            spi = NULL;
        }
#endif
#if SPI_INTERFACES_COUNT > 2
        else if (spi == &SPI2) {
            spi = NULL;
        }
#endif
#if SPI_INTERFACES_COUNT > 3
        else if (spi == &SPI3) {
            spi = NULL;
        }
#endif
#if SPI_INTERFACES_COUNT > 4
        else if (spi == &SPI4) {
            spi = NULL;
        }
#endif
#if SPI_INTERFACES_COUNT > 5
        else if (spi == &SPI5) {
            spi = NULL;
        }
#endif

#ifdef SPI
        if (spi != NULL) {
            delete spi;
            spi = NULL;
        }
#endif
    }
    return true;
}

/**
 * @brief Set a pixel to given color with given brightness
 *
 * @param[in] k: pixel number
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 * @param[in] Bright:  Brightness
 *
 * @return None
 */
void WLICLED::set_pixel(uint16_t pixel, uint16_t G_H, uint8_t R_S, uint8_t B_V,
                        uint8_t Bright) {
    // Check, if parameters are ok & write LEDBuf
    if (pixel < LEDCOUNT) {
        uint8_t G = 0, R = 0, B = 0;
        switch (ColorSystem) {
            case GRB: {
                if (G_H > 255) return;
                // LEDBuf[pixel].G = calc((uint8_t)G_H, Bright);
                // LEDBuf[pixel].R = calc(R_S, Bright);
                // LEDBuf[pixel].B = calc(B_V, Bright);
                G = calc((uint8_t)G_H, Bright);
                R = calc(R_S, Bright);
                B = calc(B_V, Bright);
                Temperature_Safety(G, R, B, Bright, G_H, R_S,
                                   B_V);  // checks PWM-levels for safety
                LEDBuf[pixel].G = G;
                LEDBuf[pixel].R = R;
                LEDBuf[pixel].B = B;
                return;
            }
            case HSV: {
                // use HSV-methode if color System is HSV
                set_pixelHSV(pixel, G_H, R_S, B_V, G, R, B, Bright);
                Temperature_Safety(G, R, B, Bright, G_H, R_S,
                                   B_V);  // checks PWM-levels for safety
                LEDBuf[pixel].G = G;
                LEDBuf[pixel].R = R;
                LEDBuf[pixel].B = B;
                return;
            }
            default:
                // colSys == Invalid
                return;
        }
    }
}

/**
 * @brief Sets a pixel with HSV coordinates in expanded LEDBuf
 *
 * @param[in] column: pixel column
 * @param[in] row: pixel row
 * @param[in] H: H-coordinate of color/2
 * @param[in] S: S-coordinate of color
 * @param[in] V: V-coordinate of color
 * @param[in] G: Pionter to Green color coordinate for Temperature_Safety Method
 * @param[in] R: Pionter to Red color coordinate for Temperature_Safety Method
 * @param[in] B: Pionter to Blue color coordinate for Temperature_Safety Method
 * @param[in] Bright: Pionter to Brightness for Temperature_Safety Method
 *
 * @return None
 */
void WLICLED::set_pixelHSV(uint16_t pixel, uint16_t H, uint8_t S, uint8_t V,
                           uint8_t &G, uint8_t &R, uint8_t &B,
                           uint8_t &Bright) {
    if (H > 360 || S > 100 || V > 100) return;
    // nominate s & v to 1
    double s = (double)S / 100;
    double v = (double)V / 100;
    double p, q, t, f = 0;
    // uint8_t G, R, B = 0;
    p = V * (1 - s);
    f = (double)H / 60;
    // set Brightnes for Temperature_safety method
    Bright = (uint8_t)(((uint16_t)V / 100) * 255);

    // check angle range and set R,G,B in dependency of it
    if (H < 60) {
        t = V * (1 - s * (1 - f));
        R = V;
        G = (uint8_t)(t);
        B = (uint8_t)(p);
    } else if (H < 120) {
        f -= 1;
        q = V * (1 - s * f);
        R = (uint8_t)(q);
        G = V;
        B = (uint8_t)(p);
    } else if (H < 180) {
        f -= 2;
        t = V * (1 - s * (1 - f));
        R = (uint8_t)(p);
        G = V;
        B = (uint8_t)(t);
    } else if (H < 240) {
        f -= 3;
        q = V * (1 - s * f);
        R = (uint8_t)(p);
        G = (uint8_t)(q);
        B = V;
    } else if (H < 300) {
        f -= 4;
        t = V * (1 - s * (1 - f));
        R = (uint8_t)(t);
        G = (uint8_t)(p);
        B = V;
    } else if (H < 360) {
        f -= 5;
        q = V * (1 - s * f);
        R = V;
        G = (uint8_t)(p);
        B = (uint8_t)(q);
    }

    
    // write G,R,B in LEDBuf
    LEDBuf[pixel].G = G;
    LEDBuf[pixel].R = R;
    LEDBuf[pixel].B = B;
}

/**
 * @brief Calculates coordinates for brightness
 *
 * @param[in] color: color coordinate
 * @param[in] Bright:  Brightness
 *
 * @return None
 */
inline uint8_t WLICLED::calc(uint8_t color, uint8_t Bright) {
    //(coordinate/255) *Brightness
    return (uint8_t)(((uint16_t)color * (uint16_t)Bright) / ((uint16_t)255));
}

/**
 * @brief Prevents Board from overheating! Please do only change, if you know,
 * what you are doing! Sets max PWM-level for all colors summed up to max 210
 * per LED. If this is commented out, board is allowed to light brighter. In
 * this case board could heat up uncontrolled. It could be very dangerous, be
 * careful with this!
 *
 * @param[in] G: Pionter to Green color coordinate
 * @param[in] R: Pionter to Red color coordinate
 * @param[in] B: Pionter to Blue color coordinate
 * @param[in] Bright:  Brightness
 *
 * @return None
 */
void WLICLED::Temperature_Safety(uint8_t &G, uint8_t &R, uint8_t &B,
                                 uint8_t Bright, uint8_t G_H, uint8_t R_S,
                                 uint8_t B_V) {
    uint16_t PWM = G + R + B;
    if (PWM > MAXBRIGHTNESS) {
        // calculate difference to dim down LEDs
        uint16_t dif = (uint16_t)((PWM - (uint16_t)MAXBRIGHTNESS) / 3);
        Bright = Bright - dif;
        G = calc(G_H, Bright - 2);
        R = calc(R_S, Bright - 2);
        B = calc(B_V, Bright - 2);
        // SSerial_printf(SerialDebug, "B: %d", B);//debugging
    }
    // check again
    // if only 2 colors used, division by 3 results in wrong levels
    PWM = G + R + B;
    if (PWM > MAXBRIGHTNESS) {
        // calculate difference to dim down LEDs
        uint16_t dif = (uint16_t)((PWM - (uint16_t)MAXBRIGHTNESS) / 2);
        Bright = Bright - dif;
        G = calc(G_H, Bright - 2);
        R = calc(R_S, Bright - 2);
        B = calc(B_V, Bright - 2);
        // SSerial_printf(SerialDebug, "B: %d", B);//debugging
    } else
        return;
    // and again
    PWM = G + R + B;
    if (PWM > MAXBRIGHTNESS) {
        // calculate difference to dim down LEDs
        uint16_t dif = (uint16_t)(PWM - (uint16_t)MAXBRIGHTNESS);
        Bright = Bright - dif;
        G = calc(G_H, Bright - 2);
        R = calc(R_S, Bright - 2);
        B = calc(B_V, Bright - 2);
        // SSerial_printf(SerialDebug, "B: %d", B);//debugging
    }
}

/**
 * @brief Set a pixel to given color by coordinates on screen
 *
 * @param[in] column: pixel column
 * @param[in] row: pixel row
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 * @param[in] Bright:  Brightness
 *
 * @return None
 */
void WLICLED::set_screenPixel(uint8_t column, uint8_t row, uint16_t G_H,
                              uint8_t R_S, uint8_t B_V, uint8_t Bright) {
    if (column < COLUMNS && row < ROWS) {
        // calculating LED-number n
        uint16_t n = column * ROWS + row;
        // set corresponding pixel
        set_pixel(n, G_H, R_S, B_V, Bright);
    }
}

/**
 * @brief Set all pixels on screen to given color and brightnes
 *
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 * @param[in] Bright:  Brightness
 *
 * @return None
 */
void WLICLED::set_all(uint16_t G_H, uint8_t R_S, uint8_t B_V, uint8_t Bright) {
    for (int i = 0; i < LEDCOUNT; i++) set_pixel(i, G_H, R_S, B_V, Bright);
}

/**
 * @brief Starts transfering data to LEDs
 *
 * @param[in] del: delay time in ms --> usefull for animations, slows it a bit
 * down (should be at least 5ms)
 *
 * @return None
 */
void WLICLED::show(uint32_t del) {
    if (del < 5) del = 5;
    if (!dmaTrans) {
        // start DMA transaction
        dma.startJob();
        dma.trigger();
        dmaTrans = true;
    }
    while (del > 0) {
        // check for orientation
        if (interrupt == true) return;  // return if interrupt
        uint32_t *out = dmaBuf;
        if (turned == false) {
            for (int i = 0; i < LEDCOUNT; i++) {
                // write data from LEDBuf to dmaBuf and expand bits 4:1 for SPI
                // sequence
                *out++ = bitExpand[LEDBuf[i].G];
                *out++ = bitExpand[LEDBuf[i].R];
                *out++ = bitExpand[LEDBuf[i].B];
            }
        } else {
            for (int i = LEDCOUNT - 1; i >= 0; i--) {
                // write data from LEDBuf to dmaBuf and expand bits 4:1 for SPI
                // sequence mirror data, PCB is turned
                *out++ = bitExpand[LEDBuf[i].G];
                *out++ = bitExpand[LEDBuf[i].R];
                *out++ = bitExpand[LEDBuf[i].B];
            }
        }
        // optional delay to slow animations a bit down, default 0
        // repeats only same DMA transmission without changes for del ms
        // 10ms steps to save MCU from executing all commands above too often
        if (del >= 10) {
            delay(10);
            del -= 10;
        } else {
            delay(del);
            del = 0;
        }
    }
}

/**
 * @brief Starts transfering data to pixels on screen & delays until interrupt
 *
 * @param[in] del: delay time in ms between 2 interrupt checks
 *
 * @return None
 */
void WLICLED::showLoop(uint32_t del) {
    // interrupt = false;
    if (!dmaTrans) {
        // start DMA transaction
        dma.startJob();
        dma.trigger();
        dmaTrans = true;
    }
    while (interrupt == false) {
        uint32_t *out = dmaBuf;
        if (turned == false) {
            for (int i = 0; i < LEDCOUNT; i++) {
                // write data from LEDBuf to dmaBuf and expand bits 4:1 for SPI
                // sequence
                *out++ = bitExpand[LEDBuf[i].G];
                *out++ = bitExpand[LEDBuf[i].R];
                *out++ = bitExpand[LEDBuf[i].B];
            }
        } else {
            for (int i = LEDCOUNT - 1; i >= 0; i--) {
                // write data from LEDBuf to dmaBuf and expand bits 4:1 for SPI
                // sequence
                // mirrored data, PCB turned
                *out++ = bitExpand[LEDBuf[i].G];
                *out++ = bitExpand[LEDBuf[i].R];
                *out++ = bitExpand[LEDBuf[i].B];
            }
        }

        delay(del);  // time between 2 interrupt checks --> possible delay
                     // before reaction on click or orientation change
    }
    // reset interrupt bool
    interrupt = false;
    // clear pixel buffer
    clear();
}

/**
 * @brief Clears LEDBuf
 *
 * @return None
 */
void WLICLED::clear() {
    // set all pixel color coordinates to zero
    for (int i = 0; i < LEDCOUNT; i++) {
        LEDBuf[i].G = 0;
        LEDBuf[i].R = 0;
        LEDBuf[i].B = 0;
    }
}

/**
 * @brief Initialising and test program --> similar to a hello world program
 *
 * @return None
 */
void WLICLED::init_show() {
 

//Original function from Christohp Lehnert
    // check for color system and set it to GRB
    // here are some hard coded colors used
    bool hsv = false;
    if (ColorSystem == HSV) {
        hsv = true;
        ColorSystem = GRB;
    }
    interrupt = false;
    for (int i = 0; i < ROWS; i++) {
        for (int k = 0; k < COLUMNS; k++) {
            clear();
            set_screenPixel(k, i, 128, 128, 128);
            show(80);  //~0.1 second delay time
                        // abort on interrupt
            if (interrupt == true) {
                interrupt = false;
                clearChar();
                if (hsv == true) ColorSystem = HSV;
                return;
            }
        }
    }
    clear();  // clear Buffer
    // white light, increase brightness every 20ms
    for (int i = 0; i < 25; i++) {
        set_all(255, 255, 255, i);
        show(40);
        if (interrupt == true) {
            interrupt = false;
            clearChar();
            if (hsv == true) ColorSystem = HSV;
            return;
        }
    }
    show(500);
    clear();
    uint16_t x = 0;
    fill(0, 60, 0, 55);     // fill background red
    x = set_WElogo(x, 10);  // set a WE logo on screen
    x = set_string("More than you expect!", x, 60, 60, 60,
                   50);  // add text behind logo
    show(2000);           // show only logo
    showRun(90,x,1);
    if (interrupt == true) {
        clearChar();
        if (hsv == true) ColorSystem = HSV;
        interrupt = false;
        return;
    }
    fill(0, 60, 0, 55);  // fill background red
    x = set_string("More than you expect!", COLUMNS - 1, 60, 60, 60,
                   50);  // set string in buffer
    showRunLoop(90, x);  // start transmission in loop until interrupt
    interrupt = false;   // reset interrupt
    clearChar();         // clear buffer
    // set color system back to hsv if necessary
    if (hsv == true) ColorSystem = HSV;
}



/*
**  Extra function for commitment meeting for automatical display of sequences &
*effects without using the push button to change states
**  Written by: Carlos R. Heranandez Gomez
*/

void WLICLED::init_concern() {
   // check for color system and set it to GRB
    // here are some hard coded colors used
    bool hsv = false;
    if (ColorSystem == HSV) {
        hsv = true;
        ColorSystem = GRB;
    }
    interrupt = false;
    
/*
**********************************************************************************************************************
WE Logo
**********************************************************************************************************************
*/
    clear();
    uint16_t x = 0;
    for(int i = 0; i <= 10; i++){
        fill(0, 255, 0, i);     // fill background red
        show(50);
    }
    show(100);
    for(int i = 0; i <= 10; i++){
        set_WElogo(x, i);      //Show the WE logo slowly
        show(50);
    }
    x = set_WElogo(x, 10);  // set a WE logo on screen
    show(1500);
    x = set_string("MORE THAN YOU EXPECT!", x + 5, 60, 60, 60,
                   50);  // add text behind logo
    show(2000);          // show only logo
    showRun(125, x, 1);

/*
**********************************************************************************************************************
Würth Group logo
**********************************************************************************************************************
*/
    clearChar();
    // Creation of the Würth Group logo
    for (int i = 0; i < 9; i++) {
        switch (i) {
            case 0:
                for (int k = 0; k <= 10; k++) {
                    set_charPixel(4, i, 0, 255, 0, k);
                    set_charPixel(5, i, 0, 255, 0, k);
                    set_charPixel(6, i, 0, 255, 0, k);
                    set_charPixel(8, i, 0, 255, 0, k);
                    set_charPixel(9, i, 0, 255, 0, k);
                    set_charPixel(10, i, 0, 255, 0, k);
                    show(20);
                }
                break;
            case 1:
                for (int k = 0; k <= 10; k++) {
                    set_charPixel(4, i, 0, 255, 0, k);
                    set_charPixel(5, i, 0, 255, 0, k);
                    set_charPixel(6, i, 0, 255, 0, k);
                    set_charPixel(8, i, 0, 255, 0, k);
                    set_charPixel(9, i, 0, 255, 0, k);
                    set_charPixel(10, i, 0, 255, 0, k);
                    show(20);
                }
                break;

            case 2:
                for (int k = 0; k <= 10; k++) {
                    set_charPixel(4, i, 0, 255, 0, k);
                    set_charPixel(5, i, 0, 255, 0, k);
                    set_charPixel(6, i, 0, 255, 0, k);
                    set_charPixel(7, i, 0, 255, 0, k);
                    set_charPixel(8, i, 0, 255, 0, k);
                    set_charPixel(9, i, 0, 255, 0, k);
                    set_charPixel(10, i, 0, 255, 0, k);
                    show(20);
                }

            case 3:
                
                break;

            case 4:
                for (int k = 0; k <= 10; k++) {
                    set_charPixel(4, i, 0, 255, 0, k);
                    set_charPixel(5, i, 0, 255, 0, k);
                    set_charPixel(6, i, 0, 255, 0, k);
                    set_charPixel(7, i, 0, 255, 0, k);
                    set_charPixel(8, i, 0, 255, 0, k);
                    set_charPixel(9, i, 0, 255, 0, k);
                    set_charPixel(10, i, 0, 255, 0, k);
                    show(20);
                }
                break;

            case 5:
                for (int k = 0; k <= 10; k++) {
                    set_charPixel(5, i, 0, 255, 0, k);
                    set_charPixel(6, i, 0, 255, 0, k);
                    set_charPixel(7, i, 0, 255, 0, k);
                    set_charPixel(8, i, 0, 255, 0, k);
                    set_charPixel(9, i, 0, 255, 0, k);
                    show(20);
                }
                break;

            case 6:
                for (int k = 0; k <= 10; k++) {
                    set_charPixel(6, i, 0, 255, 0, k);
                    set_charPixel(8, i, 0, 255, 0, k);
                    show(20);
                }
                show(1500);
                break;

            case 7:
                for (int k = 0; k < 10; k++) {
                    set_charPixel(0, 0, 200, 200, 200, k);
                    set_charPixel(1, 0, 200, 200, 200, k);
                    set_charPixel(2, 0, 200, 200, 200, k);
                    set_charPixel(3, 0, 200, 200, 200, k);
                    set_charPixel(7, 0, 200, 200, 200, k);
                    set_charPixel(11, 0, 200, 200, 200, k);
                    set_charPixel(12, 0, 200, 200, 200, k);
                    set_charPixel(13, 0, 200, 200, 200, k);
                    set_charPixel(14, 0, 200, 200, 200, k);

                    set_charPixel(0, 1, 200, 200, 200, k);
                    set_charPixel(1, 1, 200, 200, 200, k);
                    set_charPixel(2, 1, 200, 200, 200, k);
                    set_charPixel(3, 1, 200, 200, 200, k);
                    set_charPixel(7, 1, 200, 200, 200, k);
                    set_charPixel(11, 1, 200, 200, 200, k);
                    set_charPixel(12, 1, 200, 200, 200, k);
                    set_charPixel(13, 1, 200, 200, 200, k);
                    set_charPixel(14, 1, 200, 200, 200, k);

                    set_charPixel(0, 2, 200, 200, 200, k);
                    set_charPixel(1, 2, 200, 200, 200, k);
                    set_charPixel(2, 2, 200, 200, 200, k);
                    set_charPixel(3, 2, 200, 200, 200, k);
                    set_charPixel(11, 2, 200, 200, 200, k);
                    set_charPixel(12, 2, 200, 200, 200, k);
                    set_charPixel(13, 2, 200, 200, 200, k);
                    set_charPixel(14, 2, 200, 200, 200, k);

                    set_charPixel(0, 3, 200, 200, 200, k);
                    set_charPixel(1, 3, 200, 200, 200, k);
                    set_charPixel(2, 3, 200, 200, 200, k);
                    set_charPixel(3, 3, 200, 200, 200, k);
                    set_charPixel(4, 3, 200, 200, 200, k);
                    set_charPixel(5, 3, 200, 200, 200, k);
                    set_charPixel(6, 3, 200, 200, 200, k);
                    set_charPixel(7, 3, 200, 200, 200, k);
                    set_charPixel(8, 3, 200, 200, 200, k);
                    set_charPixel(9, 3, 200, 200, 200, k);
                    set_charPixel(10, 3, 200, 200, 200, k);
                    set_charPixel(11, 3, 200, 200, 200, k);
                    set_charPixel(12, 3, 200, 200, 200, k);
                    set_charPixel(13, 3, 200, 200, 200, k);
                    set_charPixel(14, 3, 200, 200, 200, k);

                    set_charPixel(0, 4, 200, 200, 200, k);
                    set_charPixel(1, 4, 200, 200, 200, k);
                    set_charPixel(2, 4, 200, 200, 200, k);
                    set_charPixel(3, 4, 200, 200, 200, k);
                    set_charPixel(11, 4, 200, 200, 200, k);
                    set_charPixel(12, 4, 200, 200, 200, k);
                    set_charPixel(13, 4, 200, 200, 200, k);
                    set_charPixel(14, 4, 200, 200, 200, k);

                    set_charPixel(0, 5, 200, 200, 200, k);
                    set_charPixel(1, 5, 200, 200, 200, k);
                    set_charPixel(2, 5, 200, 200, 200, k);
                    set_charPixel(3, 5, 200, 200, 200, k);
                    set_charPixel(4, 5, 200, 200, 200, k);
                    set_charPixel(10, 5, 200, 200, 200, k);
                    set_charPixel(11, 5, 200, 200, 200, k);
                    set_charPixel(12, 5, 200, 200, 200, k);
                    set_charPixel(13, 5, 200, 200, 200, k);
                    set_charPixel(14, 5, 200, 200, 200, k);

                    set_charPixel(0, 6, 200, 200, 200, k);
                    set_charPixel(1, 6, 200, 200, 200, k);
                    set_charPixel(2, 6, 200, 200, 200, k);
                    set_charPixel(3, 6, 200, 200, 200, k);
                    set_charPixel(4, 6, 200, 200, 200, k);
                    set_charPixel(5, 6, 200, 200, 200, k);
                    set_charPixel(7, 6, 200, 200, 200, k);
                    set_charPixel(9, 6, 200, 200, 200, k);
                    set_charPixel(10, 6, 200, 200, 200, k);
                    set_charPixel(11, 6, 200, 200, 200, k);
                    set_charPixel(12, 6, 200, 200, 200, k);
                    set_charPixel(13, 6, 200, 200, 200, k);
                    set_charPixel(14, 6, 200, 200, 200, k);
                    show(20);
                }
                show(3500);
                break;
            default:
                break;
        }
    }
    x = 0;
    x = set_Wuerthlogo(x, 10);
    x = set_string("WVRTH GROUP", x , 60, 60, 60,
                   50);  // add text behind logo
                         // show only logo
    showRun(125, x, 1);
    clearChar();
    /*
    **********************************************************************************************************************
    Rainbow
    **********************************************************************************************************************
    */
    uint8_t step =
        (uint8_t)(255 / (uint16_t)COLUMNS);  // fade frome one base color to an
                                             // other in one screen width
    if (step == 0) step++;                   // display wider than 255 pixels
        for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k, i, 0, 0, 0,0);
        }
    }
    
    // Red --> green
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + COLUMNS, i, step * k, 255 - (step * k), 0, 5);
        }
    }
    // green --> blue
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + (2 * COLUMNS), i, 255 - (step * k), 0, step * k,
                          5);
        }
    }
    // blue --> red
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + (3 * COLUMNS), i, 0, step * k, 255 - (step * k),
                          5);
        }
    }
    // red --> green again for scmooth change on restart
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + (4 * COLUMNS), i, step * k, 255 - (step * k), 0,
                          5);
        }
    }
    // green --> blue
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + (5* COLUMNS), i, 255 - (step * k), 0, step * k,
                          5);
        }
    }
    // blue --> red
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + (6 * COLUMNS), i, 0, step * k, 255 - (step * k),
                          5);
        }
    }
    // red --> green again for scmooth change on restart
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + (7 * COLUMNS), i, step * k, 255 - (step * k), 0,
                          5);
        }
    }
    // green --> blue
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + (8* COLUMNS), i, 255 - (step * k), 0, step * k,
                          5);
        }
    }
    // blue --> red
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + (9 * COLUMNS), i, 0, step * k, 255 - (step * k),
                          5);
        }
    }
    x = 0;
    set_string("COMMITMENT 2023", x+30, 60, 60, 60,
                   50);  // add text behind logo
    showRun(125, (10 * COLUMNS), 1);  // stop 1 screen earlier to get a smooth change
    // reset color system and interrupt bool
    fill(0,0,0,0);
    show(500);
    interrupt = false;
    if (hsv == true) ColorSystem = HSV;
    // clear expanded buffer
    clearChar();
    
}

/**
 * @brief Initialising and test program --> similar to a hello world program
 *
 * @return None
 */
void WLICLED::init_show_light() {
    // check for color system and set it to GRB
    // here are some hard coded colors used
    bool hsv = false;
    if (ColorSystem == HSV) {
        hsv = true;
        ColorSystem = GRB;
    }
    interrupt = false;
    uint16_t x = 0;
    fill(0, 60, 0, 55);     // fill background red
    x = set_WElogo(x, 10);  // set a WE logo on screen
    x = set_string("More than you expect!", x, 60, 60, 60,
                   50);  // add text behind logo
    show(500);           // show only logo
    if (interrupt == true) {
        clearChar();
        if (hsv == true) ColorSystem = HSV;
        interrupt = false;
        return;
    }
    show(500);  // show only logo
    if (interrupt == true) {
        clearChar();
        if (hsv == true) ColorSystem = HSV;
        interrupt = false;
        return;
    }
    show(500);  // show only logo
    if (interrupt == true) {
        clearChar();
        if (hsv == true) ColorSystem = HSV;
        interrupt = false;
        return;
    }
    showRun(75, x, 1);  // move text 1 times
    clearChar();        // clear buffer
    if (interrupt == true) {
        interrupt = false;
        if (hsv == true) ColorSystem = HSV;
        return;
    }
    fill(0, 60, 0, 55);  // fill background red
    x = set_string("More than you expect!", COLUMNS - 1, 60, 60, 60,
                   50);  // set string in buffer
    showRunLoop(75, x);  // start transmission in loop until interrupt
    interrupt = false;   // reset interrupt
    clearChar();         // clear buffer
    // set color system back to hsv if necessary
    if (hsv == true) ColorSystem = HSV;
}

/**
 * @brief Set a pixel to given color by coordinates on expanded screen -->
 * similar to set_pixel but for expanded screen and scrolling text
 *
 * @param[in] column: pixel column
 * @param[in] row: pixel row
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 * @param[in] Bright:  Brightness
 *
 * @return None
 */
void WLICLED::set_charPixel(uint16_t column, uint16_t row, uint16_t G_H,
                            uint8_t R_S, uint8_t B_V, uint8_t Bright) {
    // check if in bounds
    if (column < COLUMNS * SCREENSTORUN && row < ROWS) {
        // calculating LED-number n
        uint16_t k = column * ROWS + row;
        uint8_t G = 0, R = 0, B = 0;
        switch (ColorSystem) {
            case GRB: {
                if (G_H > 255) return;
                // LEDBuf[k].G = calc((uint8_t)G_H, Bright);
                // LEDBuf[k].R = calc(R_S, Bright);
                // LEDBuf[k].B = calc(B_V, Bright);
                G = calc((uint8_t)G_H, Bright);
                R = calc(R_S, Bright);
                B = calc(B_V, Bright);
                Temperature_Safety(G, R, B, Bright, G_H, R_S, B_V);
                LEDBuf[k].G = G;
                LEDBuf[k].R = R;
                LEDBuf[k].B = B;
                return;
            }
            case HSV: {
                set_pixelHSV(k, G_H, R_S, B_V, G, R, B, Bright);
                LEDBuf[k].G = G;
                LEDBuf[k].R = R;
                LEDBuf[k].B = B;
                return;
            }
            default:
                // ColSys == invalid
                return;
        }
    }
}

/**
 * @brief Clears expanded LEDBuf
 *
 * @return None
 */
void WLICLED::clearChar() {
    // set all color coordinates in expanded buffer to zero
    for (uint32_t i = 0; i < LEDCOUNT * SCREENSTORUN; i++) {
        LEDBuf[i].G = 0;
        LEDBuf[i].R = 0;
        LEDBuf[i].B = 0;
    }
}

/**
 * @brief Fills expanded LEDBuf with given color and brightness -->
 * useful to fill text background
 *
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 * @param[in] Bright:  Brightness
 *
 * @return None
 */
void WLICLED::fill(uint16_t G_H, uint8_t R_S, uint8_t B_V, uint8_t Bright) {
    // set all color coordinates in expanded buffer to given values
    for (uint16_t i = 0; i < COLUMNS * SCREENSTORUN; i++) {
        for (uint16_t k = 0; k < ROWS; k++) {
            set_charPixel(i, k, G_H, R_S, B_V, Bright);
        }
    }
}

/**
 * @brief writes a whole string in expanded LEDBuf
 *
 * @param[in] c: string to set in buffer
 * @param[in] place: left boundry of string
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 * @param[in] Bright:  Brightness
 *
 * @return Right boundry of string with 1 collumn space
 */
uint16_t WLICLED::set_string(char c[], uint16_t place, uint16_t G_H,
                             uint8_t R_S, uint8_t B_V, uint8_t Bright) {
    // check if not empty
    if (c == 0) return place;
    // place all characters in row in expanded buffer
    for (uint16_t cnum = 0; cnum < (uint16_t)strlen(c); cnum++) {
        place = set_char(c[cnum], place, G_H, R_S, B_V, Bright);
    }
    return place + 2;
}

/**
 * @brief writes a whole string in expanded LEDBuf and adds an € at the end
 *
 * @param[in] c: string to set in buffer
 * @param[in] place: left boundry of string
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 * @param[in] Bright:  Brightness
 *
 * @return None
 */
void WLICLED::showPrice(char c[], uint16_t place, uint16_t G_H, uint8_t R_S,
                        uint8_t B_V, uint8_t Bright) {
    interrupt = false;
    // writes all characters in buffer
    for (uint16_t cnum = 0; cnum < (uint16_t)strlen(c); cnum++) {
        place = set_char(c[cnum], place, G_H, R_S, B_V, Bright);
    }
    // adds an € at the end
    place = set_euro(place, G_H, R_S, B_V, Bright);
    if (place <= COLUMNS)
        showLoop(100);  // fits on screen, no scrolling necessary
    else
        showRunLoop(100, place);
    interrupt = false;
    clearChar();
}

/**
 * @brief start scrolling text from right to left for 'times' times or until
 * interrupt
 *
 * @param[in] del: delay time in ms --> slows animation down, should be at least
 * 5ms
 * @param[in] end: end of last symbol in RAM --> here ends transmission --> end
 * of expanded LEDBuf by default
 * @param[in] times: transmission repeats 'times' times
 *
 * @return None
 */
void WLICLED::showRun(uint32_t del, uint16_t end, uint16_t times) {
    if (del < 5) del = 5;  // delay should be at least 5ms to avoid problems
    // interrupt = false;
    //  start DMA transaction
    if (!dmaTrans) {
        // start DMA transaction
        dma.startJob();
        dma.trigger();
        dmaTrans = true;
    }
    for (uint16_t ti = 0; ti < times; ti++) {
        if (end > COLUMNS * (SCREENSTORUN - 1))
            end = COLUMNS * (SCREENSTORUN - 1);  // check boundry
        // delay and incementation of start pointer after every transmission
        for (uint16_t k = 0; k < end; k++) {
            uint32_t *out = dmaBuf;
            // check for orintation
            
            if (turned == false) {
                for (int i = 0; i < LEDCOUNT; i++) {
                    // write data from LEDBuf to dmaBuf and expand bits 4:1 for
                    // SPI sequence
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].G];
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].R];
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].B];
                }
            } else {
                for (int i = LEDCOUNT - 1; i >= 0; i--) {
                    // write data from LEDBuf to dmaBuf and expand bits 4:1 for
                    // SPI sequence
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].G];
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].R];
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].B];
                }
            }
            if (interrupt == true) return;
            delay(del);  // optional delay to slow animations a bit down,
                         // default 5 --> ~200 FPS at 105 pixels
                         // repeats only same DMA transmission without changes
                         // for del ms
        }
    }
}

/**
 * @brief start scrolling text from right to left until interrupt
 *
 * @param[in] del: delay time in ms --> slows animation down, should be at least
 * 5ms
 * @param[in] end: end of last symbol in RAM --> here ends transmission --> end
 * of expanded LEDBuf by default
 *
 * @return None
 */
void WLICLED::showRunLoop(uint32_t del, uint16_t end) {
    if (del < 5) del = 5;  // delay should be at least 5ms to avoid problems
    // interrupt = false;
    if (!dmaTrans) {
        // start DMA transaction
        dma.startJob();
        dma.trigger();
        dmaTrans = true;
    }

    while (interrupt == false) {
        if (end > COLUMNS * (SCREENSTORUN - 1))
            end = COLUMNS * (SCREENSTORUN - 1);
        for (uint16_t k = 0; k < end; k++) {
            uint32_t *out = dmaBuf;
            // check for orientation
           
            if (turned == false) {
                for (int i = 0; i < LEDCOUNT; i++) {
                    // write data from LEDBuf to dmaBuf and expand bits 4:1 for
                    // SPI sequence
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].G];
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].R];
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].B];
                }
            } else {
                for (int i = LEDCOUNT - 1; i >= 0; i--) {
                    // write data from LEDBuf to dmaBuf and expand bits 4:1 for
                    // SPI sequence
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].G];
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].R];
                    *out++ = bitExpand[LEDBuf[i + (k * ROWS)].B];
                    // check for interrupt
                }
            }
            // check for interrupt
            if (interrupt == true) {
                interrupt = false;
                clearChar();
                return;
            }
            delay(del);  // optional delay to slow animations a bit down,
                         // default 5 --> ~200 FPS at 105 pixels
                         // repeats only same transmission without changes for
                         // del ms
        }
    }
}

/**
 * @brief shows Alphabet on screen until interrupt
 *
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 * @param[in] Bright:  Brightness
 *
 * @return None
 */
void WLICLED::send_Alphabet(uint16_t G_H, uint8_t R_S, uint8_t B_V,
                            uint8_t Bright) {
    interrupt = false;
    uint16_t x = COLUMNS - 1;  // start column
    // write alphabet in buffer
    x = set_string("ABCDEFGHIJKLMNOPQRSTUVWXYZ", x, G_H, R_S, B_V, Bright);
    x = set_string(" abcdefghijklmnopqrstuvwxyz", x, G_H, R_S, B_V, Bright);
    x = set_string(" 0123456789", x, G_H, R_S, B_V, Bright);
    x = set_string("!?&.,():;/+-=<>@#%$_^[]", x, G_H, R_S, B_V, Bright);
    // start transmission until interrupt
    showRunLoop(100, x + 5);
}

/**
 * @brief shows a test image on screen in given color -->
 * brightness increases every collumn --> use for screen tests
 *
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 *
 * @return None
 */
void WLICLED::testimage(uint16_t G_H, uint8_t R_S, uint8_t B_V) {
    uint16_t i = 0;
    uint8_t step = (uint8_t)(255 / (uint16_t)COLUMNS);
    if (step == 0) step++;  // display wider than 255 pixels
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (i = 0; i < ROWS; i++)
            if (k > 255)
                set_screenPixel(k, i, G_H, R_S, B_V, 255);
            else
                set_screenPixel(k, i, G_H, R_S, B_V, k * step);
    }
    showLoop(100);
}

/**
 * @brief shows a scrolling rainbow on screen until interrupt
 *
 * @param[in] del: delay time in ms, should be at least 5ms
 * @param[in] Bright: brightness
 *
 * @return None
 */
void WLICLED::showRainbow(uint32_t del, uint8_t Bright) {
    // check for color system and set to GRB --> hard coded colors
    bool hsv = false;
    if (ColorSystem == HSV) {
        hsv = true;
        ColorSystem = GRB;
    }
    uint8_t step =
        (uint8_t)(255 / (uint16_t)COLUMNS);  // fade frome one base color to an
                                             // other in one screen width
    if (step == 0) step++;                   // display wider than 255 pixels
    // Red --> green
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k, i, step * k, 255 - (step * k), 0, Bright);
        }
    }
    // green --> blue
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + COLUMNS, i, 255 - (step * k), 0, step * k,
                          Bright);
        }
    }
    // blue --> red
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + (2 * COLUMNS), i, 0, step * k, 255 - (step * k),
                          Bright);
        }
    }
    // red --> green again for scmooth change on restart
    for (uint8_t k = 0; k < COLUMNS; k++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            set_charPixel(k + (3 * COLUMNS), i, step * k, 255 - (step * k), 0,
                          Bright);
        }
    }
    showRunLoop(del, (3 * COLUMNS) -
                         1);  // stop 1 screen earlier to get a smooth change
    // reset color system and interrupt bool
    interrupt = false;
    if (hsv == true) ColorSystem = HSV;
    // clear expanded buffer
    clearChar();
}




/**
 * @brief writes a € Symbol in LEDBuf
 *
 * @param[in] place: left boundry of symbol
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 * @param[in] Bright: Brightness
 *
 * @return Right boundry of € Symbol with 1 collumn space
 */
uint16_t WLICLED::set_euro(uint16_t place, uint16_t G_H, uint8_t R_S,
                           uint8_t B_V, uint8_t Bright) {
    if (place > (SCREENSTORUN * COLUMNS) - 5)
        return place;  // be sure it's not out of bounds
    set_charPixel(place, 4, G_H, R_S, B_V, Bright);
    set_charPixel(place, 2, G_H, R_S, B_V, Bright);

    set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
    set_charPixel(place, 3, G_H, R_S, B_V, Bright);
    set_charPixel(place, 2, G_H, R_S, B_V, Bright);

    set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
    set_charPixel(place, 2, G_H, R_S, B_V, Bright);
    set_charPixel(place, 4, G_H, R_S, B_V, Bright);
    set_charPixel(place, 5, G_H, R_S, B_V, Bright);

    set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
    set_charPixel(place, 2, G_H, R_S, B_V, Bright);
    set_charPixel(place, 4, G_H, R_S, B_V, Bright);
    set_charPixel(place, 6, G_H, R_S, B_V, Bright);

    set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
    set_charPixel(place, 2, G_H, R_S, B_V, Bright);
    set_charPixel(place, 4, G_H, R_S, B_V, Bright);
    set_charPixel(place, 6, G_H, R_S, B_V, Bright);

    return place + 2;
}

/**
 * @brief writes a WE Logo in LEDBuf
 *
 * @param[in] place: left boundry of logo
 * @param[in] Bright: Brightness
 *
 * @return Right boundry of logo with 1 collumn space
 */
uint16_t WLICLED::set_WElogo(uint16_t place, uint8_t Bright) {
    if (place > (SCREENSTORUN * COLUMNS) - 14)
        return place;  // be sure it's not out of bounds
    bool hsv = false;
    if (ColorSystem == HSV) {
        hsv = true;
        ColorSystem = GRB;
    }
    set_charPixel(place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 0, 255, 0, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);
    set_charPixel(place, 6, 0, 255, 0, Bright);

    set_charPixel(++place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 0, 255, 0, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);
    set_charPixel(place, 6, 0, 255, 0, Bright);

    set_charPixel(++place, 6, 255, 255, 255, Bright);
    set_charPixel(place, 5, 255, 255, 255, Bright);
    set_charPixel(place, 4, 255, 255, 255, Bright);
    set_charPixel(place, 3, 255, 255, 255, Bright);
    set_charPixel(place, 2, 255, 255, 255, Bright);
    set_charPixel(place, 1, 255, 255, 255, Bright);
    set_charPixel(place, 0, 255, 255, 255, Bright);

    set_charPixel(++place, 5, 255, 255, 255, Bright);
    set_charPixel(place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 0, 255, 0, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 6, 0, 255, 0, Bright);

    set_charPixel(++place, 2, 255, 255, 255, Bright);
    set_charPixel(place, 3, 255, 255, 255, Bright);
    set_charPixel(place, 4, 255, 255, 255, Bright);
    set_charPixel(place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);
    set_charPixel(place, 6, 0, 255, 0, Bright);

    set_charPixel(++place, 5, 255, 255, 255, Bright);
    set_charPixel(place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 0, 255, 0, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 6, 0, 255, 0, Bright);

    set_charPixel(++place, 6, 255, 255, 255, Bright);
    set_charPixel(place, 5, 255, 255, 255, Bright);
    set_charPixel(place, 4, 255, 255, 255, Bright);
    set_charPixel(place, 3, 255, 255, 255, Bright);
    set_charPixel(place, 2, 255, 255, 255, Bright);
    set_charPixel(place, 1, 255, 255, 255, Bright);
    set_charPixel(place, 0, 255, 255, 255, Bright);

    set_charPixel(++place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 0, 255, 0, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);
    set_charPixel(place, 6, 0, 255, 0, Bright);

    set_charPixel(++place, 6, 255, 255, 255, Bright);
    set_charPixel(place, 5, 255, 255, 255, Bright);
    set_charPixel(place, 4, 255, 255, 255, Bright);
    set_charPixel(place, 3, 255, 255, 255, Bright);
    set_charPixel(place, 2, 255, 255, 255, Bright);
    set_charPixel(place, 1, 255, 255, 255, Bright);
    set_charPixel(place, 0, 255, 255, 255, Bright);

    set_charPixel(++place, 0, 255, 255, 255, Bright);
    set_charPixel(place, 3, 255, 255, 255, Bright);
    set_charPixel(place, 6, 255, 255, 255, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);

    set_charPixel(++place, 0, 255, 255, 255, Bright);
    set_charPixel(place, 3, 255, 255, 255, Bright);
    set_charPixel(place, 6, 255, 255, 255, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);

    set_charPixel(++place, 0, 255, 255, 255, Bright);
    set_charPixel(place, 3, 255, 255, 255, Bright);
    set_charPixel(place, 6, 255, 255, 255, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);

    set_charPixel(++place, 0, 255, 255, 255, Bright);
    set_charPixel(place, 3, 255, 255, 255, Bright);
    set_charPixel(place, 6, 255, 255, 255, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);

    set_charPixel(++place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 0, 255, 0, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);
    set_charPixel(place, 6, 0, 255, 0, Bright);

    set_charPixel(++place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 0, 255, 0, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);
    set_charPixel(place, 6, 0, 255, 0, Bright);

    if (hsv == true) ColorSystem = HSV;
    return place + 2;
}

/**
 * @brief writes a Würth Logo in LEDBuf
 *
 * @param[in] place: left boundry of logo
 * @param[in] Bright: Brightness
 *
 * @return Right boundry of logo with 1 collumn space
 */
uint16_t WLICLED::set_Wuerthlogo(uint16_t place, uint8_t Bright) {
    if (place > (SCREENSTORUN * COLUMNS) - 14)
        return place;  // be sure it's not out of bounds
    bool hsv = false;
    if (ColorSystem == HSV) {
        hsv = true;
        ColorSystem = GRB;
    }

    set_charPixel(place, 0, 200, 200, 200, Bright);
    set_charPixel(place, 1, 200, 200, 200, Bright);
    set_charPixel(place, 2, 200, 200, 200, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 200, 200, 200, Bright);
    set_charPixel(place, 5, 200, 200, 200, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);

    set_charPixel(++place, 0, 200, 200, 200, Bright);
    set_charPixel(place, 1, 200, 200, 200, Bright);
    set_charPixel(place, 2, 200, 200, 200, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 200, 200, 200, Bright);
    set_charPixel(place, 5, 200, 200, 200, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);

    set_charPixel(++place, 0, 200, 200, 200, Bright);
    set_charPixel(place, 1, 200, 200, 200, Bright);
    set_charPixel(place, 2, 200, 200, 200, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 200, 200, 200, Bright);
    set_charPixel(place, 5, 200, 200, 200, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);

    set_charPixel(++place, 0, 200, 200, 200, Bright);
    set_charPixel(place, 1, 200, 200, 200, Bright);
    set_charPixel(place, 2, 200, 200, 200, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 200, 200, 200, Bright);
    set_charPixel(place, 5, 200, 200, 200, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);
    // Start of the logo
    set_charPixel(++place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 200, 200, 200, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);

    set_charPixel(++place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);

    set_charPixel(++place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);
    set_charPixel(place, 6, 0, 255, 0, Bright);

    set_charPixel(++place, 0, 200, 200, 200, Bright);
    set_charPixel(place, 1, 200, 200, 200, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);
    // Half
    set_charPixel(++place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);
    set_charPixel(place, 6, 0, 255, 0, Bright);

    set_charPixel(++place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 0, 255, 0, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);

    set_charPixel(++place, 0, 0, 255, 0, Bright);
    set_charPixel(place, 1, 0, 255, 0, Bright);
    set_charPixel(place, 2, 0, 255, 0, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 0, 255, 0, Bright);
    set_charPixel(place, 5, 200, 200, 200, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);

    // End of the logo
    set_charPixel(++place, 0, 200, 200, 200, Bright);
    set_charPixel(place, 1, 200, 200, 200, Bright);
    set_charPixel(place, 2, 200, 200, 200, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 200, 200, 200, Bright);
    set_charPixel(place, 5, 200, 200, 200, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);

    set_charPixel(++place, 0, 200, 200, 200, Bright);
    set_charPixel(place, 1, 200, 200, 200, Bright);
    set_charPixel(place, 2, 200, 200, 200, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 200, 200, 200, Bright);
    set_charPixel(place, 5, 200, 200, 200, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);

    set_charPixel(++place, 0, 200, 200, 200, Bright);
    set_charPixel(place, 1, 200, 200, 200, Bright);
    set_charPixel(place, 2, 200, 200, 200, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 200, 200, 200, Bright);
    set_charPixel(place, 5, 200, 200, 200, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);

    set_charPixel(++place, 0, 200, 200, 200, Bright);
    set_charPixel(place, 1, 200, 200, 200, Bright);
    set_charPixel(place, 2, 200, 200, 200, Bright);
    set_charPixel(place, 3, 200, 200, 200, Bright);
    set_charPixel(place, 4, 200, 200, 200, Bright);
    set_charPixel(place, 5, 200, 200, 200, Bright);
    set_charPixel(place, 6, 200, 200, 200, Bright);

    if (hsv == true) ColorSystem = HSV;
    return place + 2;
}
/**
 * @brief writes a character Symbol in LEDBuf --> most ASCII-128 characters
 * included, but not all
 *
 * @param[in] c: character to write
 * @param[in] place: left boundry of symbol
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 * @param[in] Bright: Brightness
 *
 * @return Right boundry of Symbol with 1 collumn space
 */
uint16_t WLICLED::set_char(char c, uint16_t place, uint16_t G_H, uint8_t R_S,
                           uint8_t B_V, uint8_t Bright) {
    // starts with left column, sets all necessary pixels and increments place
    // for next collumn
    if (c == 0) return place;
    if (place > (SCREENSTORUN * COLUMNS) - 5)
        return place;  // be sure it's not out of bounds

    switch (c) {
        case 'A': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'B': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'C': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'D': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'E': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'F': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'G': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'H': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'I': {
            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case 'J': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'K': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'L': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'M': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'N': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'O': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'P': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'Q': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'R': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'S': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'T': {
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'U': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'V': {
            /*set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 2;*/

            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            return place + 2;
        }

        case 'W': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'X': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'Y': {
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'Z': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'a': {
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'b': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            // set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'c': {
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            return place + 2;
        }

        case 'd': {
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            // set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'e': {
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'f': {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'g': {
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            // set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'h': {
            // set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            // set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'i': {
            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case 'j': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case 'k': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'l': {
            // set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case 'm': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'n': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'o': {
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'p': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            // set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'q': {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            // set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            // set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'r': {
            // set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);

            // set_charPixel(++place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 's': {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 't': {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'u': {
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'v': {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'w': {
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'x': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'y': {
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 'z': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '0': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '1': {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case '2': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '3': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '4': {
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '5': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '6': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '7': {
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '8': {
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '9': {
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '!': {
            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case '?': {
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '&': {
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '.': {
            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case ',': {
            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case ':': {
            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case ';': {
            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case '(': {
            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case ')': {
            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 3;
        }

        case '/': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '+': {
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '-': {
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '=': {
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '<': {
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '>': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '@': {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '%': {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '#': {
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '"': {
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place++, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '$': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '_': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '[': {
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case ']': {
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 6, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 0, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case '^': {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case ' ': {
            return place + 5;
        }
        default:
            return place;  // character not included --> skip
    }
}

/**
 * @brief writes a emoji Symbol in LEDBuf
 *
 * @param[in] e: number of emoji to write
 * @param[in] place: left boundry of symbol
 * @param[in] G_H: G/H-coordinate of color
 * @param[in] R_S: R/S-coordinate of color
 * @param[in] B_V: B/V-coordinate of color
 * @param[in] Bright: Brightness
 *
 * @return Right boundry of Symbol with 1 collumn space
 */
uint16_t WLICLED::set_emoji(uint8_t e, int32_t place, uint16_t G_H, uint8_t R_S,
                            uint8_t B_V, uint8_t Bright) {
    if (place > (SCREENSTORUN * COLUMNS) - 5)
        return place;  // be sure it's not out of bounds
    switch (e) {
        case 1: {
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 2: {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 3: {
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 5, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 4: {
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            if (R_S < 200) {
                set_charPixel(place, 5, G_H, R_S + 100, B_V, Bright);
            } else {
                set_charPixel(place, 5, G_H, R_S - 100, B_V, Bright);
            }

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            if (R_S < 200) {
                set_charPixel(place, 5, G_H, R_S + 100, B_V, Bright);
                set_charPixel(place, 6, G_H, R_S + 100, B_V, Bright);
            } else {
                set_charPixel(place, 5, G_H, R_S - 100, B_V, Bright);
                set_charPixel(place, 6, G_H, R_S - 100, B_V, Bright);
            }

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            if (R_S < 200) {
                set_charPixel(place, 5, G_H, R_S + 100, B_V, Bright);
            } else {
                set_charPixel(place, 5, G_H, R_S - 100, B_V, Bright);
            }

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 5: {
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 6: {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            return place + 2;
        }

        case 7: {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            return place + 2;
        }

        case 8: {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            return place + 2;
        }

        case 9: {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            return place + 2;
        }

        case 10: {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            return place + 2;
        }

        case 11: {
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 0, G_H, R_S, B_V, Bright);
            set_charPixel(place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 1, G_H, R_S, B_V, Bright);
            set_charPixel(place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);
            set_charPixel(place, 5, G_H, R_S, B_V, Bright);
            set_charPixel(place, 6, G_H, R_S, B_V, Bright);

            set_charPixel(++place, 2, G_H, R_S, B_V, Bright);
            set_charPixel(place, 3, G_H, R_S, B_V, Bright);
            set_charPixel(place, 4, G_H, R_S, B_V, Bright);

            return place + 2;
        }

        default:
            return place;  // invalid number
    }
}
