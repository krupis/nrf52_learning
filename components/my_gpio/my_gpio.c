#include "my_gpio.h"


#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec red_led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec blue_led = GPIO_DT_SPEC_GET(LED1_NODE, gpios);



#define BUTTON0_NODE DT_NODELABEL(button0)
#define BUTTON1_NODE DT_NODELABEL(button1)
#define BUTTON2_NODE DT_NODELABEL(button2)
#define BUTTON3_NODE DT_NODELABEL(button3)



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



void configure_leds()
{
	uint8_t test = TEST_123;
	int ret;

	if (!gpio_is_ready_dt(&red_led)) {
		return;
	}
	if (!gpio_is_ready_dt(&blue_led)) {
		return;
	}

	ret = gpio_pin_configure_dt(&red_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

	ret = gpio_pin_configure_dt(&blue_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
}






void configure_buttons(){
	int ret;

	ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
	if (ret < 0) {
		return;
	}

	ret = gpio_pin_configure_dt(&button2, GPIO_INPUT);
	if (ret < 0) {
		return;
	}

	ret = gpio_pin_configure_dt(&button3, GPIO_INPUT);
	if (ret < 0) {
		return;
	}

	ret = gpio_pin_configure_dt(&button4, GPIO_INPUT);
	if (ret < 0) {
		return;
	}



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
}



void turn_on_red_led()
{
	gpio_pin_set_dt(&red_led, 1);
}

void turn_on_blue_led()
{
	gpio_pin_set_dt(&blue_led, 1);
}