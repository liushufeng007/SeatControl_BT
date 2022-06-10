#if !defined(NW_CAN_DLL_H)
#define NW_CAN_DLL_H

#include "system.h"

/* ===========================================================================
//
//  Name:           nw_can_dll.h
//
//  Description:    Network Single-Instance CAN Data Link Layer Header File
//
//  Organization:   Multiplex Core Technology
//
// =========================================================================*/

/* ===========================================================================
//  P U B L I C   T Y P E   D E F I N I T I O N S
// =========================================================================*/

/* ===========================================================================
//  P U B L I C   M A C R O S
// =========================================================================*/



/* ===========================================================================
//  P U B L I C   F U N C T I O N   P R O T O T Y P E S
// =========================================================================*/
extern void DllInit( void );

extern void DllTxTask( void );

extern void DllRxTask( void );

extern void dll_il_rx_suspend(void);

extern void dll_il_rx_resume(void);


SINT8 DllTransmit (SINT8 const hMv, CAN_TMD const * const pTmd);

void
CanAppHookTransmit( UINT16 const hTxMsg, SINT8 const hMv );

void
CanAppHookReceive( void);

#endif /* VNIM_MICAN_DLL_H */

/* End of file ============================================================ */
