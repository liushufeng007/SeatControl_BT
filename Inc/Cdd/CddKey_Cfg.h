/*******************************************************************************
|  File Name:  SwM_Cfg.h
|  Description:  Implementation of the Switch Management
|-------------------------------------------------------------------------------
|
|******************************************************************************/

#if !defined (SWM_CFG_H)
# define SWM_CFG_H

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"
#include "Ioif.h"
/*******************************************************************************
|    Compile Option or configuration Section (for test/debug)
|******************************************************************************/
#ifndef STATIC
#define STATIC static
#endif
/*******************************************************************************
|    Macro Definition
|******************************************************************************/


#define	CeSWM_u_NUM_CH						32U
#define CeSWM_KEY_TASKTIME     				5U   								/*key deal task time cycle*/


#define	CeSWM_u_TOUCH_LOW_CH				0U									/*TOUCH LOW CHANNEL*/
#define	CeSWM_u_TOUCH_HIGH_CH				1U									/*TOUCH HIGH CHANNEL*/
#define	CeSWM_u_FTP_LOW_CH					2U									/*FTP LOW CHANNEL*/
#define	CeSWM_u_FTP_HIGH_CH					3U									/*FTP HIGH CHANNEL*/
#define CeSWM_u_JY_HW_CH					4U									/*JY hardware channel*/


/*
JY Hardware If bitx Mapping
*/
#define CeSWM_u_Seatback_B_SW_IO_KEY		0U	
#define	CeSWM_u_Seatback_F_SW_IO_KEY		1U									

#define CeSWM_u_Seat_B_SW_IO_KEY		    2U	
#define	CeSWM_u_Seat_F_SW_IO_KEY		    3U	

#define CeSWM_u_Seat3_B_SW_IO_KEY		    4U	
#define	CeSWM_u_Seat3_F_SW_IO_KEY		    5U	

#define CeSWM_u_Seat4_B_SW_IO_KEY		    6U	
#define	CeSWM_u_Seat4_F_SW_IO_KEY		    7U	

#define CeSWM_u_Seat_FastB_SW_IO_KEY		8U	
#define	CeSWM_u_Seat_FastF_SW_IO_KEY		9U	

#define	CeSWM_u_Test_SW_IO_KEY		        10U	

#define CeSWM_u_Key_NUM                     11U



	
/*********************************************************************************
							Key Real Value 
*********************************************************************************/
#if 0
#define ServSWM_u_GetKey_Seatback_B_SW_RealVaule()		(Ioif_GetPinLevel(GPIO_NUMBER_12_SeatBack_B_SW_IO)==0)
#define ServSWM_u_GetKey_Seatback_F_SW_RealVaule()		(Ioif_GetPinLevel(GPIO_NUMBER_57_Seatback_F_SW_IO)==0)
#define ServSWM_u_GetKey_Seat_B_SW_RealVaule()		    (Ioif_GetPinLevel(GPIO_NUMBER_23_Seat_B_SW_IO)==0)
#define ServSWM_u_GetKey_Seat_F_SW_RealVaule()		    (Ioif_GetPinLevel(GPIO_NUMBER_27_Seat_F_SW_IO)==0)
#define ServSWM_u_GetKey_Seat3_B_SW_RealVaule()		    (Ioif_GetPinLevel(GPIO_NUMBER_42_Seat3_B_SW_IO)==0)
#define ServSWM_u_GetKey_Seat3_F_SW_RealVaule()		    (Ioif_GetPinLevel(GPIO_NUMBER_32_Seat3_F_SW_IO)==0)
#define ServSWM_u_GetKey_Seat4_B_SW_RealVaule()		    (Ioif_GetPinLevel(GPIO_NUMBER_38_Seat4_B_SW_IO)==0)
#define ServSWM_u_GetKey_Seat4_F_SW_RealVaule()		    (Ioif_GetPinLevel(GPIO_NUMBER_61_Seat4_F_SW_IO)==0)
#define ServSWM_u_GetKey_Seat_FastB_SW_RealVaule()		(Ioif_GetPinLevel(GPIO_NUMBER_69_Seat_FastB_SW_IO)==0)
#define ServSWM_u_GetKey_Seat_FastF_SW_RealVaule()		(Ioif_GetPinLevel(GPIO_NUMBER_16_Seat_FastF_SW_IO)==0)
#define ServSWM_u_GetKey_Test_SW_RealVaule()		    (Ioif_GetPinLevel(GPIO_NUMBER_57_Seatback_F_SW_IO)==0)

#else
#define ServSWM_u_GetKey_Seatback_B_SW_RealVaule()		0
#define ServSWM_u_GetKey_Seatback_F_SW_RealVaule()		0
#define ServSWM_u_GetKey_Seat_B_SW_RealVaule()		   0
#define ServSWM_u_GetKey_Seat_FastB_SW_RealVaule()		0
#define ServSWM_u_GetKey_Seat3_B_SW_RealVaule()		   0
#define ServSWM_u_GetKey_Seat3_F_SW_RealVaule()		    0
#define ServSWM_u_GetKey_Seat4_B_SW_RealVaule()		    0
#define ServSWM_u_GetKey_Seat4_F_SW_RealVaule()		   0
#define ServSWM_u_GetKey_Seat_FastF_SW_RealVaule()		0
#define ServSWM_u_GetKey_Seat_F_SW_RealVaule()		    0
#define ServSWM_u_GetKey_Test_SW_RealVaule()		    0
#define ServSWM_u_GetKey_IGN_RealVaule()                0
#endif


/*******************************************************************************
|    Enum Definition
|******************************************************************************/

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef struct TsSWM_h_KeyStruct
{
    uint8_t e_u_SigTimeCnt;
    uint8_t e_u_SigOldStatus;
    uint8_t e_u_SigNowStatus;
    uint16_t e_w_SigDeadCnt;
    uint16_t e_w_SigRecoverCnt;
} TsSWM_h_KeyStruct;

typedef struct TsSWM_h_KeyStatusStruct
{
    uint32_t e_dw_Status;
    uint32_t e_dw_NewStatus;
    uint32_t e_dw_OldStatus;
    uint32_t e_dw_KeyRealValue;
    uint32_t e_dw_ContinuousWord;
    uint32_t e_dw_EventWord;
    uint32_t e_dw_EventOldWord;
    uint8_t  e_u_KeyLngDownFlag;
    uint32_t e_dw_LongDownTimeCnt;
    uint32_t e_dw_LngDnHisTimeCnt;
} TsSWM_h_KeyStatusStruct;



typedef struct TsSWM_h_SwmConfigStruct
{
    uint8_t e_u_SwmGroup;
    uint8_t e_u_SwmChEnableTab[CeSWM_u_NUM_CH];
    TsSWM_h_KeyStruct *e_p_SwmStructArray[CeSWM_u_NUM_CH];
    uint8_t e_u_ShortFiltTime;
    uint8_t e_u_OverFiltTime;
    uint16_t e_u_OverFiltRecTime;

} TsSWM_h_SwmConfigStruct;



#endif

/*EOF*/
