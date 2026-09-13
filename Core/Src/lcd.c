#include "lcd.h"
#include "main.h"

/*
 * LCD1602 + I2C backpack PCF8574
 *
 * Common mapping:
 * P0 -> RS
 * P1 -> RW
 * P2 -> EN
 * P3 -> Backlight
 * P4 -> D4
 * P5 -> D5
 * P6 -> D6
 * P7 -> D7
 */

extern I2C_HandleTypeDef hi2c1;

/*
 * STM32 HAL expects the 7-bit I2C address shifted left by 1.
 *
 * Common LCD backpack address: 0x27
 * If the LCD does not respond, the real address may be 0x3F
 * or another value.
 */
#define LCD_I2C_ADDR       (0x27U << 1)

#define LCD_RS             0x01U
#define LCD_EN             0x04U
#define LCD_BACKLIGHT      0x08U


static void LCD_I2C_Write(uint8_t data)
{
    HAL_I2C_Master_Transmit(
        &hi2c1,
        LCD_I2C_ADDR,
        &data,
        1U,
        HAL_MAX_DELAY
    );
}


static void LCD_EnablePulse(uint8_t data)
{
    LCD_I2C_Write(data | LCD_EN);
    HAL_Delay(1U);

    LCD_I2C_Write(data & ~LCD_EN);
    HAL_Delay(1U);
}


static void LCD_SendNibble(uint8_t data, uint8_t rs)
{
    uint8_t output;

    /*
     * data only contains the lower 4 bits.
     * Shift them to P4-P7 of the PCF8574.
     */
    output = (uint8_t)((data & 0x0FU) << 4U);

    /*
     * Keep LCD backlight ON.
     */
    output |= LCD_BACKLIGHT;

    /*
     * RS = 1 -> data
     * RS = 0 -> command
     */
    if(rs != 0U)
    {
        output |= LCD_RS;
    }

    LCD_EnablePulse(output);
}


static void LCD_SendCommand(uint8_t command)
{
    LCD_SendNibble(command >> 4U, 0U);
    LCD_SendNibble(command & 0x0FU, 0U);

    HAL_Delay(2U);
}


static void LCD_SendData(uint8_t data)
{
    LCD_SendNibble(data >> 4U, 1U);
    LCD_SendNibble(data & 0x0FU, 1U);
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
    /*
     * Wait for LCD power-up.
     */
    HAL_Delay(50U);

    /*
     * Initial state:
     * backlight ON, RS = 0, EN = 0.
     */
    LCD_I2C_Write(LCD_BACKLIGHT);

    /*
     * HD44780 initialization sequence.
     */
    LCD_SendNibble(0x03U, 0U);
    HAL_Delay(5U);

    LCD_SendNibble(0x03U, 0U);
    HAL_Delay(1U);

    LCD_SendNibble(0x03U, 0U);
    HAL_Delay(1U);

    /*
     * Switch to 4-bit mode.
     */
    LCD_SendNibble(0x02U, 0U);

    /*
     * 4-bit interface, 2 lines, 5x8 font.
     */
    LCD_SendCommand(0x28U);

    /*
     * Display ON, cursor OFF, blink OFF.
     */
    LCD_SendCommand(0x0CU);

    /*
     * Entry mode: increment cursor.
     */
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
