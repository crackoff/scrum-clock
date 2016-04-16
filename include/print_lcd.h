/*
 * print_lcd.h
 *
 *  Created on: Apr 2, 2016
 *      Author: crackoff
 */

#ifndef INCLUDE_PRINT_LCD_H_
#define INCLUDE_PRINT_LCD_H_

#include "c_types.h"
#include "driver/mt6116.hpp"

#define MT6116_LATCH_PIN 2
#define MT6116_CLOCK_PIN 0
#define MT6116_DATA_PIN 1
#define MT6116_A0_PIN 6
#define MT6116_E_PIN 7
#define MT6116_RESET_PIN 5

void init_lcd();
void print_lcd(uint8* line, uint8 len);
void print_lcd_turn_on();
void print_lcd_loading();
void print_lcd_time(uint8 hour, uint8 minute, uint8 dots, uint8 mon, uint8 day);

#endif /* INCLUDE_PRINT_LCD_H_ */
