#ifndef __PERIPHERAL_IPCC_H_
#define __PERIPHERAL_IPCC_H_

#include "stm32wbxx_hal.h"
#include "retcode.h"

#define GENERATE_RETCODE_IPCC(severity, code) (GENERATE_RETCODE(severity, HIERARCHY_PERIPHERAL, MODULE_IPCC, code))
#define GENERATE_IPCC_SUCCESS(code)           (GENERATE_RETCODE_IPCC(SEVERITY_SUCCESS, code))
#define GENERATE_IPCC_INFO(code)              (GENERATE_RETCODE_IPCC(SEVERITY_INFO, code))
#define GENERATE_IPCC_WARNING(code)           (GENERATE_RETCODE_IPCC(SEVERITY_WARNING, code))
#define GENERATE_IPCC_ERROR(code)             (GENERATE_RETCODE_IPCC(SEVERITY_ERROR, code))

typedef enum retcode_ipcc
{
	// Successes
	IPCC_SUCCESS =                       GENERATE_IPCC_SUCCESS(0x00),

	// Infos

	// Warnings

	// Errors
	IPCC_ERROR_NULL_PTR =                GENERATE_IPCC_ERROR(0x00),
	IPCC_ERROR_INIT =                    GENERATE_IPCC_ERROR(0x01),
} retcode_ipcc_t;

retcode_t IPCC_Open(void);

// void IPCC_C1_RX_IRQHandler(void);
// void IPCC_C1_TX_IRQHandler(void);

#endif /* __PERIPHERAL_IPCC_H_ */
