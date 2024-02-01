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


static void uart0_irq_handler(const struct device *dev, void *context);




static void uart1_irq_receive_handler(const struct device *dev, void *context);
static void uart1_irq_transmit_handler(const struct device *dev, void *context);

static K_SEM_DEFINE(tx_sem, 0, 1); // Semaphore to signal UART data transmitted

static uint8_t* tx_data;
static size_t tx_data_length;

#define RX_BUF_SIZE 64


//Initialize UART0
void app_uart0_init()
{
	dev_uart0 = DEVICE_DT_GET(DT_NODELABEL(uart0));
	if (!device_is_ready(dev_uart0))
	{
		printk("UART0 is not ready \n");
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
				printk("UART0 Received = %s \n", command_line);
				memset(&command_line, 0, sizeof(command_line));
			}
		}
	}
}




//Initialize UART1
void app_uart1_init()
{
	dev_uart1 = DEVICE_DT_GET(DT_NODELABEL(uart1));
	if (!device_is_ready(dev_uart1))
	{
		printk("UART1 is not ready \n");
		return;
	}
	uart_irq_callback_set(dev_uart1, uart1_irq_receive_handler);
	uart_irq_rx_enable(dev_uart1);
}



static void uart1_irq_receive_handler(const struct device *dev, void *context)
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
				printk("UART1 Received = %s \n", command_line);
				memset(&command_line, 0, sizeof(command_line));
			}
		}
	}
}



static void uart1_irq_transmit_handler(const struct device *dev, void *user_data)
{
    ARG_UNUSED(user_data);
    static int tx_data_idx;
    if (!uart_irq_update(dev))
    {
            LOG_ERR("Couldn't update IRQ\n");
            return;
    }



    if (uart_irq_tx_ready(dev))
    {
		int tx_sent = uart_fifo_fill(dev, (uint8_t *)tx_data, tx_data_length);

        if (tx_sent <= 0)
        {
             LOG_ERR("Error %d sending data over UART1 bus\n", tx_sent);
             return;
        }

		// tx_data_idx += tx_sent;
        // if (tx_data_idx == tx_data_length)
        // {
		// 	LOG_INF("UART1 data transmitted\n");
        //     uart_irq_tx_disable(dev);
        //     tx_data_idx = 0;
        //     k_sem_give(&tx_sem);
        // }

		while(uart_irq_tx_complete(dev) != 1){
			LOG_INF("Wait for UART1 data transmition complete\n");
		}
		LOG_INF("UART1 data transmitted\n");
		uart_irq_tx_disable(dev);
		k_sem_give(&tx_sem);
		
	}


    // if (uart_irq_tx_ready(dev) && tx_data_idx < tx_data_length)
    // {
	// 	LOG_INF("Transmitting data over UART1\n");
	// 	LOG_INF("tx_data_length = %u \n",tx_data_length);
	// 	LOG_INF("tx_data_idx = %u \n",tx_data_idx);
    //     int tx_send = MIN(CONFIG_UART_1_NRF_TX_BUFFER_SIZE, tx_data_length - tx_data_idx);
    //     int tx_sent = uart_fifo_fill(dev, (uint8_t *)&tx_data[tx_data_idx], tx_send);
	    //     if (tx_sent <= 0)
        // {
        //      LOG_ERR("Error %d sending data over UART1 bus\n", tx_sent);
        //      return;
        // }

    //     tx_data_idx += tx_sent;
    //     if (tx_data_idx == tx_data_length)
    //     {
    //         uart_irq_tx_disable(dev);
    //         tx_data_idx = 0;
    //         k_sem_give(&tx_sem);
    //     }
	// }


    
 }

int uart_irq_tx(const struct device *dev, const uint8_t *buf, size_t len)
{
    tx_data = buf;
    tx_data_length = len;
    uart_irq_callback_set(dev, uart1_irq_transmit_handler);
    uart_irq_tx_enable(dev);
    k_sem_take(&tx_sem, K_MSEC(1000));
    //LOG_INF("UART data transmitted\n");
    return len; 
}










int main(void)
{
	app_uart0_init();
	app_uart1_init();
	while(1){
		k_msleep(1000);
		uart_irq_tx(dev_uart1, "Hello from UART1\n", 17);
	}

}
