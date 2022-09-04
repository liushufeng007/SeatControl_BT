/*******************************************************************************
|  File Name:  SwM.h
|  Description:  Implementation of the Swtich Management
|-------------------------------------------------------------------------------
|
|******************************************************************************/

#if !defined (CDDLED_H_)
# define CDDLED_H_

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "User_Init.h"
/*******************************************************************************
|    Compile Option or configuration Section (for test/debug)
|******************************************************************************/

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
|    Global Variable with extern linkage
|******************************************************************************/


/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
/*ECUM*/
extern void Init_CddLed(void);

/*OS*/
extern void CddLed_5ms_Task(void);

extern void CddLed_Req(uint8_t Req);

extern void CddLed_Covert(void);

extern uint8_t CddLed_Get_Req(void);
#endif

/*EOF*/
