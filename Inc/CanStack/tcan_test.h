#ifndef TCAN_H
#define TCAN_H

#include "system.h"

/******************rx********************/

/*ID 515 rx*/
typedef struct {
	UINT8                       unused0_bit4_bit7: 5; 
	UINT8                       bit0_DVD_NFC_Req: 1;/*byte5*/
	UINT8                       bit1_DVD_NFC_connect_BM: 1;
	UINT8                       unused5_bit6_bit7: 1; 
	
	UINT8                       unused0:8;    /*byte0*/
	UINT8                       unused1:8;    /*byte1*/
	UINT8                       unused2:8;    /*byte2*/
	UINT8                       unused3:8;    /*byte3*/
	UINT8                       unused4:8;    /*byte4*/




	
	UINT8                       unused6: 8;   /*byte6*/
	
	UINT8                       unused7: 8;  /*byt7*/
	
}_RRM1_1_515_msgType_t;



/*ID 56D rx*/
typedef struct {
	UINT8                       BT_ADDR_BYTE0:8;    /*byte0*/
	UINT8                       BT_ADDR_BYTE1:8;    /*byte1*/
	UINT8                       BT_ADDR_BYTE2:8;    /*byte2*/
	UINT8                       BT_ADDR_BYTE3:8;    /*byte3*/
	UINT8                       BT_ADDR_BYTE4:8;    /*byte4*/
	UINT8                       BT_ADDR_BYTE5:8;    /*byte5*/
	
	UINT8                       unused6: 8;  /*byte6*/
	UINT8                       unused7: 8;  /*byte7*/

}_RRM_WPC_56D_msgType_t;



/*ID 3E6 rx*/
typedef struct {
	UINT8                       unused0:8;    /*byte0*/
	UINT8                       unused1:8;    /*byte1*/
	UINT8                       unused2:8;    /*byte2*/
	UINT8                       unused3:8;    /*byte3*/
	UINT8                       unused4:8;    /*byte4*/

	UINT8                       bit0_WPC_Sts: 2;/*byte5*/
	UINT8                       unused5_bit1_bit7: 6; 

	
	UINT8                       unused6: 8;   /*byte6*/
	UINT8                       unused7: 8;  /*byte7*/
}_BCM_PEPS_G_3E6_msgType_t;




typedef union {
	UINT8 byte[8];
	_RRM1_1_515_msgType_t ACU_515_msg;
}_RRM1_1_515_msg_buf_t;

typedef union {
	UINT8 byte[8];
	_RRM_WPC_56D_msgType_t ACU_56D_msg;
}_RRM_WPC_56D_msg_buf_t;

typedef union {
	UINT8 byte[8];
	_BCM_PEPS_G_3E6_msgType_t ACU_3E6_msg;
}_BCM_PEPS_G_3E6_msg_buf_t;


/*ID 460 tx*/
typedef struct {

	UINT8                       Bit0_Bit3_WPC_WirelessCharging_Status:4;    /*byte0*/
	UINT8                       Bit4_NFC_Status: 1;
    UINT8                       Bit5_NFC_FastConnectBMSts: 1;
    UINT8                       Bit6_NFC_FastConnectBTResult: 1;
	UINT8                       Bit7_unused0: 1; 

	UINT8                       WPC_Voltage_IN:8;    /*byte1*/

	UINT8                       WPC_Send_Power: 8; /*byte2*/

	UINT8                       WPC_Recv_Power: 8;   /*byte3*/

	UINT8                       unused4: 8;  /*byte4*/
	UINT8                       unused5: 8;  /*byte5*/
	UINT8                       unused6: 8;  /*byte6*/
	UINT8                       unused7: 8;  /*byte7*/

	
}_WPC_1_460_msgType_t;



typedef union {
	UINT8 byte[8];
	_WPC_1_460_msgType_t	WPC_460_msg;
}_WPC_1_460_msg_buf_t;



/*****************************************************************************
*                                 Global Function Prototypes                 *
******************************************************************************/
 void tcan_test_KSColdInit(void);
 void tcan_test_KSWarmInit(void);
 void tcan_test_task(void);


 u8 tcan_get_btconnection_is_reqon(void);
 u8 tcan_get_wpc_is_reqon(void);

#endif





