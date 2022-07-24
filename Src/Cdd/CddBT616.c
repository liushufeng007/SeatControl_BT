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



/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */


/* ====================================  Functions declaration  ===================================== */
void CddBT616_Push_Button(CddBT616_KEY_VAL_e fl_key);


/************************************
CddTest_init

************************************/

void CddBT616_init(void)
{
	uint8_t index = 0;
	memset(&CddBT616_Main_Ctrl,0,sizeof(CddBT616_Main_Ctrl)); 
	CddBT616_Main_Ctrl.txst.Data[index++] = TRUE;//battery1 vol ,unit = 0.1V
	CddBT616_Main_Ctrl.txst.Data[index++] = TRUE;//battery2 vol ,unit = 0.1V
	CddBT616_Main_Ctrl.txst.Data[index++] = 0;//Bit0-bit7,TRUE is motor n hall fault
	CddBT616_Main_Ctrl.txst.Data[index++] = 0xFF;//Bit0-bit7,TRUE is motor learned
	CddBT616_Main_Ctrl.txst.Data[index++] = 0;//motor0 position
	CddBT616_Main_Ctrl.txst.Data[index++] = 1;//motor1 position
	CddBT616_Main_Ctrl.txst.Data[index++] = 2;//motor2 position
	CddBT616_Main_Ctrl.txst.Data[index++] = 3;//motor3 position
	CddBT616_Main_Ctrl.txst.Data[index++] = 4;//motor4 position
	CddBT616_Main_Ctrl.txst.Data[index++] = 5;//motor5 position
	CddBT616_Main_Ctrl.txst.Data[index++] = 6;//motor6 position
	CddBT616_Main_Ctrl.txst.Data[index++] = 7;//motor7 position
	CddBT616_Main_Ctrl.txst.DLC = index;
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
CddBT616_Set_BatVol
************************************/

void CddBT616_Set_BatVol(uint8_t bat1_vol,uint8_t bat2_vol)
{
	uint8_t index = 0;
	CddBT616_Main_Ctrl.txst.Data[index++] = bat1_vol;//battery1 vol ,unit = 0.1V
	CddBT616_Main_Ctrl.txst.Data[index++] = bat2_vol;//battery2 vol ,unit = 0.1V
}
/************************************
CddBT616_Set_BatVol
************************************/

void CddBT616_Set_HallFault(uint8_t MotorId,uint8_t fault)
{
	uint8_t index = 2,flag= 1;
	if(!fault)
	{
		flag=0;
	}
	CddBT616_Main_Ctrl.txst.Data[index] &= (~(1 << MotorId));
	CddBT616_Main_Ctrl.txst.Data[index] |= (flag<<MotorId);
}
/************************************
CddBT616_Set_BatVol
************************************/

void CddBT616_Set_LearnFlag(uint8_t MotorId,uint8_t Learn)
{
	uint8_t index = 3,flag= 1;
	if(!Learn)
	{
		flag=0;
	}
	CddBT616_Main_Ctrl.txst.Data[index] &= (~(1 << MotorId));
	CddBT616_Main_Ctrl.txst.Data[index] |= (flag<<MotorId);
}

/************************************
CddTest_Task

************************************/

void CddBT616_Task(void)
{
	Uartif_Msg_Str fl_str_e;

	if(BT_Connected())
	{
		if(CddBT616_Main_Ctrl.State < BT616_WORK_CHECK_CONNECT)
		{
			CddBT616_Main_Ctrl.State = BT616_WORK_CHECK_CONNECT;
		}
	}

	switch(CddBT616_Main_Ctrl.State)
	{
		case BT616_WORK_TEST_TX:
		{
			fl_str_e.DLC = CddBT616_Cmd_Table[BT616_CMD_AT][0].tx_len;
			memcpy(fl_str_e.Data ,CddBT616_Cmd_Table[BT616_CMD_AT][0].tx_ptr,fl_str_e.DLC);
			Uartif_tx_queue_push_e(fl_str_e);
			CddBT616_Main_Ctrl.State = BT616_WORK_TEST_RX;
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
		}
		break;
				
		case BT616_WORK_READ_NAME:
		{
			fl_str_e.DLC = CddBT616_Cmd_Table[BT616_CMD_NAME][0].tx_len;
			memcpy(fl_str_e.Data ,CddBT616_Cmd_Table[BT616_CMD_NAME][0].tx_ptr,fl_str_e.DLC);
			Uartif_tx_queue_push_e(fl_str_e);
			CddBT616_Main_Ctrl.State = BT616_WORK_CHECK_NAME;

		}
		break;

		case BT616_WORK_CHECK_NAME:
		{
			if(TRUE == Uartif_rx_queue_pull_e(&fl_str_e))
			{
				if(FALSE == memcmp(fl_str_e.Data ,CddBT616_Cmd_Table[BT616_CMD_NAME][0].rx_ptr,CddBT616_Cmd_Table[BT616_CMD_NAME][0].rx_len))
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_READ_NAME;
				}
				else
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_WRITE_NAME;
				}
			}
		}
		break;
			
		case BT616_WORK_WRITE_NAME:
		{
			fl_str_e.DLC = CddBT616_Cmd_Table[BT616_CMD_WNAME][0].tx_len;
			memcpy(fl_str_e.Data ,CddBT616_Cmd_Table[BT616_CMD_WNAME][0].tx_ptr,fl_str_e.DLC);
			Uartif_tx_queue_push_e(fl_str_e);
			CddBT616_Main_Ctrl.State = BT616_WORK_CHECK_NAME;
		}
		break;

		case BT616_WORK_W_CHECK_NAME:
		{
			if(TRUE == Uartif_rx_queue_pull_e(&fl_str_e))
			{
				if(FALSE == memcmp(fl_str_e.Data ,CddBT616_Cmd_Table[BT616_CMD_WNAME][0].rx_ptr,CddBT616_Cmd_Table[BT616_CMD_WNAME][0].rx_len))
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_CHECK_CONNECT;
				}
				else
				{
					CddBT616_Main_Ctrl.State = BT616_WORK_WRITE_NAME;
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
					Uartif_tx_queue_push_e(fl_str_e);
				}
				
				if(TRUE == Uartif_rx_queue_pull_e(&fl_str_e))
				{
					if(FALSE == memcmp(fl_str_e.Data ,"KEY=",4))
					{
					    CddBT616_Push_Button((CddBT616_KEY_VAL_e)fl_str_e.Data[4]);
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

/************************************
CddTest_Task

************************************/

void CddBT616_Push_Button(CddBT616_KEY_VAL_e fl_key)
{
	__disable_irq();
	CddBT616_Main_Ctrl.Button[2] = CddBT616_Main_Ctrl.Button[1];
	CddBT616_Main_Ctrl.Button[1] = CddBT616_Main_Ctrl.Button[0];
	CddBT616_Main_Ctrl.Button[0] = fl_key;
	__enable_irq();
}


/************************************
CddTest_Task

************************************/

CddBT616_KEY_VAL_e CddBT616_Pop_Button(void)
{
	CddBT616_KEY_VAL_e fl_key = BT616_NONE_KEY;
	__disable_irq();
	if(CddBT616_Main_Ctrl.Button[2] != BT616_NONE_KEY)
	{
		fl_key = CddBT616_Main_Ctrl.Button[2];
		CddBT616_Main_Ctrl.Button[2] = BT616_NONE_KEY;
	}
	else if(CddBT616_Main_Ctrl.Button[1] != BT616_NONE_KEY)
	{
		fl_key = CddBT616_Main_Ctrl.Button[1];
		CddBT616_Main_Ctrl.Button[1] = BT616_NONE_KEY;
	}
	else if(CddBT616_Main_Ctrl.Button[0] != BT616_NONE_KEY)
	{
		fl_key = CddBT616_Main_Ctrl.Button[0];
		CddBT616_Main_Ctrl.Button[0] = BT616_NONE_KEY;
	}
	__enable_irq();
	
	return fl_key;
}

/**********************************************************************************************************************
 *  END OF FILE
***********************************************************************************************************************/

/*EOF*/
