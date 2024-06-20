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

#include <stdint.h>
#include "emoji_symbol.h"

const LED_Symbol EMOJI_ARRAY[Emoji_Total_Count] =
    {
        [Emoji_Smile_Face] = {.pixel_locations = {{0, 4}, {1, 1}, {1, 2}, {1, 5}, {2, 5}, {3, 1}, {3, 2}, {3, 5}, {4, 4}, PIXEL_LOCATION_END}, .symbol_width = 5},
        [Emoji_Frown_Face] = {.pixel_locations = {{0, 5}, {1, 1}, {1, 2}, {1, 4}, {2, 4}, {3, 1}, {3, 2}, {3, 4}, {4, 5}, PIXEL_LOCATION_END}, .symbol_width = 5},
        [Emoji_Neutral_Face] = {.pixel_locations = {{0, 5}, {1, 1}, {1, 2}, {1, 5}, {2, 5}, {3, 1}, {3, 2}, {3, 5}, {4, 5}, PIXEL_LOCATION_END}, .symbol_width = 5},
        [Emoji_Tongue_Face] = {.pixel_locations = {{0, 4}, {1, 1}, {1, 2}, {1, 4}, {1, 5}, {2, 4}, {2, 5}, {2, 6}, {3, 1}, {3, 2}, {3, 4}, {3, 5}, {4, 4}, PIXEL_LOCATION_END}, .symbol_width = 5},
        [Emoji_Check_Mark] = {.pixel_locations = {{0, 3}, {1, 4}, {2, 3}, {3, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        [Emoji_Heart] = {.pixel_locations = {{0, 2}, {0, 3}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {4, 2}, {4, 3}, PIXEL_LOCATION_END}, .symbol_width = 5},
        [Emoji_Droplet] = {.pixel_locations = {{0, 3}, {0, 4}, {0, 5}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {4, 3}, {4, 4}, {4, 5}, PIXEL_LOCATION_END}, .symbol_width = 5},
};
