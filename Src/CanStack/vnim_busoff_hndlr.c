/* ===========================================================================
//
//  Name:           vnim_busoff_hndlr.c
//
//  Description:    VNIM Bus OFF handler
//
//  Organization:   Driver Information Systems
//
// =========================================================================*/
#include "vnim_types.h"
#include "vnim_net.h"
#include "il.h"
#include "vnim_busoff_hndlr.h"
#include "vnim_can_cfg.h"


static vnim_unsigned8 nw_num_fast_busoff ;
static vnim_unsigned16 nw_busoff_cntr ;
static vnim_unsigned8 nw_busoff_status_bool = FALSE;

/* ===========================================================================
//
// Name:            vnim_bus_off_hndlr_init
//
// Description:     Bus off handler init
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void vnim_bus_off_hndlr_init (void)
{
  nw_num_fast_busoff = NW_NUM_FAST_BUSOFF;
  nw_busoff_cntr = 0;
}

/* ===========================================================================
//
// Name:            vnim_bus_off_hndlr_periodic
//
// Description:     Periodic task for bus off error monitoring
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void vnim_bus_off_hndlr_periodic (void)
{
  if(nw_busoff_cntr > (vnim_unsigned16)0)
  {
    nw_busoff_cntr --;
    
    if(nw_busoff_cntr == (vnim_unsigned16)0)
    {
      (void) NW_Initialize(VNIM_BUSOFF_RECOVERY_INIT); 
    }
  }
}

/* ===========================================================================
//
// Name:            Appl_CAN_busoff_start
//
// Description:     Application callback from NOS on BUS OFF error
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void Appl_CAN_busoff_start (void)
{
  nw_busoff_status_bool = TRUE;
  IlSuspend();
  #if (NW_NUM_FAST_BUSOFF > 0)  
  if(nw_num_fast_busoff > 0)
  {
    #if (NW_FAST_BUSOFF_TIMECNT > 0)
    nw_busoff_cntr = NW_FAST_BUSOFF_TIMECNT;
    #else
    (void) NW_Initialize(VNIM_BUSOFF_RECOVERY_INIT);
    #endif
    nw_num_fast_busoff--;
  }
  else
  {
    nw_busoff_cntr = NW_SLOW_BUSOFF_TIMECNT;
  }
  
  #else
  nw_busoff_cntr = NW_SLOW_BUSOFF_TIMECNT;
  #endif
}
/* ===========================================================================
//
// Name:            Appl_CAN_busoff_end
//
// Description:     Application Call back on Bus OFF recovery
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
void Appl_CAN_busoff_end(void)
{
  vnim_bus_off_hndlr_init();
  nw_busoff_status_bool = FALSE;
}

/* ===========================================================================
//
// Name:            vnim_unsigned8 can_bus_off_count
//
// Description:     
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
vnim_unsigned8 can_bus_off_count(void)
{
	vnim_unsigned8 i;

    i = NW_NUM_FAST_BUSOFF - nw_num_fast_busoff;
    
	return(i);
}
/* ===========================================================================
//
// Name:            vnim_unsigned8 can_bus_off_count
//
// Description:     
//
// Inputs:          none
//
// Returns:         none
//
// =========================================================================*/
vnim_unsigned8 vnim_get_busoff_status(void)
{
	return(nw_busoff_status_bool);
}

