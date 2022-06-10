
/******************************************************************************
* INCLUDE FILES                                                               *
******************************************************************************/
#include "lookup.h"



/******************************************************************************
* GLOBAL RAM VARIABLES                                                        *
******************************************************************************/


/******************************************************************************
* LOCAL CONSTANTS, DEFINITIONS, and TYPEDEFS                                   *
******************************************************************************/

#define LOOKUP_TWO ((UINT8)2)

#define UCHAR_MAX ((UINT8)0xFF)
#define UINT_MAX  ((UINT16)0xFFFF)
/******************************************************************************
* LOCAL RAM VARIABLES                                                         *
******************************************************************************/


/******************************************************************************
* LOCAL FUNCTION PROTOTYPES                                                   *
******************************************************************************/
#ifdef UTL_PROD_SPEC_VALIDATE_ADDRESS
static UINT8 UTL_u8ValidateAddress(UINT8*);

#endif



/******************************************************************************
* CODE                                                                        *
******************************************************************************/


/* ---------------------------------------------------------------------------*/
/* FUNCTION DEFINITION: linear interpolation function                         */
/*                                                                            */
/* ---------------------------------------------------------------------------*/

#ifdef UTL_PROD_SPEC_U8LOOKUP

UINT8 UTL_u8Lookup(UINT8 *TablePtr, UINT8 Key, UINT8 Switch)
{
    UINT8 PointsLeft;
    UINT8 X1;
    UINT8 X2;
    UINT8 Y1;
    UINT8 Y2;

    PointsLeft = *TablePtr;

#ifdef UTL_PROD_SPEC_VALIDATE_ADDRESS
    if ( (UTL_u8ValidateAddress(TablePtr)==FALSE) )
    {
        return FALSE;
    }
    else
    {
#endif
        TablePtr++;
        if (Switch == 0) {
            X2 = *TablePtr;
            TablePtr++;
            Y2 = *TablePtr;
            TablePtr++;
        }
        else {
            Y2 = *TablePtr;
            TablePtr++;
            X2 = *TablePtr;
            TablePtr++;
        }

        X1 = 0;
        Y1 = Y2;

        while (1)
            {
            if (Key <= X1)
                {
                return (Y1);
                }
            else if (Key <= X2)
                {
                return (UINT8)(Y1 + (long)(Y2 - Y1) * (long)(Key - X1) / (long)(X2 - X1));
                }
            else
                {
                X1 = X2;
                Y1 = Y2;
                --PointsLeft;
                if (PointsLeft > 0) {
                    if (Switch == 0) {
                        X2 = *TablePtr;
                        TablePtr++;
                        Y2 = *TablePtr;
                        TablePtr++;
                    }
                    else {
                        Y2 = *TablePtr;
                        TablePtr++;
                        X2 = *TablePtr;
                        TablePtr++;
                    }
                }
                else
                    {
                    X2 = UCHAR_MAX;
                    Y2 = Y1;
                    }
                }
            }

#ifdef UTL_PROD_SPEC_VALIDATE_ADDRESS
    }
#endif

    return FALSE;

/* QAC warning / error begin**************************************************
         **
         ** The following line of code generates QAC level 4 warning 2006 ('UTL_u8Lookup()' 
         ** has more than one 'return' statement.).  Mark Mcbroom reviewed it on 5/25/06 and 
         ** agreed that the code is acceptable 
         ** and does not require changing.
         */
}

#endif

#ifdef UTL_PROD_SPEC_U16LOOKUP
/* ---------------------------------------------------------------------------*/
/* FUNCTION DEFINITION: linear interpolation function for a 16 bit unsigned   */
/*  integer.                                                                  */
/*                                                                            */
/* WARNING. WARNING.  This function will fail for situations in which either  */
/*  the x or y entries in the table are more than 32767 away.  Make sure      */
/*  consecutive entries in the table are no more than 32767 apart.  Roll-     */
/*  over will occur if this condition exists.   The problem occurs when the   */
/*  sum of the two numbers in the numerator are larger than 2^31.             */
/*                                                                            */
/*                                                                            */
/* ---------------------------------------------------------------------------*/

UINT16 UTL_u16Lookup(UINT16 *TablePtr, UINT16 Key, UINT8 Switch)
{
    UINT16 PointsLeft;                    /* remaining number of x,y pairs in table   */

    UINT16 X1;                            /* x,y values for pair number one           */
    UINT16 Y1;

    UINT16 X2;                            /* x, y values for pair number two          */
    UINT16 Y2;

    PointsLeft = *TablePtr;             /* get initial number of pairs in the table */

        TablePtr++;                     /* increment to first x value in table      */
        if (Switch == 0) 
            {
            X2 = *TablePtr;             /* set second pair of data to first entry in table */
            TablePtr++;
            Y2 = *TablePtr;
            TablePtr++;
            }
        else
            {
            Y2 = *TablePtr;
            TablePtr++;
            X2 = *TablePtr;
            TablePtr++;
            }

        X1 = 0;                         /* Initialize first x value to 0. this will value cases when Key < first X value in table */
        Y1 = Y2;

        while (1)                       /* loop until find x1 < Key < x2 or reach end of table  */
            {
            if (Key <= X1)              /* if less than first entry in table, don't extrapolate.  Return first y value          */
                {
                return (Y1);
                }
            else if (Key <= X2)         /* if we have found 2 points that bracket Key, interpolate and return   */
                {
                return (UINT16)((LONG)Y1 + ((LONG)Y2 - (LONG)Y1) * ((LONG)Key - (LONG)X1) / ((LONG)X2 - (LONG)X1));
                }
            else
                {
                X1 = X2;                /* else, shift forward to next pair of points in table  */
                Y1 = Y2;
                --PointsLeft;
                if (PointsLeft > 0)   /* if we aren't at end of table                         */
                    {
                    if (Switch == 0) 
                        {
                        X2 = *TablePtr;
                        TablePtr++;
                        Y2 = *TablePtr;
                        TablePtr++;
                        }
                    else
                        {
                        Y2 = *TablePtr;
                        TablePtr++;
                        X2 = *TablePtr;
                        TablePtr++;
                        }
                    }
                else                    /* else, we are at end of table and we didn't find a match  */
                    {
                    X2 = (UINT16)UINT_MAX;      /* therefore, return last y value in table                  */
                    Y2 = Y1;
                    }
                }
            }
            //return (UINT16)FALSE;
/* QAC warning / error begin**************************************************
         **
         ** The following line of code generates QAC level 4 warning 2006 ('UTL_u16Lookup()' 
         ** has more than one 'return' statement.).  Mark Mcbroom reviewed it on 5/25/06 and 
         ** agreed that the code is acceptable 
         ** and does not require changing.
         */
}
#endif

#ifdef UTL_PROD_SPEC_S8LOOKUP
/* ---------------------------------------------------------------------------*/
/* FUNCTION DEFINITION: linear interpolation function for a 16 bit unsigned   */
/*  integer.                                                                  */
/*                                                                            */
/* WARNING. WARNING.  This function will fail for situations in which either  */
/*  the x or y entries in the table are more than 127 away.  Make sure        */
/*  consecutive entries in the table are no more than 127 apart.  Roll-       */
/*  over will occur if this condition exists.   The problem occurs when the   */
/*  sum of the two numbers in the numerator are larger than 2^31.             */
/*                                                                            */
/*                                                                            */
/* ---------------------------------------------------------------------------*/
SINT16 UTL_s8Lookup(SINT8 *TablePtr, SINT8 Key, UINT8 Switch)
{
    SINT8 PointsLeft;                    /* remaining number of x,y pairs in table   */

    SINT8 X1;                            /* x,y values for pair number one           */
    SINT8 Y1;

    SINT8 X2;                            /* x, y values for pair number two          */
    SINT8 Y2;

    PointsLeft = *TablePtr;             /* get initial number of pairs in the table */

    TablePtr++;                     /* increment to first x value in table      */
    if (Switch == 0)
        {
        X1 = *TablePtr;             /* set second pair of data to first entry in table */
        TablePtr++;
        Y1 = *TablePtr;
        TablePtr++;
        }
    else
        {
        Y1 = *TablePtr;
        TablePtr++;
        X1 = *TablePtr;
        TablePtr++;
        }

    if (Key <= X1)              /* if less than first entry in table, don't extrapolate.  Return first y value          */
        {
        return (Y1);
        }

    if (Switch == 0)
        {
        X2 = *TablePtr;             /* set second pair of data to first entry in table */
        TablePtr++;
        Y2 = *TablePtr;
        TablePtr++;
        }
    else
        {
        Y2 = *TablePtr;
        TablePtr++;
        X2 = *TablePtr;
        TablePtr++;
        }
    --PointsLeft;                   /* the first point is used */

    while (1)                       /* loop until find x1 < Key < x2 or reach end of table  */
        {
        if (Key <= X2)         /* if we have found 2 points that bracket Key, interpolate and return   */
            {
            return (SINT8)(Y1 + (LONG)((SINT16)Y2 - (SINT16)Y1) * (SINT16)((SINT16)Key - (SINT16)X1) / (SINT16)((SINT16)X2 - (SINT16)X1));
            }
        else
            {
            X1 = X2;                /* else, shift forward to next pair of points in table  */
            Y1 = Y2;
            --PointsLeft;
            if (PointsLeft > 0)   /* if we aren't at end of table                         */
                {
                if (Switch == 0)
                    {
                    X2 = *TablePtr;
                    TablePtr++;
                    Y2 = *TablePtr;
                    TablePtr++;
                    }
                else
                    {
                    Y2 = *TablePtr;
                    TablePtr++;
                    X2 = *TablePtr;
                    TablePtr++;
                    }
                }
            else                    /* else, we are at end of table and we didn't find a match  */
                {
                return Y1;
                }
            }
        }
    return FALSE;
/* QAC warning / error begin**************************************************
         **
         ** The following line of code generates QAC level 4 warning 2006 ('UTL_s8Lookup()' 
         ** has more than one 'return' statement.).  Mark Mcbroom reviewed it on 5/25/06 and 
         ** agreed that the code is acceptable 
         ** and does not require changing.
         */
}
#endif


#ifdef UTL_PROD_SPEC_S16LOOKUP
/* ---------------------------------------------------------------------------*/
/* FUNCTION DEFINITION: linear interpolation function for a 16 bit unsigned   */
/*  integer.                                                                  */
/*                                                                            */
/* WARNING. WARNING.  This function will fail for situations in which either  */
/*  the x or y entries in the table are more than 32767 away.  Make sure      */
/*  consecutive entries in the table are no more than 32767 apart.  Roll-     */
/*  over will occur if this condition exists.   The problem occurs when the   */
/*  sum of the two numbers in the numerator are larger than 2^31.             */
/*                                                                            */
/*                                                                            */
/* ---------------------------------------------------------------------------*/
SINT16 UTL_s16Lookup(SINT16 *TablePtr, SINT16 Key, UINT8 Switch)
{
    SINT16 PointsLeft;                    /* remaining number of x,y pairs in table   */

    SINT16 X1;                            /* x,y values for pair number one           */
    SINT16 Y1;

    SINT16 X2;                            /* x, y values for pair number two          */
    SINT16 Y2;

    PointsLeft = *TablePtr;             /* get initial number of pairs in the table */

    TablePtr++;                     /* increment to first x value in table      */
    if (Switch == 0)
        {
        X1 = *TablePtr;             /* set second pair of data to first entry in table */
        TablePtr++;
        Y1 = *TablePtr;
        TablePtr++;
        }
    else
        {
        Y1 = *TablePtr;
        TablePtr++;
        X1 = *TablePtr;
        TablePtr++;
        }

    if (Key <= X1)              /* if less than first entry in table, don't extrapolate.  Return first y value          */
        {
        return (Y1);
        }

    if (Switch == 0)
        {
        X2 = *TablePtr;             /* set second pair of data to first entry in table */
        TablePtr++;
        Y2 = *TablePtr;
        TablePtr++;
        }
    else
        {
        Y2 = *TablePtr;
        TablePtr++;
        X2 = *TablePtr;
        TablePtr++;
        }
    --PointsLeft;                   /* the first point is used */

    while (1)                       /* loop until find x1 < Key < x2 or reach end of table  */
        {
        if (Key <= X2)         /* if we have found 2 points that bracket Key, interpolate and return   */
            {
            return (SINT16)(Y1 + ((LONG)Y2 - (LONG)Y1) * ((LONG)Key - (LONG)X1) / ((LONG)X2 - (LONG)X1));
            }
        else
            {
            X1 = X2;                /* else, shift forward to next pair of points in table  */
            Y1 = Y2;
            --PointsLeft;
            if (PointsLeft > 0)   /* if we aren't at end of table                         */
                {
                if (Switch == 0)
                    {
                    X2 = *TablePtr;
                    TablePtr++;
                    Y2 = *TablePtr;
                    TablePtr++;
                    }
                else
                    {
                    Y2 = *TablePtr;
                    TablePtr++;
                    X2 = *TablePtr;
                    TablePtr++;
                    }
                }
            else                    /* else, we are at end of table and we didn't find a match  */
                {
                return Y1;
                }
            }
        }
    return (SINT16)FALSE;

/* QAC warning / error begin**************************************************
         **
         ** The following line of code generates QAC level 4 warning 2006 ('UTL_s16Lookup()' 
         ** has more than one 'return' statement.).  Mark Mcbroom reviewed it on 5/25/06 and 
         ** agreed that the code is acceptable 
         ** and does not require changing.
         */
}
#endif

#ifdef UTL_PROD_SPEC_VALIDATE_ADDRESS
/* ---------------------------------------------------------------------------*/
/* NAME: ValidateAddress                                                      */
/* DESCRIPTION: Check that the addresses referenced are in a valid range.     */
/* This is specific for the TMS370 RAM, ROM, and EE ranges of memory.         */
/* ---------------------------------------------------------------------------*/
UINT8 UTL_u8ValidateAddress(UINT8 *TablePtr)

    {
    const UINT8 NPoints = *TablePtr;

/* QAC warning / error begin*************************************************
**
** The following line of code generates QAC warning  0306 (MISRA Rule 45).
** It has been reviewed by Anushia Balendran and Mark McBroom on 9/24/2004 and deemed
** acceptable.  The casting from pointer to integer is required in order to range check
** the address.  If/When porting to 32 bit processor, this logic may need to be revised. 
**
** QAC warning / error end**************************************************/

    const UINT16 Address = (UINT16)TablePtr;

    if((NPoints==0)  ||           /* condition check for EE written over with 00 */
       (NPoints==UCHAR_MAX)  )          /* condition check for EE written over with ff */
       {
       return FALSE;
       }

    if( ( (Address >= RAM_START_ADDRESS) && ( (Address + (UINT16)(NPoints *LOOKUP_TWO) ) >= RAM_START_ADDRESS) &&        
          (Address <= RAM_END_ADDRESS)   && ( (Address + (UINT16)(NPoints *LOOKUP_TWO) ) <= RAM_END_ADDRESS)     ) ||    /* table begins AND ends in RAM OR */

        ( (Address >= EE_START_ADDRESS)  && ( (Address + (UINT16)(NPoints *LOOKUP_TWO) ) >= EE_START_ADDRESS)  &&
          (Address <= EE_END_ADDRESS)    && ( (Address + (UINT16)(NPoints *LOOKUP_TWO) ) <= EE_END_ADDRESS)      ) ||    /* table begins AND ends in EE OR */

        ( (Address >= ROM_START_ADDRESS) && ( (Address + (UINT16)(NPoints *LOOKUP_TWO) ) >= ROM_START_ADDRESS) &&
          (Address <= ROM_END_ADDRESS)   && ( (Address + (UINT16)(NPoints *LOOKUP_TWO) ) <= ROM_END_ADDRESS)     )   )   /* table begins AND ends in ROM */

        {
        return TRUE;      /* valid address(es) */
        }
    else
        {
        return FALSE;     /* invalid address(es) */
        }

/* QAC warning / error begin**************************************************
         **
         ** The following line of code generates QAC level 4 warning 2006 ('UTL_u8ValidateAddress()' 
         ** has more than one 'return' statement.).  Mark Mcbroom reviewed it on 5/25/06 and 
         ** agreed that the code is acceptable 
         ** and does not require changing.
         */
    }

#endif

