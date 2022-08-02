#include "user_init.h"
#include "fm33lg0xx_fl.h"
#include "CddMtr_Mng.h"
#include "CddMtr_HFKF.h"
#include "Adcif.h"

#ifdef CPU_LOAD_OPEN
volatile uint32_t CPU_RunTime;
#endif

/**
  * @brief  LPTIM中断函数
  * @param  void
  * @retval void
  */
void LPTIM_IRQHandler(void)
{
	uint16_t fl_ad_val;
    /* 定时器溢出时翻转LED0 */
    if(FL_LPTIM16_IsActiveFlag_Update(LPTIM16))
    {
        FL_LPTIM16_ClearFlag_Update(LPTIM16);
#ifdef CPU_LOAD_OPEN
		CPU_RunTime++;
#endif
		AdcIf_Polling();
		fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Rail_IO_HALL_AD);
		CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_0,fl_ad_val);
		fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Back_IN_IO_HALL_AD);
		CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_1,fl_ad_val);	
		fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Head_IN_IO_HALL_AD);
		CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_2,fl_ad_val);
		fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Angle_IO_HALL_AD);
		CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_3,fl_ad_val);
		fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Rotate_IN_IO_HALL_AD);
		CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_4,fl_ad_val);
		fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Leg_IN_IO_HALL_AD);
		CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_5,fl_ad_val);
    }
	/**
    if(FL_LPTIM32_IsActiveFlag_Update(LPTIM32))
    {
        FL_LPTIM32_ClearFlag_CC(LPTIM32,FL_LPTIM32_CHANNEL_1);
    }**/
}


/************************************
Comp poweron init
************************************/
void Compif_Init(void)
{
   
	   FL_LPTIM16_InitTypeDef      timInit;

    /*---------------- 定时器时间基准配置 ----------------*/
    FL_LPTIM16_StructInit(&timInit);

    timInit.clockSource          = FL_CMU_LPTIM16_CLK_SOURCE_APBCLK;
    timInit.mode                 = FL_LPTIM16_OPERATION_MODE_NORMAL;
    timInit.prescalerClockSource = FL_LPTIM16_CLK_SOURCE_INTERNAL;
    timInit.prescaler            = FL_LPTIM16_PSC_DIV64;
	
#ifdef CPU_LOAD_OPEN
    timInit.autoReload           = 50 - 1;
#else
	timInit.autoReload			 = 1000 - 1;
#endif

    timInit.encoderMode          = FL_LPTIM16_ENCODER_MODE_DISABLE;
    timInit.onePulseMode         = FL_LPTIM16_ONE_PULSE_MODE_CONTINUOUS;
    timInit.triggerEdge          = FL_LPTIM16_ETR_TRIGGER_EDGE_RISING;
    timInit.countEdge            = FL_LPTIM16_ETR_COUNT_EDGE_RISING;
    FL_LPTIM16_Init(LPTIM16, &timInit);


    /*---------------- 中断配置 ----------------*/
    /* 清除标志 */
    FL_LPTIM16_ClearFlag_Update(LPTIM16);

    /* 中断使能 */
    FL_LPTIM16_EnableIT_Update(LPTIM16);

    /* 使能并配置NVIC */
    NVIC_DisableIRQ(LPTIMx_IRQn);
    NVIC_SetPriority(LPTIMx_IRQn, 2); /* 设置中断优先级 */
    NVIC_EnableIRQ(LPTIMx_IRQn);
    /*---------------------------------------------*/

    /* 使能LPTIM16 */
    FL_LPTIM16_Enable(LPTIM16);

}
/************************************
Comp sleep init
************************************/

void Compif_DeInit(void)
{
	FL_LPTIM16_Disable(LPTIM16);			/* 使能定时器 */
	NVIC_DisableIRQ(LPTIMx_IRQn);
	FL_LPTIM16_ClearFlag_Update(LPTIM16);; /* 清除计数器中断标志位 */
}


