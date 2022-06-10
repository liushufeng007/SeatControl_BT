

/* ===========================================================================
//
//  Name:           vnim_di_nos.c
//
//  Description:    VNIM/DINOS Interface Functions
//
//  Organization:   Multiplex Core Technology
//
// =========================================================================*/
#include "il.h"
#include "il_par.h"
#include "vnim_app_signals.h"
#include "nw_can_dll.h"

#include "vnim_types.h"
#include "vnim_if.h"
#include "vnim_net.h"

#include "vnim_busoff_hndlr.h"

#include "vnim_can_cfg.h"

#include "tp.h"
#include "kwpdiag.h"
#include "system.h"
#include "fdiag.h"


#if CAN_ADAPTIVE_ENABLE == 1
extern vnim_unsigned8 can_adaptive_flag;
#endif

static vnim_unsigned8 nw_can_init_sts = 0;
static vnim_unsigned8 nw_can_init_mode = 0;
static vnim_unsigned8 nw_can_tp_transmit_sts;

/*Add for A90*/
UINT8 f_network_wakeup_flag(void)
{

return 0;
}

extern vnim_unsigned8 f_network_wakeup_flag(void);
extern void vnim_set_init_signals(void);
#define SHUTDOWN_RETRY_MAX_CNT  5

/* ===========================================================================
//
// Name:            NW_Initialize
//
// Description:     Initialize the Vector FNOS Stack
//
// Inputs:          mode: Indicates Bootstrap Initialization Mode
//
// Returns:         none
//
// =========================================================================*/
VNIM_BOOL
NW_Initialize(vnim_bootstrap_mode_type const mode)
{
//	vnim_unsigned8 init_sts = 0xFF;


	nw_can_init_sts = VNIM_TRUE;

	if((mode == VNIM_COLD_BOOT) || (mode == VNIM_WARM_BOOT))
	{      
	IlInit();

	DllInit();

	VTP_Init();

	kwpdiag_init();

	vnim_set_init_signals();

	vnim_bus_off_hndlr_init();
	}
	else if (mode == VNIM_BUSOFF_RECOVERY_INIT)
	{
		IlResume();
	}
	else
	{
	}

    return (nw_can_init_sts);

}

/* ===========================================================================
//
// Name:            NW_Disable
//
// Description:     Disable the NOS Stack
//
//                  This function is used to disable the NOS stack.
//                  NOS CAN functionality can only be restored after
//                  calling this function by re-initializing the stack.
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
VNIM_BOOL
NW_Disable(void)
{
    vnim_unsigned8 ret_sts;


    if(ret_sts == (vnim_unsigned8)CANRC_OK)
    {
        ret_sts = VNIM_TRUE;
    }
    else
    {
   //     nw_srvc_in_progress = CANSV_POWER_DOWN;
        ret_sts = VNIM_FALSE;
    }

    return(ret_sts);
}

/* ===========================================================================
//
// Name:            NW_Shutdown
//
// Description:     Shut Down the NOS Stack
//
//                  This function is used to Shutdown the NOS stack.
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
VNIM_BOOL
NW_Shutdown(void)
{
    vnim_unsigned8 ret_sts;

	ret_sts = VNIM_FALSE;
	 
//    can_sts = CanGetComStatus(0);

 //   if(can_sts != CANB_COMST_SLEEP)
    {
    //    ret_sts = (vnim_unsigned8) CanService(CANSV_SLEEP,0);
    }
    
    if(ret_sts == (vnim_unsigned8)CANRC_OK)
    {
      /* Put the CAN Transceiver in the Sleep State */
        //Can_BusTransceiverToStandby();
        ret_sts = VNIM_TRUE;
    }
    else
    {
//        nw_srvc_in_progress = CANSV_SLEEP;
        ret_sts = VNIM_FALSE;
    }
    return(ret_sts);
}

/* ===========================================================================
//
// Name:            NW_OpenConnection
//
// Description:     Open the NOS Network Connection .Enable CAn Txion/rxion
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
NW_OpenConnection(void)
{
//    (void)CanService(CANSV_RESUME_ALL, 0);
    IlResume();
}

/* ===========================================================================
//
// Name:            NW_CloseConnection
//
// Description:     Close the NOS Network Connection .Disable CAn Txion/rxion
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
NW_CloseConnection(void)
{
//    (void) CanService(CANSV_SUSPEND_ALL,0);
 //   (void) CanService(CANSV_TX_FLUSH, 0);
    IlSuspend();
}
/**************************************************************************************************

 Name:            Network_TxSuspend

 Description:     This function is used to disable messages transmission.

 Inputs:          none

 Returns:         none

**************************************************************************************************/

void Network_TxSuspend(void)
{


    vnim_disable_tx();

    //IlDisableTxPeriodic(IL_TX_MESSAGE_HNDL_270);
	//IlDisableTxPeriodic(IL_TX_MESSAGE_HNDL_260);/*qyin add*/
	
    //kwpdiag_TxSuspend();

    /* Disable Interrupts */
   // saveCCR = //CAN_ENTER_CRITICAL_SECTION();

    /* Disable transmission on the Bus */
   // (void) CanService(CANSV_SUSPEND_TX,0);

    /* Flush Pending Transmit Requests */
  //  (void) CanService(CANSV_TX_FLUSH, CANH_MV_ALL);

    /* Enable Interrupts */
 //   //CAN_EXIT_CRITICAL_SECTION (saveCCR);

    /*#if (USING_FNOS_CONNECTION_MANAGER == YES)
        vnim_cm_disconnect_all_nodes();
    #endif

    Vnim_Disable_Receive_Msg_Timeout();*/
}
/**************************************************************************************************

 Name:            Network_TxResume

 Description:     This function is used to enable messages transmission.

 Inputs:          none

 Returns:         none

**************************************************************************************************/

void Network_TxResume(void)
{
    vnim_enable_tx();
}
/* ===========================================================================
//
// Name:            NW_Task
//
// Description:     Periodic NOS Task
//
//                  Periodic task that is used to call all of the individual
//                  NOS layer periodic tasks.
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
NW_Task(void)
{
    static vnim_unsigned8 per_cnt = 0;
    static vnim_unsigned8 twenty_msec_cnt = 0;
    
    //static vnim_signed16 shutdown_retry_cnt = 0;

    VNIM_BOOL ret_sts = VNIM_FALSE;

    response_11_01();

    if(nw_can_init_sts == FALSE)
    {
        /* (void) NW_Initialize(nw_can_init_mode); */
        ret_sts = NW_Initialize((vnim_bootstrap_mode_type)nw_can_init_mode);  
        if(ret_sts == VNIM_TRUE)
        {
            NW_OpenConnection();
        }
    }
    if(nw_can_init_sts == VNIM_TRUE)
    {
   
       // shutdown_retry_cnt = 0;

        DllRxTask();

        IlRxTask();
        
        #if (CAN_ADAPTIVE_ENABLE == 1)
        if(can_adaptive_flag != 0)
        {
            IlTxTask();
        }
        #else
        IlTxTask();
        #endif
        
  		VTP_PeriodicTask();

        per_cnt++;

        twenty_msec_cnt++;

        if(twenty_msec_cnt >= (KWPDIAG_CALLCYCLE_INMSEC/VNIM_PERIODIC_TASK_IN_MSEC))
        {
            twenty_msec_cnt = 0;

            kwpdiag_periodic();
        }

        if(per_cnt >= (VNIM_BUSOFF_SCHEDULER_IN_MS/VNIM_PERIODIC_TASK_IN_MSEC))
        {
            per_cnt = 0 ;
            vnim_bus_off_hndlr_periodic();
        }
    }
}

void Appl_Can_Wakeup (void)
{

}

void vnim_enable_tp_tx(void)
{
	if(nw_can_tp_transmit_sts == VNIM_FALSE)
	{
		VTP_Init();
      	nw_can_tp_transmit_sts = VNIM_TRUE;
   	}
}

void vnim_disable_tp_tx(void)
{
   nw_can_tp_transmit_sts = VNIM_FALSE;
}

vnim_unsigned8 can_init_diag_is_fault(void)
{
	return(nw_can_init_sts);
}


