
#include <zephyr/kernel.h>
#include "zephyr/drivers/uart.h"
#include <zephyr/sys/ring_buffer.h>

#define UART0_BUF_SIZE 64
#define UART0_RX_TIMEOUT_MS 1000

#define uart0_message_queue_size 10
#define uart0_message_size 64

void app_uart0_init();
void uart0_parser_thread(void);
bool uart0_send_string(const char *str);