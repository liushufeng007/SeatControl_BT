#include "user_init.h"
#include "CddMtr_HFKF.h"
#include "public.h"
#include "ButtonCtrl.h"
#include "CddMtr_Mng.h"
#include "CddLed.h"

#include "CddEeprom.h"
#include "lin.h"
#include "il.h"

#include "il_par.h"
#include "crc.h"

#define MODE_E2_LEN  4

ButtonCtrl_Queue_str ButtonCtrl_queue;

ButtonCtrl_Req_Str ButtonCtrl_Req;
ButtonCtrl_Mode_Req_Str ButtonCtrl_PreReq;
ButtonCtrl_Mode_Req_Str ButtonCtrl_LastReq;
ButtonCtrl_Mode_Req_Str ButtonCtrl_ExitReq;

uint8_t StopBtn_Cnt[5];
uint16_t Button_Mode[MODE_E2_LEN+1] = {0};


uint8_t Button_Update_Flag = TRUE;
uint8_t ButtonMode_Update_Flag[8] ;

ButtonCtrl_Id_MtrCal_e ButtonCtrl_Mtr_Calbration_State = BTN_ID_CTRL_CAL_IDLE;
uint8_t  ButtonCtrl_Mtr_DelayTicks = 0;

ButtonCtrl_Action_Step ButtonCtrl_Action_State = BTN_CTRL_IDLE;
uint16_t PreAction_Expire_Ticks = 0;
uint16_t ExitAction_Expire_Ticks = 0;
uint16_t LastAction_Expire_Ticks = 0;


static uint8_t ButtonCtrl_queue_is_empty(void);
static uint8_t ButtonCtrl_queue_is_full(void);
//static uint8_t ButtonCtrl_queue_push_e(ButtonCtrl_Str fl_str_e);
static uint8_t ButtonCtrl_queue_pull_e(ButtonCtrl_Str * fl_str_e);
static void ButtonCtrl_Update_Req(ButtonCtrl_Str fl_str_e);
static ButtonCtrl_Req_Str ButtonCtrl_Convert_Pos_EventProcess(ButtonCtrl_Str fl_str_e);
static ButtonCtrl_Mode_Req_Str ButtonCtrl_Convert_Mode_EventProcess(ButtonCtrl_Str fl_str_e);
static void ButtonCtrl_Motor_ActionProcess(ButtonCtrlReq_Str req,uint8_t mtr_index);
static uint8_t ButtonCtrl_Update_StopBtn_Ticks(ButtonCtrl_Str fl_str_e);
static void ButtonCtrl_Update_Mode(ButtonCtrl_Str fl_str_e);
static void ButtonCtrl_Update_Check(void);
static void Buttonctrl_ModePos_Check(void);
static void ButtonCtrl_SavePosReq_Process(void);
static void ButtonCtrl_Req_Process(void);
static uint8_t ButtonCtrl_Motor_IsOk(void);
static 	void ButtonCtrl_NoMtr_EventProcess(void);
static 	uint8_t ButtonCtrl_CheckReq_IsDone(ButtonCtrl_Mode_Req_Str BtnReq[]);
static 	void ButtonCtrl_ActionStep_Process(void);
/*

BTN_ID_CTRL_MODE_ZERO_GRAVITY_e,
BTN_ID_CTRL_MODE_DRIVERIESS_CAR_e,
BTN_ID_CTRL_MODE_MEETING_e,
BTN_ID_CTRL_MODE_LEISURE_e,
BTN_ID_CTRL_OFF_e,
BTN_ID_CTRL_MOVIE_e,
BTN_ID_CTRL_SLEEP_e,
BTN_ID_CTRL_PREPARE_MEAL_e,
*/



const uint8_t ButtonCtrl_ExitReq_Active_Convert_Table[MAX_SAVE_NUMBER][6] = 
{
/*frontrear 	 |	  Back_Angle  |  Rotate 		|  Head 		   | Leg			  |   Total Angle	 */  
BTNVAL_OFF,    BTNVAL_OFF,		BTNVAL_OFF,    BTNVAL_OFF,	 BTNVAL_OFF,	 BTNVAL_ON,
BTNVAL_OFF,    BTNVAL_OFF,		BTNVAL_OFF,    BTNVAL_OFF,	 BTNVAL_OFF,	 BTNVAL_OFF,

BTNVAL_OFF,    BTNVAL_OFF,		BTNVAL_OFF,    BTNVAL_OFF,	 BTNVAL_OFF,	 BTNVAL_OFF,

BTNVAL_OFF,    BTNVAL_OFF,		BTNVAL_OFF,    BTNVAL_OFF,	 BTNVAL_OFF,	 BTNVAL_OFF,
BTNVAL_OFF,    BTNVAL_OFF,		BTNVAL_OFF,    BTNVAL_OFF,	 BTNVAL_OFF,	 BTNVAL_OFF,
BTNVAL_OFF,    BTNVAL_OFF,		BTNVAL_OFF,    BTNVAL_OFF,	 BTNVAL_OFF,	 BTNVAL_OFF,

BTNVAL_OFF,    BTNVAL_OFF,		BTNVAL_OFF,    BTNVAL_OFF,	 BTNVAL_OFF,	 BTNVAL_OFF,
BTNVAL_OFF,    BTNVAL_OFF,		BTNVAL_OFF,    BTNVAL_OFF,	 BTNVAL_OFF,	 BTNVAL_OFF,	
};

const uint8_t ButtonCtrl_ExitReq_Val_Convert_Table[MAX_SAVE_NUMBER][6] = 
{
/*frontrear 	 |	  Back_Angle  |  Rotate 		|  Head 		   | Leg			  |   Total Angle	 */  
50,    50,	 0, 	0,	0 , 	0,	  
50,    50,	 0, 	0,	0 , 	0,	

50,    50,	 0, 	0,	0 , 	0,	

50,    50,	 0, 	0,	0 , 	0,	
50,    50,	 0, 	0,	0 , 	0,	   
50,    50,	 0, 	0,	0	,	 0, 

50,    50,	 0, 	0,	0  ,	0,	
50,    50,	 0, 	0,	0  ,	0,	
};


/*
BTN_ID_CTRL_MODE_ZERO_GRAVITY_e,
BTN_ID_CTRL_MODE_DRIVERIESS_CAR_e,
BTN_ID_CTRL_MODE_MEETING_e,
BTN_ID_CTRL_MODE_LEISURE_e,
BTN_ID_CTRL_OFF_e,
BTN_ID_CTRL_MOVIE_e,
BTN_ID_CTRL_SLEEP_e,
BTN_ID_CTRL_PREPARE_MEAL_e,
*/

const uint8_t ButtonCtrl_PreReq_Active_Convert_Table[MAX_SAVE_NUMBER][6] = 
{
/*frontrear      |    Back_Angle  |  Rotate         |  Head            | Leg              |   Total Angle    */  
BTNVAL_OFF,    BTNVAL_ON ,  	BTNVAL_ON ,    BTNVAL_OFF, 	 BTNVAL_OFF,	 BTNVAL_OFF,
BTNVAL_OFF,    BTNVAL_ON,		BTNVAL_OFF,    BTNVAL_OFF,	 BTNVAL_OFF,	 BTNVAL_OFF,

BTNVAL_OFF,    BTNVAL_ON,  	BTNVAL_OFF,    BTNVAL_OFF, 	 BTNVAL_OFF,	 BTNVAL_OFF,

BTNVAL_OFF,    BTNVAL_OFF,  	BTNVAL_OFF,    BTNVAL_OFF, 	 BTNVAL_OFF,	 BTNVAL_OFF,
BTNVAL_OFF,    BTNVAL_OFF,		BTNVAL_OFF,    BTNVAL_OFF,	 BTNVAL_OFF,	 BTNVAL_OFF,
BTNVAL_OFF,    BTNVAL_ON,		BTNVAL_OFF,    BTNVAL_OFF,	 BTNVAL_OFF,	 BTNVAL_OFF,

BTNVAL_OFF,    BTNVAL_OFF,  	BTNVAL_OFF,    BTNVAL_OFF, 	 BTNVAL_OFF,	 BTNVAL_OFF,
BTNVAL_OFF,    BTNVAL_OFF,  	BTNVAL_OFF,    BTNVAL_OFF, 	 BTNVAL_OFF,	 BTNVAL_OFF,	
};

const uint8_t ButtonCtrl_PreReq_Val_Convert_Table[MAX_SAVE_NUMBER][6] = 
{
/*frontrear      |    Back_Angle  |  Rotate         |  Head            | Leg              |   Total Angle    */  
50,    0 ,   0,     0,  0 ,     0,    
50,	   0,	 0,     0,	0 ,	    0,	

50,	   0,	 0,     0,	0 ,	    0,	

50,	   50,	 0,     0,  0 ,	    0,	
50,    50,	 0,     0,	0 ,	    0,	   
50,	   0,	 0,     0,	0	,    0,	

50,    50,	 0,     0,  0  ,	0,	
50,    50,	 0,     0,	0  ,	0,	
};



const uint8_t ButtonCtrl_Req_Active_Convert_Table[MAX_SAVE_NUMBER][6] = 
{
/*frontrear      |    Back_Angle  |  Rotate         |  Head            | Leg              |   Total Angle    */  
BTNVAL_ON,    BTNVAL_ON,    BTNVAL_OFF,   BTNVAL_ON ,    BTNVAL_ON,      BTNVAL_ON,     
BTNVAL_ON, 	  BTNVAL_ON, 	BTNVAL_ON,    BTNVAL_OFF, 	 BTNVAL_OFF,	 BTNVAL_ON,	

BTNVAL_ON, 	  BTNVAL_ON, 	BTNVAL_ON,    BTNVAL_OFF, 	 BTNVAL_OFF,	 BTNVAL_ON,	

BTNVAL_ON, 	  BTNVAL_ON, 	BTNVAL_OFF,   BTNVAL_ON , 	 BTNVAL_ON, 	 BTNVAL_ON,	
BTNVAL_ON,    BTNVAL_ON, 	BTNVAL_ON,    BTNVAL_ON,  	 BTNVAL_ON, 	 BTNVAL_ON,   
BTNVAL_ON, 	  BTNVAL_ON, 	BTNVAL_ON,    BTNVAL_OFF, 	 BTNVAL_OFF,	 BTNVAL_ON,	

BTNVAL_ON,    BTNVAL_OFF,  	BTNVAL_ON,    BTNVAL_OFF, 	 BTNVAL_ON, 	 BTNVAL_ON,	
BTNVAL_ON,    BTNVAL_OFF,  	BTNVAL_ON,    BTNVAL_OFF, 	 BTNVAL_OFF,	 BTNVAL_ON,	
};

const uint8_t ButtonCtrl_Req_Val_Convert_Table[MAX_SAVE_NUMBER][6] = 
{
/*frontrear      |    Back_Angle  |  Rotate         |  Head            | Leg              |   Total Angle    */  
50,    50,   0,     0,  0 ,     0,    
50,	   50,	 0,     0,	0 ,	    0,	

50,	   50,	 0,     0,	0 ,	    0,	

50,	   50,	 0,     0,  0 ,	    0,	
50,    50,	 0,     0,	0 ,	    0,	   
50,	   50,	 0,     0,	0	,   0,	

50,    50,	 0,     0,  0  ,	0,	
50,    50,	 0,     0,	0  ,	0,	
};


uint16_t ButtonCtrl_ModePos[MAX_SAVE_NUMBER][7] = 
{
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};




const uint8_t ButtonCtrl_Mtr_Calibration_Sequence[CDDMTR_HFKF_MAX_NUM][CDDMTR_HFKF_MAX_NUM] = 
{
	0  ,  0xFF,  0xFF, 0xFF,  0xFF,  0xFF,
	1  ,  0xFF,  0xFF, 0xFF,  0xFF,  0xFF,
	2  ,  0xFF,  0xFF, 0xFF,  0xFF,  0xFF,
	3  ,  0xFF,  0xFF, 0xFF,  0xFF,  0xFF,
	4  ,  0xFF,  0xFF, 0xFF,  0xFF,  0xFF,
	5  ,  0xFF,  0xFF, 0xFF,  0xFF,  0xFF,
};

const uint8_t ButtonCtrl_Mtr_Calibration_MaxCh = 2;
const uint8_t ButtonCtrl_Mtr_Calibration_DelayTicks = 20;

/*******************************************************************************
* Function Name: ButtonCtrlInit
********************************************************************************
*
* Summary:
*  This API is called to initailize ButtoNCtrl (include SARADC for Battery)
*
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void ButtonCtrlInit(void)
{
	ButtonCtrl_queue.queue_in= 0;
	ButtonCtrl_queue.queue_out = 0;	
	memset(&ButtonCtrl_Req,0,sizeof(ButtonCtrl_Req));
	memset(&ButtonCtrl_PreReq,0,sizeof(ButtonCtrl_PreReq));
	memset(&ButtonCtrl_LastReq,0,sizeof(ButtonCtrl_LastReq));
	memset(&ButtonCtrl_ExitReq,0,sizeof(ButtonCtrl_ExitReq));
	
	memset(&StopBtn_Cnt,STOP_BTN_TIMEOUT,sizeof(StopBtn_Cnt));
	CddEeprom_Req_Read(EEPROM_BANK_APP,BUTTON_MODE_E2_START_ADDR,MODE_E2_LEN,Button_Mode);
	Button_Mode[MODE_E2_LEN] = Button_Mode[0];
	CddEeprom_Req_Read(EEPROM_BANK_APP,MODE_POS_E2_START_ADDR,MODE_POS_E2_LEN,&ButtonCtrl_ModePos[0][0]);
	Button_Update_Flag = TRUE;
	memset(ButtonMode_Update_Flag, TRUE,MAX_SAVE_NUMBER);
	LIN_CMD0_Data.SCM_Fan_SCM_msg.Fan_Pwm = Button_Mode[1];
	LIN_CMD1_Data.SCM_Fan_SCM_msg.Fan_Pwm = Button_Mode[1];
	LIN_CMD2_Data.SCM_L_SCM_msg.L_mode = Button_Mode[2];
	LIN_CMD2_Data.SCM_L_SCM_msg.L_Func = Button_Mode[3];

	Buttonctrl_ModePos_Check();

	ButtonCtrl_Mtr_Calbration_State = BTN_ID_CTRL_CAL_IDLE;
    ButtonCtrl_Mtr_DelayTicks = 0;

	ButtonCtrl_Action_State = BTN_CTRL_IDLE;
	PreAction_Expire_Ticks = 0;
	ExitAction_Expire_Ticks = 0;
	LastAction_Expire_Ticks = 0;
}
/*******************************************************************************
* Function Name: Buttonctrl_ModePos_Check
********************************************************************************
*
* Summary:
*  check eeprom position crc16 check result, reset position during crc error
*
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Buttonctrl_ModePos_Check(void)
{
	uint8_t index;
	uint16_t crcval;

	for(index = 0 ; index < ((sizeof(ButtonCtrl_ModePos))/(sizeof(ButtonCtrl_ModePos[0]))); index ++)
	{
		crcval = usMBCRC16((uint8_t*)ButtonCtrl_ModePos[index][0],6);
		if(crcval != ButtonCtrl_ModePos[index][6])
		{
			ButtonCtrl_ModePos[index][0] = (uint16_t)ButtonCtrl_Req_Val_Convert_Table[index][0];
			ButtonCtrl_ModePos[index][1] = (uint16_t)ButtonCtrl_Req_Val_Convert_Table[index][1];
			ButtonCtrl_ModePos[index][2] = (uint16_t)ButtonCtrl_Req_Val_Convert_Table[index][2];
			ButtonCtrl_ModePos[index][3] = (uint16_t)ButtonCtrl_Req_Val_Convert_Table[index][3];
			ButtonCtrl_ModePos[index][4] = (uint16_t)ButtonCtrl_Req_Val_Convert_Table[index][4];
			ButtonCtrl_ModePos[index][5] = (uint16_t)ButtonCtrl_Req_Val_Convert_Table[index][5];
		}
	}

}

/*******************************************************************************
* Function Name: ButtonCtrlFastSample_5ms
********************************************************************************
*
* Summary:
*  This API is called from Main loop to handle Button Control state transition
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void ButtonCtrl_Calibration_Process(void)
{
	uint8_t index = 0;
	uint8_t learning = FALSE;
	
	if(ButtonCtrl_Mtr_Calbration_State == BTN_ID_CTRL_CAL_IDLE)
	{
		if(ButtonCtrl_Req.Clibration.ReqActive == BTNVAL_ON && ButtonCtrl_Req.Clibration.ButtonVal == DIRECTION_FRONT)
		{
			ButtonCtrl_Mtr_Calbration_State++;
			ButtonCtrl_Mtr_DelayTicks = 0;
		}

	}
	else if((ButtonCtrl_Mtr_Calbration_State % 2) == 1)
	{
		while(index < CDDMTR_HFKF_MAX_NUM)
		{
			if(ButtonCtrl_Mtr_Calibration_Sequence[ButtonCtrl_Mtr_Calbration_State/2][index] != 0xFF)
			{
				CddMtr_Learn_Req(ButtonCtrl_Mtr_Calibration_Sequence[ButtonCtrl_Mtr_Calbration_State/2][index],CDDMTR_MNG_LEARN_ENABLE);
			}
			index ++;
		}

		if((ButtonCtrl_Mtr_Calbration_State / 2) < CDDMTR_HFKF_MAX_NUM)
		{
			ButtonCtrl_Mtr_Calbration_State++;
		}
		else
		{
			ButtonCtrl_Mtr_Calbration_State = BTN_ID_CTRL_CAL_IDLE;
		}
		
		ButtonCtrl_Mtr_DelayTicks = 0;
	}
	else if((ButtonCtrl_Mtr_Calbration_State % 2) == 0)
	{
		if(ButtonCtrl_Mtr_DelayTicks <= ButtonCtrl_Mtr_Calibration_DelayTicks)
		{
			ButtonCtrl_Mtr_DelayTicks ++;
		}
		else
		{

			while(index < CDDMTR_HFKF_MAX_NUM)
			{
				if(ButtonCtrl_Mtr_Calibration_Sequence[(ButtonCtrl_Mtr_Calbration_State-1)/2][index] != 0xFF)
				{
					if(TRUE == CddMtr_Get_Mtr_Learning_Status(index))
					{
						learning = TRUE;
					}
				}
				index ++;
			}

			if(learning == FALSE)
			{
				ButtonCtrl_Mtr_Calbration_State++;
			}
		}
	}


}
/*******************************************************************************
* Function Name: ButtonCtrlFastSample_5ms
********************************************************************************
*
* Summary:
*  This API is called from Main loop to handle Button Control state transition
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
uint8_t ButtonCtrl_Motor_IsOk(void)
{
	uint8_t MotorState = TRUE;
	uint8_t index = 0;

	for(index = 0 ; index < CDDMTR_HFKF_MAX_NUM; index ++)
	{
		if(CDDMTR_MNG_HALL_NORMAL != CddMtr_Get_Hall_Fault_Status(index))
		{
			MotorState = FALSE;
			break;
		}

		if(CDDMTR_MNG_LEARN_VALID != CddMtr_Get_LearnData_Status(index))
		{
			MotorState = FALSE;
			break;
		}

	}

	return MotorState;
}

/*******************************************************************************
* Function Name: ButtonCtrlFastSample_5ms
********************************************************************************
*
* Summary:
*  This API is called from Main loop to handle Button Control state transition
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void ButtonCtrl_50ms_Task(void)
{
	uint8_t ret_val;
	ButtonCtrl_Str fl_str_e;
	uint8_t MotorState ;

	MotorState = ButtonCtrl_Motor_IsOk();
	
	ret_val = ButtonCtrl_queue_pull_e(&fl_str_e);

	/*ingore rotate key during zero gravity mode*/
	if((fl_str_e.ButtonId == BTN_ID_CTRL_ROTATE_e) && (Button_Mode[0] == BTN_ID_CTRL_MODE_ZERO_GRAVITY_e))
	{
		ret_val = FALSE;
	}

	if(ret_val == TRUE)
	{
		ButtonCtrl_Update_Mode(fl_str_e);
		ButtonCtrl_Update_Req(fl_str_e);
		ButtonCtrl_SavePosReq_Process();
	}
	if(TRUE == ButtonCtrl_Update_StopBtn_Ticks(fl_str_e))
	{
		ret_val = TRUE;
	}
	
	ButtonCtrl_Calibration_Process();
	
	if(ret_val == TRUE)
	{
		if((ButtonCtrl_Mtr_Calbration_State == BTN_ID_CTRL_CAL_IDLE) && (MotorState))
		{
			ButtonCtrl_Req_Process();
		}

		ButtonCtrl_NoMtr_EventProcess();
		memset(&ButtonCtrl_Req,0,sizeof(ButtonCtrl_Req));
	}

	if((ButtonCtrl_Mtr_Calbration_State == BTN_ID_CTRL_CAL_IDLE) && (MotorState))
	{
		ButtonCtrl_ActionStep_Process();
	}
	else
	{
		ButtonCtrl_Action_State = BTN_CTRL_IDLE;
		PreAction_Expire_Ticks = 0;
		ExitAction_Expire_Ticks = 0;
		LastAction_Expire_Ticks = 0;
		memset(&ButtonCtrl_PreReq,0,sizeof(ButtonCtrl_PreReq));
		memset(&ButtonCtrl_LastReq,0,sizeof(ButtonCtrl_LastReq));
		memset(&ButtonCtrl_ExitReq,0,sizeof(ButtonCtrl_ExitReq));
	}

	ButtonCtrl_Update_Check();
}


/************************************
Get Sw Fifo Empty condition
************************************/
uint8_t ButtonCtrl_queue_is_empty(void)
{
	uint8_t fl_temp_U8;
	
	__disable_irq();
	if(ButtonCtrl_queue.queue_in == ButtonCtrl_queue.queue_out)
	{
		fl_temp_U8 = TRUE;
	}
	else
	{
		fl_temp_U8 = FALSE;
	}
	__enable_irq();

	return fl_temp_U8;
}

/************************************
Get Sw Fifo Full condition
************************************/

uint8_t ButtonCtrl_queue_is_full(void)
{
	uint8_t fl_temp_U8;
	__disable_irq();
	fl_temp_U8 = (ButtonCtrl_queue.queue_in + 1) % (sizeof(ButtonCtrl_queue.queue)/sizeof(ButtonCtrl_queue.queue[0]));
	if(fl_temp_U8 == ButtonCtrl_queue.queue_out)
	{
		fl_temp_U8 = TRUE;
	}
	else
	{
		fl_temp_U8 = FALSE;
	}
	__enable_irq();

	return fl_temp_U8;
}

/************************************
Put a Can data to Sw Fifo
return: TRUE->OK    FALSE->Fail
************************************/

uint8_t ButtonCtrl_queue_push_e(ButtonCtrl_Str fl_str_e)
{
	uint8_t fl_temp_U8;

	if(TRUE == ButtonCtrl_queue_is_full())
	{
		return FALSE;
	}
	else
	{
	    __disable_irq();
		fl_temp_U8 = (ButtonCtrl_queue.queue_in + 1) % (sizeof(ButtonCtrl_queue.queue)/sizeof(ButtonCtrl_queue.queue[0]));
		ButtonCtrl_queue.queue_in = fl_temp_U8;
		ButtonCtrl_queue.queue[fl_temp_U8].ButtonId  = fl_str_e.ButtonId;
		ButtonCtrl_queue.queue[fl_temp_U8].ButtonVal  = fl_str_e.ButtonVal;
		__enable_irq();
		return TRUE;
	}
}

/************************************
Read Tx Sw Fifo From Queue
************************************/
uint8_t ButtonCtrl_queue_pull_e(ButtonCtrl_Str * fl_str_e)
{
	uint8_t fl_temp_U8;

	if(TRUE == ButtonCtrl_queue_is_empty())
	{
		return FALSE;
	}
	else
	{
	    __disable_irq();
		fl_temp_U8 = (ButtonCtrl_queue.queue_out + 1) % (sizeof(ButtonCtrl_queue.queue)/sizeof(ButtonCtrl_queue.queue[0]));
		ButtonCtrl_queue.queue_out = fl_temp_U8;
		fl_str_e[0].ButtonId = ButtonCtrl_queue.queue[fl_temp_U8].ButtonId ;
		fl_str_e[0].ButtonVal = ButtonCtrl_queue.queue[fl_temp_U8].ButtonVal;
		__enable_irq();

		return TRUE;
	}
}


/************************************
Motor action Process
************************************/
void ButtonCtrl_Motor_ActionProcess(ButtonCtrlReq_Str req,uint8_t mtr_index)
{
	uint16_t mtr_current_pos;
	if(mtr_index < CDDMTR_HFKF_MAX_NUM)
	{
		//if(req.ReqActive == BTNVAL_ON)
		{
			if(req.ButtonVal == DIRECTION_STOP)
			{
				mtr_current_pos = CddMtr_Get_Mtr_PosPercent(mtr_index);	
				CddMtr_Percent_Req(mtr_index,((uint32_t)mtr_current_pos)*10);
			}
			else if(req.ButtonVal == DIRECTION_REAR)
			{
				CddMtr_Percent_Req(mtr_index,((uint32_t)MINP)*100);
			}
			else if(req.ButtonVal == DIRECTION_FRONT)
			{
				CddMtr_Percent_Req(mtr_index,((uint32_t)MAXP)*100);
			}
			else if(req.ButtonVal <= MAXP)
			{
				CddMtr_Percent_Req(mtr_index,((uint32_t)req.ButtonVal)*100);
			}
		}
	}

}


/************************************
Motor Button Process
************************************/
void ButtonCtrl_Req_EventProcess(ButtonCtrl_Mode_Req_Str BtnReq[])
{
	
	if(BtnReq[0].Front_Rear_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(BtnReq[0].Front_Rear_Motor,0);
	}
	if(BtnReq[0].Back_Angle_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(BtnReq[0].Back_Angle_Motor,3);
	}
	if(BtnReq[0].Rotate_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(BtnReq[0].Rotate_Motor,4);
	}
	if(BtnReq[0].Head_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(BtnReq[0].Head_Motor,2);
	}
	if(BtnReq[0].Leg_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(BtnReq[0].Leg_Motor,5);
	}
	if(BtnReq[0].Total_Angle_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(BtnReq[0].Total_Angle_Motor,1);
	}
}

uint8_t ButtonCtrl_CheckReq_IsDone(ButtonCtrl_Mode_Req_Str BtnReq[])
{
	uint8_t Ret = TRUE;

	if(BtnReq[0].Front_Rear_Motor.ReqActive == BTNVAL_ON)
	{
		if(BtnReq[0].Front_Rear_Motor.ButtonVal != CddMtr_Get_Mtr_PosPercent(0))
		{
			Ret = FALSE;
		}
	}
	if(BtnReq[0].Back_Angle_Motor.ReqActive == BTNVAL_ON)
	{
		if(BtnReq[0].Back_Angle_Motor.ButtonVal != CddMtr_Get_Mtr_PosPercent(3))
		{
			Ret = FALSE;
		}
	}
	if(BtnReq[0].Rotate_Motor.ReqActive == BTNVAL_ON)
	{
		if(BtnReq[0].Rotate_Motor.ButtonVal != CddMtr_Get_Mtr_PosPercent(4))
		{
			Ret = FALSE;
		}
	}
	if(BtnReq[0].Head_Motor.ReqActive == BTNVAL_ON)
	{
		if(BtnReq[0].Head_Motor.ButtonVal != CddMtr_Get_Mtr_PosPercent(2))
		{
			Ret = FALSE;
		}
	}
	if(BtnReq[0].Leg_Motor.ReqActive == BTNVAL_ON)
	{
		if(BtnReq[0].Leg_Motor.ButtonVal != CddMtr_Get_Mtr_PosPercent(5))
		{
			Ret = FALSE;
		}
	}
	if(BtnReq[0].Total_Angle_Motor.ReqActive == BTNVAL_ON)
	{
		if(BtnReq[0].Total_Angle_Motor.ButtonVal != CddMtr_Get_Mtr_PosPercent(1))
		{
			Ret = FALSE;
		}
	}

	return Ret;
}


/************************************
Motor Button Process
************************************/
void ButtonCtrl_NoMtr_EventProcess(void)
{	
	if(ButtonCtrl_Req.LED.ReqActive == BTNVAL_ON)
	{
		if(ButtonCtrl_Req.LED.ButtonVal == BTNVAL_ON)
		{
			CddLed_Req(TRUE);
		}
		else if(ButtonCtrl_Req.LED.ButtonVal == BTNVAL_OFF)
		{
			CddLed_Req(FALSE);
		}
		else if(ButtonCtrl_Req.LED.ButtonVal == DIRECTION_FRONT)
		{
			CddLed_Covert();
		}
	}
	if(ButtonCtrl_Req.Ventilation.ReqActive == BTNVAL_ON)
	{
		if(ButtonCtrl_Req.Ventilation.ButtonVal != 0)
		{
			LIN_CMD0_Data.SCM_Fan_SCM_msg.Fan_Pwm = 100;
			LIN_CMD1_Data.SCM_Fan_SCM_msg.Fan_Pwm = 100;
			Button_Mode[1] = 100;
		}
		else
		{
			LIN_CMD0_Data.SCM_Fan_SCM_msg.Fan_Pwm = 0;
			LIN_CMD1_Data.SCM_Fan_SCM_msg.Fan_Pwm = 0;
			Button_Mode[1] = 0;
		}
		Button_Update_Flag = FALSE;
	}
	if(ButtonCtrl_Req.Massage.ReqActive == BTNVAL_ON)
	{
		if(ButtonCtrl_Req.Ventilation.ButtonVal <= 3)
		{
			LIN_CMD2_Data.SCM_L_SCM_msg.L_mode = ButtonCtrl_Req.Ventilation.ButtonVal;
			Button_Mode[2] = ButtonCtrl_Req.Ventilation.ButtonVal;
		}

		if(ButtonCtrl_Req.Ventilation.ButtonVal != 0)
		{
			LIN_CMD2_Data.SCM_L_SCM_msg.L_Func = TRUE;
			Button_Mode[3] = TRUE;
		}
		else
		{
			LIN_CMD2_Data.SCM_L_SCM_msg.L_Func = FALSE;
			Button_Mode[3] = FALSE;
		}
		
		Button_Update_Flag = FALSE;
	}
}


/************************************
Motor Button Convert
************************************/
ButtonCtrl_Mode_Req_Str ButtonCtrl_Convert_Mode_EventProcess(ButtonCtrl_Str fl_str_e)
{
	ButtonCtrl_Mode_Req_Str ButtonCtrl_Mode_Req;

	if(fl_str_e.ButtonId< ((sizeof(ButtonCtrl_Req_Active_Convert_Table))/(sizeof(ButtonCtrl_Req_Active_Convert_Table[0]))))
	{
		ButtonCtrl_Mode_Req.Front_Rear_Motor.ReqActive = ButtonCtrl_Req_Active_Convert_Table[fl_str_e.ButtonId][0];
		ButtonCtrl_Mode_Req.Front_Rear_Motor.ButtonVal = ButtonCtrl_Req_Val_Convert_Table[fl_str_e.ButtonId][0];

		ButtonCtrl_Mode_Req.Back_Angle_Motor.ReqActive = ButtonCtrl_Req_Active_Convert_Table[fl_str_e.ButtonId][1];
		ButtonCtrl_Mode_Req.Back_Angle_Motor.ButtonVal = ButtonCtrl_Req_Val_Convert_Table[fl_str_e.ButtonId][1];

		ButtonCtrl_Mode_Req.Rotate_Motor.ReqActive = ButtonCtrl_Req_Active_Convert_Table[fl_str_e.ButtonId][2];
		ButtonCtrl_Mode_Req.Rotate_Motor.ButtonVal = ButtonCtrl_Req_Val_Convert_Table[fl_str_e.ButtonId][2];

		ButtonCtrl_Mode_Req.Head_Motor.ReqActive = ButtonCtrl_Req_Active_Convert_Table[fl_str_e.ButtonId][3];
		ButtonCtrl_Mode_Req.Head_Motor.ButtonVal = ButtonCtrl_Req_Val_Convert_Table[fl_str_e.ButtonId][3];

		ButtonCtrl_Mode_Req.Leg_Motor.ReqActive = ButtonCtrl_Req_Active_Convert_Table[fl_str_e.ButtonId][4];
		ButtonCtrl_Mode_Req.Leg_Motor.ButtonVal = ButtonCtrl_Req_Val_Convert_Table[fl_str_e.ButtonId][4];

		ButtonCtrl_Mode_Req.Total_Angle_Motor.ReqActive = ButtonCtrl_Req_Active_Convert_Table[fl_str_e.ButtonId][5];
		ButtonCtrl_Mode_Req.Total_Angle_Motor.ButtonVal = ButtonCtrl_Req_Val_Convert_Table[fl_str_e.ButtonId][5];

	}
	else
	{
		memset(&ButtonCtrl_Mode_Req,0,sizeof(ButtonCtrl_Mode_Req));
	}

	return ButtonCtrl_Mode_Req;
}

/************************************
Motor Button Convert
************************************/
ButtonCtrl_Req_Str ButtonCtrl_Convert_Pos_EventProcess(ButtonCtrl_Str fl_str_e)
{
	ButtonCtrl_Req_Str ButtonCtrl_Req;

	memset(&ButtonCtrl_Req , 0 , sizeof(ButtonCtrl_Req));

	switch(fl_str_e.ButtonId)
	{
		case BTN_ID_CTRL_POS_FRONT_REAR_e:
			ButtonCtrl_Req.ButtonCtrl_Mode.Front_Rear_Motor.ReqActive = BTNVAL_ON;
			ButtonCtrl_Req.ButtonCtrl_Mode.Front_Rear_Motor.ButtonVal = fl_str_e.ButtonVal;
		break;
		
		case BTN_ID_CTRL_BACK_ANGLE_e:
			ButtonCtrl_Req.ButtonCtrl_Mode.Back_Angle_Motor.ReqActive = BTNVAL_ON;
			ButtonCtrl_Req.ButtonCtrl_Mode.Back_Angle_Motor.ButtonVal = fl_str_e.ButtonVal;
		break;
		
		case BTN_ID_CTRL_ROTATE_e:
			ButtonCtrl_Req.ButtonCtrl_Mode.Rotate_Motor.ReqActive = BTNVAL_ON;
			ButtonCtrl_Req.ButtonCtrl_Mode.Rotate_Motor.ButtonVal = fl_str_e.ButtonVal;
		break;
		
		case BTN_ID_CTRL_HEAD_e:
			ButtonCtrl_Req.ButtonCtrl_Mode.Head_Motor.ReqActive = BTNVAL_ON;
			ButtonCtrl_Req.ButtonCtrl_Mode.Head_Motor.ButtonVal = fl_str_e.ButtonVal;
		break;

		case BTN_ID_CTRL_LEG_e:
			ButtonCtrl_Req.ButtonCtrl_Mode.Leg_Motor.ReqActive = BTNVAL_ON;
			ButtonCtrl_Req.ButtonCtrl_Mode.Leg_Motor.ButtonVal = fl_str_e.ButtonVal;
		break;
		
		case BTN_ID_CTRL_VENTILITION_e:
			ButtonCtrl_Req.Ventilation.ReqActive = BTNVAL_ON;
			ButtonCtrl_Req.Ventilation.ButtonVal = fl_str_e.ButtonVal;
		break;
			
		case BTN_ID_CTRL_LED_e:
			ButtonCtrl_Req.LED.ReqActive = BTNVAL_ON;
			ButtonCtrl_Req.LED.ButtonVal = fl_str_e.ButtonVal;
		break;
		
		case BTN_ID_CTRL_MASSAGE_e:
			ButtonCtrl_Req.Massage.ReqActive = BTNVAL_ON;
			ButtonCtrl_Req.Massage.ButtonVal = fl_str_e.ButtonVal;
		break;

		case BTN_ID_CTRL_SAVE_e:
			ButtonCtrl_Req.Save.ReqActive = BTNVAL_ON;
			ButtonCtrl_Req.Save.ButtonVal = fl_str_e.ButtonVal;
		break;

		case BTN_ID_CTRL_CALIBARTION_MTR_e:
			ButtonCtrl_Req.Clibration.ReqActive = BTNVAL_ON;
			ButtonCtrl_Req.Clibration.ButtonVal = fl_str_e.ButtonVal;
		break;

		
	}

	return ButtonCtrl_Req;
}


/************************************
Motor Button Dispatch
************************************/
void ButtonCtrl_Update_Req(ButtonCtrl_Str fl_str_e)
{
	ButtonCtrl_Mode_Req_Str ButtonCtrl_Mode_Req;
	
	ButtonCtrl_Req_Str ButtonCtrl_Req_Temp;

	if(fl_str_e.ButtonId <= BTN_ID_CTRL_PREPARE_MEAL_e)
	{
		memset(&ButtonCtrl_Req , 0 , sizeof(ButtonCtrl_Req));
		ButtonCtrl_Mode_Req = ButtonCtrl_Convert_Mode_EventProcess(fl_str_e);
		ButtonCtrl_Req_Temp.ButtonCtrl_Mode = ButtonCtrl_Mode_Req;
	}
	else
	{
		ButtonCtrl_Req_Temp = ButtonCtrl_Convert_Pos_EventProcess(fl_str_e);
	}

	ButtonCtrl_Req =  ButtonCtrl_Req_Temp;
}

/************************************
update stop btn ticks
************************************/
uint8_t ButtonCtrl_Update_StopBtn_Ticks(ButtonCtrl_Str fl_str_e)
{
	uint8_t retval = FALSE;
	StopBtn_Cnt[0] ++;
	StopBtn_Cnt[1] ++;
	StopBtn_Cnt[2] ++;
	StopBtn_Cnt[3] ++;
	StopBtn_Cnt[4] ++;
	
	switch(fl_str_e.ButtonId)
	{
		case BTN_ID_CTRL_POS_FRONT_REAR_e:
			StopBtn_Cnt[0] = 0;
		break;
		
		case BTN_ID_CTRL_BACK_ANGLE_e:
			StopBtn_Cnt[1] = 0;
		break;
		
		case BTN_ID_CTRL_ROTATE_e:
			StopBtn_Cnt[2] = 0;
		break;
		
		case BTN_ID_CTRL_HEAD_e:
			StopBtn_Cnt[3] = 0;
		break;

		case BTN_ID_CTRL_LEG_e:
			StopBtn_Cnt[4] = 0;
		break;
		
		default:
			if(StopBtn_Cnt[0] == STOP_BTN_TIMEOUT)
			{
				ButtonCtrl_Req.ButtonCtrl_Mode.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_Req.ButtonCtrl_Mode.Front_Rear_Motor.ButtonVal = DIRECTION_STOP;
				retval = TRUE;
			}
			else if(StopBtn_Cnt[0] > STOP_BTN_TIMEOUT)
			{
				StopBtn_Cnt[0] = STOP_BTN_TIMEOUT;
			}
			
			if(StopBtn_Cnt[1] == STOP_BTN_TIMEOUT)
			{
				ButtonCtrl_Req.ButtonCtrl_Mode.Back_Angle_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_Req.ButtonCtrl_Mode.Back_Angle_Motor.ButtonVal = DIRECTION_STOP;
				retval = TRUE;
			}
			else if(StopBtn_Cnt[1] > STOP_BTN_TIMEOUT)
			{
				StopBtn_Cnt[1] = STOP_BTN_TIMEOUT;
			}

			if(StopBtn_Cnt[2] == STOP_BTN_TIMEOUT)
			{
				ButtonCtrl_Req.ButtonCtrl_Mode.Rotate_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_Req.ButtonCtrl_Mode.Rotate_Motor.ButtonVal = DIRECTION_STOP;
				retval = TRUE;
			}
			else if(StopBtn_Cnt[2] > STOP_BTN_TIMEOUT)
			{
				StopBtn_Cnt[2] = STOP_BTN_TIMEOUT;
			}

			if(StopBtn_Cnt[3] == STOP_BTN_TIMEOUT)
			{
				ButtonCtrl_Req.ButtonCtrl_Mode.Head_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_Req.ButtonCtrl_Mode.Head_Motor.ButtonVal = DIRECTION_STOP;
				retval = TRUE;
			}
			else if(StopBtn_Cnt[3] > STOP_BTN_TIMEOUT)
			{
				StopBtn_Cnt[3] = STOP_BTN_TIMEOUT;
			}
			
			if(StopBtn_Cnt[4] == STOP_BTN_TIMEOUT)
			{
				ButtonCtrl_Req.ButtonCtrl_Mode.Leg_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_Req.ButtonCtrl_Mode.Leg_Motor.ButtonVal = DIRECTION_STOP;
				retval = TRUE;
			}
			else if(StopBtn_Cnt[4] > STOP_BTN_TIMEOUT)
			{
				StopBtn_Cnt[4] = STOP_BTN_TIMEOUT;
			}
		break;
		
	}

	return retval;
}

/************************************
update mode
************************************/
void ButtonCtrl_Update_Mode(ButtonCtrl_Str fl_str_e)
{
	
	switch (fl_str_e.ButtonId)
	{
		case BTN_ID_CTRL_MODE_ZERO_GRAVITY_e:
		case BTN_ID_CTRL_MODE_DRIVERIESS_CAR_e:
		case BTN_ID_CTRL_MODE_MEETING_e:
		case BTN_ID_CTRL_MODE_LEISURE_e:
		case BTN_ID_CTRL_OFF_e:
		case BTN_ID_CTRL_MOVIE_e:
		case BTN_ID_CTRL_SLEEP_e:
		case BTN_ID_CTRL_PREPARE_MEAL_e:
		if(Button_Mode[0] != fl_str_e.ButtonId)
		{	
			Button_Mode[0] = fl_str_e.ButtonId;
			Button_Update_Flag = CddEeprom_Req_Write(EEPROM_BANK_APP,BUTTON_MODE_E2_START_ADDR,MODE_E2_LEN,0,Button_Mode);
		}
		break;
		
		case BTN_ID_CTRL_POS_FRONT_REAR_e:
		case BTN_ID_CTRL_BACK_ANGLE_e:
		case BTN_ID_CTRL_ROTATE_e:
		case BTN_ID_CTRL_HEAD_e:
		case BTN_ID_CTRL_LEG_e:
		case BTN_ID_CTRL_VENTILITION_e:	
		case BTN_ID_CTRL_LED_e:
		case BTN_ID_CTRL_MASSAGE_e:			
		case BTN_ID_CTRL_SAVE_e:
		case BTN_ID_CTRL_CALIBARTION_MTR_e:
		default:

		break;
	}
}
/************************************
update mode check
************************************/
void ButtonCtrl_Update_Check(void)
{
	uint8_t index = 0;
	if(FALSE == Button_Update_Flag)
	{
		Button_Update_Flag = CddEeprom_Req_Write(EEPROM_BANK_APP,BUTTON_MODE_E2_START_ADDR,MODE_E2_LEN,0,Button_Mode);
	}
	else
	{
		for(index = 0; index < MAX_SAVE_NUMBER  ; index ++)
		{
			if(FALSE == ButtonMode_Update_Flag[index])
			{
				ButtonMode_Update_Flag[index] = CddEeprom_Req_Write(EEPROM_BANK_APP,MODE_POS_E2_START_ADDR,MODE_E2_LEN,0,ButtonCtrl_ModePos[index]);
				break;
			}
		}
	}
}


ButtonCtrl_Id_e ButtonCtrl_Get_CtrlMode(void)
{
	ButtonCtrl_Id_e BtnId;

	BtnId = (ButtonCtrl_Id_e)Button_Mode[0];

	return BtnId;
}

/************************************
save mode pos
************************************/
void ButtonCtrl_SavePosReq_Process(void)
{
	uint8_t index ;
	
	if(ButtonCtrl_Req.Save.ReqActive)
	{
		index = Button_Mode[0];
		if(index < MAX_SAVE_NUMBER)
		{
			if(BTNVAL_ON == ButtonCtrl_Req_Active_Convert_Table[index][0])
			{
				ButtonCtrl_ModePos[index][0] = CddMtr_Get_Mtr_PosPercent(0);
			}
			if(BTNVAL_ON == ButtonCtrl_Req_Active_Convert_Table[index][1])
			{
				ButtonCtrl_ModePos[index][1] = CddMtr_Get_Mtr_PosPercent(3);
			}
			if(BTNVAL_ON == ButtonCtrl_Req_Active_Convert_Table[index][2])
			{
				ButtonCtrl_ModePos[index][2] = CddMtr_Get_Mtr_PosPercent(4);
			}
			if(BTNVAL_ON == ButtonCtrl_Req_Active_Convert_Table[index][3])
			{
				ButtonCtrl_ModePos[index][3] = CddMtr_Get_Mtr_PosPercent(2);
			}
			if(BTNVAL_ON == ButtonCtrl_Req_Active_Convert_Table[index][4])
			{
				ButtonCtrl_ModePos[index][4] = CddMtr_Get_Mtr_PosPercent(5);
			}
			if(BTNVAL_ON == ButtonCtrl_Req_Active_Convert_Table[index][5])
			{
				ButtonCtrl_ModePos[index][5] = CddMtr_Get_Mtr_PosPercent(1);
			}
			ButtonCtrl_ModePos[index][6] = usMBCRC16((uint8_t*)ButtonCtrl_ModePos[index][0],6);
			ButtonMode_Update_Flag[index] = FALSE;
		}
	}
}

/************************************
Active
************************************/
void ButtonCtrl_Req_Process(void)
{
	uint8_t index,lastindex;
	uint8_t temp ;
	
	if(Button_Mode[MODE_E2_LEN] != Button_Mode[0])
	{
		index = Button_Mode[0];
		lastindex = Button_Mode[MODE_E2_LEN] ;
		Button_Mode[MODE_E2_LEN] = index;
		if(index < MAX_SAVE_NUMBER)
		{
			 temp = FALSE;
			if(ButtonCtrl_PreReq_Active_Convert_Table[index][0] == BTNVAL_ON)
			{
				ButtonCtrl_PreReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_PreReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_PreReq_Val_Convert_Table[index][0];
				temp= TRUE;
			}
			if(ButtonCtrl_PreReq_Active_Convert_Table[index][1] == BTNVAL_ON)
			{
				ButtonCtrl_PreReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_PreReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_PreReq_Val_Convert_Table[index][1];
				temp= TRUE;
			}
			if(ButtonCtrl_PreReq_Active_Convert_Table[index][2] == BTNVAL_ON)
			{
				ButtonCtrl_PreReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_PreReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_PreReq_Val_Convert_Table[index][2];
				temp= TRUE;
			}
			if(ButtonCtrl_PreReq_Active_Convert_Table[index][3] == BTNVAL_ON)
			{
				ButtonCtrl_PreReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_PreReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_PreReq_Val_Convert_Table[index][3];
				temp= TRUE;
			}
			if(ButtonCtrl_PreReq_Active_Convert_Table[index][4] == BTNVAL_ON)
			{
				ButtonCtrl_PreReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_PreReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_PreReq_Val_Convert_Table[index][4];
				temp= TRUE;
			}
			if(ButtonCtrl_PreReq_Active_Convert_Table[index][5] == BTNVAL_ON)
			{
				ButtonCtrl_PreReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_PreReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_PreReq_Val_Convert_Table[index][5];
				temp= TRUE;
			}

			if(temp)
			{
				PreAction_Expire_Ticks = MOTOR_ACTION_MAX_TICKS;
			}
			else
			{
				PreAction_Expire_Ticks = 0;
			}
			ButtonCtrl_Action_State = BTN_CTRL_EXIT_ACT;
		}
		
		if(lastindex < MAX_SAVE_NUMBER)
		{
			temp= FALSE;
			if(ButtonCtrl_ExitReq_Active_Convert_Table[lastindex][0] == BTNVAL_ON)
			{
				ButtonCtrl_ExitReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_ExitReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_ExitReq_Val_Convert_Table[lastindex][0];
				temp= TRUE;
			}
			if(ButtonCtrl_ExitReq_Active_Convert_Table[lastindex][1] == BTNVAL_ON)
			{
				ButtonCtrl_ExitReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_ExitReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_ExitReq_Val_Convert_Table[lastindex][1];
				temp= TRUE;
			}
			if(ButtonCtrl_ExitReq_Active_Convert_Table[lastindex][2] == BTNVAL_ON)
			{
				ButtonCtrl_ExitReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_ExitReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_ExitReq_Val_Convert_Table[lastindex][2];
				temp= TRUE;
			}
			if(ButtonCtrl_ExitReq_Active_Convert_Table[lastindex][3] == BTNVAL_ON)
			{
				ButtonCtrl_ExitReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_ExitReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_ExitReq_Val_Convert_Table[lastindex][3];
				temp= TRUE;
			}
			if(ButtonCtrl_ExitReq_Active_Convert_Table[lastindex][4] == BTNVAL_ON)
			{
				ButtonCtrl_ExitReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_ExitReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_ExitReq_Val_Convert_Table[lastindex][4];
				temp= TRUE;
			}
			if(ButtonCtrl_ExitReq_Active_Convert_Table[lastindex][5] == BTNVAL_ON)
			{
				ButtonCtrl_ExitReq.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_ExitReq.Front_Rear_Motor.ButtonVal = ButtonCtrl_ExitReq_Val_Convert_Table[lastindex][5];
				temp= TRUE;
			}

			if(temp)
			{
				ExitAction_Expire_Ticks = MOTOR_ACTION_MAX_TICKS;
			}
			else
			{
				PreAction_Expire_Ticks = 0;
			}
		}
	}
	
	LastAction_Expire_Ticks = MOTOR_ACTION_MAX_TICKS;
	memcpy(&ButtonCtrl_LastReq,&ButtonCtrl_Req,sizeof(ButtonCtrl_LastReq));
}

/************************************
Action
************************************/
void ButtonCtrl_ActionStep_Process(void)
{
	switch(ButtonCtrl_Action_State)
	{
		case BTN_CTRL_IDLE:

		break;
		case BTN_CTRL_EXIT_ACT:
			if(ExitAction_Expire_Ticks > 0)
			{
				if(MOTOR_ACTION_MAX_TICKS == ExitAction_Expire_Ticks)
				{
					ButtonCtrl_Req_EventProcess(&ButtonCtrl_ExitReq);
				}
				if(ButtonCtrl_CheckReq_IsDone(&ButtonCtrl_ExitReq))
				{
					ExitAction_Expire_Ticks = 0;
				}
				else
				{
					ExitAction_Expire_Ticks --;
				}
			}
			else
			{
				ButtonCtrl_Action_State = BTN_CTRL_PRE_ACT;
				memset(&ButtonCtrl_ExitReq,0,sizeof(ButtonCtrl_ExitReq));
			}
		break;
		case BTN_CTRL_PRE_ACT:
			if(PreAction_Expire_Ticks > 0)
			{
				if(MOTOR_ACTION_MAX_TICKS == PreAction_Expire_Ticks)
				{
					ButtonCtrl_Req_EventProcess(&ButtonCtrl_PreReq);
				}
				if(ButtonCtrl_CheckReq_IsDone(&ButtonCtrl_PreReq))
				{
					PreAction_Expire_Ticks = 0;
				}
				else
				{
					PreAction_Expire_Ticks --;
				}
			}
			else
			{
				ButtonCtrl_Action_State = BTN_CTRL_LAST_ACT;
				memset(&ButtonCtrl_PreReq,0,sizeof(ButtonCtrl_PreReq));
			}
		break;
		case BTN_CTRL_LAST_ACT:
			if(LastAction_Expire_Ticks > 0)
			{
				if(MOTOR_ACTION_MAX_TICKS == PreAction_Expire_Ticks)
				{
					ButtonCtrl_Req_EventProcess(&ButtonCtrl_LastReq);
				}
				if(ButtonCtrl_CheckReq_IsDone(&ButtonCtrl_LastReq))
				{
					PreAction_Expire_Ticks = 0;
				}
				else
				{
					PreAction_Expire_Ticks --;
				}
			}
			else
			{
				ButtonCtrl_Action_State = BTN_CTRL_IDLE;
				memset(&ButtonCtrl_LastReq,0,sizeof(ButtonCtrl_LastReq));
			}
		break;

		default:
			ButtonCtrl_Action_State = BTN_CTRL_IDLE;
			memset(&ButtonCtrl_PreReq,0,sizeof(ButtonCtrl_PreReq));
			memset(&ButtonCtrl_LastReq,0,sizeof(ButtonCtrl_LastReq));
			memset(&ButtonCtrl_ExitReq,0,sizeof(ButtonCtrl_ExitReq));
		break;
	}

}

/* [] END OF FILE */
