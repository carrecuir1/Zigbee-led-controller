#ifndef __PERIPHERAL_RNG_H_
#define __PERIPHERAL_RNG_H_

#include "stm32wbxx_hal.h"
#include "retcode.h"

#define GENERATE_RETCODE_RNG(severity, code) (GENERATE_RETCODE(severity, HIERARCHY_PERIPHERAL, MODULE_RNG, code))
#define GENERATE_RNG_SUCCESS(code)           (GENERATE_RETCODE_RNG(SEVERITY_SUCCESS, code))
#define GENERATE_RNG_INFO(code)              (GENERATE_RETCODE_RNG(SEVERITY_INFO, code))
#define GENERATE_RNG_WARNING(code)           (GENERATE_RETCODE_RNG(SEVERITY_WARNING, code))
#define GENERATE_RNG_ERROR(code)             (GENERATE_RETCODE_RNG(SEVERITY_ERROR, code))

typedef enum retcode_rng
{
	// Successes
	RNG_SUCCESS =                       GENERATE_RNG_SUCCESS(0x00),

	// Infos

	// Warnings

	// Errors
	RNG_ERROR_NULL_PTR =                GENERATE_RNG_ERROR(0x00),
	RNG_ERROR_RNG_INIT =                GENERATE_RNG_ERROR(0x01)
} retcode_rng_t;

retcode_t RNG_Open(void);

#endif /* __PERIPHERAL_RNG_H_ */
