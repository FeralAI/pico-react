#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

typedef enum
{
	HID,
	NET,
} UsbMode;

void setUsbMode(UsbMode mode);

#endif
