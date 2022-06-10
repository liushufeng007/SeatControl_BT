

#define DIAG_KWP2K_DI_CORE_ERLG_C

#ifndef KERNEL_PRIVATE_DIAG
    #define KERNEL_PRIVATE_DIAG
#endif

#define KERNEL_RESTRICTED_DIAG


/***************************************************************************
**                I N C L U D E   F I L E S
***************************************************************************/
#include "system.h"
#include "string.h"
#include "diag_kwp2k_di_core_nvm.h"
#include "diag_kwp2k_di_core_mgr.cfg"
#include "diag_kwp2k_di_core_mgr.h"
#include "di_kwp2k_erlg_interface.h"
#include "diag_kwp2k_di_core_erlg.h"
#include "diag_kwp2k_di_core_erlg.cfg"
#include "CddEeprom.h"
/*
___DEFINITIONS(){   }
*/
/***************************************************************************
**   I N T E R N A L   M A C R O   A N D   T Y P E   D E F I N I T I O N S
***************************************************************************/
/*
**  Configuration parameter checks.
*/

/*
** Verify the Maximum No of DTC that can be logged.
*/
#if (PROD_SPEC_NUMBER_OF_DTCS > 255)
   #error CON2ERLG.C: INVALID CONFIGURATION VALUE: PROD_SPEC_NUMBER_OF_DTCS.
#endif


/* 
** byte pattern: erased dtc
** for multible byte erase
*/
static const UINT16 erase_dtc_value = 0x00;/* DTC_STS_NOT_COMP_LAST; */ /* Only Bit4 will be set */

/*
** Bit definitions for dtc_status control
*/
#define DTC_AGING_FINISHED                  ((UINT8) 0x01)
#define NVM_ERASE_IN_PROGRESS               ((UINT8) 0x04)
#define NVM_MCC_STORE_IN_PROGRESS           ((UINT8) 0x08)
#define MIRROR_DTC_MCC_UPDATE_ONGOING       ((UINT8) 0x10)
#define MIRROR_NVM_MCC_STORE_IN_PROGRESS    ((UINT8) 0x20)
#define NVM_ERASE_RESP_PENDING              ((UINT8) 0x40)
#define NVM_ENV_STORE_IN_PROGRESS           ((UINT8) 0x80)

/*
** Bit definitions for dtc_extra_status control
*/
#define DTC_MCC_NVM_UPDATE_REQRD        ((UINT16)0x0001)
#define DTC_STATUS_TO_STORE             ((UINT16)0x0002) /* Only Status will be stored            */
#define DTC_STORE_NOT_ALLOWED           ((UINT16)0x0004) /* This DTC store is disabled            */
#define DTC_MCC_MIRROR_NVM_UPDATE_REQRD ((UINT16)0x0008) /* Mirror Memory Updation required       */
#define DTC_STATUS_TO_STORE_NVM         ((UINT16)0x0010) /* Only Status will be stored in NVM     */
#define DTC_ENV_NVM_UPDATE_REQRD        ((UINT16)0x0020) /* Environment data update in NVM */
#define DTC_ENV_UPDATE_REQRD            ((UINT16)0x0040)
#define DTC_ENV_MIRROR_NVM_UPDATE_REQRD ((UINT16)0x0080) /* Mirror Memory Updation required       */


/*
** Fault detection counter updation definitions
*/
#define DTC_FAULT_FOUND                 ((UINT8)0x00)
#define DTC_FAULT_NOT_FOUND             ((UINT8)0x01)
#define DTC_FAULT_COUNT_ZERO            ((UINT8)128)
#define DTC_FAULT_COUNT_MAX             ((UINT8)255)
#define DTC_FAULT_COUNT_MIN             ((UINT8)0)


/********************************************************************************

NAME:        LAST_ADDR_IN_DTCODES

DESCRIPTION: Address of last byte in dtcodes

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
June/16/2011   avenkat4	 Fixed compiler warning by subtracting 1 for nvm_local_data
						 and nvm_global_data structure
********************************************************************************/
	

#define LAST_ADDR_IN_DTCODES  \
(&dtcodes_counters[sizeof(dtcodes_counters) / sizeof(DTCODES_NVM_RECORD) - 1u].nvm_dtc_index)

  


/*
___DECLERATIONS(){   }
*/
/***************************************************************************
   I N T E R N A L   F U N C T I O N   P R O T O T Y P E S
***************************************************************************/
/*
** Picks up the staus info from the stored list
*/
static void diag_errlog_init_dtc(void);

/*
** Updates the status info of the specified DTC
*/
// static void diag_errlog_update_status_info_unlog(UINT8 p_dtc_index_U8, UINT8 log_status);

/*
** Writes DTC info byte into NVM
*/
//static void diag_errlog_write_dtc_mcc_env_to_nvm(void);

/*
** Erases all DTC information
*/
static void diag_errlog_erase_all_dtcs(void);

/*
** Validates the index passed
*/
static UINT8 diag_errlog_is_index_valid(UINT8 p_index_U8);

/*
** Updates the status info of the specified DTC
*/
static void diag_errlog_update_status_info_log(DG_ERLG_UINT8 p_dtc_index_U8, DG_ERLG_UINT8 log_status);
static void diag_errunlog_update_status_info_unlog(DG_ERLG_UINT8 p_dtc_index_U8, DG_ERLG_UINT8 log_status);
static void diag_errlog_write_dtc_status_to_nvm(void);
/***************************************************************************
   M E M O R Y   A L L O C A T I O N
***************************************************************************/
/*
** The dtc_table_rom stores product specific DTC's.  The first entry in the list
** MUST be padded with a 0 (since DTC indexing by 0 is not supported in the
** code); the last entry in the list MUST be zero in case the product
** specific table is empty.
*/
static const UINT32 dtc_table_rom[] =  {0ul, PROD_SPEC_DTC_TABLE ,0ul };

/*
** DTC code buffer in RAM.
*/
static  UINT8 code_buff[PROD_SPEC_NUMBER_OF_DTCS];


/*
** Erase Index
*/
static  UINT8 dtc_erase_index;

/*
** Index into "code_buff" which is advance one slot each time the diagnnostic
** manager is invoked.
*/
static DG_ERLG_UINT8 code_buff_index;

/*
** DTC storage structure in RAM.
*/

static DTCODES_RAM_RECORD dtcodes[ PROD_SPEC_NUMBER_OF_DTCS ];


/*
** Bit-mapped variable containing local status information.
*/
static DG_ERLG_UINT8 dtc_extra_status[PROD_SPEC_NUMBER_OF_DTCS];

/*
** Control the NVM modifications
*/
static DG_ERLG_UINT8 dtc_nvm_status;

static DG_ERLG_UINT8 store_dtc_status;

/********************************************************************************

NAME:        KernelLogGGDSError

DESCRIPTION: This function logs errors to the ODC or dtc code buffers, or
             doesn't log anything, depending on the operational state of the
             ECU.

INPUTS:      p_dtc_index_U8 ->  index of DTC to be logged

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.
May/11/2009    AVENKAT4  Fix for CQ BSDI00069566
May/29/2009    AVENKAT4  Fix for CQ BSDI00071157. Added check to prevent DTC update
                         if NVM Erase is in progress
17-Aug-2009    avenkat4  BSDI00075002(74333)->Cleared the IGN_CNTR_ROL_OVER bit
                         so that DTC aging count will not display the vaule as 0xFF
                         upon roll over of ignition counter
Dec/04/2009    avenkat4  Updated to fix CQ BSDI00072499
********************************************************************************/
void diag_log_error(DG_ERLG_UINT8 dtc_index, DG_ERLG_UINT8 log_status)
{
    DG_ERLG_UINT8 *fl_buff_ptr_PU8;
    DG_ERLG_UINT8 diag_flag_status;
    /*
    ** If the error code is not in the DTC table, ignore the request.
    */
    if(!TESTBIT(dtc_nvm_status, NVM_ERASE_IN_PROGRESS))
    {
        if(diag_errlog_is_index_valid(dtc_index) != 0)
        {
            /*
            ** If product specific condition do not allow error logging,
            ** ignore the request.
            */
            if(FALSE == Prod_Spec_Disable_Error_Logging())
            {
                /*
                ** Keep the index decremented to access arrays
                */
                dtc_index--;
                diag_flag_status = kwp2k_core_inspect_diag_flag();
                /* If NSCL (no stored codes logging mode), do not log the error */
                if( (!TESTBIT(diag_flag_status, NSCLMODE)) &&
                    (!TESTBIT(dtc_extra_status[dtc_index], DTC_STORE_NOT_ALLOWED)) &&
                    (!TESTBIT(dtc_extra_status[dtc_index], DTC_STATUS_TO_STORE)) )
                {

                    /*
                    ** Use temp pointer to save execution time.
                    */
                    fl_buff_ptr_PU8 = &code_buff[dtc_index];

                    /*
                    ** Check if DTC RAM storage is the same as NVM storage.
                    ** If a RAM error occurs, try to recovery instead of call
                    ** a reset.
                    */
                    #if (DTC_STATUS_ALGORITHM == ISO_14229_BASED)
                    if( TESTBIT(*fl_buff_ptr_PU8, DTC_STS_CONFIRM_DTC) )
                    {
                        if(*fl_buff_ptr_PU8 != nvm_dtc_status[dtc_index] )
                        {
                           /*
                           ** Restore the status from NVM storage.
                           */
                           *fl_buff_ptr_PU8 = nvm_dtc_status[dtc_index];
                        }
                    }
                    #endif

                    #if (DTC_STATUS_ALGORITHM == KWP_BASED)
                    if( *fl_buff_ptr_PU8 == DTC_IS_PRESENT)
                    {
                        if(*fl_buff_ptr_PU8 != nvm_dtc_status[dtc_index] )
                        {
                           /*
                           ** Restore the status from NVM storage.
                           */
                           *fl_buff_ptr_PU8 = nvm_dtc_status[dtc_index];
                        }
                    }
                    #endif

                    #if (DTC_STATUS_ALGORITHM == ISO_14229_BASED)
                        /* Update the fault counter */
 //                       diag_errlog_update_fault_counter(dtc_index, DTC_FAULT_FOUND);

                        #if (FDC_ENABLED == YES)
                        {
                            if (FDC_CONFIG_TEST_FAILED == YES)
                            {
                                if(DTC_FAULT_COUNT_MAX == fault_detection_counter[dtc_index])
                                {
                                    diag_errlog_update_status_info_log(dtc_index, log_status);
                                }
                            }
                            else
                            {
                                diag_errlog_update_status_info_log(dtc_index, log_status);
                            }
                        }
                        #else
                        diag_errlog_update_status_info_log(dtc_index, log_status);
                        #endif
                    #else
                        diag_errlog_update_status_info_log(dtc_index, log_status);
                    #endif /* (DTC_STATUS_ALGORITHM == ISO_14229_BASED) */
                }
            }
          }
    }/*End of NVM_ERASE_IN_PROGRESS*/
    else
    {
    /*Do Nothing*/
    }
}

/********************************************************************************

NAME:        KernelClearGGDSError

DESCRIPTION: This function is called when a fault is being reported as not
             present to update the "status" byte, so it indicates that fault
             is not present.

INPUTS:      dtc_index - index of the DTC which is no longer present.


OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By        Reason For Change
-------------   -------     ---------------------------------------------------
April/23/2004   SKHAN17     Initial revision.
May/31/2004     SKHAN17     Updated to handle status info.

  updated: August 02, 2005 mraisky
           1. BSDI00023177. compiler warning. unused local variable
May/29/2009    AVENKAT4  Fix for CQ BSDI00071157. Added check to prevent DTC update
                         if NVM Erase is in progress
Dec/04/2009    avenkat4  Updated to fix CQ BSDI00072499
Jul/20/2010    avenkat4  BSDI00090329 -> Added logic to check whether DTC is already
                         logged before updating the FDC for clearing DTC.
********************************************************************************/
void diag_clear_error(DG_ERLG_UINT8 dtc_index, DG_ERLG_UINT8 log_status)
{
    /* Check if NVM erase is in progress. If yes don't perform any update */
    if(!TESTBIT(dtc_nvm_status, NVM_ERASE_IN_PROGRESS))
    {
        /*
        ** Verify that the error code is valid.
        */
        if(diag_errlog_is_index_valid(dtc_index) != 0)
        {
           /*
            ** If product specific condition do not allow error logging,
            ** ignore the request.
            */
            if(FALSE == Prod_Spec_Disable_Fault_Clearing())
            {
                /*
                ** Decrement the index to access arrays.
                */
                dtc_index--;
                if (!TESTBIT(dtc_extra_status[dtc_index], DTC_STATUS_TO_STORE))
                {
                    #if (DTC_STATUS_ALGORITHM == ISO_14229_BASED)
                    //    diag_errlog_update_fault_counter(dtc_index, DTC_FAULT_NOT_FOUND);
                    #endif
                        diag_errunlog_update_status_info_unlog(dtc_index, log_status);
                }
            }
         }
    }
    else
    {
      /*Do nothing*/
    }
}


/********************************************************************************

NAME:        errlog_is_dtc_matching_to_status

DESCRIPTION: Returns the dtc record pointer if the requested dtc matched to
             status passed

INPUTS:      p_index_U16    - DTC index
             p_status_U8    - DTC status.
             p_mem_type_U8  - Indicates from which mem to search

OUTPUTS:     NONE.

RETURNS:     DG_ERLG_UINT16: Number of DTCs matching

CHANGE HISTORY:
---------------
    Date          By        Reason For Change
-------------  -------      -------------------------------------------------------
April/23/2004   SKHAN17     Initial revision.
July/07/2004    VKUMBAR     Added Mirror memory conditional compilation
Oct/27/2004     BVISWANA    Modified comments for masking the status bits.
17-Aug-2009    avenkat4  BSDI00075002(74978)-> Modified to report the DTC status
                         matches with the status reported through Service $19 0A.
24-Sep-2009    avenkat4  BSDI00077766 -> Removed the previous fix
********************************************************************************/
DTCODES_RAM_RECORD* diag_errlog_is_dtc_matching_to_status(UINT8 p_index_U8, UINT8 p_status_U8, UINT8 p_mem_type_U8)
{
    DTCODES_RAM_RECORD *fl_dtc_rec_PS = NULL_PTR;

    /*
    ** Mask off the non-supported status bit
    ** that is fetched from application
    */
    p_status_U8 = (p_status_U8 & DTC_SUPPORTED_MASK);

    /*
    ** Check for matching status and return the pointer from appropriate
    ** memeory storage
    */
#if (DTC_STATUS_ALGORITHM == ISO_14229_BASED)
    if(NVM_MAIN_MEMORY == p_mem_type_U8)
    {
		/* Find match by definition */
		if(0x00 != (p_status_U8 & code_buff[p_index_U8]))
		{
		    dtcodes[p_index_U8].dtc_ram_status = code_buff[p_index_U8];
		    /* Matching status, return it */
		    fl_dtc_rec_PS = &dtcodes[p_index_U8];
		}
    }
#endif
	 return fl_dtc_rec_PS;
}


/********************************************************************************

NAME:        errlog_cold_init

DESCRIPTION: This function performs cold initialization.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By        Reason For Change
-------------   -------     ---------------------------------------------------
Dec/04/2009    avenkat4  Initial Version
********************************************************************************/
 void diag_errlog_cold_init(void)
{

     memset(&dtcodes[0], 0, sizeof(dtcodes));

    /*
    ** Initialize the code_buff in RAM from the actual content in NVM.
    */
    diag_errlog_init_dtc();
}


/********************************************************************************

NAME:        diag_errlog_warm_init

DESCRIPTION: This function performs warm initialization.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.
********************************************************************************/
 void diag_errlog_warm_init(void)
{
  diag_errlog_cold_init();
}


/********************************************************************************

NAME:        diag_errlog_sleep

DESCRIPTION: This function performs sleep preparation logic for error logging.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.
********************************************************************************/
 void diag_errlog_sleep(void)
{
    /* No need to do anything here */
}


/********************************************************************************

NAME:        errlog_wakeup

DESCRIPTION: This function performs wakeup functionality.


INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.
********************************************************************************/
 void diag_errlog_wakeup(void)
{
    /*
    ** Initialize the code_buff in RAM from the actual content in NVM
    */
    diag_errlog_init_dtc();
}

/********************************************************************************

NAME:        diag_errlog_init_dtc

DESCRIPTION: This function initialize the code buff base on the information
             in dtcode in NVM.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
Dec/04/2009    avenkat4     Initial Version
********************************************************************************/
static void diag_errlog_init_dtc(void)
{
    UINT16 dtc_status_tmp[NUMBER_OF_DTC_CODES];
    UINT16 dtc_status_init[NUMBER_OF_DTC_CODES] = {0};
    UINT8 i;
	 
    memset(&code_buff[0], 0, sizeof(code_buff));

    (void)CddEeprom_Req_Read(EEPROM_BANK_DIAG, EEP_DTC_START_ADDR, NUMBER_OF_DTC_CODES, dtc_status_tmp);

    if(dtc_status_tmp[0] == 0xFFFF) /* Flash contents are all FFs and need to initial to 0 */
    {
        (void)CddEeprom_Req_Write(EEPROM_BANK_DIAG, EEP_DTC_START_ADDR, NUMBER_OF_DTC_CODES,NULL, dtc_status_init);

        for(i=0; i<NUMBER_OF_DTC_CODES; i++)
        {
            nvm_dtc_status[i] = 0;
        }
    }
    else
    {
        for(i=0; i<NUMBER_OF_DTC_CODES; i++)
        {
            nvm_dtc_status[i] = (UINT8)dtc_status_tmp[i];

            code_buff[i] = (UINT8)dtc_status_tmp[i];
        }
    }
}

/********************************************************************************

NAME:        errlog_find_nth_dtc

DESCRIPTION: Finds and returns pointer to the Nth DTC stored in NVM.

INPUTS:      p_dtc_num_U8 - the Nth DTC to find. The first DTC is 1, the second 2,
                       and so on.

OUTPUTS:     NONE.

RETURNS:     UINT32: Value of the Nth DTC stored in NVM

CHANGE HISTORY:
---------------
    Date          By        Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004   SKHAN17     Initial revision.
July/07/2004    VKUMBAR     Changed function to find the nth logged DTC and
                            return DTC value instead of pointer
03-Feb-2010     avenkat4    Fix for CQ BSDI00083775
********************************************************************************/
 UINT32 diag_errlog_find_nth_dtc(UINT8 p_dtc_num_U8)
{
    UINT8 fl_index_U8  = 0;
    UINT32 fl_dtc_val    = 0;

    /* Check through the list to find the nth one */

    /* CYC:BEGIN_TEST_WHILE MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
    while(fl_index_U8 < PROD_SPEC_NUMBER_OF_DTCS)
    /* CYC:END_TEST_WHILE */
    {
#if (DTC_STATUS_ALGORITHM == ISO_14229_BASED)
            /* Was it logged ? */
            if(  TESTBIT(code_buff[fl_index_U8], DTC_STS_CONFIRM_DTC ) )
            {
                /* Decrement the counter */
                p_dtc_num_U8--;
            }
#endif

           /* Have we found the nth one? */
            if(0 == p_dtc_num_U8)
            {
                /* Found it, return the value */
                fl_dtc_val = dtc_table_rom[fl_index_U8 + 1];

               break;
        }

        /* Move to next index */
        fl_index_U8++;
    }

    return fl_dtc_val;
}



/********************************************************************************

NAME:        errlog_get_dtc_count_by_status

DESCRIPTION: Returns the number of DTCs matching specified mask

INPUTS:      p_status_U8 - DTC status.

OUTPUTS:     NONE.

RETURNS:     UINT16: Number of DTCs matching

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.
17-Aug-2009    avenkat4  BSDI00075002(74978)-> Modified to report the DTC status
                         matches with the status reported through Service $19 0A
24-Sep-2009    avenkat4  BSDI00077766 -> Removed the previous fix
********************************************************************************/
 UINT16 diag_errlog_get_dtc_count_by_status(UINT8 p_status_U8)
{
    UINT8 fl_dtc_count_U8 = ((UINT8)0x00);
    UINT8 fl_index_U8;
    /*
    ** Mask off the non-supported status bit
    ** 7th bit is not supported as of now
    */
    p_status_U8 = (p_status_U8 & DTC_SUPPORTED_MASK);

    /*
    ** Check for matching status and return the count
    */
    /* CYC:BEGIN_TEST_FOR   MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
    for(fl_index_U8 = 0; fl_index_U8 < PROD_SPEC_NUMBER_OF_DTCS; fl_index_U8++)
    /* CYC:END_TEST_FOR */
    {
#if (DTC_STATUS_ALGORITHM == ISO_14229_BASED)
			if(FALSE == kwp2k_core_Diag_Get_DTC_cfg_status(fl_index_U8))
			{
				continue;
			}
            /* Find match by definition */
            if(0x00 != (p_status_U8 & code_buff[fl_index_U8]))
            {
                /* Matching status, count it */
                fl_dtc_count_U8++;
            }
#endif

    }
    return fl_dtc_count_U8;
}
/********************************************************************************

NAME:        errlog_get_nth_dtc_val

DESCRIPTION: Returns the specified DTC value

INPUTS:      p_dtc_index_U16 : DTC index

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.
********************************************************************************/
 UINT32 diag_errlog_get_nth_dtc_val(UINT8 index)
{
    UINT32 fl_dtc_val_U32 = 0x00;

    /*
    ** Return the odc value
    */
    if(diag_errlog_is_index_valid(index) != 0)
    {
        fl_dtc_val_U32 = dtc_table_rom[index];
    }

    return (fl_dtc_val_U32);
}

/********************************************************************************

NAME:        diag_errlog_get_index_by_dtc

DESCRIPTION: Returns DTC index from the dtc value

INPUTS:      p_dtc_val_U32 : DTC value

OUTPUTS:     NONE.

RETURNS:     UINT8 indicating the index where DTC is stored

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.
********************************************************************************/
 UINT8 diag_errlog_get_index_by_dtc(UINT32 dtc)
{
    /* Get the index of DTC we are talking about */
    UINT8 fl_dtc_index_U8 = 1;

    /* CYC:BEGIN_TEST_WHILE MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
    while(fl_dtc_index_U8 <= PROD_SPEC_NUMBER_OF_DTCS)
    /* CYC:END_TEST_WHILE */
    {
        if(dtc == dtc_table_rom[fl_dtc_index_U8])
        {
            /* Found it */
            return (fl_dtc_index_U8);
        }

        fl_dtc_index_U8++;
    }

    return (0);
}

/********************************************************************************

NAME:        diag_errlog_find_nth_dtc_status

DESCRIPTION: Returns the full status of the DTC (All fields in the status
             byte are determined).

INPUTS:      index - DTC index to the ROM table.

OUTPUTS:     NONE.

RETURNS:     status of the DTC.


CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.
Sep/24/2009    AVENKAT4  Fix for CQ BSDI00077766->Modified the default status of
                         DTC to zero instead of 0x50
********************************************************************************/
 UINT8 diag_errlog_find_nth_dtc_status(UINT8 p_dtc_num_U8)
{
    /* By default this bit is set */
    UINT8 fl_temp_status_U8;

/*  Application team should define the macro DTC_DEFAULT_STATUS if  */
/*  diff default status is required          */


   fl_temp_status_U8 = 0x00;

#if (DTC_STATUS_ALGORITHM == ISO_14229_BASED)
        /* Valid index, return the value */
        fl_temp_status_U8 = code_buff[p_dtc_num_U8 - 1];
#endif

#if (DTC_STATUS_ALGORITHM == KWP_BASED)
   /* Was it logged ? */
    if(  (code_buff[p_dtc_num_U8 - 1] == DTC_IS_PRESENT) ||
         (code_buff[p_dtc_num_U8 - 1] == DTC_NOT_PRESENT) )
    {
        /* Valid index, return the value */
        fl_temp_status_U8 = code_buff[p_dtc_num_U8 - 1];
    }
#endif
    return (fl_temp_status_U8);
}

 UINT32 diag_errlog_convert_index_to_dtc(UINT8 index)
{  
  UINT32 ret_value;
  
  /*
  ** Check for a Valid Index, and the Index in
  */
  
  if( (index > 0) && (index <= ((UINT8)NUMBER_OF_DTC_CODES) ))
  {
    ret_value = dtc_table_rom[index];
  }
  else
  {
    ret_value = (UINT32)0u;
  }
  return(ret_value);
}
 


/********************************************************************************

NAME:        diag_errlog_count_dtc

DESCRIPTION: Returns the total logged dtc count

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     UINT16: Indicating number of DTCs

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.
********************************************************************************/
 UINT8 diag_errlog_count_dtc(void)
{
    UINT8 fl_index_U8;
    UINT8 fl_dtc_count_U8 = 0;

    /* CYC:BEGIN_TEST_FOR   MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
    for(fl_index_U8 = 0; fl_index_U8 < PROD_SPEC_NUMBER_OF_DTCS; fl_index_U8++)
    /* CYC:END_TEST_FOR */
    {
#if (DTC_STATUS_ALGORITHM == ISO_14229_BASED)
        if( TESTBIT(code_buff[fl_index_U8], DTC_STS_CONFIRM_DTC) )
        {
            fl_dtc_count_U8++;
        }
#endif

    }

    return fl_dtc_count_U8;
}
 

/********************************************************************************

NAME:        diag_errlog_erase_all_dtcs

DESCRIPTION: Erases all DTC's stored in the continuous trouble code
             memory storage area.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.
May/09/2005    PSUBHASH  Updated to fix BSDI00017993.
updated: 09/16/2005 mraisky
        1.  BSDI00024381 merged change by skoch15
May/29/2009    AVENKAT4  Fix for CQ BSDI00071157. Cleared the DTC_NVM_UPDATE_REQRD
                         bit for all the DTCs after the completion of Service $14.
Dec/04/2009    avenkat4  Updated to fix BSDI00072499.
Feb/26/2010    avenkat4  Fix for CQ BSDI00083479
********************************************************************************/
#if 0
static void diag_errlog_erase_all_dtcs(void)
{

  /* CYC:BEGIN_TEST_WHILE MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
  while(dtc_erase_index < PROD_SPEC_NUMBER_OF_DTCS)
    /* CYC:END_TEST_WHILE */
  {
    if( (code_buff[dtc_erase_index] != 0x00))
	{		
           /* Its DTC present, erase it */
        code_buff[dtc_erase_index] = 0x00;
      
    }
     dtc_erase_index++;
  }

}
#endif


/********************************************************************************

NAME:        diag_errlog_is_index_valid

DESCRIPTION: Checks if specified DTC index is valid

INPUTS:      p_dtc_index_U8: DTC index

OUTPUTS:     UINT8 indicating DTC index is valid or not

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.

  updated:
  01/26/2006 mraisky
  1. merged change made by mmbroom corrected number of dtc codes to be returned.
********************************************************************************/
static UINT8 diag_errlog_is_index_valid(UINT8 p_index_U8)
{
    UINT8 fl_ret_U8 = 0x00;

    if( (0 != p_index_U8) &&
        (PROD_SPEC_NUMBER_OF_DTCS >= p_index_U8) )
    {
        fl_ret_U8 = 0x01;
    }

    return fl_ret_U8;
} 

/********************************************************************************

NAME:        errlog_erase_dtcs

DESCRIPTION: Initiates erase of DTC info for all DTCs

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.

updated: 09/16/2005 mraisky
        1.  BSDI00024382 merged change by skoch15
********************************************************************************/
void diag_errlog_erase_dtcs(void)
{
    dtc_erase_index = 0;
    SETBIT(dtc_nvm_status, NVM_ERASE_IN_PROGRESS | NVM_ERASE_RESP_PENDING);
}


/********************************************************************************

NAME:        diag_errlog_update_status_info_log

DESCRIPTION: This function updates the status information for each DTC.

INPUTS:      p_dtc_index_U16        : Index of error code to store.

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By        Reason For Change
-------------   -------     ---------------------------------------------------
Dec/04/2009    avenkat4     Initial Version
******************************************************************************/
static void diag_errlog_update_status_info_log(DG_ERLG_UINT8 p_dtc_index_U8, DG_ERLG_UINT8 log_status)
{
#if (DTC_STATUS_ALGORITHM == ISO_14229_BASED)
    DG_ERLG_UINT8 fdc_max_flag = 0x00;
    
    /* Check whether test is completed before setting the other status bits */
    if( FALSE == TESTBIT(code_buff[p_dtc_index_U8], DTC_STS_NOT_COMP_THIS_MC) )
    {
        if( FALSE == TESTBIT(code_buff[p_dtc_index_U8], DTC_STS_TEST_FAIL) )
        {
            if (FALSE == TESTBIT(code_buff[p_dtc_index_U8], DTC_STS_CONFIRM_DTC))
            {
                /* DTC is not logged previously. Update the status info */
                code_buff[p_dtc_index_U8]          = ( DTC_STS_TEST_FAIL |
                                                       DTC_STS_FAIL_THIS_MC |
                                                       DTC_STS_FAIL_LAST_CLEAR |
                                                       DTC_STS_PENDING_DTC );

                if ( (log_status == NOT_USED) || (log_status == CONFIRM_STATUS_OK) )
                {
                    #if( (FDC_ENABLED == YES) && ( FDC_CONFIG_TEST_FAILED != YES ))
                        /* If FDC_CONFIG_TEST_FAILED = NO, then Bit3 will set only when FDC=MAX */
                        if ( DTC_FAULT_COUNT_MAX == fault_detection_counter[p_dtc_index_U8])
                        {
                            SETBIT (code_buff[p_dtc_index_U8], DTC_STS_CONFIRM_DTC);
                        }
                    #else
                        SETBIT (code_buff[p_dtc_index_U8], DTC_STS_CONFIRM_DTC);
                    #endif
                    if (TESTBIT(code_buff[p_dtc_index_U8], DTC_STS_CONFIRM_DTC))
                    {
                        dtcodes[p_dtc_index_U8].dtc_mc4 = dtcodes[p_dtc_index_U8].dtc_mc4 + 1;
                        #if (MC4_SUPPORTED == YES)
                        /* Set the bit to indicate NVM updates is required for MCC values */
                        SETBIT(dtc_extra_status[p_dtc_index_U8], DTC_MCC_NVM_UPDATE_REQRD);
                        #endif
                        
                        SETBIT (dtc_extra_status[p_dtc_index_U8], (DTC_ENV_NVM_UPDATE_REQRD | DTC_ENV_MIRROR_NVM_UPDATE_REQRD) );
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                dtcodes[p_dtc_index_U8].dtc_ram_status = code_buff[p_dtc_index_U8];
                /* Clear MCC1 amd MCC2 counters */
                dtcodes[p_dtc_index_U8].dtc_mc1 = 0x00;
                dtcodes[p_dtc_index_U8].dtc_mc2 = 0x00;
                #if (FPC_SUPPORTED == YES)
                dtcodes[p_dtc_index_U8].dtc_fpc = 0x01; /* Start incrementing Fault pending counter */
                #endif
                #if ( (FPC_SUPPORTED == YES) || (MC1_SUPPORTED == YES) || (MC2_SUPPORTED == YES) )
                /* Set the bit to indicate NVM updates is required for MCC values */
                SETBIT(dtc_extra_status[p_dtc_index_U8], DTC_MCC_NVM_UPDATE_REQRD);
                #endif
                /* Set the bit to indicate NVM update is required for DTC status*/
                SETBIT (dtc_extra_status[p_dtc_index_U8], (DTC_STATUS_TO_STORE | DTC_STATUS_TO_STORE_NVM) );
            } /* DTC_STS_CONFIRM_DTC */
            else
            {
                    SETBIT (code_buff[p_dtc_index_U8], (DTC_STS_TEST_FAIL | DTC_STS_FAIL_THIS_MC | DTC_STS_PENDING_DTC) );
                    SETBIT (dtc_extra_status[p_dtc_index_U8], (DTC_STATUS_TO_STORE | DTC_STATUS_TO_STORE_NVM) );
                    /* Clear MCC1 amd MCC2 counters */
                    dtcodes[p_dtc_index_U8].dtc_mc1 = 0x00;
                    dtcodes[p_dtc_index_U8].dtc_mc2 = 0x00;
                    #if (FPC_SUPPORTED == YES)
                    dtcodes[p_dtc_index_U8].dtc_fpc = 0x01; /* Start incrementing Fault pending counter */
                    #endif
                    #if ( (FPC_SUPPORTED == YES) || (MC1_SUPPORTED == YES) || (MC2_SUPPORTED == YES) )
                    /* Set the bit to indicate NVM updates is required for MCC values */
                    SETBIT(dtc_extra_status[p_dtc_index_U8], DTC_MCC_NVM_UPDATE_REQRD);
                    #endif
                    if (ENV_UPDATE_LATEST == YES)
                    {
                        if ( (log_status == CONFIRM_STATUS_OK ) || (log_status == NOT_USED) )
                        {
                                #if( (FDC_ENABLED == YES) && ( FDC_CONFIG_TEST_FAILED != YES ))
                                /* If FDC_CONFIG_TEST_FAILED = NO, then Bit3 will set only when FDC=MAX */
                                if ( DTC_FAULT_COUNT_MAX == fault_detection_counter[p_dtc_index_U8])
                                {
                                    fdc_max_flag = 0x01;
                                }
                                else
                                {
                                    SETBIT (dtc_extra_status[p_dtc_index_U8], DTC_ENV_UPDATE_REQRD);
                                }
                                #else
                                fdc_max_flag = 0x01;
                                #endif
                            if (fdc_max_flag == 0x01)
                            {
                                /* fdc_max_flag = 0x00;  for covery warning*/
                                dtcodes[p_dtc_index_U8].dtc_mc4 = dtcodes[p_dtc_index_U8].dtc_mc4 + 1;
                                #if (MC4_SUPPORTED == YES)
                                /* Set the bit to indicate NVM updates is required for MCC values */
                                SETBIT(dtc_extra_status[p_dtc_index_U8], DTC_MCC_NVM_UPDATE_REQRD);
                                #endif

                                #if (ENV_UPDATE_LATEST == YES)
                                #if (GLOBAL_DATA_SUPPORTED == YES)
                                Prod_Spec_Get_Global_Data (global_data_buff);
                                (void)memcpy((void *)&dtcodes[p_dtc_index_U8].global_data[GD_RO_START_BYTE], (void *)&global_data_buff[GD_RO_START_BYTE], GD_RO_SIZE);
                                #endif /* GLOBAL_DATA_SUPPORTED */
                                #if (LOCAL_DATA_SUPPORTED == YES)
                                Prod_Spec_Get_Local_Data (local_data_buff);
                                (void)memcpy((void *)&dtcodes[p_dtc_index_U8].local_data[LD_RO_START_BYTE], (void *)&local_data_buff[LD_RO_START_BYTE], LD_RO_SIZE);
                                #endif /* LOCAL_DATA_SUPPORTED */
                                #endif
                                SETBIT (dtc_extra_status[p_dtc_index_U8], (DTC_ENV_NVM_UPDATE_REQRD | DTC_ENV_MIRROR_NVM_UPDATE_REQRD) );
                            }
                        }
                        else
                        {
                            SETBIT (dtc_extra_status[p_dtc_index_U8], DTC_ENV_UPDATE_REQRD);
                        }
                    } /* (ENV_UPDATE_LATEST == YES) */
            } /* DTC_STS_CONFIRM_DTC */
        } /* DTC_STS_TEST_FAIL */
        else
        {
            if ( TESTBIT(code_buff[p_dtc_index_U8], DTC_STS_CONFIRM_DTC) )
            {
                if ( TESTBIT(dtc_extra_status[p_dtc_index_U8], DTC_ENV_UPDATE_REQRD) )
                {
                    if ( (log_status == CONFIRM_STATUS_OK ) || (log_status == NOT_USED) )
                    {
                        #if( (FDC_ENABLED == YES) && ( FDC_CONFIG_TEST_FAILED != YES ))
                            /* If FDC_CONFIG_TEST_FAILED = NO, then Bit3 will set only when FDC=MAX */
                            if ( DTC_FAULT_COUNT_MAX == fault_detection_counter[p_dtc_index_U8])
                            {
                                fdc_max_flag = 0x01;
                            }
                        #else
                            fdc_max_flag = 0x01;
                        #endif
                        if (fdc_max_flag == 0x01)
                        {
                            dtcodes[p_dtc_index_U8].dtc_mc4 = dtcodes[p_dtc_index_U8].dtc_mc4 + 1;
                            #if (MC4_SUPPORTED == YES)
                            /* Set the bit to indicate NVM updates is required for MCC values */
                            SETBIT(dtc_extra_status[p_dtc_index_U8], DTC_MCC_NVM_UPDATE_REQRD);
                            #endif

                            #if (GLOBAL_DATA_SUPPORTED == YES)
                            Prod_Spec_Get_Global_Data (global_data_buff);
                            (void)memcpy((void *)&dtcodes[p_dtc_index_U8].global_data[GD_RO_START_BYTE], (void *)&global_data_buff[GD_RO_START_BYTE], GD_RO_SIZE);
                            #endif
                            #if (LOCAL_DATA_SUPPORTED == YES)
                            Prod_Spec_Get_Local_Data (local_data_buff);
                            (void)memcpy((void *)&dtcodes[p_dtc_index_U8].local_data[LD_RO_START_BYTE], (void *)&local_data_buff[LD_RO_START_BYTE], LD_RO_SIZE);
                            #endif
                            SETBIT(dtc_extra_status[p_dtc_index_U8], (DTC_ENV_NVM_UPDATE_REQRD | DTC_ENV_MIRROR_NVM_UPDATE_REQRD) );
                            CLEARBIT(dtc_extra_status[p_dtc_index_U8], DTC_ENV_UPDATE_REQRD);
                        }
                    }
                }
            }
            else
            {
                if ( (log_status == CONFIRM_STATUS_OK ) || (log_status == NOT_USED) )
                {
                    #if( (FDC_ENABLED == YES) && ( FDC_CONFIG_TEST_FAILED != YES ))
                        /* If FDC_CONFIG_TEST_FAILED = NO, then Bit3 will set only when FDC=MAX */
                        if ( DTC_FAULT_COUNT_MAX == fault_detection_counter[p_dtc_index_U8])
                        {
                            SETBIT (code_buff[p_dtc_index_U8], DTC_STS_CONFIRM_DTC);
                        }
                    #else
                        SETBIT (code_buff[p_dtc_index_U8], DTC_STS_CONFIRM_DTC);
                    #endif
                    if (TESTBIT(code_buff[p_dtc_index_U8], DTC_STS_CONFIRM_DTC))
                    {
                        dtcodes[p_dtc_index_U8].dtc_mc4 = dtcodes[p_dtc_index_U8].dtc_mc4 + 1;
                        #if (MC4_SUPPORTED == YES)
                        /* Set the bit to indicate NVM updates is required for MCC values */
                        SETBIT(dtc_extra_status[p_dtc_index_U8], DTC_MCC_NVM_UPDATE_REQRD);
                        #endif

                        SETBIT(dtc_extra_status[p_dtc_index_U8], (DTC_ENV_NVM_UPDATE_REQRD | DTC_ENV_MIRROR_NVM_UPDATE_REQRD) );
                        SETBIT (dtc_extra_status[p_dtc_index_U8], (DTC_STATUS_TO_STORE | DTC_STATUS_TO_STORE_NVM) );
                    }
                }
            }
        } /* DTC_STS_TEST_FAIL */
    } /* DTC_STS_NOT_COMP_THIS_MC */
#endif /*(DTC_STATUS_ALGORITHM == ISO_14229_BASED)*/

  dtcodes[p_dtc_index_U8].dtc_ram_status = code_buff[p_dtc_index_U8];
}


/********************************************************************************

NAME:        diag_errlog_update_status_info_unlog

DESCRIPTION: This function updates the status information for each DTC.

INPUTS:      p_dtc_index_U16        : Index of error code to store.

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By        Reason For Change
-------------   -------     ---------------------------------------------------
Dec/04/2009    avenkat4     Initial Version
Jan/21/2010    avenkat4     To resolve CQ BSDI00082811 -> Added logic to clear 
                            DTC only one time.
Jan/28/2011    avenkat4     Te resolve CQ BSDI00099054 -> Modified to clear DTC
                            even if confirmed DTC bit is not set
Feb/04/2011    avenkat4     Te resolve CQ BSDI00099429 -> Modified to avoid repeated
                            clearing of DTC
******************************************************************************/
static void diag_errunlog_update_status_info_unlog(DG_ERLG_UINT8 p_dtc_index_U8, DG_ERLG_UINT8 log_status)
{
    log_status = log_status; /* To avoid warning */
#if (DTC_STATUS_ALGORITHM == ISO_14229_BASED)
    if( FALSE == TESTBIT(code_buff[p_dtc_index_U8], DTC_STS_NOT_COMP_THIS_MC) )
   {    
        if( TESTBIT(code_buff[p_dtc_index_U8], DTC_STS_TEST_FAIL) ) 
        {
          /* DTC is logged previously. Update the status info */
          code_buff[p_dtc_index_U8] &= ~(DTC_STS_TEST_FAIL);
          /* Set the bit to indicate NVM update is required for DTC status*/
          SETBIT (dtc_extra_status[p_dtc_index_U8], (DTC_STATUS_TO_STORE | DTC_STATUS_TO_STORE_NVM) );
        }
        dtcodes[p_dtc_index_U8].dtc_ram_status = code_buff[p_dtc_index_U8];
    }
#endif
}


/********************************************************************************

NAME:        diag_errlog_write_dtc_status_to_nvm

DESCRIPTION: To update the DTC status info into NVM

INPUTS:      None.

OUTPUTS:     None.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
Dec/04/2009    avenkat4     Initial Version
********************************************************************************/
static void diag_errlog_write_dtc_status_to_nvm(void)
{
    UINT16 tmp_eep_readout;
    UINT16 tmp_eep_write;

  if (code_buff_index < PROD_SPEC_NUMBER_OF_DTCS)
  {
      /* Check if DTC status storage is required */
    if ( TESTBIT(dtc_extra_status[code_buff_index], DTC_STATUS_TO_STORE) )
    {
        (void)CddEeprom_Req_Read(EEPROM_BANK_DIAG, code_buff_index + EEP_DTC_START_ADDR, 1, &tmp_eep_readout);

        store_dtc_status = code_buff[code_buff_index];
        tmp_eep_write = (UINT16)store_dtc_status;

        if(tmp_eep_readout != tmp_eep_write)
        {
            (void)CddEeprom_Req_Write(EEPROM_BANK_DIAG, code_buff_index + EEP_DTC_START_ADDR, 1, NULL, &tmp_eep_write);       
        } 
        /* Check if write was successful */
        (void)CddEeprom_Req_Read(EEPROM_BANK_DIAG, code_buff_index + EEP_DTC_START_ADDR, 1, &tmp_eep_readout);
        nvm_dtc_status[code_buff_index] = (uint8_t)tmp_eep_readout;
        if ( nvm_dtc_status[code_buff_index] == store_dtc_status )
        {
            /* Write Successful. So clear the status flag */
            CLEARBIT(dtc_extra_status[code_buff_index], DTC_STATUS_TO_STORE);
            /* Increment the index to check for the next index */
            code_buff_index = code_buff_index + 1;
        }
        else
        {
            /* Retry write in next cycle */
        }
    }
    else
    {
      /* Increment the index to check for the next status update */
      code_buff_index = code_buff_index + 1;
    }
  }
  else
  {
    code_buff_index = 0x00;
  }
}


/********************************************************************************

NAME:        diag_errlog_dtc_managers

DESCRIPTION: Manages DTC storing / erasing / updating.

INPUTS:      NONE

OUTPUTS:     NONE.

RETURNS:     NONE


CHANGE HISTORY:
---------------
    Date          By        Reason For Change
-------------   -------     -------------------------------------------------------
Dec/04/2009    avenkat4      Initial Version
********************************************************************************/
void diag_errlog_dtc_manager(void)
{
    /* Is erase going on? */
    if(TESTBIT(dtc_nvm_status, NVM_ERASE_IN_PROGRESS))
    {
        /*
        ** If erase is in prograss, do erase.
        */
        diag_errlog_erase_all_dtcs();
        return;
    }

    /*
    ** If erase is not in progress, determine if need to write DTC status to NVM.
    */
    #if (AGING_PROCESS_SUPPORTED == YES)
    diag_errlog_dtc_aging_manager(); /* Check if DTC status needs to be updated for aging process */
    #endif
    diag_errlog_write_dtc_status_to_nvm();
 #if 0
    if ( TESTBIT(dtc_nvm_status, (NVM_MCC_STORE_IN_PROGRESS | NVM_ENV_STORE_IN_PROGRESS)) )
    {
      diag_errlog_write_dtc_mcc_env_to_nvm();
      return;
    }
   
    if (dtc_mcc_index < PROD_SPEC_NUMBER_OF_DTCS)
    {
      if (TESTBIT(dtc_extra_status[dtc_mcc_index], (DTC_MCC_NVM_UPDATE_REQRD | DTC_ENV_NVM_UPDATE_REQRD)))
      {
        diag_errlog_start_write_dtc_mcc_env_to_nvm(dtc_mcc_index);
      }
      else
      {
        dtc_mcc_index += 1;
      }
    }
    else
    {
      dtc_mcc_index = 0;
    }
#endif
}


/********************************************************************************

NAME:        diag_errlog_erase_all_dtcs

DESCRIPTION: Erases all DTC's stored in the continuous trouble code
             memory storage area.

INPUTS:      NONE.

OUTPUTS:     NONE.

RETURNS:     NONE.

CHANGE HISTORY:
---------------
    Date          By     Reason For Change
-------------  -------   -------------------------------------------------------
April/23/2004  SKHAN17   Initial revision.
May/09/2005    PSUBHASH  Updated to fix BSDI00017993.
updated: 09/16/2005 mraisky
        1.  BSDI00024381 merged change by skoch15
May/29/2009    AVENKAT4  Fix for CQ BSDI00071157. Cleared the DTC_NVM_UPDATE_REQRD
                         bit for all the DTCs after the completion of Service $14.
Dec/04/2009    avenkat4  Updated to fix BSDI00072499.
Feb/26/2010    avenkat4  Fix for CQ BSDI00083479
********************************************************************************/
static void diag_errlog_erase_all_dtcs(void)
{
  DG_ERLG_UINT8 clear_index;
  DG_ERLG_UINT8 queue_index;
  DG_ERLG_UINT8 index_found_status = 0x00;
#ifdef PROD_SPEC_NVM_MULTI_BYTE_WRITE
  DG_ERLG_UINT8 erase_value[sizeof(DTCODES_NVM_RECORD)];
  DG_ERLG_UINT8 erase_index = 0; 
#else
  DG_ERLG_UINT8 erase_data; 
#endif
  
  /*
  ** Go to till the one which needs to be erased
  */
  /* CYC:BEGIN_TEST_WHILE MAX_ITER = PROD_SPEC_NUMBER_OF_DTCS EXP_ITER = x */
  while(dtc_erase_index < PROD_SPEC_NUMBER_OF_DTCS)
    /* CYC:END_TEST_WHILE */
  {
    if( code_buff[dtc_erase_index] != 0x00 )
    {        
        /* Its DTC present, erase it */
        code_buff[dtc_erase_index] = 0x00 ;
        dtcodes[dtc_erase_index].dtc_ram_status = 0x00 ; /* DTC_STS_NOT_COMP_LAST ;*/
        dtcodes[dtc_erase_index].dtc_mc1    = 0x00 ;
        dtcodes[dtc_erase_index].dtc_mc2    = 0x00 ;
        dtcodes[dtc_erase_index].dtc_mc3    = 0x00 ;
        dtcodes[dtc_erase_index].dtc_mc4    = 0x00 ;
        dtcodes[dtc_erase_index].dtc_fdc    = DTC_FAULT_COUNT_ZERO ;
        dtcodes[dtc_erase_index].dtc_fpc    = 0x00 ;
        dtcodes[dtc_erase_index].dtc_aged_counter    = 0x00 ;
//        fault_detection_counter[dtc_erase_index] = DTC_FAULT_COUNT_ZERO;

          if(FALSE == CddEeprom_Req_Write(EEPROM_BANK_DIAG, dtc_erase_index + EEP_DTC_START_ADDR, 1, NULL, (DG_ERLG_UINT16 *)&erase_dtc_value))
          { 
          /* If writing fails (epm queue is full) try to erase the DTC in the next cylce. 
          ** The flags must not be cleaned now! 
          */
            return; 
          }
          
        /* Its DTC present, erase it */
        code_buff[dtc_erase_index] = 0x00;
        return;
   }
    
    dtc_erase_index++;
  }

  for (clear_index = 0 ; clear_index < PROD_SPEC_NUMBER_OF_DTCS; clear_index++)
  {
    /*Clear the DTC update History*/
    CLEARBIT(dtc_extra_status[clear_index], (DTC_MCC_NVM_UPDATE_REQRD|DTC_ENV_NVM_UPDATE_REQRD|DTC_ENV_MIRROR_NVM_UPDATE_REQRD|DTC_MCC_MIRROR_NVM_UPDATE_REQRD));
  }
  
   /* Finished erasing of all dtcs */
  CLEARBIT(dtc_nvm_status, NVM_ERASE_IN_PROGRESS);

#if defined(MODE_14_KWP_SUPPORTED) || defined(MODE_14_UDS_SUPPORTED)
  /* Indicate this to ggdsdiag to send response */
  if (TESTBIT( dtc_nvm_status, NVM_ERASE_RESP_PENDING ) )
  {
    diag_DiagMode14CallBack(1);
    CLEARBIT( dtc_nvm_status, NVM_ERASE_RESP_PENDING );
  }
#endif
}
