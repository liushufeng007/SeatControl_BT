#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

#include "System.h"
#include "System_OsApp.h"
#include "System_OsTask.h"
#include "System_OsMessageQueue.h"

#include "fm33lg0xx_fl_atim.h"



extern void *pvTimerGetTimerID( const TimerHandle_t xTimer );

typedef struct 
{
    TimerHandle_t   xTimer;
    StaticTimer_t   pxTimerBuffer;
} System_TimerInfo_t;


static void System_TimerCreate(void);
static void TaskContainer(void * arg);


extern const System_OsTaskAttribute_t * cOsTaskAttribute_t[OsTask_ID_NUMBER];
extern const System_TimerConfig_t cSystem_TimerConfig[eSYSTEM_TIMER_ID_NUMBER];

/* Define Timer Handle */
static System_TimerInfo_t tSystem_TimerInfo[eSYSTEM_TIMER_ID_NUMBER];
static System_TaskMsg_t u16System_TaskMessage[OsTask_ID_NUMBER];

uint32_t cpuload_total = 0 ;
uint32_t cpuload_Current_stamp  = 0;
uint32_t cpuload_Last_stamp = 0 ;
uint32_t cpuload_reset_counter = 0;
uint32_t cpuload_percent = 0;
uint32_t cpuload_min_percent = 0xFFFFFFFF;
uint32_t cpuload_max_percent = 0;
uint32_t cpuload_min_stamp = 0xffffffff;

const uint32_t cpuload_threshold = 7;//3.5us
const uint32_t cpuload_ResetThreshold = 33;//1s

void ATIM_Init(void)
{
    FL_ATIM_InitTypeDef        InitStructer;

    InitStructer.clockSource           = FL_CMU_ATIM_CLK_SOURCE_APBCLK;  /* 时钟源选择APB2 */
    InitStructer.prescaler             = 31;                              /* 分频系数8 */
    InitStructer.counterMode           = FL_ATIM_COUNTER_DIR_DOWN;         /* 向上计数 */
    InitStructer.autoReload            = 0xFFFF;                            /* 自动重装载值1000 */
    InitStructer.clockDivision         = FL_ATIM_CLK_DIVISION_DIV1;      /* 死区和滤波设置 */
    InitStructer.repetitionCounter     = 0;                              /* 重复计数 */
    InitStructer.autoReloadState       = FL_ENABLE;                     /* 自动重装载禁止preload */
    FL_ATIM_Init(ATIM, &InitStructer);

    //NVIC_DisableIRQ(ATIM_IRQn);
   // NVIC_SetPriority(ATIM_IRQn, 2); /* 中断优先级配置 */
   // NVIC_EnableIRQ(ATIM_IRQn);

    //FL_ATIM_ClearFlag_Update(ATIM); /* 清除计数器中断标志位 */
   // FL_ATIM_EnableIT_Update(ATIM);  /* 开启计数器中断 */

    FL_ATIM_Enable(ATIM);           /* 使能定时器 */

}


void CpuLoad_Task( void *pvParameters )
{
    ATIM_Init();
	for( ;; )
	{
		__disable_irq();
		cpuload_Current_stamp = ATIM->CNT;//SysTick->VAL;
		if(cpuload_Current_stamp<=cpuload_Last_stamp)
		{
			if(cpuload_Last_stamp - cpuload_Current_stamp > cpuload_threshold)//3.5us
			//64000=1ms
			{
				cpuload_total =( cpuload_total - cpuload_Current_stamp +cpuload_Last_stamp)/1;
			}
			if(cpuload_min_stamp > cpuload_Last_stamp - cpuload_Current_stamp )
				cpuload_min_stamp= cpuload_Last_stamp - cpuload_Current_stamp;
		}
		else
		{
			if(0xFFFF - cpuload_Current_stamp + cpuload_Last_stamp > cpuload_threshold)//10us
			//64000=1ms
			{
				cpuload_total = (cpuload_total + 0xFFFF - cpuload_Current_stamp + cpuload_Last_stamp)/1;
			}
			if(cpuload_min_stamp > ( 0xFFFF - cpuload_Current_stamp +cpuload_Last_stamp))
			cpuload_min_stamp= 0xFFFF - cpuload_Current_stamp + cpuload_Last_stamp;
			
			if(cpuload_reset_counter > cpuload_ResetThreshold)
			{
				cpuload_reset_counter = 0;
				cpuload_percent = cpuload_total  / 21626;
				cpuload_total = 0;
				if(cpuload_max_percent < cpuload_percent)
				{
					cpuload_max_percent = cpuload_percent;
				}
				if(cpuload_min_percent > cpuload_percent)
				{
					cpuload_min_percent = cpuload_percent;
				}
			}
			else
			{
				cpuload_reset_counter++;
			}
		}
		cpuload_Last_stamp =cpuload_Current_stamp;
		__enable_irq();

	}
} 


void System_StarOsTask(void)
{
    INT32 i;
	

    System_TimerCreate();

    for (i = 0; i < OsTask_ID_NUMBER; i++)
    {
        xTaskCreate(TaskContainer, 
            cOsTaskAttribute_t[i]->pcName, 
            cOsTaskAttribute_t[i]->ulStackDepth, 
            (void *) i, 
            cOsTaskAttribute_t[i]->uxPriority, 
            NULL);
    }
	
    xTaskCreate(CpuLoad_Task,"cpuload",100,(void *)i,0,NULL);
 
    vTaskStartScheduler();
}


static void System_TimerCreate(void)
{
    INT32 i;

    for (i = 0; i < eSYSTEM_TIMER_ID_NUMBER; i++)
    {
        tSystem_TimerInfo[i].xTimer = xTimerCreate(cSystem_TimerConfig[i].pcTimerName, 
            cSystem_TimerConfig[i].xTimerPeriodInTicks, 
            pdTRUE, 
            (void *) cSystem_TimerConfig[i].pvTimerID, 
            cSystem_TimerConfig[i].pxCallbackFunction);
        xTimerStart(tSystem_TimerInfo[i].xTimer, 10);

    }
}


void System_TimerCallBack(TimerHandle_t xTimer)
{
    INT32 i;
    System_TaskMsg_t tMsg;
    uint16_t tTimerMask;

    tTimerMask = (uint32_t)pvTimerGetTimerID(xTimer);

    for (i = 0; i < OsTask_ID_NUMBER; i++)
    {
        if (tTimerMask & cOsTaskAttribute_t[i]->TimerMask)
        {
            tMsg.u16Event = SYSTEM_OS_EVENT_TIMER;
            tMsg.u16Message = tTimerMask;
            System_SendMessage((OsTask_ID_t) i, &tMsg);
        }
    }

}


static void TaskContainer(void * arg)
{
    INT32 pTaskId = (uint32_t)arg;

    System_TaskMsg_t tMsg ;

    if (pTaskId < OsTask_ID_NUMBER)
    {
        while (1)
        {
            if (System_ReceiveMessage(pTaskId, &tMsg, cOsTaskAttribute_t[pTaskId]->xTicksToWait) ==
                 eSystem_Return_Success)
            {
                if (NULL_PTR != cOsTaskAttribute_t[pTaskId]->pxTaskCode)
                {
                    u16System_TaskMessage[pTaskId].u16Message = tMsg.u16Message;
                    u16System_TaskMessage[pTaskId].u16Event= tMsg.u16Event;
                    cOsTaskAttribute_t[pTaskId]->pxTaskCode(tMsg.u16Event, tMsg.u16Message);
                }
            }
            else 
            {
                if (NULL_PTR != cOsTaskAttribute_t[pTaskId]->pxTaskCode)
                {
                    u16System_TaskMessage[pTaskId].u16Message = 0;
                    u16System_TaskMessage[pTaskId].u16Event = 0;
                    cOsTaskAttribute_t[pTaskId]->pxTaskCode(0, 0);
                }
            }
        }
    }
}

System_Return_t System_OsTask_GetTaskMask(OsTask_ID_t tTaskId,uint16_t * u16Event_ptr,uint16_t * u16Message_ptr)
{
    System_Return_t rte = eSystem_Return_Error;
    
    if (tTaskId >= OsTask_ID_NUMBER)
    {
        rte = eSystem_Return_E_PARAM;
    }
    else if ((NULL_PTR == u16Event_ptr)||(NULL_PTR == u16Message_ptr))
    {
        rte = eSystem_Return_E_PARAM;
    }
    else
    {
        *u16Event_ptr = u16System_TaskMessage[tTaskId].u16Event;
        *u16Message_ptr = u16System_TaskMessage[tTaskId].u16Message;
        
        rte = eSystem_Return_Success;
    }

    return rte;
}

#if (                                       configSUPPORT_STATIC_ALLOCATION == 1)
void vApplicationGetIdleTaskMemory(StaticTask_t * *ppxIdleTaskTCBBuffer, StackType_t * *ppxIdleTaskStackBuffer, uint32_t * pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &IdleTaskTCB;
    *ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


#endif

#if (                                       configSUPPORT_STATIC_ALLOCATION == 1)

/* If static allocation is supported then the application must provide the
    following callback function - which enables the application to optionally
    provide the memory that will be used by the timer task as the task's stack
    and TCB. */
void vApplicationGetTimerTaskMemory(StaticTask_t * *ppxTimerTaskTCBBuffer, StackType_t * *ppxTimerTaskStackBuffer, uint32_t * pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &TimerTaskTCB;
    *ppxTimerTaskStackBuffer = TimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}


#endif


