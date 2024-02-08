#include "uart1.h"
#include "uart1_parser.h"

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(uart1_parser);

bool parse_message(const uint8_t *message, uint8_t message_length)
{
    LOG_INF("Parsing message: %s", message);
    return true;
}