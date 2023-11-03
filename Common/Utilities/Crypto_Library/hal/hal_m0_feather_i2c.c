/**
 * \file
 * \brief ATCA Hardware abstraction layer for Adafruit M0 feather I2C using Arduino Wire drivers.
 *
 * This code is structured in two parts.  Part 1 is the connection of the ATCA HAL API to the physical I2C
 * implementation. Part 2 is the ASF I2C primitives to set up the interface.
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

#include <string.h>
#include <stdio.h>

#include "cryptoauthlib.h"
#include "hal_m0_feather_i2c.h"

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 * using I2C driver of ASF.
 *
   @{ */

/** \brief initialize an I2C interface using given config
 * \param[in] hal - opaque ptr to HAL data
 * \param[in] cfg - interface configuration
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
    if (I2CInit(cfg->atcai2c.slave_address >> 1) != WE_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }
    I2CSetClock(cfg->atcai2c.baud);
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C post init
 * \param[in] iface  instance
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C send over ASF
 * \param[in] iface     instance
 * \param[in] txdata    pointer to space to bytes to send
 * \param[in] txlength  number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    I2CSetAddress(cfg->atcai2c.slave_address >> 1);

    // for this implementation of I2C with CryptoAuth chips, txdata is assumed to have ATCAPacket format
    // Devices such as ATSHA204A and ATECCx08A require a word address value pre-pended to the packet
    // txdata[0] is using _reserved byte of the ATCAPacket
    if (0xFF != word_address)
    {
        txdata[0] = word_address; // insert the Word Address Value, Command token
        txlength++;               // account for word address value byte.
    }
    if (I2CSend(txdata, txlength) != WE_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C receive function for ASF I2C
 * \param[in]    iface     Device to interact with.
 * \param[out]   rxdata    Data received will be returned here.
 * \param[inout] rxlength  As input, the size of the rxdata buffer.
 *                         As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    I2CSetAddress(cfg->atcai2c.slave_address >> 1);
    int retries = cfg->rx_retries;
    int status = !ATCA_SUCCESS;
    uint16_t rxdata_max_size = *rxlength;

    if (rxdata_max_size < 1)
    {
        return ATCA_SMALL_BUFFER;
    }

    while (retries-- > 0 && status != ATCA_SUCCESS)
    {
        if (I2CReceive(rxdata, *rxlength) != WE_SUCCESS)
        {
            status = ATCA_COMM_FAIL;
        }
        else
        {
            status = ATCA_SUCCESS;
        }
    }
    if (status != ATCA_SUCCESS)
    {
        return status;
    }
    if (rxdata[0] < ATCA_RSP_SIZE_MIN)
    {
        return ATCA_INVALID_SIZE;
    }
    if (rxdata[0] > rxdata_max_size)
    {
        return ATCA_SMALL_BUFFER;
    }

    *rxlength = rxdata[0];
    return ATCA_SUCCESS;
}

/** \brief wake up CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to wakeup
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    I2CSetAddress(0x00);
    //For the wake up pulse to be of sufficient width,  use 100 kHz clock
    I2CSetClock(I2C_CLOCK_SPEED_STANDARD);

    int retries = cfg->rx_retries;
    int status = !ATCA_SUCCESS;
    uint8_t data[4];

    //A send to address 0x00 pulls the SDA low for atleast 60 µS
    //No ACK is expected at this staga and hence no check on returned value
    I2CSend(NULL, 0);

    atca_delay_us(cfg->wake_delay);

    I2CSetAddress(cfg->atcai2c.slave_address >> 1);
    while (retries-- > 0 && status != ATCA_SUCCESS)
    {
        if (I2CReceive(data, 4) == WE_SUCCESS)
        {
            status = ATCA_SUCCESS;
        }
    }
    //Reset to default fast clock
    I2CSetClock(cfg->atcai2c.baud);

    if (status != ATCA_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

    return hal_check_wake(data, 4);
}

/** \brief idle CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to idle
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
    uint8_t cmd[1];

    cmd[0] = ATECC608A_CMD_IDLE;

    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    I2CSetAddress(cfg->atcai2c.slave_address >> 1);

    if (I2CSend(cmd, 1) != WE_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/** \brief sleep CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to sleep
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
    uint8_t cmd[1];

    cmd[0] = ATECC608A_CMD_SLEEP;
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    I2CSetAddress(cfg->atcai2c.slave_address >> 1);

    if (I2CSend(cmd, 1) != WE_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/** \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 * return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_release(void *hal_data)
{
    return ATCA_SUCCESS;
}

/** @} */
