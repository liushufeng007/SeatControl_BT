#ifndef __SCM_SEATCONTROL_H__
#define __SCM_SEATCONTROL_H__
#include "public.h"
#include "user_init.h"


extern UINT8 Get_ForwardBack_CurrentPos(void);   /* Get Current Forward back position */
extern UINT8 Get_SeatBack_CurrentPos(void);      /* Get Current Seat back position */
extern UINT8 Get_SeatSwtichSts(void);     /* Get Seat control switch status */
extern void Scm_SeatControl_Init(void);
extern UINT8 Get_SeatMotor_SoftPoint_Min(void);  /* Motor software Point */
extern UINT8 Get_SeatMotor_SoftPoint_Max(void);
extern UINT8 Get_SeatBackMotor_SoftPoint_Min(void);  /* Motor software Point */
extern UINT8 Get_SeatBackMotor_SoftPoint_Max(void);

extern UINT8 Get_Current_ForwardBackMotor_Dir;
extern UINT8 Get_Current_SeatBackMotor_Dir;
extern UINT8 Dis_SCM_HMI_Req;

/* Seat control function(10ms) */
extern void Scm_SeatControl_App(void);

#endif
