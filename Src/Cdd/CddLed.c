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

uint8_t CddLed_Req_State = FALSE;
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
	CddLed_Req_State = FALSE;
	
	Ioif_SetPinLevel(GPIO_NUMBER_B1_AtLamp_EN,FALSE);
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
	if(CddLed_Req_State == FALSE)
	{
		Ioif_SetPinLevel(GPIO_NUMBER_B1_AtLamp_EN,FALSE);
	}
	else
	{
		Ioif_SetPinLevel(GPIO_NUMBER_B1_AtLamp_EN,TRUE);
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
		CddLed_Req_State = TRUE;
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
