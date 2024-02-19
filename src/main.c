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

#define UART_BUF_SIZE 24

#define UART_RX_TIMEOUT_MS 1000
K_SEM_DEFINE(rx_disabled, 0, 1);

// UART RX primary buffers
uint8_t uart_double_buffer[2][UART_BUF_SIZE];

uint8_t *uart_buf_next = uart_double_buffer[1];

uint8_t complete_message[UART_BUF_SIZE];
uint8_t complete_message_counter = 0;
bool currently_active_buffer = 1; // 0 - uart_double_buffer[0] is active, 1 - uart_double_buffer[1] is active

static const struct device *dev_uart;

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data);

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{

	switch (evt->type)
	{

	case UART_TX_DONE:
		break;

	case UART_TX_ABORTED:
		// do something
		break;

	case UART_RX_RDY:

		printk("Received %i bytes \n", evt->data.rx.len);
		printk("Offset = %i  \n", evt->data.rx.offset);

		if (currently_active_buffer == 0)
		{
			// read all characters one by one till new line is found
			for (int i = 0 + evt->data.rx.offset; i < UART_BUF_SIZE; i++)
			{
				complete_message[complete_message_counter] = uart_double_buffer[0][i];
				complete_message_counter++;
				if (uart_double_buffer[0][i] == '\n')
				{
					complete_message_counter = 0;
					printk("complete_message = %s \n", complete_message);
					memset(&complete_message, 0, sizeof(complete_message)); // clear out the buffer to prepare for next read.
					break;
				}
			}
		}

		if (currently_active_buffer == 1)
		{
			// read all characters one by one till new line is found
			for (int i = 0 + evt->data.rx.offset; i < UART_BUF_SIZE; i++)
			{
				complete_message[complete_message_counter] = uart_double_buffer[1][i];
				complete_message_counter++;
				if (uart_double_buffer[1][i] == '\n')
				{
					complete_message_counter = 0;
					printk("complete_message = %s \n", complete_message);
					memset(&complete_message, 0, sizeof(complete_message)); // clear out the buffer to prepare for next read.
					break;
				}
			}
		}

		break;

	case UART_RX_BUF_REQUEST:
		uart_rx_buf_rsp(dev_uart, uart_buf_next, UART_BUF_SIZE);
		currently_active_buffer = !currently_active_buffer;
		break;

	case UART_RX_BUF_RELEASED:
		uart_buf_next = evt->data.rx_buf.buf;
		break;

	case UART_RX_DISABLED:
		k_sem_give(&rx_disabled);
		break;

	case UART_RX_STOPPED:
		// do something
		break;

	default:
		break;
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

	//ASYNC API
	int err;
	err = uart_callback_set(usb_device, uart_cb, NULL);
	printk("err = %i \n", err);
	if (err)
	{
		return err;
	}
	uart_rx_enable(usb_device, uart_double_buffer[0], UART_BUF_SIZE, UART_RX_TIMEOUT_MS);


	//INTERRUPT API
	// uart_irq_callback_set(usb_device, uart0_irq_handler);
	// uart_irq_rx_enable(usb_device);
	
	while (1)
	{
		printk("Hello World! %s\n", CONFIG_BOARD);
		k_sleep(K_MSEC(1000));
	}
}
