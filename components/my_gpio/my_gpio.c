#include "my_gpio.h"


#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec red_led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec blue_led = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

void configure_gpios()
{
	uint8_t test = TEST_123;
	int ret;

	if (!gpio_is_ready_dt(&red_led)) {
		return 0;
	}
	if (!gpio_is_ready_dt(&blue_led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&red_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&blue_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}
}