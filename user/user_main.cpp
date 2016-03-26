/*
 * user_main.cpp
 *
 */

#include "LiquidCrystal.h"

extern "C" {
#include "user_config.h"
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
}

unsigned char *default_certificate;
unsigned int default_certificate_len = 0;
unsigned char *default_private_key;
unsigned int default_private_key_len = 0;

typedef void (*http_callback)(char * response_body, int http_status,
		char * full_response);

typedef struct {
	char * path;
	int port;
	char * post_data;
	char * headers;
	char * hostname;
	char * buffer;
	int buffer_size;
	bool secure;
	http_callback user_callback;
} request_args;

os_timer_t timer;
#define user_procTaskPrio 0
#define user_procTaskQueueLen 1
os_event_t user_procTaskQueue[user_procTaskQueueLen];
DHT_Sensor dht11;
DHT_Sensor_Data dht11_data;

unsigned char httpPost[512] =
		"PUT /v2/feeds/2055461783 HTTP/1.1\r\n"
		"Host: api.xively.com\r\n"
		"User-Agent: MeteoStation\r\n"
		"Connection: close\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 86\r\n"
		"X-ApiKey: PIS0TDt26EVgKZGF2DbtUcKOlJ0KFAW5MkWF5MX3dziPibTE\r\n"
		"\r\n"
		"{\"version\":\"1.0.0\", \"datastreams\": [{\"id\": \"RoomTemperature\", \"current_value\": \"27\"}]}\r\n";

const char feed_id[20] = "2055461783";
const char api_key[50] = "PIS0TDt26EVgKZGF2DbtUcKOlJ0KFAW5MkWF5MX3dziPibTE";
const char data_template[512] = "{\"version\":\"1.0.0\", \"datastreams\": ["
		"{\"id\": \"RoomTemperature\", \"current_value\": \"%d\"}"
//    ",{\"id\": \"RoomHumidity\", \"current_value\": \"%d\"}"
//    ",{\"id\": \"OutsideTemperature\", \"current_value\": \"%d\"}"
//    ",{\"id\": \"OutsideHumidity\", \"current_value\": \"%d\"}"
//    ",{\"id\": \"Pressure\", \"current_value\": \"%d\"}"
		"]}";
const char http_query_template[512] = "PUT /v2/feeds/%s HTTPS/1.1\r\n"
		"Host: %s\r\n"
		"User-Agent: MeteoStation\r\n"
		"Connection: close\r\n"
		"Content-Type: application/json\r\n"
		"X-ApiKey: %s\r\n"
		"\r\n"
		"%s\r\n";

void ICACHE_FLASH_ATTR
print_lcd(const char* line1, const char* line2) {
	LiquidCrystal lcd(4, 2, 7, 6, 5, 0);
	lcd.printEx(line1, line2);
}

void ICACHE_FLASH_ATTR
print_lcd(const char* line1, uint32_t line2) {
	LiquidCrystal lcd(4, 2, 7, 6, 5, 0);
	lcd.printEx(line1, line2);
}

static void ICACHE_FLASH_ATTR
user_procTask(os_event_t *events) {
	uint32_t micros = system_get_time();
	print_lcd("Current time:\0", micros / 1000);
	os_delay_us(10);
}

extern "C" ICACHE_FLASH_ATTR
void read_dht(DHTType dht_type, int dht_pin, char* temperature,
		char* humidity) {
	dht11.pin = 1;
	dht11.type = DHT11;
	DHTInit(&dht11);
	DHTRead(&dht11, &dht11_data);
	DHTFloat2String(temperature, dht11_data.temperature);
	DHTFloat2String(humidity, dht11_data.humidity);
}

extern "C" ICACHE_FLASH_ATTR
void receive_callback(void *arg, char *pdata, unsigned short len) {
	print_lcd("Received:", len);
}

extern "C" ICACHE_FLASH_ATTR
void tcpclient_sent_cb(void *arg) {
	print_lcd("Data sent", "");
}

extern "C" ICACHE_FLASH_ATTR
void dns_callback(const char * hostname, ip_addr_t * addr, void * arg) {
	char temp_ip[16];
	ets_sprintf(temp_ip, "%d.%d.%d.%d\0", IP2STR(&addr));
	os_timer_disarm(&timer);

	struct espconn conn;
	esp_tcp conn_tcp;
	conn_tcp.remote_port = espconn_port();
	ets_memcpy(conn_tcp.remote_ip, &addr->addr, 4);
	conn_tcp.remote_port = 443;
	conn.proto.tcp = &conn_tcp;
	conn.type = ESPCONN_TCP;
	conn.state = ESPCONN_NONE;
	espconn_regist_recvcb(&conn, receive_callback);
	espconn_regist_sentcb(&conn, tcpclient_sent_cb);

	sint8 espcon_status = espconn_connect(&conn);
	switch(espcon_status)
	{
		case ESPCONN_OK:
			print_lcd("TCP created.\0", temp_ip);
			break;
		case ESPCONN_RTE:
			print_lcd("Routing problem.\0", "");
			break;
		case ESPCONN_TIMEOUT:
			print_lcd("Timeout.\0", "");
			break;
		default:
			print_lcd("Connection error\0", "");
	}
	os_delay_us(1000000);
	size_t len = ets_strlen((char*)httpPost);
	espconn_sent(&conn, httpPost, len);
	//espconn_secure_disconnect(&conn);
	//print_lcd("len:", len);
}

extern "C" ICACHE_FLASH_ATTR
void wifi_check_ip() {
	struct ip_info ipConfig;
	os_timer_disarm(&timer);
	switch (wifi_station_get_connect_status()) {
	case STATION_GOT_IP:
		wifi_get_ip_info(STATION_IF, &ipConfig);
		if (ipConfig.ip.addr != 0) {
			ip_addr_t addr;
			addr.addr = 0;
			espconn_gethostbyname((espconn*) NULL, "api.xively.com", &addr,
					dns_callback);
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
	default:
		print_lcd("Connecting...\0", "\0");
		break;
	}
	ets_timer_setfn(&timer, (os_timer_func_t *) wifi_check_ip, NULL);
	ets_timer_arm(&timer, 1000, 0);
}

extern "C" void ICACHE_FLASH_ATTR
user_init(void) {

	UARTInit(BIT_RATE_115200);

	ets_delay_us(10000);

	char line1_t[16], line1[16];
	char line2_t[16], line2[16];
	read_dht(DHT11, 1, line1_t, line2_t);
	ets_sprintf(line1, "Temp: %s%cC\0", line1_t, 223);
	ets_sprintf(line2, "Humidity: %s\%\0", line2_t);
	print_lcd(line1, line2);
	print_lcd(line1, line2);
	print_lcd(line1, line2);

	ets_delay_us(10000);

	ets_bzero(&timer, sizeof(struct _ETSTIMER_));

	if (wifi_set_opmode(STATION_MODE)) {
		ets_delay_us(1000000);
		station_config config;
		ets_bzero(&config, sizeof(struct station_config));
		ets_strcpy((char*) config.ssid, "TheDarkSoul");
		ets_strcpy((char*) config.password, "19216801");
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

	system_os_task(user_procTask, user_procTaskPrio, user_procTaskQueue,
			user_procTaskQueueLen);
}
