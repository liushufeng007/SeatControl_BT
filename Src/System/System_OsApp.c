/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"


#include "System.h"
#include "System_OsApp.h"
#include "System_OsMessageQueue.h"
#include "System_OsTask.h"


#include "Ioif.h"
#include "Adcif.h"
#include "Canif.h"
#include "Eepromif.h"
#include "CddKey.h"
#include "CddTest.h"
#include "CddEeprom.h"
#include "CddModeM.h"


#include "vnim_net.h"
#include "Iwdt.h"
#include "CddMtr_Mng.h"
#include "rx_can.h"
#include "diag_kwp2k_di_core_mgr.h"
#include "PowerMgmt.h"

#include "Scm_SeatControl.h"

#include "CanNm.h"
#include "il_par.h"
#include "CanNm_UserData.h"

#include "uartif.h"

#include "CddBT616.h"

#include "ButtonCtrl.h"
#include "CddLed.h"

#include "fm33lg0xx_fl_atim.h"


#define CHECK_OS_EVENT(event)               (event) == u16Event
#define CHECK_OS_MSG(msg)                   (msg) == u16Msg


volatile uint32_t cpuload_Current_stamp_A  = 0;
volatile uint32_t cpuload_Last_stamp_A = 0 ;


#ifdef CPU_LOAD_OPEN
char    CPU_RunInfo[400];
#endif

extern void fdiag_Robin(void);
extern void can_send_test(void);
/***********************************************************************
 * @brief       : 
 * @param       : 
 * @retval      : 
 ***********************************************************************/
TASK(BSW_COMM)
{
    if (CHECK_OS_EVENT(SYSTEM_OS_EVENT_TIMER))
    {
        if (CHECK_OS_MSG(eSYSTEM_TIMER_EVENT_5MS))
        {
			__disable_irq();
            cpuload_Last_stamp_A = cpuload_Current_stamp_A ;
			cpuload_Current_stamp_A  = ATIM->CNT;
			__enable_irq();
            IWDT_Clr();
            NW_Task();
			Uarif_Task();
			

        }

#ifdef CPU_LOAD_OPEN
		memset(CPU_RunInfo,0,400);
		vTaskList(CPU_RunInfo);
		memset(CPU_RunInfo,0,400);
		vTaskGetRunTimeStats(CPU_RunInfo);
		vTaskDelay(10000);
#endif 

    }
}


/***********************************************************************
 * @brief       : 
 * @param       : 
 * @retval      : 
 ***********************************************************************/
TASK(BSW_DIAGNOSE)
{

    if (CHECK_OS_EVENT(SYSTEM_OS_EVENT_TIMER))
    {
        if (CHECK_OS_MSG(eSYSTEM_TIMER_EVENT_50MS))
        {
               // fdiag_Robin();
               // DiagMain();
        	ButtonCtrl_50ms_Task();
        }
		if (CHECK_OS_MSG(eSYSTEM_TIMER_EVENT_5MS))
        {
 
        }
    }
}


/***********************************************************************
 * @brief       : 
 * @param       : 
 * @retval      : 
 ***********************************************************************/
TASK(DRIVERS_SAFE)
{
    if (CHECK_OS_EVENT(SYSTEM_OS_EVENT_TIMER))
    {
        if (CHECK_OS_MSG(eSYSTEM_TIMER_EVENT_5MS))
        {
			/*CddKey_5ms_Task();*/
			CddMtr_Mng_Task();
			CddEeprom_Task();            
            //PowerModeFastSample_5ms();
			CddModeM_Task();
			CddBT616_Task();
			
			CddLed_5ms_Task();
        }
        if (CHECK_OS_MSG(eSYSTEM_TIMER_EVENT_1MS))
        {
			Canif_task();
			
        }
    }
	
		if (CHECK_OS_EVENT(SYSTEM_OS_EVENT_EEPROM_REQ))
		{
			CddEeprom_Task();		
			
		}

}


/***********************************************************************
 * @brief       : 
 * @param       : 
 * @retval      : 
 ***********************************************************************/
TASK(DRIVERS_UNSAFE)
{
    if (CHECK_OS_EVENT(SYSTEM_OS_EVENT_TIMER))
    {
        if (CHECK_OS_MSG(eSYSTEM_TIMER_EVENT_10MS))
        {
			CddTest_Task();
            tcan_Task();
            //PowerModeHandler();
            //Scm_SeatControl_App();
            //Scm_PowerDown();
			//CanNm_UserData_Fun();
        }
    }
}



void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	/* This will be calSample if a task overflows its stack.  pxCurrentTCB
	can be inspected to see which is the offending task. */
	for( ;; );
}

void vApplicationMallocFailedHook(void)
{
    while(1)
    {
    }
}

void vApplicationTickHook( void )
{

}

void vApplicationIdleHook( void )
{

}


