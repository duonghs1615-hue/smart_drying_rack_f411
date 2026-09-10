# Smart Clothes Drying Rack using STM32F411CEU6 and FreeRTOS

A smart clothes drying rack system developed using **STM32F411CEU6** and **FreeRTOS**.

The system automatically extends or retracts the clothes rack based on rainfall and ambient light conditions. It also supports manual control using push buttons.

The firmware can be simulated using **Renode** without requiring physical hardware.

---

## Features

- Rain detection
- Ambient light detection
- Environment states:
  - RAIN
  - BRIGHT
  - DARK
- AUTO operating mode
- MANUAL operating mode
- SG90 servo control for Rack IN / OUT
- LCD 16x2 status display
- UART monitoring and debugging
- FreeRTOS multitasking
- Inter-task communication using message queues
- Renode-based firmware simulation
- Custom HD44780 LCD model for Renode

---

## Hardware

| Component | Description |
|---|---|
| STM32F411CEU6 | Main microcontroller |
| Rain Sensor | Detects rainfall |
| LDR Sensor | Detects ambient light |
| LCD 16x2 | Displays system state |
| SG90 Servo | Represents Rack IN / OUT actuator |
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

Therefore:

```text
Rack IN  -> CCR1 = 1000
Rack OUT -> CCR1 = 1500
