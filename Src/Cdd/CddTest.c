/*******************************************************************************
|  File Name:  SwM.c
|  Description:  Implementation of the Swtich Management
|-------------------------------------------------------------------------------
|******************************************************************************/

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"
#include "CddTest.h"
#include "Ioif.h"
#include "Adcif.h"
#include "Canif.h"
#include "Eepromif.h"
#include "CddKey.h"
#include "CddEeprom.h"
#include "CddMtr_Mng.h"
#include "CanNmCtrl.h"
/*******************************************************************************
|    Macro Definition
|******************************************************************************/



/*******************************************************************************
|    Enum Definition
|******************************************************************************/

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/

/*******************************************************************************
|    Global variables Declaration
|******************************************************************************/


/*******************************************************************************
|    Static local variables Declaration
|******************************************************************************/



/*******************************************************************************
|    Table Definition
|******************************************************************************/


uint32_t g_Temp_U32 = 0;
uint16_t g_Temp_val = 0;
uint16_t g_Temp_ch = 0;
/* ============================================  Define  ============================================ */
#define TEST_WORDS  20

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

uint16_t g_testA[TEST_WORDS*2] = {0};/* Buffer for software eeprom read and write */
uint16_t g_testReadA[TEST_WORDS*2] = {0};
/* ====================================  Functions declaration  ===================================== */


void CddTest_init(void)
{
	g_Temp_U32 = 0;

}

void CddEeprom_Callback(uint8_t ret)
{
	static  uint8_t retry = 0;
    uint8_t tempData = 0,i;
	if(FALSE == ret)
	{
		if(retry++ < 3)
		{
			tempData =CddEeprom_Req_Write(EEPROM_BANK_MOTOR,0,TEST_WORDS,0,g_testA);
		}
		else
		{
			retry=0;
		}
	}
	else
	{
		retry=0;
		tempData =CddEeprom_Req_Read(EEPROM_BANK_MOTOR,0,TEST_WORDS,g_testReadA);
		 for(i = 0; i < TEST_WORDS; i++)
        {
            if(g_testA[i] != g_testReadA[i])
            {
                while (1);
            }
        } 
	}
	if(!tempData)
	{

	}
}

void CddEeprom_Test(void)
{

    uint8_t tempData = 0,i =0;
	uint8_t cycle= 0;

	cycle++;
	for(i = 0; i < TEST_WORDS*2; i++)
	{
		g_testA[i] = 0xAA00+i+cycle;  
	}
	

	tempData = CddEeprom_Req_Write((uint8_t)EEPROM_BANK_MOTOR,(uint16_t)0,(uint16_t)TEST_WORDS,CddEeprom_Callback,g_testA);
	if(!tempData)
	{

	}
	


}

void CddTest_Task(void)
{
	uint16_t fl_current_val = 0;
    //SWEEPROMDemoTest();
	//return;
    //CddEeprom_Test();
	//if(GET_KEY())
	{
	//	retval = Ioif_SetPinLevel(GPIO_NUMBER_32,0);
	}
	//else
	{
	//retval = Ioif_SetPinLevel(GPIO_NUMBER_32,1);
	}
	
    /*g_Temp_U32 = GetSWM_dw_JYHardwareKey();
	if(g_Temp_U32&0x400)
	{
		Ioif_SetPinLevel(GPIO_NUMBER_32_Seat3_F_SW_IO,0);
	}
	else
	{
		Ioif_SetPinLevel(GPIO_NUMBER_32_Seat3_F_SW_IO,1);
	}*/

	//fl_current_val = CddMtr_Get_Mtr_Current_Val(0);
	//fl_current_val = CddMtr_Get_Mtr_Current_Val(1);
	//g_Temp_U32 = 1;
	switch(g_Temp_U32)
	{
		case 0:
			
		break;

		case 1:
			CddMtr_Learn_Req(g_Temp_ch,CDDMTR_MNG_LEARN_ENABLE);
			//CddMtr_Learn_Req(1,CDDMTR_MNG_LEARN_ENABLE);
			g_Temp_U32 = 0;
		break;

		case 2:
			CddMtr_Learn_Req(g_Temp_ch,CDDMTR_MNG_LEARN_DISABLE);
			//CddMtr_Learn_Req(1,CDDMTR_MNG_LEARN_DISABLE);
			g_Temp_U32 = 0;
		break;

		case 3:
			CddMtr_Direction_Req(g_Temp_ch,CDDMTR_MNG_DIRECTION_FORWARD);
			g_Temp_U32 = 0;
		break;

		case 4:
			CddMtr_Direction_Req(g_Temp_ch,CDDMTR_MNG_DIRECTION_BACKWARD);
			g_Temp_U32 = 0;
		break;

		case 5:
			CddMtr_Direction_Req(g_Temp_ch,CDDMTR_MNG_DIRECTION_STOP);
			g_Temp_U32 = 0;
		break;

		case 6:
			CddMtr_Percent_Req(g_Temp_ch,g_Temp_val);
			g_Temp_U32 = 0;
		break;


		case 7:
			CddMtr_Learn_Req(g_Temp_ch,CDDMTR_MNG_LEARN_ENABLE);
			g_Temp_U32 = 0;
		break;

		case 8:
			CddMtr_Learn_Req(g_Temp_ch,CDDMTR_MNG_LEARN_DISABLE);
			g_Temp_U32 = 0;
		break;
		case 9:
			CddMtr_Direction_Req(g_Temp_ch,CDDMTR_MNG_DIRECTION_FORWARD);
			g_Temp_U32 = 0;
		break;

		case 10:
			CddMtr_Direction_Req(g_Temp_ch,CDDMTR_MNG_DIRECTION_BACKWARD);
			g_Temp_U32 = 0;
		break;

		case 11:
			CddMtr_Direction_Req(g_Temp_ch,CDDMTR_MNG_DIRECTION_STOP);
			g_Temp_U32 = 0;
		break;

		case 12:
			CddMtr_Percent_Req(g_Temp_ch,g_Temp_val);
			g_Temp_U32 = 0;
		break;
		
		case 155:
			CddMtr_Percent_Req(g_Temp_ch,fl_current_val);
			g_Temp_U32 = 0;
		break;

		case 13:
			CanNmCtrl_Req_NetWork(0);g_Temp_U32 = 0;
		break;
		case 14:
			CanNmCtrl_Release_NetWork(0);g_Temp_U32 = 0;
		break;
		case 15:
			CddEeprom_Test();g_Temp_U32 = 0;
		break;
		default:
		break;
	}


}


/**********************************************************************************************************************
 *  END OF FILE
***********************************************************************************************************************/

/*EOF*/
