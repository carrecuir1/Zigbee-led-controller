#ifndef __PERIPHERAL_CPU_H_
#define __PERIPHERAL_CPU_H_

#include "stm32wbxx_hal.h"
#include "retcode.h"

#define GENERATE_RETCODE_CPU(severity, code) (GENERATE_RETCODE(severity, HIERARCHY_PERIPHERAL, MODULE_CPU, code))
#define GENERATE_CPU_SUCCESS(code)           (GENERATE_RETCODE_CPU(SEVERITY_SUCCESS, code))
#define GENERATE_CPU_INFO(code)              (GENERATE_RETCODE_CPU(SEVERITY_INFO, code))
#define GENERATE_CPU_WARNING(code)           (GENERATE_RETCODE_CPU(SEVERITY_WARNING, code))
#define GENERATE_CPU_ERROR(code)             (GENERATE_RETCODE_CPU(SEVERITY_ERROR, code))

typedef enum retcode_cpu
{
	// Successes
	CPU_SUCCESS =                       GENERATE_CPU_SUCCESS(0x00),

	// Infos

	// Warnings

	// Errors
	CPU_ERROR_NULL_PTR =                GENERATE_CPU_ERROR(0x00),
	CPU_ERROR_INIT =                    GENERATE_CPU_ERROR(0x01),
	CPU_ERROR_INIT_OSCILLATOR =         GENERATE_CPU_ERROR(0x02),
	CPU_ERROR_INIT_CLOCK =              GENERATE_CPU_ERROR(0x03),
	CPU_ERROR_INIT_PERIPHERAL =         GENERATE_CPU_ERROR(0x04),
} retcode_cpu_t;

retcode_t CPU_Open(void);

// void SysTick_Handler(void);

#endif /* __PERIPHERAL_CPU_H_ */
