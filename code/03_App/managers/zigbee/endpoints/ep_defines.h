#ifndef APP_ZIGBEE_ENDPOINTS_DEFINES_H
#define APP_ZIGBEE_ENDPOINTS_DEFINES_H

#include "zcl/zcl.enum.h"

#define MANUF_CUSTOM_CODE         0x1234U

typedef enum eCustomClusterID {
    CUSTOM_CLUSTER_ID_LEDSTRIP = 0xFC01,//ZCL_CLUSTER_MFR_SPECIFIC_MIN + 1,
} eCustomClusterID_t;

typedef enum eCustomDeviceID {
    CUSTOM_DEVICE_ID_LEDSTRIP = 0x8001U,
} eCustomDeviceID_t;

#endif // APP_ZIGBEE_ENDPOINTS_DEFINES_H