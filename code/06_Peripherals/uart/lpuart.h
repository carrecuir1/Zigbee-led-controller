#ifndef __PERIPHERAL_LPUART_H_
#define __PERIPHERAL_LPUART_H_

#include "stm32wbxx_hal.h"
#include "retcode.h"

#define GENERATE_RETCODE_LPUART(severity, code) (GENERATE_RETCODE(severity, HIERARCHY_PERIPHERAL, MODULE_LPUART, code))
#define GENERATE_LPUART_SUCCESS(code)           (GENERATE_RETCODE_LPUART(SEVERITY_SUCCESS, code))
#define GENERATE_LPUART_INFO(code)              (GENERATE_RETCODE_LPUART(SEVERITY_INFO, code))
#define GENERATE_LPUART_WARNING(code)           (GENERATE_RETCODE_LPUART(SEVERITY_WARNING, code))
#define GENERATE_LPUART_ERROR(code)             (GENERATE_RETCODE_LPUART(SEVERITY_ERROR, code))

typedef enum retcode_lpuart
{
	// Successes
	LPUART_SUCCESS =                       GENERATE_LPUART_SUCCESS(0x00),

	// Infos

	// Warnings

	// Errors
	LPUART_ERROR_NULL_PTR =                GENERATE_LPUART_ERROR(0x00),
	LPUART_ERROR_UART_INIT =               GENERATE_LPUART_ERROR(0x01),
	LPUART_ERROR_FIFO_THRESHOLD =          GENERATE_LPUART_ERROR(0x02),
	LPUART_ERROR_DISABLE_FIFO =            GENERATE_LPUART_ERROR(0x03),
} retcode_lpuart_t;

retcode_t LPUART_Open(void);

#endif /* __PERIPHERAL_LPUART_H_ */
