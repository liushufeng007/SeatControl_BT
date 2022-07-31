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


#if 0
#define NFC_LOCKED  	(0u)
#define NFC_UNLOCKED    (1u)


UINT8 seed_val[4] = {0,0,0,0};;
UINT8 aes_key[4] = {0,0,0,0};
UINT8 nfc_sw_sts = 1;
UINT8 pre_nfc_sw_sts = 0;
UINT8 wpc_sw_sts = 0;
UINT8 bt_sw_sts = 0;
UINT8 nfc_lock_state = NFC_LOCKED;
UINT8 nfc_wakeup_state = 0;
UINT8 nfc_sleep_req = 0;
UINT8 white_list_req = 0;
UINT16 motor_speed = 0;
UINT32 odo_meter = 0;
#endif
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
UINT32 VCU_VehSpd = 0;
UINT8 VCU_VehSpd_VD = 0x01;
UINT8 ABM_PassengerSeatOccSts = 0x01;
UINT8 IHU_SeatFoldReq = 0;
UINT8 IHU_SeatReleaseReq = 0;
UINT8 BCM_PsngrSeatMotoPositionReq_ForwardBack = 0;
UINT8 BCM_PsngrSeatMotoPositionReq_SeatBack = 0;

UINT16 BT_Signal_Group[BT_SIGNAL_GROUP_NUM*2] = {0,0,0,0,0};

#else
UINT8 SCM_L_BCM_DoorAjarSts_FL = 0;
UINT8 SCM_L_BCM_FaceRecognitionEnableSts = 0;
UINT8 SCM_L_BCM_EntryExitSeatCtrlEnableSts = 0;
UINT8 SCM_L_BCM_MotoPositionReq_ForwardBack = 0;
UINT8 SCM_L_BCM_MotoPositionReq_SeatBack = 0;
#endif


uint8_t update_sucess =TRUE;

/* Rx CAN after set motor position */
UINT16 Set_ForwardBack_TargetPos(void);
UINT16 Set_SeatBack_TargetPos(void);

/* Tx CAN */
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
static void process_MSG45B(void);
#else
static void process_MSG45A(void);
static void process_MSG663(void);
#endif
#if 0
static void seed_send_handle(void);
static void nfc_sw_handle(void);
static void wireless_charge_handle(void);
static void bt_quick_conn_handle(void);
static void add_card_handle(void);
static void delete_card_handle(void);
static void nfc_lock_handle(void);
static void nfc_unlock_handle(void);
static void nfc_sleep_handle(void);
static void seed_receive_handle(void);
static void unlock_request_handle(void);
static void nfc_wakeup_handle(void);
static void nfc_cmd_response(UINT8 nfc_cmd_func, UINT8 nfc_cmd_err_code);
/* RX frame timeout: frame periodic time*5 */ 
#endif

void Message_variable_init(void)
{
	UINT8 index;

	
	CddEeprom_Req_Read(EEPROM_BANK_APP,0,5,BT_Signal_Group);
	CddEeprom_Req_Read(EEPROM_BANK_APP,0,5,&BT_Signal_Group[BT_SIGNAL_GROUP_NUM]);
	
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
	VCU_VehSpd = 0;
	VCU_VehSpd_VD = 0x01;
	ABM_PassengerSeatOccSts = 0x01;
	IHU_SeatFoldReq = 0;
	IHU_SeatReleaseReq = 0;
	BCM_PsngrSeatMotoPositionReq_ForwardBack = 0;
	BCM_PsngrSeatMotoPositionReq_SeatBack = 0;
	msg_VIST_never_recvd = TRUE;
	msg_DDSS_never_recvd = TRUE;
#else
	SCM_L_BCM_DoorAjarSts_FL = 0;
	SCM_L_BCM_FaceRecognitionEnableSts = 0;
	SCM_L_BCM_EntryExitSeatCtrlEnableSts = 0;
	SCM_L_BCM_MotoPositionReq_ForwardBack = 0;
	SCM_L_BCM_MotoPositionReq_SeatBack = 0;
#endif
	update_sucess =TRUE;
	#if 0
	nfc_sw_sts = 1;
	pre_nfc_sw_sts = 1;
	wpc_sw_sts = 0;
	bt_sw_sts = 0;
	nfc_lock_state = NFC_LOCKED;
	nfc_wakeup_state = 0;
	nfc_sleep_req = 0;
	white_list_req = 0;
	motor_speed = 0;
	odo_meter = 0;


	for(index=0; index<4; index++)
	{
		seed_val[index] = 0;
		aes_key[index] = 0;
	}
	#endif
}

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
void Rx_VCU_DDSS_func(void)
{
	msg_DDSS_never_recvd = FALSE;
	//VCU_VehSpd = (((il_rx_VCU_DDSS_msg.VCU_DDSS_msg.VCU_VehSpdHigh << 5)|(il_rx_VCU_123_msg.VCU_123_msg.VCU_VehSpdLow))*625)/10000;
	//VCU_VehSpd_VD = il_rx_VCU_DDSS_msg.VCU_DDSS_msg.VCU_VehSpd_VD;
	BT_Signal_Group[BT_Welcome_e] = il_rx_VCU_DDSS_msg.VCU_DDSS_msg.welcome_state1;
}

void Rx_ABM_VIST_func(void)
{
	msg_VIST_never_recvd = FALSE;
	//ABM_PassengerSeatOccSts = il_rx_ABM_234_msg.ABM_234_msg.ABM_PassengerSeatOccSts;
	BT_Signal_Group[BT_Angle_e] = il_rx_ABM_VIST_msg.ABM_VIST_msg.angle;
	BT_Signal_Group[BT_Fatigue_e] = il_rx_ABM_VIST_msg.ABM_VIST_msg.fatigue;
	BT_Signal_Group[BT_Pos_e] = il_rx_ABM_VIST_msg.ABM_VIST_msg.pos;
	BT_Signal_Group[BT_Scene_Mode_e] = il_rx_ABM_VIST_msg.ABM_VIST_msg.Scene_mode;
}

void Rx_IHU_3B7_func(void)
{
	msg_3B7_never_recvd = FALSE;
	IHU_SeatFoldReq = il_rx_IHU_3B7_msg.IHU_3B7_msg.IHU_SeatFoldReq;
	IHU_SeatReleaseReq = il_rx_IHU_3B7_msg.IHU_3B7_msg.IHU_SeatReleaseReq;
}

void Rx_BCM_458_func(void)
{
	msg_458_never_recvd = FALSE;
	BCM_PsngrSeatMotoPositionReq_ForwardBack = (il_rx_BCM_458_msg.BCM_458_msg.BCM_PsngrSeatMotoPositionReq_ForwardBack)/2;
	BCM_PsngrSeatMotoPositionReq_SeatBack = (il_rx_BCM_458_msg.BCM_458_msg.BCM_PsngrSeatMotoPositionReq_SeatBack)/2;

}

static void process_MSG45B(void)
{
	Canif_Msg_Str fl_str_e;
	UINT8 i = 0;

#if 1
	//il_tx_SCM_L_SCM_msg.SCM_L_45B_msg.SCM_HMI_Req = Dis_SCM_HMI_Req;
	//il_tx_SCM_L_SCM_msg.SCM_L_45B_msg.SCM_PsngrSeatMotoDirection_ForwardBack = Get_Current_ForwardBackMotor_Dir;
	//il_tx_SCM_L_SCM_msg.SCM_L_45B_msg.SCM_PsngrSeatMotoDirection_SeatBack = Get_Current_SeatBackMotor_Dir;
	//il_tx_SCM_L_SCM_msg.SCM_L_45B_msg.SCM_PsngrSeatMotoPosition_ForwardBack = ((Get_ForwardBack_Current_Pos())*MOTOR_DUTY_SCANING);
	//il_tx_SCM_L_SCM_msg.SCM_L_45B_msg.SCM_PsngrSeatMotoPosition_SeatBack = ((Get_SeatBack_Current_Pos())*MOTOR_DUTY_SCANING);
	//il_tx_SCM_L_SCM_msg.SCM_L_45B_msg.SCM_MsgAliveCounter = Get_MsgAlive_Counter();
	//il_tx_SCM_L_SCM_msg.il_rx_VCU_DDSS_msg.angle_bacck = 0;
	//il_tx_SCM_L_SCM_msg.il_rx_VCU_DDSS_msg.autopilot_mode = 0;
	//il_tx_SCM_L_SCM_msg.il_rx_VCU_DDSS_msg.conference_mode = 0;
	//il_tx_SCM_L_SCM_msg.il_rx_VCU_DDSS_msg.Pos = 0;


#else
	il_tx_SCM_L_SCM_msg.SCM_L_45B_msg.SCM_HMI_Req = 0xFF;
	il_tx_SCM_L_SCM_msg.SCM_L_45B_msg.SCM_PsngrSeatMotoDirection_ForwardBack = 0xFF;
	il_tx_SCM_L_SCM_msg.SCM_L_45B_msg.SCM_PsngrSeatMotoDirection_SeatBack = 0xFF;
	il_tx_SCM_L_SCM_msg.SCM_L_45B_msg.SCM_PsngrSeatMotoPosition_ForwardBack = 0xFF;
	il_tx_SCM_L_SCM_msg.SCM_L_45B_msg.SCM_PsngrSeatMotoPosition_SeatBack = 0xFF;

#endif
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
	il_tx_SCM_L_SCM_msg.SCM_L_SCM_msg.angle_bacck = 0;
	il_tx_SCM_L_SCM_msg.SCM_L_SCM_msg.autopilot_mode = 0;
	il_tx_SCM_L_SCM_msg.SCM_L_SCM_msg.conference_mode = 0;
	il_tx_SCM_L_SCM_msg.SCM_L_SCM_msg.Pos = 0;


	fl_str_e.StdId = 0x19FF2A36;
	fl_str_e.DLC = 8;

	for(i=0; i<fl_str_e.DLC; i++)
    {
        fl_str_e.Data[i] = il_tx_SCM_L_SCM_msg.byte[i];        
    }

	if( TRUE == ServSWM_u_GetKey_IGN_RealVaule() )
	{
		if( (il_status & IL_STATUS_TX_SUSPEND) != IL_STATUS_TX_SUSPEND )
		{
			Canif_tx_queue_push_e(fl_str_e);
		}
	}
#else


#endif
	//Vnim_SCM_45B_Send_handler();
}


#else
void Rx_BCM_245_func(void)
{
	msg_245_never_recvd = FALSE;
	SCM_L_BCM_DoorAjarSts_FL = il_rx_BCM_245_msg.BCM_245_msg.BCM_DoorAjarSts_FL;
}

void Rx_BCM_457_func(void)
{
	msg_245_never_recvd = FALSE;
	SCM_L_BCM_FaceRecognitionEnableSts = il_rx_BCM_457_msg.BCM_457_msg.BCM_FaceRecognitionEnableSts;
	SCM_L_BCM_EntryExitSeatCtrlEnableSts = il_rx_BCM_457_msg.BCM_457_msg.BCM_EntryExitSeatCtrlEnableSts;
	SCM_L_BCM_MotoPositionReq_ForwardBack = (il_rx_BCM_457_msg.BCM_457_msg.BCM_MotoPositionReq_ForwardBack)/2;
	SCM_L_BCM_MotoPositionReq_SeatBack = (il_rx_BCM_457_msg.BCM_457_msg.BCM_MotoPositionReq_SeatBack)/2;
}

static void process_MSG45A(void)
{
	Canif_Msg_Str fl_str_e;
	UINT8 i = 0;
	
	#if 1
	//il_tx_SCM_L_45A_msg.SCM_L_45A_msg.SCM_DriverSeatLocalCtrlSwithSts = Get_SeatSwtichSts();
	//il_tx_SCM_L_45A_msg.SCM_L_45A_msg.SCM_DriverSeatMotoDirection_ForwardBack = Get_Current_ForwardBackMotor_Dir;
	//il_tx_SCM_L_45A_msg.SCM_L_45A_msg.SCM_DriverSeatMotoDirection_SeatBack = Get_Current_SeatBackMotor_Dir;
	//il_tx_SCM_L_45A_msg.SCM_L_45A_msg.SCM_DriverSeatMotoPosition_ForwardBack = ((Get_ForwardBack_Current_Pos())*MOTOR_DUTY_SCANING);
	//il_tx_SCM_L_45A_msg.SCM_L_45A_msg.SCM_DriverSeatMotoPosition_SeatBack = ((Get_SeatBack_Current_Pos())*MOTOR_DUTY_SCANING);
	//il_tx_SCM_L_45A_msg.SCM_L_45A_msg.SCM_MsgAliveCounter = Get_MsgAlive_Counter();
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.angle_bacck = 0;
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.autopilot_mode = 0;
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.conference_mode = 0;
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.Pos = 0;
	
	#else
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.SCM_DriverSeatLocalCtrlSwithSts = 0xFF;
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.SCM_DriverSeatMotoDirection_ForwardBack = 0xFF;
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.SCM_DriverSeatMotoDirection_SeatBack = 0xFF;
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.SCM_DriverSeatMotoPosition_ForwardBack = 0xFF;
	il_tx_SCM_L_45A_msg.SCM_L_45A_msg.SCM_DriverSeatMotoPosition_SeatBack = 0xFF;
	#endif

	fl_str_e.StdId = 0x45A;
	fl_str_e.DLC = 8;

	for(i=0; i<fl_str_e.DLC; i++)
    {
        fl_str_e.Data[i] = il_tx_SCM_L_45A_msg.byte[i];        
    }


	if((il_status & IL_STATUS_TX_SUSPEND) != IL_STATUS_TX_SUSPEND )
	{
		Canif_tx_queue_push_e(fl_str_e);
	}

}

static void process_MSG663(void)
{
	#if 0
	Canif_Msg_Str fl_str_e;
	UINT8 i = 0;


	fl_str_e.StdId = 0x663;
	fl_str_e.DLC = 8;

	for(i=0; i<fl_str_e.DLC; i++)
    {
        fl_str_e.Data[i] = il_tx_SCM_L_NM_663_msg.byte[i];        
    }
	
	if( (il_status & IL_STATUS_TX_SUSPEND) == IL_STATUS_TX_SUSPEND )
	{
		Canif_tx_queue_push_e(fl_str_e);
	}	
	#endif
}
#endif


void tcan_Btn(void)
{
	uint8_t index = 0;
	uint8_t update = FALSE;
	ButtonCtrl_Str Btn;
	
	index = BT_Pos_e;
	if(BT_Signal_Group[index] != 0xFFFF)
	{
		if(BT_Signal_Group[index] != BT_Signal_Group[BT_BACK(index)])
		{
			BT_Signal_Group[BT_BACK(index)] = BT_Signal_Group[index];
			update = TRUE;
			Btn.ButtonId = BTN_ID_CTRL_POS_FRONT_REAR_e;
			Btn.ButtonVal = BT_Signal_Group[index];
			ButtonCtrl_queue_push_e(Btn);
		}
	}

	index = BT_Angle_e;
	if(BT_Signal_Group[index] != 0xFFFF)
	{
		if(BT_Signal_Group[index] != BT_Signal_Group[BT_BACK(index)])
		{
			BT_Signal_Group[BT_BACK(index)] = BT_Signal_Group[index];
			update = TRUE;
			Btn.ButtonId = BTN_ID_CTRL_BACK_ANGLE_e;
			Btn.ButtonVal = BT_Signal_Group[index];
			ButtonCtrl_queue_push_e(Btn);
		}
	}

	index = BT_Scene_Mode_e;
	if(BT_Signal_Group[index] != 0xFFFF)
	{
		if(BT_Signal_Group[index] != BT_Signal_Group[BT_BACK(index)])
		{
			BT_Signal_Group[BT_BACK(index)] = BT_Signal_Group[index];
			update = TRUE;
			switch (BT_Signal_Group[index])
			{
				case 1:
					Btn.ButtonId = BTN_ID_CTRL_PREPARE_MEAL_e;
				break;

				case 3:
					Btn.ButtonId = BTN_ID_CTRL_MOVIE_e;
				break;

				case 5:
					Btn.ButtonId = BTN_ID_CTRL_SLEEP_e;
				break;

				default:
					Btn.ButtonId = BTN_ID_CTRL_OFF_e	;
				break;
				
			}
			Btn.ButtonVal = BT_Signal_Group[index];
			ButtonCtrl_queue_push_e(Btn);
		}
	}

	index = BT_Fatigue_e;
	if(BT_Signal_Group[index] != 0xFFFF)
	{
		if(BT_Signal_Group[index] != BT_Signal_Group[BT_BACK(index)])
		{
			BT_Signal_Group[BT_BACK(index)] = BT_Signal_Group[index];
			//update = TRUE;

			switch (BT_Signal_Group[index])
			{
				case 2:
					//turn on vent,打开鼓风机
				break;
				default:
					//turn off vent,打开鼓风机
				break;
			}
		}
	}
	
	index = BT_Welcome_e;
	if(BT_Signal_Group[index] != 0xFFFF)
	{
		if(BT_Signal_Group[index] != BT_Signal_Group[BT_BACK(index)])
		{
			BT_Signal_Group[BT_BACK(index)] = BT_Signal_Group[index];
			update = TRUE;
			Btn.ButtonId = BTN_ID_CTRL_POS_FRONT_REAR_e;
			switch (BT_Signal_Group[index])
			{
				case 1:
					Btn.ButtonVal = 0;
				break;

				case 0:
					Btn.ButtonVal = 100;
				break;
			}
			
			ButtonCtrl_queue_push_e(Btn);
		}
	}

	if(update || !update_sucess)
	{
		if(CddEeprom_Req_Write(EEPROM_BANK_APP,0,5,0,BT_Signal_Group))
		{
			update_sucess = TRUE;
		}
		else
		{
			update_sucess = FALSE;
		}
	}
}

void tcan_Task(void)
{
	static UINT8 Tcan_Send_Cycle = 0;
	static UINT8 NM_Send_Cycle = 0;

	if( Tcan_Send_Cycle < 10 )
	{
		Tcan_Send_Cycle ++;
	}

	if( Tcan_Send_Cycle == 10 )
	{
		Tcan_Send_Cycle = 0;
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
		process_MSG45B();
#else
		process_MSG45A();
#endif
	}

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
#else
	if(NM_Send_Cycle < 100)
	{
		NM_Send_Cycle ++;
	}
	if(NM_Send_Cycle == 100)
	{
		NM_Send_Cycle = 0;
		process_MSG663();
	}
#endif
	tcan_Btn();
}


UINT16 Set_ForwardBack_TargetPos(void)
{
	UINT16 ForwordBack_TargetPos = 0;

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
	ForwordBack_TargetPos = BCM_PsngrSeatMotoPositionReq_ForwardBack*100;
#else
	ForwordBack_TargetPos = SCM_L_BCM_MotoPositionReq_ForwardBack*100;
#endif

	return(ForwordBack_TargetPos);
}

UINT16 Set_SeatBack_TargetPos(void)
{
	UINT16 SeatBack_TargetPos = 0;
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
	SeatBack_TargetPos = BCM_PsngrSeatMotoPositionReq_SeatBack*100;
#else
	SeatBack_TargetPos = SCM_L_BCM_MotoPositionReq_SeatBack*100;
#endif

	return(SeatBack_TargetPos);
}


#if 0
void Rx_NFC_MSG_REQ_1800A5FF_func(void)
{
	UINT8 card_info[CARD_QUANTITY*12] = {0};
	UINT8 byte_num = 0;

	if((il_rx_NFC_MSG_REQ_1800A5FF_msg.byte[0] == 0x18) && (il_rx_NFC_MSG_REQ_1800A5FF_msg.byte[1] == 0x03) &&\
	   (il_rx_NFC_MSG_REQ_1800A5FF_msg.byte[2] == 0xFF) && (il_rx_NFC_MSG_REQ_1800A5FF_msg.byte[3] == 0xA5))
	{
		white_list_req = 1;
	}

	if((il_rx_NFC_MSG_REQ_1800A5FF_msg.byte[0] == 0x18) && (il_rx_NFC_MSG_REQ_1800A5FF_msg.byte[1] == 0x04) &&\
	   (il_rx_NFC_MSG_REQ_1800A5FF_msg.byte[2] == 0xFF) && (il_rx_NFC_MSG_REQ_1800A5FF_msg.byte[3] == 0xA5))
	{
		if(card_key_verified == 1) /* 3DES encrypt verify passed of card */
		{
			send_current_CardNum(CardNum);
		}
		else
		{
			UINT8 card_number[10] = {0};
			send_current_CardNum(card_number);
		}
	}
}


void send_card_white_list(void)
{
	UINT8 card_info[CARD_QUANTITY*12] = {0};
	UINT8 byte_num = 0;
	if(white_list_req == 1)
	{
		wrap_card_list(&byte_num, card_info);
		CanUser_KeyCardsSend(card_info, byte_num);
	}
}


void Rx_NFC_CMD_REQ_1888A5FF_func(void)
{
	if(NFC_CMD_ECU == 0xA5)
	{
		switch(NFC_CMD_FUNC)
		{
			case CTRL_FUNC_SEED_REQ:
				seed_send_handle();
			break;

			case CTRL_FUNC_NFC_SW:
				if(nfc_lock_state == NFC_UNLOCKED)
				{
					nfc_sw_handle();
				}
				else
				{
					nfc_cmd_response(CTRL_FUNC_NFC_SW, NFC_INLOCK);
				}
			break;

			case CTRL_FUNC_WPC_SW:
				if(nfc_lock_state == NFC_UNLOCKED)
				{
					wireless_charge_handle();
				}
				else
				{
					nfc_cmd_response(CTRL_FUNC_WPC_SW, NFC_INLOCK);
				}
			break;

			case CTRL_FUNC_BT_QUICK_CONN_SW:
				if(nfc_lock_state == NFC_UNLOCKED)
				{
					bt_quick_conn_handle();
				}
				else
				{
					nfc_cmd_response(CTRL_FUNC_BT_QUICK_CONN_SW, NFC_INLOCK);
				}
			break;

			case CTRL_FUNC_ADD_CARD:
				if(nfc_lock_state == NFC_UNLOCKED)
				{
					add_card_handle();
				}
				else
				{
					nfc_cmd_response(CTRL_FUNC_ADD_CARD, NFC_INLOCK);
				}
			break;

			case CTRL_FUNC_DEL_CARD:
				if(nfc_lock_state == NFC_UNLOCKED)
				{
					delete_card_handle();
				}
				else
				{
					nfc_cmd_response(CTRL_FUNC_DEL_CARD, NFC_INLOCK);
				}
			break;

			case CTRL_FUNC_LOCK:
				nfc_lock_handle();
			break;

			case CTRL_FUNC_UNLOCK:
				nfc_unlock_handle();
			break;

			case CTRL_FUNC_SLEEP:
				nfc_sleep_handle();
			break;

			case CTRL_FUNC_WAKEUP:
				nfc_wakeup_handle();
			break;
		}
	}
}


void Rx_VCU_CMD_RES_1888FFA0_func(void)
{
	if(VCU_RES_ECU == 0xA0)
	{
		switch(VCU_RES_FUNC)
		{
			case CTRL_FUNC_SEED_REQ:
				if((VCU_RES_ERR_CODE == 0x0) && (VCU_RES_DES == 0xA5) && (card_authentication_pass == 1)) /* Positive response */
				{
					card_authentication_pass = 0;
					seed_receive_handle();
				}
			break;

			case CTRL_FUNC_UNLOCK:
			//	unlock_request_handle(); /* Don't unlock after card authentication and wait VCU unlock command */
			break;

			default:
			break;
		}
	}
}


void Rx_VCU_MOTOR_SPEED_1401FFA0_func(void)
{
	msg_1401FFA0_never_recvd = FALSE;
	motor_speed = il_rx_VCU_MOTOR_SPEED_1401FFA0_msg.VCU_MOTOR_SPEED_1401FFA0_msg.MotorSpeed[0] << 8;
	motor_speed |= il_rx_VCU_MOTOR_SPEED_1401FFA0_msg.VCU_MOTOR_SPEED_1401FFA0_msg.MotorSpeed[1];
}


void Rx_VCU_ODO_METER_1804FFA0_func(void)
{
	msg_1804FFA0_never_recvd = FALSE;
	odo_meter = il_rx_VCU_ODO_METER_1804FFA0_msg.VCU_ODO_METER_1804FFA0_msg.OdoMeter[0] << 16;	
	odo_meter |= il_rx_VCU_ODO_METER_1804FFA0_msg.VCU_ODO_METER_1804FFA0_msg.OdoMeter[1] << 8;
	odo_meter |= il_rx_VCU_ODO_METER_1804FFA0_msg.VCU_ODO_METER_1804FFA0_msg.OdoMeter[2];
}


static void seed_send_handle(void)
{
	struct AES_ctx ctx;

	can_parameter fl_str_e;
	UINT32 current_time = 0;
	UINT8 i;
	UINT8 plain_text[16] = {0};

	current_time = TIMER_GetCurrentValue(TIMER_CHANNEL1);

	NFC_RES_ECU = 0xA5;
	NFC_RES_FUNC = 0x0;
	NFC_RES_ERR_CODE = 0x0;

	seed_val[3] = (UINT8) (current_time & 0xFF);
	seed_val[2] = (UINT8) ((current_time >> 8) & 0xFF);
	seed_val[1] = (UINT8) ((current_time & 0xFF) * 0x37);
	seed_val[0] = (UINT8) (((current_time >> 8) & 0xFF) * 0x51);

	for(i=0; i<4; i++)
	{
		NFC_RES_SEED[i] = seed_val[i];
	}

	for(i=0; i<8; i++)
    {
        fl_str_e.can_data[i] = il_tx_WPC_1888FFA5_msg.byte[i];
    }
    fl_str_e.Can_info.ID = 0x1888FFA5;
    fl_str_e.Can_info.DLC = 8;
    fl_str_e.Can_info.IDE = 1;  
    can_comm_tx_queue_push_e(fl_str_e); 

    /* Generate aes-128 key */
	for(i=0;i<4;i++)
	{
		plain_text[i] = seed_val[i];
	}
	for(i=4;i<16;i++)
	{
		plain_text[i] = 0xC; /* PKCS7 fill */
	}

	AES_init_ctx(&ctx, NFC_KEY);
    AES_ECB_encrypt(&ctx, plain_text);

	for(i=0;i<4;i++)
	{
		aes_key[i] = plain_text[i];
	}
}


static void nfc_sw_handle(void)
{
	UINT8 err_code = 0;

	if(NFC_CMD_PARA[0] == 1)
	{
		nfc_sw_sts = 1;
	}
	else if(NFC_CMD_PARA[0] == 0)
	{
		nfc_sw_sts = 0;
	}
	else
	{
		err_code = 1; /* parameter error */
	}

	nfc_cmd_response(CTRL_FUNC_NFC_SW, err_code);
}


static void wireless_charge_handle(void)
{
	UINT8 err_code = 0;

	if(NFC_CMD_PARA[0] == 1)
	{
		wpc_sw_sts = 1;
	}
	else if(NFC_CMD_PARA[0] == 0)
	{
		wpc_sw_sts = 0;
	}
	else
	{
		err_code = 1; /* parameter error */
	}

	nfc_cmd_response(CTRL_FUNC_WPC_SW, err_code);
}


static void bt_quick_conn_handle(void)
{
	UINT8 err_code = 0;

	if(NFC_CMD_PARA[0] == 1)
	{
		bt_sw_sts = 1;
	}
	else if(NFC_CMD_PARA[0] == 0)
	{
		bt_sw_sts = 0;
	}
	else
	{
		err_code = 1; /* parameter error */
	}

	nfc_cmd_response(CTRL_FUNC_BT_QUICK_CONN_SW, err_code);
}


static void add_card_handle(void)
{
	UINT8 fl_index_U8 =0;
	UINT8 card_index = 0xFF;
	UINT8 card_info_updated = FALSE;
	static UINT8 card_buff[10] = {0};
	static UINT8 add_card_seq = 0;
	UINT8 err_code = 0;

	switch(add_card_seq)
	{
		case 0:
			if(NFC_CMD_PARA[0] == 0x0) /* First frame of card number :5 bytes */
			{
				for(fl_index_U8 = 0 ;fl_index_U8 < 5;fl_index_U8++)
				{
					card_buff[fl_index_U8] = NFC_CMD_PARA[fl_index_U8 + 1];
				}
				add_card_seq = 1;
			}
			else
			{
				err_code = CARD_FORMAT_ERROR; /* card format error */
			}
		break;

		case 1:
			if(NFC_CMD_PARA[0] == 0x1) /* Second frame of card number :5 bytes */
			{
				for(fl_index_U8 = 0 ;fl_index_U8 < 5;fl_index_U8++)
				{
					card_buff[fl_index_U8 + 5] = NFC_CMD_PARA[fl_index_U8 + 1];
				}
				add_card_seq = 2;
			}
			else
			{
				err_code = CARD_FORMAT_ERROR; /* card format error */
				add_card_seq = 0;
			}
		break;

		case 2:
			if(NFC_CMD_PARA[0] == 0x82) /* Third frame of card number :2 bytes */
			{
				add_card_seq = 0;

				if((NFC_CMD_PARA[1] == 0x0) && (NFC_CMD_PARA[2] == 0x0)) /* Last 2 bytes is reserved and value is 0x0 */
				{
					/* Card info received then add the card num into card list */
					if(Find_Card_In_CurrentCardList(card_buff, &card_index) != FALSE)
					{
						if(get_Card_ValidByte(card_index) == 0xFF)
						{
							Update_Card_ValidByte(card_index, 0x1); /* Already in current CardList then set valid byte to 1 */
							card_info_updated = TRUE;
						}
						else
						{
							err_code = CARD_DUPLICATED; /* card already exist */
						}
					}
					else
					{
						if(Find_InvalidPosition_In_CurrentCardList(&card_index))
						{
							Add_CardNumber(card_index, card_buff);
							card_info_updated = TRUE;
						}
						else
						{
							/* No room for new Card */
							err_code = CARD_LIST_FULL; /* card list is full */
						}
					}
				}
				else
				{
					err_code = CARD_FORMAT_ERROR; /* card format error */
				}				
			}
			else
			{
				err_code = CARD_FORMAT_ERROR; /* card format error */
				add_card_seq = 0;
			}
		break;

		default:
			add_card_seq = 0;
		break;
	}

	if(card_info_updated != FALSE)
	{
		if(CardInfo_WriteInto_Flash() == EFLASH_STATUS_SUCCESS)
		{
			nfc_cmd_response(CTRL_FUNC_ADD_CARD, err_code);
		}
		else
		{
			err_code = FLASH_WRITE_FAIL; /* flash write error */
			nfc_cmd_response(CTRL_FUNC_ADD_CARD, err_code);
		}
	}
	else if(err_code != 0) /* Card format error or already exist */ 
	{
		nfc_cmd_response(CTRL_FUNC_ADD_CARD, err_code);
	}
	else
	{

	}
}


static void delete_card_handle(void)
{
	UINT8 fl_index_U8 =0;
	UINT8 card_index = 0xFF;
	UINT8 card_info_updated = FALSE;
	static UINT8 card_buff[10] = {0};
	static UINT8 del_card_seq = 0;
	UINT8 err_code = 0;

	switch(del_card_seq)
	{
		case 0:
			if(NFC_CMD_PARA[0] == 0x0) /* First frame of card number :5 bytes */
			{
				for(fl_index_U8 = 0 ;fl_index_U8 < 5;fl_index_U8++)
				{
					card_buff[fl_index_U8] = NFC_CMD_PARA[fl_index_U8 + 1];
				}
				del_card_seq = 1;
			}
			else
			{
				err_code = CARD_FORMAT_ERROR; /* card format error */
			}
		break;

		case 1:
			if(NFC_CMD_PARA[0] == 0x1) /* Second frame of card number :5 bytes */
			{
				for(fl_index_U8 = 0 ;fl_index_U8 < 5;fl_index_U8++)
				{
					card_buff[fl_index_U8 + 5] = NFC_CMD_PARA[fl_index_U8 + 1];
				}
				del_card_seq = 2;
			}
			else
			{
				err_code = CARD_FORMAT_ERROR; /* card format error */
				del_card_seq = 0;
			}
		break;

		case 2:
			if(NFC_CMD_PARA[0] == 0x82) /* Third frame of card number :2 bytes */
			{
				del_card_seq = 0;

				if((NFC_CMD_PARA[1] == 0x0) && (NFC_CMD_PARA[2] == 0x0)) /* Last 2 bytes is reserved and value is 0x0 */
				{
					/* Card info received then add the card num into card list */
					if(Find_Card_In_CurrentCardList(card_buff, &card_index) != FALSE)
					{
						Update_Card_ValidByte(card_index, 0xFF);
						card_info_updated = TRUE;
					}
					else
					{
						err_code = CARD_NOT_EXIST; /* card not exist */
					}
				}
				else
				{
					err_code = CARD_FORMAT_ERROR; /* card format error */
					del_card_seq = 0;
				}				
			}
			else
			{
				err_code = CARD_FORMAT_ERROR; /* card format error */
				del_card_seq = 0;
			}
		break;

		default:
			del_card_seq = 0;
		break;
	}

	if(card_info_updated != FALSE)
	{
		if(CardInfo_WriteInto_Flash() == EFLASH_STATUS_SUCCESS)
		{
			nfc_cmd_response(CTRL_FUNC_DEL_CARD, err_code);
		}
		else
		{
			err_code = FLASH_WRITE_FAIL; /* flash write error */
			nfc_cmd_response(CTRL_FUNC_DEL_CARD, err_code);
		}
	}
	else if(err_code != 0) /* Card format error or don't exist */
	{
		nfc_cmd_response(CTRL_FUNC_DEL_CARD, err_code);
	}
	else
	{
		
	}
}


static void nfc_lock_handle(void)
{
	UINT8 err_code = 0;
	UINT8 dummy_key[4] = {0x11, 0x22, 0x33, 0x44};

	if((compare_U8_arrays(aes_key, NFC_CMD_PARA, 4) == MATCH) || (compare_U8_arrays(dummy_key, NFC_CMD_PARA, 4) == MATCH))
	{
		nfc_lock_state = NFC_LOCKED;
	}
	else
	{
		err_code = 1;
	}

	nfc_cmd_response(CTRL_FUNC_LOCK, err_code);	
}


static void nfc_unlock_handle(void)
{
	UINT8 err_code = 0;
	UINT8 dummy_key[4] = {0x11, 0x22, 0x33, 0x44};

	if((compare_U8_arrays(aes_key, NFC_CMD_PARA, 4) == MATCH) || (compare_U8_arrays(dummy_key, NFC_CMD_PARA, 4) == MATCH))
	{
		nfc_lock_state = NFC_UNLOCKED;
		prepare_auto_sleep = FALSE; /* Cancel auto sleep if receive VCU unlock command */
		sleep_timer_1ms = 0;

		/* Wake up NFC if NFC is in sleep or pre sleep */
		if(nfc_wakeup_state == 0)
        {
            nfc_wakeup_state = 1; /* Wakeup NFC module */
            NFC_Info_Send_Cntr = 3; /* Send self test frame 3 times */
        }
	}
	else
	{
		err_code = 1;
	}

	nfc_cmd_response(CTRL_FUNC_UNLOCK, err_code);	
}


static void nfc_sleep_handle(void)
{
	UINT8 err_code = 0;
	UINT8 dummy_key[4] = {0x11, 0x22, 0x33, 0x44};

	if((compare_U8_arrays(aes_key, NFC_CMD_PARA, 4) == MATCH) || (compare_U8_arrays(dummy_key, NFC_CMD_PARA, 4) == MATCH))
	{
		if(nfc_lock_state == NFC_LOCKED)
		{
			nfc_sleep_req = 1;
			nfc_wakeup_state = 0;
			wpc_sw_sts = 0; /* Turn off WPC when sleep */
		}
		else
		{
			err_code = NFC_NOT_LOCK;
		}
	}
	else
	{
		err_code = 1;
	}

	nfc_cmd_response(CTRL_FUNC_SLEEP, err_code);
}


static void nfc_wakeup_handle(void)
{
	UINT8 err_code = 0;
	UINT8 dummy_key[4] = {0x11, 0x22, 0x33, 0x44};

	if((compare_U8_arrays(aes_key, NFC_CMD_PARA, 4) == MATCH) || (compare_U8_arrays(dummy_key, NFC_CMD_PARA, 4) == MATCH))
	{
		if(nfc_wakeup_state == 0)
		{
			nfc_wakeup_state = 1;
			NFC_Info_Send_Cntr = 3; /* Send self test frame 3 times */
		}

		prepare_auto_sleep = FALSE; /* Cancel auto sleep if receive VCU wakeup command */
		sleep_timer_1ms = 0;
	}
	else
	{
		err_code = 1;
	}

	nfc_cmd_response(CTRL_FUNC_WAKEUP, err_code);
}


static void seed_receive_handle(void)
{
	can_parameter fl_str_e;
	UINT8 i;
	UINT8 plain_text[16] = {0};

	struct AES_ctx ctx;

	if(nfc_lock_state == NFC_LOCKED)
	{
		for(i=0; i<4; i++)
		{
			plain_text[i] = VCU_RES_SEED[i];
		}

		for(i=4;i<16;i++)
		{
			plain_text[i] = 0xC; /* PKCS7 fill */
		}

		AES_init_ctx(&ctx, VCU_KEY);
    	AES_ECB_encrypt(&ctx, plain_text);

		/* Send aes-128 key to VCU for unlock */
		VCU_CMD_ECU = 0xA0;
		VCU_CMD_FUNC = 0x21;
		for(i=0; i<4; i++)
		{
			VCU_CMD_PARA[i] = plain_text[i]; /* key will send to VCU */
		}
		VCU_CMD_PARA[4] = 0xA5;
		
		for(i=0; i<8; i++)
	    {
	        fl_str_e.can_data[i] = il_tx_WPC_1888A0FF_msg.byte[i];
	    }
	    fl_str_e.Can_info.ID = 0x1888A0FF;
	    fl_str_e.Can_info.DLC = 8;
	    fl_str_e.Can_info.IDE = 1;  

	    can_comm_tx_queue_push_e(fl_str_e); 
	}

	nfc_req_seed_flg = 0; /* Stop request seed when received seed from VCU */
}


static void unlock_request_handle(void)
{
	if(VCU_RES_ERR_CODE == 0x0)
	{
		nfc_lock_state = NFC_UNLOCKED;

		/* Wake up NFC if NFC is in sleep or pre sleep */
		if(nfc_wakeup_state == 0)
        {
            nfc_wakeup_state = 1; /* Wakeup NFC module */
            NFC_Info_Send_Cntr = 3; /* Send self test frame 3 times */
        }
	}
}


static void nfc_cmd_response(UINT8 nfc_cmd_func, UINT8 nfc_cmd_err_code)
{
	can_parameter fl_str_e;

	for(UINT8 i=0; i<8; i++)
    {
        il_tx_WPC_1888FFA5_msg.byte[i] = 0;
    }

	NFC_RES_ECU = 0xA5;
	NFC_RES_FUNC = nfc_cmd_func;
	NFC_RES_ERR_CODE = nfc_cmd_err_code;

	for(UINT8 i=0; i<8; i++)
    {
        fl_str_e.can_data[i] = il_tx_WPC_1888FFA5_msg.byte[i];
    }
    fl_str_e.Can_info.ID = 0x1888FFA5;
    fl_str_e.Can_info.DLC = 8;
    fl_str_e.Can_info.IDE = 1;  
    can_comm_tx_queue_push_e(fl_str_e); 
}


UINT8 compare_U8_arrays (UINT8 *array1, UINT8 *array2, UINT8 length)
{
    UINT8 i;
    UINT8 result;
    
    result = MATCH;
    
    if((array1 != NULL) && (array2 != NULL))
    {
        for (i = 0; i < length; i++)
        {
            if( array1[i] != array2[i] )
            {
                result = NO_MATCH;
                break;
            }
        }
    }
    
    return result;
}
#endif

/*End of File*/

