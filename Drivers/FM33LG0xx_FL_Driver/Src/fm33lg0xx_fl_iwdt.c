/**
  ****************************************************************************************************
  * @file    fm33lg0xx_fl_iwdt.c
  * @author  FMSH Application Team
  * @brief   Src file of IWDT FL Module
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
#include "fm33lg0xx_fl_iwdt.h"
#include "fm33_assert.h"
/** @addtogroup FM33LG0xx_FL_Driver_IWDT
  * @{
  */

/** @addtogroup Private_Macros
  * @{
  */
#define         IS_IWDT_INSTANCE(INTANCE)                    ((INTANCE) == IWDT)

#define         IS_FL_IWDT_WINDOWSVEL(__VALUE__)                ((__VALUE__) < 0xFFF)

#define         IS_FL_IWDT_OVERFLOWPERIOD(__VALUE__)            (((__VALUE__) == FL_IWDT_PERIOD_125MS)||\
                                                                 ((__VALUE__) == FL_IWDT_PERIOD_250MS)||\
                                                                 ((__VALUE__) == FL_IWDT_PERIOD_500MS)||\
                                                                 ((__VALUE__) == FL_IWDT_PERIOD_1000MS)||\
                                                                 ((__VALUE__) == FL_IWDT_PERIOD_2000MS)||\
                                                                 ((__VALUE__) == FL_IWDT_PERIOD_4000MS)||\
                                                                 ((__VALUE__) == FL_IWDT_PERIOD_8000MS)||\
                                                                 ((__VALUE__) == FL_IWDT_PERIOD_16000MS))
/**
  * @}
  */

/** @addtogroup IWDT_FL_EF_Init
  * @{
  */

/**
  * @brief  ??????IWDT??????
  *
  * @note   ???????????????????????????????????????????????????IWDT????????????????????????
  *
  * @param  IWDTx ??????????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_PASS ????????????
  *         -FL_FAIL ????????????????????????
  */
FL_ErrorStatus FL_IWDT_DeInit(IWDT_Type *IWDTx)
{
    assert_param(IS_IWDT_INSTANCE(IWDTx));
    return FL_PASS;
}
/**
  * @brief  ?????? IWDT_InitStruct ????????????????????????????????????.
  *
  * @note   IWTD??????????????????????????????????????????????????????
  *
  * @param  IWDTx  ??????????????????
  * @param  IWDT_InitStruct ??? @ref FL_IWDT_InitTypeDef???????????????????????????IWDT?????????????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_PASS ????????????
  *         -FL_FAIL ????????????????????????
  *         -FL_PASS ????????????
  */
FL_ErrorStatus FL_IWDT_Init(IWDT_Type *IWDTx, FL_IWDT_InitTypeDef *IWDT_InitStruct)
{
    FL_ErrorStatus status = FL_PASS;
    /* ?????????????????? */
    assert_param(IS_IWDT_INSTANCE(IWDTx));
    assert_param(IS_FL_IWDT_WINDOWSVEL(IWDT_InitStruct->iwdtWindows));
    assert_param(IS_FL_IWDT_OVERFLOWPERIOD(IWDT_InitStruct->overflowPeriod));
    /* ?????????????????? */
    FL_CMU_EnableGroup1BusClock(FL_CMU_GROUP1_BUSCLK_IWDT);
    /* ????????????????????????????????? */
    FL_IWDT_SetPeriod(IWDTx, IWDT_InitStruct->overflowPeriod);
    /* ?????????????????????????????????*/
    FL_IWDT_WriteWindow(IWDTx, IWDT_InitStruct->iwdtWindows);
    /* ??????????????? */
    FL_IWDT_ReloadCounter(IWDTx);
    return status;
}
/**
  * @brief  ?????? IWDT_InitStruct ???????????????
  *
  * @param  IWDT_InitStruct ??????????????????????????????????????????????????? @ref LL_IWDT_InitTypeDef ?????????
  *
  * @retval None
  */
void FL_IWDT_StructInit(FL_IWDT_InitTypeDef *IWDT_InitStruct)
{
    /* ????????????????????? */
    IWDT_InitStruct->iwdtWindows    = 0;
    /*??????????????????*/
    IWDT_InitStruct->overflowPeriod = FL_IWDT_PERIOD_500MS;
}
/**
  *@}
  */

/**
  *@}
  */
/*************************************************************END OF FILE************************************************************/
