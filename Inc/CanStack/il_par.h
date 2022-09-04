#if !defined(VNIM_CAN_IL_PAR_H)
#define VNIM_CAN_IL_PAR_H

#include "system.h"
#include "il.h"
/* Variant configuration */
#define SCM_L_VARIANT 0
#define SCM_R_VARIANT 1
#define SCM_SEATCONTROL_VARIANT SCM_R_VARIANT

/* ===========================================================================
//  P U B L I C   M A C R O S
// =========================================================================*/

/* APP mssage ID 45B tx */
typedef struct {
	/* Byte0 */
	UINT8                      Pos: 8;
	/* Byte1 */
	UINT8                      angle_bacck: 5;//bit0~bit4
	UINT8                      conference_mode: 2;//bit5~bit6
	UINT8                      reserved_bit_0: 1;
	/* Byte2 */
	UINT8                      autopilot_mode: 2;
	UINT8                      reserved_bit_1: 6;
	/* Byte3 */            
	UINT8                      reserved_byte0: 8;
	/* Byte4 */       
	UINT8                      reserved_byte1: 8;
	/* Byte5 */   
	UINT8                      reserved_byte2: 8;
	/* Byte6 */
	UINT8                      reserved_byte3: 4;
	/* Byte7 */
	UINT8                      reserved_byte4: 8;
}_SCM_L_SCM_msgType;

typedef union {
	UINT8 byte[8];
	_SCM_L_SCM_msgType	SCM_L_SCM_msg;
}_SCM_L_SCM_msg_buf;

extern _SCM_L_SCM_msg_buf			        il_tx_SCM_L_SCM_msg;

/* Frame Reception Timeout Not Gain/Loss Callback Function Pointer Type Definition */
typedef void (* pIL_RX_FRAME_NOT_LOSS_INDICATION)( void );

/* transmit message at once after can init finished */
#define IL_TX_IMMEDIATELY	CAN_DISABLE

/*Add Tx for A90*/

#define IL_TX_NUM_MESSAGES          (1)

/*Add Rx for A90*/
#define IL_RX_NUM_MESSAGES          (6)

#define IL_TX_NUM_PERIODIC          (1)

#define IL_TX_NUM_IDS               (4)


enum
{
    IL_RX_MESSAGE_VCU_123,
	IL_RX_MESSAGE_ABM_234,
    IL_RX_MESSAGE_IHU_3B7,
	IL_RX_MESSAGE_BCM_458,
	IL_RX_MESSAGE_DIAG_PHY_741,
	IL_RX_MESSAGE_DIAG_FUN_7DF
};

typedef struct {
	/* Byte0 */
	UINT8                      Pos: 8;
	/* Byte1 */
	UINT8                      angle_bacck: 5;//bit0~bit4
	UINT8                      conference_mode: 2;//bit5~bit6
	UINT8                      reserved_bit_0: 1;
	/* Byte2 */
	UINT8                      autopilot_mode: 2;
	UINT8                      reserved_bit_1: 6;
	/* Byte3 */            
	UINT8                      reserved_byte0: 8;
	/* Byte4 */       
	UINT8                      reserved_byte1: 8;
	/* Byte5 */   
	UINT8                      reserved_byte2: 8;
	/* Byte6 */
	UINT8                      reserved_byte3: 4;
	/* Byte7 */
	UINT8                      reserved_byte4: 8;
}_SCM_L_45A_msgType;
typedef union {
	UINT8 byte[8];
	_SCM_L_45A_msgType	SCM_L_45A_msg;
}_SCM_L_45A_msg_buf;

/*ID 123 rx*/
typedef struct {
	UINT8                       welcome_state1: 2;               /*byte0*/
	UINT8                       unused0: 6;               /*byte1*/
	
	UINT8                       unused1: 8;               /*byte1*/
	UINT8                       unused2: 8;               /*byte2*/
	UINT8                       unused2_2: 8;        /*byte3*/
	UINT8                       unused3: 3;
	UINT8                       unused3_2: 5;         /*byte4*/
	UINT8                       unused4: 4;               /*byte5*/
	UINT8                       unused4_2: 1;
	UINT8                       unused5: 3;            
	UINT8                       unused6: 8;               /*byte6*/
	UINT8                       unused7: 8;               /*byte7*/
}_VCU_DDSS_msgType;

/*ID 234 rx*/
typedef struct {
	UINT8                       unused0: 8;               /*byte0*/
	UINT8                       unused1: 8;
	
	UINT8                       unused1_2: 3;
	UINT8                       Scene_mode: 3;
	UINT8                       unused1_3: 2;
	
	UINT8                       fatigue: 3;               /*byte3*/
	UINT8                       unused3_2: 5;
	
	UINT8                       pos: 8;               /*byte4*/
	
	UINT8                       angle: 5;               /*byte5*/
	UINT8                       unused5_1: 3;               /*byte5*/
	
	UINT8                       unused6: 8;               /*byte6*/
	UINT8                       unused7: 8;               /*byte7*/
}_ABM_VIST_msgType;

/*ID 3B7 rx*/
typedef struct {
	UINT8                       unused0: 8;               /*byte0*/
	UINT8                       unused1: 8;               /*byte1*/
	UINT8                       unused2: 8;               /*byte2*/
	UINT8                       unused3: 8;               /*byte3*/
	UINT8                       unused4: 4;               /*byte4*/
	UINT8                       IHU_SeatFoldReq: 1;
	UINT8                       IHU_SeatReleaseReq: 1;
	UINT8                       unused5: 2;

	UINT8                       unused6: 8;               /*byte5*/
	UINT8                       unused7: 8;               /*byte6*/
	UINT8                       unused8: 8;               /*byte7*/
}_IHU_3B7_msgType;


/*ID 458 rx*/
typedef struct {
	UINT8                       BCM_PsngrSeatMotoPositionReq_ForwardBack: 8;   /*byte0*/

	UINT8                       unused0: 8;                          /*byte1*/

	UINT8                       BCM_PsngrSeatMotoPositionReq_SeatBack: 8;  /*byte2*/
              
	UINT8                       unused1: 8;                          /*byte3*/

	UINT8                       unused2: 8;                          /*byte4*/
	UINT8                       unused3: 8;                          /*byte5*/
	UINT8                       unused4: 8;                          /*byte6*/
	UINT8                       unused5: 8;                          /*byte7*/

}_BCM_458_msgType;


typedef union {
	UINT8 byte[8];
	_VCU_DDSS_msgType    VCU_DDSS_msg;
}_VCU_DDSS_msg_buf;

typedef union {
	UINT8 byte[8];
	_ABM_VIST_msgType     ABM_VIST_msg;
}_ABM_VIST_msg_buf;

typedef union {
	UINT8 byte[8];
	_IHU_3B7_msgType    IHU_3B7_msg;
}_IHU_3B7_msg_buf;

typedef union {
	UINT8 byte[8];
	_BCM_458_msgType     BCM_458_msg;
}_BCM_458_msg_buf;

/* RX frame timeout: frame periodic time*5 */

extern _VCU_DDSS_msg_buf     il_rx_VCU_DDSS_msg;
extern _ABM_VIST_msg_buf     il_rx_ABM_VIST_msg;
extern _SCM_L_SCM_msg_buf		il_rx_Diver_Conference_msg;
extern _BCM_458_msg_buf		il_rx_BCM_458_msg;


#define IlSetEvent(c) IlUtilSetStatusBits(&il_tx_status[c], IL_TX_STATUS_EVENT_TX_PENDING )

extern const UINT16 il_tx_periodic_count[IL_TX_NUM_MESSAGES];

extern const UINT8 il_tx_attr[IL_TX_NUM_MESSAGES] ;

extern const UINT8* il_init_tx_per_data[IL_TX_NUM_MESSAGES];

extern const UINT8* il_tx_data_ptr[IL_TX_NUM_MESSAGES];

extern UINT8 il_tx_status[IL_TX_NUM_MESSAGES];

extern UINT16 il_tx_periodc_counter[IL_TX_NUM_MESSAGES];

extern CAN_TMD const il_tx_tmd[IL_TX_NUM_MESSAGES];




/* Bit Definition for Sub States */
#define IL_IGN_OFF   0x01
#define IL_IGN_ACC   0x02
#define IL_IGN_RUN   0x04
#define IL_IGN_CRANK 0x08


extern UINT8* il_rx_data_ptr[IL_RX_NUM_MESSAGES];

extern const UINT8 il_rx_attr[IL_RX_NUM_MESSAGES];

extern const UINT16 il_rx_periodic_count[IL_RX_NUM_MESSAGES];

extern UINT8 il_rx_status[IL_RX_NUM_MESSAGES];

extern UINT16 il_rx_periodic_counter[IL_RX_NUM_MESSAGES];

extern const UINT8 il_rx_DLC[];

extern const pIL_RX_FRAME_INDICATION il_rx_indication_fn[];
extern const pIL_RX_FRAME_LOSS_INDICATION il_rx_timeout_fn[];

extern const pIL_TX_FRAME_INDICATION il_tx_Notify_ptr[IL_TX_NUM_MESSAGES];


#endif
