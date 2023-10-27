/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "stdio.h"
#include "my_gpio.h"
#include "my_adc.h"



#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(nrf52_learning);




int main(void)
{
	configure_adc();
	// int32_t adc_read_voltage7 = ADC_read_mv1();
	// printk("adc_read_voltage7 = %i mV \n",adc_read_voltage7);

	// int32_t adc_read_voltage6 = ADC_read_mv2();
	// printk("adc_read_voltage6 = %i mV \n",adc_read_voltage6);

	int32_t adc_read_voltage_6 = ADC_read_mv(&sequence_6);
	printk("adc_read_voltage_6 = %i mV \n",adc_read_voltage_6);
	return 0;
}
