#include "user_init.h"
#include "fm33lg0xx_fl.h"
#include "fm33lg0xx_fl_iwdt.h"



void IWDT_Init(void)
{
    FL_IWDT_InitTypeDef IWDT_InitStruct;
    /* Ĭ�ϲ�ʹ�ô��� */
    IWDT_InitStruct.iwdtWindows    = 0;
    /*����ʱ��*/
    IWDT_InitStruct.overflowPeriod = FL_IWDT_PERIOD_250MS;

    FL_IWDT_Init(IWDT, &IWDT_InitStruct);
	FL_IWDT_EnableFreezeWhileSleep(IWDT);
}


void IWDT_Clr(void)
{
    FL_IWDT_ReloadCounter(IWDT);
}

void IWDT_DeInit(void)
{
	FL_IWDT_DeInit(IWDT);
}


