
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : App/app_zigbee.c
  * Description        : Zigbee Application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  * code partially wirten by Ruben Middelman.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "app_entry.h"
#include "dbg_trace.h"
#include "app_zigbee.h"
#include "zigbee_interface.h"
#include "shci.h"
#include "stm_logging.h"
#include "app_conf.h"
#include "stm32wbxx_core_interface_def.h"
#include "zigbee_types.h"
#include "stm32_seq.h"
#include "callbackmanager.h"
#include "ee.h"
#include "hw_flash.h"

/* Private includes -----------------------------------------------------------*/
#include <assert.h>
#include "zcl/zcl.h"
#include "zcl/general/zcl.onoff.h"
#include "zcl/general/zcl.level.h"
#include "zigbee.aps.h"

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_STARTUP_FAIL_DELAY               500U
#define CHANNEL                                     11
#define CFG_NVM                 1U /* use FLASH */
#define CFG_NVM_DIAG            1U

#define SW1_GROUP_ADDR          0x0001

/* Private macros ------------------------------------------------------------*/

/* External definition -------------------------------------------------------*/
enum ZbStatusCodeT ZbStartupWait(struct ZigBeeT *zb, struct ZbStartupT *config);


/* Private function prototypes -----------------------------------------------*/
static void APP_ZIGBEE_StackLayersInit(void);
static void APP_ZIGBEE_ConfigEndpoints(void);
static void APP_ZIGBEE_NwkForm(void);

static void APP_ZIGBEE_TraceError(const char *pMess, uint32_t ErrCode);
static void APP_ZIGBEE_CheckWirelessFirmwareInfo(void);

static void Wait_Getting_Ack_From_M0(void);
static void Receive_Ack_From_M0(void);
static void Receive_Notification_From_M0(void);
void APP_ZIGBEE_ProcessNotifyM0ToM4(void);
void APP_ZIGBEE_ProcessRequestM0ToM4(void);

static void APP_ZIGBEE_ConfigGroupAddr(void);
static void APP_ZIGBEE_Print_Network_Info(void);

static void APP_ZIGBEE_SW1_Process(void);
static void APP_ZIGBEE_SW2_Process(void);
static void APP_ZIGBEE_SW3_Process(void);
static void APP_ZIGBEE_TraceError(const char *pMess, uint32_t ErrCode);
static void APP_ZIGBEE_CheckWirelessFirmwareInfo(void);
static bool APP_ZIGBEE_persist_load(void);
static bool APP_ZIGBEE_persist_save(void);
static void APP_ZIGBEE_persist_delete(void);
static void APP_ZIGBEE_persist_notify_cb(struct ZigBeeT *zb, void *cbarg);
static enum ZbStatusCodeT APP_ZIGBEE_ZbStartupPersist(struct ZigBeeT *zb);
static void APP_ZIGBEE_PersistCompleted_callback(enum ZbStatusCodeT status,void *arg);
static enum ZclStatusCodeT APP_ZIGBEE_RestoreClusterAttr(struct ZbZclClusterT *clusterPtr);
#ifdef CFG_NVM
static void APP_ZIGBEE_NVM_Init(void);
static bool APP_ZIGBEE_NVM_Read(void);
static bool APP_ZIGBEE_NVM_Write(void);
static void APP_ZIGBEE_NVM_Erase(void);
#endif /* CFG_NVM */
static void APP_ZIGBEE_NVM_Diag_Init(void);
static void APP_ZIGBEE_NVM_Diag_Exec(void);
static bool APP_ZIGBEE_NVM_Diag_DataValidation(void);

/* Private variables ---------------------------------------------------------*/
static TL_CmdPacket_t *p_ZIGBEE_otcmdbuffer;
static TL_EvtPacket_t *p_ZIGBEE_notif_M0_to_M4;
static TL_EvtPacket_t *p_ZIGBEE_request_M0_to_M4;
static __IO uint32_t CptReceiveNotifyFromM0 = 0;
static __IO uint32_t CptReceiveRequestFromM0 = 0;

PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static TL_ZIGBEE_Config_t ZigbeeConfigBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t ZigbeeOtCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ZigbeeNotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ZigbeeNotifRequestBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];

typedef struct {
    uint16_t panId;
    uint16_t shortAddr;
    uint64_t extPanId;
    uint16_t managerAddr;
    uint8_t networkKeySeqNum;
    uint8_t profile;
    uint8_t channel;
    uint8_t activeChannelList[32]; /* For channel mask */
} zigbee_network_params_t;

struct zigbee_app_info {
  struct ZigBeeT *zb;
  enum ZbStartType startupControl;
  enum ZbStatusCodeT join_status;
  uint32_t join_delay;
  uint32_t persistNumWrites;
  bool fresh_startup;
  bool init_after_join;
  zigbee_network_params_t network_params;

  struct ZbZclClusterT *onoff_server_1;
  struct ZbZclClusterT *levelControl_server_1;
  struct ZbZclClusterT *onOff_server_2;
  struct ZbZclClusterT *levelControl_server_2;
  struct ZbZclClusterT *onOff_server_3;
  struct ZbZclClusterT *levelControl_server_3;
  struct ZbZclClusterT *onOff_server_4;
  struct ZbZclClusterT *levelControl_server_4;
  struct ZbZclClusterT *onOff_server_5;
  struct ZbZclClusterT *levelControl_server_5;
  struct ZbZclClusterT *onOff_server_6;
  struct ZbZclClusterT *levelControl_server_6;
  struct ZbZclClusterT *ledstrip_server;
};
static struct zigbee_app_info zigbee_app_info;

/* OnOff server 1 custom callbacks */
static enum ZclStatusCodeT onOff_server_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclOnOffServerCallbacksT OnOffServerCallbacks = {
  .off = onOff_server_off,
  .on = onOff_server_on,
  .toggle = onOff_server_toggle,
};

/* LevelControl server 1 custom callbacks */
static enum ZclStatusCodeT levelControl_server_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclLevelServerCallbacksT LevelServerCallbacks = {
  .move_to_level = levelControl_server_move_to_level,
  .move = levelControl_server_move,
  .step = levelControl_server_step,
  .stop = levelControl_server_stop,
};

/* NVM variables */
/* cache in uninit RAM to store/retrieve persistent data */
union cache
{
  uint8_t  U8_data[ST_PERSIST_MAX_ALLOC_SZ];     // in bytes 
  uint32_t U32_data[ST_PERSIST_MAX_ALLOC_SZ/4U]; // in U32 words
};
// __attribute__ ((section(".noinit"))) union cache cache_persistent_data;

// __attribute__ ((section(".noinit"))) union cache cache_diag_reference;

union cache cache_persistent_data;
union cache cache_diag_reference;


/* timer to delay reading attribute back from persistence */
//static uint8_t TS_ID1; 
static uint8_t TS_ID2;  

/* USER CODE BEGIN PV */
/* USER CODE END PV */
/* Functions Definition ------------------------------------------------------*/

/* OnOff server off 1 command callback */
static enum ZclStatusCodeT onOff_server_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg){
  if(cluster != NULL)
    Callback_RunZigbeeOnOff(cluster->endpoint, Zb_OnOff_Off);
  return ZCL_STATUS_SUCCESS;
}

static enum ZclStatusCodeT onOff_server_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg){
  if(cluster != NULL)
    Callback_RunZigbeeOnOff(cluster->endpoint, Zb_OnOff_On);
  return ZCL_STATUS_SUCCESS;
}

static enum ZclStatusCodeT onOff_server_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg){
  if(cluster != NULL)
    Callback_RunZigbeeOnOff(cluster->endpoint, Zb_OnOff_Toggle);
  return ZCL_STATUS_SUCCESS;
}

static enum ZclStatusCodeT levelControl_server_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg){
  if(cluster != NULL)
	  Callback_RunZigbeeLevelControl(cluster->endpoint, Zb_LevelControl_MoveToLevel, req->level);
  return ZCL_STATUS_SUCCESS;
}

static enum ZclStatusCodeT levelControl_server_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg){
  if(cluster != NULL)
	  Callback_RunZigbeeLevelControl(cluster->endpoint, Zb_LevelControl_Move, NULL);
  return ZCL_STATUS_SUCCESS;
}

/* LevelControl server step 1 command callback */
static enum ZclStatusCodeT levelControl_server_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg){
  if(cluster != NULL)
	  Callback_RunZigbeeLevelControl(cluster->endpoint, Zb_LevelControl_Step, NULL);
  return ZCL_STATUS_SUCCESS;
}

/* LevelControl server stop 1 command callback */
static enum ZclStatusCodeT levelControl_server_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg){
  if(cluster != NULL)
	  Callback_RunZigbeeLevelControl(cluster->endpoint, Zb_LevelControl_Stop, NULL);
  return ZCL_STATUS_SUCCESS;
}

/**
 * @brief  Zigbee application initialization
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_Init(void)
{
  SHCI_CmdStatus_t ZigbeeInitStatus;

  APP_DBG("APP_ZIGBEE_Init");

  /* Check the compatibility with the Coprocessor Wireless Firmware loaded */
  APP_ZIGBEE_CheckWirelessFirmwareInfo();

  /* Register cmdbuffer */
  APP_ZIGBEE_RegisterCmdBuffer(&ZigbeeOtCmdBuffer);

  /* Init config buffer and call TL_ZIGBEE_Init */
  APP_ZIGBEE_TL_INIT();

  /* NVM Init */
#if CFG_NVM
  APP_ZIGBEE_NVM_Init(); 
#endif
  /* Register task */
  /* Create the different tasks */

  UTIL_SEQ_RegTask(1U << (uint32_t)CFG_TASK_NOTIFY_FROM_M0_TO_M4, UTIL_SEQ_RFU, APP_ZIGBEE_ProcessNotifyM0ToM4);
  UTIL_SEQ_RegTask(1U << (uint32_t)CFG_TASK_REQUEST_FROM_M0_TO_M4, UTIL_SEQ_RFU, APP_ZIGBEE_ProcessRequestM0ToM4);

  /* Task associated with network creation process */
  UTIL_SEQ_RegTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, UTIL_SEQ_RFU, APP_ZIGBEE_NwkForm);

  /* Task associated with push button SW1 */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BUTTON_SW1, UTIL_SEQ_RFU, APP_ZIGBEE_SW1_Process); /* on off command*/
  UTIL_SEQ_RegTask(1U << CFG_TASK_BUTTON_SW2, UTIL_SEQ_RFU, APP_ZIGBEE_SW2_Process); /* clear persistence */
  UTIL_SEQ_RegTask(1U << CFG_TASK_BUTTON_SW3, UTIL_SEQ_RFU, APP_ZIGBEE_SW3_Process); /*  */

  /* Start the Zigbee on the CPU2 side */
  ZigbeeInitStatus = SHCI_C2_ZIGBEE_Init();
  /* Prevent unused argument(s) compilation warning */
  UNUSED(ZigbeeInitStatus);

  /* Initialize Zigbee stack layers */
  APP_ZIGBEE_StackLayersInit();

} /* APP_ZIGBEE_Init */

/**
 * @brief  Initialize Zigbee stack layers
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_StackLayersInit(void)
{
  APP_DBG("APP_ZIGBEE_StackLayersInit");

  zigbee_app_info.zb = ZbInit(0U, NULL, NULL);
  assert(zigbee_app_info.zb != NULL);

  /* Create the endpoint and cluster(s) */
  APP_ZIGBEE_ConfigEndpoints();

  /* USER CODE BEGIN APP_ZIGBEE_StackLayersInit */
  /* USER CODE END APP_ZIGBEE_StackLayersInit */

  /* Configure the joining parameters */
  zigbee_app_info.join_status = (enum ZbStatusCodeT) 0x01; /* init to error status */
  zigbee_app_info.join_delay = HAL_GetTick(); /* now */
  zigbee_app_info.startupControl = ZbStartTypeJoin; // Join instead of preconfigured

  /* Initialization Complete */
  zigbee_app_info.fresh_startup = true;
  /* STEP 1 - TRY to START FROM PERSISTENCE */
  
/* define if we need to do a fresh start */
  zigbee_app_info.fresh_startup = true;
  
  /* First we disable the persistent notification */
  ZbPersistNotifyRegister(zigbee_app_info.zb,NULL,NULL);
 
  /* Call a startup from persistence */
  enum ZbStatusCodeT status = APP_ZIGBEE_ZbStartupPersist(zigbee_app_info.zb);
  if(status == ZB_STATUS_SUCCESS)
  {
     /* no fresh stratup need anymore */
     zigbee_app_info.fresh_startup = false; 
     APP_DBG("ZbStartupPersist: SUCCESS, restarted from persistence");
     BSP_LED_On(LED_GREEN);
  }
  else
  {
       /* Start-up form persistence failed perform a fresh ZbStartup */
       APP_DBG("ZbStartupPersist: FAILED to restart from persistence with status: 0x%02x",status);
  }
  
  if(zigbee_app_info.fresh_startup)
  {
    /* Go for fresh start */
    UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, CFG_SCH_PRIO_0);
  }
} /* APP_ZIGBEE_StackLayersInit */

/**
 * @brief  Configure Zigbee application endpoints
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ConfigEndpoints(void)
{
  struct ZbApsmeAddEndpointReqT req, req2;
  struct ZbApsmeAddEndpointConfT conf, conf2;

  memset(&req, 0, sizeof(req));

  /* Endpoint: SW1_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_LEVEL_SWITCH;
  req.endpoint = SW1_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onoff_server_1 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, &OnOffServerCallbacks, NULL);
  assert(zigbee_app_info.onoff_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onoff_server_1);

  /* LevelControl server */
  zigbee_app_info.levelControl_server_1 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, zigbee_app_info.onoff_server_1, &LevelServerCallbacks, NULL);
  assert(zigbee_app_info.levelControl_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_1);

  memset(&req2, 0, sizeof(req2));
  /* Endpoint: SW2_ENDPOINT */
  req2.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req2.deviceId = ZCL_DEVICE_ONOFF_SWITCH;
  req2.endpoint = SW2_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req2, &conf2);
  assert(conf2.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onOff_server_2 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW2_ENDPOINT, &OnOffServerCallbacks, NULL);
  assert(zigbee_app_info.onOff_server_2 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_2);

  /* LevelControl server */
  zigbee_app_info.levelControl_server_2 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW2_ENDPOINT, zigbee_app_info.onOff_server_2, &LevelServerCallbacks, NULL);
  assert(zigbee_app_info.levelControl_server_2 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_2);

  /* Endpoint: SW3_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_ONOFF_SWITCH;
  req.endpoint = SW3_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onOff_server_3 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW3_ENDPOINT, &OnOffServerCallbacks, NULL);
  assert(zigbee_app_info.onOff_server_3 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_3);

  /* LevelControl server */
  zigbee_app_info.levelControl_server_3 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW3_ENDPOINT, zigbee_app_info.onOff_server_3, &LevelServerCallbacks, NULL);
  assert(zigbee_app_info.levelControl_server_3 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_3);

  /* Endpoint: SW4_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_LEVEL_SWITCH;
  req.endpoint = SW4_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onOff_server_4 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW4_ENDPOINT, &OnOffServerCallbacks, NULL);
  assert(zigbee_app_info.onOff_server_4 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_4);

  /* LevelControl server */
  zigbee_app_info.levelControl_server_4 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW4_ENDPOINT, zigbee_app_info.onOff_server_4, &LevelServerCallbacks, NULL);
  assert(zigbee_app_info.levelControl_server_4 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_4);

  /* Endpoint: SW5_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_LEVEL_SWITCH;
  req.endpoint = SW5_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onOff_server_5 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW5_ENDPOINT, &OnOffServerCallbacks, NULL);
  assert(zigbee_app_info.onOff_server_5 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_5);

  /* LevelControl server */
  zigbee_app_info.levelControl_server_5 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW5_ENDPOINT, zigbee_app_info.onOff_server_5, &LevelServerCallbacks, NULL);
  assert(zigbee_app_info.levelControl_server_5 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_5);

  /* Endpoint: SW6_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_LEVEL_SWITCH;
  req.endpoint = SW6_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onOff_server_6 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW6_ENDPOINT, &OnOffServerCallbacks, NULL);
  assert(zigbee_app_info.onOff_server_6 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_6);

  /* LevelControl server */
  zigbee_app_info.levelControl_server_6 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW6_ENDPOINT, zigbee_app_info.onOff_server_6, &LevelServerCallbacks, NULL);
  assert(zigbee_app_info.levelControl_server_6 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_6);

  /* Endpoint: SW6_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;   /* 0x0104 */
  req.deviceId  = CUSTOM_DEVICE_ID_LEDSTRIP;     /* 0x8001 - custom type */
  req.endpoint = SW7_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);
  
  /* Allocate custom LED effects cluster (Manufacturer Specific) */
  zigbee_app_info.ledstrip_server = ZbZclLedstripServerAlloc(zigbee_app_info.zb, SW7_ENDPOINT);
  assert(zigbee_app_info.ledstrip_server != NULL);
  /* Register cluster on endpoint */
  ZbZclClusterEndpointRegister(zigbee_app_info.ledstrip_server);

} /* APP_ZIGBEE_ConfigEndpoints */

/**
 * @brief  Handle Zigbee network forming and joining
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_NwkForm(void)
{
  if ((zigbee_app_info.join_status != ZB_STATUS_SUCCESS) && (HAL_GetTick() >= zigbee_app_info.join_delay))
  {
    struct ZbStartupT config;
    enum ZbStatusCodeT status;

    /* Configure Zigbee Logging */
    ZbSetLogging(zigbee_app_info.zb, ZB_LOG_MASK_LEVEL_5, NULL);

    /* Attempt to join a zigbee network */
    ZbStartupConfigGetProDefaults(&config);

    /* Set the centralized network */
    APP_DBG("Network config : APP_STARTUP_CENTRALIZED_ROUTER");
    config.startupControl = zigbee_app_info.startupControl;

    memcpy(config.security.preconfiguredLinkKey, sec_key_ha, ZB_SEC_KEYSIZE);
    //channel mask needs to be changed from how CubeIDE creates it.
    //CubeIDE expects the channel is known for the application but in most cases it isn't.
    //so instead of the normal channel we set the channel mask to the whole Zigbee range.
    config.channelList.count = 1;
    config.channelList.list[0].page = 0;
    config.channelList.list[0].channelMask = WPAN_CHANNELMASK_2400MHZ; /*Channel in use */

    /* Using ZbStartupWait (blocking) */
    status = ZbStartupWait(zigbee_app_info.zb, &config);

    APP_DBG("ZbStartup Callback (status = 0x%02x)", status);
    zigbee_app_info.join_status = status;

    if (status == ZB_STATUS_SUCCESS) 
    {
      APP_ZIGBEE_Print_Network_Info();
      zigbee_app_info.join_delay = 0U;
      /* Register Persistent data change notification */
      ZbPersistNotifyRegister(zigbee_app_info.zb,APP_ZIGBEE_persist_notify_cb,NULL);
      /* Call the callback once here to save persistence data */
      APP_ZIGBEE_persist_notify_cb(zigbee_app_info.zb,NULL);
    }
    else
    {
      /* USER CODE END 42 */
      APP_DBG("Startup failed, attempting again after a short delay (%d ms)", APP_ZIGBEE_STARTUP_FAIL_DELAY);
      zigbee_app_info.join_delay = HAL_GetTick() + APP_ZIGBEE_STARTUP_FAIL_DELAY;
    }
  }

  /* If Network forming/joining was not successful reschedule the current task to retry the process */
  if (zigbee_app_info.join_status != ZB_STATUS_SUCCESS)
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, CFG_SCH_PRIO_0);
  }

  /* USER CODE BEGIN NW_FORM */
  else
  {
    zigbee_app_info.init_after_join = false;

    /* Assign ourselves to the group addresses */
    APP_ZIGBEE_ConfigGroupAddr();

    /* Since we're using group addressing (broadcast), shorten the broadcast timeout */
    uint32_t bcast_timeout = 3;
    ZbNwkSet(zigbee_app_info.zb, ZB_NWK_NIB_ID_NetworkBroadcastDeliveryTime, &bcast_timeout, sizeof(bcast_timeout));
  }
  /* USER CODE END NW_FORM */
} /* APP_ZIGBEE_NwkForm */

/*************************************************************
 * ZbStartupWait Blocking Call
 *************************************************************/
struct ZbStartupWaitInfo {
  bool active;
  enum ZbStatusCodeT status;
};

static void ZbStartupWaitCb(enum ZbStatusCodeT status, void *cb_arg)
{
  struct ZbStartupWaitInfo *info = cb_arg;

  info->status = status;
  info->active = false;
  UTIL_SEQ_SetEvt(EVENT_ZIGBEE_STARTUP_ENDED);
} /* ZbStartupWaitCb */

enum ZbStatusCodeT ZbStartupWait(struct ZigBeeT *zb, struct ZbStartupT *config)
{
  struct ZbStartupWaitInfo *info;
  enum ZbStatusCodeT status;

  info = malloc(sizeof(struct ZbStartupWaitInfo));
  if (info == NULL) {
    return ZB_STATUS_ALLOC_FAIL;
  }
  memset(info, 0, sizeof(struct ZbStartupWaitInfo));

  info->active = true;
  status = ZbStartup(zb, config, ZbStartupWaitCb, info);
  if (status != ZB_STATUS_SUCCESS) {
    info->active = false;
    return status;
  }
  UTIL_SEQ_WaitEvt(EVENT_ZIGBEE_STARTUP_ENDED);
  status = info->status;
  free(info);
  return status;
} /* ZbStartupWait */

/**
 * @brief  Trace the error or the warning reported.
 * @param  ErrId :
 * @param  ErrCode
 * @retval None
 */
void APP_ZIGBEE_Error(uint32_t ErrId, uint32_t ErrCode)
{
  switch (ErrId) {
  default:
    APP_ZIGBEE_TraceError("ERROR Unknown ", 0);
    break;
  }
} /* APP_ZIGBEE_Error */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/**
 * @brief  Warn the user that an error has occurred.
 *
 * @param  pMess  : Message associated to the error.
 * @param  ErrCode: Error code associated to the module (Zigbee or other module if any)
 * @retval None
 */
static void APP_ZIGBEE_TraceError(const char *pMess, uint32_t ErrCode)
{
  APP_DBG("**** Fatal error = %s (Err = %d)", pMess, ErrCode);
  /* USER CODE BEGIN TRACE_ERROR */
  while (1U == 1U) {
  }
  /* USER CODE END TRACE_ERROR */

} /* APP_ZIGBEE_TraceError */

/**
 * @brief Check if the Coprocessor Wireless Firmware loaded supports Zigbee
 *        and display associated information
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_CheckWirelessFirmwareInfo(void)
{
  WirelessFwInfo_t wireless_info_instance;
  WirelessFwInfo_t *p_wireless_info = &wireless_info_instance;

  if (SHCI_GetWirelessFwInfo(p_wireless_info) != SHCI_Success) {
    APP_ZIGBEE_Error((uint32_t)ERR_ZIGBEE_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
  }
  else {
    APP_DBG("**********************************************************");
    APP_DBG("WIRELESS COPROCESSOR FW:");
    /* Print version */
    APP_DBG("VERSION ID = %d.%d.%d", p_wireless_info->VersionMajor, p_wireless_info->VersionMinor, p_wireless_info->VersionSub);

    switch (p_wireless_info->StackType) {
    case INFO_STACK_TYPE_ZIGBEE_FFD:
      APP_DBG("FW Type : FFD Zigbee stack");
      break;
   case INFO_STACK_TYPE_ZIGBEE_RFD:
      APP_DBG("FW Type : RFD Zigbee stack");
      break;
    default:
      /* No Zigbee device supported ! */
      APP_ZIGBEE_Error((uint32_t)ERR_ZIGBEE_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
      break;
    }
    APP_DBG("**********************************************************");
  }
} /* APP_ZIGBEE_CheckWirelessFirmwareInfo */

static bool APP_ZIGBEE_persist_load(void)
{
#ifdef CFG_NVM
    APP_DBG("Retrieving persistent data from FLASH");
    return APP_ZIGBEE_NVM_Read();
#else
    /* Check length range */
    if ((cache_persistent_data.U32_data[0] == 0) ||
        (cache_persistent_data.U32_data[0] > ST_PERSIST_MAX_ALLOC_SZ))
    {
        APP_DBG("No data or too large length : %d",cache_persistent_data.U32_data[0]);
        return false;
    }
    return true;
#endif /* CFG_NVM */
} /* APP_ZIGBEE_persist_load */

/**
 * @brief  Save persistent data 
 * @param  None
 * @retval true if success , false if fail
 */
static bool APP_ZIGBEE_persist_save(void)
{
    uint32_t len;

    /* Clear the RAM cache before saving */
    memset(cache_persistent_data.U8_data, 0x00, ST_PERSIST_MAX_ALLOC_SZ);

    /* Call the satck API t get current persistent data */
    len = ZbPersistGet(zigbee_app_info.zb, 0, 0);
    /* Check Length range */
    if (len == 0U)
    {
        /* If the persistence length was zero then no data available. */
        APP_DBG("APP_ZIGBEE_persist_save: no persistence data to save !");
        return false;
    }
    if (len > ST_PERSIST_MAX_ALLOC_SZ)
    {
        /* if persistence length to big to store */
        APP_DBG("APP_ZIGBEE_persist_save: persist size too large for storage (%d)", len);
        return false;
    }

    /* Store in cache the persistent data */
    len = ZbPersistGet(zigbee_app_info.zb, &cache_persistent_data.U8_data[ST_PERSIST_FLASH_DATA_OFFSET], len);

    /* Store in cache the persistent data length */
    cache_persistent_data.U32_data[0] = len;
   
    zigbee_app_info.persistNumWrites++;
    APP_DBG("APP_ZIGBEE_persist_save: Persistence written in cache RAM (num writes = %d) len=%d",
             zigbee_app_info.persistNumWrites, cache_persistent_data.U32_data[0]);

#ifdef CFG_NVM
    if(!APP_ZIGBEE_NVM_Write())
    {
      return false;
    }
    APP_DBG("APP_ZIGBEE_persist_save: Persistent data FLASHED");
#endif /* CFG_NVM */
    
    return true;
} /* APP_ZIGBEE_persist_save */

/**
 * @brief  Delete persistent data 
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_persist_delete(void)
{
  /* Clear RAM cache */
   memset(cache_persistent_data.U8_data, 0x00, ST_PERSIST_MAX_ALLOC_SZ);
   APP_DBG("Persistent Data RAM cache cleared");
#ifdef CFG_NVM
   APP_DBG("FLASH ERASED");
   APP_ZIGBEE_NVM_Erase();
#endif /* CFG_NVM */
} /* APP_ZIGBEE_persist_delete */

static void APP_ZIGBEE_persist_notify_cb(struct ZigBeeT *zb, void *cbarg)
{
  APP_DBG("Notification to save persistent data requested from stack");
  /* Save the persistent data */
  APP_ZIGBEE_persist_save();
}

/**
 * @brief  Start Zigbee Network from persistent data
 * @param  zb: Zigbee device object pointer
 * @retval Zigbee stack Status code
 */
static enum ZbStatusCodeT APP_ZIGBEE_ZbStartupPersist(struct ZigBeeT* zb)
{
   bool read_status;
   enum ZbStatusCodeT status = ZB_STATUS_SUCCESS;

   /* Restore persistence */
   read_status = APP_ZIGBEE_persist_load();
    
   if (read_status)
   {
       /* Make sure the EPID is cleared, before we are allowed to restore persistence */
       uint64_t epid = 0U;
       ZbNwkSet(zb, ZB_NWK_NIB_ID_ExtendedPanId, &epid, sizeof(uint64_t));
       
       /* Start-up from persistence */
       APP_DBG("APP_ZIGBEE_ZbStartupPersist: restoring stack persistence");
       status = ZbStartupPersist(zb, &cache_persistent_data.U8_data[4], cache_persistent_data.U32_data[0],NULL,APP_ZIGBEE_PersistCompleted_callback,NULL);
   }
   else
   {
       /* Failed to restart from persistence */ 
       APP_DBG("APP_ZIGBEE_ZbStartupPersist: no persistence data to restore");
       status = ZB_STATUS_ALLOC_FAIL;
   }

   /* Only for debug purpose, depending of persistent data, following traces 
      could display bytes that are irrelevants to on off cluster */ 
   if(status == ZB_STATUS_SUCCESS)
   {
     /* read the last bytes of data where the ZCL on off persistent data shall be*/
      uint32_t len = cache_persistent_data.U32_data[0] + 4 ;
      APP_DBG("ClusterID %02x %02x",cache_persistent_data.U8_data[len-9],cache_persistent_data.U8_data[len-10]);
      APP_DBG("Endpoint %02x %02x",cache_persistent_data.U8_data[len-7],cache_persistent_data.U8_data[len-8]);
      APP_DBG("Direction %02x",cache_persistent_data.U8_data[len-6]);
      APP_DBG("AttrID %02x %02x",cache_persistent_data.U8_data[len-4],cache_persistent_data.U8_data[len-5]);
      APP_DBG("Len %02x %02x",cache_persistent_data.U8_data[len-2],cache_persistent_data.U8_data[len-3]);
      APP_DBG("Value %02x",cache_persistent_data.U8_data[len-1]);
   }
   
   return status;
}/* APP_ZIGBEE_ZbStartupPersist */

/**
 * @brief  timer callback to wait end of restore cluster persistence form M0 
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_PersistCompleted_callback(enum ZbStatusCodeT status,void *arg)
{
   if(status == ZB_WPAN_STATUS_SUCCESS)
   {
  APP_DBG("Persist complete callback entered with SUCCESS");
  /* Restore the on/off value based on persitence loaded */
   if(APP_ZIGBEE_RestoreClusterAttr(zigbee_app_info.onoff_server_1)==ZCL_STATUS_SUCCESS)
   {
      APP_DBG("Read back OnOff cluster attribute : SUCCESS");
   }
   else
   {
     APP_DBG("Read back OnOff cluster attribute : FAILED");
   }
  }
  else
  {
    APP_DBG("Error in persist complete callback %x",status);
  }
   /* STEP3 - Activate back the persistent notofacation */
     /* Register Persistent data change notification */
     ZbPersistNotifyRegister(zigbee_app_info.zb,APP_ZIGBEE_persist_notify_cb,NULL);
                                                         
     /* Call the callback once here to save persistence data */
     APP_ZIGBEE_persist_notify_cb(zigbee_app_info.zb,NULL); 
}/* APP_ZIGBEE_PersistCompleted_callback */

/**
 * @brief  read on off attribute after a startup form persistence
 * @param  clusterPtr: pointer to cluster
 *         
 * @retval stack status code
 */
static enum ZclStatusCodeT APP_ZIGBEE_RestoreClusterAttr(struct ZbZclClusterT *clusterPtr)
{
    uint8_t attrVal;

    if (ZbZclAttrRead(clusterPtr, ZCL_ONOFF_ATTR_ONOFF, NULL,
            &attrVal, sizeof(attrVal), false) != ZCL_STATUS_SUCCESS) 
    {
        return ZCL_STATUS_FAILURE;
    }
    if (attrVal)
    {
        APP_DBG("RESTORE LED_RED TO ON");
        // BSP_LED_On(LED_RED);
    }
    else 
    {
        APP_DBG("RESTORE LED_RED TO OFF");
        // BSP_LED_Off(LED_RED);
    }

    return ZCL_STATUS_SUCCESS;
} /* APP_ZIGBEE_RestoreClusterAttr */

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

void APP_ZIGBEE_RegisterCmdBuffer(TL_CmdPacket_t *p_buffer)
{
  p_ZIGBEE_otcmdbuffer = p_buffer;
} /* APP_ZIGBEE_RegisterCmdBuffer */

Zigbee_Cmd_Request_t * ZIGBEE_Get_OTCmdPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)p_ZIGBEE_otcmdbuffer->cmdserial.cmd.payload;
} /* ZIGBEE_Get_OTCmdPayloadBuffer */

Zigbee_Cmd_Request_t * ZIGBEE_Get_OTCmdRspPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)((TL_EvtPacket_t *)p_ZIGBEE_otcmdbuffer)->evtserial.evt.payload;
} /* ZIGBEE_Get_OTCmdRspPayloadBuffer */

Zigbee_Cmd_Request_t * ZIGBEE_Get_NotificationPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)(p_ZIGBEE_notif_M0_to_M4)->evtserial.evt.payload;
} /* ZIGBEE_Get_NotificationPayloadBuffer */

Zigbee_Cmd_Request_t * ZIGBEE_Get_M0RequestPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)(p_ZIGBEE_request_M0_to_M4)->evtserial.evt.payload;
}

/**
 * @brief  This function is used to transfer the commands from the M4 to the M0.
 *
 * @param   None
 * @return  None
 */
void ZIGBEE_CmdTransfer(void)
{
  Zigbee_Cmd_Request_t *cmd_req = (Zigbee_Cmd_Request_t *)p_ZIGBEE_otcmdbuffer->cmdserial.cmd.payload;

  /* Zigbee OT command cmdcode range 0x280 .. 0x3DF = 352 */
  p_ZIGBEE_otcmdbuffer->cmdserial.cmd.cmdcode = 0x280U;
  /* Size = otCmdBuffer->Size (Number of OT cmd arguments : 1 arg = 32bits so multiply by 4 to get size in bytes)
   * + ID (4 bytes) + Size (4 bytes) */
  p_ZIGBEE_otcmdbuffer->cmdserial.cmd.plen = 8U + (cmd_req->Size * 4U);

  TL_ZIGBEE_SendM4RequestToM0();

  /* Wait completion of cmd */
  Wait_Getting_Ack_From_M0();
} /* ZIGBEE_CmdTransfer */

/**
 * @brief  This function is called when the M0+ acknowledge the fact that it has received a Cmd
 *
 *
 * @param   Otbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_CmdEvtReceived(TL_EvtPacket_t *Otbuffer)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Otbuffer);

  Receive_Ack_From_M0();
} /* TL_ZIGBEE_CmdEvtReceived */

/**
 * @brief  This function is called when notification from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_NotReceived(TL_EvtPacket_t *Notbuffer)
{
  p_ZIGBEE_notif_M0_to_M4 = Notbuffer;

  Receive_Notification_From_M0();
} /* TL_ZIGBEE_NotReceived */

/**
 * @brief  This function is called before sending any ot command to the M0
 *         core. The purpose of this function is to be able to check if
 *         there are no notifications coming from the M0 core which are
 *         pending before sending a new ot command.
 * @param  None
 * @retval None
 */
void Pre_ZigbeeCmdProcessing(void)
{
  UTIL_SEQ_WaitEvt(EVENT_SYNCHRO_BYPASS_IDLE);
} /* Pre_ZigbeeCmdProcessing */

/**
 * @brief  This function waits for getting an acknowledgment from the M0.
 *
 * @param  None
 * @retval None
 */
static void Wait_Getting_Ack_From_M0(void)
{
  UTIL_SEQ_WaitEvt(EVENT_ACK_FROM_M0_EVT);
} /* Wait_Getting_Ack_From_M0 */

/**
 * @brief  Receive an acknowledgment from the M0+ core.
 *         Each command send by the M4 to the M0 are acknowledged.
 *         This function is called under interrupt.
 * @param  None
 * @retval None
 */
static void Receive_Ack_From_M0(void)
{
  UTIL_SEQ_SetEvt(EVENT_ACK_FROM_M0_EVT);
} /* Receive_Ack_From_M0 */

/**
 * @brief  Receive a notification from the M0+ through the IPCC.
 *         This function is called under interrupt.
 * @param  None
 * @retval None
 */
static void Receive_Notification_From_M0(void)
{
    CptReceiveNotifyFromM0++;
    UTIL_SEQ_SetTask(1U << (uint32_t)CFG_TASK_NOTIFY_FROM_M0_TO_M4, CFG_SCH_PRIO_0);
}

/**
 * @brief  This function is called when a request from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_M0RequestReceived(TL_EvtPacket_t *Reqbuffer)
{
    p_ZIGBEE_request_M0_to_M4 = Reqbuffer;

    CptReceiveRequestFromM0++;
    UTIL_SEQ_SetTask(1U << (uint32_t)CFG_TASK_REQUEST_FROM_M0_TO_M4, CFG_SCH_PRIO_0);
}

/**
 * @brief Perform initialization of TL for Zigbee.
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_TL_INIT(void)
{
    ZigbeeConfigBuffer.p_ZigbeeOtCmdRspBuffer = (uint8_t *)&ZigbeeOtCmdBuffer;
    ZigbeeConfigBuffer.p_ZigbeeNotAckBuffer = (uint8_t *)ZigbeeNotifRspEvtBuffer;
    ZigbeeConfigBuffer.p_ZigbeeNotifRequestBuffer = (uint8_t *)ZigbeeNotifRequestBuffer;
    TL_ZIGBEE_Init(&ZigbeeConfigBuffer);
}

/**
 * @brief Process the messages coming from the M0.
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_ProcessNotifyM0ToM4(void)
{
    if (CptReceiveNotifyFromM0 != 0) {
        /* If CptReceiveNotifyFromM0 is > 1. it means that we did not serve all the events from the radio */
        if (CptReceiveNotifyFromM0 > 1U) {
            APP_ZIGBEE_Error(ERR_REC_MULTI_MSG_FROM_M0, 0);
        }
        else {
            Zigbee_CallBackProcessing();
        }
        /* Reset counter */
        CptReceiveNotifyFromM0 = 0;
    }
}

/**
 * @brief Process the requests coming from the M0.
 * @param
 * @return
 */
void APP_ZIGBEE_ProcessRequestM0ToM4(void)
{
    if (CptReceiveRequestFromM0 != 0) {
        Zigbee_M0RequestProcessing();
        CptReceiveRequestFromM0 = 0;
    }
}
/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/**
 * @brief  Set group addressing mode
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ConfigGroupAddr(void)
{
  struct ZbApsmeAddGroupReqT req;
  struct ZbApsmeAddGroupConfT conf;

  memset(&req, 0, sizeof(req));
  req.endpt = SW1_ENDPOINT;
  req.groupAddr = SW1_GROUP_ADDR;
  ZbApsmeAddGroupReq(zigbee_app_info.zb, &req, &conf);

} /* APP_ZIGBEE_ConfigGroupAddr */

static void APP_ZIGBEE_Print_Network_Info(void)
{
  
  zigbee_app_info.join_delay = 0U;
  enum ZbStatusCodeT status;
  /* Get PAN ID */
  status = ZbNwkGet(zigbee_app_info.zb, ZB_NWK_NIB_ID_PanId, 
                  &zigbee_app_info.network_params.panId, 
                  sizeof(zigbee_app_info.network_params.panId));
  if (status != ZB_STATUS_SUCCESS) {
      APP_DBG("Failed to get PAN ID: 0x%02x", status);
  }
  
  /* Get Extended PAN ID */
  status = ZbNwkGet(zigbee_app_info.zb, ZB_NWK_NIB_ID_ExtendedPanId,
                  &zigbee_app_info.network_params.extPanId,
                  sizeof(zigbee_app_info.network_params.extPanId));
  if (status != ZB_STATUS_SUCCESS) {
      APP_DBG("Failed to get Extended PAN ID: 0x%02x", status);
  }
  
  /* Get Network Manager Address */
  status = ZbNwkGet(zigbee_app_info.zb, ZB_NWK_NIB_ID_ManagerAddr,
                  &zigbee_app_info.network_params.managerAddr,
                  sizeof(zigbee_app_info.network_params.managerAddr));
  if (status != ZB_STATUS_SUCCESS) {
      APP_DBG("Failed to get Manager Address: 0x%02x", status);
  }
  
  /* Get Short Address */
  status = ZbNwkGet(zigbee_app_info.zb, ZB_NWK_NIB_ID_NetworkAddress, 
                    &zigbee_app_info.network_params.shortAddr, 
                    sizeof(zigbee_app_info.network_params.shortAddr));
  if (status != ZB_STATUS_SUCCESS) {
      APP_DBG("Failed to get Short Address: 0x%02x", status);
  }

  /* Get Network Active Key Seq Number */
  status = ZbNwkGet(zigbee_app_info.zb, ZB_NWK_NIB_ID_ActiveKeySeqNumber,
                  &zigbee_app_info.network_params.networkKeySeqNum,
                  sizeof(zigbee_app_info.network_params.networkKeySeqNum));
  if (status != ZB_STATUS_SUCCESS) {
      APP_DBG("Failed to get Network Key Sequence Number: 0x%02x", status);
  }

  /* Get Network Manager Address */
  status = ZbNwkGet(zigbee_app_info.zb, ZB_NWK_NIB_ID_StackProfile,
                  &zigbee_app_info.network_params.profile,
                  sizeof(zigbee_app_info.network_params.profile));
  if (status != ZB_STATUS_SUCCESS) {
      APP_DBG("Failed to get Network Key Sequence Number: 0x%02x", status);
  }
  
  /* Get Current Channel Information */
  status = ZbNwkGet(zigbee_app_info.zb, ZB_NWK_NIB_ID_ActiveChannelList,
                  &zigbee_app_info.network_params.activeChannelList,
                  sizeof(zigbee_app_info.network_params.activeChannelList));
  if (status == ZB_STATUS_SUCCESS) {
      /* Find the active channel */
      for (int i = 0; i < 32; i++) {
          if (zigbee_app_info.network_params.activeChannelList[i] != 0) {
              zigbee_app_info.network_params.channel = i + CHANNEL; /* Channels start at 11 */
              break;
          }
      }
  }
  else {
      APP_DBG("Failed to get Channel List: 0x%02x", status);
  }
  
  struct ZbNwkSecMaterialT active_key = {0};
  ZbNwkGetActiveKey(zigbee_app_info.zb, &active_key);
  char activeKey[ZB_SEC_KEYSIZE*2] = {0};
  char currentByte[16] = {0};
  for(int i = 0; i<ZB_SEC_KEYSIZE; i++)
  {
    snprintf(currentByte, sizeof(currentByte), "%02x", active_key.key[i]);
    strcat(activeKey, currentByte);
  }

  /* Log the retrieved parameters */
  APP_DBG("Active Key information:");
  APP_DBG("  ActiveKey: 0x%s", activeKey);
  APP_DBG("  Active Sequence Key Number: %d", active_key.keySeqNumber);
  APP_DBG("  Key Type: %d", active_key.keyType);
  APP_DBG("Retrieved Network Parameters:");
  APP_DBG("  PAN ID: 0x%04x", zigbee_app_info.network_params.panId);
  APP_DBG("  Extended PAN ID: 0x%016llx", zigbee_app_info.network_params.extPanId);
  APP_DBG("  Manager Address: 0x%04x", zigbee_app_info.network_params.managerAddr);
  APP_DBG("  Short Address: 0x%04x", zigbee_app_info.network_params.shortAddr);
  APP_DBG("  Active Key Sequence Number: %d", zigbee_app_info.network_params.profile);
  APP_DBG("  Stack Profile: %d", zigbee_app_info.network_params.profile);
  APP_DBG("  Current Channel: %d", zigbee_app_info.network_params.channel);
}

/* USER CODE END FD_LOCAL_FUNCTIONS */

#ifdef CFG_NVM
/**
 * @brief  Init the NVM 
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_NVM_Init(void)
{
  int eeprom_init_status;
  
  APP_DBG("Flash starting address = %x",HW_FLASH_ADDRESS  + CFG_NVM_BASE_ADDRESS);
  eeprom_init_status = EE_Init( 0 , HW_FLASH_ADDRESS + CFG_NVM_BASE_ADDRESS );
 
  if(eeprom_init_status != EE_OK)
  {
    /* format NVM since init failed */
    eeprom_init_status= EE_Init( 1, HW_FLASH_ADDRESS + CFG_NVM_BASE_ADDRESS );
  }
  APP_DBG("EE_init status = %d",eeprom_init_status);

} /* APP_ZIGBEE_NVM_Init */

/**
*@brief  Read the persistent data from NVM
* @param  None
* @retval true if success , false if failed
*/
static bool APP_ZIGBEE_NVM_Read(void)
{
    uint16_t num_words = 0;
    bool status = true;
    int ee_status = 0;
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR | FLASH_FLAG_OPTVERR); 

    /* Read the data length from cache */
    ee_status = EE_Read(0, ZIGBEE_DB_START_ADDR, &cache_persistent_data.U32_data[0]);
    if (ee_status != EE_OK)
    {
        APP_DBG("Read -> persistent data length not found ERASE to be done - Read Stopped");
        status = false;
    }
      /* Check length is not too big nor zero */
    else if((cache_persistent_data.U32_data[0] == 0) || 
            (cache_persistent_data.U32_data[0]> ST_PERSIST_MAX_ALLOC_SZ))
    {
            APP_DBG("No data or too large length : %d", cache_persistent_data.U32_data[0]);
            status = false;
    }
        /* Length is within range */
    else
    {
           /* Adjust the length to be U32 aligned */
            num_words = (uint16_t) (cache_persistent_data.U32_data[0]/4) ;
            if (cache_persistent_data.U32_data[0] % 4 != 0)
            {
                num_words++;
            }

            /* copy the read data from Flash to cache including length */
            for (uint16_t local_length = 1; local_length <= num_words; local_length++)
            {
            	if (local_length >= ST_PERSIST_MAX_ALLOC_SZ/4)
            	{
                    APP_DBG("Local length exceeds the size of the cache persistent data!");
                    status = false;
                    break;
            	}

                /* read data from first data in U32 unit */
                ee_status = EE_Read(0, local_length + ZIGBEE_DB_START_ADDR, &cache_persistent_data.U32_data[local_length] );
                if (ee_status != EE_OK)
                {
                    APP_DBG("Read not found leaving");
                    status = false;
                    break;
                }
            }
    }
    
    HAL_FLASH_Lock();
    if(status)
    {
        APP_DBG("READ PERSISTENT DATA LEN = %d",cache_persistent_data.U32_data[0]);
    }
    return status;
} /* APP_ZIGBEE_NVM_Read */

/**
 * @brief  Write the persistent data in NVM
 * @param  None
 * @retval None
 */
static bool APP_ZIGBEE_NVM_Write(void)
{
    int ee_status = 0;
    
    uint16_t num_words;
    uint16_t local_current_size;
    

    num_words = 1U; /* 1 words for the length */
    num_words+= (uint16_t) (cache_persistent_data.U32_data[0]/4);
    
    
    /* Adjust the length to be U32 aligned */
    if (cache_persistent_data.U32_data[0] % 4 != 0)
    {
        num_words++;
    }
    
    //save data in flash
    for (local_current_size = 0; local_current_size < num_words; local_current_size++)
    {
        ee_status = EE_Write(0, (uint16_t)local_current_size + ZIGBEE_DB_START_ADDR, cache_persistent_data.U32_data[local_current_size]);
        if (ee_status != EE_OK)
        {
           if(ee_status == EE_CLEAN_NEEDED) /* Shall not be there if CFG_EE_AUTO_CLEAN = 1*/
           {
              APP_DBG("CLEAN NEEDED, CLEANING");
              EE_Clean(0,0);
           }
           else
           {
              /* Failed to write , an Erase shall be done */
              APP_DBG("APP_ZIGBEE_NVM_Write failed @ %d status %d", local_current_size,ee_status);
              break;
           }
        }
    }
    
 
    if(ee_status != EE_OK)
    {
       APP_DBG("WRITE STOPPED, need a FLASH ERASE");
       return false;
    }
    
    APP_DBG("WRITTEN PERSISTENT DATA LEN = %d",cache_persistent_data.U32_data[0]);
    return true;

} /* APP_ZIGBEE_NVM_Write */

/**
 * @brief  Erase the NVM
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_NVM_Erase(void)
{
   EE_Init(1, HW_FLASH_ADDRESS + CFG_NVM_BASE_ADDRESS); /* Erase Flash */
} /* APP_ZIGBEE_NVM_Erase */

#endif /* CFG_NVM */


/**
 * @brief SW1 button pushed toggle req send
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_SW1_Process()
{
  struct ZbApsAddrT dst;
  uint64_t epid = 0U;

  if(zigbee_app_info.zb == NULL){
    return;
  }
  
  /* Check if the router joined the network */
  if (ZbNwkGet(zigbee_app_info.zb, ZB_NWK_NIB_ID_ExtendedPanId, &epid, sizeof(epid)) != ZB_STATUS_SUCCESS) {
    return;
  }
  if (epid == 0U) {
    return;
  }

  memset(&dst, 0, sizeof(dst));
  dst.mode = ZB_APSDE_ADDRMODE_GROUP;
  dst.endpoint = SW2_ENDPOINT;
  // dst.nwkAddr = SW2_GROUP_ADDR;

  APP_DBG("SW1 PUSHED (SENDING TOGGLE TO GROUP 0x0002)");
  // if (ZbZclOnOffClientToggleReq(zigbee_app_info.onoff_client_1, &dst, NULL, NULL) != ZCL_STATUS_SUCCESS) {
  //   APP_DBG("Error, ZbZclOnOffClientToggleReq failed (SW2_ENDPOINT)");
  // }
}

/**
 * @brief SW2 button pushed Clean NVM
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_SW2_Process()
{
  APP_DBG("SW2 PUSHED : Clearing NVM");
  
   HW_TS_Create(CFG_TIM_WAIT_BEOFRE_READ_ATTR, &TS_ID2, hw_ts_SingleShot, APP_ZIGBEE_persist_delete);
   HW_TS_Start(TS_ID2, 1000);
 // APP_ZIGBEE_persist_delete();
}

/**
 * @brief SW3 button pushed NVM diagnostic test execution.
 * This will DESTROY the existing NVM data
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_SW3_Process()
{
#ifdef CFG_NVM
    /* Test the NVM existing NVM data will be DESTROY */
    APP_ZIGBEE_NVM_Diag_Exec();
#else
    APP("NVM not activated -> No Test to Execute");
#endif /* CFG_NVM */
} /* APP_ZIGBEE_SW3_Process */


/**
 * @brief Init NVM Diagnostic test. Init test pattern and reset NVM memory.
 * This will DESTROY the existing NVM data
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_NVM_Diag_Init(void)
{
   /* disable notification */
    ZbPersistNotifyRegister(zigbee_app_info.zb,NULL,NULL);
    
    /* Display information on the configuration */
    APP_DBG("");
    APP_DBG("NVM DIAGNOSTICS TEST STARTED")
    APP_DBG("NVM CONFIGURATION");
    APP_DBG("Nb Flash Page allocated : %d",CFG_NB_OF_PAGE);
    APP_DBG("Bank Size : %d bytes",CFG_EE_BANK0_SIZE);
    APP_DBG("Flash Base Address: %08x",(0x08000000+CFG_NVM_BASE_ADDRESS));
    APP_DBG("Persist Max alloc buffer: %d bytes",ST_PERSIST_MAX_ALLOC_SZ);
    APP_DBG("Auto Clean : %d",CFG_EE_AUTO_CLEAN);
    APP_DBG("");
    /* Clear cache and Erase NVM */
    APP_ZIGBEE_persist_delete();
    
    /* build the test pattern */
    cache_persistent_data.U32_data[0]= 725;
    memset(&cache_persistent_data.U8_data[4],0xAA,95);
    memset(&cache_persistent_data.U8_data[100],0xBB,100);
    memset(&cache_persistent_data.U8_data[200],0xCC,100);
    memset(&cache_persistent_data.U8_data[300],0xDD,100);
    memset(&cache_persistent_data.U8_data[400],0xEE,100);
    memset(&cache_persistent_data.U8_data[500],0x77,221);
    cache_persistent_data.U8_data[722] = 0x01;
    cache_persistent_data.U8_data[723] = 0x02;
    cache_persistent_data.U8_data[724] = 0x03;
    cache_persistent_data.U8_data[725] = 0x04;
    
    /* Save the pattern in the reference cache for data validation */
    memcpy(&cache_diag_reference.U8_data[0],&cache_persistent_data.U8_data[0],ST_PERSIST_MAX_ALLOC_SZ);

} /* APP_ZIGBEE_NVM_Diag_Init */

/**
 * @brief Execute  NVM Diagnostic test.
 * 
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_NVM_Diag_Exec(void)
{
  uint16_t test = 1;
  
  bool write_err = false;
  bool read_err = false;
  bool data_err = false;

  
  /* Test init */
  APP_ZIGBEE_NVM_Diag_Init();
  
  /* Execute tests 30 loops is enough to overlapped 2n half of a 64k flash   */
  for (test = 1 ; test <= 30; test++)
  {
    APP_DBG("TEST #%d --> EXECUTION",test);
    APP_DBG(">>> WRITE NVM");
    if(!APP_ZIGBEE_NVM_Write())
    {
      write_err = true;
      break;
    }
    APP_DBG("<<< READ NVM");
    if(!APP_ZIGBEE_NVM_Read())
    {
      read_err = true;
      break;
    }
    APP_DBG("DATA VALIDATION");
    if(!APP_ZIGBEE_NVM_Diag_DataValidation())
    {
      data_err = true;
      break;
    }
    else
    {
      APP_DBG("TEST #%d --> PASSED",test);
      APP_DBG("");
    }
  }
  if(write_err)
  {
     APP_DBG("NVM DIAGNOSTIC TESTS STATUS #%d --> FAILED (write error)",test);
  }
  else if(read_err)
  {
     APP_DBG("NVM DIAGNOSTIC TESTS STATUS #%d --> FAILED (read error)",test);
  }
  else if(data_err)
  {
     APP_DBG("NVM DIAGNOSTIC TESTS STATUS #%d --> FAILED (data integrity error)",test);
  }
  else{
    APP_DBG("NVM DIAGNOSTIC TESTS STATUS #%d --> PASSED",test);
  }
  APP_DBG("");
  APP_DBG("NVM DIAGNOSTIC TESTS IS OVER");
}

/**
 * @brief Execute  NVM Diagnostic data integrity validation.
 * 
 * @param  None
 * @retval None
 */
static bool APP_ZIGBEE_NVM_Diag_DataValidation(void)
{
  bool status = true;
  /* Check length */
  if(cache_persistent_data.U32_data[0] != cache_diag_reference.U32_data[0])
  { 
    APP_DBG("DATA LENGTH FAILED %d vs ref %d",cache_persistent_data.U32_data[0],
                                              cache_diag_reference.U32_data[0]);
    return false;
  }
 
  /* Check data */
  for (uint16_t i = 4 ; i <= 725; i++)
  {
    if(cache_persistent_data.U8_data[i] != cache_diag_reference.U8_data[i])
    {
      APP_DBG("FAILED data validation byte #%d - %02x vs ref %02x",i,
                                     cache_persistent_data.U8_data[i],
                                     cache_diag_reference.U8_data[i] );
      status = false ;
    }
  }
  
  return status;
} /* APP_ZIGBEE_NVM_Diag_DataValidation */