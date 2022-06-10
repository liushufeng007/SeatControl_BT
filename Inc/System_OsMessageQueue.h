
#ifndef _SYSTEM_OSMESSAGEQUEUE_H_
#define _SYSTEM_OSMESSAGEQUEUE_H_
#include "System.h"

typedef uint8_t OsTask_ID_t;
typedef INT8 System_Return_t;

#ifndef NULL_PTR
#define NULL_PTR                                   ((void *) 0)
#endif


#define eSystem_Return_Success              ((INT8)0)
#define eSystem_Return_Error                ((INT8)-1)
#define eSystem_Return_E_PARAM              ((INT8)-2)
#define eSystem_Return_E_TIMEOUT            ((INT8)-3)


typedef struct
{
    uint16_t  u16Event;
    uint16_t  u16Message;

}System_TaskMsg_t;

void System_OsMessageQueueInit(void);
System_Return_t System_SendMessage(const OsTask_ID_t eTask,const System_TaskMsg_t *pData);
System_Return_t System_SendMessageIRQ(const OsTask_ID_t eTask,const System_TaskMsg_t *pData);
System_Return_t System_ReceiveMessage(const OsTask_ID_t eTask, System_TaskMsg_t *pData, uint32_t xTicksToWait);


#endif /* ifndef _SYSTEM_OSMESSAGEQUEUE_H_.2020-4-15 17:11:01 GW00188879 */

