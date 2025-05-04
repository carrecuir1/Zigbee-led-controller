#ifndef __PERIPHERAL_PWM_H_
#define __PERIPHERAL_PWM_H_

#include "stm32wbxx_hal.h"
#include "retcode.h"

#define GENERATE_RETCODE_PWM(severity, code) (GENERATE_RETCODE(severity, HIERARCHY_PERIPHERAL, MODULE_PWM, code))
#define GENERATE_PWM_SUCCESS(code) (GENERATE_RETCODE_PWM(SEVERITY_SUCCESS, code))
#define GENERATE_PWM_INFO(code) (GENERATE_RETCODE_PWM(SEVERITY_INFO, code))
#define GENERATE_PWM_WARNING(code) (GENERATE_RETCODE_PWM(SEVERITY_WARNING, code))
#define GENERATE_PWM_ERROR(code) (GENERATE_RETCODE_PWM(SEVERITY_ERROR, code))

typedef enum retcode_pwm
{
    // Successes
    PWM_SUCCESS =           GENERATE_PWM_SUCCESS(0x00),

    // Infos
    PWM_NOT_READY =         GENERATE_PWM_INFO(0x00),

    // Warnings

    // Errors
    PWM_ERROR_NULL_PTR =    GENERATE_PWM_ERROR(0x00),
    PWM_ERROR_INIT =        GENERATE_PWM_ERROR(0x01),
    PWM_ERROR_SENDING =     GENERATE_PWM_ERROR(0x02),
    PWM_ERROR_STATUS =      GENERATE_PWM_ERROR(0x03),
} retcode_pwm_t;

typedef enum
{
    PWM_FLAG_IDLE = 0,
    PWM_FLAG_SENDING,
    PWM_FLAG_SENT,
    PWM_FLAG_ERROR,
} pwm_flag_t;

typedef enum
{
    PWM_TYPE_LEDS = 0
} pwm_type_t;

retcode_t PWM_Open(pwm_type_t pChannel);
retcode_t PWM_Send(pwm_type_t pChannel, uint8_t *pData, uint16_t pLength);

#endif /* __PERIPHERAL_PWM_H_ */
