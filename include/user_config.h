/*
 * user_config.h
 *
 *  Created on: 29 ����. 2016 �.
 *      Author: cat
 */

#ifndef USER_USER_CONFIG_H_
#define USER_USER_CONFIG_H_

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "espconn.h"
#include "driver/uart.h"
#include "driver/gpio16.h"

extern void ets_delay_us(uint32_t);
extern void ets_bzero(void*, size_t);
extern void ets_timer_arm(ETSTimer*, uint32_t, bool);
extern void ets_timer_disarm(ETSTimer*);
extern void ets_timer_setfn(ETSTimer*, ETSTimerFunc*, void*);
extern void ets_sprintf(char*, const char*, ...);
extern void ets_strcpy(char*, const char*);
extern int ets_uart_printf(const char *fmt, ...);
extern int ets_memcpy(void *, const void *, size_t);
extern size_t ets_strlen (const char*);
extern char *ets_strstr (const char *, const char *);

#define RS_GPIO 2
#define E_GPIO 4
#define D4_GPIO 13
#define D5_GPIO 12
#define D6_GPIO 14
#define D7_GPIO 16

#define RS_PIN 4
#define E_PIN 2
#define D4_PIN 7
#define D5_PIN 6
#define D6_PIN 5
#define D7_PIN 0

#endif /* USER_USER_CONFIG_H_ */
