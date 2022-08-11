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

#define MODE_E2_LEN  4

ButtonCtrl_Queue_str ButtonCtrl_queue;

ButtonCtrl_Req_Str ButtonCtrl_Req;

uint8_t StopBtn_Cnt[4];
uint16_t Button_Mode[MODE_E2_LEN] = {0};


uint8_t Button_Update_Flag = TRUE;

static uint8_t ButtonCtrl_queue_is_empty(void);
static uint8_t ButtonCtrl_queue_is_full(void);
//static uint8_t ButtonCtrl_queue_push_e(ButtonCtrl_Str fl_str_e);
static uint8_t ButtonCtrl_queue_pull_e(ButtonCtrl_Str * fl_str_e);
static void ButtonCtrl_Update_Req(ButtonCtrl_Str fl_str_e);
static ButtonCtrl_Req_Str ButtonCtrl_Convert_Pos_EventProcess(ButtonCtrl_Str fl_str_e);
static ButtonCtrl_Mode_Req_Str ButtonCtrl_Convert_Mode_EventProcess(ButtonCtrl_Str fl_str_e);
static void ButtonCtrl_Motor_ActionProcess(ButtonCtrlReq_Str req,uint8_t mtr_index);
static void ButtonCtrl_Motor_EventProcess(void);
static void ButtonCtrl_Update_StopBtn_Ticks(ButtonCtrl_Str fl_str_e);
static void ButtonCtrl_Update_Mode(ButtonCtrl_Str fl_str_e);
static void ButtonCtrl_Update_Check(void);

/*

BTN_ID_CTRL_MODE_ZERO_GRAVITY_e,
BTN_ID_CTRL_MODE_DRIVERIESS_CAR_e,

BTN_ID_CTRL_MODE_MEETING_e,

BTN_ID_CTRL_MODE_LEISURE_e,
BTN_ID_CTRL_OFF_e,
BTN_ID_CTRL_MOVIE_e,

BTN_ID_CTRL_SLEEP_e,
BTN_ID_CTRL_PREPARE_MEAL_e,

BTN_ID_CTRL_POS_FRONT_REAR_e,
BTN_ID_CTRL_BACK_ANGLE_e,
BTN_ID_CTRL_ROTATE_e,
BTN_ID_CTRL_HEAD_e,
BTN_ID_CTRL_VENTILITION_e,	
BTN_ID_CTRL_LEG_e,
BTN_ID_CTRL_MASSAGE_e,

*/

#if SEAT_CTRL_VARIANT == SEAT_CTRL_VARIANT_R

const ButtonCtrl_Mode_Req_Str ButtonCtrl_Req_Convert_Table[] = 
{
/*frontrear      |    Back_Angle  |  Rotate         |  Head            | Leg              |   Total Angle    */  
BTNVAL_ON, 30,    BTNVAL_ON, 30,    BTNVAL_OFF,  0,   BTNVAL_OFF,  0,    BTNVAL_ON, 30   ,   BTNVAL_ON,  0,    
BTNVAL_ON, 40,	  BTNVAL_ON, 40,	BTNVAL_ON,  0,    BTNVAL_OFF,  0,	 BTNVAL_OFF, 30	 ,	 BTNVAL_OFF,  0,	

BTNVAL_ON, 50,	  BTNVAL_ON, 50,	BTNVAL_ON,  0,    BTNVAL_OFF,  0,	 BTNVAL_OFF, 30	 ,	 BTNVAL_OFF,  0,	

BTNVAL_ON, 60,	  BTNVAL_ON, 60,	BTNVAL_OFF,  0,   BTNVAL_OFF,  0,	 BTNVAL_ON, 30	 ,	 BTNVAL_OFF,  0,	
BTNVAL_ON, MAXP,  BTNVAL_ON, 70,	BTNVAL_ON,  0,    BTNVAL_ON,  0,	 BTNVAL_ON, 30  ,	 BTNVAL_ON,  0,	   
BTNVAL_ON, 70,	  BTNVAL_ON, 80,	BTNVAL_ON,  0,    BTNVAL_OFF,  0,	 BTNVAL_OFF, 30	 ,	 BTNVAL_OFF,  0,	

BTNVAL_ON, 80,    BTNVAL_OFF, 90,	BTNVAL_ON,  0,    BTNVAL_OFF,  0,	 BTNVAL_ON, 30  ,	 BTNVAL_OFF,  0,	
BTNVAL_ON, 90,    BTNVAL_OFF, 75,	BTNVAL_ON,  0,   BTNVAL_OFF,  0,	 BTNVAL_OFF, 30  ,	 BTNVAL_OFF,  0,	
};

#elif SEAT_CTRL_VARIANT == SEAT_CTRL_VARIANT_L

const ButtonCtrl_Mode_Req_Str ButtonCtrl_Req_Convert_Table[] = 
{
/*frontrear      |    Back_Angle  |  Rotate         |  Head            | Leg              |   Total Angle    */  
BTNVAL_ON, 30,    BTNVAL_ON, 30,    BTNVAL_OFF,  0,   BTNVAL_OFF,  0,    BTNVAL_ON, 30   ,   BTNVAL_ON,  0,    
BTNVAL_ON, 40,	  BTNVAL_ON, 40,	BTNVAL_ON,  0,    BTNVAL_OFF,  0,	 BTNVAL_OFF, 30	 ,	 BTNVAL_OFF,  0,	

BTNVAL_ON, 50,	  BTNVAL_ON, 50,	BTNVAL_ON,  0,    BTNVAL_OFF,  0,	 BTNVAL_OFF, 30	 ,	 BTNVAL_OFF,  0,	

BTNVAL_ON, 60,	  BTNVAL_ON, 60,	BTNVAL_OFF,  0,   BTNVAL_OFF,  0,	 BTNVAL_ON, 30	 ,	 BTNVAL_OFF,  0,	
BTNVAL_ON, MAXP,  BTNVAL_ON, 70,	BTNVAL_ON,  0,    BTNVAL_ON,  0,	 BTNVAL_ON, 30  ,	 BTNVAL_ON,  0,	   
BTNVAL_ON, 70,	  BTNVAL_ON, 80,	BTNVAL_ON,  0,    BTNVAL_OFF,  0,	 BTNVAL_OFF, 30	 ,	 BTNVAL_OFF,  0,	

BTNVAL_ON, 80,    BTNVAL_OFF, 90,	BTNVAL_ON,  0,    BTNVAL_OFF,  0,	 BTNVAL_ON, 30  ,	 BTNVAL_OFF,  0,	
BTNVAL_ON, 90,    BTNVAL_OFF, 75,	BTNVAL_ON,  0,   BTNVAL_OFF,  0,	 BTNVAL_OFF, 30  ,	 BTNVAL_OFF,  0,	
};


#endif



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
	
	memset(&StopBtn_Cnt,STOP_BTN_TIMEOUT,sizeof(StopBtn_Cnt));
	CddEeprom_Req_Read(EEPROM_BANK_APP,100,MODE_E2_LEN,Button_Mode);
	Button_Update_Flag = TRUE;
	LIN_CMD0_Data.SCM_Fan_SCM_msg.Fan_Pwm = Button_Mode[1];
	LIN_CMD1_Data.SCM_Fan_SCM_msg.Fan_Pwm = Button_Mode[1];
	LIN_CMD2_Data.SCM_L_SCM_msg.L_mode = Button_Mode[2];
	LIN_CMD2_Data.SCM_L_SCM_msg.L_mode = Button_Mode[3];
	LIN_CMD2_Data.SCM_L_SCM_msg.L_Func = Button_Mode[4];
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
	
	ret_val = ButtonCtrl_queue_pull_e(&fl_str_e);

	if(ret_val == TRUE)
	{
		ButtonCtrl_Update_Mode(fl_str_e);
		ButtonCtrl_Update_Req(fl_str_e);
		ButtonCtrl_Update_StopBtn_Ticks(fl_str_e);
		ButtonCtrl_Motor_EventProcess();
		memset(&ButtonCtrl_Req,0,sizeof(ButtonCtrl_Req));
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
void ButtonCtrl_Motor_EventProcess(void)
{

	if(ButtonCtrl_Req.ButtonCtrl_Mode.Front_Rear_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(ButtonCtrl_Req.ButtonCtrl_Mode.Front_Rear_Motor,0);
	}
	if(ButtonCtrl_Req.ButtonCtrl_Mode.Back_Angle_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(ButtonCtrl_Req.ButtonCtrl_Mode.Back_Angle_Motor,3);
	}
	if(ButtonCtrl_Req.ButtonCtrl_Mode.Rotate_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(ButtonCtrl_Req.ButtonCtrl_Mode.Rotate_Motor,4);
	}
	if(ButtonCtrl_Req.ButtonCtrl_Mode.Head_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(ButtonCtrl_Req.ButtonCtrl_Mode.Head_Motor,2);
	}
	if(ButtonCtrl_Req.ButtonCtrl_Mode.Leg_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(ButtonCtrl_Req.ButtonCtrl_Mode.Leg_Motor,5);
	}
	if(ButtonCtrl_Req.ButtonCtrl_Mode.Total_Angle_Motor.ReqActive == BTNVAL_ON)
	{
		ButtonCtrl_Motor_ActionProcess(ButtonCtrl_Req.ButtonCtrl_Mode.Total_Angle_Motor,1);
	}

	
	if(ButtonCtrl_Req.LED.ReqActive == BTNVAL_ON)
	{
		if(ButtonCtrl_Req.LED.ButtonVal == BTNVAL_ON)
		{
			CddLed_Req(TRUE);
		}
		else
		{
			CddLed_Req(FALSE);
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

	if(fl_str_e.ButtonId< ((sizeof(ButtonCtrl_Req_Convert_Table))/(sizeof(ButtonCtrl_Req_Convert_Table[0]))))
	{
		ButtonCtrl_Mode_Req = ButtonCtrl_Req_Convert_Table[fl_str_e.ButtonId];
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
void ButtonCtrl_Update_StopBtn_Ticks(ButtonCtrl_Str fl_str_e)
{
	StopBtn_Cnt[0] ++;
	StopBtn_Cnt[1] ++;
	StopBtn_Cnt[2] ++;
	StopBtn_Cnt[3] ++;
	
	
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
		
		default:
			if(StopBtn_Cnt[0] == STOP_BTN_TIMEOUT)
			{
				ButtonCtrl_Req.ButtonCtrl_Mode.Front_Rear_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_Req.ButtonCtrl_Mode.Front_Rear_Motor.ButtonVal = DIRECTION_STOP;
			}
			else if(StopBtn_Cnt[0] > STOP_BTN_TIMEOUT)
			{
				StopBtn_Cnt[0] = STOP_BTN_TIMEOUT;
			}
			
			if(StopBtn_Cnt[1] == STOP_BTN_TIMEOUT)
			{
				ButtonCtrl_Req.ButtonCtrl_Mode.Back_Angle_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_Req.ButtonCtrl_Mode.Back_Angle_Motor.ButtonVal = DIRECTION_STOP;
			}
			else if(StopBtn_Cnt[1] > STOP_BTN_TIMEOUT)
			{
				StopBtn_Cnt[1] = STOP_BTN_TIMEOUT;
			}

			if(StopBtn_Cnt[2] == STOP_BTN_TIMEOUT)
			{
				ButtonCtrl_Req.ButtonCtrl_Mode.Rotate_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_Req.ButtonCtrl_Mode.Rotate_Motor.ButtonVal = DIRECTION_STOP;
			}
			else if(StopBtn_Cnt[2] > STOP_BTN_TIMEOUT)
			{
				StopBtn_Cnt[2] = STOP_BTN_TIMEOUT;
			}

			if(StopBtn_Cnt[3] == STOP_BTN_TIMEOUT)
			{
				ButtonCtrl_Req.ButtonCtrl_Mode.Head_Motor.ReqActive = BTNVAL_ON;
				ButtonCtrl_Req.ButtonCtrl_Mode.Head_Motor.ButtonVal = DIRECTION_STOP;
			}
			else if(StopBtn_Cnt[3] > STOP_BTN_TIMEOUT)
			{
				StopBtn_Cnt[3] = STOP_BTN_TIMEOUT;
			}
			
		break;
		
	}

	
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
			Button_Update_Flag = CddEeprom_Req_Write(EEPROM_BANK_APP,100,MODE_E2_LEN,0,Button_Mode);
		}
		break;
		
		case BTN_ID_CTRL_POS_FRONT_REAR_e:
		case BTN_ID_CTRL_BACK_ANGLE_e:
		case BTN_ID_CTRL_ROTATE_e:
		case BTN_ID_CTRL_HEAD_e:
		case BTN_ID_CTRL_VENTILITION_e:	
		case BTN_ID_CTRL_LED_e:
		case BTN_ID_CTRL_MASSAGE_e:
		default:

		break;
	}
}
/************************************
update mode check
************************************/
void ButtonCtrl_Update_Check(void)
{
	if(FALSE == Button_Update_Flag)
	{
		Button_Update_Flag = CddEeprom_Req_Write(EEPROM_BANK_APP,100,MODE_E2_LEN,0,Button_Mode);
	}
}


ButtonCtrl_Id_e ButtonCtrl_Get_CtrlMode(void)
{
	ButtonCtrl_Id_e BtnId;

	BtnId = (ButtonCtrl_Id_e)Button_Mode[0];

	return BtnId;
}


/* [] END OF FILE */
