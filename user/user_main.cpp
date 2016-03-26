/*
 * user_main.cpp
 *
 */

/*----------------------------------------------------*/
/*       defines & includes                           */
/*----------------------------------------------------*/

#include "LiquidCrystal.h"

extern "C" {
#include "user_config.h"
}

#define WIFI_SSID "TheDarkSoul"
#define WIFI_PASS "19216801"

/*----------------------------------------------------*/
/*       declarations                                 */
/*----------------------------------------------------*/

os_timer_t timer;
esp_tcp conn_tcp;
espconn conn;

extern "C" void wifi_check_ip();

const char http_site[] = "www.satan.lt";
const int http_port = 80;
unsigned char http[512] =
		"GET /WebServices/Time.asmx/GetServerUtcTime HTTP/1.1\r\n"
		"Host: www.satan.lt\r\n"
		"Connection: close\r\n"
		"\r\n";

/*----------------------------------------------------*/
/*       functions                                    */
/*----------------------------------------------------*/

// Print LCD function
void ICACHE_FLASH_ATTR
print_lcd(const char* line1, const char* line2) {
	LiquidCrystal lcd(4, 2, 7, 6, 5, 0);
	lcd.printEx(line1, line2);
}

// Print LCD function
void ICACHE_FLASH_ATTR
print_lcd(const char* line1, uint32_t line2) {
	LiquidCrystal lcd(4, 2, 7, 6, 5, 0);
	lcd.printEx(line1, line2);
}

extern "C" ICACHE_FLASH_ATTR
void tcpclient_receive_cb(void *arg, char *data, uint16 len) {

	char *dt = ets_strstr(data, "<dateTime");
	char *time = ets_strstr(dt, ">") + 12;
	time[8] = 0;
	print_lcd("Time", time);
}

extern "C" ICACHE_FLASH_ATTR
void tcpclient_discon_cb(void *arg) {
	wifi_station_disconnect();
	ets_timer_setfn(&timer, (os_timer_func_t *) wifi_check_ip, NULL);
	ets_timer_arm(&timer, 1000, 0);
}

extern "C" ICACHE_FLASH_ATTR
void tcpclient_connect_cb(void *arg){

	struct espconn *pespconn = (struct espconn *)arg;
	espconn_regist_recvcb(pespconn, tcpclient_receive_cb);
	espconn_regist_disconcb(pespconn, tcpclient_discon_cb);

	espconn_sent(pespconn, http, ets_strlen((char*)http));
}

extern "C" ICACHE_FLASH_ATTR
void dns_callback(const char * hostname, ip_addr_t * addr, void * arg) {

	ets_memcpy(conn_tcp.remote_ip, &addr->addr, 4);
	conn_tcp.local_port = espconn_port();
	conn_tcp.remote_port = http_port;

	conn.type = ESPCONN_TCP;
	conn.state = ESPCONN_NONE;
	conn.proto.tcp = &conn_tcp;

	espconn_regist_connectcb(&conn, tcpclient_connect_cb);
	sint8 espcon_status = espconn_connect(&conn);
}

extern "C" ICACHE_FLASH_ATTR
void wifi_check_ip() {

	struct ip_info ipConfig;
	os_timer_disarm(&timer);

	bool cnt = true;

	switch (wifi_station_get_connect_status()) {
	case STATION_GOT_IP:
		wifi_get_ip_info(STATION_IF, &ipConfig);
		if (ipConfig.ip.addr != 0) {
			ip_addr_t addr;
			addr.addr = 0;
			espconn_gethostbyname((espconn*) NULL, http_site, &addr, dns_callback);
			return;
		}
		break;
	case STATION_WRONG_PASSWORD:
		print_lcd("Wrong password\0", "\0");
		break;
	case STATION_NO_AP_FOUND:
		print_lcd("No AP found\0", "\0");
		break;
	case STATION_CONNECT_FAIL:
		print_lcd("Fail connecting\0", "\0");
		break;
	case STATION_IDLE:
		// TODO here must be clock functions
		cnt = false;
		break;
	default:
		print_lcd("Connecting...\0", "\0");
		break;
	}

	if (cnt) {
		ets_timer_setfn(&timer, (os_timer_func_t *) wifi_check_ip, NULL);
		ets_timer_arm(&timer, 1000, 0);
	}
}

extern "C" void ICACHE_FLASH_ATTR
user_init(void) {

	UARTInit(BIT_RATE_115200);

	ets_delay_us(10000);

	print_lcd("", ""); // emptying LCD

	ets_bzero(&timer, sizeof(struct _ETSTIMER_));

	if (wifi_set_opmode(STATION_MODE)) {

		ets_delay_us(1000000);
		station_config config;
		ets_bzero(&config, sizeof(struct station_config));
		ets_strcpy((char*) config.ssid, WIFI_SSID);
		ets_strcpy((char*) config.password, WIFI_PASS);

		print_lcd("Trying to\0", "connect...\0");

		wifi_station_disconnect();
		wifi_station_set_config(&config);
		wifi_station_connect();
		wifi_station_dhcpc_start();
		wifi_station_set_auto_connect(1);

		ets_timer_disarm(&timer);
		ets_timer_setfn(&timer, (os_timer_func_t *) wifi_check_ip, NULL);
		ets_timer_arm(&timer, 1000, 0);
	} else {
		print_lcd("Set mode failed\0", "\0");
	}
}
