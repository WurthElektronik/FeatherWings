/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
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
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Contains global function definitions for the Wireless Connectivity SDK for STM32.
 */

#include "global/global.h"
#include "global/global_types.h"
#include "global_platform_types.h"
#include "ConfigPlatform.h"
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void WE_Platform_Init(void)
    {
        return;
    }

    void WE_Error_Handler(void)
    {
        return;
    }

    bool WE_InitPins(WE_Pin_t pins[], uint8_t numPins)
    {
        /* Sort pins by port */
        for (uint8_t i = 0; i < numPins; i++)
        {
            if (IS_WE_PIN_UNDEFINED(pins[i]))
            {
                /* Unused */
                continue;
            }

            WE_Arduino_Pin_t arduino_pin = (*(WE_Arduino_Pin_t *)pins[i].pin_def);

            switch (pins[i].type)
            {
            case WE_Pin_Type_Output:
            {
                SetPinMode(arduino_pin.pin, OUTPUT);
                switch (pins[i].initial_value.output)
                {
                case WE_Pin_Level_Low:
                {
                    WritePin(arduino_pin.pin, LOW);
                    break;
                }
                case WE_Pin_Level_High:
                {
                    WritePin(arduino_pin.pin, HIGH);
                    break;
                }
                default:
                {
                    return false;
                }
                }
                break;
            }
            case WE_Pin_Type_Input:
            {
                switch (pins[i].initial_value.input_pull)
                {
                case WE_Pin_PullType_No:
                {
                    SetPinMode(arduino_pin.pin, INPUT);
                    break;
                }
                case WE_Pin_PullType_Down:
                {
                    SetPinMode(arduino_pin.pin, INPUT_PULLDOWN);
                    break;
                }
                case WE_Pin_PullType_Up:
                {
                    SetPinMode(arduino_pin.pin, INPUT_PULLUP);
                    break;
                }
                default:
                {
                    return false;
                }
                }
                break;
            }
            default:
            {
                return false;
            }
            }
        }

        return true;
    }

    bool WE_Reconfigure(WE_Pin_t pin)
    {
        if (IS_WE_PIN_UNDEFINED(pin))
        {
            return false;
        }

        if (!WE_DeinitPins(&pin, 1))
        {
            return false;
        }

        if (!WE_InitPins(&pin, 1))
        {
            return false;
        }

        return true;
    }

    bool WE_DeinitPins(WE_Pin_t pins[], uint8_t numPins)
    {

        for (uint8_t i = 0; i < numPins; i++)
        {
            if (IS_WE_PIN_UNDEFINED(pins[i]))
            {
                /* Unused */
                continue;
            }

            WE_Arduino_Pin_t arduino_pin = (*(WE_Arduino_Pin_t *)pins[i].pin_def);

            SetPinMode(arduino_pin.pin, INPUT);
        }

        return true;
    }

    bool WE_SetPin(WE_Pin_t pin, WE_Pin_Level_t out)
    {

        if (IS_WE_PIN_UNDEFINED(pin))
        {
            return false;
        }

        if (pin.type != WE_Pin_Type_Output)
        {
            return false;
        }

        WE_Arduino_Pin_t arduino_pin = (*(WE_Arduino_Pin_t *)pin.pin_def);

        switch (out)
        {
        case WE_Pin_Level_High:
            WritePin(arduino_pin.pin, HIGH);
            break;

        case WE_Pin_Level_Low:
            WritePin(arduino_pin.pin, LOW);
            break;

        default:
            return false;
        }

        return true;
    }

    bool WE_GetPinLevel(WE_Pin_t pin, WE_Pin_Level_t *pin_levelP)
    {
        if (IS_WE_PIN_UNDEFINED(pin) || (pin_levelP == NULL))
        {
            return false;
        }

        WE_Arduino_Pin_t arduino_pin = (*(WE_Arduino_Pin_t *)pin.pin_def);

        uint8_t pinLevel;

        readPin(arduino_pin.pin, &pinLevel);

        switch (pinLevel)
        {
        case LOW:
        {
            *pin_levelP = WE_Pin_Level_Low;
            break;
        }
        case HIGH:
        {
            *pin_levelP = WE_Pin_Level_High;
            break;
        }
        default:
            return false;
        }

        return true;
    }

    uint32_t WE_GetTick()
    {
        return millis();
    }

#ifndef WE_MICROSECOND_TICK
    void WE_DelayMicroseconds(uint32_t sleepForUsec)
    {
        /* Microsecond tick is disabled: round to ms */
        WE_Delay(((sleepForUsec + 500) / 1000));
    }

    uint32_t WE_GetTickMicroseconds()
    {
        /* Microsecond tick is disabled: return ms tick * 1000 */
        return WE_GetTick() * 1000;
    }
#endif /* WE_MICROSECOND_TICK */

#ifdef __cplusplus
}
#endif
