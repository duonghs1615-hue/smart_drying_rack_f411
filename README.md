## LCD1602 I2C Version

This branch is an extended version of the project using an LCD1602 with an I2C backpack instead of the parallel GPIO interface.

### Changes

- LCD1602 communicates through I2C1.
- PB8 is used as I2C1_SCL.
- PB7 is used as I2C1_SDA.
- The LCD I2C address used in the simulation is `0x27`.
- The LCD driver uses `HAL_I2C_Master_Transmit()` instead of direct GPIO control.
- Sensor, control, servo, UART, FreeRTOS tasks, and message queues remain unchanged.

### LCD Connection

| LCD I2C | STM32F411 |
|---|---|
| SCL | PB8 |
| SDA | PB7 |
| GND | GND |
| VCC | Power |

### Renode Simulation

The I2C LCD version can also be simulated in Renode using a custom HD44780 I2C peripheral model.

Simulation files:

- `HD44780Mock.cs`
- `drying_rack_f411.repl`
- `Debug/smart_drying_rack_f411.elf`

Run in Renode:

```text
include @C:\Users\Admin\Documents\smart_drying_rack_f411_i2c\HD44780Mock.cs
mach create
machine LoadPlatformDescription @C:\Users\Admin\Documents\smart_drying_rack_f411_i2c\drying_rack_f411.repl
sysbus LoadELF @C:\Users\Admin\Documents\smart_drying_rack_f411_i2c\Debug\smart_drying_rack_f411.elf
showAnalyzer sysbus.usart2
start
