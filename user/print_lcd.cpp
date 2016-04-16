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

void print_lcd_small(uint8* line1, uint8 len1, uint8* line2, uint8 len2, uint8 o1, uint8 o2)
{
	MT6116 mt6116(MT6116_LATCH_PIN, MT6116_CLOCK_PIN, MT6116_DATA_PIN, MT6116_A0_PIN, MT6116_E_PIN, MT6116_RESET_PIN);
	Font6x8 font;

	mt6116.Init();
	mt6116.Clear();
	if (line1 != 0)
		mt6116.DrawLine(&font, 0, o1, line1, len1);
	if (line2 != 0)
		mt6116.DrawLine(&font, 1, o2, line2, len2);
	mt6116.Flush();
}

void print_lcd_loading()
{
	uint8 line[] = { 3, 10, 18, 26, 28, 3, 24, 10 };
	print_lcd_small(line, 8, 0, 0, 3, 0);
}

void print_lcd_turn_on()
{
	uint8 line[] = { 25, 23, 27, 10, 22, 23, 14 };
	print_lcd_small(line, 7, 0, 0, 7, 0);
}

void print_lcd_time(uint8 hour, uint8 minute, uint8 dots, uint8 mon, uint8 day)
{
	uint8 digit[2];
	MT6116 mt6116(MT6116_LATCH_PIN, MT6116_CLOCK_PIN, MT6116_DATA_PIN, MT6116_A0_PIN, MT6116_E_PIN, MT6116_RESET_PIN);
	Font8x16 font8x16;
	Font6x8 font6x8;
	uint8 months[12][3] =
	{
			{32, 22, 11}, {29, 14, 11}, {21, 10, 26}, {10, 25, 26}, {21, 10, 23}, {23, 31, 22},
			{23, 31, 20}, {10, 11, 18}, {12, 14, 22}, {0, 24, 27}, {22, 0, 32}, {19, 14, 24}
	};

	mt6116.Init();
	mt6116.Clear();
	digit[0] = hour / 10;
	digit[1] = hour % 10;
	mt6116.DrawLine(&font8x16, 0, 0, digit, 2);
	if (dots > 0)
	{
		mt6116.DrawData(0, 19, 48);
		mt6116.DrawData(0, 20, 48);
		mt6116.DrawData(1, 19, 12);
		mt6116.DrawData(1, 20, 12);
	}
	digit[0] = minute / 10;
	digit[1] = minute % 10;
	mt6116.DrawLine(&font8x16, 0, 23, digit, 2);

	mt6116.DrawLine(&font6x8, 0, 41, months[mon-1], 3);
	uint8 dat[2];
	dat[0] = day / 10;
	dat[1] = day % 10;
	mt6116.DrawLine(&font6x8, 1, 45, dat, 2);

	mt6116.Flush();
}
