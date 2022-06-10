#include "user_init.h"
#include "Ioif.h"
#include "CanNmCtrl.h"

#define Seatback_B_Key   18
#define Seatback_F_Key   14
#define Seat_F_Key       8
#define Seat_B_Key       3
#define Seat_FastB_Key   16
#define Seat_FastF_Key   4
#define IGN_Status       13

/* Function Prototypes */
void CanNm_UserData_Fun(void);



/*******************************************************************************
* Function Name: CanNm_UserData_Fun
********************************************************************************
*
* Summary:
*  This API is called to Can Nm send user data
*
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void CanNm_UserData_Fun(void)
{
#if(SCM_SEATCONTROL_VARIANT == SCM_L_VARIANT)
	uint8_t Get_IGN_WakeupSrc = 0;
	uint8_t Get_Seatback_B_WakeupSrc = 0;
	uint8_t Get_Seatback_F_WakeupSrc = 0;
	uint8_t Get_Seat_B_WakeupSrc = 0;
	uint8_t Get_Seat_F_WakeupSrc = 0;
	uint8_t Get_Seat_FastB_WakeupSrc = 0;
	uint8_t Get_Seat_FastF_WakeupSrc = 0;
	uint8_t Get_EcuSpec_WakeupSrc = 0;

	Get_IGN_WakeupSrc =        Ioif_Get_WakeupSrc(IGN_Status);
	Get_Seatback_B_WakeupSrc = Ioif_Get_WakeupSrc(Seatback_B_Key);
	Get_Seatback_F_WakeupSrc = Ioif_Get_WakeupSrc(Seatback_F_Key);
	Get_Seat_B_WakeupSrc =     Ioif_Get_WakeupSrc(Seat_B_Key);
	Get_Seat_F_WakeupSrc =     Ioif_Get_WakeupSrc(Seat_F_Key);
	Get_Seat_FastB_WakeupSrc = Ioif_Get_WakeupSrc(Seat_FastB_Key);
	Get_Seat_FastF_WakeupSrc = Ioif_Get_WakeupSrc(Seat_FastF_Key);

	if( TRUE == Get_IGN_WakeupSrc )
	{
		CanNmCtrl_Set_ActiveWkupSrc(0, WKUPSRC_IGN, WKUP_ACTIVE);
	}
	else
	{
		CanNmCtrl_Set_ActiveWkupSrc(0, WKUPSRC_IGN, WKUP_INACTIVE);
	}

	if( (TRUE == Get_Seatback_B_WakeupSrc) || ( TRUE == Get_Seatback_F_WakeupSrc ) || ( TRUE == Get_Seat_B_WakeupSrc )
		|| (TRUE == Get_Seat_F_WakeupSrc) || (TRUE == Get_Seat_FastB_WakeupSrc) || ( TRUE == Get_Seat_FastF_WakeupSrc ) )
	{
		CanNmCtrl_Set_ActiveWkupSrc(0, WKUPSRC_ECU_SPEC, WKUP_ACTIVE);
	}
	else
	{
		CanNmCtrl_Set_ActiveWkupSrc(0, WKUPSRC_ECU_SPEC, WKUP_INACTIVE);
	}
	
#endif

}
