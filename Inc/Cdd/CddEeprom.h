

#if !defined (__CDDEEPROM_H__)
# define __CDDEEPROM_H__

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"
#include "CddEeprom_Cfg.h"
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
extern void CddEeprom_Init(void);
extern void CddEeprom_Task(void);
/*
bankId:
motor bank
app   bank
diag  bank
fbl   bank

Addr:software eeropm addr

Length:Length * uint16_t 

CallBack:active notifaction function

WriteBuffer: ram buffer

*/
extern uint8_t CddEeprom_Req_Write(uint8_t      BankId,uint16_t  Addr,uint16_t  Length,func_ptr CallBack,uint16_t  * WriteBuffer);
/*
bankId:
motor bank
app   bank
diag  bank
fbl   bank

Addr:software eeropm addr

Length:Length * uint16_t 

ReadBuffer: ram buffer

*/

extern uint8_t CddEeprom_Req_Read(uint8_t      BankId,uint16_t  Addr,uint16_t  Length,uint16_t  * ReadBuffer);



/*
True is condition ok
False is condition Nok
*/

extern uint8_t CddEeprom_Get_SleepCondition(void);


#endif

/*EOF*/
