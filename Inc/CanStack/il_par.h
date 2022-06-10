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
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)

/* APP mssage ID 45B tx */
typedef struct {
	/* Byte0 */
	UINT8                      SCM_Reserved0: 6;
	UINT8                      SCM_HMI_Req: 1;
	UINT8                      SCM_Reserved1: 1;
	/* Byte1 */
	UINT8                      SCM_PsngrSeatMotoDirection_ForwardBack: 2;
	UINT8                      SCM_Reserved2: 2;
	UINT8                      SCM_PsngrSeatMotoDirection_SeatBack: 2;
	UINT8                      SCM_Reserved3: 2;
	/* Byte2 */
	UINT8                      SCM_Reserved4: 8;
	/* Byte3 */            
	UINT8                      SCM_PsngrSeatMotoPosition_ForwardBack: 8;
	/* Byte4 */       
	UINT8                      SCM_Reserved5: 8;
	/* Byte5 */   
	UINT8                      SCM_PsngrSeatMotoPosition_SeatBack: 8;
	/* Byte6 */
	UINT8                      SCM_MsgAliveCounter: 4;
	UINT8                      SCM_Reserved6: 4;
	/* Byte7 */
	UINT8                      SCM_Reserved7: 8;
}_SCM_L_45B_msgType;

typedef union {
	UINT8 byte[8];
	_SCM_L_45B_msgType	SCM_L_45B_msg;
}_SCM_L_45B_msg_buf;

extern _SCM_L_45B_msg_buf			        il_tx_SCM_L_45B_msg;

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


/*ID 123 rx*/
typedef struct {
	UINT8                       unused0: 8;               /*byte0*/
	UINT8                       unused1: 8;               /*byte1*/
	UINT8                       unused2: 8;               /*byte2*/
	UINT8                       VCU_VehSpdHigh: 8;        /*byte3*/
	UINT8                       unused3: 3;
	UINT8                       VCU_VehSpdLow: 5;         /*byte4*/
	UINT8                       unused4: 4;               /*byte5*/
	UINT8                       VCU_VehSpd_VD: 1;
	UINT8                       unused5: 3;            
	UINT8                       unused6: 8;               /*byte6*/
	UINT8                       unused7: 8;               /*byte7*/
}_VCU_123_msgType;

/*ID 234 rx*/
typedef struct {
	UINT8                       unused0: 8;               /*byte0*/
	UINT8                       unused1: 2;
	UINT8                       ABM_PassengerSeatOccSts: 2;
	UINT8                       unused2: 4;               /*byte1*/
	UINT8                       unused30: 8;               /*byte2*/
	UINT8                       unused31: 8;               /*byte3*/

	UINT8                       unused4: 8;               /*byte4*/
	UINT8                       unused5: 8;               /*byte5*/
	UINT8                       unused6: 8;               /*byte6*/
	UINT8                       unused7: 8;               /*byte7*/
}_ABM_234_msgType;

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
	_VCU_123_msgType    VCU_123_msg;
}_VCU_123_msg_buf;

typedef union {
	UINT8 byte[8];
	_ABM_234_msgType     ABM_234_msg;
}_ABM_234_msg_buf;

typedef union {
	UINT8 byte[8];
	_IHU_3B7_msgType    IHU_3B7_msg;
}_IHU_3B7_msg_buf;

typedef union {
	UINT8 byte[8];
	_BCM_458_msgType     BCM_458_msg;
}_BCM_458_msg_buf;

/* RX frame timeout: frame periodic time*5 */

extern _VCU_123_msg_buf     il_rx_VCU_123_msg;
extern _ABM_234_msg_buf     il_rx_ABM_234_msg;
extern _IHU_3B7_msg_buf		il_rx_IHU_3B7_msg;
extern _BCM_458_msg_buf		il_rx_BCM_458_msg;

#else
/* APP mssage ID 45A tx */
typedef struct {
	/* Byte0 */
	UINT8                      SCM_Reserved0: 6;
	UINT8                      SCM_DriverSeatLocalCtrlSwithSts: 2;
	/* Byte1 */
	UINT8                      SCM_DriverSeatMotoDirection_ForwardBack: 2;
	UINT8                      SCM_Reserved1: 2;
	UINT8                      SCM_DriverSeatMotoDirection_SeatBack: 2;
	UINT8                      SCM_Reserved2: 2;
	/* Byte2 */
	UINT8                      SCM_Reserved3: 8;
	/* Byte3 */            
	UINT8                      SCM_DriverSeatMotoPosition_ForwardBack: 8;
	/* Byte4 */       
	UINT8                      SCM_Reserved4: 8;
	/* Byte5 */   
	UINT8                      SCM_DriverSeatMotoPosition_SeatBack: 8;
	/* Byte6 */
	UINT8                      SCM_MsgAliveCounter: 4;
	UINT8                      SCM_Reserved5: 4;
	/* Byte7 */
	UINT8                      SCM_Reserved6: 8;
}_SCM_L_45A_msgType;

/* NM message ID 663 tx */
typedef struct {
	/* Byte0 */
	UINT8                      NM_SCM_SourceNode_ID: 8;
	/* Byte1 */
	UINT8                      NM_SCM_RepeatMsgReq: 1;
	UINT8                      NM_SCM_Reserved0: 3;
	UINT8                      NM_SCM_ActiveWakeupBit: 1;
	UINT8                      NM_SCM_Reserved1: 3;
	/* Byte2 */
	UINT8                      NM_SCM_UserData_2: 8;
	/* Byte3 */            
	UINT8                      NM_SCM_UserData_1_Ignition_Wakeup: 1;
	UINT8                      NM_SCM_UserData_1_Reset_Wakeup: 1;
	UINT8                      NM_SCM_UserData_1_Network_Wakeup: 1;
	UINT8                      NM_SCM_UserData_1_ECU_Spec_Wakeup: 1;
	UINT8                      NM_SCM_UserData_1_Network_Awake: 1;
	UINT8                      NM_SCM_UserData_1_Ignition_Awake: 1;
	UINT8                      NM_SCM_UserData_1_Diagnostic_Awake: 1;
	UINT8                      NM_SCM_UserData_1_Spec_Awake: 1;
	/* Byte4 */       
	UINT8                      NM_SCM_UserData_2_NM_State: 3;
	UINT8                      NM_SCM_UserData_2_GatewayRequest: 1;
	UINT8                      NM_SCM_UserData_2_Reserved: 4;
	/* Byte5 */   
	UINT8                      NM_SCM_UserData_3_Reserved: 8;
	/* Byte6 */
	UINT8                      NM_SCM_UserData_4_Reserved: 8;
	/* Byte7 */
	UINT8                      NM_SCM_UserData_5_Reserved: 8;
}_SCM_L_NM_663_msgType;

typedef union {
	UINT8 byte[8];
	_SCM_L_45A_msgType	SCM_L_45A_msg;
}_SCM_L_45A_msg_buf;

typedef union {
	UINT8 byte[8];
	_SCM_L_NM_663_msgType	SCM_L_NM_663_msg;
}_SCM_L_NM_663_msg_buf;

extern _SCM_L_45A_msg_buf			        il_tx_SCM_L_45A_msg;
extern _SCM_L_NM_663_msg_buf			    il_tx_SCM_L_NM_663_msg;


/* Frame Reception Timeout Not Gain/Loss Callback Function Pointer Type Definition */
typedef void (* pIL_RX_FRAME_NOT_LOSS_INDICATION)( void );

/* transmit message at once after can init finished */
#define IL_TX_IMMEDIATELY	CAN_DISABLE

/*Add Tx for A90*/

#define IL_TX_NUM_MESSAGES          (2)

/*Add Rx for A90*/
#define IL_RX_NUM_MESSAGES          (4)

#define IL_TX_NUM_PERIODIC          (2)

#define IL_TX_NUM_IDS               (4)


enum
{
    IL_RX_MESSAGE_BCM_245,
    IL_RX_MESSAGE_IHU_457,
	IL_RX_MESSAGE_DIAG_PHY_7A1,
	IL_RX_MESSAGE_DIAG_FUN_7DF
};


/*ID 245 rx*/
typedef struct {
	UINT8                       unused0: 8;               /*byte0*/
	UINT8                       unused1: 8;               /*byte1*/
	UINT8                       unused2: 8;               /*byte2*/
	UINT8                       BCM_DoorAjarSts_FL: 1;    /*byte3*/
	UINT8                       unused3: 7;

	UINT8                       unused4: 8;               /*byte4*/
	UINT8                       unused5: 8;               /*byte5*/
	UINT8                       unused6: 8;               /*byte6*/
	UINT8                       unused7: 8;               /*byte7*/
}_BCM_245_msgType;

/*ID 457 rx*/
typedef struct {
	UINT8                       unused0: 2;
	UINT8                       BCM_FaceRecognitionEnableSts: 2;               
	UINT8                       BCM_EntryExitSeatCtrlEnableSts: 2;            
	UINT8                       unused1: 2;                          /*byte0*/

	UINT8                       unused2: 8;                          /*byte1*/

	UINT8                       BCM_MotoPositionReq_ForwardBack: 8;  /*byte2*/
              
	UINT8                       unused3: 8;                          /*byte3*/

	UINT8                       BCM_MotoPositionReq_SeatBack: 8;     /*byte4*/
	UINT8                       unused4: 8;                          /*byte5*/
	UINT8                       unused5: 8;                          /*byte6*/
	UINT8                       unused6: 8;                          /*byte7*/

}_BCM_457_msgType;

typedef union {
	UINT8 byte[8];
	_BCM_245_msgType    BCM_245_msg;
}_BCM_245_msg_buf;

typedef union {
	UINT8 byte[8];
	_BCM_457_msgType     BCM_457_msg;
}_BCM_457_msg_buf;

/* RX frame timeout: frame periodic time*5 */

extern _BCM_245_msg_buf		il_rx_BCM_245_msg;
extern _BCM_457_msg_buf		il_rx_BCM_457_msg;
#endif

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
