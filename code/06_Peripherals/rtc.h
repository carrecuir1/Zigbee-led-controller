#ifndef __PERIPHERAL_RTC_H_
#define __PERIPHERAL_RTC_H_

#include "stm32wbxx_hal.h"
#include "retcode.h"

#define GENERATE_RETCODE_RTC(severity, code) (GENERATE_RETCODE(severity, HIERARCHY_PERIPHERAL, MODULE_RTC, code))
#define GENERATE_RTC_SUCCESS(code)           (GENERATE_RETCODE_RTC(SEVERITY_SUCCESS, code))
#define GENERATE_RTC_INFO(code)              (GENERATE_RETCODE_RTC(SEVERITY_INFO, code))
#define GENERATE_RTC_WARNING(code)           (GENERATE_RETCODE_RTC(SEVERITY_WARNING, code))
#define GENERATE_RTC_ERROR(code)             (GENERATE_RETCODE_RTC(SEVERITY_ERROR, code))

typedef enum retcode_rtc
{
	// Successes
	RTC_SUCCESS =                       GENERATE_RTC_SUCCESS(0x00),

	// Infos

	// Warnings

	// Errors
	RTC_ERROR_NULL_PTR =                GENERATE_RTC_ERROR(0x00),
	RTC_ERROR_RTC_INIT =                GENERATE_RTC_ERROR(0x01),
	RTC_ERROR_INIT_TIMER =              GENERATE_RTC_ERROR(0x02),
} retcode_rtc_t;

retcode_t RTC_Open(void);

#endif /* __PERIPHERAL_RTC_H_ */
