#ifndef __MANAGERS_CALLBACKMANAGER_H_
#define __MANAGERS_CALLBACKMANAGER_H_

#include "stm32wbxx_hal.h"
#include "retcode.h"
#include "app_zigbee.h"

#define CALLBACK_MAX 15

#define GENERATE_RETCODE_CALLBACKMAN(severity, code) (GENERATE_RETCODE(severity, HIERARCHY_APP, MODULE_CALLBACK, code))
#define GENERATE_CALLBACKMAN_SUCCESS(code)       (GENERATE_RETCODE_CALLBACKMAN(SEVERITY_SUCCESS, code))
#define GENERATE_CALLBACKMAN_INFO(code)        (GENERATE_RETCODE_CALLBACKMAN(SEVERITY_INFO, code))
#define GENERATE_CALLBACKMAN_WARNING(code)       (GENERATE_RETCODE_CALLBACKMAN(SEVERITY_WARNING, code))
#define GENERATE_CALLBACKMAN_ERROR(code)       (GENERATE_RETCODE_CALLBACKMAN(SEVERITY_ERROR, code))

typedef enum retcode_callback
{
  // Successes
  CALLBACKMAN_SUCCESS =        GENERATE_CALLBACKMAN_SUCCESS(0x00),

  // Info

  // Warnings

  // Errors
  CALLBACKMAN_ERROR_NULL_PTR =   GENERATE_CALLBACKMAN_ERROR(0x00), // null pointer found
} retcode_callback_t;

typedef uint8_t FuncNbr;
typedef void (*void_callback_t)(void);

// Zigbee
typedef void (*zigbee_onoff_callback_t)(eZigbeeEndpoints_t, eZigbeeOnOffType_t);
typedef void (*zigbee_levelcontrol_callback_t)(eZigbeeEndpoints_t, eZigbeeLevelControlType_t, uint8_t);

// Timing callbacks
void Callback_RegisterEach1ms(void_callback_t callback);
void Callback_RunEach1ms(void);
void Callback_RegisterEach10ms(void_callback_t callback);
void Callback_RunEach10ms(void);
void Callback_RegisterEach25ms(void_callback_t callback);
void Callback_RunEach25ms(void);
void Callback_RegisterEach50ms(void_callback_t callback);
void Callback_RunEach50ms(void);
void Callback_RegisterEach100ms(void_callback_t callback);
void Callback_RunEach100ms(void);
void Callback_RegisterEach250ms(void_callback_t callback);
void Callback_RunEach250ms(void);
void Callback_RegisterEach500ms(void_callback_t callback);
void Callback_RunEach500ms(void);
void Callback_RegisterEach1000ms(void_callback_t callback);
void Callback_RunEach1000ms(void);

// Zigbee callbacks
void Callback_RegisterZigbeeOnOff(zigbee_onoff_callback_t callback);
void Callback_RunZigbeeOnOff(eZigbeeEndpoints_t endpoint, eZigbeeOnOffType_t type);
void Callback_RegisterZigbeeLevelControl(zigbee_levelcontrol_callback_t callback);
void Callback_RunZigbeeLevelControl(eZigbeeEndpoints_t endpoint, eZigbeeLevelControlType_t type, uint8_t level);

#endif /* __MANAGERS_CALLBACKMANAGER_H_ */
