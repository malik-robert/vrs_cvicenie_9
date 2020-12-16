/*
 * lsm6ds0.c
 *
 *  Created on: 17. 11. 2019
 *      Author: Stancoj
 */

#include "lsm6ds0.h"

uint8_t g_lsm6ds0_address = LSM6DS0_DEVICE_ADDRESS_0;
float g_lsm6ds0_temperature = 0;
float g_lsm6ds0_acceleration[3] = {0, 0, 0};

void lsm6ds0_write_byte(uint8_t reg_addr, uint8_t value)
{
	i2c_master_write(value, reg_addr, g_lsm6ds0_address, 0);
}

void lsm6ds0_read(uint8_t *data, uint8_t reg, uint8_t length, uint8_t read_flag)
{
	i2c_master_read(data, length, reg, g_lsm6ds0_address, read_flag);
}

void lsm6ds0_get_temperature(float *T)
{
	const float offset = 89.0, scale = 16.0;
	uint8_t data[2];
	lsm6ds0_read(data, LSM6DS0_ADDRESS_TEMP_L, 2, 1);

	*T = offset + ((int16_t)(data[1] << 8) | data[0])/scale;
}

void lsm6ds0_get_acceleration(float *a)
{
	const float scale = 0.061/1000.0;
	uint8_t data[6];

	lsm6ds0_read(data, LSM6DS0_ADDRESS_ACCX, 6, 1);

	a[0] = ((int16_t)(data[1] << 8) | data[0])*scale;
	a[1] = ((int16_t)(data[3] << 8) | data[2])*scale;
	a[2] = ((int16_t)(data[5] << 8) | data[4])*scale;
}

uint8_t lsm6ds0_init(void)
{
	uint8_t status = 1;

	//LIS3MDL_ACC_ON;

	LL_mDelay(100);

	uint8_t val;
	lsm6ds0_read(&val, LSM6DS0_WHO_AM_I_ADDRESS, 1, 0);

	if(val == LSM6DS0_WHO_AM_I_VALUE)
	{
		status = 1;
	}
	else			//if the device is not found on one address, try another one
	{
		g_lsm6ds0_address = LSM6DS0_DEVICE_ADDRESS_1;
		lsm6ds0_read(&val, LSM6DS0_WHO_AM_I_ADDRESS, 1, 0);
		if(val == LSM6DS0_WHO_AM_I_VALUE)
		{
			status = 1;
		}
		else
		{
			status = 0;
			//return status;
		}
	}

	//acc device init

	uint8_t ctrl1 = 8 << 4; // +-2g res
	lsm6ds0_write_byte(LSM6DS0_ADDRESS_CTRL1, ctrl1);

	return status;
}
