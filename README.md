# Smart Clothes Drying Rack using STM32F411CEU6 and FreeRTOS

An embedded smart clothes drying rack system developed using **STM32F411CEU6** and **FreeRTOS**. This contains the LCD1602 I2C version of the smart drying rack project.

The system automatically extends or retracts the clothes rack based on rainfall and ambient light conditions. It also supports manual control using push buttons.

---

## Features

- Rain detection
- Ambient light detection
- AUTO and MANUAL modes
- Servo control for Rack IN / OUT
- LCD 16x2 status display
- UART monitoring
- FreeRTOS multitasking
- Inter-task communication using message queues
- Renode simulation

---

## Hardware

| Component | Description |
|---|---|
| STM32F411CEU6 | Main microcontroller |
| Rain Sensor | Detects rainfall |
| LDR Sensor | Detects ambient light |
| LCD 16x2 | Displays system status |
| SG90 Servo | Rack IN / OUT actuator |
| Push Buttons | Mode and manual control |

---

## Pin Mapping

| Function | STM32 Pin |
|---|---|
| Rain Sensor | PA1 |
| LDR Sensor | PA4 |
| Mode Button | PB0 |
| Manual IN Button | PB1 |
| Manual OUT Button | PB2 |
| Servo PWM | PB6 - TIM4_CH1 |
| UART TX | PA2 - USART2_TX |
| LCD RS | PB8 |
| LCD E | PB9 |
| LCD D4 | PB12 |
| LCD D5 | PB13 |
| LCD D6 | PB14 |
| LCD D7 | PB15 |

---

## FreeRTOS Architecture

```text
Sensors + Buttons
       │
       ▼
   SensorTask
       │
   sensorQueue
       │
       ▼
   ControlTask
       │
       ├──► Servo
       │
   displayQueue
       │
       ▼
   DisplayTask
       │
       ├──► LCD 16x2
       └──► UART
```

| Task | Priority | Function |
|---|---|---|
| ControlTask | AboveNormal | System logic and servo control |
| SensorTask | Normal | Reads sensors and buttons |
| DisplayTask | BelowNormal | LCD and UART output |

The project uses **FreeRTOS through CMSIS-RTOS V2**.

---

## System Logic

### AUTO Mode

| Rain | Light | Action |
|---|---|---|
| Yes | Any | Rack IN |
| No | BRIGHT | Rack OUT |
| No | DARK | Rack IN |

### MANUAL Mode

| Button | Action |
|---|---|
| Manual IN | Rack IN |
| Manual OUT | Rack OUT |

---

## Simulation

Simulated using [Renode](https://renode.io/) — no physical hardware is required for firmware testing.

The simulation supports:

- Rain and light sensor inputs
- Push buttons
- Servo PWM
- UART monitoring
- LCD 16x2
- FreeRTOS task scheduling

### Run with Renode

```text
include @HD44780Mock_F411.cs

mach create

machine LoadPlatformDescription @drying_rack_f411_with_lcd.repl

sysbus LoadELF @Debug\smart_drying_rack_f411.elf

showAnalyzer sysbus.usart2

start
```

### Test Commands

```text
gpioPortA.RainSensor Press
gpioPortA.RainSensor Release

gpioPortA.LightSensor Press
gpioPortA.LightSensor Release

gpioPortB.ModeButton Press
gpioPortB.ModeButton Release

gpioPortB.ManualInButton Press
gpioPortB.ManualInButton Release

gpioPortB.ManualOutButton Press
gpioPortB.ManualOutButton Release
```

Show LCD:

```text
gpioPortB.LCD16x2 Show
```

Check servo PWM:

```text
sysbus ReadDoubleWord 0x40000834
```

Expected values:

```text
Rack IN  -> 1000
Rack OUT -> 1500
```

---

## Tools

- STM32CubeMX
- STM32CubeIDE
- STM32 HAL
- FreeRTOS
- CMSIS-RTOS V2
- Renode
- Git / GitHub
