#ifndef NW_TP_PAR_H
#define NW_TP_PAR_H

#include "tp_cfg.h"
#include "system.h"

#if (VTP_NUM_CHANNEL_COUNT > 1)
extern VTP_UINT32 const VTPmc_Response_ID[VTP_NUM_CHANNEL_COUNT];
extern VTP_UINT32 const VTPmc_Request_ID[VTP_NUM_CHANNEL_COUNT];
extern const VTP_UINT8  VTPmc_Message_Handle[VTP_NUM_CHANNEL_COUNT];
extern VTP_UINT8 const VTPmc_TxBlock_Size[VTP_NUM_CHANNEL_COUNT];
extern VTP_UINT8 const  VTPmc_Max_Wait_Cnt[VTP_NUM_CHANNEL_COUNT];
extern VTP_UINT16 const VTPmc_Tx_CF_Timecnt[VTP_NUM_CHANNEL_COUNT];
extern VTP_UINT16 const VTPmc_Wait_FC_Timecnt[VTP_NUM_CHANNEL_COUNT];
extern VTP_UINT16 const VTPmc_Wait_CF_Timecnt[VTP_NUM_CHANNEL_COUNT];
extern VTP_UINT16 const VTPmc_Wait_TXConf_Timecnt[VTP_NUM_CHANNEL_COUNT];

#define VTP_RESPONSE_ID                 VTPmc_Response_ID[channel]
#define VTP_REQUEST_ID                  VTPmc_Request_ID[channel]
#define VTP_MAX_WAIT_CNT                VTPmc_Max_Wait_Cnt[channel]
#define VTP_BLOCKSIZE                   VTPmc_TxBlock_Size[channel]
#define VTP_STMIN                       VTPmc_TxSTmin[channel]
#define VTP_MESSAGE_HANDLE              VTPmc_Message_Handle[channel]
extern VTP_UINT8 VTP_Call_DllTransmit (VTP_UINT8 channel);
#else

#define VTP_RESPONSE_ID                 VTP_DIAG_RESPONSE_ID
#define VTP_REQUEST_ID                  VTP_DIAG_REQUEST_ID
#define VTP_MAX_WAIT_CNT                VTP_DIAG_MAX_WAIT_CNT
#define VTP_BLOCKSIZE                   VTP_DIAG_BLOCKSIZE
#define VTP_STMIN                       VTP_DIAG_STMIN
#define VTP_TX_CF_TIME_INMSEC           VTP_DIAG_TX_CF_TIME_INMSEC
#define VTP_WAIT_FC_TIME_INMSEC         VTP_DIAG_WAIT_FC_TIME_INMSEC
#define VTP_WAIT_CF_TIME_INMSEC         VTP_DIAG_WAIT_CF_TIME_INMSEC
#define VTP_TX_WAIT_CONF_TIME_INMSEC    VTP_DIAG_TX_WAIT_CONF_TIME_INMSEC
#define VTP_MESSAGE_HANDLE              VTP_DIAG_MESSAGE_HANDLE
extern UINT8 VTP_Call_DllTransmit (void);
#endif

#if (VTP_NUM_CHANNEL_COUNT > 1)
typedef void  (*tp_tx_confirmation_func_t ) (unsigned char);
typedef void  (*tp_tx_error_ind_func_t ) (unsigned char);
typedef void  (*tp_rx_error_ind_func_t ) (unsigned char);
typedef void  (*tp_rx_indication_func_t ) (unsigned short);
typedef unsigned char * (*tp_rx_getbuffer_func_t ) (unsigned short);

extern tp_tx_confirmation_func_t const  ApplVTP_tx_confirmation[VTP_NUM_CHANNEL_COUNT];
extern tp_tx_error_ind_func_t    const  ApplVTP_tx_error_indication[VTP_NUM_CHANNEL_COUNT];
extern tp_rx_error_ind_func_t    const  ApplVTP_rx_error_indication[VTP_NUM_CHANNEL_COUNT];
extern tp_rx_indication_func_t   const  ApplVTP_rx_indication[VTP_NUM_CHANNEL_COUNT];
extern tp_rx_getbuffer_func_t    const  ApplVTP_rx_get_buffer[VTP_NUM_CHANNEL_COUNT];
#endif

extern UINT8 VTP_Diag_PhyID_Flag;
UINT8 get_diag_addressing(void);


extern void kwpdiag_confirmation(unsigned char state);
extern void kwpdiag_txerrorind(unsigned char state);
extern void kwpdiag_physicalreqind(unsigned short dataLen);
extern void kwpdiag_rxerrorind(unsigned char state);
extern unsigned char* kwpdiag_physicalgetbuffer(unsigned short dataLength);



extern unsigned char * kwpdiag_functionalgetbuffer (unsigned short dataLength);
extern void kwpdiag_functionalreqind(unsigned short dataLength);
extern void VTP_Hook_Receive( CAN_RMD const * const, UINT16 const );/* TP Layer */
extern UINT8 VTP_Hook_Transmit (UINT16 hTxMsg);


#endif
