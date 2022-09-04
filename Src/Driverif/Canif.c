#include "user_init.h"
#include "fm33lg0xx_fl.h"
#include "fm33lg0xx_fl_can.h"

#include "Canif.h"	
#include "Ioif.h"
#include "tp_cfg.h"
#include "nw_can_dll.h"

#include "il_par.h"
#include "Vnim_busoff_hndlr.h"
#include "Vnim_can_cfg.h"
#include "PowerMgmt.h"

Canif_Msg_Str RxMessage = {0};
Canif_queue_str Canif_rx_queue;
Canif_queue_str Canif_tx_queue;
uint32_t gl_tx_id = 0;


void FL_CAN_HPBUF_Write(uint32_t id, uint32_t len, uint32_t data1, uint32_t data2);
void FL_CAN_FIFO_Read(void);
void Clockchange(void);
void FL_CAN_Frame_Send(uint32_t frame_format, Canif_Msg_Str *TxMessage);

uint8_t Canif_tx_queue_is_empty(void);
uint8_t Canif_tx_queue_is_full(void);
uint8_t Canif_tx_queue_push_e(Canif_Msg_Str fl_str_e);
uint8_t Canif_tx_queue_pull_e(Canif_Msg_Str * fl_str_e);
uint8_t Canif_rx_queue_is_empty(void);
uint8_t Canif_rx_queue_is_full(void);
uint8_t Canif_rx_queue_push_e(Canif_Msg_Str fl_str_e);
uint8_t Canif_rx_queue_pull_e(Canif_Msg_Str *fl_str_e);
static void can_tx_done_callback(void);

/**
  * @brief  CAN中断函数
  * @param  void
  * @retval void
  */
void CAN_IRQHandler(void)       
{
    if((FL_ENABLE == FL_CAN_IsEnabledIT_RXOK(CAN))
            && (FL_SET == FL_CAN_IsActiveFlag_RXOK(CAN)))
    {
        FL_CAN_FIFO_Read();
        FL_CAN_ClearFlag_RXOK(CAN);
		Clean_Receive_Message_Flag();
    }

    if((FL_ENABLE == FL_CAN_IsEnabledIT_TXOK(CAN))
            && (FL_SET == FL_CAN_IsActiveFlag_TXOK(CAN)))
    {
    	FL_CAN_ClearFlag_TXOK(CAN);
    	can_tx_done_callback();
    }
}



/************************************
Comp sleep init
************************************/

void Canif_DeInit(void)
{
	Ioif_SetPinLevel(GPIO_NUMBER_C14_CAN_STB,1);
    FL_CAN_ClearFlag_RXOK(CAN);
	FL_CAN_DisableIT_RXOK(CAN);      /* 接收中断使能 */
    NVIC_DisableIRQ(CAN_IRQn);
}

/**
  * @brief  更改系统主频为48M,外部晶体XTHF
            XTHF的全温区的误差更小
  * @param  void
  * @retval void
  */
void Clockchange(void)
{
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* PC2.3配置成模拟功能，外接XTHF */

    GPIO_InitStruct.pin = FL_GPIO_PIN_2 | FL_GPIO_PIN_3;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* 使能XTHF */
    FL_CMU_XTHF_Enable();
    FL_CMU_XTHF_WriteDriverStrength(0x1F);/* 振荡强度最大 */
    FL_DelayMs(3);

    //    /* FLASH 等待周期为1 */
    //    FL_FLASH_SetReadWait(FLASH, FL_FLASH_READ_WAIT_1CYCLE);
    //
    //    /* PLL配置 */
    //
    //    FL_CMU_PLL_SetClockSource(FL_CMU_PLL_CLK_SOURCE_XTHF);
    //    FL_CMU_PLL_SetPrescaler(FL_CMU_PLL_PSC_DIV8); /* 8分频为1M */
    //    FL_CMU_PLL_WriteMultiplier(0x2F);             /* 倍频48M */
    //    FL_CMU_PLL_Enable();
    //
    //    while(!FL_CMU_IsActiveFlag_PLLReady());       /* 等待建立 */

    FL_CMU_SetSystemClockSource(FL_CMU_SYSTEM_CLK_SOURCE_XTHF);
    FL_CMU_SetAHBPrescaler(FL_CMU_AHBCLK_PSC_DIV1);
    FL_CMU_SetAPBPrescaler(FL_CMU_APBCLK_PSC_DIV1);

}


/************************************
Comp poweron init
************************************/
void Canif_Init(void)
{
    FL_CAN_InitTypeDef       CAN_InitStructure = {0};
    FL_CAN_FilterInitTypeDef CAN_FilterInitStructure = {0};

	Ioif_SetPinLevel(GPIO_NUMBER_C14_CAN_STB,0);

	
	Canif_rx_queue.queue_in = 0;
	Canif_rx_queue.queue_out = 0;
	Canif_tx_queue.queue_in =0;
	Canif_tx_queue.queue_out=0;
	gl_tx_id = 0;
	
   
    /*----------------------------CAN结构体初始化--------------------------------------*/
    /* 波特率设置= CAN_CLK/(BRP+1)/(TS1_Tq+TS2_Tq+1);  8M/(0+1)/(9+6+1)=500K */
    CAN_InitStructure.TS1 = FL_CAN_TS1_9Tq;
    CAN_InitStructure.TS2 = FL_CAN_TS2_6Tq;                    /* 位时序设置 */
    CAN_InitStructure.SJW = FL_CAN_SJW_1Tq;
    CAN_InitStructure.BRP = 0;                                 /* 波特率预分频 */
    CAN_InitStructure.mode = FL_CAN_MODE_NORMAL;               /* 工作模式设置 */
    CAN_InitStructure.clockSource = FL_CMU_CAN_CLK_SOURCE_XTHF;/* 时钟源设置 */
    FL_CAN_Init(CAN, &CAN_InitStructure);
    /*----------------------------接收滤波结构体初始化--------------------------------------*/
#if (SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
    CAN_FilterInitStructure.filterIdExtend = 0X741;       /* 扩展ID */
	CAN_FilterInitStructure.filterIdStandard = 0X741; 	 

    CAN_FilterInitStructure.filterIdSRR = 0X01;
    CAN_FilterInitStructure.filterIdIDE = 0X01;
    CAN_FilterInitStructure.filterIdRTR = 0X00;

    CAN_FilterInitStructure.filterMaskIdHigh = 0X761;
    CAN_FilterInitStructure.filterMaskIdLow = 0X3FFFF;
    CAN_FilterInitStructure.filterMaskIdSRR = 0X00;
    CAN_FilterInitStructure.filterMaskIdIDE = 0X01;            /* 滤波器掩码,1：该位参与滤波器比较，0：不参与 */
    CAN_FilterInitStructure.filterMaskIdRTR = 0x01;
    CAN_FilterInitStructure.filterEn = FL_DISABLE;
    FL_CAN_FilterInit(CAN, &CAN_FilterInitStructure, FL_CAN_FILTER1);


    CAN_FilterInitStructure.filterIdExtend = 0X020;       /* 扩展ID */
	CAN_FilterInitStructure.filterIdStandard = 0X020; 	 
    CAN_FilterInitStructure.filterIdSRR = 0X01;
    CAN_FilterInitStructure.filterIdIDE = 0X01;
    CAN_FilterInitStructure.filterIdRTR = 0X00;

    CAN_FilterInitStructure.filterMaskIdHigh = 0X468;
    CAN_FilterInitStructure.filterMaskIdLow = 0X3FFFF;
    CAN_FilterInitStructure.filterMaskIdSRR = 0X00;
    CAN_FilterInitStructure.filterMaskIdIDE = 0X00;            /* 滤波器掩码,1：该位参与滤波器比较，0：不参与 */
    CAN_FilterInitStructure.filterMaskIdRTR = 0x00;
    CAN_FilterInitStructure.filterEn = FL_DISABLE;
    FL_CAN_FilterInit(CAN, &CAN_FilterInitStructure, FL_CAN_FILTER2);

    CAN_FilterInitStructure.filterIdExtend = 0X458;       /* 扩展ID */
	CAN_FilterInitStructure.filterIdStandard = 0X458; 	 
    CAN_FilterInitStructure.filterIdSRR = 0X01;
    CAN_FilterInitStructure.filterIdIDE = 0X01;
    CAN_FilterInitStructure.filterIdRTR = 0X00;

    CAN_FilterInitStructure.filterMaskIdHigh = 0X7FF;
    CAN_FilterInitStructure.filterMaskIdLow = 0X3FFFF;
    CAN_FilterInitStructure.filterMaskIdSRR = 0X00;
    CAN_FilterInitStructure.filterMaskIdIDE = 0X00;            /* 滤波器掩码,1：该位参与滤波器比较，0：不参与 */
    CAN_FilterInitStructure.filterMaskIdRTR = 0x00;
    CAN_FilterInitStructure.filterEn = FL_DISABLE;
    FL_CAN_FilterInit(CAN, &CAN_FilterInitStructure, FL_CAN_FILTER3);

#else

	CAN_FilterInitStructure.filterIdExtend = 0X600; 	  /* 扩展ID */
	CAN_FilterInitStructure.filterIdStandard = 0X600; 	 
	CAN_FilterInitStructure.filterIdSRR = 0X01;
	CAN_FilterInitStructure.filterIdIDE = 0X01;
	CAN_FilterInitStructure.filterIdRTR = 0X00;

	CAN_FilterInitStructure.filterMaskIdHigh = 0X600;
	CAN_FilterInitStructure.filterMaskIdLow = 0X3FFFF;
	CAN_FilterInitStructure.filterMaskIdSRR = 0X00;
	CAN_FilterInitStructure.filterMaskIdIDE = 0X00; 		   /* 滤波器掩码,1：该位参与滤波器比较，0：不参与 */
	CAN_FilterInitStructure.filterMaskIdRTR = 0x00;
	CAN_FilterInitStructure.filterEn = FL_DISABLE;
	FL_CAN_FilterInit(CAN, &CAN_FilterInitStructure, FL_CAN_FILTER3);
	FL_CAN_FilterInit(CAN, &CAN_FilterInitStructure, FL_CAN_FILTER4);


	CAN_FilterInitStructure.filterIdExtend = 0X045; 	  /* 扩展ID */
	CAN_FilterInitStructure.filterIdStandard = 0X045; 	  
	CAN_FilterInitStructure.filterIdSRR = 0X01;
	CAN_FilterInitStructure.filterIdIDE = 0X01;
	CAN_FilterInitStructure.filterIdRTR = 0X00;

	CAN_FilterInitStructure.filterMaskIdHigh = 0X1ED;
	CAN_FilterInitStructure.filterMaskIdLow = 0X3FFFF;
	CAN_FilterInitStructure.filterMaskIdSRR = 0X00;
	CAN_FilterInitStructure.filterMaskIdIDE = 0X01; 		   /* 滤波器掩码,1：该位参与滤波器比较，0：不参与 */
	CAN_FilterInitStructure.filterMaskIdRTR = 0x01;
	CAN_FilterInitStructure.filterEn = FL_DISABLE;
	FL_CAN_FilterInit(CAN, &CAN_FilterInitStructure, FL_CAN_FILTER1);

	CAN_FilterInitStructure.filterIdExtend = 0x781; 	  /* 扩展ID */
	CAN_FilterInitStructure.filterIdStandard = 0x781; 	 
	CAN_FilterInitStructure.filterIdSRR = 0X01;
	CAN_FilterInitStructure.filterIdIDE = 0X01;
	CAN_FilterInitStructure.filterIdRTR = 0X00;

	CAN_FilterInitStructure.filterMaskIdHigh = 0x781;
	CAN_FilterInitStructure.filterMaskIdLow = 0X3FFFF;
	CAN_FilterInitStructure.filterMaskIdSRR = 0X00;
	CAN_FilterInitStructure.filterMaskIdIDE = 0X00; 		   /* 滤波器掩码,1：该位参与滤波器比较，0：不参与 */
	CAN_FilterInitStructure.filterMaskIdRTR = 0x00;
	CAN_FilterInitStructure.filterEn = FL_DISABLE;
	FL_CAN_FilterInit(CAN, &CAN_FilterInitStructure, FL_CAN_FILTER2);

#endif


    FL_CAN_ClearFlag_RXOK(CAN);
    FL_CAN_EnableIT_RXOK(CAN);      /* 接收中断使能 */
    FL_CAN_EnableIT_TXOK(CAN);

    NVIC_DisableIRQ(CAN_IRQn);
    NVIC_SetPriority(CAN_IRQn, 2);  /* 中断优先级配置 */
    NVIC_EnableIRQ(CAN_IRQn);

	

}

/**
  * @brief  通过TX FIFO发送数据
  * @param  CANx
  * @retval None
  */
void FL_CAN_FIFO_Write(uint32_t id, uint32_t len, uint32_t data1, uint32_t data2)
{
    //uint32_t i=5;
    /* 状态标志为0时，才能清0中断标志 */
    /*while((FL_CAN_IsActiveFlag_TXBuffFullSignal(CAN) != FL_RESET)&&i)
    {
        i--;
       FL_DelayMs(1);  
    }*/
    FL_CAN_ClearFlag_TXBuffFull(CAN);

    FL_CAN_WriteTXMessageID(CAN, id);
    FL_CAN_WriteTXMessageLength(CAN, len);
    FL_CAN_WriteTXMessageWord1(CAN, data1);
    FL_CAN_WriteTXMessageWord2(CAN, data2);
}

/**
  * @brief  通过HPB发送数据
  * @param  CANx
  * @retval None
  */
void FL_CAN_HPBUF_Write(uint32_t id, uint32_t len, uint32_t data1, uint32_t data2)
{
    uint32_t i=5;
    while((FL_CAN_IsActiveFlag_TXHighPriorBuffFullSignal(CAN) != FL_RESET)&&i)
    {
        i--;
       FL_DelayMs(1);  
    }
    FL_CAN_ClearFlag_TXHighPriorBuffFull(CAN);

    FL_CAN_WriteHighPriorTXMessageID(CAN, id);
    FL_CAN_WriteHighPriorMessageLength(CAN, len);
    FL_CAN_WriteHighPriorMessageWord1(CAN, data1);
    FL_CAN_WriteHighPriorMessageWord2(CAN, data2);
}



/**
  * @brief  读RX FIFO数据
  * @param  CANx
  * @retval  None
  */
void FL_CAN_FIFO_Read(void)
{

    uint32_t data1 = 0;
    uint32_t data2 = 0;
    //uint32_t real_id;

    RxMessage.StdId = FL_CAN_ReadRXMessageID(CAN);
	data1 = RxMessage.StdId ;
	data2 = RxMessage.StdId ;
	
	data1 = (data1 << 18) & 0x1FFC0000;
	data2 = (data2 >> 13) & 0x3FFFF;
	RxMessage.StdId = data1 | data2;
	
    RxMessage.DLC = FL_CAN_ReadRXMessageLength(CAN);

    data1 = FL_CAN_ReadRXMessageWord1(CAN);
    data2 = FL_CAN_ReadRXMessageWord2(CAN);

    RxMessage.Data[0] = (uint8_t)data1 & 0xff;
    RxMessage.Data[1] = (uint8_t)(data1 >> 8) & 0xff;
    RxMessage.Data[2] = (uint8_t)(data1 >> 16) & 0xff;
    RxMessage.Data[3] = (uint8_t)(data1 >> 24) & 0xff;
    RxMessage.Data[4] = (uint8_t)data2 & 0xff;
    RxMessage.Data[5] = (uint8_t)(data2 >> 8) & 0xff;
    RxMessage.Data[6] = (uint8_t)(data2 >> 16) & 0xff;
    RxMessage.Data[7] = (uint8_t)(data2 >> 24) & 0xff;
    
    //real_id = ((RxMessage.ExtId & 0x7FF) << 18) + ((RxMessage.ExtId >> 13) & 0x3FFFF);
	Canif_rx_queue_push_e(RxMessage);
}

/**
  * @brief  帧模式选择
  * @param  CANx
  * @retval  None
  */
void FL_CAN_Frame_Send(uint32_t frame_format, Canif_Msg_Str *TxMessage)
{

    uint32_t id = 0;
    uint32_t len = 0;
    uint32_t data1 = 0;
    uint32_t data2 = 0;

    if(frame_format == FL_CAN_FORMAT_STANDARD_DATA) /* 标准数据帧 */
    {
        id = TxMessage->StdId & 0x7ff;
    }
	else if(frame_format == FL_CAN_FORMAT_STANDARD_REMOTE) /* 标准远程帧 */
	{
		id = (TxMessage->StdId & 0x7ff) | (1 << 11);
	}
	else if(frame_format == FL_CAN_FORMAT_EXTEND_DATA) /* 扩展数据帧 */
	{
		id = ((TxMessage->ExtId & 0x3ffff) << 13) | ((uint32_t)1 << 12) | ((uint32_t)1 << 11) | ((TxMessage->ExtId & 0x1ffc0000) >> 18);
	}
	else if(frame_format == FL_CAN_FORMAT_EXTEND_REMOTE) /* 扩展远程帧 */
	{
	    id = ((TxMessage->ExtId & 0x3ffff) << 13) | ((uint32_t)1 << 12) | ((uint32_t)1 << 31) | ((TxMessage->ExtId & 0x1ffc0000) >> 18);
	}


    len = TxMessage->DLC;
    data1 = (((uint32_t)TxMessage->Data[3] << 24) |
             ((uint32_t)TxMessage->Data[2] << 16) |
             ((uint32_t)TxMessage->Data[1] << 8) |
             ((uint32_t)TxMessage->Data[0]));
    data2 = (((uint32_t)TxMessage->Data[7] << 24) |
             ((uint32_t)TxMessage->Data[6] << 16) |
             ((uint32_t)TxMessage->Data[5] << 8) |
             ((uint32_t)TxMessage->Data[4]));

    FL_CAN_FIFO_Write(id, len, data1, data2);
    //FL_CAN_HPBUF_Write(id,len,data1,data2);    /* 高优先级FIFO */
}




/************************************
Get Sw Fifo Empty condition
************************************/
uint8_t Canif_tx_queue_is_empty(void)
{
	uint8_t fl_temp_U8;
	
	__disable_irq();
	if(Canif_tx_queue.queue_in == Canif_tx_queue.queue_out)
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

uint8_t Canif_tx_queue_is_full(void)
{
	uint8_t fl_temp_U8;
	__disable_irq();
	fl_temp_U8 = (Canif_tx_queue.queue_in + 1) % (sizeof(Canif_tx_queue.queue)/sizeof(Canif_tx_queue.queue[0]));
	if(fl_temp_U8 == Canif_tx_queue.queue_out)
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

uint8_t Canif_tx_queue_push_e(Canif_Msg_Str fl_str_e)
{
	uint8_t fl_temp_U8;

	if(TRUE == Canif_tx_queue_is_full())
	{
		return FALSE;
	}
	else
	{
	    __disable_irq();
		fl_temp_U8 = (Canif_tx_queue.queue_in + 1) % (sizeof(Canif_tx_queue.queue)/sizeof(Canif_tx_queue.queue[0]));
		Canif_tx_queue.queue_in = fl_temp_U8;
		Canif_tx_queue.queue[fl_temp_U8]  = fl_str_e;
		__enable_irq();
		return TRUE;
	}
}

/************************************
Read Tx Sw Fifo From Queue
************************************/
uint8_t Canif_tx_queue_pull_e(Canif_Msg_Str * fl_str_e)
{
	uint8_t fl_temp_U8;

	if(TRUE == Canif_tx_queue_is_empty())
	{
		return FALSE;
	}
	else
	{
	    __disable_irq();
		fl_temp_U8 = (Canif_tx_queue.queue_out + 1) % (sizeof(Canif_tx_queue.queue)/sizeof(Canif_tx_queue.queue[0]));
		Canif_tx_queue.queue_out = fl_temp_U8;
		fl_str_e[0] = 	Canif_tx_queue.queue[fl_temp_U8]        ;
		__enable_irq();

		return TRUE;
	}
}

/************************************
Get Sw Fifo empty condition
************************************/

uint8_t Canif_rx_queue_is_empty(void)
{
	uint8_t fl_temp_U8;
	__disable_irq();
	if(Canif_rx_queue.queue_in == Canif_rx_queue.queue_out)
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

uint8_t Canif_rx_queue_is_full(void)
{
	uint8_t fl_temp_U8;
	//__disable_irq();
	fl_temp_U8 = (Canif_rx_queue.queue_in + 1) % (sizeof(Canif_rx_queue.queue)/sizeof(Canif_rx_queue.queue[0]));
	if(fl_temp_U8 == Canif_rx_queue.queue_out)
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

uint8_t Canif_rx_queue_push_e(Canif_Msg_Str fl_str_e)
{
	uint8_t fl_temp_U8;
	
	CanAppHookReceive();
	
	if(TRUE == Canif_rx_queue_is_full())
	{
		return FALSE;
	}
	else
	{
		fl_temp_U8 = (Canif_rx_queue.queue_in + 1) % (sizeof(Canif_rx_queue.queue)/sizeof(Canif_rx_queue.queue[0]));
		Canif_rx_queue.queue_in = fl_temp_U8;
		Canif_rx_queue.queue[fl_temp_U8]  = fl_str_e;

		
		return TRUE;
	}
}


/************************************
Get Can Data from Rx Sw Fifo
return: TRUE->OK    FALSE->Fail
************************************/

uint8_t Canif_rx_queue_pull_e(Canif_Msg_Str *fl_str_e)
{
	uint8_t fl_temp_U8;

	if(TRUE == Canif_rx_queue_is_empty())
	{
		return FALSE;
	}
	else
	{
	    __disable_irq();
		fl_temp_U8 = (Canif_rx_queue.queue_out + 1) % (sizeof(Canif_rx_queue.queue)/sizeof(Canif_rx_queue.queue[0]));
		Canif_rx_queue.queue_out = fl_temp_U8;
		fl_str_e[0] = 	Canif_rx_queue.queue[fl_temp_U8];
	
		__enable_irq();

		return TRUE;
	}
}


/********************************************
Canif_task: 
Tx can data to can controller,
read can data from software data 
********************************************/

void Canif_task(void)
{
	static Canif_Msg_Str  fl_str_e;
	/*Can Fifo is not full*/
	if(FL_CAN_IsActiveFlag_TXBuffFullSignal(CAN) == FL_RESET)
	{
		/*Software Queue is Not empty*/
		if(TRUE == Canif_tx_queue_pull_e(&fl_str_e))
		{
			FL_CAN_Frame_Send(FL_CAN_FORMAT_EXTEND_DATA,&fl_str_e);

			gl_tx_id = fl_str_e.StdId;
		}
	}

	if(TRUE == FL_CAN_IsActiveFlag_RXOverflow(CAN))
	{
		__disable_irq();
		FL_CAN_FIFO_Read();
		__enable_irq();
		FL_CAN_ClearFlag_RXOverflow(CAN);
		
	}

	/* CAN Bus Off */
	if( TRUE == FL_CAN_IsActiveFlag_BusOff(CAN) )
	{
		//Appl_CAN_busoff_start();
	}
}


static void can_tx_done_callback(void)
{
	if(gl_tx_id == VTP_DIAG_RESPONSE_ID)
	{
		CanAppHookTransmit(VTP_DIAG_MESSAGE_HANDLE,0);
	}
	else
	{
	    CanAppHookTransmit(0,0);
	}

}


