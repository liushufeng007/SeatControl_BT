/**
  ****************************************************************************************************
  * @file    fm33lg0xx_fl_flash.c
  * @author  FMSH Application Team
  * @brief   Src file of FLASH FL Module
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
#include "fm33lg0xx_fl_flash.h"
#include "fm33lg0xx_fl_cmu.h"
#include "fm33lg0xx_fl_dma.h"
#include "fm33_assert.h"

/** @addtogroup FM33LG0XX_FL_Driver_FLASH
  * @{
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup GPIO_FL_Private_Macros
  * @{
  */

#define         IS_FLASH_ALL_INSTANCE(INTENCE)              (((INTENCE) == FLASH))

#define         IS_FL_FLASH_PAGE_NUM(__VALUE__)              ((((uint32_t)0x00000000U) < (__VALUE__)) &&\
                                                             ((__VALUE__) <= (FL_FLASH_MAX_PAGE_NUM)))

#define         IS_FL_FLASH_SECTOR_NUM(__VALUE__)              ((((uint32_t)0x00000000U) < (__VALUE__)) &&\
                                                             ((__VALUE__) <= (FL_FLASH_MAX_SECTOR_NUM)))

#define         IS_FL_FLASH_MAX_ADDR(__VALUE__)              ((((uint32_t)0x00000000U) < (__VALUE__)) &&\
                                                             ((__VALUE__) <= (FL_FLASH_ADDR_MAXPROGRAM)))

#define         IS_FL_FLASH_MAX_PAGE(__VALUE__)              ((((uint32_t)0x00000000U) < (__VALUE__)) &&\
                                                             ((__VALUE__) <= (FL_FLASH_MAX_PAGE_NUM)))

#define         IS_FL_FLASH_MAX_SECTOR(__VALUE__)           ((((uint32_t)0x00000000U) < (__VALUE__)) &&\
                                                             ((__VALUE__) <= (FL_FLASH_MAX_SECTOR_NUM)))
/**
  * @}
  */

/** @addtogroup FLASH_FL_OPERATE_API
  * @{
  */

/**
  * @brief  Flash ??????????????????????????????512byte.
  * @param  FLASHx FLASH Port
  * @param  address ???????????????????????????????????????????????????????????????????????????????????????
  *         .
  * @retval ErrorStatus?????????
  *         -FL_FAIL ??????????????????
  *         -FL_PASS ????????????
  */
FL_ErrorStatus FL_FLASH_PageErase(FLASH_Type *FLASHx, uint32_t address)
{
    uint32_t timeout = 0;
    uint32_t primask;
    FL_ErrorStatus ret = FL_PASS;
    /* ?????????????????? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_ADDR((uint32_t)address));
    /*????????????*/
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    if(address & (FL_FLASH_ADDRS_ALIGN - 1))
    {
        /*???????????????*/
        return FL_FAIL;
    }
    if(FL_FLASH_GetFlashLockStatus(FLASHx) == FL_FLASH_KEY_STATUS_ERROR)
    {
        /*Flash ????????????????????????????????????*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    /*??????????????????*/
    FL_FLASH_SetFlashEraseType(FLASHx, FL_FLASH_ERASE_TYPE_PAGE);
    /* ???????????????*/
    FL_FLASH_EnableErase(FLASHx);
    /* Key ??????*/
    primask = __get_PRIMASK();
    __disable_irq();
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_ERASE_KEY);
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_PGAE_ERASE_KEY);
    __set_PRIMASK(primask);
    FL_FLASH_ClearFlag_EraseComplete(FLASHx);
    /* ????????? */
    *((uint32_t *)address) =  FL_FLASH_ERASE_REQUEST;
    while(1)
    {
        timeout++;
        if((timeout > FL_FLASH_ERASE_TIMEOUT)\
                || (FL_FLASH_IsActiveFlag_ClockError(FLASHx))\
                || (FL_FLASH_IsActiveFlag_KeyError(FLASHx))\
                || (FL_FLASH_IsActiveFlag_AuthenticationError(FLASHx)))
        {
            /* ????????????????????? */
            ret = FL_FAIL;
            break;
        }
        else
            if(FL_FLASH_IsActiveFlag_EraseComplete(FLASHx))
            {
                /*????????????*/
                FL_FLASH_ClearFlag_EraseComplete(FLASHx);
                ret =  FL_PASS;
                break;
            }
    }
    FL_FLASH_LockFlash(FLASHx);
    FL_CMU_DisableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_CMU_DisableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    return ret;
}

/**
  * @brief  Flash ????????????????????????????????????2k byte.
  * @param  FLASHx FLASH Port
  * @param  address ?????????????????????????????????????????????????????????????????????????????????????????????
  *         .
  * @retval ErrorStatus?????????
  *         -FL_FAIL ??????????????????
  *         -FL_PASS ????????????
  */
FL_ErrorStatus FL_FLASH_SectorErase(FLASH_Type *FLASHx, uint32_t address)
{
    uint32_t timeout = 0;
    uint32_t primask;
    FL_ErrorStatus ret = FL_PASS;
    /* ?????????????????? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_ADDR((uint32_t)address));
    /*????????????*/
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    if(address & (FL_FLASH_ADDRS_ALIGN - 1))
    {
        /*???????????????*/
        return FL_FAIL;
    }
    if(FL_FLASH_GetFlashLockStatus(FLASHx) == FL_FLASH_KEY_STATUS_ERROR)
    {
        /*Flash ????????????????????????????????????*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    /*??????????????????*/
    FL_FLASH_SetFlashEraseType(FLASHx, FL_FLASH_ERASE_TYPE_SECTOR);
    /* ??????????????????*/
    FL_FLASH_EnableErase(FLASHx);
    /* Key ??????*/
    primask = __get_PRIMASK();
    __disable_irq();
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_ERASE_KEY);
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_SECTOR_ERASE_KEY);
    __set_PRIMASK(primask);
    FL_FLASH_ClearFlag_EraseComplete(FLASHx);
    /* ????????? */
    *((uint32_t *)address) =  FL_FLASH_ERASE_REQUEST;
    while(1)
    {
        timeout++;
        if((timeout > FL_FLASH_ERASE_TIMEOUT)\
                || (FL_FLASH_IsActiveFlag_ClockError(FLASHx))\
                || (FL_FLASH_IsActiveFlag_KeyError(FLASHx))\
                || (FL_FLASH_IsActiveFlag_AuthenticationError(FLASHx)))
        {
            /* ????????????????????? */
            ret = FL_FAIL;
            break;
        }
        else
            if(FL_FLASH_IsActiveFlag_EraseComplete(FLASHx))
            {
                /*????????????*/
                FL_FLASH_ClearFlag_EraseComplete(FLASHx);
                ret =  FL_PASS;
                break;
            }
    }
    FL_FLASH_LockFlash(FLASHx);
    FL_CMU_DisableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_CMU_DisableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    return ret;
}

/**
  * @brief  ?????????????????????????????????????????????????????????.
  * @param  FLASHx FLASH Port
  * @param  address ??????????????????????????????????????????????????????????????????????????????????????????fault???
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ??????????????????
  *         -FL_PASS ????????????
  */
FL_ErrorStatus FL_FLASH_Program_Word(FLASH_Type *FLASHx, uint32_t address, uint32_t data)
{
    uint32_t timeout = 0;
    uint32_t primask;
    FL_ErrorStatus ret = FL_PASS;
    /* ?????????????????? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_ADDR((uint32_t)address));
    /*????????????*/
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    if(address & (FL_FLASH_ADDRS_ALIGN - 1))
    {
        /*???????????????*/
        return FL_FAIL;
    }
    if(FL_FLASH_GetFlashLockStatus(FLASHx) == FL_FLASH_KEY_STATUS_ERROR)
    {
        /*Flash ????????????????????????????????????*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    /* ????????????*/
    FL_FLASH_EnableProgram(FLASHx);
    /* Key ??????*/
    primask = __get_PRIMASK();
    __disable_irq();
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_PROGRAM_KEY1);
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_PROGRAM_KEY2);
    __set_PRIMASK(primask);
    *((uint32_t *)address) =  data;
    while(1)
    {
        timeout++;
        if((timeout > FL_FLASH_ERASE_TIMEOUT)\
                || (FL_FLASH_IsActiveFlag_ClockError(FLASHx))\
                || (FL_FLASH_IsActiveFlag_KeyError(FLASHx))\
                || (FL_FLASH_IsActiveFlag_AuthenticationError(FLASHx)))
        {
            /* ????????????????????? */
            ret = FL_FAIL;
            break;
        }
        else
            if(FL_FLASH_IsActiveFlag_ProgramComplete(FLASHx))
            {
                /*????????????*/
                FL_FLASH_ClearFlag_ProgramComplete(FLASHx);
                ret = FL_PASS;
                break;
            }
    }
    FL_FLASH_LockFlash(FLASHx);
    FL_CMU_DisableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_CMU_DisableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    return ret;
}

/**
  * @brief  ??????????????????????????????????????????????????????.
  * @param  FLASHx FLASH Port
  * @param  PageNum ????????????????????????????????????????????????FM33LC04?????????256?????????????????????????????????fault???
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ??????????????????
  *         -FL_PASS ????????????
  */
FL_ErrorStatus FL_FLASH_Program_Page(FLASH_Type *FLASHx, uint32_t pageNum, uint32_t *data)
{
    uint32_t count;
    uint32_t primask;
    uint32_t address;
    uint32_t timeout;
    FL_ErrorStatus ret;
    /* ?????????????????? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_PAGE((uint32_t)pageNum));
    address = pageNum * FL_FLASH_PGAE_SIZE_BYTE;
    /* ?????????*/
    if(address & (FL_FLASH_PGAE_SIZE_BYTE - 1))
    {
        /*???????????????*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_FLASH_EnableProgram(FLASHx);
    /* Key ??????*/
    primask = __get_PRIMASK();
    __disable_irq();
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_PROGRAM_KEY1);
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_PROGRAM_KEY2);
    __set_PRIMASK(primask);
    for(count = 0; count < FL_FLASH_PGAE_SIZE_BYTE; count += 4)
    {
        timeout = 0;
        FL_FLASH_EnableProgram(FLASHx);
        *((uint32_t *)address) =  *data;
        address += 4;
        data++;
        while(1)
        {
            timeout++;
            if((timeout > FL_FLASH_ERASE_TIMEOUT)\
                    || (FL_FLASH_IsActiveFlag_ClockError(FLASHx))\
                    || (FL_FLASH_IsActiveFlag_KeyError(FLASHx))\
                    || (FL_FLASH_IsActiveFlag_AuthenticationError(FLASHx)))
            {
                /* ????????????????????? */
                ret = FL_FAIL;
                break;
            }
            if(FL_FLASH_IsActiveFlag_ProgramComplete(FLASHx))
            {
                /*????????????*/
                FL_FLASH_ClearFlag_ProgramComplete(FLASHx);
                ret = FL_PASS;
                break;
            }
        }
        if(ret == FL_FAIL)
        {
            break;
        }
    }
    FL_FLASH_LockFlash(FLASHx);
    FL_CMU_DisableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_CMU_DisableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    return ret;
}

/**
  * @brief  ?????????????????????????????????????????????????????????.
  * @param  FLASHx FLASH Port
  * @param  sectorNum ?????????????????????????????????????????????????????????128?????????????????????????????????fault???
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ??????????????????
  *         -FL_PASS ????????????
  */
FL_ErrorStatus FL_FLASH_Program_Sector(FLASH_Type *FLASHx, uint32_t sectorNum, uint32_t *data)
{
    uint32_t count;
    uint32_t primask;
    uint32_t address;
    uint32_t timeout;
    FL_ErrorStatus ret;
    /* ?????????????????? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_SECTOR((uint32_t)sectorNum));
    address = sectorNum * FL_FLASH_SECTOR_SIZE_BYTE;
    /* ?????????*/
    if(address & (FL_FLASH_SECTOR_SIZE_BYTE - 1))
    {
        /*???????????????*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_FLASH_EnableProgram(FLASHx);
    /* Key ??????*/
    primask = __get_PRIMASK();
    __disable_irq();
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_PROGRAM_KEY1);
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_PROGRAM_KEY2);
    __set_PRIMASK(primask);
    for(count = 0; count < FL_FLASH_SECTOR_SIZE_BYTE; count += 4)
    {
        timeout = 0;
        FL_FLASH_EnableProgram(FLASHx);
        *((uint32_t *)address) =  *data;
        address += 4;
        data++;
        while(1)
        {
            timeout++;
            if((timeout > FL_FLASH_ERASE_TIMEOUT)\
                    || (FL_FLASH_IsActiveFlag_ClockError(FLASHx))\
                    || (FL_FLASH_IsActiveFlag_KeyError(FLASHx))\
                    || (FL_FLASH_IsActiveFlag_AuthenticationError(FLASHx)))
            {
                /* ????????????????????? */
                ret = FL_FAIL;
                break;
            }
            if(FL_FLASH_IsActiveFlag_ProgramComplete(FLASHx))
            {
                /*????????????*/
                FL_FLASH_ClearFlag_ProgramComplete(FLASHx);
                ret = FL_PASS;
                break;
            }
        }
        if(ret == FL_FAIL)
        {
            break;
        }
    }
    FL_FLASH_LockFlash(FLASHx);
    FL_CMU_DisableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_CMU_DisableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    return ret;
}

/**
  * @brief  DMA??????????????????????????????????????????halt-page??????????????????64???.
  * @param  FLASHx FLASH Port
  * @param  address ?????????Flash??????
  * @param  *data ?????????Flash??????
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ??????????????????
  *         -FL_PASS ????????????
  */
FL_ErrorStatus FL_FLASH_Write_Dma(FLASH_Type *FLASHx, uint32_t address, uint32_t *data)
{
    FL_ErrorStatus ret;
    uint32_t primask;
    uint32_t timeout;
    FL_DMA_InitTypeDef DMA_InitStruct = {0};
    /* ?????????????????? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_ADDR(address));
    /* ????????????*/
    if(address & (FL_FLASH_PGAE_SIZE_BYTE / 2 - 1))
    {
        /*???????????????*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_FLASH_EnableProgram(FLASHx);
    /* Key ??????*/
    primask = __get_PRIMASK();
    __disable_irq();
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_PROGRAM_KEY1);
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_PROGRAM_KEY2);
    __set_PRIMASK(primask);
    FL_FLASH_EnableProgram(FLASHx);
    DMA_InitStruct.circMode = FL_DISABLE;
    DMA_InitStruct.direction = FL_DMA_DIR_RAM_TO_FLASH;
    DMA_InitStruct.memoryAddressIncMode = FL_DMA_CH7_RAM_INC_MODE_INCREASE;
    DMA_InitStruct.flashAddressIncMode = FL_DMA_CH7_FLASH_INC_MODE_INCREASE;
    DMA_InitStruct.priority = FL_DMA_PRIORITY_HIGH;
    FL_DMA_Init(DMA, &DMA_InitStruct, FL_DMA_CHANNEL_7);
    FL_DMA_WriteFlashAddress(DMA, address >> 2);
    FL_DMA_WriteMemoryAddress(DMA, (uint32_t)data >> 2, FL_DMA_CHANNEL_7);
    FL_DMA_WriteTransmissionSize(DMA, 64 - 1, FL_DMA_CHANNEL_7);
    FL_DMA_ClearFlag_TransferComplete(DMA, FL_DMA_CHANNEL_7);
    FL_DMA_EnableChannel(DMA, FL_DMA_CHANNEL_7);
    timeout = 0;
    while(1)
    {
        timeout++;
        if(timeout > FL_FLASH_ERASE_TIMEOUT)
        {
            ret = FL_FAIL;
            break;
        }
        if(FL_DMA_IsActiveFlag_TransferComplete(DMA, FL_DMA_CHANNEL_7) == FL_SET)
        {
            ret = FL_PASS;
            break;
        }
    }
    FL_FLASH_LockFlash(FLASHx);
    FL_CMU_DisableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_CMU_DisableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    return ret;
}

/**
  * @brief  DMA??????????????????????????????????????????halt-page.
  * @param  FLASHx FLASH Port
  * @param  address ????????????Flash??????
  * @param  *data ?????????????????????
  * @param  length ????????????????????????
  *
  * @retval ErrorStatus?????????
  *         -FL_FAIL ??????????????????
  *         -FL_PASS ????????????
  */
FL_ErrorStatus FL_FLASH_Read_Dma(FLASH_Type *FLASHx, uint32_t address, uint32_t *data, uint16_t length)
{
    FL_ErrorStatus ret;
    uint32_t Timeout;
    FL_DMA_InitTypeDef DMA_InitStruct = {0};
    /* ?????????????????? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_ADDR(address));
    /* ?????????*/
    if(address & (FL_FLASH_ADDRS_ALIGN - 1))
    {
        /*???????????????*/
        return FL_FAIL;
    }
    DMA_InitStruct.circMode = FL_DISABLE;
    DMA_InitStruct.direction = FL_DMA_DIR_FLASH_TO_RAM;
    DMA_InitStruct.memoryAddressIncMode = FL_DMA_CH7_RAM_INC_MODE_INCREASE;
    DMA_InitStruct.flashAddressIncMode = FL_DMA_CH7_FLASH_INC_MODE_INCREASE;
    DMA_InitStruct.priority = FL_DMA_PRIORITY_HIGH;
    FL_DMA_Init(DMA, &DMA_InitStruct, FL_DMA_CHANNEL_7);
    FL_DMA_WriteFlashAddress(DMA, address >> 2);
    FL_DMA_WriteMemoryAddress(DMA, (uint32_t)data >> 2, FL_DMA_CHANNEL_7);
    FL_DMA_WriteTransmissionSize(DMA, length - 1, FL_DMA_CHANNEL_7);
    FL_DMA_ClearFlag_TransferComplete(DMA, FL_DMA_CHANNEL_7);
    FL_DMA_EnableChannel(DMA, FL_DMA_CHANNEL_7);
    Timeout = 0;
    while(1)
    {
        Timeout++;
        if(Timeout > FL_FLASH_ERASE_TIMEOUT)
        {
            ret = FL_FAIL;
            break;
        }
        if(FL_DMA_IsActiveFlag_TransferComplete(DMA, FL_DMA_CHANNEL_7) == FL_SET)
        {
            ret = FL_PASS;
            break;
        }
    }
    return ret;
}

/**
  * @}
  */
/**
  * @}
  */
/*************************************************************END OF FILE************************************************************/









