#include "usb.h"



#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(usb);

// UART 0 VARIABLES

K_SEM_DEFINE(rx_disabled_usb, 0, 1);

// UART RX primary buffers
uint8_t usb_double_buffer[2][USB_BUF_SIZE];

uint8_t *usb_buf_next = usb_double_buffer[1];

uint8_t complete_message_usb[USB_BUF_SIZE];
uint8_t complete_message_usb_counter = 0;
bool currently_active_buffer_usb = 1; // 0 - uart_double_buffer[0] is active, 1 - uart_double_buffer[1] is active
static const struct device *dev_usb;
static void usb_cb(const struct device *dev, struct uart_event *evt, void *user_data);
// END OF UART0 VARIABLES

K_MSGQ_DEFINE(usb_message_queue, usb_message_size * sizeof(uint8_t *), usb_message_queue_size, 1);

static void usb_cb(const struct device *dev, struct uart_event *evt, void *user_data)
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

        if (currently_active_buffer_usb == 0)
        {
            // read all characters one by one till new line is found
            for (int i = 0 + evt->data.rx.offset; i < USB_BUF_SIZE; i++)
            {
                complete_message_usb[complete_message_usb_counter] = usb_double_buffer[0][i];
                complete_message_usb_counter++;
                if (usb_double_buffer[0][i] == '\n')
                {
                    complete_message_usb_counter = 0;
                    // LOG_INF("complete_message_uart0 = %s \n", complete_message_uart0);
                    k_msgq_put(&usb_message_queue, &complete_message_usb, K_NO_WAIT);
                    memset(&complete_message_usb, 0, sizeof(complete_message_usb)); // clear out the buffer to prepare for next read.
                    break;
                }
            }
        }

        if (currently_active_buffer_usb == 1)
        {
            // read all characters one by one till new line is found
            for (int i = 0 + evt->data.rx.offset; i < USB_BUF_SIZE; i++)
            {
                complete_message_usb[complete_message_usb_counter] = usb_double_buffer[1][i];
                complete_message_usb_counter++;
                if (usb_double_buffer[1][i] == '\n')
                {
                    complete_message_usb_counter = 0;
                    // LOG_INF("complete_message_uart0 = %s \n", complete_message_uart0);
                    k_msgq_put(&usb_message_queue, &complete_message_usb, K_NO_WAIT);
                    memset(&complete_message_usb, 0, sizeof(complete_message_usb)); // clear out the buffer to prepare for next read.
                    break;
                }
            }
        }

        break;

    case UART_RX_BUF_REQUEST:
        uart_rx_buf_rsp(dev_usb, usb_buf_next, USB_BUF_SIZE);
        currently_active_buffer_usb = !currently_active_buffer_usb;
        break;

    case UART_RX_BUF_RELEASED:
        usb_buf_next = evt->data.rx_buf.buf;
        break;

    case UART_RX_DISABLED:
        k_sem_give(&rx_disabled_usb);
        break;

    case UART_RX_STOPPED:
        // do something
        break;

    default:
        break;
    }
}

void app_usb_init()
{

	printk("Starting USB CDC ACM example\n");
	if (usb_enable(NULL))
	{
		return 0;
	}

	dev_usb = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
	if (!device_is_ready(dev_usb))
	{
		return 0;
	}


    int err;
    err = uart_callback_set(dev_usb, usb_cb, NULL);
    if (err)
    {
        return err;
    }
    uart_rx_enable(dev_usb, usb_double_buffer[0], USB_BUF_SIZE, USB_RX_TIMEOUT_MS);
}

void usb_parser_thread(void)
{
    uint8_t data[24];
    while (1)
    {
        /* get a data item */
        k_msgq_get(&usb_message_queue, &data, K_FOREVER);
        LOG_DBG("USB_RX(%u): %s", strlen(data), data);
        k_yield();
    }
}