#ifndef LCD_H
#define LCD_H

#include "app_types.h"

void LCD_Init(void);
void LCD_Clear(void);

void LCD_Update(
    SystemMode mode,
    RackState rack,
    EnvironmentState environment
);

#endif
