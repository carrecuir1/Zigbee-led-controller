#ifndef APP_ZIGBEE_LEDSTRIP_H
#define APP_ZIGBEE_LEDSTRIP_H

#include "ep_defines.h"
#include "zigbee_types.h"
#include "zcl/zcl.h"

typedef enum eZigbeeLedstripType
{
  Zb_Ledstrip_Off = 0,
  Zb_Ledstrip_On = 1,
  Zb_Ledstrip_Toggle = 2,
} eZigbeeLedstripType_t;

#define ZCL_CLUSTER_LEDSTRIP_CUSTOM  0xFC01U

enum {
    CMD_SET_MODE        = 0x00,
    CMD_SET_RANGE       = 0x01,
    CMD_SET_COLOR       = 0x02,
    CMD_SET_BRIGHTNESS  = 0x03,
    CMD_SET_SPEED       = 0x04,
    CMD_STOP            = 0x05,
};

struct ZbZclClusterT * ZbZclLedstripServerAlloc(struct ZigBeeT *zb, uint8_t endpoint);

#endif // APP_ZIGBEE_LEDSTRIP_H