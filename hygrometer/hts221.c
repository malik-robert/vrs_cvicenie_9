#include "hts221.h"

uint8_t addres = HTS221_DEVICE_ADDRESS_0;
float g_hts221_temperature = 0;
float g_hts221_relative_humidity = 0;

void hts221_readArray(uint8_t * data, uint8_t reg, uint8_t length)
{
	i2c_master_read(data, length, reg, addres, 1);
}

uint8_t hts221_read_byte(uint8_t reg_addr)
{
	uint8_t data = 0;
	return *(i2c_master_read(&data, 1, reg_addr, addres, 0));
}

void hts221_get_temp_calibration()
{
	uint8_t T0_degC_x8  = hts221_read_byte(HTS221_ADDRESS_T0_degC_x8);
	uint8_t T1_degC_x8  = hts221_read_byte(HTS221_ADDRESS_T1_degC_x8);
	uint8_t T1_T0msb    = hts221_read_byte(HTS221_ADDRESS_T1_T0_MSB);

	uint8_t data[4];
	hts221_readArray(data, HTS221_ADDRESS_T0_OUT_1, 4);

	int16_t T0_OUT = (int16_t)(data[1] << 8) | data[0];
	int16_t T1_OUT = (int16_t)(data[3] << 8) | data[2];

	float T0_degC = (T0_degC_x8 + (1 << 8) * (T1_T0msb & 0b00000011)) / 8.0;
	float T1_degC = (T1_degC_x8 + (1 << 6) * (T1_T0msb & 0b00001100)) / 8.0;

	/*      (a       + (b     - c     ) * (d       - a      ) / (e      - c     ))
			(T0_degC + (T_OUT - T0_OUT) * (T1_degC - T0_degC) / (T1_OUT - T0_OUT));

			 	(d - a)				a+c(a-d)
			A =	-------			B = --------
				(e - c)				(e - c)
	*/

	hts221data.A = (T1_degC - T0_degC)/(T1_OUT - T0_OUT);
	hts221data.B = T0_degC - T0_OUT*(T1_degC - T0_degC)/(T1_OUT - T0_OUT);
}

void hts221_get_hum_calibration()
{
	uint8_t H0_rH_x2  = hts221_read_byte(HTS221_ADDRESS_H0_rH_x2);
	uint8_t H1_rH_x2  = hts221_read_byte(HTS221_ADDRESS_H1_rH_x2);

	float H0_rH  = H0_rH_x2 / 2.0;
	float H1_rH  = H1_rH_x2 / 2.0;

	uint8_t data[4];
	hts221_readArray(data, HTS221_ADDRESS_H0_T0_OUT_1, 2);
	hts221_readArray(data + 2, HTS221_ADDRESS_H1_T0_OUT_1, 2);

	int16_t H0_T0_OUT = (int16_t)(data[1] << 8) | data[0];
	int16_t H1_T0_OUT = (int16_t)(data[3] << 8) | data[2];

	/*      (a        + (b     - c     )    * (d        - a       ) / (e         - c        ))
			(H0_rH_x2 + (T_OUT - H0_T0_OUT) * (H1_rH_x2 - H0_rH_x2) / (H1_T0_OUT - H0_T0_OUT));

				 	(d - a)				a+c(a-d)
				C =	-------			D = --------
					(e - c)				(e - c)
	*/

	hts221data.C = (H1_rH - H0_rH)/(H1_T0_OUT - H0_T0_OUT);
	hts221data.D = H0_rH - H0_T0_OUT*(H1_rH - H0_rH)/(H1_T0_OUT - H0_T0_OUT);
}

void hts221_get_temperature(float *temp)
{
	hts221_get_temp_data(temp);
	*temp = (*temp)*hts221data.A + hts221data.B;
}

void hts221_get_relative_humidity(float *hum)
{
	hts221_get_hum_data(hum);
	*hum = (*hum)*hts221data.C + hts221data.D;
}


void hts221_get_temp_data(float *data)
{
	uint8_t temp_out_l = hts221_read_byte(HTS221_ADDRESS_TEMP_OUT_L);
	uint8_t temp_out_h = hts221_read_byte(HTS221_ADDRESS_TEMP_OUT_H);

	*data = (int16_t)(temp_out_h << 8) | temp_out_l;
}

void hts221_get_hum_data(float *hum)
{
	uint8_t hum_out_l = hts221_read_byte(HTS221_ADDRESS_HUMIDITY_OUT_L);
	uint8_t hum_out_h = hts221_read_byte(HTS221_ADDRESS_HUMIDITY_OUT_H);

	*hum = (int16_t)(hum_out_h << 8) | hum_out_l;
}


void hts221_write_byte(uint8_t reg_addr, uint8_t value)
{
	i2c_master_write(value, reg_addr, addres, 0);
}

uint8_t hts221_init(void)
{

	uint8_t status = 1;

	//LIS3MDL_ACC_ON;

	LL_mDelay(100);

	uint8_t val = hts221_read_byte(HTS221_WHO_AM_I_ADDRES);

	if(val == HTS221_WHO_AM_I_VALUE)
	{
		status = 1;
	}
	else			//if the device is not found on one address, try another one
	{
		addres = HTS221_DEVICE_ADDRESS_1;
		val = hts221_read_byte(HTS221_WHO_AM_I_ADDRES);
		if(val == HTS221_WHO_AM_I_VALUE)
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

	uint8_t ctrl1 = 0b10000111;

	hts221_write_byte(HTS221_ADDRESS_CTRL1, ctrl1);
	hts221_get_temp_calibration();
	hts221_get_hum_calibration();

	return status;
}
