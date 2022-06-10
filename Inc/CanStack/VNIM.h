#ifndef VNIM_H_INCLUDED
#define VNIM_H_INCLUDED

#include "types.h"

/* #define VNIM_FALSE  0 
#define VNIM_TRUE   1	*/

typedef unsigned8  VnimMsgId;	/* Size of the msg Id */

/* the network wakeup source */
typedef enum
{  NET_WAKE_POWER_ON,
   NET_WAKE_REMOTE,
   NET_WAKE_LOCAL
} WakeupSourceType;

/* the type of function address requested */
typedef enum StoredFunc
{
    VNIM_INIT_CONTROL,
    VNIM_SHUTDOWN_CONTROL,
	VNIM_MAIN_TASK_CONTROL,
	VNIM_OPEN_CONN,
	VNIM_CLOSE_CONN
} StoredFunc;

#define VMsgType unsigned8

/* Type of message:  each is a different bit position */
#define TX_MSG 			1		/* transmit message	*/
#define RX_MSG 			2		/* receive message */
#define STREAM_MSG 		4		/* stream message. > 2 bytes in length */
#define RESET_AFTER_TX 	8		/* (no longer used)	*/
#define EARLY_TX_QUEUE 	8		/* message can be queued prior to vnim_init call */
#define MSG_ONLY 		0x10	/* message contains no data	*/
#define QUEUE_MSG 		0x20	/* message should be queued and handled by vnim_queue */
#define DONT_TRANSMIT 	0x40	/* VNIM does not execute the transmit command directly
								     the lower level network driver software will */
#define LONG_MSG 		0x80	/* long message. Possibly > 8 bytes */

/* Defines used in the Msg Table */
#define SEND_NO_EVENT		0
#define NO_SUPP_INFO		0
#define DATALEN_0			0
#define NO_ADDR				0

/* Specifies which attribute is being inspected in vnim_inspect_attribute
   function */
#define INSPECT_VNIM_STATE	0
#define INSPECT_DEVICE_STATE	1

/* VNIM State information, returned for INSPECT_VNIM_STATE */
typedef enum VState
{
  VNIM_UNINIT,
  VNIM_AWAKE,
  VNIM_ASLEEP
} VState;

/* Device State information, returned for INSPECT_DEVICE_STATE */
typedef enum DeviceState
{
    DEV_UNINIT,
    DEV_NORMAL,
    DEV_QUIET,
    DEV_ERROR,
    DEV_OFFLINE
} DeviceState;

#endif /* VNIM_H_INCLUDED */
