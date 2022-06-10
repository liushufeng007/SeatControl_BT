#if !defined (__CDDMTR_HFKF_H__)
# define __CDDMTR_HFKF_H__

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"
#include "Ioif.h"
#include "fm33_assert.h"
/*******************************************************************************
|    Compile Option or configuration Section (for test/debug)
|******************************************************************************/

/*******************************************************************************
|    Macro Definition
|******************************************************************************/

#define CDDMTR_HFKF_CTRL_P_HIGH_N_LOW(ch)   \
	assert_param(ch < CDDMTR_HFKF_MAX_NUM);  \
	Ioif_SetPinLevel(CddMtr_HFKF_CH_Table[ch].PIN_P,TRUE);\
	Ioif_SetPinLevel(CddMtr_HFKF_CH_Table[ch].PIN_N,FALSE);

#define CDDMTR_HFKF_CTRL_P_LOW_N_HIGH(ch) \
	assert_param(ch < CDDMTR_HFKF_MAX_NUM);  \
	Ioif_SetPinLevel(CddMtr_HFKF_CH_Table[ch].PIN_P,FALSE);\
	Ioif_SetPinLevel(CddMtr_HFKF_CH_Table[ch].PIN_N,TRUE);

#define CDDMTR_HFKF_CTRL_P_HIGH_N_HIGH(ch) \
	assert_param(ch < CDDMTR_HFKF_MAX_NUM);  \
	Ioif_SetPinLevel(CddMtr_HFKF_CH_Table[ch].PIN_P,TRUE);\
	Ioif_SetPinLevel(CddMtr_HFKF_CH_Table[ch].PIN_N,TRUE);

#define CDDMTR_HFKF_CTRL_P_LOW_N_LOW(ch)  \
	assert_param(ch < CDDMTR_HFKF_MAX_NUM);  \
	Ioif_SetPinLevel(CddMtr_HFKF_CH_Table[ch].PIN_P,FALSE);\
	Ioif_SetPinLevel(CddMtr_HFKF_CH_Table[ch].PIN_N,FALSE);



/*******************************************************************************
|    Enum Definition
|******************************************************************************/

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/
typedef enum
{
	CDDMTR_HFKF_CH_0 = 0,
	CDDMTR_HFKF_CH_1,
	CDDMTR_HFKF_CH_2,
	CDDMTR_HFKF_CH_3,
	CDDMTR_HFKF_MAX_NUM
}CddMtr_HFKF_e;

typedef struct
{
	Ioif_Gpio_Num_e PIN_P;
	Ioif_Gpio_Num_e PIN_N;
}CddMtr_HFKF_Ctrl;
/*******************************************************************************
|    Global Variable with extern linkage
|******************************************************************************/
extern const CddMtr_HFKF_Ctrl CddMtr_HFKF_CH_Table[CDDMTR_HFKF_MAX_NUM]; 

/*******************************************************************************
|    Global Function Prototypes
|******************************************************************************/


#endif

/*EOF*/
