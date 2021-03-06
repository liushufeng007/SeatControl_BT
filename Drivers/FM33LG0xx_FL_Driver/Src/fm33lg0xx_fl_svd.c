/**
  ****************************************************************************************************
  * @file    fm33lC0xx_fl_svd.c
  * @author  FMSH Application Team
  * @brief   Src file of SVD FL Module
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
#include "fm33lg0xx_fl_svd.h"
#include "fm33_assert.h"

/** @addtogroup FM33LG0XX_FL_Driver_SVD
  * @{
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup SVD_FL_Private_Macros
  * @{
  */

#define         IS_SVD_INSTANCE(INSTANCE)                           (((INSTANCE) == SVD))

#define         IS_FL_SVD_REFERENCE_VOLTAGE(__VALUE__)              (((__VALUE__) == FL_SVD_REFERENCE_1P0V)||\
                                                                     ((__VALUE__) == FL_SVD_REFERENCE_0P95V)||\
                                                                     ((__VALUE__) == FL_SVD_REFERENCE_0P9V))

#define         IS_FL_SVD_WARNING_THRESHOLD_LEVEL(__VALUE__)        (((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP0)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP1)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP2)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP3)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP4)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP5)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP6)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP7)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP8)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP9)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP10)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP11)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP12)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP13)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP14)||\
                                                                     ((__VALUE__) == FL_SVD_WARNING_THRESHOLD_GROUP15))

#define         IS_FL_SVD_WORK_MODE(__VALUE__)                      (((__VALUE__) == FL_SVD_WORK_MODE_CONTINUOUS)||\
                                                                     ((__VALUE__) == FL_SVD_WORK_MODE_PERIODIC))

#define         IS_FL_SVD_ENABLE_PERIOD(__VALUE__)                  (((__VALUE__) == FL_SVD_ENABLE_PERIOD_62P5MS)||\
                                                                     ((__VALUE__) == FL_SVD_ENABLE_PERIOD_256MS)||\
                                                                     ((__VALUE__) == FL_SVD_ENABLE_PERIOD_1000MS)||\
                                                                     ((__VALUE__) == FL_SVD_ENABLE_PERIOD_4000MS))

/**
  * @}
  */

/** @addtogroup SVD_FL_EF_Init
  * @{
  */

/**
  * @brief  ??????SVD??????
  * @param  ??????????????????
  * @retval ?????????????????????????????????
  *         -FL_PASS ?????????????????????????????????
  *         -FL_FAIL ???????????????
  */
FL_ErrorStatus FL_SVD_DeInit(SVD_Type *SVDx)
{
    /* ???????????? */
    assert_param(IS_SVD_INSTANCE(SVDx));
    /* ???????????? */
    FL_RMU_EnablePeripheralReset(RMU);
    /* ????????????????????? */
    FL_RMU_EnableResetAPBPeripheral(RMU, FL_RMU_RSTAPB_SVD);
    FL_RMU_DisableResetAPBPeripheral(RMU, FL_RMU_RSTAPB_SVD);
    /* ???????????????????????? */
    FL_CMU_DisableGroup1BusClock(FL_CMU_GROUP1_BUSCLK_SVD);
    /* ???????????? */
    FL_RMU_DisablePeripheralReset(RMU);
    return FL_PASS;
}

/**
  * @brief  ????????????????????????SVD?????????
  *
  * @param  SVDx  ??????????????????
  * @param  init @ref FL_SVD_InitTypeDef??????????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ????????????????????????
  *         -FL_PASS SVD????????????
  */
FL_ErrorStatus FL_SVD_Init(SVD_Type *SVDx, FL_SVD_InitTypeDef *init)
{
    /* ???????????? */
    assert_param(IS_SVD_INSTANCE(SVDx));
    assert_param(IS_FL_SVD_REFERENCE_VOLTAGE(init->referenceVoltage));
    assert_param(IS_FL_SVD_WARNING_THRESHOLD_LEVEL(init->warningThreshold));
    assert_param(IS_FL_SVD_WORK_MODE(init->workMode));
    assert_param(IS_FL_SVD_ENABLE_PERIOD(init->enablePeriod));
    /* ??????SVD?????? */
    if(SVDx == SVD)
    {
        FL_CMU_EnableGroup1BusClock(FL_CMU_GROUP1_BUSCLK_SVD);
    }
    else
    {
        return FL_FAIL;
    }
    /* ?????????????????? */
    FL_SVD_EnableReference(SVDx, init->referenceVoltage);
    /* ?????????????????? */
    FL_SVD_SetWarningThreshold(SVDx, init->warningThreshold);
    /* ???????????? */
    if(init->digitalFilter == FL_ENABLE)
    {
        FL_SVD_EnableDigitalFilter(SVDx);
    }
    else
    {
        FL_SVD_DisableDigitalFilter(SVDx);
    }
    /* ???????????? */
    FL_SVD_SetWorkMode(SVDx, init->workMode);
    if(init->workMode == FL_SVD_WORK_MODE_PERIODIC)
    {
        /* ?????????????????? */
        FL_SVD_SetEnablePeriod(SVDx, init->enablePeriod);
    }
    /* SVS?????? */
    if(init->SVSChannel == FL_ENABLE)
    {
        FL_SVD_EnableSVSChannel(SVDx);
    }
    else
    {
        FL_SVD_DisableSVSChannel(SVDx);
    }
    return FL_PASS;
}

/**
  * @brief  ?????? SVD_InitStruct ???????????????
  * @param  init @ref FL_SVD_InitTypeDef??????????????????
  *
  * @retval None
  */
void FL_SVD_StructInit(FL_SVD_InitTypeDef *init)
{
    init->referenceVoltage = FL_SVD_REFERENCE_1P0V;
    init->SVSChannel = FL_DISABLE;
    init->digitalFilter = FL_DISABLE;
    init->workMode = FL_SVD_WORK_MODE_CONTINUOUS;
    init->enablePeriod = FL_SVD_ENABLE_PERIOD_62P5MS;
    init->warningThreshold = FL_SVD_WARNING_THRESHOLD_GROUP0;
}

/**
  * @}
  */

/**
  * @}
  */
/*************************************************************END OF FILE************************************************************/
