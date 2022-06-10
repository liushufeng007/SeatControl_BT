#if !defined (__CDDMTR_CTRL_H__)
# define __CDDMTR_CTRL_H__

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"

/*******************************************************************************
|    Compile Option or configuration Section (for test/debug)
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
typedef enum
{
	CDDMTR_CTRL_REQ_STOP = 0,
	CDDMTR_HFKF_REQ_FORWARD,
	CDDMTR_HFKF_REQ_BACKWARD,
	CDDMTR_HFKF_REQ_MAXVAL
}CddMtr_Ctrl_Req_e;

typedef enum
{
	CDDMTR_CTRL_MTRL_NORMAL = 0,
	CDDMTR_CTRL_MTRL_STUCK
}CddMtr_Ctrl_MtrStcuk_e;


typedef struct
{
	CddMtr_Ctrl_Req_e Req_State;
}CddMtr_Ctrl_Str;

/*******************************************************************************
|    Global Variable with extern linkage
|******************************************************************************/


/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/
extern void CddMtr_Ctrl_Handle(uint8_t fl_Mtr_Id,CddMtr_Ctrl_Req_e fl_Ctrl_State);
extern void CddMtr_Ctrl_Init(void);
extern CddMtr_Ctrl_Req_e CddMtr_Ctrl_Get_RunDiection(uint8_t fl_Mtr_Id);
#endif

/*EOF*/
