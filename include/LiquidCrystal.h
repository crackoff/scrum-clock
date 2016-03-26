/*
 * LiquidCrystal.h
 *
 */

#include "c_types.h"

#ifndef USER_LIQUIDCRYSTAL_H_
#define USER_LIQUIDCRYSTAL_H_

// commands
typedef enum {
  LCD_CLEARDISPLAY = 0x01,
  LCD_RETURNHOME = 0x02,
  LCD_ENTRYMODESET = 0x04,
  LCD_DISPLAYCONTROL = 0x08,
  LCD_CURSORSHIFT = 0x10,
  LCD_FUNCTIONSET = 0x20,
  LCD_SETCGRAMADDR = 0x40,
  LCD_SETDDRAMADDR = 0x80
} LcdCommand;

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LiquidCrystal {
public:
  LiquidCrystal(uint8 rs, uint8 enable, uint8 d4, uint8 d5, uint8 d6, uint8 d7);

  void setCursor(uint8, uint8);
  uint8 print(const char*);
  uint8 print(unsigned long, uint8);
  uint8 print(unsigned long);
  void printEx(const char*, const char*);
  void printEx(const char*, unsigned long);
  void command(uint8);

private:
  uint8 write(uint8);
  void send(uint8, uint8);
  void write4bits(uint8);
  void pulseEnable();

  uint8 _rs_pin; // LOW: command.  HIGH: character.
  uint8 _enable_pin; // activated by a HIGH pulse.
  uint8 _data_pins[4];

  uint8 _row_offsets[2] = { 0x00, 0x40 };
};


#endif /* USER_LIQUIDCRYSTAL_H_ */
