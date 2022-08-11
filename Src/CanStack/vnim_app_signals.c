
#include "system.h"
#include "vnim_types.h"
#include "vnim_if.h"
#include "il_inc.h" 
#include "vnim_app_signals.h"
#include "fdiag.h"
#include "CanNmCtrl.h"

BOOLEAN msg_123_missing = FALSE;
BOOLEAN msg_DDSS_never_recvd = TRUE;
BOOLEAN msg_234_missing = FALSE;
BOOLEAN msg_VIST_never_recvd = TRUE;
BOOLEAN msg_3B7_missing = FALSE;
BOOLEAN msg_3B7_never_recvd = TRUE;
BOOLEAN msg_458_missing = FALSE;
BOOLEAN msg_458_never_recvd = TRUE;

static uint8_t Msg_Alive_Counter = 0;

void vnim_set_init_signals(void)
{
    msg_123_missing = FALSE;
    msg_DDSS_never_recvd = TRUE;
    msg_234_missing = FALSE;
    msg_VIST_never_recvd = TRUE;
    msg_3B7_missing = FALSE;
    msg_3B7_never_recvd = TRUE;
    msg_458_missing = FALSE;
    msg_458_never_recvd = TRUE;

}


/* IL Message Timeout functions */   
void Vnim_VCU_123_Timeout(void)
{
    msg_123_missing = TRUE;

    return;
}


/* IL Message Timeout functions */   
void Vnim_ABM_234_Timeout(void)
{
    msg_234_missing = TRUE;

    return;
}

/* IL Message Timeout functions */   
void Vnim_IHU_3B7_Timeout(void)
{
    msg_3B7_missing = TRUE;

    return;
}

/* IL Message Timeout functions */   
void Vnim_BCM_458_Timeout(void)
{
    msg_458_missing = TRUE;

    return;
}



void SCM_MsgAliveCounter_Fun(void)
{
    if( Msg_Alive_Counter < 14 )
    {
        Msg_Alive_Counter ++;
    }
    else
    {
        Msg_Alive_Counter = 0;
    }
}

void Vnim_SCM_Send_notify_handler(void)
{
    if(Dis_SCM_HMI_Req_Count > 0)
    {
        Dis_SCM_HMI_Req_Count --;
    }
    
    if( Dis_SCM_HMI_Req_Count == 0 )
    {
        Dis_SCM_HMI_Req = 0;
    }
    /* Alive Counter */
    SCM_MsgAliveCounter_Fun();
}

uint8_t Get_MsgAlive_Counter(void)
{
    return (Msg_Alive_Counter);
}


void vnim_disable_tx (void)
{
    IlTxSuspend();
    vnim_disable_tp_tx();
	CanNmCtrl_ReqNm_State(0,NM_CLOSE);

}

void vnim_enable_tx (void)
{
    IlTxResume();
    vnim_enable_tp_tx();
	CanNmCtrl_ReqNm_State(0,NM_OPEN);
}
