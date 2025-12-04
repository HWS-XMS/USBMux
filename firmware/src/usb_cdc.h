#ifndef USB_CDC_H
#define USB_CDC_H

#include <stdint.h>
#include <stddef.h>

void usb_cdc_init(void);
void usb_cdc_poll(void);

size_t usb_cdc_read(char *buf, size_t len);
void usb_cdc_write(const char *buf, size_t len);
void usb_cdc_write_string(const char *str);

#endif
