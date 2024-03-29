/* ===========================================================================
//
//  Name:           il_par.c
//
//  Description:    Interaction Layer Tx and Rx Parameters
//
//                  Application Specific Tx and Rx Message and Signal
//                  Data Structure Definitions
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
#include "rx_can.h"
#include "vnim_app_signals.h"
/* ===========================================================================
//  P R I V A T E   M A C R O S
// =========================================================================*/

/* ===========================================================================
//  Interaction Layer Transmit Message (Frame) Handles
// =========================================================================*/

/* ===========================================================================
//  M E M O R Y   A L L O C A T I O N
// =========================================================================*/

enum
{
  IL_TX_MESSAGE_HNDL_45B,

};

_SCM_L_SCM_msg_buf              il_tx_SCM_L_SCM_msg;


const UINT16 il_tx_periodic_count[IL_TX_NUM_MESSAGES] = 
{
  IL_TIME_IN_TASK_TICS(100),
};

const UINT8 il_tx_attr[IL_TX_NUM_MESSAGES] = 
{
  (IL_TX_ATTR_PERIODIC|IL_TX_ATTR_TXC_NOTIFY),
};

const UINT8* il_init_tx_per_data[IL_TX_NUM_MESSAGES] = 
{
  NULL
};

const UINT8* il_tx_data_ptr[IL_TX_NUM_MESSAGES]=
{
  &il_tx_SCM_L_SCM_msg.byte[0],
};

UINT8 il_tx_status[IL_TX_NUM_MESSAGES];

UINT16 il_tx_periodc_counter[IL_TX_NUM_MESSAGES];
/*Add Tx for A90*/

CAN_TMD const il_tx_tmd[IL_TX_NUM_MESSAGES] =
{
  {
    CAN_GPNUM_8,                     /* CAN message data length  */
 
    0x19FF2A36,                           /* CAN message identifier   */

    &il_tx_SCM_L_SCM_msg.byte[0],    /* Pointer to Data          */

    CANB_TX_STD_DATA,                /* CAN message options      */

    IL_TX_MESSAGE_HNDL_45B           /* Transmit Message Handle  */

  } 
};
  

/* RX frame timeout: frame periodic time*5 */
_VCU_DDSS_msg_buf     il_rx_VCU_DDSS_msg;
_SCM_L_SCM_msg_buf     il_rx_Diver_Conference_msg;
_ABM_VIST_msg_buf     il_rx_ABM_VIST_msg;
_BCM_458_msg_buf     il_rx_BCM_458_msg;

const pIL_TX_FRAME_INDICATION il_tx_Notify_ptr[IL_TX_NUM_MESSAGES]=
{
	Vnim_SCM_Send_notify_handler,
};

UINT8* il_rx_data_ptr[]=
{ 
	&il_rx_VCU_DDSS_msg.byte[0],
  &il_rx_ABM_VIST_msg.byte[0],
  &il_rx_Diver_Conference_msg.byte[0],
	&il_rx_BCM_458_msg.byte[0]
};

/*Add for Rx A90*/
const UINT8 il_rx_attr[] = 
{
  (IL_RX_ATTR_PERIODIC | IL_RX_ATTR_TIMEOUT_MONITOR),
  (IL_RX_ATTR_PERIODIC | IL_RX_ATTR_TIMEOUT_MONITOR),
  (IL_RX_ATTR_PERIODIC | IL_RX_ATTR_TIMEOUT_MONITOR),
  (IL_RX_ATTR_PERIODIC | IL_RX_ATTR_TIMEOUT_MONITOR)
};

UINT8 il_rx_status[IL_RX_NUM_MESSAGES];
UINT16 il_rx_periodic_counter[IL_RX_NUM_MESSAGES];
_SCM_L_45A_msg_buf              il_tx_SCM_L_45A_msg;

const UINT8 il_rx_DLC[]=
{
  sizeof(il_rx_VCU_DDSS_msg),
  sizeof(il_rx_ABM_VIST_msg),
  sizeof(il_rx_Diver_Conference_msg),
  sizeof(il_rx_BCM_458_msg)
};


const pIL_RX_FRAME_INDICATION il_rx_indication_fn[]=
{
  Rx_VCU_DDSS_func,
  Rx_ABM_VIST_func,
  Rx_Driver_Conference_func,
  Rx_BCM_458_func
};


const pIL_RX_FRAME_LOSS_INDICATION il_rx_timeout_fn[]=
{
  Vnim_VCU_123_Timeout,
  Vnim_ABM_234_Timeout,
  (void*)0,
  Vnim_BCM_458_Timeout
};

/* RX frame timeout: frame periodic time*2 */
const UINT16 il_rx_periodic_count[IL_RX_NUM_MESSAGES] = 
{
  IL_TIME_IN_TASK_TICS(10*5),          /* Receive_Message_123 */ 
  IL_TIME_IN_TASK_TICS(200*5),         /* Receive_Message_234 */
  0,                                 /* Receive_Message_3B7 */
  IL_TIME_IN_TASK_TICS(100*5),         /* Receive_Message_458 */
};


/* End of File =============================================================*/
