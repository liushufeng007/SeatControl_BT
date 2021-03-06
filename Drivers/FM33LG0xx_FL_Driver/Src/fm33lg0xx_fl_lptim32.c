/**
  ****************************************************************************************************
  * @file    fm33lg0xx_ll_lptim32.c
  * @author  FMSH Application Team
  * @brief   Src file of LPTIM32 FL Module
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
#include "fm33lg0xx_fl_cmu.h"
#include "fm33lg0xx_fl_rmu.h"
#include "fm33lg0xx_fl_lptim32.h"
#include "fm33_assert.h"

/** @addtogroup FM33LG0XX_FL_Driver_LPTIM32
  * @{
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup LPTIM32_FL_Private_Macros
  * @{
  */

#define         IS_LPTIM32_INSTANCE(INSTANCE)                            ((INSTANCE) == LPTIM32)

#define         IS_FL_LPTIM32_CHANNEL(__VALUE__)                        (((__VALUE__) == FL_LPTIM32_CHANNEL_1)||\
                                                                         ((__VALUE__) == FL_LPTIM32_CHANNEL_2)||\
                                                                         ((__VALUE__) == FL_LPTIM32_CHANNEL_3)||\
                                                                         ((__VALUE__) == FL_LPTIM32_CHANNEL_4))

#define         IS_FL_LPTIM32_CMU_CLK_SOURCE(__VALUE__)                 (((__VALUE__) == FL_CMU_LPTIM32_CLK_SOURCE_RCLF)||\
                                                                         ((__VALUE__) == FL_CMU_LPTIM32_CLK_SOURCE_RCLP)||\
                                                                         ((__VALUE__) == FL_CMU_LPTIM32_CLK_SOURCE_LSCLK)||\
                                                                         ((__VALUE__) == FL_CMU_LPTIM32_CLK_SOURCE_APBCLK))

#define         IS_FL_LPTIM32_CLK_SOURCE(__VALUE__)                     (((__VALUE__) == FL_LPTIM32_CLK_SOURCE_INTERNAL)||\
                                                                         ((__VALUE__) == FL_LPTIM32_CLK_SOURCE_EXTERNAL))

#define         IS_FL_LPTIM32_PSC(__VALUE__)                            (((__VALUE__) == FL_LPTIM32_PSC_DIV1)||\
                                                                         ((__VALUE__) == FL_LPTIM32_PSC_DIV2)||\
                                                                         ((__VALUE__) == FL_LPTIM32_PSC_DIV4)||\
                                                                         ((__VALUE__) == FL_LPTIM32_PSC_DIV8)||\
                                                                         ((__VALUE__) == FL_LPTIM32_PSC_DIV16)||\
                                                                         ((__VALUE__) == FL_LPTIM32_PSC_DIV32)||\
                                                                         ((__VALUE__) == FL_LPTIM32_PSC_DIV64)||\
                                                                         ((__VALUE__) == FL_LPTIM32_PSC_DIV128))

#define         IS_FL_LPTIM32_OPERATION_MODE(__VALUE__)                 (((__VALUE__) == FL_LPTIM32_OPERATION_MODE_NORMAL)||\
                                                                         ((__VALUE__) == FL_LPTIM32_OPERATION_MODE_EXTERNAL_TRIGGER_CNT)||\
                                                                         ((__VALUE__) == FL_LPTIM32_OPERATION_MODE_EXTERNAL_ASYNC_PULSE_CNT)||\
                                                                         ((__VALUE__) == FL_LPTIM32_OPERATION_MODE_TIMEOUT))

#define         IS_FL_LPTIM32_ETR_TRIGGER_EDGE(__VALUE__)               (((__VALUE__) == FL_LPTIM32_ETR_TRIGGER_EDGE_RISING)||\
                                                                         ((__VALUE__) == FL_LPTIM32_ETR_TRIGGER_EDGE_FALLING)||\
                                                                         ((__VALUE__) == FL_LPTIM32_ETR_TRIGGER_EDGE_BOTH))

#define         IS_FL_LPTIM32_ETR_COUNT_EDGE(__VALUE__)                 (((__VALUE__) == FL_LPTIM32_ETR_COUNT_EDGE_RISING)||\
                                                                         ((__VALUE__) == FL_LPTIM32_ETR_COUNT_EDGE_FALLING))

#define         IS_FL_LPTIM32_ONE_PULSE_MODE(__VALUE__)                 (((__VALUE__) == FL_LPTIM32_ONE_PULSE_MODE_CONTINUOUS)||\
                                                                         ((__VALUE__) == FL_LPTIM32_ONE_PULSE_MODE_SINGLE))

#define         IS_FL_LPTIM32_IC_EDGE(__VALUE__)                        (((__VALUE__) == FL_LPTIM32_IC_EDGE_RISING)||\
                                                                         ((__VALUE__) == FL_LPTIM32_IC_EDGE_FALLING)||\
                                                                         ((__VALUE__) == FL_LPTIM32_IC_EDGE_BOTH))

#define         IS_FL_LPTIM32_OC_POLARITY(__VALUE__)                    (((__VALUE__) == FL_LPTIM32_OC_POLARITY_NORMAL)||\
                                                                         ((__VALUE__) == FL_LPTIM32_OC_POLARITY_INVERT))

#define         IS_FL_LPTIM32_IC1_CAPTURE_SOURCE(__VALUE__)             (((__VALUE__) == FL_LPTIM32_IC1_CAPTURE_SOURCE_CHANNEL1)||\
                                                                         ((__VALUE__) == FL_LPTIM32_IC1_CAPTURE_SOURCE_XTLF)||\
                                                                         ((__VALUE__) == FL_LPTIM32_IC1_CAPTURE_SOURCE_RCLP)||\
                                                                         ((__VALUE__) == FL_LPTIM32_IC1_CAPTURE_SOURCE_RCLF))

#define         IS_FL_LPTIM32_TRGO_SOURCE(__VALUE__)                    (((__VALUE__) == FL_LPTIM32_TRGO_ENABLE)||\
                                                                         ((__VALUE__) == FL_LPTIM32_TRGO_UPDATE)||\
                                                                         ((__VALUE__) == FL_LPTIM32_TRGO_OC1_CMP_PULSE)||\
                                                                         ((__VALUE__) == FL_LPTIM32_TRGO_IC1_EVENT)||\
                                                                         ((__VALUE__) == FL_LPTIM32_TRGO_IC2_EVENT))

/**
  * @}
  */

/** @addtogroup LPTIM32_FL_EF_Init
  * @{
  */

/**
  * @brief  ??????LPTIM32 ??????
  * @param  ??????????????????
  * @retval ?????????????????????????????????
  *         -FL_PASS ?????????????????????????????????
  *         -FL_FAIL ???????????????
  */
FL_ErrorStatus FL_LPTIM32_DeInit(LPTIM32_Type *LPTIM32x)
{
    /* ???????????? */
    assert_param(IS_LPTIM32_INSTANCE(LPTIM32x));
    /* ?????????????????? */
    FL_RMU_EnablePeripheralReset(RMU);
    /* ????????????????????? */
    FL_RMU_EnableResetAPBPeripheral(RMU, FL_RMU_RSTAPB_LPTIM32);
    FL_RMU_EnableResetAPBPeripheral(RMU, FL_RMU_RSTAPB_LPTIM32);
    /* ??????????????????????????????????????? */
    FL_CMU_DisableGroup1BusClock(FL_CMU_GROUP1_BUSCLK_LPTIM32);
    FL_CMU_DisableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_LPTIM32);
    /* ?????????????????? */
    FL_RMU_DisablePeripheralReset(RMU);
    return FL_PASS;
}

/**
  * @brief  ????????????????????????LPTIM32????????????????????????????????????????????????
  *
  * @param  LPTIM32x  ??????????????????
  * @param  init ??? @ref FL_LPTIM32_InitTypeDef??????????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ????????????????????????
  *         -FL_PASS LPTIM32????????????
  */
FL_ErrorStatus FL_LPTIM32_Init(LPTIM32_Type *LPTIM32x, FL_LPTIM32_InitTypeDef *init)
{
    /* ???????????? */
    assert_param(IS_LPTIM32_INSTANCE(LPTIM32x));
    assert_param(IS_FL_LPTIM32_CMU_CLK_SOURCE(init->clockSource));
    assert_param(IS_FL_LPTIM32_CLK_SOURCE(init->prescalerClockSource));
    assert_param(IS_FL_LPTIM32_PSC(init->prescaler));
    assert_param(IS_FL_LPTIM32_OPERATION_MODE(init->mode));
    assert_param(IS_FL_LPTIM32_ONE_PULSE_MODE(init->onePulseMode));
    assert_param(IS_FL_LPTIM32_ETR_TRIGGER_EDGE(init->triggerEdge));
    assert_param(IS_FL_LPTIM32_ETR_COUNT_EDGE(init->countEdge));
    /* ???????????? */
    if(LPTIM32x == LPTIM32)
    {
        /* ?????????????????? */
        FL_CMU_EnableGroup1BusClock(FL_CMU_GROUP1_BUSCLK_LPTIM32);
        /* ?????????????????????????????????????????????????????????????????? */
        if(init->mode != FL_LPTIM32_OPERATION_MODE_EXTERNAL_ASYNC_PULSE_CNT)
        {
            /* ?????????????????? */
            FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_LPTIM32);
            /* ??????????????????????????? */
            FL_CMU_SetLPTIM32ClockSource(init->clockSource);
        }
    }
    /* ??????????????????????????? */
    FL_LPTIM32_SetClockSource(LPTIM32x, init->prescalerClockSource);
    /* ?????????????????? */
    FL_LPTIM32_SetPrescaler(LPTIM32x, init->prescaler);
    /* ??????????????????????????? */
    FL_LPTIM32_SetOperationMode(LPTIM32x, init->mode);
    /* ???????????????????????????????????????????????? */
    switch(init->mode)
    {
        case FL_LPTIM32_OPERATION_MODE_NORMAL:
        {
        }
        break;
        case FL_LPTIM32_OPERATION_MODE_EXTERNAL_TRIGGER_CNT:
        {
            /* ?????????????????????????????? */
            FL_LPTIM32_SetETRTriggerEdge(LPTIM32x, init->triggerEdge);
        }
        break;
        case FL_LPTIM32_OPERATION_MODE_EXTERNAL_ASYNC_PULSE_CNT:
        {
            /* ???????????????????????? */
            FL_LPTIM32_SetETRCountEdge(LPTIM32x, init->countEdge);
            /* ???????????????????????? */
            FL_LPTIM32_EnableETRFilter(LPTIM32x);
        }
        break;
        case FL_LPTIM32_OPERATION_MODE_TIMEOUT:
        {
            /* ?????????????????????????????? */
            FL_LPTIM32_SetETRTriggerEdge(LPTIM32x, init->triggerEdge);
        }
        break;
    }
    /* ?????????????????? */
    FL_LPTIM32_SetOnePulseMode(LPTIM32x, init->onePulseMode);
    /* ?????????????????? */
    FL_LPTIM32_WriteAutoReload(LPTIM32x, init->autoReload);
    return FL_PASS;
}

/**
  * @brief  ?????? LPTIM32_InitStruct ???????????????
  * @param  init ??? @ref FL_LPTIM32_InitTypeDef??????????????????
  *
  * @retval None
  */
void FL_LPTIM32_StructInit(FL_LPTIM32_InitTypeDef *init)
{
    init->clockSource           = FL_CMU_LPTIM32_CLK_SOURCE_APBCLK;
    init->prescalerClockSource  = FL_LPTIM32_CLK_SOURCE_INTERNAL;
    init->prescaler             = FL_LPTIM32_PSC_DIV1;
    init->autoReload            = 0;
    init->mode                  = FL_LPTIM32_OPERATION_MODE_NORMAL;
    init->countEdge             = FL_LPTIM32_ETR_COUNT_EDGE_RISING;
    init->triggerEdge           = FL_LPTIM32_ETR_TRIGGER_EDGE_RISING;
    init->onePulseMode          = FL_LPTIM32_ONE_PULSE_MODE_CONTINUOUS;
}

/**
  * @brief  ??????LPTIM32???????????????????????????
  *
  * @param  LPTIM32x  ??????????????????
  * @param  ic_init ??? @ref FL_LPTIM32_IC_InitTypeDef??????????????????
  * @param  Channel LPTIM32????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ????????????????????????
  *         -FL_PASS LPTIM32????????????
  */
//??????????????????
FL_ErrorStatus FL_LPTIM32_IC_Init(LPTIM32_Type *LPTIM32x, uint32_t Channel, FL_LPTIM32_IC_InitTypeDef *ic_init)
{
    FL_ErrorStatus result = FL_PASS;
    /* ???????????? */
    assert_param(IS_LPTIM32_INSTANCE(LPTIM32x));
    assert_param(IS_FL_LPTIM32_CHANNEL(Channel));
    assert_param(IS_FL_LPTIM32_IC_EDGE(ic_init->ICEdge));
    assert_param(IS_FL_LPTIM32_IC1_CAPTURE_SOURCE(ic_init->ICSource));
    /* ??????1????????? */
    if(Channel == FL_LPTIM32_CHANNEL_1)
    {
        FL_LPTIM32_IC_SetChannel1CaptureSource(LPTIM32, ic_init->ICSource);
    }
    /* ?????????????????? */
    FL_LPTIM32_IC_SetCaptureEdge(LPTIM32, ic_init->ICEdge, Channel);
    /* ???????????????????????? */
    FL_LPTIM32_SetChannelMode(LPTIM32, FL_LPTIM32_CHANNEL_MODE_INPUT, Channel);
    return result;
}

/**
  * @brief  ?????? LPTIM32_IC_InitStruct ???????????????
  * @param  ic_init??? @ref FL_LPTIM32_IC_InitTypeDef??????????????????
  *
  * @retval None
  */
void FL_LPTIM32_IC_StructInit(FL_LPTIM32_IC_InitTypeDef *ic_init)
{
    ic_init->ICEdge      = FL_LPTIM32_IC_EDGE_RISING;
    ic_init->ICSource    = FL_LPTIM32_IC1_CAPTURE_SOURCE_CHANNEL1;
}

/**
  * @brief  ????????????????????????LPTIM32????????????????????????????????????
  *
  * @param  LPTIM32x  ??????????????????
  * @param  oc_init ??? @ref FL_LPTIM32_OC_InitTypeDef??????????????????
  * @param  Channel LPTIM32????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ????????????????????????
  *         -FL_PASS LPTIM32????????????
  */
FL_ErrorStatus FL_LPTIM32_OC_Init(LPTIM32_Type *LPTIM32x, uint32_t Channel, FL_LPTIM32_OC_InitTypeDef *oc_init)
{
    FL_ErrorStatus result = FL_PASS;
    /* ???????????? */
    assert_param(IS_LPTIM32_INSTANCE(LPTIM32x));
    assert_param(IS_FL_LPTIM32_CHANNEL(Channel));
    assert_param(IS_FL_LPTIM32_OC_POLARITY(oc_init->OCPolarity));
    /* ?????????????????? */
    FL_LPTIM32_OC_SetPolarity(LPTIM32x, oc_init->OCPolarity, Channel);
    /* ??????????????? */
    switch(Channel)
    {
        case FL_LPTIM32_CHANNEL_1:
            FL_LPTIM32_WriteCompareCH1(LPTIM32x, oc_init->compareValue);
            break;
        case FL_LPTIM32_CHANNEL_2:
            FL_LPTIM32_WriteCompareCH2(LPTIM32x, oc_init->compareValue);
            break;
        case FL_LPTIM32_CHANNEL_3:
            FL_LPTIM32_WriteCompareCH3(LPTIM32x, oc_init->compareValue);
            break;
        case FL_LPTIM32_CHANNEL_4:
            FL_LPTIM32_WriteCompareCH4(LPTIM32x, oc_init->compareValue);
            break;
        default :
            result = FL_FAIL;
            break;
    }
    /* ???????????????????????? */
    FL_LPTIM32_SetChannelMode(LPTIM32x, FL_LPTIM32_CHANNEL_MODE_OUTPUT, Channel);
    return result;
}

/**
  * @brief  ?????? LPTIM32_OC_InitStruct ???????????????
  * @param  oc_init??? @ref FL_LPTIM32_OC_InitTypeDef??????????????????
  *
  * @retval None
  */
void FL_LPTIM32_OC_StructInit(FL_LPTIM32_OC_InitTypeDef *oc_init)
{
    oc_init->compareValue = 0;
    oc_init->OCPolarity   = FL_LPTIM32_OC_POLARITY_NORMAL;
}

/**
  * @}
  */

/**
  * @}
  */
/*************************************************************END OF FILE************************************************************/
