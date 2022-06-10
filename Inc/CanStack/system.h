#ifndef SYSTEM_H
#define SYSTEM_H

//#include "ac780x.h"

#define NEAR
#define INTERRUPT
#define NEARFUNC
#define REENTRANT




#ifndef SINT8
typedef   signed char   SINT8;
typedef   signed char   s8;
#endif

#ifndef UINT8
typedef   unsigned char   UINT8;
typedef   unsigned char   u8;
#endif

#ifndef SIN16
typedef   signed short int   SINT16;
typedef   signed short int   s6;
#endif

#ifndef UIN16
typedef   unsigned short int   UINT16;
typedef   unsigned short int   u6;
#endif

#ifndef SINT32
typedef   signed long   SINT32;
typedef   signed long   s32;
#endif

#ifndef BOOLEAN
typedef unsigned char   BOOLEAN;
//typedef unsigned char   bool;
#endif

#define TESTBIT( operand, bit_mask )        (((operand) &  (bit_mask)) != ((bit_mask) - (bit_mask)))
#define SETBIT( operand, bit_mask )         ((operand) |= (bit_mask))
#define CLEARBIT( operand, bit_mask )       ((operand) &= (~(bit_mask)))


#define IS_TRUE(x) (x != FALSE)
#define IS_FALSE(x) (x == FALSE)

#if !defined( BIT0 )
#define BIT0    (0x01)
#endif
#if !defined( BIT1 )
#define BIT1    (0x02)
#endif
#if !defined( BIT2 )
#define BIT2    (0x04)
#endif
#if !defined( BIT3 )
#define BIT3    (0x08)
#endif
#if !defined( BIT4 )
#define BIT4    (0x10)
#endif
#if !defined( BIT5 )
#define BIT5    (0x20)
#endif
#if !defined( BIT6 )
#define BIT6    (0x40)
#endif
#if !defined( BIT7 )
#define BIT7    (0x80)
#endif

#if !defined( BIT8 )
#define BIT8    (0x0100u)
#endif
#if !defined( BIT9 )
#define BIT9    (0x0200u)
#endif
#if !defined( BIT10 )
#define BIT10   (0x0400u)
#endif
#if !defined( BIT11 )
#define BIT11   (0x0800u)
#endif
#if !defined( BIT12 )
#define BIT12   (0x1000u)
#endif
#if !defined( BIT13 )
#define BIT13   (0x2000u)
#endif
#if !defined( BIT14 )
#define BIT14   (0x4000u)
#endif
#if !defined( BIT15 )
#define BIT15   (0x8000u)
#endif


#if !defined( BIT16 )
#define BIT16   (0x00010000uL)
#endif
#if !defined( BIT17 )
#define BIT17   (0x00020000uL)
#endif
#if !defined( BIT18 )
#define BIT18   (0x00040000uL)
#endif
#if !defined( BIT19 )
#define BIT19   (0x00080000uL)
#endif
#if !defined( BIT20 )
#define BIT20   (0x00100000uL)
#endif
#if !defined( BIT21 )
#define BIT21   (0x00200000uL)
#endif
#if !defined( BIT22 )
#define BIT22   (0x00400000uL)
#endif
#if !defined( BIT23 )
#define BIT23   (0x00800000uL)
#endif
#if !defined( BIT24 )
#define BIT24   (0x01000000uL)
#endif
#if !defined( BIT25 )
#define BIT25   (0x02000000uL)
#endif
#if !defined( BIT26 )
#define BIT26   (0x04000000uL)
#endif
#if !defined( BIT27 )
#define BIT27   (0x08000000uL)
#endif
#if !defined( BIT28 )
#define BIT28   (0x10000000uL)
#endif
#if !defined( BIT29 )
#define BIT29   (0x20000000uL)
#endif
#if !defined( BIT30 )
#define BIT30   (0x40000000uL)
#endif
#if !defined( BIT31 )
#define BIT31   (0x80000000uL)
#endif


typedef struct tagCAN_RMD
{
    unsigned long   Identifier;                    /* CAN message identifier  */
    UINT8    Data [8];                      /* CAN message data        */
    UINT8    Size;                          /* CAN message data length */
    UINT8    Qualifiers;                    /* Receive Qualifiers      */
    UINT8    Vector;                        /* CAN message vector ID   */
    UINT8    XgateWordAlign;                /* Xgate Word Alignment    */

} CAN_RMD, *PCAN_RMD;



typedef struct tagCAN_TMD
{
    UINT8           Size;           /* CAN message data length  */
    unsigned long          Identifier;     /* CAN message identifier   */
    UINT8 const *   pData;          /* Pointer to Data          */
    UINT8           Options;        /* CAN message options      */
    UINT16          MsgHandle;      /* Transmit Message Handle  */

} CAN_TMD, *PCAN_TMD;




#define CANH_MV_GPTX                    (-2)
#define CANH_MV_SWQ                     (-1)
#define CANH_MV_0                       ( 0)
#define CANH_MV_1                       ( 1)
#define CANH_MV_2                       ( 2)
#define CANH_MV_3                       ( 3)
#define CANH_MV_4                       ( 4)
#define CANH_MV_5                       ( 5)
#define CANH_MV_6                       ( 6)
#define CANH_MV_7                       ( 7)
#define CANH_MV_8                       ( 8)
#define CANH_MV_9                       ( 9)
#define CANH_MV_10                      (10)
#define CANH_MV_11                      (11)
#define CANH_MV_12                      (12)
#define CANH_MV_13                      (13)
#define CANH_MV_14                      (14)
#define CANH_MV_15                      (15)
#define CANH_MV_16                      (16)
#define CANH_MV_17                      (17)
#define CANH_MV_18                      (18)
#define CANH_MV_19                      (19)
#define CANH_MV_20                      (20)
#define CANH_MV_21                      (21)
#define CANH_MV_22                      (22)
#define CANH_MV_23                      (23)
#define CANH_MV_24                      (24)
#define CANH_MV_25                      (25)
#define CANH_MV_26                      (26)
#define CANH_MV_27                      (27)
#define CANH_MV_28                      (28)
#define CANH_MV_29                      (29)
#define CANH_MV_30                      (30)
#define CANH_MV_31                      (31)
#define CANH_MV_ALL                     (32)

#ifndef NULL
#define NULL (0)
#endif

#define CAN_GPNUM_1         (1)
#define CAN_GPNUM_2         (2)
#define CAN_GPNUM_3         (3)
#define CAN_GPNUM_4         (4)
#define CAN_GPNUM_5         (5)
#define CAN_GPNUM_6         (6)
#define CAN_GPNUM_7         (7)
#define CAN_GPNUM_8         (8)
#define CAN_GPNUM_9         (9)
#define CAN_GPNUM_10        (10)
#define CAN_GPNUM_11        (11)
#define CAN_GPNUM_12        (12)
#define CAN_GPNUM_13        (13)
#define CAN_GPNUM_14        (14)
#define CAN_GPNUM_15        (15)
#define CAN_GPNUM_16        (16)




#define CANB_TX_STD_DATA                    ((UINT8)(0x00u))
#define CANB_TX_EXTENDED                    ((UINT8)(0x01u))
#define CANB_TX_REMOTE                      ((UINT8)(0x02u))
#define CANB_TX_QUEUED_TMD                  ((UINT8)(0x04u))
#define CANB_REMOTE_DATA_UPDATE             ((UINT8)(0x08u))
#define CANB_TX_REMOTE_DATA                 ((UINT8)(0x10u))
#define CANB_TX_PAD_DATA                    ((UINT8)(0x20u))


#define CANRC_OK                        ( 1)
#define CANRC_SUCCESS                   ( 1)
#define CANRC_TRUE                      ( 1)
#define CANRC_FALSE                     ( 0)
#define CANRC_NULL                      ( 0)
#define CANRC_FAIL                      ( 0)
#define CANRC_EMPTY                     ( 0)
#define CANRC_ERROR                     (-1)
#define CANRC_INVALID_PARM              (-2)
#define CANRC_INVALID_HANDLE            (-3)
#define CANRC_NULL_PTR                  (-4)
#define CANRC_INVALID_SERVICE           (-5)
#define CANRC_NOT_AVAILABLE             (-6)
#define CANRC_RX_OVERRUN                (-7)
#define CANRC_TXQ_OVERRUN               (-8)
#define CANRC_TMDQ_OVERRUN              (-9)
#define CANRC_RXQ_OVERRUN               (-7)


#define CANEX_BUSOFF                    ( 0)
#define CANEX_RX_OVERRUN                ( 1)
#define CANEX_RXQ_OVERRUN               ( 2)
#define CANEX_TXQ_ERROR                 ( 3)
#define CANEX_TMDQ_ERROR                ( 4)
#define CANEX_RXQ_ERROR                 ( 5)
#define CANEX_TX_ERROR_PASSIVE          ( 6)
#define CANEX_RX_ERROR_PASSIVE          ( 7)
#define CANEX_ERROR_PASSIVE             ( 8)
#define CANEX_TX_ERROR_WARNING          ( 9)
#define CANEX_RX_ERROR_WARNING          (10)
#define CANEX_ERROR_WARNING             (11)



#define CANH_OM_ONLINE                  ( 0)
#define CANH_OM_OFFLINE                 ( 1)
#define CANH_OM_POWER_DOWN              ( 2)
#define CANH_OM_SLEEP                   ( 3)
#define CANH_OM_WAKEUP                  ( 4)
#define CANH_OM_SUSPEND_ALL             ( 5)
#define CANH_OM_SUSPEND_TX              ( 6)
#define CANH_OM_SUSPEND_RX              ( 7)
#define CANH_OM_RESUME_ALL              ( 8)
#define CANH_OM_RESUME_TX               ( 9)
#define CANH_OM_RESUME_RX               (10)
#define CANH_OM_ENABLE_INTERRUPTS       (11)
#define CANH_OM_DISABLE_INTERRUPTS      (12)
#define CANH_OM_ENABLE_RX_INTERRUPT     (13)
#define CANH_OM_DISABLE_RX_INTERRUPT    (14)
#define CANH_OM_ENABLE_LOOPBACK         (15)
#define CANH_OM_DISABLE_LOOPBACK        (16)
#define CANH_OM_ENABLE_SILENTMODE       (17)
#define CANH_OM_DISABLE_SILENTMODE      (18)

#define CAN_MASK8_COMPLEMENT(mask)         ((UINT8) (0xFF ^ (mask)))

#define CAN_MASK16_COMPLEMENT(mask)        ((UINT16) (0xFFFF ^ (mask)))




/*-----------------------------------------------------------------------------*/
/* Types Declarations       	                                              						     */
/*-----------------------------------------------------------------------------*/

typedef unsigned short 		     UINT16;
typedef unsigned int 		       UINT32;
typedef unsigned int            uint32_t;
typedef unsigned long long 	   UINT64;
typedef char 				           INT8;
typedef short 				         INT16;
typedef int 				           INT32;
typedef long 				           LONG32;
typedef float 				         FLOAT32;
typedef double 				         FLOAT64;

typedef unsigned char         UINT8;
typedef unsigned char         uint8_t;
typedef unsigned int          u32;


#define TRUE     (1U)
#define FALSE    (0U)

#endif



