#include "user_init.h"
#include "Adcif.h"
#include "PowerMgmt.h"
#include "CddKey_Cfg.h"
#include "CddMtr_Mng.h"
#include "rx_can.h"


#define VEH_SPEED  5
#define MOTOR_POS_MIN  1000
#define MOTOR_POS_MIN1 10
#define MOTOR_POS_MAX1 90
#define MOTOR_POS_MAX  9000
#define MOTOR_HARD_POS_MIN 500
#define MOTOR_HARD_POS_MAX 9500

/* Function Prototypes */
void Scm_SeatControl_App(void);
void Scm_SeatControl_Init(void);
UINT8 Get_ForwardBack_Current_Pos(void);
UINT8 Get_SeatBack_Current_Pos(void);
UINT8 Get_SeatSwtichSts(void);
UINT8 Get_SeatMotor_SoftPoint_Min(void);
UINT8 Get_SeatMotor_SoftPoint_Max(void);
UINT8 Get_SeatBackMotor_SoftPoint_Min(void);  /* Motor software Point */
UINT8 Get_SeatBackMotor_SoftPoint_Max(void);


/* Static Function Prototypes */
static void Scm_SeatLearn_Func(void);
static UINT8 Scm_GetKey_Func(void);
static void Scm_SeatControl_Func(void);
static BOOLEAN Scm_Get_Target_Func(uint8_t fl_Mtr_Id,uint32_t fl_percent_u32);
static UINT8 Scm_GetSeatB_Key_Func(void);
static UINT8 Scm_GetSeatF_Key_Func(void);
static UINT8 Scm_GetSeatBackB_Key_Func(void);
static UINT8 Scm_GetSeatBackf_Key_Func(void);


/* Static Variables */
static UINT8 SCM_DrvSeatMotoPos_ForwardBack = 0;
static UINT8 SCM_DrvSeatMotoPos_SeatBack = 0;
static UINT8 SCM_DrvSeatMotoDirection_ForwardBack = 0;
static UINT8 SCM_DrvSeatMotoDirection_SeatBack = 0;
static UINT8 SCM_DrvSeatLocalCtrlSwithSts = 0;
static UINT8 Forword_Back_Rest = 0;
static UINT8 Seat_Back_Rest = 0;
static BOOLEAN Motor_Learn_Start = 0;
static UINT8 Seat_Manual_Mode = 0;
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
static UINT8 Seat_EasyStatus = 0;
static UINT8 Seat_EntryStatus = 0;
static UINT8 PreGet_FastF_Key = 0;
static UINT8 PreGet_FastB_Key = 0;
static UINT8 PreGetIHU_SeatFoldReq = 0;
static UINT8 PreGetIHU_SeatReleaseReq = 0;
UINT8 Dis_SCM_HMI_Req = 0;
UINT8 Dis_SCM_HMI_Req_Count = 0;
#endif
static UINT8 PreSeatB_Key = 0;
static UINT8 PreSeatF_Key= 0;
static UINT8 PreSeatBackB_Key= 0;
static UINT8 PreSeatBackF_Key= 0;
static UINT8 SeatB_HallFault_Count = 0;
static UINT8 SeatF_HallFault_Count = 0;
static UINT8 SeatBackB_HallFault_Count = 0;
static UINT8 SeatBackF_HallFault_Count = 0;

/* Gobal Variables */
UINT8 Get_Current_ForwardBackMotor_Dir = 0;
UINT8 Get_Current_SeatBackMotor_Dir = 0;

/* Variables */
UINT8 PreForword_Back_TargetPos = 0;
UINT8 PreSeat_Back_TargetPos = 0;
UINT8 PreSeat_FastF_Key = 0;
UINT8 Seat_FastF_Key_Count = 0;
UINT8 Seat_StduyEnevt_flag = 0;
UINT16 Seat_StduyTime = 500;
UINT8 Seat_StduyEnevt = 0;


void Scm_SeatControl_App(void)
{
    //Scm_GetKey_Func();
    Scm_SeatLearn_Func();
    Scm_SeatControl_Func();
}

/*******************************************************************************
* Function Name: Scm_SeatControl_Init
********************************************************************************
*
* Summary:
*  This API is called to Scm_SeatControl_Init
*
* Parameters:
*  None
*
* Return:
*  Forward Back Position
*
*******************************************************************************/
void Scm_SeatControl_Init(void)
{
    PreSeat_FastF_Key = 0;
    Seat_StduyEnevt = 0;
    PreForword_Back_TargetPos = 0;
    PreSeat_Back_TargetPos = 0;
    Forword_Back_Rest = 0;
    Seat_Back_Rest = 0;
    Motor_Learn_Start = 0;
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
    Seat_EasyStatus = 0;
    Seat_EntryStatus = 0;
    Dis_SCM_HMI_Req = 0;
    Dis_SCM_HMI_Req_Count = 0;
#endif
}


/*******************************************************************************
* Function Name: Scm_SeatLearn_Func
********************************************************************************
*
* Summary:
*  This API is called to Scm_SeatLearn_Func
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Scm_SeatLearn_Func(void)
{
    UINT8 Seat_FastF_Key = 0;

    static UINT8 Motor_Learn_Start_Count = 0;

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
    Seat_FastF_Key = ServSWM_u_GetKey_Seat_FastF_SW_RealVaule();
#else
    Seat_FastF_Key = ServSWM_u_GetKey_Seatback_F_SW_RealVaule();
#endif

    if( (FALSE == Get_Voltage_Flag()) )
    {
        /* Learn request */
        if(ServSWM_u_GetKey_Seat_F_SW_RealVaule())
        {
            if(Seat_StduyEnevt <= 100)
            {
                Seat_StduyEnevt ++;
                if(Seat_StduyEnevt == 100)
                {
                    Seat_StduyEnevt_flag = 1;
                }
            }
        }
        else
        {
            Seat_StduyEnevt = 0;
        }

        if( 1 == Seat_StduyEnevt_flag )
        {
            if( Seat_StduyTime > 0 )
            {
                Seat_StduyTime = Seat_StduyTime -1;
                if( (PreSeat_FastF_Key != Seat_FastF_Key) && (Seat_FastF_Key == 1) )
                {
                    Seat_FastF_Key_Count = Seat_FastF_Key_Count + 1;
                }
            }
            else
            {
                Seat_StduyEnevt_flag = 0;
                Seat_StduyTime = 500;
                if( Seat_FastF_Key_Count < 3 )
                {
                    Seat_FastF_Key_Count = 0;
                }
            }
        }
        if( Seat_FastF_Key_Count == 3 )
        {
            Motor_Learn_Start = 1;
            CddMtr_Learn_Req(0,CDDMTR_MNG_LEARN_ENABLE );
            CddMtr_Learn_Req(1,CDDMTR_MNG_LEARN_ENABLE );
            Seat_StduyEnevt_flag = 0;
            Seat_FastF_Key_Count = 0;
        }
        PreSeat_FastF_Key = Seat_FastF_Key;
    }

    if( ((CDDMTR_MNG_LEARN_VALID == CddMtr_Get_LearnData_Status(0)) && ( CDDMTR_MNG_LEARN_VALID == CddMtr_Get_LearnData_Status(1) ) )
    && (1 == Motor_Learn_Start) )
    {
        Seat_StduyTime = 500;
        Motor_Learn_Start = 0;
        Seat_StduyEnevt_flag = 0;
        CddMtr_Percent_Req(0, 5000);
        CddMtr_Percent_Req(1, 5000);
    }

    if(1 == Motor_Learn_Start)
    {
        if( Motor_Learn_Start_Count < 0xFF )
        {
            Motor_Learn_Start_Count ++;
        }
    }

#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
    if( (1 == Motor_Learn_Start) && ( Motor_Learn_Start_Count >= 150 ) && ( (1 == Scm_GetKey_Func()) || (TRUE == Get_Voltage_Flag()) || (1 == ABM_PassengerSeatOccSts) ||((0 != VCU_VehSpd) && (VCU_VehSpd_VD == 0x0)) ))
#else
    if( (1 == Motor_Learn_Start) && (Motor_Learn_Start_Count >= 150) && ( (1 == Scm_GetKey_Func()) || (TRUE == Get_Voltage_Flag()) ))
#endif
    {
        Seat_StduyTime = 500;
        Motor_Learn_Start = 0;
        Seat_StduyEnevt_flag = 0;
        Motor_Learn_Start_Count = 0;
        CddMtr_Learn_Req(0,CDDMTR_MNG_LEARN_DISABLE );
        CddMtr_Learn_Req(1,CDDMTR_MNG_LEARN_DISABLE );

    }

}

/*******************************************************************************
* Function Name: Scm_GetKey_Sts
********************************************************************************
*
* Summary:
*  This API is called to Scm_GetKey_Sts
*
* Parameters:
*  None
*
* Return:
*  None

*******************************************************************************/
UINT8 Scm_GetKey_Func(void)
{
    UINT8 Get_KeySts = 0;
    UINT8 GetKey_Seat_B_SW_RealVaule = ServSWM_u_GetKey_Seat_B_SW_RealVaule();
    UINT8 GetKey_Seat_F_SW_RealVaule = ServSWM_u_GetKey_Seat_F_SW_RealVaule();
    UINT8 GetKey_Seatback_B_SW_RealVaule = ServSWM_u_GetKey_Seatback_B_SW_RealVaule();
    UINT8 GetKey_Seatback_F_SW_RealVaule = ServSWM_u_GetKey_Seatback_F_SW_RealVaule();
    UINT8 GetKey_Seat_FastF_SW_RealVaule = ServSWM_u_GetKey_Seat_FastF_SW_RealVaule();
    UINT8 GetKey_Seat_FastB_SW_RealVaule = ServSWM_u_GetKey_Seat_FastB_SW_RealVaule();

    static UINT8 Pre_GetKey_Seat_B_SW_RealVaule = 0;
    static UINT8 Pre_GetKey_Seat_F_SW_RealVaule = 0;
    static UINT8 Pre_GetKey_Seatback_B_SW_RealVaule = 0;
    static UINT8 Pre_GetKey_Seatback_F_SW_RealVaule = 0;
    static UINT8 Pre_GetKey_Seat_FastF_SW_RealVaule = 0;
    static UINT8 Pre_GetKey_Seat_FastB_SW_RealVaule = 0;

    if(\
       ((Pre_GetKey_Seat_B_SW_RealVaule != GetKey_Seat_B_SW_RealVaule) && ( 1 == GetKey_Seat_B_SW_RealVaule ))\
    || ((Pre_GetKey_Seat_F_SW_RealVaule != GetKey_Seat_F_SW_RealVaule) && ( 1 == GetKey_Seat_F_SW_RealVaule ))\
    || ((Pre_GetKey_Seatback_B_SW_RealVaule != GetKey_Seatback_B_SW_RealVaule) && ( 1 == GetKey_Seatback_B_SW_RealVaule ))\
    || ((Pre_GetKey_Seatback_F_SW_RealVaule != GetKey_Seatback_F_SW_RealVaule) && ( 1 == GetKey_Seatback_F_SW_RealVaule ))\
    || ((Pre_GetKey_Seat_FastF_SW_RealVaule != GetKey_Seat_FastF_SW_RealVaule) && ( 1 == GetKey_Seat_FastF_SW_RealVaule ))\
    || ((Pre_GetKey_Seat_FastB_SW_RealVaule != GetKey_Seat_FastB_SW_RealVaule) && ( 1 == GetKey_Seat_FastB_SW_RealVaule ))\
    )
    {
        Get_KeySts = 1;
    }
    else
    {
        Get_KeySts = 0;
    }

    Pre_GetKey_Seat_B_SW_RealVaule = GetKey_Seat_B_SW_RealVaule;
    Pre_GetKey_Seat_F_SW_RealVaule = GetKey_Seat_F_SW_RealVaule;
    Pre_GetKey_Seatback_B_SW_RealVaule = GetKey_Seatback_B_SW_RealVaule;
    Pre_GetKey_Seatback_F_SW_RealVaule = GetKey_Seatback_F_SW_RealVaule;
    Pre_GetKey_Seat_FastF_SW_RealVaule = GetKey_Seat_FastF_SW_RealVaule;
    Pre_GetKey_Seat_FastB_SW_RealVaule = GetKey_Seat_FastB_SW_RealVaule;

    return(Get_KeySts);
}

/*******************************************************************************
* Summary:
*  This API is get all key status. 
*  
*******************************************************************************/
BOOLEAN Scm_Get_Target_Func(uint8_t fl_Mtr_Id,uint32_t fl_percent_u32)
{
    UINT32 Get_Current_Pos = 0;

    Get_Current_Pos = CddMtr_Get_Mtr_PosPercent(fl_Mtr_Id)*100;
    if( Get_Current_Pos == fl_percent_u32 )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

/*******************************************************************************
* Summary:
*  This API is get Seat Back key status. 
*  
*******************************************************************************/
UINT8 Scm_GetSeatB_Key_Func(void)
{
    UINT8 Return_Sts = 0;

    UINT8 SeatB_Key = ServSWM_u_GetKey_Seat_B_SW_RealVaule();

    if( ( PreSeatB_Key != SeatB_Key ) && ( SeatB_Key == 1 ) )
    {
        Return_Sts = 1;
    }
    else if( ( PreSeatB_Key != SeatB_Key ) && ( SeatB_Key == 0 ) )
    {
        Return_Sts = 2;
    }
    else
    {
        Return_Sts = 0;
    }
    PreSeatB_Key = SeatB_Key;

    return (Return_Sts);

}

/*******************************************************************************
* Summary:
*  This API is get Seat forward key status. 
*  
*******************************************************************************/
UINT8 Scm_GetSeatF_Key_Func(void)
{
    UINT8 Return_Sts = 0;

    UINT8 SeatF_Key = ServSWM_u_GetKey_Seat_F_SW_RealVaule();

    if( ( PreSeatF_Key != SeatF_Key ) && ( SeatF_Key == 1 ) )
    {
        Return_Sts = 1;
    }
    else if( ( PreSeatF_Key != SeatF_Key ) && ( SeatF_Key == 0 ) )
    {
        Return_Sts = 2;
    }
    else
    {
        Return_Sts = 0;
    }
    PreSeatF_Key = SeatF_Key;

    return (Return_Sts);

}
/*******************************************************************************
* Summary:
*  This API is get Seatback back key status. 
*  
*******************************************************************************/
UINT8 Scm_GetSeatBackB_Key_Func(void)
{
    UINT8 Return_Sts = 0;

    UINT8 SeatBackB_Key = ServSWM_u_GetKey_Seatback_B_SW_RealVaule();

    if( ( PreSeatBackB_Key != SeatBackB_Key ) && ( SeatBackB_Key == 1 ) )
    {
        Return_Sts = 1;
    }
    else if( ( PreSeatBackB_Key != SeatBackB_Key ) && ( SeatBackB_Key == 0 ) )
    {
        Return_Sts = 2;
    }
    else
    {
        Return_Sts = 0;
    }
    PreSeatBackB_Key = SeatBackB_Key;

    return (Return_Sts);

}
/*******************************************************************************
* Summary:
*  This API is get Seatback Forward key status. 
*  
*******************************************************************************/
UINT8 Scm_GetSeatBackF_Key_Func(void)
{
    UINT8 Return_Sts = 0;

    UINT8 SeatBackF_Key = ServSWM_u_GetKey_Seatback_F_SW_RealVaule();

    if( ( PreSeatBackF_Key != SeatBackF_Key ) && ( SeatBackF_Key == 1 ) )
    {
        Return_Sts = 1;
    }
    else if( ( PreSeatBackF_Key != SeatBackF_Key ) && ( SeatBackF_Key == 0 ) )
    {
        Return_Sts = 2;
    }
    else
    {
        Return_Sts = 0;
    }
    PreSeatBackF_Key = SeatBackF_Key;

    return (Return_Sts);

}
#if(SCM_SEATCONTROL_VARIANT == SCM_R_VARIANT)
/*******************************************************************************
* Function Name: Scm_SeatControl_Func
********************************************************************************
*
* Summary:
*  This API is called to Scm_SeatControl_Func
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Scm_SeatControl_Func(void)
{
    UINT8 GetForwardBack_LearnSts = 0;
    UINT8 GetSeatBack_LearnSts = 0;
    UINT8 Forword_Back_TargetPos = 0;
    UINT8 Seat_Back_TargetPos = 0;
    UINT8 Get_FastF_Key = 0;
    UINT8 Get_FastB_Key = 0;
    UINT8 GetIHU_SeatFoldReq = 0;
    UINT8 GetIHU_SeatReleaseReq = 0;

    GetForwardBack_LearnSts = CddMtr_Get_LearnData_Status(1);
    GetSeatBack_LearnSts = CddMtr_Get_LearnData_Status(0);
   // Forword_Back_TargetPos = Set_ForwardBack_TargetPos();
    //Seat_Back_TargetPos = Set_SeatBack_TargetPos();
    Get_FastF_Key = ServSWM_u_GetKey_Seat_FastF_SW_RealVaule();
    Get_FastB_Key = ServSWM_u_GetKey_Seat_FastB_SW_RealVaule();
    GetIHU_SeatFoldReq = IHU_SeatFoldReq;
    GetIHU_SeatReleaseReq = IHU_SeatReleaseReq;

    if( 0 == Motor_Learn_Start )
    {
        if( FALSE == Get_Voltage_Flag())
        {
            /* Mauual Mode */
            if( (ServSWM_u_GetKey_Seat_B_SW_RealVaule()) || (ServSWM_u_GetKey_Seat_F_SW_RealVaule())
            || (ServSWM_u_GetKey_Seatback_B_SW_RealVaule()) || (ServSWM_u_GetKey_Seatback_F_SW_RealVaule()) )
            {
                Seat_Manual_Mode = 1;

                if((1==Seat_EasyStatus) || (1==Seat_EntryStatus))
                {
                    CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
                    CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
                    Seat_EasyStatus = 0;
                    Seat_EntryStatus = 0;
                }
				
                if( ( 1== Seat_Back_Rest ) || ( 1== Forword_Back_Rest ))
                {
                    CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
                    CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
                    Seat_Back_Rest = 0;
                    Forword_Back_Rest = 0;
                }
            }
            else if( (ServSWM_u_GetKey_Seat_FastB_SW_RealVaule()) || ( ServSWM_u_GetKey_Seat_FastF_SW_RealVaule() )
            || (PreSeat_Back_TargetPos != Seat_Back_TargetPos) || (PreForword_Back_TargetPos != Forword_Back_TargetPos) )
            {
                Seat_Manual_Mode = 0;
            }
            else
            {}

            if( 1 == Seat_Manual_Mode )
            {
                /* Forward Back Motor -> 0% */
                if( ServSWM_u_GetKey_Seat_F_SW_RealVaule() )
                {
                    if( GetForwardBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        //if( ( CddMtr_Get_Mtr_PosPercent(1) >= 0 ) && ( CddMtr_Get_Mtr_PosPercent(1) <= 100) )
                        {
                          //  if( 1 == Scm_GetSeatF_Key_Func())
                            {
                         //       CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_FORWARD );
                            }
                            
                        }
                       // SCM_DrvSeatMotoPos_ForwardBack = 0xFE;
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN) )  /* Judge forward back hall sensor signal status */
                        {
                            SCM_DrvSeatMotoPos_ForwardBack = 0xFF;
                            if( 1 == Scm_GetSeatF_Key_Func() )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_FORWARD );
                            }
                            
                            if(SeatF_HallFault_Count <= 200)
                            {
                                SeatF_HallFault_Count ++;
                            }

                            if( SeatF_HallFault_Count == 200 )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                            }
                            
                        }
                        else
                        {
                            if( CddMtr_Get_Mtr_PosPercent(1) > MOTOR_POS_MIN1 )
                            {
                                if( 1 == Scm_GetSeatF_Key_Func() )
                                {
                                    CddMtr_Percent_Req(1, MOTOR_POS_MIN);  /* soft point */
                                }
                            }
                            else
                            {
                                if( 1 == Scm_GetSeatF_Key_Func() )
                                {
                                    CddMtr_Percent_Req(1, MOTOR_HARD_POS_MIN);  /* Hard point */
                                }
                            }
                            // SCM_DrvSeatMotoPos_ForwardBack = CddMtr_Get_Mtr_PosPercent(1);  /* Normal status */
                            // Get_Current_ForwardBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(1); /* Get Forward back Direction */
                        }
                    }
                }
                else
                {
                    SeatF_HallFault_Count = 0;
                    if( GetForwardBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( 2 == Scm_GetSeatF_Key_Func() )
                        {
                            CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                        }
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN) )
                        {
                            if( 2 == Scm_GetSeatF_Key_Func() )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( 2 == Scm_GetSeatF_Key_Func() )
                            {
                                CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
                            }
                        }
                    }
                }
                /* Forward Back Motor -> 100% */
                if( ServSWM_u_GetKey_Seat_B_SW_RealVaule() )
                {
                    if( GetForwardBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( ( CddMtr_Get_Mtr_PosPercent(1) >= 0 ) && ( CddMtr_Get_Mtr_PosPercent(1) <= 100) )
                        {
                            if( 1 == Scm_GetSeatB_Key_Func() )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_BACKWARD );
                            }
                            
                        }
                        SCM_DrvSeatMotoPos_ForwardBack = 0xFE;
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN) )  /* Judge forward back hall sensor signal status */
                        {
                            SCM_DrvSeatMotoPos_ForwardBack = 0xFF;
                            if( 1 == Scm_GetSeatB_Key_Func() )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_BACKWARD );
                            }
                            
                            if(SeatB_HallFault_Count <= 200)
                            {
                                SeatB_HallFault_Count ++;
                            }

                            if( SeatB_HallFault_Count == 200 )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( CddMtr_Get_Mtr_PosPercent(1) < MOTOR_POS_MAX1 )
                            {
                                if( 1 == Scm_GetSeatB_Key_Func() )
                                {
                                    CddMtr_Percent_Req(1, MOTOR_POS_MAX);    /* Soft point */
                                }
                            }
                            else
                            {
                                if( 1 == Scm_GetSeatB_Key_Func() )
                                {
                                    CddMtr_Percent_Req(1, MOTOR_HARD_POS_MAX);  /* Hard point */
                                }
                            }
                            // SCM_DrvSeatMotoPos_ForwardBack = CddMtr_Get_Mtr_PosPercent(1);  /* Normal status */
                            // Get_Current_ForwardBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(1); /* Get Forward back Direction */
                        }
                    }
                }
                else
                {
                    SeatB_HallFault_Count = 0;
                    if( GetForwardBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( 2 == Scm_GetSeatB_Key_Func() )
                        {
                            CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                        }
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN) )
                        {
                            if( 2 == Scm_GetSeatB_Key_Func() )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( 2 == Scm_GetSeatB_Key_Func() )
                            {
                                CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
                            }
                        }
                    }
                }
                /* Seat Back Motor -> 0% */
                if( ServSWM_u_GetKey_Seatback_F_SW_RealVaule() )
                {
                    if(  GetSeatBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( ( CddMtr_Get_Mtr_PosPercent(0) >= 0 ) && ( CddMtr_Get_Mtr_PosPercent(0) <= 100) )
                        {
                            if( 1 == Scm_GetSeatBackF_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_FORWARD );
                            }
                            
                        }
                        SCM_DrvSeatMotoPos_SeatBack = 0xFE;
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTBAT_OPEN) )  /* Judge forward back hall sensor signal status */
                        {
                            SCM_DrvSeatMotoPos_SeatBack = 0xFF;
                            if( 1 == Scm_GetSeatBackF_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_FORWARD );
                            }
                            
                            if(SeatBackF_HallFault_Count <= 200)
                            {
                                SeatBackF_HallFault_Count ++;
                            }

                            if( SeatBackF_HallFault_Count == 200 )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( CddMtr_Get_Mtr_PosPercent(0) > MOTOR_POS_MIN1 )
                            {
                                if(1 == Scm_GetSeatBackF_Key_Func())
                                {
                                    CddMtr_Percent_Req(0, MOTOR_POS_MIN);    /* Soft point */
                                }
                            }
                            else
                            {
                                if( 1 == Scm_GetSeatBackF_Key_Func() )
                                {
                                    CddMtr_Percent_Req(0, MOTOR_HARD_POS_MIN);  /* Hard point */
                                }
                            }
                            // SCM_DrvSeatMotoPos_SeatBack = CddMtr_Get_Mtr_PosPercent(0);  /* Normal status */
                            // Get_Current_SeatBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(0); /* Get SeatBack Direction */
                        }
                    }
                }
                else
                {
                    SeatBackF_HallFault_Count = 0;
                    if( GetSeatBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( 2 == Scm_GetSeatBackF_Key_Func() )
                        {
                            CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                        }
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
                        {
                            if( 2 == Scm_GetSeatBackF_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( 2 == Scm_GetSeatBackF_Key_Func() )
                            {
                                CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
                            }
                        }
                    }
                }

                /* Seat Back Motor -> 100% */
                if( ServSWM_u_GetKey_Seatback_B_SW_RealVaule() )
                {
                    if(  GetSeatBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( ( CddMtr_Get_Mtr_PosPercent(0) >= 0 ) && ( CddMtr_Get_Mtr_PosPercent(0) <= 100) )
                        {
                            if( 1 == Scm_GetSeatBackB_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_BACKWARD );
                            }
                            
                        }
                        SCM_DrvSeatMotoPos_SeatBack = 0xFE;
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTBAT_OPEN) )  /* Judge forward back hall sensor signal status */
                        {
                            SCM_DrvSeatMotoPos_SeatBack = 0xFF;
                            if( 1 == Scm_GetSeatBackB_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_BACKWARD );
                            }
                            
                            if(SeatBackB_HallFault_Count <= 200)
                            {
                                SeatBackB_HallFault_Count ++;
                            }

                            if( SeatBackB_HallFault_Count == 200 )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( CddMtr_Get_Mtr_PosPercent(0) < MOTOR_POS_MAX1 )
                            {
                                if( 1 == Scm_GetSeatBackB_Key_Func() )
                                {
                                    CddMtr_Percent_Req(0, MOTOR_POS_MAX);  /* Soft point */
                                }
                            }
                            else
                            {
                                if( 1 == Scm_GetSeatBackB_Key_Func() )
                                {
                                    CddMtr_Percent_Req(0, MOTOR_HARD_POS_MAX);  /* Hard point */
                                }
                            }
                            // SCM_DrvSeatMotoPos_SeatBack = CddMtr_Get_Mtr_PosPercent(0);  /* Normal status */
                            // Get_Current_SeatBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(0); /* Get SeatBack Direction */
                        }
                    }
                }
                else
                {
                    SeatBackB_HallFault_Count = 0;
                    if( GetSeatBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( 2 == Scm_GetSeatBackB_Key_Func() )
                        {
                            CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                        }
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTBAT_OPEN) )
                        {
                            if( 2 == Scm_GetSeatBackB_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( 2 == Scm_GetSeatBackB_Key_Func() )
                            {
                                CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
                            }
                        }
                    }
                }
            }
            /* Easy-Entry Mode */
            else if( ((ABM_PassengerSeatOccSts == 0x01) && (VCU_VehSpd <= VEH_SPEED) && (VCU_VehSpd_VD == 0x0)
            && (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_NORMAL )&&(CDDMTR_MNG_LEARN_VALID == CddMtr_Get_LearnData_Status(1))
            && (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_NORMAL )&&(CDDMTR_MNG_LEARN_VALID == CddMtr_Get_LearnData_Status(0)))
            && ((ServSWM_u_GetKey_Seat_FastF_SW_RealVaule() || ServSWM_u_GetKey_Seat_FastB_SW_RealVaule() )
            || ( 1 == GetIHU_SeatFoldReq ) || ( 1 == GetIHU_SeatReleaseReq )))
            {

                if( ( 1== Seat_Back_Rest ) || (1== Forword_Back_Rest) )
                {
                    CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
                    CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
                    Seat_Back_Rest = 0;
                    Forword_Back_Rest = 0;
                }

                if( ((( PreGet_FastF_Key != Get_FastF_Key ) && ( 1 == Get_FastF_Key )) || (( PreGetIHU_SeatFoldReq != GetIHU_SeatFoldReq ) && ( 1 == GetIHU_SeatFoldReq )))
                && ( 0 == Seat_EasyStatus ))
                { 
                    Dis_SCM_HMI_Req_Count = 3;
                    Dis_SCM_HMI_Req = 1;
                    Seat_EasyStatus = 1;
                    Seat_EntryStatus = 0;
                    CddMtr_Percent_Req(1, MOTOR_POS_MIN);
                    CddMtr_Percent_Req(0, MOTOR_POS_MAX);

                }
                else if( ((( PreGet_FastF_Key != Get_FastF_Key ) && ( 1 == Get_FastF_Key )) || (( PreGetIHU_SeatFoldReq != GetIHU_SeatFoldReq ) && ( 1 == GetIHU_SeatFoldReq )))
                && ( 1 == Seat_EasyStatus ) && ( 0 == Dis_SCM_HMI_Req_Count ) && ( CddMtr_Get_Mtr_PosPercent(1) > MOTOR_POS_MIN1 ) && ( CddMtr_Get_Mtr_PosPercent(0) < MOTOR_POS_MAX1 ))
                {
                    Dis_SCM_HMI_Req_Count = 3;
                    Dis_SCM_HMI_Req = 1;
                    Seat_EasyStatus = 0;
                    CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
                    CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
                }
                else
                {}

                
                if( ((( PreGet_FastB_Key != Get_FastB_Key ) && ( 1 == Get_FastB_Key ) ) || ( ( PreGetIHU_SeatReleaseReq != GetIHU_SeatReleaseReq ) && ( 1 == GetIHU_SeatReleaseReq )))
                && ( 0 == Seat_EntryStatus ) )
                {
                    Dis_SCM_HMI_Req_Count = 3;
                    Dis_SCM_HMI_Req = 1;
                    Seat_EntryStatus = 1;
                    Seat_EasyStatus = 0;
                    CddMtr_Percent_Req(1, MOTOR_POS_MAX);
                    CddMtr_Percent_Req(0, MOTOR_POS_MIN);
                }
                else if( ((( PreGet_FastB_Key != Get_FastB_Key ) && ( 1 == Get_FastB_Key ) ) || ( ( PreGetIHU_SeatReleaseReq != GetIHU_SeatReleaseReq ) && ( 1 == GetIHU_SeatReleaseReq )))
                && ( 1 == Seat_EntryStatus ) && ( 0 == Dis_SCM_HMI_Req_Count ) && ( CddMtr_Get_Mtr_PosPercent(1) < MOTOR_POS_MAX1 ) && ( CddMtr_Get_Mtr_PosPercent(0) > MOTOR_POS_MIN1 ) )
                {
                    Dis_SCM_HMI_Req_Count = 3;
                    Dis_SCM_HMI_Req = 1;
                    Seat_EntryStatus = 0;
                    CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
                    CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
                }
                else
                {}

                IHU_SeatFoldReq = 0;
                IHU_SeatReleaseReq = 0;
            }
            /* Rest Mode */
            else if( ( 0 == Seat_EasyStatus ) && ( 0 == Seat_EntryStatus ) )
            {
                if( (CDDMTR_MNG_LEARN_VALID == CddMtr_Get_LearnData_Status(0)) && (PreSeat_Back_TargetPos != Seat_Back_TargetPos) && (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_NORMAL) )
                {
                    Seat_Back_Rest = 1;
                    CddMtr_Percent_Req(0, Set_SeatBack_TargetPos());
                }

                if( (CDDMTR_MNG_LEARN_VALID == CddMtr_Get_LearnData_Status(1)) && (PreForword_Back_TargetPos != Forword_Back_TargetPos) && (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_NORMAL) )
                {
                    Forword_Back_Rest = 1;
                    CddMtr_Percent_Req(1, Set_ForwardBack_TargetPos());
                } 
                
            }
            
        }
        else
        {
            Forword_Back_Rest = 0;
            Seat_Back_Rest = 0;
            Seat_EasyStatus = 0;
            Seat_EntryStatus = 0;
            Dis_SCM_HMI_Req_Count = 0;
            Dis_SCM_HMI_Req = 0;
            Seat_Manual_Mode = 0;
            CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
            CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
        }
    }

    /* Clean Easy-Entry mode or Rest mode */
    if( (1 == Seat_EasyStatus)  
    && (( 0 == ABM_PassengerSeatOccSts) || ((VCU_VehSpd > VEH_SPEED) && (VCU_VehSpd_VD == 0x0))
    || ((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
    || ((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
    || ((1 == Scm_Get_Target_Func(1,MOTOR_POS_MIN)) && ( 1 == Scm_Get_Target_Func(0,MOTOR_POS_MAX) )) ) )
    {
        CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
        CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
        Seat_EasyStatus = 0;
    }
    else if( (1 == Seat_EntryStatus) 
    && (( 0 == ABM_PassengerSeatOccSts) || ((VCU_VehSpd > VEH_SPEED) && (VCU_VehSpd_VD == 0x0))
    || ((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
    || ((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
    || ((1 == Scm_Get_Target_Func(1,MOTOR_POS_MAX)) && (1 == Scm_Get_Target_Func(0,MOTOR_POS_MIN) )) ) )
    {
        CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
        CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
        Seat_EntryStatus = 0;
    }
    /* Clean Rest Mode */
    else if( ((1 == Seat_Back_Rest) || (1 == Forword_Back_Rest)) )
    {
        if( (((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
			|| (1 == Scm_Get_Target_Func(0,Set_SeatBack_TargetPos()))) && ( 1 == Seat_Back_Rest ) )
        {
            CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
            Seat_Back_Rest = 0;
        }

        if( (((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
			|| (1 == Scm_Get_Target_Func(1,Set_ForwardBack_TargetPos()))) && ( 1 == Forword_Back_Rest ) )
        {
            CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
            Forword_Back_Rest = 0;
        }
    }
    else
    {
        /* Do nothing */
    }

    
    /* Send to CAN motor posotion */
    if( (GetForwardBack_LearnSts == CDDMTR_MNG_LEARN_VALID) && ( CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_NORMAL ) )
    {
        SCM_DrvSeatMotoPos_ForwardBack = CddMtr_Get_Mtr_PosPercent(1);  /* Normal status */
        Get_Current_ForwardBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(1); /* Get Forward back Direction */
    }
		
	if( (GetSeatBack_LearnSts == CDDMTR_MNG_LEARN_VALID) && ( CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_NORMAL ) )
    {
        SCM_DrvSeatMotoPos_SeatBack = CddMtr_Get_Mtr_PosPercent(0);  /* Normal status */
        Get_Current_SeatBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(0); /* Get SeatBack Direction */
    }

    PreGet_FastF_Key = Get_FastF_Key;
    PreGet_FastB_Key = Get_FastB_Key;
    PreForword_Back_TargetPos = Forword_Back_TargetPos;
    PreSeat_Back_TargetPos = Seat_Back_TargetPos;
    PreGetIHU_SeatFoldReq = GetIHU_SeatFoldReq;
    PreGetIHU_SeatReleaseReq = GetIHU_SeatReleaseReq;

}

#else
/*******************************************************************************
* Function Name: Scm_SeatControl_Func
********************************************************************************
*
* Summary:
*  This API is called to Scm_SeatControl_Func
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Scm_SeatControl_Func(void)
{
	UINT8 GetForwardBack_LearnSts = 0;
	UINT8 GetSeatBack_LearnSts = 0;
    UINT8 Forword_Back_TargetPos = 0;
    UINT8 Seat_Back_TargetPos = 0;

	
    GetForwardBack_LearnSts = CddMtr_Get_LearnData_Status(1);
    GetSeatBack_LearnSts = CddMtr_Get_LearnData_Status(0);
    Forword_Back_TargetPos = Set_ForwardBack_TargetPos();
    Seat_Back_TargetPos = Set_SeatBack_TargetPos();
	
    /* Clean Reset mode */
    if( ((1 == Seat_Back_Rest) || (1 == Forword_Back_Rest)) )
    {
        if( (((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
			|| (1 == Scm_Get_Target_Func(0,Set_SeatBack_TargetPos()))) && ( 1 == Seat_Back_Rest ) )
        {
            CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
            Seat_Back_Rest = 0;
        }

        if( (((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
			|| (1 == Scm_Get_Target_Func(1,Set_ForwardBack_TargetPos()))) && ( 1 == Forword_Back_Rest ) )
        {
            CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
            Forword_Back_Rest = 0;
        }
    }

    if( 0 == Motor_Learn_Start )
    {
        if(FALSE == Get_Voltage_Flag())
        {
            /* Manual Mode */
            if(  (ServSWM_u_GetKey_Seat_B_SW_RealVaule()) || (ServSWM_u_GetKey_Seat_F_SW_RealVaule())
            || (ServSWM_u_GetKey_Seatback_B_SW_RealVaule()) || (ServSWM_u_GetKey_Seatback_F_SW_RealVaule()) )
            {
                Seat_Manual_Mode = 1;
                if( ( 1== Seat_Back_Rest ) || ( 1== Forword_Back_Rest ))
                {
                    Seat_Back_Rest = 0;
                    Forword_Back_Rest = 0;
                    CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
                    CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
                }
            }
            else if( (PreSeat_Back_TargetPos != Seat_Back_TargetPos) || (PreForword_Back_TargetPos != Forword_Back_TargetPos) )
            {
                Seat_Manual_Mode = 0;
            }
            else
            {}

            if( 1 == Seat_Manual_Mode )
            {
                /* Forward Back Motor -> 0% */
                if( ServSWM_u_GetKey_Seat_F_SW_RealVaule() )
                {
                    if( GetForwardBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( ( CddMtr_Get_Mtr_PosPercent(1) >= 0 ) && ( CddMtr_Get_Mtr_PosPercent(1) <= 100) )
                        {
                            if( 1 == Scm_GetSeatF_Key_Func() )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_FORWARD );
                            }
                            
                        }
                        SCM_DrvSeatMotoPos_ForwardBack = 0xFE;
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN) )  /* Judge forward back hall sensor signal status */
                        {
                            SCM_DrvSeatMotoPos_ForwardBack = 0xFF;
                            if( 1 == Scm_GetSeatF_Key_Func() )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_FORWARD );
                            }
                            
                            if(SeatF_HallFault_Count <= 200)
                            {
                                SeatF_HallFault_Count ++;
                            }

                            if( SeatF_HallFault_Count == 200 )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                            }
                            
                        }
                        else
                        {
                            if( CddMtr_Get_Mtr_PosPercent(1) > MOTOR_POS_MIN1 )
                            {
                                if( 1 == Scm_GetSeatF_Key_Func() )
                                {
                                    CddMtr_Percent_Req(1, MOTOR_POS_MIN);     /* Soft point */
                                }
                            }
                            else
                            {
                                if( 1 == Scm_GetSeatF_Key_Func() )
                                {
                                    CddMtr_Percent_Req(1, MOTOR_HARD_POS_MIN);   /* Hard point */
                                }
                            }
                            // SCM_DrvSeatMotoPos_ForwardBack = CddMtr_Get_Mtr_PosPercent(1);  /* Normal status */
                            // Get_Current_ForwardBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(1); /* Get Forward back Direction */
                        }
                    }
                }
                else
                {
                    SeatF_HallFault_Count = 0;
                    if( GetForwardBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( 2 == Scm_GetSeatF_Key_Func() )
                        {
                            CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                        }
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
                        {
                            if( 2 == Scm_GetSeatF_Key_Func() )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( 2 == Scm_GetSeatF_Key_Func() )
                            {
                                CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
                            }
                        }
                    }
                }
                /* Forward Back Motor -> 100% */
                if( ServSWM_u_GetKey_Seat_B_SW_RealVaule() )
                {
                    if( GetForwardBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( ( CddMtr_Get_Mtr_PosPercent(1) >= 0 ) && ( CddMtr_Get_Mtr_PosPercent(1) <= 100) )
                        {
                            if( 1 == Scm_GetSeatB_Key_Func() )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_BACKWARD );
                            }
                            
                        }
                        SCM_DrvSeatMotoPos_ForwardBack = 0xFE;
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))  /* Judge forward back hall sensor signal status */
                        {
                            SCM_DrvSeatMotoPos_ForwardBack = 0xFF;
                            if( 1 == Scm_GetSeatB_Key_Func() )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_BACKWARD );
                            }
                            
                            if(SeatB_HallFault_Count <= 200)
                            {
                                SeatB_HallFault_Count ++;
                            }

                            if( SeatB_HallFault_Count == 200 )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( CddMtr_Get_Mtr_PosPercent(1) < MOTOR_POS_MAX1 )
                            {
                                if( 1 == Scm_GetSeatB_Key_Func() )
                                {
                                    CddMtr_Percent_Req(1, MOTOR_POS_MAX);   /* Soft point */
                                }
                            }
                            else
                            {
                                if( 1 == Scm_GetSeatB_Key_Func() )
                                {
                                    CddMtr_Percent_Req(1, MOTOR_HARD_POS_MAX);  /* Hard point */
                                }
                            }
                            // SCM_DrvSeatMotoPos_ForwardBack = CddMtr_Get_Mtr_PosPercent(1);  /* Normal status */
                            // Get_Current_ForwardBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(1); /* Get Forward back Direction */
                        }
                    }
                }
                else
                {
                    SeatB_HallFault_Count = 0;
                    if( GetForwardBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( 2 == Scm_GetSeatB_Key_Func() )
                        {
                            CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                        }
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
                        {
                            if( 2 == Scm_GetSeatB_Key_Func() )
                            {
                                CddMtr_Direction_Req(1,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( 2 == Scm_GetSeatB_Key_Func() )
                            {
                                CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
                            }
                        }
                    }
                }
                /* Seat Back Motor -> 0% */
                if( ServSWM_u_GetKey_Seatback_F_SW_RealVaule() )
                {
                    if(  GetSeatBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( ( CddMtr_Get_Mtr_PosPercent(0) >= 0 ) && ( CddMtr_Get_Mtr_PosPercent(0) <= 100) )
                        {
                            if( 1 == Scm_GetSeatBackF_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_FORWARD );
                            }
                            
                        }
                        SCM_DrvSeatMotoPos_SeatBack = 0xFE;
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))  /* Judge forward back hall sensor signal status */
                        {
                            SCM_DrvSeatMotoPos_SeatBack = 0xFF;
                            if( 1 == Scm_GetSeatBackF_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_FORWARD );
                            }
                            
                            if(SeatBackF_HallFault_Count <= 200)
                            {
                                SeatBackF_HallFault_Count ++;
                            }

                            if( SeatBackF_HallFault_Count == 200 )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                            }
                            
                        }
                        else
                        {
                            if( CddMtr_Get_Mtr_PosPercent(0) > MOTOR_POS_MIN1 )
                            {
                                if( 1 == Scm_GetSeatBackF_Key_Func() )
                                {
                                    CddMtr_Percent_Req(0, MOTOR_POS_MIN);   /* Soft point */
                                }
                            }
                            else
                            {
                                if(  1 == Scm_GetSeatBackF_Key_Func() )
                                {
                                    CddMtr_Percent_Req(0, MOTOR_HARD_POS_MIN);  /* Hard point */
                                }
                            }
                            // SCM_DrvSeatMotoPos_SeatBack = CddMtr_Get_Mtr_PosPercent(0);  /* Normal status */
                            // Get_Current_SeatBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(0); /* Get SeatBack Direction */
                        }
                    }
                }
                else
                {
                    SeatBackF_HallFault_Count = 0;
                    if( GetSeatBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( 2 == Scm_GetSeatBackF_Key_Func() )
                        {
                            CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                        }
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
                        {
                            if( 2 == Scm_GetSeatBackF_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( 2 == Scm_GetSeatBackF_Key_Func() )
                            {
                                CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
                            }
                        }
                    }
                }

                /* Seat Back Motor -> 100% */
                if( ServSWM_u_GetKey_Seatback_B_SW_RealVaule() )
                {
                    if(  GetSeatBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( ( CddMtr_Get_Mtr_PosPercent(0) >= 0 ) && ( CddMtr_Get_Mtr_PosPercent(0) <= 100) )
                        {
                            if( 1 == Scm_GetSeatBackB_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_BACKWARD );
                            }
                            
                        }
                        SCM_DrvSeatMotoPos_SeatBack = 0xFE;
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))  /* Judge forward back hall sensor signal status */
                        {
                            SCM_DrvSeatMotoPos_SeatBack = 0xFF;
                            if( 1 == Scm_GetSeatBackB_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_BACKWARD );
                            }
                            
                            if(SeatBackB_HallFault_Count <= 200)
                            {
                                SeatBackB_HallFault_Count ++;
                            }

                            if( SeatBackB_HallFault_Count == 200 )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( CddMtr_Get_Mtr_PosPercent(0) < MOTOR_POS_MAX1 )
                            {
                                if( 1 == Scm_GetSeatBackB_Key_Func() )
                                {
                                    CddMtr_Percent_Req(0, MOTOR_POS_MAX);    /* Soft point */
                                }
                            }
                            else
                            {
                                if( 1 == Scm_GetSeatBackB_Key_Func() )
                                {
                                    CddMtr_Percent_Req(0, MOTOR_HARD_POS_MAX);   /* Hard point */
                                }
                            }
                            // SCM_DrvSeatMotoPos_SeatBack = CddMtr_Get_Mtr_PosPercent(0);  /* Normal status */
                            // Get_Current_SeatBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(0); /* Get SeatBack Direction */
                        }
                    }
                }
                else
                {
                    SeatBackB_HallFault_Count = 0;
                    if( GetSeatBack_LearnSts == CDDMTR_MNG_LEARNDATA_INVALID )
                    {
                        if( 2 == Scm_GetSeatBackB_Key_Func() )
                        {
                            CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                        }
                    }
                    else
                    {
                        if((CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTGND) || (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_SHORTBAT_OPEN))
                        {
                            if( 2 == Scm_GetSeatBackB_Key_Func() )
                            {
                                CddMtr_Direction_Req(0,CDDMTR_MNG_DIRECTION_STOP );
                            }
                        }
                        else
                        {
                            if( 2 == Scm_GetSeatBackB_Key_Func() )
                            {
                                CddMtr_Percent_Req(0,(CddMtr_Get_Mtr_PosPercent(0)*100));
                            }
                        }
                    }
                }
            }
            else
            {
                /* Reset Mode */
                if( (CDDMTR_MNG_LEARN_VALID == CddMtr_Get_LearnData_Status(0)) && (PreSeat_Back_TargetPos != Seat_Back_TargetPos) && (CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_NORMAL) )
                {
                    Seat_Back_Rest = 1;
                    CddMtr_Percent_Req(0, Set_SeatBack_TargetPos());
                }

                if( (CDDMTR_MNG_LEARN_VALID == CddMtr_Get_LearnData_Status(1)) && (PreForword_Back_TargetPos != Forword_Back_TargetPos) && (CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_NORMAL) )
                {
                    Forword_Back_Rest = 1;
                    CddMtr_Percent_Req(1, Set_ForwardBack_TargetPos());
                }          
            }
        }
        else
        {
            Seat_Manual_Mode = 0;
            Seat_Back_Rest = 0;
            Forword_Back_Rest = 0;
            CddMtr_Percent_Req(0, (CddMtr_Get_Mtr_PosPercent(0)*100));
            CddMtr_Percent_Req(1, (CddMtr_Get_Mtr_PosPercent(1)*100));
        }
    }

    if( (GetForwardBack_LearnSts == CDDMTR_MNG_LEARN_VALID) && ( CddMtr_Get_Hall_Fault_Status(1) == CDDMTR_MNG_HALL_NORMAL ) )
    {
        SCM_DrvSeatMotoPos_ForwardBack = CddMtr_Get_Mtr_PosPercent(1);  /* Normal status */
        Get_Current_ForwardBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(1); /* Get Forward back Direction */
    }
		
	if( (GetSeatBack_LearnSts == CDDMTR_MNG_LEARN_VALID) && ( CddMtr_Get_Hall_Fault_Status(0) == CDDMTR_MNG_HALL_NORMAL ) )
    {
        SCM_DrvSeatMotoPos_SeatBack = CddMtr_Get_Mtr_PosPercent(0);  /* Normal status */
        Get_Current_SeatBackMotor_Dir = CddMtr_Get_Mtr_Run_Direction(0); /* Get SeatBack Direction */
    }

    PreForword_Back_TargetPos = Forword_Back_TargetPos;
    PreSeat_Back_TargetPos = Seat_Back_TargetPos;

}
#endif

/*******************************************************************************
* Function Name: Get_ForwardBack_Current_Pos
********************************************************************************
*
* Summary:
*  This API is called to Get_Current_ForwardBack_Pos
*
* Parameters:
*  None
*
* Return:
*  Forward Back Position
*
*******************************************************************************/
UINT8 Get_ForwardBack_Current_Pos(void)
{
    UINT8 Current_ForwardBack_Pos = 0xFF;

    Current_ForwardBack_Pos = SCM_DrvSeatMotoPos_ForwardBack;
    
    return(Current_ForwardBack_Pos);
}

/*******************************************************************************
* Function Name: Get_SeatBack_Current_Pos
********************************************************************************
*
* Summary:
*  This API is called to Get_SeatBack_Current_Pos
*
* Parameters:
*  None
*
* Return:
*  Seat Back Position
*
*******************************************************************************/
UINT8 Get_SeatBack_Current_Pos(void)
{
    UINT8 Current_SeatBack_Pos = 0xFF;

    Current_SeatBack_Pos = SCM_DrvSeatMotoPos_SeatBack;
    
    return(Current_SeatBack_Pos);
}

/*******************************************************************************
* Function Name: Get_SeatSwtichSts
********************************************************************************
*
* Summary:
*  This API is called to Get_SeatSwtichSts
*
* Parameters:
*  None
*
* Return:
*  Seat Back Position
*
*******************************************************************************/
UINT8 Get_SeatSwtichSts(void)
{
    UINT8 SeatSwitchSts = 0;
	
    SeatSwitchSts = SCM_DrvSeatLocalCtrlSwithSts;

    return(SeatSwitchSts);
}


/*******************************************************************************
* Function Name: Get_SeatMotor_SoftPoint_Min
********************************************************************************
*
* Summary:
*  This API is called to Get_SeatMotor_SoftPoint_Min
*
* Parameters:
*  None
*
* Return:
*  Seat Back Position
*
*******************************************************************************/
UINT8 Get_SeatMotor_SoftPoint_Min(void)
{
    return (MOTOR_POS_MIN1);
}

/*******************************************************************************
* Function Name: Get_SeatMotor_SoftPoint_Max
********************************************************************************
*
* Summary:
*  This API is called to Get_SeatMotor_SoftPoint_Max
*
* Parameters:
*  None
*
* Return:
*  Seat Back soft point Position
*
*******************************************************************************/
UINT8 Get_SeatMotor_SoftPoint_Max(void)
{
    return (MOTOR_POS_MAX1);
}

/*******************************************************************************
* Function Name: Get_SeatBackMotor_SoftPoint_Min
********************************************************************************
*
* Summary:
*  This API is called to Get_SeatBackMotor_SoftPoint_Min
*
* Parameters:
*  None
*
* Return:
*  Seat Back Position
*
*******************************************************************************/
UINT8 Get_SeatBackMotor_SoftPoint_Min(void)
{
    return (MOTOR_POS_MIN1);
}

/*******************************************************************************
* Function Name: Get_SeatBackMotor_SoftPoint_Max
********************************************************************************
*
* Summary:
*  This API is called to Get_SeatBackMotor_SoftPoint_Max
*
* Parameters:
*  None
*
* Return:
*  Seat Back soft point Position
*
*******************************************************************************/
UINT8 Get_SeatBackMotor_SoftPoint_Max(void)
{
    return (MOTOR_POS_MAX1);
}