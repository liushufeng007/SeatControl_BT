#ifndef __CANIF_H__
#define __CANIF_H__
#include "public.h"

  


typedef struct
{
    uint32_t StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

    uint32_t ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

    uint8_t DLC;     /*!< Specifies the length of the frame that will be received.
                        This parameter can be a value between 0 to 8 */

    uint8_t Data[8]; /*!< Contains the data to be received. It ranges from 0 to
                        0xFF. */
} Canif_Msg_Str;



typedef struct
{
	Canif_Msg_Str queue[20];
	uint8_t queue_in;
	uint8_t queue_out;
}Canif_queue_str;



extern uint8_t Canif_rx_queue_pull_e(Canif_Msg_Str *fl_str_e);
extern uint8_t Canif_tx_queue_push_e(Canif_Msg_Str fl_str_e);
void Canif_Init(void);
void Canif_DeInit(void);
void Canif_task(void);


#endif
