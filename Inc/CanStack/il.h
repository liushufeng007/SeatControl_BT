#if !defined(IL_H)
#define IL_H

#include "system.h"
/* ===========================================================================
//  P U B L I C   T Y P E   D E F I N I T I O N S
// =========================================================================*/

/* Frame Transmit Complete Callback Function Pointer Type Definition */
typedef void (* pIL_TX_FRAME_INDICATION)( void );

/* Burst Mode Transmission Complete Callback Function Pointer Type Definition */
typedef void (* pIL_TX_BURST_COMPLETE_INDICATION)( void );

/* Frame Reception Indication Callback Function Pointer Type Definition */
typedef void (* pIL_RX_FRAME_INDICATION)( void );

/* Frame Reception Timeout Gain/Loss Callback Function Pointer Type Definition */
typedef void (* pIL_RX_FRAME_LOSS_INDICATION)( void );

/* ===========================================================================
//  P U B L I C   M A C R O S
// =========================================================================*/
/* Conversion from Time in Milliseconds to Interaction Layer Task Tics */
#define IL_TIME_IN_TASK_TICS( tMs )  ((UINT16) (((tMs) /IL_TASK_PERIOD_MS)))

/* Transmit Frame Attributes */
#define IL_TX_ATTR_DEFAULT                 (0x00u)
#define IL_TX_ATTR_EVENT                   BIT0
#define IL_TX_ATTR_PERIODIC                BIT1
#define IL_TX_ATTR_BURST                   BIT2
#define IL_TX_ATTR_TXC_NOTIFY              BIT3
#define IL_TX_ATTR_INIT                    BIT4

/* Transmit Frame Status */
#define IL_TX_STATUS_BURST_ACTIVE          BIT0
#define IL_TX_STATUS_BURST_COMPLETE        BIT1
#define IL_TX_STATUS_PERIODIC_TX_PENDING   BIT2
#define IL_TX_STATUS_EVENT_TX_PENDING      BIT3
#define IL_TX_STATUS_TXC_NOTIFY            BIT4
#define IL_TX_STATUS_INIT                  BIT5
#define IL_TX_STATUS_PERIODIC              BIT6


/* Receive Frame Attributes */
#define IL_RX_ATTR_DEFAULT                 (0x00u)
#define IL_RX_ATTR_PERIODIC                BIT0
#define IL_RX_ATTR_TIMEOUT_MONITOR         BIT1
#define IL_RX_ATTR_RECEIVE_REQUEST         BIT2

/* Receive Frame Status Bits */
#define IL_RX_STATUS_RX_PENDING            BIT0
#define IL_RX_STATUS_LOSS_NOTIFIED         BIT2


/* Interaction Layer Status Bits */
#define IL_STATUS_SUSPEND                  BIT0
#define IL_STATUS_TX_SUSPEND               BIT1
#define IL_STATUS_RX_TIMEOUT_DISABLE       BIT2
/* Frame Type Mask (Type 1 or Type 2 in CAN ID) */
#define IL_FRAME_TYPE_MASK                 ((UINT32) 0x00F80000u)

/* PF - DA Mask  (PF and DA Bytes in CAN ID) */
#define IL_PF_DA_MASK                      ((UINT32) 0x00FFFF00u)

extern UINT8 il_status;

/* ===========================================================================
//  P U B L I C   F U N C T I O N   P R O T O T Y P E S
// =========================================================================*/

/* ===========================================================================
// Transmit (Put) and Receive ( Get) Functions
// =========================================================================*/
void
IlInit( void );

void
IlTxTask( void );

void
IlRxTask( void );

void
IlHookReceive( CAN_RMD const * const, UINT16 const );

void
IlHookTransmit( UINT16 const );

void
IlEnableTxPeriodic (UINT16 const);

/* ===========================================================================
// Interaction Layer Suspend, Resume and Enable Functions
// =========================================================================*/
void
IlSuspend( void );

void
IlResume( void );

void
IlTxSuspend( void );

void
IlTxResume( void );

void
IlEnable( void );

void
IlInitTxStatus( void );


/* ===========================================================================
// Receive Message Timeout Control Functions
// =========================================================================*/

void
IlUtilCopyBytesAtomic( UINT8 const * const, UINT8 * const, UINT8 const );

void
IlUtilCopyBytes( UINT8 const * const, UINT8 * const, UINT8 const );

void
IlUtilSetStatusBits( UINT8 * const, UINT8 const );

void
IlUtilClearStatusBits( UINT8 * const, UINT8 const );


#endif /* IL_H */

/* End of file ============================================================ */
