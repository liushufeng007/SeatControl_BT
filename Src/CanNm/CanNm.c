#include   "CanNm.h"
/***************************************************************
copyright from private LiuXiao
if you have any question,you can contact me by email 461445092@qq.com
2022-04-20 21:46:31.770078
***************************************************************/
#include "CanNmCtrl.h"
#include "CanNm_Cfg.h"
/***************************************************************/
CanNmMain_State_e CanNm_mainctrl_status[CanNmMax_CH ];
CanNmMain_State_e CanNm_lastmainctrl_status[CanNmMax_CH ];
extern 	CanNmCtrl_MainCtrl_Str CanNmCtrl_MainCtrl[CanNmMax_CH];
extern CanNmCtrl_Msg_Un       CanNmCtrl_Tx_Buf[CanNmMax_CH];
/***************************************************************/


/***************************static function statement begin************************************/
static CanNmMain_State_e CanNm_handle_entry_BusSleep(STATE_UINT8 channel);
static CanNmMain_State_e CanNm_handle_during_BusSleep(STATE_UINT8 channel);
static CanNmMain_State_e CanNm_handle_exit_BusSleep(STATE_UINT8 channel);


static CanNmMain_State_e CanNm_handle_entry_RepeatMsg(STATE_UINT8 channel);
static CanNmMain_State_e CanNm_handle_during_RepeatMsg(STATE_UINT8 channel);
static CanNmMain_State_e CanNm_handle_exit_RepeatMsg(STATE_UINT8 channel);


static CanNmMain_State_e CanNm_handle_entry_NormalOperate(STATE_UINT8 channel);
static CanNmMain_State_e CanNm_handle_during_NormalOperate(STATE_UINT8 channel);
static CanNmMain_State_e CanNm_handle_exit_NormalOperate(STATE_UINT8 channel);


static CanNmMain_State_e CanNm_handle_entry_ReadySleep(STATE_UINT8 channel);
static CanNmMain_State_e CanNm_handle_during_ReadySleep(STATE_UINT8 channel);
static CanNmMain_State_e CanNm_handle_exit_ReadySleep(STATE_UINT8 channel);


static CanNmMain_State_e CanNm_handle_entry_PrepareBusSleep(STATE_UINT8 channel);
static CanNmMain_State_e CanNm_handle_during_PrepareBusSleep(STATE_UINT8 channel);
static CanNmMain_State_e CanNm_handle_exit_PrepareBusSleep(STATE_UINT8 channel);


/***************************static function statement end  ************************************/




/***************************table begin************************************/
const CanNmState_Handle_Ptr CanNm_handle_func_tbl[5][3] = 
 {
    (CanNmState_Handle_Ptr) CanNm_handle_entry_BusSleep,
    (CanNmState_Handle_Ptr) CanNm_handle_during_BusSleep,
    (CanNmState_Handle_Ptr) CanNm_handle_exit_BusSleep,
    (CanNmState_Handle_Ptr) CanNm_handle_entry_RepeatMsg,
    (CanNmState_Handle_Ptr) CanNm_handle_during_RepeatMsg,
    (CanNmState_Handle_Ptr) CanNm_handle_exit_RepeatMsg,
    (CanNmState_Handle_Ptr) CanNm_handle_entry_NormalOperate,
    (CanNmState_Handle_Ptr) CanNm_handle_during_NormalOperate,
    (CanNmState_Handle_Ptr) CanNm_handle_exit_NormalOperate,
    (CanNmState_Handle_Ptr) CanNm_handle_entry_ReadySleep,
    (CanNmState_Handle_Ptr) CanNm_handle_during_ReadySleep,
    (CanNmState_Handle_Ptr) CanNm_handle_exit_ReadySleep,
    (CanNmState_Handle_Ptr) CanNm_handle_entry_PrepareBusSleep,
    (CanNmState_Handle_Ptr) CanNm_handle_during_PrepareBusSleep,
    (CanNmState_Handle_Ptr) CanNm_handle_exit_PrepareBusSleep,
};

/***************************table end  ************************************/




/***************************static function define begin************************************/



/***************************BusSleep_entry***********************************/
static CanNmMain_State_e CanNm_handle_entry_BusSleep(STATE_UINT8 channel)
{
/*please fill your code*/
    return BusSleep_STATE_E_0;
}


/***************************BusSleep_during***********************************/
static CanNmMain_State_e CanNm_handle_during_BusSleep(STATE_UINT8 channel)
{
/*please fill your code*/
	if(CanNmCtrl_MainCtrl[channel].Request == TRUE)
	{
		CanNmCtrl_Tx_Buf[channel].str.Active_Wkup_Bit = WKUP_LOCAL;
		CanNmCtrl_Tx_Buf[channel].str.NM_State = REPEAT_MSG_FROM_BUS_SLP;
		return RepeatMsg_STATE_E_1;
	}
	if(CanNmCtrl_MainCtrl[channel].NmRx_Flag == TRUE)
	{
		CanNmCtrl_MainCtrl[channel].NmRx_Flag = FALSE;
		CanNmCtrl_Tx_Buf[channel].str.Active_Wkup_Bit = WKUP_PASSIVE;
		CanNmCtrl_Tx_Buf[channel].str.NM_State = REPEAT_MSG_FROM_BUS_SLP;
		return RepeatMsg_STATE_E_1;
	}

	CANNMCTRL_CLOSE_APP_MSG();
	
    return BusSleep_STATE_E_0;
}


/***************************BusSleep_exit***********************************/
static CanNmMain_State_e CanNm_handle_exit_BusSleep(STATE_UINT8 channel)
{
/*please fill your code*/
    return BusSleep_STATE_E_0;
}


/***************************RepeatMsg_entry***********************************/
static CanNmMain_State_e CanNm_handle_entry_RepeatMsg(STATE_UINT8 channel)
{
/*please fill your code*/
	CanNmCtrl_MainCtrl[channel].Delay_ticks = CANNM_REPEAT_MSG_TIMER;
	CanNmCtrl_MainCtrl[channel].NmTx_Times = CANNM_IMMDIATE_TIMES-1;
	CanNmCtrl_MainCtrl[channel].Interval_Ticks = CANNM_IMMDIATE_CYCLE_TIMER;
	CanNmCtrl_NmTx_Msg(channel);
	
	CANNMCTRL_OPEN_APP_MSG();
    return BusSleep_STATE_E_0;
}


/***************************RepeatMsg_during***********************************/
static CanNmMain_State_e CanNm_handle_during_RepeatMsg(STATE_UINT8 channel)
{
/*please fill your code*/
	if(CanNmCtrl_MainCtrl[channel].Delay_ticks > 0)
	{
		CanNmCtrl_MainCtrl[channel].Delay_ticks --;
	}
	else
	{
		if(CanNmCtrl_MainCtrl[channel].Request == TRUE)
		{
			CanNmCtrl_Tx_Buf[channel].str.NM_State = NORMAL_FROM_REPEAT_MSG;
			return NormalOperate_STATE_E_2;
		}
		else
		{
			return ReadySleep_STATE_E_3;
		}
	}

	if(CanNmCtrl_MainCtrl[channel].NmTx_Times > 0)
	{
		if(CanNmCtrl_MainCtrl[channel].Interval_Ticks > 0)
		{
			CanNmCtrl_MainCtrl[channel].Interval_Ticks --;
		}
		else
		{
			CanNmCtrl_NmTx_Msg(channel);
			CanNmCtrl_MainCtrl[channel].NmTx_Times--;
			if(CanNmCtrl_MainCtrl[channel].NmTx_Times == 0)
			{
				CanNmCtrl_MainCtrl[channel].Interval_Ticks = CANNM_NM_CYCLE_TIMER;	
			}
			else
			{
				CanNmCtrl_MainCtrl[channel].Interval_Ticks = CANNM_IMMDIATE_CYCLE_TIMER;
			}
		}
	}
	else
	{
		if(CanNmCtrl_MainCtrl[channel].Interval_Ticks > 0)
		{
			CanNmCtrl_MainCtrl[channel].Interval_Ticks --;
		}
		else
		{
			CanNmCtrl_MainCtrl[channel].Interval_Ticks = CANNM_NM_CYCLE_TIMER;			
			CanNmCtrl_NmTx_Msg(channel);
		}
	}

	if(CanNmCtrl_MainCtrl[channel].NmTimerOut_ticks > 0)
	{
		CanNmCtrl_MainCtrl[channel].NmTimerOut_ticks --;
	}

	if(CanNmCtrl_MainCtrl[channel].NmTimerOut_ticks == 0)
	{
		CanNmCtrl_MainCtrl[channel].NmTimerOut_ticks = CANNM_NM_TIMEOUT_TIMER;
	}

    return RepeatMsg_STATE_E_1;
}


/***************************RepeatMsg_exit***********************************/
static CanNmMain_State_e CanNm_handle_exit_RepeatMsg(STATE_UINT8 channel)
{
/*please fill your code*/
    return BusSleep_STATE_E_0;
}


/***************************NormalOperate_entry***********************************/
static CanNmMain_State_e CanNm_handle_entry_NormalOperate(STATE_UINT8 channel)
{
/*please fill your code*/

    return BusSleep_STATE_E_0;
}


/***************************NormalOperate_during***********************************/
static CanNmMain_State_e CanNm_handle_during_NormalOperate(STATE_UINT8 channel)
{
/*please fill your code*/
	if(CanNmCtrl_MainCtrl[channel].NmRx_Flag == TRUE)
	{
		CanNmCtrl_MainCtrl[channel].NmRx_Flag = FALSE;
	}

	if(CanNmCtrl_MainCtrl[channel].Request == FALSE)
	{
		return ReadySleep_STATE_E_3;
	}

	if(CanNmCtrl_MainCtrl[channel].Interval_Ticks > 0)
	{
		CanNmCtrl_MainCtrl[channel].Interval_Ticks --;
	}
	else
	{
		CanNmCtrl_MainCtrl[channel].Interval_Ticks = CANNM_NM_CYCLE_TIMER;			
		CanNmCtrl_NmTx_Msg(channel);
	}

	if(CanNmCtrl_MainCtrl[channel].NmTimerOut_ticks > 0)
	{
		CanNmCtrl_MainCtrl[channel].NmTimerOut_ticks --;
	}

	if(CanNmCtrl_MainCtrl[channel].NmTimerOut_ticks == 0)
	{
		CanNmCtrl_MainCtrl[channel].NmTimerOut_ticks = CANNM_NM_TIMEOUT_TIMER;
	}


    return NormalOperate_STATE_E_2;
}


/***************************NormalOperate_exit***********************************/
static CanNmMain_State_e CanNm_handle_exit_NormalOperate(STATE_UINT8 channel)
{
/*please fill your code*/
    return BusSleep_STATE_E_0;
}


/***************************ReadySleep_entry***********************************/
static CanNmMain_State_e CanNm_handle_entry_ReadySleep(STATE_UINT8 channel)
{
/*please fill your code*/
    return BusSleep_STATE_E_0;
}


/***************************ReadySleep_during***********************************/
static CanNmMain_State_e CanNm_handle_during_ReadySleep(STATE_UINT8 channel)
{
/*please fill your code*/
	if(CanNmCtrl_MainCtrl[channel].Request == TRUE)
	{
		CanNmCtrl_MainCtrl[channel].Interval_Ticks = CANNM_NM_CYCLE_TIMER;	
		CanNmCtrl_NmTx_Msg(channel);
		CanNmCtrl_Tx_Buf[channel].str.NM_State = NORMAL_FROM_READY_SLE;
		return NormalOperate_STATE_E_2;
	}

	if(CanNmCtrl_MainCtrl[channel].NmRx_Flag == TRUE)
	{
		CanNmCtrl_MainCtrl[channel].NmRx_Flag = FALSE;
	}
	else
	{
		if(CanNmCtrl_MainCtrl[channel].NmTimerOut_ticks > 0)
		{
			CanNmCtrl_MainCtrl[channel].NmTimerOut_ticks --;
		}
		else
		{
			return PrepareBusSleep_STATE_E_4;
		}
	}

    return ReadySleep_STATE_E_3;
}


/***************************ReadySleep_exit***********************************/
static CanNmMain_State_e CanNm_handle_exit_ReadySleep(STATE_UINT8 channel)
{
/*please fill your code*/
    return BusSleep_STATE_E_0;
}


/***************************PrepareBusSleep_entry***********************************/
static CanNmMain_State_e CanNm_handle_entry_PrepareBusSleep(STATE_UINT8 channel)
{
/*please fill your code*/
	CanNmCtrl_MainCtrl[channel].Delay_ticks = CANNM_WAIT_BUS_SLEEP_TIMER;
	CANNMCTRL_CLOSE_APP_MSG();
    return BusSleep_STATE_E_0;
}


/***************************PrepareBusSleep_during***********************************/
static CanNmMain_State_e CanNm_handle_during_PrepareBusSleep(STATE_UINT8 channel)
{
/*please fill your code*/
	if(CanNmCtrl_MainCtrl[channel].Request == TRUE)
	{
		CanNmCtrl_Tx_Buf[channel].str.NM_State = REPEAT_MSG_FROM_PREPARE_SLP;
		return RepeatMsg_STATE_E_1;
	}

	if(CanNmCtrl_MainCtrl[channel].NmRx_Flag == TRUE)
	{
		CanNmCtrl_MainCtrl[channel].NmRx_Flag = FALSE;
		CanNmCtrl_Tx_Buf[channel].str.NM_State = REPEAT_MSG_FROM_PREPARE_SLP;
		return RepeatMsg_STATE_E_1;
	}
	else
	{
		if(CanNmCtrl_MainCtrl[channel].Delay_ticks > 0)
		{
			CanNmCtrl_MainCtrl[channel].Delay_ticks --;
		}
		else
		{
			return BusSleep_STATE_E_0;
		}
	}
    return PrepareBusSleep_STATE_E_4;
}


/***************************PrepareBusSleep_exit***********************************/
static CanNmMain_State_e CanNm_handle_exit_PrepareBusSleep(STATE_UINT8 channel)
{
/*please fill your code*/
    return BusSleep_STATE_E_0;
}


/***************************static function define end  ************************************/




/***************************init function define begin************************************/
void  CanNm_Init(void)
{
    STATE_UINT32 fl_index_u32 = 0;  

    for(fl_index_u32 = 0 ; fl_index_u32 < CanNmMax_CH ; fl_index_u32++)
    {
        CanNm_mainctrl_status[fl_index_u32]=BusSleep_STATE_E_0;
        CanNm_lastmainctrl_status[fl_index_u32]=BusSleep_STATE_E_0;
    }

	CanNmCtrl_init();

}
/***************************init function define end  ************************************/




/***************************main function define begin************************************/
void  CanNm_MainTask(void)
{
    STATE_UINT32 fl_index_u32 = 0;  

    for(fl_index_u32 = 0 ; fl_index_u32 < CanNmMax_CH ; fl_index_u32++)
    {

        if(CanNm_mainctrl_status[fl_index_u32] <= PrepareBusSleep_STATE_E_4)
        {
            CanNm_mainctrl_status[fl_index_u32]=CanNm_handle_func_tbl[CanNm_mainctrl_status[fl_index_u32]][1](fl_index_u32);
        }
        else
        {
            CanNm_mainctrl_status[fl_index_u32]=BusSleep_STATE_E_0;
        }
    

        if(CanNm_mainctrl_status[fl_index_u32] <= PrepareBusSleep_STATE_E_4)
        {
            if(CanNm_lastmainctrl_status[fl_index_u32] != CanNm_mainctrl_status[fl_index_u32])
            {
                (void)CanNm_handle_func_tbl[CanNm_lastmainctrl_status[fl_index_u32]][2](fl_index_u32);
            }
        }
        else
        {
            CanNm_mainctrl_status[fl_index_u32]=BusSleep_STATE_E_0;
        }
    

        if(CanNm_mainctrl_status[fl_index_u32] <= PrepareBusSleep_STATE_E_4)
        {
            if(CanNm_lastmainctrl_status[fl_index_u32] != CanNm_mainctrl_status[fl_index_u32])
            {
                (void)CanNm_handle_func_tbl[CanNm_mainctrl_status[fl_index_u32]][0](fl_index_u32);
            }
        }
        else
        {
            CanNm_mainctrl_status[fl_index_u32]=BusSleep_STATE_E_0;
        }
		
		CanNm_lastmainctrl_status[fl_index_u32]=CanNm_mainctrl_status[fl_index_u32];
    }
    


}
/***************************main function define end  ************************************/


