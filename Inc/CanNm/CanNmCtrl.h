#ifndef __CanNmCtrl_H__
#define __CanNmCtrl_H__
/***************************************************************
copyright from private LiuXiao
if you have any question,you can contact me by email 461445092@qq.com
2022-04-20 21:46:31.770078
***************************************************************/

#include "Canif.h"
#include "CanNm.h"

/***************************datatype define begin************************************/
#define STATE_CTRL_UINT8 unsigned char
#define STATE_CTRL_SINT8 char
#define STATE_CTRL_UINT16 unsigned short
#define STATE_CTRL_SINT16 short
#define STATE_CTRL_UINT32 unsigned int
#define STATE_CTRL_SINT32 int
/***************************datatype define end************************************/

typedef enum
{
	APP_CLOSE = 0xAA,
	APP_OPEN = 0x0,
}CanNmCtrl_AppMsg_State_e;


typedef enum
{

	NM_OPEN = 0x0,
	NM_CLOSE = 0xAA,
}CanNmCtrl_NmMsg_State_e;


typedef enum
{
	NMSTATE_RESERVED = 0,
	REPEAT_MSG_FROM_BUS_SLP ,
	REPEAT_MSG_FROM_PREPARE_SLP,
	REPEAT_MSG_FROM_NORMAL,
	REPEAT_MSG_FROM_READY_SLP,
	NORMAL_FROM_REPEAT_MSG,
	NORMAL_FROM_READY_SLE,
}CanNmCtrl_NM_State_e;


typedef enum
{
	WKUP_INACTIVE = 0,
	WKUP_ACTIVE ,
}CanNmCtrl_NM_WakeUp_e;


typedef enum
{
	WKUP_PASSIVE = 0,
	WKUP_LOCAL ,
}CanNmCtrl_NM_WakeUpBit_e;


typedef enum
{
	WKUPSRC_IGN = 0,
	WKUPSRC_RESET ,
	WKUPSRC_NETWORK ,
	WKUPSRC_ECU_SPEC ,
	KEEPSRC_NETWORK ,
	KEEPSRC_IGN ,
	KEEPSRC_DIAG ,
	KEEPSRC_ECU_SPEC ,
}CanNmCtrl_NM_WakeUpSrc_Ch_e;


typedef struct
{
	/*byte0*/
	STATE_CTRL_UINT8 SourceID;
	/*byte1*/
	STATE_CTRL_UINT8 RMR:1;
	STATE_CTRL_UINT8 Reserved_0:3;
	STATE_CTRL_UINT8 Active_Wkup_Bit:1;
	STATE_CTRL_UINT8 Reserved_1:3;
	/*byte2*/
	STATE_CTRL_UINT8 UserData0;
	/*byte3*/
	STATE_CTRL_UINT8 Ign_Wkup:1;
	STATE_CTRL_UINT8 Rst_Wkup:1;
	STATE_CTRL_UINT8 NetWk_Wkup:1;
	STATE_CTRL_UINT8 ECU_Spec_Wkup:1;
	STATE_CTRL_UINT8 NetWk_Awake:1;
	STATE_CTRL_UINT8 Ign_Awake:1;
	STATE_CTRL_UINT8 Diag_Awake:1;
	STATE_CTRL_UINT8 Ecu_Spec_Awake:1;
	/*byte4*/
	STATE_CTRL_UINT8 NM_State:3;
	STATE_CTRL_UINT8 GateWayReq:1;
	STATE_CTRL_UINT8 Reserved_2:4;
	/*byte5*/
	STATE_CTRL_UINT8 UserData3;
	/*byte6*/
	STATE_CTRL_UINT8 UserData4;
	/*byte7*/
	STATE_CTRL_UINT8 UserData5;
}CanNmCtrl_Msg_Str;

typedef union
{
	CanNmCtrl_Msg_Str str;
	STATE_CTRL_UINT8 data[8];
}CanNmCtrl_Msg_Un;


typedef struct
{
	STATE_CTRL_UINT8  Request;
	STATE_CTRL_UINT8  Interval_Ticks;
	STATE_CTRL_UINT8  NmRx_Flag;
	STATE_CTRL_UINT8  NmTx_Times;
	STATE_CTRL_UINT16 Delay_ticks;
	STATE_CTRL_UINT16 NmTimerOut_ticks;
	CanNmCtrl_AppMsg_State_e AppMsgState;
	CanNmCtrl_NmMsg_State_e  NmMsgState;
	CanNmCtrl_Msg_Un Msg;
	
}CanNmCtrl_MainCtrl_Str;

/*control can network message interface*/
extern STATE_CTRL_UINT8 CanNmCtrl_Get_AppMsgIsOpen(STATE_CTRL_UINT8 can_ch);
extern void CanNmCtrl_ReqNm_State(STATE_CTRL_UINT8 can_ch,CanNmCtrl_NmMsg_State_e State);

extern void CanNmCtrl_Req_NetWork(STATE_CTRL_UINT8 can_ch);
extern void CanNmCtrl_Release_NetWork(STATE_CTRL_UINT8 can_ch);
extern void CanNmCtrl_Set_ActiveWkupSrc(STATE_CTRL_UINT8 can_ch,CanNmCtrl_NM_WakeUpSrc_Ch_e ch ,CanNmCtrl_NM_WakeUp_e WkupSrc);

/*tx and tx message interface*/
extern void CanNmCtrl_NmRx_Indication(STATE_CTRL_UINT8 can_ch,STATE_CTRL_UINT8 * CanMsgData);
extern void CanNmCtrl_NmTx_Msg(STATE_CTRL_UINT8 can_ch);

extern void CanNmCtrl_init(void);
extern CanNmMain_State_e CanNmCtrl_Get_CanNm_State(STATE_CTRL_UINT8 can_ch);


#endif
