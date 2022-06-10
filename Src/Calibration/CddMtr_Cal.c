
/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "CddMtr_Cal.h"
#include "CddMtr_Mng.h"

#include  "CddMtr_Ctrl.h"
#include  "CddMtr_HFKF.h"
#include "Adcif.h"

#include "PowerMgmt.h"
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


/*******************************************************************************
|    Static local variables Declaration
|******************************************************************************/

/*******************************************************************************
|    Table Definition
|******************************************************************************/

const uint16_t CddMtr_Mng_Battery_Compensate_Tbl[11][2] = 
{
	1006, 400,//7V
	1150, 400,
	1293, 400,
	1437, 400,//10V
	1581, 400,
	1725, 400,
	1868, 400,//13V
	2012, 400,
	2156, 400,
	2300, 400,//16V
	2443, 400,//17V
};


const CddMtr_Mng_Threshold_Ad_Str CddMtr_Mng_Ad_Threshold_Tbl[CDDMTR_HFKF_MAX_NUM] = 
{
/*cpunt step ,stuck ad   , short to battery */
	280,  280, 1290 , 990,  100 , 
	280,  280, 1290 , 990,  100 , 
	280,  280, 1290 , 990,  100 , 
	280,  280, 1290 , 990,  100 ,
};

const uint8_t CddMtr_Mng_Ad_Ch_Map[CDDMTR_HFKF_MAX_NUM][4] = 
{
	ADCIF_CH_Seat_RAIL_AD,             ADCIF_CH_RAIL_MOTOR_HALL_AD,    ADCIF_CH_BATT_SW_AD,   ADCIF_CH_BATT2_AD,
	ADCIF_CH_SEAT_BACK_MTR_SNSR_AD,    ADCIF_CH_MotorBack_HALL_AD,     ADCIF_CH_BATT_SW_AD,   ADCIF_CH_BATT1_AD,
	ADCIF_CH_Motor3_SNSR_AD,           ADCIF_CH_Motor3_HALL_AD,        ADCIF_CH_BATT_SW_AD,   ADCIF_CH_BATT2_AD,
	ADCIF_CH_Motor4_SNSR_AD,           ADCIF_CH_Motor4_HALL_AD,        ADCIF_CH_BATT_SW_AD,   ADCIF_CH_BATT1_AD,
};



const uint16_t CddMtr_Mng_FB_Current_Tbl[37] = 
{
	18,
	30, 0 ,
	61,500,
	184,1500,
	307,2500,
	430,3500,
	552,4500,
	675,5500,
	798,6500,
	921,7500,
	1044,8500,
	1167,9500,
	1228,10000,
	1290,10500,
	1474,12000,
	1597,13000,
	1720,14000,
	1966,16000,
	2211,18000
};


/*******************************************************************************
|    Static Local Functions Declaration
|******************************************************************************/


/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/



/*EOF*/
