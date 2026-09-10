#include "uart.h"
#include "main.h"
#include <string.h>

extern UART_HandleTypeDef huart2;

void UART2_SendString(const char *str)
{
    HAL_UART_Transmit(
        &huart2,
        (uint8_t *)str,
        strlen(str),
        HAL_MAX_DELAY
    );
}
