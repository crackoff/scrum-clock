/*
 * wifi_clock.h
 *
 *  Created on: Apr 2, 2016
 *      Author: crackoff
 */

#ifndef INCLUDE_WIFI_CLOCK_H_
#define INCLUDE_WIFI_CLOCK_H_

struct wifi_clock_time
{
	uint8 hour;
	uint8 minute;
	uint8 second;
	uint8 day;
	uint8 month;
};

typedef void (* wifi_clock_on_draw_cb)(wifi_clock_time *arg);

void wifi_clock_init();
void wifi_clock_set_timer(os_timer_t* timer);
void wifi_clock_set_draw_cb(wifi_clock_on_draw_cb* cb);

struct u_time
{
	char init_time[8];
	uint32 init_clock;
	uint8 init_hour;
	uint8 init_minute;
	uint8 init_second;
	uint8 init_month;
	uint8 init_day;
	uint32 magic;
};

#define TIMEZONE 3
#define HTTP_SITE "www.satan.lt"
#define HTTP_PORT 80
#define GET_TIME_HTTP "GET /WebServices/Time.asmx/GetServerUtcTime HTTP/1.1\r\nHost: www.satan.lt\r\nConnection: close\r\n\r\n"

#endif /* INCLUDE_WIFI_CLOCK_H_ */
