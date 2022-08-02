/*******************************************************************************
|  File Name:  CddModeM.c
|  Description:  Implementation of the PowerMode Management
|-------------------------------------------------------------------------------
|******************************************************************************/

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"
#include "CddModeM.h"

#include "Iwdt.h"
#include "Adcif.h"
#include "Canif.h"
#include "Compif.h"
#include "Ioif.h"


#include "CddEeprom.h"
#include "CddMtr_Mng.h"
#include "CddKey.h"

#include "fm33lg0xx_fl_rmu.h"

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



CddModeM_Req_e  CddModeM_Req_State ;
CddModeM_Main_e CddModeM_Main_State;
uint16_t        CddModeM_IncreaseCnt;

/*******************************************************************************
|    Table Definition
|******************************************************************************/

/*******************************************************************************
|    Static Function Definition
|******************************************************************************/

extern void prvStopTickInterruptTimer ( void );
extern void prvStartTickInterruptTimer ( void );
extern void UserInit(void);
extern void SystemInit(void);

void DeepSleep(void);
void Sleep(void);

/*******************************************************************************
Name			: CddModeM_SleepInit
Syntax			: void CddModeM_SleepInit(void)
|******************************************************************************/
void CddModeM_SleepInit(void)
{
	SystemInit();
    MF_Clock_Init();
    FL_Init();
    MF_SystemClock_Config();
	
	/*CddEeprom_Init();*/
	Ioif_GPIO_Init();
	Adcif_Init();
	Compif_Init();
	Canif_Init();
	
	/*Init_CddKey();*/
	/*CddMtr_Mng_Init();*/
	IWDT_Init();
	/*CddModeM_Init();*/
	/*CddTest_init();*/
	
    /*PowerModeInit();
    Message_variable_init();
    Scm_SeatControl_Init();*/

	CanNm_Init();
}


/*******************************************************************************
Name			: CddModeM_DeInit
Syntax			: void CddModeM_DeInit(void)
|******************************************************************************/


void CddModeM_DeInit(void)
{
	CddModeM_Req_State = CDDMODEM_REQ_RELEASE;
	CddModeM_Main_State = CDDMODEM_MAIN_POLLING;
	CddModeM_IncreaseCnt = 0;

	__disable_irq();
	Ioif_ClearAll_WakeupSrc();
	IWDT_DeInit();
	Compif_DeInit();
	Adcif_DeInit();
	Canif_DeInit();
	Ioif_GPIO_DeInit();
	
	prvStopTickInterruptTimer();
	__enable_irq();
	
	if(Ioif_GetPinLevel(GPIO_NUMBER_D5_IGN_INT) == FALSE)
	{
	DeepSleep();
	}
	
	CddModeM_SleepInit();

	prvStartTickInterruptTimer();
}


/*******************************************************************************
Name			: v
Syntax			: void CddModeM_Init(void)
|******************************************************************************/

void CddModeM_Init(void)
{
	uint32_t fl_temp_u32 = FALSE;
	uint8_t  fl_temp_u8 = FALSE;
	
	CddModeM_Req_State = CDDMODEM_REQ_RELEASE;
	CddModeM_Main_State = CDDMODEM_MAIN_POLLING;
	CddModeM_IncreaseCnt = 0;

	fl_temp_u32 = FL_RMU_IsActiveFlag_PORN(RMU);
	if(TRUE == fl_temp_u32)
	{
		fl_temp_u8 = TRUE;
	}

	fl_temp_u32 = FL_RMU_IsActiveFlag_PDRN(RMU);
	if(TRUE == fl_temp_u32)
	{
		fl_temp_u8 = TRUE;
	}

	fl_temp_u32 = FL_RMU_IsActiveFlag_NRSTN(RMU);
	if(TRUE == fl_temp_u32)
	{
		fl_temp_u8 = TRUE;
	}

	FL_RMU_ClearFlag_MDF(RMU);
	FL_RMU_ClearFlag_NRSTN(RMU);
	FL_RMU_ClearFlag_PRCN(RMU);
	FL_RMU_ClearFlag_PORN(RMU);
	FL_RMU_ClearFlag_PDRN(RMU);
	
	FL_RMU_ClearFlag_SOFTN(RMU);
	FL_RMU_ClearFlag_IWDTN(RMU);
	FL_RMU_ClearFlag_WWDTN(RMU);
	FL_RMU_ClearFlag_LKUPN(RMU);
	FL_RMU_ClearFlag_NVICN(RMU);
	
	if(fl_temp_u8 == TRUE)	
	{
		Ioif_ClearAll_WakeupSrc();
	}
}

/*******************************************************************************
Name			: CddEeprom_Task
Syntax			: void CddEeprom_Task(void)
|******************************************************************************/
void CddModeM_Task(void)
{
	uint8_t EEPROM_Condition = FALSE;
	uint8_t CanNm_Condition = FALSE;
	uint8_t MtrMng_Condition = FALSE;
	uint8_t IGN_Condition = FALSE;
	
	EEPROM_Condition = CddEeprom_Get_SleepCondition();
	MtrMng_Condition = CddMtr_Get_SleepCondition();
	if(BusSleep_STATE_E_0 == CanNmCtrl_Get_CanNm_State(0))
	{
		CanNm_Condition = TRUE;
	}
	
	if(Ioif_GetPinLevel(GPIO_NUMBER_D5_IGN_INT) == FALSE)
	{
		IGN_Condition = TRUE;
	}

	switch(CddModeM_Main_State)
	{
	
		case CDDMODEM_MAIN_POLLING:
			if((TRUE == CanNm_Condition)  && \
			(TRUE == IGN_Condition)  && \
			(CDDMODEM_REQ_RELEASE == CddModeM_Req_State))
			{
				CddModeM_Main_State = CDDMODEM_MAIN_WRITE_EEPROM;
			}
			CddModeM_IncreaseCnt = 0;
		break;

		case CDDMODEM_MAIN_WRITE_EEPROM:
			if((TRUE == MtrMng_Condition) && \
			(TRUE == CanNm_Condition)  && \
			(TRUE == IGN_Condition)  && \
			(CDDMODEM_REQ_RELEASE == CddModeM_Req_State))
			{
				if(TRUE ==CddMtr_Write_To_EEPROM())
				{
				CddModeM_Main_State = CDDMODEM_MAIN_SLEEP;
				CddModeM_IncreaseCnt = 0;
				}
				else
				{
					if(CDDMODEM_DELAY_TIMER > CddModeM_IncreaseCnt)
					{
						CddModeM_IncreaseCnt ++;
					}
					else
					{
						CddModeM_Main_State = CDDMODEM_MAIN_SLEEP;
						CddModeM_IncreaseCnt = 0;
					}
				}
			}
			else
			{
				CddModeM_Main_State = CDDMODEM_MAIN_POLLING;
			}
		break;

		case CDDMODEM_MAIN_SLEEP:
			if(CDDMODEM_DELAY_TIMER > CddModeM_IncreaseCnt)
			{
				CddModeM_IncreaseCnt ++;
			}
			else
			{
				if((TRUE == EEPROM_Condition) && \
				(TRUE == MtrMng_Condition) && \
				(TRUE == CanNm_Condition)  && \
				(TRUE == IGN_Condition)  && \
				(CDDMODEM_REQ_RELEASE == CddModeM_Req_State))
				{
					CddModeM_DeInit();
				}
				else
				{
					CddModeM_Main_State = CDDMODEM_MAIN_POLLING;
				}
			}

		break;
		
		default:
			CddModeM_Main_State = CDDMODEM_MAIN_POLLING;
		break;
	}
}

/*******************************************************************************
Name			: CddModeM_Req_Normal
Syntax			: void CddModeM_Req_Normal(CddModeM_Req_e req)
param           : req -> request mcu sleep 
|******************************************************************************/
void CddModeM_Req_Normal(CddModeM_Req_e req)
{
	if(CDDMODEM_REQ_REQUEST == req)
	{
		CddModeM_Req_State = CDDMODEM_REQ_REQUEST;
	}
	else
	{
		CddModeM_Req_State = CDDMODEM_REQ_RELEASE;
	}
}

/*******************************************************************************
Name			: Sleep
Syntax			: void Sleep(void)
param           : void
|******************************************************************************/
void Sleep(void)
{
    FL_PMU_SleepInitTypeDef LPM_InitStruct;
  
    //  FL_CMU_RCLF_Enable();               /* 暂开启RCLF */
    FL_RMU_PDR_Enable(RMU);                 /* 打开PDR */
    FL_RMU_BOR_Disable(RMU);                /* 关闭BOR 2uA */

    FL_VREF_Disable(VREF);                  /* 关闭VREF1p2 */

    LPM_InitStruct.deepSleep            = FL_PMU_SLEEP_MODE_NORMAL;
    LPM_InitStruct.powerMode            = FL_PMU_POWER_MODE_SLEEP_OR_DEEPSLEEP;
    LPM_InitStruct.wakeupFrequency      = FL_PMU_RCHF_WAKEUP_FREQ_8MHZ;
    LPM_InitStruct.wakeupDelay          = FL_PMU_WAKEUP_DELAY_2US;
    LPM_InitStruct.LDOLowPowerMode      = FL_PMU_LDO_LPM_DISABLE;
    LPM_InitStruct.coreVoltageScaling   = FL_DISABLE;
    FL_PMU_Sleep_Init(PMU, &LPM_InitStruct);
    FL_RMU_BOR_Enable(RMU);                 /* 使能BOR */
}

/*******************************************************************************
Name			: DeepSleep
Syntax			: void DeepSleep(void)
param           : void
|******************************************************************************/
void DeepSleep(void)
{
    FL_PMU_SleepInitTypeDef LPM_InitStruct;
  
    //  FL_CMU_RCLF_Enable();               /* 暂开启RCLF */
    FL_RMU_PDR_Enable(RMU);                 /* 打开PDR */
    FL_RMU_BOR_Disable(RMU);                /* 关闭BOR 2uA */

    FL_VREF_Disable(VREF);                  /* 关闭VREF1p2 */

    LPM_InitStruct.deepSleep            = FL_PMU_SLEEP_MODE_DEEP;
    LPM_InitStruct.powerMode            = FL_PMU_POWER_MODE_SLEEP_OR_DEEPSLEEP;
    LPM_InitStruct.wakeupFrequency      = FL_PMU_RCHF_WAKEUP_FREQ_8MHZ;
    LPM_InitStruct.wakeupDelay          = FL_PMU_WAKEUP_DELAY_2US;
    LPM_InitStruct.LDOLowPowerMode    = FL_PMU_LDO_LPM_DISABLE;
    LPM_InitStruct.coreVoltageScaling   = FL_DISABLE;
    FL_PMU_Sleep_Init(PMU, &LPM_InitStruct);    
    FL_RMU_BOR_Enable(RMU);                /* 使能BOR */
}



/**********************************************************************************************************************
 *  END OF FILE
***********************************************************************************************************************/

/*EOF*/
