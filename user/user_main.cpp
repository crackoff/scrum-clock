/*
 * user_main.cpp
 *
 */

#include "LiquidCrystal.h"

extern "C" {
#include "user_config.h"
#include "flash_param.h"
#include "wifi.h"
#include "servo_arrow.h"

extern void ets_delay_us(uint32_t);
}

#include "wifi_clock.h"
#include "print_lcd.h"
#include "driver/mt6116.hpp"
#include "driver/gpio16.h"

/* Global variables */
os_timer_t _global_timer;

/* User start point */
extern "C" void ICACHE_FLASH_ATTR
user_init(void)
{
	UARTInit(BIT_RATE_115200);

	init_lcd();
	init_servo();

	ets_bzero(&_global_timer, sizeof(struct _ETSTIMER_));
	wifi_clock_set_timer(&_global_timer);

	ets_delay_us(10000);

	arrow_set_position_us(814);

	flash_params ps;
	ets_bzero(&ps, sizeof(struct flash_params));
	ets_strcpy(ps.wifi_ssid, "TheDarkSoul");
	ets_strcpy(ps.wifi_pass, "19216801");

	print_lcd_turn_on();

	// Init device in clock mode
	wifi_init_station(&ps, &_global_timer, (os_timer_func_t *) wifi_clock_init);

	// Init in configuration mode
	//wifi_init_ap();
}
