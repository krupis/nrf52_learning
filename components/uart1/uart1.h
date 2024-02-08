
#include <zephyr/kernel.h>
#include "zephyr/drivers/uart.h"
#include <zephyr/sys/ring_buffer.h>

#define UART1_BUF_SIZE 24
#define UART1_RX_TIMEOUT_MS 1000

//MESSAGE QUEUE
#define uart1_message_queue_size 10
#define uart1_message_size 24

void app_uart1_init();
bool uart1_send_string(const char *str);
void uart1_parser_thread(void);