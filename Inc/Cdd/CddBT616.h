

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


#define  BT_Connected()   0

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
	BT616_NONE_KEY = 0,
	BT616_NONE_FORWARD = 'F',
	BT616_NONE_BACKWARD= 'B',
	BT616_NONE_STOP= 'S',
}CddBT616_KEY_VAL_e;


/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef struct
{
	CddBT616_Work_State_e State;
	Uartif_Msg_Str        txst;
	uint16_t              Txticks;
	CddBT616_KEY_VAL_e    Button[3];
}CddBT616_Main_Ctrl_Str;

/*******************************************************************************
|    Global Variable with extern linkage
|******************************************************************************/


/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
void CddBT616_init(void);

void CddBT616_Task(void);

CddBT616_KEY_VAL_e CddBT616_Pop_Button(void);

void CddBT616_Set_Pos(uint8_t MotorId,uint8_t Pos);;
void CddBT616_Set_BatVol(uint8_t bat1_vol,uint8_t bat2_vol);
void CddBT616_Set_HallFault(uint8_t MotorId,uint8_t fault);
void CddBT616_Set_LearnFlag(uint8_t MotorId,uint8_t Learn);

#endif

/*EOF*/
