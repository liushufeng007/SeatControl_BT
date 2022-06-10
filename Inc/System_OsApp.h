#ifndef _SYSTEM_OSAPP_H_
#define _SYSTEM_OSAPP_H_


#include "System.h"

#define TASK(TaskName)       void Os_Task_##TaskName(uint16_t u16Event, uint16_t u16Msg)

#define SYSTEM_TEST

TASK(BSW_COMM);
TASK(BSW_DIAGNOSE);
TASK(DRIVERS_SAFE);
TASK(DRIVERS_UNSAFE);



#endif /* ifndef _SYSTEM_OSAPP_H_.2020-4-14 10:12:50 GW00188879 */

