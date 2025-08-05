
#include <platform.h>
#include <ConfigPlatform.h>

int8_t WE_ReadReg(WE_sensorInterface_t *interface,
                  uint8_t regAdr,
                  uint16_t numBytesToRead,
                  uint8_t *data)
{
    switch (interface->interfaceType)
    {
    case WE_i2c:
        I2CSetAddress(interface->options.i2c.address);
        if (interface->options.i2c.burstMode != 0 || numBytesToRead == 1)
        {
            if (numBytesToRead > 1 && interface->options.i2c.useRegAddrMsbForMultiBytesRead)
            {
                /* Register address most significant bit is used to enable multi bytes read */
                regAdr |= 1 << 7;
            }
            switch (interface->options.i2c.protocol)
            {
            case WE_i2cProtocol_RegisterBased:
                return ReadReg(regAdr, numBytesToRead, data);
            case WE_i2cProtocol_Raw:
                return I2CReceive(data, numBytesToRead);
            default:
                return WE_FAIL;
            }
        }
        else
        {
            uint8_t status = WE_SUCCESS;
            for (uint16_t i = 0; (i < numBytesToRead) && (status == WE_SUCCESS); i++)
            {
                switch (interface->options.i2c.protocol)
                {
                case WE_i2cProtocol_RegisterBased:
                    status = ReadReg(regAdr + i, 1, data + i);
                    break;
                case WE_i2cProtocol_Raw:
                    status = I2CReceive(data + i, 1);
                    break;
                default:
                    return WE_FAIL;
                }
            }
            return status;
        }
    default:
        return WE_FAIL;
    }
}

/* Write a register's content */
int8_t WE_WriteReg(WE_sensorInterface_t *interface,
                   uint8_t regAdr,
                   uint16_t numBytesToWrite,
                   uint8_t *data)
{
    switch (interface->interfaceType)
    {
    case WE_i2c:
        I2CSetAddress(interface->options.i2c.address);
        if (interface->options.i2c.burstMode != 0 || numBytesToWrite == 1)
        {
            switch (interface->options.i2c.protocol)
            {
            case WE_i2cProtocol_RegisterBased:
                return WriteReg(regAdr, numBytesToWrite, data);
            case WE_i2cProtocol_Raw:
                return I2CSend(data, numBytesToWrite);
            default:
                return WE_FAIL;
            }
        }
        else
        {
            uint8_t status = WE_SUCCESS;
            for (uint16_t i = 0; (i < numBytesToWrite) && (status == WE_SUCCESS); i++)
            {
                switch (interface->options.i2c.protocol)
                {
                case WE_i2cProtocol_RegisterBased:
                    status = WriteReg(regAdr, numBytesToWrite, data);
                    break;
                case WE_i2cProtocol_Raw:
                    status = I2CSend(data, numBytesToWrite);
                    break;
                default:
                    return WE_FAIL;
                }
            }
            return status;
        }
    default:
        return WE_FAIL;
    }
}

int8_t WE_isSensorInterfaceReady(WE_sensorInterface_t *interface)
{
    return WE_SUCCESS;
}