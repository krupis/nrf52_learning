/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include "stdio.h"
#include "zephyr/drivers/uart.h"



static const struct device *dev_uart;



void app_uart_init()
{
	dev_uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

	if (!device_is_ready(dev_uart))
	{
		return 0;
	}



}

int main(void)
{
	app_uart_init();


}
