#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tusb.h"
#include "class/hid/hid.h"
#include "pico/stdlib.h"
#include "webserver.h"
#include "usb_descriptors.h"

UsbMode mode = NET;

int main(void)
{
  stdio_init_all();
	setUsbMode(mode);

	tusb_init();

	if (mode == NET)
	{
		webserver();
	}
	else
	{
		while (1)
		{
			if (tud_suspended())
			{
					// Wake up host if we are in suspend mode
					// and REMOTE_WAKEUP feature is enabled by host
					tud_remote_wakeup();
			}

			tud_task();
		}
	}

}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // This example doesn't use multiple report and report ID
  (void) itf;
  (void) report_id;
  (void) report_type;

  // echo back anything we received from host
  tud_hid_report(0, buffer, bufsize);
}
