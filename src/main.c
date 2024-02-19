/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/poweroff.h>


#include "stdio.h"
#include "my_gpio.h"
#include "uart0.h"
#include "uart1.h"
#include "usb.h"


BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
			 "Console device is not ACM CDC UART device");


#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(nrf52_learning);

#define STACKSIZE 1024

 //K_THREAD_DEFINE(uart0_parser, STACKSIZE, uart0_parser_thread, NULL, NULL, NULL, 7, 0, 0);
 //K_THREAD_DEFINE(uart1_parser, STACKSIZE, uart1_parser_thread, NULL, NULL, NULL, 7, 0, 0);
 K_THREAD_DEFINE(usb_parser, STACKSIZE, usb_parser_thread, NULL, NULL, NULL, 7, 0, 0);





int main(void)
{





	app_usb_init();
	//app_uart0_init();
	//app_uart1_init();

	while (1)
	{
		//uart1_send_string("Hello World\n");
		k_sleep(K_MSEC(1000));
	}
}
