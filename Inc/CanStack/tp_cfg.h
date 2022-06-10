#ifndef TP_CFG_H
#define TP_CFG_H

#include "il_par.h"

#define VTP_CFG_VERSION         (0x0100)
/* ===========================================================================
//
//  Name:           tp_cfg.h
//
//  Description:    Configuration Input File for TP
//
//  Organization:   Network Subsystem
//
// =========================================================================*/

/* ===========================================================================
//
// Name:            VTP_NUM_CHANNEL_COUNT
//
// Description:     This parameter specifies the number of TP channels 
//                  used by the Application
//
// Template:        #define VTP_NUM_CHANNEL_COUNT  (3)
//
// Assumptions:     
//
// =========================================================================*/
#define VTP_NUM_CHANNEL_COUNT  (1)

/* ===========================================================================
//
// Name:            VTP_PERIODICITY
//
// Description:     This parameter specifies the TP periodic task schedule time
//                  in msec.This parameter defines the STmin for all TP channel
//
// Template:       #define VTP_PERIODICITY          (5)
//
// =========================================================================*/
#define VTP_PERIODICITY          (5)

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
/* ===========================================================================
//
// Name(s):         VTP_XXX_RESPONSE_ID
//
// Description:     This paramter specifies the TP Tx ID associated with 
//                  the TP channel XXX
//
// Templates:       #define VTP_DIAG_RESPONSE_ID                 (0xXXX)
//
// =========================================================================*/
/* The CAN Id for Diag RESPONSE from ECU */
#define VTP_DIAG_RESPONSE_ID                (0x751)

/* ===========================================================================
//
// Name(s):         VTP_XXX_REQUEST_ID
//
// Description:     This paramter specifies the TP Rx ID associated with 
//                  the TP channel XXX
//
// Templates:       #define VTP_DIAG_REQUEST_ID                 XXXX
//
// =========================================================================*/
/* The CAN Id for Diag Request from tool */
#define VTP_DIAG_REQUEST_ID                 (0x741)

#else

/* ===========================================================================
//
// Name(s):         VTP_XXX_RESPONSE_ID
//
// Description:     This paramter specifies the TP Tx ID associated with 
//                  the TP channel XXX
//
// Templates:       #define VTP_DIAG_RESPONSE_ID                 (0xXXX)
//
// =========================================================================*/
/* The CAN Id for Diag RESPONSE from ECU */
#define VTP_DIAG_RESPONSE_ID                (0x7A9)

/* ===========================================================================
//
// Name(s):         VTP_XXX_REQUEST_ID
//
// Description:     This paramter specifies the TP Rx ID associated with 
//                  the TP channel XXX
//
// Templates:       #define VTP_DIAG_REQUEST_ID                 XXXX
//
// =========================================================================*/
/* The CAN Id for Diag Request from tool */
#define VTP_DIAG_REQUEST_ID                 (0x7A1)

#endif
/* ===========================================================================
//
// Name:            VTP_FUNC_DIAG_REQUEST_ID
//
// Description:     This parameter specifies the Diagnostics Functional 
//                  Request ID
//
// Template:       #define VTP_FUNC_DIAG_REQUEST_ID            (0x7DF)
//
// =========================================================================*/
#define VTP_FUNC_DIAG_REQUEST_ID            (0x7DF)

/* ===========================================================================
//
// Name(s):         VTP_XXX_MAX_WAIT_CNT
//
// Description:     This paramter specifies the Number of successive 
                    FC with Wait Status accepted during data transfer.
//
// Templates:       #define VTP_DIAG_MAX_WAIT_CNT          (2)
//
// =========================================================================*/
#define VTP_DIAG_MAX_WAIT_CNT               (0x00)

/* ===========================================================================
//
// Name(s):         VTP_XXX_BLOCKSIZE
//
// Description:     The block size of TP frame during Transmission.
//                  Block size zero mentions no BS used by TP
//
// Templates:       #define VTP_DIAG_BLOCKSIZE          (0x00)
//
// =========================================================================*/

#define VTP_DIAG_BLOCKSIZE                  0x08/*(0x00)*/      

/* ===========================================================================
//
// Name(s):         VTP_XXX_STMIN
//
// Description:     The STmin of TP frame during Reception.
//                  Block size zero mentions no BS used by TP
//
// Templates:       #define VTP_DIAG_STMIN          (0x00)
//
// =========================================================================*/

#define VTP_DIAG_STMIN                      (20)/*(0x00)*/

/* ===========================================================================
//
// Name(s):         VTP_XXX_TX_CF_TIME_INMSEC
//
// Description:     Time out value for transmit Consecutive Frame for 
//                  the TP channel XXX
//
// Templates:       #define VTP_DIAG_TX_CF_TIME_INMSEC          (35)
//
// =========================================================================*/

#define VTP_DIAG_TX_CF_TIME_INMSEC           (70)            

/* ===========================================================================
//
// Name(s):         VTP_XXX_WAIT_FC_TIME_INMSEC
//
// Description:     Time out value for waiting for FLow Control Frame for 
//                  the TP channel XXX
//
// Templates:       #define VTP_DIAG_WAIT_FC_TIME_INMSEC        (150)
//
// =========================================================================*/
#define VTP_DIAG_WAIT_FC_TIME_INMSEC            /*(150)*/(1000)       

/* ===========================================================================
//
// Name(s):         VTP_XXX_WAIT_CF_TIME_INMSEC
//
// Description:     Time out value for waiting for Consecutive Frame for 
//                  the TP channel XXX
//
// Templates:       #define VTP_DIAG_WAIT_CF_TIME_INMSEC        (150)
//
// =========================================================================*/

#define VTP_DIAG_WAIT_CF_TIME_INMSEC            (150)/*(1000)*/      

/* ===========================================================================
//
// Name(s):         VTP_XXX_WAIT_CF_TIME_INMSEC
//
// Description:     Time out value for waiting for Confirmation to Txed message
//                  for TP channel XXX
//
// Templates:       #define VTP_DIAG_TX_WAIT_CONF_TIME_INMSEC        (70)
//
// =========================================================================*/

#define VTP_DIAG_TX_WAIT_CONF_TIME_INMSEC       (70)/*(1000)*/    

/* ===========================================================================
//
// Name(s):         VTP_XXX_MESSAGE_HANDLE
//
// Description:     The CAN Transmit message handle to be used.
//
// Templates:       #define VTP_DIAG_MESSAGE_HANDLE             (0x01)
//
// =========================================================================*/
/* Number of IL Message handles + 1 */
#define VTP_DIAG_MESSAGE_HANDLE             (0x7)

#define   TP_TIME_IN_TASK_TICS( tMs )  ((UINT16) (((tMs)/VTP_PERIODICITY)))

/* ===========================================================================
//
// Name(s):         VTPMC_RESPONSE_ID
//
// Description:     This paramter specifies the TP Tx ID associated with 
//                  all the TP channels. This list should hold the TP Tx IDs 
//                  of all the channels in the order of Connections
//
// =========================================================================*/

#define VTPMC_RESPONSE_ID               \
        VTP_DIAG_RESPONSE_ID

/* ===========================================================================
//
// Name(s):         VTPMC_REQUEST_ID
//
// Description:     This paramter specifies the TP RX ID associated with 
//                  all the TP channels. This list should hold the TP RX IDs 
//                  of all the channels in the order of Connections
//
// =========================================================================*/

#define VTPMC_REQUEST_ID          \
        VTP_DIAG_REQUEST_ID

/* ===========================================================================
//
// Name(s):         VTPMC_MESSAGE_HANDLE
//
// Description:     This paramter specifies the CAN Transmit Message Handle 
//                  associated with all the TP channels. 
//                  This list should hold the CAN Transmit Message Handle 
//                  of all the channels in the order of Connections
//
// =========================================================================*/

#define VTPMC_MESSAGE_HANDLE          \
        VTP_DIAG_MESSAGE_HANDLE

/* ===========================================================================
//
// Name(s):         VTPMC_MAX_WAIT_CNT
//
// Description:     This paramter specifies the Number of successive 
//                  FC with Wait Status accepted during data transfer for 
//                  all the channels.
//                  This list should hold the Max wair count parameter 
//                  of all the channels in the order of Connections
//
// =========================================================================*/

#define VTPMC_MAX_WAIT_CNT          \
        VTP_DIAG_MAX_WAIT_CNT

/* ===========================================================================
//
// Name(s):         VTPMC_BLOCKSIZE
//
// Description:     The block size of TP frame during Transmission.
//                  Block size zero mentions no BS used by TP.
//                  This list should hold the Block size parameter 
//                  of all the channels in the order of Connections
//
// =========================================================================*/
#define VTPMC_BLOCKSIZE           \
        VTP_DIAG_BLOCKSIZE

/* ===========================================================================
//
// Name(s):         VTPMC_STMIN
//
// Description:     The STmin of TP frame during Reception.
//                  Block size zero mentions no BS used by TP
//
// =========================================================================*/
#define VTPMC_STMIN           \
        VTP_DIAG_STMIN

/* ===========================================================================
//
// Name(s):         VTPMC_TX_CF_TIMECNT
//
// Description:     Time out value for transmit Consecutive Frame time Count 
//                  value for all the TP channels
//                  This list should hold the Consecutive frame wait count
//                   parameter of all the channels in the order of Connections
//
// =========================================================================*/
#define VTPMC_TX_CF_TIMECNT                                     \
        TP_TIME_IN_TASK_TICS(VTP_DIAG_TX_CF_TIME_INMSEC)

/* ===========================================================================
//
// Name(s):         VTPMC_WAIT_FC_TIMECNT
//
// Description:     Time out value for Flow Control wait  time Count 
//                  value for all the TP channels
//                  This list should hold Flow Control wait count
//                  parameter of all the channels in the order of Connections
//
// =========================================================================*/

#define VTPMC_WAIT_FC_TIMECNT                                     \
        TP_TIME_IN_TASK_TICS(VTP_DIAG_WAIT_FC_TIME_INMSEC)

/* ===========================================================================
//
// Name(s):         VTPMC_WAIT_CF_TIMECNT
//
// Description:     Time out value for Consecutive Frame wait  time Count 
//                  value for all the TP channels
//                  This list should hold Consecutive Frame wait count
//                  parameter of all the channels in the order of Connections
//
// =========================================================================*/
#define VTPMC_WAIT_CF_TIMECNT                                     \
        TP_TIME_IN_TASK_TICS(VTP_DIAG_WAIT_CF_TIME_INMSEC)

/* ===========================================================================
//
// Name(s):         VTPMC_TX_WAIT_CONF_TIMECNT
//
// Description:     Time out value for Tx Confirmation wait time Count 
//                  value for all the TP channels
//                  This list should hold Tx Confirmation wait time count
//                  parameter of all the channels in the order of Connections
//
// =========================================================================*/

#define VTPMC_TX_WAIT_CONF_TIMECNT                                   \
        TP_TIME_IN_TASK_TICS(VTP_DIAG_TX_WAIT_CONF_TIME_INMSEC)

/* ===========================================================================
//
// Name(s):         VTPMC_TX_CONFIRMATION_FUNC
//
// Description:     This function pointer list holds all the TP Channels 
//                  Tx Confirmation function of type 
//                  void  (*tp_tx_confirmation_func_t ) (unsigned char);
//                  This list should hold Tx Confirmation fn 
//                  of all the channels in the order of Connections
//
// =========================================================================*/
#define VTPMC_TX_CONFIRMATION_FUNC     \
        &kwpdiag_confirmation

/* ===========================================================================
//
// Name(s):         VTPMC_RX_ERROR_IND_FUNC
//
// Description:     This function pointer list holds all the TP Channels 
//                  RX Error Indication function of type 
//                  void  (*tp_rx_error_ind_func_t ) (unsigned char);
//                  This list should hold RX Error Indication fn 
//                  of all the channels in the order of Connections
//
// =========================================================================*/
#define VTPMC_RX_ERROR_IND_FUNC        \
        &kwpdiag_rxerrorind

/* ===========================================================================
//
// Name(s):         VTPMC_TX_ERROR_IND_FUNC
//
// Description:     This function pointer list holds all the TP Channels 
//                  TX Error Indication function of type 
//                  void  (*tp_tx_error_ind_func_t ) (unsigned char);
//                  This list should hold TX Error Indication fn 
//                  of all the channels in the order of Connections
//
// =========================================================================*/

#define VTPMC_TX_ERROR_IND_FUNC        \
        &kwpdiag_txerrorind

/* ===========================================================================
//
// Name(s):         VTPMC_RX_IND_FUNC
//
// Description:     This function pointer list holds all the TP Channels 
//                  Rx Indication function of type 
//                  void  (*tp_rx_indication_func_t ) (unsigned short);
//                  This list should hold Rx Indication fn 
//                  of all the channels in the order of Connections
//
// =========================================================================*/
#define VTPMC_RX_IND_FUNC              \
        &kwpdiag_physicalreqind

/* ===========================================================================
//
// Name(s):         VTPMC_RX_GET_BUFFER_FUNC
//
// Description:     This function pointer list holds all the TP Channels 
//                  Rx get buffer function of type 
//                  unsigned char * (*tp_rx_getbuffer_func_t ) (unsigned short);
//                  This list should hold Rx get buffer fn 
//                  of all the channels in the order of Connections
//
// =========================================================================*/
#define VTPMC_RX_GET_BUFFER_FUNC       \
        &kwpdiag_physicalgetbuffer

#endif

/*******************************************************************************
================================================================================
DATE           AUTHOR      VERSION  DESCRIPTION
================================================================================
03-Apr-09      vkannan     1      Initial version

17-Apr-09      vkannan     2      After Peer review Comments of 20077

07-May-09      vkannan     3      Udated for VNIM Interfaces to Multi Channel TP

19-Feb-10      hsubha1     4      Updated DIAG REQUEST msg as per V2.2 DBC

================================================================================
*******************************************************************************/
/* End of File =============================================================*/


