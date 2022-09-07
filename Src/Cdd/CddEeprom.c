/*******************************************************************************
|  File Name:  SwM.c
|  Description:  Implementation of the Swtich Management
|-------------------------------------------------------------------------------
|******************************************************************************/

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"
#include "Eepromif.h"
#include "CddEeprom_Cfg.h"


#include "System_OsTask.h"
#include "System_OsMessageQueue.h"



/*******************************************************************************
|    Macro Definition
|******************************************************************************/

#define RAM_START_ADDR  ((uint32_t)0x20000000)
#define RAM_END_ADDR    ((uint32_t)0x20007FFF)
/*******************************************************************************
|    Enum Definition
|******************************************************************************/

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
#define CddEeprom_Error_Handle() \
			if(PTR_NULL != (*Eeprom_Mgn_Ctrl[BankId].CallBack_Func))\
			{\
				(*Eeprom_Mgn_Ctrl[BankId].CallBack_Func)(FALSE);\
			}\
			Eeprom_BanK_Mng_Step[BankId] = EEPROM_BANK_WRITE_STEP_IDLE;\
			*(Eeprom_Mgn_Ctrl[BankId].Req_Write_Flag) = FALSE

			
/*******************************************************************************
|    Global variables Declaration
|******************************************************************************/


/*******************************************************************************
|    Static local variables Declaration
|******************************************************************************/
static void CddEeprom_Bank_Handle(uint8_t		BankId);



/*******************************************************************************
|    Table Definition
|******************************************************************************/




/*******************************************************************************
Name			: CddEeprom_Init
Syntax			: void CddEeprom_Init(void)
|******************************************************************************/

void CddEeprom_Init(void)
{
	SWEEPROM_Init(SWEEPROM_SIZE_MOTOR, SWEEPROM_START_SECTOR_ID_MOTOR,EEPROM_BANK_MOTOR);
    SWEEPROM_Init(SWEEPROM_SIZE_APP, SWEEPROM_START_SECTOR_ID_APP,EEPROM_BANK_APP);
    SWEEPROM_Init(SWEEPROM_SIZE_DIAG, SWEEPROM_START_SECTOR_ID_DIAG,EEPROM_BANK_DIAG);
    SWEEPROM_Init(SWEEPROM_SIZE_FBL, SWEEPROM_START_SECTOR_ID_FBL,EEPROM_BANK_FBL);

	memset(Eeprom_BanK_Mng_Step,0,sizeof(Eeprom_BanK_Mng_Step));
	memset(Eeprom_Req_W_Flag_u8,0,sizeof(Eeprom_Req_W_Flag_u8));
	memset(Eeprom_Req_Addr_u16,0,sizeof(Eeprom_Req_Addr_u16));
	memset(Eeprom_Req_Len_u16,0,sizeof(Eeprom_Req_Len_u16));
	memset(Eeprom_Callback_Arr,0,sizeof(Eeprom_Callback_Arr));
	

	SWEEPROM_Read(0,Eeprom_Mgn_Ctrl[EEPROM_BANK_MOTOR].Ram_Mirror_Ptr,Eeprom_Mgn_Ctrl[EEPROM_BANK_MOTOR].Max_Length,EEPROM_BANK_MOTOR);
	SWEEPROM_Read(0,Eeprom_Mgn_Ctrl[EEPROM_BANK_APP].Ram_Mirror_Ptr,Eeprom_Mgn_Ctrl[EEPROM_BANK_APP].Max_Length,EEPROM_BANK_APP);
	SWEEPROM_Read(0,Eeprom_Mgn_Ctrl[EEPROM_BANK_DIAG].Ram_Mirror_Ptr,Eeprom_Mgn_Ctrl[EEPROM_BANK_DIAG].Max_Length,EEPROM_BANK_DIAG);	
	SWEEPROM_Read(0,Eeprom_Mgn_Ctrl[EEPROM_BANK_DIAG].Ram_Mirror_Ptr,Eeprom_Mgn_Ctrl[EEPROM_BANK_DIAG].Max_Length,EEPROM_BANK_FBL);

	
}

/*******************************************************************************
Name			: CddEeprom_Task
Syntax			: void CddEeprom_Task(void)
|******************************************************************************/

void CddEeprom_Task(void)
{
	Eeprom_Bank_ID_e fl_bank_id ;
	for(fl_bank_id = EEPROM_BANK_MOTOR; fl_bank_id < EEPROM_BANK_NUMBER ; fl_bank_id ++)
	{
		CddEeprom_Bank_Handle(fl_bank_id);
		if(Eeprom_BanK_Mng_Step[fl_bank_id] != EEPROM_BANK_WRITE_STEP_IDLE)
		{
			break;
		}
	}
}

/*******************************************************************************
Name			: CddEeprom_Bank_Write
Syntax			: void CddEeprom_Bank_Write(uint8_t  BankId)
|******************************************************************************/
static void CddEeprom_Bank_Handle(uint8_t       BankId)
{

	switch(Eeprom_BanK_Mng_Step[BankId])
	{
		case EEPROM_BANK_WRITE_STEP_IDLE:
			if(TRUE == *(Eeprom_Mgn_Ctrl[BankId].Req_Write_Flag) && (Eeprom_Mgn_Ctrl[BankId].BankId == BankId))
			{
				Eeprom_BanK_Mng_Step[BankId] = EEPROM_BANK_WRITE_STEP_WRITE;
			}
		break;
		
		case EEPROM_BANK_WRITE_STEP_WRITE:
			/*test->write 20byte normal time = 1.4ms,max time = 12ms at battery =12V*/
			if(FL_PASS == SWEEPROM_Write(*(Eeprom_Mgn_Ctrl[BankId].Addr) , (uint16_t *)(Eeprom_Mgn_Ctrl[BankId].Ram_Mirror_Ptr+ *(Eeprom_Mgn_Ctrl[BankId].Addr)), *(Eeprom_Mgn_Ctrl[BankId].Length),BankId))
			{
				Eeprom_BanK_Mng_Step[BankId] = EEPROM_BANK_WRITE_STEP_READ;
			}
			else
			{
				/*Write fail*/
				CddEeprom_Error_Handle();
			}
		break;
		
		case EEPROM_BANK_WRITE_STEP_READ:
			if(FL_PASS == SWEEPROM_Read(*(Eeprom_Mgn_Ctrl[BankId].Addr) , (uint16_t *)&Ram_Mirror_Chk_u16[BankId][*(Eeprom_Mgn_Ctrl[BankId].Addr)], *(Eeprom_Mgn_Ctrl[BankId].Length),BankId))
			{
				Eeprom_BanK_Mng_Step[BankId] = EEPROM_BANK_WRITE_STEP_CHECK;
			}
			else
			{
				/*Write fail*/
				CddEeprom_Error_Handle();
			}
		break;
		
		case EEPROM_BANK_WRITE_STEP_CHECK:
			if(0 != memcmp(Eeprom_Mgn_Ctrl[BankId].Ram_Mirror_Ptr + *(Eeprom_Mgn_Ctrl[BankId].Addr),&Ram_Mirror_Chk_u16[BankId][*Eeprom_Mgn_Ctrl[BankId].Addr],*Eeprom_Mgn_Ctrl[BankId].Length))
			{
				CddEeprom_Error_Handle();
			}
			else
			{
				/*Write Success*/
				if(PTR_NULL != (*Eeprom_Mgn_Ctrl[BankId].CallBack_Func))
				{
					(*Eeprom_Mgn_Ctrl[BankId].CallBack_Func)(TRUE);
				}
				Eeprom_BanK_Mng_Step[BankId] = EEPROM_BANK_WRITE_STEP_IDLE;
			}
			*(Eeprom_Mgn_Ctrl[BankId].Req_Write_Flag) = FALSE;
		break;
		
		default:
			Eeprom_BanK_Mng_Step[BankId] = EEPROM_BANK_WRITE_STEP_IDLE;
			*(Eeprom_Mgn_Ctrl[BankId].Req_Write_Flag) = FALSE;
		break;

	}

}



/*******************************************************************************
Name			: CddEeprom_Req_Write
Syntax			: void CddEeprom_Req_Write(uint8_t      BankId,uint16_t  Addr,uint16_t  Length,func_ptr CallBack)
return: TRUE->request success  FALSE->request fail
|******************************************************************************/

uint8_t CddEeprom_Req_Write(uint8_t      BankId,uint16_t  Addr,uint16_t  Length,func_ptr CallBack,uint16_t  * WriteBuffer)
{

    System_TaskMsg_t tMsg;

	if(BankId >= EEPROM_BANK_NUMBER)
	{
		return FALSE;
	}

	if(Length > Eeprom_Mgn_Ctrl[BankId].Max_Length)
	{
		return FALSE;
	}

	if(Addr > Eeprom_Mgn_Ctrl[BankId].Max_Length)
	{
		return FALSE;
	}

	if(Addr + Length > Eeprom_Mgn_Ctrl[BankId].Max_Length)
	{
		return FALSE;
	}
	/*busy*/
	if(TRUE == *(Eeprom_Mgn_Ctrl[BankId].Req_Write_Flag))
	{
		return FALSE;
	}
	
	/*out of range rad addr*/
	if(((uint32_t)WriteBuffer < RAM_START_ADDR) || ((uint32_t)WriteBuffer > RAM_END_ADDR)  || ((uint32_t)WriteBuffer + (uint32_t)(Length*2) > RAM_END_ADDR) )
	{
		return FALSE;
	}
	*(Eeprom_Mgn_Ctrl[BankId].Req_Write_Flag) = TRUE;
	*(Eeprom_Mgn_Ctrl[BankId].Addr) = Addr;
	*(Eeprom_Mgn_Ctrl[BankId].Length) = Length;
	*(Eeprom_Mgn_Ctrl[BankId].CallBack_Func) = CallBack;
	memcpy(Eeprom_Mgn_Ctrl[BankId].Ram_Mirror_Ptr+Addr,WriteBuffer,Length*2);
	
	tMsg.u16Event = SYSTEM_OS_EVENT_EEPROM_REQ;
	tMsg.u16Message = 0;
	System_SendMessage((OsTask_ID_t) DRIVERS_SAFE, &tMsg);
	
	return TRUE;
	
}


/*******************************************************************************
Name			: CddEeprom_Req_Read
Syntax			: void CddEeprom_Req_Read(uint8_t      BankId,uint16_t  Addr,uint16_t  Length,func_ptr CallBack)
return: TRUE->request success  FALSE->request fail
|******************************************************************************/
uint8_t CddEeprom_Req_Read(uint8_t      BankId,uint16_t  Addr,uint16_t  Length,uint16_t  * ReadBuffer)
{
	if(BankId >= EEPROM_BANK_NUMBER)
	{
		return FALSE;
	}

	if(Length > Eeprom_Mgn_Ctrl[BankId].Max_Length)
	{
		return FALSE;
	}

	if(Addr > Eeprom_Mgn_Ctrl[BankId].Max_Length)
	{
		return FALSE;
	}

	if(Addr + Length > Eeprom_Mgn_Ctrl[BankId].Max_Length)
	{
		return FALSE;
	}
	/*busy*/
	//if(TRUE == *(Eeprom_Mgn_Ctrl[BankId].Req_Write_Flag))
	{
		//return FALSE;
	}
	/*out of range rad addr*/
	if(((uint32_t)ReadBuffer < RAM_START_ADDR) || ((uint32_t)ReadBuffer > RAM_END_ADDR)  || (((uint32_t)ReadBuffer + (uint32_t)Length*2) > RAM_END_ADDR) )
	{
		return FALSE;
	}

	memcpy(ReadBuffer,Eeprom_Mgn_Ctrl[BankId].Ram_Mirror_Ptr+Addr,Length*2);

	return TRUE;


}


/*
True is condition ok
False is condition Nok
*/

uint8_t CddEeprom_Get_SleepCondition(void)
{
	uint8_t condition = TRUE;
	Eeprom_Bank_ID_e fl_bank_id ;
	
	for(fl_bank_id = EEPROM_BANK_MOTOR; fl_bank_id < EEPROM_BANK_NUMBER ; fl_bank_id ++)
	{
		if(Eeprom_BanK_Mng_Step[fl_bank_id] != EEPROM_BANK_WRITE_STEP_IDLE)
		{
			condition = FALSE;
			break;
		}
	}
	
	return condition;
}


/**********************************************************************************************************************
 *  END OF FILE
***********************************************************************************************************************/

/*EOF*/
