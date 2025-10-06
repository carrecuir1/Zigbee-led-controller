#include "usart.h"
#include "stm32wbxx_it.h"

#warning hw_uart uses these for some reason
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

retcode_t MX_USART1_UART_Init(void);
static retcode_t MX_DMA_Init(void);

retcode_t USART_Open(void)
{
  retcode_t ret = MX_USART1_UART_Init();
  if(IS_FAILURE(ret))
    return ret;
  ret = MX_DMA_Init();
  return ret;
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
retcode_t MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_8;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
    return USART_ERROR_UART_INIT;

  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    return USART_ERROR_FIFO_THRESHOLD;

  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    return USART_ERROR_FIFO_THRESHOLD;

  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
    return USART_ERROR_DISABLE_FIFO;

  return USART_SUCCESS;
}

static retcode_t MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  return USART_SUCCESS;
}

/**
  * @brief This function handles USART1 global interrupt.
  */
 void USART1_IRQHandler(void)
 {
   HAL_UART_IRQHandler(&huart1);
 }
 
/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */
void DMA1_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
}
