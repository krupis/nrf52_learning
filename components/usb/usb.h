
#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/drivers/uart.h>

#define USB_BUF_SIZE 64
#define USB_RX_TIMEOUT_MS 1000

#define usb_message_queue_size 10
#define usb_message_size 64

void app_usb_init();
void usb_parser_thread(void);