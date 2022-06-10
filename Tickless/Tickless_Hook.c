#include "Tickless_Hook.h" 
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"

#define   ulGetExternalTime()    FL_LPTIM32_ReadCounter(LPTIM32)


void sleepConfig(void)
{

    FL_PMU_SleepInitTypeDef LPM_InitStruct;
    
//  FL_CMU_RCLF_Enable();               // ???RCLF   
    FL_RMU_PDR_Enable(RMU);  						// ??PDR
    FL_RMU_BOR_Disable(RMU);  					// ??BOR 2uA
    
    FL_VREF_Disable(VREF);              // ??VREF1p2
    
    LPM_InitStruct.deepSleep 					= FL_PMU_SLEEP_MODE_DEEP;
    LPM_InitStruct.powerMode 					= FL_PMU_POWER_MODE_SLEEP_OR_DEEPSLEEP;
    LPM_InitStruct.wakeupFrequency 		= FL_PMU_RCHF_WAKEUP_FREQ_8MHZ;
    LPM_InitStruct.wakeupDelay 				= FL_PMU_WAKEUP_DELAY_2US;
	  LPM_InitStruct.LDOLowPowerMode   	= FL_PMU_LDO_LPM_DISABLE;
    LPM_InitStruct.coreVoltageScaling = FL_DISABLE; 
    FL_PMU_Sleep_Init(PMU, &LPM_InitStruct);
    __WFI();
    FL_RMU_BOR_Enable(RMU); //??BOR
    
}
void prvStopTickInterruptTimer ( void )
{
    NVIC_DisableIRQ ( SysTick_IRQn );
    NVIC_ClearPendingIRQ ( SysTick_IRQn );
    SysTick->CTRL= 0;
}

__INLINE void disable_interrupts ( void )
{
    __asm volatile ( "cpsid i" );
    __asm volatile ( "dsb" );
    __asm volatile ( "isb" );
}

void prvStartTickInterruptTimer ( void )
{
    SysTick_Config ( configCPU_CLOCK_HZ / configTICK_RATE_HZ );
    NVIC_EnableIRQ ( SysTick_IRQn );
}

__INLINE void enable_interrupts ( void )
{
    __asm volatile ( "cpsie i" );
}

void prvSleep ( void )
{
    FL_LPTIM32_EnableIT_CC(LPTIM32,FL_LPTIM32_CHANNEL_1);       
    NVIC_DisableIRQ ( LPTIMx_IRQn );
    NVIC_SetPriority ( LPTIMx_IRQn, 2 );
    NVIC_EnableIRQ ( LPTIMx_IRQn );
    sleepConfig (); 
    FL_LPTIM32_DisableIT_CC(LPTIM32,FL_LPTIM32_CHANNEL_1);           
}

void vSetWakeTimeInterrupt ( TickType_t xExpectedIdleTime )
{
    FL_LPTIM32_WriteCompareCH1(LPTIM32, xExpectedIdleTime + ulGetExternalTime()); 
}
  
void vApplicationSleep ( TickType_t xExpectedIdleTime )
{
	#if configUSE_TICKLESS_IDLE
    unsigned long ulLowPowerTimeBeforeSleep, ulLowPowerTimeAfterSleep;
    eSleepModeStatus eSleepStatus;

    /* Read the current time from a time source that will remain operational
    while the microcontroller is in a low power state. */
    ulLowPowerTimeBeforeSleep = ulGetExternalTime ();

    /* Stop the timer that is generating the tick interrupt. */
    prvStopTickInterruptTimer ();

    /* Enter a critical section that will not effect interrupts bringing the MCU
    out of sleep mode. */
    disable_interrupts ();

    /* Ensure it is still ok to enter the sleep mode. */
    eSleepStatus = eTaskConfirmSleepModeStatus ();

    if ( eSleepStatus == eAbortSleep )
    {
      /* A task has been moved out of the Blocked state since this macro was
      executed, or a context siwth is being held pending.  Do not enter a
      sleep state.  Restart the tick and exit the critical section. */
      prvStartTickInterruptTimer ();
      enable_interrupts ();
    }
    else 
    {
      if ( eSleepStatus == eNoTasksWaitingTimeout )
      {
        /* It is not necessary to configure an interrupt to bring the
        microcontroller out of its low power state at a fixed time in the
        future. */
        prvSleep ();
      }
      else 
      {

        /* Configure an interrupt to bring the microcontroller out of its low
        power state at the time the kernel next needs to execute.  The
        interrupt must be generated from a source that remains operational
        when the microcontroller is in a low power state. */
        vSetWakeTimeInterrupt ( xExpectedIdleTime - 1 );

        /* Enter the low power state. */
        prvSleep ();

        /* Determine how long the microcontroller was actually in a low power
        state for, which will be less than xExpectedIdleTime if the
        microcontroller was brought out of low power mode by an interrupt
        other than that configured by the vSetWakeTimeInterrupt() call.
        Note that the scheduler is suspended before
        portSUPPRESS_TICKS_AND_SLEEP() is called, and resumed when
        portSUPPRESS_TICKS_AND_SLEEP() returns.  Therefore no other tasks will
        execute until this function completes. */
        ulLowPowerTimeAfterSleep = ulGetExternalTime ();

        /* Correct the kernels tick count to account for the time the
        microcontroller spent in its low power state. */
        vTaskStepTick ( ( ulLowPowerTimeAfterSleep - ulLowPowerTimeBeforeSleep ) );
      }

      /* Exit the critical section - it might be possible to do this immediately
      after the prvSleep() calls. */
      enable_interrupts ();

      /* Restart the timer that is generating the tick interrupt. */
      prvStartTickInterruptTimer ();
    }
#endif
}
 
