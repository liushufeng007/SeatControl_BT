#if !defined(VNIM_IF_H)
#define VNIM_IF_H
/* ===========================================================================
//  I N C L U D E   F I L E S
// =========================================================================*/
#define PUBLIC_FN

#include "vnim_types.h"

/* ===========================================================================
//  P U B L I C   T Y P E   D E F I N I T I O N S 
// =========================================================================*/

/* ===========================================================================
//  P U B L I C   T Y P E   D E F I N I T I O N S 
// =========================================================================*/

/*!
 * Public component wake state defintions. Component specific public 
 * wake states should be added between WS_IDLE and WS_RUNNING, lowest 
 * level of functionality to highest going down.
 */
typedef enum
{
    WS_OFF,             /*!< Only the platform_wake_if operations are valid. */
    WS_IDLE,            /*!< No commands, but may accept initial condition 
    						 changes */
	WS_RUNNING_SI,		/*!< VNIM specific wake state, network running with 
							 sleep bit set */
    WS_RUNNING,         /*!< All commands and state changes. */
    WS_HALTED,          /*!< Component is emergency shutdown, initial condition 
    						 changes only*/
    WS_BOOTSTRAP,       /*!< Component is operating in boostrap context with 
    						 limited functionality */
    WS_TRANSITIONING,   /*!< Return value only, component is in a private wake 
    						 state */

    NUM_WAKESTATES

} wake_state_t;

/*!
 * Reset type argument for component reset function.
 */
typedef enum
{
    COLD_RESET, /*!< Component performs a cold reset (retained ram initialized) */
    WARM_RESET  /*!< Component performs a warm reset (retained ram kept) */
}
reset_t;

#define NON_RETAINED static

void 
vnim_task_init (void);

void 
vnim_start_communication (void);

void 
vnim_stop_communication (void);

void 
vnim_task_shutdown (void);

vnim_unsigned8 
vnim_remote_wakeup_inspect (void);

void
vnim_task_KSWarmInit (void);


#endif /* VNIM_IF_H */

/* End of file ============================================================ */

