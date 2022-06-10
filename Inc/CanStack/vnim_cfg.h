#ifndef __VNIM_CFG_H
#define __VNIM_CFG_H

#include "types.h"
#include "vnim_mcan.cfg"

/****************************************************************************
 *
 *  Required VNIM Defines
 *
 ****************************************************************************/

/****************************************************************************
 * Specify the number of networks to be configured into VNIM
 ****************************************************************************/
#define NUM_VNIM_DEVICES  1

/****************************************************************************
 * Uncomment one or more lines corresponding to networks in use
 ****************************************************************************/
/*#define J1850_ENABLED
  #define DCX_ENABLED
  #define ACP_ENABLED
  #define NBUS_ENABLED
  #define CBUS_ENABLED
  #define GMLAN_ENABLED
  #define FNOS_ENABLED
  #define SICAN_ENABLED */
  #define MCAN_ENABLED


/****************************************************************************
 * Leave the following enum as is, except-Remove the comma from the last
 *    enabled network to prevent compile error
 ****************************************************************************/
typedef enum VDeviceNode
{
#ifdef ACP_ENABLED
  ACP_OUTPUT,
#endif
#ifdef FNOS_ENABLED
  FNOS
#endif
#ifdef DCX_ENABLED
  DCX_TYPE,
#endif
#ifdef J1850_ENABLED
  J1850_TYPE
#endif
#ifdef NBUS_ENABLED
  NBUS_TYPE
#endif
#ifdef CBUS_ENABLED
  CBUS_TYPE
#endif
#ifdef MCAN_ENABLED
  MCAN_TYPE
#endif
#ifdef SICAN_ENABLED
  SICAN_TYPE
#endif
#ifdef GMLAN_ENABLED
  GMLAN_TYPE
#endif
} VDeviceNode;

/****************************************************************************
 * Specify which network type should be considered the default
 ****************************************************************************/
#define VNIM_DEFAULT_DEVICE	 MCAN_TYPE

/****************************************************************************
 * Uncomment the following if using MMOS
 ****************************************************************************/
//#define VNIM_USING_MMOS

/****************************************************************************
 * Uncomment the following if the VNIM task is preemptive (not usual)
 ****************************************************************************/
/*#define VNIM_IS_PREEMPTIVE */

/****************************************************************************
 * Uncomment the following if using ProOsek
 ****************************************************************************/
//#define VNIM_USING_PROOSEK
#define VNIM_PROOSEK_START_TIME   2
#define VNIM_PROOSEK_TASK_TIME    1
#define event_type unsigned char

#ifndef VNIM_YES 
#define VNIM_YES          0x01
#endif

#ifndef VNIM_NO 
#define VNIM_NO           0x00
#endif
/****************************************************************************
 * Uncomment the following if using the FRED OS
 ****************************************************************************/
/*#define VNIM_USING_FRED
#define event_type EVENT_T */

/****************************************************************************
 * Specify which queue events into VNIM should be used
 *    Required in MMOS with non-preemptive VNIM task
 ****************************************************************************/
#define VNIM_QUEUE REGULAR_QUEUE

/****************************************************************************
 * Uncomment the appropriate application (list contains only a subset of
 *     possible apps.  Others did not need anything specific in the code)
 ****************************************************************************/
//#define GAP_APPLICATION

/****************************************************************************
 * Uncomment the appropriate hardware line
 *    Required in CAN applications
 ****************************************************************************/
//#define TMS470_HARDWARE
/*#define ST9_HARDWARE
#define STAR12_HARDWARE*/

/****************************************************************************
 * Uncomment the following line if the specific message database requires
 *    some type of periodic handling - this is not a usual condition
 ****************************************************************************/
/*#define SPECIFIC_APPL_TASK*/

/****************************************************************************
 * Uncomment the following line if transmit requests are to be accepted via
 *    events only
 *  Must be used in projects with VNIM Preemptive Task
 *  (31Oct2003- TX via Events not possible yet using PROOSEK)
 ****************************************************************************/
/*#define VNIM_TX_DATA_VIA_EVENT */

/****************************************************************************
 * Uncomment the following line if requests for setting data to be transmited
 *    are to be accepted via events only
 ****************************************************************************/
/*#define VNIM_SET_DATA_VIA_EVENT */

/****************************************************************************
 * Comment the following line if transmit requests are not handled by VNIM
 *    at all
 ****************************************************************************/
#define VNIM_ENABLE_TX

/****************************************************************************
 * Uncomment the following line if direct calls into VNIM to retrieve previously
 *    received data are allowed
 *  Must NOT be used in projects with VNIM Preemptive Task
 ****************************************************************************/
/*#define VNIM_ENABLE_RX*/

/****************************************************************************
 * Uncomment the following line if direct calls into VNIM to transmit stream
 *    data are allowed
 *  Must NOT be used in projects with VNIM Preemptive Task
 ****************************************************************************/
/*#define VNIM_ENABLE_TX_STREAMS*/

/****************************************************************************
 * Uncomment the following line if application components must register their
 *    need to keep the network awake
 ****************************************************************************/
//#define VNIM_REGISTER_CLIENTS

/****************************************************************************
 * Uncomment the following if back-to-back Tx message queueing is required
 *    WAIT_FOR_TXMSG_TIMEOUT is the max for signal waiting for Tx Msg req
 *    WAIT_FOR_TXMSG_ACK is the max for msg waiting for network ACK
 ****************************************************************************/
//#define VNIM_USING_MSG_QUEUEING
//#define WAIT_FOR_TXMSG_TIMEOUT  5
//#define WAIT_FOR_TXMSG_ACK	    5

/****************************************************************************
 * Uncomment the following line if the application is using the standard
 *    bookshelf wake manager.  Assign the wake manager header file name.
 ****************************************************************************/
/* #define VNIM_USING_WAKE_MANAGER	"wake_cfg.h" */

/****************************************************************************
 * Set to NO for no debugging
 ****************************************************************************/
#define VNIM_DEBUG_LOG	VNIM_NO

#if (VNIM_DEBUG_LOG == VNIM_YES)
   #define VNIM_TEST_LOG_INIT() vnim_test_log_event_init();
   #define VNIM_TEST_LOG_EVENT(a,b) vnim_test_log_event(VNIM_EVENT, a, b, 0, 0);
#else
   #define VNIM_TEST_LOG_INIT()
   #define VNIM_TEST_LOG_EVENT(a,b)
#endif

/****************************************************************************
 * Set to YES if emulator debugging is required
 ****************************************************************************/
#define VNIM_TEST  VNIM_NO

/****************************************************************************
 * Set to YES if logging of all wake states received in VNIM is required
 ****************************************************************************/
#define VNIM_WAKE_STATE_LOGGING     VNIM_NO

#if (VNIM_WAKE_STATE_LOGGING == VNIM_YES)
    #define VNIM_ANN_WAKE_LOG_TRANSITION(s) vnim_wake_log_transition(s);
    #define VNIM_ANN_WAKE_LOG_TRANSITION_INIT() vnim_wake_log_transition_init();
#else
    #define VNIM_ANN_WAKE_LOG_TRANSITION(s)
    #define VNIM_ANN_WAKE_LOG_TRANSITION_INIT()
#endif

/****************************************************************************
 *
 * VNIM announcements to application
 *	 Post event announcements if required by application
 *	 Note:  Follow each with semi-colon
 *
 ****************************************************************************/
#define VNIM_ANN_INIT_DONE()
#define VNIM_ANN_SHUTDOWN_DONE()
#define VNIM_ANN_FULL_RUN_ENTERED()
#define VNIM_ANN_SELF_ONLINE()
#define VNIM_ANN_SELF_OFFLINE()
#define VNIM_ANN_SELF_RESET()
#define VNIM_ANN_TX_START()
#define VNIM_ANN_SET_INIT_SIGNALS()
#ifndef VNIM_MCAN_RX_DATA
#define VNIM_MCAN_RX_DATA(a,b,c) ApplRxMsg(a,b,c)
#endif

/*******************************************************************************
 *
 * Message definitions
 *
 * 	Note:  Tx messages must be placed at the top followed by Rx Messages
 *******************************************************************************/

#define VMSG_NULL                               0

/*************************Channel:0 - 0x28***************************************/

#define VMSG_TX_CONFIG_SET			            1
#define VMSG_TX_CONFIG_STATUS		            2
#define VMSG_TX_HW_VERSION_GET		            3
#define VMSG_TX_SW_VERSION_GET  	            4
#define VMSG_TX_UNIT_CH_ITU_SET  	    		5
#define VMSG_TX_UNIT_CH_METER_STAT    			6
#define VMSG_TX_LANG_CH_METER_STAT				7
#define VMSG_TX_METER_LANG_GET					8
#define VMSG_TX_RADIO_IND             			9
#define VMSG_TX_INTERNET_RADIO_IND        		10
#define VMSG_TX_DISC_IND              			11
#define VMSG_TX_TV_IND                			12
#define VMSG_TX_AUX_IND               			13
#define VMSG_TX_AUDIO_WARN_IND        			14
#define VMSG_TX_HF_IND  	                	15
#define VMSG_TX_SMS_IND  	                	16
#define VMSG_TX_VR_IND  	                	17
#define VMSG_TX_IT_PWR_ON_KEEP_IND              18
#define VMSG_TX_VEH_PWR_STATE_GET				19

/*************************Channel:1 - 0x03***************************************/

#define VMSG_TX1_CONFIG_SET			            20
#define VMSG_TX1_CONFIG_STATUS		            21
#define VMSG_TX1_HW_VERSION_GET		            22
#define VMSG_TX1_SW_VERSION_GET  	            23
#define VMSG_TX1_TIME_INFO_IND             		24



/******************************************************************************/

/*************************Channel:0 - 0x28***************************************/

#define VMSG_RX_CONFIG_GET			    25
#define VMSG_RX_CONFIG_STAT			    26
#define VMSG_RX_HW_VERSION_STAT		    27
#define VMSG_RX_SW_VERSION_STAT  	    28
#define VMSG_RX_METER_SET_INFO_IND  	29
#define VMSG_RX_UNIT_CH_ITU_STAT  	    30
#define VMSG_RX_UNIT_CH_METER_SET  	    31
#define VMSG_RX_LANG_CHANGE_METER_SET	32
#define VMSG_RX_METER_LANG_STAT			33
#define VMSG_RX_VEH_POWER_STATE_STAT	34
#define VMSG_RX_VEH_POWER_STATE_IND		35

/*************************Channel:1 - 0x03***************************************/

#define VMSG_RX1_CONFIG_GET			    36
#define VMSG_RX1_CONFIG_STAT			37
#define VMSG_RX1_HW_VERSION_STAT		38
#define VMSG_RX1_SW_VERSION_STAT  	    39
#define VMSG_RX1_PUSH_SW_IND          	40
#define VMSG_RX1_FAIL_INFO_IND          41

/*******************************************************************************/
/*temp for existing 1.0 software */
/*#define VMSG_DISPLAY_INFO			VMSG_TX_POP_INFO */

/* for MCAN, this count is the number of Tx messages */
#define NUM_VNIM_MSGS    24      /* does not count VMSG_NULL */

/*******************************************************************************
 *
 * Transmit macros for this application
 *
 *******************************************************************************/

#ifndef VNIM_TX_DATA_VIA_EVENT
#endif



/***********************************************************************
 *
 * Macros contained in receive data callback routine for this application
 *   could be defined as blank, event calls, or calls directly into
 *   application code (not recommended)
 *
 ***********************************************************************/


/****************************************************************************
 * Uncomment the following line if either ENABLE_TX or ENABLE_RX is defined
 ****************************************************************************/
#define VNIM_ENABLE_MSG_LIST

#endif /* __VNIM_CFG_H */
