#include "usb.h"



#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(usb);

// UART 0 VARIABLES


static const struct device *dev_usb;

static void uart0_irq_handler(const struct device *dev, void *context);

static void uart0_irq_handler(const struct device *dev, void *context)
{
	uint8_t char_received;
	static uint8_t command_line[USB_BUF_SIZE];
	static int char_counter = 0;
	while (uart_irq_update(dev) && uart_irq_is_pending(dev))
	{
		if (uart_irq_rx_ready(dev))
		{
			int len = uart_fifo_read(dev, &char_received, 1);
			if (len)
			{
				command_line[char_counter] = char_received;
				char_counter++;
				if (char_received == '\n' || char_received == '\r')
				{
					command_line[char_counter] = '\0'; // put null at the end of the string to indicate end of message or end of string
					printk("USB RX(%u):%s \n", char_counter,command_line);
                    char_counter = 0;				   // reset the char counter
					memset(&command_line, 0, sizeof(command_line));
				}
			}
		}
	}
}


void app_usb_init()
{

	printk("Starting USB CDC ACM example\n");
	if (usb_enable(NULL))
	{
		return 0;
	}

	dev_usb = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
	if (!device_is_ready(dev_usb))
	{
		return 0;
	}


	uart_irq_callback_set(dev_usb, uart0_irq_handler);
	uart_irq_rx_enable(dev_usb);
}

