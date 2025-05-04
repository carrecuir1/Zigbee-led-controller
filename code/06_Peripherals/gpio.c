#include "gpio.h"

#warning "DO NOT USE EXTERN"
extern sGPIO_t GPIODefinition[GPIO_QTY];

retcode_t GPIO_Open(void){
  retcode_t ret = GPIO_SUCCESS;
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  for(uint16_t i = 0; i < GPIO_QTY; i++){
    if(GPIODefinition[i].State == GPIO_STATE_ACTIVE){

      GPIO_InitStruct.Pin = GPIODefinition[i].Pin;
      GPIO_InitStruct.Mode = GPIODefinition[i].Mode;
      GPIO_InitStruct.Pull = GPIODefinition[i].Pull;
      GPIO_InitStruct.Speed = GPIODefinition[i].Speed;
      // Write the output pin if open drain or push-pull
      if ((GPIODefinition[i].Mode == GPIO_MODE_OUTPUT_PP)||(GPIODefinition[i].Mode == GPIO_MODE_OUTPUT_OD))
        HAL_GPIO_WritePin(GPIODefinition[i].Port, GPIODefinition[i].Pin, GPIODefinition[i].InitVal);

      HAL_GPIO_Init(GPIODefinition[i].Port, &GPIO_InitStruct);
      }
    }
  return ret;
}

retcode_t GPIO_Write(gpio_pin_index_t pGpio, gpio_value_t pValue){
  if(pGpio >= GPIO_QTY) return GPIO_WARNING_DONT_EXIST;
  if(GPIODefinition[pGpio].State == GPIO_STATE_INACTIVE) return GPIO_INFO_NOT_ACTIVE;

  HAL_GPIO_WritePin(GPIODefinition[pGpio].Port, GPIODefinition[pGpio].Pin, pValue);
  return GPIO_SUCCESS;
}

retcode_t GPIO_Toggle(gpio_pin_index_t pGpio){
  if(pGpio >= GPIO_QTY) return GPIO_WARNING_DONT_EXIST;
  if(GPIODefinition[pGpio].State == GPIO_STATE_INACTIVE) return GPIO_INFO_NOT_ACTIVE;

  HAL_GPIO_TogglePin(GPIODefinition[pGpio].Port, GPIODefinition[pGpio].Pin);
  return GPIO_SUCCESS;
}

gpio_value_t GPIO_Read(gpio_pin_index_t pGpio){
  if(pGpio >= GPIO_QTY) return GPIO_VAL_NONE;
  if(GPIODefinition[pGpio].State == GPIO_STATE_INACTIVE) return GPIO_VAL_NONE;

  if (HAL_GPIO_ReadPin(GPIODefinition[pGpio].Port, GPIODefinition[pGpio].Pin) == GPIO_PIN_RESET) return GPIO_VAL_0;
  return GPIO_VAL_1;
}
