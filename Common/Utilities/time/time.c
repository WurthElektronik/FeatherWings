/**
 * \file
 * \brief API implementation for the Gateway of the WE IoT desin kit.
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

#include "time.h"

// leap year calculator expects year argument as years offset from 1970
#define LEAP_YEAR(Y) (((1970 + (Y)) > 0) && !((1970 + (Y)) % 4) && (((1970 + (Y)) % 100) || !((1970 + (Y)) % 400)))

static const uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // API starts months from 1, this array starts from 0

/**
 * @brief  Convert time to unix time stamp
 * @param  time pointer to time
 * @retval UNIX time stamp
 */
unsigned long long Time_ConvertToUnix(Timestamp *time)
{
    unsigned long long seconds = 0;
    int unixYear = 0;
    /* Unit epoch starts in 1970 */
    unixYear = time->year - 1970;
    seconds = unixYear * (365 * SECONDS_PER_DAY);

    for (int i = 0; i < unixYear; i++)
    {
        if (LEAP_YEAR(i))
        {
            seconds += SECONDS_PER_DAY; // add extra days for leap years
        }
    }

    // add days for this year, months start from 1
    for (int i = 1; i < time->month; i++)
    {
        seconds += SECONDS_PER_DAY * monthDays[i - 1];

        /* if february of a leap year, add one extra day  */
        if ((i == 2) && LEAP_YEAR(unixYear))
        {
            seconds += SECONDS_PER_DAY;
        }
    }

    seconds += (time->day - 1) * SECONDS_PER_DAY;
    seconds += time->hour * SECONDS_PER_HOUR;
    seconds += time->minute * SECONDS_PER_MINUTES;
    seconds += time->second;

    return seconds;
}
/**
 * @brief  Initialize time stamp
 * @param  time pointer to time
 * @retval none
 */
void Timer_initTime(Timestamp *time)
{
    time->year = 1970;
    time->month = 1;
    time->day = 1;
    time->hour = 0;
    time->minute = 0;
    time->second = 0;
}
