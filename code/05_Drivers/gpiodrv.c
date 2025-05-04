#include "gpiodrv.h"

retcode_t GPIODriver_Open(void){
  return GPIO_Open();
}

retcode_t GPIODriver_Write(gpio_pin_index_t pGpio, gpio_value_t pValue){
  return GPIO_Write(pGpio, pValue);
}

retcode_t GPIODriver_Toggle(gpio_pin_index_t pGpio){
  return GPIO_Toggle(pGpio);
}

gpio_value_t GPIODriver_Read(gpio_pin_index_t pGpio){
  return GPIO_Read(pGpio);
}
