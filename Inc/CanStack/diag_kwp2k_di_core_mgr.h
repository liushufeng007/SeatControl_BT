#if !defined(DIAG_KWP2K_DI_CORE_MGR_H)
#define DIAG_KWP2K_DI_CORE_MGR_H
/*============================================================================
** I N C L U D E   F I L E S
**==========================================================================*/

#ifndef EXTERN
#define EXTERN extern
#endif
/* moved from .c to .h file */
#define NULL_PTR                                   ((void *) 0)

typedef unsigned char  DG_MGR_UINT8;
typedef unsigned short DG_MGR_UINT16;
typedef unsigned long  DG_MGR_UINT32;
typedef	unsigned char  DG_MGR_BOOLEAN;

typedef enum
{
 CORE_HANDLE,
 APPL_HANDLE
}RESPONSE_HANDLER;
/*
** Bit definitions for the diag_flags data store.
*/
/*
** Diagnostic session defines.
*/

#define NO_SESSIONCHECK 	0x00
#define DG_DEFAULT 			0x01	
#define DG_EXTENDED			0x03	
#define DG_PROGRAMMING		0x02	
#define DG_SESS_1			0x08	
#define DG_SESS_2			0x10	
#define DG_SESS_3			0x20	
#define DG_SESS_4			0x40	
#define DG_SESS_5    		0x80

#define  DIAG_DEFAULT_SESSION                 (DG_DEFAULT)
#define  DIAG_PROGRAM_SESSION                 (DG_PROGRAMMING)
#define  DIAG_EXTENDED_SESSION                (DG_EXTENDED)

#define ISO_14229_BASED     0x01
#define KWP_BASED           0x02
#define APPL_SPECIFIC       0x03

EXTERN void kwp2k_core_Diag_CheckSessionTransition(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen);
EXTERN void kwp2k_core_Diag_OnTransitionSession(void);
EXTERN void kwp2k_core_Diag_ECU_reset(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen);
EXTERN void kwp2k_core_Diag_ecu_resp_confirmed(void);
EXTERN void kwp2k_core_Diag_read_dtc_info_13(DG_MGR_UINT8 * pMsgContext,DG_MGR_UINT16 dataLen);
EXTERN void kwp2k_core_Diag_read_dtc_info(DG_MGR_UINT8 * pMsgContext,DG_MGR_UINT16 dataLen);
EXTERN void kwp2k_core_Diag_read_data_msgid(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen);
EXTERN void kwp2k_core_Diag_write_data_msgid( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen );
EXTERN void kwp2k_core_Diag_IOControlByDataIdentifier( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen );
EXTERN void kwp2k_core_Diag_Read_Memory_by_Address( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen );
EXTERN void kwp2k_core_Diag_Write_Memory_by_Address( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen );
EXTERN void kwp2k_core_Diag_ControlCommunicationControl( DG_MGR_UINT8 *bufptr,DG_MGR_UINT16 dataLen );
EXTERN void kwp2k_core_Diag_RoutineControl( DG_MGR_UINT8* bufptr, DG_MGR_UINT16 dataLen );
EXTERN DG_MGR_UINT8 kwp2k_core_diag_routine_status(void);
EXTERN void kwp2k_core_Diag_ControlDTCSetting( DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen );
EXTERN unsigned char inspect_dtc_logging(void);
EXTERN void DiagMain(void);
EXTERN void DiagColdInit(void);
EXTERN void DiagWarmInit(void);
EXTERN DG_MGR_UINT8 kwp2k_core_inspect_diagnostics_session(void);
EXTERN DG_MGR_BOOLEAN kwp2k_core_inspect_NSCL_mode_status(void);
EXTERN void kwp2k_core_clear_NSCL_mode(void);
EXTERN DG_MGR_UINT8 kwp2k_core_inspect_diag_flag(void);
EXTERN void DiagMode14CallBack(DG_MGR_UINT8 erase_status);
EXTERN void kwp2k_core_Diag_read_status_of_dtc(DG_MGR_UINT8 * pMsgContext,DG_MGR_UINT16 dataLen);
EXTERN void kwp2k_core_Diag_read_dtc_by_status(DG_MGR_UINT8 * pMsgContext,DG_MGR_UINT16 dataLen);
EXTERN void kwp2k_core_Diag_ClearDTCs_KWP(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen);
EXTERN void kwp2k_core_Diag_ClearDTCs_UDS(DG_MGR_UINT8* pMsgContext, DG_MGR_UINT16 dataLen);
EXTERN void kwp2k_core_Diag_SecurityAccess(DG_MGR_UINT8 *bufptr, DG_MGR_UINT16 dataLen);
EXTERN void kwp2k_core_reset_security_status(void);

EXTERN void kwp2k_core_Diag_2F_standby(void);

EXTERN void kwp2k_core_Diag_27_standby(void);

EXTERN void kwp2k_core_Diag_28_standby(void);

EXTERN void kwp2k_core_Diag_85_standby(void);

EXTERN void kwp2k_core_Diag_SecurityAccess_timetask(void);
EXTERN void fetm_read_DTC_by_status_mask(DG_MGR_UINT8 * pMsgContext);
/*EXTERN UINT8 fetm_request_dtc_number(void);*/
EXTERN DG_MGR_UINT8 get_diag_cur_session(void);
EXTERN DG_MGR_UINT8 get_diag_prev_session(void);
EXTERN BOOLEAN kwp2k_core_Diag_Get_DTC_cfg_status(UINT8 DTC_Index);
EXTERN void kwp2k_core_Diag_Set_Session_Req(UINT8 Session_Req);



#undef EXTERN
#endif /* DIAG_KWP2K_DI_CORE_MGR_H */

/*---------------------------------------------------------------------------*
* Date              : 			                                             *
* By                : gswamina                                               *
* Traceability      : 							                             *               
* Change Description: 														 *
*----------------------------------------------------------------------------*/

/* End of file ============================================================ */

