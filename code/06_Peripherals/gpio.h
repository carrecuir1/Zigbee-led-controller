#ifndef __PERIPHERAL_GPIO_H_
#define __PERIPHERAL_GPIO_H_

#include "gpiodefines.h"
#include "stm32wbxx_hal.h"
#include "retcode.h"

#define GENERATE_RETCODE_GPIO(severity, code) (GENERATE_RETCODE(severity, HIERARCHY_PERIPHERAL, MODULE_GPIO, code))
#define GENERATE_GPIO_SUCCESS(code)           (GENERATE_RETCODE_GPIO(SEVERITY_SUCCESS, code))
#define GENERATE_GPIO_INFO(code)              (GENERATE_RETCODE_GPIO(SEVERITY_INFO, code))
#define GENERATE_GPIO_WARNING(code)           (GENERATE_RETCODE_GPIO(SEVERITY_WARNING, code))
#define GENERATE_GPIO_ERROR(code)             (GENERATE_RETCODE_GPIO(SEVERITY_ERROR, code))

typedef enum retcode_gpio
{
	// Successes
	GPIO_SUCCESS =                       GENERATE_GPIO_SUCCESS(0x00),

	// Infos
	GPIO_INFO_NOT_ACTIVE =               GENERATE_GPIO_INFO(0x00),

	// Warnings
	GPIO_WARNING_DONT_EXIST =            GENERATE_GPIO_WARNING(0x00),

	// Errors
	GPIO_ERROR_NULL_PTR =                GENERATE_GPIO_ERROR(0x00),
	GPIO_ERROR_INIT =                    GENERATE_GPIO_ERROR(0x01),
} retcode_gpio_t;

retcode_t GPIO_Open(void);
retcode_t GPIO_Write(gpio_pin_index_t pGpio, gpio_value_t pValue);
retcode_t GPIO_Toggle(gpio_pin_index_t pGpio);
gpio_value_t GPIO_Read(gpio_pin_index_t pGpio);

#endif /* GPIO_H_ */
