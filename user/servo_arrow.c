/*
 * servo_arrow.c
 *
 *  Created on: Apr 16, 2016
 *      Author: crackoff
 */

#include "c_types.h"
#include "driver/gpio16.h"
#include "servo_arrow.h"
extern void ets_delay_us(uint32_t);

void ICACHE_FLASH_ATTR
init_servo()
{
	set_gpio_mode(SERVO_PIN, GPIO_PULLDOWN, GPIO_OUTPUT);
}

void ICACHE_FLASH_ATTR
arrow_set_position_us(unsigned ultime)
{
	unsigned lltime = PWM_CYCLE_DUTY_US - ultime;
	int i;
	for (i = 0; i < PWM_STEPS_COUNT; i++)
	{
		gpio_write(SERVO_PIN, 1);
		ets_delay_us(ultime);
		gpio_write(SERVO_PIN, 0);
		ets_delay_us(lltime);
	}
}

int current_pos = -1;

void ICACHE_FLASH_ATTR
arrow_set_position_by_time(unsigned time)
{
	int i;
	unsigned pos_times[] = { 1000, 1130, 1200, 1230, 1600, 1630, 1830 };
	unsigned pos_us[] = { 2182, 1954, 1726, 1498, 1270, 1042, 814 };
	int new_pos = 0;
	if (time < pos_times[0])
		new_pos = 6;
	else
		for (i = 6; i >=0; i--)
			if (time >= pos_times[i])
			{
				new_pos = i;
				break;
			}

	if (current_pos != new_pos)
	{
		current_pos = new_pos;
		arrow_set_position_us(pos_us[current_pos]);
	}
}
