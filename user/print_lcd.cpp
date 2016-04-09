/*
 * print_lcd.c
 *
 *  Created on: Apr 2, 2016
 *      Author: crackoff
 */
#include "driver/mt6116.hpp"
#include "print_lcd.h"

void init_lcd()
{
}

void print_lcd(uint8* line, uint8 len)
{
	MT6116 mt6116(MT6116_LATCH_PIN, MT6116_CLOCK_PIN, MT6116_DATA_PIN, MT6116_A0_PIN, MT6116_E_PIN, MT6116_RESET_PIN);
	Font8x16 font8x16;

	mt6116.Init();
	mt6116.Clear();
	mt6116.DrawLine(&font8x16, 0, 0, line, 6);
	mt6116.Flush();
}

void print_lcd_time(uint8 hour, uint8 minute, uint8 dots)
{
	uint8 digit[2];
	MT6116 mt6116(MT6116_LATCH_PIN, MT6116_CLOCK_PIN, MT6116_DATA_PIN, MT6116_A0_PIN, MT6116_E_PIN, MT6116_RESET_PIN);
	Font8x16 font8x16;

	mt6116.Init();
	mt6116.Clear();
	digit[0] = hour / 10;
	digit[1] = hour % 10;
	mt6116.DrawLine(&font8x16, 0, 10, digit, 2);
	if (dots > 0)
	{
		mt6116.DrawData(0, 29, 48);
		mt6116.DrawData(0, 30, 48);
		mt6116.DrawData(1, 29, 12);
		mt6116.DrawData(1, 30, 12);
	}
	digit[0] = minute / 10;
	digit[1] = minute % 10;
	mt6116.DrawLine(&font8x16, 0, 33, digit, 2);
	mt6116.Flush();
}
