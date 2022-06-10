/* ===========================================================================
//  I N C L U D E   F I L E S
// =========================================================================*/

#define NW_CAN_DLL_C
 

#include "il.h"
#include "il_par.h"
#include "nw_can_dll.h"
#include "vnim_types.h"
#include "vnim_busoff_hndlr.h"
#include "vnim_net.h"
#include "tp_cfg.h"
#include "tp.h"
#include "tp_par.h"
#include "system.h"
//#include "can_comm.h"
#include "Canif.h"
//#include "can_user.h"
#include "Canif.h"  
#include "CanNmCtrl.h"

/* ===========================================================================
//  P R I V A T E   M A C R O S
// =========================================================================*/
/* ===========================================================================
//  Macros Related to Data Link Layer Status
// =========================================================================*/
#define DLL_STATUS_RX_PENDING      BIT0
#define DLL_STATUS_TXC_PENDING     BIT1
#define DLL_STATUS_TXC_OVERFLOW    BIT2

/* ===========================================================================
//  Macros to Support Rx Qualification and Dispatch and Transmit Complete
//  Dispatch and Notification
// =========================================================================*/
#define DLL_RX_IL_FRAME            (0)
#define DLL_RX_DIAG_FRAME          (1)
#define DLL_RX_TP_FRAME            (2)
#define DLL_RX_NM_FRAME            (3)

#define DLL_NUM_RX_VECTORS         (1)

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
#define DLL_RX_VECTOR_0_NUM_RX_IDS     (6)
#else
#define DLL_RX_VECTOR_0_NUM_RX_IDS     (4)
#endif



/* ===========================================================================
//  This is to be defined in nw_diag.h
// =========================================================================*/
/*#define DLL_DIAG_TX_FRAME_HANDLE           (0x700) */
 
/* ===========================================================================
//  Transmit Complete Queue Size
//
//  Initially Define it as Number of Interaction Layer TMD's + 2, Times 2
//  The additional 2 are for a Diagnostic and an NM transmission
// =========================================================================*/
#define DLL_TXC_QUEUE_SIZE     ((IL_TX_NUM_IDS + 2) * 2)

#define DLL_IL_RX_SUSPEND            (4)
#define DLL_IL_TX_SUSPEND            (8)
/* ===========================================================================
//  P R I V A T E  T Y P E  D E F I N I T I O N S
// =========================================================================*/
typedef struct tagDLL_RX_VECTOR_DISPATCH
{
    UINT32  const identifier;     /* CAN Frame Identifier         */

    UINT16  const frmHandle;      /* CAN Frame Handle (Index)     */

    UINT8   const frmLayer;       /* Network Layer to Receive Frame  */

} DLL_RX_VECTOR_DISPATCH;


typedef struct tagDLL_RX_DISPATCH
{
    UINT8   const numIDs;                               /* Number of ID's in the Vector */

    DLL_RX_VECTOR_DISPATCH const * const  pVectorDispatch; /* Pointer to Vector Dispatch   */

} DLL_RX_DISPATCH;


/* ===========================================================================
//  P R I V A T E   F U N C T I O N   P R O T O T Y P E S
// =========================================================================*/
static void
DllDispatchReceivedMessage( CAN_RMD const * const pRmd );



/* ===========================================================================
//  P R I V A T E   M E M O R Y   A L L O C A T I O N
// =========================================================================*/
/* ===========================================================================
//  Data Link Layer Status Variable
// =========================================================================*/
static UINT8 dll_status;
static UINT8 busoff_sts;
/* ===========================================================================
//  Transmit Complete Queue Variables
// =========================================================================*/
/* ===========================================================================
//  CAN Hardware Receive Vector Qualification Data Structures
//
//  This set of data structures defines the CAN Identifiers that are received
//  by each CAN receive filter/mask combination (termed a receive "vector").
//  A specific filter mask/combination may qualify multiple CAN Identifiers,
//  so this table is needed to further qualify which CAN ID's that pass a
//  specific filter/mask combination are valid and therefore stored in the
//  software receive queue for further processing.
//
// =========================================================================*/

/* CAN Hardware Receive Vector 0 */

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
static DLL_RX_VECTOR_DISPATCH const
dllRxIdsVector0[  DLL_RX_VECTOR_0_NUM_RX_IDS ] =
{
	{ 0x123u, ((UINT16)  IL_RX_MESSAGE_VCU_123), DLL_RX_IL_FRAME },	
	{ 0x234u, ((UINT16)  IL_RX_MESSAGE_ABM_234), DLL_RX_IL_FRAME },
    { 0x3B7u, ((UINT16)  IL_RX_MESSAGE_IHU_3B7), DLL_RX_IL_FRAME },
    { 0x458u, ((UINT16)  IL_RX_MESSAGE_BCM_458), DLL_RX_IL_FRAME },
    { 0x741u, ((UINT16)  IL_RX_MESSAGE_DIAG_PHY_741), DLL_RX_TP_FRAME },
    { 0x7DFu, ((UINT16)  IL_RX_MESSAGE_DIAG_FUN_7DF), DLL_RX_TP_FRAME },
};
#else
static DLL_RX_VECTOR_DISPATCH const
dllRxIdsVector0[  DLL_RX_VECTOR_0_NUM_RX_IDS ] =
{
	{ 0x245u, ((UINT16)  IL_RX_MESSAGE_BCM_245), DLL_RX_IL_FRAME },	
	{ 0x457u, ((UINT16)  IL_RX_MESSAGE_IHU_457), DLL_RX_IL_FRAME },
    { 0x7A1u, ((UINT16)  IL_RX_MESSAGE_DIAG_PHY_7A1), DLL_RX_TP_FRAME },
    { 0x7DFu, ((UINT16)  IL_RX_MESSAGE_DIAG_FUN_7DF), DLL_RX_TP_FRAME },
};
#endif

/* ===========================================================================
//  Received Frame Dispatch Table
//
//  This data structure is an array of pointers to the data structures that
//  define the received CAN ID's that are to be qualified for each hardware
//  receive vector. A hardware receive vector corresponds to a CAN filter/mask
//  combination that qualifies received CAN messages that may need to be
//  further filtered at the software level.
//
// =========================================================================*/

static DLL_RX_DISPATCH const
dllRxDispatchTable[ DLL_NUM_RX_VECTORS ] =
{
    { DLL_RX_VECTOR_0_NUM_RX_IDS, &dllRxIdsVector0[ 0 ] }

};

uint32_t dllRxDispatch_NmIdRange[2] = {0x600,0x6FF}; 

/* ===========================================================================
//  P U B L I C   F U N C T I O N S
// =========================================================================*/
/* ===========================================================================
//
// Name:            DllInit
//
// Description:     Initialization of the Data Link Layer
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
DllInit( void )
{
    /* Initialize the DLL Layer Status */
    dll_status = 0;
    busoff_sts = FALSE;
}


/* ===========================================================================
//
// Name:            DllRxTask
//
// Description:     Task to Process Received CAN Frames in the Software
//                  Receive Queue and Dispatch them to the Appropriate Layer
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void
DllRxTask( void )
{
    CAN_RMD canTempRmd;

	Canif_Msg_Str fl_str_e;

    UINT8  rC;
    UINT8  i;
    
    /* Check for Pending Receive Messages to Process */
    if (0 != (dll_status & (UINT8)DLL_STATUS_RX_PENDING))
    {
        /* Process Receive Queue to Completion */
        do
        {
            /* Get the Next Received Message */
            rC = Canif_rx_queue_pull_e( &fl_str_e);

            /* Check for Valid Received Message */
            if (TRUE  == rC)
            {
                /* Dispatch the Received Message */
                for(i=0; i<8 ;i++)
                {
                    canTempRmd.Data[i] = fl_str_e.Data[i];
                }
			    canTempRmd.Identifier = fl_str_e.StdId;
			    canTempRmd.Size = 8;
			    canTempRmd.Vector = 0;
                
			    /******************************************/ 
                
                DllDispatchReceivedMessage( &canTempRmd );
            }

        } while(TRUE == rC);

        /* Clear the Receive Pending Status Bit */
		__disable_irq();
		dll_status &= (~DLL_STATUS_RX_PENDING);
		__enable_irq();

    } /* if (0 != (vnim_dll_status & VNIM_DLL_STATUS_RX_PENDING)) */

}

/* ===========================================================================
//
// Name:            CanAppHookReceive
//
// Description:     Application Callback for Received Multi-Instance CAN
//                  Messages
//
// Inputs:          hMv:     Handle to Message Vector
//
//
// Returns:         none
//
// =========================================================================*/
void
CanAppHookReceive(void)
{
	/* Set Flag Indication Received Frames to Process */
	//__disable_irq();
	dll_status |= DLL_STATUS_RX_PENDING;
	//__enable_irq();
}

/* ===========================================================================
//
// Name:            CanAppHookTransmit
//
// Description:     Application Callback for Received Multi-Instance CAN
//                  Messages
//
// Inputs:          hTxMsg:  Transmit Message Handle
//
//                  hMv:     Handle to Message Vector
//
//                  hwInst:  Hardware Instance
//
// Assumptions:     Interrupts are Masked
//
// Returns:         none
//
// =========================================================================*/
void
CanAppHookTransmit( UINT16 const hTxMsg, SINT8 const hMv )
{

	if( hTxMsg == VTP_MESSAGE_HANDLE )
	{
		VTP_Hook_Transmit(hTxMsg);
	}
	else
	{
		IlHookTransmit( hTxMsg );
	}
  

	if(busoff_sts == TRUE)
	{
		busoff_sts = FALSE;
		Appl_CAN_busoff_end();
	}
}

/* ===========================================================================
//
// Name:            CanAppHookOpMode
//
// Description:     Application Callback for Multi-Instance CAN
//                  Operational Mode Changes
//
// Inputs:          hOm:     Handle to Operational Mode
//
//                  hwInst:  Hardware Instance
//
// Returns:         none
//
// =========================================================================*/
void
CanAppHookOpMode( UINT8 const hOm )
{
	/* Notify Network Management of Operational Mode Transition */
	/* Call the appropriate function */
	if(hOm == CANH_OM_WAKEUP)
	{
		//Appl_Can_Wakeup();
	}
}

/* ===========================================================================
//
// Name:            CanAppHookException
//
// Description:     Application Callback for Multi-Instance CAN
//                  Exception Event
//
// Inputs:          hEx:     Handle to Operational Mode
//
//                  hwInst:  Hardware Instance
//
// Returns:         none
//
// =========================================================================*/
void
CanAppHookException( UINT8 const hEx )
{
  	if(CANEX_BUSOFF == hEx)
  	{
    	busoff_sts = TRUE;
    	//Appl_CAN_busoff_start(); 
  	}
    else
    {
    	if(CANEX_TXQ_ERROR == hEx)
        {
        }
        else
        {
        	if(CANEX_TMDQ_ERROR == hEx)
            {
            }
        }
    }
}


/* ===========================================================================
//
// Name:            DllDispatchReceivedMessage
//
// Description:     Dispatch Received Message to Appropriate VNIM Layer
//
// Inputs:          pRmd: Pointer to Received Message Data Structure
//
// Returns:         none
//
// =========================================================================*/
static void
DllDispatchReceivedMessage( CAN_RMD const * const pRmd )
{
    DLL_RX_VECTOR_DISPATCH const * pRxDispatch;

    UINT16 canFrameHandle;

    UINT8  filtVector;

    UINT8  numVectorIDs;

    UINT8  idIndex;

    UINT8  matchFound;

    UINT8  networkLayer;

    /* Initialize Variables */
    matchFound = FALSE;

    /* Get the Hardware Filter Vector that Passed this Received Message */
    filtVector = pRmd->Vector;

    numVectorIDs = dllRxDispatchTable[ filtVector ].numIDs;

    pRxDispatch  = dllRxDispatchTable[ filtVector ].pVectorDispatch;

    idIndex = 0;

	if((dllRxDispatch_NmIdRange[0] <= pRmd->Identifier)&& (dllRxDispatch_NmIdRange[1] >= pRmd->Identifier))
	{
		CanNmCtrl_NmRx_Indication(0,(UINT8 * )pRmd->Data);
	}
	else
	{
    do
    {
        /* Check for Identifier Match */
        if (pRmd->Identifier == pRxDispatch[ idIndex ].identifier)
        {
            matchFound = TRUE;
        }
        else
        {
            /* Check the Next Identifier */
            idIndex++;
        }

    } while ((idIndex < numVectorIDs) && (FALSE == matchFound));

    /* Check if Match was Found */
    if (TRUE == matchFound)
    {
        /* Get the CAN Frame Handle */
        canFrameHandle = pRxDispatch[ idIndex ].frmHandle;

        /* Determine Which VNIM Layer Gets the Received Message */
        networkLayer = pRxDispatch[ idIndex ].frmLayer;    

        if((networkLayer == DLL_RX_IL_FRAME)&&(0 == (dll_status & ((UINT8)DLL_IL_RX_SUSPEND))))
        {
          /* Interaction Layer */
           IlHookReceive( pRmd, canFrameHandle );
        }
        else if(networkLayer == DLL_RX_TP_FRAME)
        {
            VTP_Hook_Receive(pRmd, canFrameHandle );
        }
        else
        {
        }

    } /* if (TRUE == matchFound) */
	}

}

SINT8
DllTransmit (SINT8 const hMv, CAN_TMD const * const pTmd)
{
	SINT8 ret_type;

	Canif_Msg_Str fl_str_e;

	fl_str_e.Data[0]=pTmd->pData[0];
	fl_str_e.Data[1]=pTmd->pData[1];
	fl_str_e.Data[2]=pTmd->pData[2];
	fl_str_e.Data[3]=pTmd->pData[3];
	fl_str_e.Data[4]=pTmd->pData[4];
	fl_str_e.Data[5]=pTmd->pData[5];
	fl_str_e.Data[6]=pTmd->pData[6];
	fl_str_e.Data[7]=pTmd->pData[7];
	fl_str_e.StdId=pTmd->Identifier;
//	fl_str_e.Can_info.IDE= 1;
	fl_str_e.DLC=8;

	ret_type = Canif_tx_queue_push_e(fl_str_e);

    return(ret_type);
}

void dll_il_rx_suspend(void)
{
	__disable_irq();
	dll_status |= DLL_IL_RX_SUSPEND;
	__enable_irq();
}

void dll_il_rx_resume(void)
{
	__disable_irq();
	dll_status &= (~DLL_IL_RX_SUSPEND);
	__enable_irq();
}

/* End of File =============================================================*/
