#include "main.h"
#include "user_init.h"
#include "lin.h"
#include "uartif.h"
#include "user_init.h"
#include "fm33lg0xx_fl.h"
#include "CddMtr_Mng.h"
#include "CddMtr_HFKF.h"
#include "Adcif.h"


#define FRAME_DATA_MAX_LEN          11  /* 帧最大数据长度 */
#define SCHEDULE_FRAME_MAX_NUM      25 /* 进度表最大帧数量 */

#define FRAME_TYPE_UNCONDITIONAL    0   /* 无条件帧 */
#define FRAME_TYPE_EVENT_TRIGGER    1   /* 事件触发帧 */
#define FRAME_TYPE_SPORADIC         2   /* 偶发帧 */
#define FRAME_TYPE_DIAGNOSTIC       3   /* 诊断帧 */
#define FRAME_TYPE_RESERVED         4   /* 保留帧 */
#define FRAME_TYPE_NO_RESPONSE      5   /* 自定义，主机发送无需回应帧 */

//帧处理状态机
#define FRAME_STATE_IDLE            0   /* 空闲状态 主：等待任务开始 从：固定波特率：等待间隔段 自动波特率：等待间隔段+同步段 */
#define FRAME_STATE_SEND_SYN        1   /* 主机发送SYN(包括间隔段和同步段) */
#define FRAME_STATE_RECV_SYN        2   /* 固定波特率：接收同步段0x55 */
#define FRAME_STATE_RECV_PID        3   /* 接收PID */
#define FRAME_STATE_RECV_DATA       4   /* 接收数据 */
#define FRAME_STATE_SEND            5   /* 串口发送 */
#define FRAME_STATE_SLEEP           6   /* 休眠状态 */

/**
  * @brief  计算PID
  * @param  id: 输入id
  * @retval 计算后的PID 
  */
uint8_t calculatePID(uint8_t id)
{
    uint8_t P0 = 0, P1 = 0;

    P0 = ((id) ^ (id >> 1) ^ (id >> 2) ^ (id >> 4)) & 0x01;
    P1 = (~((id >> 1) ^ (id >> 3) ^ (id >> 4) ^ (id >> 5))) & 0x01;

    return ((P1 << 7) | (P0 << 6) | id);
}

/**
  * @brief  计算校验和
  * @param  data: 数据指针
  *         len:  数据长度
  * @retval checkSum: 校验和 
  */
uint8_t calculateChecksum(uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint16_t checkSum = 0;

    for (i = 0; i < len; i++)
    {
        checkSum += data[i];
        if (checkSum >= 0x100)
        {
            checkSum -= 0xFF;
        }
    }
    checkSum = (~checkSum) & 0xFF;

    return checkSum;
}

/**
  * @brief  LIN slp初始化
  * @param  void
  * @retval void
  */
void lin_slp_pin_init(void)
{
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LIN_SLP_DISABLE();

    GPIO_InitStruct.pin = LIN_SLP_PIN;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(LIN_SLP_GPIO, &GPIO_InitStruct);
}

#define Min(a, b) (((a)<(b))?(a):(b))
#define lin_calc_max_res_timeout_cnt(bitLength, size) (uint16_t)((14*(1+(size))*((bitLength)/TIMER_1US_LOAD))/TIME_BASE_PERIOD + 3 )

/* 进度表帧格式 [最大11字节] */
/* 同步段0x55-1字节；受保护ID-1字节; 数据段-0-8字节；校验和-1字节 */
typedef struct s_lin_frame_type
{
    uint16_t frame_slot;           /* 帧时隙，LIN子网时基的整数倍 */
    uint16_t frame_left_slot;      /* 剩余slot */
    uint8_t frame_type;            /* 帧类型 */
    uint8_t timeout;               /* 应答超时 0-无应答超时 其余-ms */
    uint8_t tx_len;                /* 发送长度，至少包括2字节 */
    uint8_t rx_len;                /* 接收长度 */
    uint8_t data[FRAME_DATA_MAX_LEN];
}lin_schedule_data_type;

/* 进度表，最大支持255帧 */
typedef struct s_lin_schedule_type
{
    uint8_t frame_num;              /* 帧数目 */
    lin_schedule_data_type lin_schedule_data[SCHEDULE_FRAME_MAX_NUM];
}lin_schedule_type;

/* 任务处理：循环发送 */
typedef struct s_lin_task_type
{
    uint8_t frame_index;            /* 当前进度表帧索引值 */
    uint8_t frame_state;            /* 帧处理状态机，参照 FRAME_STATE_INIT 定义 */
    
    /* 帧超时计数，接收到同步段后置起，按照8字节数据计算超时 */
    /* 公式：round((1.4x(10+8x10)xTbit)/Tbase_period) + 3，8为接收字节数，Tbit为单个bit用时，Tbase_period为定时器周期，单位都是us */
    uint16_t frame_timeout_cnt;
    /* 响应超时计数，接收到需要响应的ID后置起，按照实际接收长度计算超时 */
    /* 公式：round((1.4x(10+Nx10)xTbit)/Tbase_period) + 3，N为接收字节数，Tbit为单个bit用时，Tbase_period为定时器周期，单位都是us */
    uint16_t res_frame_timeout_cnt;    
    lin_schedule_type lin_schedule; /* 进度表 */
}lin_task_type;

uint16_t schedule_table_slot[10];
lin_task_type g_lin_task;

_Seat_msgType_Fan_buf LIN_CMD0_Data;
_Seat_msgType_Fan_buf LIN_CMD1_Data;
_Seat_msgType_buf LIN_CMD2_Data;
_Seat_msgType_L_Status_buf LIN_CMD3_Data;
_Seat_msgType_R_Status_buf LIN_CMD4_Data;
_Seat_msgType_Fan_buf LIN_CMD5_Data;
_Seat_msgType_Fan_buf LIN_CMD6_Data;


uint8_t ticks = 0;
lin_cmd_packet_t scheduleTable[] = {
  /* 用户命令码 */
  {LIN_CMD0, TRANSMIT, LIN_CMD0_LENGTH, LIN_CMD0_TIMEOUT, LIN_CMD0_PERIOD, LIN_CMD0_Data.byte},
  {LIN_CMD1, TRANSMIT, LIN_CMD1_LENGTH, LIN_CMD1_TIMEOUT, LIN_CMD1_PERIOD, LIN_CMD1_Data.byte},
  {LIN_CMD2, TRANSMIT, LIN_CMD2_LENGTH, LIN_CMD2_TIMEOUT, LIN_CMD2_PERIOD, LIN_CMD2_Data.byte},
  {LIN_CMD3, RECEIVE,  LIN_CMD2_LENGTH, LIN_CMD0_TIMEOUT, LIN_CMD0_PERIOD, LIN_CMD3_Data.byte},
  {LIN_CMD4, RECEIVE,  LIN_CMD2_LENGTH, LIN_CMD1_TIMEOUT, LIN_CMD1_PERIOD, LIN_CMD4_Data.byte},
  {LIN_CMD5, RECEIVE,  LIN_CMD2_LENGTH, LIN_CMD2_TIMEOUT, LIN_CMD2_PERIOD, LIN_CMD5_Data.byte},
  {LIN_CMD6, RECEIVE,  LIN_CMD2_LENGTH, LIN_CMD2_TIMEOUT, LIN_CMD2_PERIOD, LIN_CMD6_Data.byte},
};
struct UARTOpStruct_LIN UARTxOp_LIN;
/**
  * @brief  接收超时定时器初始化
  * @param  void
  * @retval void
  */
void lin_timeout_init(void)
{
    FL_GPTIM_InitTypeDef        timInit;

    timInit.prescaler             = 0;                                      /* 分频系数1 */
    timInit.counterMode           = FL_GPTIM_COUNTER_DIR_UP;                /* 向上计数 */
    timInit.autoReload            = TIMER_1US_LOAD * TIME_BASE_PERIOD - 1;  /* 自动重装载值 */
    timInit.clockDivision         = FL_GPTIM_CLK_DIVISION_DIV1;             /* 死区和滤波分频 */
    timInit.autoReloadState       = FL_DISABLE;                             /* 预装载preload使能 */
    FL_GPTIM_Init(GPTIM0, &timInit);

    FL_GPTIM_ClearFlag_Update(GPTIM0);

    /* NVIC中断配置 */
    NVIC_ClearPendingIRQ(GPTIM01_IRQn);
    NVIC_DisableIRQ(GPTIM01_IRQn);
    NVIC_SetPriority(GPTIM01_IRQn, 2);
    NVIC_EnableIRQ(GPTIM01_IRQn);
    
    FL_GPTIM_EnableIT_Update(GPTIM0);

    /* 使能定时器 */
    FL_GPTIM_Enable(GPTIM0);    
}

/**
  * @brief  lin idle
  * @param  void
  * @retval void
  */
void lin_goto_idle(void)
{
    lin_schedule_data_type *plin_shcedule_data;
    
    plin_shcedule_data = &g_lin_task.lin_schedule.lin_schedule_data[g_lin_task.frame_index];
    
    //LED2_TOG();
    
    plin_shcedule_data->rx_len = 0;
    
    g_lin_task.frame_state = FRAME_STATE_IDLE;
    
    UARTxOp_LIN.RxLen = 0;
    UARTxOp_LIN.TxLen = 0;
    
    //LED2_TOG();
}

/**
  * @brief  定时，用于判断字节间超时
  * @note   12位波特率，如1200-10ms 19.2KHz-625us
  * @param  void
  * @retval void
  */
void GPTIM0_1_IRQHandler(void)
{
		uint16_t fl_ad_val;
    FL_GPTIM_ClearFlag_Update(GPTIM0);
    
    switch (g_lin_task.frame_state)
    {
        case FRAME_STATE_IDLE:
            /* 之后增加休眠超时判定 */
            break;
        case FRAME_STATE_SEND_SYN:
        case FRAME_STATE_SEND:
            if (0 == g_lin_task.frame_timeout_cnt)
            {
                //LED2_TOG();
                lin_goto_idle();
            }
            else
            {
                g_lin_task.frame_timeout_cnt--;
            }
            break;
        case FRAME_STATE_RECV_DATA:
            if (0 == g_lin_task.res_frame_timeout_cnt)
            {
                //LED3_TOG();
                
                if (UARTxOp_LIN.RxLen)
                {
                    UARTxOp_LIN.RxLen = 0;
                }
                lin_goto_idle();
            }
            else
            {
                g_lin_task.res_frame_timeout_cnt--;
            }            
            break;
        default:
            break;
    }
		
		if(ticks++ >= 3)
		{
				ticks = 0;
				AdcIf_Polling();
				fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Rail_IO_HALL_AD);
				CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_0,fl_ad_val);
				fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Back_IN_IO_HALL_AD);
				CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_1,fl_ad_val);	
				fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Head_IN_IO_HALL_AD);
				CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_2,fl_ad_val);
				fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Angle_IO_HALL_AD);
				CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_3,fl_ad_val);
				fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Rotate_IN_IO_HALL_AD);
				CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_4,fl_ad_val);
				fl_ad_val = Adcif_Get_AdcVal(ADCIF_CH_Leg_IN_IO_HALL_AD);
				CddMtr_Mng_Motor_Step(CDDMTR_HFKF_CH_5,fl_ad_val);
		}

}

/**
  * @brief  LIN主机发送同步段
  * @param  plin_shcedule_data: 进度表指针
  * @retval void
  */
void lin_send_syn(lin_schedule_data_type *plin_shcedule_data)
{
    plin_shcedule_data->frame_left_slot = plin_shcedule_data->frame_slot;
    
    g_lin_task.frame_state = FRAME_STATE_SEND_SYN;
    
    /* 使用延时发送间隔段 */
    /* 拉低发送引脚 */
    LIN_UART_TX_GPIO->DRST |= LIN_UART_TX_PIN;
    LIN_UART_TX_GPIO->FCR &= ~LIN_UART_TX_PIN_FCR_MASK;
 
    /* 切换为输出引脚 */
    LIN_UART_TX_GPIO->FCR |= LIN_UART_TX_PIN_FCR_OUTPUT;
    FL_DelayUs(LIN_BREAK_BIT * 1000000 / LIN_BAUDRATE);
    LIN_UART_TX_GPIO->FCR &= ~LIN_UART_TX_PIN_FCR_MASK;

    /* 切换为数字引脚 */
    LIN_UART_TX_GPIO->FCR |= LIN_UART_TX_PIN_FCR_DIGIT;

    /* 重置发送定时器 */
    LIN_UART->CSR &= ~UART_CSR_TXEN_Msk;
    LIN_UART->CSR |= UART_CSR_TXEN_Msk;    
}

/**
  * @brief  LPTIM中断函数
  * @param  void
  * @retval void
  */
void LPTIM_IRQHandler(void)
{
    static uint32_t count = 0, flag = 0;
    uint8_t i;
    
    lin_schedule_data_type *plin_shcedule_data;
    
    plin_shcedule_data = &g_lin_task.lin_schedule.lin_schedule_data[g_lin_task.frame_index];
    
    /* 定时器溢出时翻转LED */
    if (LPTIM32->ISR & LPTIM32_ISR_OVIF_Msk)
    {
        if (plin_shcedule_data->frame_left_slot)
        {
            plin_shcedule_data->frame_left_slot--;
        }
        
        if (flag == 0)
        {
            flag = 1;
            
            g_lin_task.frame_index = 0;
            
            plin_shcedule_data = &g_lin_task.lin_schedule.lin_schedule_data[g_lin_task.frame_index];  
            lin_send_syn(plin_shcedule_data);
        }
        
        /* 寻找切换时刻 */
        for (i = 0; i < TABLE_SIZE; i++)
        {
            if (count == schedule_table_slot[i])
            {
                g_lin_task.frame_index++;
                if (g_lin_task.frame_index >= TABLE_SIZE)
                {
                    g_lin_task.frame_index = 0;
                    count = 0;
                }
               
                plin_shcedule_data = &g_lin_task.lin_schedule.lin_schedule_data[g_lin_task.frame_index];  
                lin_send_syn(plin_shcedule_data);
                
                break;
            }
        }
        
        count++;
        
        LPTIM32->ISR = LPTIM32_ISR_OVIF_Msk;
    }
}

/**
  * @brief  LIN-UART中断函数
  * @param  void
  * @retval void
  */
void LIN_UART_IRQHandler(void)
{
    uint8_t tmp08;
    uint32_t timeout;
    lin_schedule_data_type *plin_shcedule_data;
    
    plin_shcedule_data = &g_lin_task.lin_schedule.lin_schedule_data[g_lin_task.frame_index];    
    
   // LED0_TOG();
   
    /* 接收中断处理 */
    if (LIN_UART->ISR & UART_ISR_RXBF_Msk)
    {
        switch (g_lin_task.frame_state)
        {
            /* 接收数据阶段，直到接收完成才处理 */
            case FRAME_STATE_RECV_DATA:
                /* 接收数据 */
                /* 接收中断标志可通过读取rxreg寄存器清除 */
                tmp08 = LIN_UART->RXBUF;
                
                UARTxOp_LIN.RxBuf[UARTxOp_LIN.RxLen] = tmp08;
                UARTxOp_LIN.RxLen++;
            
                if (UARTxOp_LIN.RxLen == scheduleTable[g_lin_task.frame_index].length + 1)
                {
                    /* 接收完整数据 */
                    memcpy(&plin_shcedule_data->data[2], UARTxOp_LIN.RxBuf, UARTxOp_LIN.RxLen);
    
                    #if (CHECKSUM_TYPE == CHECKSUM_TYPE_ENHANCED)
                        if (calculateChecksum(&plin_shcedule_data->data[1], UARTxOp_LIN.RxLen) == UARTxOp_LIN.RxBuf[UARTxOp_LIN.RxLen - 1])
                        {
                            plin_shcedule_data->rx_len = UARTxOp_LIN.RxLen;
                            memcpy(scheduleTable[g_lin_task.frame_index].data, UARTxOp_LIN.RxBuf, scheduleTable[g_lin_task.frame_index].length);
                        }
                        else
                        {
                            plin_shcedule_data->rx_len = 0;                    
                        }
                    #else
                        if (calculateChecksum(&plin_shcedule_data->data[2], UARTxOp_LIN.RxLen - 1) == UARTxOp_LIN.RxBuf[UARTxOp_LIN.RxLen - 1])
                        {
                            plin_shcedule_data->rx_len = UARTxOp_LIN.RxLen;
                            memcpy(scheduleTable[g_lin_task.frame_index].data, UARTxOp_LIN.RxBuf, scheduleTable[g_lin_task.frame_index].length);
                        }
                        else
                        {
                            plin_shcedule_data->rx_len = 0;
                        }
                    #endif
                    lin_goto_idle();
                }
                
                break;
            case FRAME_STATE_SEND:
                /* 接收数据 */
                /* 接收中断标志可通过读取rxreg寄存器清除 */
                tmp08 = LIN_UART->RXBUF;
            
                /* 发送指定长度的数据 */
                if (UARTxOp_LIN.TxOpc < UARTxOp_LIN.TxLen)
                {
                    LIN_UART->TXBUF = UARTxOp_LIN.TxBuf[UARTxOp_LIN.TxOpc]; 
                    UARTxOp_LIN.TxOpc++;
                }
                else
                {
                    /* 发送完毕 */
                    if (plin_shcedule_data->frame_type == FRAME_TYPE_NO_RESPONSE)
                    {
                        /* 帧类型为无需回应 */                
                        lin_goto_idle();
                    }
                    else
                    {
                        /* 需等待回应 */
                        g_lin_task.frame_state = FRAME_STATE_RECV_DATA;
                        
                        /* 开启应答间隔判断 */
                        /* 加入余量控制的超时，单位ms */
                        if (plin_shcedule_data->timeout == 0)
                        {
                            timeout =  plin_shcedule_data->frame_left_slot;
                        }
                        else
                        {
                            timeout = Min(plin_shcedule_data->timeout, plin_shcedule_data->frame_left_slot);
                        }
                        
                        timeout = timeout * 1000 / TIME_BASE_PERIOD;
                        
                        g_lin_task.res_frame_timeout_cnt = Min(timeout, g_lin_task.res_frame_timeout_cnt);
                    }
                }
                break;
            default:
                /* 接收数据 */
                /* 接收中断标志可通过读取rxreg寄存器清除 */
                tmp08 = LIN_UART->RXBUF;
            
                break;
        }
    }
    
    /* 接收错误中断处理 需要注意帧类型错误会同时有RXBF标志，用于判定同步段 */
    if (LIN_UART->ISR & UART_ISR_FERR_Msk)
    {
        tmp08 = LIN_UART->RXBUF;
        if (g_lin_task.frame_state != FRAME_STATE_SEND_SYN)
        {
            /* 当前状态不正确，直接结束任务 */
            lin_goto_idle();
        }
        else if (tmp08 == LIN_BREAK_BYTE)
        {
            /* 第一时间发送第一个字节 0x55 */
            LIN_UART->TXBUF = plin_shcedule_data->data[0];
            
            /* 实时更新buffer */                
            memcpy(&plin_shcedule_data->data[2], scheduleTable[g_lin_task.frame_index].data, scheduleTable[g_lin_task.frame_index].length);
            #if (CHECKSUM_TYPE == CHECKSUM_TYPE_ENHANCED)
                plin_shcedule_data->data[scheduleTable[g_lin_task.frame_index].length + 2] = calculateChecksum(&plin_shcedule_data->data[1], scheduleTable[g_lin_task.frame_index].length + 1);
            #else
                plin_shcedule_data->data[scheduleTable[g_lin_task.frame_index].length + 2] = calculateChecksum(&plin_shcedule_data->data[2], scheduleTable[g_lin_task.frame_index].length);
            #endif
                            
            /* 设置发送长度和数据 */
            UARTxOp_LIN.TxOpc = 1;
            UARTxOp_LIN.TxLen = plin_shcedule_data->tx_len;
            UARTxOp_LIN.TxBuf = plin_shcedule_data->data;
            
            /* 主发送数据阶段 */
            g_lin_task.frame_state = FRAME_STATE_SEND;
            
            /* 收到间隔段之后置起超时，等待在超时内处理后续流程 */
            g_lin_task.frame_timeout_cnt = lin_calc_max_res_timeout_cnt(LIN_BIT_TIMER_LOAD, 8);
            g_lin_task.res_frame_timeout_cnt = lin_calc_max_res_timeout_cnt(LIN_BIT_TIMER_LOAD, scheduleTable[g_lin_task.frame_index].length + 1);
        }
        LIN_UART->ISR = UART_ISR_FERR_Msk;
    }
    
   // LED0_TOG();
}

/**
  * @brief  lin UART初始化
  * @param  void
  * @retval void
  */
void Uartx_Init(UART_Type *UARTx);
void lin_uart_init(void)
{
    /* 初始化uart配置 */
    Uartx_Init(LIN_UART);

    /* 接收错误中断使能，用于同步判断 */
    FL_UART_EnableIT_RXError(LIN_UART);
    FL_UART_EnableIT_RXBuffFull(LIN_UART);
    
    FL_UART_EnableTX(LIN_UART);
    FL_UART_EnableRX(LIN_UART);
}

/**
  * @brief  LPTIM32初始化
  * @note   lin任务时基定时器 1ms
  * @param  void
  * @retval void
  */
void LPTIM32_Setup(void)
{
    FL_LPTIM32_InitTypeDef      timInit;

    /*---------------- 定时器时间基准配置 ----------------*/
    FL_LPTIM32_StructInit(&timInit);

    timInit.clockSource          = FL_CMU_LPTIM32_CLK_SOURCE_APBCLK;
    timInit.mode                 = FL_LPTIM32_OPERATION_MODE_NORMAL;
    timInit.prescalerClockSource = FL_LPTIM32_CLK_SOURCE_INTERNAL;
    timInit.prescaler            = FL_LPTIM32_PSC_DIV1;
    timInit.autoReload           = TIMER_1MS_LOAD;         
    //timInit.encoderMode          = FL_LPTIM32_ENCODER_MODE_DISABLE;
    timInit.onePulseMode         = FL_LPTIM32_ONE_PULSE_MODE_CONTINUOUS;
    timInit.triggerEdge          = FL_LPTIM32_ETR_TRIGGER_EDGE_RISING;
    timInit.countEdge            = FL_LPTIM32_ETR_COUNT_EDGE_RISING;
    FL_LPTIM32_Init(LPTIM32, &timInit);


    /*---------------- 中断配置 ----------------*/
    /* 清除标志 */
    FL_LPTIM32_ClearFlag_Update(LPTIM32);

    /* 中断使能 */
    FL_LPTIM32_EnableIT_Update(LPTIM32);

    /* 使能并配置NVIC */
    NVIC_DisableIRQ(LPTIMx_IRQn);
    NVIC_SetPriority(LPTIMx_IRQn, 2); 
    NVIC_EnableIRQ(LPTIMx_IRQn);
    /*---------------------------------------------*/

    FL_LPTIM32_Enable(LPTIM32);
}

/**
  * @brief  lin 任务初始化
  * @param  void
  * @retval void
  */
void lin_task_init(void)
{
    uint8_t index = 0, i;
    lin_schedule_data_type *plin_shcedule_data;
    
    /* 状态机相关初始化 */
    g_lin_task.frame_state = FRAME_STATE_IDLE;
    g_lin_task.frame_index = 0;
    
    /* 进度表初始化 */
    g_lin_task.lin_schedule.frame_num = TABLE_SIZE;

    for (i = 0; i < TABLE_SIZE; i++)
    {
        plin_shcedule_data = &g_lin_task.lin_schedule.lin_schedule_data[i];
        
        plin_shcedule_data->frame_slot = scheduleTable[i].period;
        if (i == 0)
        {
            schedule_table_slot[i] = scheduleTable[i].period;
        }
        else
        {
            schedule_table_slot[i] = schedule_table_slot[i - 1] + scheduleTable[i].period;
        }
        
        if (scheduleTable[i].type == TRANSMIT)
        {
            plin_shcedule_data->frame_type = FRAME_TYPE_NO_RESPONSE;
        }
        else
        {
            plin_shcedule_data->frame_type = FRAME_TYPE_EVENT_TRIGGER;
        }
        plin_shcedule_data->rx_len = 0;
        plin_shcedule_data->timeout = scheduleTable[i].timeout;
        
        /* 准备数据区 */
        index = 0;
        plin_shcedule_data->data[index] = 0x55;
        index += 1;
        plin_shcedule_data->data[index] = calculatePID(scheduleTable[i].cmd);
        index += 1;
        if (scheduleTable[i].type == TRANSMIT)
        {
            memcpy(&plin_shcedule_data->data[index], scheduleTable[i].data, scheduleTable[i].length);
            index += scheduleTable[i].length;
            #if (CHECKSUM_TYPE == CHECKSUM_TYPE_ENHANCED)
                plin_shcedule_data->data[index] = calculateChecksum(&plin_shcedule_data->data[1], scheduleTable[i].length + 1);
            #else
                plin_shcedule_data->data[index] = calculateChecksum(&plin_shcedule_data->data[2], scheduleTable[i].length);
            #endif
            index += 1;            
        }
        plin_shcedule_data->tx_len = index;
    }
}

/**
  * @brief  lin 初始化
  * @param  void
  * @retval void
  */
void lin_init(void)
{
    /* LIN任务初始化 */
    lin_task_init();

    /* LIN使能引脚初始化 */
   // lin_slp_pin_init();

    /*LIN UART初始化 */
    lin_uart_init();

    /* 使用定时器用作同步段发送和接收超时判断 */
    lin_timeout_init();

    /* 1ms定时器，主任务处理 */
    LPTIM32_Setup();
}
