/*
 * mt6116.hpp
 *
 *  Created on: Apr 4, 2016
 *      Author: crackoff
 */

#ifndef INCLUDE_DRIVER_MT6116_HPP_
#define INCLUDE_DRIVER_MT6116_HPP_

#include "c_types.h"

class Font
{
public:
	virtual void DrawSymbol(uint8 *row1, uint8 *row2, uint8 row, uint8 offset, uint8 charIndex);
	virtual uint8 GetSymbolWidth();
	virtual ~Font();
};

class Font8x16 : public Font
{
public:
	Font8x16();
	~Font8x16();
	void DrawSymbol(uint8 *row1, uint8 *row2, uint8 row, uint8 offset, uint8 charIndex);
	uint8 GetSymbolWidth();
};

class Font5x8 : public Font
{
public:
	Font5x8();
	~Font5x8();
	void DrawSymbol(uint8 *row1, uint8 *row2, uint8 row, uint8 offset, uint8 charIndex);
	uint8 GetSymbolWidth();
};

class MT6116
{
public:
	// Control with 74hc595
	MT6116(uint8 latchPin, uint8 clockPin, uint8 dataPin, uint8 a0Pin, uint8 ePin, uint8 resetPin);
	void DrawLine(Font font, uint8 row, uint8 offset, uint8* line, uint8 len);
	void DrawData(uint8 row, uint8 offset, uint8 data);
	void Flush();

private:
	uint8 _latchPin, _clockPin, _dataPin, _a0Pin, _ePin, _resetPin;
	uint8 _row1[61], _row2[61];
};


#endif /* INCLUDE_DRIVER_MT6116_HPP_ */
