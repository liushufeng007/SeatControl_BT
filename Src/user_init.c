#include "user_init.h"
#include "Ioif.h"
#include "Adcif.h"
#include "Compif.h"
#include "Canif.h"
#include "CddKey.h"
#include "CddEeprom.h"
#include "Iwdt.h"
#include "CddMtr_Mng.h"
#include "CddTest.h"
#include "CddModeM.h"

#include "PowerMgmt.h"
#include "rx_can.h"
#include "Scm_SeatControl.h"
#include "diag_kwp2k_di_core_mgr.h"

#include "CanNm.h"

#include "CddBT616.h"
#include "lin.h"
#include "ButtonCtrl.h"

#include "CddLed.h"

void UserInit(void)
{
	
#ifdef DEBUG_ENABLE
	//SCB->VTOR = DEBUG_START_ADDR;
#else
	SCB->VTOR = APP_START_ADDR;//重定向中断向量表
#endif
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    /* SHOULD BE KEPT!!! */
    MF_Clock_Init();

    FL_Init();
    MF_SystemClock_Config();
	CddEeprom_Init();
	Ioif_GPIO_Init();
	Adcif_Init();
	Compif_Init();
	Canif_Init();
	/*Init_CddKey();*/
	CddMtr_Mng_Init();
	IWDT_Init();
	CddModeM_Init();
	CddTest_init();

	
    PowerModeInit();
    Message_variable_init();
    Scm_SeatControl_Init();

	CanNm_Init();
    DiagColdInit();

	CddBT616_init();
	lin_init();
	ButtonCtrlInit();
	Init_CddLed();
/*	DebugUartInit();*/
	
}


//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
    FL_UART_WriteTXBuff(UART0, (uint8_t)ch);
    while(FL_UART_IsActiveFlag_TXBuffEmpty(UART0) != FL_SET);
    return ch;
}

void DebugUartInit(void)
{
     FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    FL_UART_InitTypeDef UART_InitStruct = {0};   

    //PA13:UART0-RX   PA14:UART0-TX
    GPIO_InitStruct.pin        = FL_GPIO_PIN_2|FL_GPIO_PIN_3;
    GPIO_InitStruct.mode       = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;   
    GPIO_InitStruct.pull       = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    UART_InitStruct.clockSrc = FL_CMU_UART0_CLK_SOURCE_APBCLK;
	
    UART_InitStruct.baudRate            = 115200;							        //波特率
    UART_InitStruct.dataWidth           = FL_UART_DATA_WIDTH_8B;			      	//数据位数
    UART_InitStruct.stopBits            = FL_UART_STOP_BIT_WIDTH_1B;				//停止位
    UART_InitStruct.parity              = FL_UART_PARITY_EVEN;				        //奇偶校验
    UART_InitStruct.transferDirection   = FL_UART_DIRECTION_TX_RX;	                //接收-发送使能
    FL_UART_Init(UART4, &UART_InitStruct);	
}

