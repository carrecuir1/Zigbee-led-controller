#ifndef RETCODE_H
#define RETCODE_H

#include <stdint.h>

#define SEVERITY_OFFSET	(30)
#define SEVERITY_MASK (0b11)
#define HIERIARCHY_OFFSET	(26) // Just the category or hierarchy of the system module
#define HIERIARCHY_MASK (0b1111)
#define MODULE_OFFSET	(16)
#define MODULE_MASK (0b1111111111)

// Retcode = SSHH HHMM MMMM MMMM CCCC CCCC CCCC CCCC
#define GENERATE_RETCODE(severity, hieriachy, module, code)	((int)(severity << SEVERITY_OFFSET) | (hieriachy << HIERIARCHY_OFFSET) | (module << MODULE_OFFSET) | code)
#define GET_MODULE(retcode) ((retcode >> MODULE_OFFSET) & MODULE_MASK)
#define GET_HIERARCHY(retcode) ((retcode >> HIERIARCHY_OFFSET) & HIERIARCHY_MASK)
#define GET_SEVERITY(retcode) ((retcode >> SEVERITY_OFFSET) & SEVERITY_MASK)

#define VERIFY_SEVERITY(retcode, severity)	((((retcode >> SEVERITY_OFFSET) & SEVERITY_MASK) && severity) == severity)
#define VERIFY_SUCCESS(retcode)				(VERIFY_SEVERITY(retcode, SEVERITY_SUCCESS))
#define VERIFY_INFO(retcode)				(VERIFY_SEVERITY(retcode, SEVERITY_INFO))
#define VERIFY_WARNING(retcode)				(VERIFY_SEVERITY(retcode, SEVERITY_WARNING))
#define VERIFY_ERROR(retcode)				(VERIFY_SEVERITY(retcode, SEVERITY_ERROR))

#define IS_SUCCESS(retcode)	((retcode_t) retcode >= 0)
#define IS_FAILURE(retcode)	(!IS_SUCCESS(retcode))

#define RETCODE_OK		GENERATE_RETCODE(SEVERITY_SUCCESS, HIERARCHY_KERNEL, MODULE_RETCODE, 0x00)	// Generic OK return code
#define RETCODE_ERROR	GENERATE_RETCODE(SEVERITY_ERROR, HIERARCHY_KERNEL, MODULE_RETCODE, 0x00)		// Generic Error return code

#define HIERARCHY_LIST \
    HIERARCHY(HIERARCHY_KERNEL) \
    HIERARCHY(HIERARCHY_BSP) \
    HIERARCHY(HIERARCHY_APP) \
    HIERARCHY(HIERARCHY_THREADS) \
    HIERARCHY(HIERARCHY_DRIVERS) \
    HIERARCHY(HIERARCHY_PERIPHERAL) \
    HIERARCHY(HIERARCHY_MIDDLEWARE) \
    HIERARCHY(HIERARCHY_MISC)

// Having similar module for different hierarchy of the code
#define MODULES_LIST \
    MODULE(MODULE_RETCODE) \
    MODULE(MODULE_CALLBACK) \
    MODULE(MODULE_LEDSTRIP) \
    MODULE(MODULE_CPU) \
    MODULE(MODULE_IPCC) \
    MODULE(MODULE_GPIO) \
    MODULE(MODULE_PWM) \
    MODULE(MODULE_LPUART) \
    MODULE(MODULE_USART) \
    MODULE(MODULE_RTC) \
    MODULE(MODULE_RNG) 

typedef int32_t retcode_t;

typedef enum retcode_severity
{
	SEVERITY_SUCCESS = 0,  // A success is an indication that the operation completed without any issues.
	SEVERITY_INFO = 1,     // An info is a message that provides additional information but does not indicate any problem.
	SEVERITY_WARNING = 2,  // A warning is an indication that something might be wrong, but the operation can still continue.
	SEVERITY_ERROR = 3     // An error is an indication that something went wrong and the operation could not be completed.
} retcode_severity;

typedef enum retcode_hieriarchy
{
    #define HIERARCHY(name) name,
    HIERARCHY_LIST
    #undef HIERARCHY

    HIERARCHY_QTY
} retcode_hieriarchy_t;

typedef enum retcode_module
{
    #define MODULE(name) name,
	MODULES_LIST
    #undef MODULE

    MODULE_QTY
} retcode_module_t;

#endif /* RETCODE_H */
