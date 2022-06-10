/* ===========================================================================
//
//  Name:           il.c
//
//  Description:    CAN Interaction Layer
//
//                  Interaction Layer to Support Signal/Message Based Interface
//
//  Organization:   Multiplex Subsystems
//
// =========================================================================*/
/* ===========================================================================
//  I N C L U D E   F I L E S
// =========================================================================*/
#include "vnim_can_cfg.h"
#include "il.h"
#include "il_par.h"
#include "system.h"
//#include "can_comm.h"
#include "Canif.h"
#include "PowerMgmt.h"

/* ===========================================================================
//  P R I V A T E   M A C R O S
// =========================================================================*/
#define pRxInd (il_rx_indication_fn[canFrameHandle])
#define pRxtimeout (il_rx_timeout_fn[canFrameHandle])
/* ===========================================================================
//  P R I V A T E   F U N C T I O N   P R O T O T Y P E S
// =========================================================================*/

static void
IlSaveReceivedMessage( CAN_RMD const * const, UINT16 const );

static void IlProcessReceiveFrame( UINT16 const );

static void IlInitTx( void );

static void IlTxTaskService( void );

static void IlInitTxFrameStatus( void );

static void IlInitTxFrameData( void );

static void IlInitRx( void );

/* ===========================================================================
//  P R I V A T E   M E M O R Y   A L L O C A T I O N
// =========================================================================*/


/* ===========================================================================
//  Interaction Layer Status Variables
// =========================================================================*/
UINT8 il_status;

/* ===========================================================================
//  Bit Width Masks (Used to Pack and Unpack Frames)
// =========================================================================*/

/* ===========================================================================
//  P U B L I C   F U N C T I O N S
// =========================================================================*/
/* ===========================================================================
//
// Name:            IlInit
//
// Description:     Initialize the Interaction Layer
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
IlInit( void )
{
    /* Initialize Tx and Rx Data Structures */
    IlInitTx( );

    IlInitRx( );

    /* Initialize the Interaction Layer Status */
	IlResume();

}

/* ===========================================================================
//
// Name:            IlSuspend
//
// Description:     Suspend Interaction Layer Operation
//
//                  This function is intended to support suspension of
//                  Interaction Layer operation in the BusOFF condition
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
IlSuspend( void )
{
    /* Suspend Interaction Layer Operation */
    il_status = IL_STATUS_SUSPEND;

}

/* ===========================================================================
//
// Name:            IlResume
//
// Description:     Resume Interaction Layer Operation
//
//                  This function is intended to support resumption of
//                  Interaction Layer operation when Network Management
//                  recovers from a BusOFF condition.
//
//                  This function causes all transmit periodic messages to
//                  reset their timing, but the periodic transmit data is
//                  left unchanged.
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
IlResume( void )
{

    /* Initialize the Interaction Layer Status */
    il_status = 0;

    /* Initialize the Status of All Tx Frames */
    IlInitTxFrameStatus( );

}

/* ===========================================================================
//
// Name:            IlTxSuspend
//
// Description:     Suspend Interaction Layer Transmit Operation
//
//                  This function suspends Interaction Layer transmit
//                  operations in support of a "Bus Quiet" mode that is
//                  needed when an ECU is being programmed on the CAN bus.
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
IlTxSuspend( void )
{
    /* Set Status Bit to Suspend Interaction Layer Signal Transmissions */
    IlUtilSetStatusBits(&il_status, IL_STATUS_TX_SUSPEND);
}

/* ===========================================================================
//
// Name:            IlTxResume
//
// Description:     Resume Interaction Layer Transmit Operation
//
//                  This function resumes Interaction Layer transmit
//                  operations in support of a "Bus Quiet" mode that is
//                  needed when an ECU is being programmed on the CAN bus.
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
IlTxResume( void )
{
    /* Set Status Bit to Suspend Interaction Layer Signal Transmissions */
    IlUtilClearStatusBits(&il_status, IL_STATUS_TX_SUSPEND);
}


/* ===========================================================================
//
// Name:            IlEnable
//
// Description:     Enable Interaction Layer Operation
//
//                  This function is intended to support enabling the
//                  Interaction Layer when Network Management transitions from
//                  the Offline to the Online state.
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
IlEnable( void )
{
    /* Enable Interaction Layer Operation */
    il_status = 0;

}

/* ===========================================================================
//
// Name:            IlTxTask
//
// Description:     Interaction Layer Periodic Transmit Task
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
IlTxTask( void )
{
    /* Check for Interaction Layer Task Suspended */
    if (0 == (il_status & (IL_STATUS_SUSPEND | IL_STATUS_TX_SUSPEND)))
    {
        /* Service the Interaction Layer Transmit Task */
        IlTxTaskService( );
    }

}


/* ===========================================================================
//
// Name:            IlHookReceive
//
// Description:     Callback from Multi-Instance CAN Driver Indicating
//                  Message Received
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
IlHookReceive( CAN_RMD const * const pRmd, UINT16 const canFrameHandle )
{
    /* Save the Received Data */
    IlSaveReceivedMessage( pRmd,  canFrameHandle );

}


/* ===========================================================================
//
// Name:            IlHookTransmit
//
// Description:     Callback from Multi-Instance CAN Driver Indicating
//                  Message Transmission Completed
//
//                  This function is assumed to be called with interrupts
//                  masked. This function is called from within a CAN
//                  driver ISR. A flag is set here indicating that the
//                  specified CAN frame has completed transmission that
//                  results in a transmit complete notification to the
//                  application as part of the Interaction Layer task
//                  processing.
//
//                  If the Transmit Message Handle is greater than
//                  0x7FFF, it is assumed to be a transmitted request
//                  frame for a receive message. In this case, 0x8000
//                  must be subtracted from the transmit message
//                  handle to get the handle of the corresponding
//                  receive frame.
//
// Inputs:          hTxMsg: Transmit Message Handle
//
// Returns:         none
//
// =========================================================================*/
void
IlHookTransmit( UINT16 const hTxMsg )
{

     /* Check for a Valid Transmit Message Handle */
    if (((UINT16) IL_TX_NUM_MESSAGES) > hTxMsg)
    {
        /* Check for Transmit Complete Notification Enabled */
        if (0 != (il_tx_attr[hTxMsg] & IL_TX_ATTR_TXC_NOTIFY))
        {
            /* Set Status Bit Indicating Transmit Complete */
            IlUtilSetStatusBits( &il_tx_status[hTxMsg], IL_TX_STATUS_TXC_NOTIFY );

        }

    } 
   

}

/* ===========================================================================
//
// Name:            IlRxTask
//
// Description:     Task to Process Received Messages in the Software Queue
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
IlRxTask( void )
{
    UINT16 iFrame;

    /* Check for Interaction Layer Task Suspended */
    if (0 == (il_status & IL_STATUS_SUSPEND))
    {
        /* Process All Received Frames */
        for ( iFrame = 0; iFrame < ((UINT16) IL_RX_NUM_MESSAGES); iFrame++ )
        {
            IlProcessReceiveFrame( iFrame );

        } 

    } 

}


/* ===========================================================================
//  P R I V A T E   F U N C T I O N S
// =========================================================================*/
/* ===========================================================================
//
// Name:            IlTxTaskService
//
// Description:     Service the Interaction Layer Periodic Transmit Task
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
static void
IlTxTaskService( void )
{

//    CAN_TMD const * pTxTmd;

    UINT16     iHandle;

    BOOLEAN txNow;

//	Canif_Msg_Str fl_str_e;

    /*
    // Service Periodically Transmitted Frames and/or any Frames
    // with Pending Transmission Requests that have been Deferred
    // due to the Minimum Delay between Frames Requirement
    */
    #if (IL_TX_NUM_MESSAGES <= 1)
    iHandle = 0;
    #else
    for (iHandle = 0; iHandle < ((UINT16) IL_TX_NUM_MESSAGES); iHandle++)
    #endif
    {
        /* Reset txNow Flag... */
        txNow = FALSE;

        /* Check for Any Pending Transmission (Event, Periodic or Initialization) */
        if (0 != (il_tx_status[iHandle] & IL_TX_STATUS_EVENT_TX_PENDING))
        {
            /* Transmit Immediately */
            txNow = TRUE;
        }

        /* Check for Periodic Frame Transmission... */
        if (0 != (il_tx_attr[iHandle] & IL_TX_ATTR_PERIODIC))
        {
           
            /* Check for Non-Zero Periodic Count */
            if (0u != il_tx_periodc_counter[iHandle])
            {
                /* Decrement the Periodic Count */
                il_tx_periodc_counter[iHandle]--;
            }

            /* Check for Down Count to Zero */
            if (0u == il_tx_periodc_counter[iHandle])
            {
                 /* Reset the Periodic Counter */
                il_tx_periodc_counter[iHandle] = il_tx_periodic_count[iHandle];

                txNow = TRUE;

            } /* if (0 == *(pTxPeriodic->pTxPeriodicCount)) */

        } /* if (0 != (pTxFrame->txAttributes & IL_TX_ATTR_PERIODIC)) */


        /* Check for Immediate Transmission */
        if (FALSE != txNow)
        {
            /* Transmit the Pending or Scheduled Message Now... */
//            pTxTmd = &il_tx_tmd[iHandle];

            /* Clear Any Pending Delayed Transmit Requests */
            IlUtilClearStatusBits( &il_tx_status[iHandle],
                                   ( IL_TX_STATUS_PERIODIC_TX_PENDING |
                                     IL_TX_STATUS_EVENT_TX_PENDING ));
            /* Alive counter */
            // SCM_MsgAliveCounter_Fun();

        }

		if (0 != (il_tx_status[iHandle] & IL_TX_STATUS_TXC_NOTIFY))
		{			
			if(NULL != il_tx_Notify_ptr[iHandle])
			{
					(*il_tx_Notify_ptr[iHandle])();
			}			
			IlUtilClearStatusBits( &il_tx_status[iHandle], IL_TX_STATUS_TXC_NOTIFY );
		}

    }  /* for (iHandle = 0; iHandle < IL_TX_NUM_MESSAGES; iHandle++) */

}


/* ===========================================================================
//
// Name:            IlInitTx
//
// Description:     Initialize the Transmit Variables
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
static void
IlInitTx( void )
{
    /* Initialize the Status of All Tx Frames */
    IlInitTxFrameStatus( );
    /* Initialize All Tx Frame Data */
    IlInitTxFrameData( );
}
/* ===========================================================================
//
// Name:            IlInitTxStatus
//
// Description:     Initialize the Transmit Frame Status and the Transmit Delay
//                  Count for All Tx Frames
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
IlInitTxStatus( void )
{
    /* Initialize the Status of All Tx Frames */
    IlInitTxFrameStatus( );

}

/* ===========================================================================
//
// Name:            IlInitTxFrameStatus
//
// Description:     Initialize the Transmit Frame Status and the Transmit Delay
//                  Count for All Tx Frames
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
static void
IlInitTxFrameStatus( void )
{

    UINT16  iHandle;

    #if (IL_TX_NUM_MESSAGES <= 1)
    iHandle = 0;
    #else
        /* Initialize Frame Status and Delay Count for All Tx Frames */
    for (iHandle = 0; iHandle < ((UINT16) IL_TX_NUM_MESSAGES); iHandle++)
    #endif
    {
      /* Clear the Frame Status */
        il_tx_status[iHandle] = 0;

    } /* for (iHandle = 0; iHandle < IL_TX_NUM_MESSAGES; iHandle++) */

}

/* ===========================================================================
//
// Name:            IlInitTxFrameData
//
// Description:     Initialize the Transmit Frame Data for All Tx Frames
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
static void
IlInitTxFrameData( void )
{
    UINT8 * pFrameData;

    UINT8 * pFrameInitData;

    UINT16  iHandle;

    UINT8   iByte;

    UINT8  iBytelength;

    #if (IL_TX_NUM_MESSAGES <= 1)
    iHandle = 0;
    #else
    /* Zero the Data for All Transmit Frames */
    for (iHandle = 0; iHandle < ((UINT16) IL_TX_NUM_MESSAGES); iHandle++)
    #endif
    {
        /* Get Pointer to Frame Data */
        pFrameData = (UINT8 *)il_tx_data_ptr[iHandle];

        iBytelength = il_tx_tmd[iHandle].Size;

        if(il_init_tx_per_data[iHandle] == NULL)
        {
          /* Clear the Transmit Frame Data */
            for (iByte = 0; iByte < iBytelength; iByte++)
            {
                pFrameData[ iByte ] = 0;
            } /* for (iByte = 0; iByte < CAN_MAX_DATA_LENGTH; iByte++) */
        }
        else
        {
            pFrameInitData = (UINT8 *)il_init_tx_per_data[iHandle];

            /* Clear the Transmit Frame Data */
            for (iByte = 0; iByte < iBytelength; iByte++)
            {
                pFrameData[ iByte ] = pFrameInitData [iByte];
            }
        }


    }  /* for (iHandle = 0; iHandle < IL_TX_NUM_MESSAGES; iHandle++) */

}

/* ===========================================================================
//
// Name:            IlInitRx
//
// Description:     Initialize the Receive Variables
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
static void
IlInitRx( void )
{
    UINT8 * pFrameData;

    UINT8 iHandle;

    UINT8 iByte;

    UINT8 iBytelength;

    UINT8 iFrame;

    /* Initialize Receive Frame Data, Status Variables */
    for (iHandle = 0; iHandle < IL_RX_NUM_MESSAGES; iHandle++)
    {
        /* Get the Pointer to the Received Frame Data */
        pFrameData = (UINT8 *)il_rx_data_ptr[iHandle];

        iBytelength = il_rx_DLC[iHandle];

        /* Clear the Receive Frame Data */
        for (iByte = 0; iByte < iBytelength; iByte++)
        {
            pFrameData[ iByte ] = 0;
        }

        /* Clear the Status Byte */
        il_rx_status[iHandle]= 0;

    } /* for (iHandle = 0; iHandle < IL_RX_NUM_MESSAGES; iHandle++) */

    
    for ( iFrame = 0; iFrame < IL_RX_NUM_MESSAGES; iFrame++ )
    {   
        il_rx_periodic_counter[iFrame] = il_rx_periodic_count[iFrame];
    }

}

/* ===========================================================================
//
// Name:            IlSaveReceivedMessage
//
// Description:     Save Single Received Message for Subsequent Processing
//
// Inputs:          pRmd: Pointer to Received Message Data Structure
//
// Returns:         none
//
// =========================================================================*/
static void
IlSaveReceivedMessage( CAN_RMD const * const pRmd, UINT16 const canFrameHandle )
{
    UINT8 * pFrameData;

    /* Get Pointer to the Received Frame Data */
    pFrameData = il_rx_data_ptr[canFrameHandle];

    /* Check that RMD Pointer isn't NULL */
    if (NULL != pRmd)
    {
        /*
        Check that DLC of the Received Frame Satisfies the Minimum*/
        
        if (pRmd->Size >= il_rx_DLC[canFrameHandle])
 
        {
            /* Copy the Received Data into the Data Frame */
            IlUtilCopyBytesAtomic( pRmd->Data, pFrameData, il_rx_DLC[canFrameHandle] );

            /* Set the Receive Pending Flag for this Frame */
            IlUtilSetStatusBits(&il_rx_status[canFrameHandle], IL_RX_STATUS_RX_PENDING );
        }

    }

}


/* ===========================================================================
//
// Name:            IlProcessReceiveFrame
//
// Description:     Process a Single Received Frame
//
//                  Processes Each Receive Frame as Part of the Periodic
//                  Receive Task
//
// Inputs:          pRmd: Pointer to Received Message Data Structure
//
// Returns:         none
//
// =========================================================================*/
static void
IlProcessReceiveFrame( UINT16 const canFrameHandle )
{
    BOOLEAN newRxFrame;

    /* Initialize New Received Frame Flag */
    newRxFrame = FALSE;

    /* Check for New Frame Reception */
    if (0 != (il_rx_status[canFrameHandle] &  IL_RX_STATUS_RX_PENDING))
    {
        /* Set New Received Frame Flag */
        newRxFrame = TRUE;
    }
	else
	{
		Set_Receive_Message_Flag();
	}

    if ((0 != (il_rx_attr[canFrameHandle] & IL_RX_ATTR_TIMEOUT_MONITOR)) &&
        (0 == (il_status & IL_STATUS_RX_TIMEOUT_DISABLE )))
    {
        /* Check for Newly Received Frame */
        if (FALSE != newRxFrame)
        {	
            il_rx_periodic_counter[canFrameHandle] = il_rx_periodic_count[canFrameHandle];
            IlUtilClearStatusBits(&il_rx_status[canFrameHandle], IL_RX_STATUS_RX_PENDING );

            if(NULL != pRxInd)
            {
                (*pRxInd)();
            }
        } 
        else
        {
            /* New Frame not Received, Check Rx Timeout Count */
            if ( (UINT16)0 != il_rx_periodic_counter[canFrameHandle])
            {
                /* Decrement the Count */
                il_rx_periodic_counter[canFrameHandle]--;

                /* Check for Timeout... */
                if ((UINT16)0 == il_rx_periodic_counter[canFrameHandle])
                {
                    if(NULL != pRxtimeout)
                    {
                        (*pRxtimeout)();
                    }

                } /* if (0 == (*(pRxFrame->pRxTimeoutCount))) */

            } /*  if ( 0u != (*(pRxFrame->pRxTimeoutCount))) */

        } /* else (not a newly received frame) */
    }
    else
    {
        if (FALSE != newRxFrame)
        {
            IlUtilClearStatusBits(&il_rx_status[canFrameHandle], IL_RX_STATUS_RX_PENDING );
            if(NULL != pRxInd)
            {
                (*pRxInd)();
            }
        }
    }    


}


/* ===========================================================================
//
// Name:            IlUtilCopyBytesAtomic
//
// Description:     Copy Bytes from One Buffer to Another in a Critical Section
//
// Inputs:          pFrom:      "From" Copy Pointer
//
//                  pTo:        "To" Copy Pointer
//
//                  numBytes:   Number of Bytes to Copy
//
// Returns:         none
//
// =========================================================================*/
void
IlUtilCopyBytesAtomic( UINT8 const * const pFrom, UINT8 * const pTo, UINT8 const numBytes )
{

	__disable_irq();

    IlUtilCopyBytes( pFrom, pTo, numBytes );

	__enable_irq();
}

/* ===========================================================================
//
// Name:            IlUtilCopyBytes
//
// Description:     Copy Bytes from One Buffer to Another
//
// Inputs:          pFrom:      "From" Copy Pointer
//
//                  pTo:        "To" Copy Pointer
//
//                  numBytes:   Number of Bytes to Copy
//
// Returns:         none
//
// =========================================================================*/
void
IlUtilCopyBytes( UINT8 const * const pFrom, UINT8 * const pTo, UINT8 const numBytes )
{
    UINT8 iByte;

    for(iByte = 0; iByte < numBytes; iByte++)
    {
        pTo[ iByte ] = pFrom[ iByte ];
    }
}

/* ===========================================================================
//
// Name:            IlUtilSetStatusBits
//
// Description:     Private Function to Set Tx or Rx Status Bits Atomically
//
// Inputs:          pStatus: Pointer to UINT8 Status Byte
//
//                  bitMask: Bit Mask for Bits to Set
//
// Returns:         none
//
// =========================================================================*/
void
IlUtilSetStatusBits( UINT8 * const pStatus, UINT8 const bitMask )
{

    /* Set the Status Bits in a Critical Section */
	__disable_irq();

    (*(pStatus)) |= bitMask;

	__enable_irq();

}

/* ===========================================================================
//
// Name:            IlUtilClearStatusBits
//
// Description:     Private Function to Clear Tx or Rx Status Bits Atomically
//
// Inputs:          pStatus: Pointer to UINT8 Status Byte
//
//                  bitMask: Bit Mask for Bits to Set
//
// Returns:         none
//
// =========================================================================*/
void
IlUtilClearStatusBits( UINT8 * const pStatus, UINT8 const bitMask )
{

    /* Set the Status Bits in a Critical Section */
	__disable_irq();

    (*(pStatus)) &= CAN_MASK8_COMPLEMENT( bitMask );

	__enable_irq();

}



void
IlEnableTxPeriodic (UINT16 const iHandle)
{
    if((il_tx_attr[iHandle] & IL_TX_ATTR_PERIODIC)== IL_TX_ATTR_PERIODIC)
    {
		il_tx_periodc_counter[iHandle] = 0; /* send immediately */
    }
}




/* End of File =============================================================*/
