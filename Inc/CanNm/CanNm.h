#ifndef __CanNm_H__
#define __CanNm_H__
/***************************************************************
copyright from private LiuXiao
if you have any question,you can contact me by email 461445092@qq.com
2022-04-20 21:46:31.770078
***************************************************************/



#define CanNmMax_CH   1


/***************************datatype define begin************************************/
#define STATE_UINT8 unsigned char
#define STATE_SINT8 char
#define STATE_UINT16 unsigned short
#define STATE_SINT16 short
#define STATE_UINT32 unsigned int
#define STATE_SINT32 int
/***************************datatype define end************************************/




/***************************enum define begin************************************/
typedef enum{
     BusSleep_STATE_E_0,
     RepeatMsg_STATE_E_1,
     NormalOperate_STATE_E_2,
     ReadySleep_STATE_E_3,
     PrepareBusSleep_STATE_E_4,
}CanNmMain_State_e;
/***************************enum define end************************************/


/*****************************main control function begin**********************************/
typedef  CanNmMain_State_e  ( * CanNmState_Handle_Ptr)(STATE_UINT8 channel);
/*********************************main control function end*******************************/


/***************************extern function begin************************************/
extern  void  CanNm_Init(void);
extern  void  CanNm_MainTask(void);
/***************************extern function end************************************/


#endif
