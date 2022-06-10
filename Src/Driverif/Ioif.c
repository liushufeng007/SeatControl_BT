#include "user_init.h"
#include "GpioCfg.h"
#include "Ioif.h"
#include "fm33lg0xx_fl_exti.h"
#include "fm33lg0xx_fl_rmu.h"


#define IOIF_CAPTURE_GPIO_n_WKEUPSRC(name)                    \
    if(FL_GPIO_IsActiveFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_##name))\
    {\
        FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_##name);\
        Ioif_Set_WakeupSrc(GPIO_EXTI_##name);\
    }
uint8_t   GPIO_EXTI_WakeupSrc[GPIO_EXTI_NUMBER_MAX];
/**
  * @brief  GPIO中断函数
  * @param  void
  * @retval void
  */
void GPIO_IRQHandler(void)
{

	IOIF_CAPTURE_GPIO_n_WKEUPSRC(0);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(1);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(2);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(3);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(4);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(5);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(6);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(7);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(8);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(9);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(10);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(11);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(12);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(13);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(14);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(15);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(16);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(17);
	IOIF_CAPTURE_GPIO_n_WKEUPSRC(18);
    FL_GPIO_ClearFlag_EXTI(GPIO, 0x7ffff);
}
void Ioif_ClearAll_WakeupSrc(void)
    {
	uint8_t fl_index_u8 = 0;
	for(fl_index_u8 = 0;fl_index_u8 <GPIO_EXTI_NUMBER_MAX;fl_index_u8 ++)
	{
		GPIO_EXTI_WakeupSrc[fl_index_u8] = FALSE;
    }
}

void Ioif_Set_WakeupSrc(uint8_t fl_index_u8)
{	
	if(fl_index_u8 < GPIO_EXTI_NUMBER_MAX)
	{
		GPIO_EXTI_WakeupSrc[fl_index_u8] = TRUE;
	}
}
	//FL_RMU_SetSoftReset(RMU);
uint8_t Ioif_Get_WakeupSrc(uint8_t fl_index_u8)
{
	uint8_t fl_wksrc_u8 = FALSE;
	if(fl_index_u8 < GPIO_EXTI_NUMBER_MAX)
	{
		fl_wksrc_u8 = GPIO_EXTI_WakeupSrc[fl_index_u8];
}

	return fl_wksrc_u8;
}


/**
  * @brief  GPIO中断唤醒初始化
  * @param  void
  * @retval void
  */
void GPIO_interrupt_Deinit(void)
{
    FL_EXTI_InitTypeDef extiInitStruct = {0};
    FL_EXTI_CommonInitTypeDef  extiCommonInitStruct = {0};
	uint8_t fl_index_u8 = 0;
	
    FL_CMU_EnableEXTIOnSleep();      /* 休眠使能外部中断采样 */
    
    /* EXTI中断采样时钟选择 */
    extiCommonInitStruct.clockSource = FL_CMU_EXTI_CLK_SOURCE_HCLK;
    FL_EXTI_CommonInit(&extiCommonInitStruct);

	for(fl_index_u8 = 0;fl_index_u8 <GPIO_EXTI_NUMBERS;fl_index_u8 ++)
	{		
		extiInitStruct.input = Gpio_Exti_Init_Cfg_Table[fl_index_u8].input;
		extiInitStruct.filter = Gpio_Exti_Init_Cfg_Table[fl_index_u8].filter;
		extiInitStruct.triggerEdge = Gpio_Exti_Init_Cfg_Table[fl_index_u8].triggerEdge;
		FL_EXTI_Init(Gpio_Exti_Init_Cfg_Table[fl_index_u8].extiLineX,&extiInitStruct);    
	}
    /* NVIC中断配置 */
    NVIC_DisableIRQ(GPIO_IRQn);
    NVIC_SetPriority(GPIO_IRQn, 2); /* 中断优先级配置 */
    NVIC_EnableIRQ(GPIO_IRQn);
}


/**
  * @brief  GPIO中断唤醒初始化
  * @param  void
  * @retval void
  */
void GPIO_interrupt_init(void)
{
    /* NVIC中断配置 */
    NVIC_DisableIRQ(GPIO_IRQn);
}



/************************************
Gpio poweron init
************************************/

void Ioif_GPIO_Init(void)
{
	uint8_t fl_index_u8 = 0;
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	
    /* NVIC中断配置 */
    NVIC_DisableIRQ(GPIO_IRQn);
	
	for(fl_index_u8 = 0;fl_index_u8 <GPIO_NUMBERS;fl_index_u8 ++)
	{
		FL_GPIO_ResetOutputPin(Gpio_Init_Cfg_Table[fl_index_u8].port,Gpio_Init_Cfg_Table[fl_index_u8].pin);
		GPIO_InitStruct.pin = Gpio_Init_Cfg_Table[fl_index_u8].pin;
		GPIO_InitStruct.mode = Gpio_Init_Cfg_Table[fl_index_u8].mode;
		GPIO_InitStruct.outputType = Gpio_Init_Cfg_Table[fl_index_u8].outputType;
		GPIO_InitStruct.remapPin = Gpio_Init_Cfg_Table[fl_index_u8].remapPin;
		GPIO_InitStruct.analogSwitch = Gpio_Init_Cfg_Table[fl_index_u8].analogSwitch;
		GPIO_InitStruct.pull = Gpio_Init_Cfg_Table[fl_index_u8].pull;
		FL_GPIO_Init(Gpio_Init_Cfg_Table[fl_index_u8].port, &GPIO_InitStruct);
	}
}
/************************************
Gpio Sleet init
************************************/

void Ioif_GPIO_DeInit(void)
{
	uint8_t fl_index_u8 = 0;
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	FL_EXTI_InitTypeDef extiInitStruct = {0};
    FL_EXTI_CommonInitTypeDef  extiCommonInitStruct = {0};
	
    FL_CMU_EnableEXTIOnSleep();      /* 休眠使能外部中断采样 */
    
    /* EXTI中断采样时钟选择 */
    extiCommonInitStruct.clockSource = FL_CMU_EXTI_CLK_SOURCE_HCLK;
    FL_EXTI_CommonInit(&extiCommonInitStruct);


	for(fl_index_u8 = 0;fl_index_u8 <GPIO_NUMBERS;fl_index_u8 ++)
	{
		FL_GPIO_ResetOutputPin(Gpio_Init_Cfg_Table[fl_index_u8].port,Gpio_Init_Cfg_Table[fl_index_u8].pin);
		GPIO_InitStruct.pin = Gpio_DeInit_Cfg_Table[fl_index_u8].pin;
		GPIO_InitStruct.mode = Gpio_DeInit_Cfg_Table[fl_index_u8].mode;
		GPIO_InitStruct.outputType = Gpio_DeInit_Cfg_Table[fl_index_u8].outputType;
		GPIO_InitStruct.remapPin = Gpio_DeInit_Cfg_Table[fl_index_u8].remapPin;
		GPIO_InitStruct.analogSwitch = Gpio_DeInit_Cfg_Table[fl_index_u8].analogSwitch;
		GPIO_InitStruct.pull = Gpio_DeInit_Cfg_Table[fl_index_u8].pull;
		FL_GPIO_Init(Gpio_Init_Cfg_Table[fl_index_u8].port, &GPIO_InitStruct);
	}


    /* 用到的GPIO引脚    设为输入 */
    GPIO_InitStruct.pin        = FL_GPIO_PIN_6;
    GPIO_InitStruct.mode       = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull       = FL_ENABLE;
    GPIO_InitStruct.remapPin   = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    extiInitStruct.input = FL_GPIO_EXTI_INPUT_GROUP2;            /* 中断线上对应的IO */
    extiInitStruct.filter = FL_ENABLE;                           /* 使能数字滤波 */
    extiInitStruct.triggerEdge = FL_GPIO_EXTI_TRIGGER_EDGE_BOTH; /* 设置触发边沿 */
    FL_EXTI_Init(FL_GPIO_EXTI_LINE_9,&extiInitStruct);  

	for(fl_index_u8 = 0;fl_index_u8 <GPIO_EXTI_NUMBERS;fl_index_u8 ++)
	{		
		extiInitStruct.input = Gpio_Exti_Init_Cfg_Table[fl_index_u8].input;
		extiInitStruct.filter = Gpio_Exti_Init_Cfg_Table[fl_index_u8].filter;
		extiInitStruct.triggerEdge = Gpio_Exti_Init_Cfg_Table[fl_index_u8].triggerEdge;
		FL_EXTI_Init(Gpio_Exti_Init_Cfg_Table[fl_index_u8].extiLineX,&extiInitStruct);    
	}
	
    /* NVIC中断配置 */
    NVIC_DisableIRQ(GPIO_IRQn);
    NVIC_SetPriority(GPIO_IRQn, 2); /* 中断优先级配置 */
    NVIC_EnableIRQ(GPIO_IRQn);
}




/************************************
return:io level
1:high
0:low
255:input parameter is error
************************************/
uint8_t Ioif_GetPinLevel(Ioif_Gpio_Num_e GpioNum_u8)
{
	uint8_t Io_level_u8 = 0xFF;
	if(GpioNum_u8 < GPIO_NUMBERS)
	{
		if(FL_GPIO_MODE_INPUT == Gpio_Init_Cfg_Table[GpioNum_u8].mode)
		{
			Io_level_u8 = FL_GPIO_GetInputPin(Gpio_Init_Cfg_Table[GpioNum_u8].port,Gpio_Init_Cfg_Table[GpioNum_u8].pin);
		}
	}

	return Io_level_u8;
}


/************************************
GpioNum_u8:  Ioif_Gpio_Num_e

GpioLevel_u8: not 0->high  0->low

return:
1:TRUE
0:FALSE  input parameter is error
************************************/
uint8_t Ioif_SetPinLevel(Ioif_Gpio_Num_e GpioNum_u8,uint8_t GpioLevel_u8)
{
	uint8_t RetVal_u8 = FALSE;
	if(GpioNum_u8 < GPIO_NUMBERS)
	{
		if(FL_GPIO_MODE_OUTPUT == Gpio_Init_Cfg_Table[GpioNum_u8].mode)
		{
			if(0 == GpioLevel_u8)
			{
				FL_GPIO_ResetOutputPin(Gpio_Init_Cfg_Table[GpioNum_u8].port,Gpio_Init_Cfg_Table[GpioNum_u8].pin);
			}
			else
			{
				FL_GPIO_SetOutputPin(Gpio_Init_Cfg_Table[GpioNum_u8].port,Gpio_Init_Cfg_Table[GpioNum_u8].pin);
			}
			RetVal_u8 = TRUE;
		}
	}
	
	return RetVal_u8;
}


