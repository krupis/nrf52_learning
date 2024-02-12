#include "uart0.h"

#include "uart1.h"

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(uart0);

// UART 0 VARIABLES

K_SEM_DEFINE(rx_disabled_uart0, 0, 1);

// UART RX primary buffers
uint8_t uart0_double_buffer[2][UART0_BUF_SIZE];

uint8_t *uart0_buf_next = uart0_double_buffer[1];

uint8_t complete_message_uart0[UART0_BUF_SIZE];
uint8_t complete_message_uart0_counter = 0;
bool currently_active_buffer_uart0 = 1; // 0 - uart_double_buffer[0] is active, 1 - uart_double_buffer[1] is active
static const struct device *dev_uart0;
static void uart0_cb(const struct device *dev, struct uart_event *evt, void *user_data);
// END OF UART0 VARIABLES

K_MSGQ_DEFINE(uart0_message_queue, uart0_message_size * sizeof(uint8_t *), uart0_message_queue_size, 1);

static void uart0_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{

    switch (evt->type)
    {

    case UART_TX_DONE:
        break;

    case UART_TX_ABORTED:
        // do something
        break;

    case UART_RX_RDY:

        // LOG_INF("Received %i bytes \n", evt->data.rx.len);
        // LOG_INF("Offset = %i  \n", evt->data.rx.offset);

        if (currently_active_buffer_uart0 == 0)
        {
            // read all characters one by one till new line is found
            for (int i = 0 + evt->data.rx.offset; i < UART0_BUF_SIZE; i++)
            {
                complete_message_uart0[complete_message_uart0_counter] = uart0_double_buffer[0][i];
                complete_message_uart0_counter++;
                if (uart0_double_buffer[0][i] == '\n')
                {
                    complete_message_uart0_counter = 0;
                    // LOG_INF("complete_message_uart0 = %s \n", complete_message_uart0);
                    k_msgq_put(&uart0_message_queue, &complete_message_uart0, K_NO_WAIT);
                    memset(&complete_message_uart0, 0, sizeof(complete_message_uart0)); // clear out the buffer to prepare for next read.
                    break;
                }
            }
        }

        if (currently_active_buffer_uart0 == 1)
        {
            // read all characters one by one till new line is found
            for (int i = 0 + evt->data.rx.offset; i < UART0_BUF_SIZE; i++)
            {
                complete_message_uart0[complete_message_uart0_counter] = uart0_double_buffer[1][i];
                complete_message_uart0_counter++;
                if (uart0_double_buffer[1][i] == '\n')
                {
                    complete_message_uart0_counter = 0;
                    // LOG_INF("complete_message_uart0 = %s \n", complete_message_uart0);
                    k_msgq_put(&uart0_message_queue, &complete_message_uart0, K_NO_WAIT);
                    memset(&complete_message_uart0, 0, sizeof(complete_message_uart0)); // clear out the buffer to prepare for next read.
                    break;
                }
            }
        }

        break;

    case UART_RX_BUF_REQUEST:
        uart_rx_buf_rsp(dev_uart0, uart0_buf_next, UART0_BUF_SIZE);
        currently_active_buffer_uart0 = !currently_active_buffer_uart0;
        break;

    case UART_RX_BUF_RELEASED:
        uart0_buf_next = evt->data.rx_buf.buf;
        break;

    case UART_RX_DISABLED:
        k_sem_give(&rx_disabled_uart0);
        break;

    case UART_RX_STOPPED:
        // do something
        break;

    default:
        break;
    }
}

void app_uart0_init()
{
    dev_uart0 = DEVICE_DT_GET(DT_NODELABEL(uart0));

    if (!device_is_ready(dev_uart0))
    {
        return 0;
    }

    int err;
    err = uart_callback_set(dev_uart0, uart0_cb, NULL);
    if (err)
    {
        return err;
    }
    uart_rx_enable(dev_uart0, uart0_double_buffer[0], UART0_BUF_SIZE, UART0_RX_TIMEOUT_MS);
}

void uart0_parser_thread(void)
{
    uint8_t data[24];
    while (1)
    {
        /* get a data item */
        k_msgq_get(&uart0_message_queue, &data, K_FOREVER);
        LOG_DBG("UART0_RX(%u): %s", strlen(data), data);
        if (strncmp((char *)data, "UART1:", 6) == 0)
        {
            strtok(data, ":"); // Split and ignore the "UART6" part
            char *token = strtok(NULL, " ");
            while (token != NULL)
            {
                char *message_buff = malloc(UART0_BUF_SIZE);
                if (message_buff == NULL)
                {
                    LOG_ERR("Failed to allocate memory \n");
                    k_msleep(1000);
                    return;
                }
                // Copy the token into message_buff
                strncpy(message_buff, token, UART0_BUF_SIZE - 1);
                message_buff[-1] = '\0'; // Ensure null termination
                // Append newline character if space is available
                size_t len = strlen(message_buff);
                if (len < UART0_BUF_SIZE - 1)
                {
                    message_buff[len] = '\n';
                    message_buff[len + 1] = '\0';
                }

                // Print the message
                //LOG_DBG("%s", message_buff);                             // Newline already included
                //LOG_DBG("Message length = %u \n", strlen(message_buff)); // Newline already included
                //  Free the allocated memory
                uart1_send_string(message_buff);

                free(message_buff);
                // Move to the next token
                token = strtok(NULL, " ");
            }
            //return;
        }
        k_yield();
    }
}