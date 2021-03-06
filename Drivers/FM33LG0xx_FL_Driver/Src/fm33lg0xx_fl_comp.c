/**
  ****************************************************************************************************
  * @file    fm33lg0xx_fl_comp.c
  * @author  FMSH Application Team
  * @brief   Src file of COMP FL Module
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
#include "fm33lg0xx_fl_comp.h"
#include "fm33lg0xx_fl_rmu.h"
#include "fm33lg0xx_fl_cmu.h"
#include "fm33_assert.h"


/** @addtogroup FM33LG0xx_FL_Driver_COMP
  * @{
  */





/* Private macros ------------------------------------------------------------*/
/** @addtogroup COMP_FL_Private_Macros
  * @{
  */
#define         IS_COMP_ALL_INSTANCE(INTENCE)              (((INTENCE) == COMP1)||\
                                                            ((INTENCE) == COMP2)||\
                                                            ((INTENCE) == COMP3))

#define         IS_FL_COMP_POSITIVEINPUT(__VALUE__)        (((__VALUE__) == FL_COMP_INP_SOURCE_INP1)||\
                                                            ((__VALUE__) == FL_COMP_INP_SOURCE_INP2)||\
                                                            ((__VALUE__) == FL_COMP_INP_SOURCE_AVREF)||\
                                                            ((__VALUE__) == FL_COMP_INP_SOURCE_ULPBG_AREF)||\
                                                            ((__VALUE__) == FL_COMP_INP_SOURCE_VDD15)||\
                                                            ((__VALUE__) == FL_COMP_INP_SOURCE_VREF_VREG))

#define         IS_FL_COMP_NEGATIVEINPUT(__VALUE__)        (((__VALUE__) == FL_COMP_INN_SOURCE_INN1)||\
                                                             ((__VALUE__) == FL_COMP_INN_SOURCE_INN2)||\
                                                             ((__VALUE__) == FL_COMP_INN_SOURCE_VREF)||\
                                                             ((__VALUE__) == FL_COMP_INN_SOURCE_VREF_DIV_2)||\
                                                             ((__VALUE__) == FL_COMP_INN_SOURCE_VREF_BUF)||\
                                                             ((__VALUE__) == FL_COMP_INN_SOURCE_DAC))

#define         IS_FL_COMP_POLARITY(__VALUE__)             (((__VALUE__) == FL_COMP_OUTPUT_POLARITY_NORMAL)||\
                                                             ((__VALUE__) == FL_COMP_OUTPUT_POLARITY_INVERT))

#define         IS_FL_COMP_EDGE(__VALUE__)                 (((__VALUE__) == FL_COMP_INTERRUPT_EDGE_BOTH)||\
                                                            ((__VALUE__) == FL_COMP_INTERRUPT_EDGE_RISING )||\
                                                            ((__VALUE__) == FL_COMP_INTERRUPT_EDGE_FALLING))

#define         IS_FL_COMP_DIGITAL_FILTER(__VALUE__)       (((__VALUE__) == FL_DISABLE)||\
                                                            ((__VALUE__) == FL_ENABLE))

#define         IS_FL_COMP_DIGITAL_FILTER_LEN(__VALUE__)    (((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_3APBCLK)||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_4APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_5APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_6APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_7APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_8APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_9APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_10APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_11APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_12APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_13APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_14APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_15APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_16APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_17APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_18APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_19APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_20APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_21APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_22APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_23APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_24APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_25APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_26APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_27APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_28APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_29APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_30APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_31APBCLK )||\
                                                            ((__VALUE__) == FL_COMP_OUTPUT_FILTER_WINDOW_32APBCLK))
/**
  * @}
  */

/** @addtogroup COMP_FL_EF_Init
  * @{
  */

/**
  * @brief  ????????????COMP???????????????.
  * @param  COMPx COMP Port
  * @retval ErrorStatus?????????:
  *           -FL_PASS ?????????????????????????????????
  *           -FL_FAIL ???????????????
  */
FL_ErrorStatus FL_COMP_DeInit(COMP_Type *COMPx)
{
    /* ?????????????????? */
    assert_param(IS_COMP_ALL_INSTANCE(COMPx));
    /* ?????????????????????????????? */
    COMPx->CR = 0x00000000U;
    return FL_PASS;
}

/**
  * @brief  ?????? COMP_InitStruct????????????????????????????????????.
  * @param  COMPx COMP Port
  * @param  initStruct ???????????? @ref FL_COMP_InitTypeDef ?????????
  *         ??????????????????????????????????????????.
  * @param  Serial ???????????????????????????
  *         1 ???????????????1
  *         2 ???????????????2
  * @retval ErrorStatus?????????
  *            -FL_FAIL ????????????????????????
  *            -FL_PASS COMP????????????
  */
FL_ErrorStatus FL_COMP_Init(COMP_Type *COMPx, FL_COMP_InitTypeDef *initStruct)
{
    /* ?????????????????? */
    assert_param(IS_COMP_ALL_INSTANCE(COMPx));
    assert_param(IS_FL_COMP_EDGE(initStruct->edge));
    assert_param(IS_FL_COMP_POLARITY(initStruct->polarity));
    assert_param(IS_FL_COMP_POSITIVEINPUT(initStruct->positiveInput));
    assert_param(IS_FL_COMP_NEGATIVEINPUT(initStruct->negativeInput));
    assert_param(IS_FL_COMP_DIGITAL_FILTER(initStruct->digitalFilter));
    assert_param(IS_FL_COMP_DIGITAL_FILTER_LEN(initStruct->digitalFilterLen));
    /* ?????????????????? */
    FL_CMU_EnableGroup1BusClock(FL_CMU_GROUP1_BUSCLK_COMP);
    /* ??????????????????????????? */
    FL_COMP_SetOutputPolarity(COMPx, initStruct->polarity);
    /* ??????????????????????????? */
    FL_COMP_SetINPSource(COMPx, initStruct->positiveInput);
    /* ??????????????????????????? */
    FL_COMP_SetINNSource(COMPx, initStruct->negativeInput);
    /* ????????????????????? */
    if(COMPx == COMP1)
    {
        /* ??????????????????????????? */
        FL_COMP_SetComparator1InterruptEdge(COMP, initStruct->edge);
    }
    else
        if(COMPx == COMP2)
        {
            /* ??????????????????????????? */
            FL_COMP_SetComparator2InterruptEdge(COMP, initStruct->edge);
        }
        else
        {
            /* ??????????????????????????? */
            FL_COMP_SetComparator3InterruptEdge(COMP, initStruct->edge);
        }
    /* ?????? */
    if(initStruct->digitalFilter)
    {
        FL_COMP_EnableOutputFilter(COMPx);
    }
    else
    {
        FL_COMP_DisableOutputFilter(COMPx);
    }
    /* ???????????? */
    FL_COMP_SetOutputFilterWindow(COMPx, initStruct->digitalFilterLen);
    return FL_PASS;
}
/**
  * @brief    ?????? initStruct ???????????????
  * @param     initStruct ??????????????????????????????????????????????????? @ref FL_COMP_InitTypeDef ?????????
  *
  * @retval    None
  */
void FL_COMP_StructInit(FL_COMP_InitTypeDef *initStruct)
{
    /* ?????????????????? */
    initStruct->edge             = FL_COMP_INTERRUPT_EDGE_BOTH;
    initStruct->polarity         = FL_COMP_OUTPUT_POLARITY_NORMAL;
    initStruct->negativeInput    = FL_COMP_INN_SOURCE_INN1;
    initStruct->positiveInput    = FL_COMP_INP_SOURCE_INP1;
    initStruct->digitalFilter    = FL_ENABLE;
    initStruct->digitalFilterLen = FL_COMP_OUTPUT_FILTER_WINDOW_3APBCLK;
}


/**
  * @}
  */

/**
  * @}
  */
/*************************************************************END OF FILE************************************************************/
