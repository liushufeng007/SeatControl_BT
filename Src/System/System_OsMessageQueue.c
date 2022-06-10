#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"
#include "queue.h"
 
#include "System.h"
#include "System_OsMessageQueue.h"
#include "System_OsTask.h"


#define MSG_DATA_MAX_LEN        (sizeof(System_TaskMsg_t))
#define QUEUE_MAX_LEN           (10U)



typedef struct
{
    QueueHandle_t xQueue;
}MessageQueueHandler_t;

static MessageQueueHandler_t tMessageQueueHandler[OsTask_ID_NUMBER];

void System_OsMessageQueueInit(void)
{ 
  	uint16_t i;

    for (i=0; i< OsTask_ID_NUMBER; i++)
    {
        tMessageQueueHandler[i].xQueue =  xQueueCreate(QUEUE_MAX_LEN, MSG_DATA_MAX_LEN );
    }
}
/***********************************************************************
 * @brief       : 
 * @param       : 
 * @retval      : 
 ***********************************************************************/
System_Return_t System_SendMessage(const OsTask_ID_t eTask,const System_TaskMsg_t *pData)
{
    System_Return_t ret = eSystem_Return_Error;

    MessageQueueHandler_t * msg_prt;
    
    if (eTask >= OsTask_ID_NUMBER)
    {
        ret = eSystem_Return_E_PARAM;
    }
    else if (NULL_PTR == pData)
    {
        ret = eSystem_Return_E_PARAM;
    }
    else
    {
        msg_prt = &tMessageQueueHandler[eTask];
        if (pdPASS != xQueueSendToBack(msg_prt->xQueue, pData, 0))
        {
            ret = eSystem_Return_Error;
        }
        else
        {
            ret = eSystem_Return_Success;
        }
        
    }
    return ret;
}

/***********************************************************************
 * @brief       : 
 * @param       : 
 * @retval      : 
 ***********************************************************************/
System_Return_t System_SendMessageIRQ(const OsTask_ID_t eTask,const System_TaskMsg_t *pData)
{
    System_Return_t ret = eSystem_Return_Error;

    MessageQueueHandler_t * msg_prt;
    BaseType_t HigherPriority = pdFALSE;
    
    
    if (eTask >= OsTask_ID_NUMBER)
    {
        ret = eSystem_Return_E_PARAM;
    }
    else if (NULL_PTR == pData)
    {
        ret = eSystem_Return_E_PARAM;
    }
    else
    {
        msg_prt = &tMessageQueueHandler[eTask];
        if (pdPASS != xQueueSendToBackFromISR(msg_prt->xQueue, pData, &HigherPriority))
        {
            ret = eSystem_Return_Error;
        }
        else
        {
            ret = eSystem_Return_Success;
        }
        
    }
    return ret;
}


/***********************************************************************
 * @brief       : 
 * @param       : 
 * @retval      : 
 ***********************************************************************/
System_Return_t System_ReceiveMessage(const OsTask_ID_t eTask, System_TaskMsg_t *pData, uint32_t xTicksToWait)
{
    System_Return_t ret = eSystem_Return_Error;
    MessageQueueHandler_t * msg_prt;
    
    if (eTask >= OsTask_ID_NUMBER)
    {
        ret = eSystem_Return_E_PARAM;
    }
    else if (NULL_PTR == pData)
    {
        ret = eSystem_Return_E_PARAM;
    }
    else
    {
         msg_prt = &tMessageQueueHandler[eTask];
         if (pdPASS != xQueueReceive(msg_prt->xQueue,pData,xTicksToWait))
         {
            ret = eSystem_Return_E_TIMEOUT;
         }
         else
         {
            ret = eSystem_Return_Success;
         }
    }
    return ret;
}


