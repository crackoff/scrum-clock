/*
 * user_main.cpp
 *
 */

#include "LiquidCrystal.h"

extern "C" {
#include "user_config.h"
#include "flash_param.h"
#include "wifi.h"

extern void ets_delay_us(uint32_t);
}

#include "wifi_clock.h"
#include "print_lcd.h"
#include "driver/mt6116.hpp"
#include "driver/gpio16.h"

/* Global variables */
os_timer_t _global_timer;
//MT6116 _global_mt6116 (MT6116_LATCH_PIN, MT6116_CLOCK_PIN, MT6116_DATA_PIN, MT6116_A0_PIN, MT6116_E_PIN, MT6116_RESET_PIN);

/* Draw the time */
extern "C" void ICACHE_FLASH_ATTR
draw_time(wifi_clock_time* time)
{
	uint8 line[] = {8,1,2,6,3,6,8,2,5};
	print_lcd(line, 6);
}

/* User start point */
extern "C" void ICACHE_FLASH_ATTR
user_init(void)
{
	UARTInit(BIT_RATE_115200);

	init_lcd();
	//_global_mt6116.Init();

	ets_bzero(&_global_timer, sizeof(struct _ETSTIMER_));
	wifi_clock_set_timer(&_global_timer);
	//wifi_clock_set_draw_cb((wifi_clock_on_draw_cb*)draw_time);

	ets_delay_us(10000);

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
