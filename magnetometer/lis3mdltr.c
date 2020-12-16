/*
 * lis3mdltr.c
 *
 *  Created on: 17. 7. 2019
 *      Author: Stancoj
 */


#include "lis3mdltr.h"

#define LIS2_DH_CTRL2_FILTER_SETTINGS		(LIS3MDL_CTRL2_HPM_NORMAL_REF | LIS3MDL_CTRL2_HPCF0)

static uint8_t g_lis3mdl_address = LIS3MDL_DEVICE_ADDRESS;
float g_lis3mdl_temperature = 0;
float g_lis3mdl_magnetic_induction[3] = {0, 0, 0};
float g_lis3mdl_azimuth = 0;

void lis3mdl_write_byte(uint8_t reg_addr, uint8_t value)
{
	i2c_master_write(value, reg_addr, g_lis3mdl_address, 0);
}

void lis3mdl_read(uint8_t * data, uint8_t reg, uint8_t length, uint8_t read_flag)
{
	i2c_master_read(data, length, reg, g_lis3mdl_address, read_flag);
}

void lis3mdl_get_temperature(float *T)
{
	const float offset = 25.0, scale = 8.0;
	uint8_t temp[2];
	lis3mdl_read(temp, LIS3MDL_ADDRESS_TEMP_L, 2, 1);

	*T = offset + ((int16_t)(temp[1] << 8) | temp[0])/scale;
}

void lis3mdl_get_magnetic_induction(float *B)
{
	const float scale = 6842.0;
	uint8_t data[6];

	lis3mdl_read(data, LIS3MDL_ADDRESS_XL, 6, 1);

	B[0] = ((int16_t)(data[1] << 8) | data[0])/scale;
	B[1] = ((int16_t)(data[3] << 8) | data[2])/scale;
	B[2] = ((int16_t)(data[5] << 8) | data[4])/scale;
}

void lis3mdl_get_azimuth(float *phi)
{
	const float scale = 6842.0, phase = 90.0, cf = 180.0/M_PI;
	uint8_t data[6];
	float B[3] = {0, 0, 0};

	lis3mdl_read(data, LIS3MDL_ADDRESS_XL, 6, 1);

	B[0] = ((int16_t)(data[1] << 8) | data[0])/scale;
	B[1] = ((int16_t)(data[3] << 8) | data[2])/scale;
	B[2] = ((int16_t)(data[5] << 8) | data[4])/scale;

	*phi = phase + atan2(B[1], B[0])*cf;
}

uint8_t lis3mdl_init(void)
{
	uint8_t status = 1;

	LL_mDelay(100);

	uint8_t val;
	lis3mdl_read(&val, LIS3MDL_WHO_AM_I_ADDRES, 1, 0);

	if(val == LIS3MDL_WHO_AM_I_VALUE)
	{
		status = 1;
	}
	else			//if the device is not found on one address, try another one
	{
		g_lis3mdl_address = LIS3MDL_DEVICE_ADDRESS_ALTERNATIVE;
		lis3mdl_read(&val, LIS3MDL_WHO_AM_I_ADDRES, 1, 0);
		if(val == LIS3MDL_WHO_AM_I_VALUE)
		{
			status = 1;
		}
		else
		{
			status = 0;
			return status;
		}
	}

	//acc device init

	uint8_t ctrl1 = LIS3MDL_CTRL1_OM_UHP | LIS3MDL_CTRL1_DO_80HZ | (1 << 7);
	lis3mdl_write_byte(LIS3MDL_ADDRESS_CTRL1, ctrl1);

	uint8_t ctrl2 = LIS3MDL_CTRL2_FS_4GAUS;
	lis3mdl_write_byte(LIS3MDL_ADDRESS_CTRL2, ctrl2);

	uint8_t ctrl3 = LIS3MDL_CTRL3_MD_CONTINUES;
	lis3mdl_write_byte(LIS3MDL_ADDRESS_CTRL3, ctrl3);

	uint8_t ctrl4 = LIS3MDL_CTRL4_OMZ_UHP;
	lis3mdl_write_byte(LIS3MDL_ADDRESS_CTRL4, ctrl4);

	return status;
}
