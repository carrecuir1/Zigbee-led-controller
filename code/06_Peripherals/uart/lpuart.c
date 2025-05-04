#include "lpuart.h"

#warning hw_uart uses these for some reason
UART_HandleTypeDef hlpuart1;
DMA_HandleTypeDef hdma_lpuart1_tx;

retcode_t MX_LPUART1_UART_Init(void);
static retcode_t MX_DMA_Init(void);

retcode_t LPUART_Open(void)
{
  retcode_t ret = MX_LPUART1_UART_Init();
  if(IS_FAILURE(ret))
    return ret;
  ret = MX_DMA_Init();
  return ret;
}

/**
 * @brief LPUART1 Initialization Function
 * @param None
 * @retval None
 */
retcode_t MX_LPUART1_UART_Init(void)
{
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
    return LPUART_ERROR_UART_INIT;

  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    return LPUART_ERROR_FIFO_THRESHOLD;

  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    return LPUART_ERROR_FIFO_THRESHOLD;

  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
    return LPUART_ERROR_DISABLE_FIFO;

  return LPUART_SUCCESS;
}

retcode_t MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  

  return LPUART_SUCCESS;
}

/**
  * @brief This function handles LPUART1 global interrupt.
  */
 void LPUART1_IRQHandler(void)
 {
   HAL_UART_IRQHandler(&hlpuart1);
 }
 
/**
  * @brief This function handles DMA1 channel3 global interrupt.
  */
void DMA1_Channel3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_lpuart1_tx);
}
