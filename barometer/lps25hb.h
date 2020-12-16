/*
 * lps25hb.h
 *
 *  Created on: 3. 12. 2020
 *      Author: Asus
 */

#ifndef LPS25HB_H_
#define LPS25HB_H_

#include "main.h"
#include "i2c.h"
#include "math.h"

/* Device address */
#define 	LPS25HB_DEVICE_ADDRESS_0		0xB8
#define 	LPS25HB_DEVICE_ADDRESS_1		0xBA

/* Reference pressure registers */
#define		LPS25HB_REF_P_XL_ADDRESS		0x08
#define		LPS25HB_REF_P_XL_VALUE			0x00

#define		LPS25HB_REF_P_L_ADDRESS			0x09
#define		LPS25HB_REF_P_L_VALUE			0x00

#define		LPS25HB_REF_P_H_ADDRESS			0x0A
#define		LPS25HB_REF_P_H_VALUE			0x00

/* Who am I register */
#define		LPS25HB_WHO_AM_I_ADDRESS		0x0F
#define		LPS25HB_WHO_AM_I_VALUE			0xBD	// 0b10111101

/* Resolution register */
#define		LPS25HB_RES_CONF_ADDRESS		0x10
#define		LPS25HB_RES_CONF_VALUE			0x0F	// 0b00001111

/* Control registers */
#define		LPS25HB_CTRL_REG1_ADDRESS		0x20
#define		LPS25HB_CTRL_REG1_VALUE			0x00

#define		LPS25HB_CTRL_REG2_ADDRESS		0x21
#define		LPS25HB_CTRL_REG2_VALUE			0x00

#define		LPS25HB_CTRL_REG3_ADDRESS		0x22
#define		LPS25HB_CTRL_REG3_VALUE			0x00

#define		LPS25HB_CTRL_REG4_ADDRESS		0x23
#define		LPS25HB_CTRL_REG4_VALUE			0x00

/* Interupt registers */
#define		LPS25HB_INTERRUPT_CFG_ADDRESS	0x24
#define		LPS25HB_INTERRUPT_CFG_VALUE		0x00

#define		LPS25HB_INT_SOURCE_ADDRESS		0x25
#define		LPS25HB_INT_SOURCE_VALUE		0x00

/* status register */
#define		LPS25HB_STATUS_REG_ADDRESS		0x27
#define		LPS25HB_STATUS_REG_VALUE		0x00

/* Pressure output registers */
#define		LPS25HB_PRESS_OUT_XL_ADDRESS	0x28
#define		LPS25HB_PRESS_OUT_L_ADDRESS		0x29
#define		LPS25HB_PRESS_OUT_H_ADDRESS		0x2A

/* Temperature output registers */
#define		LPS25HB_TEMP_OUT_L_ADDRESS		0x2B
#define		LPS25HB_TEMP_OUT_H_ADDRESS		0x2C

/* FIFO configure registers */
#define		LPS25HB_FIFO_CTRL_ADDRESS		0x2E
#define		LPS25HB_FIFO_CTRL_VALUE			0x00

#define		LPS25HB_FIFO_STATUS_ADDRESS		0x2F
#define		LPS25HB_FIFO_STATUS_VALUE		0x20

/* Pressure threshold registers */
#define		LPS25HB_THS_P_L_ADDRESS			0x30
#define		LPS25HB_THS_P_L_VALUE			0x00
#define		LPS25HB_THS_P_H_ADDRESS			0x31
#define		LPS25HB_THS_P_H_VALUE			0x00

/* Pressure offset registers */
#define		LPS25HB_RPDS_L_ADDRESS			0x39
#define		LPS25HB_RPDS_L_VALUE			0x00
#define		LPS25HB_RPDS_H_ADDRESS			0x3A
#define		LPS25HB_RPDS_H_VALUE			0x00

uint8_t lps25hb_init(void);
void lps25hb_write_byte(uint8_t, uint8_t);
void lps25hb_read(uint8_t *, uint8_t, uint8_t, uint8_t);
void lps25hb_get_temperature(float *);
void lps25hb_get_pressure(float *);
void lps25hb_get_altitude(float *);

#endif /* LPS25HB_H_ */
