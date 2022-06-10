 /**************************************************************************//**
 * @file     system_fm33lg0xx.c
 * @brief    CMSIS Cortex-M0 Device Peripheral Access Layer Source File for
 *           Device FM33LG0XX
 * @version  V2.00
 * @date     15. March 2021
 *
 * @note
 *
 ******************************************************************************/
/* Copyright (c) 2012 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THES
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/


#include "fm33lg0xx.h"
/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/
#define XTHF_DEFAULT_VALUE    ((uint32_t)8000000U)  /*!< Default value of XTHF in Hz */
#define XTLF_DEFAULT_VALUE    ((uint32_t)32768U)    /*!< Default value of XTLF in Hz */

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
/* ToDo: add here your necessary defines for device initialization
         following is an example for different system frequencies             */

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
/* ToDo: initialize SystemCoreClock with the system core clock frequency value
         achieved after system intitialization.
         This means system core clock frequency after call to SystemInit()    */
uint32_t SystemCoreClock = __SYSTEM_CLOCK;  /*!< System Clock Frequency (Core Clock)*/

/* Clock Variable definitions ------------------------------------------------*/
uint32_t XTLFClock = XTLF_DEFAULT_VALUE;        /*!< External Low-freq Osc Clock Frequency (XTLF) */
uint32_t XTHFClock = XTHF_DEFAULT_VALUE;        /*!< External High-freq Osc Clock Frequency (XTHF) */
//uint32_t SystemCoreClock = HCLK_DEFAULT_VALUE;  /*!< System Clock Frequency (Core Clock) */



/* Clock functions -----------------------------------------------------------*/
/**
 *  @brief Retrieve the PLL clock frequency
 * 
 *  @retval PLL clock frequency 
 */
static uint32_t SystemPLLClockUpdate(void)
{
    uint32_t clock = 0;
    
    /* Acquire PLL clock source */
    switch ((CMU->PLLCR >> 1) & 0x1)
    {
        case 0:
            switch ((CMU->RCHFCR >> 16) & 0xFU)
            {
                case 1: /* 16MHz */
                    clock = 16000000;
                    break;
                
                case 2: /* 24MHz */
                    clock = 24000000;
                    break;
                
                case 3: /* 32MHz */
                    clock = 32000000;
                
                case 0: /* 8MHz */
                default:
                    clock = 8000000;
                    break;
            }
            break;
        
        case 1:
            clock = XTHFClock;
            break;
    }
    
    /* Acquire PLL prescaler */
    switch ((CMU->PLLCR >> 0x4) & 0x7)
    {
        case 0: /* input divided by 1 */
            clock /= 1;
            break;
        
        case 1: /* input divided by 2 */
            clock /= 2; 
            break;
        
        case 2: /* input divided by 4 */
            clock /= 4;
            break;
        
        case 3: /* input divided by 8 */
            clock /= 8;
            break;
        
        case 4: /* input divided by 12 */
            clock /= 12;
            break;
        
        case 5: /* input divided by 16 */
            clock /= 16;
            break;
        
        case 6: /* input divided by 24 */
            clock /= 24;
            break;
        
        case 7: /* input divided by 32 */
            clock /= 32;
            break;
    }
    
    /* Acquire PLL multiplier and calculate PLL frequency */
    clock = clock * (((CMU->PLLCR >> 16) & 0x7F) + 1);
    
    /* Acquire PLL output channel(PLLx1 or PLLx2) */
    if ((CMU->PLLCR >> 3) & 0x1)
    {
        clock *= 2;
    }
    
    return clock;
}

/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
     switch ((CMU->SYSCLKCR >> 0) & 0x7)
    {        
        case 1: /* XTHF */
            SystemCoreClock = XTHFClock;
            break;
        
        case 2: /* PLL */
            SystemCoreClock = SystemPLLClockUpdate();
            break;
        
        case 4: /* RCLF */
            switch ((CMU->RCLFCR >> 16) & 0x3)
            {
                case 0: /* output divided by 1 */
                    SystemCoreClock = 614400;
                    break;
                
                case 1: /* output divided by 4 */
                    SystemCoreClock = 153600;
                    break;
                
                case 2: /* output divided by 8 */
                    SystemCoreClock = 76800;
                    break;
                
                case 3: /* output divided by 16 */
                    SystemCoreClock = 38400;
                    break;
            }
            break;
        
        case 5: /* XTLF */
            SystemCoreClock = XTLFClock;
            break;
        
        case 6: /* RCLP */
            SystemCoreClock = 32000;
            break;

        default:
            switch ((CMU->RCHFCR >> 16) & 0xf)
            {
                case 1: /* 16MHz */
                    SystemCoreClock = 16000000;
                    break;
                
                case 2: /* 24MHz */
                    SystemCoreClock = 24000000;
                    break;
                
                case 3: /* 32MHz */
                    SystemCoreClock = 32000000;
                    break;
                
                case 0: /* 8MHz */
                default:
                    SystemCoreClock = 8000000;
                    break;
            }
            break;
    }
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
 #if 0
void SystemInit (void)
{
    /* BUS Clock */
    CMU->PCLKCR1 |= 0x1U << 12;     // VREF
    CMU->PCLKCR1 |= 0x1U << 7;      // PAD
    
    /* DEBUG Config */
    DBG->CR = 0x10003;
    
    /* POWER Trim */
    PMU->ULPB_TR = ULPBG_TRIM;
    
    /* CLOCK Trim */
    CMU->RCLFTR = RCLF_TRIM;
    CMU->RCLPTR = RCLP_TRIM;
    PMU->ULPB_TR= ULPBG_TRIM;
    /* SWD PULLUP On */
    GPIOD->PUEN |= 0x3U << 7;
    
    /* PDR Config */
    RMU->PDRCR = 0x5;   // Enable 1.5v
    
    /*If bor is not available , the power down is then monitored by the pdr.
    This mean VDD can be below the minimum operating(1.65V) without any reset
    until Vpdr threshold.To solve this problem,use SVD to monitor vdd.when vdd 
    drop blew 1.65V,peograme can call sleep function.   */
    RMU->BORCR = 0x0;  //enable bor power down
                       
    /* CDIF Config */
    CDIF->CR |= 0x1U << 1;  // VAO->CPU Enable

	SystemCoreClockUpdate();
} 
#else
void SystemInit(void)
{
    
    #if defined(USE_IWDT_ON_STARTUP)
    CMU->PCLKCR1 |= 0x20U;              /* Enable IWDT Operation Clock */
    IWDT->CR = IWDT_OVERFLOW_PERIOD;    /* Configure IWDT overflow period */
    IWDT->SERV = 0x12345A5AU;           /* Enable IWDT */
    #endif
    
    /* Enable VREF Operation Clock */
    CMU->PCLKCR1 |= 0x1U << 12;
    
    /* Enable PAD Operation Clock */
    CMU->PCLKCR1 |= 0x1U << 7;
    
    /* CDIF£ºVAO->CPU Enable */
    CDIF->CR |= 0x1U << 1;
    
    #ifndef MFANG   /* MFANG handles clock configurations by itself */
    #ifdef USE_LSCLK_CLOCK_SRC_XTLF
    #ifdef USE_LSCLK_AUTO_SWITCH
  
        /* Enable LSCLK auto switch */
        CMU->SYSCLKCR |= 0x8000000U;
    
        /* LSCLK from XTLF */
        CMU->LSCLKSEL = 0xAAU;
        
    #else

        /* Disable LSCLK auto switch */
        CMU->SYSCLKCR &= 0x7FFFFFFU;

        /* LSCLK from XTLF */
        CMU->LSCLKSEL = 0xAAU;

    #endif  /* USE_LSCLK_AUTO_SWITCH */
    #else
        
        /* Disable LSCLK auto switch */
        CMU->SYSCLKCR &= 0x7FFFFFFU;
        
        /* LSCLK from RCLP */
        CMU->LSCLKSEL = 0x55U;
        
    #endif  /* USE_LSCLK_CLOCK_SRC_XTLF */
    #endif  /* MFANG */
    
    /* Keep timers running and disable IWDT && WWDT under debug mode */
    DBG->CR = 0x3U;
    
    #ifdef USE_DEBUG_UNDER_SLEEP
        /* Keep debug connnection under sleep mode */
        DBG->CR |= 0x1U << 16;
    #endif
    
    /* Load power trim value */
    PMU->ULPB_TR = ULPBG_TRIM;
    
    /* Load default clock trim value */
    CMU->RCHFTR = RCHF8M_TRIM;
    CMU->RCLFTR = RCLF_TRIM;
    CMU->RCLPTR = RCLP_TRIM;
    PMU->ULPB_TR= ULPBG_TRIM;
    
    /* Enable SWD port pull up */
    GPIOD->PUEN |= 0x3U << 7;
    
    /*
      If BOR is disabled, power down will be monitored by PDR. This means VDD can 
      be below the minimum operating voltage(1.65V) to V_PDR threshold without
      power down reset. To solve this, user should use SVD to monitor VDD voltage. 
      When the VDD voltage drop below 1.65V, program can enter sleep.   
    */
    
    /* PDR Config enable 1.55v */
    RMU->PDRCR = 0x7;
   
    /* Disable BOR power down */
    RMU->BORCR = 0x01;
    
    /* Update System Core Clock */
    SystemCoreClockUpdate();
    
    #if defined(USE_IWDT_ON_STARTUP)
    IWDT->SERV = 0x12345A5AU; /* Feed IWDT */
    #endif
} 

#endif

/*----------------------------------------------------------------------------
  NVIC MFang Config functions
 *----------------------------------------------------------------------------*/
/**
  * @brief	NVIC_Init config NVIC
  *
  * @param 	nvicConfigStruct config params
  * @param 	IRQn             Interrupt number
  *
  * @retval	None
  */
void NVIC_Init(NVIC_ConfigTypeDef *nvicConfigStruct,IRQn_Type IRQn)
{
    /* Params Check */
    if(nvicConfigStruct->preemptPriority > 3)
    {
        nvicConfigStruct->preemptPriority = 3;
    }
	NVIC_DisableIRQ(IRQn);
	NVIC_SetPriority(IRQn, nvicConfigStruct->preemptPriority);
	NVIC_EnableIRQ(IRQn);
}



