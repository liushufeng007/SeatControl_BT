/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : mf_config.c
  * @brief          : MCU FUNCTION CONFIG
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 FMSH.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by FMSH under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "mf_config.h"

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  The application entry point.
  * @retval int
  */
void MF_Clock_Init(void)
{
    /* MCU Configuration--------------------------------------------------------*/
    FL_CMU_EnableGroup1BusClock(FL_CMU_GROUP1_BUSCLK_RTCA); 
    FL_RTCA_WriteAdjustValue (RTCA, 0); 
    FL_CMU_DisableGroup1BusClock(FL_CMU_GROUP1_BUSCLK_RTCA);
    FL_RTCB_WriteAdjustValue (RTCB, 0);
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

    /* System interrupt init*/

    /* Initialize all configured peripherals */
}


void Set_Clock_64MHZ(void)

{



    //PC2:XTHIN   PC3:XTHOUT    

    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    

    GPIO_InitStruct.pin = FL_GPIO_PIN_2|FL_GPIO_PIN_3;

    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;

    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;

    GPIO_InitStruct.pull = FL_DISABLE;

    GPIO_InitStruct.remapPin = FL_DISABLE;

    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

       

    //??XTHF

    FL_CMU_XTHF_Enable();

    FL_CMU_XTHF_WriteDriverStrength(7);   

    FL_DelayMs(2);

    while (!FL_CMU_XTHF_IsEnabled());

 

    //FLASH ?????1

    FL_FLASH_SetReadWait(FLASH, FL_FLASH_READ_WAIT_2CYCLE);

    

    FL_CMU_SetSystemClockSource(FL_CMU_SYSTEM_CLK_SOURCE_XTHF);

    

   // // ?????:??PLL,?64M????

   

    FL_CMU_PLL_SetClockSource(FL_CMU_PLL_CLK_SOURCE_XTHF);

    FL_CMU_PLL_SetPrescaler(FL_CMU_PLL_PSC_DIV8); //8???1M

    FL_CMU_PLL_WriteMultiplier(64 - 1);; //??64M

    FL_CMU_PLL_Enable();

 

    while (!FL_CMU_IsActiveFlag_PLLReady());  //????

    

    FL_CMU_SetSystemClockSource(FL_CMU_SYSTEM_CLK_SOURCE_PLL);

    FL_CMU_SetAHBPrescaler(FL_CMU_AHBCLK_PSC_DIV1);

    FL_CMU_SetAPBPrescaler(FL_CMU_APBCLK_PSC_DIV1);

 

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void MF_SystemClock_Config(void)
{
    Set_Clock_64MHZ();
}

void MF_Config_Init(void)
{
    
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT FMSH *****END OF FILE****/
