/**
  ****************************************************************************************************
  * @file    fm33lg0xx_ll_adc.c
  * @author  FMSH Application Team
  * @brief   Src file of ADC FL Module
  ****************************************************************************************************
  * @attention
  *
  * Copyright (c) [2021] [Fudan Microelectronics]
  * THIS SOFTWARE is licensed under Mulan PSL v2.
  * You can use this software according to the terms and conditions of the Mulan PSL v2.
  * You may obtain a copy of Mulan PSL v2 at:
  *          http://license.coscl.org.cn/MulanPSL2
  * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
  * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
  * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
  * See the Mulan PSL v2 for more details.
  *
  ****************************************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "fm33lg0xx_fl_adc.h"
#include "fm33lg0xx_fl_cmu.h"
#include "fm33lg0xx_fl_rmu.h"
#include "fm33lg0xx_fl_vref.h"
#include "fm33_assert.h"

/** @addtogroup fm33lg0xx_FL_Driver
  * @{
  */

/**
  *@}
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup ADC_FL_Private_Macros
  * @{
  */

#define         IS_FL_ADC_INSTANCE(INSTANCE)                ((INSTANCE) == ADC)

#define         IS_FL_ADC_ADCCLK_SOURCE(__VALUE__)          (((__VALUE__) == FL_CMU_ADC_CLK_SOURCE_RCLF)||\
                                                            ((__VALUE__) == FL_CMU_ADC_CLK_SOURCE_RCHF)||\
                                                            ((__VALUE__) == FL_CMU_ADC_CLK_SOURCE_XTHF)||\
                                                            ((__VALUE__) == FL_CMU_ADC_CLK_SOURCE_PLL))


#define         IS_FL_ADC_ADCCLK_PRESCALER(__VALUE__)       (((__VALUE__) == FL_CMU_ADC_PSC_DIV1)||\
                                                            ((__VALUE__) == FL_CMU_ADC_PSC_DIV2)||\
                                                            ((__VALUE__) == FL_CMU_ADC_PSC_DIV4)||\
                                                            ((__VALUE__) == FL_CMU_ADC_PSC_DIV8)||\
                                                            ((__VALUE__) == FL_CMU_ADC_PSC_DIV16)||\
                                                            ((__VALUE__) == FL_CMU_ADC_PSC_DIV32))

#define         IS_FL_ADC_OPERATION_SOURCE(__VALUE__)       (((__VALUE__) == FL_ADC_CLK_SOURCE_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_CLK_SOURCE_APBCLK))

#define         IS_FL_ADC_APBCLK_PRESCALER(__VALUE__)       (((__VALUE__) == FL_ADC_APBCLK_PSC_DIV1)||\
                                                            ((__VALUE__) == FL_ADC_APBCLK_PSC_DIV2)||\
                                                            ((__VALUE__) == FL_ADC_APBCLK_PSC_DIV4)||\
                                                            ((__VALUE__) == FL_ADC_APBCLK_PSC_DIV8))

#define         IS_FL_ADC_PEFERENCE_SOURCE(__VALUE__)       (((__VALUE__) == FL_ADC_REF_SOURCE_VDDA)||\
                                                            ((__VALUE__) == FL_ADC_REF_SOURCE_VREFP))

#define         IS_FL_ADC_BITWIDTH(__VALUE__)               (((__VALUE__) == FL_ADC_BIT_WIDTH_12B)||\
                                                            ((__VALUE__) == FL_ADC_BIT_WIDTH_10B)||\
                                                            ((__VALUE__) == FL_ADC_BIT_WIDTH_8B)||\
                                                            ((__VALUE__) == FL_ADC_BIT_WIDTH_6B))

#define         IS_FL_ADC_CONTINUOUSCONVMODE(__VALUE__)     (((__VALUE__) == FL_ADC_CONV_MODE_SINGLE)||\
                                                            ((__VALUE__) == FL_ADC_CONV_MODE_CONTINUOUS))

#define         IS_FL_ADC_AUTO_MODE(__VALUE__)              (((__VALUE__) == FL_ADC_SINGLE_CONV_MODE_AUTO)||\
                                                            ((__VALUE__) == FL_ADC_SINGLE_CONV_MODE_SEMIAUTO))


#define         IS_FL_ADC_SCANDIRECTION(__VALUE__)          (((__VALUE__) == FL_ADC_SEQ_SCAN_DIR_FORWARD)||\
                                                            ((__VALUE__) == FL_ADC_SEQ_SCAN_DIR_BACKWARD))


#define         IS_FL_ADC_EXTERNALTRIGCONV(__VALUE__)       (((__VALUE__) == FL_ADC_TRIGGER_EDGE_NONE)||\
                                                            ((__VALUE__) == FL_ADC_TRIGGER_EDGE_RISING)||\
                                                            ((__VALUE__) == FL_ADC_TRIGGER_EDGE_FALLING)||\
                                                            ((__VALUE__) == FL_ADC_TRIGGER_EDGE_BOTH))

#define         IS_FL_ADC_EXTERNALTRIGSOURCE(__VALUE__)     (((__VALUE__) == FL_ADC_TRGI_LUT0)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_LUT1)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_LUT2)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_ATIM)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_GPTIM1)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_GPTIM2)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_BSTIM16)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_LPTIM12)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_COMP1)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_COMP2)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_RTCA)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_LUT3)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_GPTIM0)||\
                                                            ((__VALUE__) == FL_ADC_TRGI_COMP3))

#define         IS_FL_ADC_CHANNEL_FAST_TIME(__VALUE__)      (((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_2_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_4_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_8_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_16_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_32_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_64_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_80_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_96_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_128_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_160_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_192_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_256_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_320_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_384_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_FAST_CH_SAMPLING_TIME_512_ADCCLK))

#define         IS_FL_ADC_CHANNEL_SLOW_TIME(__VALUE__)      (((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_2_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_4_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_8_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_16_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_32_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_64_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_80_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_96_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_128_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_160_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_192_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_256_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_320_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_384_ADCCLK)||\
                                                            ((__VALUE__) == FL_ADC_SLOW_CH_SAMPLING_TIME_512_ADCCLK))

#define         IS_FL_ADC_OVERSAMPCOFIG(__VALUE__)          (((__VALUE__) == FL_DISABLE)||\
                                                            ((__VALUE__) == FL_ENABLE))

#define         IS_FL_ADC_OVERSAMPINGRATIO(__VALUE__)       (((__VALUE__) == FL_ADC_OVERSAMPLING_MUL_2X)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_MUL_4X)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_MUL_8X)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_MUL_16X)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_MUL_32X)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_MUL_64X)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_MUL_128X)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_MUL_256X))

#define         IS_FL_ADC_OVERSAMPINGSHIFT(__VALUE__)       (((__VALUE__) == FL_ADC_OVERSAMPLING_SHIFT_0B)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_SHIFT_1B)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_SHIFT_2B)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_SHIFT_3B)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_SHIFT_4B)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_SHIFT_5B)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_SHIFT_6B)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_SHIFT_7B)||\
                                                            ((__VALUE__) == FL_ADC_OVERSAMPLING_SHIFT_8B))

#define         ADC_CALIBRATIN_TIME_OUT                     (500000)
/**
  * @}
  */

/**
  * @brief  ADC??????????????????????????????
  * @param  ??????????????????
  * @retval ?????????????????????????????????
  *         -FL_PASS ?????????????????????????????????
  *         -FL_FAIL ???????????????
  */
FL_ErrorStatus FL_ADC_CommonDeInit(void)
{
    /* ?????????????????? */
    FL_CMU_DisableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_ADC);
    /* ?????????????????? */
    FL_CMU_DisableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_ADC);
    return FL_PASS;
}
/**
  * @brief  ADC????????????????????????????????????????????????
  *
  * @note   ??????FL_LPTIM_OPERATION_MODE_EXTERNAL_ASYN_PAUSE_CNT ?????????????????????????????????LPTIM?????????????????????????????????
  *         LPTIM?????????????????????????????????
  * @param  LPTIM  ??????????????????
  * @param  LPTIM_InitStruct??????FL_LPTIM_TimeInitTypeDef?????????????????????????????????LPTIM?????????????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ????????????????????????
  *         -FL_PASS ADC????????????
  */
FL_ErrorStatus FL_ADC_CommonInit(FL_ADC_CommonInitTypeDef *ADC_CommonInitStruct)
{
    FL_ErrorStatus status = FL_PASS;
    /* ?????????????????? */
    assert_param(IS_FL_ADC_OPERATION_SOURCE(ADC_CommonInitStruct->operationSource));
    assert_param(IS_FL_ADC_ADCCLK_PRESCALER(ADC_CommonInitStruct->clockPrescaler));
    assert_param(IS_FL_ADC_ADCCLK_SOURCE(ADC_CommonInitStruct->clockSource));
    assert_param(IS_FL_ADC_APBCLK_PRESCALER(ADC_CommonInitStruct->APBClockPrescaler));
    assert_param(IS_FL_ADC_PEFERENCE_SOURCE(ADC_CommonInitStruct->referenceSource));
    assert_param(IS_FL_ADC_BITWIDTH(ADC_CommonInitStruct->bitWidth));
    /* ?????????????????? */
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_ADC);
    /* ?????????????????? */
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_ADC);
    /* ??????ADCCLOCK??????????????? */
    FL_CMU_SetADCPrescaler(ADC_CommonInitStruct->clockPrescaler);
    /* ??????ADCCLOCK????????????????????? */
    FL_CMU_SetADCClockSource(ADC_CommonInitStruct->clockSource);
    /* ??????ADC????????????????????? */
    FL_ADC_SetClockSource(ADC, ADC_CommonInitStruct->operationSource);
    /* ??????APBCLOCK??????????????? */
    FL_ADC_SetAPBPrescaler(ADC, ADC_CommonInitStruct->APBClockPrescaler);
    /* ??????ADC????????????*/
    FL_ADC_SetReferenceSource(ADC, ADC_CommonInitStruct->referenceSource);
    /* ??????ADC????????????*/
    FL_ADC_SetBitWidth(ADC, ADC_CommonInitStruct->bitWidth);
    return status;
}
/**
  * @brief  ?????? ADC_CommonInitStruct ???????????????
  * @param  ADC_CommonInitStruct ??????????????????????????????????????????????????? @ref FL_ADC_CommonInitTypeDef ?????????
  *
  * @retval None
  */
void FL_ADC_CommonStructInit(FL_ADC_CommonInitTypeDef *ADC_CommonInitStruct)
{
    /*????????????RCHF??????ADC???????????????????????????????????????16*/
    ADC_CommonInitStruct->operationSource     = FL_ADC_CLK_SOURCE_ADCCLK;
    ADC_CommonInitStruct->clockSource               = FL_CMU_ADC_CLK_SOURCE_RCHF;
    ADC_CommonInitStruct->clockPrescaler        = FL_CMU_ADC_PSC_DIV16;
    ADC_CommonInitStruct->APBClockPrescaler     = FL_ADC_APBCLK_PSC_DIV1;
    ADC_CommonInitStruct->referenceSource         = FL_ADC_REF_SOURCE_VDDA;
    ADC_CommonInitStruct->bitWidth                = FL_ADC_BIT_WIDTH_12B;
}
/**
  * @brief  ???????????????ADC?????????????????????????????????
  *
  * @param  ADCx  ??????????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ????????????????????????
  *         -FL_PASS ADC????????????
  */
FL_ErrorStatus  FL_ADC_DeInit(ADC_Type *ADCx)
{
    FL_ErrorStatus status = FL_PASS;
    /* ????????????????????? */
    assert_param(IS_FL_ADC_INSTANCE(ADCx));
    /* ?????????????????? */
    FL_RMU_EnablePeripheralReset(RMU);
    FL_RMU_EnableResetAPBPeripheral(RMU, FL_RMU_RSTAPB_ADC);
    FL_RMU_DisableResetAPBPeripheral(RMU, FL_RMU_RSTAPB_ADC);
    FL_RMU_EnableResetAPBPeripheral(RMU, FL_RMU_RSTAPB_ADCCR);
    FL_RMU_DisableResetAPBPeripheral(RMU, FL_RMU_RSTAPB_ADCCR);
    FL_RMU_DisablePeripheralReset(RMU);
    return status;
}
/**
  * @brief  ?????????ADCx???????????????????????????????????????
  *
  * @note   ????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????ADC?????????????????????????????????ADC???
  *         ???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
  * @param  ADCx  ??????????????????
  * @param  ADC_InitStruct ????????? @ref FL_ADC_InitTypeDef ???????????????????????????ADC?????????????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ????????????????????????
  *         -FL_PASS ADC????????????
  */
FL_ErrorStatus FL_ADC_Init(ADC_Type *ADCx, FL_ADC_InitTypeDef  *ADC_InitStruct)
{
    FL_ErrorStatus status = FL_PASS;
    uint32_t i = 0;
    /* ????????????????????? */
    assert_param(IS_FL_ADC_INSTANCE(ADCx));
    assert_param(IS_FL_ADC_CONTINUOUSCONVMODE(ADC_InitStruct->conversionMode));
    assert_param(IS_FL_ADC_AUTO_MODE(ADC_InitStruct->autoMode));
    assert_param(IS_FL_ADC_SCANDIRECTION(ADC_InitStruct->scanDirection));
    assert_param(IS_FL_ADC_EXTERNALTRIGCONV(ADC_InitStruct->externalTrigConv));
    assert_param(IS_FL_ADC_OVERSAMPCOFIG(ADC_InitStruct->oversamplingMode));
    assert_param(IS_FL_ADC_OVERSAMPINGRATIO(ADC_InitStruct->overSampingMultiplier));
    assert_param(IS_FL_ADC_OVERSAMPINGSHIFT(ADC_InitStruct->oversamplingShift));
    FL_CMU_EnableGroup1BusClock(FL_CMU_GROUP1_BUSCLK_VREF1P2);
    if(!FL_VREF_IsEnabled(VREF))
    {
        FL_VREF_ClearFlag_Ready(VREF);
        FL_VREF_Enable(VREF);//??????VREF_EN??????????????????VREF1p2??????
    }
    FL_VREF_EnableTemperatureSensor(VREF);//??????PTAT_EN?????????
    while(FL_VREF_IsActiveFlag_Ready(VREF) == 0)  //??????VREF??????
    {
        if(i >= 128000)
        {
            break;
        }
        i++;
    }
    FL_ADC_Enable(ADCx);
    FL_ADC_EnableCalibration(ADC);
    while(FL_ADC_IsActiveFlag_EndOfCalibration(ADCx) == 0);  //??????????????????
    FL_ADC_ClearFlag_EndOfCalibration(ADC);
    /* ??????ADC????????????ADC???????????????????????? */
    FL_ADC_Disable(ADCx);
    if(FL_ADC_IsEnabled(ADCx) == 0U)
    {
        /* ?????????????????? */
        FL_ADC_SetConversionMode(ADCx, ADC_InitStruct->conversionMode);
        /* ?????????????????? */
        FL_ADC_SetSingleConversionAutoMode(ADCx, ADC_InitStruct->autoMode);
        /* ?????????????????? */
        if(ADC_InitStruct->waitMode)
        {
            FL_ADC_EnableWaitMode(ADCx);
        }
        else
        {
            FL_ADC_DisableWaitMode(ADCx);
        }
        /*????????????????????????*/
        if(ADC_InitStruct->overrunMode)
        {
            FL_ADC_EnableOverrunMode(ADCx);
        }
        else
        {
            FL_ADC_DisableOverrunMode(ADCx);
        }
        /* ????????????????????? */
        FL_ADC_SetSequenceScanDirection(ADCx, ADC_InitStruct->scanDirection);
        /* ?????????????????? */
        FL_ADC_DisableExternalConversion(ADCx);
        /* ???????????? */
        FL_ADC_SetTriggerEdge(ADCx, ADC_InitStruct->externalTrigConv);
        /* ????????? */
        FL_ADC_SetTriggerSource(ADCx, ADC_InitStruct->triggerSource);
        /*????????????????????????*/
        FL_ADC_SetFastChannelSamplingTime(ADCx, ADC_InitStruct->fastChannelTime);
        FL_ADC_SetSlowChannelSamplingTime(ADCx, ADC_InitStruct->lowChannelTime);
        if(ADC_InitStruct->oversamplingMode)
        {
            /*?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
            ?????????16?????????????????????256???????????????????????????20bit????????????4bit????????????16bit??????*/
            FL_ADC_SetOverSamplingMultiplier(ADCx, ADC_InitStruct->overSampingMultiplier);
            FL_ADC_SetOverSamplingShift(ADCx, ADC_InitStruct->oversamplingShift);
            /* ??????????????? */
            FL_ADC_EnableOverSampling(ADCx);
        }
        else
        {
            /* ??????????????? */
            FL_ADC_DisableOverSampling(ADCx);
        }
    }
    else
    {
        status = FL_FAIL;
    }
    return status;
}

/**
  * @brief  ?????? ADC_InitStruct ???????????????
  * @param  ADC_InitStruct ??????????????????????????????????????????????????? @ref FL_ADC_InitTypeDef ?????????
  *
  * @retval None
  */
void FL_ADC_StructInit(FL_ADC_InitTypeDef *ADC_InitStruct)
{
    ADC_InitStruct->conversionMode                  = FL_ADC_CONV_MODE_SINGLE;
    ADC_InitStruct->autoMode                    = FL_ADC_SINGLE_CONV_MODE_AUTO;
    ADC_InitStruct->scanDirection               = FL_ADC_SEQ_SCAN_DIR_FORWARD;
    ADC_InitStruct->externalTrigConv            = FL_ADC_TRIGGER_EDGE_NONE;
    ADC_InitStruct->overrunMode                 = FL_ENABLE;
    ADC_InitStruct->waitMode                    = FL_ENABLE;
    ADC_InitStruct->fastChannelTime             = FL_ADC_FAST_CH_SAMPLING_TIME_2_ADCCLK;
    ADC_InitStruct->lowChannelTime                  = FL_ADC_SLOW_CH_SAMPLING_TIME_512_ADCCLK;
    ADC_InitStruct->oversamplingMode            = FL_ENABLE;
    ADC_InitStruct->overSampingMultiplier   = FL_ADC_OVERSAMPLING_MUL_16X;
    ADC_InitStruct->oversamplingShift           = FL_ADC_OVERSAMPLING_SHIFT_4B;
}
/*************************************************************END OF FILE************************************************************/



