

#if !defined (__CDDEEPROM_CFG_H__)
# define __CDDEEPROM_CFG_H__

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"
#include "Eepromif.h"
/*******************************************************************************
|    Compile Option or configuration Section (for test/debug)
|******************************************************************************/

/*******************************************************************************
|    Macro Definition
|******************************************************************************/
#define PTR_NULL   (0)
/*******************************************************************************
|    Enum Definition
|******************************************************************************/

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef enum
{
	EEPROM_BANK_WRITE_STEP_IDLE = 0,
	EEPROM_BANK_WRITE_STEP_WRITE = 1,
	EEPROM_BANK_WRITE_STEP_READ = 2,
	EEPROM_BANK_WRITE_STEP_CHECK = 3,
}Eeprom_Bank_Mgn_e;

typedef void (*func_ptr)(uint8_t req_result);

typedef struct
{
	uint8_t  *  Req_Write_Flag;
	uint8_t     BankId;
	uint16_t    Max_Length;
	uint16_t  * Addr;
	uint16_t *  Length;
	uint16_t *  Ram_Mirror_Ptr;
	func_ptr *  CallBack_Func;
	
}Eeprom_Mgn_Str;




/*******************************************************************************
|    Global Variable with extern linkage
|******************************************************************************/

extern const Eeprom_Mgn_Str Eeprom_Mgn_Ctrl[EEPROM_BANK_NUMBER] ;
extern Eeprom_Bank_Mgn_e	Eeprom_BanK_Mng_Step[EEPROM_BANK_NUMBER];
extern uint8_t  	Eeprom_Req_W_Flag_u8[EEPROM_BANK_NUMBER];
extern uint16_t  	Eeprom_Req_Addr_u16[EEPROM_BANK_NUMBER];
extern uint16_t  	Eeprom_Req_Len_u16[EEPROM_BANK_NUMBER];
extern func_ptr     Eeprom_Callback_Arr[EEPROM_BANK_NUMBER];
extern uint16_t     Ram_Mirror_Chk_u16[EEPROM_BANK_NUMBER][SWEEPROM_SIZE_FBL];

/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/





#endif

/*EOF*/
