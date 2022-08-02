
/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "CddMtr_HFKF.h"
#include "CddMtr_Ctrl.h"
#include "fm33_assert.h"

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

CddMtr_Ctrl_Str CddMtr_Ctrl_St_Table[CDDMTR_HFKF_MAX_NUM] = 
{
	CDDMTR_CTRL_REQ_STOP,
	CDDMTR_CTRL_REQ_STOP,
	CDDMTR_CTRL_REQ_STOP,
	CDDMTR_CTRL_REQ_STOP,
	CDDMTR_CTRL_REQ_STOP,
	CDDMTR_CTRL_REQ_STOP
};

/*******************************************************************************
|    Static local variables Declaration
|******************************************************************************/


/*******************************************************************************
|    Table Definition
|******************************************************************************/

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
|	 Function Source Code:CddMtr_Ctrl_Init
|******************************************************************************/
void CddMtr_Ctrl_Init(void)
{
	uint8_t fl_Mtr_Id_u8;
	for (fl_Mtr_Id_u8 = 0; fl_Mtr_Id_u8 < CDDMTR_HFKF_MAX_NUM; fl_Mtr_Id_u8 ++)
	{
		CddMtr_Ctrl_St_Table[fl_Mtr_Id_u8].Req_State = CDDMTR_CTRL_REQ_STOP;
	}

}

/*******************************************************************************
|    Function Source Code:CddMtr_Ctrl_Task
|******************************************************************************/
void CddMtr_Ctrl_Task(void)
{

}
/*******************************************************************************
|    Function Source Code:CddMtr_Ctrl_Handle
|******************************************************************************/

void CddMtr_Ctrl_Handle(uint8_t fl_Mtr_Id,CddMtr_Ctrl_Req_e fl_Ctrl_State)
{
	assert_param(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM);
	assert_param(fl_Ctrl_State < CDDMTR_HFKF_REQ_MAXVAL);
	CddMtr_Ctrl_St_Table[fl_Mtr_Id].Req_State = fl_Ctrl_State;
	switch(CddMtr_Ctrl_St_Table[fl_Mtr_Id].Req_State)
	{
		case CDDMTR_CTRL_REQ_STOP:
			CDDMTR_HFKF_CTRL_P_LOW_N_LOW(fl_Mtr_Id);
		break;

		case CDDMTR_HFKF_REQ_FORWARD:
			CDDMTR_HFKF_CTRL_P_HIGH_N_LOW(fl_Mtr_Id);
		break;

		case CDDMTR_HFKF_REQ_BACKWARD:
			CDDMTR_HFKF_CTRL_P_LOW_N_HIGH(fl_Mtr_Id);
		break;

		default:
		break;
	}

}


CddMtr_Ctrl_Req_e CddMtr_Ctrl_Get_RunDiection(uint8_t fl_Mtr_Id)
{
	assert_param(fl_Mtr_Id < CDDMTR_HFKF_MAX_NUM);
	return CddMtr_Ctrl_St_Table[fl_Mtr_Id].Req_State;
}


/*EOF*/
