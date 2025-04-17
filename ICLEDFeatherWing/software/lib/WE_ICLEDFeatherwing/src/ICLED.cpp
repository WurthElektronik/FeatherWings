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
#include <SPI.h>
#include "ASCII_character.h"
#include "emoji_symbol.h"
#include "ConfigPlatform.h"
#include "debug.h"
#include "global.h"

/**
 * @brief       Apply the specified brightness to the color coordinate.
 *
 * @param[in]   color: Color coordinate value.
 * @param[in]   brightness:  Brightness value.
 *
 * @return      The color coordinate value with brightness applied.
 */
static inline uint8_t calculate_brightness(uint8_t color, uint8_t brightness);

/**
 * @brief       Applies the current LED buffer from the specified start column to the
 *              ICLED board by copying the LED buffer to the DMA buffer.
 *
 * @param[in]   column: The starting column from which the LED buffer will be copied.
 *
 * @return      None
 */
static void write_ledbuffer_to_DMAbuffer(uint16_t column = 0);

/**
 * @brief           This function ceils the sum of the color coordinates to
 *                  not be higher than ICLED_MAX_BRIGHTNESS.
 *
 * @param[in,out]   R: Pointer to Red color coordinate
 * @param[in,out]   G: Pointer to Green color coordinate
 * @param[in,out]   B: Pointer to Blue color coordinate
 *
 * @return None
 */
static void ceil_brightness(uint8_t *R, uint8_t *G, uint8_t *B);

/**
 * @brief       Convert HSV values to RGB.
 *
 * @param[in]   h: Hue.
 * @param[in]   s: Saturation.
 * @param[in]   v: Value.
 * @param[out]  r: Red.
 * @param[out]  g: Green.
 * @param[out]  b: Blue.
 *
 * @return      None
 */
static void HSV_to_RGB(float h, float s, float v, float *r, float *g, float *b);

static ICLED_Color_System ColorSystem = RGB;
static ICLED_Orientation Orientation = Landscape;

static uint8_t dmaBuf[ICLED_BYTESTOTAL] = {
    0}; // The raw buffer we write to SPI

static Adafruit_ZeroDMA dma; ///< The DMA manager for the SPI class
static SPIClass *spi;        ///< Underlying SPI hardware interface we use to DMA

#define MIN_LOOP_DELAY_MS 5

typedef union
{
    struct
    {
        uint8_t G;
        uint8_t R;
        uint8_t B;
    };
    uint8_t GBR[3];
} Pixel;

// buffer for LEDs --> will be written into dmaBuf after bit-expansion in
static Pixel LEDBuf[ICLED_LED_COUNT * ICLED_SCREENSTORUN];

#define ZEROPATTERN 0x8 // 4-bit
#define ONEPATTERN 0xE  // 4-bit

#define ZEROZEROPATTERN ((ZEROPATTERN << 4) | ZEROPATTERN) // 10001000
#define ZEROONEPATTERN ((ZEROPATTERN << 4) | ONEPATTERN)   // 10001110
#define ONEZEROPATTERN ((ONEPATTERN << 4) | ZEROPATTERN)   // 11101000
#define ONEONEPATTERN ((ONEPATTERN << 4) | ONEPATTERN)     // 11101110

bool ICLED_Init(ICLED_Color_System color_system, ICLED_Orientation orientation)
{
    SERCOM *sercomP;
    void *sercom_spi_data_reg_P;
    SercomSpiTXPad sercom_SPI_TX_PAD;
    _EPioType pin_type;
    uint8_t sercom_dmac_id;
    switch (ICLED_DIN_PIN)
    {
    case 6:
        sercomP = &sercom5;
        sercom_SPI_TX_PAD = SPI_PAD_2_SCK_3;
        pin_type = PIO_SERCOM;
        sercom_dmac_id = SERCOM5_DMAC_ID_TX;
        sercom_spi_data_reg_P = (void *)(&SERCOM5->SPI.DATA.reg);
        break;
    case 12:
        sercomP = &sercom3;
        sercom_SPI_TX_PAD = SPI_PAD_3_SCK_1;
        pin_type = PIO_SERCOM_ALT;
        sercom_dmac_id = SERCOM3_DMAC_ID_TX;
        sercom_spi_data_reg_P = (void *)(&SERCOM3->SPI.DATA.reg);
        break;
    default:
        return false;
    }

    // set color System to given Color system
    ColorSystem = color_system;

    Orientation = orientation;

    // clear Buffer and set all values to zero
    ICLED_clear();

    spi = new SPIClass(sercomP, ICLED_DIN_PIN, ICLED_DIN_PIN, ICLED_DIN_PIN, sercom_SPI_TX_PAD, SERCOM_RX_PAD_1);
    spi->begin();

    if (pinPeripheral(ICLED_DIN_PIN, pin_type) < 0)
    {
        WE_DEBUG_PRINT("Problem changing pin %d configuration.\r\n", ICLED_DIN_PIN);
        return false;
    }

    dma.setTrigger(sercom_dmac_id);

    dma.setAction(DMA_TRIGGER_ACTON_BEAT);

    if (dma.allocate() != DMA_STATUS_OK)
    {
        WE_DEBUG_PRINT("Failed to allocate DMA channel.\r\n");
        return false;
    }

    if (dma.addDescriptor(dmaBuf, sercom_spi_data_reg_P, ICLED_BYTESTOTAL, DMA_BEAT_SIZE_BYTE, true, false) == NULL)
    {
        WE_DEBUG_PRINT("Failed to allocate DMA descriptor.\r\n");
        return false;
    }

    dma.loop(true);

    spi->beginTransaction(
        SPISettings(3200000, MSBFIRST, SPI_MODE0));

    if (dma.startJob() != DMA_STATUS_OK)
    {
        WE_DEBUG_PRINT("Failed to start DMA job.\r\n");
        return false;
    }

    return true;
}

bool ICLED_Deinit()
{
    // clear Buffer and set all values to zero
    ICLED_clear();

    dma.abort();

    if (dma.free() != DMA_STATUS_OK)
    {
        WE_DEBUG_PRINT("Failed to free DMA channel.\r\n");
        return false;
    }

    spi->endTransaction();

    delete spi;

    return true;
}

void ICLED_set_color_system(ICLED_Color_System color_system)
{
    ColorSystem = color_system;
}

void ICLED_set_orientation(ICLED_Orientation orientation)
{
    Orientation = orientation;
}

ICLED_Color_System ICLED_get_color_system()
{
    return ColorSystem;
}

ICLED_Orientation ICLED_get_orientation()
{
    return Orientation;
}

static void HSV_to_RGB(float h, float s, float v, float *r, float *g, float *b)
{
    int i = floor(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    switch (i % 6)
    {
    case 0:
        *r = v, *g = t, *b = p;
        break;
    case 1:
        *r = q, *g = v, *b = p;
        break;
    case 2:
        *r = p, *g = v, *b = t;
        break;
    case 3:
        *r = p, *g = q, *b = v;
        break;
    case 4:
        *r = t, *g = p, *b = v;
        break;
    case 5:
        *r = v, *g = p, *b = q;
        break;
    }
}

bool ICLED_set_pixel(uint16_t pixel_number, uint16_t R_H, uint16_t G_S, uint16_t B_V,
                     uint8_t brightness, bool write_buffer)
{
    // Check, if parameters are ok & write LEDBuf
    if (pixel_number >= ICLED_LED_COUNT)
    {
        WE_DEBUG_PRINT("Pixel index %d is out of the first screen.\r\n", pixel_number);
        return false;
    }

    uint8_t R = 0, G = 0, B = 0;
    switch (ColorSystem)
    {
    case RGB:
    {
        if (R_H > 255 || G_S > 255 || B_V > 255)
        {
            WE_DEBUG_PRINT("RGB values should be between (0-255).\r\n");
            return false;
        }
        break;
    }
    case HSV:
    {
        if (R_H > 360 || G_S > 100 || B_V > 100)
        {
            WE_DEBUG_PRINT("H should be between (0-360), S and V between (0-100).\r\n");
            return false;
        }

        // Convert HSV to RGB
        float r = 0, g = 0, b = 0;
        HSV_to_RGB((float)R_H / 360, (float)G_S / 100, (float)B_V / 100, &r, &g, &b);
        R_H = (uint8_t)(r * 255);
        G_S = (uint8_t)(g * 255);
        B_V = (uint8_t)(b * 255);
        break;
    }
    default:
        WE_DEBUG_PRINT("Invalid color system.\r\n");
        return false;
    }

    R = calculate_brightness((uint8_t)R_H, brightness);
    G = calculate_brightness((uint8_t)G_S, brightness);
    B = calculate_brightness((uint8_t)B_V, brightness);

    ceil_brightness(&R, &G, &B); // checks PWM-levels for safety

    LEDBuf[pixel_number].G = G;
    LEDBuf[pixel_number].R = R;
    LEDBuf[pixel_number].B = B;

    if (write_buffer)
    {
        write_ledbuffer_to_DMAbuffer();
    }

    return true;
}

static inline uint8_t calculate_brightness(uint8_t color, uint8_t brightness)
{
    return (uint8_t)(((uint16_t)color * (uint16_t)brightness) / ((uint16_t)255));
}

static void write_ledbuffer_to_DMAbuffer(uint16_t column)
{

    int offset;
    int offset_sign;

    switch (Orientation)
    {
    default:
    case Landscape:
        offset = 0;
        offset_sign = 1;
        break;
    case Landscape_UpsideDown:
        offset = ICLED_LED_COUNT - 1;
        offset_sign = -1;
        break;
    }

    for (int i = 0; i < ICLED_LED_COUNT; i++)
    {
        for (uint8_t colorIdx = 0; colorIdx < 3; colorIdx++)
        {
            for (uint8_t bitIdx = 0; bitIdx < 4; bitIdx++)
            {
                switch ((LEDBuf[(offset + offset_sign * i) + (column * ICLED_ROWS)].GBR[colorIdx] << (2 * bitIdx)) & 0xC0)
                { // mask upper two bit
                case 0x00:
                    dmaBuf[4 * 3 * i + 4 * colorIdx + bitIdx] = ZEROZEROPATTERN;
                    break;
                case 0x40:
                    dmaBuf[4 * 3 * i + 4 * colorIdx + bitIdx] = ZEROONEPATTERN;
                    break;
                case 0x80:
                    dmaBuf[4 * 3 * i + 4 * colorIdx + bitIdx] = ONEZEROPATTERN;
                    break;
                case 0xC0:
                    dmaBuf[4 * 3 * i + 4 * colorIdx + bitIdx] = ONEONEPATTERN;
                    break;
                }
            }
        }
    }
}

static void ceil_brightness(uint8_t *R, uint8_t *G, uint8_t *B)
{
    uint16_t PWM = *R + *G + *B;

    if (PWM <= ICLED_MAX_BRIGHTNESS)
    {
        return;
    }

    float factor = (float)ICLED_MAX_BRIGHTNESS / PWM;

    *R = (uint8_t)(factor * *R);
    *G = (uint8_t)(factor * *G);
    *B = (uint8_t)(factor * *B);
}

bool ICLED_set_screen_pixel(uint8_t row, uint8_t column, uint16_t R_H,
                            uint16_t G_S, uint16_t B_V, uint8_t brightness, bool write_buffer)
{
    if (column >= ICLED_COLUMNS && row >= ICLED_ROWS)
    {
        WE_DEBUG_PRINT("the row should be between (0-%d) and the column should be between (0-%d).\r\n", ICLED_ROWS - 1, ICLED_COLUMNS - 1);
        return false;
    }

    // set corresponding pixel
    return ICLED_set_pixel(column * ICLED_ROWS + row, R_H, G_S, B_V, brightness, write_buffer);
}

bool ICLED_set_expanded_screen_pixel(uint8_t row, uint16_t column, uint16_t R_H,
                                     uint16_t G_S, uint16_t B_V, uint8_t brightness, bool write_buffer)
{
    // Check, if parameters are ok & write LEDBuf
    if (column >= (ICLED_COLUMNS * ICLED_SCREENSTORUN) || row >= ICLED_ROWS)
    {
        WE_DEBUG_PRINT("the row should be between (0-%d) and the column should be between (0-%d).\r\n", ICLED_ROWS - 1, (ICLED_COLUMNS * ICLED_SCREENSTORUN) - 1);
        return false;
    }

    uint8_t R = 0, G = 0, B = 0;
    switch (ColorSystem)
    {
    case RGB:
    {
        if (R_H > 255 || G_S > 255 || B_V > 255)
        {
            WE_DEBUG_PRINT("RGB values should be between (0-255).\r\n");
            return false;
        }
        break;
    }
    case HSV:
    {
        if (R_H > 360 || G_S > 100 || B_V > 100)
        {
            WE_DEBUG_PRINT("H should be between (0-360), S and V between (0-100).\r\n");
            return false;
        }

        // Convert HSV to RGB
        float r = 0, g = 0, b = 0;
        HSV_to_RGB((float)R_H / 360, (float)G_S / 100, (float)B_V / 100, &r, &g, &b);
        R_H = (uint8_t)(r * 255);
        G_S = (uint8_t)(g * 255);
        B_V = (uint8_t)(b * 255);
        break;
    }
    default:
        WE_DEBUG_PRINT("Invalid color system.\r\n");
        return false;
    }

    R = calculate_brightness((uint8_t)R_H, brightness);
    G = calculate_brightness((uint8_t)G_S, brightness);
    B = calculate_brightness((uint8_t)B_V, brightness);

    ceil_brightness(&R, &G, &B); // checks PWM-levels for safety

    uint16_t pixel_number = column * ICLED_ROWS + row;

    LEDBuf[pixel_number].G = G;
    LEDBuf[pixel_number].R = R;
    LEDBuf[pixel_number].B = B;

    if (write_buffer)
    {
        write_ledbuffer_to_DMAbuffer();
    }

    return true;
}

bool ICLED_set_all_pixels(uint16_t R_H, uint16_t G_S, uint16_t B_V, uint8_t brightness, bool write_buffer)
{
    for (int i = 0; i < ICLED_LED_COUNT; i++)
    {
        if (!ICLED_set_pixel(i, R_H, G_S, B_V, brightness, false))
        {
            return false;
        }
    }

    if (write_buffer)
    {
        write_ledbuffer_to_DMAbuffer();
    }

    return true;
}

bool ICLED_set_expanded_screen_all_pixels(uint16_t R_H, uint16_t G_S, uint16_t B_V, uint8_t brightness, bool write_buffer)
{
    for (int i = 0; i < ICLED_LED_COUNT; i++)
    {
        if (!ICLED_set_pixel(i, R_H, G_S, B_V, brightness, false))
        {
            return false;
        }
    }

    for (uint16_t i = 0; i < ICLED_COLUMNS * ICLED_SCREENSTORUN; i++)
    {
        for (uint16_t j = 0; j < ICLED_ROWS; j++)
        {
            if (!ICLED_set_expanded_screen_pixel(j, i, R_H, G_S, B_V, brightness, false))
            {
                return false;
            }
        }
    }

    if (write_buffer)
    {
        write_ledbuffer_to_DMAbuffer();
    }

    return true;
}

void ICLED_clear(bool write_buffer)
{
    memset(LEDBuf, 0, sizeof(LEDBuf));

    if (write_buffer)
    {
        memset(dmaBuf, 0, sizeof(dmaBuf));
    }
}

bool ICLED_set_char(char c, uint16_t *place, uint16_t R_H, uint8_t G_S,
                    uint8_t B_V, uint8_t brightness, bool write_buffer)
{
    if (place == NULL)
    {
        WE_DEBUG_PRINT("place pointer is NULL.\r\n");
        return false;
    }

    // starts with left column, sets all necessary pixels and increments place
    // for next collumn
    if (*place > (ICLED_SCREENSTORUN * ICLED_COLUMNS) - MAX_CHARACTER_WIDTH)
    {
        WE_DEBUG_PRINT("The character will not fit in the LED buffer.\r\n");
        return false; // be sure it's not out of bounds
    }

    if (c > 127)
    {
        WE_DEBUG_PRINT("The character is not ASCII 7 bit.\r\n");
        return false;
    }

    uint8_t index = (uint8_t)c;

    if (ASCII_CHARACTERS_ARRAY[index].symbol_width == 0)
    {
        // character does not exist in table
        WE_DEBUG_PRINT("The character %x doesn't have pixel representation in table.\r\n", c);
        return false;
    }

    uint8_t i = 0;

    while ((ASCII_CHARACTERS_ARRAY[index].pixel_locations[i].pixel_x != 0xFF) && (ASCII_CHARACTERS_ARRAY[index].pixel_locations[i].pixel_y != 0xFF))
    {
        if (!ICLED_set_expanded_screen_pixel(ASCII_CHARACTERS_ARRAY[index].pixel_locations[i].pixel_y,
                                             *place + ASCII_CHARACTERS_ARRAY[index].pixel_locations[i].pixel_x,
                                             R_H, G_S, B_V, brightness, false))
        {
            return false;
        }
        i++;
    }

    if (write_buffer)
    {
        write_ledbuffer_to_DMAbuffer();
    }

    *place = *place + ASCII_CHARACTERS_ARRAY[index].symbol_width + 1;

    return true;
}

bool ICLED_set_emoji(Emoji emoji, uint16_t *place, uint16_t R_H, uint8_t G_S,
                     uint8_t B_V, uint8_t brightness, bool write_buffer)
{
    if (place == NULL)
    {
        WE_DEBUG_PRINT("place pointer is NULL.\r\n");
        return false;
    }

    if (*place > (ICLED_SCREENSTORUN * ICLED_COLUMNS) - MAX_EMOJI_WIDTH)
    {
        WE_DEBUG_PRINT("The emoji will not fit in the LED buffer.\r\n");
        return false; // be sure it's not out of bounds
    }

    if (emoji >= Emoji_Total_Count)
    {
        WE_DEBUG_PRINT("The emoji is not valid.\r\n");
        return false;
    }

    uint8_t index = (uint8_t)emoji;

    uint8_t i = 0;

    while ((EMOJI_ARRAY[index].pixel_locations[i].pixel_x != 0xFF) && (EMOJI_ARRAY[index].pixel_locations[i].pixel_y != 0xFF))
    {
        if (!ICLED_set_expanded_screen_pixel(EMOJI_ARRAY[index].pixel_locations[i].pixel_y,
                                             *place + EMOJI_ARRAY[index].pixel_locations[i].pixel_x,
                                             R_H, G_S, B_V, brightness, false))
        {
            return false;
        }
        i++;
    }

    if (write_buffer)
    {
        write_ledbuffer_to_DMAbuffer();
    }

    *place = *place + EMOJI_ARRAY[index].symbol_width + 1;

    return true;
}

bool ICLED_set_euro(uint16_t *place, uint16_t R_H, uint8_t G_S,
                    uint8_t B_V, uint8_t brightness, bool write_buffer)
{
    if (place == NULL)
    {
        WE_DEBUG_PRINT("place pointer is NULL.\r\n");
        return false;
    }

    // starts with left column, sets all necessary pixels and increments place
    // for next collumn
    if (*place > (ICLED_SCREENSTORUN * ICLED_COLUMNS) - MAX_CHARACTER_WIDTH)
    {
        WE_DEBUG_PRINT("The euro sign will not fit in the LED buffer.\r\n");
        return false; // be sure it's not out of bounds
    }

    uint8_t euro_sign[][2] = {{0, 4}, {0, 2}, {1, 4}, {1, 3}, {1, 2}, {2, 1}, {2, 2}, {2, 4}, {2, 5}, {3, 0}, {3, 2}, {3, 4}, {3, 6}, {4, 0}, {4, 2}, {4, 4}, {4, 6}};

    for (uint8_t i = 0; i < (sizeof(euro_sign) / (2 * sizeof(uint8_t))); i++)
    {
        if (!ICLED_set_expanded_screen_pixel(euro_sign[i][1],
                                             *place + euro_sign[i][0],
                                             R_H, G_S, B_V, brightness, false))
        {
            return false;
        }
    }

    if (write_buffer)
    {
        write_ledbuffer_to_DMAbuffer();
    }

    *place = *place + 6;

    return true;
}

bool ICLED_set_string(char c[], uint16_t *place, uint16_t R_H,
                      uint8_t G_S, uint8_t B_V, uint8_t brightness, bool write_buffer)
{
    if ((c == NULL) || (place == NULL))
    {
        WE_DEBUG_PRINT("c or place pointer is NULL.\r\n");
        return false;
    }

    // place all characters in row in expanded buffer
    for (uint16_t cnum = 0; cnum < (uint16_t)strlen(c); cnum++)
    {
        if (!ICLED_set_char(c[cnum], place, R_H, G_S, B_V, brightness, false))
        {
            return false;
        }
    }

    if (write_buffer)
    {
        write_ledbuffer_to_DMAbuffer();
    }

    return true;
}

bool ICLED_set_start_column(uint16_t column, bool write_buffer)
{
    if (column > ((ICLED_COLUMNS * (ICLED_SCREENSTORUN - 1))))
    {
        WE_DEBUG_PRINT("The column should be between (0-%d).\r\n", ((ICLED_COLUMNS * (ICLED_SCREENSTORUN - 1))));
        return false;
    }

    if (write_buffer)
    {
        write_ledbuffer_to_DMAbuffer(column);
    }

    return true;
}

bool ICLED_start_conditional_loop(uint16_t start_column, uint16_t end_column, uint32_t delay, uint16_t *current_columnP, bool *running)
{
    uint16_t current_column = 0;

    if (current_columnP)
    {

        if ((*current_columnP < start_column) || (*current_columnP > end_column))
        {
            WE_DEBUG_PRINT("The column should be between (%d-%d).\r\n", start_column, end_column);
            return false;
        }
        else
        {
            current_column = *current_columnP - start_column;
        }
    }

    if (start_column >= end_column)
    {
        WE_DEBUG_PRINT("The start column is larger than or equal to the end column.\r\n");
        return false;
    }

    if (end_column > ((ICLED_COLUMNS * (ICLED_SCREENSTORUN - 1))))
    {
        WE_DEBUG_PRINT("The end column is can't be more than %d.\r\n", ((ICLED_COLUMNS * (ICLED_SCREENSTORUN - 1))));
        return false;
    }

    if (delay < MIN_LOOP_DELAY_MS)
    {
        return false;
    }

    while ((running == NULL) || *running)
    {
        if (!ICLED_set_start_column(current_column + start_column))
        {
            return false;
        }
        current_column = ((current_column + 1) % (end_column - start_column + 1));
        WE_Delay(delay);
    }

    if (current_columnP)
    {
        *current_columnP = current_column + start_column;
    }

    return true;
}

bool ICLED_start_timed_loop(uint16_t start_column, uint16_t end_column, uint32_t delay, uint32_t duration)
{
    uint16_t current_column = 0;

    if (start_column >= end_column)
    {
        WE_DEBUG_PRINT("The start column is larger than or equal to the end column.\r\n");
        return false;
    }

    if (end_column > ((ICLED_COLUMNS * (ICLED_SCREENSTORUN - 1))))
    {
        WE_DEBUG_PRINT("The end column is can't be more than %d.\r\n", ((ICLED_COLUMNS * (ICLED_SCREENSTORUN - 1))));
        return false;
    }

    if (delay < MIN_LOOP_DELAY_MS)
    {
        WE_DEBUG_PRINT("The delay is less than the minimum delay %d.\r\n", MIN_LOOP_DELAY_MS);
        return false;
    }

    if (delay >= duration)
    {
        WE_DEBUG_PRINT("The delay %d is larger than the duration %d.\r\n", delay, duration);
        return false;
    }

    while (duration > delay)
    {
        if (!ICLED_set_start_column(current_column + start_column))
        {
            return false;
        }
        current_column = ((current_column + 1) % (end_column - start_column + 1));
        WE_Delay(delay);
        duration -= delay;
    }

    return true;
}

bool ICLED_start_iteration_loop(uint16_t start_column, uint16_t end_column, uint32_t delay, uint32_t iterations)
{
    return ICLED_start_timed_loop(start_column, end_column, delay, iterations * delay * (end_column - start_column + 1));
}
