/*******************************************************************************
|  File Name:  CddLed.c
|  Description:  Implementation of the Swtich Management
|-------------------------------------------------------------------------------
|******************************************************************************/

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "CddLed.h"
#include "Ioif.h"
#include "CddEeprom.h"

uint16_t CddLed_Req_State = FALSE;
uint16_t CddLed_Req_State_Mirror = 0;
uint8_t  CddLed_update_Flag = TRUE;
/*******************************************************************************
ID				: 1
Name			: Init_CddLed
Syntax			: void Init_CddLed(void)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: None						:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: set key history 8bit value
Call By			: ECUM
|******************************************************************************/
void Init_CddLed(void)
{
	CddEeprom_Req_Read(EEPROM_BANK_APP,CDDLED_E2_ADDR,1,&CddLed_Req_State_Mirror);
	Ioif_SetPinLevel(GPIO_NUMBER_B1_AtLamp_EN,FALSE);
	CddLed_Req_State = CddLed_Req_State_Mirror;
}


/*0x12: 18*/
/*******************************************************************************
ID				: 2
Name			: CddLed_5ms_Task
Syntax			: void CddLed_5ms_Task(void)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: None						:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: check os task all
Call By			: OS_5ms
|******************************************************************************/
void CddLed_5ms_Task(void)
{
	if(CddLed_Req_State == 0xAA55)
	{
		Ioif_SetPinLevel(GPIO_NUMBER_B1_AtLamp_EN,TRUE);
	}
	else
	{
		Ioif_SetPinLevel(GPIO_NUMBER_B1_AtLamp_EN,FALSE);
	}

	if(CddLed_Req_State != CddLed_Req_State_Mirror)
	{
		CddLed_Req_State_Mirror = CddLed_Req_State;
		CddLed_update_Flag = FALSE;
	}

	if(CddLed_update_Flag == FALSE)
	{
		CddLed_update_Flag = CddEeprom_Req_Write(EEPROM_BANK_APP,CDDLED_E2_ADDR,1,0,&CddLed_Req_State_Mirror);
	}

#if 0
	__disable_irq();
	Ioif_SetPinLevel(GPIO_NUMBER_B1_AtLamp_EN,TRUE);
	ticks_delay = 106 ; /*50us*/
	while(ticks_delay -- );
	Ioif_SetPinLevel(GPIO_NUMBER_B1_AtLamp_EN,FALSE);
	__enable_irq();
#endif
}

/*******************************************************************************
ID				: 3
Name			: CddLed_Req
Syntax			: void CddLed_Req(uint8_t Req)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: None						:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: check os task all
Call By			: OS_5ms
|******************************************************************************/
void CddLed_Req(uint8_t Req)
{
	if(Req)
	{
		CddLed_Req_State = 0xAA55;
	}
	else
	{
		CddLed_Req_State = FALSE;
	}
}


void CddLed_Covert(void)
{
	if(CddLed_Req_State != 0xAA55)
	{
		CddLed_Req_State = 0xAA55;
	}
	else
	{
		CddLed_Req_State = FALSE;
	}
}


/**********************************************************************************************************************
 *  END OF FILE: CddLed.c
***********************************************************************************************************************/

/*EOF*/
