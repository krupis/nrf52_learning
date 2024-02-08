
#include <zephyr/kernel.h>
#include "zephyr/drivers/uart.h"
#include <zephyr/sys/ring_buffer.h>

#define UART0_BUF_SIZE 24
#define UART0_RX_TIMEOUT_MS 1000

void app_uart0_init();
