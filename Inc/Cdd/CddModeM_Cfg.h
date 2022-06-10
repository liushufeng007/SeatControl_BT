

#if !defined (__CDDMODEM_CFG_H__)
# define __CDDMODEM_CFG_H__

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"
/*******************************************************************************
|    Compile Option or configuration Section (for test/debug)
|******************************************************************************/

/*******************************************************************************
|    Macro Definition
|******************************************************************************/

#define CDDMODEM_TASK_PERIOD             5  /*unit:ms*/

#define CDDMODEM_DELAY_TIMER            (1000/CDDMODEM_TASK_PERIOD)





/*******************************************************************************
|    Enum Definition
|******************************************************************************/
	typedef enum
	{
		CDDMODEM_REQ_REQUEST = 0,
		CDDMODEM_REQ_RELEASE = 0xAAAA
	}CddModeM_Req_e;
	
	typedef enum
	{
		CDDMODEM_MAIN_POLLING = 0,
		CDDMODEM_MAIN_WRITE_EEPROM = 0x5555,
		CDDMODEM_MAIN_SLEEP = 0xAAAA
	}CddModeM_Main_e;

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/



/*******************************************************************************
|    Global Variable with extern linkage
|******************************************************************************/



/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/





#endif

/*EOF*/
