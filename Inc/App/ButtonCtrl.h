#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "public.h"
#include "user_init.h"



typedef enum
{
	BTN_ID_CTRL_MODE_ZERO_GRAVITY_e,
	BTN_ID_CTRL_MODE_DRIVERIESS_CAR_e,
	BTN_ID_CTRL_MODE_MEETING_e,
	BTN_ID_CTRL_MODE_LEISURE_e,
	BTN_ID_CTRL_OFF_e,
	BTN_ID_CTRL_MOVIE_e,
	BTN_ID_CTRL_SLEEP_e,
	BTN_ID_CTRL_PREPARE_MEAL_e,
	
	BTN_ID_CTRL_POS_FRONT_REAR_e,
	BTN_ID_CTRL_BACK_ANGLE_e,
	BTN_ID_CTRL_ROTATE_e,
	BTN_ID_CTRL_HEAD_e,
	
	BTN_ID_CTRL_VENTILITION_e,	
	BTN_ID_CTRL_LED_e,
	BTN_ID_CTRL_MASSAGE_e,
}ButtonCtrl_Id_e;



#define BTNVAL_OFF   ((uint8_t)0)
#define BTNVAL_ON    ((uint8_t)1)
#define BTNVAL_LEVEL_1   ((uint8_t)1)
#define BTNVAL_LEVEL_2   ((uint8_t)2)
#define BTNVAL_LEVEL_3   ((uint8_t)3)

#define MAXP  100
#define MINP  0

#define DIRECTION_FRONT  ((uint8_t)0xAA)
#define DIRECTION_REAR   ((uint8_t)0xA5)
#define DIRECTION_STOP   ((uint8_t)0xAF)

#define STOP_BTN_TIMEOUT 30

typedef struct
{
	uint8_t ButtonId;
    uint8_t ButtonVal; 
} ButtonCtrl_Str;

typedef struct
{
	ButtonCtrl_Str queue[10];
	uint8_t queue_in;
	uint8_t queue_out;
}ButtonCtrl_Queue_str;


typedef struct
{
	uint8_t ReqActive;
    uint8_t ButtonVal; 
} ButtonCtrlReq_Str;




typedef struct
{
	ButtonCtrlReq_Str Front_Rear_Motor;
	ButtonCtrlReq_Str Back_Angle_Motor;
	ButtonCtrlReq_Str Rotate_Motor;
	ButtonCtrlReq_Str Head_Motor;
	ButtonCtrlReq_Str Leg_Motor;
	ButtonCtrlReq_Str Total_Angle_Motor;
	//ButtonCtrlReq_Str Massage;
	//ButtonCtrlReq_Str LED;
}ButtonCtrl_Mode_Req_Str;

typedef struct
{
	ButtonCtrl_Mode_Req_Str  ButtonCtrl_Mode;
	ButtonCtrlReq_Str LED;
	ButtonCtrlReq_Str Ventilation;
	ButtonCtrlReq_Str Massage;
}ButtonCtrl_Req_Str;


extern ButtonCtrl_Queue_str ButtonCtrl_tx_queue;


extern void ButtonCtrlInit(void);
/* Fast sample function(50ms) */
extern void ButtonCtrl_50ms_Task(void);

extern uint8_t ButtonCtrl_queue_push_e(ButtonCtrl_Str fl_str_e);

extern 	ButtonCtrl_Id_e ButtonCtrl_Get_CtrlMode(void);
#endif
