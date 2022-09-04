
#if !defined (__CDDMTR_MNG_H__)
# define __CDDMTR_MNG_H__

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "CddMtr_Ctrl.h"
#include "user_init.h"

/*******************************************************************************
|    Compile Option or configuration Section (for test/debug)
|******************************************************************************/

/*******************************************************************************
|    Macro Definition
|******************************************************************************/
#define CDDMTR_MNG_TASK_PERIOD          5  /*unit:ms*/

#define CDDMTR_MNG_DIAG_HALL_MIN_STEP   4
#define CDDMTR_MNG_DELAY_50MS           (50/CDDMTR_MNG_TASK_PERIOD)
#define CDDMTR_MNG_DELAY_300MS          (300/CDDMTR_MNG_TASK_PERIOD)
#define CDDMTR_MNG_DELAY_1000MS         (1000/CDDMTR_MNG_TASK_PERIOD)
#define CDDMTR_MNG_DELAY_1500MS         (1500/CDDMTR_MNG_TASK_PERIOD)
#define CDDMTR_MNG_DELAY_30S            (300000/CDDMTR_MNG_TASK_PERIOD)


#define CDDMTR_MNG_MAX_PERCENT          (10000)
#define CDDMTR_MNG_PERCENT_INVALID      (0xFFFF)

#define CDDMTR_MNG_MIN_MOVE_STEP        (50)


#define CDDMTR_MNG_LERAN_SUCCESS        (0xA5A5)
#define CDDMTR_MNG_LERAN_FAIL           (0x5A5A)

#define CDDMTR_MNG_HALL_FAULT_MIN_STEP  (4)

#define CDDMTR_MNG_MAX_STEP_VAL            (0xFFFF)
#define CDDMTR_MNG_CAL_MAXSTEP_MAXVAL      (60000)
#define CDDMTR_MNG_CAL_MAXSTEP_MINVAL      (300)

#define CDDMTR_MNG_MAIN_E2_ADDR         (0)

#define CDDMTR_MNG_7V_AD_VAL            (550)
#define CDDMTR_MNG_8V_AD_VAL            (616)
#define CDDMTR_MNG_9V_AD_VAL            (732)
#define CDDMTR_MNG_STOP_VOL             (7000)
#define CDDMTR_MNG_START_VOL             (8000)
#define CDDMTR_MNG_VOL_LVI_FILTER            (20)
#define CDDMTR_MNG_VOL_NORMAL_FILTER         (100)
#define CDDMTR_SHUTDOWN_WRITE_NVM_MOTOR_NUM  (CDDMTR_HFKF_MAX_NUM)
#define CDDMTR_DEBUG_MTR_LEARN          TRUE
/*******************************************************************************
|    Enum Definition
|******************************************************************************/
typedef enum
{
	CDDMTR_MNG_LEARN_NULL = 0,
	CDDMTR_MNG_LEARN_DISABLE = 0x5555 ,
	CDDMTR_MNG_LEARN_ENABLE=0xAAAA
}CddMtr_Mng_Learn_e;

typedef enum
{
	CDDMTR_MNG_LEARNDATA_INVALID = (uint16_t)0,
	CDDMTR_MNG_LEARN_VALID = (uint16_t)0xA5A5
}CddMtr_Mng_LearnData_e;

typedef enum
{
	CDDMTR_MNG_DIRECTION_NULL=0,
	CDDMTR_MNG_DIRECTION_STOP=0x5555,
	CDDMTR_MNG_DIRECTION_FORWARD=0x5AA5 ,
	CDDMTR_MNG_DIRECTION_BACKWARD=0xAAAA
}CddMtr_Mng_Run_Direction_e;

typedef enum
{
	CDDMTR_MNG_HALL_NORMAL = 0,
	CDDMTR_MNG_HALL_FAULT  = 0x5555,
	CDDMTR_MNG_HALL_SHORTGND   = 0xAA55,
	CDDMTR_MNG_HALL_SHORTBAT_OPEN   = 0xAAAA,
}CddMtr_Mng_Hall_Status_e;

typedef enum
{
	CDDMTR_MNG_NO_STUCK = 0,
	CDDMTR_MNG_STUCK_FORWARD=0x5555,
	CDDMTR_MNG_STUCK_BACKWARD=0xAAAA
}CddMtr_Mng_Stuck_Status_e;

typedef enum
{
	CDDMTR_MNG_START = 0,
	CDDMTR_MNG_HIGH=0x5555,
	CDDMTR_MNG_LOW =0xAAAA
}CddMtr_Mng_DiagStep_Status_e;
typedef enum
{
	CDDMTR_MNG_VOL_LVI = 0,
	CDDMTR_MNG_VOL_NORMAL=0x5555
}CddMtr_Mng_BatVol_Status_e;
typedef enum
{
	CDDMTR_MNG_STATUS_IDLE = 0,
	CDDMTR_MNG_STATUS_STOP,
	CDDMTR_MNG_STATUS_LEARN_1_DELAY,
	CDDMTR_MNG_STATUS_LEARN_5_BACKWARD,
	CDDMTR_MNG_STATUS_LEARN_6_DELAY,
	CDDMTR_MNG_STATUS_LEARN_7_FORWARD,
	CDDMTR_MNG_STATUS_LEARN_8_FAIL,
	CDDMTR_MNG_STATUS_DIRECT_1_START,
	CDDMTR_MNG_STATUS_DIRECT_2_FORWARD,
	CDDMTR_MNG_STATUS_DIRECT_3_BACKWARD,
	CDDMTR_MNG_STATUS_PERCENT_1_DELAY,
	CDDMTR_MNG_STATUS_PERCENT_5_HALLDIAG_FORWARD,
	CDDMTR_MNG_STATUS_PERCENT_10_HALLDIAG_BACKWARD,
	CDDMTR_MNG_STATUS_MAX
}CddMtr_Mng_Main_Status_e;

typedef CddMtr_Mng_Main_Status_e (* State_Handle_Ptr)(uint8_t fl_Mtr_Id);
/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef struct
{
	CddMtr_Mng_Run_Direction_e Req_Direction_Queue[3];
	CddMtr_Mng_Run_Direction_e Req_Direction;
	CddMtr_Mng_Learn_e		   Learn_Req_Queue[3];
	CddMtr_Mng_Hall_Status_e   Hall_Status;
	CddMtr_Mng_Stuck_Status_e  Stcuk_Status;
	CddMtr_Mng_LearnData_e	   LearnData_Status;
	CddMtr_Mng_Main_Status_e   Main_Ctrl_Status;
	CddMtr_Mng_Main_Status_e   Last_Main_Ctrl_Status;
	CddMtr_Mng_Run_Direction_e Run_Direct;
	CddMtr_Mng_Learn_e         Learn_Req_State;
	CddMtr_Mng_Run_Direction_e Direction_Req_State;
	uint16_t AdVal[4];
	uint16_t Mng_Delay_Tick;
	uint16_t Hall_Diag_Tick;
	uint16_t Hall_Diag_CancelTick;
	uint16_t Stuck_Diag_Comfirm_Tick;
	uint16_t Stuck_Diag_Cancel_Tick;
	CddMtr_Mng_DiagStep_Status_e Diag_Start;
	uint16_t Req_Percent_Queue[3];
	uint16_t Target_Step;
	uint16_t Current_Step;
	uint16_t Cal_Max_Step;
}CddMtr_Mng_Str;


typedef struct
{
	uint16_t High_Threshold_Ad;
	uint16_t Low_Threshold_Ad;
	uint16_t OverCurrent_High_Threshold_Ad;	
	uint16_t OverCurrent_Low_Threshold_Ad;
	uint16_t Hall_Low_Threshold_Ad;
}CddMtr_Mng_Threshold_Ad_Str;
typedef struct
{
CddMtr_Mng_BatVol_Status_e LastState;
CddMtr_Mng_BatVol_Status_e CurrentState;
uint8_t   LviCnt;
uint8_t   NormalCnt;
}CddMtr_Mng_BatMode_Str;
/*******************************************************************************
|    Global Variable with extern linkage
|******************************************************************************/


/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
extern void CddMtr_Mng_Init(void);
extern void CddMtr_Mng_Task(void);
extern uint8_t CddMtr_Get_SleepCondition(void);
extern uint8_t CddMtr_Write_To_EEPROM(void);
extern void CddMtr_Mng_Motor_Step(uint8_t fl_Mtr_Id,uint16_t fl_Ad_Val);

extern uint8_t CddMtr_Learn_Req(uint8_t fl_Mtr_Id,CddMtr_Mng_Learn_e fl_learn_status );
extern uint8_t CddMtr_Direction_Req(uint8_t fl_Mtr_Id,CddMtr_Mng_Run_Direction_e fl_Ctrl_Direct );
extern uint8_t CddMtr_Percent_Req(uint8_t fl_Mtr_Id,uint32_t fl_percent_u32 );


extern CddMtr_Mng_Hall_Status_e CddMtr_Get_Hall_Fault_Status(uint8_t fl_Mtr_Id);
extern CddMtr_Mng_LearnData_e   CddMtr_Get_LearnData_Status(uint8_t fl_Mtr_Id);

extern uint16_t CddMtr_Get_Current_Hall_Cnt(uint8_t fl_Mtr_Id);
extern uint16_t CddMtr_Get_Mtr_Current_Val(uint8_t fl_Mtr_Id);
extern CddMtr_Ctrl_Req_e CddMtr_Get_Mtr_Run_Direction(uint8_t fl_Mtr_Id);
extern uint16_t CddMtr_Get_Mtr_PosPercent(uint8_t fl_Mtr_Id);

extern uint8_t CddMtr_Get_SleepCondition(void);
extern 	uint8_t CddMtr_Get_Mtr_Learning_Status(uint8_t fl_Mtr_Id);
extern 	uint8_t CddMtr_Get_Mtr_Run_Status(uint8_t fl_Mtr_Id);

#endif

/*EOF*/
