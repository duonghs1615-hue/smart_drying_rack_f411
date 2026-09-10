#include "lcd.h"
#include "main.h"


#define LCD_PORT       GPIOB

#define LCD_RS_PIN     GPIO_PIN_8
#define LCD_EN_PIN     GPIO_PIN_9

#define LCD_D4_PIN     GPIO_PIN_12
#define LCD_D5_PIN     GPIO_PIN_13
#define LCD_D6_PIN     GPIO_PIN_14
#define LCD_D7_PIN     GPIO_PIN_15


static void LCD_EnablePulse(void)
{
    HAL_GPIO_WritePin(
        LCD_PORT,
        LCD_EN_PIN,
        GPIO_PIN_SET
    );

    for(volatile uint32_t i = 0; i < 100U; i++)
    {
    }

    HAL_GPIO_WritePin(
        LCD_PORT,
        LCD_EN_PIN,
        GPIO_PIN_RESET
    );

    for(volatile uint32_t i = 0; i < 100U; i++)
    {
    }
}


static void LCD_SendNibble(uint8_t data)
{
    HAL_GPIO_WritePin(
        LCD_PORT,
        LCD_D4_PIN,
        (data & 0x01U) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        LCD_PORT,
        LCD_D5_PIN,
        (data & 0x02U) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        LCD_PORT,
        LCD_D6_PIN,
        (data & 0x04U) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        LCD_PORT,
        LCD_D7_PIN,
        (data & 0x08U) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    LCD_EnablePulse();
}


static void LCD_SendCommand(uint8_t command)
{
    HAL_GPIO_WritePin(
        LCD_PORT,
        LCD_RS_PIN,
        GPIO_PIN_RESET
    );

    LCD_SendNibble(command >> 4U);
    LCD_SendNibble(command & 0x0FU);

    HAL_Delay(2U);
}


static void LCD_SendData(uint8_t data)
{
    HAL_GPIO_WritePin(
        LCD_PORT,
        LCD_RS_PIN,
        GPIO_PIN_SET
    );

    LCD_SendNibble(data >> 4U);
    LCD_SendNibble(data & 0x0FU);
}


static void LCD_SendString(const char *str)
{
    while(*str != '\0')
    {
        LCD_SendData((uint8_t)*str);
        str++;
    }
}


static void LCD_SetCursor(uint8_t row, uint8_t column)
{
    uint8_t address;

    if(row == 0U)
    {
        address = column;
    }
    else
    {
        address = 0x40U + column;
    }

    LCD_SendCommand(0x80U | address);
}


void LCD_Init(void)
{
    HAL_Delay(40U);

    HAL_GPIO_WritePin(
        LCD_PORT,
        LCD_RS_PIN | LCD_EN_PIN,
        GPIO_PIN_RESET
    );

    LCD_SendNibble(0x03U);
    HAL_Delay(5U);

    LCD_SendNibble(0x03U);
    HAL_Delay(1U);

    LCD_SendNibble(0x03U);
    HAL_Delay(1U);

    LCD_SendNibble(0x02U);

    /* 4-bit, 2-line */
    LCD_SendCommand(0x28U);

    /* Display ON, cursor OFF */
    LCD_SendCommand(0x0CU);

    /* Increment cursor */
    LCD_SendCommand(0x06U);

    LCD_Clear();
}


void LCD_Clear(void)
{
    LCD_SendCommand(0x01U);
    HAL_Delay(2U);
}


void LCD_Update(
    SystemMode mode,
    RackState rack,
    EnvironmentState environment
)
{
    LCD_Clear();

    /* Line 1 */
    LCD_SetCursor(0U, 0U);

    if(mode == MODE_AUTO)
    {
        LCD_SendString("MODE: AUTO");
    }
    else
    {
        LCD_SendString("MODE: MANUAL");
    }

    /* Line 2 */
    LCD_SetCursor(1U, 0U);

    if(rack == RACK_IN)
    {
        LCD_SendString("R:IN ");
    }
    else
    {
        LCD_SendString("R:OUT");
    }

    LCD_SendString(" E:");

    if(environment == ENV_RAIN)
    {
        LCD_SendString("RAIN");
    }
    else if(environment == ENV_BRIGHT)
    {
        LCD_SendString("BRIGHT");
    }
    else
    {
        LCD_SendString("DARK");
    }
}
