/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "stdio.h"
//#include "my_gpio.h"

#define LED0_NODE DT_NODELABEL(led0)
#define LED1_NODE DT_NODELABEL(led1)

#define BUTTON0_NODE DT_NODELABEL(button0)
#define BUTTON1_NODE DT_NODELABEL(button1)
#define BUTTON2_NODE DT_NODELABEL(button2)
#define BUTTON3_NODE DT_NODELABEL(button3)

static const struct gpio_dt_spec red_led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec blue_led = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(BUTTON0_NODE, gpios);
static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(BUTTON1_NODE, gpios);
static const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET(BUTTON2_NODE, gpios);
static const struct gpio_dt_spec button4 = GPIO_DT_SPEC_GET(BUTTON3_NODE, gpios);
static struct gpio_callback button1_cb_data;
static struct gpio_callback button2_cb_data;
static struct gpio_callback button3_cb_data;
static struct gpio_callback button4_cb_data;


static void pin_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	//printf("button pressed (pin %u ) \n",pins);
	for (int i = 0; i <= 16; i++) {
        if (pins & (1 << i)) {
            printf("button pressed (pin %d)\n", i);
        }
    }
}

int main(void)
{	

	int ret;

	if (!gpio_is_ready_dt(&red_led)) {
		return 0;
	}
	if (!gpio_is_ready_dt(&blue_led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&red_led, GPIO_OUTPUT | GPIO_ACTIVE_LOW);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&blue_led, GPIO_OUTPUT | GPIO_ACTIVE_LOW);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&button2, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&button3, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&button4, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	//configure_gpios();


	gpio_pin_interrupt_configure_dt(&button1,GPIO_INT_EDGE_TO_ACTIVE);
	gpio_pin_interrupt_configure_dt(&button2,GPIO_INT_EDGE_TO_ACTIVE);
	gpio_pin_interrupt_configure_dt(&button3,GPIO_INT_EDGE_TO_ACTIVE);
	gpio_pin_interrupt_configure_dt(&button4,GPIO_INT_EDGE_TO_ACTIVE);

	gpio_init_callback(&button1_cb_data, pin_isr, BIT(button1.pin));
	gpio_add_callback(button1.port, &button1_cb_data);
	printk("Set up button at %s pin %d\n", button1.port->name, button1.pin);

	gpio_init_callback(&button2_cb_data, pin_isr, BIT(button2.pin));
	gpio_add_callback(button2.port, &button2_cb_data);
	printk("Set up button at %s pin %d\n", button2.port->name, button2.pin);

	gpio_init_callback(&button3_cb_data, pin_isr, BIT(button3.pin));
	gpio_add_callback(button3.port, &button3_cb_data);
	printk("Set up button at %s pin %d\n", button3.port->name, button3.pin);

	gpio_init_callback(&button4_cb_data, pin_isr, BIT(button4.pin));
	gpio_add_callback(button4.port, &button4_cb_data);
	printk("Set up button at %s pin %d\n", button4.port->name, button4.pin);

	


	//configure_gpio();
	
	while (1) {
		ret = gpio_pin_toggle_dt(&red_led);
		if (ret < 0) {
			return 0;
		}
		ret = gpio_pin_toggle_dt(&blue_led);
		if (ret < 0) {
			return 0;
		}
		k_msleep(2000);
	}
	return 0;
}
