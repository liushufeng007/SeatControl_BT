#if !defined(KWPDIAG_CFG_H)
#define KWPDIAG_CFG_H

#include "tp_cfg.h"
#include "tp.h"
#include "system.h"



#define KWPDIAG_TPCHANNEL 0
#define KWPDIAG_MAXPRIMARYBUFFLEN   290     
#define KWPDIAG_MAXSECONDARYBUFFLEN 10      

#define KWPDIAG_P2_TIME_INMSEC      50     /* Time between Req to Resp */ 
#define KWPDIAG_S1_TIME_INMSEC      5000   /* Session Timer - Resp to Req */   
#define KWPDIAG_RCRRPMAXTIME_INMSEC 5000   /* Max Time for RCRRP's */
#define KWPDIAG_P3MAXTIME_INMSEC    500    /* Time between consecutive RCRRP's */

#define KWPDIAG_CALLCYCLE_INMSEC    20   

#define KWPDIAG_NUM_SERVICES        12

//#define KWPDIAG_RCRRP KWPDIAG_ENABLE
#define KWPDIAG_RCRRP KWPDIAG_DISABLE


#define KWPDIAG_RESPONSE_ON_FUNC_REQ      KWPDIAG_ENABLE
#define KWPDIAG_RESPONSE_ALL_ON_FUNC_REQ  KWPDIAG_DISABLE

#define NOSESSIONCHECK            0xFF
/* diag  session control */
#define DIAG_NULL_SESS			  0x00
#define DIAG_DF_SESS              0x01
#define DIAG_PROG_SESS            0x02
#define DIAG_EXT_SESS             0x04

#define DEFAULTSESSION			DIAG_DF_SESS
#define EXTENDEDSESSION         DIAG_EXT_SESS
#define REPROGRAMMINGSESSION    DIAG_PROG_SESS



#define DEFAULTSUBFUNC 		0x01
#define REPROGSUBFUNC 		0x02
#define EXTENDEDSUBFUNC 	0x03

#define NORESPONSE_MASK		((vnim_unsigned8)(0x80))

/* this marco just for debug without diagnotic core */
/* #define _DEBUG_WITOUT_DIAG_CORE */

/*****************************************************************************/
/* Private Constant Definitions                                              */
/*****************************************************************************/

#if( (KWPDIAG_RESPONSE_ON_FUNC_REQ == KWPDIAG_ENABLE) && \
     (KWPDIAG_RESPONSE_ALL_ON_FUNC_REQ == KWPDIAG_DISABLE))
#define KWPDIAG_SRV_INFO_TABLE_LIST                                                                               \
    {0x10, 0x50, (DIAG_DF_SESS | DIAG_PROG_SESS | DIAG_EXT_SESS ),  (DIAG_DF_SESS | DIAG_EXT_SESS )                 ,   KWPDIAG_ENABLE, {&kwpdiag_SessionHandler            , &ApplDiagStartDiagnosticSessionPosthandler}},\
	{0x11, 0x51, (DIAG_DF_SESS | DIAG_PROG_SESS | DIAG_EXT_SESS ),  (DIAG_DF_SESS | DIAG_PROG_SESS | DIAG_EXT_SESS ),   KWPDIAG_ENABLE,	{&ApplDiagEcuReset                  , &ApplDiagEcuResetPosthandler}},\
	{0x14, 0x54, (DIAG_DF_SESS | DIAG_EXT_SESS )                 , 	(DIAG_DF_SESS | DIAG_EXT_SESS)                  , 	KWPDIAG_ENABLE,	{&ApplDiagClearDiagnosticInformation, NULL}},\
    {0x19, 0x59, (DIAG_DF_SESS | DIAG_EXT_SESS )                 , 	(DIAG_DF_SESS | DIAG_EXT_SESS)                  , 	KWPDIAG_ENABLE, {&ApplDiagReadDTCInformation        , NULL}},\
    {0x22, 0x62, (DIAG_DF_SESS | DIAG_EXT_SESS )                 , 	(DIAG_DF_SESS | DIAG_EXT_SESS )                 , 	KWPDIAG_ENABLE, {&ApplDiagReadDataByCommonId        , NULL}},\
	/*{0x23, 0x63, (DIAG_EXT_SESS)                               ,	(DIAG_NULL_SESS)                                ,	KWPDIAG_DISABLE,{&ApplDiag_Read_Memory_by_Address   , NULL}},*/\
	{0x27, 0x67, (DIAG_EXT_SESS)                                 ,	(DIAG_NULL_SESS)                                ,	KWPDIAG_DISABLE,{&ApplDiagSecurityAccess            , NULL}},\
	{0x28, 0x68, (DIAG_EXT_SESS )                                ,	(DIAG_DF_SESS |DIAG_EXT_SESS)                   ,	KWPDIAG_ENABLE,	{&ApplDiag_CommunicatinControl      , NULL}},\
	{0x2E, 0x6E, (DIAG_EXT_SESS)                                 ,  (DIAG_NULL_SESS)                                ,   KWPDIAG_DISABLE,{&ApplDiagWriteDataByCommonId       , NULL}},\
	{0x2F, 0x6F, (DIAG_EXT_SESS)                                 , 	(DIAG_NULL_SESS)                                , 	KWPDIAG_DISABLE,{&ApplDiagInputOutputControlByLocalId, NULL}},\
    {0x31, 0x71, (DIAG_EXT_SESS)                                 ,  (DIAG_NULL_SESS)                                ,   KWPDIAG_DISABLE,{&ApplDiagStartRoutineByLocaldentifier, NULL}},\
    /*{0x3d, 0x7d, (DIAG_EXT_SESS)                               ,  (DIAG_NULL_SESS)                                ,   KWPDIAG_DISABLE,{&ApplDiag_Write_Memory_by_Address   , NULL}},*/\
    {0x3e, 0x7e, (DIAG_DF_SESS | DIAG_PROG_SESS | DIAG_EXT_SESS ),	(DIAG_DF_SESS | DIAG_EXT_SESS )                 ,   KWPDIAG_ENABLE,	{&kwpdiag_testerpresenthandler      , NULL}},\
	{0x85, 0xc5, (DIAG_EXT_SESS )                                ,	(DIAG_EXT_SESS)                                 ,	KWPDIAG_ENABLE,	{&ApplDiag_ControlDTCSetting        , NULL}},

#else
#define KWPDIAG_SRV_INFO_TABLE_LIST                                                                               \
    {0x10, 0x50, (DIAG_DF_SESS | DIAG_PROG_SESS | DIAG_EXT_SESS ),   (DIAG_DF_SESS | DIAG_EXT_SESS ),   	{&kwpdiag_SessionHandler, &ApplDiagStartDiagnosticSessionPosthandler}},\
	{0x11, 0x51, (DIAG_DF_SESS | DIAG_PROG_SESS | DIAG_EXT_SESS ),   (DIAG_DF_SESS | DIAG_PROG_SESS | DIAG_EXT_SESS ),   	{&ApplDiagEcuReset, &ApplDiagEcuResetPosthandler}},\
	{0x14, 0x54, (DIAG_DF_SESS | DIAG_EXT_SESS ), 	(DIAG_DF_SESS | DIAG_EXT_SESS), 	{&ApplDiagClearDiagnosticInformation, NULL}},\
    {0x19, 0x59, (DIAG_DF_SESS | DIAG_EXT_SESS ), 	(DIAG_DF_SESS | DIAG_EXT_SESS), 	{&ApplDiagReadDTCInformation, NULL}},\
    {0x22, 0x62, (DIAG_DF_SESS | DIAG_EXT_SESS ), 	(DIAG_DF_SESS | DIAG_EXT_SESS ), 	{&ApplDiagReadDataByCommonId, NULL}},\
	/*{0x23, 0x63, (DIAG_EXT_SESS),					(DIAG_NULL_SESS),	{&ApplDiag_Read_Memory_by_Address, NULL}},*/\
	{0x27, 0x67, (DIAG_EXT_SESS),					(DIAG_NULL_SESS),	{&ApplDiagSecurityAccess, NULL}},\
	{0x28, 0x68, (DIAG_EXT_SESS ),					(DIAG_EXT_SESS),	{&ApplDiag_CommunicatinControl, NULL}},\
	{0x2E, 0x6E, (DIAG_EXT_SESS),                   (DIAG_NULL_SESS),   {&ApplDiagWriteDataByCommonId, NULL}},\
	{0x2F, 0x6F, (DIAG_EXT_SESS), 				 	(DIAG_NULL_SESS), 	{&ApplDiagInputOutputControlByLocalId, NULL}},\
    {0x31, 0x71, (DIAG_EXT_SESS),                   (DIAG_NULL_SESS),   {&ApplDiagStartRoutineByLocaldentifier, NULL}},\
    /*{0x3d, 0x7d, (DIAG_EXT_SESS),                   (DIAG_NULL_SESS),   {&ApplDiag_Write_Memory_by_Address, NULL}},*/\
    {0x3e, 0x7e, (DIAG_DF_SESS | DIAG_PROG_SESS | DIAG_EXT_SESS ),	(DIAG_DF_SESS | DIAG_EXT_SESS ),  {&kwpdiag_testerpresenthandler, NULL}},\
	{0x85, 0xc5, (DIAG_EXT_SESS ),					(DIAG_EXT_SESS),	{&ApplDiag_ControlDTCSetting , NULL}},

#endif


UINT8 kwpdiag_enter_crtical(void);
UINT8 kwpdiag_exit_crtical(UINT8 ccr);

#define KWPDIAG_ENTER_CRITICAL_SECTION()   kwpdiag_enter_crtical()
#define KWPDIAG_EXIT_CRITICAL_SECTION(ccr)  kwpdiag_exit_crtical(ccr)

#if (VTP_NUM_CHANNEL_COUNT > 1)
#define KWPDIAG_TP_TRANSMIT(channel,ptr,len) VTP_Transmit(KWPDIAG_TPCHANNEL,ptr,len)
#define TpFuncResetChannel() VTP_Channel_Init(KWPDIAG_TPCHANNEL)
#define TpRxResetChannel() VTP_Channel_Init(KWPDIAG_TPCHANNEL)
#else
#define KWPDIAG_TP_TRANSMIT(channel,ptr,len) VTP_Transmit(ptr,len)
#define TpFuncResetChannel() VTP_Init()
#define TpRxResetChannel(channel) VTP_Init()
#endif

#endif /* KWPDIAG_CFG_H */

/* End of file ============================================================ */

/*****************************************************************************
C L E A R C A S E    R E V I S I O N     N O T E S
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
For each change to this file, be sure to record:
1.   Who made the change and when the change was made.
2.   Why the change was made and the intended result.

Date       By         Reason For Change
----------------------------------------------------------------------------
25-Feb-09
Clearcase Rev 1.0   ssaranya        Initial version.
26-Feb-09                            
Clearcase Rev 1.1   ssaranya        Peer Reveiw Changes.

26-Mar-09     2  vkannan      Updated with QAC fixes after using latest 
                                   personality files from integration team.

03-Apr-09     3  vkannan     Updated with Multi Channel TP driver interfaces

22-Apr-09   4      vkannan       1. To FIX QAC Level 2 warning 3211 
                                 "The global identifier '%s' is defined but not used."
                                 2. Update the services list based on the 
                                 Mahindra Diag spec after     Grand meeting


26-Jun-09     5  vkannan       Diag Service List updates

30-Jul-09     6  vkannan       Post handler support request-->TMS00101763

******************************************************************************/

