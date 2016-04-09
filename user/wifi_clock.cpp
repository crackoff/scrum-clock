/*
 * wifi_clock.c
 *
 *  Created on: Apr 2, 2016
 *      Author: crackoff
 */

extern "C" {
#include "user_config.h"
}
#include "wifi_clock.h"
#include "print_lcd.h"

esp_tcp _conn_tcp;
espconn _conn;
u_time _user_time;
os_timer_t* global_timer;
wifi_clock_on_draw_cb global_wifi_clock_on_draw_cb;
wifi_clock_time global_wifi_clock_time;

ICACHE_FLASH_ATTR
char *get_current_time(char *current_time)
{
	uint32 rtc = system_get_rtc_time();
	uint32 cal = system_rtc_clock_cali_proc();

	uint64 curr_time = ( ((uint64) (rtc - _user_time.init_clock)) * ((uint64) ((cal * 1000) >> 12)) );
	uint32 second = _user_time.init_second + curr_time / 1000000000l;
	uint16 minute = _user_time.init_minute;
	uint16 hour = _user_time.init_hour + TIMEZONE;
	if (second >= 60) {
		minute += second / 60;
		second %= 60;
		if (minute >= 60) {
			hour += minute / 60;
			minute %= 60;
			while (hour >= 24)
				hour -= 24;
		}
	}

	ets_sprintf(current_time, "%02d:%02d:%02d", hour, minute, second);
	return current_time;
}

ICACHE_FLASH_ATTR
wifi_clock_time get_current_time()
{
	uint32 rtc = system_get_rtc_time();
	uint32 cal = system_rtc_clock_cali_proc();

	uint64 curr_time = ( ((uint64) (rtc - _user_time.init_clock)) * ((uint64) ((cal * 1000) >> 12)) );
	uint32 second = _user_time.init_second + curr_time / 1000000000l;
	uint16 minute = _user_time.init_minute;
	uint16 hour = _user_time.init_hour + TIMEZONE;
	if (second >= 60) {
		minute += second / 60;
		second %= 60;
		if (minute >= 60) {
			hour += minute / 60;
			minute %= 60;
			while (hour >= 24)
				hour -= 24;
		}
	}

	wifi_clock_time ret;
	ret.hour = hour;
	ret.minute = minute;
	ret.second = second;
	return ret;
}

ICACHE_FLASH_ATTR
void wifi_clock_update()
{
	char current_time[16];
	//print_lcd("Time = ", get_current_time(current_time));
	wifi_clock_time time = get_current_time();

	uint8 dots = 0;
	if (system_get_time() % 1000000 > 500000)
		dots = 1;
	print_lcd_time(time.hour, time.minute, dots);

	// !!! DO NOT DELETE IT, OLEG !!!
	//system_rtc_mem_write(64, &curr_time, sizeof(curr_time));
}

ICACHE_FLASH_ATTR
void wifi_clock_receive_cb(void *arg, char *data, uint16 len)
{
	ets_bzero(&_user_time, sizeof(struct u_time));
	char *dt = ets_strstr(data, "<dateTime");
	char *time = ets_strstr(dt, ">") + 12;
	time[2] = time[5] = time[8] = 0;
	_user_time.init_clock = system_get_rtc_time();
	_user_time.init_hour = atoi(time);
	_user_time.init_minute = atoi(time + 3);
	_user_time.init_second = atoi(time + 6);
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

ICACHE_FLASH_ATTR
void wifi_clock_set_draw_cb(void (* wifi_clock_on_draw_cb)(wifi_clock_time *))
{
	global_wifi_clock_on_draw_cb = wifi_clock_on_draw_cb;
}

ICACHE_FLASH_ATTR
void wifi_clock_init()
{
	struct ip_info ipConfig;
	os_timer_disarm(global_timer);

	bool cnt = true;
	uint8 line2[] = {1,1,1,1,1,1};

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
		//print_lcd("Initializing...", "");
		print_lcd(line2, 6);
		break;
	}

	if (cnt) {
		ets_timer_setfn(global_timer, (os_timer_func_t *) wifi_clock_init, NULL);
		ets_timer_arm(global_timer, 1000, 0);
	}
}
