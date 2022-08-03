#ifndef __LIN_H__
#define __LIN_H__
#include "main.h"

/* 执行时间优化相关宏定义 */
#define LIN_GPTIM2_ISR_MASK 6   /* 输入捕获通道1和2中断标志, 写1清0 CC1IF和CC2IF */
#define LIN_GPTIM2_IER_MASK 2   /* 输入捕获通道1中断使能 */

/* LIN 相关宏定义 */
#define SYSTEM_CLOCK      (64000000)
#define MAIN_CLOCK          SYSTEM_CLOCK                  /* 主频, 单位Hz */
#define TIMER_1MS_LOAD      (MAIN_CLOCK/1000)               /* 定时器1ms, Load值 */ 
#define TIMER_1US_LOAD      (MAIN_CLOCK/1000000)            /* 定时器1us, Load值 */
#define TIME_BASE_PERIOD    500                             /* 接收超时定时器, 单位us */
#define LIN_IDLE_TIMEOUT    10000                           /* TIME_BASE_PERIOD为单位,休眠周期,此时间段内无通信则进入sleep,有通信则重置超时 */

/* 校验类型 */
#define CHECKSUM_TYPE_STANDARD   0
#define CHECKSUM_TYPE_ENHANCED   1
#define CHECKSUM_TYPE            CHECKSUM_TYPE_ENHANCED     /* 0-标准型 1-增强型 */
struct UARTOpStruct_LIN
{
    uint8_t *TxBuf;
    uint8_t TxLen;
    uint8_t TxOpc;
    uint8_t RxBuf[20];
    uint8_t RxLen;
};
typedef enum {
    LIN_CMD0 = 0x17,
    LIN_CMD1 = 0x37,  
    LIN_CMD2 = 0x38,  
    /* 此处增加自定义ID */
}lin_cmd_t;

typedef enum {
    TRANSMIT,   /* 主机发送 */
    RECEIVE     /* 主机接收 */
}lin_packet_type_t;

typedef struct {
    uint8_t cmd;           /* 参考LIN_CMD0 */
    lin_packet_type_t type;/* 命令类型 */
    uint8_t length;
    uint8_t timeout;       /* 超时/ms, 可在帧时隙到达之前提前判定任务接收 */
    uint8_t period;        /* 周期/ms */
    uint8_t* data;
    uint8_t pid;           /* 转换cmd为pid, 初始化时进行 */
}lin_cmd_packet_t;

#define LIN_BREAK_BIT       13                                          /* 同步间隔, 13bit显性电平 */
#define LIN_BREAK_BYTE      0x00
#define LIN_SYN_BYTE        0x55

/* lin波特率和字节间间隔 */
#define LIN_BAUDRATE                    19200                           /* 波特率, 上限20K */
#define LIN_RECV_BYTE_TIME_SPACE        12                              /* 接收字节间间隔, 最大支持255bit */

#define LIN_SYN_BYTE_TIME_SPACE         50                              /* 收到间隔段之后 */
#define LIN_BIT_TIMER_LOAD  (MAIN_CLOCK / LIN_BAUDRATE)                 /* 比特定时器Load值, TIME时钟源24MHz */
#define RESPONSE_SPACE      (50 * LIN_BIT_TIMER_LOAD)                   /* 应答间隔, 5字节长度, 每字节10bit */

#define LIN_BREAK_DELIMITER_TIMES       2                               /* (1<<2)间隔段和间隔符长度倍数关系，不足则判定为错误 */
#define LIN_BREAK_BIT_TIMES             10                              /* 间隔段和起始位长度倍数关系, 不足则判定为错误 */
#define LIN_SYN_BIT_GAP_PERCENTAGE      19 >> 7                         /* (x/100 = y >> 7, x为期望的差距, y为计算出的值, 如x=15, 则y=19), 同步段各位与起始位差距, 超出则判定为错误 */

#define LIN_UART                    UART4
#define LIN_UART_RX_GPIO            GPIOD
#define LIN_UART_RX_PIN             FL_GPIO_PIN_0
#define LIN_UART_TX_GPIO            GPIOD
#define LIN_UART_TX_PIN             FL_GPIO_PIN_1
#define LIN_UART_TX_PIN_IDX         1
#define LIN_UART_TX_PIN_FCR_MASK    0xC                                  /* (3 << (LIN_UART_TX_PIN_IDX << 1)) 便于优化执行时间 */
#define LIN_UART_TX_PIN_FCR_OUTPUT  4                                    /* (0 << (LIN_UART_TX_PIN_IDX << 1)) */
#define LIN_UART_TX_PIN_FCR_DIGIT   8                                    /* (2 << (LIN_UART_TX_PIN_IDX << 1)) */
#define LIN_UART_RX_PIN_IDX         0
#define LIN_UART_RX_PIN_FCR_MASK    3                                    /* (3 << (LIN_UART_RX_PIN_IDX << 1)) */
#define LIN_UART_RX_PIN_FCR_INPUT   0                                    /* (0 << (LIN_UART_RX_PIN_IDX << 1)) */
#define LIN_UART_RX_PIN_FCR_DIGIT   2                                    /* (2 << (LIN_UART_RX_PIN_IDX << 1)) */
#define LIN_RX_EXTI_LINE            FL_GPIO_EXTI_LINE_12
#define LIN_RX_EXTI_LINE_PIN        FL_GPIO_EXTI_LINE_12_PD0
#define LIN_RX_EXTI_LINE_INDEX      12
#define LIN_RX_EXTI_EDS_INDEX       24
#define LIN_UART_IRQHandler         UART4_IRQHandler
#define LIN_UART_IRQType            UART4_IRQn

#define LIN_SLP_GPIO        GPIOA
#define LIN_SLP_PIN         FL_GPIO_PIN_4
#define LIN_SLP_ENABLE()    FL_GPIO_ResetOutputPin(LIN_SLP_GPIO, LIN_SLP_PIN)
#define LIN_SLP_DISABLE()   FL_GPIO_SetOutputPin(LIN_SLP_GPIO, LIN_SLP_PIN)

/* 用户处理 */
#define LIN_CMD0_LENGTH     3
#define LIN_CMD0_TIMEOUT    20
#define LIN_CMD0_PERIOD     100 /* 最大255ms */

#define LIN_CMD1_LENGTH     4
#define LIN_CMD1_TIMEOUT    30
#define LIN_CMD1_PERIOD     200 /* 最大255ms */

#define LIN_CMD2_LENGTH     8
#define LIN_CMD2_TIMEOUT    40
#define LIN_CMD2_PERIOD     200 /* 最大255ms */

extern uint8_t LIN_CMD0_Data[8];
extern uint8_t LIN_CMD1_Data[8];
extern uint8_t LIN_CMD2_Data[8];
extern lin_cmd_packet_t scheduleTable[];
#define TABLE_SIZE  (sizeof(scheduleTable)/sizeof(lin_cmd_packet_t))

void lin_init(void);

#if (SLEEP_SUPPORT == 1)
void lin_sleep_init(void);

void lin_awake_init(void);

uint8_t lin_is_sleep(void);
#endif

#endif
