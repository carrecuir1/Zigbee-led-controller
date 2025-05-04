#ifndef __DRIVER_GPIODRV_H_
#define __DRIVER_GPIODRV_H_
#include "gpio.h"

retcode_t GPIODriver_Open(void);
retcode_t GPIODriver_Write(gpio_pin_index_t pGpio, gpio_value_t pValue);
retcode_t GPIODriver_Toggle(gpio_pin_index_t pGpio);
gpio_value_t GPIODriver_Read(gpio_pin_index_t pGpio);


#endif /* GPIODRV_H_ */
