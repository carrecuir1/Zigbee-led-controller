#include "pwm.h"
#include "gpiodefines.h"

#define PWM_TIMER_LEDS TIM1
#define PWM_TIMER_LEDS_INIT_FCT() __HAL_RCC_TIM1_CLK_ENABLE()
#define PWM_TIMER_LEDS_DEINIT_FCT() __HAL_RCC_TIM1_CLK_DISABLE()
#define PWM_TIMER_CHANNEL_LEDS TIM_CHANNEL_1
#define PWM_TIMER_OCMODE_LEDS TIM_OCMODE_PWM1
#define PWM_DMA_CHANNEL_LEDS DMA1_Channel1
#define PWM_DMA_REQUEST_LEDS DMA_REQUEST_TIM1_CH1
#define PWM_LEDS_PERIOD 80 - 1

#define PWM_LEDS_HIGH (uint32_t)(PWM_LEDS_PERIOD * 0.64)
#define PWM_LEDS_LOW (uint32_t)(PWM_LEDS_PERIOD * 0.32)

TIM_HandleTypeDef htimpwm_leds;
DMA_HandleTypeDef hdma_timpwm_leds;

pwm_flag_t mDataFlagLeds = PWM_FLAG_IDLE;

static retcode_t MX_TIM_LEDS_Init(void);
static retcode_t MX_DMA_LEDS_Init(void);

retcode_t PWM_Open(pwm_type_t pChannel)
{
  retcode_t ret = PWM_ERROR_INIT; // Assume Error
  if (pChannel == PWM_TYPE_LEDS)
  {
    mDataFlagLeds = PWM_FLAG_IDLE;
    if(IS_SUCCESS(MX_TIM_LEDS_Init()))
        if(IS_SUCCESS(MX_DMA_LEDS_Init()))
            ret = PWM_SUCCESS;
  }
  return ret;
}

retcode_t PWM_Send(pwm_type_t pChannel, uint8_t *pData, uint16_t pLength)
{
  retcode_t ret = PWM_ERROR_SENDING; // Assume Error
  if (pChannel == PWM_TYPE_LEDS)
  {
    if (mDataFlagLeds == PWM_FLAG_IDLE)
    {
      HAL_TIM_PWM_Start_DMA(&htimpwm_leds, PWM_TIMER_CHANNEL_LEDS, (uint32_t *)pData, pLength);
      mDataFlagLeds = PWM_FLAG_SENDING;
    }
    else
    {
      ret = PWM_NOT_READY;
    }
  }
  return ret;
}

retcode_t PWM_Get_PWM_Status(pwm_type_t pChannel)
{
  retcode_t ret = PWM_ERROR_STATUS; // Assume Error
  if (pChannel == PWM_TYPE_LEDS)
  {
    if (mDataFlagLeds == PWM_FLAG_IDLE)
      ret = PWM_SUCCESS;
    else
      ret = PWM_NOT_READY;
  }
  return ret;
}

// HAL_Callback
// void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
// {
//   if (htim == &htimpwm_leds)
//   {
//     HAL_TIM_PWM_Stop_DMA(&htimpwm_leds, PWM_TIMER_CHANNEL_LEDS);
//     mDataFlagLeds = PWM_FLAG_IDLE;
//   }
// }

void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htimpwm_leds)
  {
    mDataFlagLeds = PWM_FLAG_ERROR;
  }
}

// Private functions
static retcode_t MX_TIM_LEDS_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  htimpwm_leds.Instance = PWM_TIMER_LEDS;
  htimpwm_leds.Init.Prescaler = 0;
  htimpwm_leds.Init.CounterMode = TIM_COUNTERMODE_UP;
  htimpwm_leds.Init.Period = PWM_LEDS_PERIOD;
  htimpwm_leds.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htimpwm_leds.Init.RepetitionCounter = 0;
  htimpwm_leds.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  // if (HAL_TIM_Base_Init(&htimpwm_leds) != HAL_OK)
  // {
  //   return PWM_ERROR_INIT;
  // }
  // sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  // if (HAL_TIM_ConfigClockSource(&htimpwm_leds, &sClockSourceConfig) != HAL_OK)
  // {
  //   return PWM_ERROR_INIT;
  // }
  if (HAL_TIM_PWM_Init(&htimpwm_leds) != HAL_OK)
  {
    return PWM_ERROR_INIT;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htimpwm_leds, &sMasterConfig) != HAL_OK)
  {
    return PWM_ERROR_INIT;
  }

  sConfigOC.OCMode = PWM_TIMER_OCMODE_LEDS;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htimpwm_leds, &sConfigOC, PWM_TIMER_CHANNEL_LEDS) != HAL_OK)
  {
    return PWM_ERROR_INIT;
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htimpwm_leds, &sBreakDeadTimeConfig) != HAL_OK)
  {
    return PWM_ERROR_INIT;
  }

  STRIP_LEDS_PORT_INIT_FCT();
  GPIO_InitStruct.Pin = STRIP_LEDS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = STRIP_LEDS_AF;
  HAL_GPIO_Init(STRIP_LEDS_PORT, &GPIO_InitStruct);
  return PWM_SUCCESS;
}

/**
 * Enable DMA controller clock
 */
static retcode_t MX_DMA_LEDS_Init(void)
{
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  return PWM_SUCCESS;
}

void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_timpwm_leds);
}

// void DMA2_Channel1_IRQHandler(void)
// {
//   HAL_DMA_IRQHandler(&hdma_timpwm_leds);
// }

/**
* @brief TIM_PWM MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{
  if(htim_pwm->Instance==PWM_TIMER_LEDS)
  {
    PWM_TIMER_LEDS_INIT_FCT();

    hdma_timpwm_leds.Instance = PWM_DMA_CHANNEL_LEDS;
    hdma_timpwm_leds.Init.Request = PWM_DMA_REQUEST_LEDS;
    hdma_timpwm_leds.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_timpwm_leds.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_timpwm_leds.Init.MemInc = DMA_MINC_ENABLE;
    hdma_timpwm_leds.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_timpwm_leds.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_timpwm_leds.Init.Mode = DMA_NORMAL;
    hdma_timpwm_leds.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_timpwm_leds) != HAL_OK)
    {
      while(1){}
    }

    __HAL_LINKDMA(htim_pwm,hdma[TIM_DMA_ID_CC1],hdma_timpwm_leds);
  }

}

/**
* @brief TIM_PWM MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{
  if(htim_pwm->Instance==PWM_TIMER_LEDS)
  {
    /* Peripheral clock disable */
    PWM_TIMER_LEDS_DEINIT_FCT();

    HAL_DMA_DeInit(htim_pwm->hdma[TIM_DMA_ID_CC1]);
  }

}