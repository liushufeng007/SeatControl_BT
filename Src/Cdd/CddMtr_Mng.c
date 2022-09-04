
/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include  "CddMtr_Cal.h"
#include  "CddMtr_Mng.h"
#include  "CddMtr_Ctrl.h"
#include  "CddMtr_HFKF.h"
#include "Adcif.h"
#include "fm33_assert.h"
#include "CddEeprom.h"
#include "Lookup.h"

#include "PowerMgmt.h"

#include "crc.h"
/*******************************************************************************
|    Macro Definition
|******************************************************************************/



/*******************************************************************************
|    Enum Definition
|******************************************************************************/

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/


/*******************************************************************************
|    Global variables Declaration
|******************************************************************************/
static uint16_t CddMtr_E2_Buffer[6*CDDMTR_HFKF_MAX_NUM];
CddMtr_Mng_BatMode_Str CddMtr_Mng_BatMode;

/*******************************************************************************
|    Static local variables Declaration
|******************************************************************************/
CddMtr_Mng_Str CddMtr_Mng_Main_St[CDDMTR_HFKF_MAX_NUM] = 
{
	{
		{ CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL},
		CDDMTR_MNG_DIRECTION_NULL,
		{ CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL},
		CDDMTR_MNG_HALL_NORMAL ,
		CDDMTR_MNG_NO_STUCK, 
		CDDMTR_MNG_LEARNDATA_INVALID,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_DIRECTION_NULL,
		CDDMTR_MNG_LEARN_NULL,
		CDDMTR_MNG_DIRECTION_NULL,
		{0,0,0,0},
		0,
		0,
		0,
		0,
		0,
		CDDMTR_MNG_START,
		{0,0,0},
		0,
		0,
		0xFFFF
	},
	{
		{ CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL},
		CDDMTR_MNG_DIRECTION_NULL,
		{ CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL},
		CDDMTR_MNG_HALL_NORMAL ,
		CDDMTR_MNG_NO_STUCK, 
		CDDMTR_MNG_LEARNDATA_INVALID,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_DIRECTION_NULL,
		CDDMTR_MNG_LEARN_NULL,
		CDDMTR_MNG_DIRECTION_NULL,
		{0,0,0,0},
		0,
		0,
		0,
		0,
		0,
		CDDMTR_MNG_START,
		{0,0,0},
		0,
		0,
		0xFFFF
	},
	{
		{ CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL},
		CDDMTR_MNG_DIRECTION_NULL,
		{ CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL},
		CDDMTR_MNG_HALL_NORMAL ,
		CDDMTR_MNG_NO_STUCK, 
		CDDMTR_MNG_LEARNDATA_INVALID,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_DIRECTION_NULL,
		CDDMTR_MNG_LEARN_NULL,
		CDDMTR_MNG_DIRECTION_NULL,
		{0,0,0,0},
		0,
		0,
		0,
		0,
		0,
		CDDMTR_MNG_START,
		{0,0,0},
		0,
		0,
		0xFFFF
	},
	{
		{ CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL},
		CDDMTR_MNG_DIRECTION_NULL,
		{ CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL},
		CDDMTR_MNG_HALL_NORMAL ,
		CDDMTR_MNG_NO_STUCK, 
		CDDMTR_MNG_LEARNDATA_INVALID,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_DIRECTION_NULL,
		CDDMTR_MNG_LEARN_NULL,
		CDDMTR_MNG_DIRECTION_NULL,
		{0,0,0,0},
		0,
		0,
		0,
		0,
		0,
		CDDMTR_MNG_START,
		{0,0,0},
		0,
		0,
		0xFFFF
	},
	{
		{ CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL},
		CDDMTR_MNG_DIRECTION_NULL,
		{ CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL},
		CDDMTR_MNG_HALL_NORMAL ,
		CDDMTR_MNG_NO_STUCK, 
		CDDMTR_MNG_LEARNDATA_INVALID,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_DIRECTION_NULL,
		CDDMTR_MNG_LEARN_NULL,
		CDDMTR_MNG_DIRECTION_NULL,
		{0,0,0,0},
		0,
		0,
		0,
		0,
		0,
		CDDMTR_MNG_START,
		{0,0,0},
		0,
		0,
		0xFFFF
	},
	{
		{ CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL , CDDMTR_MNG_DIRECTION_NULL},
		CDDMTR_MNG_DIRECTION_NULL,
		{ CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL , CDDMTR_MNG_LEARN_NULL},
		CDDMTR_MNG_HALL_NORMAL ,
		CDDMTR_MNG_NO_STUCK, 
		CDDMTR_MNG_LEARNDATA_INVALID,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_STATUS_IDLE,
		CDDMTR_MNG_DIRECTION_NULL,
		CDDMTR_MNG_LEARN_NULL,
		CDDMTR_MNG_DIRECTION_NULL,
		{0,0,0,0},
		0,
		0,
		0,
		0,
		0,
		CDDMTR_MNG_START,
		{0,0,0},
		0,
		0,
		0xFFFF
	}

};



/*******************************************************************************
|    Static Local Functions Declaration
|******************************************************************************/
void CddMtr_Read_E2_Ckc(uint8_t fl_Mtr_Id);
void CddMtr_Update_Mtr_E2_Ckc(uint8_t fl_Mtr_Id_u8);
void CddMtr_Mng_Detect(uint8_t fl_Mtr_Id,uint16_t fl_Current_Ad,uint16_t fl_hall_Ad,uint16_t fl_swbat_ad,uint16_t fl_Compensate_ad);
void CddMtr_Mng_Start_Run(uint8_t fl_Mtr_Id,CddMtr_Mng_Run_Direction_e fl_stuck_direct);
void CddMtr_Mng_Stop_Run(uint8_t fl_Mtr_Id);
void CddMtr_Mng_Stop_Diag(uint8_t fl_Mtr_Id);
uint8_t CddMtr_Mng_Max_Is_Valid(uint32_t fl_Max_Step);
uint8_t CddMtr_ShutDown_Update_Mtr_E2_Ckc(void);
uint16_t CddMtr_LookUp_Compensate_Val(uint16_t fl_bat_ad);


CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_idle(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_idle(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_idle(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_Stop(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_Stop(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_Stop(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_LEARN_1_DELAY(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_LEARN_1_DELAY(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_LEARN_1_DELAY(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_LEARN_5_BACKWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_LEARN_5_BACKWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_LEARN_5_BACKWARD(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_LEARN_6_DELAY(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_LEARN_6_DELAY(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_LEARN_6_DELAY(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_LEARN_7_FORWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_LEARN_7_FORWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_LEARN_7_FORWARD(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_LEARN_8_FAIL(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_LEARN_8_FAIL(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_LEARN_8_FAIL(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_DIRECT_1_START(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_DIRECT_1_START(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_DIRECT_1_START(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_DIRECT_2_FORWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_DIRECT_2_FORWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_DIRECT_2_FORWARD(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_DIRECT_3_BACKWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_DIRECT_3_BACKWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_DIRECT_3_BACKWARD(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_PERCENT_1_DELAY(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_PERCENT_1_DELAY(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_PERCENT_1_DELAY(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_PERCENT_5_HALLDIAG_FORWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_PERCENT_5_HALLDIAG_FORWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_PERCENT_5_HALLDIAG_FORWARD(uint8_t fl_Mtr_Id);

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_PERCENT_10_HALLDIAG_BACKWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_PERCENT_10_HALLDIAG_BACKWARD(uint8_t fl_Mtr_Id);
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_PERCENT_10_HALLDIAG_BACKWARD(uint8_t fl_Mtr_Id);


	
void CddMtr_Mng_Handle(uint8_t fl_Mtr_Id);

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/
const State_Handle_Ptr  CddMtr_Mng_Handle_Func_Tbl[CDDMTR_MNG_STATUS_MAX][3] = 
{
	(State_Handle_Ptr)CddMtr_Mng_handle_en_idle,
	(State_Handle_Ptr)CddMtr_Mng_handle_idle,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_idle,

	(State_Handle_Ptr)CddMtr_Mng_handle_en_Stop,
	(State_Handle_Ptr)CddMtr_Mng_handle_Stop,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_Stop,

	(State_Handle_Ptr)CddMtr_Mng_handle_en_LEARN_1_DELAY,
	(State_Handle_Ptr)CddMtr_Mng_handle_LEARN_1_DELAY,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_LEARN_1_DELAY,

	(State_Handle_Ptr)CddMtr_Mng_handle_en_LEARN_5_BACKWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_LEARN_5_BACKWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_LEARN_5_BACKWARD,

	(State_Handle_Ptr)CddMtr_Mng_handle_en_LEARN_6_DELAY,
	(State_Handle_Ptr)CddMtr_Mng_handle_LEARN_6_DELAY,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_LEARN_6_DELAY,

	(State_Handle_Ptr)CddMtr_Mng_handle_en_LEARN_7_FORWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_LEARN_7_FORWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_LEARN_7_FORWARD,

	(State_Handle_Ptr)CddMtr_Mng_handle_en_LEARN_8_FAIL,
	(State_Handle_Ptr)CddMtr_Mng_handle_LEARN_8_FAIL,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_LEARN_8_FAIL,

	(State_Handle_Ptr)CddMtr_Mng_handle_en_DIRECT_1_START,
	(State_Handle_Ptr)CddMtr_Mng_handle_DIRECT_1_START,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_DIRECT_1_START,

	(State_Handle_Ptr)CddMtr_Mng_handle_en_DIRECT_2_FORWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_DIRECT_2_FORWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_DIRECT_2_FORWARD,

	(State_Handle_Ptr)CddMtr_Mng_handle_en_DIRECT_3_BACKWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_DIRECT_3_BACKWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_DIRECT_3_BACKWARD,

	(State_Handle_Ptr)CddMtr_Mng_handle_en_PERCENT_1_DELAY,
	(State_Handle_Ptr)CddMtr_Mng_handle_PERCENT_1_DELAY,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_PERCENT_1_DELAY,

	(State_Handle_Ptr)CddMtr_Mng_handle_en_PERCENT_5_HALLDIAG_FORWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_PERCENT_5_HALLDIAG_FORWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_PERCENT_5_HALLDIAG_FORWARD,
	
	(State_Handle_Ptr)CddMtr_Mng_handle_en_PERCENT_10_HALLDIAG_BACKWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_PERCENT_10_HALLDIAG_BACKWARD,
	(State_Handle_Ptr)CddMtr_Mng_handle_exit_PERCENT_10_HALLDIAG_BACKWARD,
};


/*******************************************************************************
|	 Function Source Code:CddMtr_Ctrl_Init
|******************************************************************************/
void CddMtr_Mng_Init(void)
{
	uint8_t fl_Mtr_Id_u8;
	memset(&CddMtr_Mng_BatMode,0,sizeof(CddMtr_Mng_BatMode_Str));
	memset(CddMtr_Mng_Main_St,0,sizeof(CddMtr_Mng_Main_St));
	for (fl_Mtr_Id_u8 = 0; fl_Mtr_Id_u8 < CDDMTR_HFKF_MAX_NUM; fl_Mtr_Id_u8 ++)
	{
		CddMtr_Read_E2_Ckc(fl_Mtr_Id_u8);
		
		if(CddMtr_Mng_Main_St[fl_Mtr_Id_u8].LearnData_Status == CDDMTR_MNG_LEARN_VALID)
		{
			CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Target_Step = CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Current_Step;
		}
		
		CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Req_Percent_Queue[0] = CDDMTR_MNG_PERCENT_INVALID;
		CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Req_Percent_Queue[1] = CDDMTR_MNG_PERCENT_INVALID;
		CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Req_Percent_Queue[2] = CDDMTR_MNG_PERCENT_INVALID;
		CddMtr_Ctrl_Handle(fl_Mtr_Id_u8,CDDMTR_CTRL_REQ_STOP);
	}

}

/*******************************************************************************
|	 Function Source Code:CddMtr_Update_PowerState
|******************************************************************************/
void CddMtr_Update_PowerState(void)
{
	uint32_t fl_temp_ad1=0,fl_temp_ad2=0;

	fl_temp_ad1 = Adcif_Get_AdcVal(ADCIF_CH_VBAT_SW_AD_AD);
	fl_temp_ad2 = Adcif_Get_AdcVal(ADCIF_CH_VBAT_SW_AD_AD);
	
	/*fl_PwdMode_vol1 = ( 57 * 500 * fl_temp_ad1 )/4096;*/
	/*fl_PwdMode_vol2 = ( 57 * 500 * fl_temp_ad2 )/4096;*/

	
	if((fl_temp_ad1 <= CDDMTR_MNG_7V_AD_VAL) || (fl_temp_ad2 <= CDDMTR_MNG_7V_AD_VAL))
	{
		if(CddMtr_Mng_BatMode.LviCnt < CDDMTR_MNG_VOL_LVI_FILTER)
		{
			CddMtr_Mng_BatMode.LviCnt ++;
		}
		else
		{
			CddMtr_Mng_BatMode.CurrentState = CDDMTR_MNG_VOL_LVI;
			CddMtr_Mng_BatMode.NormalCnt = 0;
		}
	}
	else if((fl_temp_ad1 > CDDMTR_MNG_8V_AD_VAL) && (fl_temp_ad2 > CDDMTR_MNG_8V_AD_VAL))
	{
		if(CddMtr_Mng_BatMode.NormalCnt < CDDMTR_MNG_VOL_NORMAL_FILTER)
		{
			CddMtr_Mng_BatMode.NormalCnt ++;
		}
		else
		{
			CddMtr_Mng_BatMode.CurrentState = CDDMTR_MNG_VOL_NORMAL;
			CddMtr_Mng_BatMode.LviCnt = 0;
		}
	}
	else
	{
		/*  keep current state*/
		/*	CddMtr_Current_PowerMode_u8 = CddMtr_Current_PowerMode_u8;*/
	}
}
/*******************************************************************************
|	 Function Source Code:CddMtr_LookUp_Compensate_Val
|******************************************************************************/
uint16_t CddMtr_LookUp_Compensate_Val(uint16_t fl_bat_ad)
{
	uint8_t fl_index_u8=0;
	uint16_t fl_temp_ad=0;

	fl_temp_ad = CddMtr_Mng_Battery_Compensate_Tbl[(sizeof(CddMtr_Mng_Battery_Compensate_Tbl)/sizeof(CddMtr_Mng_Battery_Compensate_Tbl[0]))-1][1];;
	
	for(fl_index_u8 = 0 ; fl_index_u8 < (sizeof(CddMtr_Mng_Battery_Compensate_Tbl)/sizeof(CddMtr_Mng_Battery_Compensate_Tbl[0])); fl_index_u8 ++)
	{
		if(fl_bat_ad <= CddMtr_Mng_Battery_Compensate_Tbl[fl_index_u8][0])
		{
			fl_temp_ad= CddMtr_Mng_Battery_Compensate_Tbl[fl_index_u8][1];
		}
	}

	return fl_temp_ad;
}

/*******************************************************************************
|	 Function Source Code:CddMtr_Ctrl_Task
|******************************************************************************/
void CddMtr_Mng_Task(void)
{
	uint8_t fl_Mtr_Id_u8;
	uint16_t fl_current_ad,fl_hall_ad,fl_swbat_ad,fl_Compensate_ad,fl_bat1_ad,fl_bat2_ad;
	uint8_t fl_ad_ch;
	
	CddMtr_Update_PowerState();
		
	if(CddMtr_Mng_BatMode.CurrentState == CDDMTR_MNG_VOL_LVI)
	{
		if(CddMtr_Mng_BatMode.LastState == CDDMTR_MNG_VOL_NORMAL)
		{
			for(fl_Mtr_Id_u8 = 0 ; fl_Mtr_Id_u8 < CDDMTR_HFKF_MAX_NUM; fl_Mtr_Id_u8 ++)
			{
				CddMtr_Mng_Stop_Run(fl_Mtr_Id_u8);
				CddMtr_Mng_Stop_Diag(fl_Mtr_Id_u8);
			}
			CddMtr_ShutDown_Update_Mtr_E2_Ckc();
			CddMtr_Mng_Init();
		}
	}
	else
	{
		fl_bat1_ad = Adcif_Get_AdcVal(ADCIF_CH_VBAT_SW_AD_AD);
		fl_bat1_ad = CddMtr_LookUp_Compensate_Val(fl_bat1_ad);
		fl_bat2_ad = Adcif_Get_AdcVal(ADCIF_CH_VBAT_SW_AD_AD);
		fl_bat2_ad = CddMtr_LookUp_Compensate_Val(fl_bat2_ad);
		
		for(fl_Mtr_Id_u8 = 0 ; fl_Mtr_Id_u8 < CDDMTR_HFKF_MAX_NUM; fl_Mtr_Id_u8 ++)
		{
			CddMtr_Mng_Handle(fl_Mtr_Id_u8);
			fl_ad_ch = CddMtr_Mng_Ad_Ch_Map[fl_Mtr_Id_u8][0];
			fl_current_ad = Adcif_Get_AdcVal(fl_ad_ch); 
			fl_ad_ch = CddMtr_Mng_Ad_Ch_Map[fl_Mtr_Id_u8][1];
			fl_hall_ad = Adcif_Get_AdcVal(fl_ad_ch); 
			fl_ad_ch = CddMtr_Mng_Ad_Ch_Map[fl_Mtr_Id_u8][2];
			fl_swbat_ad = Adcif_Get_AdcVal(fl_ad_ch);
			
			fl_Compensate_ad = fl_bat2_ad;
			if(ADCIF_CH_VBAT_SW_AD_AD ==  CddMtr_Mng_Ad_Ch_Map[fl_Mtr_Id_u8][3])
			{
				fl_Compensate_ad = fl_bat1_ad;
			}
			
			CddMtr_Mng_Detect(fl_Mtr_Id_u8,fl_current_ad,fl_hall_ad,fl_swbat_ad,fl_Compensate_ad);
		}
	}
	CddMtr_Mng_BatMode.LastState = CddMtr_Mng_BatMode.CurrentState;
}

/*******************************************************************************
|	 Function Source Code:CddMtr_Get_SleepCondition
|    TRUE-> Sleep condition is match 
|    FALSE->FALSE condition is not match
|******************************************************************************/
uint8_t CddMtr_Get_SleepCondition(void)
{
	uint8_t fl_Mtr_Id_u8;
	uint8_t fl_condition = TRUE;

	
	for(fl_Mtr_Id_u8 = 0 ; fl_Mtr_Id_u8 < CDDMTR_HFKF_MAX_NUM; fl_Mtr_Id_u8 ++)
	{
		if(CDDMTR_MNG_STATUS_IDLE != CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Main_Ctrl_Status)
		{
			fl_condition = FALSE;
			break;
		}
	}

	return fl_condition;
}

/*******************************************************************************
|	 Function Source Code:CddMtr_Write_To_EEPROM
|    
|    
|******************************************************************************/
uint8_t CddMtr_Write_To_EEPROM(void)
{
	uint8_t fl_ret = TRUE;
	
	fl_ret = CddMtr_ShutDown_Update_Mtr_E2_Ckc();
	
	return fl_ret;
}


/*******************************************************************************
|    Global Function interface Prototypes
|******************************************************************************/
uint8_t CddMtr_Learn_Req(uint8_t fl_Mtr_Id,CddMtr_Mng_Learn_e fl_learn_status )
{
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[2] = CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[1];
		CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[1] = CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[0];
		CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[0] = fl_learn_status;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/*******************************************************************************
|    Global Function interface Prototypes
|******************************************************************************/
CddMtr_Mng_Learn_e CddMtr_Get_Learn_Req(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Learn_e fl_learn_state;
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		if( CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[0] == CDDMTR_MNG_LEARN_NULL && \
			CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[1] == CDDMTR_MNG_LEARN_NULL && \
			CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[2] == CDDMTR_MNG_LEARN_NULL)
		{
			fl_learn_state = CDDMTR_MNG_LEARN_NULL;
		}
		else
		{
			if( CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[0] != CDDMTR_MNG_LEARN_NULL)
			{
				fl_learn_state = CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[0];
			}
			else if( CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[1] != CDDMTR_MNG_LEARN_NULL)
			{
				fl_learn_state = CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[1];
			}
			else if( CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[2] != CDDMTR_MNG_LEARN_NULL)
			{
				fl_learn_state = CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[2];
			}
			CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[0] = CDDMTR_MNG_LEARN_NULL;
			CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[1] = CDDMTR_MNG_LEARN_NULL;
			CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_Queue[2] = CDDMTR_MNG_LEARN_NULL;	
		}

		return fl_learn_state;
	}
	else
	{
		return CDDMTR_MNG_LEARN_NULL;
	}
}

/*******************************************************************************
|    Global Function interface Prototypes
|******************************************************************************/
CddMtr_Mng_Run_Direction_e CddMtr_Get_Direction_Req(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Run_Direction_e fl_Dict_state;
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		if( CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[0] == CDDMTR_MNG_DIRECTION_NULL && \
			CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[1] == CDDMTR_MNG_DIRECTION_NULL && \
			CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[2] == CDDMTR_MNG_DIRECTION_NULL)
		{
			fl_Dict_state = CDDMTR_MNG_DIRECTION_NULL;
		}
		else
		{
			if( CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[0] != CDDMTR_MNG_DIRECTION_NULL)
			{
				fl_Dict_state = CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[0];
			}
			else if( CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[1] != CDDMTR_MNG_DIRECTION_NULL)
			{
				fl_Dict_state = CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[1];
			}
			else if( CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[2] != CDDMTR_MNG_DIRECTION_NULL)
			{
				fl_Dict_state = CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[2];
			}
			CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[0] = CDDMTR_MNG_DIRECTION_NULL;
			CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[1] = CDDMTR_MNG_DIRECTION_NULL;
			CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[2] = CDDMTR_MNG_DIRECTION_NULL;	
		}

		return fl_Dict_state;
	}
	else
	{
		return CDDMTR_MNG_DIRECTION_NULL;
	}
}



/*******************************************************************************
|    Global Function interface Prototypes
|******************************************************************************/
uint32_t CddMtr_Get_Percent_Req(uint8_t fl_Mtr_Id)
{
	uint32_t fl_percent_val;
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		if( CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[0] == CDDMTR_MNG_PERCENT_INVALID && \
			CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[1] == CDDMTR_MNG_PERCENT_INVALID && \
			CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[2] == CDDMTR_MNG_PERCENT_INVALID)
		{
			fl_percent_val = CDDMTR_MNG_PERCENT_INVALID;
		}
		else
		{
			if( CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[0] != CDDMTR_MNG_PERCENT_INVALID)
			{
				fl_percent_val = CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[0];
			}
			else if( CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[1] != CDDMTR_MNG_PERCENT_INVALID)
			{
				fl_percent_val = CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[1];
			}
			else if( CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[2] != CDDMTR_MNG_PERCENT_INVALID)
			{
				fl_percent_val = CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[2];
			}
			CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[0] = CDDMTR_MNG_PERCENT_INVALID;
			CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[1] = CDDMTR_MNG_PERCENT_INVALID;
			CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[2] = CDDMTR_MNG_PERCENT_INVALID;	
		}

		return fl_percent_val;
	}
	else
	{
		return CDDMTR_MNG_PERCENT_INVALID;
	}
}



/*******************************************************************************
|    Global Function interface Prototypes
|******************************************************************************/
uint8_t CddMtr_Direction_Req(uint8_t fl_Mtr_Id,CddMtr_Mng_Run_Direction_e fl_Ctrl_Direct )
{
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[2] = CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[1];
		CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[1] = CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[0];
		CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction_Queue[0] = fl_Ctrl_Direct;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/*******************************************************************************
|    Global Function interface Prototypes
|******************************************************************************/
uint8_t CddMtr_Percent_Req(uint8_t fl_Mtr_Id,uint32_t fl_percent_u32 )
{
	UINT64 fl_Temp_U64;
	if((fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)  && (CDDMTR_MNG_MAX_PERCENT >= fl_percent_u32))
	{
		fl_Temp_U64 = (UINT64)fl_percent_u32;
		fl_Temp_U64 = fl_Temp_U64 * CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step / CDDMTR_MNG_MAX_PERCENT;
		CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[2] = CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[1];
		CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[1] = CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[0];
		CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Percent_Queue[0] = (uint32_t)fl_Temp_U64;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/*******************************************************************************
|    Global Function interface Prototypes
|******************************************************************************/
CddMtr_Mng_Hall_Status_e CddMtr_Get_Hall_Fault_Status(uint8_t fl_Mtr_Id)
{
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		return CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status;
	}
	else
	{
		return CDDMTR_MNG_HALL_FAULT;
	}
}
/*******************************************************************************
|    Global Function interface Prototypes
|******************************************************************************/
CddMtr_Mng_LearnData_e   CddMtr_Get_LearnData_Status(uint8_t fl_Mtr_Id)
{
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		return CddMtr_Mng_Main_St[fl_Mtr_Id].LearnData_Status;
	}
	else
	{
		return CDDMTR_MNG_LEARNDATA_INVALID;
	}
}


/*******************************************************************************
|	 Function Source Code:CddMtrl_Mng_Start_Detect
|******************************************************************************/
void CddMtr_Mng_Start_Run(uint8_t fl_Mtr_Id,CddMtr_Mng_Run_Direction_e fl_run_direct)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Run_Direct = fl_run_direct;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Cancel_Tick = 0;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Comfirm_Tick = 0;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Diag_Start = CDDMTR_MNG_START;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Diag_Tick = 0;
	if(CDDMTR_MNG_DIRECTION_FORWARD == fl_run_direct){CddMtr_Ctrl_Handle(fl_Mtr_Id,CDDMTR_HFKF_REQ_FORWARD);}
	else if(CDDMTR_MNG_DIRECTION_BACKWARD == fl_run_direct){CddMtr_Ctrl_Handle(fl_Mtr_Id,CDDMTR_HFKF_REQ_BACKWARD);}
	else{CddMtr_Ctrl_Handle(fl_Mtr_Id,CDDMTR_CTRL_REQ_STOP);}
	
}

/*******************************************************************************
|	 Function Source Code:CddMtr_Mng_Stop_Diag
|******************************************************************************/
void CddMtr_Mng_Stop_Diag(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Run_Direct = CDDMTR_MNG_DIRECTION_STOP;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Cancel_Tick = 0;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Comfirm_Tick = 0;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Diag_Start = CDDMTR_MNG_START;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Diag_Tick = 0;
}
/*******************************************************************************
|	 Function Source Code:CddMtr_Mng_Stop_Diag
|******************************************************************************/
void CddMtr_Mng_Stop_Run(uint8_t fl_Mtr_Id)
{
	CddMtr_Ctrl_Handle(fl_Mtr_Id,CDDMTR_CTRL_REQ_STOP);
}


/*******************************************************************************
|	 Function Source Code:CddMtr_Mng_Max_Is_Valid
|******************************************************************************/
uint8_t CddMtr_Mng_Max_Is_Valid(uint32_t fl_Max_Step)
{
	if(fl_Max_Step > CDDMTR_MNG_CAL_MAXSTEP_MAXVAL)
	{
		return FALSE;
	}
	if(fl_Max_Step < CDDMTR_MNG_CAL_MAXSTEP_MINVAL)
	{
		return FALSE;
	}
	return TRUE;
}



/*******************************************************************************
|	 Function Source Code:CddMtr_Read_E2_Ckc
|******************************************************************************/

void CddMtr_Read_E2_Ckc(uint8_t fl_Mtr_Id_u8)
{	
	CddEeprom_Req_Read(EEPROM_BANK_MOTOR,CDDMTR_MNG_MAIN_E2_ADDR+fl_Mtr_Id_u8*5,5,CddMtr_E2_Buffer);
	CddMtr_Mng_Main_St[fl_Mtr_Id_u8].LearnData_Status = (CddMtr_Mng_LearnData_e)CddMtr_E2_Buffer[0];
	CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Current_Step = CddMtr_E2_Buffer[1];
	CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Cal_Max_Step = CddMtr_E2_Buffer[2];
	CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Hall_Status = (CddMtr_Mng_Hall_Status_e)CddMtr_E2_Buffer[3];
    CddMtr_E2_Buffer[5] = usMBCRC16((uint8_t*)CddMtr_E2_Buffer,4);
	
	if(CddMtr_Mng_Main_St[fl_Mtr_Id_u8].LearnData_Status != CDDMTR_MNG_LEARN_VALID)
	{			
		CddMtr_Mng_Main_St[fl_Mtr_Id_u8].LearnData_Status = CDDMTR_MNG_LEARNDATA_INVALID;
	}
	
	if(FALSE == CddMtr_Mng_Max_Is_Valid(CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Cal_Max_Step))
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id_u8].LearnData_Status = CDDMTR_MNG_LEARNDATA_INVALID;
	}
	
	if(CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Cal_Max_Step < CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Current_Step)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id_u8].LearnData_Status = CDDMTR_MNG_LEARNDATA_INVALID;
	}
	if(CddMtr_E2_Buffer[4] != CddMtr_E2_Buffer[5])
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id_u8].LearnData_Status = CDDMTR_MNG_LEARNDATA_INVALID;
	}
}


/*******************************************************************************
|	 Function Source Code:CRC8_Table
|******************************************************************************/

void CddMtr_Update_Mtr_E2_Ckc(uint8_t fl_Mtr_Id_u8)
{		 
#if CDDMTR_DEBUG_MTR_LEARN
	uint8_t fl_e2_diff = FALSE;

	CddEeprom_Req_Read(EEPROM_BANK_MOTOR,CDDMTR_MNG_MAIN_E2_ADDR+fl_Mtr_Id_u8*5,5,CddMtr_E2_Buffer);
	if(CddMtr_E2_Buffer[0] != CddMtr_Mng_Main_St[fl_Mtr_Id_u8].LearnData_Status)
	{
		fl_e2_diff = TRUE;
	}
	if(CddMtr_E2_Buffer[1] != CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Current_Step)
	{
		fl_e2_diff = TRUE;
	}
	if(CddMtr_E2_Buffer[2] != CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Cal_Max_Step)
	{
		fl_e2_diff = TRUE;
	}
	if(CddMtr_E2_Buffer[3] != CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Hall_Status)
	{
		fl_e2_diff = TRUE;
	}
	
	if(TRUE == fl_e2_diff)	
	{
		CddMtr_E2_Buffer[0] = (uint16_t)CddMtr_Mng_Main_St[fl_Mtr_Id_u8].LearnData_Status;
		CddMtr_E2_Buffer[1] = (uint16_t)CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Current_Step;
		CddMtr_E2_Buffer[2] = (uint16_t)CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Cal_Max_Step;
		CddMtr_E2_Buffer[3] = (uint16_t)CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Hall_Status;
		CddMtr_E2_Buffer[4] = usMBCRC16((uint8_t *)CddMtr_E2_Buffer,4);
		CddEeprom_Req_Write(EEPROM_BANK_MOTOR,CDDMTR_MNG_MAIN_E2_ADDR+fl_Mtr_Id_u8*5,5,0,CddMtr_E2_Buffer);
	}
#endif
}
uint8_t CddMtr_ShutDown_Update_Mtr_E2_Ckc(void)
{		 
	uint8_t fl_e2_diff = FALSE;
	uint8_t fl_Mtr_Id_u8 = 0;
	uint8_t fl_ret = TRUE;
	for(fl_Mtr_Id_u8 = 0; fl_Mtr_Id_u8 < CDDMTR_SHUTDOWN_WRITE_NVM_MOTOR_NUM;fl_Mtr_Id_u8 ++)
	{
		CddEeprom_Req_Read(EEPROM_BANK_MOTOR,CDDMTR_MNG_MAIN_E2_ADDR+fl_Mtr_Id_u8*5,5,CddMtr_E2_Buffer);
		if(CddMtr_E2_Buffer[0] != CddMtr_Mng_Main_St[fl_Mtr_Id_u8].LearnData_Status)
		{
			fl_e2_diff = TRUE;
		}
		if(CddMtr_E2_Buffer[1] != CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Current_Step)
		{
			fl_e2_diff = TRUE;
		}
		if(CddMtr_E2_Buffer[2] != CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Cal_Max_Step)
		{
			fl_e2_diff = TRUE;
		}
		if(CddMtr_E2_Buffer[3] != CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Hall_Status)
		{
			fl_e2_diff = TRUE;
		}
		if(TRUE == fl_e2_diff)
		{
			break;
		}
	}
	if(TRUE == fl_e2_diff)	
	{
		for(fl_Mtr_Id_u8 = 0; fl_Mtr_Id_u8 < CDDMTR_SHUTDOWN_WRITE_NVM_MOTOR_NUM;fl_Mtr_Id_u8 ++)
		{
			CddMtr_E2_Buffer[fl_Mtr_Id_u8*5+0] = (uint16_t)CddMtr_Mng_Main_St[fl_Mtr_Id_u8].LearnData_Status;
			CddMtr_E2_Buffer[fl_Mtr_Id_u8*5+1] = (uint16_t)CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Current_Step;
			CddMtr_E2_Buffer[fl_Mtr_Id_u8*5+2] = (uint16_t)CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Cal_Max_Step;
			CddMtr_E2_Buffer[fl_Mtr_Id_u8*5+3] = (uint16_t)CddMtr_Mng_Main_St[fl_Mtr_Id_u8].Hall_Status;
			CddMtr_E2_Buffer[fl_Mtr_Id_u8*5+4] = usMBCRC16((uint8_t *)&(CddMtr_E2_Buffer[fl_Mtr_Id_u8*5]),4);
		}
		fl_ret = CddEeprom_Req_Write(EEPROM_BANK_MOTOR,CDDMTR_MNG_MAIN_E2_ADDR,5*CDDMTR_SHUTDOWN_WRITE_NVM_MOTOR_NUM,0,CddMtr_E2_Buffer);
	}
	return fl_ret;
}


/*******************************************************************************
|	 Function Source Code:CddMtr_Ctrl_Handle
|******************************************************************************/

void CddMtr_Mng_Handle(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_Status_e fl_Mtr_Status;
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status < CDDMTR_MNG_STATUS_MAX)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status = CddMtr_Mng_Handle_Func_Tbl[CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status][1](fl_Mtr_Id);
	}
	else
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status = CDDMTR_MNG_STATUS_IDLE;
	}

	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status < CDDMTR_MNG_STATUS_MAX)
	{
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Last_Main_Ctrl_Status != CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status)
		{
			/*exit last state*/
			fl_Mtr_Status = CddMtr_Mng_Handle_Func_Tbl[CddMtr_Mng_Main_St[fl_Mtr_Id].Last_Main_Ctrl_Status][2](fl_Mtr_Id);
		}
	}
	else
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status = CDDMTR_MNG_STATUS_IDLE;
	}

	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status < CDDMTR_MNG_STATUS_MAX)
	{
		/*handle state change*/
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Last_Main_Ctrl_Status != CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status)
		{			
			/*enter new state*/
			fl_Mtr_Status = CddMtr_Mng_Handle_Func_Tbl[CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status][0](fl_Mtr_Id);
		}
	}
	else
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status = CDDMTR_MNG_STATUS_IDLE;
	}

	fl_Mtr_Status = fl_Mtr_Status;/*avoid warning*/

	CddMtr_Mng_Main_St[fl_Mtr_Id].Last_Main_Ctrl_Status = CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status;
}

/*******************************************************************************
|	 Function Source Code:Hall and Stuck Detect 
|******************************************************************************/

void CddMtr_Mng_Detect(uint8_t fl_Mtr_Id,uint16_t fl_Current_Ad,uint16_t fl_hall_Ad,uint16_t fl_swbat_ad,uint16_t fl_Compensate_ad)
{
	uint32_t fl_temp1_val = 0,fl_temp2_val = 0;

	
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Run_Direct == CDDMTR_MNG_DIRECTION_BACKWARD)
	{
		if(fl_Current_Ad > CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].OverCurrent_High_Threshold_Ad)
		{
			if(CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Comfirm_Tick >= CDDMTR_MNG_DELAY_1000MS)
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status = CDDMTR_MNG_STUCK_BACKWARD;
			}
			else
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Comfirm_Tick++;
			}
			CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Cancel_Tick = 0;
		}
		else if(fl_Current_Ad < CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].OverCurrent_Low_Threshold_Ad)
		{
			if(CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Cancel_Tick >= CDDMTR_MNG_DELAY_300MS)
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status = CDDMTR_MNG_NO_STUCK;
			}
			else
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Cancel_Tick++;
			}
			CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Comfirm_Tick = 0;
		}
	}
	else if(CddMtr_Mng_Main_St[fl_Mtr_Id].Run_Direct == CDDMTR_MNG_DIRECTION_FORWARD)
	{
		if(fl_Current_Ad > CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].OverCurrent_High_Threshold_Ad)
		{
			if(CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Comfirm_Tick >= CDDMTR_MNG_DELAY_1000MS)
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status = CDDMTR_MNG_STUCK_FORWARD;
			}
			else
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Comfirm_Tick++;
			}
			CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Cancel_Tick = 0;
		}
		else if(fl_Current_Ad < CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].OverCurrent_Low_Threshold_Ad)
		{
			if(CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Cancel_Tick >= CDDMTR_MNG_DELAY_300MS)
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status = CDDMTR_MNG_NO_STUCK;
			}
			else
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Cancel_Tick++;
			}
			CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Comfirm_Tick = 0;
		}

	}
	else
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Cancel_Tick = 0;
		CddMtr_Mng_Main_St[fl_Mtr_Id].Stuck_Diag_Comfirm_Tick = 0;
	}

	fl_temp1_val = (uint32_t)fl_hall_Ad;
	fl_temp2_val = (uint32_t)fl_swbat_ad;
	fl_temp1_val = fl_temp1_val * 151 / 51;
	fl_temp2_val = fl_temp2_val * 57 / 10;
	fl_temp1_val += fl_Compensate_ad;

	if(fl_hall_Ad < CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].Hall_Low_Threshold_Ad)
	{
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Diag_Tick >= CDDMTR_MNG_DELAY_300MS)
		{
			CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status = CDDMTR_MNG_HALL_SHORTGND;
		}
		else
		{
			CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Diag_Tick++;
		}
		CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Diag_CancelTick=0;
	}
	else if(fl_temp1_val > fl_temp2_val)
	{
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Diag_Tick >= CDDMTR_MNG_DELAY_300MS)
		{
			CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status = CDDMTR_MNG_HALL_SHORTBAT_OPEN;
		}
		else
		{
			CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Diag_Tick++;
		}
		CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Diag_CancelTick=0;
	}
	else
	{
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Diag_CancelTick >= CDDMTR_MNG_DELAY_300MS)
		{
			CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status = CDDMTR_MNG_HALL_NORMAL;
		}
		else
		{
			CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Diag_CancelTick++;
		}
		CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Diag_Tick = 0;
	}

}
/*******************************************************************************
|	 Function Source Code:Counter Motor Step 
|******************************************************************************/

void CddMtr_Mng_Motor_Step(uint8_t fl_Mtr_Id,uint16_t fl_Ad_Val)
{
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Run_Direct == CDDMTR_MNG_DIRECTION_FORWARD)
	{
		if((fl_Ad_Val > CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].High_Threshold_Ad + CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[2]) && \
			(CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[0] > CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].High_Threshold_Ad + CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[3]))
		{
			if(CDDMTR_MNG_HIGH != CddMtr_Mng_Main_St[fl_Mtr_Id].Diag_Start)
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Diag_Start = CDDMTR_MNG_HIGH;
			if(CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step < CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step)
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step ++;
				}
			}
		}
		else
		{
			if((fl_Ad_Val + CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].Low_Threshold_Ad < CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[2]) && \
			  (CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[0] + CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].Low_Threshold_Ad < CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[3]))
			{
				if(CDDMTR_MNG_LOW != CddMtr_Mng_Main_St[fl_Mtr_Id].Diag_Start)
				{
					CddMtr_Mng_Main_St[fl_Mtr_Id].Diag_Start = CDDMTR_MNG_LOW;
				if(CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step < CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step)
				{
					CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step ++;
					}
				}
			}

		}
	}
	else if(CddMtr_Mng_Main_St[fl_Mtr_Id].Run_Direct == CDDMTR_MNG_DIRECTION_BACKWARD)
	{
		if((fl_Ad_Val > CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].High_Threshold_Ad + CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[2]) && \
			(CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[0] > CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].High_Threshold_Ad + CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[3]))
		{
			if(CDDMTR_MNG_HIGH != CddMtr_Mng_Main_St[fl_Mtr_Id].Diag_Start)
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Diag_Start = CDDMTR_MNG_HIGH;
			if(CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step > (uint32_t)0)
			{
				CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step --;
				}
			}
		}
		else
		{
			if((fl_Ad_Val + CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].Low_Threshold_Ad < CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[2]) && \
			  (CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[0] + CddMtr_Mng_Ad_Threshold_Tbl[fl_Mtr_Id].Low_Threshold_Ad < CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[3]))
			{
				if(CDDMTR_MNG_LOW != CddMtr_Mng_Main_St[fl_Mtr_Id].Diag_Start)
				{
					CddMtr_Mng_Main_St[fl_Mtr_Id].Diag_Start = CDDMTR_MNG_LOW;
				if(CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step > (uint32_t)0)
				{
					CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step --;
					}
				}
			}

		}
	}
	
	CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[3] = CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[2];
	CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[2] = CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[1];
	CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[1] = CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[0];
	CddMtr_Mng_Main_St[fl_Mtr_Id].AdVal[0] = fl_Ad_Val;
}


/*******************************************************************************
|	 Function Source Code:State Management Table Function
|******************************************************************************/


CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_idle(uint8_t fl_Mtr_Id){return CDDMTR_MNG_STATUS_IDLE;}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_idle(uint8_t fl_Mtr_Id)
{	
	CddMtr_Mng_Learn_e fl_learn_state;
	CddMtr_Mng_Run_Direction_e fl_direct_state;
	uint32_t fl_percent_u32;
	
	fl_learn_state = CddMtr_Get_Learn_Req(fl_Mtr_Id);
	if(CDDMTR_MNG_LEARN_ENABLE == fl_learn_state)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_State = CDDMTR_MNG_LEARN_ENABLE;
	}

	if(CDDMTR_MNG_LEARN_ENABLE == CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_State )
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_State = CDDMTR_MNG_LEARN_NULL;
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status == CDDMTR_MNG_HALL_NORMAL)
		{
			return CDDMTR_MNG_STATUS_LEARN_1_DELAY;
		}
	}

	fl_direct_state = CddMtr_Get_Direction_Req(fl_Mtr_Id);

	if(CDDMTR_MNG_DIRECTION_NULL != fl_direct_state)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State = fl_direct_state;
	}
	
	if(CDDMTR_MNG_DIRECTION_FORWARD == CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State && CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status != CDDMTR_MNG_STUCK_FORWARD)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction =CDDMTR_MNG_DIRECTION_FORWARD; 
		CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State = CDDMTR_MNG_DIRECTION_NULL;

		CddMtr_Mng_Main_St[fl_Mtr_Id].LearnData_Status = CDDMTR_MNG_LEARNDATA_INVALID;
		CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step =CDDMTR_MNG_MAX_STEP_VAL;
		CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step =0;
		CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step =0;
		CddMtr_Update_Mtr_E2_Ckc(fl_Mtr_Id);
		
		return CDDMTR_MNG_STATUS_DIRECT_1_START;
	}
	if(CDDMTR_MNG_DIRECTION_BACKWARD == CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State && CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status != CDDMTR_MNG_STUCK_BACKWARD)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction =CDDMTR_MNG_DIRECTION_BACKWARD;
		CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State = CDDMTR_MNG_DIRECTION_NULL;

		CddMtr_Mng_Main_St[fl_Mtr_Id].LearnData_Status = CDDMTR_MNG_LEARNDATA_INVALID;
		CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step =CDDMTR_MNG_MAX_STEP_VAL;
		CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step =0;
		CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step =0;
		CddMtr_Update_Mtr_E2_Ckc(fl_Mtr_Id);
		
		return CDDMTR_MNG_STATUS_DIRECT_1_START;
	}
	
	CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State = CDDMTR_MNG_DIRECTION_NULL;

	fl_percent_u32 = CddMtr_Get_Percent_Req(fl_Mtr_Id);
	if(CDDMTR_MNG_PERCENT_INVALID != fl_percent_u32)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step = fl_percent_u32 ;
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step > CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step)
		{
			CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step = CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step;
		}
	}

	if(CddMtr_Mng_Main_St[fl_Mtr_Id].LearnData_Status == CDDMTR_MNG_LEARN_VALID)
	{
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status == CDDMTR_MNG_HALL_NORMAL)
		{
			if(CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step > CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step + CDDMTR_MNG_MIN_MOVE_STEP)
			{
				if(CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step < CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step)
				{
					CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction = CDDMTR_MNG_DIRECTION_FORWARD;
					return CDDMTR_MNG_STATUS_PERCENT_1_DELAY;
				}
			}
			if(CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step + CDDMTR_MNG_MIN_MOVE_STEP < CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step)
			{
				if(CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step > 0)
				{
					CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction = CDDMTR_MNG_DIRECTION_BACKWARD;
					return CDDMTR_MNG_STATUS_PERCENT_1_DELAY;
				}
			}
		}
	}
	
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_idle(uint8_t fl_Mtr_Id){return CDDMTR_MNG_STATUS_IDLE;}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_Stop(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_50MS;
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_Stop(uint8_t fl_Mtr_Id)
{
	if(0 < CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick--;
		return CDDMTR_MNG_STATUS_STOP;
	}
	else
	{
		CddMtr_Mng_Stop_Diag(fl_Mtr_Id);
		return CDDMTR_MNG_STATUS_IDLE;
	}
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_Stop(uint8_t fl_Mtr_Id){return CDDMTR_MNG_STATUS_IDLE;}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_LEARN_1_DELAY(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].LearnData_Status = CDDMTR_MNG_LEARNDATA_INVALID;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status =CDDMTR_MNG_HALL_NORMAL;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step =CDDMTR_MNG_MAX_STEP_VAL;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step =0;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step =0;
	CddMtr_Update_Mtr_E2_Ckc(fl_Mtr_Id);
	
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_50MS;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Run_Direct = CDDMTR_MNG_DIRECTION_STOP;
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_LEARN_1_DELAY(uint8_t fl_Mtr_Id)
{
	if(0 < CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick--;
		return CDDMTR_MNG_STATUS_LEARN_1_DELAY;
	}
	else
	{
		return CDDMTR_MNG_STATUS_LEARN_5_BACKWARD;
	}
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_LEARN_1_DELAY(uint8_t fl_Mtr_Id){return CDDMTR_MNG_STATUS_IDLE;}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_LEARN_5_BACKWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_30S;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step = 0;
	CddMtr_Mng_Start_Run(fl_Mtr_Id,CDDMTR_MNG_DIRECTION_BACKWARD);
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_LEARN_5_BACKWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Learn_e fl_learn_state;
	
	fl_learn_state = CddMtr_Get_Learn_Req(fl_Mtr_Id);
	if(fl_learn_state == CDDMTR_MNG_LEARN_DISABLE)
	{
		return CDDMTR_MNG_STATUS_LEARN_8_FAIL;
	}
	
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick > 0)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick --;
	}
	else/*timeout*/
	{
		return CDDMTR_MNG_STATUS_LEARN_8_FAIL;
	}

	if(CDDMTR_MNG_HALL_FAULT <= CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status)
	{
		return CDDMTR_MNG_STATUS_LEARN_8_FAIL;
	}
	
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status == CDDMTR_MNG_STUCK_BACKWARD)
	{
		return CDDMTR_MNG_STATUS_LEARN_6_DELAY;
	}
	
	return CDDMTR_MNG_STATUS_LEARN_5_BACKWARD;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_LEARN_5_BACKWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Stop_Run(fl_Mtr_Id);
	return CDDMTR_MNG_STATUS_IDLE;
}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_LEARN_6_DELAY(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_50MS;
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_LEARN_6_DELAY(uint8_t fl_Mtr_Id)
{
	if(0 < CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick--;
		return CDDMTR_MNG_STATUS_LEARN_7_FORWARD;
	}
	else
	{
		return CDDMTR_MNG_STATUS_LEARN_6_DELAY;
	}
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_LEARN_6_DELAY(uint8_t fl_Mtr_Id){return CDDMTR_MNG_STATUS_IDLE;}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_LEARN_7_FORWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step = 0;
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_30S;
	CddMtr_Mng_Start_Run(fl_Mtr_Id,CDDMTR_MNG_DIRECTION_FORWARD);
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_LEARN_7_FORWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Learn_e fl_learn_state;
	
	fl_learn_state = CddMtr_Get_Learn_Req(fl_Mtr_Id);
	if(fl_learn_state == CDDMTR_MNG_LEARN_DISABLE)
	{
		return CDDMTR_MNG_STATUS_LEARN_8_FAIL;
	}
	
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick > 0)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick --;
	}
	else/*timeout*/
	{
		return CDDMTR_MNG_STATUS_LEARN_8_FAIL;
	}

	if(CDDMTR_MNG_HALL_FAULT <= CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status)
	{
		return CDDMTR_MNG_STATUS_LEARN_8_FAIL;
	}

	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status == CDDMTR_MNG_STUCK_FORWARD)
	{
		CddMtr_Mng_Stop_Run(fl_Mtr_Id);

		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step > CDDMTR_MNG_CAL_MAXSTEP_MAXVAL || CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step < CDDMTR_MNG_CAL_MAXSTEP_MINVAL)
		{
			return CDDMTR_MNG_STATUS_LEARN_8_FAIL;
		}
		else
		{
			CddMtr_Mng_Main_St[fl_Mtr_Id].LearnData_Status = CDDMTR_MNG_LEARN_VALID;
			CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step = CddMtr_Learn_End_Pos[fl_Mtr_Id];
			CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step = CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step;
			CddMtr_Update_Mtr_E2_Ckc(fl_Mtr_Id);
			return CDDMTR_MNG_STATUS_STOP;
		}
	}

	return CDDMTR_MNG_STATUS_LEARN_7_FORWARD;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_LEARN_7_FORWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Stop_Run(fl_Mtr_Id);
	return CDDMTR_MNG_STATUS_IDLE;
}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_LEARN_8_FAIL(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].LearnData_Status = CDDMTR_MNG_LEARNDATA_INVALID;
	CddMtr_Update_Mtr_E2_Ckc(fl_Mtr_Id);
	CddMtr_Mng_Stop_Run(fl_Mtr_Id);
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_50MS;
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_LEARN_8_FAIL(uint8_t fl_Mtr_Id)
{
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick > 0)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick --;
	}
	else/*timeout*/
	{
		CddMtr_Mng_Stop_Diag(fl_Mtr_Id);
		return CDDMTR_MNG_STATUS_IDLE;
	}
	return CDDMTR_MNG_STATUS_LEARN_8_FAIL;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_LEARN_8_FAIL(uint8_t fl_Mtr_Id){return CDDMTR_MNG_STATUS_IDLE;}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_DIRECT_1_START(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_50MS;
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_DIRECT_1_START(uint8_t fl_Mtr_Id)
{
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick > 0)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick --;
	}
	else/*timeout*/
	{
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction == CDDMTR_MNG_DIRECTION_FORWARD)
		{
			CddMtr_Mng_Start_Run(fl_Mtr_Id,CDDMTR_MNG_DIRECTION_FORWARD);
			return CDDMTR_MNG_STATUS_DIRECT_2_FORWARD;
		}
		else
		{
			CddMtr_Mng_Start_Run(fl_Mtr_Id,CDDMTR_MNG_DIRECTION_BACKWARD);
			return CDDMTR_MNG_STATUS_DIRECT_3_BACKWARD;
		}
	}
	return CDDMTR_MNG_STATUS_DIRECT_1_START;

}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_DIRECT_1_START(uint8_t fl_Mtr_Id){return CDDMTR_MNG_STATUS_IDLE;}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_DIRECT_2_FORWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_30S;

	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_DIRECT_2_FORWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Learn_e fl_learn_state;
	CddMtr_Mng_Run_Direction_e fl_direct_state;
	
	fl_learn_state = CddMtr_Get_Learn_Req(fl_Mtr_Id);
	if(fl_learn_state == CDDMTR_MNG_LEARN_ENABLE)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_State = CDDMTR_MNG_LEARN_ENABLE;
		return CDDMTR_MNG_STATUS_STOP;
	}

	fl_direct_state = CddMtr_Get_Direction_Req(fl_Mtr_Id);
	if(CDDMTR_MNG_DIRECTION_STOP == fl_direct_state )
	{
		return CDDMTR_MNG_STATUS_STOP;
	}
	if(CDDMTR_MNG_DIRECTION_BACKWARD == fl_direct_state )
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State = CDDMTR_MNG_DIRECTION_BACKWARD;
		return CDDMTR_MNG_STATUS_STOP;
	}
	
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick > 0)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick --;
	}
	else/*timeout*/
	{	
		return CDDMTR_MNG_STATUS_STOP;
	}

	if(CDDMTR_MNG_HALL_FAULT <= CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status)
	{	
		//return CDDMTR_MNG_STATUS_STOP;
	}

	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status == CDDMTR_MNG_STUCK_FORWARD)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step = CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step;
		return CDDMTR_MNG_STATUS_STOP;
	}

	return CDDMTR_MNG_STATUS_DIRECT_2_FORWARD;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_DIRECT_2_FORWARD(uint8_t fl_Mtr_Id)
{	
	CddMtr_Mng_Stop_Run(fl_Mtr_Id);
	return CDDMTR_MNG_STATUS_IDLE;
}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_DIRECT_3_BACKWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_30S;
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_DIRECT_3_BACKWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Learn_e fl_learn_state;
	CddMtr_Mng_Run_Direction_e fl_direct_state;
	
	fl_learn_state = CddMtr_Get_Learn_Req(fl_Mtr_Id);
	if(fl_learn_state == CDDMTR_MNG_LEARN_ENABLE)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_State = CDDMTR_MNG_LEARN_ENABLE;
		return CDDMTR_MNG_STATUS_STOP;
	}

	fl_direct_state = CddMtr_Get_Direction_Req(fl_Mtr_Id);
	if(CDDMTR_MNG_DIRECTION_STOP == fl_direct_state )
	{
		return CDDMTR_MNG_STATUS_STOP;
	}
	if(CDDMTR_MNG_DIRECTION_FORWARD == fl_direct_state )
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State = CDDMTR_MNG_DIRECTION_FORWARD;
		return CDDMTR_MNG_STATUS_STOP;
	}
	
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick > 0)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick --;
	}
	else/*timeout*/
	{	
		return CDDMTR_MNG_STATUS_STOP;
	}

	if(CDDMTR_MNG_HALL_FAULT <= CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status)
	{	
		//return CDDMTR_MNG_STATUS_STOP;
	}

	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status == CDDMTR_MNG_STUCK_BACKWARD)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step = 0;
		return CDDMTR_MNG_STATUS_STOP;
	}

	return CDDMTR_MNG_STATUS_DIRECT_3_BACKWARD;
}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_DIRECT_3_BACKWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Stop_Run(fl_Mtr_Id);
	return CDDMTR_MNG_STATUS_IDLE;
}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_PERCENT_1_DELAY(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_50MS;
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_PERCENT_1_DELAY(uint8_t fl_Mtr_Id)
{
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick > 0)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick --;
	}
	else/*timeout*/
	{
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Req_Direction == CDDMTR_MNG_DIRECTION_FORWARD)
		{
			CddMtr_Mng_Start_Run(fl_Mtr_Id,CDDMTR_MNG_DIRECTION_FORWARD);
			return CDDMTR_MNG_STATUS_PERCENT_5_HALLDIAG_FORWARD;
		}
		else
		{
			CddMtr_Mng_Start_Run(fl_Mtr_Id,CDDMTR_MNG_DIRECTION_BACKWARD);
			return CDDMTR_MNG_STATUS_PERCENT_10_HALLDIAG_BACKWARD;
		}
	}

	return CDDMTR_MNG_STATUS_PERCENT_1_DELAY;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_PERCENT_1_DELAY(uint8_t fl_Mtr_Id){return CDDMTR_MNG_STATUS_IDLE;}


CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_PERCENT_5_HALLDIAG_FORWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_30S;
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_PERCENT_5_HALLDIAG_FORWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Learn_e fl_learn_state;
	CddMtr_Mng_Run_Direction_e fl_direct_state;
	uint32_t fl_percent_u32;
	
	fl_learn_state = CddMtr_Get_Learn_Req(fl_Mtr_Id);
	if(fl_learn_state == CDDMTR_MNG_LEARN_ENABLE)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_State = CDDMTR_MNG_LEARN_ENABLE;
		return CDDMTR_MNG_STATUS_STOP;
	}
	
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick > 0)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick --;
	}
	else/*timeout*/
	{
		return CDDMTR_MNG_STATUS_STOP;
	}
	
	fl_direct_state = CddMtr_Get_Direction_Req(fl_Mtr_Id);
	if(CDDMTR_MNG_DIRECTION_FORWARD == fl_direct_state )
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State = CDDMTR_MNG_DIRECTION_FORWARD;
		return CDDMTR_MNG_STATUS_STOP;
	}
	if(CDDMTR_MNG_DIRECTION_BACKWARD == fl_direct_state )
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State = CDDMTR_MNG_DIRECTION_BACKWARD;
		return CDDMTR_MNG_STATUS_STOP;
	}
	
	if(CDDMTR_MNG_HALL_FAULT <= CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status)
	{
		return CDDMTR_MNG_STATUS_STOP;
	}

	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status == CDDMTR_MNG_STUCK_FORWARD)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step = CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step;
		return CDDMTR_MNG_STATUS_STOP;
	}

	
	fl_percent_u32 = CddMtr_Get_Percent_Req(fl_Mtr_Id);
	if(fl_percent_u32 != CDDMTR_MNG_PERCENT_INVALID)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step = fl_percent_u32;
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step > CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step)
		{
			CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step = CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step;
		}
	}

	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step <= CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step)
	{
		return CDDMTR_MNG_STATUS_STOP;
	}
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step <= CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step)
	{
		return CDDMTR_MNG_STATUS_STOP;
	}

	return CDDMTR_MNG_STATUS_PERCENT_5_HALLDIAG_FORWARD;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_PERCENT_5_HALLDIAG_FORWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Stop_Run(fl_Mtr_Id);
	return CDDMTR_MNG_STATUS_IDLE;
}

CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_en_PERCENT_10_HALLDIAG_BACKWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick = CDDMTR_MNG_DELAY_30S;
	return CDDMTR_MNG_STATUS_IDLE;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_PERCENT_10_HALLDIAG_BACKWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Learn_e fl_learn_state;
	CddMtr_Mng_Run_Direction_e fl_direct_state;
	uint32_t fl_percent_u32;
	
	fl_learn_state = CddMtr_Get_Learn_Req(fl_Mtr_Id);
	if(fl_learn_state == CDDMTR_MNG_LEARN_ENABLE)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Learn_Req_State = CDDMTR_MNG_LEARN_ENABLE;
		return CDDMTR_MNG_STATUS_STOP;
	}
	
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick > 0)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Mng_Delay_Tick --;
	}
	else/*timeout*/
	{
		return CDDMTR_MNG_STATUS_STOP;
	}
	
	fl_direct_state = CddMtr_Get_Direction_Req(fl_Mtr_Id);
	if(CDDMTR_MNG_DIRECTION_FORWARD == fl_direct_state )
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State = CDDMTR_MNG_DIRECTION_FORWARD;
		return CDDMTR_MNG_STATUS_STOP;
	}
	if(CDDMTR_MNG_DIRECTION_BACKWARD == fl_direct_state )
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Direction_Req_State = CDDMTR_MNG_DIRECTION_BACKWARD;
		return CDDMTR_MNG_STATUS_STOP;
	}

	if(CDDMTR_MNG_HALL_FAULT <= CddMtr_Mng_Main_St[fl_Mtr_Id].Hall_Status)
	{
		return CDDMTR_MNG_STATUS_STOP;
	}

	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Stcuk_Status == CDDMTR_MNG_STUCK_BACKWARD)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step = 0;
		return CDDMTR_MNG_STATUS_STOP;
	}
	
	fl_percent_u32 = CddMtr_Get_Percent_Req(fl_Mtr_Id);
	if(fl_percent_u32 != CDDMTR_MNG_PERCENT_INVALID)
	{
		CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step = fl_percent_u32;
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step > CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step)
		{
			CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step = CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step;
		}
	}
	
	if(CddMtr_Mng_Main_St[fl_Mtr_Id].Target_Step >= CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step)
	{
		return CDDMTR_MNG_STATUS_STOP;
	}
	if(0 == CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step)
	{
		return CDDMTR_MNG_STATUS_STOP;
	}
	return CDDMTR_MNG_STATUS_PERCENT_10_HALLDIAG_BACKWARD;
}
CddMtr_Mng_Main_Status_e  CddMtr_Mng_handle_exit_PERCENT_10_HALLDIAG_BACKWARD(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Stop_Run(fl_Mtr_Id);
	return CDDMTR_MNG_STATUS_IDLE;
}


/*service interface*/

uint16_t CddMtr_Get_Current_Hall_Cnt(uint8_t fl_Mtr_Id)
{
	uint16_t hallcnt = 0;
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		hallcnt = CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step;
	}
	return hallcnt;
}


uint16_t CddMtr_Get_Mtr_Current_Val(uint8_t fl_Mtr_Id)
{
	uint16_t currentval = 0;
	uint16_t fl_ad_val;
	uint8_t fl_ad_ch;
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		fl_ad_ch = CddMtr_Mng_Ad_Ch_Map[fl_Mtr_Id][0];
		fl_ad_val = Adcif_Get_AdcVal(fl_ad_ch);
		currentval = UTL_u16Lookup((uint16_t *)CddMtr_Mng_FB_Current_Tbl,fl_ad_val,0);
	}
	
	return currentval;
}


CddMtr_Ctrl_Req_e CddMtr_Get_Mtr_Run_Direction(uint8_t fl_Mtr_Id)
{
	CddMtr_Ctrl_Req_e ReqDirection = CDDMTR_CTRL_REQ_STOP;
	
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		ReqDirection = CddMtr_Ctrl_Get_RunDiection(fl_Mtr_Id);
	}
	
	return ReqDirection;
}


uint16_t CddMtr_Get_Mtr_PosPercent(uint8_t fl_Mtr_Id)
{
	uint16_t percent = 0;
	uint32_t temp = 0;
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].LearnData_Status ==   CDDMTR_MNG_LEARN_VALID)
		{
			if(CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step < CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step)
			{
				temp = CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step;
				temp = temp * 100;
				temp = temp  / CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step;
				percent = (uint16_t)(temp);
			}
			else
			{
				percent = 100;
			}
		}
		else
		{
			if(CddMtr_Mng_Main_St[fl_Mtr_Id].Current_Step < CddMtr_Mng_Main_St[fl_Mtr_Id].Cal_Max_Step)
			{
				percent = 0;
			}
			else
			{
				percent = 100;
			}
		}

	}
	return percent;
}


uint8_t CddMtr_Get_Mtr_Learning_Status(uint8_t fl_Mtr_Id)
{
	uint8_t learning = FALSE;
	
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		if(CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status <= CDDMTR_MNG_STATUS_LEARN_8_FAIL  )
		{
			if(CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status >= CDDMTR_MNG_STATUS_STOP)
			{
				learning = TRUE;
			}
		}
	}
	
	return learning;
}

uint8_t CddMtr_Get_Mtr_Run_Status(uint8_t fl_Mtr_Id)
{
	CddMtr_Mng_Main_Status_e Mtr_RunSts =CDDMTR_MNG_STATUS_IDLE;
	
	if(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM)
	{
		Mtr_RunSts = CddMtr_Mng_Main_St[fl_Mtr_Id].Main_Ctrl_Status;
	}
	
	return Mtr_RunSts;
}


/*EOF*/
