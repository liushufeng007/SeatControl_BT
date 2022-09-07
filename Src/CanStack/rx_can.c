#define TCAN_C
/*****************************************************************************
*                                 System Includes                            *
*****************************************************************************/
#include "system.h"
#include "string.h"
/*****************************************************************************
*                                 Project Includes                           *
******************************************************************************/
#include "il.h"
#include "il_par.h"
//#include "can_comm.h"
#include "Canif.h"
#include "vnim_app_signals.h"
#include "Scm_SeatControl.h"
#include "rx_can.h"
#include "CddKey_Cfg.h"

#include "CddEeprom.h"

#include "ButtonCtrl.h"

#include "CddMtr_Mng.h"

#include "il_par.h"

#include "CddLed.h"

extern _SCM_L_45A_msg_buf			        il_tx_SCM_L_45A_msg;






/*****************************************************************************
*                                 Macro Definitions                          *
*----------------------------------------------------------------------------*
* Definition of macro shall be followed by a comment that explains the       *
* purpose of the macro.                                                      *
******************************************************************************/
#ifndef FALSE
#define FALSE  (0u)
#endif
#ifndef TRUE
#define TRUE   (1u)
#endif
#define MOTOR_DUTY_SCANING  2
UINT16 Btn_Signal_Group[BT_SIGNAL_GROUP_NUM*2] = {0,0,0,0,0};

UINT32 VCU_VehSpd = 0;
UINT8 VCU_VehSpd_VD = 0x01;
UINT8 ABM_PassengerSeatOccSts = 0x01;
UINT8 BCM_PsngrSeatMotoPositionReq_ForwardBack = 0;
UINT8 BCM_PsngrSeatMotoPositionReq_SeatBack = 0;





uint8_t update_sucess =TRUE;

/* Rx CAN after set motor position */
UINT16 Set_ForwardBack_TargetPos(void);
UINT16 Set_SeatBack_TargetPos(void);

/* Tx CAN */



void Buttonctrl_ModePos_Check(void)
{
	uint16_t crcval;

	crcval = usMBCRC16((uint8_t*)(Btn_Signal_Group[0]),BT_SIGNAL_GROUP_NUM-1);
	if(crcval != Btn_Signal_Group[BT_SIGNAL_GROUP_NUM-1])
	{
		Btn_Signal_Group[0] = 0;
		Btn_Signal_Group[1] = 0;
		Btn_Signal_Group[2] = 0;
		Btn_Signal_Group[3] = 0;
		Btn_Signal_Group[4] = 0;
		Btn_Signal_Group[5] = 0;
	}

}


void Message_variable_init(void)
{
 
	CddEeprom_Req_Read(EEPROM_BANK_APP,APP_E2_START_ADDR,BT_SIGNAL_GROUP_NUM,Btn_Signal_Group);
	Buttonctrl_ModePos_Check();
	memcpy(&Btn_Signal_Group[BT_SIGNAL_GROUP_NUM],&Btn_Signal_Group[0],BT_SIGNAL_GROUP_NUM*2);
	
	VCU_VehSpd = 0;
	VCU_VehSpd_VD = 0x01;
	ABM_PassengerSeatOccSts = 0x01;
	BCM_PsngrSeatMotoPositionReq_ForwardBack = 0;
	BCM_PsngrSeatMotoPositionReq_SeatBack = 0;
	msg_VIST_never_recvd = TRUE;
	msg_DDSS_never_recvd = TRUE;

	update_sucess =TRUE;
	//CddLed_Req(Btn_Signal_Group[BT_Welcome_e]);

	il_rx_VCU_DDSS_msg.VCU_DDSS_msg.welcome_state1=  	Btn_Signal_Group[BT_Welcome_e]       ;
	il_rx_ABM_VIST_msg.ABM_VIST_msg.angle         =  	Btn_Signal_Group[BT_Angle_e]         ;
	il_rx_ABM_VIST_msg.ABM_VIST_msg.fatigue       =  	Btn_Signal_Group[BT_Fatigue_e]       ;
	il_rx_ABM_VIST_msg.ABM_VIST_msg.pos           =  	Btn_Signal_Group[BT_Pos_e]           ;
	il_rx_ABM_VIST_msg.ABM_VIST_msg.Scene_mode    =  	Btn_Signal_Group[BT_Scene_Mode_e]    ;
	il_rx_ABM_VIST_msg.ABM_VIST_msg.Scene_mode    =  	Btn_Signal_Group[BT_Scene_Mode_e]    ;
	il_rx_Diver_Conference_msg.SCM_L_SCM_msg.conference_mode = Btn_Signal_Group[BT_Conference_Mode_e]    ;

}

void Rx_VCU_DDSS_func(void)
{
	msg_DDSS_never_recvd = FALSE;
	
#if(SCM_SEATCONTROL_VARIANT == SCM_L_VARIANT)
	Btn_Signal_Group[BT_Welcome_e] = il_rx_VCU_DDSS_msg.VCU_DDSS_msg.welcome_state1;
#endif
}

void Rx_ABM_VIST_func(void)
{
	msg_VIST_never_recvd = FALSE;
#if(SCM_SEATCONTROL_VARIANT == SCM_L_VARIANT)
	Btn_Signal_Group[BT_Angle_e] = il_rx_ABM_VIST_msg.ABM_VIST_msg.angle;
	Btn_Signal_Group[BT_Fatigue_e] = il_rx_ABM_VIST_msg.ABM_VIST_msg.fatigue;
	Btn_Signal_Group[BT_Pos_e] = il_rx_ABM_VIST_msg.ABM_VIST_msg.pos;
#endif
	Btn_Signal_Group[BT_Scene_Mode_e] = il_rx_ABM_VIST_msg.ABM_VIST_msg.Scene_mode;
}

void Rx_Driver_Conference_func(void)
{
	msg_3B7_never_recvd = FALSE;
	Btn_Signal_Group[BT_Conference_Mode_e]  = il_rx_Diver_Conference_msg.SCM_L_SCM_msg.conference_mode;
}

void Rx_BCM_458_func(void)
{
	msg_458_never_recvd = FALSE;
	BCM_PsngrSeatMotoPositionReq_ForwardBack = (il_rx_BCM_458_msg.BCM_458_msg.BCM_PsngrSeatMotoPositionReq_ForwardBack)/2;
	BCM_PsngrSeatMotoPositionReq_SeatBack = (il_rx_BCM_458_msg.BCM_458_msg.BCM_PsngrSeatMotoPositionReq_SeatBack)/2;

}
#if(SCM_SEATCONTROL_VARIANT == SCM_L_VARIANT)
static void process_MSG45A(void)
{
	Canif_Msg_Str fl_str_e;
	UINT8 temp = 0;
	ButtonCtrl_Id_e btnMode;
	UINT16 temp16 = 0;
	
	btnMode = ButtonCtrl_Get_CtrlMode();
	if(btnMode == BTN_ID_CTRL_MODE_DRIVERIESS_CAR_e)
	{
		temp = TRUE;
	}
	else
	{
		temp = FALSE;
	}
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.autopilot_mode = temp;
	if(btnMode == BTN_ID_CTRL_MODE_MEETING_e)
	{
		temp = TRUE;
	}
	else
	{
		temp = FALSE;
	}
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.conference_mode = temp;
	temp16 = CddMtr_Get_Mtr_PosPercent(0);
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.Pos = temp16*5/2;
	
	temp16 = CddMtr_Get_Mtr_PosPercent(1);
	temp16 = temp16 / 3;
	if(temp16 > 31)temp16 = 31;
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.angle_bacck =  temp16;


	fl_str_e.ExtId = 0x19FF2A36;
	fl_str_e.DLC = 8;

	for(temp=0; temp<fl_str_e.DLC; temp++)
    {
        fl_str_e.Data[temp] = il_tx_SCM_L_45A_msg.byte[temp];        
    }

	Canif_tx_queue_push_e(fl_str_e);

}
#endif


void update_can_signal(void)
{
	uint8_t index = 0;
	uint16_t temp=0;
	ButtonCtrl_Str btn;
	
	if(Btn_Signal_Group[index] != Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] )
	{
		Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] = Btn_Signal_Group[index];
		update_sucess = FALSE;
		btn.ButtonId = BTN_ID_CTRL_POS_FRONT_REAR_e;
		temp = Btn_Signal_Group[index];
		temp  = temp  * 2 / 5;
		if(temp >100)
		{
			btn.ButtonVal = 100;
		}
		else
		{
			btn.ButtonVal = temp;
		}
		ButtonCtrl_Set_BtnSrc(BTN_ID_SrcID_FRONT_REAR_e,BTN_ID_Src_CAN_e);
		ButtonCtrl_queue_push_e(btn);
	}
	index++;
	if(Btn_Signal_Group[index] != Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] )
	{
		Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] = Btn_Signal_Group[index];
		update_sucess = FALSE;
		btn.ButtonId = BTN_ID_CTRL_BACK_ANGLE_e;
		btn.ButtonVal = Btn_Signal_Group[index]*33/10;
		if(Btn_Signal_Group[index] <= 0x1F)
		{
			ButtonCtrl_Set_BtnSrc(BTN_ID_SrcID_BACK_ANGLE_e,BTN_ID_Src_CAN_e);
			ButtonCtrl_queue_push_e(btn);
		}
	}
	index++;
	if(Btn_Signal_Group[index] != Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] )
	{
		Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] = Btn_Signal_Group[index];
		update_sucess = FALSE;
		temp = TRUE;
		switch(Btn_Signal_Group[index])
		{
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
            /*cancel prepare off button @20220-9-3 from xuyan*/
			/*case 0:   btn.ButtonId = BTN_ID_CTRL_OFF_e; break;  */
			case 1:   btn.ButtonId = BTN_ID_CTRL_PREPARE_MEAL_e; break;
#endif			
			case 2:   btn.ButtonId = BTN_ID_CTRL_OFF_e;break;
			case 3:   btn.ButtonId = BTN_ID_CTRL_MOVIE_e;break;
			case 4:   btn.ButtonId = BTN_ID_CTRL_OFF_e; break;
			case 5:   btn.ButtonId = BTN_ID_CTRL_SLEEP_e; break;
			default: temp = FALSE;break;
		}
		btn.ButtonVal = 0;
		if(temp)
		{
			ButtonCtrl_queue_push_e(btn);
		}
	}
	index++;
	
#if(SCM_SEATCONTROL_VARIANT == SCM_L_VARIANT)
	if(Btn_Signal_Group[index] != Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] )
	{
		Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] = Btn_Signal_Group[index];
		update_sucess = FALSE;
		
		temp = TRUE;
		switch(Btn_Signal_Group[index])
		{
			case 0:   btn.ButtonVal = 0; break;
			case 1:   btn.ButtonVal = 0; break;
			case 2:   btn.ButtonVal = 3;break;
			case 3:   btn.ButtonVal = 0;break;
			default: temp = FALSE;break;
		}
		btn.ButtonId =BTN_ID_CTRL_VENTILITION_e;
		if(temp)
		{
			ButtonCtrl_queue_push_e(btn);
		}
	}
#endif
	index++;
#if(SCM_SEATCONTROL_VARIANT == SCM_L_VARIANT)

	if(Btn_Signal_Group[index] != Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] )
	{
		Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] = Btn_Signal_Group[index];
		update_sucess = FALSE;
		temp = TRUE;
		switch(Btn_Signal_Group[index])
		{
			case 0:   btn.ButtonVal = DIRECTION_REAR;btn.ButtonId =BTN_ID_CTRL_POS_FRONT_REAR_e;; break;
			case 1:   btn.ButtonVal = 50            ;btn.ButtonId =BTN_ID_CTRL_POS_FRONT_REAR_e; break;
			default: temp = FALSE;break;
		}

		if(temp)
		{
			ButtonCtrl_queue_push_e(btn);
		}
	}
#endif

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
	index++;
	if(Btn_Signal_Group[index] != Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] )
	{
		Btn_Signal_Group[index+BT_SIGNAL_GROUP_NUM] = Btn_Signal_Group[index];
		update_sucess = FALSE;
		temp = TRUE;
		switch(Btn_Signal_Group[index])
		{
			case 0:   temp = FALSE; break;
			case 1:   btn.ButtonVal = 1; break;
			default: temp = FALSE;break;
		}
		btn.ButtonId =BTN_ID_CTRL_MODE_MEETING_e;
		if(temp)
		{
			ButtonCtrl_queue_push_e(btn);
		}
	}
#endif
	if(FALSE == update_sucess)
	{
		
		Btn_Signal_Group[6] = usMBCRC16((uint8_t*)(Btn_Signal_Group[0]),BT_SIGNAL_GROUP_NUM-1);
		update_sucess = CddEeprom_Req_Write(EEPROM_BANK_APP,APP_E2_START_ADDR,BT_SIGNAL_GROUP_NUM,0,Btn_Signal_Group);
	}

}

void tcan_Task(void)
{
	static UINT8 Tcan_Send_Cycle = 0;
 

	if( Tcan_Send_Cycle < 10 )
	{
		Tcan_Send_Cycle ++;
	}

	if( Tcan_Send_Cycle == 10 )
	{
		Tcan_Send_Cycle = 0;
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
#else
		process_MSG45A();
#endif
	}

	update_can_signal();
}


UINT16 Set_ForwardBack_TargetPos(void)
{
	UINT16 ForwordBack_TargetPos = 0;

	ForwordBack_TargetPos = BCM_PsngrSeatMotoPositionReq_ForwardBack*100;


	return(ForwordBack_TargetPos);
}

UINT16 Set_SeatBack_TargetPos(void)
{
	UINT16 SeatBack_TargetPos = 0;
	SeatBack_TargetPos = BCM_PsngrSeatMotoPositionReq_SeatBack*100;


	return(SeatBack_TargetPos);
}

/*End of File*/

