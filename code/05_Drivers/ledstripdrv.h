/*
 * ledstripdrv.h
 *
 *  Created on: May 20, 2024
 *      Author: tomto
 */

#ifndef _DRIVER_LEDSTRIP_H_
#define _DRIVER_LEDSTRIP_H_

#include "pwm.h"
#include "retcode.h"

#define GENERATE_RETCODE_LEDSTRIP(severity, code) (GENERATE_RETCODE(severity, HIERARCHY_DRIVERS, MODULE_LEDSTRIP, code))
#define GENERATE_LEDSTRIP_SUCCESS(code) (GENERATE_RETCODE_LEDSTRIP(SEVERITY_SUCCESS, code))
#define GENERATE_LEDSTRIP_INFO(code) (GENERATE_RETCODE_LEDSTRIP(SEVERITY_INFO, code))
#define GENERATE_LEDSTRIP_WARNING(code) (GENERATE_RETCODE_LEDSTRIP(SEVERITY_WARNING, code))
#define GENERATE_LEDSTRIP_ERROR(code) (GENERATE_RETCODE_LEDSTRIP(SEVERITY_ERROR, code))

typedef enum retcode_ledstrip
{
  // Successes
  LEDSTRIP_SUCCESS = GENERATE_LEDSTRIP_SUCCESS(0x00),

  // Infos
  LEDSTRIP_NOT_READY = GENERATE_LEDSTRIP_INFO(0x00),

  // Warnings

  // Errors
  LEDSTRIP_ERROR_NULL_PTR = GENERATE_LEDSTRIP_ERROR(0x00),
  LEDSTRIP_ERROR_INIT = GENERATE_LEDSTRIP_ERROR(0x01),
  LEDSTRIP_ERROR_SENDING = GENERATE_LEDSTRIP_ERROR(0x02)
} retcode_ledstrip_t;

retcode_t LedstripDriver_Open(void);
retcode_t LedstripDriver_Send(uint16_t *pData, uint32_t pLength);

#endif /* _DRIVER_LEDSTRIP_H_ */
