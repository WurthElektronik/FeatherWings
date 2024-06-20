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

#ifndef ICLED_H
#define ICLED_H

#include <stdint.h>
#include <stddef.h>

// Letters are optimized for the WE-WLICLED Featherwing, no guarantee for other
// display sizes and shapes
#define ICLED_COLUMNS 15                             // Columns on screen
#define ICLED_ROWS 7                                 // Rows on screen
#define ICLED_LED_COUNT (ICLED_COLUMNS * ICLED_ROWS) // LEDs on screen
#define ICLED_SCREENSTORUN 45
// Maximum screen width reserved in RAM for scrolling Text
// set to 1 if no scrolling texts wanted --> saves RAM
#define ICLED_BYTESPERPIXEL 3 // GRB , each is 8bit = 1 Byte

// If the SPI clock was accurate we would only need 80 bytes since 80 * 8 * 0.3125 = 200 Microsecond
// but since the SPI clock was measured at 3.4299 Mhz we need to increase the number of bytes to 100 so that 100 *
// 100 * 8 * 0.29155 = 233 Microsecond which is still more than the minimum latch

#define ICLED_LATCHBYTECOUNT 100 // 100 * 8 * 0.29155 ~= 233 Microsecond latch

#define ICLED_BYTESTOTAL (ICLED_LED_COUNT * ICLED_BYTESPERPIXEL * 4) + ICLED_LATCHBYTECOUNT

#define ICLED_MAX_BRIGHTNESS \
    210 // Max Brightness of all 3 colors summed up ; do only change if you
        // know what you are doing

#define ICLED_DIN_PIN 6
#define ICLED_PROG_PIN 5

typedef enum
{
    RGB,
    HSV,
} ICLED_Color_System;

typedef enum
{
    Landscape,
    Landscape_UpsideDown,
} ICLED_Orientation;

typedef enum
{
    Emoji_Smile_Face,
    Emoji_Frown_Face,
    Emoji_Neutral_Face,
    Emoji_Tongue_Face,
    Emoji_Check_Mark,
    Emoji_Heart,
    Emoji_Droplet,
    Emoji_Total_Count
} Emoji;

/**
 * @brief       Intialize the interfaces for ICLEDFeatherWing.
 *
 * @param[in]   color_system: Specify the color system to be used. See ICLED_Color_System.
 * @param[in]   orientation: Specify the orientation. See ICLED_Orientation.
 *
 * @return      True if successful, false otherwise.
 */
bool ICLED_Init(ICLED_Color_System color_system = RGB, ICLED_Orientation orientation = Landscape);

/**
 * @brief       Deintializes the interfaces for ICLEDFeatherWing.
 *
 * @return      True if successful, false otherwise.
 */
bool ICLED_Deinit();

/**
 * @brief       Set a pixel in the first screen using its index to the given color with the given brightness.
 *
 * @param[in]   pixel_number: Index of the pixel.
 * @param[in]   R_H: R/H-coordinate of color.
 * @param[in]   G_S: G/S-coordinate of color.
 * @param[in]   B_V: B/V-coordinate of color.
 * @param[in]   brightness: Brightness.
 * @param[in]   write_buffer: Optional argument that indicates whether the LED buffer should be applied to the LED screen. Defaults to true.
 *
 * @return      True if successful, false otherwise.
 */
bool ICLED_set_pixel(uint16_t pixel_number, uint16_t R_H, uint16_t G_S,
                     uint16_t B_V, uint8_t brightness, bool write_buffer = true); // sets pixel on given color

/**
 * @brief       Set a pixel in the first screen using its coordinates to the given color with the given brightness.
 *
 * @param[in]   row: Row of the pixel.
 * @param[in]   column: Column of the pixel.
 * @param[in]   R_H: R/H-coordinate of color.
 * @param[in]   G_S: G/S-coordinate of color.
 * @param[in]   B_V: B/V-coordinate of color.
 * @param[in]   brightness: Brightness.
 * @param[in]   write_buffer: Optional argument that indicates whether the LED buffer should be applied to the LED screen. Defaults to true.
 *
 * @return      True if successful, false otherwise.
 */
bool ICLED_set_screen_pixel(uint8_t row, uint8_t column, uint16_t R_H,
                            uint16_t G_S, uint16_t B_V, uint8_t brightness, bool write_buffer = true);

/**
 * @brief       Set a pixel using its coordinates to any of the screens (useful for animations)
 *              to the given color with the given brightness.
 *
 * @param[in]   row: Row of the pixel.
 * @param[in]   column: Column of the pixel.
 * @param[in]   R_H: R/H-coordinate of color.
 * @param[in]   G_S: G/S-coordinate of color.
 * @param[in]   B_V: B/V-coordinate of color.
 * @param[in]   brightness: Brightness.
 * @param[in]   write_buffer: Optional argument that indicates whether the LED buffer should be applied to the LED screen. Defaults to true.
 *
 * @return      True if successful, false otherwise.
 */
bool ICLED_set_expanded_screen_pixel(uint8_t row, uint16_t column, uint16_t R_H,
                                     uint16_t G_S, uint16_t B_V, uint8_t brightness, bool write_buffer = true);

/**
 * @brief       Set all pixels in the first screen to the given color with the given brightness.
 *
 * @param[in]   row: Row of the pixel.
 * @param[in]   column: Column of the pixel.
 * @param[in]   R_H: R/H-coordinate of color.
 * @param[in]   G_S: G/S-coordinate of color.
 * @param[in]   B_V: B/V-coordinate of color.
 * @param[in]   brightness: Brightness.
 * @param[in]   write_buffer: Optional argument that indicates whether the LED buffer should be applied to the LED screen. Defaults to true.
 *
 * @return      True if successful, false otherwise.
 */
bool ICLED_set_all_pixels(uint16_t R_H, uint16_t G_S, uint16_t B_V, uint8_t brightness, bool write_buffer = true);

/**
 * @brief       Set all pixels in all screens to the given color with the given brightness.
 *
 * @param[in]   R_H: R/H-coordinate of color.
 * @param[in]   G_S: G/S-coordinate of color.
 * @param[in]   B_V: B/V-coordinate of color.
 * @param[in]   brightness: Brightness.
 * @param[in]   write_buffer: Optional argument that indicates whether the LED buffer should be applied to the LED screen. Defaults to true.
 *
 * @return      True if successful, false otherwise.
 */
bool ICLED_set_expanded_screen_all_pixels(uint16_t R_H, uint16_t G_S, uint16_t B_V, uint8_t brightness, bool write_buffer = true);

/**
 * @brief           Set a specific ASCII (7 bit) character at a certain column.
 *
 * @param[in]       c: The ASCII (7 bit) character.
 * @param[in,out]   place: Pointer to the column where to place the character, this value will be updated with the value after the character was set.
 * @param[in]       R_H: R/H-coordinate of color.
 * @param[in]       G_S: G/S-coordinate of color.
 * @param[in]       B_V: B/V-coordinate of color.
 * @param[in]       brightness: Brightness.
 * @param[in]       write_buffer: Optional argument that indicates whether the LED buffer should be applied to the LED screen. Defaults to true.
 *
 * @return          True if successful, false otherwise.
 */
bool ICLED_set_char(char c, uint16_t *place, uint16_t R_H, uint8_t G_S,
                    uint8_t B_V, uint8_t brightness, bool write_buffer = true);

/**
 * @brief           Set an emonji at a certain column.
 *
 * @param[in]       emoji: The emoji. See Emoji.
 * @param[in,out]   place: Pointer to the column where to place the character, this value will be updated with the value after the character was set.
 * @param[in]       R_H: R/H-coordinate of color.
 * @param[in]       G_S: G/S-coordinate of color.
 * @param[in]       B_V: B/V-coordinate of color.
 * @param[in]       brightness: Brightness.
 * @param[in]       write_buffer: Optional argument that indicates whether the LED buffer should be applied to the LED screen. Defaults to true.
 *
 * @return          True if successful, false otherwise.
 */
bool ICLED_set_emoji(Emoji emoji, uint16_t *place, uint16_t R_H, uint8_t G_S,
                     uint8_t B_V, uint8_t brightness, bool write_buffer = true);

/**
 * @brief           Set a string at a certain column.
 *
 * @param[in]       c: The string (character array).
 * @param[in,out]   place: Pointer to the column where to place the character, this value will be updated with the value after the character was set.
 * @param[in]       R_H: R/H-coordinate of color.
 * @param[in]       G_S: G/S-coordinate of color.
 * @param[in]       B_V: B/V-coordinate of color.
 * @param[in]       brightness: Brightness.
 * @param[in]       write_buffer: Optional argument that indicates whether the LED buffer should be applied to the LED screen. Defaults to true.
 *
 * @return          True if successful, false otherwise.
 */
bool ICLED_set_string(char c[], uint16_t *place, uint16_t R_H,
                      uint8_t G_S, uint8_t B_V, uint8_t brightness, bool write_buffer = true);

/**
 * @brief           Set a euro sign at a certain column.
 *
 * @param[in,out]   place: Pointer to the column where to place the character, this value will be updated with the value after the character was set.
 * @param[in]       R_H: R/H-coordinate of color.
 * @param[in]       G_S: G/S-coordinate of color.
 * @param[in]       B_V: B/V-coordinate of color.
 * @param[in]       brightness: Brightness.
 * @param[in]       write_buffer: Optional argument that indicates whether the LED buffer should be applied to the LED screen. Defaults to true.
 *
 * @return          True if successful, false otherwise.
 */
bool ICLED_set_euro(uint16_t *place, uint16_t R_H, uint8_t G_S,
                    uint8_t B_V, uint8_t brightness, bool write_buffer = true);

/**
 * @brief       Set the left most column of the screen (can be used for animations).
 *
 * @param[in]   column: the left most column to be set.
 * @param[in]   write_buffer: Optional argument that indicates whether the LED buffer should be applied to the LED screen. Defaults to true.
 *
 * @return      rue if successful, false otherwise.
 */
bool ICLED_set_start_column(uint16_t column, bool write_buffer = true);

/**
 * @brief       Set the color system to be used, this will be applied
 *              for the following calls to set_pixel and similar functions.
 *
 * @param[in]   color_system: Specify the color system to be used. See ICLED_Color_System.
 *
 * @return      None
 */
void ICLED_set_color_system(ICLED_Color_System color_system);

/**
 * @brief       Get the color system that is currently set.
 *
 * @return      The color system that is currently set. See ICLED_Color_System.
 */
ICLED_Color_System ICLED_get_color_system();

/**
 * @brief       Set orientation to be used, this will be applied
 *              for the following calls to set_pixel and similar functions.
 *
 * @param[in]   orientation: Specify the orientation. See ICLED_Orientation.
 *
 * @return      None
 */
void ICLED_set_orientation(ICLED_Orientation orientation);

/**
 * @brief       Get the orientation that is currently set.
 *
 * @return      The orientation that is currently set. See ICLED_Orientation.
 */
ICLED_Orientation ICLED_get_orientation();

/**
 * @brief       Clear the LED buffer.
 *
 * @param[in]   write_buffer: Optional argument that indicates whether the LED buffer should be applied to the LED screen. Defaults to true.
 *
 * @return      True if successful, false otherwise.
 */
void ICLED_clear(bool write_buffer = true);

/**
 * @brief           Start a looping animation that can be stopped conditionally if chosen.
 *
 * @param[in]       start_column: The first column to start the animation from.
 * @param[in]       end_column: The last column to end the animation on.
 * @param[in]       delay: The delay in milliseconds between each column, it controls the speed of the animation.
 * @param[in,out]   current_columnP: Optional argument that is a pointer to the location of the current reached column of the animation when
 *                  the animation is stopped it will be updates with the last column reached,
 *                  this can be used to resume the animation from where it was stopped.
 * @param[in]       running: Optional argument that is a pointer to a boolean that determintes
 *                  if the animation continues or stops, if not passed the animation will loop forever.
 *
 * @return          True if successful, false otherwise.
 */
bool ICLED_start_conditional_loop(uint16_t start_column, uint16_t end_column, uint32_t delay, uint16_t *current_columnP = NULL, bool *running = NULL);

/**
 * @brief           Start a looping animation that lasts for a certain duration.
 *
 * @param[in]       start_column: The first column to start the animation from.
 * @param[in]       end_column: The last column to end the animation on.
 * @param[in]       delay: The delay in milliseconds between each column, it controls the speed of the animation.
 * @param[in]       duration: The duration in milliseconds that the animation should run for.
 *
 * @return          True if successful, false otherwise.
 */
bool ICLED_start_timed_loop(uint16_t start_column, uint16_t end_column, uint32_t delay, uint32_t duration);

/**
 * @brief           Start a looping animation that runs for a number of iterations.
 *
 * @param[in]       start_column: The first column to start the animation from.
 * @param[in]       end_column: The last column to end the animation on.
 * @param[in]       delay: The delay in milliseconds between each column, it controls the speed of the animation.
 * @param[in]       iterations: The amount of times the animation should run for.
 *
 * @return          True if successful, false otherwise.
 */
bool ICLED_start_iteration_loop(uint16_t start_column, uint16_t end_column, uint32_t delay, uint32_t iterations);

#endif
