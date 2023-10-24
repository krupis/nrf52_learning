#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define TEST_123 5

void configure_leds();
void configure_buttons();
void turn_on_red_led();
void turn_on_blue_led();