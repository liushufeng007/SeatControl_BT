/**
  ****************************************************************************************************
  * @file    fm33lg0xx_fl_vrefp.c
  * @author  FMSH Application Team
  * @brief   Src file of VREFP FL Module
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
#include "fm33lg0xx_fl_vrefp.h"
#include "fm33_assert.h"
/** @addtogroup FM33LG0xx_FL_Driver_VREFP
  * @{
  */


/** @addtogroup Private_Macros
  * @{
  */
#define         IS_VREFP_INSTANCE(INTANCE)                       ((INTANCE) == VREFP)

#define         IS_FL_VREFP_VOLTAGETRIM(__VALUE__)               (((__VALUE__) == FL_VREFP_OUTPUT_VOLTAGE_2P0V_TRIM)||\
                                                                 ((__VALUE__) == FL_VREFP_OUTPUT_VOLTAGE_2P5V_TRIM)||\
                                                                 ((__VALUE__) == FL_VREFP_OUTPUT_VOLTAGE_3P0V_TRIM)||\
                                                                 ((__VALUE__) == FL_VREFP_OUTPUT_VOLTAGE_4P5V_TRIM)||\
                                                                 ((__VALUE__) == FL_VREFP_OUTPUT_VOLTAGE_1P5V_TRIM))

#define         IS_FL_VREFP_OUTPUTVOLTAGE(__VALUE__)             (((__VALUE__) == FL_VREFP_OUTPUT_VOLTAGE_2P0V)||\
                                                                 ((__VALUE__) == FL_VREFP_OUTPUT_VOLTAGE_2P5V)||\
                                                                 ((__VALUE__) == FL_VREFP_OUTPUT_VOLTAGE_3P0V)||\
                                                                 ((__VALUE__) == FL_VREFP_OUTPUT_VOLTAGE_4P5V)||\
                                                                 ((__VALUE__) == FL_VREFP_OUTPUT_VOLTAGE_1P5V))

#define         IS_FL_VREFP_ENABLEPERIOD(__VALUE__)              (((__VALUE__) == FL_VREFP_ENABLE_PERIOD_1MS)||\
                                                                 ((__VALUE__) == FL_VREFP_ENABLE_PERIOD_4MS)||\
                                                                 ((__VALUE__) == FL_VREFP_ENABLE_PERIOD_16MS)||\
                                                                 ((__VALUE__) == FL_VREFP_ENABLE_PERIOD_32MS)||\
                                                                 ((__VALUE__) == FL_VREFP_ENABLE_PERIOD_64MS)||\
                                                                 ((__VALUE__) == FL_VREFP_ENABLE_PERIOD_256MS)||\
                                                                 ((__VALUE__) == FL_VREFP_ENABLE_PERIOD_1000MS)||\
                                                                 ((__VALUE__) == FL_VREFP_ENABLE_PERIOD_4000MS))

#define         IS_FL_VREFP_DRIVINGTIME(__VALUE__)               (((__VALUE__) == FL_VREFP_DRIVING_TIME_4LSCLK)||\
                                                                 ((__VALUE__) == FL_VREFP_DRIVING_TIME_8LSCLK)||\
                                                                 ((__VALUE__) == FL_VREFP_DRIVING_TIME_16LSCLK)||\
                                                                 ((__VALUE__) == FL_VREFP_DRIVING_TIME_32LSCLK)||\
                                                                 ((__VALUE__) == FL_VREFP_DRIVING_TIME_64LSCLK)||\
                                                                 ((__VALUE__) == FL_VREFP_DRIVING_TIME_128LSCLK)||\
                                                                 ((__VALUE__) == FL_VREFP_DRIVING_TIME_256LSCLK)||\
                                                                 ((__VALUE__) == FL_VREFP_DRIVING_TIME_512LSCLK))

#define         IS_FL_VREFP_WORKMODE(__VALUE__)                  (((__VALUE__) == FL_VREFP_WORK_MODE_CONTINUOUS)||\
                                                                 ((__VALUE__) == FL_VREFP_WORK_MODE_PERIODIC))

/**
  * @}
  */

/** @addtogroup VREFP_FL_EF_Init
  * @{
  */

/**
  * @brief  ??????VREFP??????????????????
  *
  * @param  VREFPx ??????????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_PASS ????????????
  *         -FL_FAIL ????????????????????????
  */
FL_ErrorStatus FL_VREFP_DeInit(VREFP_Type *VREFPx)
{
    assert_param(IS_VREFP_INSTANCE(VREFPx));
    return FL_PASS;
}
/**
  * @brief  ?????? VREFP_InitStruct????????????????????????????????????????????????.
  *
  * @note   WWTD??????????????????????????????????????????????????????
  *
  * @param  VREFPx ??????????????????
  *
  * @param  VREFP_InitStruct ?????? @ref FL_VREFP_InitTypeDef ??????????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_PASS ????????????
  *         -FL_FAIL ????????????????????????
  */
FL_ErrorStatus FL_VREFP_Init(VREFP_Type *VREFPx, FL_VREFP_InitTypeDef *VREFP_InitStruct)
{
    FL_ErrorStatus status = FL_PASS;
    /* ?????????????????? */
    assert_param(IS_VREFP_INSTANCE(VREFPx));
    assert_param(IS_FL_VREFP_VOLTAGETRIM(VREFP_InitStruct->voltageTrim));
    assert_param(IS_FL_VREFP_OUTPUTVOLTAGE(VREFP_InitStruct->outputVoltage));
    assert_param(IS_FL_VREFP_ENABLEPERIOD(VREFP_InitStruct->timeOfPeriod));
    assert_param(IS_FL_VREFP_DRIVINGTIME(VREFP_InitStruct->timeOfDriving));
    assert_param(IS_FL_VREFP_WORKMODE(VREFP_InitStruct->mode));
    /* ?????????????????? */
    FL_CMU_EnableGroup1BusClock(FL_CMU_GROUP1_BUSCLK_PMU);
    /* ??????VREFP????????????TRIM??? */
    FL_VREFP_WriteOutputVoltageTrim(VREFPx, VREFP_InitStruct->voltageTrim);
    /* ??????VREFP???????????? */
    FL_VREFP_SetOutputVoltage(VREFPx, VREFP_InitStruct->outputVoltage);
    /* ?????????????????? */
    FL_VREFP_SetWorkMode(VREFPx, VREFP_InitStruct->mode);
    /* ??????????????????????????? */
    FL_VREFP_SetEnablePeriod(VREFPx, VREFP_InitStruct->timeOfPeriod);
    /* ????????????????????????????????? */
    FL_VREFP_SetDrivingTime(VREFPx, VREFP_InitStruct->timeOfDriving);
    /* ??????VREFP */
    FL_VREFP_Enable(VREFPx);
    return status;
}
/**
  * @brief  ?????? VREFP_InitStruct ???????????????
  *
  * @param  VREFP_InitStruct ??????????????????????????????????????????????????? @ref FL_VREFP_InitTypeDef ?????????
  *
  * @retval None
  */
void FL_VREFP_StructInit(FL_VREFP_InitTypeDef *VREFP_InitStruct)
{
    VREFP_InitStruct->voltageTrim = FL_VREFP_OUTPUT_VOLTAGE_3P0V_TRIM;
    VREFP_InitStruct->outputVoltage = FL_VREFP_OUTPUT_VOLTAGE_3P0V;
    VREFP_InitStruct->mode = FL_VREFP_WORK_MODE_CONTINUOUS;
    VREFP_InitStruct->timeOfPeriod = FL_VREFP_ENABLE_PERIOD_1MS;
    VREFP_InitStruct->timeOfDriving = FL_VREFP_DRIVING_TIME_4LSCLK;
}
/**
  *@}
  */

/**
  *@}
  */
/*************************************************************END OF FILE************************************************************/

