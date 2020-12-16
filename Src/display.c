/*
 * display.c
 *
 *  Created on: Nov 23, 2020
 *      Author: Stancoj
 */

#include "display.h"

extern uint64_t stm32f3xx_it_SysTick;

extern float g_lis3mdl_azimuth;

extern float g_hts221_temperature;
extern float g_hts221_relative_humidity;

extern float g_lps25hb_pressure;
extern float g_lps25hb_altitude;

const float *g_display_quantity_address[NUMBER_OF_MESSAGES] = {
		&g_lis3mdl_azimuth,
		&g_hts221_temperature,
		&g_hts221_relative_humidity,
		&g_lps25hb_pressure,
		&g_lps25hb_altitude
};

const float g_display_quantity_saturation[NUMBER_OF_MESSAGES][2] = {
	{-99.9, 99.9},
	{-99.9, 99.9},
	{0, 99},
	{-9999.99, 9999.99},
	{-9999.9, 9999.9}
};

uint8_t g_display_message_format[NUMBER_OF_MESSAGES][MAX_MESSAGE_SIZE] = {
		"MAG_%04.1f", "TEMP_%04.1f", "HUM_%02.0f", "BAR_%07.2f", "ALT_%06.1f"
};
uint8_t g_display_message[NUMBER_OF_MESSAGES][MAX_MESSAGE_SIZE];
uint8_t g_display_message_idx = 0;

uint8_t g_display_buffer[DISPLAY_SIZE];
uint8_t g_display_buffer_idx = 0;
uint8_t g_dp_flag[4] = {0, 0, 0, 0};

const uint8_t characters[] = {
//    ABCDEFG                7-segment map:
	0b1111110, // 0   "0"   	    AAAA
	0b0110000, // 1   "1"          F    B
	0b1101101, // 2   "2"          F    B
	0b1111001, // 3   "3"           GGGG
	0b0110011, // 4   "4"          E    C
	0b1011011, // 5   "5"          E    C
	0b1011111, // 6   "6"           DDDD
	0b1110000, // 7   "7"
	0b1111111, // 8   "8"
	0b1111011, // 9   "9"

	0b1110111, // 65  'A'
	0b1111111, // 66  'B'
	0b1001110, // 67  'C'
	0b1111100, // 68  'D'
	0b1001111, // 69  'E'
	0b1000111, // 70  'F'
	0b1011110, // 71  'G'
	0b0110111, // 72  'H'
	0b0110000, // 73  'I'
	0b0111100, // 74  'J'
	0b1010111, // 75  'K'
	0b0001110, // 76  'L'
	0b1101010, // 77  'M'
	0b1110110, // 78  'N'
	0b1111110, // 79  'O'
	0b1100111, // 80  'P'
	0b1101011, // 81  'Q'
	0b1101111, // 82  'R'
	0b1011011, // 83  'S'
	0b1000110, // 84  'T'
	0b0111110, // 85  'U'
	0b0111010, // 86  'V'
	0b1011100, // 87  'W'
	0b1001001, // 88  'X'
	0b0101011, // 89  'Y'
	0b1101101, // 90  'Z'

	0b1111101, // 65  'a'
	0b0011111, // 66  'b'
	0b0001101, // 67  'c'
	0b0111101, // 68  'd'
	0b0001100, // 69  'e'
	0b0000111, // 70  'f'
	0b1001101, // 71  'g'
	0b0010111, // 72  'h'
	0b1001100, // 73  'i'
	0b1011000, // 74  'j'
	0b1001011, // 75  'k'
	0b0000110, // 76  'l'
	0b1010101, // 77  'm'
	0b0010101, // 78  'n'
	0b0011101, // 79  'o'
	0b1100111, // 80  'p'
	0b1110011, // 81  'q'
	0b0000101, // 82  'r'
	0b0011000, // 83  's'
	0b0001111, // 84  't'
	0b0011100, // 85  'u'
	0b0011000, // 86  'v'
	0b0101010, // 87  'w'
	0b0001001, // 88  'x'
	0b0111011, // 89  'y'
	0b0001010, // 90  'z'

	0b0000001, // 45  '-'
	0b0001000, // 95  '_'
};

float saturation(float q, float *sat) {
	if (q >= sat[0] && q <= sat[1]) {
		return q;
	}

	if (q < sat[0]) {
		return sat[0];
	}

	if (q > sat[1]) {
		return sat[1];
	}

	return -1;
}

uint8_t length(uint8_t *str) {
	uint8_t n = 0;

	while (str[n] != '\0') {
		n++;
	}

	return n;
}

const uint8_t ascii_to_mask(char character) {
	if ('0' <= character && character <= '9') return characters[character - '0'];
	else if ('A' <= character && character <= 'Z') return characters[character - 'A' + 10];
	else if ('a' <= character && character <= 'z') return characters[character - 'a' + 36];
	else if ('-' == character) return characters[character - '-' + 62];
	else if ('_' == character) return characters[character - '_' + 63];

	return 0;
}

void fill_buffer(uint8_t *buffer, uint8_t buffer_size, uint8_t *str, uint8_t len, uint8_t idx, uint8_t *dp_flag, uint8_t *dp_count) {
	for (uint8_t n = 0; n < buffer_size; n++) {
		dp_flag[n] = 0;
	}
	*dp_count = 0;

	uint8_t m = 0, n = 0;
	while (n < buffer_size) {
		if (str[(m + idx) % len] != '.') {
			buffer[n] = str[(m + idx) % len];

			if (str[(m + 1 + idx) % len] == '.') {
				dp_flag[n] = 1;
				(*dp_count)++;
			}

			n++;
		}

		m++;
	}
}

void displayMessage(uint8_t *msg, uint8_t len) {
	uint8_t dp_count = 0;

  	fill_buffer(g_display_buffer, DISPLAY_SIZE, msg, len, g_display_buffer_idx, g_dp_flag, &dp_count);
  	g_display_buffer_idx += increment(g_display_buffer_idx, len - DISPLAY_SIZE - dp_count);

  	while (msg[g_display_buffer_idx] == '.') {
  		g_display_buffer_idx += increment(g_display_buffer_idx, len - DISPLAY_SIZE - dp_count);
  	}
}

int8_t increment(uint8_t idx, uint8_t max) {
	static int8_t inc = 0;

	if (idx >= max) {
		inc = -1;
	}
	else if (idx <= 0) {
		inc = 1;
	}

	return inc;
}

/*Reset (turn-off) all the segments of display*/
void resetSegments(void)
{
	LL_GPIO_SetOutputPin(SEGMENT_A_PORT, 1 << SEGMENT_A_PIN);
	LL_GPIO_SetOutputPin(SEGMENT_B_PORT, 1 << SEGMENT_B_PIN);
	LL_GPIO_SetOutputPin(SEGMENT_C_PORT, 1 << SEGMENT_C_PIN);
	LL_GPIO_SetOutputPin(SEGMENT_D_PORT, 1 << SEGMENT_D_PIN);
	LL_GPIO_SetOutputPin(SEGMENT_E_PORT, 1 << SEGMENT_E_PIN);
	LL_GPIO_SetOutputPin(SEGMENT_F_PORT, 1 << SEGMENT_F_PIN);
	LL_GPIO_SetOutputPin(SEGMENT_G_PORT, 1 << SEGMENT_G_PIN);
	LL_GPIO_SetOutputPin(SEGMENT_DP_PORT, 1 << SEGMENT_DP_PIN);
}

/*Set (turn-on) all the segments of display*/
void setSegments(void)
{
	LL_GPIO_ResetOutputPin(SEGMENT_A_PORT, 1 << SEGMENT_A_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_B_PORT, 1 << SEGMENT_B_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_C_PORT, 1 << SEGMENT_C_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_D_PORT, 1 << SEGMENT_D_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_E_PORT, 1 << SEGMENT_E_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_F_PORT, 1 << SEGMENT_F_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_G_PORT, 1 << SEGMENT_G_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_DP_PORT, 1 << SEGMENT_DP_PIN);
}

/* Reset (turn-off) all digits*/
void resetDigits(void)
{
	DIGIT_1_OFF;
	DIGIT_2_OFF;
	DIGIT_3_OFF;
	DIGIT_4_OFF;
	DIGIT_TIME_OFF;
}

/* Reset (turn-on) all digits*/
void setDigits(void)
{
	DIGIT_1_ON;
	DIGIT_2_ON;
	DIGIT_3_ON;
	DIGIT_4_ON;
	DIGIT_TIME_ON;
}

void displayCharacter(uint8_t mask, uint8_t dp_flag) {
	LL_GPIO_ResetOutputPin(SEGMENT_A_PORT, ((mask >> 6) & 1) << SEGMENT_A_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_B_PORT, ((mask >> 5) & 1) << SEGMENT_B_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_C_PORT, ((mask >> 4) & 1) << SEGMENT_C_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_D_PORT, ((mask >> 3) & 1) << SEGMENT_D_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_E_PORT, ((mask >> 2) & 1) << SEGMENT_E_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_F_PORT, ((mask >> 1) & 1) << SEGMENT_F_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_G_PORT, ((mask >> 0) & 1) << SEGMENT_G_PIN);
	LL_GPIO_ResetOutputPin(SEGMENT_DP_PORT, dp_flag << SEGMENT_DP_PIN);
}

/*
 * Turns required digit ON
 */
void setDigit(uint8_t pos)
{
	switch(pos)
	{
		case 0:
			DIGIT_1_ON;
			break;
		case 1:
			DIGIT_2_ON;
			break;
		case 2:
			DIGIT_3_ON;
			break;
		case 3:
			DIGIT_4_ON;
			break;
	}
}

void resetDigit(uint8_t pos)
{
	switch(pos)
	{
		case 0:
			DIGIT_1_OFF;
			break;
		case 1:
			DIGIT_2_OFF;
			break;
		case 2:
			DIGIT_3_OFF;
			break;
		case 3:
			DIGIT_4_OFF;
			break;
	}
}

/**
 * Display data in dDisplayData.
 * Sets every digit to display its value and decimal point.
 */
void updateDisplay(void) {
	for(uint8_t i = 0; i < DISPLAY_SIZE; i++) {
		setDigit(i);
		displayCharacter(ascii_to_mask(g_display_buffer[i]), g_dp_flag[i]);

		uint64_t prev_time = stm32f3xx_it_SysTick;
		while ((prev_time + 2) > stm32f3xx_it_SysTick) {};

		resetDigit(i);
		resetSegments();
	}
}
