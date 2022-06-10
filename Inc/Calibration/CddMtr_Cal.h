
#if !defined (__CDDMTR_CAL_H__)
# define __CDDMTR_CAL_H__

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "CddMtr_Ctrl.h"
#include "user_init.h"

#include  "CddMtr_Ctrl.h"
#include  "CddMtr_HFKF.h"
#include "Adcif.h"
#include "CddMtr_Mng.h"

/*******************************************************************************
|    Compile Option or configuration Section (for test/debug)
|******************************************************************************/

/*******************************************************************************
|    Macro Definition
|******************************************************************************/

/*******************************************************************************
|    const Definition
|******************************************************************************/
extern const uint16_t CddMtr_Mng_Battery_Compensate_Tbl[11][2];
extern const uint16_t CddMtr_Mng_FB_Current_Tbl[37];
extern const uint8_t CddMtr_Mng_Ad_Ch_Map[CDDMTR_HFKF_MAX_NUM][4];
extern const CddMtr_Mng_Threshold_Ad_Str CddMtr_Mng_Ad_Threshold_Tbl[CDDMTR_HFKF_MAX_NUM];

#endif

/*EOF*/
