/*
 * wifi_clock.c
 *
 *  Created on: Apr 2, 2016
 *      Author: crackoff
 */

extern "C" {
#include "user_config.h"
#include "servo_arrow.h"
}
#include "wifi_clock.h"
#include "print_lcd.h"

esp_tcp _conn_tcp;
espconn _conn;
u_time _user_time;
os_timer_t* global_timer;
unsigned last_printed_time = 0;
//wifi_clock_on_draw_cb global_wifi_clock_on_draw_cb;
//wifi_clock_time global_wifi_clock_time;

ICACHE_FLASH_ATTR
wifi_clock_time get_current_time()
{
	// т.к. год не хранится, високосные года не учитываются
	int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	uint32 rtc = system_get_rtc_time();
	uint32 cal = system_rtc_clock_cali_proc();

	uint64 curr_time = ( ((uint64) (rtc - _user_time.init_clock)) * ((uint64) ((cal * 1000) >> 12)) );
	uint32 second = _user_time.init_second + curr_time / 1000000000l;
	uint16 minute = _user_time.init_minute;
	uint16 hour = _user_time.init_hour + TIMEZONE;
	uint8 days_delta = 0;
	if (second >= 60) {
		minute += second / 60;
		second %= 60;
		if (minute >= 60) {
			hour += minute / 60;
			minute %= 60;
			while (hour >= 24)
			{
				hour -= 24;
				days_delta++;
			}
		}
	}

	uint8 month = _user_time.init_month;
	uint8 day = _user_time.init_day + days_delta;
	while (day > days_in_month[month - 1])
	{
		day -= days_in_month[month - 1];
		month++;
	}
	while (month > 12)
		month -= 12;

	wifi_clock_time ret;
	ret.hour = hour;
	ret.month = month;
	ret.minute = minute;
	ret.second = second;
	ret.day = day;
	return ret;
}

ICACHE_FLASH_ATTR
void wifi_clock_update()
{
	wifi_clock_time time = get_current_time();

	uint8 dots = 0;
	if (system_get_time() % 1000000 > 500000)
		dots = 1;
	print_lcd_time(time.hour, time.minute, dots, time.month, time.day);
	arrow_set_position_by_time(time.hour * 100 + time.minute);

	// !!! DO NOT DELETE IT, OLEG !!!
	//system_rtc_mem_write(64, &curr_time, sizeof(curr_time));
}

ICACHE_FLASH_ATTR
void wifi_clock_receive_cb(void *arg, char *data, uint16 len)
{
	ets_bzero(&_user_time, sizeof(struct u_time));
	char *dt = ets_strstr(data, "<dateTime");
	char *time = ets_strstr(dt, ">") + 1;
	time[4] = time[7] = time[10] = time[13] = time[16] = time[19] = 0;
	_user_time.init_clock = system_get_rtc_time();
	_user_time.init_month = atoi(time + 5);
	_user_time.init_day = atoi(time + 8);
	_user_time.init_hour = atoi(time + 11);
	_user_time.init_minute = atoi(time + 14);
	_user_time.init_second = atoi(time + 17);
	/*char *time = ets_strstr(dt, ">") + 12;
	time[2] = time[5] = time[8] = 0;
	_user_time.init_clock = system_get_rtc_time();
	_user_time.init_hour = atoi(time);
	_user_time.init_minute = atoi(time + 3);
	_user_time.init_second = atoi(time + 6);*/
}

ICACHE_FLASH_ATTR
void wifi_clock_discon_cb(void *arg)
{
	wifi_station_disconnect();
	ets_timer_setfn(global_timer, (os_timer_func_t *) wifi_clock_init, NULL);
	ets_timer_arm(global_timer, 1000, 0);
}

ICACHE_FLASH_ATTR
void wifi_clock_connect_cb(void *arg)
{
	struct espconn *pespconn = (struct espconn *)arg;
	espconn_regist_recvcb(pespconn, wifi_clock_receive_cb);
	espconn_regist_disconcb(pespconn, wifi_clock_discon_cb);

	espconn_sent(pespconn, (uint8*)GET_TIME_HTTP, ets_strlen(GET_TIME_HTTP));
}

ICACHE_FLASH_ATTR
void wifi_clock_dns_callback(const char * hostname, ip_addr_t * addr, void * arg)
{
	ets_memcpy(_conn_tcp.remote_ip, &addr->addr, 4);
	_conn_tcp.local_port = espconn_port();
	_conn_tcp.remote_port = HTTP_PORT;

	_conn.type = ESPCONN_TCP;
	_conn.state = ESPCONN_NONE;
	_conn.proto.tcp = &_conn_tcp;

	espconn_regist_connectcb(&_conn, wifi_clock_connect_cb);
	sint8 espcon_status = espconn_connect(&_conn);
}

ICACHE_FLASH_ATTR
void wifi_clock_set_timer(os_timer_t* timer)
{
	global_timer = timer;
}

//ICACHE_FLASH_ATTR
//void wifi_clock_set_draw_cb(void (* wifi_clock_on_draw_cb)(wifi_clock_time *))
//{
//	global_wifi_clock_on_draw_cb = wifi_clock_on_draw_cb;
//}

ICACHE_FLASH_ATTR
void wifi_clock_init()
{
	struct ip_info ipConfig;
	os_timer_disarm(global_timer);

	bool cnt = true;

	switch (wifi_station_get_connect_status()) {
	case STATION_GOT_IP:
		wifi_get_ip_info(STATION_IF, &ipConfig);
		if (ipConfig.ip.addr != 0) {
			ip_addr_t addr;
			addr.addr = 0;
			espconn_gethostbyname((struct espconn*) NULL, HTTP_SITE, &addr, wifi_clock_dns_callback);
			return;
		}
		break;
	case STATION_WRONG_PASSWORD:
		//print_lcd("Wrong password", "");
		break;
	case STATION_NO_AP_FOUND:
		//print_lcd("No AP found", "");
		break;
	case STATION_CONNECT_FAIL:
		//print_lcd("Fail connecting", "");
		break;
	case STATION_IDLE:
		ets_timer_setfn(global_timer, (os_timer_func_t *) wifi_clock_update, NULL);
		ets_timer_arm(global_timer, 500, 1);
		cnt = false;
		break;
	default:
		print_lcd_loading();
		break;
	}

	if (cnt) {
		ets_timer_setfn(global_timer, (os_timer_func_t *) wifi_clock_init, NULL);
		ets_timer_arm(global_timer, 1000, 0);
	}
}
