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

const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

//static uint8_t tx_buf[] = {"nRF Connect SDK Fundamentals Course \n\r"};
static uint8_t rx_buf[20] = {0}; // A buffer to store incoming UART data



#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(nrf52_learning);





const struct uart_config uart_cfg = {
	.baudrate = 115200,
	.parity = UART_CFG_PARITY_NONE,
	.stop_bits = UART_CFG_STOP_BITS_1,
	.data_bits = UART_CFG_DATA_BITS_8,
	.flow_ctrl = UART_CFG_FLOW_CTRL_NONE};

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
		//printf("Data received = %s \n",evt->data.rx_buf);
		//printf("Data received length = %u \n",evt->data.rx.len);

		LOG_HEXDUMP_DBG(&evt->data.rx_buf.buf[evt->data.rx.offset], evt->data.rx.len  , "Data received: ");




		break;

	case UART_RX_BUF_REQUEST:
		printf("requesting buffer \n");
		// do something
		break;

	case UART_RX_BUF_RELEASED:
		printf("buffer released \n");
		// do something
		break;

	case UART_RX_DISABLED:
		printf("rx disabled \n");
		uart_rx_enable(dev, rx_buf, sizeof(rx_buf), 100);
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

	if (!device_is_ready(uart))
	{
		return 0;
	}

	int err;
	err = uart_callback_set(uart, uart_cb, NULL);
	if (err)
	{
		return err;
	}

	uart_rx_enable(uart, rx_buf, sizeof(rx_buf), 100);

	// err = uart_tx(uart, tx_buf, sizeof(tx_buf), SYS_FOREVER_US);
	// if (err)
	// {
	// 	return err;
	// }

	// configure_leds();
	// configure_buttons();
	// turn_on_red_led();
	// turn_on_blue_led();

	while (1)
	{
		k_msleep(1000);
	}
	return 0;
}
