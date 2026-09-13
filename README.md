## LCD1602 I2C Version

This branch contains the LCD1602 I2C version of the smart drying rack project.
The LCD1602 I2C backpack is based on the PCF8574 I/O expander.
The simulation uses I2C address `0x27`.
### Changes

- LCD1602 communicates through I2C1.
- PB8 is used as I2C1_SCL.
- PB7 is used as I2C1_SDA.
- The LCD I2C address used in the simulation is `0x27`.
- The LCD driver uses `HAL_I2C_Master_Transmit()` instead of direct GPIO control.
- Sensor, control, servo, UART, FreeRTOS tasks, and message queues remain unchanged.

### LCD Connection

| LCD I2C | STM32F411 |
| --- | --- |
| SCL | PB8 |
| SDA | PB7 |
| GND | GND |
| VCC | Power |

### Renode Simulation

The I2C LCD version can also be simulated in Renode using a custom HD44780 I2C peripheral model.

Simulation files:

- `HD44780Mock.cs`
- `drying_rack_f411.repl`
- `Debug/smart_drying_rack_f411_i2c.elf`

Run the simulation with:

```text
include @C:\Users\Admin\Documents\smart_drying_rack_f411_i2c\HD44780Mock.cs
mach create
machine LoadPlatformDescription @C:\Users\Admin\Documents\smart_drying_rack_f411_i2c\drying_rack_f411.repl
sysbus LoadELF @C:\Users\Admin\Documents\smart_drying_rack_f411_i2c\Debug\smart_drying_rack_f411_i2c.elf
showAnalyzer sysbus.usart2
start
```

### Show LCD Content

To display the current LCD1602 content in Renode:

```text
i2c1.LCD16x2 Show
```

Example:

```text
+----------------+
|MODE: AUTO      |
|R:IN  E:DARK    |
+----------------+
```

### Check Servo PWM

The servo is still controlled by TIM4 Channel 1 and uses the same configuration as the parallel LCD version.

To read the TIM4 CCR1 register:

```text
sysbus ReadDoubleWord 0x40000834
```

Expected values:

```text
RACK_IN  -> 0x3E8 = 1000 us = 1.0 ms
RACK_OUT -> 0x5DC = 1500 us = 1.5 ms
```

### Renode Input Control

Rain sensor:

```text
gpioPortA.RainSensor Press
gpioPortA.RainSensor Release
```

Light sensor:

```text
gpioPortA.LightSensor Press
gpioPortA.LightSensor Release
```

Mode button:

```text
gpioPortB.ModeButton Press
gpioPortB.ModeButton Release
```

Manual IN button:

```text
gpioPortB.ManualInButton Press
gpioPortB.ManualInButton Release
```

Manual OUT button:

```text
gpioPortB.ManualOutButton Press
gpioPortB.ManualOutButton Release
```

Only the LCD communication method is changed in this version. The FreeRTOS tasks, message queues, sensor processing, control logic, UART, and servo control remain the same as in the parallel LCD version.
