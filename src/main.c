/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include "stdio.h"
#include "zephyr/drivers/uart.h"
#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(nrf52_learning);

static const struct device *dev_uart0;
static const struct device *dev_uart1;

static void app_uart0_init();
static void uart0_irq_handler(const struct device *dev, void *context);

static void app_uart1_init();
static void uart1_irq_handler(const struct device *dev, void *context);

static K_SEM_DEFINE(tx_sem, 0, 1); // Semaphore to signal UART data transmitted

static uint8_t *tx_data;
static size_t tx_data_length;

#define RX_BUF_SIZE 64

static void app_uart0_init()
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
					command_line[char_counter] = '\0'; // put null at the end of the string to indicate end of message or end of string			   // reset the char counter
					LOG_INF("UART0_RX(%u): %s \n", char_counter, command_line);
					char_counter = 0;
					memset(&command_line, 0, sizeof(command_line));
				}
			}
		}
	}
}

// Initialize UART1
static void app_uart1_init()
{
	dev_uart1 = DEVICE_DT_GET(DT_NODELABEL(uart1));
	if (!device_is_ready(dev_uart1))
	{
		printk("UART1 is not ready \n");
		return;
	}
	uart_irq_rx_disable(dev_uart1);
	uart_irq_tx_disable(dev_uart1);

	uart_irq_callback_set(dev_uart1, uart1_irq_handler);
	uart_irq_rx_enable(dev_uart1);
}


static void uart1_irq_handler(const struct device *dev, void *context)
{
	uint8_t char_received;
	static uint8_t command_line[RX_BUF_SIZE];
	static int char_counter = 0;

	if (!(uart_irq_rx_ready(dev) || uart_irq_tx_ready(dev)))
	{
		LOG_DBG("spurious interrupt");
	}

	if (uart_irq_tx_ready(dev))
	{
		int tx_sent = uart_fifo_fill(dev, (uint8_t *)tx_data, tx_data_length);

		if (tx_sent <= 0)
		{
			LOG_ERR("Error %d sending data over UART1 bus\n", tx_sent);
			return;
		}

		while (uart_irq_tx_complete(dev) != 1)
		{
			LOG_INF("Wait for UART1 data transmition complete\n");
		}
		LOG_INF("UART1 data transmitted\n");
		uart_irq_tx_disable(dev);
		k_sem_give(&tx_sem);
	}

	if (uart_irq_rx_ready(dev))
	{
		int len = uart_fifo_read(dev, &char_received, 1);
		if (len)
		{
			LOG_INF("UART1 data received: %c\n", char_received);
			command_line[char_counter] = char_received;
			char_counter++;
			if (char_received == '\n' || char_received == '\r')
			{
				command_line[char_counter] = '\0'; // put null at the end of the string to indicate end of message or end of string
				LOG_INF("UART1_RX(%u): %s \n", char_counter, command_line);
				char_counter = 0; // reset the char counter

				memset(&command_line, 0, sizeof(command_line));
			}
		}
	}
}


int uart_irq_tx(const struct device *dev, const uint8_t *buf, size_t len)
{
	tx_data = buf;
	tx_data_length = len;
	uart_irq_tx_enable(dev);
	k_sem_take(&tx_sem, K_FOREVER);
	return len;
}

int main(void)
{
	app_uart0_init();
	app_uart1_init();
	while (1)
	{
		k_msleep(5000);
		uart_irq_tx(dev_uart1, "Hello from UART1\n", 17);
	}
}
