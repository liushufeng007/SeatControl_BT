/**
  *******************************************************************************************************
  * @file    fm33lg0xx_fl_bstim16.c
  * @author  FMSH Application Team
  * @brief   Src file of BSTIM FL Module
  *******************************************************************************************************
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
  *******************************************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "fm33lg0xx_fl_cmu.h"
#include "fm33lg0xx_fl_rmu.h"
#include "fm33lg0xx_fl_bstim16.h"
#include "fm33_assert.h"

/** @addtogroup FM33LG0xx_FL_Driver_BSTIM
  * @{
  */

/** @addtogroup Private_Macros
  * @{
  */
#define         IS_FL_BSTIM16_INSTANCE(INTANCE)                     ((INTANCE) == BSTIM16)

#define         IS_FL_BSTIM16_PSC(__VALUE__)                        ((__VALUE__) <= 0x0000FFFF)

#define         IS_FL_BSTIM16_AUTORELOAD(__VALUE__)                 ((__VALUE__) <= 0x0000FFFF)


#define         IS_FL_BSTIM16_AUTORELOAD_MODE(__VALUE__)           (((__VALUE__) == FL_ENABLE)||\
                                                                    ((__VALUE__) == FL_DISABLE))

#define         IS_FL_BSTIM16_CLOCK_SRC(__VALUE__)                 (((__VALUE__) == FL_CMU_BSTIM16_CLK_SOURCE_APBCLK)||\
                                                                    ((__VALUE__) == FL_CMU_BSTIM16_CLK_SOURCE_RCLP)||\
                                                                    ((__VALUE__) == FL_CMU_BSTIM16_CLK_SOURCE_RCLF)||\
                                                                    ((__VALUE__) == FL_CMU_BSTIM16_CLK_SOURCE_LSCLK))

/**
  * @}
  */

/** @addtogroup BSTIM16_FL_EF_Init
  * @{
  */

/**
  * @brief  ????????????BSTIM16?????????.
  * @param  BSTIMx
  * @retval ErrorStatus?????????:
  *         -FL_PASS ?????????????????????????????????
  *         -FL_FAIL ???????????????
  */
FL_ErrorStatus FL_BSTIM16_DeInit(BSTIM16_Type *BSTIM16x)
{
    assert_param(IS_FL_BSTIM16_INSTANCE(BSTIM16x));
    /* ?????????????????? */
    FL_RMU_EnablePeripheralReset(RMU);
    /* ??????IIC??????????????? */
    FL_RMU_EnableResetAPBPeripheral(RMU, FL_RMU_RSTAPB_BSTIM16);
    FL_RMU_DisableResetAPBPeripheral(RMU, FL_RMU_RSTAPB_BSTIM16);
    /* ?????????????????????????????????????????? */
    FL_CMU_DisableGroup4BusClock(FL_CMU_GROUP4_BUSCLK_BSTIM16);
    FL_CMU_DisableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_BSTIM16);
    /* ?????????????????? */
    FL_RMU_DisablePeripheralReset(RMU);
    return FL_PASS;
}
/**
  * @brief  ?????? BSTIM16_InitStruct ???????????????????????????????????????????????????????????????.
  * @param  BSTIMx BSTIMx
  * @param  BSTIM16_InitStruct ???????????? @ref FL_BSTIM16_InitTypeDef ?????????
  *         ??????????????????????????????????????????.
  * @retval ErrorStatus?????????
  *         -FL_FAIL ????????????????????????
  *         -FL_PASS ????????????
  */
FL_ErrorStatus FL_BSTIM16_Init(BSTIM16_Type *BSTIM16x, FL_BSTIM16_InitTypeDef *init)
{
    /* ???????????? */
    assert_param(IS_FL_BSTIM16_INSTANCE(BSTIM16x));
    assert_param(IS_FL_BSTIM16_CLOCK_SRC(init->clockSource));
    assert_param(IS_FL_BSTIM16_PSC(init->prescaler));
    assert_param(IS_FL_BSTIM16_AUTORELOAD(init->autoReload));
    assert_param(IS_FL_BSTIM16_AUTORELOAD_MODE(init->autoReloadState));
    /* ???????????? */
    FL_CMU_EnableGroup4BusClock(FL_CMU_GROUP4_BUSCLK_BSTIM16);
    /* ??????????????? */
    FL_CMU_SetBSTIM16ClockSource(init->clockSource);
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_BSTIM16);
    /* ???????????? */
    FL_BSTIM16_WritePrescaler(BSTIM16x, init->prescaler);
    /* ?????????????????? */
    FL_BSTIM16_EnableUpdateEvent(BSTIM16x);
    FL_BSTIM16_WriteAutoReload(BSTIM16x, init->autoReload);
    if(init->autoReloadState == FL_ENABLE)
    {
        FL_BSTIM16_EnableARRPreload(BSTIM16x);
    }
    else
    {
        FL_BSTIM16_DisableARRPreload(BSTIM16x);
    }
    FL_BSTIM16_GenerateUpdateEvent(BSTIM16x);
    return FL_PASS;
}
/**
  * @brief  ?????? BSTIM16_InitStruct ???????????????
  * @param  BSTIM16_InitStruct ??????????????????????????????????????????????????? @ref FL_BSTIM16_InitTypeDef ?????????
  *
  * @retval None
  */

void FL_BSTIM16_StructInit(FL_BSTIM16_InitTypeDef *init)
{
    init->prescaler         = 0;
    init->autoReload        = 0xFFFFFFFF;
    init->autoReloadState   = FL_ENABLE;
    init->clockSource       = FL_CMU_BSTIM16_CLK_SOURCE_APBCLK;
}
/**
  * @}
  */

/**
  * @}
  */


/**************************Py_Code_Generator Version: 0.1-0.5-0.1 @ 2019-10-18**************************/
/*************************(C) COPYRIGHT Fudan Microelectronics **** END OF FILE*************************/
