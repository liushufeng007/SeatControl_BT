#ifndef VTP_HEADER
#define VTP_HEADER
/* ===========================================================================
**
**  Name:
**
**  Description:
**
**  Organization:   Multiplex Core Technology
**
** =========================================================================*/

/* ===========================================================================
**  I N C L U D E   F I L E S
** =========================================================================*/

#define VTP_HEADER_VERSION  0x0100

/*******************************************************************************
================================================================================
 P U B L I C   T Y P E   D E F I N I T I O N S
================================================================================
*******************************************************************************/
typedef unsigned char   VTP_UINT8;
typedef unsigned char   VTP_BOOL;
typedef unsigned short  VTP_UINT16;
typedef unsigned int    VTP_UINT32;

typedef struct {
   VTP_UINT16  DataLength;
   VTP_UINT8   *Data;
   }VTP_SDU;

typedef struct {
   VTP_UINT32  MsgID;
   VTP_UINT8   Size;
   VTP_UINT8   Data[8];
   }VTP_PDU;

/*******************************************************************************
================================================================================
 P U B L I C   M A C R O S
================================================================================
*******************************************************************************/
#define VTP_TRUE                 (VTP_BOOL)1
#define VTP_FALSE                (VTP_BOOL)0
#define VTP_NOTOK                VTP_FALSE
#define VTP_OK                   VTP_TRUE
#define VTP_ERROR                (0xFF)
#define VTP_SUCCESS              (0x01)



/*******************************************************************************
================================================================================
 P U B L I C   F U N C T I O N S   P R O T O T Y P E S
================================================================================
*******************************************************************************/

extern void VTP_Init(void);
extern void VTP_Channel_Init(VTP_UINT8);
extern void      VTP_PeriodicTask(void);

#if (VTP_NUM_CHANNEL_COUNT > 1)
extern VTP_PDU          VTPmc_TxMsg[VTP_NUM_CHANNEL_COUNT];
extern VTP_UINT8 VTP_Transmit(VTP_UINT8,VTP_UINT8 *, VTP_UINT16);
extern void VTP_AbortTransmit(VTP_UINT8);
extern void VTP_TxConfirm(VTP_UINT8);
extern void VTP_RxIndication(VTP_UINT8,VTP_PDU *);
extern VTP_UINT8 VTPmc_ApplCB_RxStart(VTP_UINT8,VTP_UINT32,VTP_UINT16);
extern void VTPmc_ApplCB_RxInProg(VTP_UINT8,VTP_UINT8 *, VTP_UINT8);
extern void VTPmc_ApplCB_RxIndication(VTP_UINT8,VTP_UINT8);
extern void VTPmc_ApplCB_TxConfirm(VTP_UINT8,VTP_UINT8);
extern void VTP_SetFC_Overflow(VTP_UINT8);
extern void VTP_SetFC_Wait(VTP_UINT8);
#else
extern VTP_PDU          VTP_TxMsg;
extern VTP_UINT8 VTP_Transmit(VTP_UINT8 *, VTP_UINT16);
extern void VTP_AbortTransmit(void);
extern void VTP_TxConfirm(void);
extern void VTP_RxIndication(VTP_PDU *);
extern VTP_UINT8 VTP_ApplCB_RxStart(VTP_UINT32,VTP_UINT16);
extern void VTP_ApplCB_RxInProg(VTP_UINT8 *, VTP_UINT8);
extern void VTP_ApplCB_RxIndication(VTP_UINT8);
extern void VTP_ApplCB_TxConfirm(VTP_UINT8);
extern void VTP_SetFC_Overflow(void);
extern void VTP_SetFC_Wait(void);
#endif

/*******************************************************************************
================================================================================
DATE           AUTHOR      VERSION  DESCRIPTION
================================================================================
2/Apr/2007     rmohanku    1      Initial Release.

24-Feb-09      vkannan     2      Configured TP component to be used with 
                                  MUX CAN driver and KWPDIAG Component

26-Feb-09      vkannan     3      Updated as per Review Comments
                                  SMIS ID --> 18758

26-Mar-09      vkannan     4      Updated with QAC fixes after using latest 
                                 personality files from integration team.

03-Apr-09      vkannan     5      Multi Channel TP support

================================================================================
*******************************************************************************/

#endif

