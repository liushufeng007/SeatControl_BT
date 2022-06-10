#if !defined(VNIM_NET_H)
#define VNIM_NET_H

#include "vnim_types.h"

/* ===========================================================================
//  P U B L I C   M A C R O S
// =========================================================================*/

#define VNIM_CAN_FRAME_8BYTES   (8)
#define VNIM_CAN_FRAME_7BYTES   (7)
#define VNIM_CAN_FRAME_6BYTES   (6)
#define VNIM_CAN_FRAME_5BYTES   (5)
#define VNIM_CAN_FRAME_4BYTES   (4)
#define VNIM_CAN_FRAME_3BYTES   (3)
#define VNIM_CAN_FRAME_2BYTES   (2)
#define VNIM_CAN_FRAME_1BYTES   (1)
#define VNIM_CAN_FRAME_0BYTES   (0)

#define VNIM_CAN_8BITS          (8)
#define VNIM_CAN_7BITS          (7)
#define VNIM_CAN_6BITS          (6)
#define VNIM_CAN_5BITS          (5)
#define VNIM_CAN_4BITS          (4)
#define VNIM_CAN_3BITS          (3)
#define VNIM_CAN_2BITS          (2)
#define VNIM_CAN_1BITS          (1)
#define VNIM_CAN_0BITS          (0)

#define VNIM_CAN_INDEX8         (8)
#define VNIM_CAN_INDEX7         (7)
#define VNIM_CAN_INDEX6         (6)
#define VNIM_CAN_INDEX5         (5)
#define VNIM_CAN_INDEX4         (4)
#define VNIM_CAN_INDEX3         (3)
#define VNIM_CAN_INDEX2         (2)
#define VNIM_CAN_INDEX1         (1)
#define VNIM_CAN_INDEX0         (0)

#define VNIM_NOTHING

/* ===========================================================================
//  P U B L I C   T Y P E   D E F I N I T I O N S
// =========================================================================*/

/* Type Definition for VNIM Bootstrap Mode */
typedef enum
{
    VNIM_COLD_BOOT,
    VNIM_WARM_BOOT,
    VNIM_WAKEUP_BOOT,
    VNIM_LVI_RECOVERY_BOOT,
    VNIM_BUSOFF_RECOVERY_INIT,
    VNIM_NUM_BOOT_MODES

} vnim_bootstrap_mode_type;

enum
{
  VNIM_LOCAL_WAKEUP,
  VNIM_REMOTE_WAKEUP,
  VNIM_POWER_ON_WAKEUP
};

enum
{
  VNA_NETWORK_SLEEP_MSGID,
  VNA_NETWORK_FAULTED_MSGID,
  VNA_LOCAL_WAKEUP_MSGID,
  VNA_NETWORK_POWER_UP_MSGID,
  VNA_NETWORK_WAKEUP_MSGID
};

/* ===========================================================================
//  P U B L I C   F U N C T I O N   P R O T O T Y P E S
// =========================================================================*/
VNIM_BOOL
NW_Initialize(vnim_bootstrap_mode_type const);

VNIM_BOOL
NW_Shutdown(void);

VNIM_BOOL
NW_Disable(void);

void
NW_OpenConnection(void);

void
NW_CloseConnection(void);

void
NW_Task(void);

void Appl_Can_Wakeup (void);

void Network_TxSuspend(void);
void Network_TxResume(void);

#endif /* VNIM_NET_H */

/* End of file ============================================================ */
