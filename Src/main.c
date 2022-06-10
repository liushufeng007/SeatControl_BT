#include "main.h"
#include "user_init.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Tickless_Hook.h" 


#include "timers.h"
#include "event_groups.h"

#include "System_OsMessageQueue.h"
#include "System_OsTask.h"



extern void DeepSleep(void);
extern void CddModeM_DeInit(void);

void SystemStar(void)
{
    System_OsMessageQueueInit();
    System_StarOsTask();
}


int main(void)
{

    UserInit();

	SystemStar();

 
    while(1)
    {

    }
}


