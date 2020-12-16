/*
 * display.c
 *
 *  Created on: Nov 23, 2020
 *      Author: Stancoj
 */

#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "main.h"
#include <stdio.h>
#include <string.h>

#define SEGMENT_A_PIN	((uint8_t) 1)
#define SEGMENT_A_PORT	GPIOA

#define SEGMENT_B_PIN	((uint8_t) 0)
#define SEGMENT_B_PORT	GPIOA

#define SEGMENT_C_PIN	((uint8_t) 8)
#define SEGMENT_C_PORT	GPIOA

#define SEGMENT_D_PIN	((uint8_t) 5)
#define SEGMENT_D_PORT	GPIOB

#define SEGMENT_E_PIN 	((uint8_t) 11)
#define SEGMENT_E_PORT	GPIOA

#define SEGMENT_F_PIN 	((uint8_t) 3)
#define SEGMENT_F_PORT	GPIOA

#define SEGMENT_G_PIN	((uint8_t) 4)
#define SEGMENT_G_PORT	GPIOB

#define SEGMENT_DP_PIN	((uint8_t) 1)
#define SEGMENT_DP_PORT	GPIOB

#define DIGIT_1_ON		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_0)
#define DIGIT_2_ON		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4)
#define DIGIT_3_ON		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_12)
#define DIGIT_4_ON		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_2)
#define DIGIT_TIME_ON	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7)

#define DIGIT_1_OFF		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_0)
#define DIGIT_2_OFF		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4)
#define DIGIT_3_OFF		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_12)
#define DIGIT_4_OFF		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_2)
#define DIGIT_TIME_OFF	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7)

#define DISPLAY_SIZE			4
#define NUMBER_OF_MESSAGES		5
#define MAX_MESSAGE_SIZE		20
#define TRIGGER_FALL			0
#define TRIGGER_RISE			1

float saturation(float, float *);
uint8_t length(uint8_t *);
uint8_t checkButtonState(GPIO_TypeDef *, uint8_t, uint8_t, uint16_t, uint16_t);
const uint8_t ascii_to_mask(char);

void resetDigits(void);
void resetDigit(uint8_t);
void resetSegments(void);
void setDigits(void);
void setDigit(uint8_t);
void setSegments(void);

int8_t increment(uint8_t, uint8_t);
void fill_buffer(uint8_t *, uint8_t, uint8_t*, uint8_t, uint8_t, uint8_t *, uint8_t *);
void displayCharacter(uint8_t, uint8_t);
void displayMessage(uint8_t *, uint8_t);
void updateDisplay(void);

#endif
