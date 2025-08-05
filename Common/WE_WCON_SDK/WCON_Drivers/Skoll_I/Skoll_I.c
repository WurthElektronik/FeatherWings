/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK:
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
 * COPYRIGHT (c) 2025 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Skoll-I driver source file.
 */

#include <Skoll_I/Skoll_I.h>
#include <global/global.h>
#include <stdio.h>
#include <string.h>

#define CMD_WAIT_TIME_STEP_MS 0

/**
 * @brief Byte handler to be passed to the UART peripheral
 */
static void Skoll_I_HandleRxByte(uint8_t* dataP, size_t size);
static WE_UART_HandleRxByte_t byteRxCallback = Skoll_I_HandleRxByte;

/**
 * @brief Mode describing how to handle received data when Bluetooth connection is open
 */
typedef enum Skoll_I_DataMode_t
{
    Skoll_I_DataMode_Invalid,
    Skoll_I_DataMode_Command,    /* handle data as commands */
    Skoll_I_DataMode_Transparent /* handle data as transparent payload */
} Skoll_I_DataMode_t;

static Skoll_I_DataMode_t datamode = Skoll_I_DataMode_Invalid;

/**
 * @brief Receive buffer to handle several received UART packets at the same time
 */
typedef struct
{
    bool isInUse;
    ezs_packet_t packet;
} Skoll_I_ReceiveBuffer_t;

static Skoll_I_ReceiveBuffer_t receiveBuffer[2];
static ezs_packet_type_t expectedResponseType = EZS_PACKET_TYPE_ANY;

/**
 * @brief Pin configuration struct pointer.
 */
static Skoll_I_Pins_t* Skoll_I_pinsP = NULL;

/**
 * @brief Uart configuration struct pointer.
 */
static WE_UART_t* Skoll_I_uartP = NULL;

/**
 * @brief Event and data handlers
 */
static void (*event_handler)(ezs_packet_t* packet);
static void (*transparentdata_handler)(uint8_t* payload, uint16_t payloadLength);
static uint8_t pending_response = 0;

static bool Skoll_I_TransparentTransmit(uint16_t length, const uint8_t* data);

/**************************************
 *         Static functions           *
 **************************************/

/**
 * @brief Event handler which is the interface between ezs handler and application event handler
 */
static void internal_ezsHandler(ezs_packet_t* packetP)
{
    /* here is the chance to handle the event */
    if ((EZS_PACKET_TYPE_ANY == expectedResponseType) || (expectedResponseType == packetP->packet_type))
    {
        for (unsigned int i = 0; i < (sizeof(receiveBuffer) / sizeof(Skoll_I_ReceiveBuffer_t)); i++)
        {
            if (receiveBuffer[i].isInUse == false)
            {
                memcpy(&receiveBuffer[i].packet, packetP, sizeof(ezs_packet_t));
                receiveBuffer[i].isInUse = true;
            }
        }
    }

    if ((packetP->packet_type == EZS_PACKET_TYPE_RESPONSE) || ((packetP->packet_type == EZS_PACKET_TYPE_EVENT) && (packetP->tbl_index == EZS_IDX_EVT_SYSTEM_ERROR)))
    {
        /* clear pending response flag */
        if (pending_response != 0)
        {
            pending_response--;
        }
    }

    /* forward event/response to application */
    if (event_handler != NULL)
    {
        event_handler(packetP);
    }
}

/**
 * @brief Byte handler which is the interface between ezs handler and application event handler
 */
static void Skoll_I_HandleRxByte(uint8_t* dataP, size_t size)
{
    bool channelOpenState;

    if ((transparentdata_handler != NULL) && (datamode == Skoll_I_DataMode_Transparent) && (Skoll_I_PinIsChannelOpen(&channelOpenState) && channelOpenState))
    {
        /* relay data to transparent rx callback */
        transparentdata_handler(dataP, size);
        return;
    }

    for (; size > 0; size--, dataP++)
    {
        switch (EZSerial_Parse(*dataP))
        {
            case EZS_INPUT_RESULT_NO_HANDLER:
            {
                /* cancel interpretation */
                return;
            }
            case EZS_INPUT_RESULT_BUFFER_OVERFLOW:
            case EZS_INPUT_RESULT_PACKET_COMPLETE:
            case EZS_INPUT_RESULT_UNHANDLED_PACKET:
            case EZS_INPUT_RESULT_INVALID_CHECKSUM:
            case EZS_INPUT_RESULT_IN_PROGRESS:
            default:
            {
                /* go on with remaining bytes */
                break;
            }
        }
    }
}

/**
 * @brief Transmit function which is the interface between ezs transmit function and application transmit function
 */
static ezs_output_result_t appOutput_uarttransmit(uint16_t length, const uint8_t* data)
{
    /* make sure we aren't already waiting for a response */
    if (pending_response != 0)
    {
        /* only one pending response at a time is allowed */
        return EZS_OUTPUT_RESULT_RESPONSE_PENDING;
    }

    /* increment pending response counter */
    pending_response++;

    if ((data == NULL) || (length == 0))
    {
        return EZS_OUTPUT_RESULT_UNRECOGNIZED_COMMAND;
    }
    else if (Skoll_I_uartP->uartTransmit == NULL)
    {
        return EZS_OUTPUT_RESULT_NO_HANDLER;
    }
    return (Skoll_I_uartP->uartTransmit((uint8_t*)data, length) == true) ? EZS_OUTPUT_RESULT_DATA_WRITTEN : EZS_OUTPUT_RESULT_NO_HANDLER;
}

/**
 * @brief Transmit transparent data
 */
static bool Skoll_I_TransparentTransmit(uint16_t length, const uint8_t* data)
{
    if (Skoll_I_uartP->uartTransmit == NULL)
    {
        return false;
    }

    return Skoll_I_uartP->uartTransmit((uint8_t*)data, length);
}

/**************************************
 *         Global functions           *
 **************************************/

/**
 * @brief Function that waits for the return value of Skoll_I, when a command was sent before
 */
ezs_packet_t* Skoll_I_Wait4CNF(ezs_packet_type_t type, uint32_t max_time_ms)
{
    uint32_t t0 = WE_GetTick();

    for (unsigned int i = 0; i < (sizeof(receiveBuffer) / sizeof(Skoll_I_ReceiveBuffer_t)); i++)
    {
        receiveBuffer[i].isInUse = false;
    }
    expectedResponseType = type;

    while (1)
    {
        for (unsigned int i = 0; i < (sizeof(receiveBuffer) / sizeof(Skoll_I_ReceiveBuffer_t)); i++)
        {
            if (receiveBuffer[i].isInUse == true)
            {
                /* we received a packet */

                if ((EZS_PACKET_TYPE_ANY == type) || (type == receiveBuffer[i].packet.packet_type))
                {
                    /* we received the packet we wanted */
                    return &receiveBuffer[i].packet;
                }

                /* that's not the packet we expected */
                receiveBuffer[i].isInUse = false;
            }
        }

        uint32_t now = WE_GetTick();
        if (now - t0 > max_time_ms)
        {
            /* received no correct response within timeout */
            return NULL;
        }

        if (CMD_WAIT_TIME_STEP_MS > 0)
        {
            /* wait */
            WE_Delay(CMD_WAIT_TIME_STEP_MS);
        }
    }
    return NULL;
}

bool Skoll_I_BluetoothClassic_SPPTransparentTransmit(uint16_t length, const uint8_t* data)
{
    bool channelOpenState;

    if (datamode != Skoll_I_DataMode_Transparent)
    {
        /* transparent mode not active */
        return false;
    }
    else if (Skoll_I_PinIsChannelOpen(&channelOpenState) && !channelOpenState)
    {
        /* connection not open */
        return false;
    }

    return Skoll_I_TransparentTransmit(length, data);
}

bool Skoll_I_BluetoothLE_CYSPPTransparentTransmit(uint16_t length, const uint8_t* data)
{
    bool channelOpenState;

    if (datamode != Skoll_I_DataMode_Transparent)
    {
        /* transparent mode not active */
        return false;
    }
    else if (Skoll_I_PinIsChannelOpen(&channelOpenState) && !channelOpenState)
    {
        /* connection not open */
        return false;
    }

    return Skoll_I_TransparentTransmit(length, data);
}

bool Skoll_I_Init(WE_UART_t* uartP, Skoll_I_Pins_t* pinoutP, void (*event_handler_in)(ezs_packet_t* packet), void (*transparentdata_handler_in)(uint8_t* payload, uint16_t payloadLength))
{
    if ((pinoutP == NULL) || (uartP == NULL) || (uartP->uartInit == NULL) || (uartP->uartDeinit == NULL) || (uartP->uartTransmit == NULL))
    {
        return false;
    }

    /* set handlers */
    event_handler = event_handler_in;
    transparentdata_handler = transparentdata_handler_in;

    /* init receive buffer */
    for (unsigned int i = 0; i < (sizeof(receiveBuffer) / sizeof(Skoll_I_ReceiveBuffer_t)); i++)
    {
        receiveBuffer[i].isInUse = false;
    }

    /* define and init the pins */
    Skoll_I_pinsP = pinoutP;
    Skoll_I_pinsP->Skoll_I_Pin_Reset.type = WE_Pin_Type_Output;
    Skoll_I_pinsP->Skoll_I_Pin_Reset.initial_value.output = WE_Pin_Level_High;
    Skoll_I_pinsP->Skoll_I_Pin_SPP.type = WE_Pin_Type_Input;
    Skoll_I_pinsP->Skoll_I_Pin_SPP.initial_value.input_pull = WE_Pin_PullType_No;
    Skoll_I_pinsP->Skoll_I_Pin_Connection.type = WE_Pin_Type_Input;
    Skoll_I_pinsP->Skoll_I_Pin_Connection.initial_value.input_pull = WE_Pin_PullType_No;
    Skoll_I_pinsP->Skoll_I_Pin_Role.type = WE_Pin_Type_Output;
    Skoll_I_pinsP->Skoll_I_Pin_Role.initial_value.output = WE_Pin_Level_High;

    WE_Pin_t pins[sizeof(Skoll_I_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &Skoll_I_pinsP->Skoll_I_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Skoll_I_pinsP->Skoll_I_Pin_SPP, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Skoll_I_pinsP->Skoll_I_Pin_Connection, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Skoll_I_pinsP->Skoll_I_Pin_Role, sizeof(WE_Pin_t));

    if (!WE_InitPins(pins, pin_count))
    {
        /* error */
        return false;
    }

    /* set to GAP peripheral */
    if (false == Skoll_I_BluetoothLE_PinSetRole(Skoll_I_BluetoothLE_Role_Peripheral))
    {
        return false;
    }

    /* init UART */
    Skoll_I_uartP = uartP;
    if (false == Skoll_I_uartP->uartInit(Skoll_I_uartP->baudrate, Skoll_I_uartP->flowControl, Skoll_I_uartP->parity, &byteRxCallback))
    {
        return false;
    }
    WE_Delay(10);

    /* initialize EZ-Serial interface and callbacks */
    EZSerial_Init(internal_ezsHandler, appOutput_uarttransmit, NULL);

#if 1
    return Skoll_I_Configure();
#else
    return Skoll_I_PinReset();
#endif
}

bool Skoll_I_Configure()
{
    /* check if module responds to a ping */
    if (NULL == EZS_SEND_AND_WAIT(ezs_cmd_system_ping(), 25))
    {
        /* failed, maybe the module is still in text mode, therefore set module to binary mode first */
        pending_response = 0;

        /* the first \r\n is terminating the previous misinterpreted ping command */
        const char parsemode_binary[] = "\r\nSPPM,M=01\r\n";
        if (false == Skoll_I_TransparentTransmit((uint16_t)sizeof(parsemode_binary), (const uint8_t*)parsemode_binary))
        {
            /* was not able to transmit data */
            return false;
        }
        WE_Delay(10);
    }

    /* disable echo mode to reduce traffic on the UART */
    if (NULL == EZS_SEND_AND_WAIT(ezs_cmd_protocol_set_echo_mode(0), SKOLL_I_COMMAND_TIMEOUT_MS))
    {
        return false;
    }

    if (transparentdata_handler == NULL)
    {
        /* binary mode, non-transparent when connection is open */
        if (NULL == EZS_SEND_AND_WAIT(ezs_cmd_protocol_set_parse_mode(3), SKOLL_I_COMMAND_TIMEOUT_MS))
        {
            datamode = Skoll_I_DataMode_Invalid;
            return false;
        }
        datamode = Skoll_I_DataMode_Command;
    }
    else
    {
        /* binary mode, transparent when connection is open */
        if (NULL == EZS_SEND_AND_WAIT(ezs_cmd_protocol_set_parse_mode(1), SKOLL_I_COMMAND_TIMEOUT_MS))
        {
            datamode = Skoll_I_DataMode_Invalid;
            return false;
        }
        datamode = Skoll_I_DataMode_Transparent;
    }

    return true;
}

bool Skoll_I_Deinit()
{
    WE_Pin_t pins[sizeof(Skoll_I_Pins_t) / sizeof(WE_Pin_t)];
    uint8_t pin_count = 0;
    memcpy(&pins[pin_count++], &Skoll_I_pinsP->Skoll_I_Pin_Reset, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Skoll_I_pinsP->Skoll_I_Pin_SPP, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Skoll_I_pinsP->Skoll_I_Pin_Connection, sizeof(WE_Pin_t));
    memcpy(&pins[pin_count++], &Skoll_I_pinsP->Skoll_I_Pin_Role, sizeof(WE_Pin_t));

    /* deinit pins */
    if (!WE_DeinitPins(pins, pin_count))
    {
        return false;
    }

    datamode = Skoll_I_DataMode_Invalid;

    /* deinit UART */
    return Skoll_I_uartP->uartDeinit();
}

bool Skoll_I_PinReset()
{
    if (!WE_SetPin(Skoll_I_pinsP->Skoll_I_Pin_Reset, WE_Pin_Level_Low))
    {
        return false;
    }

    WE_Delay(5);

    if (!WE_SetPin(Skoll_I_pinsP->Skoll_I_Pin_Reset, WE_Pin_Level_High))
    {
        return false;
    }

    /* Workaround: boot-up message may be in text mode, and can not be interpreted by this driver which is using binary mode.
	 * All fine so far, thus wait for boot up */
    WE_Delay(SKOLL_I_BOOT_DURATION);

    return Skoll_I_Configure();
}

bool Skoll_I_BluetoothClassic_PinDisconnect()
{
    /* set pin to output */
    WE_Pin_t p;
    memcpy(&p, &Skoll_I_pinsP->Skoll_I_Pin_SPP, sizeof(WE_Pin_t));
    p.type = WE_Pin_Type_Output;
    p.initial_value.output = WE_Pin_Level_High;
    if (!WE_Reconfigure(p))
    {
        return false;
    }

    WE_Delay(10);

    /* reset pin to input */
    if (!WE_Reconfigure(Skoll_I_pinsP->Skoll_I_Pin_SPP))
    {
        return false;
    }

    return NULL != EZS_WAIT_FOR_EVENT(SKOLL_I_COMMAND_TIMEOUT_MS);
}

/* this function first switches to command mode and sends the disconnect command */
bool Skoll_I_BluetoothLE_PinDisconnect(uint8_t conn_handle)
{
    bool ret = true;

    /* set pin to output */
    WE_Pin_t p;
    memcpy(&p, &Skoll_I_pinsP->Skoll_I_Pin_SPP, sizeof(WE_Pin_t));
    p.type = WE_Pin_Type_Output;
    p.initial_value.output = WE_Pin_Level_High;
    if (!WE_Reconfigure(p))
    {
        return false;
    }

    WE_Delay(10);

    if (EZS_SEND_AND_WAIT(ezs_cmd_gap_disconnect(conn_handle), SKOLL_I_COMMAND_TIMEOUT_MS) == (ezs_packet_t*)NULL)
    {
        ret = false;
    }

    /* SPP pin to input */
    if (!WE_Reconfigure(Skoll_I_pinsP->Skoll_I_Pin_SPP))
    {
        return false;
    }

    return ret;
}

bool Skoll_I_PinIsConnectionOpen(bool* connectionOpenStateP)
{
    if (connectionOpenStateP == NULL)
    {
        return false;
    }

    WE_Pin_Level_t pin_level;

    if (!WE_GetPinLevel(Skoll_I_pinsP->Skoll_I_Pin_Connection, &pin_level))
    {
        return false;
    }

    *connectionOpenStateP = (pin_level == WE_Pin_Level_Low);

    return true;
}

bool Skoll_I_PinIsChannelOpen(bool* channelOpenStateP)
{
    if (channelOpenStateP == NULL)
    {
        return false;
    }

    WE_Pin_Level_t pin_level;

    if (!WE_GetPinLevel(Skoll_I_pinsP->Skoll_I_Pin_SPP, &pin_level))
    {
        return false;
    }

    *channelOpenStateP = (pin_level == WE_Pin_Level_Low);

    return true;
}

bool Skoll_I_BluetoothLE_PinSetRole(Skoll_I_BluetoothLE_Role_t role)
{
    switch (role)
    {
        case Skoll_I_BluetoothLE_Role_Peripheral:
            if (!WE_SetPin(Skoll_I_pinsP->Skoll_I_Pin_Role, WE_Pin_Level_High))
            {
                return false;
            }
            break;
        case Skoll_I_BluetoothLE_Role_Central:
            if (!WE_SetPin(Skoll_I_pinsP->Skoll_I_Pin_Role, WE_Pin_Level_Low))
            {
                return false;
            }
            break;
        default:
            return false;
    }

    return true;
}
