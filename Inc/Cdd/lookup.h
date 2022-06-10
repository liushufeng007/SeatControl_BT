/*******************************************************************************
* TITLE:       lookup.h                                                       *
* DESCRIPTION: header file for linear interpolation function                  *
*                                                                             *
******************************************************************************/

#ifndef _LOOKUP_H_ 
#define _LOOKUP_H_

/******************************************************************************
* INCLUDE FILES                                                               *
******************************************************************************/

#include "system.h"


/******************************************************************************
* GLOBAL CONSTANTS, DEFINITIONS, and TYPEDEFS                                 *
******************************************************************************/
typedef signed long int  LONG;

#define UTL_PROD_SPEC_U16LOOKUP


/******************************************************************************
* EXTERNAL RAM VARIABLES                                                      *
******************************************************************************/


/******************************************************************************
* GLOBAL FUNCTION PROTOTYPES                                                  *
******************************************************************************/
#ifdef  UTL_PROD_SPEC_U8LOOKUP
UINT8 UTL_u8Lookup   (UINT8 *TablePtr, UINT8 Key, UINT8 Switch);
#endif

#ifdef UTL_PROD_SPEC_U16LOOKUP
UINT16  UTL_u16Lookup(UINT16  *TablePtr, UINT16  Key, UINT8 Switch);
#endif

#ifdef  UTL_PROD_SPEC_S16LOOKUP
SINT16   UTL_s16Lookup( SINT16  *TablePtr,  SINT16  Key, UINT8 Switch);
#endif

#ifdef  UTL_PROD_SPEC_S8LOOKUP
SINT16   UTL_s8Lookup( SINT8  *TablePtr,  SINT8  Key, UINT8 Switch);
#endif

#endif


