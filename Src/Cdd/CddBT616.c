/*******************************************************************************
|  File Name:  CddBT161.c
|  Description:  Implementation of the Swtich Management
|-------------------------------------------------------------------------------
|******************************************************************************/

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "user_init.h"
#include "uartif.h"
#include "CddBT616.h"
#include "CddBT616_Cfg.h"
#include "ButtonCtrl.h"
#include "IoIf.h"
#include "lin.h"
/*******************************************************************************
|    Macro Definition
|******************************************************************************/



/*******************************************************************************
|    Enum Definition
|******************************************************************************/

/*******************************************************************************
|    Typedef Definition
|******************************************************************************/

/*******************************************************************************
|    Global variables Declaration
|******************************************************************************/

/*******************************************************************************
|    Static local variables Declaration
|******************************************************************************/
CddBT616_Main_Ctrl_Str CddBT616_Main_Ctrl;



/*******************************************************************************
|    Table Definition
|******************************************************************************/

const uint8_t BT_cmd_to_Button_ID[][3] = 
{
	0xA0, 0  , 0,
	0xA1, BTN_ID_CTRL_MODE_ZERO_GRAVITY_e  , 0,
	0xA2, BTN_ID_CTRL_MODE_DRIVERIESS_CAR_e  , 0,
	0xA3, BTN_ID_CTRL_MODE_MEETING_e  , 0,
	0xA4, BTN_ID_CTRL_MODE_LEISURE_e  , 0,
	0xA5, BTN_ID_CTRL_LED_e  , 0,

	0xB0, 0  , 0,
	0xB1, BTN_ID_CTRL_POS_FRONT_REAR_e  , DIRECTION_FRONT,
	0xB2, BTN_ID_CTRL_BACK_ANGLE_e      , DIRECTION_FRONT,
	0xB3, BTN_ID_CTRL_ROTATE_e          , DIRECTION_FRONT,
	0xB4, 0  , 0,
	0xB5, BTN_ID_CTRL_HEAD_e  , DIRECTION_FRONT,

	0xC0, 0  , 0,
	0xC1, BTN_ID_CTRL_POS_FRONT_REAR_e  , DIRECTION_REAR,
	0xC2, BTN_ID_CTRL_BACK_ANGLE_e  , DIRECTION_REAR,
	0xC3, BTN_ID_CTRL_ROTATE_e  , DIRECTION_REAR,
	0xC4, 0  , 0,
	0xC5, BTN_ID_CTRL_HEAD_e  , DIRECTION_REAR,

	0xE0, BTN_ID_CTRL_VENTILITION_e  , 0,
	0xE1, BTN_ID_CTRL_VENTILITION_e  , 1,
	0xE2, BTN_ID_CTRL_VENTILITION_e  , 2,
	0xE3, BTN_ID_CTRL_VENTILITION_e  , 3,

	0xD0, BTN_ID_CTRL_MASSAGE_e  , 0,
	0xD1, BTN_ID_CTRL_MASSAGE_e  , 1,
	0xD2, BTN_ID_CTRL_MASSAGE_e  , 2,
	0xD3, BTN_ID_CTRL_MASSAGE_e  , 3,
	
};


/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */


/* ====================================  Functions declaration  ===================================== */


ButtonCtrl_Str CddBT616_Search_Btn(uint8_t cmd)
{
	uint8_t index = 0;
	ButtonCtrl_Str btn;
	
	btn.ButtonId = 0xFF;
	btn.ButtonVal =0x0;
	
	for(index = 0 ; index < (sizeof(BT_cmd_to_Button_ID)/sizeof(BT_cmd_to_Button_ID[0])); index ++ )
	{
		if(cmd == BT_cmd_to_Button_ID[index][0])
		{
			btn.ButtonId = BT_cmd_to_Button_ID[index][1];
			btn.ButtonVal = BT_cmd_to_Button_ID[index][2];
		}
	}

	return btn;
	
}


uint8_t CddBT616_Cal_Parity(uint8_t * data , uint8_t len)
{
	uint8_t parity = 0;
	uint8_t index = 0;

	for(index = 0; index < len ; index ++)
	{
		parity = data[index] ^ parity;
	}

	return parity;
}

uint8_t CddBT616_Check_Parity(uint8_t * data , uint8_t num)
{
	uint8_t parity = 0;
	uint8_t index = 0;

	
	for(index = 0; index < num ; index ++)
	{
		parity = data[index] ^ parity;
	}
	
	return parity;
}


uint8_t  CddBT616_Check_RxFormat(uint8_t * data)
{

	uint8_t index = 0;
	uint8_t parity = 0;
	
	if(0x57 != data[index++])
	{
		return FALSE;
	}

	if(0x78 != data[index++])
	{
		return FALSE;
	}

	if(0xA0 > data[index++])
	{
		return FALSE;
	}

	if(0x00 != data[index++])
	{
		return FALSE;
	}

	if(0x00 != data[index++])
	{
		return FALSE;
	}
	
	if(0xFF != data[index++])
	{
		return FALSE;
	}

	if(0xFF != data[index++])
	{
		return FALSE;
	}
	
	if(0xFF != data[index++])
	{
		return FALSE;
	}
	
	parity = CddBT616_Check_Parity(data,8);

	if(parity != data[index++])
	{
		return FALSE;
	}
	
	return TRUE;
	
}


/************************************
CddTest_init

************************************/

void CddBT616_init(void)
{
	uint8_t index = 0;

	Ioif_SetPinLevel(GPIO_NUMBER_E3_BLE_RESET,TRUE);
	memset(&CddBT616_Main_Ctrl,0,sizeof(CddBT616_Main_Ctrl)); 
	CddBT616_Main_Ctrl.txst.DLC = 18;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0x75;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0x78;
	
	CddBT616_Main_Ctrl.txst.Data[index++] = 0xF0;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0x00;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0x09;
	
	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;

	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;

	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;

	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;
	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;

	CddBT616_Main_Ctrl.txst.Data[index] = CddBT616_Cal_Parity(CddBT616_Main_Ctrl.txst.Data,index);
	
	Uart_Init();
}

/************************************
CddBT616_Set_Signal
************************************/

void CddBT616_Set_Pos(uint8_t MotorId,uint8_t Pos)
{
	if(MotorId < 8)
	{
		CddBT616_Main_Ctrl.txst.Data[MotorId] = Pos;
	}
}

/************************************
CddTest_Task

************************************/

void CddBT616_Task(void)
{
	Uartif_Msg_Str fl_str_e;
	uint8_t ret ;
	ButtonCtrl_Str btn;

	if(CddBT616_Main_Ctrl.delayticks > CDDBT616_INIT_DELAY)
	{
		if(BT_Connected())
		{
			if(CddBT616_Main_Ctrl.State < BT616_WORK_CHECK_CONNECT)
			{
				CddBT616_Main_Ctrl.State = BT616_WORK_CHECK_CONNECT;
			}
		}
	}
	else
	{
		CddBT616_Main_Ctrl.delayticks++;
	}


	switch(CddBT616_Main_Ctrl.State)
	{
		case BT616_WORK_TEST_TX:
		{
			fl_str_e.DLC = CddBT616_Cmd_Table[BT616_CMD_AT][0].tx_len;
			memcpy(fl_str_e.Data ,CddBT616_Cmd_Table[BT616_CMD_AT][0].tx_ptr,fl_str_e.DLC);
			Uartif_tx_queue_push_e(fl_str_e);
			CddBT616_Main_Ctrl.State = BT616_WORK_TEST_RX;
			CddBT616_Main_Ctrl.statedelayticks = CDDBT616_STATE_DELAY;
		}
		break;
			
		case BT616_WORK_TEST_RX:
		{
			if(TRUE == Uartif_rx_queue_pull_e(&fl_str_e))
			{
				if(FALSE == memcmp(fl_str_e.Data ,CddBT616_Cmd_Table[BT616_CMD_AT][0].rx_ptr,CddBT616_Cmd_Table[BT616_CMD_AT][0].rx_len))
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_READ_NAME;
				}
			}
			else
			{
				if(CddBT616_Main_Ctrl.statedelayticks == 0)
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_TEST_TX;
				}
				else
				{
					CddBT616_Main_Ctrl.statedelayticks --;
				}
			}
		}
		break;
				
		case BT616_WORK_READ_NAME:
		{
			fl_str_e.DLC = CddBT616_Cmd_Table[BT616_CMD_NAME][0].tx_len;
			memcpy(fl_str_e.Data ,CddBT616_Cmd_Table[BT616_CMD_NAME][0].tx_ptr,fl_str_e.DLC);
			Uartif_tx_queue_push_e(fl_str_e);
			CddBT616_Main_Ctrl.State = BT616_WORK_CHECK_NAME;
			CddBT616_Main_Ctrl.statedelayticks = CDDBT616_STATE_DELAY;
		}
		break;

		case BT616_WORK_CHECK_NAME:
		{
			if(TRUE == Uartif_rx_queue_pull_e(&fl_str_e))
			{
				if(FALSE == memcmp(fl_str_e.Data ,CddBT616_Cmd_Table[BT616_CMD_NAME][0].rx_ptr,CddBT616_Cmd_Table[BT616_CMD_NAME][0].rx_len))
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_CHECK_CONNECT;
				}
				else
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_WRITE_NAME;
				}
			}
			else
			{
				if(CddBT616_Main_Ctrl.statedelayticks == 0)
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_TEST_TX;
				}
				else
				{
					CddBT616_Main_Ctrl.statedelayticks --;
				}
			}
		}
		break;
			
		case BT616_WORK_WRITE_NAME:
		{
			fl_str_e.DLC = CddBT616_Cmd_Table[BT616_CMD_WNAME][0].tx_len;
			memcpy(fl_str_e.Data ,CddBT616_Cmd_Table[BT616_CMD_WNAME][0].tx_ptr,fl_str_e.DLC);
			Uartif_tx_queue_push_e(fl_str_e);
			CddBT616_Main_Ctrl.State = BT616_WORK_W_CHECK_NAME;
			CddBT616_Main_Ctrl.statedelayticks = CDDBT616_STATE_DELAY;
		}
		break;

		case BT616_WORK_W_CHECK_NAME:
		{
			if(TRUE == Uartif_rx_queue_pull_e(&fl_str_e))
			{
				if(FALSE == memcmp(fl_str_e.Data ,CddBT616_Cmd_Table[BT616_CMD_AT][0].rx_ptr,CddBT616_Cmd_Table[BT616_CMD_AT][0].rx_len))
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_CHECK_CONNECT;
				}
				else
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_WRITE_NAME;
				}
			}
			else
			{
				if(CddBT616_Main_Ctrl.statedelayticks == 0)
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_TEST_TX;
				}
				else
				{
					CddBT616_Main_Ctrl.statedelayticks --;
				}
			}
		}
		break;
			
		case BT616_WORK_CHECK_CONNECT:
		{
			if(BT_Connected())
			{
				CddBT616_Main_Ctrl.State = BT616_WORK_COM;
			}
			CddBT616_Main_Ctrl.Txticks = 0;
		}
		break;

		case BT616_WORK_COM:
		{
			if(!BT_Connected())
			{
				CddBT616_Main_Ctrl.State = BT616_WORK_CHECK_CONNECT;
			}
			else
			{
				if(CddBT616_Main_Ctrl.Txticks++ > CDDBT616_TX_PERIOD)
				{					
					CddBT616_Main_Ctrl.Txticks = 0;
					memcpy(&fl_str_e,&CddBT616_Main_Ctrl.txst,sizeof(fl_str_e));
					fl_str_e.Data[17] = CddBT616_Cal_Parity(fl_str_e.Data,17);
					Uartif_tx_queue_push_e(fl_str_e);
				}
				
				if(TRUE == Uartif_rx_queue_pull_e(&fl_str_e))
				{
					if(fl_str_e.DLC == 9)
					{
						ret = CddBT616_Check_RxFormat(fl_str_e.Data);
						if(ret == TRUE)
						{
							btn = CddBT616_Search_Btn(fl_str_e.Data[2]);
							ButtonCtrl_queue_push_e(btn);
						}
					}
				}
			}
		}
		break;
		
		default:
			CddBT616_Main_Ctrl.State = BT616_WORK_TEST_TX;
		break;
	}


}


void CddBT616_Set_Signal(CddBT616_Signal_Id_e Id,uint8_t val)
{
	if(Id < MAX_ID)
	{
		CddBT616_Main_Ctrl.txst.Data[Id+ID_OFFSET]  = val;
	}
}


/**********************************************************************************************************************
 *  END OF FILE
***********************************************************************************************************************/

/*EOF*/
