/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32wbxx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32wbxx_hal_dma.h"
#include "stm32wbxx_hal_tim.h"
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

                    /**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_HSEM_CLK_ENABLE();

  /* System interrupt init*/

  /* Peripheral interrupt init */
  /* PVD_PVM_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PVD_PVM_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(PVD_PVM_IRQn);
  /* FLASH_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(FLASH_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(FLASH_IRQn);
  /* RCC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(RCC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(RCC_IRQn);
  /* C2SEV_PWR_C2H_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(C2SEV_PWR_C2H_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(C2SEV_PWR_C2H_IRQn);
  /* PWR_SOTF_BLEACT_802ACT_RFPHASE_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PWR_SOTF_BLEACT_802ACT_RFPHASE_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(PWR_SOTF_BLEACT_802ACT_RFPHASE_IRQn);
  /* HSEM_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(HSEM_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(HSEM_IRQn);
  /* FPU_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(FPU_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(FPU_IRQn);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
