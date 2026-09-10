# Smart Clothes Drying Rack using STM32F411CEU6 and FreeRTOS

An embedded smart clothes drying rack system developed using **STM32F411CEU6** and **FreeRTOS**.

The system automatically extends or retracts the clothes drying rack based on rainfall and ambient light conditions. It also supports manual control using push buttons.

The complete firmware can be simulated using **Renode**, including sensors, buttons, UART, servo PWM, and LCD 16x2.

---

## Features

- Rain detection
- Ambient light detection
- Environment states: RAIN / BRIGHT / DARK
- AUTO and MANUAL operating modes
- Servo control for Rack IN / OUT
- LCD 16x2 status display
- UART monitoring and debugging
- FreeRTOS multitasking
- CMSIS-RTOS V2 API
- Inter-task communication using message queues
- Renode-based firmware simulation
- Custom HD44780 LCD model for Renode

---

## Hardware

| Component | Description |
|---|---|
| STM32F411CEU6 | Main microcontroller |
| Rain Sensor | Detects rainfall |
| LDR Sensor | Detects ambient light level |
| LCD 16x2 | Displays system status |
| SG90 Servo | Represents the Rack IN / OUT actuator |
| Push Buttons | Mode selection and manual control |
| UART | Monitoring and debugging |

---

## Pin Mapping

| Function | STM32F411 Pin | Configuration |
|---|---|---|
| Rain Sensor | PA1 | GPIO Input |
| LDR Sensor | PA4 | GPIO Input |
| Mode Button | PB0 | GPIO Input |
| Manual IN Button | PB1 | GPIO Input |
| Manual OUT Button | PB2 | GPIO Input |
| Servo PWM | PB6 | TIM4_CH1 |
| UART TX | PA2 | USART2_TX |
| LCD RS | PB8 | GPIO Output |
| LCD E | PB9 | GPIO Output |
| LCD D4 | PB12 | GPIO Output |
| LCD D5 | PB13 | GPIO Output |
| LCD D6 | PB14 | GPIO Output |
| LCD D7 | PB15 | GPIO Output |

---

## Servo PWM Configuration

The SG90 servo is controlled using **TIM4 Channel 1**.

| Parameter | Value |
|---|---|
| Timer Clock | 16 MHz |
| Prescaler | 15 |
| Counter Frequency | 1 MHz |
| Auto Reload Register | 19999 |
| PWM Frequency | 50 Hz |
| Rack IN Pulse | 1000 us |
| Rack OUT Pulse | 1500 us |

Servo positions:

```text
Rack IN  -> CCR1 = 1000
Rack OUT -> CCR1 = 1500
```

---

## FreeRTOS Architecture

The system uses three FreeRTOS tasks and two message queues.

```text
Rain Sensor
LDR Sensor
Buttons
    │
    ▼
SensorTask
    │
    │ InputData
    ▼
sensorQueue
    │
    ▼
ControlTask
    │
    ├────► Servo Control
    │
    │ SystemState
    ▼
displayQueue
    │
    ▼
DisplayTask
    │
    ├────► LCD 16x2
    └────► UART
```

### Task Configuration

| Task | Priority | Stack Size | Function |
|---|---|---|---|
| ControlTask | AboveNormal | 256 words | Processes system logic and controls the servo |
| SensorTask | Normal | 256 words | Reads sensors and push buttons |
| DisplayTask | BelowNormal | 256 words | Updates LCD and UART output |

The project uses **FreeRTOS through the CMSIS-RTOS V2 interface**.

---

## System Logic

### AUTO Mode

| Rain | Light | Action |
|---|---|---|
| Yes | Any | Rack IN |
| No | BRIGHT | Rack OUT |
| No | DARK | Rack IN |

In AUTO mode:

```text
Rain detected
    -> Rack IN

No rain + Bright environment
    -> Rack OUT

No rain + Dark environment
    -> Rack IN
```

### MANUAL Mode

| Input | Action |
|---|---|
| Mode Button active | MANUAL mode |
| Manual IN Button | Rack IN |
| Manual OUT Button | Rack OUT |

In the current implementation, MANUAL mode remains active while the Mode Button input is active.

---

## LCD Display

The LCD 16x2 displays the operating mode, rack position, and environment state.

Example:

```text
MODE: AUTO
R:IN  E:DARK
```

When the environment becomes bright:

```text
MODE: AUTO
R:OUT E:BRIGHT
```

When rain is detected:

```text
MODE: AUTO
R:IN  E:RAIN
```

---

## UART Monitoring

USART2 is used for system monitoring and debugging.

### UART Configuration

| Parameter | Value |
|---|---|
| Baud Rate | 115200 |
| Data Bits | 8 |
| Parity | None |
| Stop Bits | 1 |
| Mode | TX / RX |

Example UART output:

```text
============================
 SMART CLOTHES DRYING RACK
 STM32F411 + FREERTOS
============================
RTOS KERNEL: RUNNING
MODE: AUTO
ENVIRONMENT: DARK
RACK: IN
----------------------------
```

---

## Simulation

Simulated using [Renode](https://renode.io/) — no physical hardware is required for firmware testing.

The Renode simulation supports:

- STM32F4 CPU and peripherals
- Rain sensor input
- LDR sensor input
- Push buttons
- TIM4 PWM servo control
- USART2 monitoring
- LCD 16x2 using a custom HD44780 model
- FreeRTOS task scheduling
- Message queue communication

---

## Renode Simulation

The project includes a custom Renode platform description and an HD44780 LCD model.

Main simulation files:

```text
HD44780Mock_F411.cs
drying_rack_f411_with_lcd.repl
```

The compiled ELF firmware is located in:

```text
Debug/smart_drying_rack_f411.elf
```

### Start the Simulation

Open Renode Monitor and run:

```text
include @HD44780Mock_F411.cs

mach create

machine LoadPlatformDescription @drying_rack_f411_with_lcd.repl

sysbus LoadELF @Debug\smart_drying_rack_f411.elf

showAnalyzer sysbus.usart2

start
```

If the files are stored in another directory, use the corresponding full path.

---

## Renode Test Commands

### Light Sensor

Set the environment to BRIGHT:

```text
gpioPortA.LightSensor Press
```

Release the light sensor:

```text
gpioPortA.LightSensor Release
```

Expected AUTO mode behavior:

```text
BRIGHT -> Rack OUT
DARK   -> Rack IN
```

### Rain Sensor

Detect rain:

```text
gpioPortA.RainSensor Press
```

Release rain sensor:

```text
gpioPortA.RainSensor Release
```

Expected behavior:

```text
RAIN -> Rack IN
```

### MANUAL Mode

Enter MANUAL mode:

```text
gpioPortB.ModeButton Press
```

Return to AUTO mode:

```text
gpioPortB.ModeButton Release
```

### Manual Rack OUT

```text
gpioPortB.ManualOutButton Press
gpioPortB.ManualOutButton Release
```

### Manual Rack IN

```text
gpioPortB.ManualInButton Press
gpioPortB.ManualInButton Release
```

### Show LCD

```text
gpioPortB.LCD16x2 Show
```

Example:

```text
+----------------+
|MODE: AUTO      |
|R:IN  E:DARK    |
+----------------+
```

### Check Servo PWM

TIM4 Channel 1 compare register can be checked using:

```text
sysbus ReadDoubleWord 0x40000834
```

Expected values:

```text
Rack IN
CCR1 = 1000
Hex  = 0x000003E8

Rack OUT
CCR1 = 1500
Hex  = 0x000005DC
```

---

## Project Structure

```text
smart_drying_rack_f411/
│
├── Core/
│   ├── Inc/
│   │   ├── app_types.h
│   │   ├── button.h
│   │   ├── lcd.h
│   │   ├── main.h
│   │   ├── sensor.h
│   │   ├── servo.h
│   │   └── uart.h
│   │
│   ├── Src/
│   │   ├── button.c
│   │   ├── freertos.c
│   │   ├── lcd.c
│   │   ├── main.c
│   │   ├── sensor.c
│   │   ├── servo.c
│   │   └── uart.c
│   │
│   └── Startup/
│
├── Drivers/
│
├── Middlewares/
│   └── Third_Party/
│       └── FreeRTOS/
│
├── Debug/
│   └── smart_drying_rack_f411.elf
│
├── HD44780Mock_F411.cs
├── drying_rack_f411_with_lcd.repl
├── smart_drying_rack_f411.ioc
└── README.md
```

---

## Software Design

The application is separated into reusable modules:

```text
sensor.c / sensor.h
    -> Rain and light sensor interface

button.c / button.h
    -> Push button interface

servo.c / servo.h
    -> Servo PWM control

lcd.c / lcd.h
    -> LCD 16x2 driver

uart.c / uart.h
    -> UART monitoring

app_types.h
    -> Shared application data types

main.c
    -> System initialization, FreeRTOS tasks and message queues
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

---

## Current Status

| Module | Status |
|---|---|
| STM32F411 Firmware | Working |
| FreeRTOS Kernel | Working |
| SensorTask | Working |
| ControlTask | Working |
| DisplayTask | Working |
| Message Queues | Working |
| Rain Sensor | Working |
| LDR Sensor | Working |
| Manual Buttons | Working |
| Servo PWM | Working |
| UART | Working |
| LCD 16x2 | Working |
| Renode Simulation | Working |

---

## Project Information

**Project:** Smart Clothes Drying Rack using STM32F411CEU6 and FreeRTOS

**Course:** Embedded Programming

**Microcontroller:** STM32F411CEU6

**RTOS:** FreeRTOS with CMSIS-RTOS V2

**Simulation:** Renode
