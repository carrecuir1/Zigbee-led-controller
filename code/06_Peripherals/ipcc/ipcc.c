#include "ipcc.h"

#include "main.h"
#include "stm32wbxx_hal_ipcc.h"

IPCC_HandleTypeDef hipcc;

static retcode_t MX_IPCC_Init(void);

retcode_t IPCC_Open(void)
{
  return MX_IPCC_Init();
}

/**
 * @brief IPCC Initialization Function
 * @param None
 * @retval None
 */
static retcode_t MX_IPCC_Init(void)
{
  hipcc.Instance = IPCC;
  if (HAL_IPCC_Init(&hipcc) != HAL_OK)
  {
    // Error_Handler();
    return IPCC_ERROR_INIT;
  }

  return IPCC_SUCCESS;
}

/**
 * @brief This function handles IPCC RX occupied interrupt.
 */
void IPCC_C1_RX_IRQHandler(void)
{
  HAL_IPCC_RX_IRQHandler(&hipcc);
}

/**
 * @brief This function handles IPCC TX free interrupt.
 */
void IPCC_C1_TX_IRQHandler(void)
{
  HAL_IPCC_TX_IRQHandler(&hipcc);
}

/**
 * @brief IPCC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hipcc: IPCC handle pointer
 * @retval None
 */
void HAL_IPCC_MspInit(IPCC_HandleTypeDef *hipcc)
{
  if (hipcc->Instance == IPCC)
  {
    /* USER CODE BEGIN IPCC_MspInit 0 */

    /* USER CODE END IPCC_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_IPCC_CLK_ENABLE();
    /* IPCC interrupt Init */
    HAL_NVIC_SetPriority(IPCC_C1_RX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(IPCC_C1_RX_IRQn);
    HAL_NVIC_SetPriority(IPCC_C1_TX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(IPCC_C1_TX_IRQn);
    /* USER CODE BEGIN IPCC_MspInit 1 */

    /* USER CODE END IPCC_MspInit 1 */
  }
}

/**
 * @brief IPCC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hipcc: IPCC handle pointer
 * @retval None
 */
void HAL_IPCC_MspDeInit(IPCC_HandleTypeDef *hipcc)
{
  if (hipcc->Instance == IPCC)
  {
    /* USER CODE BEGIN IPCC_MspDeInit 0 */

    /* USER CODE END IPCC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_IPCC_CLK_DISABLE();

    /* IPCC interrupt DeInit */
    HAL_NVIC_DisableIRQ(IPCC_C1_RX_IRQn);
    HAL_NVIC_DisableIRQ(IPCC_C1_TX_IRQn);
    /* USER CODE BEGIN IPCC_MspDeInit 1 */

    /* USER CODE END IPCC_MspDeInit 1 */
  }
}