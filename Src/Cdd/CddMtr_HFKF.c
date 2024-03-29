/******************************************************************************/

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "Ioif.h"
#include "CddMtr_HFKF.h"
/*******************************************************************************
|    Macro Definition
|******************************************************************************/
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
#if (SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)

const CddMtr_HFKF_Ctrl CddMtr_HFKF_CH_Table[CDDMTR_HFKF_MAX_NUM] =
{
	GPIO_NUMBER_A6_Motorseat_N ,GPIO_NUMBER_A7_Motorsea_P ,
	GPIO_NUMBER_A3_NLMotorback_P ,  GPIO_NUMBER_A2_NLMotorback_N,
	GPIO_NUMBER_A9_Motorhead_N  , GPIO_NUMBER_A8_Motorhead_P    ,
	GPIO_NUMBER_B9_Motorrotate_N,GPIO_NUMBER_B8_Motorrotate_P  ,
	GPIO_NUMBER_A11_Motorangle_P  ,GPIO_NUMBER_A10_Motorangle_N,
	GPIO_NUMBER_A4_Motorleg_P,GPIO_NUMBER_A5_Motorleg_N
};
#else
const CddMtr_HFKF_Ctrl CddMtr_HFKF_CH_Table[CDDMTR_HFKF_MAX_NUM] =
{
	GPIO_NUMBER_A7_Motorsea_P ,	GPIO_NUMBER_A6_Motorseat_N ,
	GPIO_NUMBER_A3_NLMotorback_P ,  GPIO_NUMBER_A2_NLMotorback_N,
	GPIO_NUMBER_A9_Motorhead_N  , GPIO_NUMBER_A8_Motorhead_P    ,
	GPIO_NUMBER_B9_Motorrotate_N,GPIO_NUMBER_B8_Motorrotate_P  ,
	GPIO_NUMBER_A10_Motorangle_N,GPIO_NUMBER_A11_Motorangle_P  ,
	GPIO_NUMBER_A4_Motorleg_P,GPIO_NUMBER_A5_Motorleg_N
};

#endif
/*******************************************************************************
|    Static Local Functions Declaration
|******************************************************************************/

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Function Source Code
|******************************************************************************/



/*EOF*/
