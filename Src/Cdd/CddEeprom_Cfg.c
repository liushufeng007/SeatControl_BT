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
Eeprom_Bank_Mgn_e 	Eeprom_BanK_Mng_Step[EEPROM_BANK_NUMBER];

func_ptr    Eeprom_Callback_Arr[EEPROM_BANK_NUMBER];

uint8_t  	Eeprom_Req_W_Flag_u8[EEPROM_BANK_NUMBER];
uint16_t  	Eeprom_Req_Addr_u16[EEPROM_BANK_NUMBER];
uint16_t  	Eeprom_Req_Len_u16[EEPROM_BANK_NUMBER];

uint16_t    Ram_Mirror_Motor_u16[SWEEPROM_SIZE_MOTOR];
uint16_t    Ram_Mirror_App_u16[SWEEPROM_SIZE_APP];
uint16_t    Ram_Mirror_Diag_u16[SWEEPROM_SIZE_DIAG];
uint16_t    Ram_Mirror_Fbl_u16[SWEEPROM_SIZE_FBL];
uint16_t    Ram_Mirror_Chk_u16[EEPROM_BANK_NUMBER][SWEEPROM_SIZE_FBL];


  
 
  

/*******************************************************************************
|    Table Definition
|******************************************************************************/

const Eeprom_Mgn_Str Eeprom_Mgn_Ctrl[EEPROM_BANK_NUMBER] = 
{
	&Eeprom_Req_W_Flag_u8[EEPROM_BANK_MOTOR]   ,EEPROM_BANK_MOTOR  ,SWEEPROM_SIZE_MOTOR -3 ,&Eeprom_Req_Addr_u16[EEPROM_BANK_MOTOR]  ,&Eeprom_Req_Len_u16[EEPROM_BANK_MOTOR],&Ram_Mirror_Motor_u16[0]    ,    &Eeprom_Callback_Arr[EEPROM_BANK_MOTOR],
	&Eeprom_Req_W_Flag_u8[EEPROM_BANK_APP  ]   ,EEPROM_BANK_APP    ,SWEEPROM_SIZE_APP   -3 ,&Eeprom_Req_Addr_u16[EEPROM_BANK_APP  ]  ,&Eeprom_Req_Len_u16[EEPROM_BANK_APP  ],&Ram_Mirror_App_u16[0]      ,    &Eeprom_Callback_Arr[EEPROM_BANK_APP],  
	&Eeprom_Req_W_Flag_u8[EEPROM_BANK_DIAG ]   ,EEPROM_BANK_DIAG   ,SWEEPROM_SIZE_DIAG  -3 ,&Eeprom_Req_Addr_u16[EEPROM_BANK_DIAG ]  ,&Eeprom_Req_Len_u16[EEPROM_BANK_DIAG ],&Ram_Mirror_Diag_u16[0]     ,    &Eeprom_Callback_Arr[EEPROM_BANK_DIAG],
	&Eeprom_Req_W_Flag_u8[EEPROM_BANK_FBL  ]   ,EEPROM_BANK_FBL    ,SWEEPROM_SIZE_FBL   -3 ,&Eeprom_Req_Addr_u16[EEPROM_BANK_FBL  ]  ,&Eeprom_Req_Len_u16[EEPROM_BANK_FBL  ],&Ram_Mirror_Fbl_u16[0]      ,    &Eeprom_Callback_Arr[EEPROM_BANK_FBL]
};



/**********************************************************************************************************************
 *  END OF FILE
***********************************************************************************************************************/

/*EOF*/
