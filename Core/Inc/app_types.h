#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdint.h>

typedef enum
{
    RACK_IN = 0,
    RACK_OUT
} RackState;

typedef enum
{
    MODE_AUTO = 0,
    MODE_MANUAL
} SystemMode;

typedef enum
{
    ENV_RAIN = 0,
    ENV_BRIGHT,
    ENV_DARK
} EnvironmentState;

typedef struct
{
    uint8_t rain;
    uint8_t light;
    uint8_t mode_button;
    uint8_t manual_in;
    uint8_t manual_out;
    EnvironmentState environment;
} InputData;

typedef struct
{
    SystemMode mode;
    RackState rack;
    EnvironmentState environment;
} SystemState;

#endif
