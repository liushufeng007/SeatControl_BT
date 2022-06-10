#ifndef __CanNmCtrl_Cfg_H__
#define __CanNmCtrl_Cfg_H__
/***************************************************************
copyright from private LiuXiao
if you have any question,you can contact me by email 461445092@qq.com
2022-04-20 21:46:31.770078
***************************************************************/

#include "CanNmCtrl.h"

extern void IlTxResume( void );
extern void IlTxSuspend( void );

#define  CANNMCTRL_CLOSE_APP_MSG()  IlTxSuspend()
#define  CANNMCTRL_OPEN_APP_MSG()   IlTxResume()

#define CAN_NM_TASK_PERIOD        (5)  //unit:ms



#define CANNMCTRL_TX_CANNM_ID_L   ((STATE_CTRL_UINT32)0x663)
#define CANNMCTRL_TX_CANNM_ID_R   ((STATE_CTRL_UINT32)0x664)
#define CANNMCTRL_TX_CANNM_DLC    ((STATE_CTRL_UINT8)0x8)
#define CANNMCTRL_TX_CANNM_SRC_ID_L    ((STATE_CTRL_UINT8)0x63)
#define CANNMCTRL_TX_CANNM_SRC_ID_R    ((STATE_CTRL_UINT8)0x64)

#define CANNM_NM_TIMEOUT_TIMER        ((2500/CAN_NM_TASK_PERIOD)-1)
#define CANNM_WAIT_BUS_SLEEP_TIMER    ((1500/CAN_NM_TASK_PERIOD)-1)
#define CANNM_IMMDIATE_CYCLE_TIMER    ((20/CAN_NM_TASK_PERIOD)-1)
#define CANNM_NM_CYCLE_TIMER          ((1000/CAN_NM_TASK_PERIOD)-1)
#define CANNM_IMMDIATE_TIMES          (10)
#define CANNM_REPEAT_MSG_TIMER        ((3000/CAN_NM_TASK_PERIOD)-1)
#define CANNM_APP_START_TX_TIMER      ((30/CAN_NM_TASK_PERIOD)-1)


#endif
