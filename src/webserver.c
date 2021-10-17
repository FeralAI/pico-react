#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "lwip/apps/httpd.h"
#include "lwip/def.h"
#include "webserver.h"

extern int rndis_init(void);
extern void rndis_task(void);

#define LED_PIN     25
char *html[2]={"/index.html","/Index.html"};

static const char *cgi_toggle_led(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	gpio_put(LED_PIN, !gpio_get(LED_PIN));
	return html[gpio_get(LED_PIN)];
}

static const char *cgi_reset_usb_boot(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	reset_usb_boot(0, 0);
	return html[gpio_get(LED_PIN)];
}

static const tCGI cgi_handlers[] =
{
	{
		"/toggle_led",
		cgi_toggle_led
	},
	{
		"/reset_usb_boot",
		cgi_reset_usb_boot
	}
};

void webserver(void)
{
	printf("[NET] BEGIN\n");
	rndis_init();
	http_set_cgi_handlers(cgi_handlers, LWIP_ARRAYSIZE(cgi_handlers));
	while (1)
	{
			rndis_task();
	}
}