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
#include "ASCII_character.h"

const LED_Symbol ASCII_CHARACTERS_ARRAY[128] =
    {
        // Upper case letters
        ['A'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 0}, {1, 3}, {2, 0}, {2, 3}, {3, 0}, {3, 3}, {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['B'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 0}, {1, 3}, {1, 6}, {2, 0}, {2, 3}, {2, 6}, {3, 0}, {3, 3}, {3, 6}, {4, 5}, {4, 4}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['C'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 0}, {1, 6}, {2, 0}, {2, 6}, {3, 0}, {3, 6}, {4, 5}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['D'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 0}, {1, 6}, {2, 0}, {2, 6}, {3, 0}, {3, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['E'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 0}, {1, 3}, {1, 6}, {2, 0}, {2, 3}, {2, 6}, {3, 0}, {3, 3}, {3, 6}, {4, 0}, {4, 3}, {4, 6}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['F'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 0}, {1, 3}, {2, 0}, {2, 3}, {3, 0}, {3, 3}, {4, 0}, {4, 3}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['G'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 0}, {1, 6}, {2, 0}, {2, 6}, {3, 0}, {3, 6}, {4, 5}, {4, 4}, {4, 3}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['H'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 3}, {2, 3}, {3, 3}, {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['I'] = {.pixel_locations = {{1, 6}, {1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {1, 0}, PIXEL_LOCATION_END}, .symbol_width = 2},
        ['J'] = {.pixel_locations = {{0, 5}, {1, 6}, {2, 0}, {2, 6}, {3, 0}, {3, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['K'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 3}, {2, 2}, {2, 4}, {3, 1}, {3, 5}, {4, 0}, {4, 6}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['L'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 6}, {2, 6}, {3, 6}, {3, 6}, {4, 6}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['M'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 2}, {2, 3}, {3, 2}, {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['N'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 2}, {2, 3}, {3, 4}, {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['O'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 0}, {1, 6}, {2, 0}, {2, 6}, {3, 0}, {3, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['P'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 0}, {1, 3}, {2, 0}, {2, 3}, {3, 0}, {3, 3}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['Q'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 0}, {1, 6}, {2, 0}, {2, 6}, {3, 0}, {3, 5}, {4, 6}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['R'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 0}, {1, 3}, {2, 0}, {2, 3}, {3, 0}, {3, 3}, {4, 6}, {4, 5}, {4, 4}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['S'] = {.pixel_locations = {{0, 6}, {0, 2}, {0, 1}, {1, 0}, {1, 3}, {1, 6}, {2, 0}, {2, 3}, {2, 6}, {3, 0}, {3, 3}, {3, 6}, {4, 5}, {4, 4}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['T'] = {.pixel_locations = {{0, 0}, {1, 0}, {2, 6}, {2, 5}, {2, 4}, {2, 3}, {2, 2}, {2, 1}, {2, 0}, {3, 0}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['U'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 6}, {2, 6}, {3, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['V'] = {.pixel_locations = {{0, 2}, {0, 1}, {0, 0}, {1, 4}, {1, 3}, {2, 6}, {2, 5}, {3, 4}, {3, 3}, {4, 2}, {4, 1}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['W'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 5}, {2, 2}, {2, 3}, {2, 4}, {3, 5}, {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['X'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 1}, {0, 0}, {1, 2}, {1, 4}, {2, 3}, {3, 4}, {3, 2}, {4, 6}, {4, 5}, {4, 1}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['Y'] = {.pixel_locations = {{0, 1}, {0, 0}, {1, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {3, 2}, {4, 1}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['Z'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 0}, {1, 0}, {1, 4}, {1, 6}, {2, 0}, {2, 3}, {2, 6}, {3, 0}, {3, 2}, {3, 6}, {4, 6}, {4, 1}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},

        // Lower case letters
        ['a'] = {.pixel_locations = {{0, 4}, {1, 1}, {1, 3}, {1, 5}, {2, 1}, {2, 3}, {2, 5}, {3, 1}, {3, 3}, {3, 5}, {4, 2}, {4, 3}, {4, 4}, {4, 5}, PIXEL_LOCATION_END}, .symbol_width = 5},
         ['b'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 2}, {1, 5}, {2, 2}, {2, 5}, {3, 3}, {3, 4}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['c'] = {.pixel_locations = {{0, 4}, {0, 3}, {0, 2}, {1, 1}, {1, 5}, {2, 1}, {2, 5}, {3, 1}, {3, 5}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['d'] = {.pixel_locations = {{0, 4}, {0, 3}, {1, 2}, {1, 5}, {2, 2}, {2, 5}, {3, 5}, {3, 4}, {3, 3}, {3, 2}, {3, 1}, {3, 0}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['e'] = {.pixel_locations = {{0, 4}, {0, 3}, {0, 2}, {1, 1}, {1, 3}, {1, 5}, {2, 1}, {2, 3}, {2, 5}, {3, 1}, {3, 3}, {3, 5}, {4, 2}, {4, 3}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['f'] = {.pixel_locations = {{0, 2}, {1, 6}, {1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {2, 0}, {2, 2}, {3, 0}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['g'] = {.pixel_locations = {{0, 3}, {0, 2}, {1, 1}, {1, 4}, {1, 6}, {2, 1}, {2, 4}, {2, 6}, {3, 5}, {3, 4}, {3, 3}, {3, 2}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['h'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 3}, {2, 2}, {3, 3}, {3, 5}, {3, 4}, {3, 3}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['i'] = {.pixel_locations = {{1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 0}, PIXEL_LOCATION_END}, .symbol_width = 2},
        ['j'] = {.pixel_locations = {{0, 6}, {1, 6}, {2, 5}, {2, 4}, {2, 3}, {2, 2}, {2, 0}, PIXEL_LOCATION_END}, .symbol_width = 3},
        ['k'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 3}, {2, 2}, {2, 4}, {3, 1}, {3, 5}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['l'] = {.pixel_locations = {{0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 5}, PIXEL_LOCATION_END}, .symbol_width = 2},
        ['m'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {1, 1}, {2, 5}, {2, 4}, {2, 3}, {2, 2}, {3, 1}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['n'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {1, 1}, {2, 1}, {3, 1}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['o'] = {.pixel_locations = {{0, 4}, {0, 3}, {0, 2}, {1, 1}, {1, 5}, {2, 1}, {2, 5}, {3, 1}, {3, 5}, {4, 4}, {4, 3}, {4, 2}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['p'] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 1}, {1, 4}, {2, 1}, {2, 4}, {3, 2}, {3, 3}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['q'] = {.pixel_locations = {{0, 2}, {0, 3}, {1, 1}, {1, 4}, {2, 1}, {2, 4}, {3, 6}, {3, 5}, {3, 4}, {3, 3}, {3, 2}, {3, 1}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['r'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 2}, {2, 2}, {3, 3}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['s'] = {.pixel_locations = {{0, 2}, {0, 5}, {1, 1}, {1, 3}, {1, 5}, {2, 1}, {2, 3}, {2, 5}, {3, 1}, {3, 3}, {3, 5}, {4, 1}, {4, 4}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['t'] = {.pixel_locations = {{0, 2}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {1, 0}, {2, 5}, {2, 2}, {3, 5}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['u'] = {.pixel_locations = {{0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 5}, {2, 5}, {3, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['v'] = {.pixel_locations = {{0, 2}, {0, 1}, {1, 3}, {1, 4}, {2, 5}, {3, 3}, {3, 4}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['w'] = {.pixel_locations = {{0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 5}, {2, 4}, {2, 3}, {2, 2}, {2, 1}, {3, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['x'] = {.pixel_locations = {{0, 5}, {0, 1}, {1, 4}, {1, 2}, {2, 3}, {3, 4}, {3, 2}, {4, 5}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['y'] = {.pixel_locations = {{0, 3}, {0, 2}, {0, 1}, {1, 6}, {1, 4}, {2, 6}, {2, 4}, {3, 6}, {3, 4}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['z'] = {.pixel_locations = {{0, 5}, {0, 1}, {1, 1}, {1, 4}, {1, 5}, {2, 1}, {2, 3}, {2, 5}, {3, 1}, {3, 2}, {3, 5}, {4, 1}, {4, 5}, PIXEL_LOCATION_END}, .symbol_width = 5},

        // Digits
        ['0'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 0}, {1, 4}, {1, 6}, {2, 0}, {2, 3}, {2, 6}, {3, 0}, {3, 2}, {3, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['1'] = {.pixel_locations = {{1, 2}, {2, 1}, {3, 6}, {3, 5}, {3, 4}, {3, 3}, {3, 2}, {3, 1}, {3, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['2'] = {.pixel_locations = {{0, 6}, {0, 1}, {1, 0}, {1, 5}, {1, 6}, {2, 0}, {2, 4}, {2, 6}, {3, 0}, {3, 3}, {3, 6}, {4, 1}, {4, 2}, {4, 6}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['3'] = {.pixel_locations = {{0, 5}, {0, 1}, {1, 6}, {1, 3}, {1, 0}, {2, 6}, {2, 3}, {2, 0}, {3, 6}, {3, 3}, {3, 0}, {4, 1}, {4, 2}, {4, 4}, {4, 5}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['4'] = {.pixel_locations = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4}, {4, 5}, {4, 6}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['5'] = {.pixel_locations = {{0, 6}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 6}, {1, 3}, {1, 0}, {2, 6}, {2, 3}, {2, 0}, {3, 6}, {3, 3}, {3, 0}, {4, 0}, {4, 4}, {4, 5}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['6'] = {.pixel_locations = {{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 6}, {1, 3}, {1, 0}, {2, 6}, {2, 3}, {2, 0}, {3, 6}, {3, 3}, {3, 0}, {4, 4}, {4, 5}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['7'] = {.pixel_locations = {{0, 0}, {1, 0}, {2, 6}, {2, 5}, {2, 0}, {3, 4}, {3, 3}, {3, 0}, {4, 0}, {4, 1}, {4, 2}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['8'] = {.pixel_locations = {{0, 1}, {0, 2}, {0, 4}, {0, 5}, {1, 6}, {1, 3}, {1, 0}, {2, 6}, {2, 3}, {2, 0}, {3, 6}, {3, 3}, {3, 0}, {4, 1}, {4, 2}, {4, 4}, {4, 5}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['9'] = {.pixel_locations = {{0, 1}, {0, 2}, {0, 5}, {1, 6}, {1, 3}, {1, 0}, {2, 6}, {2, 3}, {2, 0}, {3, 6}, {3, 3}, {3, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4}, {4, 5}, PIXEL_LOCATION_END}, .symbol_width = 5},

        // Special characters
        ['!'] = {.pixel_locations = {{1, 6}, {1, 3}, {1, 2}, {1, 1}, {1, 0}, PIXEL_LOCATION_END}, .symbol_width = 2},
        ['?'] = {.pixel_locations = {{0, 1}, {1, 0}, {2, 6}, {2, 4}, {2, 0}, {3, 3}, {3, 0}, {4, 1}, {4, 2}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['&'] = {.pixel_locations = {{0, 1}, {0, 2}, {0, 4}, {0, 5}, {1, 6}, {1, 3}, {1, 0}, {2, 6}, {2, 3}, {2, 0}, {3, 1}, {3, 2}, {3, 4}, {3, 5}, {4, 6}, {4, 3}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['.'] = {.pixel_locations = {{1, 6}, PIXEL_LOCATION_END}, .symbol_width = 2},
        [','] = {.pixel_locations = {{1, 6}, {2, 5}, PIXEL_LOCATION_END}, .symbol_width = 3},
        [':'] = {.pixel_locations = {{1, 6}, {1, 2}, PIXEL_LOCATION_END}, .symbol_width = 2},
        [';'] = {.pixel_locations = {{1, 6}, {2, 5}, {2, 2}, PIXEL_LOCATION_END}, .symbol_width = 3},
        ['('] = {.pixel_locations = {{1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {2, 6}, {2, 0}, PIXEL_LOCATION_END}, .symbol_width = 3},
        [')'] = {.pixel_locations = {{1, 6}, {1, 0}, {2, 5}, {2, 4}, {2, 3}, {2, 2}, {2, 1}, PIXEL_LOCATION_END}, .symbol_width = 3},
        ['/'] = {.pixel_locations = {{0, 6}, {1, 5}, {2, 4}, {3, 3}, {4, 2}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['+'] = {.pixel_locations = {{0, 3}, {1, 3}, {2, 5}, {2, 4}, {2, 3}, {2, 2}, {2, 1}, {3, 3}, {4, 3}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['-'] = {.pixel_locations = {{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['='] = {.pixel_locations = {{0, 4}, {0, 2}, {1, 4}, {1, 2}, {2, 4}, {2, 2}, {3, 4}, {3, 2}, {4, 4}, {4, 2}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['<'] = {.pixel_locations = {{0, 3}, {1, 4}, {1, 2}, {2, 5}, {2, 1}, PIXEL_LOCATION_END}, .symbol_width = 3},
        ['>'] = {.pixel_locations = {{0, 5}, {0, 1}, {1, 4}, {1, 2}, {2, 3}, PIXEL_LOCATION_END}, .symbol_width = 3},
        ['@'] = {.pixel_locations = {{0, 2}, {0, 3}, {0, 4}, {0, 5}, {1, 1}, {1, 6}, {2, 0}, {2, 5}, {3, 0}, {3, 2}, {3, 4}, {3, 6}, {4, 0}, {4, 2}, {4, 4}, {4, 6}, {5, 1}, {5, 2}, {5, 3}, {5, 4}, {5, 5}, {5, 6}, PIXEL_LOCATION_END}, .symbol_width = 6},
        ['%'] = {.pixel_locations = {{0, 5}, {0, 0}, {0, 1}, {1, 4}, {1, 0}, {1, 1}, {2, 3}, {3, 2}, {3, 5}, {3, 6}, {4, 1}, {4, 5}, {4, 6}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['#'] = {.pixel_locations = {{0, 4}, {0, 2}, {1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {2, 4}, {2, 2}, {3, 5}, {3, 4}, {3, 3}, {3, 2}, {3, 1}, {4, 4}, {4, 2}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['"'] = {.pixel_locations = {{0, 0}, {0, 1}, {0, 2}, {2, 0}, {2, 1}, {2, 2}, PIXEL_LOCATION_END}, .symbol_width = 3},
        ['$'] = {.pixel_locations = {{0, 6}, {0, 2}, {0, 1}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {2, 0}, {2, 3}, {2, 6}, {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {4, 5}, {4, 4}, {4, 0}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['_'] = {.pixel_locations = {{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, PIXEL_LOCATION_END}, .symbol_width = 5},
        ['['] = {.pixel_locations = {{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 0}, {1, 6}, {2, 0}, {2, 6}, {3, 0}, {3, 6}, PIXEL_LOCATION_END}, .symbol_width = 4},
        [']'] = {.pixel_locations = {{0, 0}, {0, 6}, {1, 0}, {1, 6}, {2, 0}, {2, 6}, {3, 6}, {3, 5}, {3, 4}, {3, 3}, {3, 2}, {3, 1}, {3, 0}, PIXEL_LOCATION_END}, .symbol_width = 4},
        ['^'] = {.pixel_locations = {{0, 2}, {1, 1}, {2, 0}, {3, 1}, {4, 2}, PIXEL_LOCATION_END}, .symbol_width = 5},
        [' '] = {.pixel_locations = {PIXEL_LOCATION_END}, .symbol_width = 4},

};
