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
#include "ICLED_demos.h"
#include "ICLED.h"
#include "global.h"

static uint16_t alphabet_current_column = 0;
static uint16_t rainbow_current_column = 0;
static uint16_t temp_sensor_data_current_column = 0;
static uint16_t hum_sensor_data_current_column = 0;
static uint16_t ble_data_current_column = 0;
static uint16_t prices_current_column = 0;
static uint16_t show_current_column = 0;

bool ICLED_demo_send_alphabet(uint16_t R_H, uint16_t G_S, uint16_t B_V, uint8_t brightness, uint32_t delay, bool *running)
{
    uint16_t place = ICLED_COLUMNS;

    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789 !?&.,():;/+-=<>@#%$_^[]";

    if (!ICLED_set_string(alphabet, &place, R_H, G_S, B_V, brightness, false))
    {
        return false;
    }

    return ICLED_start_conditional_loop(0, place, delay, &alphabet_current_column, running);
}

bool ICLED_demo_show_rainbow(uint8_t brightness, uint32_t delay, bool *running)
{
    ICLED_Color_System color_system = ICLED_get_color_system();

    ICLED_set_color_system(RGB);

    uint8_t step = (uint8_t)(UINT8_MAX / (uint8_t)ICLED_COLUMNS); // fade frome one base color to an
                                                                  // other in one screen width

    // Red --> green
    for (uint8_t k = 0; k < ICLED_COLUMNS; k++)
    {
        for (uint8_t i = 0; i < ICLED_ROWS; i++)
        {
            if (!ICLED_set_expanded_screen_pixel(i, k, step * k, UINT8_MAX - (step * k), 0, brightness, false))
            {
                ICLED_set_color_system(color_system);
                return false;
            }
        }
    }

    // green --> blue
    for (uint8_t k = 0; k < ICLED_COLUMNS; k++)
    {
        for (uint8_t i = 0; i < ICLED_ROWS; i++)
        {
            if (!ICLED_set_expanded_screen_pixel(i, k + ICLED_COLUMNS, UINT8_MAX - (step * k), 0, step * k,
                                                 brightness, false))
            {
                ICLED_set_color_system(color_system);
                return false;
            }
        }
    }
    // blue --> red
    for (uint8_t k = 0; k < ICLED_COLUMNS; k++)
    {
        for (uint8_t i = 0; i < ICLED_ROWS; i++)
        {
            if (!ICLED_set_expanded_screen_pixel(i, k + (2 * ICLED_COLUMNS), 0, step * k, UINT8_MAX - (step * k),
                                                 brightness, false))
            {
                ICLED_set_color_system(color_system);
                return false;
            }
        }
    }
    // red --> green again for scmooth change on restart
    for (uint8_t k = 0; k < ICLED_COLUMNS; k++)
    {
        for (uint8_t i = 0; i < ICLED_ROWS; i++)
        {
            if (!ICLED_set_expanded_screen_pixel(i, k + (3 * ICLED_COLUMNS), step * k, UINT8_MAX - (step * k), 0,
                                                 brightness, false))
            {
                ICLED_set_color_system(color_system);
                return false;
            }
        }
    }

    ICLED_set_color_system(color_system);

    return ICLED_start_conditional_loop(0, (3 * ICLED_COLUMNS), delay, &rainbow_current_column, running);
}

/**
 * @brief writes a WE Logo in LEDBuf
 *
 * @param[in] place: left boundry of logo
 * @param[in] brightness: Brightness
 *
 * @return Right boundry of logo with 1 collumn space
 */
bool ICLED_set_WElogo(uint16_t *place, uint8_t brightness)
{
    if (place == NULL)
    {
        return false;
    }

    if (*place > (ICLED_SCREENSTORUN * ICLED_COLUMNS) - 14)
    {
        return false;
    }

    ICLED_Color_System color_system = ICLED_get_color_system();

    ICLED_set_color_system(RGB);

    uint8_t we_logo[ICLED_COLUMNS][ICLED_ROWS][3] =
        {
            {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}},
            {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}},
            {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}},
            {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 255, 255}, {255, 0, 0}},
            {{255, 0, 0}, {255, 0, 0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 0, 0}, {255, 0, 0}},
            {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 255, 255}, {255, 0, 0}},
            {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}},
            {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}},
            {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}},
            {{255, 255, 255}, {255, 0, 0}, {255, 0, 0}, {255, 255, 255}, {255, 0, 0}, {255, 0, 0}, {255, 255, 255}},
            {{255, 255, 255}, {255, 0, 0}, {255, 0, 0}, {255, 255, 255}, {255, 0, 0}, {255, 0, 0}, {255, 255, 255}},
            {{255, 255, 255}, {255, 0, 0}, {255, 0, 0}, {255, 255, 255}, {255, 0, 0}, {255, 0, 0}, {255, 255, 255}},
            {{255, 255, 255}, {255, 0, 0}, {255, 0, 0}, {255, 255, 255}, {255, 0, 0}, {255, 0, 0}, {255, 255, 255}},
            {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}},
            {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}},
        };

    for (uint8_t i = 0; i < ICLED_COLUMNS; i++)
    {
        for (uint8_t j = 0; j < ICLED_ROWS; j++)
        {
            if (!ICLED_set_expanded_screen_pixel(j, i + *place, we_logo[i][j][0], we_logo[i][j][1], we_logo[i][j][2], brightness, false))
            {
                ICLED_set_color_system(color_system);
                return false;
            }
        }
    }

    if (!ICLED_set_start_column(*place))
    {
        ICLED_set_color_system(color_system);
        return false;
    }

    ICLED_set_color_system(color_system);

    *place = *place + 16;

    return true;
}

bool ICLED_demo_show_temp_sensor_data(float temperature, uint8_t brightness, uint32_t delay, bool *running)
{
    ICLED_Color_System color_system = ICLED_get_color_system();

    ICLED_set_color_system(RGB);

    uint16_t place = ICLED_COLUMNS;

    char string[32];

    sprintf(string, "T:%.1f", temperature);

    uint16_t R_H, G_S, B_V;

    // choose color depending on temperature
    if (temperature <= -20)
    {
        R_H = 0;
        G_S = 0;
        B_V = 50;
    }
    else if (temperature >= 50)
    {
        R_H = 50;
        G_S = 0;
        B_V = 0;
    }
    else if (temperature < 20)
    {
        R_H = (uint8_t)1.25 * (temperature + 20);
        G_S = (uint8_t)1.25 * (temperature + 20);
        B_V = 50;
    }
    else if (temperature > 30)
    {
        R_H = 50;
        G_S = (uint8_t)2.5 * (50 - temperature);
        B_V = 0;
    }
    else if (temperature > 20)
    {
        R_H = 50;
        G_S = 50;
        B_V = (uint8_t)5 * (30 - temperature);
    }
    else
    {
        R_H = 40;
        G_S = 40;
        B_V = 40;
    }

    if (!ICLED_set_string(string, &place, R_H, G_S, B_V, brightness, false))
    {
        return false;
    }

    ICLED_set_expanded_screen_pixel(0, place, R_H, G_S, B_V, brightness, false);
    ICLED_set_expanded_screen_pixel(1, place, R_H, G_S, B_V, brightness, false);
    ICLED_set_expanded_screen_pixel(2, place++, R_H, G_S, B_V, brightness, false);
    ICLED_set_expanded_screen_pixel(0, place, R_H, G_S, B_V, brightness, false);
    ICLED_set_expanded_screen_pixel(2, place++, R_H, G_S, B_V, brightness, false);
    ICLED_set_expanded_screen_pixel(0, place, R_H, G_S, B_V, brightness, false);
    ICLED_set_expanded_screen_pixel(1, place, R_H, G_S, B_V, brightness, false);
    ICLED_set_expanded_screen_pixel(2, place++, R_H, G_S, B_V, brightness, false);
    ICLED_set_char('C', &(++place), R_H, G_S, B_V, brightness, false);

    ICLED_set_color_system(color_system);

    return ICLED_start_conditional_loop(0, place, delay, &temp_sensor_data_current_column, running);
}

bool ICLED_demo_show_hum_sensor_data(float humidity, uint8_t brightness, uint32_t delay, bool *running)
{

    ICLED_Color_System color_system = ICLED_get_color_system();

    ICLED_set_color_system(RGB);

    uint16_t place = ICLED_COLUMNS;

    char string[32];

    sprintf(string, "H:%.1f%%", humidity);

    uint16_t R_H, G_S, B_V;

    if (humidity <= 20)
    {
        R_H = 50;
        G_S = 0;
        B_V = 0;
    }
    else if (humidity >= 80)
    {
        R_H = 0;
        G_S = 0;
        B_V = 50;
    }
    else if (humidity >= 60)
    {
        R_H = 0;
        G_S = (uint8_t)(50 - 2.5 * (humidity - 60));
        B_V = 50;
    }
    else if (humidity > 50)
    {
        R_H = (uint8_t)(50 - 5 * (humidity - 40));
        G_S = 50;
        B_V = 50;
    }
    else if (humidity <= 40)
    {
        R_H = 50;
        G_S = (uint8_t)(2.5 * (humidity - 20));
        B_V = 0;
    }
    else if (humidity < 50)
    {
        R_H = 50;
        G_S = 50;
        B_V = (uint8_t)(5 * (humidity - 40));
    }
    else
    {
        R_H = 50;
        G_S = 50;
        B_V = 50;
    }

    if (!ICLED_set_string(string, &place, R_H, G_S, B_V, brightness, false))
    {
        return false;
    }

    ICLED_set_emoji(Emoji_Droplet, &place, 0, 0, 50, brightness, false);

    ICLED_set_color_system(color_system);

    return ICLED_start_conditional_loop(0, place, delay, &hum_sensor_data_current_column, running);
}

bool ICLED_demo_show_ble_data(char *payload_buffer, uint16_t R_H, uint16_t G_S, uint16_t B_V, uint8_t brightness, uint32_t delay, bool *running)
{
    uint16_t place = ICLED_COLUMNS;
    ICLED_set_string((char *)payload_buffer, &place, R_H, G_S, B_V, brightness, false);
    if (place > (2 * ICLED_COLUMNS) + 1)
    {
        if (ble_data_current_column > place)
        {
            ble_data_current_column = 0;
        }
        return ICLED_start_conditional_loop(0, place, 80, &ble_data_current_column, running);
    }
    else
    {
        return ICLED_set_start_column(ICLED_COLUMNS);
    }
}

bool ICLED_demo_show_price(char *string, uint16_t R_H, uint16_t G_S, uint16_t B_V, uint8_t brightness, uint32_t delay, bool *running)
{
    uint16_t place = ICLED_COLUMNS;
    if (!ICLED_set_string((char *)string, &place, R_H, G_S, B_V, brightness, false))
    {
        return false;
    }

    if (!ICLED_set_euro(&place, R_H, G_S, B_V, brightness, false))
    {
        return false;
    }

    if (place > (2 * ICLED_COLUMNS) + 1)
    {
        if (prices_current_column > place)
        {
            prices_current_column = 0;
        }
        return ICLED_start_conditional_loop(0, place, 80, &prices_current_column, running);
    }
    else
    {
        return ICLED_set_start_column(ICLED_COLUMNS);
    }
}

bool ICLED_demo_start_show(bool *initial_test_run, bool *running, uint8_t *mode)
{
    ICLED_Color_System color_system = ICLED_get_color_system();
    ICLED_set_color_system(RGB);

    uint16_t place = 0;

    char string[] = "More than you expect!";

    if (*initial_test_run)
    {
        *initial_test_run = false;
        for (int i = 0; i < ICLED_ROWS; i++)
        {
            for (int j = 0; j < ICLED_COLUMNS; j++)
            {
                ICLED_clear(false);
                ICLED_set_screen_pixel(i, j, 128, 128, 128, 25);
                if (*mode != 0)
                {
                    return true;
                }
                WE_Delay(80);
            }
        }

        ICLED_clear(false);

        for (int i = 0; i < 25; i++)
        {
            ICLED_set_all_pixels(255, 255, 255, i);
            if (*mode != 0)
            {
                return true;
            }
            WE_Delay(40);
        }

        ICLED_set_expanded_screen_all_pixels(60, 0, 0, 55);

        ICLED_set_WElogo(&place, 10);

        if (*mode != 0)
        {
            return true;
        }

        WE_Delay(250);

        if (*mode != 0)
        {
            return true;
        }
    }

    ICLED_set_expanded_screen_all_pixels(60, 0, 0, 55, false);

    place = ICLED_COLUMNS;

    ICLED_set_string(string, &place, 60, 60, 60, 50, false);

    ICLED_start_conditional_loop(0, place, 80, &show_current_column, running);

    ICLED_set_color_system(color_system);

    return true;
}
