#ifndef APP_ZIGBEE_LEVELCONTROL_H
#define APP_ZIGBEE_LEVELCONTROL_H

typedef enum eZigbeeLevelControlType
{
  Zb_LevelControl_MoveToLevel = 0,
  Zb_LevelControl_Move = 1,
  Zb_LevelControl_Step = 2,
  Zb_LevelControl_Stop = 3,
} eZigbeeLevelControlType_t;

#endif // APP_ZIGBEE_LEVELCONTROL_H