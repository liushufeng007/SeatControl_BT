#ifndef __GPIO_H__
#define __GPIO_H__

#include "user_init.h"

typedef struct
{
	GPIO_Type  * port; 
    uint32_t pin;
    uint32_t mode;
    uint32_t outputType;
    uint32_t pull;
    uint32_t remapPin;
    uint32_t analogSwitch;

} FL_GPIOCfg_InitTypeDef;

typedef struct
{
    uint32_t input;
    uint32_t triggerEdge;
    uint32_t filter;
	uint32_t extiLineX;
} FL_GPIOCfg_ExtiTypeDef;



#define GPIO_EXTI_NUMBERS  ((uint8_t)13)


#define GPIO_NUMBERS  ((uint8_t)70)


extern const  FL_GPIOCfg_InitTypeDef Gpio_Init_Cfg_Table[GPIO_NUMBERS];
extern const  FL_GPIOCfg_InitTypeDef Gpio_DeInit_Cfg_Table[GPIO_NUMBERS];
extern const FL_GPIOCfg_ExtiTypeDef Gpio_Exti_Init_Cfg_Table[GPIO_EXTI_NUMBERS];

#endif
