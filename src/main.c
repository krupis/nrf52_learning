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


//UART 0 VARIABLES
#define UART0_BUF_SIZE 24
#define UART0_RX_TIMEOUT_MS 1000
K_SEM_DEFINE(rx_disabled_uart0, 0, 1);

// UART RX primary buffers
uint8_t uart0_double_buffer[2][UART0_BUF_SIZE];

uint8_t *uart0_buf_next = uart0_double_buffer[1];

uint8_t complete_message_uart0[UART0_BUF_SIZE];
uint8_t complete_message_uart0_counter = 0;
bool currently_active_buffer_uart0 = 1; // 0 - uart_double_buffer[0] is active, 1 - uart_double_buffer[1] is active
static const struct device *dev_uart0;
static void uart0_cb(const struct device *dev, struct uart_event *evt, void *user_data);
static void app_uart0_init();
//END OF UART0 VARIABLES



//UART1 VARIABLES
//UART PINS ARE CONFIGURED IN THE DTS FILE ()
#define UART1_BUF_SIZE 24
#define UART1_RX_TIMEOUT_MS 1000
K_SEM_DEFINE(rx_disabled_uart1, 0, 1);

// UART RX primary buffers
uint8_t uart1_double_buffer[2][UART1_BUF_SIZE];

uint8_t *uart1_buf_next = uart1_double_buffer[1];

uint8_t complete_message_uart1[UART0_BUF_SIZE];
uint8_t complete_message_uart1_counter = 0;
bool currently_active_buffer_uart1 = 1; // 0 - uart_double_buffer[0] is active, 1 - uart_double_buffer[1] is active
static const struct device *dev_uart1;
static void uart1_cb(const struct device *dev, struct uart_event *evt, void *user_data);
static void app_uart1_init();
//END OF UART1 VARIABLES






static void uart0_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{

	switch (evt->type)
	{

	case UART_TX_DONE:
		break;

	case UART_TX_ABORTED:
		// do something
		break;

	case UART_RX_RDY:

		//LOG_INF("Received %i bytes \n", evt->data.rx.len);
		//LOG_INF("Offset = %i  \n", evt->data.rx.offset);

		if (currently_active_buffer_uart0 == 0)
		{
			// read all characters one by one till new line is found
			for (int i = 0 + evt->data.rx.offset; i < UART0_BUF_SIZE; i++)
			{
				complete_message_uart0[complete_message_uart0_counter] = uart0_double_buffer[0][i];
				complete_message_uart0_counter++;
				if (uart0_double_buffer[0][i] == '\n')
				{
					complete_message_uart0_counter = 0;
					LOG_INF("complete_message_uart0 = %s \n", complete_message_uart0);
					memset(&complete_message_uart0, 0, sizeof(complete_message_uart0)); // clear out the buffer to prepare for next read.
					break;
				}
			}
		}

		if (currently_active_buffer_uart0 == 1)
		{
			// read all characters one by one till new line is found
			for (int i = 0 + evt->data.rx.offset; i < UART0_BUF_SIZE; i++)
			{
				complete_message_uart0[complete_message_uart0_counter] = uart0_double_buffer[1][i];
				complete_message_uart0_counter++;
				if (uart0_double_buffer[1][i] == '\n')
				{
					complete_message_uart0_counter = 0;
					LOG_INF("complete_message_uart0 = %s \n", complete_message_uart0);
					memset(&complete_message_uart0, 0, sizeof(complete_message_uart0)); // clear out the buffer to prepare for next read.
					break;
				}
			}
		}

		break;

	case UART_RX_BUF_REQUEST:
		uart_rx_buf_rsp(dev_uart0, uart0_buf_next, UART0_BUF_SIZE);
		currently_active_buffer_uart0 = !currently_active_buffer_uart0;
		break;

	case UART_RX_BUF_RELEASED:
		uart0_buf_next = evt->data.rx_buf.buf;
		break;

	case UART_RX_DISABLED:
		k_sem_give(&rx_disabled_uart0);
		break;

	case UART_RX_STOPPED:
		// do something
		break;

	default:
		break;
	}
}

static void app_uart0_init()
{
	dev_uart0 = DEVICE_DT_GET(DT_NODELABEL(uart0));

	if (!device_is_ready(dev_uart0))
	{
		return 0;
	}

	int err;
	err = uart_callback_set(dev_uart0, uart0_cb, NULL);
	if (err)
	{
		return err;
	}
	uart_rx_enable(dev_uart0, uart0_double_buffer[0], UART0_BUF_SIZE, UART0_RX_TIMEOUT_MS);
}








//UART1 FUNCTIONS

static void uart1_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{

	switch (evt->type)
	{

	case UART_TX_DONE:
		break;

	case UART_TX_ABORTED:
		// do something
		break;

	case UART_RX_RDY:

		//LOG_INF("Received %i bytes \n", evt->data.rx.len);
		//LOG_INF("Offset = %i  \n", evt->data.rx.offset);

		if (currently_active_buffer_uart1 == 0)
		{
			// read all characters one by one till new line is found
			for (int i = 0 + evt->data.rx.offset; i < UART1_BUF_SIZE; i++)
			{
				complete_message_uart1[complete_message_uart1_counter] = uart1_double_buffer[0][i];
				complete_message_uart1_counter++;
				if (uart1_double_buffer[0][i] == '\n')
				{
					complete_message_uart1_counter = 0;
					LOG_INF("complete_message_uart1 = %s \n", complete_message_uart1);
					memset(&complete_message_uart1, 0, sizeof(complete_message_uart1)); // clear out the buffer to prepare for next read.
					break;
				}
			}
		}

		if (currently_active_buffer_uart1 == 1)
		{
			// read all characters one by one till new line is found
			for (int i = 0 + evt->data.rx.offset; i < UART1_BUF_SIZE; i++)
			{
				complete_message_uart1[complete_message_uart1_counter] = uart1_double_buffer[1][i];
				complete_message_uart1_counter++;
				if (uart1_double_buffer[1][i] == '\n')
				{
					complete_message_uart1_counter = 0;
					LOG_INF("complete_message_uart1 = %s \n", complete_message_uart1);
					memset(&complete_message_uart1, 0, sizeof(complete_message_uart1)); // clear out the buffer to prepare for next read.
					break;
				}
			}
		}

		break;

	case UART_RX_BUF_REQUEST:
		uart_rx_buf_rsp(dev_uart1, uart1_buf_next, UART1_BUF_SIZE);
		currently_active_buffer_uart1 = !currently_active_buffer_uart1;
		break;

	case UART_RX_BUF_RELEASED:
		uart1_buf_next = evt->data.rx_buf.buf;
		break;

	case UART_RX_DISABLED:
		k_sem_give(&rx_disabled_uart1);
		break;

	case UART_RX_STOPPED:
		// do something
		break;

	default:
		break;
	}
}

static void app_uart1_init()
{
	dev_uart1 = DEVICE_DT_GET(DT_NODELABEL(uart1));

	if (!device_is_ready(dev_uart1))
	{
		return 0;
	}

	int err;
	err = uart_callback_set(dev_uart1, uart1_cb, NULL);
	if (err)
	{
		return err;
	}
	uart_rx_enable(dev_uart1, uart1_double_buffer[0], UART1_BUF_SIZE, UART1_RX_TIMEOUT_MS);
}
//END OF UART1 FUNCTIONS

int main(void)
{
	app_uart0_init();
	app_uart1_init();
	while(1){	
		uart_tx(dev_uart1, "Hello World\n", 12, 1000);
		k_sleep(K_MSEC(1000));
	}

}
