#ifndef __POWERMODE_H__
#define __POWERMODE_H__
#include "public.h"
#include "user_init.h"


extern void PowerModeInit(void);
extern void PowerModeHandler(void);
extern uint8_t Get_DiagVoltage_Flag(void); /*Diagnostic voltage flag*/
extern uint8_t GetCANWorking_Voltage_Flag(void); /* CAN working voltage flag */
extern uint8_t Get_Voltage_Flag(void); /* abnormal voltage flag*/
extern uint32_t Get_VoltVaule_Fast(void);   /* Fast get voltage value(mV) */
extern void Scm_PowerDown(void);  /* Sleep Mode */
extern void Clean_Receive_Message_Flag(void); /* Judge received CAN message*/
extern void Set_Receive_Message_Flag(void); /* Received CAN message */

/* Fast sample function(5ms) */
extern void PowerModeFastSample_5ms(void);

#endif
