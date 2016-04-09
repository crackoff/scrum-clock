/*
 * mt6116.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: crackoff
 */

extern "C"
{
#include "osapi.h"
void ets_delay_us(int);
}
#include "driver/mt6116.hpp"
#include "driver/gpio16.h"

extern "C" void __cxa_pure_virtual() { while (1); }

Font8x16::Font8x16()
{
}

uint8 Font8x16::GetSymbolWidth()
{
	return 8;
}

void Font8x16::DrawSymbol(uint8* row1, uint8* row2, uint8 row, uint8 offset, uint8 charIndex)
{
	uint8 models[10][16] =
	{
		{ 0, 60, 66, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 66, 60, 0 }, // 0
		{ 0, 4, 12, 20, 36, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0 }, // 1
		{ 0, 60, 66, 129, 129, 1, 1, 2, 4, 8, 16, 32, 64, 128, 255, 0 }, // 2
		{ 0, 60, 66, 129, 1, 1, 2, 12, 2, 1, 1, 1, 129, 66, 60, 0 }, // 3
		{ 0, 6, 10, 10, 18, 18, 34, 34, 66, 66, 130, 255, 2, 2, 2, 0 }, // 4
		{ 0, 254, 128, 128, 128, 128, 188, 194, 1, 1, 1, 129, 129, 66, 60, 0 }, // 5
		{ 0, 60, 66, 129, 128, 128, 188, 194, 129, 129, 129, 129, 129, 66, 60, 0 }, // 6
		{ 0, 255, 1, 2, 2, 4, 4, 8, 8, 16, 16, 32, 32, 64, 64, 0 }, // 7
		{ 0, 60, 66, 129, 129, 129, 66, 60, 66, 129, 129, 129, 129, 66, 60, 0 }, // 8
		{ 0, 60, 66, 129, 129, 129, 129, 129, 67, 61, 1, 1, 129, 66, 60, 0 }, // 9
	};

	this->ModelToRow(0, row1, offset, models[charIndex]);
	this->ModelToRow(1, row2, offset, models[charIndex]);
}

void Font8x16::ModelToRow(uint8 r, uint8* row, uint8 offset, uint8* model)
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            row[7 - j + offset] += ((model[i + 8 * r] >> j) & 1) * (1 << i);
}

Font6x8::Font6x8()
{
}

uint8 Font6x8::GetSymbolWidth()
{
	return 6;
}

void Font6x8::DrawSymbol(uint8* row1, uint8* row2, uint8 row, uint8 offset, uint8 charIndex)
{
	uint8 models[33][6] =
	{
		{ 28, 34, 65, 65, 34, 28 }, // 0
		{ 0, 66, 127, 64, 0, 0 }, // 1
		{ 66, 96, 81, 81, 73, 70 }, // 2
		{ 34, 65, 73, 73, 73, 54 }, // 3
		{ 48, 40, 36, 34, 127, 32 }, // 4
		{ 47, 73, 73, 73, 73, 48 }, // 5
		{ 62, 73, 73, 73, 73, 50 }, // 6
		{ 65, 33, 17, 9, 5, 3 }, // 7
		{ 54, 73, 73, 73, 73, 54 }, // 8
		{ 38, 73, 73, 73, 73, 62 }, // 9
		{ 124, 18, 17, 17, 18, 124 }, // A 10
		{ 127, 73, 73, 77, 74, 48 }, // B 11
		{ 62, 65, 65, 65, 65, 34 }, // C 12
		{ 127, 65, 65, 65, 34, 28}, // D 13
		{ 127, 73, 73, 73, 73, 65 }, // E 14
		{ 127, 9, 9, 9, 9, 1 }, // F 15
		{ 0, 0, 108, 108, 0, 0 }, // : 16
		{ 127, 73, 73, 73, 73, 48 }, // Б 17
		{ 127, 1, 1, 1, 3, 0 }, // Г 18
		{ 192, 126, 65, 65, 126, 192 }, // Д 19
		{ 64, 60, 2, 1, 1, 127 }, // Л 20
		{ 127, 2, 4, 2, 127, 0 }, // М 21
		{ 127, 8, 8, 8, 8, 127 }, // Н 22
		{ 127, 32, 16, 8, 4, 127 }, // И 23
		{ 127, 8, 8, 20, 34, 65 }, // K 24
		{ 127, 1, 1, 1, 1, 127 }, // П 25
		{ 127, 17, 17, 17, 17, 14 }, // Р 26
		{ 1, 1, 127, 1, 1, 0 }, // T 27
		{ 39, 72, 72, 72, 72, 63 }, // У 28
		{ 14, 17, 127, 17, 14, 0 }, // Ф 29
		{ 127, 64, 120, 64, 64, 127 }, // Ш 30
		{ 127, 8, 62, 65, 65, 62 }, // Ю 31
		{ 78, 81, 49, 17, 17, 127 } // Я 32
		/*{ 31, 32, 64, 32, 31, 0 }, // V
		{ 63, 64, 48, 64, 63, 0 }, // W
		{ 62, 65, 65, 73, 73, 58 }, // G
		{ 119, 8, 127, 8, 119, 0 }, // Ж
		{ 48, 64, 64, 65, 63, 1 }, // J
		{ 97, 81, 73, 69, 67, 0 }, // Z
		{ 0, 65, 127, 65, 0, 0 }, // I
		{ 127, 64, 64, 64, 96, 0 }, // L
		{ 127, 2, 4, 8, 16, 127 }, // N
		{ 127, 17, 17, 49, 81, 78 }, // R
		{ 38, 73, 73, 73, 73, 50 }, // S
		{ 63, 64, 64, 64, 64, 63 }, // U
		{ 65, 54, 8, 54, 65, 0 }, // Х
		{ 127, 64, 64, 64, 127, 192 }, // Ц
		{ 15, 16, 16, 16, 16, 127 }, // Ч
		{ 127, 64, 120, 64, 127, 192 }, // Щ
		{ 1, 127, 72, 72, 72, 48 }, // Ъ
		{ 127, 72, 72, 48, 0, 127 }, // Ы
		{ 127, 72, 72, 72, 72, 48 }, // Ь
		{ 34, 65, 73, 73, 73, 62 }, // Э
		{ 0, 0, 96, 96, 0, 0 }, // .
		{ 0, 0, 160, 96, 0, 0 }, // ,
		{ 0, 0, 0, 0, 0, 0 }, // space
		{ 62, 65, 81, 97, 126, 128 }, // Q*/
	};

	uint8* r;
	if (row == 0)
		r = row1;
	else
		r = row2;

	for (int i = 0; i < 6; i++)
		r[offset + i] = models[charIndex][i];
}

MT6116::MT6116(uint8 latchPin, uint8 clockPin, uint8 dataPin, uint8 a0Pin, uint8 ePin, uint8 resetPin)
{
	this->_latchPin = latchPin;
	this->_clockPin = clockPin;
	this->_dataPin = dataPin;
	this->_a0Pin = a0Pin;
	this->_ePin = ePin;
	this->_resetPin = resetPin;
}

void MT6116::Init()
{
	set_gpio_mode(this->_latchPin, GPIO_PULLDOWN, GPIO_OUTPUT);
	set_gpio_mode(this->_clockPin, GPIO_PULLDOWN, GPIO_OUTPUT);
	set_gpio_mode(this->_dataPin, GPIO_PULLDOWN, GPIO_OUTPUT);
	set_gpio_mode(this->_a0Pin, GPIO_PULLDOWN, GPIO_OUTPUT);
	set_gpio_mode(this->_ePin, GPIO_PULLDOWN, GPIO_OUTPUT);
	set_gpio_mode(this->_resetPin, GPIO_PULLDOWN, GPIO_OUTPUT);

	gpio_write(this->_clockPin, 0);
	gpio_write(this->_dataPin, 0);
	gpio_write(this->_ePin, 0);
    gpio_write(this->_resetPin, 0);
    ets_delay_us(20);
    gpio_write(this->_resetPin, 1);
    ets_delay_us(1000);
    this->WriteCode(0xE2); // Reset
    this->WriteCode(0xEE); // ReadModifyWrite off
    this->WriteCode(0xA4); // Usualy mode
    this->WriteCode(0xA8); // Multiplex 1/16
    this->WriteCode(0xC0); // Top row to 0
    this->WriteCode(0xA0); // NonInvert scan RAM
    this->WriteCode(0xAF); // Display on
}

void MT6116::DrawData(uint8 row, uint8 offset, uint8 data)
{
	this->_row[row][offset] = data;
}

void MT6116::DrawLine(Font *font, uint8 row, uint8 offset, uint8* line, uint8 len)
{
	for (int i = 0; i < len; i++)
	{
		uint8 char_offset = offset + i * (font->GetSymbolWidth() + 1);
		font->DrawSymbol(this->_row[0], this->_row[1], row, char_offset, line[i]);
	}
}

void MT6116::Flush()
{
	this->FlushRow(0);
	this->FlushRow(1);
}

void MT6116::Clear()
{
	ets_bzero(this->_row, 61 * 2);
}

void MT6116::ShiftOut(uint8 b)
{
	gpio_write(this->_latchPin, 0);
    for (uint8 j = 0; j < 8; j++, b >>= 1)
    {
    	gpio_write(this->_dataPin, (uint8)b & 1);
        gpio_write(this->_clockPin, 1);
        gpio_write(this->_clockPin, 0);
    }
    gpio_write(this->_latchPin, 1);
}

void MT6116::WaitReady()
{
	ets_delay_us(100);
}

void MT6116::WriteByte(uint8 b, uint8 cd)
{
	this->WaitReady();
	gpio_write(this->_a0Pin, cd);
	this->ShiftOut(b);
	ets_delay_us(50);
	gpio_write(this->_ePin, 1);
	ets_delay_us(170);
	gpio_write(this->_ePin, 0);
	ets_delay_us(1800);
}

void MT6116::WriteCode(uint8 b)
{
	this->WriteByte(b, 0);
}

void MT6116::WriteData(uint8 b)
{
	this->WriteByte(b, 1);
}

void MT6116::FlushRow(uint8 row)
{
	this->WriteCode(0xB8 | row);
	this->WriteCode(0x00);
	for (int i = 0; i < 61; i++)
	    this->WriteData(this->_row[row][i]);
}

