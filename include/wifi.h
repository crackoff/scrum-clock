/*
 * wifi.h
 *
 *  Created on: Apr 2, 2016
 *      Author: crackoff
 */

#ifndef INCLUDE_WIFI_H_
#define INCLUDE_WIFI_H_

#include "os_type.h"

bool wifi_init_station(struct flash_params *params, os_timer_t* timer, os_timer_func_t* wifi_check_ip);
bool wifi_init_ap();

#endif /* INCLUDE_WIFI_H_ */
