#include "ep_ledstrip.h"
#include "app_common.h"
#include "app_conf.h"
#include "endpoints/ep_defines.h"

typedef struct {
    uint8_t mode;
    uint8_t brightness;
    uint8_t speed;
    uint16_t start_idx;
    uint16_t end_idx;
    uint8_t r, g, b;
    bool active;
} LedstripState_t;

static LedstripState_t g_ledstrip;

static enum ZclStatusCodeT Ledstrip_CommandHandler(
    struct ZbZclClusterT *clusterPtr,
    struct ZbZclHeaderT *hdr,
    struct ZbApsdeDataIndT *ind)
{
    const uint8_t *p = ind->asdu;

    switch (hdr->cmdId) {
    case CMD_SET_MODE:
        g_ledstrip.mode = p[0];
        g_ledstrip.active = true;
        break;

    case CMD_SET_RANGE:
        g_ledstrip.start_idx = (p[1] << 8) | p[0];
        g_ledstrip.end_idx   = (p[3] << 8) | p[2];
        break;

    case CMD_SET_COLOR:
        g_ledstrip.r = p[0];
        g_ledstrip.g = p[1];
        g_ledstrip.b = p[2];
        break;

    case CMD_SET_BRIGHTNESS:
        g_ledstrip.brightness = p[0];
        break;

    case CMD_SET_SPEED:
        g_ledstrip.speed = p[0];
        break;

    case CMD_STOP:
        g_ledstrip.active = false;
        g_ledstrip.mode = 0;
        break;

    default:
        return ZCL_STATUS_UNSUPP_CLUSTER_COMMAND;
    }

    // Call your LED manager to update the strip
    // Set_Current_Event();  
    return ZCL_STATUS_SUCCESS;
}

struct ZbZclClusterT * ZbZclLedstripServerAlloc(struct ZigBeeT *zb, uint8_t endpoint)
{
    struct ZbZclClusterT *cluster;

    cluster = ZbZclClusterAlloc(zb, sizeof(struct ZbZclClusterT), CUSTOM_CLUSTER_ID_LEDSTRIP, endpoint, ZCL_DIRECTION_TO_SERVER);
    if (cluster == NULL) {
        // APP_DBG("Failed to allocate LEDSTRIP cluster");
        return NULL;
    }

    cluster->command = Ledstrip_CommandHandler;

    ZbZclClusterSetProfileId(cluster, ZCL_PROFILE_HOME_AUTOMATION);
    // ZbZclClusterSetDeviceVersion(cluster, 1);

    return cluster;
}
