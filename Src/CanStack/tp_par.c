/* ===========================================================================
//
//  Name:           nw_tp_par.c
//
//  Description:    Transport Layer
//
//  Organization:   Network Subsystems
//
// =========================================================================*/
/* ===========================================================================
//  I N C L U D E   F I L E S
// =========================================================================*/
#include "tp_cfg.h"
#include "tp.h"
#include "tp_par.h"
#include "nw_can_dll.h"
#include "system.h"


#if (VTP_NUM_CHANNEL_COUNT > 1)
static VTP_UINT8*       VTPmc_RxDataptr[VTP_NUM_CHANNEL_COUNT];
static VTP_UINT16       VTPmc_Datasize[VTP_NUM_CHANNEL_COUNT];

#define __ApplTpRxIndication( datLen)       (ApplVTP_rx_indication[channel](datLen))
#define __ApplTpRxErrorIndication(errNo)    (ApplVTP_rx_error_indication[channel](errNo))
#define __ApplTpTxConfirmation(state)       (ApplVTP_tx_confirmation[channel](state))
#define __ApplTpRxGetBuffer( datLen)        (ApplVTP_rx_get_buffer[channel](datLen))
#define __ApplTpTxErrorIndication( errNo)   (ApplVTP_tx_error_indication[channel](errNo))
#else
#define __ApplTpRxIndication( datLen)       (kwpdiag_physicalreqind(datLen))
#define __ApplTpRxErrorIndication(errNo)    (kwpdiag_rxerrorind(errNo))
#define __ApplTpTxConfirmation(state)       (kwpdiag_confirmation(state))
#define __ApplTpRxGetBuffer( datLen)        (kwpdiag_physicalgetbuffer(datLen))
#define __ApplTpTxErrorIndication( errNo)   (kwpdiag_txerrorind(errNo))
static VTP_UINT8*       VTP_RxDataptr;
static VTP_UINT16       VTP_Datasize;
#endif

#define __ApplTpFuncGetBuffer  kwpdiag_functionalgetbuffer
#define __ApplTpFuncIndication kwpdiag_functionalreqind

/*temporary variables to be removed after Multi channel TP implementation*/
static VTP_UINT8        VTP_Funcdiag_req;
VTP_UINT8 VTP_Diag_PhyID_Flag = TRUE;

#if (VTP_NUM_CHANNEL_COUNT > 1)

VTP_UINT8 const VTPmc_Message_Handle[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_MESSAGE_HANDLE
};

tp_tx_confirmation_func_t const ApplVTP_tx_confirmation[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_TX_CONFIRMATION_FUNC
};
tp_tx_error_ind_func_t    const ApplVTP_tx_error_indication[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_RX_ERROR_IND_FUNC
};
tp_rx_error_ind_func_t    const ApplVTP_rx_error_indication[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_TX_ERROR_IND_FUNC
};
tp_rx_indication_func_t   const ApplVTP_rx_indication[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_RX_IND_FUNC
};
tp_rx_getbuffer_func_t    const ApplVTP_rx_get_buffer[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_RX_GET_BUFFER_FUNC
};

static CAN_TMD const tp_resp_tmd [VTP_NUM_CHANNEL_COUNT]=
{
    {
        CAN_GPNUM_8,                                   /* CAN message data length  */

        VTP_DIAG_RESPONSE_ID,                            /* CAN message identifier   */

        &VTPmc_TxMsg[0].Data[ 0 ],                       /* Pointer to Data          */
    
        CANB_TX_STD_DATA,                              /* CAN message options      */

        VTP_DIAG_MESSAGE_HANDLE                          /* Transmit Message Handle  */
     
    }
};

#else

static CAN_TMD const tp_resp_tmd =
{
    CAN_GPNUM_8,                                   /* CAN message data length  */

    VTP_RESPONSE_ID,                               /* CAN message identifier   */

    &VTP_TxMsg.Data[ 0 ],                          /* Pointer to Data          */
    
    CANB_TX_STD_DATA,                              /* CAN message options      */

    VTP_MESSAGE_HANDLE                             /* Transmit Message Handle  */
};

#endif


#if (VTP_NUM_CHANNEL_COUNT > 1)
#define VTP_TxMsg          VTPmc_TxMsg[channel]
#define VTP_RxDataptr      VTPmc_RxDataptr[channel]
#define VTP_Datasize       VTPmc_Datasize[channel]
#endif

/*******************************************************************************
** FUNCTION NAME  :  VTP_Call_DllTransmit
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
VTP_UINT8 VTP_Call_DllTransmit (VTP_UINT8 channel)
#else
VTP_UINT8 VTP_Call_DllTransmit (void)
#endif
{
    SINT8 ret_type;
    VTP_UINT8 ret;
    CAN_TMD const * pTxTmd;
    UINT8 idx;
    
    /*pTxtmd will vary based on channel in multi channel TP implementation*/
    #if (VTP_NUM_CHANNEL_COUNT > 1)
    pTxTmd = &tp_resp_tmd[channel];
    #else
    pTxTmd = &tp_resp_tmd;
    #endif
    
    idx = VTP_TxMsg.Size;
    if(VTP_TxMsg.Size < CAN_GPNUM_8)
    {
        while(idx<8)
        {
            VTP_TxMsg.Data[idx] = 0x00;	/*Pad unused bytes with 0x00*/
            idx++;
        }
    }
    
    ret_type = DllTransmit( pTxTmd->MsgHandle, pTxTmd );

    if(ret_type == CANRC_OK)
    {
        ret = VTP_OK;
    }
    else
    {
        ret = VTP_NOTOK;
    }
    
    return (ret);
}

/*******************************************************************************
** FUNCTION NAME  :  VKD_ApplCB_TxConfirm
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
void VTPmc_ApplCB_TxConfirm(VTP_UINT8 channel,VTP_UINT8 sts)
#else
void VTP_ApplCB_TxConfirm(VTP_UINT8 sts)
#endif
{
    if(sts == VTP_OK)
    {
        __ApplTpTxConfirmation(VTP_SUCCESS);
    }
    else
    {
        
        __ApplTpTxErrorIndication(VTP_ERROR);
    }

}
/*******************************************************************************
** FUNCTION NAME  :  VKD_ApplCB_RxIndication
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
void VTPmc_ApplCB_RxIndication(VTP_UINT8 channel,VTP_UINT8 sts)
#else
void VTP_ApplCB_RxIndication(VTP_UINT8 sts)
#endif
{
    if(sts == VTP_OK)
    {
		if(VTP_Funcdiag_req == 1)
        {
            VTP_Funcdiag_req =0;
            __ApplTpFuncIndication(VTP_Datasize);
        }
		else
		{
			__ApplTpRxIndication(VTP_Datasize);
		}
    }
    else
    {
        __ApplTpRxErrorIndication(VTP_ERROR);
    }
}
/*******************************************************************************
** FUNCTION NAME  :  VKD_ApplCB_RxStart
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
VTP_UINT8 VTPmc_ApplCB_RxStart(VTP_UINT8 channel,VTP_UINT32 msgid, VTP_UINT16 size)
#else
VTP_UINT8 VTP_ApplCB_RxStart(VTP_UINT32 msgid, VTP_UINT16 size)
#endif
{
    VTP_UINT8 ret;
    
    #if(VTP_NUM_CHANNEL_COUNT > 1)
      VTP_Datasize =0;
      VTP_RxDataptr = __ApplTpRxGetBuffer(size);
    #else
    if( msgid == (VTP_UINT32)VTP_REQUEST_ID)
    {
        VTP_Datasize = 0;
        VTP_RxDataptr = __ApplTpRxGetBuffer(size);
	    VTP_Diag_PhyID_Flag = TRUE;
    }
	else if (msgid == (VTP_UINT32)VTP_FUNC_DIAG_REQUEST_ID)
    {
        VTP_Datasize = 0;
        VTP_RxDataptr = __ApplTpFuncGetBuffer(size);
        VTP_Funcdiag_req = 1;
	    VTP_Diag_PhyID_Flag = FALSE;
    }
    else
    {
        VTP_RxDataptr = NULL;
    }
    #endif
    
    if(VTP_RxDataptr != NULL)
    {
        ret = VTP_OK;
    }
    else
    {
        ret = VTP_NOTOK;
    }
    return (ret);
}

/*******************************************************************************
** FUNCTION NAME  :  VKD_ApplCB_RxInProg
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
void VTPmc_ApplCB_RxInProg(VTP_UINT8 channel,VTP_UINT8 * msgptr,VTP_UINT8 size)
#else
void VTP_ApplCB_RxInProg(VTP_UINT8 * msgptr,VTP_UINT8 size)
#endif
{
    for(;size != 0;size--)
    {
        VTP_RxDataptr[VTP_Datasize] = *msgptr;
        msgptr++;
        VTP_Datasize++;
    }
}

/*******************************************************************************
** FUNCTION NAME  :  VTP_Hook_Receive
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/

void VTP_Hook_Receive( CAN_RMD const * const prmd, VTP_UINT16 const handle)
{
    VTP_PDU tp_rx_pdu;
    VTP_UINT8 dataidx;
    
    tp_rx_pdu.MsgID = prmd->Identifier;
    tp_rx_pdu.Size = prmd->Size;
    
    for(dataidx=0 ; dataidx<prmd->Size ; dataidx++ )
    {
        tp_rx_pdu.Data[dataidx] = prmd->Data[dataidx];
    }
    #if(VTP_NUM_CHANNEL_COUNT>1)
    VTP_RxIndication((VTP_UINT8)handle,&tp_rx_pdu);
    #else
    VTP_RxIndication(&tp_rx_pdu);
    #endif
}
/*******************************************************************************
** FUNCTION NAME  :  VTP_Hook_Transmit
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/

VTP_UINT8 VTP_Hook_Transmit (VTP_UINT16 hTxMsg)
{
    VTP_UINT8 msg_found =FALSE;

    #if (VTP_NUM_CHANNEL_COUNT>1)
    UINT8 index;
    for(index=0;index < VTP_NUM_CHANNEL_COUNT;index++)
    {
        if(VTPmc_Message_Handle[index] == hTxMsg)
        {
            msg_found = TRUE;
            VTP_TxConfirm(index);
        }
    }
    #else
    if(hTxMsg == VTP_MESSAGE_HANDLE)
    {
        VTP_TxConfirm();
        msg_found = TRUE;
    }
    #endif
    
    return (msg_found);
}


VTP_UINT8 get_diag_addressing(void)
{
    return VTP_Diag_PhyID_Flag;
}


/*******************************************************************************
================================================================================
DATE           AUTHOR      VERSION  DESCRIPTION
================================================================================
24-Feb-09      vkannan     1      Initial version

26-Feb-09      vkannan     2      Updated as per Review Comments
                                  SMIS ID --> 18758

27-Feb-09      vkannan     3      Updated for development test defect fix 
                                  TMS00095157 

26-Mar-09      vkannan     4      Updated with QAC fixes after using latest 
                                   personality files from integration team.

03-Apr-09      vkannan     5      Multi Channel TP support , for new TP channels 
                                  added for Debug Frame work and HMI Simulation

17-Apr-09      vkannan     6      Updated after SMIS review 20077

22-Apr-09      vkannan     7    To FIX QAC Level 2 warning 3211 "The global identifier 
                                '%s' is defined but not used."

================================================================================
*******************************************************************************/
/* End of File =============================================================*/
