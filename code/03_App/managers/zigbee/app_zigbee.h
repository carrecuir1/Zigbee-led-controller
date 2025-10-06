/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    App/app_zigbee.h
  * @author  MCD Application Team
  * @brief   Header for Zigbee Application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef APP_ZIGBEE_H
#define APP_ZIGBEE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tl.h"
#include "tl_zigbee_hci.h"

#include "endpoints/ep_defines.h"
#include "endpoints/ep_ledstrip.h"
#include "endpoints/ep_onoff.h"
#include "endpoints/ep_levelcontrol.h"

/*
 *  List of all errors tracked by the Zigbee application
 *  running on M4. Some of these errors may be fatal
 *  or just warnings
 */
typedef enum
{
  ERR_REC_MULTI_MSG_FROM_M0,
  ERR_ZIGBE_CMD_TO_M0,
/* USER CODE BEGIN ERROR_APPLI_ENUM */

/* USER CODE END ERROR_APPLI_ENUM */
  ERR_ZIGBEE_CHECK_WIRELESS
} ErrAppliIdEnum_t;

typedef enum eZigbeeEndpoints
{
  SW1_ENDPOINT = 17,
  SW2_ENDPOINT = 20,
  SW3_ENDPOINT = 21,
  SW4_ENDPOINT = 22,
  SW5_ENDPOINT = 23,
  SW6_ENDPOINT = 24,
  SW7_ENDPOINT = 25
} eZigbeeEndpoints_t;

void APP_ZIGBEE_Init(void);
void APP_ZIGBEE_Error(uint32_t ErrId, uint32_t ErrCode);
void APP_ZIGBEE_RegisterCmdBuffer(TL_CmdPacket_t *p_buffer);
void APP_ZIGBEE_ProcessNotifyM0ToM4(void);
void APP_ZIGBEE_ProcessRequestM0ToM4(void);
void APP_ZIGBEE_TL_INIT(void);
void Pre_ZigbeeCmdProcessing(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_ZIGBEE_H */
