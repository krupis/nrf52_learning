/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include "stdio.h"
#include "zephyr/drivers/uart.h"

static const struct device *dev_uart0;

static void uart0_irq_handler(const struct device *dev, void *context);


#define RX_BUF_SIZE 64



void app_uart0_init()
{
	dev_uart0 = DEVICE_DT_GET(DT_NODELABEL(uart0));
	if (!device_is_ready(dev_uart0))
	{
		return;
	}
	uart_irq_callback_set(dev_uart0, uart0_irq_handler);
	uart_irq_rx_enable(dev_uart0);
}



static void uart0_irq_handler(const struct device *dev, void *context)
{
	uint8_t char_received;
	static uint8_t command_line[RX_BUF_SIZE];
	static int char_counter = 0;

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
				char_counter = 0;				   // reset the char counter
				printk("command_line = %s \n", command_line);
				memset(&command_line, 0, sizeof(command_line));
			}
		}
	}
}

int main(void)
{
	app_uart0_init();
}
