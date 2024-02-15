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

/*
static void uart0_irq_handler(const struct device *dev, void *context)
{

	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if (uart_irq_rx_ready(dev)) {
			int recv_len, rb_len;
			uint8_t buffer[64];
			size_t len = MIN(ring_buf_space_get(&ringbuf),
					 sizeof(buffer));

			recv_len = uart_fifo_read(dev, buffer, len);
			if (recv_len < 0) {
				LOG_ERR("Failed to read UART FIFO");
				recv_len = 0;
			};

			rb_len = ring_buf_put(&ringbuf, buffer, recv_len);
			if (rb_len < recv_len) {
				LOG_ERR("Drop %u bytes", recv_len - rb_len);
			}

			LOG_DBG("tty fifo -> ringbuf %d bytes", rb_len);
			if (rb_len) {
				uart_irq_tx_enable(dev);
			}
		}

		if (uart_irq_tx_ready(dev)) {
			uint8_t buffer[64];
			int rb_len, send_len;

			rb_len = ring_buf_get(&ringbuf, buffer, sizeof(buffer));
			if (!rb_len) {
				LOG_DBG("Ring buffer empty, disable TX IRQ");
				uart_irq_tx_disable(dev);
				continue;
			}

			send_len = uart_fifo_fill(dev, buffer, rb_len);
			if (send_len < rb_len) {
				LOG_ERR("Drop %d bytes", rb_len - send_len);
			}

			LOG_DBG("ringbuf -> tty fifo %d bytes", send_len);
		}
	}
}
*/

int main(void)
{
	if (usb_enable(NULL))
	{
		return 0;
	}

	// ring_buf_init(&ringbuf, sizeof(ring_buffer), ring_buffer);

	usb_device = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
	if (!device_is_ready(usb_device))
	{
		return 0;
	}
	uart_irq_callback_set(usb_device, uart0_irq_handler);
	uart_irq_rx_enable(usb_device);
}
