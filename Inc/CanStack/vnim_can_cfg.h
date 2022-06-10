#if !defined(VNIM_CAN_CFG_H)
#define VNIM_CAN_CFG_H

/* ===========================================================================
//  P U B L I C   M A C R O S
// =========================================================================*/

/* ===========================================================================
//  Define Microcontroller Port Pin Connections to the CAN Transceiver
// =========================================================================*/

#define IL_TASK_PERIOD_MS      (5)

#define VNIM_PERIODIC_TASK_IN_MSEC  (5)

#define VNIM_BUSOFF_SCHEDULER_IN_MS  (4)

#define NW_NUM_FAST_BUSOFF  (5)        /* fast bus off recovery times */

#define NW_NUM_FAST_BUSOFF_DTC  (2)     /* busoff DTC cnt */

#define NW_FAST_BUSOFF_TIMECNT (100)    /* 24 ~= 50ms/4 */

//#define NW_SLOW_BUSOFF_TIMECNT (200/VNIM_BUSOFF_SCHEDULER_IN_MS)   /* 50 ~= 200ms/4 */

#define NW_SLOW_BUSOFF_TIMECNT 1000

#define NW_TASK_4MS_SCHEDULER (4)

#endif /* VNIM_CAN_CFG_H */

/* End of file ============================================================ */
