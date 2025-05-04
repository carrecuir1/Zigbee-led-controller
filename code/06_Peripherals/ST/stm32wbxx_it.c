#include "main.h"
#include "stm32wbxx_it.h"

extern RTC_HandleTypeDef hrtc;

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
   while (1)
  {
    
  }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{

}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{

}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{

}


/******************************************************************************/
/* STM32WBxx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32wbxx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles PVD/PVM0/PVM2 interrupts through EXTI lines 16/31/33.
  */
void PVD_PVM_IRQHandler(void)
{
  HAL_PWREx_PVD_PVM_IRQHandler();
}

/**
  * @brief This function handles RTC wake-up interrupt through EXTI line 19.
  */
void RTC_WKUP_IRQHandler(void)
{
  HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
}

/**
  * @brief This function handles Flash global interrupt.
  */
void FLASH_IRQHandler(void)
{
  HAL_FLASH_IRQHandler();
}

/**
  * @brief This function handles RCC global interrupt.
  */
void RCC_IRQHandler(void)
{

}

/**
  * @brief This function handles CPU2 SEV interrupt through EXTI line 40 and PWR CPU2 HOLD wake-up interrupt.
  */
void C2SEV_PWR_C2H_IRQHandler(void)
{

}

/**
  * @brief This function handles PWR switching on the fly, end of BLE activity, end of 802.15.4 activity, end of critical radio phase interrupt.
  */
void PWR_SOTF_BLEACT_802ACT_RFPHASE_IRQHandler(void)
{

}

/**
  * @brief This function handles HSEM global interrupt.
  */
void HSEM_IRQHandler(void)
{
  HAL_HSEM_IRQHandler();
}

/**
  * @brief This function handles FPU global interrupt.
  */
void FPU_IRQHandler(void)
{

}

