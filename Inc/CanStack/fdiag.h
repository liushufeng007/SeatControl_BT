#ifndef _DIAG_H
#define _DIAG_H

#include "system.h"
#include "diag_kwp2k_di_core_erlg.h"

#define EXTERN extern

/*The size need to define  */
#define IOCTL_WPC_SIZE         1 
#define IOCTL_BT_SIZE          1 

EXTERN UINT8 IOCTL_WPC_buffer[IOCTL_WPC_SIZE];
EXTERN UINT8 IOCTL_BT_buffer[IOCTL_BT_SIZE];

EXTERN UINT8 fdiag_3700_KSIOCTL_enabled_flag;
EXTERN UINT8 fdiag_3701_KSIOCTL_enabled_flag;
/*
Extern functions
*/
EXTERN	void Prod_Spec_Mode10_02_Handler(void);
EXTERN BOOLEAN fdiag_diag_entry_check(void);
EXTERN BOOLEAN fdiag_diag_exit_session_check(void);
EXTERN void ApplDiagServiceTimeout(void);
EXTERN void ApplDiagSessionTimeout(void);
EXTERN void fdiag_extended_epilog(void);
EXTERN void fdiag_extended_prolog(void);
EXTERN void fdiag_spec_read_memory_address(UINT32 p_dmr_addr_U32, UINT16 p_dmr_size_U16,UINT8 *p_dmr_buffer);
EXTERN void fdiag_diag_go_default_seasion(void);



/* CAN fault Management */
EXTERN void fdiag_Clear_Ntwrk_Err_Cntrs(void);
EXTERN void fdiag_KSRRobin(void);
EXTERN void fdiag_KSColdinit(void);
EXTERN void fdiag_KSWarminit(void);
EXTERN void fdiag_KSWakeup(void);
EXTERN void fdiag_Transinto_Keyoff(void);
EXTERN UINT16 fget_bat_vol_values(void);
EXTERN void fdiag_TimedTask(void);
EXTERN void fdiag_Robin(void);


/*$27*/
EXTERN UINT8 fdiag_get_seed(UINT8 *buffer);
EXTERN UINT8 fdiag_verify_key(UINT8 *buffer);

/*$22*/
EXTERN UINT8 fdiag_app_1701_get_motor_PosCnt(UINT8 *buffer);
EXTERN UINT8 fdiag_app_1702_get_motor_SoftPosCnt(UINT8 *buffer);
EXTERN UINT8 fdiag_app_1706_get_motor_learn_sts(UINT8 *buffer);
EXTERN UINT8 fdiag_app_170B_get_BAT_Voltage(UINT8 *buffer);
EXTERN UINT8 fdiag_app_1707_get_seat_motor_current(UINT8 *buffer);
EXTERN UINT8 fdiag_app_1708_get_key_sts(UINT8 *buffer);
EXTERN UINT8 fdiag_app_1709_get_motor_sts(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f183_get_boot_SW_version(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f186_get_current_session(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f187_get_OEM_Partnumber(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f188_get_Software_Number(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f189_get_SoftwareVersion(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f18a_get_SupplyID(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f18b_get_ecu_manufacture_date(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f18c_get_serial_number(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f190_get_vin_data(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f191_get_hw_ver_number_data(UINT8 *buffer);;
EXTERN UINT8 fdiag_app_f193_get_ecu_HW_number(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f195_get_ecu_SW_number(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f197_get_ECU_Name(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f199_get_program_date(UINT8 *buffer);
EXTERN UINT8 fdiag_app_f19d_get_vehicle_manu_date(UINT8 *buffer);


/*$2E*/
EXTERN UINT8 fdiag_app_0100_set_cfg_information(UINT8 *buffer);



/*DTC SID 0x19*/
EXTERN UINT8 can_bus_off_count(void);
EXTERN BOOLEAN can_init_diag_is_fault(void);

/* IO SID 0x2F */
EXTERN UINT8 dg_app_3700_KSIOCTL(UINT8 control_option, UINT8 buffer_length, UINT8 *buffer);
EXTERN void dg_app_3700_KSAbortIOCTL(void);
EXTERN UINT8 dg_app_3701_KSIOCTL(UINT8 control_option, UINT8 buffer_length, UINT8 *buffer);
EXTERN void dg_app_3701_KSAbortIOCTL(void);

/*  0x31 */
EXTERN UINT8 fdiag_app_0203_Routine_Control(UINT8 *buffer, UINT8 req_type);
EXTERN UINT8 fdiag_app_FF00_Routine_Control(UINT8 *buffer, UINT8 req_type);
EXTERN UINT8 fdiag_app_0202_Routine_Control(UINT8 *buffer, UINT8 req_type);
EXTERN UINT8 fdiag_app_FF01_Routine_Control(UINT8 *buffer, UINT8 req_type);
EXTERN UINT8 fdiag_app_F518_Routine_Control(UINT8 *buffer, UINT8 req_type);
EXTERN UINT8 fdiag_app_DF01_Routine_Control(UINT8 *buffer, UINT8 req_type);

EXTERN void response_11_01(void);
EXTERN BOOLEAN is_CAN_BusOff(void);

#undef EXTERN
#endif


