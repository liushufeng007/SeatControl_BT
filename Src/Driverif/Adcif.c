#include "user_init.h"
#include "Adcif.h"
#include "fm33lg0xx_fl.h"
#include "fm33lg0xx_fl_adc.h"
#include "Ioif.h"

static uint16_t Adcif_Buffer[ADCIF_CH_MAX_NUM];

static uint8_t test_ticks= 0;

/**
  * @brief  ADC��DMA����
  * @param  buffer: ����ָ�� 
  *         length: ���䳤��
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

    /* ADC �Ĵ������� */
    ADC_InitStruct.conversionMode                   = FL_ADC_CONV_MODE_SINGLE; /* ����ģʽ */
    ADC_InitStruct.autoMode                         = FL_ADC_SINGLE_CONV_MODE_AUTO;/* �Զ� */
    ADC_InitStruct.waitMode                         = FL_ENABLE;                   /* �ȴ� */
    ADC_InitStruct.overrunMode                      = FL_ENABLE;                   /* �����ϴ����� */
    ADC_InitStruct.scanDirection                    = FL_ADC_SEQ_SCAN_DIR_FORWARD; /* ͨ������ɨ�� */
    ADC_InitStruct.externalTrigConv                 = FL_ADC_TRIGGER_EDGE_NONE;    /* ��ֹ�����ź� */
    ADC_InitStruct.triggerSource                    = FL_ADC_TRGI_LUT0;
    ADC_InitStruct.fastChannelTime                  = FL_ADC_FAST_CH_SAMPLING_TIME_2_ADCCLK;  /* ����ͨ������ʱ�� */
    ADC_InitStruct.lowChannelTime                   = FL_ADC_SLOW_CH_SAMPLING_TIME_2_ADCCLK;/* ����ͨ������ʱ�� */
    ADC_InitStruct.oversamplingMode                 = FL_ENABLE;                   /* �������� */
    ADC_InitStruct.overSampingMultiplier            = FL_ADC_OVERSAMPLING_MUL_2X;  /* 2�������� */
    ADC_InitStruct.oversamplingShift                = FL_ADC_OVERSAMPLING_SHIFT_1B;/* ��������, /2 */

    FL_ADC_Init(ADC, &ADC_InitStruct);
	FL_ADC_EnableDMAReq(ADC);
	FL_VREF_EnableVREFBuffer(VREF);                             /* ʹ��VREF BUFFER */
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_INTERNAL_VREF1P2);/* ͨ��ѡ��VREF */
	   
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
	
	FL_ADC_ClearFlag_EndOfConversion(ADC);						 /* ���־ */
	FL_ADC_Enable(ADC); 										 /* ����ADC */
	FL_ADC_EnableSWConversion(ADC); 							 /* ��ʼת�� */

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

	/* �ȴ�ת����� */
	if(!FL_DMA_IsActiveFlag_TransferComplete(DMA, FL_DMA_CHANNEL_4))
	{
		test_ticks++;
	}      
	else
	{
	    test_ticks = 0;
		FL_DMA_ClearFlag_TransferComplete(DMA, FL_DMA_CHANNEL_4);
		ADC_DMA(Adcif_Buffer, ADCIF_CH_MAX_NUM);
		FL_ADC_ClearFlag_EndOfConversion(ADC);                      /* ���־ */
		FL_ADC_Enable(ADC);                                         /* ����ADC */	
		FL_ADC_EnableSWConversion(ADC);                             /* ��ʼת�� */
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


