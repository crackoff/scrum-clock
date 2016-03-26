/*
 * LiquidCrystal.cpp
 *
 */

extern "C"
{
#include "osapi.h"
void ets_delay_us(int);
}
#include "LiquidCrystal.h"
#include "driver/gpio16.h"

LiquidCrystal::LiquidCrystal(uint8 rs, uint8 enable, uint8 d4, uint8 d5, uint8 d6, uint8 d7)
{
  _rs_pin = rs;
  _enable_pin = enable;
  _data_pins[0] = d4;
  _data_pins[1] = d5;
  _data_pins[2] = d6;
  _data_pins[3] = d7;

  uint8 _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;

  set_gpio_mode(_rs_pin, GPIO_PULLUP, GPIO_OUTPUT);
  set_gpio_mode(_enable_pin, GPIO_PULLUP, GPIO_OUTPUT);
  for (int i = 0; i < 4; i++)
  {
    set_gpio_mode(_data_pins[i], GPIO_PULLUP, GPIO_OUTPUT);
  }

  ets_delay_us(50000);
  gpio_write(_rs_pin, 0);
  gpio_write(_enable_pin, 0);

  command(LCD_FUNCTIONSET | _displayfunction); // Send function set command sequence
  ets_delay_us(4500);  // wait more than 4.1ms
  command(LCD_FUNCTIONSET | _displayfunction); // second try
  ets_delay_us(150);
  command(LCD_FUNCTIONSET | _displayfunction); // third go
  command(LCD_FUNCTIONSET | _displayfunction); // finally, set # lines, font size, etc.

  // turn the display on with no cursor or blinking default
  uint8 _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
  command(LCD_CLEARDISPLAY);
  ets_delay_us(2000); // clear is long-time command

  uint8 _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal::setCursor(uint8 col, uint8 row)
{
  command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

inline void LiquidCrystal::command(uint8 value) {
  send(value, 0);
}

inline uint8 LiquidCrystal::write(uint8 value) {
  send(value, 1);
  return 1; // assume success
}

void LiquidCrystal::printEx(const char* line1, const char* line2) {
  this->command(LCD_CLEARDISPLAY);
  ets_delay_us(10000);
  this->setCursor(0, 0);
  this->print(line1);
  this->setCursor(0, 1);
  this->print(line2);
}

void LiquidCrystal::printEx(const char* line1, unsigned long line2) {
  this->command(LCD_CLEARDISPLAY);
  ets_delay_us(10000);
  this->setCursor(0, 0);
  this->print(line1);
  this->setCursor(0, 1);
  this->print(line2, 10);
}

uint8 LiquidCrystal::print(const char *text) {
  uint8* buffer = (uint8*)text;

  uint8 n = 0;
  while (*buffer != 0) {
    if (write(*buffer++)) n++;
    else break;
  }

  return n;
}

uint8 LiquidCrystal::print(unsigned long n, uint8 base) {
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero uint8.
  char *str = &buf[sizeof(buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  return print(str);
}

uint8 LiquidCrystal::print(unsigned long n) {
  return print(n, 10);
}

void LiquidCrystal::send(uint8 value, uint8 mode) {
  gpio_write(_rs_pin, mode);
  write4bits(value >> 4);
  write4bits(value);
}

void LiquidCrystal::pulseEnable(void) {
  gpio_write(_enable_pin, 0);
  ets_delay_us(1);
  gpio_write(_enable_pin, 1);
	ets_delay_us(1);    // enable pulse must be >450ns
	gpio_write(_enable_pin, 0);
	ets_delay_us(100);   // commands need > 37us to settle
}

void LiquidCrystal::write4bits(uint8 value) {
  for (int i = 0; i < 4; i++) {
    gpio_write(_data_pins[i], (value >> i) & 0x01);
  }

  pulseEnable();
}

