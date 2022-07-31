#if !defined(VNIM_APP_SIGNALS_H)
#define VNIM_APP_SIGNALS_H


/*============================================================================
** I N C L U D E   F I L E S
**==========================================================================*/
#include "vnim_types.h"

/* ========================================================
//   Tx Message IDs
// ========================================================*/
#define INVALID_TIME_2SEC       2000 
#define INVALID_TIME            INVALID_TIME_2SEC
#define INVALID_TIMER_MAX       (INVALID_TIME/16)

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
extern BOOLEAN msg_123_missing;
extern BOOLEAN msg_DDSS_never_recvd;
extern BOOLEAN msg_234_missing;
extern BOOLEAN msg_VIST_never_recvd;
extern BOOLEAN msg_3B7_missing;
extern BOOLEAN msg_3B7_never_recvd;
extern BOOLEAN msg_458_missing;
extern BOOLEAN msg_458_never_recvd;
extern UINT8 Dis_SCM_HMI_Req_Count;
extern UINT8 Dis_SCM_HMI_Req;
#else
extern BOOLEAN msg_245_missing;
extern BOOLEAN msg_245_never_recvd;
extern BOOLEAN msg_457_missing;
extern BOOLEAN msg_457_never_recvd;
#endif


/* CAN baudrate adaptive config macro */
#define CAN_ADAPTIVE_ENABLE     0

extern void vnim_reset_wakeup_data(void);

extern void vnim_missing_timer_update(void);

extern void vnim_disable_tx (void);

extern void vnim_enable_tx (void);

extern void vnim_enable_tp_tx(void);

extern void vnim_disable_tp_tx(void);


extern void vnim_busoff_set_missing_flag(void);

extern void Vnim_SCM_Send_notify_handler(void);

extern uint8_t Get_MsgAlive_Counter(void);


/* the following functions for the old can application interface start */
#define RCAN_RX_ERROR   	((vnim_unsigned8)0x80)
#define RCAN_SIG_MISSING    ((vnim_unsigned8)0x40)
#define RCAN_SIG_INVLD    	((vnim_unsigned8)0x20)
#define RCAN_SIG_INVDET   	((vnim_unsigned8)0x10)
#define RCAN_SIG_NVR_RCVD 	((vnim_unsigned8)0x08)
#define RCAN_SIG_RCVD_NEW   ((vnim_unsigned8)0x04) 

#define IlSetEvent_LS(c)    IlUtilSetStatusBits(&il_tx_status[c],IL_TX_STATUS_EVENT_TX_PENDING)
#define IlSetPeriod_LS(c)   IlUtilSetStatusBits(&il_tx_status[c],IL_TX_STATUS_PERIODIC_TX_PENDING)

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
/*  RX  MESSAGE MISSING MACROS  */
#define VNIM_IS_VCU_123_MSG_MISSING()          (msg_123_missing)
#define VNIM_IS_ABM_234_MSG_MISSING()          (msg_234_missing)
#define VNIM_IS_IHU_3B7_MSG_MISSING()          (msg_3B7_missing)
#define VNIM_IS_BCM_458_MSG_MISSING()          (msg_458_missing)


/*  RX  MESSAGE NEVER RECEIVED MACROS  */
#define VNIM_IS_VCU_123_MSG_NEVER_RECVD()      (msg_DDSS_never_recvd)
#define VNIM_IS_ABM_234_MSG_NEVER_RECVD()      (msg_VIST_never_recvd)
#define VNIM_IS_IHU_3B7_MSG_NEVER_RECVD()      (msg_3B7_never_recvd)
#define VNIM_IS_BCM_458_MSG_NEVER_RECVD()      (msg_458_never_recvd)

extern void Vnim_VCU_123_Timeout(void);
extern void Vnim_ABM_234_Timeout(void);
extern void Vnim_IHU_3B7_Timeout(void);
extern void Vnim_BCM_458_Timeout(void);
#else
/*  RX  MESSAGE MISSING MACROS  */
#define VNIM_IS_BCM_245_MSG_MISSING()          (msg_245_missing)
#define VNIM_IS_BCM_457_MSG_MISSING()          (msg_457_missing)


/*  RX  MESSAGE NEVER RECEIVED MACROS  */
#define VNIM_IS_BCM_245_MSG_NEVER_RECVD()      (msg_245_never_recvd)
#define VNIM_IS_BCM_457_MSG_NEVER_RECVD()      (msg_457_never_recvd)

extern void Vnim_BCM_245_Timeout(void);

extern void Vnim_BCM_457_Timeout(void);
#endif



extern void vnim_any_super_state_to_limit_timeout_counter_and_missing_flag(void);



#endif

