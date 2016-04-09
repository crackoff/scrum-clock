/*
 * mt6116.hpp
 *
 *  Created on: Apr 4, 2016
 *      Author: crackoff
 */

#ifndef INCLUDE_DRIVER_MT6116_H_
#define INCLUDE_DRIVER_MT6116_H_

#include "c_types.h"

class Font
{
public:
	virtual void DrawSymbol(uint8* row1, uint8* row2, uint8 row, uint8 offset, uint8 charIndex)=0;
	virtual uint8 GetSymbolWidth()=0;
};

class Font8x16 : public Font
{
public:
	Font8x16();
	void DrawSymbol(uint8* row1, uint8* row2, uint8 row, uint8 offset, uint8 charIndex);
	uint8 GetSymbolWidth();

private:
	void ModelToRow(uint8 r, uint8* row, uint8 offset, uint8* model);
};

class Font6x8 : public Font
{
public:
	Font6x8();
	void DrawSymbol(uint8* row1, uint8* row2, uint8 row, uint8 offset, uint8 charIndex);
	uint8 GetSymbolWidth();
};

class MT6116
{
public:
	// Control with 74hc595
	MT6116(uint8 latchPin, uint8 clockPin, uint8 dataPin, uint8 a0Pin, uint8 ePin, uint8 resetPin);
	void Init();
	void DrawLine(Font *font, uint8 row, uint8 offset, uint8* line, uint8 len);
	void DrawData(uint8 row, uint8 offset, uint8 data);
	void Clear();
	void Flush();

private:
	void ShiftOut(uint8 b);
	void WaitReady();
	void WriteByte(uint8 b, uint8 cd);
	void WriteCode(uint8 b);
	void WriteData(uint8 b);
	void FlushRow(uint8 row);

	uint8 _latchPin, _clockPin, _dataPin, _a0Pin, _ePin, _resetPin;
	uint8 _row[2][61];
};


#endif /* INCLUDE_DRIVER_MT6116_H_ */
