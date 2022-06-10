/*******************************************************************************
|  File Name:  SwM.c
|  Description:  Implementation of the Swtich Management
|-------------------------------------------------------------------------------
|******************************************************************************/

/*******************************************************************************
|    Other Header File Inclusion
|******************************************************************************/
#include "CddKey.h"
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

const uint16_t  KeSWM_w_FtpLongKeyMaxDelayTime=500;
const uint16_t  KeSWM_w_FtpLongKeyMinDelayTime=50;
const uint16_t  KeSWM_w_FtpLongKeyDelayStep=100;


STATIC TsSWM_h_KeyStruct TsSWM_h_NULL;
STATIC TsSWM_h_KeyStruct TsSWM_h_Key[CeSWM_u_Key_NUM];
STATIC TsSWM_h_KeyStatusStruct TsSWM_h_JYHwKeyStatus;

STATIC uint32_t VeSWM_dw_JYHwKeyRealValue;



/*******************************************************************************
|    Table Definition
|******************************************************************************/
STATIC const uint32_t CtSWM_dw_uint32_tSetTable[32] =
{
    0x00000001U, 0x00000002U, 0x00000004U, 0x00000008U, 						/*0-3   bit*/
    0x00000010U, 0x00000020U, 0x00000040U, 0x00000080U,							/*4-7   bit*/
    0x00000100U, 0x00000200U, 0x00000400U, 0x00000800U,							/*8-11  bit*/
    0x00001000U, 0x00002000U, 0x00004000U, 0x00008000U,							/*12-15 bit*/
    0x00010000U, 0x00020000U, 0x00040000U, 0x00080000U,							/*16-19 bit*/
    0x00100000U, 0x00200000U, 0x00400000U, 0x00800000U,							/*20-23 bit*/
    0x01000000U, 0x02000000U, 0x04000000U, 0x08000000U,							/*24-27 bit*/
    0x10000000U, 0x20000000U, 0x40000000U, 0x80000000U							/*28-31 bit*/
};

STATIC const uint32_t CtSWM_dw_uint32_tResetTable[32] =
{
    0xFFFFFFFEU, 0xFFFFFFFDU, 0xFFFFFFFBU, 0xFFFFFFF7U, 						/*0-3   bit*/
    0xFFFFFFEFU, 0xFFFFFFDFU, 0xFFFFFFBFU, 0xFFFFFF7FU,							/*4-7   bit*/
    0xFFFFFEFFU, 0xFFFFFDFFU, 0xFFFFFBFFU, 0xFFFFF7FFU,							/*8-11  bit*/
    0xFFFFEFFFU, 0xFFFFDFFFU, 0xFFFFBFFFU, 0xFFFF7FFFU,							/*12-15 bit*/
    0xFFFEFFFFU, 0xFFFDFFFFU, 0xFFFBFFFFU, 0xFFF7FFFFU,							/*16-19 bit*/
    0xFFEFFFFFU, 0xFFDFFFFFU, 0xFFBFFFFFU, 0xFF7FFFFFU,							/*20-23 bit*/
    0xFEFFFFFFU, 0xFDFFFFFFU, 0xFBFFFFFFU, 0xF7FFFFFFU,							/*24-27 bit*/
    0xEFFFFFFFU, 0xDFFFFFFFU, 0xBFFFFFFFU, 0x7FFFFFFFU							/*28-31 bit*/
};


STATIC const TsSWM_h_SwmConfigStruct TsSWM_h_JYHardwareKey =
{
    /*e_u_SwmGroup;*/
    CeSWM_u_JY_HW_CH,

    /*e_u_SwmChEnableTab[32]*/
    {
			0x01U,	0x01U,	0x01U,	0x01U,	0x01U,	0x01U,	0x01U,	0x01U,
			0x01U,	0x01U,	0x01U,	0x00U,	0x00U,	0x00U,	0x00U,	0x00U,
			0x00U,	0x00U,	0x00U,	0x00U,	0x00U,	0x00U,	0x00U,	0x00U,
			0x00U,	0x00U,	0x00U,	0x00U,	0x00U,	0x00U,	0x00U,	0x00U
    },


    /*TsSWM_h_KeyStruct *e_p_SwmStructArray[CeSWM_u_NUM_CH]*/
    {
        &TsSWM_h_Key[0],  &TsSWM_h_Key[1], &TsSWM_h_Key[2], &TsSWM_h_Key[3], 
		&TsSWM_h_Key[4],  &TsSWM_h_Key[5], &TsSWM_h_Key[6], &TsSWM_h_Key[7], 
        &TsSWM_h_Key[8],  &TsSWM_h_Key[9], &TsSWM_h_Key[10],&TsSWM_h_NULL, 
		&TsSWM_h_NULL,    &TsSWM_h_NULL,   &TsSWM_h_NULL,   &TsSWM_h_NULL,
		&TsSWM_h_NULL,    &TsSWM_h_NULL,   &TsSWM_h_NULL,   &TsSWM_h_NULL, 
		&TsSWM_h_NULL,    &TsSWM_h_NULL,   &TsSWM_h_NULL,   &TsSWM_h_NULL,
		&TsSWM_h_NULL,    &TsSWM_h_NULL,   &TsSWM_h_NULL,   &TsSWM_h_NULL, 
		&TsSWM_h_NULL,    &TsSWM_h_NULL,   &TsSWM_h_NULL,   &TsSWM_h_NULL,

    },

    /*e_u_ShortFiltTime*/
    5U,

    /*e_u_OverFiltTime*/
    60U,

    /*e_u_OverFiltRecTime*/
    1000U
};

/*******************************************************************************
|    Static Local Functions Declaration
|******************************************************************************/
STATIC void ServSWM_InitKeyStatusStruct(TsSWM_h_KeyStatusStruct *LpSWM_u_KeyStatus, uint32_t LeSWM_u_LongContinuTemp);
STATIC uint32_t GetSWM_dw_KeyBusData(uint8_t LeSWM_u_TempValue);
STATIC void ServSWM_KeyRealValueSet(const TsSWM_h_SwmConfigStruct *LpSWM_h_TempKeyValue, TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus);
STATIC void ChkSWM_KeyRealTimeValue(void);
STATIC void ServSWM_KeyDebounceDeal(TsSWM_h_KeyStruct *LpSWM_u_DebounceKeySig, uint32_t LeSWM_dw_DebouncePortValue, uint8_t LeSWM_u_DebounceShortFiltTime, uint8_t LeSWM_u_DebounceOvrFiltTime, uint16_t LeSWM_w_DebounceOvrFiltRecTm);
STATIC uint32_t GetSWM_dw_KeySigRealValue(const TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus, uint8_t LeSWM_u_TempValue);
STATIC void DealSWM_KeyStatusValue(const TsSWM_h_KeyStruct *LpSWM_KeySigStruct, TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus, uint8_t LeSWM_u_TempIndex, uint8_t LeSWM_u_TempValue);
STATIC void ServSWM_KeyStatusDeal(const TsSWM_h_SwmConfigStruct *LpSWM_h_TempKeyValue, TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus);
STATIC void ChkSWM_KeyStatus(void);
STATIC void ServSWM_KeyContinuFuncDeal(TsSWM_h_KeyStatusStruct *LpSWM_KeyStruct, uint16_t LeSWM_w_MinDelayTime, uint16_t LeSWM_w_DelayStepTime, uint16_t LeSWM_w_MaxTime);
STATIC void ChkSWM_KeyContinuousStatus(void);
STATIC uint32_t GetSWM_dw_KeyValue(TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus);

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Extern variables and functions declaration
|******************************************************************************/

/*******************************************************************************
|    Function Source Code
|******************************************************************************/


/*******************************************************************************
ID				: 02
Name			: ServSWM_InitKeyStatusStruct
Syntax			: void ServSWM_InitKeyStatusStruct(TsSWM_h_KeyStatusStruct *LpSWM_u_KeyStatus,
													uint32_t LeSWM_u_LongContinuTemp)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: LpSWM_u_KeyStatus			:-
				 LeSWM_u_LongContinuTemp
Parameters(out) : None						:-
Return value	: None						:-
Description		: Initialize Key Status Structs
Call By			: SwM
|******************************************************************************/
STATIC void ServSWM_InitKeyStatusStruct(TsSWM_h_KeyStatusStruct *LpSWM_u_KeyStatus, uint32_t LeSWM_u_LongContinuTemp)
{
    LpSWM_u_KeyStatus->e_dw_Status = 0U;
    LpSWM_u_KeyStatus->e_dw_NewStatus = 0U;
    LpSWM_u_KeyStatus->e_dw_OldStatus = 0U;
    LpSWM_u_KeyStatus->e_dw_KeyRealValue = 0U;
    LpSWM_u_KeyStatus->e_dw_ContinuousWord = LeSWM_u_LongContinuTemp;
    LpSWM_u_KeyStatus->e_dw_EventWord = 0U;
    LpSWM_u_KeyStatus->e_dw_EventOldWord = 0U;
    LpSWM_u_KeyStatus->e_u_KeyLngDownFlag = 0U;
    LpSWM_u_KeyStatus->e_dw_LongDownTimeCnt = 0U;
    LpSWM_u_KeyStatus->e_dw_LngDnHisTimeCnt = 0U;
}

/*******************************************************************************
ID				: 03
Name			: SampSWM_JYKeyRealValue
Syntax			: uint32_t SampSWM_JYKeyRealValue(uint32_t *LpSWM_KeySigRealValueBuff, 
								uint8_t LpSWM_KeyRealStatus, uint8_t LeSWM_u_TempIndex)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: None			:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: get JY Key Hardware Signal
Call By			: SwM
|******************************************************************************/
STATIC void SampSWM_JYKeyRealValue(uint32_t *LpSWM_KeySigRealValueBuff, uint8_t LpSWM_KeyRealStatus, uint8_t LeSWM_u_TempIndex)
{
    if(0U != LpSWM_KeyRealStatus)
    {
        *LpSWM_KeySigRealValueBuff |= CtSWM_dw_uint32_tSetTable[LeSWM_u_TempIndex];
    }
    else
    {
        *LpSWM_KeySigRealValueBuff &= CtSWM_dw_uint32_tResetTable[LeSWM_u_TempIndex];
    }
}

/*******************************************************************************
ID				: 03
Name			: GetSWM_dw_JYKeyHwSignal
Syntax			: uint32_t GetSWM_dw_JYKeyHwSignal(void)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: None			:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: get Key Hardware Signal
Call By			: SwM
|******************************************************************************/
STATIC uint32_t GetSWM_dw_JYKeyHwSignal(void)
{
	SampSWM_JYKeyRealValue(&VeSWM_dw_JYHwKeyRealValue, ServSWM_u_GetKey_Seatback_B_SW_RealVaule(), CeSWM_u_Seatback_B_SW_IO_KEY);
	SampSWM_JYKeyRealValue(&VeSWM_dw_JYHwKeyRealValue, ServSWM_u_GetKey_Seatback_F_SW_RealVaule(), CeSWM_u_Seatback_F_SW_IO_KEY);

	SampSWM_JYKeyRealValue(&VeSWM_dw_JYHwKeyRealValue, ServSWM_u_GetKey_Seat_B_SW_RealVaule(), CeSWM_u_Seat_B_SW_IO_KEY);
	SampSWM_JYKeyRealValue(&VeSWM_dw_JYHwKeyRealValue, ServSWM_u_GetKey_Seat_F_SW_RealVaule(), CeSWM_u_Seat_F_SW_IO_KEY);

	SampSWM_JYKeyRealValue(&VeSWM_dw_JYHwKeyRealValue, ServSWM_u_GetKey_Seat3_B_SW_RealVaule(), CeSWM_u_Seat3_B_SW_IO_KEY);
	SampSWM_JYKeyRealValue(&VeSWM_dw_JYHwKeyRealValue, ServSWM_u_GetKey_Seat3_F_SW_RealVaule(), CeSWM_u_Seat3_F_SW_IO_KEY);

	SampSWM_JYKeyRealValue(&VeSWM_dw_JYHwKeyRealValue, ServSWM_u_GetKey_Seat4_B_SW_RealVaule(), CeSWM_u_Seat4_B_SW_IO_KEY);
	SampSWM_JYKeyRealValue(&VeSWM_dw_JYHwKeyRealValue, ServSWM_u_GetKey_Seat4_F_SW_RealVaule(), CeSWM_u_Seat4_F_SW_IO_KEY);

	SampSWM_JYKeyRealValue(&VeSWM_dw_JYHwKeyRealValue, ServSWM_u_GetKey_Seat_FastB_SW_RealVaule(), CeSWM_u_Seat_FastB_SW_IO_KEY);
	SampSWM_JYKeyRealValue(&VeSWM_dw_JYHwKeyRealValue, ServSWM_u_GetKey_Seat_FastF_SW_RealVaule(), CeSWM_u_Seat_FastF_SW_IO_KEY);

	SampSWM_JYKeyRealValue(&VeSWM_dw_JYHwKeyRealValue, ServSWM_u_GetKey_Test_SW_RealVaule(), CeSWM_u_Test_SW_IO_KEY);

	
	return(VeSWM_dw_JYHwKeyRealValue);
}

/*0x64: 100U*/
/*******************************************************************************
ID				: 03
Name			: GetSWM_dw_KeyBusData
Syntax			: uint32_t GetSWM_dw_KeyBusData(uint8_t LeSWM_u_TempValue)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: LeSWM_u_TempValue			:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: get Key bus data
Call By			: SwM
|******************************************************************************/
STATIC uint32_t GetSWM_dw_KeyBusData(uint8_t LeSWM_u_TempValue)
{
    uint32_t LeSWM_dw_TempValue = 0U;

    switch(LeSWM_u_TempValue)
    {
    	case CeSWM_u_JY_HW_CH:
    	{
    		LeSWM_dw_TempValue = GetSWM_dw_JYKeyHwSignal();
    		
    		break;
    	}
		
	    case CeSWM_u_TOUCH_LOW_CH:
	    {
	        break;
	    }
	    case CeSWM_u_TOUCH_HIGH_CH:
	    {
	        break;
	    }
	    case CeSWM_u_FTP_LOW_CH:
	    {
	        break;
	    }
	    case CeSWM_u_FTP_HIGH_CH:
	    {
	        break;
	    }
	    default:
	    {
	        break;
	    }
    }

    return(LeSWM_dw_TempValue);
}

/*0x5e: 94*/
/*******************************************************************************
ID				: 05
Name			: ServSWM_KeyRealValueSet
Syntax			: void ServSWM_KeyRealValueSet(const TsSWM_h_SwmConfigStruct *LpSWM_h_TempKeyValue,
												TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: LpSWM_h_TempKeyValue			:-
				 LpSWM_KeyStatus
Parameters(out) : None						:-
Return value	: None						:-
Description		: set key real value
Call By			: SwM
|******************************************************************************/
STATIC void ServSWM_KeyRealValueSet(const TsSWM_h_SwmConfigStruct *LpSWM_h_TempKeyValue, TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus)
{
    uint8_t LeSWM_u_Condition = TRUE;

    if(LeSWM_u_Condition)
    {
        LpSWM_KeyStatus->e_dw_KeyRealValue = GetSWM_dw_KeyBusData(LpSWM_h_TempKeyValue->e_u_SwmGroup);
    }
    else
    {
        LpSWM_KeyStatus->e_dw_KeyRealValue = 0U;
        LpSWM_KeyStatus->e_dw_Status = 0U;
        LpSWM_KeyStatus->e_dw_NewStatus = 0U;
        LpSWM_KeyStatus->e_dw_OldStatus = 0U;
    }

}

/*0x58: 88*/
/*******************************************************************************
ID				: 06
Name			: ChkSWM_KeyRealTimeValue
Syntax			: void ChkSWM_KeyRealTimeValue(void)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: None						:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: check all key real value
Call By			: SwM
|******************************************************************************/
STATIC void ChkSWM_KeyRealTimeValue(void)
{
    ServSWM_KeyRealValueSet(&TsSWM_h_JYHardwareKey, &TsSWM_h_JYHwKeyStatus);
}

/*******************************************************************************
ID				: 07
Name			: ServSWM_KeyDebounceDeal
Syntax			: void ServSWM_KeyDebounceDeal(TsSWM_h_KeyStruct *LpSWM_u_DebounceKeySig, uint32_t LeSWM_dw_DebouncePortValue,
                                    uint8_t LeSWM_u_DebounceShortFiltTime, uint8_t LeSWM_u_DebounceOvrFiltTime,
                                    uint16_t LeSWM_w_DebounceOvrFiltRecTm)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: 							:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: key debounce deal
Call By			: SwM
|******************************************************************************/
STATIC void ServSWM_KeyDebounceDeal(TsSWM_h_KeyStruct *LpSWM_u_DebounceKeySig, uint32_t LeSWM_dw_DebouncePortValue,
                                    uint8_t LeSWM_u_DebounceShortFiltTime, uint8_t LeSWM_u_DebounceOvrFiltTime,
                                    uint16_t LeSWM_w_DebounceOvrFiltRecTm)
{
    uint8_t LeSWM_u_TempKeyFlag = 0U ;
    uint8_t LeSWM_u_TempKeyShortCnt = (uint8_t)(LeSWM_u_DebounceShortFiltTime / CeSWM_KEY_TASKTIME);		/*KEY debounce time*/
    uint16_t LeSWM_w_TempKeyOvrRecCnt = (uint16_t)(LeSWM_w_DebounceOvrFiltRecTm / CeSWM_KEY_TASKTIME);		/*KEY binding recover time*/
    uint16_t LeSWM_w_TempKeyOvrCnt = (uint16_t)((((uint32_t)LeSWM_u_DebounceOvrFiltTime) * 1000U) / CeSWM_KEY_TASKTIME); /*KEY binding time*/

    if(0U != LeSWM_dw_DebouncePortValue)                                        /*set key actual value now*/
    {
        LeSWM_u_TempKeyFlag = 1U ;
    }
    else
    {
        if(0U != LpSWM_u_DebounceKeySig->e_w_SigRecoverCnt)                     /*deal key binding time*/
        {
            LpSWM_u_DebounceKeySig->e_w_SigRecoverCnt--;

            if(0U == LpSWM_u_DebounceKeySig->e_w_SigRecoverCnt)
            {
                LpSWM_u_DebounceKeySig->e_w_SigDeadCnt = 0U ;
            }
        }
    }

    if(LpSWM_u_DebounceKeySig->e_u_SigOldStatus != LeSWM_u_TempKeyFlag)         /*key value changed*/
    {
        LpSWM_u_DebounceKeySig->e_u_SigTimeCnt = 0U ;							/*reset key time cnt*/
        LpSWM_u_DebounceKeySig->e_u_SigOldStatus = LeSWM_u_TempKeyFlag ;		/*refresh key old status*/

    }

    if(LpSWM_u_DebounceKeySig->e_u_SigTimeCnt < 255U)							/*overflow detection*/
    {
        LpSWM_u_DebounceKeySig->e_u_SigTimeCnt++;
    }

    if(LpSWM_u_DebounceKeySig->e_u_SigTimeCnt >= LeSWM_u_TempKeyShortCnt)
    {
        /*refresh key actual value*/
        LpSWM_u_DebounceKeySig->e_u_SigNowStatus = LpSWM_u_DebounceKeySig->e_u_SigOldStatus;

        if(0U != LpSWM_u_DebounceKeySig->e_u_SigNowStatus)
        {
            if(LpSWM_u_DebounceKeySig->e_w_SigDeadCnt < LeSWM_w_TempKeyOvrCnt)
            {
                /*set binding recover time*/
                LpSWM_u_DebounceKeySig->e_w_SigRecoverCnt = LeSWM_w_TempKeyOvrRecCnt;
                LpSWM_u_DebounceKeySig->e_w_SigDeadCnt++;
            }
            if(LpSWM_u_DebounceKeySig->e_w_SigDeadCnt >= LeSWM_w_TempKeyOvrCnt)
            {
                LpSWM_u_DebounceKeySig->e_u_SigNowStatus = 0U ;					/*key binding,key value invalid*/
            }
        }
        else
        {
            if(LpSWM_u_DebounceKeySig->e_w_SigDeadCnt < LeSWM_w_TempKeyOvrCnt)
            {
                LpSWM_u_DebounceKeySig->e_u_SigNowStatus = 0U ;					/*key binding,key value invalid*/
            }
        }
    }
}

/*******************************************************************************
ID				: 08
Name			: GetSWM_dw_KeySigRealValue
Syntax			: uint32_t GetSWM_dw_KeySigRealValue(const TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus,
														uint8_t LeSWM_u_TempValue)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: LpSWM_KeyStatus			:-
				 LeSWM_u_TempValue
Parameters(out) : None						:-
Return value	: None						:-
Description		: get key signal real value
Call By			: SwM
|******************************************************************************/
STATIC uint32_t GetSWM_dw_KeySigRealValue(const TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus, uint8_t LeSWM_u_TempValue)
{
    uint32_t LeSWM_dw_KeySignalTemp = 0U;

    LeSWM_dw_KeySignalTemp = (LpSWM_KeyStatus->e_dw_KeyRealValue) & (CtSWM_dw_uint32_tSetTable[LeSWM_u_TempValue]);

    return(LeSWM_dw_KeySignalTemp);
}


/*******************************************************************************
ID				: 09
Name			: DealSWM_KeyStatusValue
Syntax			: void DealSWM_KeyStatusValue(const TsSWM_h_KeyStruct *LpSWM_KeySigStruct,
											TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus,
													uint8_t LeSWM_u_TempIndex)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: LpSWM_KeySigStruct			:-
				 LpSWM_KeyStatus
				 LeSWM_u_TempIndex
Parameters(out) : None						:-
Return value	: None						:-
Description		: deal key status
Call By			: SwM
|******************************************************************************/
STATIC void DealSWM_KeyStatusValue(const TsSWM_h_KeyStruct *LpSWM_KeySigStruct, TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus, uint8_t LeSWM_u_TempIndex, uint8_t LeSWM_u_TempValue)
{
    if(0U != LpSWM_KeySigStruct->e_u_SigNowStatus)
    {
        LpSWM_KeyStatus->e_dw_NewStatus |= CtSWM_dw_uint32_tSetTable[LeSWM_u_TempIndex];
    }
    else
    {
        LpSWM_KeyStatus->e_dw_NewStatus &= CtSWM_dw_uint32_tResetTable[LeSWM_u_TempIndex];
    }
}

/*******************************************************************************
ID				: 10
Name			: ServSWM_KeyStatusDeal
Syntax			: void ServSWM_KeyStatusDeal(const TsSWM_h_SwmConfigStruct *LpSWM_h_TempKeyValue,
											TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: LpSWM_h_TempKeyValue			:-
				 LpSWM_KeyStatus
Parameters(out) : None						:-
Return value	: None						:-
Description		: deal key status
Call By			: SwM
|******************************************************************************/
STATIC void ServSWM_KeyStatusDeal(const TsSWM_h_SwmConfigStruct *LpSWM_h_TempKeyValue, TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus)
{
    uint8_t LeSWM_u_TempValue = 0U;
    uint32_t LeSWM_dw_TempRealKeyValue = 0U;
    uint8_t LeSWM_b_RealKeyFlag = FALSE;
    uint8_t LeSWM_b_DebounceKeyFlag = FALSE;

    for(LeSWM_u_TempValue = 0U; LeSWM_u_TempValue < CeSWM_u_NUM_CH; LeSWM_u_TempValue++)
    {
        if(0U != LpSWM_h_TempKeyValue->e_u_SwmChEnableTab[LeSWM_u_TempValue])
        {
            LeSWM_dw_TempRealKeyValue = CtSWM_dw_uint32_tSetTable[LeSWM_u_TempValue] & LpSWM_KeyStatus->e_dw_KeyRealValue;
            if(0U != LeSWM_dw_TempRealKeyValue)
            {
                LeSWM_b_RealKeyFlag = TRUE;
            }
            else
            {
                LeSWM_b_RealKeyFlag = FALSE;
            }

            LeSWM_dw_TempRealKeyValue = LpSWM_h_TempKeyValue->e_p_SwmStructArray[LeSWM_u_TempValue]->e_u_SigNowStatus;
            if(0U != LeSWM_dw_TempRealKeyValue)
            {
                LeSWM_b_DebounceKeyFlag = TRUE;
            }
            else
            {
                LeSWM_b_DebounceKeyFlag = FALSE;
            }

            if((LeSWM_b_DebounceKeyFlag != LeSWM_b_RealKeyFlag) || ((0U != LpSWM_h_TempKeyValue->e_p_SwmStructArray[LeSWM_u_TempValue]->e_w_SigRecoverCnt)/* && (LpSWM_h_TempKeyValue->e_u_OverFiltRecTime != LpSWM_h_TempKeyValue->e_p_SwmStructArray[LeSWM_u_TempValue]->e_w_SigRecoverCnt)*/))
            {
                ServSWM_KeyDebounceDeal(LpSWM_h_TempKeyValue->e_p_SwmStructArray[LeSWM_u_TempValue], GetSWM_dw_KeySigRealValue(LpSWM_KeyStatus, LeSWM_u_TempValue), LpSWM_h_TempKeyValue->e_u_ShortFiltTime, LpSWM_h_TempKeyValue->e_u_OverFiltTime, LpSWM_h_TempKeyValue->e_u_OverFiltRecTime);

                if(((uint8_t)(LpSWM_h_TempKeyValue->e_p_SwmStructArray[LeSWM_u_TempValue]->e_w_SigDeadCnt / 200U)) < LpSWM_h_TempKeyValue->e_u_OverFiltTime)
                {
                    DealSWM_KeyStatusValue(LpSWM_h_TempKeyValue->e_p_SwmStructArray[LeSWM_u_TempValue], LpSWM_KeyStatus, LeSWM_u_TempValue, LpSWM_h_TempKeyValue->e_u_SwmGroup);
                }
                else
                {
          		    LpSWM_KeyStatus->e_dw_NewStatus &= CtSWM_dw_uint32_tResetTable[LeSWM_u_TempValue];
                }
            }
        }
        else
        {
            LpSWM_KeyStatus->e_dw_NewStatus &= CtSWM_dw_uint32_tResetTable[LeSWM_u_TempValue];
        }
    }

    if(LpSWM_KeyStatus->e_dw_NewStatus == LpSWM_KeyStatus->e_dw_Status)
    {
        if(0U == LpSWM_KeyStatus->e_dw_Status)
        {
            LpSWM_KeyStatus->e_u_KeyLngDownFlag = 0U;
        }
        else
        {
            LpSWM_KeyStatus->e_u_KeyLngDownFlag = 1U;
        }
    }
    else
    {
        LpSWM_KeyStatus->e_dw_Status = LpSWM_KeyStatus->e_dw_NewStatus;
        LpSWM_KeyStatus->e_dw_EventWord = LpSWM_KeyStatus->e_dw_NewStatus;
    }
}

/*******************************************************************************
ID				: 11
Name			: ChkSWM_KeyStatus
Syntax			: void ChkSWM_KeyStatus(void)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: None						:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: check key status
Call By			: SwM
|******************************************************************************/
STATIC void ChkSWM_KeyStatus(void)
{
    ServSWM_KeyStatusDeal(&TsSWM_h_JYHardwareKey, &TsSWM_h_JYHwKeyStatus);
}

/*******************************************************************************
ID				: 12
Name			: ServSWM_KeyContinuFuncDeal
Syntax			: void ServSWM_KeyContinuFuncDeal(TsSWM_h_KeyStatusStruct *LpSWM_KeyStruct,
												uint16_t LeSWM_w_MinDelayTime,
												uint16_t LeSWM_w_DelayStepTime,
												uint16_t LeSWM_w_MaxTime)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: 							:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: check key continu func
Call By			: SwM
|******************************************************************************/
STATIC void ServSWM_KeyContinuFuncDeal(TsSWM_h_KeyStatusStruct *LpSWM_KeyStruct, uint16_t LeSWM_w_MinDelayTime, uint16_t LeSWM_w_DelayStepTime, uint16_t LeSWM_w_MaxTime)
{
    uint8_t LeSWM_u_TempMinDelayValue = (uint8_t)(LeSWM_w_MinDelayTime / CeSWM_KEY_TASKTIME);
    uint8_t LeSWM_u_TempKeyDelayStepValue = (uint8_t)(LeSWM_w_DelayStepTime / CeSWM_KEY_TASKTIME);

    if(0U != LpSWM_KeyStruct->e_u_KeyLngDownFlag)
    {
        LpSWM_KeyStruct->e_dw_LongDownTimeCnt--;

        if(0U == LpSWM_KeyStruct->e_dw_LongDownTimeCnt)
        {
            if(LeSWM_u_TempMinDelayValue == LpSWM_KeyStruct->e_dw_LngDnHisTimeCnt)
            {
                LpSWM_KeyStruct->e_dw_LongDownTimeCnt = LeSWM_u_TempMinDelayValue;
            }
            else
            {
                if(LeSWM_u_TempMinDelayValue < (LpSWM_KeyStruct->e_dw_LngDnHisTimeCnt - LeSWM_u_TempKeyDelayStepValue))
                {
                    LpSWM_KeyStruct->e_dw_LongDownTimeCnt = LpSWM_KeyStruct->e_dw_LngDnHisTimeCnt - LeSWM_u_TempKeyDelayStepValue;
                }
                else
                {
                    LpSWM_KeyStruct->e_dw_LongDownTimeCnt = LeSWM_u_TempMinDelayValue;
                }
                LpSWM_KeyStruct->e_dw_LngDnHisTimeCnt = LpSWM_KeyStruct->e_dw_LongDownTimeCnt;
            }

            LpSWM_KeyStruct->e_dw_OldStatus &= LpSWM_KeyStruct->e_dw_ContinuousWord;
            LpSWM_KeyStruct->e_dw_EventOldWord &= LpSWM_KeyStruct->e_dw_ContinuousWord;
        }
    }
    else
    {
        LpSWM_KeyStruct->e_dw_LongDownTimeCnt = (uint32_t)(((uint32_t)LeSWM_w_MaxTime) / CeSWM_KEY_TASKTIME) ;
        LpSWM_KeyStruct->e_dw_LngDnHisTimeCnt = (uint32_t)(((uint32_t)LeSWM_w_MaxTime) / CeSWM_KEY_TASKTIME) ;
    }
}

/*******************************************************************************
ID				: 13
Name			: ChkSWM_KeyContinuousStatus
Syntax			: void ChkSWM_KeyContinuousStatus(void)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: None						:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: check key continu func status
Call By			: SwM
|******************************************************************************/
STATIC void ChkSWM_KeyContinuousStatus(void)
{
    ServSWM_KeyContinuFuncDeal(&TsSWM_h_JYHwKeyStatus, KeSWM_w_FtpLongKeyMinDelayTime, KeSWM_w_FtpLongKeyDelayStep, KeSWM_w_FtpLongKeyMaxDelayTime);
}

/*******************************************************************************
ID				: 14
Name			: GetSWM_dw_KeyValue
Syntax			: uint32_t GetSWM_dw_KeyValue(TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: LpSWM_KeyStatus			:-
Parameters(out) : None						:-
Return value	: uint32_t						:-
Description		: get key value
Call By			: SwM
|******************************************************************************/
STATIC uint32_t GetSWM_dw_KeyValue(TsSWM_h_KeyStatusStruct *LpSWM_KeyStatus)
{
    uint32_t LeSWM_dw_KeyValue = 0U;

    /*scan key*/
    LeSWM_dw_KeyValue = LpSWM_KeyStatus->e_dw_Status ^ LpSWM_KeyStatus->e_dw_OldStatus;

    if(0U != LeSWM_dw_KeyValue)
    {
        /*have valid key*/
        LeSWM_dw_KeyValue &= LpSWM_KeyStatus->e_dw_Status;
    }
    LpSWM_KeyStatus->e_dw_OldStatus = LpSWM_KeyStatus->e_dw_Status;

    return(LeSWM_dw_KeyValue);
}


/*******************************************************************************
ID				: 19
Name			: Init_CddKey
Syntax			: void Init_CddKey(void)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: None						:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: set key history 8bit value
Call By			: ECUM
|******************************************************************************/
void Init_CddKey(void)
{
	/*mask continue key,1->close continue key 0->enable continue key*/
    ServSWM_InitKeyStatusStruct(&TsSWM_h_JYHwKeyStatus, 0xFFFFFBFFU);
}


/*0x12: 18*/
/*******************************************************************************
ID				: 20
Name			: CddKey_5ms_Task
Syntax			: void CddKey_5ms_Task(void)
Sync/Async		: Synchronous
Reentrancy		:
Parameters(in)	: None						:-
Parameters(out) : None						:-
Return value	: None						:-
Description		: check os task all
Call By			: OS_5ms
|******************************************************************************/
void CddKey_5ms_Task(void)
{
    ChkSWM_KeyRealTimeValue();
    ChkSWM_KeyStatus();
    ChkSWM_KeyContinuousStatus();
}

/*******************************************************************************
ID				:09
Name			:GetSWM_dw_JYHardwareKey
Syntax			:uint32_t GetSWM_dw_JYHardwareKey(void)
Sync/Async		:Synchronous
Reentrancy		:
Parameters(in)	:None						:-
Parameters(out) :None						:-
Return value	:None						:-
Description		:Get JY Hardware Key
Call By			:RTE
|******************************************************************************/
uint32_t GetSWM_dw_JYHardwareKey(void)
{
    uint32_t LeSWM_dw_KeyValue=0U;
    
   /*scan key*/
	LeSWM_dw_KeyValue = GetSWM_dw_KeyValue(&TsSWM_h_JYHwKeyStatus);    
	
    return(LeSWM_dw_KeyValue);
}

/**********************************************************************************************************************
 *  END OF FILE: SwM.c
***********************************************************************************************************************/

/*EOF*/
