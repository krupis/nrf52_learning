/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "stdio.h"
#include "my_gpio.h"
#include "zephyr/drivers/uart.h"
#include <zephyr/sys/ring_buffer.h>

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(nrf52_learning);

#define UART_BUF_SIZE 16

#define UART_RX_TIMEOUT_MS 100
K_SEM_DEFINE(rx_disabled, 0, 1);

#define UART_RX_MSG_QUEUE_SIZE 8
struct uart_msg_queue_item
{
	uint8_t bytes[UART_BUF_SIZE];
	uint32_t length;
};

// UART RX primary buffers
uint8_t uart_double_buffer[2][UART_BUF_SIZE];
uint8_t *uart_buf_next = uart_double_buffer[1];

// UART RX message queue
K_MSGQ_DEFINE(uart_rx_msgq, sizeof(struct uart_msg_queue_item), UART_RX_MSG_QUEUE_SIZE, 4);

static const struct device *dev_uart;

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	static struct uart_msg_queue_item new_message;
	switch (evt->type)
	{

	case UART_TX_DONE:
		printf("transmission complete \n");
		break;

	case UART_TX_ABORTED:
		// do something
		break;

	case UART_RX_RDY:
		memcpy(new_message.bytes, evt->data.rx.buf + evt->data.rx.offset, evt->data.rx.len);
		new_message.length = evt->data.rx.len;
		if (k_msgq_put(&uart_rx_msgq, &new_message, K_NO_WAIT) != 0)
		{
			printk("Error: Uart RX message queue full!\n");
		}
		break;

	case UART_RX_BUF_REQUEST:
		printf("requesting buffer \n");
		uart_rx_buf_rsp(dev_uart, uart_buf_next, UART_BUF_SIZE);
		break;

	case UART_RX_BUF_RELEASED:
		printf("buffer released \n");
		uart_buf_next = evt->data.rx_buf.buf;

		break;

	case UART_RX_DISABLED:
		printf("rx disabled \n");
		k_sem_give(&rx_disabled);
		break;

	case UART_RX_STOPPED:
		// do something
		break;

	default:
		break;
	}
}

void app_uart_init()
{
	dev_uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

	if (!device_is_ready(dev_uart))
	{
		return 0;
	}

	int err;
	err = uart_callback_set(dev_uart, uart_cb, NULL);
	if (err)
	{
		return err;
	}
	uart_rx_enable(dev_uart, uart_double_buffer[0], UART_BUF_SIZE, 100);
}

int main(void)
{
	app_uart_init();

	struct uart_msg_queue_item incoming_message;

	while (1)
	{
		// This function will not return until a new message is ready
		k_msgq_get(&uart_rx_msgq, &incoming_message, K_FOREVER);
		// Process the message here.
		static uint8_t string_buffer[UART_BUF_SIZE + 1];
		memcpy(string_buffer, incoming_message.bytes, incoming_message.length);
		string_buffer[incoming_message.length] = 0;
		printk("RX %i: %s\n", incoming_message.length, string_buffer);
	}
}
