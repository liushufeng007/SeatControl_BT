#include "user_init.h"
#include "Adcif.h"
#include "fm33lg0xx_fl.h"
#include "fm33lg0xx_fl_adc.h"
#include "Ioif.h"

static uint16_t Adcif_Buffer[ADCIF_CH_MAX_NUM];

static uint8_t test_ticks= 0;

/**
  * @brief  ADC的DMA配置
  * @param  buffer: 数据指针 
  *         length: 传输长度
  * @retval void
  */
void ADC_DMA(uint16_t *buffer, uint32_t length)
{
    FL_DMA_InitTypeDef DMA_InitStruct = {0};
    FL_DMA_ConfigTypeDef DMA_ConfigStruct = {0};
    DMA_InitStruct.periphAddress = FL_DMA_PERIPHERAL_FUNCTION1;
    DMA_InitStruct.direction = FL_DMA_DIR_PERIPHERAL_TO_RAM;
    DMA_InitStruct.memoryAddressIncMode = FL_DMA_MEMORY_INC_MODE_INCREASE;
    DMA_InitStruct.dataSize = FL_DMA_BANDWIDTH_16B;
    DMA_InitStruct.priority = FL_DMA_PRIORITY_HIGH;
    DMA_InitStruct.circMode = FL_DISABLE;
    FL_DMA_Init(DMA, &DMA_InitStruct, FL_DMA_CHANNEL_4);

    FL_DMA_Enable(DMA);

    DMA_ConfigStruct.memoryAddress = (uint32_t)buffer;
    DMA_ConfigStruct.transmissionCount = length - 1;
    FL_DMA_StartTransmission(DMA, &DMA_ConfigStruct, FL_DMA_CHANNEL_4);

}


/************************************
Adc poweron init
************************************/

void Adcif_Init(void)
{
    FL_ADC_CommonInitTypeDef    ADC_CommonInitStruct;
    FL_ADC_InitTypeDef          ADC_InitStruct ;

	Ioif_SetPinLevel(GPIO_NUMBER_26_VBAT_SW_EN,TRUE);
	Ioif_SetPinLevel(GPIO_NUMBER_68_VDD_EN,TRUE);

    FL_ADC_CommonStructInit(&ADC_CommonInitStruct);
    FL_ADC_CommonInit(&ADC_CommonInitStruct);

    /* ADC 寄存器设置 */
    ADC_InitStruct.conversionMode                   = FL_ADC_CONV_MODE_SINGLE; /* 单次模式 */
    ADC_InitStruct.autoMode                         = FL_ADC_SINGLE_CONV_MODE_AUTO;/* 自动 */
    ADC_InitStruct.waitMode                         = FL_ENABLE;                   /* 等待 */
    ADC_InitStruct.overrunMode                      = FL_ENABLE;                   /* 覆盖上次数据 */
    ADC_InitStruct.scanDirection                    = FL_ADC_SEQ_SCAN_DIR_FORWARD; /* 通道正序扫描 */
    ADC_InitStruct.externalTrigConv                 = FL_ADC_TRIGGER_EDGE_NONE;    /* 禁止触发信号 */
    ADC_InitStruct.triggerSource                    = FL_ADC_TRGI_LUT0;
    ADC_InitStruct.fastChannelTime                  = FL_ADC_FAST_CH_SAMPLING_TIME_2_ADCCLK;  /* 快速通道采样时间 */
    ADC_InitStruct.lowChannelTime                   = FL_ADC_SLOW_CH_SAMPLING_TIME_2_ADCCLK;/* 慢速通道采样时间 */
    ADC_InitStruct.oversamplingMode                 = FL_ENABLE;                   /* 过采样打开 */
    ADC_InitStruct.overSampingMultiplier            = FL_ADC_OVERSAMPLING_MUL_2X;  /* 2倍过采样 */
    ADC_InitStruct.oversamplingShift                = FL_ADC_OVERSAMPLING_SHIFT_1B;/* 数据右移, /2 */

    FL_ADC_Init(ADC, &ADC_InitStruct);
	FL_ADC_EnableDMAReq(ADC);
	FL_VREF_EnableVREFBuffer(VREF);                             /* 使能VREF BUFFER */
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_INTERNAL_VREF1P2);/* 通道选择VREF */
	   
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH0);   
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH1);   
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH2);   
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH4);   
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH5);  
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH6);   
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH7);  
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH8);   
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH11);  
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH12);
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH13);
	
	ADC_DMA(Adcif_Buffer, ADCIF_CH_MAX_NUM);
	
	FL_ADC_ClearFlag_EndOfConversion(ADC);						 /* 清标志 */
	FL_ADC_Enable(ADC); 										 /* 启动ADC */
	FL_ADC_EnableSWConversion(ADC); 							 /* 开始转换 */

}
/************************************
Adc Sleep init
************************************/

void Adcif_DeInit(void)
{

    FL_ADC_Disable(ADC);       
	FL_VREF_DisableVREFBuffer(VREF); 
	FL_ADC_DisableSequencerChannel(ADC, FL_ADC_INTERNAL_VREF1P2);
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH18);    
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH19);   
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH14);   
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH11);   
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH13);   
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH4);   

}

/************************************
Polling all channel value 
************************************/
void AdcIf_Polling(void)
{

	/* 等待转换完成 */
	if(!FL_DMA_IsActiveFlag_TransferComplete(DMA, FL_DMA_CHANNEL_4))
	{
		test_ticks++;
	}      
	else
	{
	    test_ticks = 0;
		FL_DMA_ClearFlag_TransferComplete(DMA, FL_DMA_CHANNEL_4);
		ADC_DMA(Adcif_Buffer, ADCIF_CH_MAX_NUM);
		FL_ADC_ClearFlag_EndOfConversion(ADC);                      /* 清标志 */
		FL_ADC_Enable(ADC);                                         /* 启动ADC */	
		FL_ADC_EnableSWConversion(ADC);                             /* 开始转换 */
	}

}


/************************************
Get Adc value 
adc value: 0 ~ 4095
************************************/
uint16_t Adcif_Get_AdcVal(uint8_t Adc_ch_u8)
{

	if( Adc_ch_u8 < ADCIF_CH_MAX_NUM)
	{
		return Adcif_Buffer[Adc_ch_u8];
	}
	
	return 0;
}


