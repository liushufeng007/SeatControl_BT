#include   "CanNmCtrl.h"
#include   "Canif.h"
#include   "string.h"
#include   "CanNm_Cfg.h"
#include   "CanNm.h"
#include   "fm33_assert.h"

/***************************************************************
copyright from private LiuXiao
if you have any question,you can contact me by email 461445092@qq.com
2022-04-20 21:46:31.770078
***************************************************************/


CanNmCtrl_MainCtrl_Str CanNmCtrl_MainCtrl[CanNmMax_CH];
CanNmCtrl_Msg_Un       CanNmCtrl_Tx_Buf[CanNmMax_CH];
extern CanNmMain_State_e CanNm_mainctrl_status[CanNmMax_CH ];


/***************************CanNmCtrl_init begin************************************/
void CanNmCtrl_init(void)
{
	memset(&CanNmCtrl_MainCtrl[0],0,sizeof(CanNmCtrl_MainCtrl[0]));
	memset(&CanNmCtrl_Tx_Buf[0],0,sizeof(CanNmCtrl_Tx_Buf[0]));
	CanNmCtrl_Tx_Buf[0].str.SourceID = CANNMCTRL_TX_CANNM_SRC_ID_L;
}


/***************************begin************************************/
void CanNmCtrl_ReqNm_State(STATE_CTRL_UINT8 can_ch,CanNmCtrl_NmMsg_State_e State)
{
	assert_param(can_ch < CanNmMax_CH);
	if(NM_CLOSE != State)
	{
		CanNmCtrl_MainCtrl[can_ch].NmMsgState = NM_OPEN;
	}
	else
	{
		CanNmCtrl_MainCtrl[can_ch].NmMsgState = NM_CLOSE;
	}
}
void CanNmCtrl_Req_NetWork(STATE_CTRL_UINT8 can_ch)
{
	assert_param(can_ch < CanNmMax_CH);
	CanNmCtrl_MainCtrl[can_ch].Request = TRUE;
}



/***************************begin************************************/
void CanNmCtrl_Release_NetWork(STATE_CTRL_UINT8 can_ch)
{
	assert_param(can_ch < CanNmMax_CH);

	CanNmCtrl_MainCtrl[can_ch].Request = FALSE;
}

/***************************begin************************************/

void CanNmCtrl_Set_ActiveWkupBit(STATE_CTRL_UINT8 can_ch,CanNmCtrl_NM_WakeUpBit_e WkupSrc)
{
	assert_param(can_ch < CanNmMax_CH);

	CanNmCtrl_Tx_Buf[can_ch].str.Active_Wkup_Bit = WkupSrc;
}

/***************************begin************************************/

void CanNmCtrl_Set_ActiveWkupSrc(STATE_CTRL_UINT8 can_ch,CanNmCtrl_NM_WakeUpSrc_Ch_e ch ,CanNmCtrl_NM_WakeUp_e WkupSrc)
{
	assert_param(can_ch < CanNmMax_CH);

	switch(ch)
	{

		case WKUPSRC_IGN:
			CanNmCtrl_Tx_Buf[can_ch].str.Ign_Wkup = WkupSrc;
		break;
		
		case WKUPSRC_RESET:
			CanNmCtrl_Tx_Buf[can_ch].str.Rst_Wkup = WkupSrc;
		break;
		
		case WKUPSRC_NETWORK:
			CanNmCtrl_Tx_Buf[can_ch].str.NetWk_Wkup = WkupSrc;
		break;
		
		case WKUPSRC_ECU_SPEC:
			CanNmCtrl_Tx_Buf[can_ch].str.ECU_Spec_Wkup = WkupSrc;
		break;
		
		case KEEPSRC_IGN:
			CanNmCtrl_Tx_Buf[can_ch].str.Ign_Awake = WkupSrc;
		break;
		
		case KEEPSRC_DIAG:
			CanNmCtrl_Tx_Buf[can_ch].str.Diag_Awake = WkupSrc;
		break;
		
		case KEEPSRC_NETWORK:
			CanNmCtrl_Tx_Buf[can_ch].str.NetWk_Awake = WkupSrc;
		break;
		
		case KEEPSRC_ECU_SPEC:
			CanNmCtrl_Tx_Buf[can_ch].str.Ecu_Spec_Awake = WkupSrc;
		break;
	}

}

/***************************begin************************************/
STATE_CTRL_UINT8 CanNmCtrl_Get_AppMsgIsOpen(STATE_CTRL_UINT8 can_ch)
{
	assert_param(can_ch < CanNmMax_CH);
	if(CanNm_mainctrl_status[can_ch] == RepeatMsg_STATE_E_1)
	{
		return TRUE;
	}
	if(CanNm_mainctrl_status[can_ch] == NormalOperate_STATE_E_2)
	{
		return TRUE;
	}
	return FALSE;
}


/***************************begin************************************/
void CanNmCtrl_NmRx_Indication(STATE_CTRL_UINT8 can_ch,STATE_CTRL_UINT8 * CanMsgData)
{
	assert_param(can_ch < CanNmMax_CH);

	memcpy((STATE_CTRL_UINT8 *)CanNmCtrl_MainCtrl[can_ch].Msg.data,(STATE_CTRL_UINT8 *)CanMsgData,8);
	CanNmCtrl_MainCtrl[can_ch].NmRx_Flag = TRUE;
	CanNmCtrl_MainCtrl[can_ch].NmTimerOut_ticks = CANNM_NM_TIMEOUT_TIMER;
}


/***************************begin************************************/
void CanNmCtrl_NmTx_Msg(STATE_CTRL_UINT8 can_ch)
{
	Canif_Msg_Str CanMsg;
	assert_param(can_ch < CanNmMax_CH);
	if(CanNmCtrl_MainCtrl[can_ch].NmMsgState == NM_OPEN)
	{
		CanNmCtrl_MainCtrl[can_ch].NmTimerOut_ticks = CANNM_NM_TIMEOUT_TIMER;
	CanMsg.StdId = CANNMCTRL_TX_CANNM_ID_L;
	CanMsg.DLC = CANNMCTRL_TX_CANNM_DLC;
	memcpy((STATE_CTRL_UINT8 *)CanMsg.Data,(STATE_CTRL_UINT8 *)CanNmCtrl_Tx_Buf[can_ch].data,8);
	Canif_tx_queue_push_e(CanMsg);	
	}
}


CanNmMain_State_e CanNmCtrl_Get_CanNm_State(STATE_CTRL_UINT8 can_ch)
{
	assert_param(can_ch < CanNmMax_CH);
	return CanNm_mainctrl_status[can_ch];
}





