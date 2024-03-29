#include "user_init.h"
#include "GpioCfg.h"

/*
typedef struct
{
	GPIO_Type  * port; 
    uint32_t pin;
    uint32_t mode;
    uint32_t outputType;
    uint32_t pull;
    uint32_t remapPin;
    uint32_t analogSwitch;

} FL_GPIO_InitTypeDef;
*/


const FL_GPIOCfg_InitTypeDef Gpio_Init_Cfg_Table[GPIO_NUMBERS] = 
{
/*PA0~PA15*/
    GPIOA  ,  FL_GPIO_PIN_0 , FL_GPIO_MODE_ANALOG   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_1 , FL_GPIO_MODE_ANALOG   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_2 , FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_3 , FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_4 , FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_5 , FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_6 , FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_7 , FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_8 , FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_9 , FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_10, FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_11, FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_12, FL_GPIO_MODE_INPUT    , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_13, FL_GPIO_MODE_ANALOG   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_14, FL_GPIO_MODE_ANALOG   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_15, FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 

/*PB0~PB15*/
	GPIOB  ,  FL_GPIO_PIN_0 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_1 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_2 , FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_ENABLE   , FL_ENABLE  , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_3 , FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_ENABLE   , FL_DISABLE  , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_4 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_5 , FL_GPIO_MODE_OUTPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_6 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_7 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_8 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_9 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_10, FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_11, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_12, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_13, FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_ENABLE   , FL_ENABLE  , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_14, FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_ENABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_15, FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_ENABLE   , FL_ENABLE  , FL_DISABLE ,  

/*PC0~PC15*/
	GPIOC  ,  FL_GPIO_PIN_0 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_1 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_2 , FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_3 , FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_4 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_5 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_6 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_7 , FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_8 , FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_9 , FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_10, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_11, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_12, FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_13, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_14, FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_15, FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_ENABLE  ,  FL_ENABLE , FL_DISABLE , 

/*PD0~PD12*/
	GPIOD  ,  FL_GPIO_PIN_0 , FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_1 , FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_2 , FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_3 , FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_4 , FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_5 , FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_6 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_7 , FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_8 , FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_9 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_10, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_11, FL_GPIO_MODE_ANALOG	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_12, FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_ENABLE   , FL_ENABLE  , FL_DISABLE , 
	
/*PE0~PE9*/
	GPIOE  ,  FL_GPIO_PIN_0 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_1 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_2 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_3 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_5 , FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_ENABLE  , FL_ENABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_6 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_7 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_8 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_9 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE 

};



/*
A12 
D9  
E8
B0

B7
B11
B12

C13
C0
C6
C11

D5
E0
*/

const FL_GPIOCfg_ExtiTypeDef Gpio_Exti_Init_Cfg_Table[GPIO_EXTI_NUMBERS] = 
{
	FL_GPIO_EXTI_INPUT_GROUP0  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_3,//PA12
	FL_GPIO_EXTI_INPUT_GROUP1  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_14,//PD9
	FL_GPIO_EXTI_INPUT_GROUP0  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_18,//PE8
	FL_GPIO_EXTI_INPUT_GROUP0  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_4,//PB0
	
	FL_GPIO_EXTI_INPUT_GROUP3  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_5,//PB7
	FL_GPIO_EXTI_INPUT_GROUP3  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_6,//PB11
	FL_GPIO_EXTI_INPUT_GROUP0  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_7,//PB12

	
	FL_GPIO_EXTI_INPUT_GROUP3  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_11,//PC15
	FL_GPIO_EXTI_INPUT_GROUP0  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_8,//PC0
	FL_GPIO_EXTI_INPUT_GROUP2  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_9,//PC6
	FL_GPIO_EXTI_INPUT_GROUP3  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_10,//PC11
	
	FL_GPIO_EXTI_INPUT_GROUP1  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_13,//PD5
	FL_GPIO_EXTI_INPUT_GROUP0  ,  FL_GPIO_EXTI_TRIGGER_EDGE_BOTH   ,FL_ENABLE  , FL_GPIO_EXTI_LINE_16,//PE0
};



const FL_GPIOCfg_InitTypeDef Gpio_DeInit_Cfg_Table[GPIO_NUMBERS] = 
{
/*PA0~PA15*/
    GPIOA  ,  FL_GPIO_PIN_0 , FL_GPIO_MODE_INPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_1 , FL_GPIO_MODE_INPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_2 , FL_GPIO_MODE_INPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_3 , FL_GPIO_MODE_OUTPUT  , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_4 , FL_GPIO_MODE_OUTPUT  , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_5 , FL_GPIO_MODE_OUTPUT  , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_6 , FL_GPIO_MODE_OUTPUT  , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_7 , FL_GPIO_MODE_OUTPUT  , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_8 , FL_GPIO_MODE_INPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_9 , FL_GPIO_MODE_INPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_10, FL_GPIO_MODE_INPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_11, FL_GPIO_MODE_INPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_12, FL_GPIO_MODE_INPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_13, FL_GPIO_MODE_INPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_14, FL_GPIO_MODE_INPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOA  ,  FL_GPIO_PIN_15, FL_GPIO_MODE_INPUT   , FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 

/*PB0~PB15*/
	GPIOB  ,  FL_GPIO_PIN_0 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_1 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_2 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_3 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_4 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_5 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_6 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_7 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_8 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_9 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_10, FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_11, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_12, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_13, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_14, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOB  ,  FL_GPIO_PIN_15, FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 

/*PC0~PC15*/
	GPIOC  ,  FL_GPIO_PIN_0 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_1 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_2 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_3 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_4 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_5 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_6 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_7 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_8 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_9 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_10, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_11, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_12, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_13, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_14, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOC  ,  FL_GPIO_PIN_15, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 

/*PD0~PD12*/
	GPIOD  ,  FL_GPIO_PIN_0 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_1 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_2 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_3 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_4 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_5 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_6 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_7 , FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_8 , FL_GPIO_MODE_DIGITAL	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_9 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_10, FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_11, FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOD  ,  FL_GPIO_PIN_12, FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 

/*PE0~PE9*/
	GPIOE  ,  FL_GPIO_PIN_0 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_1 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_2 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_3 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_5 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_6 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_7 , FL_GPIO_MODE_OUTPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_8 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE   , FL_DISABLE , FL_DISABLE , 
	GPIOE  ,  FL_GPIO_PIN_9 , FL_GPIO_MODE_INPUT	, FL_GPIO_OUTPUT_PUSHPULL  , FL_DISABLE  , FL_DISABLE , FL_DISABLE 

};


