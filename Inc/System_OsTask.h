
#ifndef _SYSTEM_OSTASK_H_
#define _SYSTEM_OSTASK_H_
#include "System_OsMessageQueue.h"

/* define Os Task Id; */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

typedef uint8_t OsTask_ID_t;
#define BSW_COMM                            (0u)
#define BSW_DIAGNOSE                        (1u)
#define DRIVERS_SAFE                        (2u)
#define DRIVERS_UNSAFE                      (3u)
#define OsTask_ID_NUMBER                    (4u)

#define SYSTEM_OS_EVENT_TIMER               (0x0100U)
#define SYSTEM_OS_EVENT_EEPROM_REQ          (0x0001U)
#define SYSTEM_OS_EVENT_IIC1_REQ            (0x0002U)
#define SYSTEM_OS_EVENT_RTC_REQ             (0x0003U)
#define SYSTEM_OS_EVENT_GYRO_REQ            (0x0004U)

#define SYSTEM_OS_IPC_EVENT_IPC_REQ         (0x0001U)



typedef uint16_t System_TimerEvent_t;
#define eSYSTEM_TIMER_EVENT_5MS             (0x01u)
#define eSYSTEM_TIMER_EVENT_10MS            (0x02u)
#define eSYSTEM_TIMER_EVENT_2MS             (0x04u)
#define eSYSTEM_TIMER_EVENT_1MS             (0x80u)
#define eSYSTEM_TIMER_EVENT_50MS             (0x08u)

typedef uint8_t System_TimerId_t;
#define eSYSTEM_TIMER_ID_1ms                (0u)
#define eSYSTEM_TIMER_ID_2ms                (1u)
#define eSYSTEM_TIMER_ID_5ms                (2u)
#define eSYSTEM_TIMER_ID_10ms               (3u)
#define eSYSTEM_TIMER_ID_50ms               (4u)
#define eSYSTEM_TIMER_ID_NUMBER             (5u)

typedef void(*OsTaskFunction) (uint16_t u16Event, uint16_t u16Message);


typedef struct 
{
    OsTaskFunction  pxTaskCode;
    const char * const pcName;
    const uint32_t  ulStackDepth;
    UBaseType_t     uxPriority;
    System_TimerEvent_t TimerMask;
    uint32_t          xTicksToWait;
} System_OsTaskAttribute_t;

typedef struct 
{
    System_TimerId_t pvTimerID;
    const char *    pcTimerName;
    TickType_t      xTimerPeriodInTicks;
    TimerCallbackFunction_t pxCallbackFunction;
} System_TimerConfig_t;



void System_StarOsTask(void);
System_Return_t System_OsTask_GetTaskMask(OsTask_ID_t tTaskId,uint16_t * u16Event_ptr,uint16_t * u16Message_ptr);


#endif /* ifndef _SYSTEM_OSTASK_H_.2020-4-14 10:13:06 GW00188879 */

