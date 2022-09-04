
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
	1006, 200,//7V
	1150, 200,
	1293, 200,
	1437, 200,//10V
	1581, 200,
	1725, 200,
	1868, 200,//13V
	2012, 200,
	2156, 200,
	2300, 200,//16V
	2443, 200,//17V
};


const CddMtr_Mng_Threshold_Ad_Str CddMtr_Mng_Ad_Threshold_Tbl[CDDMTR_HFKF_MAX_NUM] = 
{
/*cpunt step ,stuck ad   , short to battery */
	280,  280, 552 , 990,  100 , 
	280,  280, 798  , 990,  100 , 
	280,  280, 307 , 990,  100 , 
	280,  280, 798 , 990,  100 ,
	280,  280, 798 , 990,  100 , 
	280,  280, 798 , 990,  100 ,
};


const uint8_t CddMtr_Mng_Ad_Ch_Map[CDDMTR_HFKF_MAX_NUM][4] = 
{
	ADCIF_CH_MotorSeat_CURAD_AD,    ADCIF_CH_Rail_IO_HALL_AD,    ADCIF_CH_VBAT_SW_AD_AD,   ADCIF_CH_VBAT_SW_AD_AD,
	ADCIF_CH_Motorback_CURAD_AD,    ADCIF_CH_Back_IN_IO_HALL_AD,     ADCIF_CH_VBAT_SW_AD_AD,   ADCIF_CH_VBAT_SW_AD_AD,
	ADCIF_CH_Motorhead_CURAD_AD,    ADCIF_CH_Head_IN_IO_HALL_AD,        ADCIF_CH_VBAT_SW_AD_AD,   ADCIF_CH_VBAT_SW_AD_AD,
	ADCIF_CH_Motorangle_CURAD_AD,	ADCIF_CH_Angle_IO_HALL_AD,		  ADCIF_CH_VBAT_SW_AD_AD,	ADCIF_CH_VBAT_SW_AD_AD,
	ADCIF_CH_Motorrotate_CURAD_AD,	ADCIF_CH_Rotate_IN_IO_HALL_AD,	   ADCIF_CH_VBAT_SW_AD_AD,	 ADCIF_CH_VBAT_SW_AD_AD,
	ADCIF_CH_Motorleg_CURAD_AD,		ADCIF_CH_Leg_IN_IO_HALL_AD, 	   ADCIF_CH_VBAT_SW_AD_AD,   ADCIF_CH_VBAT_SW_AD_AD,
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


const uint16_t CddMtr_Learn_End_Pos[6] = 
{
	10000,
	10000,
	10000,
	10000,
	10000,
	10000
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
