/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/poweroff.h>
#include "stdio.h"
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/drivers/uart.h>

#include <zephyr/sys/ring_buffer.h>
#define RING_BUF_SIZE 1024
uint8_t ring_buffer[RING_BUF_SIZE];

struct ring_buf ringbuf;

BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
			 "Console device is not ACM CDC UART device");

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(nrf52_learning);

static const struct device *usb_device;

static void uart0_irq_handler(const struct device *dev, void *context);

#define RX_BUF_SIZE 64

static void uart0_irq_handler(const struct device *dev, void *context)
{
	uint8_t char_received;
	static uint8_t command_line[RX_BUF_SIZE];
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
					char_counter = 0;				   // reset the char counter
					printk("command_line = %s \n", command_line);
					memset(&command_line, 0, sizeof(command_line));
				}
			}
		}
	}
}

int main(void)
{
	printk("Starting USB CDC ACM example\n");
	if (usb_enable(NULL))
	{
		return 0;
	}

	usb_device = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
	if (!device_is_ready(usb_device))
	{
		return 0;
	}
	uart_irq_callback_set(usb_device, uart0_irq_handler);
	uart_irq_rx_enable(usb_device);
}
