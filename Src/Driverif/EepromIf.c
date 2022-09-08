

/* ===========================================  Includes  =========================================== */
#include "user_init.h"
#include "Eepromif.h"

/* ============================================  Define  ============================================ */
#define OPERATE_EEPROM_AREA_WRITED   0xFFFFFFFFUL                  /* flag not modifyed*/
#define OPERATE_EEPROM_AREA_FLAG     0x00000000UL                  /* write or read area flag */
#define COPY_DATA_COMPLETE_FLAG      0x00000000UL                  /* current page data copy complete flag*/
#define EEPROM_SPACE_FLAG_OFFSET_0   4UL                           /* block  operating*/
#define EEPROM_SPACE_FLAG_OFFSET_1   (EEPROM_SPACE_FLAG_OFFSET_0 + 4) /* offset address in eflash page */
#define EEPROM_COPY_FLAG_OFFSET     (EEPROM_SPACE_FLAG_OFFSET_1 + 4) /* the offset for saveing copy complete flag */
#define EEPROM_END_ADDRESS_OFFSET   (EEPROM_COPY_FLAG_OFFSET + 4)  /* eeprom end address offset */


/*!
* @brief The assert_param macro is used for function's check.
*
* @param  expr: If x is false, it calls assert_failed function which reports 
*         the name of the source file and the source line number of the call 
*         that failed. If x is true, it returns no value.
* @retval none
*/
#define DEVICE_ASSERT(x) if(x){while(0);}else{while(1);} 

    
/* ==========================================  Variables  =========================================== */
static uint32_t s_eepromSize[EEPROM_BANK_NUMBER];// = 512UL;  /* eeprom default size 512 half word ,actual use one page eflash */
static uint32_t s_eepromPages[EEPROM_BANK_NUMBER];// = 1UL;   /* eeprom default use one page eflash */
static uint32_t s_eepromStartAddr[EEPROM_BANK_NUMBER];// = FL_FLASH_ADDR_MAXPROGRAM - FL_FLASH_PGAE_SIZE_BYTE; /* eeprom start address in eflash */


/* ======================================  Functions define  ======================================== */
/*!
 * @brief software eeprom initialize,actual available space are (size - 2)
 *
 * @param[in] size: config software eeprom size(half word)
 * @param[in] pageIndex: the start page index in eflash occupied by software eeprom
 * @return none
 */
void SWEEPROM_Init(uint16_t size, uint32_t pageIndex,uint8_t bankId)
{
    DEVICE_ASSERT(pageIndex * FL_FLASH_PGAE_SIZE_BYTE + (size <<2) <= FL_FLASH_ADDR_MAXPROGRAM - FL_FLASH_PGAE_SIZE_BYTE);
	DEVICE_ASSERT(bankId<EEPROM_BANK_NUMBER);
    
    
    s_eepromSize[bankId] = size;
    s_eepromStartAddr[bankId] = FL_FLASH_ADDR_BASEPROGRAM + pageIndex * FL_FLASH_PGAE_SIZE_BYTE;
    s_eepromPages[bankId]  = s_eepromSize[bankId] / (FL_FLASH_PGAE_SIZE_BYTE >> 2);
    if (s_eepromSize[bankId] % (FL_FLASH_PGAE_SIZE_BYTE >> 2))
    {
        s_eepromPages[bankId] = s_eepromPages[bankId] + 1;
        s_eepromSize[bankId] = s_eepromPages[bankId] * (FL_FLASH_PGAE_SIZE_BYTE >> 2);
    }
}

/*!
 * @brief write data to eflash
 *
 * @param[in] writeFlashStartAddr: write eflash start address
 * @param[in] eepromAddr: software eeprom address
 * @param[in] dataBuffer: data pointer
 * @param[in] writeNum: write half word number
 * @return statusRes: software eeprom write status
 */
FL_ErrorStatus WriteDataToEflash(uint32_t writeFlashStartAddr, uint16_t eepromAddr, const uint16_t *dataBuffer, uint16_t writeNum)
{
    FL_ErrorStatus statusRes = FL_PASS;
    uint32_t newCombineData = 0;
    uint32_t tempWriteAddr = 0;
    uint32_t i = 0;

    for (i = 0; i < writeNum; i++)
    {
        tempWriteAddr = writeFlashStartAddr + i * 4;
        newCombineData = ((eepromAddr + i) << 16) + dataBuffer[i];
        statusRes = FL_FLASH_Program_Word(FLASH,tempWriteAddr, newCombineData);
        if (FL_PASS != statusRes)
        {
            break;
        }
    }

    return statusRes;
}

/*!
 * @brief look for the first data which is not 0xFFFFFFFF form end to start address
 *
 * @param[in] endAddr: end address
 * @param[in] validDataAddr: valid data start address
 * @return 0: no valid data, 1: found valid data
 */
static uint8_t GetValidDataStartAddr(uint32_t endAddr, uint32_t *validDataAddr,uint8_t bankId)
{
    uint32_t i = 0;
    uint8_t ret = 0;

    for (i = 0; i < ((s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE) >> 2) - 3; i++)
    {
        if (0xffffffff !=  *((uint32_t *)(endAddr - i * 4)))
        {
            ret = 1;
            break;
        }
    }
		
	*validDataAddr = endAddr - i * 4 + 4;

    return ret;
}

/*!
 * @brief Check data whether valid
 *
 * @param[in] newAreaCheckStartAddr: new software eeprom space check start address
 * @param[in] oldAreaCheckStartAddr: old software eeprom spacecheck start address, high 24 bits are address, low 8 bits are data
 * @param[in] checkLength: the valid data length of backup software eeprom
 * @return isDataValid: the chacked data whether valid
 */
static uint8_t IsDataValid(uint32_t newAreaCheckStartAddr, uint32_t *oldAreaCheckStartAddr, uint32_t checkLength)
{
    uint8_t isDataValid = TRUE;
    uint32_t i = 0;

    for (i = 0; i < checkLength; i++)
    {
        if (((*(uint32_t *)(newAreaCheckStartAddr - i * 4)) >> 16) == ((*(uint32_t *)(*oldAreaCheckStartAddr)) >> 16))
        {
            isDataValid = FALSE;
        }
    }

    return isDataValid;
}

/*!
 * @brief get data from software eeprom
 *
 * @param[in] eepromEndAddr: software eeprom end address
 * @param[in] eepromDataLength: software eeprom data length
 * @param[in] readAddr: read software eeprom address
 * @param[in] data: read data from software eeprom
 * @return none
 */
static void GetDataFromEEPROM(uint32_t eepromEndAddr, uint32_t eepromDataLength, uint16_t readAddr, uint16_t *data)
{
    uint32_t i = 0;

    *data = 0xFFFF;
    for (i = 0; i < eepromDataLength; i++)
    {
        if ((*(uint32_t *)((eepromEndAddr - i * 4)) >> 16) == readAddr)
        {
           *data = *(uint16_t *)((eepromEndAddr - i * 4));
           break;
        }
    }
}

/*!
 * @brief copy valid data to backup area
 *
 * @param[in] newAreaStartAddr: new software eeprom start address
 * @param[in] oldAreaStartAddr: old software eeprom start address
 * @return statusRes: copy data status
 */
static FL_ErrorStatus CopyDataToNewArea(uint32_t newAreaStartAddr, uint32_t oldAreaStartAddr,uint8_t bankId)
{
    FL_ErrorStatus statusRes = FL_PASS;
    uint32_t oldAreaCopyStartAddr = 0;
    uint32_t newAreaCopyStartAddr = 0;
    uint32_t newAreaDataLen = 0;
    uint32_t oldAreaDataLen = 0;
    uint32_t checkStartAddr = 0;
    uint32_t i = 0;

    checkStartAddr = newAreaStartAddr + s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE - EEPROM_END_ADDRESS_OFFSET;
    if(GetValidDataStartAddr(checkStartAddr, &newAreaCopyStartAddr,bankId))
    {
        newAreaDataLen = ((newAreaCopyStartAddr - newAreaStartAddr) >> 2) + 1;
    }
    checkStartAddr = oldAreaStartAddr + s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE - EEPROM_END_ADDRESS_OFFSET;
    if(GetValidDataStartAddr(checkStartAddr, &oldAreaCopyStartAddr,bankId))
    {
         /* The last used data not 0xFFFFFFFF, src copy address no need decrease 4*/
        oldAreaDataLen = ((oldAreaCopyStartAddr - oldAreaStartAddr) >> 2) + 1;
    }
    for (i = 0; i < oldAreaDataLen; i++)
    {
        /* check data effective */
        if (IsDataValid(newAreaCopyStartAddr, &oldAreaCopyStartAddr, newAreaDataLen))
        {
            /* caculate write eeprom backup start address */
            /* copy data to backup area*/
            statusRes = FL_FLASH_Program_Word(FLASH,newAreaCopyStartAddr + 4, *((uint32_t *)oldAreaCopyStartAddr));
            if (FL_PASS!= statusRes)
            {
                break;
            }
            newAreaDataLen++;
            newAreaCopyStartAddr = newAreaCopyStartAddr + 4;
        }
        oldAreaCopyStartAddr = oldAreaCopyStartAddr - 4;
    }

    return statusRes;
}

/*!
 * @brief update software eeprom operate address
 *
 * @param[in] newStartAddr: new eeprom area start address
 * @param[in] oldStartAddr: old eeprom area start address
 * @return statusRes: software eeprom write status
 */
static FL_ErrorStatus UpdateOperateAddr(uint32_t *newStartAddr, uint32_t *oldStartAddr,uint8_t bankId)
{
    FL_ErrorStatus statusRes = FL_PASS;
    uint32_t eepromEndAddr = 0;
    uint32_t eepromnewEndAddr = 0;
    uint32_t  flag = COPY_DATA_COMPLETE_FLAG;

    eepromEndAddr = s_eepromStartAddr[bankId] + s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE;

	/*current block == inactive block*/
    if (OPERATE_EEPROM_AREA_WRITED != *(uint32_t *)(eepromEndAddr - EEPROM_SPACE_FLAG_OFFSET_1))
    {
        /* operate address in eeprom backup area */
        *newStartAddr = s_eepromStartAddr[bankId] - s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE;
        *oldStartAddr = s_eepromStartAddr[bankId];
		/*write current block inactive flag*/
	}
	else
	{
		/*start flag be modify,current block == active block*/
		if (OPERATE_EEPROM_AREA_WRITED != *(uint32_t *)(eepromEndAddr - EEPROM_SPACE_FLAG_OFFSET_0))
		{
			/* operate address in eeprom area */
			*newStartAddr = s_eepromStartAddr[bankId];
			*oldStartAddr = s_eepromStartAddr[bankId] - s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE;
			eepromEndAddr = s_eepromStartAddr[bankId];
		}
		else
		{/*start flag be modify,current block == inactive block*/
			/* operate address in eeprom backup area */
			*newStartAddr = s_eepromStartAddr[bankId] - s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE;
			*oldStartAddr = s_eepromStartAddr[bankId];
			/*write current block inactive flag*/
		}
	}
	

    /* check data copy is successed last time, if not success, will copy data again to avoid power down suddenly*/
    if ((~flag) == *(uint32_t *) (eepromEndAddr - EEPROM_COPY_FLAG_OFFSET))
    {
        /* copy data again */
        statusRes = CopyDataToNewArea(*newStartAddr, *oldStartAddr,bankId);
		eepromnewEndAddr = *oldStartAddr + s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE;
		if (FL_PASS == statusRes)
		{
			if(OPERATE_EEPROM_AREA_WRITED == *(uint32_t *) (eepromnewEndAddr - EEPROM_SPACE_FLAG_OFFSET_0))
			{
			  statusRes = FL_FLASH_Program_Word(FLASH,eepromnewEndAddr  - EEPROM_SPACE_FLAG_OFFSET_0, flag);
			}
		}
		if (FL_PASS == statusRes)
		{
			if(OPERATE_EEPROM_AREA_WRITED == *(uint32_t *) (eepromEndAddr - EEPROM_SPACE_FLAG_OFFSET_1))
			{
				statusRes = FL_FLASH_Program_Word(FLASH,eepromEndAddr - EEPROM_SPACE_FLAG_OFFSET_1, flag);
			}
		}
        if (FL_PASS == statusRes)
        {
			if(OPERATE_EEPROM_AREA_WRITED == *(uint32_t *) (eepromEndAddr - EEPROM_COPY_FLAG_OFFSET))
			{
				statusRes = FL_FLASH_Program_Word(FLASH,eepromEndAddr - EEPROM_COPY_FLAG_OFFSET, flag);
			}
            /* write copy complete flag in the eighth last position */
        }
    }

   return statusRes;
}

/*!
 * @brief software eeprom Write expand
 *
 * @param[in] writeAddr: software eeprom address
 * @param[in] dataBuffer: data buffer pointer
 * @param[in] writeNum: write half word number
 * @return statusRes: software eeprom write status
 */
FL_ErrorStatus SWEEPROM_WriteExt(uint16_t writeAddr, const uint16_t *dataBuffer, uint16_t writeNum,uint8_t bankId)
{
    FL_ErrorStatus statusRes = FL_PASS;
    uint32_t leftSize = 0;
    uint32_t newWriteStartAddr = 0;
    uint32_t oldWriteStartAddr = 0;
    uint32_t validDataStartAddr = 0;
    uint32_t flag = 0;
    uint8_t i = 0;

    statusRes = UpdateOperateAddr(&newWriteStartAddr, &oldWriteStartAddr,bankId);
    if (FL_PASS != statusRes)
    {
        return statusRes;
    }
    /* because the last eflash address save the operate eeprom address flag, so
       form the last fifth eflash address check start page has writable enough space
    */
    if(GetValidDataStartAddr(newWriteStartAddr + s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE - EEPROM_END_ADDRESS_OFFSET, &validDataStartAddr,bankId))
    {
       leftSize = (newWriteStartAddr + s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE - EEPROM_END_ADDRESS_OFFSET - validDataStartAddr) >> 2; 
    }
    else
    {
        leftSize = (s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE - EEPROM_COPY_FLAG_OFFSET) >> 2;
    }

    /* check space is enough, every half word combine with write address will use 4 eflash address */
    if (leftSize >= writeNum)
    {
        /* has enough space */
        statusRes = WriteDataToEflash(validDataStartAddr, writeAddr, dataBuffer, writeNum);
    }
    else
    {
        for (i = 0; i < s_eepromPages[bankId]; i++)
        {
            /* erase backup eflash area, size equal the configraton of eeprom
               eflash backup position in front of the eeprom start position
            */

            statusRes = FL_FLASH_PageErase(FLASH,oldWriteStartAddr + FL_FLASH_PGAE_SIZE_BYTE * i);
			
            if (FL_PASS != statusRes)
            {
                return statusRes;
            }
        }
        /* new data write in the back of the old data */
        statusRes = WriteDataToEflash(oldWriteStartAddr, writeAddr, dataBuffer, writeNum);
        /* write 0x00000000 to the last eflash 4 address to flag
           current operate is in eeprom backup area
        */
        if (FL_PASS != statusRes)
        {
            return statusRes;
        }
        flag = OPERATE_EEPROM_AREA_FLAG;
        /* write current data to backup eflash area */
        if (FL_PASS == statusRes)
        {
            /* copy effective data form eeprom to backup area */
            statusRes = CopyDataToNewArea(oldWriteStartAddr, newWriteStartAddr,bankId);
			
            if (FL_PASS == statusRes)
            {
				if(OPERATE_EEPROM_AREA_WRITED == *(uint32_t *) (oldWriteStartAddr  + s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE - EEPROM_SPACE_FLAG_OFFSET_0))
				{
					statusRes = FL_FLASH_Program_Word(FLASH,oldWriteStartAddr + s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE  - EEPROM_SPACE_FLAG_OFFSET_0, flag);
				}
			}

            if (FL_PASS == statusRes)
            {
				statusRes = FL_FLASH_Program_Word(FLASH,newWriteStartAddr + s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE - EEPROM_SPACE_FLAG_OFFSET_1, flag);
			}

            if (FL_PASS == statusRes)
            {
                /* write copy complete flag in the eighth last position */
                flag = COPY_DATA_COMPLETE_FLAG;
                statusRes = FL_FLASH_Program_Word(FLASH,newWriteStartAddr + s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE - EEPROM_COPY_FLAG_OFFSET, flag);
            }
        }
    }

    return statusRes;
}

/*!
 * @brief software eeprom Write
 *
 * @param[in] writeAddr: software eeprom address
 * @param[in] dataBuffer: data buffer pointer
 * @param[in] writeNum: write half word number
 * @return statusRes: software eeprom write status
 */
FL_ErrorStatus SWEEPROM_Write(uint16_t writeAddr, const uint16_t *dataBuffer, uint16_t writeNum,uint8_t bankId)
{
    FL_ErrorStatus statusRes = FL_PASS;

    /* check write address is in eeprom size scope */
	
	  DEVICE_ASSERT(bankId<EEPROM_BANK_NUMBER);
    DEVICE_ASSERT(s_eepromSize[bankId] - 3 > writeAddr);
    DEVICE_ASSERT(dataBuffer != NULL);
    DEVICE_ASSERT(0 != writeNum);

    /* when debug suggest redefine function DEVICE_ASSERT, else it's easy happen error */
    DEVICE_ASSERT((s_eepromSize[bankId] - 3) >= (writeAddr + writeNum));

    statusRes = SWEEPROM_WriteExt(writeAddr, dataBuffer, writeNum,bankId);

    return statusRes;
}

/*!
 * @brief software eeprom read
 *
 * @param[in] readAddr: Read start address of software software eeprom
 * @param[in] dataBuffer: Point to the buffer ready to read the data from software eeorom
 * @param[in] readNum: read half word number
 * @return software eeprom read status
 */
FL_ErrorStatus SWEEPROM_Read(uint16_t readAddr, uint16_t *dataBuffer, uint16_t readNum,uint8_t bankId)
{
    FL_ErrorStatus statusRes = FL_PASS;
    uint32_t newStartAddr = 0;
    uint32_t eepromDataLen = 0;
    uint32_t validDataStartAddr = 0;
    uint32_t i = 0;
    uint32_t oldStartAddr = 0;
	
	DEVICE_ASSERT(bankId<EEPROM_BANK_NUMBER);
    DEVICE_ASSERT(s_eepromSize[bankId] > readAddr);
    DEVICE_ASSERT((s_eepromSize[bankId] - 3) >= (readAddr + readNum));
    DEVICE_ASSERT(dataBuffer != NULL);
	DEVICE_ASSERT(bankId<EEPROM_BANK_NUMBER);
	
	statusRes = UpdateOperateAddr(&newStartAddr, &oldStartAddr,bankId);
    if (FL_PASS != statusRes)
    {
        return statusRes;
    }
	
    /* check the atart address of valid data */
    if (GetValidDataStartAddr(newStartAddr + s_eepromPages[bankId] * FL_FLASH_PGAE_SIZE_BYTE - EEPROM_END_ADDRESS_OFFSET, &validDataStartAddr,bankId))
    {
        eepromDataLen = ((validDataStartAddr - newStartAddr) >> 2) + 1; 
    }

    for (i = 0; i < readNum; i++)
    {
        /* check data effective */
        GetDataFromEEPROM(validDataStartAddr, eepromDataLen, readAddr + i, &dataBuffer[i]);
    }

    return statusRes;
}

/*!
 * @brief software eeprom erase(include backup area)
 *
 * @param[in] none
 * @return statusRes: software eeprom erase status
 */
FL_ErrorStatus SWEEPROM_Erase(uint8_t bankId)
{
    FL_ErrorStatus statusRes = FL_PASS;
    uint8_t i = 0;
	DEVICE_ASSERT(bankId<EEPROM_BANK_NUMBER);
	
    for (i = 0; i < s_eepromPages[bankId] * 2; i++)
    {
        /* erase eeprom space and backup space*/
        statusRes = FL_FLASH_PageErase(FLASH,s_eepromStartAddr[bankId] + (s_eepromPages[bankId] - 1 - i) * FL_FLASH_PGAE_SIZE_BYTE);
        if (FL_PASS != statusRes)
        {
            break;
        }
    }

    return statusRes;
}
/* =============================================  EOF  ============================================== */
