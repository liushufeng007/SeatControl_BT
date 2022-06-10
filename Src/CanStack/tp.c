/* ===========================================================================
**
**  Name: tp.c
**
**  Description: TP Implementation
**
**  Organization:   Multiplex Core Technology
**
** =========================================================================*/

/*******************************************************************************
================================================================================
 I N C L U D E   F I L E S
================================================================================
*******************************************************************************/
#include "tp_cfg.h"
#include "tp.h"
#include "tp_par.h"

/*******************************************************************************
================================================================================
 I N C L U D E   F I L E   V E R S I O N   C H E C K
================================================================================
*******************************************************************************/
#if (VTP_HEADER_VERSION != 0x0100)
   #error "THE SOURCE AND THE HEADER OF TRANSPORT LAYER DOES NOT MATCH"
#endif

#if (VTP_CFG_VERSION != 0x0100)
   #error "THE SOURCE AND THE CONFIG OF TRANSPORT LAYER DOES NOT MATCH"
#endif
/*******************************************************************************
================================================================================
 P R I V A T E   T Y P E   D E F I N I T I O N S
================================================================================
*******************************************************************************/
typedef struct 
{
    VTP_UINT16   SeqNum            : 4;
    VTP_UINT16   Tx_inprogress     : 1;
    VTP_UINT16   Rx_inprogress     : 1;
    VTP_UINT16   SendMsg           : 1;
    VTP_UINT16   Wait4FC           : 1;
    VTP_UINT16   MsgRcvd           : 1;
    VTP_UINT16   Rx_Status         : 2;
    VTP_UINT16   Tx_Status         : 2;
    VTP_UINT16   FC_Status         : 2;
}VTP_StatusType;

/*******************************************************************************
================================================================================
 C O N F I G U R A T I O N S
================================================================================
*******************************************************************************/
#define VTP_SINGLEFRAME_LENGTH   0x07

/*******************************************************************************
================================================================================
 L O C A L   V A R I A B L E S
================================================================================
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
static VTP_StatusType   VTPmc_status[VTP_NUM_CHANNEL_COUNT];
VTP_PDU                 VTPmc_TxMsg[VTP_NUM_CHANNEL_COUNT];
static VTP_PDU          VTPmc_RxMsg[VTP_NUM_CHANNEL_COUNT];
static VTP_UINT16       VTPmc_Timer[VTP_NUM_CHANNEL_COUNT];
static VTP_UINT8        VTPmc_BlockSize[VTP_NUM_CHANNEL_COUNT];
static VTP_UINT8        VTPmc_STmin[VTP_NUM_CHANNEL_COUNT];
static VTP_UINT8        VTPmc_RxFFBlockSize[VTP_NUM_CHANNEL_COUNT];
static VTP_UINT8        VTPmc_RxFFSTmin[VTP_NUM_CHANNEL_COUNT];
static VTP_UINT8        VTPmc_STminCntr[VTP_NUM_CHANNEL_COUNT];
static VTP_UINT8        VTPmc_FCwaitCntr[VTP_NUM_CHANNEL_COUNT];
static VTP_UINT16       VTPmc_DataCntr[VTP_NUM_CHANNEL_COUNT];
static VTP_SDU          VTPmc_TxRx[VTP_NUM_CHANNEL_COUNT];
static VTP_SDU *        VTPmc_TxRxMsgptr[VTP_NUM_CHANNEL_COUNT];

#define VTP_status                  VTPmc_status[channel]
#define VTP_TxMsg                   VTPmc_TxMsg[channel]
#define VTP_RxMsg                   VTPmc_RxMsg[channel]
#define VTP_Timer                   VTPmc_Timer[channel]
#define VTP_BlockSize               VTPmc_BlockSize[channel]
#define VTP_STmin                   VTPmc_STmin[channel]
#define VTP_RxFFBlockSize               VTPmc_BlockSize[channel]
#define VTP_RxFFSTmin                   VTPmc_STmin[channel]
#define VTP_STminCntr               VTPmc_STminCntr[channel]
#define VTP_FCwaitCntr              VTPmc_FCwaitCntr[channel]
#define VTP_DataCntr                VTPmc_DataCntr[channel]
#define VTP_TxRx                    VTPmc_TxRx[channel]
#define VTP_TxRxMsgptr              VTPmc_TxRxMsgptr[channel]

#define VTP_Tx_CF_Timecnt           VTPmc_Tx_CF_Timecnt[channel]
#define VTP_Wait_FC_Timecnt         VTPmc_Wait_FC_Timecnt[channel]
#define VTP_Wait_CF_Timecnt         VTPmc_Wait_CF_Timecnt[channel]
#define VTP_Wait_TXConf_Timecnt     VTPmc_Wait_TXConf_Timecnt[channel]

#define VTP_ApplCB_RxStart(a,b)     VTPmc_ApplCB_RxStart(channel,a,b)
#define VTP_ApplCB_RxInProg(a,b)    VTPmc_ApplCB_RxInProg(channel,a,b)
#define VTP_ApplCB_RxIndication(a)  VTPmc_ApplCB_RxIndication(channel,a)
#define VTP_ApplCB_TxConfirm(a)     VTPmc_ApplCB_TxConfirm(channel,a)

#else
static VTP_StatusType   VTP_status;
VTP_PDU                 VTP_TxMsg;
static VTP_PDU          VTP_RxMsg;
static VTP_UINT16       VTP_Timer;
static VTP_UINT8        VTP_BlockSize;
static VTP_UINT8        VTP_STmin;
static VTP_UINT8        VTP_STminCntr;
static VTP_UINT8        VTP_FCwaitCntr;
static VTP_UINT16       VTP_DataCntr;
static VTP_SDU          VTP_TxRx;
static VTP_SDU *        VTP_TxRxMsgptr;
static VTP_UINT8        VTP_BlockSize;
static VTP_UINT8        VTP_STmin;
static VTP_UINT8        VTP_RxFFBlockSize;
static VTP_UINT8        VTP_RxFFSTmin;

#define VTP_Tx_CF_Timecnt           (VTP_TX_CF_TIME_INMSEC/VTP_PERIODICITY)
#define VTP_Wait_FC_Timecnt         (VTP_WAIT_FC_TIME_INMSEC/VTP_PERIODICITY)     
#define VTP_Wait_CF_Timecnt         (VTP_WAIT_CF_TIME_INMSEC/VTP_PERIODICITY)     
#define VTP_Wait_TXConf_Timecnt     (VTP_TX_WAIT_CONF_TIME_INMSEC/VTP_PERIODICITY)

#endif

/*******************************************************************************
================================================================================
 L O C A L   M A C R O S
================================================================================
*******************************************************************************/
#define VTP_TX_IN_PROGRESS       (VTP_status.Tx_inprogress  != (VTP_UINT8)0)
#define VTP_SET_TXINPROGRESS     (VTP_status.Tx_inprogress   = (VTP_UINT8)1)
#define VTP_CLEAR_TXINPROGRESS   (VTP_status.Tx_inprogress   = (VTP_UINT8)0)

#define VTP_RX_IN_PROGRESS       (VTP_status.Rx_inprogress  != (VTP_UINT8)0)
#define VTP_SET_RXINPROGRESS     (VTP_status.Rx_inprogress   = (VTP_UINT8)1)
#define VTP_CLEAR_RXINPROGRESS   (VTP_status.Rx_inprogress   = (VTP_UINT8)0)

#define VTP_SENDMSG              (VTP_status.SendMsg        != (VTP_UINT8)0)
#define VTP_SET_SENDMSG          (VTP_status.SendMsg         = (VTP_UINT8)1)
#define VTP_CLEAR_SENDMSG        (VTP_status.SendMsg         = (VTP_UINT8)0)

#define VTP_INC_SEQNUM           (VTP_status.SeqNum++)
#define VTP_INIT_SEQNUM          (VTP_status.SeqNum          = (VTP_UINT8)1)

#define VTP_WAIT4FC              (VTP_status.Wait4FC        != (VTP_UINT8)0)
#define VTP_SET_WAIT4FC          (VTP_status.Wait4FC         = (VTP_UINT8)1)
#define VTP_CLEAR_WAIT4FC        (VTP_status.Wait4FC         = (VTP_UINT8)0)

#define VTP_SET_FC_OVERFLOW      (VTP_status.FC_Status = (VTP_UINT8)2)
#define VTP_SET_FC_CTS           (VTP_status.FC_Status = (VTP_UINT8)0)
#define VTP_SET_FC_WAIT          (VTP_status.FC_Status = (VTP_UINT8)1)
#define VTP_FC_STATUS            (VTP_status.FC_Status)
#define VTP_FC_CTS               (VTP_status.FC_Status == (VTP_UINT8)0)
#define VTP_FC_OVERFLOW          (VTP_status.FC_Status == (VTP_UINT8)2)
#define VTP_FC_WAIT              (VTP_status.FC_Status == (VTP_UINT8)1)


#define VTP_MSGRCVD              (VTP_status.MsgRcvd        != (VTP_UINT8)0)
#define VTP_SET_MSGRCVD          (VTP_status.MsgRcvd         = (VTP_UINT8)1)
#define VTP_CLEAR_MSGRCVD        (VTP_status.MsgRcvd         = (VTP_UINT8)0)

#define VTP_START_TX_CF_TIMER      (VTP_Timer = VTP_Tx_CF_Timecnt)
#define VTP_START_RX_FC_TIMER      (VTP_Timer = VTP_Wait_FC_Timecnt)
#define VTP_START_RX_CF_TIMER      (VTP_Timer = VTP_Wait_CF_Timecnt)
#define VTP_START_TX_CONF_TIMER    (VTP_Timer = VTP_Wait_TXConf_Timecnt)

#define VTP_STOP_TIMER          (VTP_Timer = (VTP_UINT16)0)
#define VTP_TIMER_XPIRED        (VTP_Timer  == (VTP_UINT16)0)


#define VTP_LASTMSG_SF     ((VTP_UINT8)0x00 == (VTP_TxMsg.Data[0] & (VTP_UINT8)0xF0))
#define VTP_LASTMSG_FF     ((VTP_UINT8)0x10 == (VTP_TxMsg.Data[0] & (VTP_UINT8)0xF0))
#define VTP_LASTMSG_CF     ((VTP_UINT8)0x20 == (VTP_TxMsg.Data[0] & (VTP_UINT8)0xF0))
#define VTP_LASTMSG_FC     ((VTP_UINT8)0x30 == (VTP_TxMsg.Data[0] & (VTP_UINT8)0xF0))

#define VTP_FCCTS          ((VTP_UINT8)0x30 == VTP_RxMsg.Data[0])
#define VTP_FCWAIT         ((VTP_UINT8)0x31 == VTP_RxMsg.Data[0])

#define VTP_MSGRCVD_SF     ((VTP_UINT8)0x00 == (VTP_RxMsg.Data[0] & (VTP_UINT8)0xF0))
#define VTP_MSGRCVD_FF     ((VTP_UINT8)0x10 == (VTP_RxMsg.Data[0] & (VTP_UINT8)0xF0))
#define VTP_MSGRCVD_CF     ((VTP_UINT8)0x20 == (VTP_RxMsg.Data[0] & (VTP_UINT8)0xF0))

#define VTP_SET_RX_COMPLETE      (VTP_status.Rx_Status  = 1)
#define VTP_SET_RX_ERROR         (VTP_status.Rx_Status  = 2)
#define VTP_CLEAR_RX_COMPLETE    (VTP_status.Rx_Status  = 0)
#define VTP_CLEAR_RX_ERROR       (VTP_status.Rx_Status  = 0)
#define VTP_RX_STATUS_COMPLETE   (VTP_status.Rx_Status  == 1)
#define VTP_RX_STATUS_ERROR      (VTP_status.Rx_Status  == 2)

#define VTP_SET_TX_COMPLETE      (VTP_status.Tx_Status  = 1)
#define VTP_SET_TX_ERROR         (VTP_status.Tx_Status  = 2)
#define VTP_CLEAR_TX_COMPLETE    (VTP_status.Tx_Status  = 0)
#define VTP_CLEAR_TX_ERROR       (VTP_status.Tx_Status  = 0)
#define VTP_TX_STATUS_COMPLETE   (VTP_status.Tx_Status  == 1)
#define VTP_TX_STATUS_ERROR      (VTP_status.Tx_Status  == 2)


/*******************************************************************************
================================================================================
 L O C A L   F U N C T I O N S   P R O T O T Y P E S
================================================================================
*******************************************************************************/
static void VTP_memcpy(VTP_UINT8 *src,VTP_UINT8 *dst,VTP_UINT8 size);
#if (VTP_NUM_CHANNEL_COUNT > 1)
static void VTP_Wait4FC( VTP_UINT8 channel);
static void VTP_TransmitCF(VTP_UINT8 channel);
static void VTP_Receive(VTP_UINT8 channel);
static void VTP_SendFC(VTP_UINT8 channel);

#else
static void VTP_Wait4FC(void);
static void VTP_TransmitCF(void);
static void VTP_Receive(void);
static void VTP_SendFC(void);
#endif

/*******************************************************************************
================================================================================
G L O B A L     C O N S T A N T S
================================================================================
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)

VTP_UINT32 const VTPmc_Response_ID[VTP_NUM_CHANNEL_COUNT] = 
{
    VTPMC_RESPONSE_ID
};

VTP_UINT32 const VTPmc_Request_ID[VTP_NUM_CHANNEL_COUNT] =
{
    VTPMC_REQUEST_ID
};

VTP_UINT8 const VTPmc_Max_Wait_Cnt[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_MAX_WAIT_CNT
};
VTP_UINT8 const VTPmc_TxBlock_Size[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_BLOCKSIZE
};
VTP_UINT8 const VTPmc_TxSTmin[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_STMIN
};

VTP_UINT16 const VTPmc_Tx_CF_Timecnt[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_TX_CF_TIMECNT
};
VTP_UINT16 const VTPmc_Wait_FC_Timecnt[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_WAIT_FC_TIMECNT
};
VTP_UINT16 const VTPmc_Wait_CF_Timecnt[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_WAIT_CF_TIMECNT
};
VTP_UINT16 const VTPmc_Wait_TXConf_Timecnt[VTP_NUM_CHANNEL_COUNT]=
{
    VTPMC_TX_WAIT_CONF_TIMECNT
};



#endif



/*******************************************************************************
================================================================================
 P U B L I C   F U N C T I O N S
================================================================================
*******************************************************************************/

/*******************************************************************************
** FUNCTION NAME  :  VTP_Init
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
void VTP_Init(void)
{
    #if (VTP_NUM_CHANNEL_COUNT > 1)
    VTP_UINT8 channel;
    for(channel = 0; channel<VTP_NUM_CHANNEL_COUNT;channel++)
    #endif
    {
        VTP_Timer      = 0;
        VTP_BlockSize  = 0;
        VTP_STmin      = 0;
        VTP_CLEAR_TXINPROGRESS;
        VTP_CLEAR_RXINPROGRESS;
        VTP_CLEAR_SENDMSG;
        VTP_CLEAR_MSGRCVD;
        VTP_TxRxMsgptr = &VTP_TxRx;
        VTP_CLEAR_WAIT4FC;
    }
    
}

#if (VTP_NUM_CHANNEL_COUNT > 1)
void VTP_Channel_Init(VTP_UINT8 channel)
{
    VTP_Timer      = 0;
    VTP_BlockSize  = 0;
    VTP_STmin      = 0;
    VTP_CLEAR_TXINPROGRESS;
    VTP_CLEAR_RXINPROGRESS;
    VTP_CLEAR_SENDMSG;
    VTP_CLEAR_MSGRCVD;
    VTP_TxRxMsgptr = &VTP_TxRx;
}
#endif

/*******************************************************************************
** FUNCTION NAME  :  VTP_PeriodicTask
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
void VTP_PeriodicTask(void)
{
    VTP_UINT8 ret;
    
    
    #if (VTP_NUM_CHANNEL_COUNT > 1)
    VTP_UINT8 channel;
    for(channel = 0; channel<VTP_NUM_CHANNEL_COUNT;channel++)
    #endif
    {
        if((VTP_UINT8)0 != VTP_Timer)
        {
            VTP_Timer--;
        }
        else
        {}
        if(VTP_TX_IN_PROGRESS)
        {
            if(!VTP_SENDMSG)
            {
                if(VTP_LASTMSG_SF)
                {
                    VTP_CLEAR_TXINPROGRESS;
                    VTP_CLEAR_MSGRCVD;
                    /*VTP_ApplCB_TxConfirm(VTP_OK);*/
                    VTP_SET_TX_COMPLETE;
                }
                else if(VTP_LASTMSG_FF || VTP_LASTMSG_CF)
                {
                    if(VTP_WAIT4FC)
                    {
                        #if (VTP_NUM_CHANNEL_COUNT > 1)
                        VTP_Wait4FC(channel);
                        #else
                        VTP_Wait4FC();
                        #endif
                    }
                    else
                    {
                        #if (VTP_NUM_CHANNEL_COUNT > 1)
                        VTP_TransmitCF(channel);
                        #else
                        VTP_TransmitCF();
                        #endif
                    }
                }
                else
                {}
            }
            else
            {
                if(VTP_TIMER_XPIRED)
                {
                    VTP_ApplCB_TxConfirm(VTP_NOTOK);
                    VTP_CLEAR_TXINPROGRESS;
                    VTP_CLEAR_SENDMSG;
                    VTP_CLEAR_MSGRCVD;
                    VTP_CLEAR_WAIT4FC;
                }
                else
                {}
            }
        }
        else if(VTP_RX_STATUS_COMPLETE)
        {
            VTP_CLEAR_RX_COMPLETE;
            VTP_ApplCB_RxIndication(VTP_OK);
        }
        else if(VTP_RX_STATUS_ERROR)
        {
            VTP_CLEAR_RX_ERROR;
            VTP_ApplCB_RxIndication(VTP_NOTOK);
        }
        else if(VTP_RX_IN_PROGRESS)
        {
            if(VTP_TIMER_XPIRED)
            {
                VTP_ApplCB_RxIndication(VTP_NOTOK);
                VTP_CLEAR_RXINPROGRESS;
                VTP_CLEAR_SENDMSG;
                VTP_CLEAR_MSGRCVD;
            }
            else
            {}
        }
        else
        {}

        if(VTP_SENDMSG)
        {
            #if(VTP_NUM_CHANNEL_COUNT > 1)
            ret = VTP_Call_DllTransmit(channel);
            #else
            ret = VTP_Call_DllTransmit();
            #endif
            if(VTP_OK == ret)
            {
                VTP_CLEAR_SENDMSG;
                if(VTP_LASTMSG_SF)
                {
                    VTP_SET_TX_COMPLETE;	
                }
                /*VTP_START_TIMER;*/
            }
            else
            {}
        }
        else
        {}
    }
}

/*******************************************************************************
** FUNCTION NAME  :  VTP_Transmit
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
VTP_UINT8 VTP_Transmit(VTP_UINT8 channel,VTP_UINT8 * Msgptr, VTP_UINT16 length)
#else
VTP_UINT8 VTP_Transmit(VTP_UINT8 * Msgptr, VTP_UINT16 length)
#endif
{
    VTP_UINT8 retStatus;
    retStatus = VTP_NOTOK;
    if(!VTP_TX_IN_PROGRESS && 
    !VTP_RX_IN_PROGRESS &&
    (length != (VTP_UINT16)0))
    {
        VTP_TxRxMsgptr->Data    =  Msgptr;
        VTP_TxRxMsgptr->DataLength = length;
        VTP_TxMsg.MsgID   =  VTP_RESPONSE_ID;
        VTP_TxMsg.Size    =  (VTP_UINT8)8;
        VTP_DataCntr      =  (VTP_UINT8)0;

        if((VTP_UINT16)VTP_SINGLEFRAME_LENGTH >= VTP_TxRxMsgptr->DataLength)
        {
            VTP_TxMsg.Size    =  ((VTP_UINT8)VTP_TxRxMsgptr->DataLength + (VTP_UINT8)1);
            VTP_TxMsg.Data[0] =  (VTP_UINT8)VTP_TxRxMsgptr->DataLength;
            VTP_memcpy(VTP_TxRxMsgptr->Data,&(VTP_TxMsg.Data[1]),(VTP_UINT8)VTP_TxRxMsgptr->DataLength);
            VTP_TxRxMsgptr->DataLength = (VTP_UINT16)0;
        }
        else
        {
            VTP_memcpy(&(VTP_TxRxMsgptr->Data[0]),&(VTP_TxMsg.Data[2]),(VTP_UINT8)6);
            VTP_TxMsg.Data[0]    =  ((VTP_UINT8)0x10 + (VTP_UINT8)(VTP_TxRxMsgptr->DataLength >> 8));
            VTP_TxMsg.Data[1]    =  (VTP_UINT8) (VTP_TxRxMsgptr->DataLength);
            VTP_TxRxMsgptr->DataLength   -= (VTP_UINT16)6;
            VTP_DataCntr         += (VTP_UINT16)6;
            VTP_status.SeqNum    =  (VTP_UINT8) 1;
            VTP_SET_WAIT4FC;
        }

        VTP_SET_TXINPROGRESS;
        VTP_SET_SENDMSG;
        if(VTP_SENDMSG)
        {
            #if(VTP_NUM_CHANNEL_COUNT > 1)
            retStatus = VTP_Call_DllTransmit(channel);
            #else
            retStatus = VTP_Call_DllTransmit();
            #endif
            if(VTP_OK == retStatus)
            {
                VTP_CLEAR_SENDMSG;
                if(VTP_LASTMSG_SF)
                {
                    VTP_SET_TX_COMPLETE;	
                }
                /*VTP_START_TIMER;*/
            }
            else
            {}
        }
        VTP_START_TX_CONF_TIMER;
        retStatus = VTP_OK;
    }
    else
    {}
    return(retStatus);
}
/*******************************************************************************
** FUNCTION NAME  :  VTP_AbortTransmit
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
void VTP_AbortTransmit(VTP_UINT8 channel)
#else
void VTP_AbortTransmit(void)
#endif
{
    VTP_CLEAR_TXINPROGRESS;
    VTP_CLEAR_SENDMSG;
}

/*******************************************************************************
================================================================================
 P U B L I C   F U N C T I O N   C A L L B A C K S
================================================================================
*******************************************************************************/
/*******************************************************************************
** FUNCTION NAME  :  VTP_TxConfirm
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
void VTP_TxConfirm(VTP_UINT8 channel)
#else
void VTP_TxConfirm(void)
#endif
{
    if(VTP_LASTMSG_SF)
    {
        VTP_CLEAR_TXINPROGRESS;
        VTP_STOP_TIMER;
    }
    else if(VTP_LASTMSG_FF)
    {
        VTP_START_RX_FC_TIMER;
    }
    else if(VTP_LASTMSG_CF)
    {
        if(VTP_WAIT4FC)
        {
            VTP_START_RX_FC_TIMER;
        }
        else
        {
            VTP_START_TX_CF_TIMER;
        }
    }
    else if(VTP_LASTMSG_FC)
    {
        VTP_START_RX_CF_TIMER;
    }
    else
    {
    }
    if(VTP_TX_STATUS_COMPLETE)
    {
        VTP_CLEAR_TX_COMPLETE;
        VTP_ApplCB_TxConfirm(VTP_OK);
    }
}

/*******************************************************************************
** FUNCTION NAME  :  VTP_RxIndication
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
void VTP_RxIndication(VTP_UINT8 channel,VTP_PDU * Msgptr)
#else
void VTP_RxIndication(VTP_PDU * Msgptr)
#endif
{
    if(!VTP_MSGRCVD)
    {
        VTP_RxMsg.MsgID = Msgptr->MsgID;
        VTP_RxMsg.Size  = Msgptr->Size;
        VTP_memcpy(&(Msgptr->Data[0]),&(VTP_RxMsg.Data[0]),VTP_RxMsg.Size);
        if((VTP_WAIT4FC) && (VTP_RxMsg.Size >= 3))
        {
          VTP_SET_MSGRCVD;
        }
        else
        {
            #if(VTP_NUM_CHANNEL_COUNT > 1)
            VTP_Receive(channel);
            #else
            VTP_Receive();
            #endif
        }
    }
    else
    {}
}

/*******************************************************************************
================================================================================
 P R I V A T E   F U N C T I O N S
================================================================================
*******************************************************************************/

/*******************************************************************************
** FUNCTION NAME  :  VTP_Transmit
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
static void VTP_Wait4FC(VTP_UINT8 channel)
#else
static void VTP_Wait4FC(void)
#endif
{
    if(VTP_WAIT4FC && VTP_MSGRCVD)
    {
        if(VTP_FCCTS)
        {
            if(VTP_LASTMSG_FF)
            {
                VTP_FCwaitCntr = 0;
                VTP_BlockSize = VTP_RxMsg.Data[1];
                VTP_STmin     = VTP_RxMsg.Data[2];
                VTP_RxFFBlockSize = VTP_BlockSize;
                VTP_RxFFSTmin = VTP_STmin;
                if(VTP_STmin < 0x7F)
                {
                    VTP_STminCntr = (VTP_STmin / (VTP_UINT8)VTP_PERIODICITY);
                }
                else
                {
                    VTP_STminCntr = 1;
                }
                if((VTP_STminCntr * (VTP_UINT8)VTP_PERIODICITY) != VTP_STmin)
                {
                    VTP_STminCntr++;
                }
                else
                {}
            }
            else
            {
                VTP_BlockSize = VTP_RxFFBlockSize;
                VTP_STmin     = VTP_RxFFSTmin;
            }
            VTP_CLEAR_WAIT4FC;
            VTP_START_TX_CF_TIMER;
        }
        else if(VTP_FCWAIT)
        {
            VTP_FCwaitCntr++;
            if(VTP_MAX_WAIT_CNT >= VTP_FCwaitCntr)
            {
                VTP_START_RX_FC_TIMER;
            }
            else
            {
                VTP_FCwaitCntr = 0;
                VTP_ApplCB_TxConfirm(VTP_NOTOK);
                VTP_CLEAR_TXINPROGRESS;
            }
        }
        else
        {
            if(VTP_TIMER_XPIRED)
            {
                VTP_ApplCB_TxConfirm(VTP_NOTOK);
                VTP_CLEAR_TXINPROGRESS;
                VTP_CLEAR_WAIT4FC;
            }
            else
            {}
        }
        VTP_CLEAR_MSGRCVD;
    }
    else
    {
        if(VTP_TIMER_XPIRED)
        {
            VTP_ApplCB_TxConfirm(VTP_NOTOK);
            VTP_CLEAR_TXINPROGRESS;
            VTP_CLEAR_WAIT4FC;
            VTP_CLEAR_MSGRCVD;
        }
        else
        {}
    }
}
/*******************************************************************************
** FUNCTION NAME  :  VTP_Transmit
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
static void VTP_TransmitCF(VTP_UINT8 channel)
#else
static void VTP_TransmitCF(void)
#endif
{
    if((VTP_UINT8)0 != VTP_STminCntr)
    {
        VTP_STminCntr--;
    }
    else
    {}
    if((VTP_UINT8)0 == VTP_STminCntr)
    {
        VTP_TxMsg.Data[0] =  ((VTP_UINT8)0x20 + VTP_status.SeqNum);
        VTP_SET_SENDMSG;
        if((VTP_UINT8)7 >= VTP_TxRxMsgptr->DataLength)
        {
            VTP_TxMsg.Size    =  ((VTP_UINT8)VTP_TxRxMsgptr->DataLength + (VTP_UINT8)1);
            VTP_memcpy(&(VTP_TxRxMsgptr->Data[VTP_DataCntr]),&(VTP_TxMsg.Data[1]),(VTP_UINT8)VTP_TxRxMsgptr->DataLength);
            VTP_TxRxMsgptr->DataLength = (VTP_UINT16)0;
            VTP_CLEAR_TXINPROGRESS;
            VTP_CLEAR_MSGRCVD;
            VTP_SET_TX_COMPLETE;
            /*VTP_ApplCB_TxConfirm(VTP_OK);*/
        }
        else
        {
            VTP_TxMsg.Size    =  (VTP_UINT8)8;
            VTP_memcpy(&(VTP_TxRxMsgptr->Data[VTP_DataCntr]),&(VTP_TxMsg.Data[1]),(VTP_UINT8)7);
            VTP_TxRxMsgptr->DataLength   -= (VTP_UINT16)7;
            VTP_DataCntr         += (VTP_UINT16)7;
            VTP_status.SeqNum++;
        }
        VTP_STminCntr = (VTP_STmin / (VTP_UINT8)VTP_PERIODICITY);
        if((VTP_STminCntr * (VTP_UINT8)VTP_PERIODICITY) != VTP_STmin)
        {
            VTP_STminCntr++;
        }
        else
        {}
        if(((VTP_UINT8)0 != VTP_BlockSize) &&(VTP_TX_IN_PROGRESS))
        {
            VTP_BlockSize--;
            if((VTP_UINT8)0 == VTP_BlockSize)
            {
                VTP_SET_WAIT4FC;
            }
            else
            {}
        }
        else
        {}
        VTP_START_TX_CONF_TIMER;
    }
    else
    {}
}

/*******************************************************************************
** FUNCTION NAME  :  VTP_memcpy
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
static void VTP_memcpy(VTP_UINT8 *src,VTP_UINT8 *dst,VTP_UINT8 size)
{
    for(;size > 0;size--)
    {
        *dst = *src;
        dst++;
        src++;
    }
}

/*******************************************************************************
** FUNCTION NAME  :  VTP_Receive
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
static void VTP_Receive(VTP_UINT8 channel)
#else
static void VTP_Receive(void)
#endif
{
    VTP_UINT8 ret;
    if(VTP_MSGRCVD_SF)
    {
        if(VTP_RX_IN_PROGRESS)
        {
            /*VTP_SET_RX_ERROR;*/
            VTP_ApplCB_RxIndication(VTP_NOTOK);
            VTP_CLEAR_RXINPROGRESS;
        }
        else
        {}
        if(VTP_RxMsg.Size > VTP_RxMsg.Data[0])
        {
            ret = VTP_ApplCB_RxStart(VTP_RxMsg.MsgID,VTP_RxMsg.Data[0]);
            if ( ret == VTP_OK)
            {
                VTP_ApplCB_RxInProg(&VTP_RxMsg.Data[1],VTP_RxMsg.Data[0]);
                VTP_SET_RX_COMPLETE;
            }
            else
            {
                VTP_CLEAR_RXINPROGRESS;
                /*VTP_SET_RX_ERROR;*/
                VTP_ApplCB_RxIndication(VTP_NOTOK);
            }

        }
        else
        {}
    }
    else if(VTP_MSGRCVD_FF)
    {
        if(VTP_RX_IN_PROGRESS)
        {
            /*VTP_SET_RX_ERROR;*/
            VTP_ApplCB_RxIndication(VTP_NOTOK);
            VTP_CLEAR_RXINPROGRESS;
        }
        else
        {}
        if(VTP_RxMsg.MsgID ==  VTP_REQUEST_ID)
        {
            VTP_DataCntr = (VTP_UINT16)(VTP_RxMsg.Data[0] & 0x0F);
            VTP_DataCntr <<= 8;
            VTP_DataCntr += (VTP_UINT16)VTP_RxMsg.Data[1];
            if(VTP_DataCntr > (VTP_UINT16)VTP_SINGLEFRAME_LENGTH)
            {
                if((VTP_UINT8)8 == VTP_RxMsg.Size)
                {
                    ret = VTP_ApplCB_RxStart(VTP_RxMsg.MsgID,VTP_DataCntr);
                    if ( ret == VTP_OK)
                    {
                        VTP_SET_FC_CTS;
                        VTP_ApplCB_RxInProg(&VTP_RxMsg.Data[2],(VTP_UINT8)6);
                        VTP_BlockSize = (VTP_UINT8)1;
                        #if (VTP_NUM_CHANNEL_COUNT > 1)
                        VTP_SendFC(channel);
                        #else
                        VTP_SendFC();
                        #endif
                        VTP_DataCntr -= (VTP_UINT16)6;
                        VTP_status.SeqNum = (VTP_UINT8)0;
                        VTP_SET_RXINPROGRESS;
                    }
                    else
                    {
                        if(VTP_FC_OVERFLOW)
                        {
                            VTP_BlockSize = (VTP_UINT8)1;                          
                            #if (VTP_NUM_CHANNEL_COUNT > 1)
                            VTP_SendFC(channel);
                            #else
                            VTP_SendFC();
                            #endif
                        }
                        VTP_CLEAR_RXINPROGRESS;
                    }
                }
                else
                {}
            }
            else
            {}
        }
        else
        {}
    }
    else if(VTP_MSGRCVD_CF)
    {
        if(VTP_RX_IN_PROGRESS)
        {
            if(VTP_REQUEST_ID == VTP_RxMsg.MsgID)
            {
                VTP_status.SeqNum++;
                if((VTP_UINT8)7 >= VTP_DataCntr)
                {
                    //if((VTP_UINT16)VTP_RxMsg.Size == (VTP_DataCntr + (VTP_UINT16)1))
                    {
                        if(VTP_status.SeqNum == (VTP_RxMsg.Data[0] & (VTP_UINT8)0x0F))
                        {
                            VTP_ApplCB_RxInProg(&VTP_RxMsg.Data[1],(VTP_UINT8)VTP_DataCntr);
                            /*#if(VTP_NUM_CHANNEL_COUNT > 1)
                            VTP_SendFC(channel);
                            #else
                            VTP_SendFC();
                            #endif*/
                            VTP_DataCntr = (VTP_UINT16)0;
                            VTP_CLEAR_RXINPROGRESS;
                            VTP_SET_RX_COMPLETE;
                        }
                        else
                        {
                            /*VTP_SET_RX_ERROR;*/
                            VTP_ApplCB_RxIndication(VTP_NOTOK);
                            VTP_CLEAR_RXINPROGRESS;
                        }
                    }
                    /*else
                    {
                    VTP_ApplCB_RxIndication(VTP_NOTOK);
                    VTP_CLEAR_RXINPROGRESS;
                    }*/
                }
                else
                {
                    if((VTP_UINT8)8 == VTP_RxMsg.Size)
                    {
                        if(VTP_status.SeqNum == (VTP_RxMsg.Data[0] & (VTP_UINT8)0x0F))
                        {
                            VTP_ApplCB_RxInProg(&VTP_RxMsg.Data[1],(VTP_UINT8)7);
                            #if(VTP_NUM_CHANNEL_COUNT > 1)
                            VTP_SendFC(channel);
                            #else
                            VTP_SendFC();
                            #endif
                            VTP_DataCntr -= (VTP_UINT16)7;
                            /*VTP_START_TIMER;*/
                        }
                        else
                        {
                            /*VTP_SET_RX_ERROR;*/
                            VTP_ApplCB_RxIndication(VTP_NOTOK);
                            VTP_CLEAR_RXINPROGRESS;
                        }
                    }
                    else
                    {
                        /*VTP_SET_RX_ERROR;*/
                        VTP_ApplCB_RxIndication(VTP_NOTOK);
                        VTP_CLEAR_RXINPROGRESS;
                    }
                }
            }
            else
            {
                /*VTP_SET_RX_ERROR;*/
                VTP_ApplCB_RxIndication(VTP_NOTOK);
                VTP_CLEAR_RXINPROGRESS;
            }
        }
        else
        {
            /*VTP_SET_RX_ERROR;*/
            VTP_ApplCB_RxIndication(VTP_NOTOK);
        }
    }
    else
    {
        /*VTP_SET_RX_ERROR;*/
    }
    VTP_CLEAR_MSGRCVD;
}

/*******************************************************************************
** FUNCTION NAME  :  VTP_SendFC
** DESCRIPTION    :
** INPUT          :  None
** OUTPUT         :
** RETURN         :  None
*******************************************************************************/
#if (VTP_NUM_CHANNEL_COUNT > 1)
static void VTP_SendFC(VTP_UINT8 channel)
#else
static void VTP_SendFC(void)
#endif
{
    VTP_UINT8 ret;

    if((VTP_UINT8)0 != VTP_BlockSize)
    {
        VTP_BlockSize--;
        if((VTP_UINT8)0 == VTP_BlockSize)
        {
            VTP_TxMsg.MsgID    = VTP_RESPONSE_ID;
            VTP_TxMsg.Size     = (VTP_UINT8)0x03;
            VTP_TxMsg.Data[0]  = (VTP_UINT8)0x30 | (VTP_FC_STATUS);
            VTP_TxMsg.Data[1]  = VTP_BLOCKSIZE;
            VTP_TxMsg.Data[2]  = (VTP_UINT8)VTP_STMIN;
            VTP_BlockSize      = VTP_BLOCKSIZE;
            VTP_SET_SENDMSG;
            if(VTP_SENDMSG)
            {
                #if(VTP_NUM_CHANNEL_COUNT > 1)
                ret = VTP_Call_DllTransmit(channel);
                #else
                ret = VTP_Call_DllTransmit();
                #endif
                if(VTP_OK == ret)
                {
                    VTP_CLEAR_SENDMSG;
                    /*VTP_START_TIMER;*/
                }
                else
                {}
            }
            else
            {}
            VTP_START_TX_CONF_TIMER;
        }
        else
        {}
    }
    else
    {
        VTP_START_RX_CF_TIMER;
    }
}

#if (VTP_NUM_CHANNEL_COUNT > 1)
void VTP_SetFC_Overflow(VTP_UINT8 channel)
#else
void VTP_SetFC_Overflow(void)
#endif
{
    VTP_SET_FC_OVERFLOW;
}

#if (VTP_NUM_CHANNEL_COUNT > 1)
void VTP_SetFC_Wait(VTP_UINT8 channel)
#else
void VTP_SetFC_Wait(void)
#endif
{
    VTP_SET_FC_WAIT;
}

