#include "sensor.h"
#include "main.h"

uint8_t Sensor_IsRaining(void)
{
    return (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET);
}

uint8_t Sensor_IsLight(void)
{
    return (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET);
}
