
/*****************************************************************************
File Name        :  fdiag.c
Short Name       :  fdiag
VOBName          :  sw_di_int\2012_nissan_x11c_ic\
Author           :  ksreenat
Description:     :  Module for diagnostics Application
Organization     :  Driver Information Software Section
----------------------------------------------------------------------------
Compiler Name    :  Cosmic V4.7.11
Target Hardware  :  Sea wasp MC9S12XHY256, 144pin
*****************************************************************************/
#ifndef FDIAG_C
#define FDIAG_C
#define _DIAG_C
/*****************************************************************************
*                                 System Includes                            *
******************************************************************************/

#include "fm33lg0xx_fl.h"
#include "fm33lg0xx_fl_can.h"
#include "diag_kwp2k_di_core_mgr.cfg"
#include "diag_kwp2k_di_core_mgr.h"
#include "vnim_can_cfg.h"

#include "vnim_busoff_hndlr.h"
#include "nw_can_dll.h"
#include "vnim_app_signals.h"
#include "Canif.h"  

#include "system.h"
#include "rx_can.h"
#include "diag_kwp2k_di_core_erlg.cfg"
#include  "CddMtr_Mng.h"
#include  "Adcif.h"
#include "CddKey_Cfg.h"
#include "il_par.h"
#include "CddEeprom.h"
#include "Scm_SeatControl.h"
/*****************************************************************************
*                                 Project Includes                           *
******************************************************************************/
/*****************************************************************************
*                                 Type Declarations                          *
******************************************************************************/
/*****************************************************************************
*                                 File Scope Prototypes                      *
******************************************************************************/
static void f_diag_Init( void );
//static BOOLEAN compare_to_recv_key(UINT32 recv_key, UINT32 cal_key);

static BOOLEAN DTC_BAT_VOLTAGE_IS_HIGH(void);
static BOOLEAN DTC_BAT_VOLTAGE_IS_LOW(void);
static BOOLEAN DTC_IS_CAN_BUSS_OFF(void);
static BOOLEAN DTC_IS_EASY_IN_SW_STUCK(void);
static BOOLEAN DTC_IS_EASY_OUT_SW_STUCK(void);
static BOOLEAN DTC_IS_SEAT_FORWARD_SW_STUCK(void);
static BOOLEAN DTC_IS_SEAT_BACKWARD_SW_STUCK(void);
static BOOLEAN DTC_IS_BACK_FORWARD_SW_STUCK(void);
static BOOLEAN DTC_IS_BACK_BACKWARD_SW_STUCK(void);

static BOOLEAN DTC_IS_SEAT_HALL_SHROTGND(void);
static BOOLEAN DTC_IS_SEAT_HALL_SHROTBAT(void);
static BOOLEAN DTC_IS_BACK_HALL_SHROTGND(void);
static BOOLEAN DTC_IS_BACK_HALL_SHROTBAT(void);

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
static BOOLEAN DTC_VCU_MSG123_IS_TIMEOUT(void);
static BOOLEAN DTC_BCM_MSG458_IS_TIMEOUT(void);
static BOOLEAN DTC_ABM_MSG234_IS_TIMEOUT(void);
#else
static BOOLEAN DTC_BCM_MSG245_IS_TIMEOUT(void);
static BOOLEAN DTC_BCM_MSG457_IS_TIMEOUT(void);
#endif

static void f_diag_Handle_DTC_Logging(void);


/*****************************************************************************
*                                 Constants                                  *
*----------------------------------------------------------------------------*
* Declaration shall be followed by a comment that gives the following info.  *
* about the constant.                                                        *
* purpose, unit and resolution                                               *
******************************************************************************/
/*****************************************************************************
*                                 Manifest Constants                         *
*----------------------------------------------------------------------------*
* Definition of Manifest constant shall be followed by a comment that        *
* explains the purpose of the constant.                                      *
******************************************************************************/

/*****************************************************************************
*                                 Macro Definitions                          *
*----------------------------------------------------------------------------*
* Definition of macro shall be followed by a comment that explains the       *
* purpose of the macro.                                                      *
******************************************************************************/
#define FBL_REQ_VALUE           (0x55AA55AAU)
#define ECU_RESET_FLAG_ADDR     (0x20007FFCU)
#define ECU_RESET_REQ           (0xAA55AA55U)
/* 1:AUTO; 0: STATIC */
#define SECURITY_KEY_MASK		0x7CE41212UL

/* power state define */
#define POWER_OFF	0
#define POWER_ACC	1
#define POWER_IGN	2
#define POWER_START	3

#define APP_F170_ADDRESS	    0x03C000
#define APP_F171_ADDRESS	    0x03C004

#define APP_BATTERY_OFFSET      (30)
#define APP_MAX_U8              (0xFF)

#define BAT_VOL_BASE                   (68)    /* 68*0.1V = 6.8V */
#define BAT_AD_VAL_BASE                (283) /* AD Value when Vol is 6.8V */
#define BAT_AD_VAL_PER_1V              (44)  /* AD Value change when Vol change 1V */
#define BAT_VAL_FACOR                  (10)  /* 10 * 0.1V */
#define BAT_VOL_HIGH_NG_THRESHOLD      (180)
#define BAT_VOL_HIGH_OK_THRESHOLD      (170)
#define BAT_VOL_LOW_NG_THRESHOLD       (70)
#define BAT_VOL_LOW_OK_THRESHOLD       (80)

#define DTC_LOG_STAT_NOT_USED (0u)
/*0x27 security access buffer */
static UINT8 SID27_seed[VM_SEED_LENGTH]={0x55,0xaa,0xaa,0x55};
										
static UINT8 seed_to_get_key[4] = {0x55,0xaa,0xaa,0x55};

//static UINT32 get_seed__BASE_64MS_KSWTIMER_U16 = 0x1234;

#define LAZYBACK_MOTOR_ID    (0U)
#define HORIZON_MOTOR_ID     (1U)

/* 0X2F service DIDs buffer */
#define IOCTL_2F_RETURN_TRUE       0
#define IOCTL_MODE_DOOR_INDEX      1
#define IOCTL_BLD_DOOR_INDEX       2
#define IOCTL_REC_DOOR_INDEX       3

#if(SCM_SEATCONTROL_VARIANT == SCM_L_VARIANT)
/* 0X22 service DIDs */
#define F183_BOOT_SW_VERSION    'V','1','0','1','0','0','0','0','0','0','0','0','0','0','0','0'
#define F187_ECU_PART_NUMBER    'R','H','0','2','-','6','8','0','1','3','2','0','1'
#define F188_ECU_SW_NUMBER      'S','C','M','L','_','D','_','2','2','_','0','1','_','0','1'
#define F189_ECU_SW_VERSION     'S','0','0','0','0','0','0','A'
#define F18A_SUPPLIER_ID        '8','M','Q'
#define F18B_ECU_MANUFACTURE_DATE    0x20, 0x22, 0x06, 0x25
#define F18C_SERIAL_NUMBER      '9','9','9','9','9','9','2','2','0','6','1','5','1','A','5','0'
#define F190_VIN_DATA_ID        'A','B','C','D','E','F','G','0','1','2','3','4','5','6','7','8','9'
#define F191_HW_VER_NUMBER      'H','0','1','_','0','0','_','0','0','S','L'
#define F193_ECU_HW_NUMBER      'H','A','R','D','W','A','R','E','N','U','M','B','E','R','2'
#define F195_ECU_SW_NUMBER      'D','7','E','5','T','0','0','J','U','N','Y','I','0','0','A'
#define F197_ECU_NAME           'T','Y','_','D','S','C','M',' ',' ',' '
#define F199_PROGRAM_DATE                  0x20, 0x22, 0x06, 0x25
#define F19D_VEHICLE_MANUFACTURE_DATE      0x20, 0x22, 0x06, 0x25
#define F1A0_VEHICLE_CONFIG_DATA      0x0
#define F1F0_PROGRAM_COUNTER     0x0,0x3

#else
/* 0X22 service DIDs */
#define F183_BOOT_SW_VERSION    'V','1','0','1','0','0','0','0','0','0','0','0','0','0','0','0'
#define F187_ECU_PART_NUMBER    'R','H','0','2','-','6','9','0','1','3','2','0','1'
#define F188_ECU_SW_NUMBER      'S','C','M','R','_','D','_','2','2','_','0','1','_','0','1'
#define F189_ECU_SW_VERSION     'S','0','0','0','0','0','0','A'
#define F18A_SUPPLIER_ID        '8','M','Q'
#define F18B_ECU_MANUFACTURE_DATE    0x20, 0x22, 0x06, 0x25
#define F18C_SERIAL_NUMBER      '9','9','9','9','9','9','2','2','0','6','1','5','1','A','5','0'
#define F190_VIN_DATA_ID        'A','B','C','D','E','F','G','0','1','2','3','4','5','6','7','8','9'
#define F191_HW_VER_NUMBER      'H','0','1','_','0','0','_','0','0','S','R'
#define F193_ECU_HW_NUMBER      'H','A','R','D','W','A','R','E','N','U','M','B','E','R','2'
#define F195_ECU_SW_NUMBER      'D','7','E','5','T','0','0','J','U','N','Y','I','0','0','A'
#define F197_ECU_NAME           'T','Y','_','D','S','C','M',' ',' ',' '
#define F199_PROGRAM_DATE                  0x20, 0x22, 0x06, 0x25
#define F19D_VEHICLE_MANUFACTURE_DATE      0x20, 0x22, 0x06, 0x25
#define F1A0_VEHICLE_CONFIG_DATA      0x0
#define F1F0_PROGRAM_COUNTER     0x0,0x3

#endif  /* end of #if(SCM_SEATCONTROL_VARIANT == SCM_L_VARIANT) */

#define F187_ECU_PART_NUMBER_LEN    (13U)
#define F188_ECU_SW_NUMBER_LEN      (15U)
#define F18A_SUPPLIER_ID_LEN        (3U)
#define F191_HW_VER_NUMBER_LEN      (11U)
#define F197_ECU_NAME_LEN           (10U)

UINT8 IOCTL_WPC_buffer[IOCTL_WPC_SIZE];
UINT8 IOCTL_BT_buffer[IOCTL_BT_SIZE];

UINT8 fdiag_3700_KSIOCTL_enabled_flag;
UINT8 fdiag_3701_KSIOCTL_enabled_flag;

UINT8 * fl_msgptr;

//extern UINT32 FBL_REQ_FLAG;

uint32_t FBL_REQ_FLAG __attribute__((section(".ARM.__at_0x20007FFC"))) = 0x00000000;

#ifndef DEBUG_ENABLE
const uint32_t APP_VALID_1 __attribute__((section(".ARM.__at_0x00008400"))) = 0xAAAAAAAA;
const uint32_t APP_VALID_2 __attribute__((section(".ARM.__at_0x0001F000"))) = 0xAAAAAAAA;
#endif
/*****************************************************************************
*                                 Globally  accessed Variable Declarations   *
*----------------------------------------------------------------------------*
* Declaration shall be followed by a comment that gives the following info.  *
* about the variable.                                                        *
* purpose, critical section, unit, resolution, Valid Range and ValidityCheck *
******************************************************************************/
/*****************************************************************************
*                                 Locally used Variable Declarations         *
*----------------------------------------------------------------------------*
* Declaration shall be followed by a comment that gives the following info.  *
* about the variable.                                                        *
* purpose, critical section, unit, resolution, Valid Range and ValidityCheck *
******************************************************************************/
/*
purpose          : Variable to store the diag session timer ststus
critical section : None
Range            : TRUE/FALSE
Unit             : None
Resoultion       : None
*/
static BOOLEAN l_diag_timeout_flag_BOOL;


/*****************************************************************************
*                                 Functions                                  *
******************************************************************************/
/****************************************************************************
Function Name     : fdiag_KSColdinit

Description       : Cold init function for diagnostics application 

Invocation        : Sched cfg on battery re-connect list

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
void fdiag_KSColdinit(void)
{
    l_diag_timeout_flag_BOOL = FALSE;
    f_diag_Init();

	return;
}
/****************************************************************************
Function Name     : fdiag_KSWarminit

Description       : Warm init function for diagnostics application 

Invocation        : Sched cfg on warm reset list

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
void fdiag_KSWarminit(void)
{
    l_diag_timeout_flag_BOOL = FALSE;
    f_diag_Init();

	return;
}
/****************************************************************************
Function Name     : fdiag_KSWakeup

Description       : Wakeup function for diagnostics application  

Invocation        : Sched cfg on wakeup list 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
void fdiag_KSWakeup(void)
{
    l_diag_timeout_flag_BOOL = FALSE;
    f_diag_Init();

	return; 
}
void fdiag_Transinto_Keyoff(void)
{
	fdiag_KSColdinit();
	UPDATE_SESSION(DEFAULTSESSION);
    //KernelClear13BitTimer(IGN_on_5S_KSWTimer);
	//KernelClear13BitTimer(IGN_on_1S_KSWTimer);	

	return;
}

/*============================================================================
**
** Function Name:      f_diag_Init( void )
**
** Visibility:         Local to fdiag.c
**
** Description:        Handles the intialization of the current
**                     configuration data and the System ID information
**                     for use in diagnostic queries.
**
** Invocation:         Called by: KSColdInit
**                                KSWarmInit
**                                KSWakeup
**
** Inputs/Outputs:     1)System ID bytes - O
**                       Global volatile 24 byte array of
**                       bytes containing system ID information to support
**                       service ID $21, local ID $83
**
** Critical Section:   none
**
**==========================================================================*/
static void f_diag_Init( void )
{ 
  fdiag_3700_KSIOCTL_enabled_flag =0;
  fdiag_3701_KSIOCTL_enabled_flag =0;

  IOCTL_WPC_buffer[0] = 0;
  IOCTL_BT_buffer[0] = 0;
}


/****************************************************************************
Function Name     : Prod_Spec_Mode10_02_Handler

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
void Prod_Spec_Mode10_02_Handler(void)
{
    UINT8 buff[8] = {0};
    UINT8 i;
    Canif_Msg_Str  fl_str_e;
    UINT8 EEPROM_Condition = FALSE;
    UINT8 MtrMng_Condition = FALSE;
    UINT8 diag_neg_response = FALSE;

    buff[0] = 0x03;
    buff[1] = 0x7f;
    buff[2] = 0x10;
    buff[3] = 0x78;
    
    fl_str_e.StdId = VTP_DIAG_RESPONSE_ID;
    fl_str_e.DLC = 8;

    for(i=0; i<fl_str_e.DLC; i++)
    {
        fl_str_e.Data[i] = buff[i];        
    }

    FL_CAN_Frame_Send(FL_CAN_FORMAT_STANDARD_DATA,&fl_str_e);


    EEPROM_Condition = CddEeprom_Get_SleepCondition();
    MtrMng_Condition = CddMtr_Get_SleepCondition();

    if((TRUE == EEPROM_Condition) && (TRUE == MtrMng_Condition))
    {
        if(TRUE ==CddMtr_Write_To_EEPROM())
        {
            FL_DelayMs(100);
            FBL_REQ_FLAG = FBL_REQ_VALUE;
            while(1);
        }
        else
        {
            diag_neg_response = TRUE;
        }
    }
    else
    {
        diag_neg_response = TRUE;
    }

    if(diag_neg_response == TRUE)
    {
        buff[0] = 0x03;
        buff[1] = 0x7f;
        buff[2] = 0x10;
        buff[3] = 0x22;
        
        fl_str_e.StdId = VTP_DIAG_RESPONSE_ID;
        fl_str_e.DLC = 8;

        for(i=0; i<fl_str_e.DLC; i++)
        {
            fl_str_e.Data[i] = buff[i];        
        }

        FL_CAN_Frame_Send(FL_CAN_FORMAT_STANDARD_DATA,&fl_str_e);
    }
}

/*****************************************************************************
Function Name     : fdiag_diag_entry_check

Description       : Entry check for diag mode

Invocation        : Called from PROD_SPEC_DIAG_SESSION_TABLE

Parameters        : None

Return Value      : TRUE/FALSE

Critical Section  : None

Traceability      : fdiag_dd.c

******************************************************************************/
BOOLEAN fdiag_diag_entry_check(void)
{
    BOOLEAN fl_diag_entry_bool;

    fl_diag_entry_bool = FALSE;

    return(fl_diag_entry_bool);
}


/*****************************************************************************
Function Name     : fdiag_extended_prolog

Description       : Prolog for extended session (dealer session)

Invocation        : Called from PROD_SPEC_DIAG_SESSION_TABLE

Parameters        : None

Return Value      : None

Critical Section  : None

Traceability      : fdiag_dd.c

******************************************************************************/
void fdiag_extended_prolog(void)
{
    l_diag_timeout_flag_BOOL = FALSE;
    //UPDATE_SESSION(DIAG_DF_SESS);


    /*$27 standby */
    kwp2k_core_Diag_27_standby();

    /*$2F standby */
    kwp2k_core_Diag_2F_standby();

    /*$28 standby */
    kwp2k_core_Diag_28_standby();
    //kwp2k_core_Diag_85_standby();
}

/****************************************************************************
Function Name     : fdiag_diag_go_default_seasion
                                   
Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
void fdiag_diag_go_default_seasion(void)
{
    /*$27 standby */
	kwp2k_core_Diag_27_standby();

	/*$2F standby */
	kwp2k_core_Diag_2F_standby();

	/*$28 standby */
	kwp2k_core_Diag_28_standby();
	//kwp2k_core_Diag_85_standby();

	return;
}

/*****************************************************************************
Function Name     : fdiag_extended_epilog

Description       : Entry check for diag mode

Invocation        : Called from PROD_SPEC_DIAG_SESSION_TABLE

Parameters        : None

Return Value      : TRUE/FALSE

Critical Section  : None

Traceability      : fdiag_dd.c

******************************************************************************/
void fdiag_extended_epilog(void)
{
    l_diag_timeout_flag_BOOL = FALSE;

	/*$27 standby */
	kwp2k_core_Diag_27_standby();

	/*$2F standby */
	kwp2k_core_Diag_2F_standby();

	/*$28 standby */
	kwp2k_core_Diag_28_standby();

	kwp2k_core_Diag_85_standby();

	return;
}
/*****************************************************************************
Function Name     : fdiag_diag_exit_session_check

Description       : Entry check for diag mode

Invocation        : Called from PROD_SPEC_DIAG_SESSION_TABLE

Parameters        : None

Return Value      : TRUE/FALSE

Critical Section  : None

Traceability      : fdiag_dd.c

******************************************************************************/
BOOLEAN fdiag_diag_exit_session_check(void)
{
    BOOLEAN fl_diag_exit_session_BOOL;

    fl_diag_exit_session_BOOL = TRUE;

    if ( FALSE != l_diag_timeout_flag_BOOL )      /* P3 timeout */
    {
        fl_diag_exit_session_BOOL = FALSE;  
    }

    return(fl_diag_exit_session_BOOL);
}
/*****************************************************************************
Function Name     : ApplDiagServiceTimeout

Description       : Service timeout indication to diag application from KWPDIAG

Invocation        : Called for busy repeat request to tester

Parameters        : None

Return Value      : None

Critical Section  : None

Traceability      : fdiag_dd.c

******************************************************************************/
void ApplDiagServiceTimeout(void)
{
    UINT8 fl_NRC_val_U8;
    fl_NRC_val_U8 = kwp2k_diag_NrcBusyRepeatRequest;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(&fl_NRC_val_U8);

	return;
}
/*****************************************************************************
Function Name     : ApplDiagSessionTimeout

Description       : Seccion timeout indication to diag application from KWPDIAG

Invocation        : Called from kwpdiag_P3timeout

Parameters        : None

Return Value      : None

Critical Section  : None

Traceability      : fdiag_dd.c

******************************************************************************/
void ApplDiagSessionTimeout(void)
{
    l_diag_timeout_flag_BOOL = TRUE;
}

/****************************************************************************
Function Name     : fdiag_get_seed
                                   
Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/

UINT8 fdiag_get_seed(UINT8 *buffer)
{
    UINT8 index;

    for (index=0; index<VM_SEED_LENGTH; index++)
    {
        buffer[index] = SID27_seed[index];
        /*Store seed for calculate key*/
        
    }

    return 0;
}



UINT8 fdiag_verify_key(UINT8 *buffer)
{
/* Compare the key generated by the diag and tester key */
 /* Verify the Key return true for sucess 0 for false  */
    UINT32 receive_key;
    UINT32 get_cal_key;

	receive_key = ((UINT32)buffer[0]) << 24;
	receive_key |= ((UINT32)buffer[1]) << 16;
	receive_key |= ((UINT32)buffer[2]) << 8;
	receive_key |= ((UINT32)buffer[3]);


    get_cal_key = seed_to_get_key[0];
    get_cal_key |= seed_to_get_key[1];
    get_cal_key |= seed_to_get_key[2];
    get_cal_key |= seed_to_get_key[3];

    if ((seed_to_get_key[0] != buffer[0]) ||\
		(seed_to_get_key[1] != buffer[1]) ||\
		(seed_to_get_key[2] != buffer[2]) ||\
		(seed_to_get_key[3] != buffer[3]) )
    {
        return kwp2k_diag_NrcRequestInvalidKey;
    }
    else
    {
        return 0; 
    }


}
/****************************************************************************
Function Name:      fdiag_TimedTask( void )

Description:        Round Robin function that always runs to poll the
                    routines that log Network Errors.
                    These functions must be polled during normal operation
                    to gather data used during diagnostics sessions.
                    Call f_diag_Network_Error_Logging.

Invocation:         Called periodically from Kernel scheduler.

Parameters        : none

Return Value      : None

Critical Section  : None
*****************************************************************************/
void fdiag_TimedTask(void)
{
#if 0
    UINT8 cur_ses;
    UINT8 prev_ses;
    UINT8 superstate;
    UINT8 fl_sched_context_U8;
#endif


#if 0
    superstate = FSM_get_SuperState_U8();
    
    cur_ses = get_diag_cur_session();
    prev_ses = get_diag_prev_session();

    /* curerent is default session and last is extend session */
    if((cur_ses == 0) && (prev_ses == 1))
    {
        if(superstate != NORMAL_MODE)
        {
            FSM_Set_Immediate_SuperState(NORMAL_MODE);
        }
    }
    /* curerent is extend session and last is deault session */
    else if((cur_ses == 1) && (prev_ses == 0))
    {
        if((superstate != DIAGNOSTIC_MODE)&&(superstate != FRONT_PANEL_DIAG_MODE))
        {
            FSM_Set_Immediate_SuperState(DIAGNOSTIC_MODE);
        }
    } 
    else
    {
         
    }

    fl_sched_context_U8 = KernelGetSchedulerContext();

    if((NORMAL_MODE == fl_sched_context_U8) 
        || (DIAGNOSTIC_MODE == fl_sched_context_U8))
    {
	    f_diag_Handle_DTC_Logging();
        f_diag_Handle_DTC_Logging_Network();
    }
    
    f_diag_Handle_DTC_Logging_Voltage();
#endif



	return;
}
/****************************************************************************
Function Name:      fdiag_Robin( void )

Description:        Round Robin function that always runs to poll the
                    routines that log Network Errors.
                    These functions must be polled during normal operation
                    to gather data used during diagnostics sessions.
                    Call f_diag_Network_Error_Logging.

Invocation:         Called periodically from Kernel scheduler.

Parameters        : none

Return Value      : None

Critical Section  : None
*****************************************************************************/
void fdiag_Robin(void)
{
    f_diag_Handle_DTC_Logging();
}


/****************************************************************************
Function Name     : fdiag_app_1701_get_motor_PosCnt

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_1701_get_motor_PosCnt(UINT8 *buffer)
{
    UINT16 horizon_motor_pos = 0;
    UINT16 lazyback_motor_pos = 0;

    lazyback_motor_pos = CddMtr_Get_Current_Hall_Cnt(LAZYBACK_MOTOR_ID);
    horizon_motor_pos = CddMtr_Get_Current_Hall_Cnt(HORIZON_MOTOR_ID);

    *buffer = (UINT8)(horizon_motor_pos >> 8);
    *(buffer+1) = (UINT8)(horizon_motor_pos);
    *(buffer+2) = (UINT8)(lazyback_motor_pos >> 8);
    *(buffer+3) = (UINT8)(lazyback_motor_pos);
    
    return 4;
}


/****************************************************************************
Function Name     : fdiag_app_1702_get_motor_SoftPosCnt

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_1702_get_motor_SoftPosCnt(UINT8 *buffer)
{
    UINT16 horizon_motor_softpos_max = 0;
    UINT16 horizon_motor_softpos_min = 0;
    UINT16 lazyback_motor_softpos_max = 0;
    UINT16 lazyback_motor_softpos_min = 0;

    horizon_motor_softpos_min = Get_SeatMotor_SoftPoint_Min();
    horizon_motor_softpos_max = Get_SeatMotor_SoftPoint_Max();

    lazyback_motor_softpos_max = Get_SeatBackMotor_SoftPoint_Max();
    lazyback_motor_softpos_min = Get_SeatBackMotor_SoftPoint_Min();

    *buffer = (UINT8)(horizon_motor_softpos_max >> 8);
    *(buffer+1) = (UINT8)(horizon_motor_softpos_max);
    *(buffer+2) = (UINT8)(horizon_motor_softpos_min >> 8);
    *(buffer+3) = (UINT8)(horizon_motor_softpos_min);

    *(buffer+4) = (UINT8)(lazyback_motor_softpos_max >> 8);
    *(buffer+5) = (UINT8)(lazyback_motor_softpos_max);
    *(buffer+6) = (UINT8)(lazyback_motor_softpos_min >> 8);
    *(buffer+7) = (UINT8)(lazyback_motor_softpos_min);

    
    return 8;
}


/****************************************************************************
Function Name     : fdiag_app_170B_get_BAT_Voltage

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_170B_get_BAT_Voltage(UINT8 *buffer)
{
    UINT16 adc_val_bat = 0;
    UINT8 bat_vol = 0; /* factor :0.1V */

    adc_val_bat = Adcif_Get_AdcVal(ADCIF_CH_BATT1_AD); 

    bat_vol = BAT_VOL_BASE + (BAT_VAL_FACOR * (adc_val_bat -  BAT_AD_VAL_BASE) / BAT_AD_VAL_PER_1V); 
    *buffer = bat_vol;
    
    return 1;
}


/****************************************************************************
Function Name     : fdiag_app_1706_get_motor_learn_sts

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_1706_get_motor_learn_sts(UINT8 *buffer)
{
    UINT8 motor_learn_sts = 0;

    if(CddMtr_Get_LearnData_Status(HORIZON_MOTOR_ID) == CDDMTR_MNG_LEARN_VALID)
    {
        motor_learn_sts |= BIT0;
    }

    if(CddMtr_Get_LearnData_Status(LAZYBACK_MOTOR_ID) == CDDMTR_MNG_LEARN_VALID)
    {
        motor_learn_sts |= BIT1;
    }

   *buffer = motor_learn_sts;
    
    return 1;
}


/****************************************************************************
Function Name     : fdiag_app_1707_get_seat_motor_current

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_1707_get_seat_motor_current(UINT8 *buffer)
{
    UINT16 seat_motor_current = 0;

    seat_motor_current = CddMtr_Get_Mtr_Current_Val(HORIZON_MOTOR_ID);
   *buffer = (UINT8)(seat_motor_current >> 8);
   *(buffer+1) = (UINT8)(seat_motor_current);
    
    return 2;
}


/****************************************************************************
Function Name     : fdiag_app_1708_get_key_sts

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_1708_get_key_sts(UINT8 *buffer)
{
    UINT8 input_key_sts = 0;

    if(ServSWM_u_GetKey_Seatback_B_SW_RealVaule())
    {
        input_key_sts |= BIT3;
    }
    else
    {
        input_key_sts &= BIT3;
    }

    if(ServSWM_u_GetKey_Seatback_F_SW_RealVaule())
    {
        input_key_sts |= BIT2;
    }
    else
    {
        input_key_sts &= BIT2;
    }

    if(ServSWM_u_GetKey_Seat_B_SW_RealVaule())
    {
        input_key_sts |= BIT1;
    }
    else
    {
        input_key_sts &= BIT1;
    }
    
    if(ServSWM_u_GetKey_Seat_F_SW_RealVaule())
    {
        input_key_sts |= BIT0;
    }
    else
    {
        input_key_sts &= BIT0;
    }


   *buffer = input_key_sts;
    
    return 1;
}


/****************************************************************************
Function Name     : fdiag_app_1709_get_motor_sts

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_1709_get_motor_sts(UINT8 *buffer)
{
    UINT8 lazy_back_motor_sts = 0;
    UINT8 horizon_motor_sts = 0;
    UINT8 motor_sts = 0;

    lazy_back_motor_sts = CddMtr_Get_Mtr_Run_Direction(LAZYBACK_MOTOR_ID);
    horizon_motor_sts = CddMtr_Get_Mtr_Run_Direction(HORIZON_MOTOR_ID);

    if(lazy_back_motor_sts == CDDMTR_HFKF_REQ_BACKWARD)
    {
        motor_sts |= BIT3;
    }
    else
    {
        motor_sts &= BIT3;
    }

    if(lazy_back_motor_sts == CDDMTR_HFKF_REQ_FORWARD)
    {
        motor_sts |= BIT2;
    }
    else
    {
        motor_sts &= BIT2;
    }

    if(horizon_motor_sts == CDDMTR_HFKF_REQ_BACKWARD)
    {
        motor_sts |= BIT1;
    }
    else
    {
        motor_sts &= BIT1;
    }

    if(horizon_motor_sts == CDDMTR_HFKF_REQ_FORWARD)
    {
        motor_sts |= BIT0;
    }
    else
    {
        motor_sts &= BIT0;
    }
    
    *buffer = motor_sts;

    return 1;
}


/****************************************************************************
Function Name     : fdiag_app_f183_get_boot_SW_version

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f183_get_boot_SW_version(UINT8 *buffer)
{
    UINT8 fl_respLength_U8;
    UINT8 fl_temp_index ;

    UINT8 boot_sw_version[16] = {F183_BOOT_SW_VERSION};
    
    fl_respLength_U8 = 16;

    for(fl_temp_index = 0;fl_temp_index < fl_respLength_U8;fl_temp_index++)
    {
        buffer[fl_temp_index] = boot_sw_version[fl_temp_index];
    }
    
    return (fl_respLength_U8);
}


/****************************************************************************
Function Name     : fdiag_app_f183_get_boot_SW_version

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f186_get_current_session(UINT8 *buffer)
{
    UINT8 fl_respLength_U8;
    
    fl_respLength_U8 = 1;
    if(kwpdiag_currsess == DIAG_EXT_SESS)
    {
        *buffer = 3;
    }
    else
    {
        *buffer = kwpdiag_currsess;
    }
   
    return (fl_respLength_U8);
}


/****************************************************************************
Function Name     : fdiag_app_f187_get_OEM_Partnumber

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f187_get_OEM_Partnumber(UINT8 *buffer)
{
	UINT8 fl_temp_index ;
    UINT8 ecu_part_number[F187_ECU_PART_NUMBER_LEN] = {F187_ECU_PART_NUMBER};
    
    for(fl_temp_index = 0;fl_temp_index < F187_ECU_PART_NUMBER_LEN;fl_temp_index++)
    {
        buffer[fl_temp_index] = ecu_part_number[fl_temp_index];
    }
    
    return (F187_ECU_PART_NUMBER_LEN);
}


/****************************************************************************
Function Name     : fdiag_app_f188_get_Software_Number

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f188_get_Software_Number(UINT8 *buffer)
{
    UINT8 fl_temp_index ;
    UINT8 sw_number[F188_ECU_SW_NUMBER_LEN] = {F188_ECU_SW_NUMBER};
    
    for(fl_temp_index = 0;fl_temp_index < F188_ECU_SW_NUMBER_LEN;fl_temp_index++)
    {
        buffer[fl_temp_index] = sw_number[fl_temp_index];
    }

    return (F188_ECU_SW_NUMBER_LEN);
}


/****************************************************************************
Function Name     : extern kwpdiagu8 kwpdiag_currsess;

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f189_get_SoftwareVersion(UINT8 *buffer)
{
    UINT8 fl_respLength_U8;
    UINT8 fl_temp_index ;
    UINT8 sw_version[8] = {F189_ECU_SW_VERSION};
    
    fl_respLength_U8 = 8;

    for(fl_temp_index = 0;fl_temp_index < fl_respLength_U8;fl_temp_index++)
    {
        buffer[fl_temp_index] = sw_version[fl_temp_index];
    }

    return (fl_respLength_U8);
}


/****************************************************************************
Function Name     : fdiag_app_f18a_get_SupplyID

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f18a_get_SupplyID(UINT8 *buffer)
{
	UINT8 fl_temp_index ;
    UINT8 supplier_id[F18A_SUPPLIER_ID_LEN] = {F18A_SUPPLIER_ID};

    for(fl_temp_index = 0;fl_temp_index < F18A_SUPPLIER_ID_LEN;fl_temp_index++)
    {
        buffer[fl_temp_index] = supplier_id[fl_temp_index];
    }

    return (F18A_SUPPLIER_ID_LEN);
}



/****************************************************************************
Function Name     : fdiag_app_f18b_get_ecu_manufacture_date

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f18b_get_ecu_manufacture_date(UINT8 *buffer)
{
    UINT8 fl_respLength_U8;
    UINT8 fl_temp_index ;
    UINT8 ecu_manufacture_date[4] = {F18B_ECU_MANUFACTURE_DATE};
    
    fl_respLength_U8 = 4;

    for(fl_temp_index = 0;fl_temp_index < fl_respLength_U8;fl_temp_index++)
    {
        buffer[fl_temp_index] = ecu_manufacture_date[fl_temp_index];
    }

    return (fl_respLength_U8);
}


/****************************************************************************
Function Name     : fdiag_app_f18c_get_serial_number

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f18c_get_serial_number(UINT8 *buffer)
{
    UINT8 fl_respLength_U8;
    UINT8 fl_temp_index ;
    UINT8 serial_number[16] = {F18C_SERIAL_NUMBER};
    
    fl_respLength_U8 = 16;

    for(fl_temp_index = 0;fl_temp_index < fl_respLength_U8;fl_temp_index++)
    {
        buffer[fl_temp_index] = serial_number[fl_temp_index];
    }

    return (fl_respLength_U8);
}


/****************************************************************************
Function Name     : fdiag_app_f190_get_serial_number

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f190_get_vin_data(UINT8 *buffer)
{
    UINT8 fl_respLength_U8;
    UINT8 fl_temp_index ;
    UINT8 vin_data[17] = {F190_VIN_DATA_ID};
    
    fl_respLength_U8 = 17;

    for(fl_temp_index = 0;fl_temp_index < fl_respLength_U8;fl_temp_index++)
    {
        buffer[fl_temp_index] = vin_data[fl_temp_index];
    }

    return (fl_respLength_U8);
}


/****************************************************************************
Function Name     : fdiag_app_f191_get_hw_ver_number_data

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f191_get_hw_ver_number_data(UINT8 *buffer)
{
    UINT8 fl_temp_index ;
    UINT8 vin_data[F191_HW_VER_NUMBER_LEN] = {F191_HW_VER_NUMBER};

    for(fl_temp_index = 0;fl_temp_index < F191_HW_VER_NUMBER_LEN;fl_temp_index++)
    {
        buffer[fl_temp_index] = vin_data[fl_temp_index];
    }

    return (F191_HW_VER_NUMBER_LEN);
}


/****************************************************************************
Function Name     : fdiag_app_f193_get_ecu_HW_number

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f193_get_ecu_HW_number(UINT8 *buffer)
{
    UINT8 fl_respLength_U8;
    UINT8 fl_temp_index ;
    UINT8 hw_number[15] = {F193_ECU_HW_NUMBER};
    
    fl_respLength_U8 = 15;

    for(fl_temp_index = 0;fl_temp_index < fl_respLength_U8;fl_temp_index++)
    {
        buffer[fl_temp_index] = hw_number[fl_temp_index];
    }

    return (fl_respLength_U8);
}


/****************************************************************************
Function Name     : fdiag_app_f195_get_ecu_SW_number

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f195_get_ecu_SW_number(UINT8 *buffer)
{
    UINT8 fl_respLength_U8;
    UINT8 fl_temp_index ;
    UINT8 ecu_sw_number[15] = {F195_ECU_SW_NUMBER};
    
    fl_respLength_U8 = 15;

    for(fl_temp_index = 0;fl_temp_index < fl_respLength_U8;fl_temp_index++)
    {
        buffer[fl_temp_index] = ecu_sw_number[fl_temp_index];
    }

    return (fl_respLength_U8);
}


/****************************************************************************
Function Name     : fdiag_app_f197_get_ECU_Name

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f197_get_ECU_Name(UINT8 *buffer)
{   
    UINT8 fl_temp_index ;
    UINT8 ecu_name[F197_ECU_NAME_LEN] = {F197_ECU_NAME};

    for(fl_temp_index = 0;fl_temp_index < F197_ECU_NAME_LEN;fl_temp_index++)
    {
        buffer[fl_temp_index] = ecu_name[fl_temp_index];
    }
    
    return (F197_ECU_NAME_LEN);
}


/****************************************************************************
Function Name     : fdiag_app_f199_get_ECU_Name

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f199_get_program_date(UINT8 *buffer)
{   
    UINT8 fl_respLength_U8;
    UINT8 fl_temp_index ;
    UINT8 program_date[4] = {F199_PROGRAM_DATE};
    
    fl_respLength_U8 = 4;

    for(fl_temp_index = 0;fl_temp_index < fl_respLength_U8;fl_temp_index++)
    {
        buffer[fl_temp_index] = program_date[fl_temp_index];
    }
    
    return (fl_respLength_U8);
}


/****************************************************************************
Function Name     : fdiag_app_f19d_get_vehicle_manu_date

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_f19d_get_vehicle_manu_date(UINT8 *buffer)
{   
    UINT8 fl_respLength_U8;
    UINT8 fl_temp_index ;
    UINT8 vehicle_manufacture_date[4] = {F19D_VEHICLE_MANUFACTURE_DATE};
    
    fl_respLength_U8 = 4;

    for(fl_temp_index = 0;fl_temp_index < fl_respLength_U8;fl_temp_index++)
    {
        buffer[fl_temp_index] = vehicle_manufacture_date[fl_temp_index];
    }
    
    return (fl_respLength_U8);
}


/***********************  Write Data By Identifier ***************************/	
/****************************************************************************
Function Name     : fdiag_app_0100_set_cfg_information

Description       : 

Invocation        : 

Parameters        : none

Return Value      : None

Critical Section  : None

******************************************************************************/
 UINT8 fdiag_app_0100_set_cfg_information(UINT8 *buffer)
{
//    UINT8 fl_NVM_write_status_BOOL;
    UINT8 fl_write_status_U8;


	return (fl_write_status_U8);

}


/**********************  input/output control By Identifier *******************/

/* WPC IO Control */    
UINT8 dg_app_3700_KSIOCTL(UINT8 control_option, UINT8 buffer_length, UINT8 *buffer)
{
    UINT8 ret;

    if(RETURN_CONTROL_ECU == control_option)
    {
        /*buffer_length is datavalue size plus mask size*/
        if(buffer_length > 0)
        {
            ret = kwp2k_diag_NrcInvalidFormat;
        }
        else
        {
            /*return control to ECU */
            fdiag_3700_KSIOCTL_enabled_flag = 0x00;
        }
    }
    else if(SHORT_TERM_ADJUST == control_option)
    {
        if(buffer_length != IOCTL_WPC_SIZE)
        {
            ret = kwp2k_diag_NrcInvalidFormat;
        }
        else
        {
            fdiag_3700_KSIOCTL_enabled_flag = 0x01;
            if((buffer[0] == 0) || (buffer[0] == 1))
            {
                IOCTL_WPC_buffer[0] = buffer[0];
            } 
        }       
    }
    else
    {
        ret = kwp2k_diag_NrcRequestOutOfRange;
    }

    return ret;
}


void dg_app_3700_KSAbortIOCTL(void)
{
	fdiag_3700_KSIOCTL_enabled_flag = 0x00;
}


/* BT Quick connection IO Control */   
UINT8 dg_app_3701_KSIOCTL(UINT8 control_option, UINT8 buffer_length, UINT8 *buffer)
{
    UINT8 ret;

    if(RETURN_CONTROL_ECU == control_option)
    {
        /*buffer_length is datavalue size plus mask size*/
        if(buffer_length > 0)
        {
            ret = kwp2k_diag_NrcInvalidFormat;
        }
        else
        {
            /*return control to ECU */
            fdiag_3701_KSIOCTL_enabled_flag = 0x00;
        }
    }
    else if(SHORT_TERM_ADJUST == control_option)
    {
        if(buffer_length != IOCTL_BT_SIZE)
        {
            ret = kwp2k_diag_NrcInvalidFormat;
        }
        else
        {
            fdiag_3701_KSIOCTL_enabled_flag = 0x01;
            IOCTL_BT_buffer[0] = buffer[0];
        }       
    }
    else
    {
        ret = kwp2k_diag_NrcRequestOutOfRange;
    }

    return ret;
}


void dg_app_3701_KSAbortIOCTL(void)
{
    fdiag_3701_KSIOCTL_enabled_flag = 0x00;
}


/****************************************************************************
Function Name     : DTC_GW320_IS_TIMEOUT

Description       : This function is used for setting the response status for the write operation.

Invocation        : dg_app

Parameters        : None

Return Value      : UINT8

Critical Section  : None

******************************************************************************/
static BOOLEAN DTC_BAT_VOLTAGE_IS_HIGH(void)
{
    BOOLEAN IsBatt_high = FALSE;
    UINT16 adc_val_bat = 0;
    UINT8 bat_vol = 0; /* factor :0.1V */
    static UINT8 ng_debounce_cnt = 0;
    static UINT8 ok_debounce_cnt = 0;

    adc_val_bat = Adcif_Get_AdcVal(ADCIF_CH_BATT1_AD); 

    bat_vol = BAT_VOL_BASE + (BAT_VAL_FACOR * (adc_val_bat -  BAT_AD_VAL_BASE) / BAT_AD_VAL_PER_1V); 

    if(bat_vol > BAT_VOL_HIGH_NG_THRESHOLD) /* 18V */
    {
        ng_debounce_cnt++;
        if(ng_debounce_cnt >= 100) /* 50ms * 100 = 5S */
        {
            IsBatt_high = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else if(bat_vol <= BAT_VOL_HIGH_OK_THRESHOLD)  /* 17V */
    {
        ok_debounce_cnt++;
        if(ok_debounce_cnt >= 20) /* 50ms * 20 = 1S */
        {
            IsBatt_high = FALSE;
            ok_debounce_cnt = 0;
        }
    } 
    else 
    {
        /* Do nothing */
    }   

	return IsBatt_high;	
}
/****************************************************************************
Function Name     : DTC_GW320_IS_TIMEOUT

Description       : This function is used for setting the response status for the write operation.

Invocation        : dg_app

Parameters        : None

Return Value      : UINT8

Critical Section  : None

******************************************************************************/
static BOOLEAN DTC_BAT_VOLTAGE_IS_LOW(void)
{
    BOOLEAN IsBatt_low = FALSE;
    UINT16 adc_val_bat = 0;
    UINT8 bat_vol = 0; /* factor :0.1V */
    static UINT8 ng_debounce_cnt = 0;
    static UINT8 ok_debounce_cnt = 0;

    adc_val_bat = Adcif_Get_AdcVal(ADCIF_CH_BATT1_AD); 

    bat_vol = BAT_VOL_BASE + (BAT_VAL_FACOR * (adc_val_bat -  BAT_AD_VAL_BASE) / BAT_AD_VAL_PER_1V); 

    if(bat_vol < BAT_VOL_LOW_NG_THRESHOLD) /* 7V */
    {
        ng_debounce_cnt++;
        if(ng_debounce_cnt >= 100) /* 50ms * 100 = 5S */
        {
            IsBatt_low = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else if(bat_vol >= BAT_VOL_LOW_OK_THRESHOLD)  /* 8V */
    {
        ok_debounce_cnt++;
        if(ok_debounce_cnt >= 20) /* 50ms * 20 = 1S */
        {
            IsBatt_low = FALSE;
            ok_debounce_cnt = 0;
        }
    } 
    else 
    {
        /* Do nothing */
    }   

    return IsBatt_low; 	
}


static BOOLEAN DTC_IS_CAN_BUSS_OFF(void)
{
    static BOOLEAN IsBus_off = FALSE;   
    static UINT8 busoff_debounce_cnt = 0; 
    static UINT8 normal_debounce_cnt = 0; 
    static UINT8 cnt_1S = 0;

    cnt_1S++;

    if(cnt_1S >= 20) /* 50ms * 20 = 1S */
    {
        cnt_1S = 0;
        if(is_CAN_BusOff() != FALSE)
        {
            busoff_debounce_cnt++;
            normal_debounce_cnt = 0;

            if(busoff_debounce_cnt >= 5)
            {
                IsBus_off = TRUE;
                busoff_debounce_cnt = 0;
            }
        }
        else
        {
            busoff_debounce_cnt = 0;
            normal_debounce_cnt++;

            if(normal_debounce_cnt >= 5)
            {
                IsBus_off = FALSE;
                normal_debounce_cnt = 0;
            }
        }
    }

   return IsBus_off;
}


static BOOLEAN DTC_IS_EASY_IN_SW_STUCK(void)
{
    static UINT8 ng_debounce_cnt = 0; 
    static UINT8 ok_debounce_cnt = 0; 
    BOOLEAN EasyIn_SW_pushd = FALSE;
    BOOLEAN IsEasyInSW_stuck = FALSE;

    EasyIn_SW_pushd = ServSWM_u_GetKey_Seat_FastF_SW_RealVaule();

    if(EasyIn_SW_pushd != FALSE)
    {
        ng_debounce_cnt++;
        ok_debounce_cnt = 0;

        if(ng_debounce_cnt >= 200) /* 50ms * 200 = 10S */
        {
            IsEasyInSW_stuck = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else
    {
        ok_debounce_cnt++;
        ng_debounce_cnt = 0;

        if(ok_debounce_cnt >= 40) /* 50ms * 40 = 2S */
        {
            IsEasyInSW_stuck = FALSE;
            ok_debounce_cnt = 0;
        }
    }

    return IsEasyInSW_stuck;
}


static BOOLEAN DTC_IS_EASY_OUT_SW_STUCK(void)
{
    static UINT8 ng_debounce_cnt = 0; 
    static UINT8 ok_debounce_cnt = 0; 
    BOOLEAN EasyOut_SW_pushd = FALSE;
    BOOLEAN IsEasyOutSW_stuck = FALSE;

    EasyOut_SW_pushd = ServSWM_u_GetKey_Seat_FastB_SW_RealVaule();

    if(EasyOut_SW_pushd != FALSE)
    {
        ng_debounce_cnt++;
        ok_debounce_cnt = 0;

        if(ng_debounce_cnt >= 200) /* 50ms * 200 = 10S */
        {
            IsEasyOutSW_stuck = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else
    {
        ok_debounce_cnt++;
        ng_debounce_cnt = 0;

        if(ok_debounce_cnt >= 40) /* 50ms * 40 = 2S */
        {
            IsEasyOutSW_stuck = FALSE;
            ok_debounce_cnt = 0;
        }
    }

    return IsEasyOutSW_stuck;
}


static BOOLEAN DTC_IS_SEAT_FORWARD_SW_STUCK(void)
{
    static UINT16 ng_debounce_cnt = 0; 
    static UINT8 ok_debounce_cnt = 0; 
    BOOLEAN SeatForward_SW_pushd = FALSE;
    BOOLEAN IsSeatForwardSW_stuck = FALSE;

    SeatForward_SW_pushd = ServSWM_u_GetKey_Seat_F_SW_RealVaule();

    if(SeatForward_SW_pushd != FALSE)
    {
        ng_debounce_cnt++;
        ok_debounce_cnt = 0;

        if(ng_debounce_cnt >= 600) /* 50ms * 600 = 30S */
        {
            IsSeatForwardSW_stuck = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else
    {
        ok_debounce_cnt++;
        ng_debounce_cnt = 0;

        if(ok_debounce_cnt >= 40) /* 50ms * 40 = 2S */
        {
            IsSeatForwardSW_stuck = FALSE;
            ok_debounce_cnt = 0;
        }
    }

    return IsSeatForwardSW_stuck;
}


static BOOLEAN DTC_IS_SEAT_BACKWARD_SW_STUCK(void)
{
    static UINT16 ng_debounce_cnt = 0; 
    static UINT8 ok_debounce_cnt = 0; 
    BOOLEAN SeatBackward_SW_pushd = FALSE;
    BOOLEAN IsSeatBackwardSW_stuck = FALSE;

    SeatBackward_SW_pushd = ServSWM_u_GetKey_Seat_B_SW_RealVaule();

    if(SeatBackward_SW_pushd != FALSE)
    {
        ng_debounce_cnt++;
        ok_debounce_cnt = 0;

        if(ng_debounce_cnt >= 600) /* 50ms * 600 = 30S */
        {
            IsSeatBackwardSW_stuck = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else
    {
        ok_debounce_cnt++;
        ng_debounce_cnt = 0;

        if(ok_debounce_cnt >= 40) /* 50ms * 40 = 2S */
        {
            IsSeatBackwardSW_stuck = FALSE;
            ok_debounce_cnt = 0;
        }
    }

    return IsSeatBackwardSW_stuck;
}


static BOOLEAN DTC_IS_BACK_FORWARD_SW_STUCK(void)
{
    static UINT16 ng_debounce_cnt = 0; 
    static UINT8 ok_debounce_cnt = 0; 
    BOOLEAN BackForward_SW_pushd = FALSE;
    BOOLEAN IsBackForwardSW_stuck = FALSE;

    BackForward_SW_pushd = ServSWM_u_GetKey_Seatback_F_SW_RealVaule();

    if(BackForward_SW_pushd != FALSE)
    {
        ng_debounce_cnt++;
        ok_debounce_cnt = 0;

        if(ng_debounce_cnt >= 600) /* 50ms * 600 = 30S */
        {
            IsBackForwardSW_stuck = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else
    {
        ok_debounce_cnt++;
        ng_debounce_cnt = 0;

        if(ok_debounce_cnt >= 40) /* 50ms * 40 = 2S */
        {
            IsBackForwardSW_stuck = FALSE;
            ok_debounce_cnt = 0;
        }
    }

    return IsBackForwardSW_stuck;
}


static BOOLEAN DTC_IS_BACK_BACKWARD_SW_STUCK(void)
{
    static UINT16 ng_debounce_cnt = 0; 
    static UINT8 ok_debounce_cnt = 0; 
    BOOLEAN BackBackward_SW_pushd = FALSE;
    BOOLEAN IsBackBackwardSW_stuck = FALSE;

    BackBackward_SW_pushd = ServSWM_u_GetKey_Seatback_B_SW_RealVaule();

    if(BackBackward_SW_pushd != FALSE)
    {
        ng_debounce_cnt++;
        ok_debounce_cnt = 0;

        if(ng_debounce_cnt >= 600) /* 50ms * 600 = 30S */
        {
            IsBackBackwardSW_stuck = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else
    {
        ok_debounce_cnt++;
        ng_debounce_cnt = 0;

        if(ok_debounce_cnt >= 40) /* 50ms * 40 = 2S */
        {
            IsBackBackwardSW_stuck = FALSE;
            ok_debounce_cnt = 0;
        }
    }

    return IsBackBackwardSW_stuck;
}


static BOOLEAN DTC_IS_SEAT_HALL_SHROTGND(void)
{
    static UINT8 ng_debounce_cnt = 0; 

    CddMtr_Mng_Hall_Status_e SeatHall_Sts = CDDMTR_MNG_HALL_NORMAL;
    BOOLEAN Is_SeatHall_ShortGnd = FALSE;

    SeatHall_Sts = CddMtr_Get_Hall_Fault_Status(HORIZON_MOTOR_ID);

    if(SeatHall_Sts == CDDMTR_MNG_HALL_SHORTGND)
    {
        ng_debounce_cnt++;

        if(ng_debounce_cnt >= 30) /* 50ms * 30 = 1.5S */
        {
            Is_SeatHall_ShortGnd = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else
    {
        Is_SeatHall_ShortGnd = FALSE;
        ng_debounce_cnt = 0;
    }

    return Is_SeatHall_ShortGnd;
}


static BOOLEAN DTC_IS_SEAT_HALL_SHROTBAT(void)
{
    static UINT8 ng_debounce_cnt = 0; 

    CddMtr_Mng_Hall_Status_e SeatHall_Sts = CDDMTR_MNG_HALL_NORMAL;
    BOOLEAN Is_SeatHall_ShortBat = FALSE;

    SeatHall_Sts = CddMtr_Get_Hall_Fault_Status(HORIZON_MOTOR_ID);

    if(SeatHall_Sts == CDDMTR_MNG_HALL_SHORTBAT_OPEN)
    {
        ng_debounce_cnt++;

        if(ng_debounce_cnt >= 30) /* 50ms * 30 = 1.5S */
        {
            Is_SeatHall_ShortBat = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else
    {
        Is_SeatHall_ShortBat = FALSE;
        ng_debounce_cnt = 0;
    }

    return Is_SeatHall_ShortBat;
}


static BOOLEAN DTC_IS_BACK_HALL_SHROTGND(void)
{
    static UINT8 ng_debounce_cnt = 0; 

    CddMtr_Mng_Hall_Status_e BackHall_Sts = CDDMTR_MNG_HALL_NORMAL;
    BOOLEAN Is_BackHall_ShortGnd = FALSE;

    BackHall_Sts = CddMtr_Get_Hall_Fault_Status(LAZYBACK_MOTOR_ID);

    if(BackHall_Sts == CDDMTR_MNG_HALL_SHORTGND)
    {
        ng_debounce_cnt++;

        if(ng_debounce_cnt >= 30) /* 50ms * 30 = 1.5S */
        {
            Is_BackHall_ShortGnd = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else
    {
        Is_BackHall_ShortGnd = FALSE;
        ng_debounce_cnt = 0;
    }

    return Is_BackHall_ShortGnd;
}


static BOOLEAN DTC_IS_BACK_HALL_SHROTBAT(void)
{
    static UINT8 ng_debounce_cnt = 0; 

    CddMtr_Mng_Hall_Status_e BackHall_Sts = CDDMTR_MNG_HALL_NORMAL;
    BOOLEAN Is_BackHall_ShortBat = FALSE;

    BackHall_Sts = CddMtr_Get_Hall_Fault_Status(LAZYBACK_MOTOR_ID);

    if(BackHall_Sts == CDDMTR_MNG_HALL_SHORTBAT_OPEN)
    {
        ng_debounce_cnt++;

        if(ng_debounce_cnt >= 30) /* 50ms * 30 = 1.5S */
        {
            Is_BackHall_ShortBat = TRUE;
            ng_debounce_cnt = 0;
        }
    }
    else
    {
        Is_BackHall_ShortBat = FALSE;
        ng_debounce_cnt = 0;
    }

    return Is_BackHall_ShortBat;
}


#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)

static BOOLEAN DTC_VCU_MSG123_IS_TIMEOUT(void)
{
    if((VNIM_IS_VCU_123_MSG_MISSING() != FALSE) || (VNIM_IS_VCU_123_MSG_NEVER_RECVD() != FALSE))
    {
        return TRUE;        
    }
    else
    {
        return FALSE;
    }
}


static BOOLEAN DTC_BCM_MSG458_IS_TIMEOUT(void)
{
    if((VNIM_IS_BCM_458_MSG_MISSING() != FALSE) || (VNIM_IS_BCM_458_MSG_NEVER_RECVD() != FALSE))
    {
        return TRUE;        
    }
    else
    {
        return FALSE;
    }
}


static BOOLEAN DTC_ABM_MSG234_IS_TIMEOUT(void)
{
    if((VNIM_IS_ABM_234_MSG_MISSING() != FALSE) || (VNIM_IS_ABM_234_MSG_NEVER_RECVD() != FALSE))
    {
        return TRUE;        
    }
    else
    {
        return FALSE;
    }
}

#else

static BOOLEAN DTC_BCM_MSG245_IS_TIMEOUT(void)
{
    if((VNIM_IS_BCM_245_MSG_MISSING() != FALSE) || (VNIM_IS_BCM_245_MSG_NEVER_RECVD() != FALSE))
    {
        return TRUE;        
    }
    else
    {
        return FALSE;
    }
}


static BOOLEAN DTC_BCM_MSG457_IS_TIMEOUT(void)
{
    if((VNIM_IS_BCM_457_MSG_MISSING() != FALSE) || (VNIM_IS_BCM_457_MSG_NEVER_RECVD() != FALSE))
    {
        return TRUE;        
    }
    else
    {
        return FALSE;
    }
}


#endif

/****************************************************************************
Function Name     : DTC_GW320_IS_TIMEOUT

Description       : This function is used for setting the response status for the write operation.

Invocation        : dg_app

Parameters        : None

Return Value      : UINT8

Critical Section  : None

******************************************************************************/
	/*============================================================================
	**
	** Function Name:	   f_diag_Handle_DTC_Logging( void )
	**
	** Visibility:		   Local to fdiag.c
	**
	** Description: 	   Monitors and logs error conditions that should be logged
	**					   through the kernel DTC logging mechanism if present.
	**					   Currently, only two (invalid speed and network error) are
	**					   specified for this program.
	**
	**					   Operation as follows:
	**					   IF ignition is in run AND
	**							low or high voltage conditions not true AND
	**							diag mux on not true
	**
	**						 IF messages missing
	**						   log ConsultII CAN communication failure
	**						 ELSE
	**						   Clear ConsultII CAN communication failure
	**
	**						 IF Speed Invalid
	**						   log ConsultII Speed Invalid failure
	**						 ELSE
	**						   Clear ConsultII Speed Invalid failure
	**
	** Invocation:		   Called by f_diag_KSRRobin.
	**
	** Inputs/Outputs:	   1)lh_volt_pending - I
	**						 Boolean indicates low or high voltage status pending.
	**					   2)lh_volt_shutdown - I
	**						 Boolean value indicates low or high voltage shutdown.
	**					   5)VNIM_IS_VEHSPEED_BCM_INVALID - I
	**						 when invalid speed
	**						 input (0xFFFF) is received from MSG 284 for > 1 second.
	**					   5)Missing Message Status Semaphores - I
	**						 l_comm_dtc_bool.
	**
	** Critical Section:   none
	**
	**==========================================================================*/
static void f_diag_Handle_DTC_Logging( void )
{	
    if(DTC_BAT_VOLTAGE_IS_HIGH() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_BAT_VOL_HIGH,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_BAT_VOL_HIGH,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_BAT_VOLTAGE_IS_LOW() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_BAT_VOL_LOW,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_BAT_VOL_LOW,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_IS_CAN_BUSS_OFF() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_CAN_BUS_OFF,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_CAN_BUS_OFF,DTC_LOG_STAT_NOT_USED);
    }

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT) /* Varint passenger */
    if(DTC_IS_EASY_IN_SW_STUCK() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_EASY_IN_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_EASY_IN_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_IS_EASY_OUT_SW_STUCK() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_EASY_IN_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_EASY_IN_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_VCU_MSG123_IS_TIMEOUT() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_VCU_MESSAGE_LOST,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_VCU_MESSAGE_LOST,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_BCM_MSG458_IS_TIMEOUT() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_BCM_MESSAGE_LOST,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_BCM_MESSAGE_LOST,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_ABM_MSG234_IS_TIMEOUT() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_ABM_MESSAGE_LOST,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_ABM_MESSAGE_LOST,DTC_LOG_STAT_NOT_USED);
    }

#else  /* Varint Driver */

    if((DTC_BCM_MSG245_IS_TIMEOUT() != FALSE) && (DTC_BCM_MSG457_IS_TIMEOUT() != FALSE))
    {
        KernelLogRefineError(DTC_BCM_MESSAGE_LOST,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_BCM_MESSAGE_LOST,DTC_LOG_STAT_NOT_USED);
    }
    
#endif

    if(DTC_IS_SEAT_FORWARD_SW_STUCK() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_SEAT_FORWARD_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_SEAT_FORWARD_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_IS_SEAT_BACKWARD_SW_STUCK() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_SEAT_BACKWARD_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_SEAT_BACKWARD_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }


    if(DTC_IS_BACK_FORWARD_SW_STUCK() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_BACK_FORWARD_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_BACK_FORWARD_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_IS_BACK_BACKWARD_SW_STUCK() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_BACK_BACKWARD_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_BACK_BACKWARD_SW_STUCK,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_IS_SEAT_HALL_SHROTGND() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_SEAT_HALL_SHORT_GND,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_SEAT_HALL_SHORT_GND,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_IS_SEAT_HALL_SHROTBAT() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_SEAT_HALL_SHORT_BAT,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_SEAT_HALL_SHORT_BAT,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_IS_BACK_HALL_SHROTGND() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_BACK_HALL_SHORT_GND,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_BACK_HALL_SHORT_GND,DTC_LOG_STAT_NOT_USED);
    }

    if(DTC_IS_BACK_HALL_SHROTBAT() != FALSE)
    {
        KernelLogRefineError(DTC_ERC_BACK_HALL_SHORT_BAT,DTC_LOG_STAT_NOT_USED);
    }
    else
    {
        KernelClearRefineError(DTC_ERC_BACK_HALL_SHORT_BAT,DTC_LOG_STAT_NOT_USED);
    }
}
	
	
/**********************Routine Control SID 31 ***************************************/
UINT8 fdiag_app_0203_Routine_Control(UINT8 *buffer, UINT8 req_type)
{
	UINT8 rC = FALSE;
	/*static UINT8 sque;*/
	/*UINT8 CtrlOptRrd;*/
	
	return rC;
}

/****************************************************************************
Function Name     : DTC_GW320_IS_TIMEOUT

Description       : This function is used for setting the response status for the write operation.

Invocation        : dg_app

Parameters        : None

Return Value      : UINT8

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_FF00_Routine_Control(UINT8 *buffer, UINT8 req_type)
{
	UINT8 rC = FALSE;

	return rC;
}

/****************************************************************************
Function Name     : DTC_GW320_IS_TIMEOUT

Description       : This function is used for setting the response status for the write operation.

Invocation        : dg_app

Parameters        : None

Return Value      : UINT8

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_0202_Routine_Control(UINT8 *buffer, UINT8 req_type)
{
	UINT8 rC = FALSE;

	return rC;
}


/****************************************************************************
Function Name     : DTC_GW320_IS_TIMEOUT

Description       : This function is used for setting the response status for the write operation.

Invocation        : dg_app

Parameters        : None

Return Value      : UINT8

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_FF01_Routine_Control(UINT8 *buffer, UINT8 req_type)
{
	UINT8 rC = FALSE;

	return rC;
}
/****************************************************************************
Function Name     : DTC_GW320_IS_TIMEOUT

Description       : This function is used for setting the response status for the write operation.

Invocation        : dg_app

Parameters        : None

Return Value      : UINT8

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_F518_Routine_Control(UINT8 *buffer, UINT8 req_type)
{
	UINT8 rC = FALSE;


	return rC;
}


/****************************************************************************
Function Name     : DTC_GW320_IS_TIMEOUT

Description       : This function is used for setting the response status for the write operation.

Invocation        : dg_app

Parameters        : None

Return Value      : UINT8

Critical Section  : None

******************************************************************************/
UINT8 fdiag_app_DF01_Routine_Control(UINT8 *buffer, UINT8 req_type)
{
    UINT8 rC = FALSE;

    return rC;
}


void can_send_test(void)
{
    SINT8 ret_type;

    Canif_Msg_Str fl_str_e;

    fl_str_e.Data[0]=0xFF;
    fl_str_e.Data[1]=0xFF;
    fl_str_e.Data[2]=0xFF;
    fl_str_e.Data[3]=0xFF;
    fl_str_e.Data[4]=0xFF;
    fl_str_e.Data[5]=0xFF;
    fl_str_e.Data[6]=0xFF;
    fl_str_e.Data[7]=0xFF;
    fl_str_e.StdId = 0x45A;
    fl_str_e.DLC=8;

    if(FL_CAN_IsActiveFlag_TXBuffFullSignal(CAN) == FALSE)
    {
        ret_type = Canif_tx_queue_push_e(fl_str_e);
    }
    
}


void response_11_01(void)
{
    UINT8 buff[8] = {0};
    UINT8 i;
    Canif_Msg_Str  fl_str_e;
    UINT32 *cdata_p = (UINT32 *)ECU_RESET_FLAG_ADDR;

    if(*cdata_p == ECU_RESET_REQ)  /* Check ECU reset flag */
    {
        buff[0] = 0x02;
        buff[1] = 0x51;
        buff[2] = 0x01;
        
       fl_str_e.StdId = VTP_DIAG_RESPONSE_ID;
       fl_str_e.DLC = 8;

        for(i=0; i<fl_str_e.DLC; i++)
        {
            fl_str_e.Data[i] = buff[i];        
        }

        *cdata_p = 0x0;  /* Clear ECU reset flag */
        FL_CAN_Frame_Send(FL_CAN_FORMAT_STANDARD_DATA,&fl_str_e);
    }
}


BOOLEAN is_CAN_BusOff(void)
{
    if(READ_BIT(CAN->SR, CAN_SR_ESTAT_Msk) == 0x10) /* ESTAT = 0x10 means bus off */
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


#endif /*End of FDIAG_C*/
/*---------------------------------------------------------------------------
Release Label     : 
Date              : 19-September-2011
By                : lhu4
Traceability      : Initial version for E301 diagnoctics module
Change Description: Initial version for E301 diagnoctics module
-----------------------------------------------------------------------------*/
