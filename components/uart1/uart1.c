#include "uart1.h"

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(uart1);


//UART1 VARIABLES
//UART PINS ARE CONFIGURED IN THE DTS FILE ( P1.01 and P1.02)
K_SEM_DEFINE(rx_disabled_uart1, 0, 1);

// UART RX primary buffers
uint8_t uart1_double_buffer[2][UART1_BUF_SIZE];

uint8_t *uart1_buf_next = uart1_double_buffer[1];

uint8_t complete_message_uart1[UART1_BUF_SIZE];
uint8_t complete_message_uart1_counter = 0;
bool currently_active_buffer_uart1 = 1; // 0 - uart_double_buffer[0] is active, 1 - uart_double_buffer[1] is active
static const struct device *dev_uart1;
static void uart1_cb(const struct device *dev, struct uart_event *evt, void *user_data);


static void uart1_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{

	switch (evt->type)
	{

	case UART_TX_DONE:
		break;

	case UART_TX_ABORTED:
		// do something
		break;

	case UART_RX_RDY:

		//LOG_INF("Received %i bytes \n", evt->data.rx.len);
		//LOG_INF("Offset = %i  \n", evt->data.rx.offset);

		if (currently_active_buffer_uart1 == 0)
		{
			// read all characters one by one till new line is found
			for (int i = 0 + evt->data.rx.offset; i < UART1_BUF_SIZE; i++)
			{
				complete_message_uart1[complete_message_uart1_counter] = uart1_double_buffer[0][i];
				complete_message_uart1_counter++;
				if (uart1_double_buffer[0][i] == '\n')
				{
					complete_message_uart1_counter = 0;
					LOG_INF("complete_message_uart1 = %s \n", complete_message_uart1);
					memset(&complete_message_uart1, 0, sizeof(complete_message_uart1)); // clear out the buffer to prepare for next read.
					break;
				}
			}
		}

		if (currently_active_buffer_uart1 == 1)
		{
			// read all characters one by one till new line is found
			for (int i = 0 + evt->data.rx.offset; i < UART1_BUF_SIZE; i++)
			{
				complete_message_uart1[complete_message_uart1_counter] = uart1_double_buffer[1][i];
				complete_message_uart1_counter++;
				if (uart1_double_buffer[1][i] == '\n')
				{
					complete_message_uart1_counter = 0;
					LOG_INF("complete_message_uart1 = %s \n", complete_message_uart1);
					memset(&complete_message_uart1, 0, sizeof(complete_message_uart1)); // clear out the buffer to prepare for next read.
					break;
				}
			}
		}

		break;

	case UART_RX_BUF_REQUEST:
		uart_rx_buf_rsp(dev_uart1, uart1_buf_next, UART1_BUF_SIZE);
		currently_active_buffer_uart1 = !currently_active_buffer_uart1;
		break;

	case UART_RX_BUF_RELEASED:
		uart1_buf_next = evt->data.rx_buf.buf;
		break;

	case UART_RX_DISABLED:
		k_sem_give(&rx_disabled_uart1);
		break;

	case UART_RX_STOPPED:
		// do something
		break;

	default:
		break;
	}
}

void app_uart1_init()
{
	dev_uart1 = DEVICE_DT_GET(DT_NODELABEL(uart1));

	if (!device_is_ready(dev_uart1))
	{
		return 0;
	}

	int err;
	err = uart_callback_set(dev_uart1, uart1_cb, NULL);
	if (err)
	{
		return err;
	}
	uart_rx_enable(dev_uart1, uart1_double_buffer[0], UART1_BUF_SIZE, UART1_RX_TIMEOUT_MS);
}


//mest be terminated with \n
void uart1_send_string(const char *str){
    LOG_INF("Sending string: %s", str);
    LOG_INF("String length: %d", strlen(str));
    uart_tx(dev_uart1, str, strlen(str), 1000);
}


//END OF UART1 FUNCTIONS