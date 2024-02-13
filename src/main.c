/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "stdio.h"
#include "my_gpio.h"
#include "uart0.h"


#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(nrf52_learning);


#define STACKSIZE 1024


K_THREAD_DEFINE(uart0_parser, STACKSIZE, uart0_parser_thread, NULL, NULL, NULL, 7, 0, 0);


int main(void)
{
	app_uart0_init();
	while(1){	
		k_sleep(K_MSEC(1000));
	}

}
