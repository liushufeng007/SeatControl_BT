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
  * @brief  Flash 页擦除函数，�?个页�?512byte.
  * @param  FLASHx FLASH Port
  * @param  address 为需要擦除的页内任意地址，推荐使用页�?始的首地�?（字对齐�?
  *         .
  * @retval ErrorStatus枚举�?
  *         -FL_FAIL 擦写发生错误
  *         -FL_PASS 擦写成功
  */
FL_ErrorStatus FL_FLASH_PageErase(FLASH_Type *FLASHx, uint32_t address)
{
    uint32_t timeout = 0;
    uint32_t primask;
    FL_ErrorStatus ret = FL_PASS;
    /* 入口参数�?�? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_ADDR((uint32_t)address));
    /*时钟使能*/
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    if(address & (FL_FLASH_ADDRS_ALIGN - 1))
    {
        /*地址未对�?*/
        return FL_FAIL;
    }
    if(FL_FLASH_GetFlashLockStatus(FLASHx) == FL_FLASH_KEY_STATUS_ERROR)
    {
        /*Flash 已经锁定，复位前无法操作*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    /*配置擦写类型*/
    FL_FLASH_SetFlashEraseType(FLASHx, FL_FLASH_ERASE_TYPE_PAGE);
    /* �?始擦除页*/
    FL_FLASH_EnableErase(FLASHx);
    /* Key 序列*/
    primask = __get_PRIMASK();
    __disable_irq();
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_ERASE_KEY);
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_PGAE_ERASE_KEY);
    __set_PRIMASK(primask);
    FL_FLASH_ClearFlag_EraseComplete(FLASHx);
    /* 擦请�? */
    *((uint32_t *)address) =  FL_FLASH_ERASE_REQUEST;
    while(1)
    {
        timeout++;
        if((timeout > FL_FLASH_ERASE_TIMEOUT)\
                || (FL_FLASH_IsActiveFlag_ClockError(FLASHx))\
                || (FL_FLASH_IsActiveFlag_KeyError(FLASHx))\
                || (FL_FLASH_IsActiveFlag_AuthenticationError(FLASHx)))
        {
            /* 超时或出现错�? */
            ret = FL_FAIL;
            break;
        }
        else
            if(FL_FLASH_IsActiveFlag_EraseComplete(FLASHx))
            {
                /*编程成功*/
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
  * @brief  Flash 扇区擦除函数，一个扇区为2k byte.
  * @param  FLASHx FLASH Port
  * @param  address 为需要擦除的扇区内任意地�?，推荐使用扇区开始的首地�?（字对齐�?
  *         .
  * @retval ErrorStatus枚举�?
  *         -FL_FAIL 擦写发生错误
  *         -FL_PASS 擦写成功
  */
FL_ErrorStatus FL_FLASH_SectorErase(FLASH_Type *FLASHx, uint32_t address)
{
    uint32_t timeout = 0;
    uint32_t primask;
    FL_ErrorStatus ret = FL_PASS;
    /* 入口参数�?�? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_ADDR((uint32_t)address));
    /*时钟使能*/
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    if(address & (FL_FLASH_ADDRS_ALIGN - 1))
    {
        /*地址未对�?*/
        return FL_FAIL;
    }
    if(FL_FLASH_GetFlashLockStatus(FLASHx) == FL_FLASH_KEY_STATUS_ERROR)
    {
        /*Flash 已经锁定，复位前无法操作*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    /*配置擦写类型*/
    FL_FLASH_SetFlashEraseType(FLASHx, FL_FLASH_ERASE_TYPE_SECTOR);
    /* �?始擦除扇�?*/
    FL_FLASH_EnableErase(FLASHx);
    /* Key 序列*/
    primask = __get_PRIMASK();
    __disable_irq();
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_ERASE_KEY);
    FL_FLASH_UnlockFlash(FLASHx, FL_FLASH_SECTOR_ERASE_KEY);
    __set_PRIMASK(primask);
    FL_FLASH_ClearFlag_EraseComplete(FLASHx);
    /* 擦请�? */
    *((uint32_t *)address) =  FL_FLASH_ERASE_REQUEST;
    while(1)
    {
        timeout++;
        if((timeout > FL_FLASH_ERASE_TIMEOUT)\
                || (FL_FLASH_IsActiveFlag_ClockError(FLASHx))\
                || (FL_FLASH_IsActiveFlag_KeyError(FLASHx))\
                || (FL_FLASH_IsActiveFlag_AuthenticationError(FLASHx)))
        {
            /* 超时或出现错�? */
            ret = FL_FAIL;
            break;
        }
        else
            if(FL_FLASH_IsActiveFlag_EraseComplete(FLASHx))
            {
                /*编程成功*/
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
  * @brief  单次编程函数，编程地�?必须对齐到字边界.
  * @param  FLASHx FLASH Port
  * @param  address 为需要编程的已经擦除过的扇区内任意地�?，非对齐地址编程将触发fault�?
  *
  * @retval ErrorStatus枚举�?
  *         -FL_FAIL 擦写发生错误
  *         -FL_PASS 擦写成功
  */
FL_ErrorStatus FL_FLASH_Program_Word(FLASH_Type *FLASHx, uint32_t address, uint32_t data)
{
    uint32_t timeout = 0;
    uint32_t primask;
    FL_ErrorStatus ret = FL_PASS;
    /* 入口参数�?�? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_ADDR((uint32_t)address));
    /*时钟使能*/
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    if(address & (FL_FLASH_ADDRS_ALIGN - 1))
    {
        /*地址未对�?*/
        return FL_FAIL;
    }
    if(FL_FLASH_GetFlashLockStatus(FLASHx) == FL_FLASH_KEY_STATUS_ERROR)
    {
        /*Flash 已经锁定，复位前无法操作*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    /* �?始编�?*/
    FL_FLASH_EnableProgram(FLASHx);
    /* Key 序列*/
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
            /* 超时或出现错�? */
            ret = FL_FAIL;
            break;
        }
        else
            if(FL_FLASH_IsActiveFlag_ProgramComplete(FLASHx))
            {
                /*编程成功*/
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
  * @brief  页编程函数，编程地址必须对齐到字边界.
  * @param  FLASHx FLASH Port
  * @param  PageNum 为需要编程的已经擦除过的扇区号，FM33LC04�?大为256，非对齐地址编程将触发fault�?
  *
  * @retval ErrorStatus枚举�?
  *         -FL_FAIL 擦写发生错误
  *         -FL_PASS 擦写成功
  */
FL_ErrorStatus FL_FLASH_Program_Page(FLASH_Type *FLASHx, uint32_t pageNum, uint32_t *data)
{
    uint32_t count;
    uint32_t primask;
    uint32_t address;
    uint32_t timeout;
    FL_ErrorStatus ret;
    /* 入口参数�?�? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_PAGE((uint32_t)pageNum));
    address = pageNum * FL_FLASH_PGAE_SIZE_BYTE;
    /* 页对�?*/
    if(address & (FL_FLASH_PGAE_SIZE_BYTE - 1))
    {
        /*地址未对�?*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_FLASH_EnableProgram(FLASHx);
    /* Key 序列*/
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
                /* 超时或出现错�? */
                ret = FL_FAIL;
                break;
            }
            if(FL_FLASH_IsActiveFlag_ProgramComplete(FLASHx))
            {
                /*编程成功*/
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
  * @brief  扇区编程函数，编程地�?必须对齐到字边界.
  * @param  FLASHx FLASH Port
  * @param  sectorNum 为需要编程的已经擦除过的扇区号，�?大为128，非对齐地址编程将触发fault�?
  *
  * @retval ErrorStatus枚举�?
  *         -FL_FAIL 擦写发生错误
  *         -FL_PASS 擦写成功
  */
FL_ErrorStatus FL_FLASH_Program_Sector(FLASH_Type *FLASHx, uint32_t sectorNum, uint32_t *data)
{
    uint32_t count;
    uint32_t primask;
    uint32_t address;
    uint32_t timeout;
    FL_ErrorStatus ret;
    /* 入口参数�?�? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_SECTOR((uint32_t)sectorNum));
    address = sectorNum * FL_FLASH_SECTOR_SIZE_BYTE;
    /* 页对�?*/
    if(address & (FL_FLASH_SECTOR_SIZE_BYTE - 1))
    {
        /*地址未对�?*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_FLASH_EnableProgram(FLASHx);
    /* Key 序列*/
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
                /* 超时或出现错�? */
                ret = FL_FAIL;
                break;
            }
            if(FL_FLASH_IsActiveFlag_ProgramComplete(FLASHx))
            {
                /*编程成功*/
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
  * @brief  DMA编程函数，编程地�?必须对齐到halt-page，长度固定为64�?.
  * @param  FLASHx FLASH Port
  * @param  address 待编程Flash地址
  * @param  *data 待写入Flash数据
  *
  * @retval ErrorStatus枚举�?
  *         -FL_FAIL 擦写发生错误
  *         -FL_PASS 擦写成功
  */
FL_ErrorStatus FL_FLASH_Write_Dma(FLASH_Type *FLASHx, uint32_t address, uint32_t *data)
{
    FL_ErrorStatus ret;
    uint32_t primask;
    uint32_t timeout;
    FL_DMA_InitTypeDef DMA_InitStruct = {0};
    /* 入口参数�?�? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_ADDR(address));
    /* 半页对齐*/
    if(address & (FL_FLASH_PGAE_SIZE_BYTE / 2 - 1))
    {
        /*地址未对�?*/
        return FL_FAIL;
    }
    FL_CMU_EnableGroup2BusClock(FL_CMU_GROUP2_BUSCLK_FLASH);
    FL_CMU_EnableGroup3OperationClock(FL_CMU_GROUP3_OPCLK_FLASH);
    FL_FLASH_EnableProgram(FLASHx);
    /* Key 序列*/
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
  * @brief  DMA读取函数，编程地�?必须对齐到halt-page.
  * @param  FLASHx FLASH Port
  * @param  address 读取数据Flash地址
  * @param  *data 读出数据存储�?
  * @param  length 读出数据的字长度
  *
  * @retval ErrorStatus枚举�?
  *         -FL_FAIL 擦写发生错误
  *         -FL_PASS 擦写成功
  */
FL_ErrorStatus FL_FLASH_Read_Dma(FLASH_Type *FLASHx, uint32_t address, uint32_t *data, uint16_t length)
{
    FL_ErrorStatus ret;
    uint32_t Timeout;
    FL_DMA_InitTypeDef DMA_InitStruct = {0};
    /* 入口参数�?�? */
    assert_param(IS_FLASH_ALL_INSTANCE(FLASHx));
    assert_param(IS_FL_FLASH_MAX_ADDR(address));
    /* 字对�?*/
    if(address & (FL_FLASH_ADDRS_ALIGN - 1))
    {
        /*地址未对�?*/
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









