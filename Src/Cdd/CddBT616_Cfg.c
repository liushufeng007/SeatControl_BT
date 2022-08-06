/*******************************************************************************
|  File Name:  CddBT616_Cfg.c
|  Description:  Implementation of the Swtich Management
|-------------------------------------------------------------------------------
|******************************************************************************/

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"
#include "uartif.h"
#include "CddBT616_Cfg.h"
#include "CddBT616.h"
#include "il_par.h"

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

const uint8_t BT616_Txcmd_AT[] = {'A','T',0xD,0xA};
const uint8_t BT616_Rxcmd_AT[] = {0xD,0xA,'O','K',0xD,0xA};

const CddBT616_cmd_Str CddBT616_cmd_AT =
{
	(uint8_t *)BT616_Txcmd_AT,
	(uint8_t *)BT616_Rxcmd_AT,
	sizeof(BT616_Txcmd_AT)/sizeof(uint8_t),
	sizeof(BT616_Rxcmd_AT)/sizeof(uint8_t)
};
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)

const uint8_t BT616_Txcmd_Name[] = {'A','T','+','N','A','M','E',0xD,0xA};
const uint8_t BT616_Rxcmd_Name[] = {0xD,0xA,'+','N','A','M','E','=','J','U','N','Y','I','-','-','R',0xD,0xA,0xD,0xA,'O','K',0xD,0xA};

#else

const uint8_t BT616_Txcmd_Name[] = {'A','T','+','N','A','M','E',0xD,0xA};
const uint8_t BT616_Rxcmd_Name[] = {0xD,0xA,'+','N','A','M','E','=','J','U','N','Y','I','-','-','L',0xD,0xA,0xD,0xA,'O','K',0xD,0xA};

#endif


const CddBT616_cmd_Str CddBT616_cmd_Name =
{
	(uint8_t *)BT616_Txcmd_Name,
	(uint8_t *)BT616_Rxcmd_Name,
	sizeof(BT616_Txcmd_Name)/sizeof(uint8_t),
	sizeof(BT616_Rxcmd_Name)/sizeof(uint8_t)
};
	
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)

const uint8_t BT616_Txcmd_WName[] = {'A','T','+','N','A','M','E','=','J','U','N','Y','I','-','-','R',0xD,0xA};
const uint8_t BT616_Rxcmd_WName[] = {0xD,0xA,'A','T',0xD,0xA};

#else

const uint8_t BT616_Txcmd_WName[] = {'A','T','+','N','A','M','E','=','J','U','N','Y','I','-','-','L',0xD,0xA};
const uint8_t BT616_Rxcmd_WName[] = {0xD,0xA,'A','T',0xD,0xA};

#endif

const CddBT616_cmd_Str CddBT616_cmd_WName =
{
	(uint8_t *)BT616_Txcmd_WName,
	(uint8_t *)BT616_Rxcmd_WName,
	sizeof(BT616_Txcmd_WName)/sizeof(uint8_t),
	sizeof(BT616_Rxcmd_WName)/sizeof(uint8_t)
};



const CddBT616_cmd_Str * CddBT616_Cmd_Table[] =
{
	&CddBT616_cmd_AT,
	&CddBT616_cmd_Name,
	&CddBT616_cmd_WName,
};


  

/*******************************************************************************
|    Table Definition
|******************************************************************************/



/**********************************************************************************************************************
 *  END OF FILE
***********************************************************************************************************************/

/*EOF*/
