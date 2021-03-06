/**
  *******************************************************************************************************
  * @file    fm33lg0xx_fl_vao.c
  * @author  FMSH Application Team
  * @brief   Src file of DMA FL Module
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
#include "fm33lg0xx_fl_cdif.h"
#include "fm33lg0xx_fl_vao.h"
#include "fm33_assert.h"

/* Private macros ------------------------------------------------------------*/
/** @addtogroup VAO_FL_Private_Macros
  * @{
  */
#define         IS_FL_VAO_INSTANCE(INTANCE)                      ((INTANCE) == VAO)

#define         IS_FL_VAO_OUTPUT_DRIVE_ABILITY(__VALUE__)        (((__VALUE__) == FL_VAO_XTLF_DRIVE_LEVEL_NONE)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_DRIVE_LEVEL_1)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_DRIVE_LEVEL_2)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_DRIVE_LEVEL_3)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_DRIVE_LEVEL_4)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_DRIVE_LEVEL_5)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_DRIVE_LEVEL_6)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_DRIVE_LEVEL_7))

#define         IS_FL_VAO_WORKING_CURRENT(__VALUE__)             (((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_850NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_800NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_750NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_700NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_650NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_600NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_550NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_500NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_450NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_400NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_350NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_300NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_250NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_200NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_150NA)||\
                                                                  ((__VALUE__) == FL_VAO_XTLF_WORK_CURRENT_100NA))

#define         IS_FL_VAO_PH15_INPUT(__VALUE__)                  (((__VALUE__) == FL_DISABLE)||\
                                                                  ((__VALUE__) == FL_ENABLE))

#define         IS_FL_VAO_PH15_PULL_UP(__VALUE__)                (((__VALUE__) == FL_DISABLE)||\
                                                                  ((__VALUE__) == FL_ENABLE))


#define         IS_FL_VAO_PH15_OPENDRAIN_OUTPUT(__VALUE__)       (((__VALUE__) == FL_DISABLE)||\
                                                                  ((__VALUE__) == FL_ENABLE))

#define         IS_FL_VAO_PH15_MODE(__VALUE__)                  (((__VALUE__) == FL_VAO_PH15_MODE_INPUT)||\
                                                                 ((__VALUE__) == FL_VAO_PH15_MODE_OUTPUT)||\
                                                                 ((__VALUE__) == FL_VAO_PH15_MODE_RTCOUT))

#define         IS_FL_VAO_INPUT_THRESHOID(__VALUE__)            (((__VALUE__) == FL_VAO_PH15_THRESHOLD_NORMAL)||\
                                                                 ((__VALUE__) == FL_VAO_PH15_THRESHOLD_LOW))

/**
  * @brief  ????????????VAO?????????
  * @param  VAOx VAO Port
  * @retval ErrorStatus?????????:
  *         -FL_PASS ?????????????????????????????????
  *         -FL_FAIL ???????????????
  */
FL_ErrorStatus FL_VAO_DeInit(VAO_Type *VAOx)
{
    /* ?????????????????? */
    assert_param(IS_FL_VAO_INSTANCE(VAOx));
    /* ??????vao?????? */
    FL_VAO_EnableReset(VAOx);
    /*??????CDIF*/
    FL_CDIF_DisableVAOToCPU(CDIF);
    FL_CDIF_DisableCPUToVAO(CDIF);
    return FL_PASS;
}

/**
  * @brief  ?????? VAO_IO_StructInit ???????????????????????????????????????????????????????????????
  * @param  VAOx VAOx
  * @param  VAO_IO_StructInit ???????????? @ref FL_VAO_IO_InitTypeDef ?????????
  *         ??????????????????????????????????????????.
  * @retval ErrorStatus?????????
  *         -FL_FAIL ????????????????????????
  *         -FL_PASS ????????????
  */
FL_ErrorStatus FL_VAO_IO_Init(VAO_Type *VAOx, FL_VAO_IO_InitTypeDef *VAO_IO_InitStruct)
{
    /* ???????????? */
    assert_param(IS_FL_VAO_INSTANCE(VAOx));
    assert_param(IS_FL_VAO_PH15_INPUT(VAO_IO_InitStruct->input));
    assert_param(IS_FL_VAO_PH15_PULL_UP(VAO_IO_InitStruct->pullup));
    assert_param(IS_FL_VAO_PH15_OPENDRAIN_OUTPUT(VAO_IO_InitStruct->opendrainOutput));
    assert_param(IS_FL_VAO_PH15_MODE(VAO_IO_InitStruct->mode));
    assert_param(IS_FL_VAO_INPUT_THRESHOID(VAO_IO_InitStruct->inputThreshold));
    /*??????CDIF*/
    FL_CDIF_EnableVAOToCPU(CDIF);
    FL_CDIF_EnableCPUToVAO(CDIF);
    /*! PD15???????????? */
    FL_VAO_GPIO_SetPH15Mode(VAOx, VAO_IO_InitStruct->mode);
    /*! PD15????????????????????? */
    FL_VAO_GPIO_SetPH15Threshold(VAOx, VAO_IO_InitStruct->inputThreshold);
    /*! PD15???????????? */
    if(VAO_IO_InitStruct->input == FL_ENABLE)
    {
        FL_VAO_GPIO_EnablePH15Input(VAOx);
    }
    else
    {
        FL_VAO_GPIO_DisablePH15Input(VAOx);
    }
    /*! PD15???????????? */
    if(VAO_IO_InitStruct->pullup == FL_ENABLE)
    {
        FL_VAO_GPIO_EnablePH15Pullup(VAOx);
    }
    else
    {
        FL_VAO_GPIO_DisablePH15Pullup(VAOx);
    }
    /*! PD15?????????????????? */
    if(VAO_IO_InitStruct->opendrainOutput == FL_ENABLE)
    {
        FL_VAO_GPIO_EnablePH15OpenDrain(VAOx);
    }
    else
    {
        FL_VAO_GPIO_DisablePH15OpenDrain(VAOx);
    }
    return FL_PASS;
}

/**
  * @brief  ?????? VAO_XTLF_StructInit ???????????????????????????????????????????????????????????????
  * @param  VAOx VAOx
  * @param  VAO_XTLF_StructInit ???????????? @ref FL_VAO_XTLF_InitTypeDef ?????????
  *         ??????????????????????????????????????????.
  * @retval ErrorStatus?????????
  *         -FL_FAIL ????????????????????????
  *         -FL_PASS ????????????
  */
FL_ErrorStatus FL_VAO_XTLF_Init(VAO_Type *VAOx, FL_VAO_XTLF_InitTypeDef *VAO_XTLF_InitStruct)
{
    /* ???????????? */
    assert_param(IS_FL_VAO_INSTANCE(VAOx));
    assert_param(IS_FL_VAO_OUTPUT_DRIVE_ABILITY(VAO_XTLF_InitStruct->driveMode));
    assert_param(IS_FL_VAO_WORKING_CURRENT(VAO_XTLF_InitStruct->workingCurrentMode));
    /*??????CDIF*/
    FL_CDIF_EnableVAOToCPU(CDIF);
    FL_CDIF_EnableCPUToVAO(CDIF);
    /*! XTLF?????????????????? */
    FL_VAO_XTLF_SetWorkCurrent(VAOx, VAO_XTLF_InitStruct->workingCurrentMode);
    /*! XTLF??????????????????????????? */
    FL_VAO_XTLF_SetDriveLevel(VAOx, VAO_XTLF_InitStruct->driveMode);
    return FL_PASS;
}

/**
* @brief   ?????? VAO_IO_StructInit ???????????????
* @param   VAO_IO_StructInit ??????????????????????????????????????????????????? @ref FL_VAO_IO_InitTypeDef ?????????
*
* @retval  None
*/

void FL_VAO_IO_StructInit(FL_VAO_IO_InitTypeDef *VAO_IO_InitStruct)
{
    VAO_IO_InitStruct->mode                       = FL_VAO_PH15_MODE_INPUT;
    VAO_IO_InitStruct->input                      = FL_ENABLE;
    VAO_IO_InitStruct->pullup                     = FL_ENABLE;
    VAO_IO_InitStruct->opendrainOutput            = FL_DISABLE;
    VAO_IO_InitStruct->inputThreshold             = FL_VAO_PH15_THRESHOLD_NORMAL;
}
/**
* @brief   ?????? VAO_XTLF_StructInit ???????????????
* @param   VAO_XTLF_StructInit ??????????????????????????????????????????????????? @ref FL_VAO_XTLF_InitTypeDef ?????????
*
* @retval  None
*/

void FL_VAO_XTLF_StructInit(FL_VAO_XTLF_InitTypeDef *VAO_XTLF_InitStruct)
{
    VAO_XTLF_InitStruct->driveMode                   = FL_VAO_XTLF_DRIVE_LEVEL_1;
    VAO_XTLF_InitStruct->workingCurrentMode           = FL_VAO_XTLF_WORK_CURRENT_450NA;
}

