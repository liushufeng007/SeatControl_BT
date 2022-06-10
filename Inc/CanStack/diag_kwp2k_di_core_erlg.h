#if !defined(DIAG_KWP2K_DI_CORE_ERLG_H)
#define DIAG_KWP2K_DI_CORE_ERLG_H
/*============================================================================
** I N C L U D E   F I L E S
**==========================================================================*/
#include "system.h"
#include "diag_kwp2k_di_core_erlg.cfg"

#ifndef EXTERN
#define EXTERN extern
#endif

#define NULL_PTR   ((void *) 0)

/* Define the number of bytes required to store in NVM for each DTC */
#define NUM_OF_BYTES_TO_STORE (1 + NUM_OF_ADDITIONAL_MC_SUPPORTED + GLOBAL_DATA_SIZE + LOCAL_DATA_SIZE) /* Number of MC parameters supported plus 1 */

typedef struct       
{                    
    UINT8 dtc_status;
    UINT8 dtc_icycle;
} DTCODES_RECORD;

typedef struct
{
  UINT8 dtc_ram_status;
  UINT8 dtc_mc1;
  UINT8 dtc_mc2;
  UINT8 dtc_mc3;
  UINT8 dtc_mc4;
  UINT8 dtc_fdc;
  /* Refer to SMTC 2 800 004 Technica-20101027.pdf
  in 06_documentation for details of Fault pending counter(fpc) and aged counter.
  For fault pending counter, aging counter M2 should be stored in NVM */
  UINT8 dtc_fpc;
  UINT8 dtc_aged_counter;
  #if (GLOBAL_DATA_SUPPORTED == YES)
  UINT8 global_data[GLOBAL_DATA_SIZE];
  #endif
  #if (LOCAL_DATA_SUPPORTED == YES)
  UINT8 local_data[LOCAL_DATA_SIZE];
  #endif
} DTCODES_RAM_RECORD;

typedef struct
{
  UINT8 nvm_dtc_index;

} DTCODES_NVM_RECORD;

/*
** This structure holds information needed for the process of writing
** a DTC to NVM.
**   next_byte_ptr   = pointer to where next byte is to be written in dtcodes
**   next_byte_index = next byte to be written to dtcodes as an index into
**                     bytes_to_store
**   bytes_left      = number of bytes yet to be written to NVM
**   bytes_to_store  = buffer containing bytes to be written to NVM
*/
typedef struct
{
   	UINT8 *next_byte_ptr;
   	UINT8 next_byte_index;
   	UINT8 bytes_left;
   	UINT8 bytes_to_store[NUM_OF_BYTES_TO_STORE];
} STORE_DTC_STRUCT;


typedef struct
{
	UINT16	dtc_index;
	UINT8	dtc_snap_shot_id;
	UINT8	dtc_snap_id_bits;
	UINT8	*global_snap_data;
	UINT8	global_snap_size;
	UINT8	*local_snap_data;
	UINT8	local_snap_size;
} DTC_SNAP_SHOT_DATA;

typedef unsigned char  DG_ERLG_UINT8;
typedef unsigned short DG_ERLG_UINT16;
typedef unsigned long  DG_ERLG_UINT32;
typedef unsigned char  DG_ERLG_BOOLEAN;

/* Define number of DTC codes -- subtract 2, since there are two extra */
/* entries built in.     											   */
#define DTC_ENTRY_SIZE   sizeof(ULONG)
#define NUMBER_OF_DTC_CODES     PROD_SPEC_NUMBER_OF_DTCS
#define QUEUE_SIZE              NVM_QUEUE_SIZE_MC

#define EEP_DTC_START_ADDR   (0x00U)
/*
** Value indicating an available slot in dtcodes
*/
#define SLOT_AVAILABLE  ((UINT8) 0xFF)


#define DTC_SUPPORTED_MASK			PROD_SPEC_AVAILABLE_DTC_MASK

/*
** Bit definitions for the diag_flags data store.
*/
#define    DIAGMODE        ((UINT8) 0x01)
#define    SELFTEST        ((UINT8) 0x02)
#define    NSCLMODE        ((UINT8) 0x04)
#define    BUSY            ((UINT8) 0x20)

/*
** Bit definitions for DTC status byte store.
*/
#define DTC_STS_TEST_FAIL               ((UINT8)0x01)
#define DTC_STS_FAIL_THIS_MC            ((UINT8)0x02)
#define DTC_STS_PENDING_DTC             ((UINT8)0x04)
#define DTC_STS_CONFIRM_DTC             ((UINT8)0x08)
#define DTC_STS_NOT_COMP_LAST           ((UINT8)0x10)
#define DTC_STS_FAIL_LAST_CLEAR         ((UINT8)0x20)
#define DTC_STS_NOT_COMP_THIS_MC        ((UINT8)0x40)
#define DTC_STS_WARN_IND_REQD           ((UINT8)0x80)


#if (DTC_STATUS_ALGORITHM == KWP_BASED)
#define NO_DTC_DETECTED                ((UINT8) 0x00)
#if 1
#define DTC_NOT_PRESENT                ((UINT8) 0x08)
#define DTC_IS_PRESENT                 ((UINT8) 0x09)
#endif 
#if 0
#define DTC_NOT_PRESENT                ((UINT8) 0x20)
#define DTC_IS_PRESENT                 ((UINT8) 0x60)
#endif

#endif

/*
** NVM Memory type definitions
*/
#define NVM_MAIN_MEMORY				0x01
#define NVM_MIRROR_MEMORY			0x02

EXTERN  void diag_errlog_cold_init(void);
EXTERN  void diag_errlog_warm_init(void);
EXTERN  void diag_errlog_sleep(void);
EXTERN  void diag_errlog_wakeup(void);
EXTERN  void diag_errlog_dtc_manager(void);
EXTERN  DTCODES_RAM_RECORD* diag_errlog_is_dtc_matching_to_status(UINT8 p_index_U8, UINT8 p_status_U8, UINT8 p_mem_type_U8);
EXTERN  UINT32 diag_errlog_get_nth_dtc_val(UINT8 index);
EXTERN  UINT8 diag_errlog_get_index_by_dtc(UINT32 dtc);
EXTERN  UINT8 diag_errlog_find_nth_dtc_status(UINT8 n);
EXTERN  UINT32 diag_errlog_convert_index_to_dtc(UINT8 Index);

EXTERN  void diag_log_error(UINT8 dtc_index, UINT8 log_status);
EXTERN  void diag_clear_error(UINT8 dtc_index, UINT8 log_status);
EXTERN  void diag_error_log_key_off_func(void);
/*
** Returns the M2 value
*/
 EXTERN  UINT32 diag_errlog_find_nth_dtc(UINT8 p_dtc_num_U8);
 EXTERN  UINT16 diag_errlog_get_dtc_count_by_status(UINT8 p_status_U8);
  EXTERN  UINT8 diag_errlog_count_dtc(void);



#undef EXTERN
#endif /* DIAG_KWP2K_DI_CORE_ERLG_H */



