#ifndef __PERIPHERAL_USART_H_
#define __PERIPHERAL_USART_H_

#include "stm32wbxx_hal.h"
#include "retcode.h"

#define GENERATE_RETCODE_USART(severity, code) (GENERATE_RETCODE(severity, HIERARCHY_PERIPHERAL, MODULE_USART, code))
#define GENERATE_USART_SUCCESS(code)           (GENERATE_RETCODE_USART(SEVERITY_SUCCESS, code))
#define GENERATE_USART_INFO(code)              (GENERATE_RETCODE_USART(SEVERITY_INFO, code))
#define GENERATE_USART_WARNING(code)           (GENERATE_RETCODE_USART(SEVERITY_WARNING, code))
#define GENERATE_USART_ERROR(code)             (GENERATE_RETCODE_USART(SEVERITY_ERROR, code))

typedef enum retcode_usart
{
	// Successes
	USART_SUCCESS =                       GENERATE_USART_SUCCESS(0x00),

	// Infos

	// Warnings

	// Errors
	USART_ERROR_NULL_PTR =                GENERATE_USART_ERROR(0x00),
	USART_ERROR_UART_INIT =               GENERATE_USART_ERROR(0x01),
	USART_ERROR_FIFO_THRESHOLD =          GENERATE_USART_ERROR(0x02),
	USART_ERROR_DISABLE_FIFO =            GENERATE_USART_ERROR(0x03),
} retcode_usart_t;

retcode_t USART_Open(void);

#endif /* __PERIPHERAL_USART_H_ */
