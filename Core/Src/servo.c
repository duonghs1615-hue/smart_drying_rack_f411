#include "servo.h"
#include "main.h"

#define SERVO_RACK_IN   1000U
#define SERVO_RACK_OUT  1500U

extern TIM_HandleTypeDef htim4;

void Servo_Init(void)
{
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    Servo_SetIn();
}

void Servo_SetIn(void)
{
    __HAL_TIM_SET_COMPARE(
        &htim4,
        TIM_CHANNEL_1,
        SERVO_RACK_IN
    );
}

void Servo_SetOut(void)
{
    __HAL_TIM_SET_COMPARE(
        &htim4,
        TIM_CHANNEL_1,
        SERVO_RACK_OUT
    );
}
