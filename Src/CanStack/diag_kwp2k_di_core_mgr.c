#define DIAG_KWP2K_DI_CORE_MGR_C

#define KERNEL_RESTRICTED_DIAG

/*
** Include sched.h before defining KERNEL_PRIVATE_DIAG so that sched.cfg,
** included by sched.h, only makes the KERNEL_RESTRICTED_DIAG areas visible.
*/
#define KERNEL_PRIVATE_DIAG

/***************************************************************************
                I N C L U D E   F I L E S
***************************************************************************/
#include "diag_kwp2k_di_core_mgr.cfg"
#include "diag_kwp2k_di_core_mgr.h"
#include "diag_kwp2k_di_core_erlg.h"
#include "Il.h"
#include "fdiag.h"
//#include "nvmdef.h" 
#include "tp_par.h"
#include "vnim_net.h"
#include "nw_can_dll.h"

/*#include "diag_kwp2k_di_core_nvm.h"*/
/***************************************************************************
   I N T E R N A L   M A C R O   A N D   T Y P E   D E F I N I T I O N S
***************************************************************************/

/*
** ECU RESET TYPE
*/
#define  ECU_RESET_NONE                       ( 0x00u)
#define  ECU_RESET_HARD                       ( 0x01u)
#define  ECU_RESET_KEYOFFON                   ( 0x02u)
#define  ECU_RESET_SOFT                       ( 0x03u)
#define  ECU_RESET_ENBL_SHUTDOWN              ( 0x04u)
#define  ECU_RESET_DSBL_SHUTDOWN              ( 0x05u)

#define ECU_RESET_HARD_SUPPRESSION_RESP       (0x81u)
#define ECU_RESET_KEYOFFON_SUPPRESSION_RESP   (0x82u)
#define ECU_RESET_SOFT_SUPPRESSION_RESP       (0x83u)

#define ECU_SUPPRESSION_BIT                   ((DG_MGR_UINT8) 0x80u)                    
/*
** Bit map definitions to be allocated in l_diag_extra_flags_U8;
*/
#define ECU_RESET_IN_PRGS                     ((DG_MGR_UINT8) 0x01u)
#define ECU_READY_TO_RESET                    ((DG_MGR_UINT8) 0x02u)
#define CLEAR_FAULT_RESP_REQD                 ((DG_MGR_UINT8) 0x04u)
#define DIAG_ENTRY_TO_PROG_SESSION            ((DG_MGR_UINT8) 0x08u)
#define DIAG_ENTRY_TO_PS_IN_PRGRS             ((DG_MGR_UINT8) 0x10u)

/*
** Bit definitions for "uds_status" data store.
** All the bits will be initialized to "0".
*/
#define   IOCTL_REQ_RCVD   ((DG_MGR_UINT8) 0x02u) /* =1 when IOCTL request received this
                                                session (mode 0x2F) */
#define   CC_REQ_RCVD      ((DG_MGR_UINT8) 0x04u) /* =1 when ComCTL request received this
                                                session (mode 0x28) */
#define   DC_REQ_RCVD      ((DG_MGR_UINT8) 0x08u) /* =1 when DiagCMD request received this
                                                session (mode 0xB1)*/
#define   ST_RESP_PENDING  ((DG_MGR_UINT8) 0x20u)

#define   ST_REQ_PENDING   ((DG_MGR_UINT8) 0x40u) /* =1 when positive response to request
                                                self test result is pending */
#define   SELF_TEST_ABORT  ((DG_MGR_UINT8) 0x80u) /* =1 when self test is aborted
                                                (mode 0x31-0x02)*/

/* Read DTC options - Service $19 */
#define REPORT_NUM_DTC_BY_SM                       ( 0x01u)
#define REPORT_DTC_BY_SM                           ( 0x02u)
#define REPORT_DTC_SNAPSHOT_IDENT                  ( 0x03u)
#define DTC_SNAPSHOT_REC_BY_DTC_NUM                ( 0x04u)
#define DTC_SNAPSHOT_REC_BY_REC_NUM                ( 0x05u)
#define REPORT_DTC_EXT_DATA_RECORD_SM              ( 0x06u)
#define REPORT_SUPPORTED_DTC                       ( 0x0Au)

#define REPORT_NUM_MIRROR_MEM_DTC_SM               ( 0x11u)
#define REPORT_MIRROR_MEM_DTC_SM                   ( 0x0Fu)
#define REPORT_MIRROR_MEM_DTC_EXT_DATA_RECORD_SM   ( 0x10u)
#define REPORT_MOST_RECENT_CONFIRMED_DTC           ( 0x0Eu)
#define REPORT_FAULT_DETECTION_CTR                 ( 0x14u)

#ifdef MODE_19_SUBFUNC_02_SUPPORTED
#define DTC_SUPPORTED_MASK                         PROD_SPEC_AVAILABLE_DTC_MASK
#endif

#ifdef MODE_19_SUBFUNC_10_SUPPORTED
#define EXT_DATA_MONITG_CYC_2           0x81
#define EXT_DATA_MONITG_CYC_ALL         0xFF
#endif

/*ROUTINE CONTROL*/
#define START_ROUTINE_CONTROL                      ((DG_MGR_UINT8) 0x01u)
#define STOP_ROUTINE_CONTROL                       ((DG_MGR_UINT8) 0x02u)
#define REQ_RESULT_ROUTINE_CONTROL                 ((DG_MGR_UINT8) 0x03u)
#define RC_REQUESTED                               ((DG_MGR_UINT8) 0x01u)


/*Control DTC Setting */
#define ENABLE_DTC_LOGGING                         ((DG_MGR_UINT8) 0x01u)
#define DISABLE_DTC_LOGGING                        ((DG_MGR_UINT8) 0x02u)
#define ENABLE_DTC_LOGGING_NO_REP                  ((DG_MGR_UINT8) 0x81u)
#define DISABLE_DTC_LOGGING_NO_REP                 ((DG_MGR_UINT8) 0x82u)

#define HIGH_BYTE_DTC_MASK                         ((DG_MGR_UINT32)0x00FF0000ul)
#define MIDDLE_BYTE_DTC_MASK                       ((DG_MGR_UINT32)0x0000FF00u)
#define LOW_BYTE_DTC_MASK                          ((DG_MGR_UINT32)0x000000FFu)

#define HIGH_BYTE_16BIT_MASK                       ((DG_MGR_UINT16)0xFF00u)
#define LOW_BYTE_16BIT_MASK                        ((DG_MGR_UINT16)0x00FFu)

#define SHIFT_8_BITES                              ((DG_MGR_UINT8)0x08u)
#define SHIFT_16_BITES                             ((DG_MGR_UINT8)0x10u)

#define ALL_DTC_SUP                                ((DG_MGR_UINT8)0xFFu)

#ifndef ZERO
#define ZERO                                       ((DG_MGR_UINT8)0)
#endif

#define DATA_LEN_1_BYTE                            ((DG_MGR_UINT8)0x01u)
#define DATA_LEN_2_BYTE                            ((DG_MGR_UINT8)0x02u)
#define DATA_LEN_3_BYTE                            ((DG_MGR_UINT8)0x03u)
#define DATA_LEN_4_BYTE                            ((DG_MGR_UINT8)0x04u)
#define DATA_LEN_5_BYTE                            ((DG_MGR_UINT8)0x05u)
#define DATA_LEN_6_BYTE                            ((DG_MGR_UINT8)0x06u)
#define DATA_LEN_7_BYTE                            ((DG_MGR_UINT8)0x07u)

/*
** Access constants.
*/
#define ANY_DIAG           ((DG_MGR_UINT8) 0x00)
#define DM_ONLY            ((DG_MGR_UINT8) 0x01)


#ifdef MODE_10_SUBFUNC_02_SUPPORTED
#define PROG_SESS_RESP_TIMEOUT                     ((DG_MGR_UINT8)0x01u)
#endif

#define IOCTL_IOCP 								((DG_MGR_UINT8)0x02)
#define IOCTL_DATA                              ((DG_MGR_UINT8)0x03)

#define LID_IOCTL_IOCP 								((DG_MGR_UINT8)0x01)
#define LID_IOCTL_DATA                              ((DG_MGR_UINT8)0x02)

#define    NULL_PTR        ((void *) 0)

#if (MODE_85_SUPPORTED_BASED_ON == APPL_SPECIFIC)
#define DIAG_NEGATIVE_RESPONSE			((DG_MGR_UINT8)0x00)
#define DIAG_POSITIVE_RESPONSE			((DG_MGR_UINT8)0x01)	

#endif
/*
__10_SESSION_STRUCT(){}
*/
#if defined( MODE_10_SUPPORTED ) 
typedef struct
{
    DG_MGR_UINT8 session;
     /* The 8-bit session value */
    DG_MGR_UINT8 session_index;
     /* The 8-bit session index */
    DG_MGR_UINT8 (*deny_entry_lst_fptr)(void);
     /* Pointer to function which provides entry denial list. */
    void (*prolog_lst_fptr)(void);
     /* Pointer to function which provides prolog list. */
    void (*epilog_lst_fptr)(void);
     /* Pointer to function which provides epilog list. */
    DG_MGR_UINT8 (*abort_lst_fptr)(void);
     /* Pointer to function which provides abort list. */
#ifdef 	PROD_SPEC_FSM_CONFIGURE
    DG_MGR_UINT8 fsm_mode;
#endif
}SESSION_RECORD;
#endif
/*
___10_ROM_TABLE(){}
*/
#if defined( MODE_10_SUPPORTED ) || defined( MODE_20_SUPPORTED )
	#ifdef 	PROD_SPEC_FSM_CONFIGURE
	static const SESSION_RECORD diag_session_table_rom[] =
	{
	    {kwp2k_diag_session_default, DG_DEFAULT, NULL_PTR, fdiag_diag_go_default_seasion, NULL_PTR, NULL_PTR, 0x00}, \
	    PROD_SPEC_DIAG_SESSION_TABLE

	    {0x00, NO_SESSIONCHECK, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, 0x00}
	};
	#else
	static const SESSION_RECORD diag_session_table_rom[] =
	{
	    {kwp2k_diag_session_default, NO_SESSIONCHECK, NULL_PTR, NULL_PTR, fdiag_diag_go_default_seasion, NULL_PTR}, \
	    PROD_SPEC_DIAG_SESSION_TABLE

	    {0x00, NO_SESSIONCHECK, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR}
	};
	#endif
#endif

/*
__1A_21_3B_LID_STRUCT(){}
*/
#if defined( MODE_21_SUPPORTED ) || defined( MODE_3B_SUPPORTED ) || defined( MODE_1A_SUPPORTED )
typedef struct
{
    DG_MGR_UINT8 pid_number;
     /* The 8-bit PID number */

    void *pid_data_ptr;
	/* Pointer to memory buffer where pid data resides...ONLY USED IF
	pid_length IS NON-ZERO. */
    DG_MGR_UINT8 pid_length;
         /* Length of memory buffer pid_pointer points to OR 0 if none. */
    DG_MGR_UINT8 (*pid_function_ptr)(DG_MGR_UINT8 *buffer, DG_MGR_UINT8 datalen);
         /* Pointer to function which provides pid data...ONLY CALLED IF
        pid_length IS ZERO. */
	RESPONSE_HANDLER response_data_handler;
	DG_MGR_UINT8 sup_session;
		 /*This is used to configure supported session
		 for this perticular LID */
}LID_RECORD;
#endif
/*
___1A_ROM_TABLE(){}
*/
#ifdef MODE_1A_SUPPORTED
static const LID_RECORD diag_ecu_identification_table_rom[] =
{
    PROD_SPEC_DIAG_ECU_ID_TABLE
    {0, (void *)0, 0,NULL_PTR, 0, 0 }
};
#endif

/*
___21_ROM_TABLE(){}
*/
#ifdef MODE_21_SUPPORTED
static const LID_RECORD diag_lid_table_rom[] =
{
    PROD_SPEC_DIAG_LID_TABLE
    {0, (void *)0, 0, NULL_PTR, 0, 0 }
};
#endif

/*
___3B_ROM_TABLE(){}
*/
#ifdef MODE_3B_SUPPORTED
static const LID_RECORD diag_write_lid_table_rom[] =
{
    PROD_SPEC_DIAG_WRITE_LID_TABLE
    {0, (void *) 0, 0,NULL_PTR, 0, 0 }
};
#endif


/*
__22_2E_LID_STRUCT(){}
*/
#if defined( MODE_22_SUPPORTED ) || defined( MODE_2E_SUPPORTED ) 
typedef struct
{
    DG_MGR_UINT16 pid_number;
     /* The 16-bit PID number */

    void *pid_data_ptr;
         /* Pointer to memory buffer where pid data resides...ONLY USED IF
            pid_length IS NON-ZERO. */
    DG_MGR_UINT8 pid_length;
         /* Length of memory buffer pid_pointer points to OR 0 if none. */
    DG_MGR_UINT8 (*pid_function_ptr)(DG_MGR_UINT8 *buffer);
         /* Pointer to function which provides pid data...ONLY CALLED IF
        pid_length IS ZERO. */
	DG_MGR_UINT8 sup_session;
		 /*This is used to configure supported session
		 for this perticular LID */
}PID_RECORD;
#endif
/*
___22_ROM_TABLE(){}
*/
#ifdef MODE_22_SUPPORTED
static const PID_RECORD diag_pid_table_rom[] =
{
    PROD_SPEC_DIAG_PID_TABLE
    {0, (void *) 0, 0,NULL_PTR, 0 }
};
#endif

/*
___2E_ROM_TABLE(){}
*/
#ifdef MODE_2E_SUPPORTED
static const PID_RECORD diag_write_pid_table_rom[] =
{
    PROD_SPEC_DIAG_WRITE_PID_TABLE
    {0, (void *) 0, 0,NULL_PTR, 0 }
};
#endif

/*
__27_LID_STRUCT(){}
*/

#ifdef MODE_27_SUPPORTED

typedef struct {
    DG_MGR_UINT8               	seed_value;
    DG_MGR_UINT8               	key_value;
    DG_MGR_UINT8               	seed_length;
	DG_MGR_UINT8                key_length;
    DG_MGR_UINT8                (*get_seed_function_ptr)(DG_MGR_UINT8 *buffer);
    DG_MGR_UINT8                (*verify_key_function_ptr)(DG_MGR_UINT8 *buffer);
} SECURITY_ACCESS_STRUCT;

static const SECURITY_ACCESS_STRUCT security_access_list[] =  
{                                         
    PROD_SPEC_DIAG_27_LIST
    {0, 0, 0, 0, NULL_PTR, NULL_PTR } 
};

/*
** used to maintain the current security status. 
*/
static DG_MGR_UINT8 ser_27_seed_flag = 0x00;
static DG_MGR_UINT8 l_security_status_U8 = TRUE;
static DG_MGR_UINT8 l_security_unlock_error_counter_U8 = 0;
static DG_MGR_UINT8 l_security_next_seed_timer_U8 = 0;
static DG_MGR_UINT8 l_security_key_counter_U8=0;

static DG_MGR_UINT8 l_requested_seed_U8[4] = {0x00,0x00,0x00,0x00};
static DG_MGR_UINT8 l_etm_dtc_number_U8;
static DG_MGR_UINT8 Enable_dtc_logging = TRUE;

static void kwp2k_core_Diag_Send_key_Request(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen);
static void kwp2k_core_Diag_Seed_Request(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen);
#endif

/*
__2F_LID_STRUCT(){}
*/
#ifdef MODE_2F_SUPPORTED
/* 
** Structure for incorporating product specific I/O control by Commmand ID requests
** (mode 0x2F).
** com_id           = identifies parameter for which I/O action is requested 
** action_mask      = specifies which actions are supported for the id 
** data_length      = length of parameter data
** data_ptr         = pointer to parameter data
** function_handler = function called to write parameter data
*/
typedef struct 
{
    DG_MGR_UINT16 pid;
    DG_MGR_UINT8 action_mask;
    DG_MGR_UINT8 data_length;
    DG_MGR_UINT8 *data_ptr;
    DG_MGR_UINT8 (*function_handler)(DG_MGR_UINT8 control_option, DG_MGR_UINT8 buffer_length, DG_MGR_UINT8 *buffer);
         /* Pointer to function to process the message.  The requested control 
         ** option, the buffer length, and a pointer to the buffer are passed to 
         ** the function. 
         */
    void (*recover_function)(void);
	DG_MGR_UINT8 sup_session;
		 /*This is used to configure supported session
		 for this perticular LID */
} IOCTL_RECORD;

/*
** Bit-mapped status bits.
*/
static NEAR DG_MGR_UINT8     ioctrl_diag_status;

#endif
/*
___2F_ROM_TABLE(){}
*/
#ifdef MODE_2F_SUPPORTED
/* The "I/O Control by I/O command Identifer Table".  This table contains records which
** reference I/O signals or internal ECU functions and allows the tester to 
** override or change associated parameters.   The tester accesses records in this
** table via the mode 0x2F message by specifying a "input/output command identifier"
** which is then used to search for the matching record in the table.
*/
static const IOCTL_RECORD ioctl_diag_table_rom[] = 
{
    PROD_SPEC_DIAG_IOCTL_BY_ID_TABLE
    {0, 0, 0, NULL_PTR, NULL_PTR, NULL_PTR, 0} 
};

/* 
** Number of IOCTL PIDs:
** (sizeof(ioctl_table_rom)/sizeof(IOCTL_RECORD) - 1)
*/
static  DG_MGR_UINT8 diag_ioctl_state_array[((sizeof(ioctl_diag_table_rom)/sizeof(IOCTL_RECORD)-1u) \
                                    /(DG_MGR_UINT8)8) + (DG_MGR_UINT8)1 ];

#define set_diag_ioctl_in_progress(index)                           \
    ( diag_ioctl_state_array[ (index)/ (DG_MGR_UINT8) 8 ] |=     \
           ((DG_MGR_UINT8) 1 << ((DG_MGR_UINT8)((index) & (DG_MGR_UINT8) 7))) )

#define clear_diag_ioctl_in_progress(index)                           \
    ( diag_ioctl_state_array[ (index)/ (DG_MGR_UINT8) 8 ] &=     \
           (~((DG_MGR_UINT8) 1 << ((DG_MGR_UINT8)((index) & (DG_MGR_UINT8) 7)))) )

#define is_diag_ioctl_in_progress(index)                           \
    ( diag_ioctl_state_array[ (index)/ (DG_MGR_UINT8) 8 ] &     \
           ((DG_MGR_UINT8) 1 << ((DG_MGR_UINT8)((index) & (DG_MGR_UINT8) 7))) )

#endif

/*
__30_LID_STRUCT(){}
*/
#ifdef MODE_30_SUPPORTED
/* 
** Structure for incorporating product specific I/O control by Local ID requests
** (mode 0x30).
** Local_id         = identifies parameter for which I/O action is requested 
** action_mask      = specifies which actions are supported for the id 
** data_length      = length of parameter data
** data_ptr         = pointer to parameter data
** function_handler = function called to write parameter data
*/
typedef struct 
{
    DG_MGR_UINT8 lid;
    DG_MGR_UINT8 action_mask;
    DG_MGR_UINT8 data_length;
    DG_MGR_UINT8 *data_ptr;
    DG_MGR_UINT8 (*function_handler)(DG_MGR_UINT8 control_option, DG_MGR_UINT8 buffer_length, DG_MGR_UINT8 *buffer);
         /* Pointer to function to process the message.  The requested control 
         ** option, the buffer length, and a pointer to the buffer are passed to 
         ** the function. 
         */
    void (*recover_function)(void);
	DG_MGR_UINT8 sup_session;
		 /*This is used to configure supported session
		 for this perticular LID */
} LID_IOCTL_RECORD;

/*
** Bit-mapped status bits.
*/
static NEAR DG_MGR_UINT8     lid_ioctrl_diag_status;

#endif
/*
___30_ROM_TABLE(){}
*/

#ifdef MODE_30_SUPPORTED
#ifndef LID_TABLE_APPLICATION_MANAGED
/* The "I/O Control by I/O Local Identifer Table".  This table contains records which
** reference I/O signals or internal ECU functions and allows the tester to 
** override or change associated parameters.   The tester accesses records in this
** table via the mode 0x2F message by specifying a "input/output command identifier"
** which is then used to search for the matching record in the table.
*/
static const LID_IOCTL_RECORD lid_ioctl_diag_table_rom[] = 
{
    PROD_SPEC_DIAG_IOCTL_BY_LID_TABLE
    {0, 0, 0, NULL_PTR, NULL_PTR, NULL_PTR, 0} 
};


/* 
** Number of IOCTL PIDs:
** (sizeof(ioctl_table_rom)/sizeof(IOCTL_RECORD) - 1)
*/
static  DG_MGR_UINT8 diag_lid_ioctl_state_array[((sizeof(lid_ioctl_diag_table_rom)/sizeof(LID_IOCTL_RECORD)-1u) \
                                    /(DG_MGR_UINT8)8) + (DG_MGR_UINT8)1 ];

#define set_diag_lid_ioctl_in_progress(index)                           \
    ( diag_lid_ioctl_state_array[ (index)/ (DG_MGR_UINT8) 8 ] |=     \
           ((DG_MGR_UINT8) 1 << ((DG_MGR_UINT8)((index) & (DG_MGR_UINT8) 7))) )

#define clear_diag_lid_ioctl_in_progress(index)                           \
    ( diag_lid_ioctl_state_array[ (index)/ (DG_MGR_UINT8) 8 ] &=     \
           (~((DG_MGR_UINT8) 1 << ((DG_MGR_UINT8)((index) & (DG_MGR_UINT8) 7)))) )

#define is_diag_lid_ioctl_in_progress(index)                           \
    ( diag_lid_ioctl_state_array[ (index)/ (DG_MGR_UINT8) 8 ] &     \
           ((DG_MGR_UINT8) 1 << ((DG_MGR_UINT8)((index) & (DG_MGR_UINT8) 7))) )
#endif
#endif

/*
__31_LID_STRUCT(){}
*/
#ifdef MODE_31_SUPPORTED
typedef struct
{
    DG_MGR_UINT16 rc_ID_U16;
    DG_MGR_UINT8  rc_dataLen_U8;
    DG_MGR_UINT8  rc_type_U8;
    DG_MGR_UINT8  rc_sync_async;
    DG_MGR_UINT8  (*rc_function_ptr)(DG_MGR_UINT8 *buffer, DG_MGR_UINT8 req_type);    
	DG_MGR_UINT8 sup_session;
		 /*This is used to configure supported session
		 for this perticular LID */
}RC_RECORD;
#endif
/*
___31_ROM_TABLE(){}
*/
#ifdef MODE_31_SUPPORTED
static const RC_RECORD diag_routine_control_table_rom[] =
{
    PROD_SPEC_DIAG_ROUTINE_CONTROL \
    {0, 0, 0, 0, NULL_PTR, 0} 
};

#endif

/*
___23_3D_ROM_TABLE(){}
*/

#if defined( MODE_23_SUPPORTED ) || defined( MODE_3D_SUPPORTED )
/*
** Structure for incorporating product specific DMR protect ranges.  Each range
** specified must have a beginning and ending address.
*/
typedef struct
{
    void *starting_address;
    void *ending_address;
} DMR_RANGES_RECORD;

/* The "DMR Ranges List" list.  This ROM list contains records which
** specify the DMR protect ranges on the product.  This list must be
** terminated with a {0,0} entry, and an empty list is acceptable.
*/
#ifdef MODE_23_SUPPORTED
static const DMR_RANGES_RECORD diag_dmr_protect_ranges_rom[]  =
{
    PROD_SPEC_DIAG_DMR_PROTECT_RANGES
    {0, 0}
};
#endif /* MODE_23_SUPPORTED */

#ifdef MODE_3D_SUPPORTED
/* The "DMW Ranges List" list.  This ROM list contains records which
** specify the DMW protect ranges on the product.  This list must be
** terminated with a {0,0} entry, and an empty list is acceptable.
*/
static const DMR_RANGES_RECORD diag_dmw_protect_ranges_rom[]  =
{
    PROD_SPEC_DIAG_DMW_PROTECT_RANGES
    {0, 0}
};
#endif /* MODE_3D_SUPPORTED */

/*
** Maximum Data length for ReadMemoryByAddress($23) Service
*/
#define MAX_LENGTH_SERVICE_23_2   ((UINT8) 0x07)
#define MAX_LENGTH_SERVICE_23_1   ((UINT8) 0x06)

#define LENGTH_FORMAT_IDENTIFIER_1   ((UINT8) 0x14)
#define LENGTH_FORMAT_IDENTIFIER_2   ((UINT8) 0x24)

#endif

/*
___85_ROM_TABLE(){}
*/
/* The "Deny DTC Control Entry" list.  This ROM list contains a function
** pointer for each function which can deny entry into NO DTC Log state.
** Each function in this list returns TRUE to deny entry into NO DTC Log
** state, FALSE otherwise.  Each element of this list is a function pointer.
** This list must be terminated with NULL_PTR, and an empty list (only NULL_PTR)
** is acceptable.
*/
#ifdef MODE_85_SUPPORTED
static DG_MGR_BOOLEAN (*const dtctl_deny_entry_list_rom[])(void)  =
{
    PROD_SPEC_DENY_DTCTL_ENTRY_LIST
    NULL_PTR
};
#endif

static NEAR DG_MGR_UINT8 cur_session;
static NEAR DG_MGR_UINT8 prev_session;
static NEAR DG_MGR_UINT8 requested_session;

#if (MODE_85_SUPPORTED_BASED_ON == APPL_SPECIFIC)
/* service 85 */
static DG_MGR_UINT8	ctrl_state = 0x00;
static DG_MGR_UINT8 ctrl_dtc_resp_len = 0x00;
#endif

/***************************************************************************
   I N T E R N A L   F U N C T I O N   P R O T O T Y P E S
***************************************************************************/

/*
** Initializes the flags
*/
static void kwp2k_core_init_diagnostics(void);

/*
** To traverse and call functions
*/
static DG_MGR_UINT8 kwp2k_core_diag_trav_bool_void_lst(DG_MGR_BOOLEAN (*const (*list_ptr))(void));

/*
** To traverse and call functions
*/
void kwp2k_core_diag_trav_void_void_lst(void (*const (*top))(void));

/*
** Starts the EcuReset processing.
*/
static void kwp2k_core_start_ecu_reset_proc(DG_MGR_UINT8 *p_msg_S, DG_MGR_UINT8 p_ecu_reset_U8);

/*
** To handle ECU reset operation by calling appropriate functions
*/
static void kwp2k_core_ecu_reset_manager(void);

/*
** Recovers the conditions to normalcy when a timing parameter timeouts
*/
static void kwp2k_core_on_timeout_recovery(void);

static DG_MGR_UINT8 kwp2k_core_check_session(DG_MGR_UINT8 session);

static void kwp2k_core_Diag_process_pid_request( DG_MGR_UINT8* ,const DG_MGR_UINT16 );
static void kwp2k_core_Diag_process_write_pid_request( DG_MGR_UINT8* ,DG_MGR_UINT8 );

static void kwp2k_core_Diag_process_lid_request( DG_MGR_UINT8* ,const DG_MGR_UINT8 , void* ,  DG_MGR_UINT8 , DG_MGR_UINT8 );
static void kwp2k_core_Diag_process_write_lid_request( DG_MGR_UINT8* ,DG_MGR_UINT8 , void* );

#if defined(MODE_14_KWP_SUPPORTED) || defined(MODE_14_UDS_SUPPORTED)
static DG_MGR_UINT32 grp_info_14 = (DG_MGR_UINT32)(0x00);
static DG_MGR_BOOLEAN Valid_Group_Info( DG_MGR_UINT32 grp_info);
static void kwp2k_core_Diag_ClearDTCs(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen);
static DG_MGR_UINT8 *cleardtc_ptr = NULL_PTR;
#endif

#ifdef MODE_17_SUPPORTED
static DG_MGR_BOOLEAN Valid_Group_Info_ser_17( DG_MGR_UINT32 grp_info);
static void kwp2k_core_Diag_17_read_dtc(DG_MGR_UINT8* pMsgContext);
#endif

#ifdef MODE_18_SUPPORTED
static DG_MGR_BOOLEAN Valid_Group_Info_ser_18( DG_MGR_UINT32 grp_info);
static void kwp2k_core_Diag_18_read_dtc(DG_MGR_UINT8* pMsgContext);

#if defined(MODE_18_SUBFUNC_00_SUPPORTED) || defined(MODE_18_SUBFUNC_02_SUPPORTED)
static void kwp2k_core_Diag_18_00_02_read_dtc(DG_MGR_UINT8* pMsgContext);
#endif

#if defined(MODE_18_SUBFUNC_01_SUPPORTED) || defined(MODE_18_SUBFUNC_03_SUPPORTED)
static void kwp2k_core_Diag_18_01_03_read_dtc(DG_MGR_UINT8* pMsgContext);
#endif

/* Read DTC option - Service $18 */
#define REPORT_IDENTIFIED_BCD_DTC	0x00
#define REPORT_SUPPORTED_BCD_DTC	0x01
#define REPORT_IDENTIFIED_HEX_DTC	0x02
#define REPORT_SUPPORTED_HEX_DTC	0x03
#endif /* MODE_18_SUPPORTED */

#ifdef MODE_2F_SUPPORTED
/*static void kwp2k_core_Diag_process_IOControl_pid_request( DG_MGR_UINT8* ,DG_MGR_UINT8 , void* );commented for QAC*/
#endif

#ifdef MODE_19_SUPPORTED

#ifdef MODE_19_SUBFUNC_01_SUPPORTED
static void kwp2k_core_read_DTC_count_by_status_mask(DG_MGR_UINT8 * pMsgContext);
#define DTC_FORMAT_ID   PROD_SPEC_DTC_FORMAT_ID
#endif /* MODE_19_SUBFUNC_01_SUPPORTED */

#ifdef MODE_19_SUBFUNC_02_SUPPORTED
static void kwp2k_core_read_DTC_by_status_mask(DG_MGR_UINT8 * pMsgContext);
static DG_MGR_UINT8 kwp2k_core_dtc_staus_mask_is_valid(DG_MGR_UINT8 mask);
#endif /* MODE_19_SUBFUNC_02_SUPPORTED */

#ifdef MODE_19_SUBFUNC_03_SUPPORTED
static void kwp2k_core_read_DTC_SnapshotIdentification(DG_MGR_UINT8 * pMsgContext);
#endif /* MODE_19_SUBFUNC_03_SUPPORTED */

#ifdef MODE_19_SUBFUNC_04_SUPPORTED
static void kwp2k_core_read_DTC_SnapshotRecord_by_DTCNumber(DG_MGR_UINT8 * pMsgContext);
#endif /* MODE_19_SUBFUNC_04_SUPPORTED */

#ifdef MODE_19_SUBFUNC_05_SUPPORTED
static void kwp2k_core_read_DTC_SnapshotRecord_by_RecordNumber(DG_MGR_UINT8 * pMsgContext);
#endif /* MODE_19_SUBFUNC_05_SUPPORTED */

#ifdef MODE_19_SUBFUNC_06_SUPPORTED
static void kwp2k_core_read_read_extended_data_by_DTC_id(DG_MGR_UINT8 * pMsgContext);
#endif /* MODE_19_SUBFUNC_06_SUPPORTED */

#ifdef MODE_19_SUBFUNC_0A_SUPPORTED
static void kwp2k_core_read_All_DTC_by_status_mask(DG_MGR_UINT8 * pMsgContext);
#endif/* MODE_19_SUBFUNC_0A_SUPPORTED */

#endif /* MODE_19_SUPPORTED */

#ifdef MODE_31_SUPPORTED
static void kwp2k_core_Diag_process_routine_control(DG_MGR_UINT8* bufptr, DG_MGR_UINT8 dataLen,const DG_MGR_UINT8 req_type);
static DG_MGR_BOOLEAN kwp2k_core_diag_process_routine_control_output(DG_MGR_UINT8* bufptr,RC_RECORD *p_rc_table_ptr,const DG_MGR_UINT8 req_type);
#endif

#ifdef MODE_23_SUPPORTED
#if (MODE_23_SUPPORTED_BASED_ON != APPL_SPECIFIC)
static void kwp2k_core_process_Read_Memory_by_Address(DG_MGR_UINT8 addr_format, DG_MGR_UINT8 len_format, DG_MGR_UINT8 *buffer);
static DG_MGR_BOOLEAN kwp2k_core_DMR_request_OK(DG_MGR_UINT32 p_addr_U32, DG_MGR_UINT16 p_num_bytes_U16);
#endif      /* mode 0x23 */
#endif /* MODE_23_SUPPORTED */

#ifdef MODE_3D_SUPPORTED
static DG_MGR_BOOLEAN kwp2k_core_DMW_request_OK(DG_MGR_UINT32 p_dmw_addr_U32, DG_MGR_UINT16 p_dmw_size_U16);
#endif      /* mode 0x3D */
static void kwp2k_core_3D_callback(DG_MGR_BOOLEAN p_write_success_BOOL);

#ifdef MODE_2F_SUPPORTED
static void app_abort_io_ctrl_diag( void );
#endif
#ifdef MODE_30_SUPPORTED
static void app_abort_io_ctrl_diag( void );
#endif

#ifdef MODE_13_SUPPORTED
static void kwp2k_core_read_DTC_13(DG_MGR_UINT8 * pMsgContext);
#endif


static void kwp2k_core_Diag_Product_Mode_Process(void);

/***************************************************************************

   M E M O R Y   A L L O C A T I O N
***************************************************************************/

/*
** The "Deny ECU RESET" list.  This ROM list contains a function
** pointer for each function which can deny ECU RESET. Each function
** in this list returns TRUE to deny ECU RESET, FALSE otherwise.
** Each element of this list is a function pointer. This list must
** be terminated with NULL, and an empty list (only NULL) is acceptable.
*/
static DG_MGR_BOOLEAN (*const diag_deny_ecu_reset_list_rom[])(void)  =
{
    PROD_SPEC_DENY_ECU_RESET_LIST
    ((DG_MGR_BOOLEAN (*)(void)) 0)
};

/*
**  The Deny Entry to Programming session" list. This ROM list
**  consists of functions that will return TRUE or FALSE to check
**  whether the Programming session can be entered or not. The
**  functions in this list shall return TRUE if programming session
**  should not be entered else should return FALSE. Each element
**  of this list is a function pointer. This list must be terminated
**  with NULL and an empty list is accepted
*/

static DG_MGR_BOOLEAN (*const diag_deny_entry_to_prog_sess_list[])(void) =
{
    PROD_SPEC_DENY_PROG_SESS_LIST
    ((DG_MGR_BOOLEAN (*) (void)) 0)
};

#ifdef MODE_3D_SUPPORTED
/* This variable is used to store the size of data to be written and the 3Byte starting address
   of the NVM to where the data is to be written.
   The Most significant 8 bits store the size and the Least significant 24 bits store the address */
static DG_MGR_UINT32 l_mode_3D_dmw_size_and_address_U32;
#endif

DG_MGR_UINT8 diag_mode;

/* Bit-mapped diagnostic state information. */
DG_MGR_UINT8 diag_flags;

/* Some additional control flags to go here */
static DG_MGR_UINT8 l_diag_extra_flags_U8;

/* Store active ecu reset type */
static DG_MGR_UINT8 l_ecu_reset_type_U8;

/* Bit-mapped status bits.*/
static DG_MGR_UINT8 kwp2k_core_status;

/*Used in Routine Control*/
static DG_MGR_UINT8 l_rc_status_flag[PROD_SPEC_NO_SUPP_RC];
static DG_MGR_UINT8 l_rc_index_U8;

/*Ring Buffer Parameters*/
static DG_MGR_UINT8 response_buffer_U8A[NW_MAX_BUFF_LENGTH];
static DG_MGR_UINT16 l_bytesLeftToSend_U16;
static DG_MGR_UINT16 l_totalLengthOfMessage_U16;
static DG_MGR_UINT8 l_nextBuffLength_U8;
static DG_MGR_UINT16 l_responseCurrIndex_U16;




/*
___Core_Initialize(){ }

*/
/********************************************************************************
**
** NAME:        DiagColdInit
**
** DESCRIPTION: This function performs cold initialization.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/
void DiagColdInit(void)
{
    kwp2k_core_init_diagnostics();
#ifdef ERROR_HANDLER_SUPPORTED
    kwp2k_diag_err_handler_cold_init();
#endif
	cur_session = 0x00;
	prev_session = 0x00;
	requested_session = 0x00; 
	l_etm_dtc_number_U8 = 0x00;

    l_diag_extra_flags_U8   = 0;
    l_ecu_reset_type_U8     = (DG_MGR_UINT8)ECU_RESET_NONE;
    diag_mode = FALSE;

    Enable_dtc_logging = TRUE;
#ifdef MODE_27_SUPPORTED
	ser_27_seed_flag = 0x00;
#endif
    #ifdef MODE_3D_SUPPORTED
    l_mode_3D_dmw_size_and_address_U32 = (DG_MGR_UINT32)0;
    #endif
}


/********************************************************************************
**
** NAME:        DiagWarmInit
**
** DESCRIPTION: This function performs warm initialization.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/
void DiagWarmInit(void)
{
    kwp2k_core_init_diagnostics();
#ifdef ERROR_HANDLER_SUPPORTED
    kwp2k_diag_err_handler_warm_init();
#endif
    diag_mode = FALSE;

	cur_session = 0x00;
	prev_session = 0x00;
	requested_session = 0x00; 
#ifdef MODE_27_SUPPORTED
	ser_27_seed_flag = 0x00;
#endif
	l_etm_dtc_number_U8 = 0x00;

	Enable_dtc_logging = TRUE;
    /*
    ** Initialize FNOS diagnostic.
    */
    /* TpInit(); */
/*    DescInit(0);*/
}

/********************************************************************************
**
** NAME:        kwp2k_core_init_diagnostics
**
** DESCRIPTION: This function performs diagnostics initialization.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
**********************************************************************************/
static void kwp2k_core_init_diagnostics(void)
{
    DG_MGR_UINT16 fl_rc_index_value_U8;
#ifdef ERROR_HANDLER_SUPPORTED
    /* Initialize the flags */
    CLEARBIT(diag_flags, NSCLMODE);
#endif
    kwp2k_core_status = 0;

    /*ROUTINE CONTROL init*/
    for( fl_rc_index_value_U8 = 0; fl_rc_index_value_U8 < PROD_SPEC_NO_SUPP_RC; fl_rc_index_value_U8++)
    {
        l_rc_status_flag[fl_rc_index_value_U8] = ZERO;
    }

    /*Ring Buffer variables init*/
    for(fl_rc_index_value_U8=0;fl_rc_index_value_U8<NW_MAX_BUFF_LENGTH;fl_rc_index_value_U8++)
    {
        response_buffer_U8A[fl_rc_index_value_U8] = ZERO;
    }
    l_nextBuffLength_U8 = 0;
    l_responseCurrIndex_U16 = 0;
    l_bytesLeftToSend_U16 = 0;
    l_totalLengthOfMessage_U16 = 0;
}


/*
___Core_Diagnostics_Main_Periodic_Task(){ }

*/
/********************************************************************************
**
** NAME:        DiagMain
**
** DESCRIPTION: This function is primary diagnostic control function. This function
**              is periodically called at the rate of 8ms in slow rate task.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/
void DiagMain(void)
{
	static DG_MGR_UINT8 diag_counter=0;
    
//	diag_counter++;
//	if(diag_counter>=4)
	{
		diag_counter=0;
	    /*
	    ** Manage the DTC storage in NVM.
	    */
#ifdef ERROR_HANDLER_SUPPORTED
	    kwp2k_diag_err_handler_dtc_manager();
#endif
	    /*
	    ** Check whether an Ecu Reset is requested. If it is, then call
	    ** the ecu reset manager to perform correct type of reset.
	    */
	    if(FALSE != TESTBIT(l_diag_extra_flags_U8, ECU_RESET_IN_PRGS))
	    {
	        kwp2k_core_ecu_reset_manager();
	    }
	    else
	    {
	    }
	}
    /*
    ** If in diagnostic mode, check for exit conditions.
    */
#ifdef MODE_10_SUPPORTED
	if(	(diag_session_table_rom[cur_session].abort_lst_fptr != NULL_PTR) \
		&& (FALSE == diag_session_table_rom[cur_session].abort_lst_fptr()) \
	  )
	{
		if(diag_session_table_rom[cur_session].epilog_lst_fptr != NULL_PTR)
		{
			diag_session_table_rom[cur_session].epilog_lst_fptr();
		}
		cur_session = 0x00;
		prev_session = 0x00;
		requested_session = 0x00; 
	}
#endif

    kwp2k_core_Diag_SecurityAccess_timetask();

}

/*
___Service_20(){ }

*/

/********************************************************************************
**
** NAME:        
**
** SERVICE ID:  0x20
**
** SUBFUNCTION: N/A
**
** DESCRIPTION: A diagnostic session shall only be stopped if communication has
** been established between the client and the server and a diagnostic session
** is running.
** If no diagnostic session is running the default session is active. The default
** session cannot be disabled by a stopDiagnosticSession service.
**
** INPUTS:      bufptr[0] --> New Diagnostic State requested..
**              
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/
#ifdef   MODE_20_SUPPORTED
void kwp2k_core_Diag_StopSession(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	bufptr = bufptr;
	dataLen = dataLen;

	if(diag_session_table_rom[cur_session].epilog_lst_fptr != NULL_PTR)
	{
		diag_session_table_rom[cur_session].epilog_lst_fptr();	
	}
	else
	{
		/*Do nothing*/
	}
	/* CLEAR Flags */
	cur_session = 0x00;
	KWP2K_DIAG_SEND_SESSION_TRANSITION_RESPONSE(bufptr,DATA_LEN_1_BYTE);
}
#else
void kwp2k_core_Diag_StopSession(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
	/* Response Buffer*/
	bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
	KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /* MODE_10_SUPPORTED */


/*
___Service_10(){ }

*/

/********************************************************************************
**
** NAME:        AppDescCheckSessionTransition
**
** SERVICE ID:  0x10
**
** SUBFUNCTION: N/A
**
** DESCRIPTION: This function verifies if it is ok to switch from one diagnostic
**              session to another when a session change request message is recieved.
**
** INPUTS:      bufptr[0] --> New Diagnostic State requested..
**              
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/

#ifdef   MODE_10_SUPPORTED
void kwp2k_core_Diag_CheckSessionTransition(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	DG_MGR_UINT8 i = 0;
	DG_MGR_UINT8 resp_sent = FALSE;

	if( dataLen != DATA_LEN_1_BYTE)
	{
		bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
		return;
	}


	for(i=0; diag_session_table_rom[i].session != 0x00 ; i++)
	{
 		if (diag_session_table_rom[i].session == (bufptr[0]&(~NORESPONSE_MASK)))
		{
			resp_sent = TRUE;
			break;
 		}
		
		
	}
	if(resp_sent != TRUE)
	{
			bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupported;
	        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);		
			return;
	}
/**************************************/

	
	if( dataLen == DATA_LEN_1_BYTE)
	{
		for(i=0; diag_session_table_rom[i].session != 0x00 ; i++)
		{
			if (diag_session_table_rom[i].session == (bufptr[0]&(~NORESPONSE_MASK))) /* whether in supported session */
			{
			    /* if current session is requested session */
		        if(diag_session_table_rom[cur_session].session != diag_session_table_rom[i].session )
		        {
					if( (diag_session_table_rom[i].deny_entry_lst_fptr == NULL_PTR)
						|| (FALSE != diag_session_table_rom[i].deny_entry_lst_fptr())
					  )
					{
						#ifdef 	PROD_SPEC_FSM_CONFIGURE
					    if(0x00 != diag_session_table_rom[i].fsm_mode)
					    {
							/*KWP2K_DIAG_FSM_SET_ISUPER_STATE(diag_session_table_rom[i].fsm_mode);*//*for work in abnormal*/
						}
						#endif
                        /* add for S301 */
                        /* request session is programming session and las session is default session, 
                           do not enter programming session */
                        //if((cur_session == 0) && (i == 2))
                        #if 1
						if((GET_DIAG_SESSION() == DEFAULTSESSION) && (i == 2)) 
						{
    
    						bufptr[0] = kwp2k_diag_NrcConditionsNotCorrect;
    		                KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
                           
                        }
                        //else if((cur_session == 2) && (i == 1)) /* current is prog,and request is extend */
                        else if((GET_DIAG_SESSION() == DIAG_PROG_SESS) && (i == 1))
                        {
    						bufptr[0] = kwp2k_diag_NrcConditionsNotCorrect;
    		                KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
                        }
                        else
						#endif
                        {
                            requested_session = i;

                            if(requested_session == 2) /* 2 indicates programming session */
                            {
                                if(FALSE != get_diag_addressing())
								/* phy addressing mode */
                                {
                                    if( (bufptr[0]&NORESPONSE_MASK) == 0 )
                                    {
			                            /* P2 server time:50ms */
			                            bufptr[1] = 0;
			                            bufptr[2] = 0x32;

			                            /* P2* server time:500*10ms */
			                            bufptr[3] = 0x00;
			                            bufptr[4] = 0xC8;
			    						/* Set flags to indicate diagnostic mode */
// /* Response in bootloader */			KWP2K_DIAG_SEND_SESSION_TRANSITION_RESPONSE(bufptr,DATA_LEN_5_BYTE);
                                        KWP2K_DIAG_SEND_NO_RESPONSE(bufptr,DATA_LEN_5_BYTE);
			    						//requested_session = i;
			    						diag_mode = TRUE;
					//					KWP2K_DIAG_START_TIMER(KWP2K_DIAG_TIMER_BASE,10,KWP2K_DIAG_PROG_SESSION_TIMER);
                                    }  
									else
								    {
									    diag_mode = TRUE;
									    KWP2K_DIAG_SEND_NO_RESPONSE(bufptr,DATA_LEN_1_BYTE);
								//		KWP2K_DIAG_START_TIMER(KWP2K_DIAG_TIMER_BASE,10,KWP2K_DIAG_PROG_SESSION_TIMER); 
								    }
                                }
                                else
                                {
									KWP2K_DIAG_SEND_NO_RESPONSE(bufptr,DATA_LEN_1_BYTE);
                                }
                            }
                            else /* default and extend session is requrested */
                            {
                  //          	KWP2K_DIAG_CLEAR_TIMER(KWP2K_DIAG_PROG_SESSION_TIMER);
                            	if( (bufptr[0]&NORESPONSE_MASK) == 0 )
                        		{
			                            /* P2 server time:50ms */
			                            bufptr[1] = 0;
			                            bufptr[2] = 0x32;

			                            /* P2* server time:500*10ms */
			                            bufptr[3] = 0x00;
			                            bufptr[4] = 0xC8;
	        						/* Set flags to indicate diagnostic mode */
	        		                KWP2K_DIAG_SEND_SESSION_TRANSITION_RESPONSE(bufptr,DATA_LEN_5_BYTE);
	        						//requested_session = i;
	        						diag_mode = TRUE;
                        		}
								else
								{
									diag_mode = TRUE;
									KWP2K_DIAG_SEND_NO_RESPONSE(bufptr,DATA_LEN_1_BYTE);
								}
    
                            }
                        }
					}
					else
					{
						/*
		                ** Set response of 'conditions not correct' if an entry condition
		                ** was not met, and don't enter diagnostic mode.
		                */
						bufptr[0] = kwp2k_diag_NrcConditionsNotCorrect;
		                KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
					}
				}
				else /* same as last session state */
				{
					if( (bufptr[0]&NORESPONSE_MASK) == 0 )
					{
											/* P2 server time:50ms */
											bufptr[1] = 0;
											bufptr[2] = 0x32;

											/* P2* server time:500*10ms */
											bufptr[3] = 0x00;
											bufptr[4] = 0xC8;
	                    KWP2K_DIAG_SEND_SESSION_TRANSITION_RESPONSE(bufptr,DATA_LEN_5_BYTE);
					}
					else
					{
						KWP2K_DIAG_SEND_NO_RESPONSE(bufptr,DATA_LEN_1_BYTE);	
					}
					diag_mode = TRUE;
					
				}
				resp_sent = TRUE;
				break;
			}
		}
		if(resp_sent != TRUE)
		{
			bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupported;
	        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);		
		}
	}
	else
	{
		bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);		
	}
}
#else
void kwp2k_core_Diag_CheckSessionTransition(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
	/* Response Buffer*/
	bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
	KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /* MODE_10_SUPPORTED */


/********************************************************************************
**
** NAME:        kwp2k_core_diag_programming_session_transition
**
** SERVICE ID:  0x10
**
** SUBFUNCTION:  0x02
**
** DESCRIPTION: This function processes programming sessiong.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/

void kwp2k_core_diag_programming_session_transition(void)
{
#ifdef MODE_10_SUBFUNC_02_SUPPORTED
    Prod_Spec_Mode10_02_Handler();
#endif
}

/********************************************************************************
**
** NAME:        AppDescOnTransitionSession
**
** SERVICE ID:  N/A
**
** SUBFUNCTION: N/A
**
** DESCRIPTION: This function called on successful transitions.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/
#ifdef MODE_10_SUPPORTED
void kwp2k_core_Diag_OnTransitionSession(void)
{
	if(diag_session_table_rom[cur_session].epilog_lst_fptr != NULL_PTR)
	{
		diag_session_table_rom[cur_session].epilog_lst_fptr();	
	}
#ifndef 	PROD_SPEC_FSM_CONFIGURE
	diag_mode = FALSE;
	/* Traverse the diagnostic prolog function list */
	if(diag_session_table_rom[requested_session].prolog_lst_fptr != NULL_PTR)
	{
		diag_session_table_rom[requested_session].prolog_lst_fptr();
	}
#endif

	prev_session = cur_session;
	cur_session = requested_session;
}
#endif

/********************************************************************************
**
** NAME:        kwp2k_core_check_session
**
** DESCRIPTION: This function returns TRUE if the active diagnostics session 
**              equals requested session.
**
** INPUTS:      Requested session
**
** OUTPUTS:     BOOL.
**
** RETURNS:     TEUE - if Requested session equals active session
**				False- if Requested session not equals active session 
**
********************************************************************************/

static DG_MGR_UINT8 kwp2k_core_check_session(DG_MGR_UINT8 session)
{
	DG_MGR_UINT8 return_value = FALSE;
	DG_MGR_UINT8 temp_session = 0x00;

	temp_session = kwp2k_core_inspect_diagnostics_session();

	if ((session == NO_SESSIONCHECK) || (TESTBIT(session, temp_session ) ))
	{
        return_value = TRUE;
    }
    else
    {
		/*Do Nothing*/
    }
    return return_value;
}


/********************************************************************************
**
** NAME:        kwp2k_core_on_timeout_recovery
**
** DESCRIPTION: This function recovers the conditions when timeout happens.
**
** INPUTS:      NONE
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/
void kwp2k_core_clear_NSCL_mode(void)
{
    /* If in NSCL mode */
    #ifdef MODE_85_SUPPORTED
    if(TESTBIT(diag_flags, NSCLMODE))
    {
        CLEARBIT(diag_flags, NSCLMODE);
    }
    else
    {
    }
    #endif
}

/*
___Service_11(){ }

*/

/********************************************************************************
**
** NAME:        AppDescResetHard
**
** SERVICE ID:  0x11
**
** SUBFUNCTION: 0x01 or 0x02 or 0x03 or 0x04 or 0x05
**
** DESCRIPTION: This function processes the ECU reset request message.
**
** INPUTS:      pMsgContext --> pointer to message context.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/
void kwp2k_core_Diag_ECU_reset(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen)
{
    if( DATA_LEN_1_BYTE == dataLen )
    {
        switch(pMsgContext[0])
        {
        #ifdef MODE_11_SUBFUNC_01_SUPPORTED
        case ECU_RESET_HARD:
        case ECU_RESET_HARD_SUPPRESSION_RESP:
            kwp2k_core_start_ecu_reset_proc(pMsgContext, ECU_RESET_HARD);
            break;
        #endif /* MODE_11_SUBFUNC_01_SUPPORTED */

        #ifdef MODE_11_SUBFUNC_02_SUPPORTED
        case ECU_RESET_KEYOFFON:
		case ECU_RESET_KEYOFFON_SUPPRESSION_RESP:
            kwp2k_core_start_ecu_reset_proc(pMsgContext, ECU_RESET_KEYOFFON);
            break;
        #endif /* MODE_11_SUBFUNC_02_SUPPORTED */

        #ifdef MODE_11_SUBFUNC_03_SUPPORTED
        case ECU_RESET_SOFT:
		case ECU_RESET_SOFT_SUPPRESSION_RESP:
            kwp2k_core_start_ecu_reset_proc(pMsgContext, ECU_RESET_SOFT);
            break;
        #endif /* MODE_11_SUBFUNC_03_SUPPORTED */

        #ifdef MODE_11_SUBFUNC_04_SUPPORTED
        case ECU_RESET_ENBL_SHUTDOWN:
            kwp2k_core_start_ecu_reset_proc(pMsgContext, ECU_RESET_ENBL_SHUTDOWN);
            break;
        #endif /* MODE_11_SUBFUNC_04_SUPPORTED */

        #ifdef MODE_11_SUBFUNC_05_SUPPORTED
        case ECU_RESET_DSBL_SHUTDOWN:
            kwp2k_core_start_ecu_reset_proc(pMsgContext, ECU_RESET_DSBL_SHUTDOWN);
            break;
        #endif /* MODE_11_SUBFUNC_05_SUPPORTED */

        default:
            pMsgContext[0] = kwp2k_diag_NrcSubfunctionNotSupported;
            KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            break;
        }
    }
    else
    {
        pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
    }
}


/********************************************************************************
**
** NAME:        kwp2k_core_start_ecu_reset_proc
**
** SERVICE ID:  0x11
**
** SUBFUNCTION: N/A
**
** DESCRIPTION: This function processes the request to reset the ECU.
**
** INPUTS:      p_msg_S --> pointer to diagnostics message context.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/
static void kwp2k_core_start_ecu_reset_proc(DG_MGR_UINT8* p_msg_S, DG_MGR_UINT8 p_ecu_reset_U8)
{
    /* Take actions only if there is no pending ecu reset */
    if(FALSE == TESTBIT(l_diag_extra_flags_U8, ECU_RESET_IN_PRGS))
    {
        /* Length validity already done in CANDesc */

        /* Parse deny list first */
        if(FALSE == kwp2k_core_diag_trav_bool_void_lst(diag_deny_ecu_reset_list_rom))
        {
            /* Indicate that EcuReset is in progress */
            SETBIT(l_diag_extra_flags_U8, ECU_RESET_IN_PRGS);

            /* Store EcuReset type */
            l_ecu_reset_type_U8 = p_ecu_reset_U8;

            /*
            ** Check if response was requested. If response is not requested,
            ** then reset shall happen almost immediately
            */
            if( FALSE == KWP2K_DIAG_INSPECT_DIAG_RESPONSE_REQUIRED() )
            {
#ifdef MODE_11_ECU_RESETRESP_TIMER_SUPPORTED
                /* Start timeout timer */
            //    KWP2K_DIAG_START_TIMER(KWP2K_DIAG_TIMER_BASE, PROD_SPEC_ECURESET_TIMEOUT, \
            //                            KWP2K_DIAG_ECU_RESET_TIMER);
#endif
				if((ECU_SUPPRESSION_BIT&p_msg_S[0]) == ECU_SUPPRESSION_BIT)
				{
					KWP2K_DIAG_SUPPRESS_RESPONSE();
				}
				else
				{
					KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( p_msg_S, DATA_LEN_1_BYTE );
				}
                SETBIT(l_diag_extra_flags_U8, ECU_READY_TO_RESET);
            }
            else
            {
                /*
                ** Response was not requested
                ** So, take reset action
                */
                SETBIT(l_diag_extra_flags_U8, ECU_READY_TO_RESET);
            }
        }
        else
        {
            /* Conditions are not correct */
            p_msg_S[0] = kwp2k_diag_NrcConditionsNotCorrect;
            KWP2K_DIAG_SEND_ERROR_RESPONSE(p_msg_S);
        }

    }
    else
    {
       /* Do nothing */
    }
}


/********************************************************************************
**
** NAME:        kwp2k_core_ecu_resp_confirmed
**
** SERVICE ID:  0x11 and 10 02(programming session)
**
** SUBFUNCTION: N/A
**
** DESCRIPTION: This function confirms that Ecu positive response was sent to
**              tester successfully.
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/
void kwp2k_core_Diag_ecu_resp_confirmed(void)
{
    /* Is EcuReset active? */
    if(FALSE != TESTBIT(l_diag_extra_flags_U8, ECU_RESET_IN_PRGS))
    {
        /* Positive response was sent successfully within time */
        SETBIT(l_diag_extra_flags_U8, ECU_READY_TO_RESET);
#ifdef MODE_11_ECU_RESETRESP_TIMER_SUPPORTED
      //  KWP2K_DIAG_CLEAR_TIMER(KWP2K_DIAG_ECU_RESET_TIMER);
#endif
    }

#ifdef MODE_10_SUBFUNC_02_SUPPORTED
    else if(FALSE != TESTBIT(l_diag_extra_flags_U8,DIAG_ENTRY_TO_PS_IN_PRGRS))
    {
        SETBIT(l_diag_extra_flags_U8,DIAG_ENTRY_TO_PROG_SESSION);
     //   KWP2K_DIAG_CLEAR_TIMER(KWP2K_DIAG_PROG_SESSION_TIMER);
    }
#endif
    else
    {
    }
}


/********************************************************************************
**
** NAME:        kwp2k_core_ecu_reset_manager
**
** SERVICE ID:  0x11
**
** SUBFUNCTION: N/A
**
** DESCRIPTION: This function processes the ECU Reset request based on the type
**              of reset requested or aborts the reset if timeout occurs.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/

static void kwp2k_core_ecu_reset_manager(void)
{
    /*
    ** Check to see If the positive response was sent successfully by FNOS.
    */
    if(FALSE != TESTBIT(l_diag_extra_flags_U8, ECU_READY_TO_RESET))
    {
#ifdef MODE_11_ECU_RESETRESP_TIMER_SUPPORTED
//        KWP2K_DIAG_CLEAR_TIMER(KWP2K_DIAG_ECU_RESET_TIMER);
#endif
        CLEARBIT(l_diag_extra_flags_U8, ECU_RESET_IN_PRGS);
        CLEARBIT(l_diag_extra_flags_U8, ECU_READY_TO_RESET);

        /*
        ** Deny list was parsed in kwp2k_core_start_ecu_reset_proc() function,
        ** But to be sure parse the deny list once again.
        */
        if(FALSE == kwp2k_core_diag_trav_bool_void_lst(diag_deny_ecu_reset_list_rom))
        {
            /*
            ** Now process ecu reset based on the reset type.
            */
            switch(l_ecu_reset_type_U8)
            {
            #ifdef MODE_11_SUBFUNC_01_SUPPORTED
            case ECU_RESET_HARD:
                {
                    /* Wait watchdog reset */
         //           while(1);
                }
                break;
            #endif /* MODE_11_SUBFUNC_01_SUPPORTED */

            #ifdef MODE_11_SUBFUNC_03_SUPPORTED
            case ECU_RESET_SOFT:
                {
                    /* Wait watchdog reset */
         //           while(1);
                }
                break;
            #endif /* MODE_11_SUBFUNC_03_SUPPORTED */

            default:
                /*l_ecu_reset_type_U8 = l_ecu_reset_type_U8;*/ /*QAC Warning*/
                break;
            }
             
            /* Clear the reset type */
            l_ecu_reset_type_U8 = ECU_RESET_NONE;
        }
        else
        {
           /*Do Nothing*/
        }
    }
    else
    {
        /*
        ** ECU is not ready to be reset, so check if timer is expired.
        */
#ifdef MODE_11_ECU_RESETRESP_TIMER_SUPPORTED
			#if 0
        if(KWP2K_DIAG_CHECK_TIMER(KWP2K_DIAG_ECU_RESET_TIMER) != TIMER_RUNNING)
        {
            /*
            ** Ignore Ecu reset operation as timeout has occured
            ** before we got the confirmation on positive response.
            */
            KWP2K_DIAG_CLEAR_TIMER(KWP2K_DIAG_ECU_RESET_TIMER);
            CLEARBIT(l_diag_extra_flags_U8, ECU_RESET_IN_PRGS);
            CLEARBIT(l_diag_extra_flags_U8, ECU_READY_TO_RESET);
        }
        else
        {
           /*Do Nothing*/
        }
				#endif
#endif
    }
}

/*
___Service_14(){ }

*/
/********************************************************************************
**
** NAME:        kwp2k_core_Diag_ClearDTCs
**
** SERVICE ID:  0x14
**
** SUBFUNCTION: N/A
**
** DESCRIPTION: This function processes the request to erase DTC's.
**
** INPUTS:      pMsgContext --> pointer to diagnostics message context.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/
#ifdef  MODE_14_KWP_SUPPORTED
void kwp2k_core_Diag_ClearDTCs_KWP(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen)
{
	DG_MGR_UINT8 grp_info[3] = {0x00,0x00,0x00};
	grp_info[1] = pMsgContext[0];
	grp_info[2] = pMsgContext[1];

	pMsgContext[0] = grp_info[0];
	pMsgContext[1] = grp_info[1];
	pMsgContext[2] = grp_info[2];

	kwp2k_core_Diag_ClearDTCs(pMsgContext, dataLen + 0x01);
}
#else
void kwp2k_core_Diag_ClearDTCs_KWP(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    pMsgContext[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
}
#endif /* MODE_14_UDS_SUPPORTED */

#ifdef  MODE_14_UDS_SUPPORTED
void kwp2k_core_Diag_ClearDTCs_UDS(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen)
{
	kwp2k_core_Diag_ClearDTCs(pMsgContext, dataLen);
}
#else
void kwp2k_core_Diag_ClearDTCs_UDS(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    pMsgContext[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
}
#endif /* MODE_14_UDS_SUPPORTED */

/********************************************************************************
**
** NAME:        kwp2k_core_Diag_ClearDTCs
**
** SERVICE ID:  0x14
**
** SUBFUNCTION: N/A
**
** DESCRIPTION: This function processes the request to erase DTC's.
**
** INPUTS:      pMsgContext --> pointer to diagnostics message context.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
********************************************************************************/
#if defined(MODE_14_KWP_SUPPORTED) || defined(MODE_14_UDS_SUPPORTED)
static void kwp2k_core_Diag_ClearDTCs(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen)
{
	DG_MGR_UINT32 grp_info = (DG_MGR_UINT32)(0x00);

	grp_info = ((DG_MGR_UINT32) pMsgContext[0]);
	grp_info = ((grp_info << 8) | pMsgContext[1]);
	grp_info = ((grp_info << 8) | pMsgContext[2]);

	grp_info_14 = grp_info; /* Store the group info to report in response for KWP based service $14 */
	cleardtc_ptr = &pMsgContext[0];

	if ( dataLen != DATA_LEN_3_BYTE )
    {
        pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
    }
    else if(Valid_Group_Info(grp_info) != FALSE)
	{
    	if(kwp2k_diag_errlog_count_dtc() != 0)
		{
		    /* Clear the DTC*/
			/*pMsgContext[0] = 0xFF;*/
			KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE(pMsgContext, 0x00);
			kwp2k_diag_err_handler_erase_dtcs();
		}
		else
		{
        /*
        ** Send positive reponse immediately since there is no
        ** DTC stored in NVM.
        */
			DiagMode14CallBack(1);
    }
    }
#ifdef MODE_14_PROD_SPEC_SUBFUNC
	else
	{
		/* Call application function to send response */
		Prod_Spec_14_app_spec_subfunc(pMsgContext);
	}
#else
    else
    {
        /*the specified DTC is not supported*/
        pMsgContext[0] = kwp2k_diag_NrcRequestOutOfRange;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
    }
#endif
}

/****************************************************************************
**
** NAME        : Boolean Valid_Group_Info( UWORD grp_info)
**
** DESCRIPTION : This function returns True if the grp_info is a valid
**                 Functional Group Information ID.
** 
** INPUTS      : grp_info - Group information
** 
** OUTPUTS     : None
** 
** RETURNS     : True/False
** 
** CHANGE HISTORY:
** 
** Date          By       Reason For Change
** --------     -----     -------------------------------------------------------
** Jul/16/03	GPURUSOT   Initial Revision
** Jan/28/04   GPURUSOT   Removed the PowerTrain Failure group and Chasis Failure group
**                        form the valid DTC list.
*****************************************************************************/
static DG_MGR_BOOLEAN Valid_Group_Info( DG_MGR_UINT32 grp_info)
{
  if((grp_info == GROUP1_OF_DTCS) || (grp_info == GROUP2_OF_DTCS) || (grp_info == GROUP3_OF_DTCS))
  {
    return(TRUE);
  }
  return(FALSE);
}
#endif
/********************************************************************************
**
** NAME:        DiagMode14CallBack
**
** DESCRIPTION: This function can be called to send response for mode14
**              request after erase code is excuted
**
** INPUTS:      erase_status - to be compatible with prev version declarations
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
** CHANGE HISTORY:
** ---------------
**     Date          By         Reason For Change
** -------------    -------     ---------------------------------------------------
** April/23/2004    SKHAN17     Initial revision.
** May/30/2004      VKUMBAR     Updated to remove groupofdtc checking
********************************************************************************/
#if defined(MODE_14_KWP_SUPPORTED) || defined(MODE_14_UDS_SUPPORTED)
void DiagMode14CallBack(DG_MGR_UINT8 erase_status)
{
    erase_status = erase_status; /* Remove compilation error */
     /*
    **  FNOS will halt the communication after mode 14 request received,
    **  and send busy response to the tester until this funciton is called.
    */
	#ifdef MODE_14_KWP_SUPPORTED
	cleardtc_ptr[0] = (DG_MGR_UINT8)((grp_info_14 & 0x0000FF00ul) >> 0x08);
	cleardtc_ptr[1] = (DG_MGR_UINT8)(grp_info_14 & 0x000000FFul);
	KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE(cleardtc_ptr,0x00);
	#else
    KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE((void *)0x00,0x00);
	#endif
}
#endif

/*
___Service_13(){ }

*/

/********************************************************************************
**
** NAME:        AppDescReadDiagnosticTroubleCodes
**
** SERVICE ID:  0x13
**
** SUBFUNCTION: NA
**
** DESCRIPTION: This function processes the Read DTCs request message.
**
** INPUTS:      pMsgContext --> pointer to message context.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
********************************************************************************/
#ifdef  MODE_13_SUPPORTED
void kwp2k_core_Diag_read_dtc_info_13(DG_MGR_UINT8 * pMsgContext,DG_MGR_UINT16 dataLen)
{
	if ( DATA_LEN_2_BYTE != dataLen )
	{
	    pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
	    KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
	}
	else
	{
		kwp2k_core_read_DTC_13(pMsgContext);
	}
}

/****************************************************************************
**
** NAME        :       kwp2k_core_read_DTC_13
**
** DESCRIPTION :       
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date          By         Reason For Change
** --------     -----      ---------------------------------------------------
** 03-Aug-2010  gswamina   base version
****************************************************************************/

static void kwp2k_core_read_DTC_13(DG_MGR_UINT8 * pMsgContext)
{
    DG_MGR_UINT8 dtc_status_mask = 0xFF;
    DG_MGR_UINT8 index;
    DG_MGR_UINT8 resp_len = 0x01;
	DG_MGR_UINT8 temp_len = 0x00;
	DG_MGR_UINT8 DTC_Count =0x00;
	DG_MGR_UINT8 mem_type = 0x01; /* NVM_MAIN_MEMORY;*/

    DG_MGR_UINT32 dtc;

    DG_MGR_UINT8 dtc_rec_PS;

	/* Start fetching the DTC info */
	/* CYC:BEGIN_TEST_FOR   MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
	for(index = 0; index < PROD_SPEC_NUMBER_OF_DTCS; index++)
	/* CYC:END_TEST_FOR */
	{
	  dtc_rec_PS = kwp2k_diag_err_handler_is_dtc_matching_to_status(index, dtc_status_mask, mem_type);
	  if(0 != dtc_rec_PS)
	  {
	      /* Its present */
		  DTC_Count++;

	      /* Get DTC value */
	      dtc = kwp2k_diag_err_handler_get_nth_dtc_val(index);
	      pMsgContext[resp_len++] = (DG_MGR_UINT8)((dtc & 0x0000FF00ul) >> 0x08);
	      pMsgContext[resp_len++] = (DG_MGR_UINT8)(dtc & 0x000000FFul);

	      /* Store status */
/*	      pMsgContext[resp_len++]     = dtc_rec_PS;*/
	  }
	}
    pMsgContext[temp_len]     = DTC_Count;
	KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( pMsgContext, resp_len );
}
#endif

/*
___Service_17(){ }

*/
/********************************************************************************
**
** NAME:        kwp2k_core_Diag_read_status_of_dtc
**
** SERVICE ID:  0x17
**
** SUBFUNCTION: None
**
** DESCRIPTION: This function processes the Read DTCs request message.
**
** INPUTS:      pMsgContext --> pointer to message context.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
********************************************************************************/
#ifdef  MODE_17_SUPPORTED
void kwp2k_core_Diag_read_status_of_dtc(DG_MGR_UINT8 * pMsgContext,DG_MGR_UINT16 dataLen)
{
	DG_MGR_UINT32 grp_info = (DG_MGR_UINT32)(0x00);

	grp_info = ((DG_MGR_UINT32) pMsgContext[0]);
	grp_info = ((grp_info << 8) | pMsgContext[1]);

	if ( dataLen != DATA_LEN_2_BYTE )
    {
        pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
    }
    else if(Valid_Group_Info_ser_17(grp_info) != FALSE)
	{
	    /* Get the DTC status and other paramater */
		kwp2k_core_Diag_17_read_dtc(pMsgContext);
    }
#ifdef MODE_17_PROD_SPEC_SUBFUNC
	else
	{
		/* Call application function to send response */
		Prod_Spec_17_app_spec_subfunc(pMsgContext);
	}
#else
	else
	{
        /*the specified DTC is not supported*/
        pMsgContext[0] = kwp2k_diag_NrcRequestOutOfRange;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
	}
#endif
}
#else
void kwp2k_core_Diag_read_status_of_dtc(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    pMsgContext[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
}
#endif
/****************************************************************************
**
** NAME        :       kwp2k_core_Diag_17_read_dtc
**
** DESCRIPTION :       Service $17 support
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date          By         Reason For Change
** --------     -----      ---------------------------------------------------
** 03-Mar-2011  avenkat4   base version
** 29-Apr-2011  avenkat4   Updated to add prod spec status information
****************************************************************************/
#ifdef  MODE_17_SUPPORTED
static void kwp2k_core_Diag_17_read_dtc(DG_MGR_UINT8* pMsgContext)
{
    DG_MGR_UINT8 dtc_status_mask = 0xFF;
    DG_MGR_UINT8 index, i;
	DG_MGR_UINT8* bufptr;
	DG_MGR_UINT8 buflen;
	DG_MGR_UINT8 resp_len = 0x01;
	DG_MGR_UINT8 temp_len = 0x00;
	DG_MGR_UINT8 DTC_Count =0x00;
	DG_MGR_UINT8 mem_type = 0x01; /* NVM_MAIN_MEMORY;*/

    DG_MGR_UINT32 dtc;

	DTCODES_RAM_RECORD *dtc_rec_PS;

	/* Start fetching the DTC info */
	/* CYC:BEGIN_TEST_FOR   MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
	for(index = 0; index < PROD_SPEC_NUMBER_OF_DTCS; index++)
	/* CYC:END_TEST_FOR */
	{
	  dtc_rec_PS = kwp2k_diag_err_handler_is_dtc_matching_to_status(index, dtc_status_mask, mem_type);
	  if(0 != dtc_rec_PS)
	  {
	      /* Its present */
		  DTC_Count++;

	      /* Get DTC value */
	      dtc = kwp2k_diag_err_handler_get_nth_dtc_val(index + 1);
	      pMsgContext[resp_len++] = (DG_MGR_UINT8)((dtc & 0x0000FF00ul) >> 0x08);
	      pMsgContext[resp_len++] = (DG_MGR_UINT8)(dtc & 0x000000FFul);

		  #ifdef MODE_17_PROD_SPEC_DTC_STATUS
	      /* Store status */
	      pMsgContext[resp_len++] = Prod_Spec_Dtc_Status_Ser_17(index + 1);
		  #else
	      /* Store status */
	      pMsgContext[resp_len++] = dtc_rec_PS->dtc_status;
		  #endif

		  #ifdef MODE_17_SUPPLIER_DATA_SUPPORTED
		  /* Store systemSupplierData */
		  bufptr = &pMsgContext[resp_len];
		  buflen = Prod_Spec_Env_Data_Ser_17(bufptr, index + 1);
		  for (i=0 ; i < buflen; i++)
		  {
			pMsgContext[resp_len++] = bufptr[i];
		  }
		  #endif /* MODE_17_SUPPLIER_DATA_SUPPORTED */
	  }
	}
    pMsgContext[temp_len]     = DTC_Count;
	KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( pMsgContext, resp_len );
}
#endif /* MODE_17_SUPPORTED */
/****************************************************************************
**
** NAME        : Boolean Valid_Group_Info_ser_17( UWORD grp_info)
**
** DESCRIPTION : This function returns True if the grp_info is a valid
**                 Functional Group Information ID.
** 
** INPUTS      : grp_info - Group information
** 
** OUTPUTS     : None
** 
** RETURNS     : True/False
** 
** CHANGE HISTORY:
** 
** Date          By       Reason For Change
** --------     -----     -------------------------------------------------------
** Mar/03/11	AVENKAT4   Initial Revision
*****************************************************************************/
#ifdef  MODE_17_SUPPORTED
static DG_MGR_BOOLEAN Valid_Group_Info_ser_17( DG_MGR_UINT32 grp_info)
{
  if((grp_info == GROUP1_OF_DTCS_17) || (grp_info == GROUP2_OF_DTCS_17) || (grp_info == GROUP3_OF_DTCS_17))
  {
    return(TRUE);
  }
  return(FALSE);
}
#endif /* MODE_17_SUPPORTED */

/*
___Service_18(){ }

*/
/********************************************************************************
**
** NAME:        kwp2k_core_Diag_read_dtc_by_status
**
** SERVICE ID:  0x18
**
** SUBFUNCTION: None
**
** DESCRIPTION: This function processes the Read DTCs request message.
**
** INPUTS:      pMsgContext --> pointer to message context.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
********************************************************************************/
#ifdef  MODE_18_SUPPORTED
void kwp2k_core_Diag_read_dtc_by_status(DG_MGR_UINT8 * pMsgContext,DG_MGR_UINT16 dataLen)
{
	DG_MGR_UINT32 grp_info = (DG_MGR_UINT32)(0x00);

	grp_info = ((DG_MGR_UINT32) pMsgContext[1]);
	grp_info = ((grp_info << 8) | pMsgContext[2]);

	if ( dataLen != DATA_LEN_3_BYTE )
    {
        pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
    }
    else if(Valid_Group_Info_ser_18(grp_info) != FALSE)
	{
	    /* Get the DTC status and other paramater */
		kwp2k_core_Diag_18_read_dtc(pMsgContext);
    }
    else
    {
        /*the specified DTC is not supported*/
        pMsgContext[0] = kwp2k_diag_NrcRequestOutOfRange;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
    }
}
#else
void kwp2k_core_Diag_read_dtc_by_status(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    pMsgContext[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
}
#endif
/****************************************************************************
**
** NAME        :       kwp2k_core_Diag_18_read_dtc
**
** DESCRIPTION :       Service $18 support
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date          By         Reason For Change
** --------     -----      ---------------------------------------------------
** 03-Mar-2011  avenkat4   base version
** 29-Apr-2011  avenkat4   Updated the code after intial testing to comply with KWP
****************************************************************************/
#ifdef  MODE_18_SUPPORTED
static void kwp2k_core_Diag_18_read_dtc(DG_MGR_UINT8* pMsgContext)
{
        switch ( pMsgContext[0] )
        {
		#if defined(MODE_18_SUBFUNC_00_SUPPORTED) || defined(MODE_18_SUBFUNC_02_SUPPORTED)
        case REPORT_IDENTIFIED_BCD_DTC:
				kwp2k_core_Diag_18_00_02_read_dtc(pMsgContext);
                break;
        case REPORT_IDENTIFIED_HEX_DTC:
				kwp2k_core_Diag_18_00_02_read_dtc(pMsgContext);
                break;
        #endif /* MODE_18_SUBFUNC_02_SUPPORTED */

		#if defined(MODE_18_SUBFUNC_01_SUPPORTED) || defined(MODE_18_SUBFUNC_03_SUPPORTED)
        case REPORT_SUPPORTED_BCD_DTC:
				kwp2k_core_Diag_18_01_03_read_dtc(pMsgContext);
                break;
        case REPORT_SUPPORTED_HEX_DTC:
				kwp2k_core_Diag_18_01_03_read_dtc(pMsgContext);
                break;
        #endif /* MODE_18_SUBFUNC_03_SUPPORTED */

        default:
        	#ifdef MODE_18_SUBFUNC_APP_SPEC_SUPPORTED
				Prod_Spec_18_app_spec_subfunc(pMsgContext);
			#else
            pMsgContext[0] = kwp2k_diag_NrcSubfunctionNotSupported;
            KWP2K_DIAG_SEND_ERROR_RESPONSE( pMsgContext );
			#endif
            break;
		}
}
/*
___Service_18_02(){ }

*/
/****************************************************************************
**
** NAME        :       kwp2k_core_Diag_18_02_read_dtc
**
** DESCRIPTION :       Service $18 $02 support
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date          By         Reason For Change
** --------     -----      ---------------------------------------------------
** 03-Mar-2011  avenkat4   base version
** 21-June-2011	avenkat4   BSDI00105859->Added Prod Specific DTC status function
****************************************************************************/
#if defined(MODE_18_SUBFUNC_00_SUPPORTED) || defined(MODE_18_SUBFUNC_02_SUPPORTED)
static void kwp2k_core_Diag_18_00_02_read_dtc(DG_MGR_UINT8* pMsgContext)
{
/* This service will get all DTCs with status other than zero */
    DG_MGR_UINT8 dtc_status_mask = 0xFF;
    DG_MGR_UINT8 index;
	DG_MGR_UINT8 resp_len = 0x01;
	DG_MGR_UINT8 temp_len = 0x00;
	DG_MGR_UINT8 DTC_Count =0x00;
	DG_MGR_UINT8 mem_type = 0x01; /* NVM_MAIN_MEMORY;*/

    DG_MGR_UINT32 dtc;

	DTCODES_RAM_RECORD *dtc_rec_PS;

	/* Start fetching the DTC info */
	/* CYC:BEGIN_TEST_FOR   MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
	for(index = 0; index < PROD_SPEC_NUMBER_OF_DTCS; index++)
	/* CYC:END_TEST_FOR */
	{
	  dtc_rec_PS = kwp2k_diag_err_handler_is_dtc_matching_to_status(index, dtc_status_mask, mem_type);
	  if(0 != dtc_rec_PS)
	  {
	      /* Its present */
		  DTC_Count++;

	      /* Get DTC value */
	      dtc = kwp2k_diag_err_handler_get_nth_dtc_val(index + 1);
	      pMsgContext[resp_len++] = (DG_MGR_UINT8)((dtc & 0x0000FF00ul) >> 0x08);
	      pMsgContext[resp_len++] = (DG_MGR_UINT8)(dtc & 0x000000FFul);

		  #ifdef MODE_18_PROD_SPEC_DTC_STATUS
	      /* Store status */
	      pMsgContext[resp_len++] = Prod_Spec_Dtc_Status_Ser_18(index + 1);
		  #else
	      /* Store status */
	      pMsgContext[resp_len++] = dtc_rec_PS->dtc_status;
		  #endif
	  }
	}
    pMsgContext[temp_len]     = DTC_Count;
	KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( pMsgContext, resp_len );
}
#endif /* MODE_18_SUBFUNC_02_SUPPORTED */
/*
___Service_18_03(){ }

*/
/****************************************************************************
**
** NAME        :       kwp2k_core_Diag_18_03_read_dtc
**
** DESCRIPTION :       Service $18 $03 support
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date          By         Reason For Change
** --------     -----      ---------------------------------------------------
** 03-Mar-2011  avenkat4   base version
** 21-June-2011	avenkat4   BSDI00105859->Added Prod Specific DTC status function
****************************************************************************/
#if defined(MODE_18_SUBFUNC_01_SUPPORTED) || defined(MODE_18_SUBFUNC_03_SUPPORTED)
static void kwp2k_core_Diag_18_01_03_read_dtc(DG_MGR_UINT8* pMsgContext)
{
    DG_MGR_UINT32   dtc;
    DG_MGR_UINT8    index;
    DG_MGR_UINT8    resp_len = 0;
    DG_MGR_UINT8    num_dtcs = 0;

    pMsgContext[resp_len++] = PROD_SPEC_NUMBER_OF_DTCS;
    /* Start fetching the DTC info */
    /* CYC:BEGIN_TEST_FOR   MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
    for(index = 0; index < PROD_SPEC_NUMBER_OF_DTCS; index++)
    /* CYC:END_TEST_FOR */
    {
        /* Get DTC value */
        /* Get DTC value */
        dtc = kwp2k_diag_err_handler_get_nth_dtc_val(index + 1);
        pMsgContext[resp_len++] = (DG_MGR_UINT8)((dtc & 0x0000FF00ul) >> 0x08);
        pMsgContext[resp_len++] = (DG_MGR_UINT8)(dtc & 0x000000FFul);

		#ifdef MODE_18_PROD_SPEC_DTC_STATUS
	    /* Store status */
	    pMsgContext[resp_len++] = Prod_Spec_Dtc_Status_Ser_18(index + 1);
		#else
		/* Store status value */
        pMsgContext[resp_len++] = (UINT8)(kwp2k_diag_err_handler_find_nth_dtc_status(index + 1));
		#endif
    }

    KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( pMsgContext, resp_len );
}
#endif /* MODE_18_SUBFUNC_03_SUPPORTED */
#endif /* MODE_18_SUPPORTED */
/****************************************************************************
**
** NAME        : Boolean Valid_Group_Info_ser_18( UWORD grp_info)
**
** DESCRIPTION : This function returns True if the grp_info is a valid
**                 Functional Group Information ID.
** 
** INPUTS      : grp_info - Group information
** 
** OUTPUTS     : None
** 
** RETURNS     : True/False
** 
** CHANGE HISTORY:
** 
** Date          By       Reason For Change
** --------     -----     -------------------------------------------------------
** Mar/03/11	AVENKAT4   Initial Revision
*****************************************************************************/
#ifdef  MODE_18_SUPPORTED
static DG_MGR_BOOLEAN Valid_Group_Info_ser_18( DG_MGR_UINT32 grp_info)
{
  if((grp_info == GROUP1_OF_DTCS_18) || (grp_info == GROUP2_OF_DTCS_18) || (grp_info == GROUP3_OF_DTCS_18))
  {
    return(TRUE);
  }
  return(FALSE);
}
#endif /* MODE_17_SUPPORTED */

/*
___Service_19(){ }

*/

/********************************************************************************
**
** NAME:        AppDescReportNumberByStatusMaskFaultMemory
**
** SERVICE ID:  0x19
**
** SUBFUNCTION: 0x01 or 0x02 0r 0x11 or 0x0F or 0x06 or 0x10 or 0x0A or 0x0E or 0x14
**
** DESCRIPTION: This function processes the Read DTCs request message.
**
** INPUTS:      pMsgContext --> pointer to message context.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
********************************************************************************/
#ifdef  MODE_19_SUPPORTED
void kwp2k_core_Diag_read_dtc_info(DG_MGR_UINT8 * pMsgContext,DG_MGR_UINT16 dataLen)
{
	if (dataLen < DATA_LEN_1_BYTE)
    {
        pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);

        return;
    }
    
    switch ( pMsgContext[0] )
    {
        #ifdef MODE_19_SUBFUNC_01_SUPPORTED
        case REPORT_NUM_DTC_BY_SM:
            if ( DATA_LEN_2_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				/* Need implementation*/
				kwp2k_core_read_DTC_count_by_status_mask(pMsgContext);
            }
            break;
        #endif /* MODE_19_SUBFUNC_01_SUPPORTED */

        #ifdef MODE_19_SUBFUNC_02_SUPPORTED
        case REPORT_DTC_BY_SM:
            if ( DATA_LEN_2_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				kwp2k_core_read_DTC_by_status_mask(pMsgContext);
            }
            break;
        #endif /* MODE_19_SUBFUNC_02_SUPPORTED */

        #ifdef MODE_19_SUBFUNC_03_SUPPORTED
        case REPORT_DTC_SNAPSHOT_IDENT:
            if ( DATA_LEN_1_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				kwp2k_core_read_DTC_SnapshotIdentification (pMsgContext);
            }
            break;
        #endif /* MODE_19_SUBFUNC_03_SUPPORTED */

        #ifdef MODE_19_SUBFUNC_04_SUPPORTED
        case DTC_SNAPSHOT_REC_BY_DTC_NUM:
            if ( DATA_LEN_5_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				kwp2k_core_read_DTC_SnapshotRecord_by_DTCNumber (pMsgContext);
            }
            break;
        #endif /* MODE_19_SUBFUNC_04_SUPPORTED */

        #ifdef MODE_19_SUBFUNC_05_SUPPORTED
        case DTC_SNAPSHOT_REC_BY_REC_NUM:
            if ( DATA_LEN_2_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				kwp2k_core_read_DTC_SnapshotRecord_by_RecordNumber (pMsgContext);
            }
            break;
        #endif /* MODE_19_SUBFUNC_05_SUPPORTED */

        #ifdef MODE_19_SUBFUNC_11_SUPPORTED
        case REPORT_NUM_MIRROR_MEM_DTC_SM:
            if ( DATA_LEN_2_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				/*Need implementation*/
            }
            break;
        #endif /* MODE_19_SUBFUNC_11_SUPPORTED */

        #ifdef MODE_19_SUBFUNC_0F_SUPPORTED
        case REPORT_MIRROR_MEM_DTC_SM:
            if ( DATA_LEN_2_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				/*Need implementation*/
            }
            break;
        #endif /* MODE_19_SUBFUNC_0F_SUPPORTED */

        #ifdef MODE_19_SUBFUNC_0A_SUPPORTED
        case REPORT_SUPPORTED_DTC:
            if ( DATA_LEN_1_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				/*Need implementation*/
				if((pMsgContext[0] & PROD_SPEC_AVAILABLE_DTC_MASK) == 0x00)
				{
                    pMsgContext[0] = kwp2k_diag_NrcRequestOutOfRange;
                    KWP2K_DIAG_SEND_ERROR_RESPONSE( pMsgContext );	
                    return;
				}
				/*****************Changed by dhu3 for s301********************/	
	            kwp2k_core_read_All_DTC_by_status_mask(pMsgContext);
	            /*****************End changed by dhu3 for s301****************/ 
				
            }
            break;
        #endif /* MODE_19_SUBFUNC_0A_SUPPORTED */

        #ifdef MODE_19_SUBFUNC_06_SUPPORTED
        case REPORT_DTC_EXT_DATA_RECORD_SM:
            if ( DATA_LEN_5_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				kwp2k_core_read_read_extended_data_by_DTC_id(pMsgContext);
            }
            break;
        #endif /* MODE_19_SUBFUNC_06_SUPPORTED */

        #ifdef MODE_19_SUBFUNC_10_SUPPORTED
        case REPORT_MIRROR_MEM_DTC_EXT_DATA_RECORD_SM:
            if ( DATA_LEN_5_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				/* Need Implementation */
            }
            break;
        #endif /* MODE_19_SUBFUNC_10_SUPPORTED */

        #ifdef MODE_19_SUBFUNC_0E_SUPPORTED
        case REPORT_MOST_RECENT_CONFIRMED_DTC:
            if ( DATA_LEN_1_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				/*Need implementation*/
            }
            break;
        #endif /* MODE_19_SUBFUNC_0E_SUPPORTED */
        #ifdef MODE_19_SUBFUNC_14_SUPPORTED
        case REPORT_FAULT_DETECTION_CTR:
            if ( DATA_LEN_1_BYTE != dataLen )
            {
                pMsgContext[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
            }
            else
            {
				/*Need implementation*/
            }
            break;
         #endif /* MODE_19_SUBFUNC_14_SUPPORTED */  

        default:
            pMsgContext[0] = kwp2k_diag_NrcSubfunctionNotSupported;
            KWP2K_DIAG_SEND_ERROR_RESPONSE( pMsgContext );
            break;
        }
}
#else
void kwp2k_core_Diag_read_dtc_info(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    pMsgContext[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
}
#endif /* MODE_19_SUPPORTED */

/*
___Service_19_01(){ }

*/

/****************************************************************************
**
** NAME        :       kwp2k_core_read_DTC_count_by_status_mask
**
** DESCRIPTION :       Service $19 01 support
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date          By         Reason For Change
** --------     -----      ---------------------------------------------------
** 02-May-2011  avenkat4   Base version
****************************************************************************/
#ifdef MODE_19_SUBFUNC_01_SUPPORTED
static void kwp2k_core_read_DTC_count_by_status_mask(DG_MGR_UINT8 * pMsgContext)
{
    DG_MGR_UINT16 fl_num_dtc_U16 = 0;
    DG_MGR_UINT8 resp_len = 0x01;
	DG_MGR_UINT8 dtc_status_mask;
    /* store the status mask, reqData and resData refer to the same memory location */
    dtc_status_mask = pMsgContext[ resp_len ];

    /* Get the number by passing mask */
    fl_num_dtc_U16 = diag_errlog_get_dtc_count_by_status(dtc_status_mask);

    /* Fill in the response data */
    /*
    ** The response data consists of the requested Status mask followed
    ** by the DTC Format ID and then the number of DTCs
    */

    pMsgContext[resp_len++] = DTC_SUPPORTED_MASK;

    pMsgContext[resp_len++] = DTC_FORMAT_ID;

    /* DTC Count High Byte */
    pMsgContext[resp_len++] = (UINT8)((fl_num_dtc_U16 & ((UINT16)0xFF00u)) >> 0x08);
    pMsgContext[resp_len++] = (UINT8)(fl_num_dtc_U16 & ((UINT16)0x00FF));
	KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( pMsgContext, resp_len );
}
#endif /* MODE_19_SUBFUNC_01_SUPPORTED */

/*
___Service_19_02(){ }

*/

/****************************************************************************
**
** NAME        :       kwp2k_core_read_DTC_by_status_mask
**
** DESCRIPTION :       Service $19 02 support
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date          By         Reason For Change
** --------     -----      ---------------------------------------------------
** 03-Aug-2010  gswamina   base version
** 28-Jan-2011	avenkat4   CQ BSDI00099010 -> Fixed to report the first DTC also
****************************************************************************/
#ifdef MODE_19_SUBFUNC_02_SUPPORTED
static void kwp2k_core_read_DTC_by_status_mask(DG_MGR_UINT8 * pMsgContext)
{
    DG_MGR_UINT8 dtc_status_mask = 0;
    DG_MGR_UINT8 index;
    DG_MGR_UINT16 resp_len = 0x01;
    DG_MGR_UINT32 dtc;
	DG_MGR_UINT8 mem_type = 0x01; /* NVM_MAIN_MEMORY;*/
	DTCODES_RAM_RECORD *dtc_rec_PS;
      /* store the status mask, reqData and resData refer to the same memory location */
      dtc_status_mask = pMsgContext[ resp_len ];

      /* Store the supported mask */
      pMsgContext[resp_len++] = DTC_SUPPORTED_MASK;  

      if(kwp2k_core_dtc_staus_mask_is_valid(dtc_status_mask) == 0)
      {
          /*  do nothing, the status mask is invalid    */
      }
      else
      {
	      /* Start fetching the DTC info */
	      /* CYC:BEGIN_TEST_FOR   MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
	      for(index = 0; index < PROD_SPEC_NUMBER_OF_DTCS; index++)
	      /* CYC:END_TEST_FOR */
	      {
	      	if(FALSE == kwp2k_core_Diag_Get_DTC_cfg_status(index))
			{
				continue;
			}
	          dtc_rec_PS = kwp2k_diag_err_handler_is_dtc_matching_to_status(index, dtc_status_mask, mem_type);
	          if(0 != dtc_rec_PS)
	          {
	              /* Its present */

	              /* Get DTC value */
	              dtc = kwp2k_diag_err_handler_get_nth_dtc_val(index + 1);
	              pMsgContext[resp_len++] = (DG_MGR_UINT8)((dtc & 0x00FF0000ul) >> 0x10);
	              pMsgContext[resp_len++] = (DG_MGR_UINT8)((dtc & 0x0000FF00ul) >> 0x08);
	              pMsgContext[resp_len++] = (DG_MGR_UINT8)(dtc & 0x000000FFul);

	              /* Store status */
	              pMsgContext[resp_len++]     = (dtc_rec_PS->dtc_ram_status) & DTC_SUPPORTED_MASK;
	          }
	    }
	}
	KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( pMsgContext, resp_len );
}
/****************************************************************************
**
** NAME:        kwp2k_core_dtc_staus_mask_is_valid
**
** DESCRIPTION: This function validates the supported status mask.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
******************************************************************************/

static DG_MGR_UINT8 kwp2k_core_dtc_staus_mask_is_valid(DG_MGR_UINT8 mask)
{
    DG_MGR_UINT8 return_val = 0;
    if(TESTBIT(DTC_SUPPORTED_MASK,mask))
    {
        return_val = 1;    
    }
    return return_val;
}
#endif

/*
___Service_19_03(){ }

*/

/****************************************************************************
**
** NAME        :       kwp2k_core_read_DTC_SnapshotIdentification
**
** DESCRIPTION :       Service $19 03 support
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date          By         Reason For Change
** --------     -----      ---------------------------------------------------
** 02-May-2011  avenkat4   Base version
****************************************************************************/
#ifdef MODE_19_SUBFUNC_03_SUPPORTED
static void kwp2k_core_read_DTC_SnapshotIdentification(DG_MGR_UINT8 * pMsgContext)
{
	pMsgContext[0] = kwp2k_diag_NrcRequestOutOfRange;
    KWP2K_DIAG_SEND_ERROR_RESPONSE( pMsgContext );
}
#endif /* MODE_19_SUBFUNC_03_SUPPORTED */

/*
___Service_19_04(){ }

*/

/****************************************************************************
**
** NAME        :       kwp2k_core_read_DTC_SnapshotRecord_by_DTCNumber
**
** DESCRIPTION :       Service $19 04 support
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date          By         Reason For Change
** --------     -----      ---------------------------------------------------
** 02-May-2011  avenkat4   Base version
****************************************************************************/
#ifdef MODE_19_SUBFUNC_04_SUPPORTED
static void kwp2k_core_read_DTC_SnapshotRecord_by_DTCNumber(DG_MGR_UINT8 * pMsgContext)
{
      /* No snapshot associated */
    pMsgContext[0] = kwp2k_diag_NrcRequestOutOfRange;
    KWP2K_DIAG_SEND_ERROR_RESPONSE( pMsgContext );
 }
#endif /* MODE_19_SUBFUNC_04_SUPPORTED */

/*
___Service_19_05(){ }

*/

/****************************************************************************
**
** NAME        :       kwp2k_core_read_DTC_SnapshotRecord_by_RecordNumber
**
** DESCRIPTION :       Service $19 05 support
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date          By         Reason For Change
** --------     -----      ---------------------------------------------------
** 02-May-2011  avenkat4   Base version
****************************************************************************/
#ifdef MODE_19_SUBFUNC_05_SUPPORTED
static void kwp2k_core_read_DTC_SnapshotRecord_by_RecordNumber(DG_MGR_UINT8 * pMsgContext)
{
    pMsgContext[0] = kwp2k_diag_NrcRequestOutOfRange;
    KWP2K_DIAG_SEND_ERROR_RESPONSE( pMsgContext ); 
}
#endif /* MODE_19_SUBFUNC_05_SUPPORTED */

/*
___Service_19_06(){ }

*/
/****************************************************************************
**
** NAME        :   kwp2k_core_read_read_extended_data_by_DTC_id    
**
** DESCRIPTION :   Service $19 06 support
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date          By         Reason For Change
** 03-Aug-2010   gswamina   base code
** 16-Jun-2011   avenkat4	Fixed BSDI00105715
****************************************************************************/
#ifdef MODE_19_SUBFUNC_06_SUPPORTED
static void kwp2k_core_read_read_extended_data_by_DTC_id(DG_MGR_UINT8 * pMsgContext)
{
	{
       /* Requested extended record number is not there */
	    pMsgContext[0] = kwp2k_diag_NrcRequestOutOfRange;
	    KWP2K_DIAG_SEND_ERROR_RESPONSE(pMsgContext);
	}

}
#endif

/****************************************************************************
**
** NAME        :       kwp2k_core_read_All_DTC_by_status_mask
**
** DESCRIPTION :       Service $19 0A support
** 
** INPUTS      :       
** 
** OUTPUTS     :       
** 
** CHANGE HISTORY:
** 
** Date         :2013/03/14
**Added  By  :      dhu3
** --------     -----      ---------------------------------------------------
** 
****************************************************************************/
#ifdef MODE_19_SUBFUNC_0A_SUPPORTED
static void kwp2k_core_read_All_DTC_by_status_mask(DG_MGR_UINT8 * pMsgContext)
{
    DG_MGR_UINT8 dtc_status_mask = 0;
    DG_MGR_UINT8 index;
    DG_MGR_UINT16 resp_len = 0x01;
    DG_MGR_UINT32 dtc;
	DG_MGR_UINT8 mem_type = 0x01; /* NVM_MAIN_MEMORY;*/
	DTCODES_RAM_RECORD *dtc_rec_PS;

    /* store the status mask, reqData and resData refer to the same memory location */
    dtc_status_mask = pMsgContext[resp_len];

    /* Store the supported mask */
    pMsgContext[resp_len++] = DTC_SUPPORTED_MASK;  

    /* Start fetching the DTC info */
    /* CYC:BEGIN_TEST_FOR   MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
    for(index = 0; index < PROD_SPEC_NUMBER_OF_DTCS; index++)
    /* CYC:END_TEST_FOR */
    {
    	if(FALSE == kwp2k_core_Diag_Get_DTC_cfg_status(index))
    	{
			continue;
    	}
        dtc_rec_PS = kwp2k_diag_err_handler_is_dtc_matching_to_status(index, dtc_status_mask, mem_type);
        /* Get DTC value */
        dtc = kwp2k_diag_err_handler_get_nth_dtc_val(index + 1);

        pMsgContext[resp_len++] = (DG_MGR_UINT8)((dtc & 0x00FF0000ul) >> 0x10);
        pMsgContext[resp_len++] = (DG_MGR_UINT8)((dtc & 0x0000FF00ul) >> 0x08);
        pMsgContext[resp_len++] = (DG_MGR_UINT8)(dtc & 0x000000FFul);

        /* Store status */
        if(NULL_PTR == dtc_rec_PS)
        {
			pMsgContext[resp_len++] = 0;
        }
        else
        {
			pMsgContext[resp_len++] = (dtc_rec_PS->dtc_ram_status) & DTC_SUPPORTED_MASK;
        }
    }
	
	KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( pMsgContext, resp_len );
}
#endif

/*
___Service_1A(){ }

*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_read_ecu_identification_msgid
**              (mode1A)
**
** DESCRIPTION: This function processes incoming LID requests.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
******************************************************************************/

#ifdef MODE_1A_SUPPORTED
void kwp2k_core_Diag_read_ecu_identification_msgid(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
    DG_MGR_UINT8 lid_id;
    DG_MGR_UINT8 length;


    if( DATA_LEN_1_BYTE <= dataLen )
    {
        lid_id = bufptr[0];
        length = sizeof(diag_ecu_identification_table_rom)/sizeof(LID_RECORD);
	    kwp2k_core_Diag_process_lid_request(bufptr, lid_id , (void *)diag_ecu_identification_table_rom,  length, dataLen);
    }
    else
    {
        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
    }
}
#else
void kwp2k_core_Diag_read_ecu_identification_msgid(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 1A supported*/


/*
___Service_21(){ }

*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_read_data_by_lid_msgid
**              (mode21)
**
** DESCRIPTION: This function processes incoming PID requests.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
******************************************************************************/

#ifdef MODE_21_SUPPORTED
void kwp2k_core_Diag_read_data_by_lid_msgid(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
    DG_MGR_UINT8 pid_id;
    DG_MGR_UINT8 length;

    if( DATA_LEN_1_BYTE <= dataLen )
    {
        pid_id = bufptr[0];
        length = sizeof(diag_lid_table_rom)/sizeof(LID_RECORD);
	    kwp2k_core_Diag_process_lid_request(bufptr, pid_id , (void *)diag_lid_table_rom, length, dataLen );
    }
    else
    {
        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
    }
}
#else
void kwp2k_core_Diag_read_data_by_lid_msgid(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 21 supported*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_process_pid_request
**
** DESCRIPTION: This function processes incoming PID requests.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
******************************************************************************/
#if defined( MODE_21_SUPPORTED ) || defined( MODE_1A_SUPPORTED )
static void kwp2k_core_Diag_process_lid_request( DG_MGR_UINT8* bufptr,const DG_MGR_UINT8 pid_id, void* diag_table_rom, DG_MGR_UINT8 length, DG_MGR_UINT8 dataLen )
{
    LID_RECORD  *table_ptr;
    DG_MGR_UINT8       resp_length;
    DG_MGR_UINT8       pid_bytes;
    DG_MGR_BOOLEAN     fl_return_value_BOOL = FALSE;
	DG_MGR_UINT8	i;
	DG_MGR_UINT8	loop_size;

    table_ptr = (LID_RECORD *) diag_table_rom;
	loop_size = length;
    for(i=0x01;( i < loop_size) && ( FALSE == fl_return_value_BOOL); i++)
    {
        if( table_ptr->pid_number == pid_id )
        {
			if(kwp2k_core_check_session(table_ptr->sup_session))
			{
				if(table_ptr->response_data_handler != APPL_HANDLE)
				{
                    if( DATA_LEN_1_BYTE != dataLen )
                    {
                        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
                        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
                        fl_return_value_BOOL = TRUE;
                    }
					else
					{
	                	if( table_ptr->pid_length == 0x00 )
		                {
	                            pid_bytes = (*table_ptr->pid_function_ptr)( &bufptr[DATA_LEN_1_BYTE], dataLen );
		            /*
		            ** If number of PID bytes is 0, then the condition is
		            ** not correct for the function to return the PID data.
		            */
		            if (pid_bytes == ZERO)
		            {
		                bufptr[0] = kwp2k_diag_NrcConditionsNotCorrect;
		                KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
		                fl_return_value_BOOL = TRUE;
		            }
		            else
		            {
		                KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , pid_bytes + DATA_LEN_1_BYTE );
		                fl_return_value_BOOL = TRUE;
		            }
				}
				else
				{
				   /*----------------------------------------------------------**
				   **"buffer_ptr" will be                                      **
	               ** used as a pointer to a memory buffer and "buffer_length" **
	               ** bytes will be fetched from the buffer and used to send a **
	               ** response message to the tester.				           **
				   **----------------------------------------------------------*/
				   KWP2K_DIAG_MEMCPY( &bufptr[DATA_LEN_1_BYTE], table_ptr->pid_data_ptr , table_ptr->pid_length);
						   resp_length =  table_ptr->pid_length + DATA_LEN_1_BYTE;
		                   KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr, resp_length );
		                   fl_return_value_BOOL = TRUE;
		                }
					}
				}
				else
				{
					/*
					** Have to call a function to get the PID (the function will
					** return number of bytes in the PID).
					*/
					/* Handle both positive/negative response in the application itself */
					(*table_ptr->pid_function_ptr)( &bufptr[0], dataLen );
		            fl_return_value_BOOL = TRUE;
				}
			}
			else
			{
	            bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupportedInActiveSession;
	            KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
	            fl_return_value_BOOL = TRUE;
			}
        }
        else
        {
          /* Do Nothing */
        }
        ++table_ptr;
    }
    if( FALSE != fl_return_value_BOOL )
    {
      /* Do Nothing */
    }
    else
    {
      /*
      ** If we get here, the PID was not support.
      */
        bufptr[0] = kwp2k_diag_NrcRequestOutOfRange ;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
    }
}
#endif
/*
___Service_3B(){ }

*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_write_data_by_lid_msgid
**              (mode3B)
**
** DESCRIPTION: This function processes the write data by identifier.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
******************************************************************************/
#ifdef MODE_3B_SUPPORTED
void kwp2k_core_Diag_write_data_by_lid_msgid( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen )
{
    if( dataLen < DATA_LEN_2_BYTE )
    {
        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
    }
    else
    {
        kwp2k_core_Diag_process_write_lid_request(bufptr, dataLen - DATA_LEN_1_BYTE  , (void *)diag_write_lid_table_rom);
    }
}
#else
void kwp2k_core_Diag_write_data_by_lid_msgid(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 2E supported*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_process_write_lid_request
**
** DESCRIPTION: This function processes incoming PID requests.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
******************************************************************************/
#if defined( MODE_3B_SUPPORTED )
static void kwp2k_core_Diag_process_write_lid_request( DG_MGR_UINT8* bufptr,DG_MGR_UINT8 dataLen, void* diag_table_rom )
{
    LID_RECORD *table_ptr;
    DG_MGR_UINT8      fl_write_pid_id_U8;
    DG_MGR_UINT8       fl_resp_code_U8;
    DG_MGR_BOOLEAN     fl_return_value_BOOL = FALSE;
	DG_MGR_UINT8	i;
	DG_MGR_UINT8	loop_size;

    fl_write_pid_id_U8 = bufptr[0];

    table_ptr = (LID_RECORD *) diag_table_rom;

	loop_size = sizeof(diag_write_lid_table_rom)/sizeof(LID_RECORD);
    for(i=0x01;( i < loop_size) && ( FALSE == fl_return_value_BOOL); i++)
    {
        if( table_ptr->pid_number == fl_write_pid_id_U8 )
        {
		    if(table_ptr->response_data_handler != APPL_HANDLE)
			{
				if(kwp2k_core_check_session(table_ptr->sup_session))
				{
		            if( table_ptr->pid_length == dataLen )
		            {
		            	if( table_ptr->pid_data_ptr == NULL_PTR )
						{
			                /*
			                ** Have to call a function to get the PID (the function will
			                ** return number of bytes in the PID).
			                */
			                fl_resp_code_U8 = (*table_ptr->pid_function_ptr)( &bufptr[DATA_LEN_1_BYTE], dataLen );

		                /*
		                ** If number of PID bytes is 0, then the condition is
		                ** not correct for the function to return the PID data.
		                */
		                if (ZERO != fl_resp_code_U8)
		                {
		                    bufptr[0] = fl_resp_code_U8;
		                    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr );
		                    fl_return_value_BOOL = TRUE;
		                }
		                else
		                {
		                    KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , DATA_LEN_1_BYTE );
		                    fl_return_value_BOOL = TRUE;
		                }
		            }
		            else
		            {
				   /*----------------------------------------------------------**
				   **"buffer_ptr" will be                                      **
	               ** used as a pointer to a memory buffer and "buffer_length" **  
	               ** bytes will be fetched from the buffer and used to send a **  
	               ** response message to the tester.				           **
				   **----------------------------------------------------------*/
				   KWP2K_DIAG_MEMCPY( table_ptr->pid_data_ptr, &bufptr[DATA_LEN_1_BYTE], dataLen);
	               KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr, DATA_LEN_1_BYTE );
				   fl_return_value_BOOL = TRUE;
				}
			}
			else
			{
			                bufptr[0] = kwp2k_diag_NrcInvalidFormat ;
			                KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr );
				fl_return_value_BOOL = TRUE;
					}
				}
				else
				{
					bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupportedInActiveSession;
					KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
					fl_return_value_BOOL = TRUE;
				}
			}
			else
			{
				/* Application Handle */
	                /*
	                ** Have to call a function to get the PID (the function will
	                ** return the response data).
	                */
	                fl_resp_code_U8 = (*table_ptr->pid_function_ptr)( &bufptr[0], dataLen);

	                /*
	                ** If number of PID bytes is 0, then the condition is
	                ** not correct for the function to return the PID data.
	                */
	                if (ZERO != fl_resp_code_U8)
	                {
	                    bufptr[0] = fl_resp_code_U8;
	                    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr );
	                    fl_return_value_BOOL = TRUE;
	                }
	                else
	                {
	                    KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , DATA_LEN_1_BYTE );
	                    fl_return_value_BOOL = TRUE;
	                }
			}
        }
        else
        {
          /* Do Nothing*/
        }

        ++table_ptr;
    }
    /*
    ** If we get here, the PID was not support.
    */
    if( FALSE != fl_return_value_BOOL)
    {
      /* Do Nothing*/
    }
    else
    {
        bufptr[0] = kwp2k_diag_NrcRequestOutOfRange ;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
    }
}

#endif
/*
___Service_22(){ }

*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_read_data_msgid
**              (mode22)
**
** DESCRIPTION: This function processes incoming PID requests.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
******************************************************************************/

#ifdef MODE_22_SUPPORTED
void kwp2k_core_Diag_read_data_msgid(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
    DG_MGR_UINT16 pid_id;

    if( dataLen != DATA_LEN_2_BYTE)
    {
        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
    }
    else
    {
        pid_id = ( ( bufptr[0] << SHIFT_8_BITES ) | bufptr[1] );
        kwp2k_core_Diag_process_pid_request(bufptr, pid_id);
    }
}
#else
void kwp2k_core_Diag_read_data_msgid(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 22 supported*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_process_pid_request
**
** DESCRIPTION: This function processes incoming PID requests.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
******************************************************************************/
#if defined( MODE_22_SUPPORTED ) || defined( MODE_2E_SUPPORTED )
static void kwp2k_core_Diag_process_pid_request( DG_MGR_UINT8* bufptr,const DG_MGR_UINT16 pid_id )
{
    PID_RECORD  *table_ptr;
    DG_MGR_UINT8       pid_bytes;
    DG_MGR_BOOLEAN     fl_return_value_BOOL = FALSE;
	DG_MGR_UINT8	i;
	DG_MGR_UINT8	loop_size;

    table_ptr = (PID_RECORD *) diag_pid_table_rom;
	loop_size = sizeof(diag_pid_table_rom)/sizeof(PID_RECORD);
    for(i=0x01;( i < loop_size) && ( FALSE == fl_return_value_BOOL); i++)
    {
        if( table_ptr->pid_number == pid_id )
        {
			if(kwp2k_core_check_session(table_ptr->sup_session))
			{
				if( table_ptr->pid_length == 0x00 )
				{
		             /*
		             ** Have to call a function to get the PID (the function will
		             ** return number of bytes in the PID).
		             */
		            pid_bytes = (*table_ptr->pid_function_ptr)( &bufptr[DATA_LEN_2_BYTE] );/*gswamina: need to chk*/

		            /*
		            ** If number of PID bytes is 0, then the condition is
		            ** not correct for the function to return the PID data.
		            */
		            if (pid_bytes == ZERO)
		            {
		                bufptr[0] = kwp2k_diag_NrcConditionsNotCorrect;
		                KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
		                fl_return_value_BOOL = TRUE;
		            }
		            else
		            {
		                KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , pid_bytes + DATA_LEN_2_BYTE );
		                fl_return_value_BOOL = TRUE;
		            }
				}
				else
				{
				   /*----------------------------------------------------------**
				   **"buffer_ptr" will be                                      **
	               ** used as a pointer to a memory buffer and "buffer_length" **  
	               ** bytes will be fetched from the buffer and used to send a **  
	               ** response message to the tester.				           **
				   **----------------------------------------------------------*/
				   KWP2K_DIAG_MEMCPY( &bufptr[DATA_LEN_2_BYTE], table_ptr->pid_data_ptr , table_ptr->pid_length);
	               KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr, table_ptr->pid_length );
				   fl_return_value_BOOL = TRUE;

				}
			}
			else
			{
				bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupportedInActiveSession;
				KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
				fl_return_value_BOOL = TRUE;
			}
        }
        else
        {
          /* Do Nothing */
        }
        ++table_ptr;
    }
    if( FALSE != fl_return_value_BOOL )
    {
      /* Do Nothing */
    }
    else
    {
      /*
      ** If we get here, the PID was not support.
      */
        if(FALSE != get_diag_addressing())
        {
			bufptr[0] = kwp2k_diag_NrcDIDRequestOutOfRange ;
			KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
        }
        else
        {
			KWP2K_DIAG_SEND_NO_RESPONSE(bufptr,DATA_LEN_1_BYTE);
        }

    }
}
#endif
/*
___Service_2E(){ }

*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_write_data_msgid
**              (mode2E)
**
** DESCRIPTION: This function processes the write data by identifier.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
******************************************************************************/
#ifdef MODE_2E_SUPPORTED
void kwp2k_core_Diag_write_data_msgid( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen )
{
    if( dataLen < DATA_LEN_3_BYTE )
    {
        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
    }
    else
    {
        kwp2k_core_Diag_process_write_pid_request(bufptr, dataLen - DATA_LEN_2_BYTE);
    }
}
#else
void kwp2k_core_Diag_write_data_msgid(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 2E supported*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_process_pid_request
**
** DESCRIPTION: This function processes incoming PID requests.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
******************************************************************************/
#if defined( MODE_22_SUPPORTED ) || defined( MODE_2E_SUPPORTED )
static void kwp2k_core_Diag_process_write_pid_request( DG_MGR_UINT8* bufptr,DG_MGR_UINT8 dataLen )
{
    PID_RECORD *table_ptr;
    DG_MGR_UINT16      fl_write_pid_id_U16;
    DG_MGR_UINT8       fl_resp_code_U8;
    DG_MGR_BOOLEAN     fl_return_value_BOOL = FALSE;
	DG_MGR_UINT8	i;
	DG_MGR_UINT8	loop_size;

    fl_write_pid_id_U16 = ( ( bufptr[0] << SHIFT_8_BITES ) | bufptr[1] );

    table_ptr = (PID_RECORD *) diag_write_pid_table_rom;
	loop_size = sizeof(diag_write_pid_table_rom)/sizeof(PID_RECORD);
    for(i=0x01;( i < loop_size) && ( FALSE == fl_return_value_BOOL); i++)
    {
        if( table_ptr->pid_number == fl_write_pid_id_U16 )
        {
        	
				if(kwp2k_core_check_session(table_ptr->sup_session))
				{
		            if( table_ptr->pid_length != ZERO )
					{
			            if( table_ptr->pid_length == dataLen )
			            {
			            	if(l_security_status_U8 == FALSE)
							{
				                /*
				                ** Have to call a function to get the PID (the function will
				                ** return number of bytes in the PID).
				                */
				                fl_resp_code_U8 = (*table_ptr->pid_function_ptr)( &bufptr[DATA_LEN_2_BYTE] );

				                /*
				                ** If number of PID bytes is 0, then the condition is
				                ** not correct for the function to return the PID data.
				                */
				                if (ZERO != fl_resp_code_U8)
				                {
				                    bufptr[0] = fl_resp_code_U8;
				                    KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
				                    fl_return_value_BOOL = TRUE;
				                }
				                else
				                {
				                    KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , DATA_LEN_2_BYTE );
				                    fl_return_value_BOOL = TRUE;
				                }
			                }
							else
							{
								bufptr[0] = kwp2k_diag_NrcAccessDenied;
								KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
								return;
							}
			            }
			            else
			            {
			                bufptr[0] = kwp2k_diag_NrcInvalidFormat ;
			                KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
			                fl_return_value_BOOL = TRUE;
			            }
					}
					else
					{
					   /*----------------------------------------------------------**
					   **"buffer_ptr" will be                                      **
		               ** used as a pointer to a memory buffer and "buffer_length" **  
		               ** bytes will be fetched from the buffer and used to send a **  
		               ** response message to the tester.				           **
					   **----------------------------------------------------------*/
					   KWP2K_DIAG_MEMCPY( table_ptr->pid_data_ptr, &bufptr[DATA_LEN_1_BYTE], dataLen);
		               KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr, DATA_LEN_1_BYTE );
					   fl_return_value_BOOL = TRUE;

					}
				}
				else
				{
					bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupportedInActiveSession;
					KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
					fl_return_value_BOOL = TRUE;
				}
			
        }
        else
        {
          /* Do Nothing*/
        }

        ++table_ptr;
    }
    /*
    ** If we get here, the PID was not support.
    */
    if( FALSE != fl_return_value_BOOL)
    {
      /* Do Nothing*/
    }
    else
    {
        bufptr[0] = kwp2k_diag_NrcRequestOutOfRange ;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
    }
}
#endif
/*
___Service_2F(){ }

*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_IOControlByDataIdentifier 
**              (mode2F)
**
** DESCRIPTION: This function processes the IO control by identifier.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
******************************************************************************/
#if defined(MODE_2F_SUPPORTED)
void kwp2k_core_Diag_IOControlByDataIdentifier( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen )
{
    IOCTL_RECORD const *ioctl_record_ptr;
    DG_MGR_UINT8  fl_resp_code_U8;
    DG_MGR_UINT8  fl_datalen_U8;
    DG_MGR_UINT16 fl_ioctrl_pid_id_U16;
    DG_MGR_UINT8  pid_index;

#if 1

    /* Check for min/max data byte requirements are met. If True invoke coressponding function handler*/
    if ( (( bufptr[IOCTL_IOCP] == RETURN_CONTROL_ECU)   && (dataLen != DATA_LEN_3_BYTE )) ||
         (( bufptr[IOCTL_IOCP] == SHORT_TERM_ADJUST)    && (dataLen != DATA_LEN_4_BYTE )) )
    {
        bufptr[0] = kwp2k_diag_NrcInvalidFormat ;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
        return;
    }

    /* Check for valid IO Control Parameter */
    if ( ( bufptr[IOCTL_IOCP] != RETURN_CONTROL_ECU) && \
         ( bufptr[IOCTL_IOCP] != SHORT_TERM_ADJUST) )
    {
        bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupported ;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
        return;
    }


#else
    /* Check for min/max data byte requirements are met. If True invoke coressponding function handler*/
    if (dataLen < DATA_LEN_3_BYTE )
    {
        bufptr[0] = kwp2k_diag_NrcInvalidFormat ;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
        return;
    }
#endif
      /* store the request I/O control PID number in a single variable */
    fl_ioctrl_pid_id_U16 = ( ((DG_MGR_UINT16)bufptr[0] << SHIFT_8_BITES ) | bufptr[1] );
    /*
    ** Search through the IOCTL table for the local id being requested until 
    ** found or end of table.
    */ 
    ioctl_record_ptr = &ioctl_diag_table_rom[0];
    pid_index = 0;

    while (ioctl_record_ptr->pid != 0x00)
    {
        if (ioctl_record_ptr->pid == fl_ioctrl_pid_id_U16)
        {
        	fl_datalen_U8 = dataLen - DATA_LEN_3_BYTE;

        	if(bufptr[IOCTL_IOCP] == RETURN_CONTROL_ECU)	/* for refine II */
        	{
        		if(0 != fl_datalen_U8)
				{
					bufptr[0] = kwp2k_diag_NrcInvalidFormat ;
	        		KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
	        		return;
				}
        	}
        	else
        	{
        		if((ioctl_record_ptr->data_length) != fl_datalen_U8)
				{
					bufptr[0] = kwp2k_diag_NrcInvalidFormat ;
	        		KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
	        		return;
				}
        	}
        	
			
			if(l_security_status_U8 == FALSE)
			{
				if(kwp2k_core_check_session(ioctl_record_ptr->sup_session))
				{
					/* store the length of data alone */
					fl_datalen_U8 = dataLen - DATA_LEN_3_BYTE;
					fl_resp_code_U8 = ioctl_record_ptr->function_handler(bufptr[IOCTL_IOCP],fl_datalen_U8,&bufptr[IOCTL_DATA]);
					/*
					** If number of data bytes is 0, then the condition is
					** not correct for the function to return the PID data.
					*/
					if ( fl_resp_code_U8 != 0x00)
					{
						bufptr[0] = fl_resp_code_U8;
						KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
						return;
					}
					else
					{
						if (bufptr[IOCTL_IOCP] == RETURN_CONTROL_ECU) 
						{
							clear_diag_ioctl_in_progress(pid_index);
						}
						else
						{
							SETBIT(ioctrl_diag_status, IOCTL_REQ_RCVD);
							set_diag_ioctl_in_progress(pid_index);
						}
						(void)KWP2K_DIAG_MEMCPY(&bufptr[3], ioctl_record_ptr->data_ptr, ioctl_record_ptr->data_length);
						KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr, dataLen);
						
						return;
					}
				}
				else
				{
					bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupportedInActiveSession;
					KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
				}
			}
			else
			{
				bufptr[0] = kwp2k_diag_NrcAccessDenied;
				KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
				return;
			}

        }
        ++ioctl_record_ptr;
        ++pid_index;
    }
    /*
    ** Local identifier NOT found.
    */
    bufptr[0] = kwp2k_diag_NrcIOCTLRequestOutOfRange;
    KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
}

/**************************************************************************** 

NAME:        app_abort_io_ctrl

DESCRIPTION: This function is used to set the ioctrl flags to default

INPUTS:      supp_modes- supported modes 

OUTPUTS:     NONE.

RETURNS:     TRUE if mode supported else FALSE

CHANGE HISTORY:

Date          By     Reason For Change
--------     -----   -------------------------------------------------------

******************************************************************************/

static void app_abort_io_ctrl_diag( void )
{
    IOCTL_RECORD const *ioctl_record_ptr;
    DG_MGR_UINT8 pid_index = 0x00;

    if (((DG_MGR_UINT8)TESTBIT(ioctrl_diag_status, IOCTL_REQ_RCVD)) != 0)
    {
        ioctl_record_ptr = &ioctl_diag_table_rom[0];
        pid_index = 0;
        while (ioctl_record_ptr->pid != 0x00)
        {
            if( (is_diag_ioctl_in_progress(pid_index) != 0) && 
                (ioctl_record_ptr->recover_function != NULL) )
            {
                clear_diag_ioctl_in_progress(pid_index);     
                ioctl_record_ptr->recover_function();
            }
            ++ioctl_record_ptr;
            ++pid_index;
        }
        CLEARBIT(ioctrl_diag_status, IOCTL_REQ_RCVD);
    }
}

void kwp2k_core_Diag_2F_standby(void)
{
	app_abort_io_ctrl_diag();
}


#else
void kwp2k_core_Diag_IOControlByDataIdentifier(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 2F supported*/

/*
___Service_30(){ }

*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_IOControlByLocalDataIdentifier 
**              (mode30)
**
** DESCRIPTION: This function processes the IO control by identifier.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
CHANGE HISTORY:
Date          By     Reason For Change
--------     -----   -------------------------------------------------------
25-May-2011	avenkat4	Fix for CQ BSDI00104748
******************************************************************************/
#if defined(MODE_30_SUPPORTED)
void kwp2k_core_Diag_IOControlByLocalDataIdentifier( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen )
{
#ifndef LID_TABLE_APPLICATION_MANAGED
    LID_IOCTL_RECORD const *lid_ioctl_record_ptr;
    DG_MGR_UINT8 fl_ioctrl_lid_id_U8;
    DG_MGR_UINT8  lid_index;
#endif
    DG_MGR_UINT8  fl_resp_code_U8;
    DG_MGR_UINT8  fl_datalen_U8;

#ifdef LID_TABLE_APPLICATION_MANAGED
	fl_datalen_U8 = (DG_MGR_UINT8)dataLen;
	fl_resp_code_U8 = Prod_Spec_Mode30_Service_Handler(&bufptr[0], &fl_datalen_U8);
    if ( fl_resp_code_U8 != 0x00)
    {
        bufptr[0] = fl_resp_code_U8;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
        return;
    }
    else
    {
        KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , fl_datalen_U8); /*(dataLen - DATA_LEN_1_BYTE)*/
        return;
    }

#else
    /*
    ** Search through the IOCTL table to identify whether the local id
    ** will be serviced by the Application itself.
    */
    /* store the request I/O control LID number in a single variable */
    fl_ioctrl_lid_id_U8 = bufptr[0];
    lid_ioctl_record_ptr = &lid_ioctl_diag_table_rom[0];

    /* Check for min/max data byte requirements are met. If True invoke coressponding function handler*/
    if (dataLen < DATA_LEN_2_BYTE )
    {
        bufptr[0] = kwp2k_diag_NrcInvalidFormat ;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
        return;
    }
    /*
    ** Search through the IOCTL table for the local id being requested until
    ** found or end of table.
    */
    lid_ioctl_record_ptr = &lid_ioctl_diag_table_rom[0];
    lid_index = 0;

    while (lid_ioctl_record_ptr->lid != 0x00)
    {
        if (lid_ioctl_record_ptr->lid == fl_ioctrl_lid_id_U8)
        {
			if(kwp2k_core_check_session(lid_ioctl_record_ptr->sup_session))
			{

	            /* store the length of data alone */
	            fl_datalen_U8 = dataLen - DATA_LEN_2_BYTE;

	            fl_resp_code_U8 = lid_ioctl_record_ptr->function_handler(bufptr[LID_IOCTL_IOCP],fl_datalen_U8,&bufptr[LID_IOCTL_DATA]);

	            if ( fl_resp_code_U8 != 0x00)
	            {
	                bufptr[0] = fl_resp_code_U8;
	                KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
	                return;
	            }
	            else
	            {
	                if (bufptr[LID_IOCTL_IOCP] == LID_RETURN_CONTROL_ECU)
	                {
	                    clear_diag_lid_ioctl_in_progress(lid_index);
	                }
	                else
	                {
	                    SETBIT(lid_ioctrl_diag_status, IOCTL_REQ_RCVD);
	                    set_diag_lid_ioctl_in_progress(lid_index);
	                }
	                (void)KWP2K_DIAG_MEMCPY(&bufptr[2], lid_ioctl_record_ptr->data_ptr,
	                                   lid_ioctl_record_ptr->data_length);
				    fl_datalen_U8 = lid_ioctl_record_ptr->data_length + ((UINT8)(0x02)); /* Plus 2 to include the control parameter and LID in response */
	                KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , fl_datalen_U8); /*(dataLen - DATA_LEN_1_BYTE)*/
	                return;
	            }
			}
			else
			{
				bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupportedInActiveSession;
				KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
			}
        }
        ++lid_ioctl_record_ptr;
        ++lid_index;
    }
    /*
    ** Local identifier NOT found.
    */
    bufptr[0] = kwp2k_diag_NrcRequestOutOfRange;
    KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
#endif
}

/**************************************************************************** 

NAME:        app_abort_io_ctrl

DESCRIPTION: This function is used to set the ioctrl flags to default

INPUTS:      supp_modes- supported modes 

OUTPUTS:     NONE.

RETURNS:     TRUE if mode supported else FALSE

CHANGE HISTORY:

Date          By     Reason For Change
--------     -----   -------------------------------------------------------

******************************************************************************/
#ifndef LID_TABLE_APPLICATION_MANAGED
static void app_abort_io_ctrl_lid_diag( void )
{
    LID_IOCTL_RECORD const *lid_ioctl_record_ptr;
    DG_MGR_UINT8 lid_index = 0x00;

    if (((DG_MGR_UINT8)TESTBIT(lid_ioctrl_diag_status, IOCTL_REQ_RCVD)) != 0)
    {
        lid_ioctl_record_ptr = &lid_ioctl_diag_table_rom[0];
        lid_index = 0;
        while (lid_ioctl_record_ptr->lid != 0x00)
        {
            if( (is_diag_lid_ioctl_in_progress(lid_index) != 0) &&
                (lid_ioctl_record_ptr->recover_function != NULL) )
            {
                clear_diag_lid_ioctl_in_progress(lid_index);
                lid_ioctl_record_ptr->recover_function();
            }
            ++lid_ioctl_record_ptr;
            ++lid_index;
        }
        CLEARBIT(lid_ioctrl_diag_status, IOCTL_REQ_RCVD);
    }
}
#endif
#else
void kwp2k_core_Diag_IOControlByLocalDataIdentifier(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 2F supported*/

/*
___Service_23(){ }

*/

/****************************************************************************
**
** NAME:        kwp2k_core_Diag_Read_Memory_by_Address
**              (mode23)
**
** DESCRIPTION: This function processes the Read memory by address (DMR) identifier.
**
** INPUTS:      NONE.
**
** OUTPUTS:     NONE.
**
** RETURNS:     NONE.
**
CHANGE HISTORY:

** Date          By     Reason For Change
**--------     -----   -------------------------------------------------------
** 29-Apr-2011	avenkat4	Updated to work for all the configurations
******************************************************************************/
#ifdef MODE_23_SUPPORTED
void kwp2k_core_Diag_Read_Memory_by_Address( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen )
{
#if(MODE_23_SUPPORTED_BASED_ON != APPL_SPECIFIC)
    DG_MGR_UINT8  fl_addr_format_U8;
    DG_MGR_UINT8  fl_len_format_U8;
#endif


#if(MODE_23_SUPPORTED_BASED_ON == APPL_SPECIFIC)
        /* Invoke product specific function handler to write */
        Prod_Spec_Read_Memory_Address_Handler(bufptr, dataLen);
        return;
#else
    /* dataLen = dataLen; */ /* To avoid warning */
    if( dataLen > DATA_LEN_2_BYTE)/* Requirement is that addr should be 2Bytes and length parameter should be 1Byte */
    {
        #if (MODE_23_SUPPORTED_BASED_ON == ISO_14229_BASED)
        if (((dataLen != MAX_LENGTH_SERVICE_23_1)&&(bufptr[0] == LENGTH_FORMAT_IDENTIFIER_1))||
            ((dataLen != MAX_LENGTH_SERVICE_23_2)&&(bufptr[0] == LENGTH_FORMAT_IDENTIFIER_2)))
        {
            /* incorrect length */
            bufptr[0] = kwp2k_diag_NrcInvalidFormat;
            KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
        }
        else
        {
            if ((bufptr[0] == LENGTH_FORMAT_IDENTIFIER_1) || (bufptr[0] == LENGTH_FORMAT_IDENTIFIER_2))
            {
                /* Extract length of the memory address parameter     */
                fl_len_format_U8  = (DG_MGR_UINT8)(bufptr[0]);
                fl_len_format_U8  = (DG_MGR_UINT8)( (fl_len_format_U8 >> 4) & ((DG_MGR_UINT8)0x0Fu) );
                /* Extract length of the memory size parameter        */
                fl_addr_format_U8 = (DG_MGR_UINT8)( bufptr[0] & ((DG_MGR_UINT8)0x0Fu) ) ;
                /* check if address parameter size is 2bytes and length parameter size is 1byte */
                if( (fl_addr_format_U8 <(UINT8)0x02)||(fl_addr_format_U8 >(UINT8)0x04)||
                    (fl_len_format_U8 <(UINT8)0x01)||(fl_len_format_U8 >(UINT8)0x02))
                {
                    bufptr[0] = kwp2k_diag_NrcRequestOutOfRange;
                    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
                }
                else
                {
                    kwp2k_core_process_Read_Memory_by_Address(fl_addr_format_U8, fl_len_format_U8, bufptr);
                }
            }
            else
            {
                /* Send negative response -> Invalid format */
                bufptr[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
            }
        }
        #elif (MODE_23_SUPPORTED_BASED_ON == KWP_BASED)
        if( dataLen != DATA_LEN_4_BYTE)
        {
            /* incorrect length */
            bufptr[0] = kwp2k_diag_NrcInvalidFormat;
            KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
        }
        else
        {
            fl_addr_format_U8 = (DG_MGR_UINT8)(0x03);
            fl_len_format_U8  = (DG_MGR_UINT8)(0x01);
            kwp2k_core_process_Read_Memory_by_Address(fl_addr_format_U8, fl_len_format_U8, bufptr);
        }
        #endif
    }
    else
    {
        /* incorrect length */
        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
    }
#endif
}

#else
void kwp2k_core_Diag_Read_Memory_by_Address(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 23 supported*/

/****************************************************************************

NAME:        kwp2k_core_process_Read_Memory_by_Address
             (mode23)

DESCRIPTION: This function checks if the DMR request is valid or not.

INPUTS:      DMR start address and total size of data to be read.

OUTPUTS:     NONE

RETURNS:     TRUE - if request is valid, else FALSE

****************************************************************************/
#ifdef MODE_23_SUPPORTED
#if (MODE_23_SUPPORTED_BASED_ON != APPL_SPECIFIC)
static void kwp2k_core_process_Read_Memory_by_Address(DG_MGR_UINT8 addr_format, DG_MGR_UINT8 len_format, DG_MGR_UINT8 *buffer)
{
    DG_MGR_UINT32 fl_dmr_addr_U32;
    DG_MGR_UINT16 fl_dmr_size_U16;
    DG_MGR_UINT8 *fl_resp_data_U8P;
    DG_MGR_UINT8  fl_count_U8;
    DG_MGR_UINT8  fl_data_cntr_U8;
    fl_dmr_addr_U32 = (DG_MGR_UINT32)0x00u;
    fl_dmr_size_U16 = (DG_MGR_UINT16)0x00u;

#if (MODE_23_SUPPORTED_BASED_ON == ISO_14229_BASED)
    fl_data_cntr_U8 = (DG_MGR_UINT8)0x01u; /* set to 1, because the address starts from the position bufptr[1] */
#else
    fl_data_cntr_U8 = (DG_MGR_UINT8)0x00u;
#endif
    /* Extract memory address from the request message   */
    for(fl_count_U8 = (DG_MGR_UINT8)0; fl_count_U8 < addr_format; fl_count_U8++)
    {
        fl_dmr_addr_U32 <<= (DG_MGR_UINT8)0x08u;
        fl_dmr_addr_U32 += buffer[fl_data_cntr_U8++];
    }
    /* Extract memory size from the request message      */
    for(fl_count_U8 = (DG_MGR_UINT8)0; fl_count_U8 < len_format; fl_count_U8++)
    {
        fl_dmr_size_U16 <<= (DG_MGR_UINT8)0x08u;
        fl_dmr_size_U16 += buffer[fl_data_cntr_U8++];
    }

    /* Check requested memory location is not protected  */
    /* Check DMR address is valid                        */
    /* Check DMR memory size has to be in the range      */
    if(FALSE != kwp2k_core_DMR_request_OK(fl_dmr_addr_U32,fl_dmr_size_U16))
    {
        /* assign memory starting address to response data pointer.   */
        fl_resp_data_U8P = (DG_MGR_UINT8 *)(fl_dmr_addr_U32);

        /* check if size is less than 90 bytes or more */
        if( fl_dmr_size_U16 <= NW_MAX_BUFF_LENGTH )
        {
            /* Invoke product specific function handler to write */
            Prod_Spec_Read_Memory_Address(fl_dmr_addr_U32, fl_dmr_size_U16, buffer);
            /*  KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE(bufptr,3); */
        }
        else
        {
            /* Either address range is invalid or size is invalid */
            buffer[0] = kwp2k_diag_NrcRequestOutOfRange;
            KWP2K_DIAG_SEND_ERROR_RESPONSE(buffer);
        }
    }
    else
    {
        /* Either address range is invalid or size is invalid */
        buffer[0] = kwp2k_diag_NrcRequestOutOfRange;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(buffer);
    }
}
#endif
#endif /* MODE_23_SUPPORTED */
/****************************************************************************

NAME:        kwp2k_core_DMR_request_OK
             (mode23)

DESCRIPTION: This function checks if the DMR request is valid or not.

INPUTS:      DMR start address and total size of data to be read.

OUTPUTS:     NONE

RETURNS:     TRUE - if request is valid, else FALSE

****************************************************************************/
#ifdef MODE_23_SUPPORTED
#if (MODE_23_SUPPORTED_BASED_ON != APPL_SPECIFIC)
static DG_MGR_BOOLEAN kwp2k_core_DMR_request_OK(DG_MGR_UINT32 p_addr_U32, DG_MGR_UINT16 p_num_bytes_U16)
{

    DMR_RANGES_RECORD const *fl_dmr_ranges_PS;
    DG_MGR_BOOLEAN fl_ret_val_B = TRUE;
    if((p_addr_U32 == (DG_MGR_UINT8)0) ||
      (p_addr_U32 > DMR_MAXIMUM_ADDRESS_READ)||
      ((p_addr_U32 + (DG_MGR_UINT32)(p_num_bytes_U16 - (DG_MGR_UINT8)1)) > DMR_MAXIMUM_ADDRESS_READ)||
      (p_num_bytes_U16 == (DG_MGR_UINT8)0)||
      ((p_addr_U32 == DMR_MAXIMUM_ADDRESS_READ)&&(p_num_bytes_U16 >= (DG_MGR_UINT8)2)))
    {
         fl_ret_val_B = FALSE;
    }
    else
    {

                                     /* Check to see if memory address requested is within the a protect range  */
         fl_dmr_ranges_PS = &diag_dmr_protect_ranges_rom[0];

                                     /* CYC:BEGIN_TEST_WHILE MAX_ITER = x EXP_ITER = y */
         while (fl_dmr_ranges_PS <
                &diag_dmr_protect_ranges_rom[sizeof(diag_dmr_protect_ranges_rom) / sizeof(DMR_RANGES_RECORD) - 1u])
                                     /* CYC:END_TEST_WHILE */
         {
            if( ((p_addr_U32 + (DG_MGR_UINT32)(p_num_bytes_U16 - (DG_MGR_UINT8)1)) >= (DG_MGR_UINT32)fl_dmr_ranges_PS->starting_address) &&
                (p_addr_U32 <= (DG_MGR_UINT32)fl_dmr_ranges_PS->ending_address) )
            {
                fl_ret_val_B = FALSE;
                break;
            }
            else
            {
                ++fl_dmr_ranges_PS;
            }
         }

     }

                                /* Return the address validity status */
    return(fl_ret_val_B);
}
#endif /* mode 0x23 */
#endif /* MODE_23_SUPPORTED */

/*
___Service_3D(){ }

*/

/****************************************************************************

NAME:        kwp2k_core_Diag_Write_Memory_by_Address
             (mode3D)

DESCRIPTION: This function processes the write memory by address command.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

****************************************************************************/
#ifdef MODE_3D_SUPPORTED
void kwp2k_core_Diag_Write_Memory_by_Address( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen )
{
#if (MODE_3D_SUPPORTED_BASED_ON != APPL_SPECIFIC)
    DG_MGR_UINT32  fl_dmw_addr_U32;
    DG_MGR_UINT16  fl_dmw_size_U16;
    DG_MGR_UINT8   fl_count_U8;
    DG_MGR_UINT8   fl_data_cntr_U8;
    DG_MGR_UINT8   fl_addr_format_U8;
    DG_MGR_UINT8   fl_len_format_U8;

    fl_dmw_addr_U32 = (DG_MGR_UINT32)0x00u;
    fl_dmw_size_U16 = (DG_MGR_UINT16)0x00u;
    fl_data_cntr_U8 = (DG_MGR_UINT8)0x01u; /* set to 1, because the address starts from the position bufptr[1] */
#endif

#if (MODE_3D_SUPPORTED_BASED_ON == APPL_SPECIFIC)
        /* Invoke product specific function handler to write */
        Prod_Spec_Write_Memory_Address_Handler(bufptr, dataLen);
        return;
#else
	dataLen = dataLen; /* To avoid compiler warning */
    if(DATA_LEN_4_BYTE < dataLen)     /* min length should be 5Bytes even for writing a single byte */
    {
        #if (MODE_3D_SUPPORTED_BASED_ON == ISO_14229_BASED)
        /* Extract length of the memory address parameter     */
        fl_len_format_U8  = (DG_MGR_UINT8)(bufptr[0]);
        fl_len_format_U8  = (DG_MGR_UINT8)( (fl_len_format_U8 >> 4) & ((DG_MGR_UINT8)0x0Fu) );
        /* Extract length of the memory size parameter        */
        fl_addr_format_U8 = (DG_MGR_UINT8)( bufptr[0] & ((DG_MGR_UINT8)0x0Fu) ) ;

        /* check if address parameter size is 2bytes and length parameter size is 1byte */
        if( (DATA_LEN_1_BYTE != fl_len_format_U8) || \
            (DATA_LEN_2_BYTE != fl_addr_format_U8) )
        {
            bufptr[0] = kwp2k_diag_NrcRequestOutOfRange;
            KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
        }
        else
        #elif (MODE_3D_SUPPORTED_BASED_ON == KWP_BASED)
		fl_data_cntr_U8 = (DG_MGR_UINT8)0x00u;
		fl_addr_format_U8 = (DG_MGR_UINT8)(0x03);
        fl_len_format_U8  = (DG_MGR_UINT8)(0x01);
		#endif 
		{
            /* Extract memory address from the request message   */
            for(fl_count_U8 = (DG_MGR_UINT8)0; fl_count_U8 < fl_addr_format_U8; fl_count_U8++)
            {
                fl_dmw_addr_U32 <<= (DG_MGR_UINT8)0x08u;
                fl_dmw_addr_U32 += bufptr[fl_data_cntr_U8++];
            }
            /* Extract memory size from the request message      */
            for(fl_count_U8 = (DG_MGR_UINT8)0; fl_count_U8 < fl_len_format_U8; fl_count_U8++)
            {
                fl_dmw_size_U16 <<= (DG_MGR_UINT8)0x08u;
                fl_dmw_size_U16 += bufptr[fl_data_cntr_U8++];
            }

            fl_count_U8 =  fl_addr_format_U8 + fl_len_format_U8;      /*MA,MS         */

            /* Check if the size of data provided is supplied along with the request */
            if(dataLen ==  (DG_MGR_UINT8)(fl_dmw_size_U16 + fl_count_U8)) /* subtract 4bytes from buffer length and check if the rest length equals data length */
            {
                /* Check requested memory location is not protected  */
                /* Check DMW address is valid                        */
                /* Check DMW memory size has to be in the range.     */
                if(FALSE !=kwp2k_core_DMW_request_OK(fl_dmw_addr_U32,fl_dmw_size_U16))
                {
                    /* Invoke product specific function handler to write */
					Prod_Spec_Write_Memory_Address((DG_MGR_UINT32)fl_dmw_addr_U32, fl_dmw_size_U16,
					&bufptr[fl_data_cntr_U8]);
					/*  KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE(bufptr,3); */
                }
                else
                {
                    /* Set negative response                             */
                    /* Either address range is invalid or size is invalid */
                    bufptr[0] = kwp2k_diag_NrcRequestOutOfRange;
                    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);

                }
            }
            else
            {
                /* The length of data supplied in the CAN request doesnot match with the lengh of the data to be written */
                /* so send NRC - 0x13 incorrect length */
                bufptr[0] = kwp2k_diag_NrcInvalidFormat;
                KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
            }
        }
    }
    else
    {
        /* incorrect length */
        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
    }
#endif
}

#else
void kwp2k_core_Diag_Write_Memory_by_Address(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 3D supported*/

/****************************************************************************

NAME:        kwp2k_core_DMR_request_OK
             (mode23)

DESCRIPTION: This function checks if the DMR request is valid or not.

INPUTS:      DMR start address and total size of data to be read.

OUTPUTS:     NONE

RETURNS:     TRUE - if request is valid, else FALSE

****************************************************************************/
#ifdef MODE_3D_SUPPORTED
static DG_MGR_BOOLEAN kwp2k_core_DMW_request_OK(DG_MGR_UINT32 p_addr_U32, DG_MGR_UINT16 p_num_bytes_U16)
{

    DMR_RANGES_RECORD const *fl_dmw_ranges_PS;
    DG_MGR_BOOLEAN fl_ret_val_B = TRUE;
    if((p_addr_U32 == (DG_MGR_UINT8)0) ||
      (p_addr_U32 > DMR_MAXIMUM_ADDRESS_WRITE)||
      ((p_addr_U32 + (DG_MGR_UINT32)(p_num_bytes_U16 - (DG_MGR_UINT8)1)) > DMR_MAXIMUM_ADDRESS_WRITE)||
      (p_num_bytes_U16 == (DG_MGR_UINT8)0)||
      ((p_addr_U32 == DMR_MAXIMUM_ADDRESS_WRITE)&&(p_num_bytes_U16 >= (DG_MGR_UINT8)2)))
    {
         fl_ret_val_B = FALSE;
    }
    else
    {
         /* Check to see if memory address requested is within the a protect range  */
         fl_dmw_ranges_PS = &diag_dmw_protect_ranges_rom[0];

                                     /* CYC:BEGIN_TEST_WHILE MAX_ITER = x EXP_ITER = y */
         while (fl_dmw_ranges_PS <
                &diag_dmw_protect_ranges_rom[sizeof(diag_dmw_protect_ranges_rom) / sizeof(DMR_RANGES_RECORD) - 1u])
                                     /* CYC:END_TEST_WHILE */
         {
            if( ((p_addr_U32 + (DG_MGR_UINT32)(p_num_bytes_U16 - (DG_MGR_UINT8)1)) >= (DG_MGR_UINT32)fl_dmw_ranges_PS->starting_address) &&
                (p_addr_U32 <= (DG_MGR_UINT32)fl_dmw_ranges_PS->ending_address) )
            {
                fl_ret_val_B = FALSE;
                break;
            }
            else
            {
                ++fl_dmw_ranges_PS;
            }
         }
     }
    /* Return the address validity status */
    return(fl_ret_val_B);
}
#endif /* MODE_3D_SUPPORTED */
/*
___Service_31(){ }

*/

/****************************************************************************

NAME:        kwp2k_core_Diag_Start_RoutineControl
             (mode31)

DESCRIPTION: This function processes the Routine Control Service.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

****************************************************************************/

#ifdef MODE_31_SUPPORTED
void kwp2k_core_Diag_Start_RoutineControl( DG_MGR_UINT8* bufptr, DG_MGR_UINT16 dataLen )
{
//	UINT8 sub_fnid = 0x00;
//	UINT16 routine_id = 0x00;

    if( dataLen != DATA_LEN_3_BYTE )
    {
        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
    }
	else
	{
		kwp2k_core_Diag_process_routine_control( &bufptr[0],(DG_MGR_UINT8)(dataLen - DATA_LEN_1_BYTE),bufptr[0]);
	}

	return;
}
#else
void kwp2k_core_Diag_Start_RoutineControl(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 31 supported*/

/*
___Service_32(){ }

*/

/****************************************************************************

NAME:        kwp2k_core_Diag_Stop_RoutineControl
             (mode32)

DESCRIPTION: This function processes the Routine Control Service.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

****************************************************************************/

#ifdef MODE_32_SUPPORTED
void kwp2k_core_Diag_Stop_RoutineControl( DG_MGR_UINT8* bufptr, DG_MGR_UINT16 dataLen )
{
    if( dataLen != DATA_LEN_1_BYTE )
    {
        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
    }
	else
	{
     	kwp2k_core_Diag_process_routine_control( bufptr,dataLen-DATA_LEN_1_BYTE,RC_STOP );
	}
}
#else
void kwp2k_core_Diag_Stop_RoutineControl(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 32 supported*/

/****************************************************************************

NAME:        kwp2k_core_Diag_process_routine_control
             (mode31)

DESCRIPTION: This function processes requests for routine control.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

****************************************************************************/
#ifdef MODE_31_SUPPORTED
static void kwp2k_core_Diag_process_routine_control(DG_MGR_UINT8* bufptr, DG_MGR_UINT8 dataLen, const DG_MGR_UINT8 req_type)
{
    RC_RECORD *fl_rc_table_ptr;
    DG_MGR_UINT16     fl_rc_id_U16;
    DG_MGR_BOOLEAN    fl_return_value_BOOL = FALSE;
    DG_MGR_UINT8      fl_data_len_U8 = 0;

    fl_rc_id_U16  = bufptr[1] ;

	fl_rc_id_U16 = fl_rc_id_U16 << 8;

	fl_rc_id_U16 |= bufptr[2];

    fl_rc_table_ptr = (RC_RECORD *)diag_routine_control_table_rom;

    l_rc_index_U8 = 0;

    if (fl_rc_table_ptr->rc_sync_async == ASYNC)
    {
        while( ( ZERO != fl_rc_table_ptr->rc_ID_U16 ) &&
                  ( FALSE == fl_return_value_BOOL ) )
        {
            if( fl_rc_table_ptr->rc_ID_U16 == fl_rc_id_U16 )
            {
				if(kwp2k_core_check_session(fl_rc_table_ptr->sup_session))
				{

	                if( FALSE != TESTBIT(fl_rc_table_ptr->rc_type_U8,req_type) )
	                {
	                    if( RC_START == req_type )
	                    {
	                        fl_data_len_U8 = fl_rc_table_ptr->rc_dataLen_U8;
	                        if(fl_rc_id_U16 == diag_routine_control_table_rom[0].rc_ID_U16/*0x02 0x03*/)
	                        {
								//if((/*battery abnormal*/IsBatteryNormal() == FALSE) || (/*ign off*/IsIgnitionHigh() == FALSE))
								{
									bufptr[0] = kwp2k_diag_NrcConditionsNotCorrect;
									KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
									fl_return_value_BOOL = TRUE;
								}
	                        }
	                        
							if (dataLen != fl_data_len_U8)
							{
			                    bufptr[0] = kwp2k_diag_NrcInvalidFormat;
			                    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
			                    fl_return_value_BOOL = TRUE;
							}
	                    }
	                    else
	                    {
	                        fl_data_len_U8 = 0;
	                    }

	                    if(FALSE != fl_return_value_BOOL )
	                    {
	                    }
	                    else
	                    {
	                        fl_return_value_BOOL = kwp2k_core_diag_process_routine_control_output(bufptr,fl_rc_table_ptr,req_type);
	                    }
	                }
	                else
	                {
	                    bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupported;
	                    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
	                    fl_return_value_BOOL = TRUE;
	                }
				}
				else
				{
					bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupportedInActiveSession;
					KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
					fl_return_value_BOOL = TRUE;
				}
            }
            else
            {
            }
            ++l_rc_index_U8;
            ++fl_rc_table_ptr;
        }
    }
    else
    {
        /* Do Nothing */
    }
    if( FALSE != fl_return_value_BOOL )
    {
     /* Do Nothing */
    }
    else
    {
        /*
        ** If we get here, the PID was not support.
        */
        bufptr[0] = kwp2k_diag_NrcRequestOutOfRange ;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
    }
}

/****************************************************************************

NAME:        kwp2k_core_diag_process_routine_control_output
             (mode31)

DESCRIPTION: This function processes requests for routine control.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

****************************************************************************/
DG_MGR_BOOLEAN kwp2k_core_diag_process_routine_control_output(DG_MGR_UINT8* bufptr,RC_RECORD *p_rc_table_ptr,const DG_MGR_UINT8 req_type)
{
    DG_MGR_UINT8   fl_rc_return_bytes_U8;
    DG_MGR_BOOLEAN fl_return_value_BOOL = FALSE;

    if( RC_START == req_type )
    {
        if( FALSE != TESTBIT(p_rc_table_ptr->rc_type_U8 ,RC_REQ_RESULT | RC_STOP) )
        {
            l_rc_status_flag[l_rc_index_U8] = RC_REQUESTED;
        }
        else
        {
        }
    }
    else if( RC_STOP == req_type )
    {
        if( RC_REQUESTED == l_rc_status_flag[l_rc_index_U8] )
        {
            if( FALSE == TESTBIT(p_rc_table_ptr->rc_type_U8,RC_REQ_RESULT) )
            {
                l_rc_status_flag[l_rc_index_U8] = 0;
            }
            else
            {
            }
        }
        else
        {
/*          bufptr[0] = kwp2k_diag_NrcConditionsNotCorrect;
          KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
          fl_return_value_BOOL = TRUE;
*/
        }
    }
    else if( RC_REQ_RESULT == req_type )
    {
        if( RC_REQUESTED == l_rc_status_flag[l_rc_index_U8] )
        {
        }
        else
        {
/*           bufptr[0] = kwp2k_diag_NrcConditionsNotCorrect;
            KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
            fl_return_value_BOOL = TRUE;
*/        }
    }
    else
    {
    }
    if(FALSE != fl_return_value_BOOL )
    {
    }
    else
    {
        fl_rc_return_bytes_U8 = p_rc_table_ptr->rc_function_ptr(&bufptr[1],req_type );
        if( (DG_MGR_UINT8)0xFFu == fl_rc_return_bytes_U8 )
        {
            if( RC_START == req_type )
            {
                l_rc_status_flag[l_rc_index_U8] = 0;
            }
            else
            {
            }
            KWP2K_DIAG_SEND_ERROR_RESPONSE(&bufptr[3]);
            fl_return_value_BOOL = TRUE;
        }
        else
        {
            if(RC_REQ_RESULT == req_type)
            {
            /*  l_rc_status_flag[l_rc_index_U8] = 0;*/
            }
            else
            {
            }
            KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , fl_rc_return_bytes_U8 + DATA_LEN_3_BYTE );
            fl_return_value_BOOL = TRUE;
        }
    }
    return fl_return_value_BOOL;
}

/****************************************************************************

NAME:        kwp2k_core_diag_routine_status
             (mode31)

DESCRIPTION: This function returns the routine status of whether RC_REQUESTED.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

****************************************************************************/

DG_MGR_UINT8 kwp2k_core_diag_routine_status(void)
{
    return(l_rc_status_flag[l_rc_index_U8]);

}
#endif
/*
___Service_85(){ }

*/

/****************************************************************************

NAME:        kwp2k_core_Diag_ControlDTCSetting
             (mode85)

DESCRIPTION: This function processes incoming control DTC setting message.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

****************************************************************************/
#ifdef MODE_85_SUPPORTED
void kwp2k_core_Diag_ControlDTCSetting( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen )
{

#if (MODE_85_SUPPORTED_BASED_ON == APPL_SPECIFIC)
	DG_MGR_UINT8	recv_resp;
#endif


	#if (MODE_85_SUPPORTED_BASED_ON != APPL_SPECIFIC)
	{
	    if( DATA_LEN_1_BYTE != dataLen )
	    {
	        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
	        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
	    }
	    else
	    {
	        if( kwp2k_core_check_session(PROD_SPEC_DENY_DTCTL_SUP_SESSION))
	        {
	            if( ENABLE_DTC_LOGGING == bufptr[0] )
	            {
	                CLEARBIT(diag_flags, NSCLMODE);
	                /* Enable the dtc setting in the application inorder to restart 5s debounce timer */
	                kwp2k_diag_fdtc_app_enable_dtc();
	                KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , DATA_LEN_1_BYTE );
	            }
	            else if( DISABLE_DTC_LOGGING == bufptr[0] )
	            {
	                /*
	                ** Vehicle conditions do not prohibit entering the communication control.
	                */
	                if(kwp2k_core_diag_trav_bool_void_lst(dtctl_deny_entry_list_rom) != ZERO)
	                {
	                    CLEARBIT(diag_flags, NSCLMODE);
	                    bufptr[0] = kwp2k_diag_NrcConditionsNotCorrect;
	                    KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
	                }
	                else
	                {
	                    SETBIT(diag_flags, NSCLMODE);
	                    /* Disable the dtc setting in the application inorder to reset 5s debounce timer */
	                    kwp2k_diag_fdtc_app_disable_dtc();
	                    KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , DATA_LEN_1_BYTE );
	                }
	            }
				else if( ENABLE_DTC_LOGGING_NO_REP == bufptr[0] )
	            {
	                CLEARBIT(diag_flags, NSCLMODE);
	                /* Enable the dtc setting in the application inorder to restart 5s debounce timer */
	                kwp2k_diag_fdtc_app_enable_dtc();
	                KWP2K_DIAG_SEND_NO_RESPONSE( bufptr , dataLen );
	            }
				else if( DISABLE_DTC_LOGGING_NO_REP == bufptr[0] )
	            {
	                /*
	                ** Vehicle conditions do not prohibit entering the communication control.
	                */
	                if(kwp2k_core_diag_trav_bool_void_lst(dtctl_deny_entry_list_rom) != ZERO)
	                {
	                    CLEARBIT(diag_flags, NSCLMODE);
	                    bufptr[0] = kwp2k_diag_NrcConditionsNotCorrect;
	                    KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
	                }
	                else
	                {
	                    SETBIT(diag_flags, NSCLMODE);
	                    /* Disable the dtc setting in the application inorder to reset 5s debounce timer */
	                    kwp2k_diag_fdtc_app_disable_dtc();
	                    KWP2K_DIAG_SEND_NO_RESPONSE( bufptr , dataLen );
	                }
	            }
	            else
	            {
	                bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupported;
	                KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
	            }
	        }
	        else
	        {
	            bufptr[0] = kwp2k_diag_NrcServiceNotSupportedInActiveSession ;
	            KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
	        }
	    }
	}
	#else /* #if (MODE_85_SUPPORTED_BASED_ON == APPL_SPECIFIC)*/
	{
		if( kwp2k_core_check_session(PROD_SPEC_DENY_DTCTL_SUP_SESSION))
	    {
			recv_resp = Prod_Spec_appl_specific_ctrl_dtc_format(&bufptr[0],dataLen,&ctrl_dtc_resp_len,&ctrl_state);

			if (recv_resp == DIAG_NEGATIVE_RESPONSE)
			{
				bufptr[0] = kwp2k_diag_NrcInvalidFormat;
				KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );  
			}
			else 
			{
				if( ENABLE_DTC_LOGGING == ctrl_state )
				{
					CLEARBIT(diag_flags, NSCLMODE);
					/* Enable the dtc setting in the application inorder to restart 5s debounce timer */
					kwp2k_diag_fdtc_app_enable_dtc();
					if (recv_resp == DIAG_POSITIVE_RESPONSE)
					{
					  KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , ctrl_dtc_resp_len );
					}
					else
					{
					   KWP2K_DIAG_SUPPRESS_RESPONSE();	
					}
				}
				else if( DISABLE_DTC_LOGGING == ctrl_state )
				{
					/*
					** Vehicle conditions do not prohibit entering the communication control.
					*/
					if(kwp2k_core_diag_trav_bool_void_lst(dtctl_deny_entry_list_rom) != ZERO)
					{
					    CLEARBIT(diag_flags, NSCLMODE);
					    bufptr[0] = kwp2k_diag_NrcConditionsNotCorrect;
					    KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
					}
					else
					{
					    SETBIT(diag_flags, NSCLMODE);
					    /* Disable the dtc setting in the application inorder to reset 5s debounce timer */
					    kwp2k_diag_fdtc_app_disable_dtc();
						if (recv_resp == DIAG_POSITIVE_RESPONSE)
						{
						    KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , ctrl_dtc_resp_len );
						}
						else
						{
						   KWP2K_DIAG_SUPPRESS_RESPONSE();	
						}
					}
				}
	            else
	            {
	                bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupported;
	                KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
	            }
			}
		}
		else
		{
		    bufptr[0] = kwp2k_diag_NrcServiceNotSupportedInActiveSession ;
		    KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
		}
	}
	#endif
}

#else
void kwp2k_core_Diag_ControlDTCSetting(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif /*MODE 85 supported*/

unsigned char inspect_dtc_logging(void)
{
	return(Enable_dtc_logging);
}

/*
___Service_28(){ }

*/
/****************************************************************************
	
	NAME:		 kwp2k_core_Diag_ControlCommunicatin
				 (mode28)
	
	DESCRIPTION: This function is realized by lhu4 for E301.
	
	INPUTS: 	 NONE.
	
	OUTPUTS:	 NONE.
	
	RETURNS:	 NONE.
	
****************************************************************************/
#ifdef MODE_28_SUPPORTED
void kwp2k_core_Diag_ControlCommunicationControl( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen )
{

	if ( DATA_LEN_2_BYTE != dataLen )
    {
          bufptr[0] = kwp2k_diag_NrcInvalidFormat;
          KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
		  return;
    }
	switch(bufptr[0]&(~NORESPONSE_MASK))
	{
		
	case ENRX_ENTX:
		    if(bufptr[1] == APP_MSG)
			{
				IlResume();
				dll_il_rx_resume();	
				if((bufptr[0]&NORESPONSE_MASK) == 0 )
				{
				    KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr, dataLen-1 );
				}
				else
				{
					KWP2K_DIAG_SEND_NO_RESPONSE( bufptr , dataLen );
				}	
			}
			else if(bufptr[1] == APP_NM_MSG)
			{
				IlResume();
				dll_il_rx_resume();
				if((bufptr[0]&NORESPONSE_MASK) == 0 )
				{
				    KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr, dataLen-1 );
				}
				else
				{
					KWP2K_DIAG_SEND_NO_RESPONSE( bufptr , dataLen );
				}	
			}
			else
			{
				bufptr[0] = kwp2k_diag_NrcCommCtrlRequestOutOfRange;
            	KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
			}
		break;    
	case DISENRX_DISENTX:		
			if(bufptr[1] == APP_MSG)
			{
			    IlSuspend();
				dll_il_rx_suspend();
				if((bufptr[0]&NORESPONSE_MASK) == 0 )
				{
				    KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr, dataLen-1 );
				}
				else
				{
					KWP2K_DIAG_SEND_NO_RESPONSE( bufptr , dataLen );
				}	
			}
			else if(bufptr[1] == APP_NM_MSG)
			{
				IlSuspend();
				dll_il_rx_suspend();
				if((bufptr[0]&NORESPONSE_MASK) == 0 )
				{
				    KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr, dataLen-1 );
				}
				else
				{
					KWP2K_DIAG_SEND_NO_RESPONSE( bufptr , dataLen );
			    }            
		    }
    		else
    		{
    			bufptr[0] = kwp2k_diag_NrcCommCtrlRequestOutOfRange;
    			KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
    		}

		break;
	
	default:
		/* DO Nothing */
		/*Send Error response*/
        if(get_diag_addressing() != FALSE)
        {
    		bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupported;
    		KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
        }
        else
        {           
            KWP2K_DIAG_SEND_NO_RESPONSE( bufptr , dataLen );
        }
		break;	
	}		
}


void kwp2k_core_Diag_28_standby(void)
{
	if(requested_session==0)/*default session*/
	{			
		Network_TxResume();
//		IlResume();
		dll_il_rx_resume();
	}
 //    IlResume();
}

void kwp2k_core_Diag_85_standby(void)
{	
	if(requested_session==0)/*default session*/
	{
		CLEARBIT(diag_flags, NSCLMODE);
		kwp2k_diag_fdtc_app_enable_dtc();
	}
}
#endif
/****************************************************************************

NAME:        kwp2k_core_Diag_CommunicatinControl
             (mode28)

DESCRIPTION: This function set the corresponding flags for enable or disable
             transmission of Appl or Network Communication Messages.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

****************************************************************************/
#ifdef MODE_28_SUPPORTED
void kwp2k_core_Diag_CommunicatinControl( DG_MGR_UINT8 Comm_type )
{
    if( (ZERO != Comm_type) &&
        (Comm_type <= DATA_LEN_3_BYTE) )
    {
      /*Do Nothing*/
    }
    else
    {
        KWP2K_DIAG_SEND_ERROR_RESPONSE(kwp2k_diag_NrcRequestOutOfRange);
    }
}
#else
void kwp2k_core_Diag_CommunicatinControl( DG_MGR_UINT8 Comm_type )
{
/*    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
     KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
*/
	Comm_type = Comm_type;

}
#endif

/*
___Service_27(){ }

*/

/****************************************************************************

NAME:        kwp2k_core_Diag_SecurityAccess
             (mode27)

DESCRIPTION: 
             

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

****************************************************************************/
#ifdef MODE_27_SUPPORTED
void kwp2k_core_Diag_SecurityAccess(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	if (dataLen == 0x00)
	{
        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
	}
	else if( bufptr[ZERO]==VM_SEND_KEY)
	{
		/*Key Request*/
		kwp2k_core_Diag_Send_key_Request(bufptr, dataLen);
	}
	else if(bufptr[ZERO]==VM_REQUEST_SEED)
	{
		/*Seed request*/
		kwp2k_core_Diag_Seed_Request(bufptr, dataLen);
	}
	else
	{
		bufptr[0] = kwp2k_diag_NrcSubfunctionNotSupported;
    	KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
	}
}
#else
void kwp2k_core_Diag_SecurityAccess(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
	dataLen = dataLen;
    bufptr[0] = kwp2k_diag_NrcServiceNotSupported;
    KWP2K_DIAG_SEND_ERROR_RESPONSE(bufptr);
}
#endif

#ifdef MODE_27_SUPPORTED
/****************************************************************************

NAME:        

DESCRIPTION: 
             
INPUTS:      

OUTPUTS:     

RETURNS:     

NOTE:

CHANGE HISTORY:    Changed by lhu4 for E301

Date          By       Reason For Change
--------     -------   -------------------------------------------------------


****************************************************************************/
static void kwp2k_core_Diag_Seed_Request(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
  SECURITY_ACCESS_STRUCT  *table_ptr;
  DG_MGR_UINT8 temp_response, data_length, i, loop_size;
  DG_MGR_BOOLEAN	fl_return_value_BOOL = FALSE;

    if(dataLen != (DG_MGR_UINT8)(0x01) )
    {
		fl_return_value_BOOL = TRUE;
        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
    }
    else
    {
			table_ptr = (SECURITY_ACCESS_STRUCT *) security_access_list;
        	loop_size = sizeof(security_access_list)/sizeof(SECURITY_ACCESS_STRUCT);

           /* 
           ** Call the application function to get the seed. 
           */
    		for(i=0x01;( i < loop_size) && ( FALSE == fl_return_value_BOOL); i++)
			{
        		if( table_ptr->seed_value == bufptr[0] )
				{	
					
/*****************************************Add by lhu4 for E301***********************/
					if (l_security_status_U8 == TRUE)
					{
#if 0
						/*Check whether request for seed is already sent*/
						if (ser_27_seed_flag == table_ptr->seed_value)
						{
							/*seed was sent*/
							bufptr[0] = kwp2k_diag_NrcServiceRequiredTimeDelayNotExpired;
				        	KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
							fl_return_value_BOOL = TRUE;							
						}
						else
#endif							
						{
							/*If locked by error key times>3,should delay 10s*/
							//if ((l_security_next_seed_timer_U8 == 0)&&(l_security_key_counter_U8 == 0))
                            if ((l_security_key_counter_U8 == 0))
							{
								/*Sent new seed*/
		            			temp_response = (*table_ptr->get_seed_function_ptr)(&bufptr[DATA_LEN_1_BYTE]);
				    			data_length = table_ptr->seed_length;
								/* Save the seed request to check for NRC 0x24 */
								ser_27_seed_flag = table_ptr->seed_value;
								//l_security_next_seed_timer_U8 = 20;/*20*512ms=10s*/
								
		            			if(temp_response == (UINT8) 0)
		            			{
			           				KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , (data_length + 1) );
					   				fl_return_value_BOOL = TRUE;
		            			}
		            			else
		            			{
				        			bufptr[0] = temp_response;
				        			KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
									fl_return_value_BOOL = TRUE;
		            			}								
							}
							else
							{
								/*10s time is still running, lock still*/
								bufptr[0] = kwp2k_diag_NrcServiceRequiredTimeDelayNotExpired;
				        		KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
								fl_return_value_BOOL = TRUE;								
							}
						}
					}
					else
					{
						/*bufptr[0] = kwp2k_diag_NrcServiceRequiredTimeDelayNotExpired;
				        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );*/

				        data_length = 1;
				        bufptr[1]=0x00;
						bufptr[2]=0x00;
						bufptr[3]=0x00;
						bufptr[4]=0x00;
						KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , data_length+4 );
						fl_return_value_BOOL = TRUE;
					}				
/***************************End for adding by lhu4 for E301*************************/

				}
				table_ptr++;
			}
    }
    if( FALSE != fl_return_value_BOOL )
    {
      /* Do Nothing */
    }
    else
    {
      /*
      ** If we get here, the PID was not support.
      */
        bufptr[0] = kwp2k_diag_NrcSecurityRequestOutOfRange ;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
    }
}


/****************************************************************************

NAME:        

DESCRIPTION: 
             
             

INPUTS:      

OUTPUTS:     

RETURNS:     

NOTE:

CHANGE HISTORY:   Changed by lhu4 for E301

Date          By       Reason For Change
--------     --------  -------------------------------------------------------



****************************************************************************/
static void kwp2k_core_Diag_Send_key_Request(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen)
{
    SECURITY_ACCESS_STRUCT  *table_ptr;
    DG_MGR_UINT8 temp_response, i, loop_size;
    DG_MGR_BOOLEAN  fl_return_value_BOOL = FALSE;

    table_ptr = (SECURITY_ACCESS_STRUCT *) security_access_list;
    loop_size = sizeof(security_access_list)/sizeof(SECURITY_ACCESS_STRUCT);

   /* 
   ** Call the application function to get the seed. 
   */
    for(i=0x01;( i < loop_size) && ( FALSE == fl_return_value_BOOL); i++)
	{
   		if( table_ptr->key_value == bufptr[0] )
		{
		    if((table_ptr->key_length) != (dataLen-1) )
		    {
		        bufptr[0] = kwp2k_diag_NrcInvalidFormat;
		        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
				fl_return_value_BOOL = TRUE;
		    }
			
			/* Check whether request for seed is already received */
			/* If seed request is not received earlier send NRC 0x24 */
			else if (table_ptr->seed_value == ser_27_seed_flag)
			{			
	            temp_response = (*table_ptr->verify_key_function_ptr)(&bufptr[DATA_LEN_1_BYTE]);

	            #if 0
	            
				bufptr[1] = l_requested_seed_U8[0]; /* For Refine 2 */
				bufptr[2] = l_requested_seed_U8[1];
				bufptr[3] = l_requested_seed_U8[2];
				bufptr[4] = l_requested_seed_U8[3];

				#endif

				
	            if(temp_response == (UINT8) 0)
	            {
					l_security_status_U8 = FALSE;
					l_security_unlock_error_counter_U8 = 0;
					ser_27_seed_flag = 0x00;
					KWP2K_DIAG_SEND_IMMEDIATE_RESPONSE( bufptr , 1 );
					fl_return_value_BOOL = TRUE;
	            }
	            else
	            {
					l_security_unlock_error_counter_U8++;
					if (l_security_unlock_error_counter_U8 >= 3)
					{
						ser_27_seed_flag = 0x00;
						l_security_unlock_error_counter_U8 = 0;
						l_security_key_counter_U8 = 20;/* 20*512ms=10S */
						bufptr[0] = kwp2k_diag_NrcRequestExceededNumberOfAttempts;
						KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
						fl_return_value_BOOL = TRUE;
					}
					else
					{
						bufptr[0] = kwp2k_diag_NrcRequestInvalidKey;
						KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
						fl_return_value_BOOL = TRUE;
					}

	            }			
			}
			else
			{
		        bufptr[0] = kwp2k_diag_NrcRequestSequenceError;
		        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
				l_security_status_U8 = TRUE;
				fl_return_value_BOOL = TRUE;
			}
		}
		table_ptr++;
	}
    if( FALSE != fl_return_value_BOOL )
    {
      /* Do Nothing */
    }
    else
    {
      /*
      ** If we get here, the PID was not support.
      */
        bufptr[0] = kwp2k_diag_NrcSecurityRequestOutOfRange ;
        KWP2K_DIAG_SEND_ERROR_RESPONSE( bufptr );
    }
}

void kwp2k_core_reset_security_status(void)
{
	ser_27_seed_flag = 0x00;
}

void kwp2k_core_Diag_27_standby(void)
{
	ser_27_seed_flag = 0x00;
	l_security_status_U8 = TRUE;
}

UINT8 kwp2k_core_Diag_27_get_security_status(void)
{
	return (l_security_status_U8);
}

/****************************************************************************

NAME:        kwp2k_core_Diag_SecurityAccess_timetask
             (mode27)

DESCRIPTION: 512ms task
             

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

****************************************************************************/
void kwp2k_core_Diag_SecurityAccess_timetask(void)
{

	if(l_security_next_seed_timer_U8 != 0)
	{
		l_security_next_seed_timer_U8--;
	}

	if (l_security_key_counter_U8 != 0)
	{
		l_security_key_counter_U8--;
	}
	
}

#endif


/*
___VNA_RING_BUFFER_AVAILABLE(){ }

*/

/****************************************************************************

NAME:        transmitt_ring_buffer

DESCRIPTION:

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     0 - The ECU can log the DTC
             1 - The ECU does'nt doesn't log anything.

****************************************************************************/

void kwp2k_core_Diag_transmitt_ring_buffer(void)
{
    l_nextBuffLength_U8 = NW_MAX_BUFF_LENGTH;
    if( l_bytesLeftToSend_U16 < (DG_MGR_UINT16)l_nextBuffLength_U8 )
    {
        l_nextBuffLength_U8 = (DG_MGR_UINT8)l_bytesLeftToSend_U16;
    }
    else
    {
    }
    //KWP2K_DIAG_SEND_RING_BUFFER_DATA(&response_buffer_U8A[l_responseCurrIndex_U16], l_nextBuffLength_U8);
    l_responseCurrIndex_U16 += (DG_MGR_UINT16)l_nextBuffLength_U8;
    l_bytesLeftToSend_U16 -= (DG_MGR_UINT16)l_nextBuffLength_U8;
}


/*
___Util_to_traverse_list(){ }

*/
/********************************************************************************
**
** NAME:        kwp2k_core_diag_trav_bool_void_lst
**
** DESCRIPTION: This function traverses and calls a list of NULL_PTR terminated
**              boolean/void functions (functions which take no parameters and
**              return a boolean value).
**
** INPUTS:      list_ptr - pointer to the beginning of the function list.
**
** OUTPUTS:     NONE.
**
** RETURNS:     A value of zero if ALL functions in the list returned FALSE,
**              or a non-zero value if ANY function in the list returned TRUE.
**
********************************************************************************/
static DG_MGR_UINT8 kwp2k_core_diag_trav_bool_void_lst(DG_MGR_BOOLEAN (*const (*list_ptr))(void))
{
    DG_MGR_UINT8 fl_ret_U8 = 0;

    /*
    ** Check for RAM error.
    */
    if(NULL_PTR == list_ptr)
    {
//        KWP2K_DIAG_REPORT_ERROR(KWP2K_DIAG_RAM_ERROR);
    }
    else
    {
        /* CYC:BEGIN_TEST_WHILE MAX_ITER = x EXP_ITER = y */
        while((ZERO != *list_ptr) && (ZERO == fl_ret_U8))
        /* CYC:END_TEST_WHILE */
        {
            if(FALSE != ((**list_ptr)()))
            {
                fl_ret_U8 = 1;
            }
            ++list_ptr;
        }
    }
    return (fl_ret_U8);
}

/***************************************************************************/
/* kwp2k_core_diag_trav_void_void_lst():                                             */
/*-------------------------------------------------------------------------*/
/* DESCRIPTION:                                                            */
/*    Calls every function in a list of ROM'd function pointers,           */
/*    terminated by NULL.  Each function in the list must be of type       */
/*    which takes no arguments and returns no values.                      */
/*                                                                         */
/* INPUT:                                                                  */
/*    top    :   Pointer to top of list.  List may be of zero length       */
/*               (first element NULL).                                     */
/*                                                                         */
/* OUTPUT:                                                                 */
/*    None.                                                                */
/*                                                                         */
/* RETURNS:                                                                */
/*    None.                                                                */
/*                                                                         */
/* CHANGE HISTORY:                                                         */
/*                                                                         */
/* Date       By     Reason For Change                                     */
/* ----       --     ------------------------------------------------------*/
/* May/15/98  GEP    First verion                                          */
/* Jul/07/98  GEP    Added if statement to check in top is NULL (required  */
/*                   to eliminate lint warning).                           */
/* Sep/03/02  GEP    Minor adjustments to eliminate QAC messages.          */
/*                                                                         */
/***************************************************************************/
void kwp2k_core_diag_trav_void_void_lst(void (*const (*top))(void))
{
    if (top != ((void *) 0))
    {
        while(*top != ((void (*)(void)) 0) )  /* While not NULL entry.                   */
        {
             (**top)();      /* Call function.                          */
             top++;          /* Advance pointer.                        */
        }
    }
    else
    {
        /*NULL PTR*/
//        KWP2K_DIAG_REPORT_ERROR(KWP2K_DIAG_RAM_ERROR);
    }
}


/*
___Core_Inspect_Functions(){ }

*/

/********************************************************************************
**
** NAME:        kwp2k_core_inspect_diagnostics_session
**
** DESCRIPTION: This function returns the active diagnostics session.
**
** INPUTS:      NONE
**
** OUTPUTS:     NONE.
**
** RETURNS:     Current Diagnostics Session.
**
********************************************************************************/

DG_MGR_UINT8 kwp2k_core_inspect_diagnostics_session(void)
{
    DG_MGR_UINT8 fl_return_value = 0;
#ifdef MODE_10_SUPPORTED
	fl_return_value = diag_session_table_rom[cur_session].session_index;
#endif
    return fl_return_value;
}
/********************************************************************************
**
** NAME:        kwp2k_core_inspect_diag_flag
**
** DESCRIPTION: This function returns the status of diag flag
**
** INPUTS:      NONE
**
** OUTPUTS:     NONE.
**
** RETURNS:     Status of diag flag.
**
********************************************************************************/

DG_MGR_UINT8 kwp2k_core_inspect_diag_flag(void)
{
    return (diag_flags);
}

/****************************************************************************

NAME:        read_NSCL_mode_status

DESCRIPTION: Reads the status of NSCLMODE bit in diag_flags and returns
             TRUE or FALSE

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     0 - The ECU can log the DTC
             1 - The ECU does'nt doesn't log anything.

****************************************************************************/

#ifdef ERROR_HANDLER_SUPPORTED
DG_MGR_BOOLEAN kwp2k_core_inspect_NSCL_mode_status(void)
{
    return (FALSE != TESTBIT(diag_flags, NSCLMODE) );
}
#endif

DG_MGR_UINT8 get_diag_cur_session(void)
{
    return cur_session;
}

DG_MGR_UINT8 get_diag_prev_session(void)
{
    return prev_session;
}

//extern const UINT8 NVM_DTC_Set_Ctrl_0120_U8[24];
BOOLEAN kwp2k_core_Diag_Get_DTC_cfg_status(UINT8 DTC_Index)
{
	DG_MGR_UINT16 fl_temp_index = 0;
	DG_MGR_UINT8 fl_temp_cfg_sts = FALSE;
	fl_temp_cfg_sts = TRUE;

	return fl_temp_cfg_sts;
}


void kwp2k_core_Diag_Set_Session_Req(UINT8 Session_Req)
{
	if(Session_Req <= 2)
	{
		requested_session = Session_Req;
	}
}
/*End of File*/



