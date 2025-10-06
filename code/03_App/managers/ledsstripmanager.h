/*
 * ledsstripmanager.h
 *
 *  Created on: May 20, 2024
 *      Author: tomto
 */

#ifndef __MANAGERS_LEDSSTRIPMANAGER_H_
#define __MANAGERS_LEDSSTRIPMANAGER_H_

#include "callbackmanager.h"
#include "ledstripdrv.h"
#include "defines.h"
#include "retcode.h"

#define GENERATE_RETCODE_LEDSSTRIPMAN(severity, code) (GENERATE_RETCODE(severity, HIERARCHY_APP, MODULE_LEDSTRIP, code))
#define GENERATE_LEDSSTRIPMAN_SUCCESS(code)              (GENERATE_RETCODE_LEDSSTRIPMAN(SEVERITY_SUCCESS, code))
#define GENERATE_LEDSSTRIPMAN_INFO(code)                 (GENERATE_RETCODE_LEDSSTRIPMAN(SEVERITY_INFO, code))
#define GENERATE_LEDSSTRIPMAN_WARNING(code)              (GENERATE_RETCODE_LEDSSTRIPMAN(SEVERITY_WARNING, code))
#define GENERATE_LEDSSTRIPMAN_ERROR(code)                (GENERATE_RETCODE_LEDSSTRIPMAN(SEVERITY_ERROR, code))

typedef enum retcode_ledstripman
{
  // Successes
  LEDSSTRIPMAN_SUCCESS =                  GENERATE_LEDSSTRIPMAN_SUCCESS(0x00),

  // Info
  LEDSSTRIPMAN_ENDSTEP =                  GENERATE_LEDSSTRIPMAN_INFO(0x00),

  // Warnings

  // Errors
  LEDSSTRIPMAN_ERROR_NULL_PTR =           GENERATE_LEDSSTRIPMAN_ERROR(0x00), // null pointer found
} retcode_ledstripman_t;

retcode_t LEDSSTRIPManager_Init(void);

typedef enum Led_Effect
{
  LED_MODE_CLOSE = 0,
  LED_MODE_STOP,
  LED_MODE_STATIC,
  LED_MODE_RIGHT_RAINBOW,
  LED_MODE_BLINK,
  LED_MODE_FADE_IN
} Led_Effect_t;

typedef struct Led_Color
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} Led_Color_t;

typedef struct Led_Configuration
{
  Led_Effect_t effect;
  union
  {
    Led_Color_t led_static;
  };
  uint8_t brightness;
  float brightness_scale;
} Led_Configuration_t;

#endif /* __MANAGERS_LEDSSTRIPMANAGER_H_ */
