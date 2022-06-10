#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"
    
#include "System.h"
#include "System_OsApp.h"
#include "System_OsTask.h"
#include "System_OsMessageQueue.h"



#define CREATE_OS_TASK_ATTRIBUTE(name,stackSize,priority,timerEvent,timeOut)                    \
    static const System_OsTaskAttribute_t c##name##Attribute =                      \
    {Os_Task_##name,"#name",stackSize,priority,timerEvent,timeOut}
    
#define OS_TASK_ATTRIBUTE(name)          &c##name##Attribute
//                                                   priority 
//                             Task             Stack   |   timerEvent                 timeOut
CREATE_OS_TASK_ATTRIBUTE(BSW_COMM,              256,   4,  eSYSTEM_TIMER_EVENT_5MS\
 	                                                    ,    portMAX_DELAY);
CREATE_OS_TASK_ATTRIBUTE(BSW_DIAGNOSE,          256,   3,  eSYSTEM_TIMER_EVENT_2MS|eSYSTEM_TIMER_EVENT_50MS\
 	                                                    ,  portMAX_DELAY);
CREATE_OS_TASK_ATTRIBUTE(DRIVERS_SAFE,          256,   2,  eSYSTEM_TIMER_EVENT_5MS|eSYSTEM_TIMER_EVENT_1MS\
	                                                    ,   portMAX_DELAY);
CREATE_OS_TASK_ATTRIBUTE(DRIVERS_UNSAFE, 256,          1,  eSYSTEM_TIMER_EVENT_10MS  \
	 	                                                , portMAX_DELAY);
/* Task Attribute Table */
const System_OsTaskAttribute_t * cOsTaskAttribute_t[OsTask_ID_NUMBER] =
{
    OS_TASK_ATTRIBUTE(BSW_COMM),
    OS_TASK_ATTRIBUTE(BSW_DIAGNOSE),
    OS_TASK_ATTRIBUTE(DRIVERS_SAFE),
    OS_TASK_ATTRIBUTE(DRIVERS_UNSAFE),
};

extern void System_TimerCallBack(TimerHandle_t xTimer);
/* Timer Config Table */
const System_TimerConfig_t cSystem_TimerConfig[eSYSTEM_TIMER_ID_NUMBER] =
{
    {
        eSYSTEM_TIMER_EVENT_1MS, "1ms timer", 1, System_TimerCallBack, 
    },
    {
        eSYSTEM_TIMER_EVENT_2MS, "2ms timer", 2, System_TimerCallBack, 
    },
    {
        eSYSTEM_TIMER_EVENT_5MS, "5ms timer", 5, System_TimerCallBack, 
    },
    {
        eSYSTEM_TIMER_EVENT_10MS, "10ms timer", 10, System_TimerCallBack, 
    },
	{
		eSYSTEM_TIMER_EVENT_50MS, "50ms timer", 50, System_TimerCallBack, 
	},
};



