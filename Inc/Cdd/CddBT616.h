

#if !defined (__CDDBT616_H__)
# define __CDDBT616_H__

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"
#include "uartif.h"
/*******************************************************************************
|    Compile Option or configuration Section (for test/debug)
|******************************************************************************/

/*******************************************************************************
|    Macro Definition
|******************************************************************************/

#define CDDBT616_TASK_PERIOD          10  /*unit:ms*/

#define CDDBT616_TX_PERIOD            (500/UARTIF_TASK_PERIOD)


#define  BT_Connected()   (Ioif_GetPinLevel(GPIO_NUMBER_C13_BLE_Tran)== FALSE)


#define ID_OFFSET  7
/*******************************************************************************
|    Enum Definition
|******************************************************************************/
typedef enum
{
	BT616_WORK_TEST_TX = 0,
	BT616_WORK_TEST_RX ,
	BT616_WORK_READ_NAME ,
	BT616_WORK_CHECK_NAME ,
	BT616_WORK_WRITE_NAME ,
	BT616_WORK_W_CHECK_NAME ,
	BT616_WORK_CHECK_CONNECT,
	BT616_WORK_COM
}CddBT616_Work_State_e;
	
typedef enum
{
	BT616_FRONT_REAR = 0,
	BT616_ANGLE_BACK ,
	BT616_RATOTE ,
	BT616_LEG ,
	BT616_HEAD ,
	BT616_TOTAL_ANGLE ,
	BT616_VENT,
	BT616_MODE,
	BT616_MASSAGE,
	MAX_ID
}CddBT616_Signal_Id_e;



/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef struct
{
	CddBT616_Work_State_e State;
	Uartif_Msg_Str        txst;
	uint16_t              Txticks;
}CddBT616_Main_Ctrl_Str;

/*******************************************************************************
|    Global Variable with extern linkage
|******************************************************************************/


/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
void CddBT616_init(void);

void CddBT616_Task(void);


void CddBT616_Set_Signal(CddBT616_Signal_Id_e Id,uint8_t val);


#endif

/*EOF*/
