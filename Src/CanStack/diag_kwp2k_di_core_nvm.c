
#define DIAG_KWP2K_DI_CORE_NVM_C

#include "diag_kwp2k_di_core_nvm.h"


unsigned char nvm_dtc_status[NUMBER_OF_DTC_CODES] = \
	DTC_STATUS_NVM_DEFAULT_INITIALIZATION

DTCODES_NVM_RECORD dtcodes_counters[ QUEUE_SIZE ] = \
	DTC_MCC_NVM_DEFAULT_INITIALIZATION



