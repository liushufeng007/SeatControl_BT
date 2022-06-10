#include "user_init.h"
#include "Adcif.h"
#include "PowerMgmt.h"
#include "CddModeM.h"
#include "CddKey_Cfg.h"
#include "CddMtr_Mng.h"
#include "nw_can_dll.h"
#include "CanNm.h"
#include "CanNmCtrl.h"

#define POWER_BATTERY_DEBOUNCE_COUNTER   (5u)
#define POWER_BATTERY_TORERANCE          (100u)  // mV
/* ADC input value: mV*input scale 
 * ADC range: 0 - 2*VRef(1.2V) */
#define POWER_STANDBY_VOLTAGE_HVI2            (18500)  /* Power input voltage value >= 18.5V*/
#define POWER_STANDBY_VOLTAGE_LVI2            (6500)   /* Power input voltage value <= 6.5V*/
#define POWER_STANDBY_VOLTAGE_HVI1            (18000)  /* Power input voltage value >= 18.0V*/
#define POWER_STANDBY_VOLTAGE_LVI1            (7000)   /* Power input voltage value <= 7.0V*/ 
#define POWER_VOLTAGE_RECOVER_HYS             (1000)    /* Power voltage value hystersis vaule 1000mV */
#define POWER_VOLTAGE_HLVI1                   (1u)     /* Power high voltage1, Voltage value < 8.5V and Voltage value > 16.5V */
#define POWER_VOLTAGE_HLVI2                   (2u)     /* Power high voltage2, Voltage value < 6.5V and Voltage value > 18.5V */
#define POWER_VOLTAGE_NORMAL                  (3u)     /* Power Voltage Normal mode */
#define POWER_VOLTAGE_HVI2_HYS                (POWER_STANDBY_VOLTAGE_HVI2 - POWER_VOLTAGE_RECOVER_HYS)  /* 18v */
#define POWER_VOLTAGE_LVI2_HYS                (POWER_STANDBY_VOLTAGE_LVI2 + POWER_VOLTAGE_RECOVER_HYS)  /* 7v */
#define POWER_VOLTAGE_HVI1_HYS                (POWER_STANDBY_VOLTAGE_HVI1 - POWER_VOLTAGE_RECOVER_HYS)  /* 17v */
#define POWER_VOLTAGE_LVI1_HYS                (POWER_STANDBY_VOLTAGE_LVI1 + POWER_VOLTAGE_RECOVER_HYS)  /* 8v */
#define SEAT_TRUE                             (1)
#define SEAT_FALSE                            (0)
#define ENTRY_SLEEP_COUNT                     (500)


/* Function Prototypes */
void PowerModeInit(void);
void PowerModeHandler(void);
void PowerModeFastSample_5ms(void);
void Scm_PowerDown(void);

uint8_t Get_DiagVoltage_Flag(void);
uint8_t GetCANWorking_Voltage_Flag(void);
uint8_t Get_Voltage_Flag(void);
uint32_t Get_VoltVaule_Fast(void);



/* Static Function Prototypes */
static uint8_t Set_DiagVoltage_Flag(void);
static void Network_Suspend(void);

/* Static Variables */
static uint32_t auBatteryVoltage[POWER_BATTERY_DEBOUNCE_COUNTER];
static uint32_t uCurrBatAvgVolt = 0, uPrevBatAvgVolt = 0;
static uint8_t uVoltageIndex = 0;
static uint32_t uCurrBatAvgVolt1 = 0;
static uint8_t uCANWorking_Voltage_Flag = SEAT_FALSE;
static uint8_t PowerVoltage_HLVI_flag = 0;
static uint8_t uPower_Mode_Time = 0; 
uint16_t uVoltage_HV_Timer;
uint16_t uVoltage_LV_Timer;
uint16_t uVoltage_Normal_Timer;
uint8_t uVoltageThresholdFlag;
uint8_t CanMessage_Receive_Flag = TRUE;


/* 5ms sample voltage value */
static uint32_t auBatteryVoltageFast[POWER_BATTERY_DEBOUNCE_COUNTER];
static uint32_t uCurrBatAvgVoltFast = 0;


/*******************************************************************************
* Function Name: PowerModeInit
********************************************************************************
*
* Summary:
*  This API is called to initailize PowerMode (include SARADC for Battery)
*
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void PowerModeInit(void)
{
    /* Get  Voltage value */
	uint8_t uIdx = 0;
	uPower_Mode_Time = 0;
    uCurrBatAvgVolt = ( 57 * 500 * (Adcif_Get_AdcVal(ADCIF_CH_BATT1_AD)) )/4096; 
		uCurrBatAvgVoltFast = uCurrBatAvgVolt;
	
    for (uIdx = 0; uIdx < POWER_BATTERY_DEBOUNCE_COUNTER; uIdx++ )
    {
        auBatteryVoltage[uIdx] = uCurrBatAvgVolt;
    }
		
    for (uIdx = 0; uIdx < POWER_BATTERY_DEBOUNCE_COUNTER; uIdx++ )
    {
        auBatteryVoltageFast[uIdx] = uCurrBatAvgVoltFast;
    }

	CanMessage_Receive_Flag = TRUE;

    return;
}


/*******************************************************************************
* Function Name: PowerModeFastSample_5ms
********************************************************************************
*
* Summary:
*  This API is called from Main loop to handle power mode state transition
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void PowerModeFastSample_5ms(void)
{
	uint8_t uIdx = 0;
    /* Get power converter voltages in mVs    
     * only 12V Battery Voltage need to be monitor */    
    auBatteryVoltageFast[uVoltageIndex++] = ( 57 * 500 * (Adcif_Get_AdcVal(ADCIF_CH_BATT1_AD)) )/4096;    
    
    if ( uVoltageIndex >= POWER_BATTERY_DEBOUNCE_COUNTER )    
    {    
        uVoltageIndex = 0;    
    }    
        
    uCurrBatAvgVoltFast = 0;    
    for (uIdx = 0; uIdx < POWER_BATTERY_DEBOUNCE_COUNTER; uIdx++ )    
    {    
        uCurrBatAvgVoltFast += auBatteryVoltage[uIdx];    
    }    
    uCurrBatAvgVoltFast /= POWER_BATTERY_DEBOUNCE_COUNTER;    

}

/*******************************************************************************
* Function Name: PowerModeHandler
********************************************************************************
*
* Summary:
*  This API is called from Main loop to handle power mode state transition
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void PowerModeHandler(void)
{
	uint8_t uIdx = 0;
    static uint8_t uPMDiag_Voltage_State = POWER_VOLTAGE_NORMAL;   
    /* Get power converter voltages in mVs    
     * only 12V Battery Voltage need to be monitor */    
    auBatteryVoltage[uVoltageIndex++] = ( 57 * 500 * (Adcif_Get_AdcVal(ADCIF_CH_BATT1_AD)) )/4096;    
    
    if ( uVoltageIndex >= POWER_BATTERY_DEBOUNCE_COUNTER )    
    {    
        uVoltageIndex = 0;    
    }    
        
    uCurrBatAvgVolt = 0;    
    for (uIdx = 0; uIdx < POWER_BATTERY_DEBOUNCE_COUNTER; uIdx++ )    
    {    
        uCurrBatAvgVolt += auBatteryVoltage[uIdx];    
    }    
    uCurrBatAvgVolt /= POWER_BATTERY_DEBOUNCE_COUNTER;    
		
    /* Into diagnosis mode, get power voltage value  */    
    uCurrBatAvgVolt1 = uCurrBatAvgVolt;    
        
    if (( ( uCurrBatAvgVolt > uPrevBatAvgVolt ) && ( ( uCurrBatAvgVolt - uPrevBatAvgVolt ) > POWER_BATTERY_TORERANCE ) )    
     || ( ( uPrevBatAvgVolt > uCurrBatAvgVolt ) && ( ( uPrevBatAvgVolt - uCurrBatAvgVolt ) > POWER_BATTERY_TORERANCE ) ) )    
    {    
        uPrevBatAvgVolt = uCurrBatAvgVolt;    
        uCurrBatAvgVolt = uCurrBatAvgVolt;    
    }   

	switch(uPMDiag_Voltage_State)
	{
			/* Normal mode */
			case POWER_VOLTAGE_NORMAL:
				uCANWorking_Voltage_Flag = SEAT_FALSE;
				PowerVoltage_HLVI_flag = SEAT_FALSE;
				if( ( uCurrBatAvgVolt > POWER_STANDBY_VOLTAGE_HVI2) || ( uCurrBatAvgVolt < POWER_STANDBY_VOLTAGE_LVI2 ) )
			    {
						uPMDiag_Voltage_State = POWER_VOLTAGE_HLVI2;
			    }
				else if( (uCurrBatAvgVolt > POWER_STANDBY_VOLTAGE_HVI1) || ( uCurrBatAvgVolt < POWER_STANDBY_VOLTAGE_LVI1 ) )
				{
					uPower_Mode_Time ++;
					if( uPower_Mode_Time > 100 )
					{
						uPMDiag_Voltage_State = POWER_VOLTAGE_HLVI1;
						uPower_Mode_Time = 0;
					}
					
				}
				else
				{
					uPMDiag_Voltage_State = POWER_VOLTAGE_NORMAL;
					uPower_Mode_Time = 0;
				}
				break;
			/* When voltage value >18.5V or <6.5V, All function disable. */
			case POWER_VOLTAGE_HLVI2:
        		uCANWorking_Voltage_Flag = SEAT_TRUE;
				PowerVoltage_HLVI_flag = SEAT_TRUE;
				if( (( uCurrBatAvgVolt > POWER_VOLTAGE_LVI2_HYS ) && ( uCurrBatAvgVolt < POWER_VOLTAGE_LVI1_HYS ))\
					||( ( uCurrBatAvgVolt > POWER_VOLTAGE_HVI1_HYS ) && ( uCurrBatAvgVolt < POWER_VOLTAGE_HVI2_HYS ) ))
				{
					uPMDiag_Voltage_State = POWER_VOLTAGE_HLVI1;
				}
				else if( (uCurrBatAvgVolt >= POWER_VOLTAGE_LVI1_HYS) && ( uCurrBatAvgVolt <= POWER_VOLTAGE_HVI1_HYS ) )
				{
					uPMDiag_Voltage_State = POWER_VOLTAGE_NORMAL;
				}
				else
				{
					uPMDiag_Voltage_State = POWER_VOLTAGE_HLVI2;
				}
				break;
			/* When voltage value >16.5V or <8.5V, In addition to indicator, other functions keep last state.. */
			case POWER_VOLTAGE_HLVI1:
				uCANWorking_Voltage_Flag = SEAT_TRUE;
				PowerVoltage_HLVI_flag = SEAT_TRUE;
				if( ( uCurrBatAvgVolt > POWER_STANDBY_VOLTAGE_HVI2) || ( uCurrBatAvgVolt < POWER_STANDBY_VOLTAGE_LVI2 ) )
				{
					uPMDiag_Voltage_State = POWER_VOLTAGE_HLVI2;
				}
				else if( (uCurrBatAvgVolt >= POWER_VOLTAGE_LVI1_HYS) && ( uCurrBatAvgVolt <= POWER_VOLTAGE_HVI1_HYS ) )
				{
					uPower_Mode_Time ++;
					if( uPower_Mode_Time >= 100 )
					{
						uPMDiag_Voltage_State = POWER_VOLTAGE_NORMAL;
						uPower_Mode_Time = 0;
					}
				}
				else
				{
					uPMDiag_Voltage_State = POWER_VOLTAGE_HLVI1;
					uPower_Mode_Time = 0;
				}				
				break;
			default:
				/* Do nothing */
				break;
	}

	/* Network suspend and resume */
	Network_Suspend();

}


/*******************************************************************************
* Summary:
*  This API is called when go to HV/LV logic clean not receive CAN meaasge(7~18V) 
*  Suspend CAN TX
*******************************************************************************/
void Network_Suspend(void)
{
	uint8_t Get_Network_Volt_Flag;
	static BOOLEAN NetWork_Suspend_Flag = SEAT_FALSE;
	static BOOLEAN NetWork_Resume_Flag = SEAT_FALSE;

	Get_Network_Volt_Flag = GetCANWorking_Voltage_Flag();

	if( (SEAT_TRUE == Get_Network_Volt_Flag) && ( SEAT_FALSE == NetWork_Suspend_Flag ) )
	{
		vnim_disable_tx();
		NetWork_Suspend_Flag = SEAT_TRUE;
		NetWork_Resume_Flag = SEAT_FALSE;
	}
	
	if( (SEAT_FALSE == Get_Network_Volt_Flag) && ( SEAT_FALSE == NetWork_Resume_Flag ))
	{
		vnim_enable_tx();
		NetWork_Suspend_Flag = SEAT_FALSE;
		NetWork_Resume_Flag = SEAT_TRUE;		
	}
}

/*******************************************************************************
* Summary:
*  This API is called when go to HV/LV logic clean not receive CAN meaasge(7~18V) 
*
*******************************************************************************/
uint8_t GetCANWorking_Voltage_Flag(void)
{
	return uCANWorking_Voltage_Flag;
}


/*******************************************************************************
* Summary:
*  This API is called when Entry OverVoltage. 
*  
*
*******************************************************************************/
static uint8_t Set_DiagVoltage_Flag(void)
{
    static uint8_t SetDiagVoltage_Flag = 0;
	
    if( (uCurrBatAvgVolt1 < POWER_VOLTAGE_LVI1_HYS) && (uCurrBatAvgVolt1 > POWER_VOLTAGE_HVI1_HYS) )  /* Current battery voltage < 8V or >17V */
    {
		SetDiagVoltage_Flag = SEAT_TRUE;
    }
    else if( (uCurrBatAvgVolt1 >= (POWER_VOLTAGE_LVI1_HYS + POWER_VOLTAGE_RECOVER_HYS ))
	|| (uCurrBatAvgVolt1 <= (POWER_VOLTAGE_HVI1_HYS - POWER_VOLTAGE_HVI1_HYS)) ) /* Current battery voltage < 9V */
    {
		SetDiagVoltage_Flag = SEAT_FALSE;
    }
    else
    {
		/* Do nothing */
    }
	
    return(SetDiagVoltage_Flag);
}

/*******************************************************************************
* Summary:
*  This API is called when Entry Diag OverVoltage. 
*  
*
*******************************************************************************/
uint8_t Get_DiagVoltage_Flag(void)
{
    uint8_t GetDiagVoltage_Flag = 0u;
    GetDiagVoltage_Flag = Set_DiagVoltage_Flag();
    
    return(GetDiagVoltage_Flag);
}


/*******************************************************************************
* Summary:
*  This API is called when Entry OverVoltage or underVoltage. 
*  
*******************************************************************************/
uint8_t Get_Voltage_Flag(void)
{
	uint8_t GetVoltage_flag = 0;
	GetVoltage_flag = PowerVoltage_HLVI_flag;

	return(GetVoltage_flag);
}

/*******************************************************************************
* Summary:
*  This API is called when Entry OverVoltage or underVoltage. 
*  
*******************************************************************************/
uint32_t Get_VoltVaule_Fast(void)
{
	uint32_t Get_Voltage_Value_Fast = 0;

	Get_Voltage_Value_Fast = uCurrBatAvgVoltFast;

	return (Get_Voltage_Value_Fast);
}

/*******************************************************************************
* Summary:
*  This API is called when Not received CAN message. 
*  
*******************************************************************************/
void Clean_Receive_Message_Flag(void)
{
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
	CanMessage_Receive_Flag = FALSE;
#endif
}

/*******************************************************************************
* Summary:
*  This API is called when received CAN message. 
*  
*******************************************************************************/
void Set_Receive_Message_Flag(void)
{
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
	CanMessage_Receive_Flag = TRUE;
#endif
}

/*******************************************************************************
* Summary:
*  This API is called when Entry Sleep Mode. 
*  
*******************************************************************************/
void Scm_PowerDown(void)
{
	BOOLEAN CddMtr_Get_SleepFlag = 0;
	BOOLEAN Nm_ConditionFlag = 0;
	BOOLEAN Get_IGN_key_Sts = 0;
	static uint16_t Entry_Sleep_Mode = 0;
	static uint8_t IGN_Key_Debounce = 0;

	/* IGN key signal debounce 50ms */
	if(1 == ServSWM_u_GetKey_IGN_RealVaule())
	{
		if( (IGN_Key_Debounce < 5) && (0 == Get_IGN_key_Sts) )
		{
			IGN_Key_Debounce ++;
		}
		else
		{
			Get_IGN_key_Sts = 1;
		}
	}
	else
	{
		Get_IGN_key_Sts = 0;
		IGN_Key_Debounce = 0;
	}

	/* IGN ON Send NM message, IGN OFF stop Send NM message */
#if(SCM_SEATCONTROL_VARIANT == SCM_L_VARIANT)
	if( 1 == Get_IGN_key_Sts )
	{
		CanNmCtrl_Req_NetWork(0);
	}
	else
	{
		CanNmCtrl_Release_NetWork(0);
	}


	/* Get Network and motor condition */
	if(BusSleep_STATE_E_0 == CanNmCtrl_Get_CanNm_State(0))
	{
		Nm_ConditionFlag = TRUE;
	}
#endif
	/* Get Motor sleep flag */
	CddMtr_Get_SleepFlag = CddMtr_Get_SleepCondition();

	
	/* Judge MCU sleep condition, Concition match 5s, MCU entry sleep mode */
#if(SCM_SEATCONTROL_VARIANT == SCM_L_VARIANT)
	if( ( TRUE == CddMtr_Get_SleepFlag ) && ( TRUE == Nm_ConditionFlag ))
#else
	if( ( TRUE == CddMtr_Get_SleepFlag ) && ( TRUE == CanMessage_Receive_Flag ))
#endif
	{
		Entry_Sleep_Mode ++;
	}
	else
	{
		Entry_Sleep_Mode = 0;
	}

	if( Entry_Sleep_Mode >= ENTRY_SLEEP_COUNT )
	{
		Entry_Sleep_Mode = 0;
		CddModeM_Req_Normal(CDDMODEM_REQ_RELEASE);
	}

	#if 0
	static UINT8 GetKey_IGN = 0;
	static UINT8 PreGetKey_IGN = 0;

	GetKey_IGN = ServSWM_u_GetKey_IGN_RealVaule();

	if( ( PreGetKey_IGN != GetKey_IGN ) && ( GetKey_IGN == 1 ) )
	{
		CddModeM_Req_Normal(CDDMODEM_REQ_REQUEST);
	}

	if( FALSE == GetKey_IGN )
	{
		//CddModeM_Req_Normal(CDDMODEM_REQ_RELEASE);
	}
	#endif
}

/* [] END OF FILE */
