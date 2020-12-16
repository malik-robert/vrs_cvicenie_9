/*
 * lps25hb.c
 *
 *  Created on: 3. 12. 2020
 *      Author: Asus
 */

#include "lps25hb.h"

uint8_t g_lps25hb_address = LPS25HB_DEVICE_ADDRESS_0;
float g_lps25hb_temperature = 0;
float g_lps25hb_pressure = 0;
float g_lps25hb_altitude = 0;

void lps25hb_write_byte(uint8_t reg_addr, uint8_t value)
{
	i2c_master_write(value, reg_addr, g_lps25hb_address, 0);
}

void lps25hb_read(uint8_t *data, uint8_t reg, uint8_t length, uint8_t read_flag)
{
	i2c_master_read(data, length, reg, g_lps25hb_address, read_flag);
}

void lps25hb_get_temperature(float *T)
{
	const float offset = 42.5, scale = 480.0;
	uint8_t data[2];
	lps25hb_read(data, LPS25HB_TEMP_OUT_L_ADDRESS, 2, 1);

	*T = offset + ((int16_t)(data[1] << 8) | data[0])/scale;
}

void lps25hb_get_pressure(float *p)
{
	const float scale = 4096.0;
	uint8_t data[3];

    lps25hb_read(data, LPS25HB_PRESS_OUT_XL_ADDRESS, 3, 1);
	*p = ((int32_t)(data[2] << 16) | (data[1] << 8) | data[0])/scale;
}

void lps25hb_get_altitude(float *h)
{
	const float scale = 4096.0, R = 8.3145, M = 0.029, g = 9.8067, T0 = 288.16, p0 = 1013.25;
	uint8_t data[3];

    lps25hb_read(data, LPS25HB_PRESS_OUT_XL_ADDRESS, 3, 1);
	float p = ((int32_t)(data[2] << 16) | (data[1] << 8) | data[0])/scale;
	*h = R*T0/(M*g)*log(p0/p);
}

uint8_t lps25hb_init(void)
{
	uint8_t status = 1;

	LL_mDelay(100);

	uint8_t val;
	lps25hb_read(&val, LPS25HB_WHO_AM_I_ADDRESS, 1, 0);

	if(val == LPS25HB_WHO_AM_I_VALUE)
	{
		status = 1;
	}
	else			//if the device is not found on one address, try another one
	{
		g_lps25hb_address = LPS25HB_DEVICE_ADDRESS_1;
		lps25hb_read(&val, LPS25HB_WHO_AM_I_ADDRESS, 1, 0);
		if(val == LPS25HB_WHO_AM_I_VALUE)
		{
			status = 1;
		}
		else
		{
			status = 0;
			//return status;
		}
	}

	uint8_t ctrl1 = 0b11000000;
	lps25hb_write_byte(LPS25HB_CTRL_REG1_ADDRESS, ctrl1);

	return status;
}
