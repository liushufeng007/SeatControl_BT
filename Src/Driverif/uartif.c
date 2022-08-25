#include "uartif.h"
#include "user_init.h"


Uartif_queue_str Uartif_rx_queue;
Uartif_queue_str Uartif_tx_queue;
UARTOpStruct UARTxOp;





/************************************
Get Sw Fifo Empty condition
************************************/
uint8_t Uartif_tx_queue_is_empty(void);
uint8_t Uartif_tx_queue_is_full(void);
uint8_t Uartif_tx_queue_push_e(Uartif_Msg_Str fl_str_e);
uint8_t Uartif_tx_queue_pull_e(Uartif_Msg_Str * fl_str_e);
uint8_t Uartif_rx_queue_is_empty(void);;
uint8_t Uartif_rx_queue_is_full(void);
uint8_t Uartif_rx_queue_push_e(Uartif_Msg_Str fl_str_e);
uint8_t Uartif_rx_queue_pull_e(Uartif_Msg_Str *fl_str_e);

/*************************************
rx process in IRQ
*************************************/
void Uartx_RxHandle(uint8_t data)
{
	if(UARTxOp.RxOpc < UARTIF_MAX_TXRX_SIZE)
	{
		UARTxOp.RxPool[UARTxOp.RxOpc] = data;
		UARTxOp.RxOpc ++;
	}
	UARTxOp.Rxframeticks = 0;	
}

/*************************************
rx process in IRQ
*************************************/
void Uartx_RxFrameComplete_Detect(void)
{
	Uartif_Msg_Str fl_str_e;

	/*rx buffer is empty*/
	if(UARTxOp.RxOpc > 0 && UARTxOp.RxOpc == UARTxOp.RxOpc_bak)
	{
		if(UARTxOp.Rxframeticks >= UARTIF_FRAME_COMPLETE_TIME)
		{
			__disable_irq();
			fl_str_e.DLC = UARTxOp.RxOpc;
			memcpy(fl_str_e.Data,UARTxOp.RxPool,fl_str_e.DLC);
			/*clear uart buffer*/
			UARTxOp.RxOpc = 0;
			__enable_irq();
			(void)Uartif_rx_queue_push_e(fl_str_e);
			UARTxOp.Rxframeticks = 0;
		}
		else
		{
			UARTxOp.Rxframeticks++;
		}
	}
	else
	{
		UARTxOp.Rxframeticks = 0;
		UARTxOp.RxOpc_bak = UARTxOp.RxOpc;
	}
}


/**
  * @brief  UART0中断函数
  * @param  void
  * @retval void
  */
void UART0_IRQHandler(void)
{
    uint8_t tmp08;

    /* 接收中断处理 */
    if((FL_ENABLE == FL_UART_IsEnabledIT_RXBuffFull(UART0))
            && (FL_SET == FL_UART_IsActiveFlag_RXBuffFull(UART0)))
    {
        /* 中断转发接收到的数据 */
        tmp08 = FL_UART_ReadRXBuff(UART0); /* 接收中断标志可通过读取rxreg寄存器清除 */
        //FL_UART_WriteTXBuff(UART0, tmp08);
       // Uartx_RxHandle(tmp08);
    }

    /* 发送中断处理 */
    if((FL_ENABLE == FL_UART_IsEnabledIT_TXShiftBuffEmpty(UART0))
            && (FL_SET == FL_UART_IsActiveFlag_TXShiftBuffEmpty(UART0)))
    {
        /* 发送中断标志可通过写txreg寄存器清除或txif写1清除 */
        /* 发送指定长度的数据 */
        //if(UARTxOp.TxOpc < UARTxOp.TxLen)
        {
            FL_UART_WriteTXBuff(UART0, UARTxOp.TxBuf[UARTxOp.TxOpc]); /* 发送一个数据 */
           // UARTxOp.TxOpc++;
        }

        FL_UART_ClearFlag_TXShiftBuffEmpty(UART0);    /* 清除发送中断标志 */
    }
}

/**
  * @brief  UART1中断函数
  * @param  void
  * @retval void
  */
void UART1_IRQHandler(void)
{
    uint8_t tmp08;

    /* 接收中断处理 */
    if((FL_ENABLE == FL_UART_IsEnabledIT_RXBuffFull(UART1))
            && (FL_SET == FL_UART_IsActiveFlag_RXBuffFull(UART1)))
    {
		/* 中断转发接收到的数据 */
		tmp08 = FL_UART_ReadRXBuff(UART1); /* 接收中断标志可通过读取rxreg寄存器清除 */
		if(UARTxOp.RxOpc < UARTIF_MAX_TXRX_SIZE)
		{
			UARTxOp.RxPool[UARTxOp.RxOpc] = tmp08; 
			UARTxOp.RxOpc++;
		}

	 }

    /* 发送中断处理 */
    if((FL_ENABLE == FL_UART_IsEnabledIT_TXShiftBuffEmpty(UART1))
            && (FL_SET == FL_UART_IsActiveFlag_TXShiftBuffEmpty(UART1)))
    {
        /* 发送中断标志可通过写txreg寄存器清除或txif写1清除 */
        /* 发送指定长度的数据 */
		#if 0
        if(UARTxOp.TxOpc < UARTxOp.TxLen)
        {
            FL_UART_WriteTXBuff(UART1, UARTxOp.TxBuf[UARTxOp.TxOpc]); /* 发送一个数据 */
            UARTxOp.TxOpc++;
        }
		#endif
        FL_UART_ClearFlag_TXShiftBuffEmpty(UART1);  /* 清除发送中断标志 */
    }
}

/**
  * @brief  UART3中断函数
  * @param  void
  * @retval void
  */
void UART3_IRQHandler(void)
{
    uint8_t tmp08;

    /* 接收中断处理 */
    if((FL_ENABLE == FL_UART_IsEnabledIT_RXBuffFull(UART3))
            && (FL_SET == FL_UART_IsActiveFlag_RXBuffFull(UART3)))
    {
        /* 中断转发接收到的数据 */
        tmp08 = FL_UART_ReadRXBuff(UART3);  /* 接收中断标志可通过读取rxreg寄存器清除 */
        FL_UART_WriteTXBuff(UART3, tmp08);
    }

    /* 发送中断处理 */
    if((FL_ENABLE == FL_UART_IsEnabledIT_TXShiftBuffEmpty(UART3))
            && (FL_SET == FL_UART_IsActiveFlag_TXShiftBuffEmpty(UART3)))
    {
        /* 发送中断标志可通过写txreg寄存器清除或txif写1清除 */
        /* 发送指定长度的数据 */
        if(UARTxOp.TxOpc < UARTxOp.TxLen)
        {
            FL_UART_WriteTXBuff(UART3, UARTxOp.TxBuf[UARTxOp.TxOpc]);  /* 发送一个数据 */
            UARTxOp.TxOpc++;
        }

        FL_UART_ClearFlag_TXShiftBuffEmpty(UART3);  /* 清除发送中断标志 */
    }
}

/**
  * @brief  UART4中断函数
  * @param  void
  * @retval void
  */
#if 0
void UART4_IRQHandler(void)
{
    uint8_t tmp08;

    /* 接收中断处理 */
    if((FL_ENABLE == FL_UART_IsEnabledIT_RXBuffFull(UART4))
            && (FL_SET == FL_UART_IsActiveFlag_RXBuffFull(UART4)))
    {
        /* 中断转发接收到的数据 */
        tmp08 = FL_UART_ReadRXBuff(UART4);  /* 接收中断标志可通过读取rxreg寄存器清除 */
        FL_UART_WriteTXBuff(UART4, tmp08);
    }

    /* 发送中断处理 */
    if((FL_ENABLE == FL_UART_IsEnabledIT_TXShiftBuffEmpty(UART4))
            && (FL_SET == FL_UART_IsActiveFlag_TXShiftBuffEmpty(UART4)))
    {
        /* 发送中断标志可通过写txreg寄存器清除或txif写1清除 */
        /* 发送指定长度的数据 */
        if(UARTxOp.TxOpc < UARTxOp.TxLen)
        {
            FL_UART_WriteTXBuff(UART4, UARTxOp.TxBuf[UARTxOp.TxOpc]);  /* 发送一个数据 */
            UARTxOp.TxOpc++;
        }

        FL_UART_ClearFlag_TXShiftBuffEmpty(UART4);  /* 清除发送中断标志 */
    }
}
#endif

/**
  * @brief  UART5中断函数
  * @param  void
  * @retval void
  */
void UART5_IRQHandler(void)
{
    uint8_t tmp08;

    /* 接收中断处理 */
    if((FL_ENABLE == FL_UART_IsEnabledIT_RXBuffFull(UART5))
            && (FL_SET == FL_UART_IsActiveFlag_RXBuffFull(UART5)))
    {
        /* 中断转发接收到的数据 */
        tmp08 = FL_UART_ReadRXBuff(UART5);  /* 接收中断标志可通过读取rxreg寄存器清除 */
        FL_UART_WriteTXBuff(UART5, tmp08);
    }

    /* 发送中断处理 */
    if((FL_ENABLE == FL_UART_IsEnabledIT_TXShiftBuffEmpty(UART5))
            && (FL_SET == FL_UART_IsActiveFlag_TXShiftBuffEmpty(UART5)))
    {
        /* 发送中断标志可通过写txreg寄存器清除或txif写1清除 */
        /* 发送指定长度的数据 */
        if(UARTxOp.TxOpc < UARTxOp.TxLen)
        {
            FL_UART_WriteTXBuff(UART5, UARTxOp.TxBuf[UARTxOp.TxOpc]);  /* 发送一个数据 */
            UARTxOp.TxOpc++;
        }

        FL_UART_ClearFlag_TXShiftBuffEmpty(UART5);  /* 清除发送中断标志 */
    }
}

/**
  * @brief  UART初始化
  * @param  UARTx: 串口选择
  * @retval void
  */
void Uartx_Init(UART_Type *UARTx)
{
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    FL_UART_InitTypeDef UART_InitStruct = {0};

    switch((uint32_t)UARTx)
    {
        case UART0_BASE:
            /* PA13:UART0-RX */
            GPIO_InitStruct.pin        = FL_GPIO_PIN_13;
            GPIO_InitStruct.mode       = FL_GPIO_MODE_DIGITAL;
            GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;   
            GPIO_InitStruct.pull       = FL_ENABLE;
            GPIO_InitStruct.remapPin   = FL_DISABLE;
            FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
            /* PA14:UART0-TX */
            GPIO_InitStruct.pin        = FL_GPIO_PIN_14;
            GPIO_InitStruct.mode       = FL_GPIO_MODE_DIGITAL;
            GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;       
            GPIO_InitStruct.pull       = FL_DISABLE;
            GPIO_InitStruct.remapPin   = FL_DISABLE;
            FL_GPIO_Init(GPIOA, &GPIO_InitStruct);

            /* PA2:UART0-RX */
            //          GPIO_InitStruct.pin         = FL_GPIO_PIN_2;
            //          GPIO_InitStruct.mode        = FL_GPIO_MODE_DIGITAL;
            //          GPIO_InitStruct.outputType  = FL_GPIO_OUTPUT_PUSHPULL;
            //          GPIO_InitStruct.pull        = FL_ENABLE;
            //          GPIO_InitStruct.remapPin    = FL_DISABLE;
            //          FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
            
            /* PA3:UART0-TX */
            //          GPIO_InitStruct.pin         = FL_GPIO_PIN_3;
            //          GPIO_InitStruct.mode        = FL_GPIO_MODE_DIGITAL;
            //          GPIO_InitStruct.outputType  = FL_GPIO_OUTPUT_PUSHPULL;
            //          GPIO_InitStruct.pull        = FL_DISABLE;
            //          GPIO_InitStruct.remapPin    = FL_DISABLE;
            //          FL_GPIO_Init(GPIOA, &GPIO_InitStruct);


            UART_InitStruct.clockSrc = FL_CMU_UART0_CLK_SOURCE_APBCLK;
            /* NVIC中断配置 */
            NVIC_DisableIRQ(UART0_IRQn);
            NVIC_SetPriority(UART0_IRQn, 2); /* 中断优先级配置 */
            NVIC_EnableIRQ(UART0_IRQn);

			UART_InitStruct.baudRate			= 115200;									  /* 波特率 */
			UART_InitStruct.dataWidth			= FL_UART_DATA_WIDTH_8B;					/* 数据位数 */
			UART_InitStruct.stopBits			= FL_UART_STOP_BIT_WIDTH_1B;				/* 停止位 */
			UART_InitStruct.parity				= FL_UART_PARITY_NONE;						/* 奇偶校验 */
			UART_InitStruct.transferDirection	= FL_UART_DIRECTION_TX_RX;					/* 接收-发送使能 */
			FL_UART_Init(UARTx, &UART_InitStruct);
            break;

        case UART1_BASE:
            /* PB13:UART1-RX */
            //          GPIO_InitStruct.pin         = FL_GPIO_PIN_13;
            //          GPIO_InitStruct.mode        = FL_GPIO_MODE_DIGITAL;
            //          GPIO_InitStruct.outputType  = FL_GPIO_OUTPUT_PUSHPULL;
            //          GPIO_InitStruct.pull        = FL_ENABLE;
            //          GPIO_InitStruct.remapPin    = FL_DISABLE;
            //          FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        
            /* PB14:UART1-TX */
            //          GPIO_InitStruct.pin         = FL_GPIO_PIN_14;
            //          GPIO_InitStruct.mode        = FL_GPIO_MODE_DIGITAL;
            //          GPIO_InitStruct.outputType  = FL_GPIO_OUTPUT_PUSHPULL;
            //          GPIO_InitStruct.pull        = FL_DISABLE;
            //          GPIO_InitStruct.remapPin    = FL_DISABLE;
            //          FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

            /* PC2:UART1-RX */
            GPIO_InitStruct.pin        = FL_GPIO_PIN_13;
            GPIO_InitStruct.mode       = FL_GPIO_MODE_DIGITAL;
            GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
            GPIO_InitStruct.pull       = FL_ENABLE;
            GPIO_InitStruct.remapPin   = FL_DISABLE;
            FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            
            /* PC3:UART1-TX */
            GPIO_InitStruct.pin        = FL_GPIO_PIN_14;
            GPIO_InitStruct.mode       = FL_GPIO_MODE_DIGITAL;
            GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
            GPIO_InitStruct.pull       = FL_DISABLE;
            GPIO_InitStruct.remapPin   = FL_DISABLE;
            FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

            UART_InitStruct.clockSrc = FL_CMU_UART1_CLK_SOURCE_APBCLK;
            /* NVIC中断配置 */
            NVIC_DisableIRQ(UART1_IRQn);
            NVIC_SetPriority(UART1_IRQn, 2); /* 中断优先级配置 */
            NVIC_EnableIRQ(UART1_IRQn);

			UART_InitStruct.baudRate			= 115200;									  /* 波特率 */
			UART_InitStruct.dataWidth			= FL_UART_DATA_WIDTH_8B;					/* 数据位数 */
			UART_InitStruct.stopBits			= FL_UART_STOP_BIT_WIDTH_1B;				/* 停止位 */
			UART_InitStruct.parity				= FL_UART_PARITY_NONE;						/* 奇偶校验 */
			UART_InitStruct.transferDirection	= FL_UART_DIRECTION_TX_RX;					/* 接收-发送使能 */
			FL_UART_Init(UARTx, &UART_InitStruct);
            break;

        case UART3_BASE:
            /* PB0:UART3-RX */
            GPIO_InitStruct.pin         = FL_GPIO_PIN_0;
            GPIO_InitStruct.mode        = FL_GPIO_MODE_DIGITAL;
            GPIO_InitStruct.outputType  = FL_GPIO_OUTPUT_PUSHPULL;
            GPIO_InitStruct.pull        = FL_ENABLE;
            GPIO_InitStruct.remapPin    = FL_ENABLE;
            FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        
            /* PB1:UART3-TX */
            GPIO_InitStruct.pin         = FL_GPIO_PIN_1;
            GPIO_InitStruct.mode        = FL_GPIO_MODE_DIGITAL;
            GPIO_InitStruct.outputType  = FL_GPIO_OUTPUT_PUSHPULL;
            GPIO_InitStruct.pull        = FL_DISABLE;
            GPIO_InitStruct.remapPin    = FL_ENABLE;
            FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

            /* PD7:UART3-RX */
            //          GPIO_InitStruct.pin         = FL_GPIO_PIN_7;
            //          GPIO_InitStruct.mode        = FL_GPIO_MODE_DIGITAL;
            //          GPIO_InitStruct.outputType  = FL_GPIO_OUTPUT_PUSHPULL;
            //          GPIO_InitStruct.pull        = FL_ENABLE;
            //          GPIO_InitStruct.remapPin    = FL_ENABLE;
            //          FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
            
            /* PD8:UART3-TX */
            //          GPIO_InitStruct.pin         = FL_GPIO_PIN_8;
            //          GPIO_InitStruct.mode        = FL_GPIO_MODE_DIGITAL;
            //          GPIO_InitStruct.outputType  = FL_GPIO_OUTPUT_PUSHPULL;
            //          GPIO_InitStruct.pull        = FL_DISABLE;
            //          GPIO_InitStruct.remapPin    = FL_ENABLE;
            //          FL_GPIO_Init(GPIOD, &GPIO_InitStruct);

            /* NVIC中断配置 */
            NVIC_DisableIRQ(UART3_IRQn);
            NVIC_SetPriority(UART3_IRQn, 2); /* 中断优先级配置 */
            NVIC_EnableIRQ(UART3_IRQn);

			UART_InitStruct.baudRate			= 115200;									  /* 波特率 */
			UART_InitStruct.dataWidth			= FL_UART_DATA_WIDTH_8B;					/* 数据位数 */
			UART_InitStruct.stopBits			= FL_UART_STOP_BIT_WIDTH_1B;				/* 停止位 */
			UART_InitStruct.parity				= FL_UART_PARITY_NONE;						/* 奇偶校验 */
			UART_InitStruct.transferDirection	= FL_UART_DIRECTION_TX_RX;					/* 接收-发送使能 */
			FL_UART_Init(UARTx, &UART_InitStruct);
            break;


        case UART4_BASE:
            /* PB2:UART4-RX */
            GPIO_InitStruct.pin          = FL_GPIO_PIN_2;
            GPIO_InitStruct.mode         = FL_GPIO_MODE_DIGITAL;
            GPIO_InitStruct.outputType   = FL_GPIO_OUTPUT_PUSHPULL;
            GPIO_InitStruct.pull         = FL_ENABLE;
            GPIO_InitStruct.remapPin     = FL_DISABLE;
			GPIO_InitStruct.analogSwitch   = FL_DISABLE;
            FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        
            /* PB3:UART4-TX */
            GPIO_InitStruct.pin          = FL_GPIO_PIN_3;
            GPIO_InitStruct.mode         = FL_GPIO_MODE_DIGITAL;
            GPIO_InitStruct.outputType   = FL_GPIO_OUTPUT_PUSHPULL;
            GPIO_InitStruct.pull         = FL_DISABLE;
            GPIO_InitStruct.remapPin     = FL_DISABLE;
			GPIO_InitStruct.analogSwitch   = FL_DISABLE;
            FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

            /* PA0:UART4-RX */
            //          GPIO_InitStruct.pin            = FL_GPIO_PIN_0;
            //          GPIO_InitStruct.mode           = FL_GPIO_MODE_DIGITAL;
            //          GPIO_InitStruct.outputType     = FL_GPIO_OUTPUT_PUSHPULL;
            //          GPIO_InitStruct.pull           = FL_ENABLE;
            //          GPIO_InitStruct.remapPin       = FL_DISABLE;
            //          FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
            
            /* PA1:UART4-TX */
            //          GPIO_InitStruct.pin            = FL_GPIO_PIN_1;
            //          GPIO_InitStruct.mode           = FL_GPIO_MODE_DIGITAL;
            //          GPIO_InitStruct.outputType     = FL_GPIO_OUTPUT_PUSHPULL;
            //          GPIO_InitStruct.pull           = FL_DISABLE;
            //          GPIO_InitStruct.remapPin       = FL_DISABLE;
            //          FL_GPIO_Init(GPIOA, &GPIO_InitStruct);

            /* NVIC中断配置 */
            NVIC_DisableIRQ(UART4_IRQn);
            NVIC_SetPriority(UART4_IRQn, 2); /* 中断优先级配置 */
            NVIC_EnableIRQ(UART4_IRQn);

			UART_InitStruct.baudRate			= 19200;									  /* 波特率 */
			UART_InitStruct.dataWidth			= FL_UART_DATA_WIDTH_8B;					/* 数据位数 */
			UART_InitStruct.stopBits			= FL_UART_STOP_BIT_WIDTH_1B;				/* 停止位 */
			UART_InitStruct.parity				= FL_UART_PARITY_NONE;						/* 奇偶校验 */
			UART_InitStruct.transferDirection	= FL_UART_DIRECTION_TX_RX;					/* 接收-发送使能 */
			FL_UART_Init(UARTx, &UART_InitStruct);
            break;

        case UART5_BASE:
            /* PD0:UART5-RX */
            GPIO_InitStruct.pin            = FL_GPIO_PIN_0;
            GPIO_InitStruct.mode           = FL_GPIO_MODE_DIGITAL;
            GPIO_InitStruct.outputType     = FL_GPIO_OUTPUT_PUSHPULL;
            GPIO_InitStruct.pull           = FL_ENABLE;
            GPIO_InitStruct.remapPin       = FL_DISABLE;
            FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
        
            /* PD1:UART5-TX */
            GPIO_InitStruct.pin            = FL_GPIO_PIN_1;
            GPIO_InitStruct.mode           = FL_GPIO_MODE_DIGITAL;
            GPIO_InitStruct.outputType     = FL_GPIO_OUTPUT_PUSHPULL;
            GPIO_InitStruct.pull           = FL_DISABLE;
            GPIO_InitStruct.remapPin       = FL_DISABLE;
            FL_GPIO_Init(GPIOD, &GPIO_InitStruct);

            /* PC4:UART5-RX */
            //          GPIO_InitStruct.pin           = FL_GPIO_PIN_4;
            //          GPIO_InitStruct.mode          = FL_GPIO_MODE_DIGITAL;
            //          GPIO_InitStruct.outputType    = FL_GPIO_OUTPUT_PUSHPULL;
            //          GPIO_InitStruct.pull          = FL_ENABLE;
            //          GPIO_InitStruct.remapPin      = FL_DISABLE;
            //          FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
            
            /* PC5:UART5-TX */
            //          GPIO_InitStruct.pin           = FL_GPIO_PIN_5;
            //          GPIO_InitStruct.mode          = FL_GPIO_MODE_DIGITAL;
            //          GPIO_InitStruct.outputType    = FL_GPIO_OUTPUT_PUSHPULL;
            //          GPIO_InitStruct.pull          = FL_DISABLE;
            //          GPIO_InitStruct.remapPin      = FL_DISABLE;
            //          FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

            /* NVIC中断配置 */
            NVIC_DisableIRQ(UART5_IRQn);
            NVIC_SetPriority(UART5_IRQn, 2); /* 中断优先级配置 */
            NVIC_EnableIRQ(UART5_IRQn);
			
			UART_InitStruct.baudRate			= 115200;									  /* 波特率 */
			UART_InitStruct.dataWidth			= FL_UART_DATA_WIDTH_8B;					/* 数据位数 */
			UART_InitStruct.stopBits			= FL_UART_STOP_BIT_WIDTH_1B;				/* 停止位 */
			UART_InitStruct.parity				= FL_UART_PARITY_NONE;						/* 奇偶校验 */
			UART_InitStruct.transferDirection	= FL_UART_DIRECTION_TX_RX;					/* 接收-发送使能 */
			FL_UART_Init(UARTx, &UART_InitStruct);
            break;

        default:
            break;
    }


}


/**
  * @brief  UART测试
  * @param  UARTx: 串口选择
  * @retval void
  */
void Uart_Init(void)
{
    FL_DMA_InitTypeDef dmaInitStruct = {0};
	
    Uartx_Init(UARTIF_USE_CHANNEL);
	
	/* TX配置 */
	dmaInitStruct.circMode = FL_DISABLE;
	dmaInitStruct.dataSize = FL_DMA_BANDWIDTH_8B;
	dmaInitStruct.direction = FL_DMA_DIR_RAM_TO_PERIPHERAL;
	dmaInitStruct.memoryAddressIncMode = FL_DMA_MEMORY_INC_MODE_INCREASE;
	dmaInitStruct.priority = FL_DMA_PRIORITY_HIGH ;
	dmaInitStruct.periphAddress = FL_DMA_PERIPHERAL_FUNCTION3;
	FL_DMA_Init(DMA, &dmaInitStruct, FL_DMA_CHANNEL_6);

	    /* RX配置 */
	/*
    dmaInitStruct.circMode = FL_DISABLE;
    dmaInitStruct.dataSize = FL_DMA_BANDWIDTH_8B;
    dmaInitStruct.direction = FL_DMA_DIR_PERIPHERAL_TO_RAM;
    dmaInitStruct.memoryAddressIncMode = FL_DMA_MEMORY_INC_MODE_INCREASE;
    dmaInitStruct.priority = FL_DMA_PRIORITY_HIGH ;
    dmaInitStruct.periphAddress = FL_DMA_PERIPHERAL_FUNCTION3;
    FL_DMA_Init(DMA, &dmaInitStruct, FL_DMA_CHANNEL_5);*/
	
    FL_DMA_Enable(DMA);

    /* 中断发送数组 */
    //UARTxOp.TxBuf = UARTxOp.TxBuf;
    UARTxOp.TxLen = 0;
    UARTxOp.TxOpc = 0;
    FL_UART_ClearFlag_TXShiftBuffEmpty(UARTIF_USE_CHANNEL);
    FL_UART_EnableIT_TXShiftBuffEmpty(UARTIF_USE_CHANNEL);

	
}


void UartWriteDataDMA(uint32_t dataAddr, uint16_t length)
{
    FL_DMA_WriteTransmissionSize(DMA, length - 1, FL_DMA_CHANNEL_6);
    FL_DMA_WriteMemoryAddress(DMA, dataAddr, FL_DMA_CHANNEL_6);
    FL_DMA_ClearFlag_TransferComplete(DMA, FL_DMA_CHANNEL_6);
    FL_DMA_EnableChannel(DMA, FL_DMA_CHANNEL_6);


    //while(!FL_DMA_IsActiveFlag_TransferComplete(DMA, FL_DMA_CHANNEL_6));

}


/**
  * @brief  SPI DMA读数据
  * @param  dataAddr: 数据地址
  *         length:   数据长度
  * @retval void
  */
void SpiReadDataDMA(uint32_t dataAddr, uint16_t length)
{
    FL_DMA_WriteTransmissionSize(DMA, length - 1, FL_DMA_CHANNEL_5);
    FL_DMA_WriteMemoryAddress(DMA, dataAddr, FL_DMA_CHANNEL_5);
    FL_DMA_ClearFlag_TransferComplete(DMA, FL_DMA_CHANNEL_5);
    FL_DMA_EnableChannel(DMA, FL_DMA_CHANNEL_5);

    //while(!FL_DMA_IsActiveFlag_TransferComplete(DMA, FL_DMA_CHANNEL_5));
}


/**
  * @brief  UART测试
  * @param  UARTx: 串口选择
  * @retval void
  */
void Uartxif_Tx(UART_Type *UARTx,Uartif_Msg_Str Uart_Date_e)
{
	  memcpy(UARTxOp.TxBuf,Uart_Date_e.Data,Uart_Date_e.DLC);
    UARTxOp.TxLen = Uart_Date_e.DLC;
    UARTxOp.TxOpc = UARTxOp.TxLen;

    FL_UART_ClearFlag_TXShiftBuffEmpty(UARTx);
    FL_UART_EnableIT_TXShiftBuffEmpty(UARTx);
    //FL_UART_WriteTXBuff(UARTx, UARTxOp.TxBuf[0]);
    UartWriteDataDMA((uint32_t)UARTxOp.TxBuf,UARTxOp.TxLen);
}



/************************************
Get Sw Fifo Empty condition
************************************/
uint8_t Uartif_tx_queue_is_empty(void)
{
	uint8_t fl_temp_U8;
	
	__disable_irq();
	if(Uartif_tx_queue.queue_in == Uartif_tx_queue.queue_out)
	{
		fl_temp_U8 = TRUE;
	}
	else
	{
		fl_temp_U8 = FALSE;
	}
	__enable_irq();

	return fl_temp_U8;
}

/************************************
Get Sw Fifo Full condition
************************************/

uint8_t Uartif_tx_queue_is_full(void)
{
	uint8_t fl_temp_U8;
	__disable_irq();
	fl_temp_U8 = (Uartif_tx_queue.queue_in + 1) % (sizeof(Uartif_tx_queue.queue)/sizeof(Uartif_tx_queue.queue[0]));
	if(fl_temp_U8 == Uartif_tx_queue.queue_out)
	{
		fl_temp_U8 = TRUE;
	}
	else
	{
		fl_temp_U8 = FALSE;
	}
	__enable_irq();

	return fl_temp_U8;
}

/************************************
Put a Can data to Sw Fifo
return: TRUE->OK    FALSE->Fail
************************************/

uint8_t Uartif_tx_queue_push_e(Uartif_Msg_Str fl_str_e)
{
	uint8_t fl_temp_U8;

	if(TRUE == Uartif_tx_queue_is_full())
	{
		return FALSE;
	}
	else
	{
	    __disable_irq();
		fl_temp_U8 = (Uartif_tx_queue.queue_in + 1) % (sizeof(Uartif_tx_queue.queue)/sizeof(Uartif_tx_queue.queue[0]));
		Uartif_tx_queue.queue_in = fl_temp_U8;
		Uartif_tx_queue.queue[fl_temp_U8]  = fl_str_e;
		__enable_irq();
		return TRUE;
	}
}

/************************************
Read Tx Sw Fifo From Queue
************************************/
uint8_t Uartif_tx_queue_pull_e(Uartif_Msg_Str * fl_str_e)
{
	uint8_t fl_temp_U8;

	if(TRUE == Uartif_tx_queue_is_empty())
	{
		return FALSE;
	}
	else
	{
	    __disable_irq();
		fl_temp_U8 = (Uartif_tx_queue.queue_out + 1) % (sizeof(Uartif_tx_queue.queue)/sizeof(Uartif_tx_queue.queue[0]));
		Uartif_tx_queue.queue_out = fl_temp_U8;
		fl_str_e[0] = 	Uartif_tx_queue.queue[fl_temp_U8]        ;
		__enable_irq();

		return TRUE;
	}
}

/************************************
Get Sw Fifo empty condition
************************************/

uint8_t Uartif_rx_queue_is_empty(void)
{
	uint8_t fl_temp_U8;
	__disable_irq();
	if(Uartif_rx_queue.queue_in == Uartif_rx_queue.queue_out)
	{
		fl_temp_U8 = TRUE;
	}
	else
	{
		fl_temp_U8 = FALSE;
	}
	__enable_irq();
	return fl_temp_U8;
}

/************************************
Get Sw Fifo Full condition
************************************/

uint8_t Uartif_rx_queue_is_full(void)
{
	uint8_t fl_temp_U8;
	//__disable_irq();
	fl_temp_U8 = (Uartif_rx_queue.queue_in + 1) % (sizeof(Uartif_rx_queue.queue)/sizeof(Uartif_rx_queue.queue[0]));
	if(fl_temp_U8 == Uartif_rx_queue.queue_out)
	{
		fl_temp_U8 = TRUE;
	}
	else
	{
		fl_temp_U8 = FALSE;
	}
	//__enable_irq();
	return fl_temp_U8;
}


/************************************
Push a element data to queue
************************************/

uint8_t Uartif_rx_queue_push_e(Uartif_Msg_Str fl_str_e)
{
	uint8_t fl_temp_U8;
	
	if(TRUE == Uartif_rx_queue_is_full())
	{
		return FALSE;
	}
	else
	{
		fl_temp_U8 = (Uartif_rx_queue.queue_in + 1) % (sizeof(Uartif_rx_queue.queue)/sizeof(Uartif_rx_queue.queue[0]));
		Uartif_rx_queue.queue_in = fl_temp_U8;
		Uartif_rx_queue.queue[fl_temp_U8]  = fl_str_e;
		return TRUE;
	}
}


/************************************
Get Can Data from Rx Sw Fifo
return: TRUE->OK    FALSE->Fail
************************************/

uint8_t Uartif_rx_queue_pull_e(Uartif_Msg_Str *fl_str_e)
{
	uint8_t fl_temp_U8;

	if(TRUE == Uartif_rx_queue_is_empty())
	{
		return FALSE;
	}
	else
	{
	    __disable_irq();
		fl_temp_U8 = (Uartif_rx_queue.queue_out + 1) % (sizeof(Uartif_rx_queue.queue)/sizeof(Uartif_rx_queue.queue[0]));
		Uartif_rx_queue.queue_out = fl_temp_U8;
		fl_str_e[0] = 	Uartif_rx_queue.queue[fl_temp_U8];
	
		__enable_irq();

		return TRUE;
	}
}

/************************************
uart init
************************************/

void Uartif_Init(void)
{
	memset(&UARTxOp,0,sizeof(UARTxOp));
	memset(&Uartif_rx_queue,0,sizeof(Uartif_rx_queue));
	memset(&Uartif_tx_queue,0,sizeof(Uartif_tx_queue));
	Uart_Init();
	
}

/************************************
Uarif_Task
************************************/
void Uarif_Task(void)
{
	static Uartif_Msg_Str  fl_str_e;

	/*all byte tx done*/
	if(FL_DMA_IsActiveFlag_TransferComplete(DMA, FL_DMA_CHANNEL_6) || UARTxOp.TxOpc == 0)
	{
		/*Software Queue is Not empty*/
		if(TRUE == Uartif_tx_queue_pull_e(&fl_str_e))
		{
			UARTxOp.TxOpc = 0xAA;
			FL_UART_DisableIT_TXShiftBuffEmpty(UARTIF_USE_CHANNEL);
			FL_UART_EnableIT_RXBuffFull(UARTIF_USE_CHANNEL);
			Uartxif_Tx(UARTIF_USE_CHANNEL,fl_str_e);
		}
	}

/*
	if(FL_DMA_IsActiveFlag_TransferComplete(DMA, FL_DMA_CHANNEL_5) || UARTxOp.RxOpc == 0)
	{
		UARTxOp.RxLen = 9;
		SpiReadDataDMA((uint32_t)UARTxOp.RxPool,UARTxOp.RxLen);
		if(UARTxOp.RxOpc==0)
		{UARTxOp.RxOpc=0xAA;}
		else
		{
			fl_str_e.DLC = UARTxOp.RxLen;
			memcpy(fl_str_e.Data,UARTxOp.RxPool,fl_str_e.DLC);
			(void)Uartif_rx_queue_push_e(fl_str_e);
		}
	}
*/
	Uartx_RxFrameComplete_Detect();

}

