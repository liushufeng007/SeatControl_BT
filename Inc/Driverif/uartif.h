#ifndef __UARTIF_H__
#define __UARTIF_H__

#include "user_init.h"

#define UARTIF_USE_CHANNEL ((UART_Type *)UART0_BASE)  

#define UARTIF_TASK_PERIOD          1  /*unit:ms*/

#define UARTIF_FRAME_COMPLETE_TIME           (5/UARTIF_TASK_PERIOD)

#define UARTIF_DELAY_50MS           (5/UARTIF_TASK_PERIOD)

#define UARTIF_MAX_TXRX_SIZE          30

typedef struct
{
    uint8_t TxOpc;  //已发送数据长度
    uint8_t TxBuf[UARTIF_MAX_TXRX_SIZE];
	uint8_t TxLen;
	
    uint8_t RxLen;  //待接收数据长度
    uint8_t RxOpc;  //已接收数据长度
    uint8_t RxPool[UARTIF_MAX_TXRX_SIZE];
	uint16_t Rxframeticks;
}UARTOpStruct;


typedef struct
{
    uint8_t DLC;     /*!< Specifies the length of the frame that will be received.
                        This parameter can be a value between 0 to 8 */
    uint8_t Data[UARTIF_MAX_TXRX_SIZE]; /*!< Contains the data to be received. It ranges from 0 to
                        0xFF. */
} Uartif_Msg_Str;



typedef struct
{
	Uartif_Msg_Str queue[UARTIF_MAX_TXRX_SIZE];
	uint8_t queue_in;
	uint8_t queue_out;
}Uartif_queue_str;





extern void Uart_Init(void);
extern void Uarif_Task(void);
extern uint8_t Uartif_tx_queue_push_e(Uartif_Msg_Str fl_str_e);
extern uint8_t Uartif_rx_queue_pull_e(Uartif_Msg_Str *fl_str_e);


#endif
