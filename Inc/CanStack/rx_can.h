#ifndef TCAN_H
#define TCAN_H

#ifndef TCAN_C
#define EXTERN  extern
#else
#define EXTERN
#endif
//#include "ac780x.h"
#include "il.h"
#include "il_par.h"

typedef enum{

	BT_Pos_e,
	BT_Angle_e,
	BT_Scene_Mode_e,
	BT_Fatigue_e,
	BT_Welcome_e,
	BT_SIGNAL_GROUP_NUM
}BT_Signal_type_e;


#define BT_BACK(a)  (a+BT_SIGNAL_GROUP_NUM)

/* Defines */
#define   NFC_CMD_ECU   (il_rx_NFC_CMD_REQ_1888A5FF_msg.NFC_CMD_REQ_1888A5FF_msg.NFC_Cmd_ECU)
#define   NFC_CMD_FUNC  (il_rx_NFC_CMD_REQ_1888A5FF_msg.NFC_CMD_REQ_1888A5FF_msg.NFC_Cmd_Func)
#define   NFC_CMD_PARA  (il_rx_NFC_CMD_REQ_1888A5FF_msg.NFC_CMD_REQ_1888A5FF_msg.NFC_Cmd_Func_Param)

#define   NFC_RES_ECU   	(il_tx_WPC_1888FFA5_msg.WPC_1888FFA5_msg.NFC_Res_ECU)
#define   NFC_RES_FUNC  	(il_tx_WPC_1888FFA5_msg.WPC_1888FFA5_msg.NFC_Res_Func)
#define   NFC_RES_ERR_CODE  (il_tx_WPC_1888FFA5_msg.WPC_1888FFA5_msg.NFC_Res_ErrCode)
#define   NFC_RES_SEED		(il_tx_WPC_1888FFA5_msg.WPC_1888FFA5_msg.NFC_Res_Seed)

#define CTRL_FUNC_SEED_REQ				(0x00u)
#define CTRL_FUNC_NFC_SW				(0x02u)
#define CTRL_FUNC_WPC_SW				(0x03u)
#define CTRL_FUNC_BT_QUICK_CONN_SW		(0x04u)
#define CTRL_FUNC_ADD_CARD				(0x05u)
#define CTRL_FUNC_DEL_CARD				(0x06u)
#define CTRL_FUNC_LOCK					(0x20u)
#define CTRL_FUNC_UNLOCK				(0x21u)
#define CTRL_FUNC_SLEEP					(0x22u)
#define CTRL_FUNC_WAKEUP				(0x23u)

#define MATCH           (1u)
#define NO_MATCH        (0u)

typedef enum
{
	NO_ERROR = 0,
	NFC_INLOCK,
	CARD_FORMAT_ERROR,
	CARD_DUPLICATED = 3,
	CARD_NOT_EXIST = 3,
	FLASH_WRITE_FAIL,
	CARD_LIST_FULL,
	NFC_NOT_LOCK,
}error_code_type;

/*****************************************************************************
*                                 Global Variables			                 *
******************************************************************************/
extern UINT8 nfc_sw_sts;
extern UINT8 wpc_sw_sts;
extern UINT8 bt_sw_sts;
extern UINT8 pre_nfc_sw_sts;
extern UINT8 wpc_sw_sts;
extern UINT8 bt_sw_sts;
extern UINT8 nfc_lock_state;
extern UINT8 nfc_wakeup_state;
extern UINT8 nfc_sleep_req;
extern UINT8 white_list_req;
extern UINT16 motor_speed;
extern UINT32 odo_meter;
/*****************************************************************************
*                                 Global Function Prototypes                 *
******************************************************************************/
extern void tcan_Task(void);
extern void Message_variable_init(void);
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
extern void Rx_VCU_DDSS_func(void);
extern void Rx_ABM_VIST_func(void);
extern void Rx_IHU_3B7_func(void);
extern void Rx_BCM_458_func(void);
extern UINT32 VCU_VehSpd;
extern UINT8 VCU_VehSpd_VD;
extern UINT8 ABM_PassengerSeatOccSts;
extern UINT8 IHU_SeatFoldReq;
extern UINT8 IHU_SeatReleaseReq;
#else
extern void Rx_BCM_245_func(void);
extern void Rx_BCM_457_func(void);
#endif


#if 0
extern void Rx_NFC_MSG_REQ_1800A5FF_func(void);
extern void Rx_NFC_CMD_REQ_1888A5FF_func(void);
extern void Rx_VCU_CMD_RES_1888FFA0_func(void);
extern void Rx_VCU_MOTOR_SPEED_1401FFA0_func(void);
extern void Rx_VCU_ODO_METER_1804FFA0_func(void);
extern UINT8 compare_U8_arrays (UINT8 *array1, UINT8 *array2, UINT8 length);
extern void send_card_white_list(void);
#endif

#undef EXTERN
#endif





