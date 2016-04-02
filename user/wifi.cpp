extern "C" {
#include "user_config.h"
#include "user_interface.h"
extern void ets_delay_us(uint32_t);
}
#include "flash_param.h"
#include "print_lcd.h"

#define WIFI_AP_SSID "ScrumClock"
#define WIFI_AP_PASS "scrumisashit"

extern "C" ICACHE_FLASH_ATTR
bool wifi_init_station(struct flash_params *params, os_timer_t* timer, os_timer_func_t* wifi_check_ip)
{
	if (wifi_set_opmode(STATION_MODE))
	{
		ets_delay_us(1000000);
		struct station_config config;
		ets_bzero(&config, sizeof(struct station_config));
		ets_strcpy((char*) config.ssid, params->wifi_ssid);
		ets_strcpy((char*) config.password, params->wifi_pass);

		wifi_station_disconnect();
		wifi_station_set_config(&config);
		wifi_station_connect();
		wifi_station_dhcpc_start();
		wifi_station_set_auto_connect(1);

		ets_timer_disarm(timer);
		ets_timer_setfn(timer, wifi_check_ip, NULL);
		ets_timer_arm(timer, 1000, 0);
		return true;
	}
	else
	{
		return false;
	}
}

extern "C" ICACHE_FLASH_ATTR
bool wifi_init_ap()
{
	if (wifi_set_opmode(SOFTAP_MODE))
	{
		struct softap_config apconfig;
		if (wifi_softap_get_config(&apconfig))
		{
			wifi_softap_dhcps_stop();
			char macaddr[6];
			wifi_get_macaddr(SOFTAP_IF, (uint8*)macaddr);
			os_memset(apconfig.ssid, 0, sizeof(apconfig.ssid));
			os_memset(apconfig.password, 0, sizeof(apconfig.password));
			ets_strcpy((char*)apconfig.ssid, WIFI_AP_SSID);
			apconfig.ssid_len = ets_strlen(WIFI_AP_SSID);
			ets_strcpy((char*)apconfig.password, WIFI_AP_PASS);
			apconfig.authmode = AUTH_WPA_WPA2_PSK;
			apconfig.ssid_hidden = 0;
			apconfig.channel = 7;
			apconfig.max_connection = 10;
			if(!wifi_softap_set_config(&apconfig))
			{
				return false;
			}

			struct ip_info ipinfo;
			if(wifi_get_ip_info(SOFTAP_IF, &ipinfo))
			{
				IP4_ADDR(&ipinfo.ip, 192, 168, 4, 1);
				IP4_ADDR(&ipinfo.gw, 192, 168, 4, 1);
				IP4_ADDR(&ipinfo.netmask, 255, 255, 255, 0);
				if(!wifi_set_ip_info(SOFTAP_IF, &ipinfo))
				{
					//print_lcd("ERROR set IP config!", "");
				} else {
					char temp[80];
					os_sprintf(temp, "%d.%d.%d.%d", IP2STR(&ipinfo.ip));
					//print_lcd("web server ip:", temp);
				}
			}
			wifi_softap_dhcps_start();
			if(wifi_get_phy_mode() != PHY_MODE_11N)
				wifi_set_phy_mode(PHY_MODE_11N);
			if(wifi_station_get_auto_connect() == 0)
				wifi_station_set_auto_connect(1);
		}
		return true;
	}
	else
	{
		return false;
	}
}
