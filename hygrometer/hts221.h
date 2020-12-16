#include "main.h"
#include "i2c.h"

#define 	HTS221_DEVICE_ADDRESS_0					0xBE
#define 	HTS221_DEVICE_ADDRESS_1					0xBF

#define 	HTS221_WHO_AM_I_VALUE					0xBC // 0b10111100
#define 	HTS221_WHO_AM_I_ADDRES					0x0F

#define 	HTS221_ADDRESS_CTRL1					0x20
#define 	HTS221_ADDRESS_CTRL2					0x21
#define 	HTS221_ADDRESS_CTRL3					0x22

/* Humidity */
#define 	HTS221_ADDRESS_HUMIDITY_OUT_L			0x28
#define 	HTS221_ADDRESS_HUMIDITY_OUT_H			0x29

/* Temperature */
#define 	HTS221_ADDRESS_TEMP_OUT_L				0x2A
#define 	HTS221_ADDRESS_TEMP_OUT_H				0x2B

/* Output Registers */
#define 	HTS221_ADDRESS_H_OUT_1					0x28	//singed 16-bit
#define 	HTS221_ADDRESS_H_OUT_2					0x29
#define 	HTS221_ADDRESS_T_OUT_1					0x2A	//singed 16-bit
#define 	HTS221_ADDRESS_T_OUT_2					0x2B

/* Calibration Registers */
#define 	HTS221_ADDRESS_H0_rH_x2					0x30	//unsinged 8-bit
#define 	HTS221_ADDRESS_H1_rH_x2					0x31	//unsinged 8-bit
#define 	HTS221_ADDRESS_T0_degC_x8				0x32	//unsinged 16-bit
#define 	HTS221_ADDRESS_T1_degC_x8				0x33	//unsinged 16-bit

#define 	HTS221_ADDRESS_T1_T0_MSB				0x35	//2x -> unsinged 2-bit
#define 	HTS221_ADDRESS_H0_T0_OUT_1				0x36	//singed 16-bit
#define 	HTS221_ADDRESS_H0_T0_OUT_2				0x37
#define 	HTS221_ADDRESS_H1_T0_OUT_1				0x3A	//singed 16-bit
#define 	HTS221_ADDRESS_H1_T0_OUT_2				0x3B
#define 	HTS221_ADDRESS_T0_OUT_1					0x3C	//singed 16-bit
#define 	HTS221_ADDRESS_T0_OUT_2					0x3D
#define 	HTS221_ADDRESS_T1_OUT_1					0x3E	//singed 16-bit
#define 	HTS221_ADDRESS_T1_OUT_2					0x3F

void hts221_readArray			(uint8_t * data, uint8_t reg, uint8_t length);
void hts221_write_byte			(uint8_t reg_addr, uint8_t value);
uint8_t hts221_read_byte		(uint8_t reg_addr);
uint8_t hts221_init				(void);

/* Temp */
void hts221_get_temp_calibration();
void hts221_get_temp_data		(float *data);
void hts221_get_temperature		(float *temp);

/* Hum */
void hts221_get_hum_calibration	();
void hts221_get_hum_data		(float *data);
void hts221_get_relative_humidity		(float *hum);

struct HTS221Data {
	/* Output Calculated Values*/
	float A;
	float B;
	float C;
	float D;
} hts221data;
