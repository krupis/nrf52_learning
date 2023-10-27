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

#define UART_BUF_SIZE 20

#define UART_RX_TIMEOUT_MS 1000
K_SEM_DEFINE(rx_disabled, 0, 1);

// UART RX primary buffers
uint8_t uart_double_buffer[2][UART_BUF_SIZE];

uint8_t *uart_buf_next = uart_double_buffer[1];

bool currently_active_buffer = 1; // 0 - uart_double_buffer[0] is active, 1 - uart_double_buffer[1] is active

static const struct device *dev_uart;

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{

	switch (evt->type)
	{

	case UART_TX_DONE:
		printf("transmission complete \n");
		break;

	case UART_TX_ABORTED:
		// do something
		break;

	case UART_RX_RDY:

		printk("Received %i bytes \n", evt->data.rx.len);
		printk("Offset = %i  \n", evt->data.rx.offset);
		break;

	case UART_RX_BUF_REQUEST:
		uart_rx_buf_rsp(dev_uart, uart_buf_next, UART_BUF_SIZE);
		currently_active_buffer = !currently_active_buffer;
		if (currently_active_buffer == 0)
		{
			printk("currently active buffer is uart_double_buffer[0] \n");
		}
		else
		{
			printk("currently active buffer is uart_double_buffer[1] \n");
		}
		break;

	case UART_RX_BUF_RELEASED:
		printk("Old buffer has been released \n");
		uart_buf_next = evt->data.rx_buf.buf;
		break;

	case UART_RX_DISABLED:
		printk("rx disabled \n");
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
	uart_rx_enable(dev_uart, uart_double_buffer[0], UART_BUF_SIZE, UART_RX_TIMEOUT_MS);
}

int main(void)
{
	app_uart_init();
}
