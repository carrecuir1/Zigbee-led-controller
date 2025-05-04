/*
 * gpiosdefine.h
 *
 *  Created on: May 7, 2024
 *      Author: tomto
 */

#ifndef GPIODEFINES_H_
#define GPIODEFINES_H_

#include "stm32wbxx_hal.h"

// Multipurpose
typedef enum gpio_pin_index{
  GPIO_LED1 = 0,
  GPIO_LED2,
  GPIO_LED3,
  GPIO_QTY
} gpio_pin_index_t;

// STRIP LEDS
#define STRIP_LEDS_PIN GPIO_PIN_8
#define STRIP_LEDS_PORT GPIOA
#define STRIP_LEDS_PORT_INIT_FCT() __HAL_RCC_GPIOA_CLK_ENABLE()
#define STRIP_LEDS_AF GPIO_AF1_TIM1

typedef GPIO_TypeDef* gpio_port_t;
typedef uint16_t gpio_pin_t;
typedef uint32_t gpio_mode_t;
typedef uint32_t gpio_pull_t;
typedef uint32_t gpio_speed_t;

typedef enum {
  GPIO_VAL_0 = GPIO_PIN_RESET,
  GPIO_VAL_1 = GPIO_PIN_SET,
  GPIO_VAL_NONE
} gpio_value_t;

typedef enum {
  GPIO_STATE_INACTIVE = 0,
  GPIO_STATE_ACTIVE = 1
} gpio_state_t;

typedef struct sGPIO {
  gpio_port_t Port;
  gpio_pin_t  Pin;
  gpio_mode_t Mode;
  gpio_pull_t Pull;
  gpio_speed_t Speed;
  gpio_state_t State;
  gpio_value_t InitVal;
} sGPIO_t;

#endif /* GPIODEFINES_H_ */
