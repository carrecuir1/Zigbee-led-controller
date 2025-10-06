#include "ledsstripmanager.h"
#include "math.h"
#include "app_zigbee.h"
#include <stdbool.h>

typedef struct Led_Data
{
  union
  {
    struct 
    {
      Led_Color_t color;
      uint32_t index;
    }led;
    uint32_t raw;
  };
} Led_Data_t;

typedef struct OnOff_Status_Flag
{
  bool onoff;
  bool breath;
  bool blink;
  bool rainbow;
} OnOff_Status_Flag_t;

#define LEDSSTRIP_MAN_MAX_LEDS      50
#define LEDSSTRIP_MAN_RGB_COLOR_LEN 24
#define LEDSSTRIP_MAN_ZERO_PADDING  50
#define LEDSSTRIP_MAN_BRIGHTNESS    0
#define LEDSSTRIP_MAN_MAX_STEP      100
#define LEDSSTRIP_MAN_NB_LEDS_RAIN  8
#define LEDSSTRIP_MAN_EFFSTEP_BIN   (LEDSSTRIP_MAN_MAX_STEP/3.0)
#define LEDSSTRIP_MAN_IND_STEP      LEDSSTRIP_MAN_MAX_STEP/LEDSSTRIP_MAN_NB_LEDS_RAIN
#define PI                          3.14159265

#define DEG_TO_RAD(deg)                       ((deg) * PI / 180.0f)
#define RAD_TO_DEG(rad)                       ((rad) * 180.0f / PI)
#define GET_COLOR_FROM_LED(blue, green, red)  ((blue<<16) | (green<<8) | (red))

static uint16_t mEffStep = 0;

Led_Configuration_t mLedConfig = {0};
OnOff_Status_Flag_t mOnOffFlag = {0};
Led_Data_t mLEDData[LEDSSTRIP_MAN_MAX_LEDS] = {0};
uint16_t mPwmData[(LEDSSTRIP_MAN_RGB_COLOR_LEN*LEDSSTRIP_MAN_MAX_LEDS)+LEDSSTRIP_MAN_ZERO_PADDING] = {0};

static retcode_t Rainbow_Effect_Right(uint8_t brightness);
static void Set_LED(int pLEDnum, int pRed, int pGreen, int pBlue, float brightness_scale);
static void Set_LED_ALL(int pRed, int pGreen, int pBlue, float brightness_scale);
static void Reset_LED (void);
static void Send_Data(void);
static void Set_Current_Event(void);

// Zigbee Private
static void Set_OnOff_Value(eZigbeeOnOffType_t type, bool* ret);

// Zigbee Callbacks
static void LedsStripManager_ZigbeeOnOff(eZigbeeEndpoints_t endpoint, eZigbeeOnOffType_t type);
static void LedsStripManager_ZigbeeLevelControl(eZigbeeEndpoints_t endpoint, eZigbeeLevelControlType_t type, uint8_t level);

// Timings Callbacks
static void LedsStripManager_Each10ms(void);

// Zigbee Callbacks
static void LedsStripManager_ZigbeeOnOff(eZigbeeEndpoints_t endpoint, eZigbeeOnOffType_t type){
  switch(endpoint)
  {
    case SW1_ENDPOINT:
    // OnOff
    Set_OnOff_Value(type, &mOnOffFlag.onoff);
    break;
    case SW2_ENDPOINT:
    // Breath
    Set_OnOff_Value(type, &mOnOffFlag.breath);
    break;
    case SW3_ENDPOINT:
    // Blink
    Set_OnOff_Value(type, &mOnOffFlag.blink);
    break;
    case SW4_ENDPOINT:
    // Rainbow
    Set_OnOff_Value(type, &mOnOffFlag.rainbow);
    break;
    case SW5_ENDPOINT:
    break;
    case SW6_ENDPOINT:
    break;
    default:
    break;
  }
  Set_Current_Event();
}

static void LedsStripManager_ZigbeeLevelControl(eZigbeeEndpoints_t endpoint, eZigbeeLevelControlType_t type, uint8_t level){
  // TODO : For now, only the MoveToLevel is used
  if(type == Zb_LevelControl_MoveToLevel)
  {
    switch(endpoint)
    {
      case SW1_ENDPOINT:
      // Brightness
      mLedConfig.brightness = level;
      break;
      case SW2_ENDPOINT:
      // Red
      mLedConfig.led_static.red = level;
      break;
      case SW3_ENDPOINT:
      // Green
      mLedConfig.led_static.green = level;
      break;
      case SW4_ENDPOINT:
      // Blue
      mLedConfig.led_static.blue = level;
      break;
      case SW5_ENDPOINT:
      break;
      case SW6_ENDPOINT:
      break;
      default:
      break;
    }
    Set_Current_Event();
  }
}

// Timing Callbacks
static void LedsStripManager_Each10ms(){
  switch(mLedConfig.effect)
  {
    case LED_MODE_RIGHT_RAINBOW:
      Rainbow_Effect_Right(mLedConfig.brightness);
    break;
    case LED_MODE_STATIC:
      Send_Data();
    break;
    case LED_MODE_CLOSE:
      Reset_LED();
    break;
    default:
    break;
  }
}

retcode_t LEDSSTRIPManager_Init(void){
  Callback_RegisterZigbeeOnOff(LedsStripManager_ZigbeeOnOff);
  Callback_RegisterZigbeeLevelControl(LedsStripManager_ZigbeeLevelControl);
  Callback_RegisterEach10ms(LedsStripManager_Each10ms);
  mLedConfig.brightness = 100;
  mLedConfig.brightness_scale = mLedConfig.brightness / 100.0f;
  mLedConfig.effect = LED_MODE_RIGHT_RAINBOW;

  Reset_LED();
  return LEDSSTRIPMAN_SUCCESS;
}

static retcode_t Rainbow_Effect_Right(uint8_t brightness) {
  float factor1, factor2;
  uint16_t ind;
  
  for(uint16_t j=0;j<LEDSSTRIP_MAN_MAX_LEDS;j++) {
    ind = LEDSSTRIP_MAN_MAX_STEP - (int16_t)(mEffStep - j * LEDSSTRIP_MAN_IND_STEP) % LEDSSTRIP_MAN_MAX_STEP;
    switch((int)((ind % LEDSSTRIP_MAN_MAX_STEP) / LEDSSTRIP_MAN_EFFSTEP_BIN)) {
      case 0: factor1 = 1.0 - ((float)(ind % LEDSSTRIP_MAN_MAX_STEP - 0 * LEDSSTRIP_MAN_EFFSTEP_BIN) / LEDSSTRIP_MAN_EFFSTEP_BIN);
              factor2 = (float)((int)(ind - 0) % LEDSSTRIP_MAN_MAX_STEP) / LEDSSTRIP_MAN_EFFSTEP_BIN;
              Set_LED(j, 255 * factor1, 246 * factor2, 0, mLedConfig.brightness_scale);
              break;
      case 1: factor1 = 1.0 - ((float)(ind % LEDSSTRIP_MAN_MAX_STEP - 1 * LEDSSTRIP_MAN_EFFSTEP_BIN) / LEDSSTRIP_MAN_EFFSTEP_BIN);
              factor2 = (float)((int)(ind - LEDSSTRIP_MAN_EFFSTEP_BIN) % LEDSSTRIP_MAN_MAX_STEP) / LEDSSTRIP_MAN_EFFSTEP_BIN;
              Set_LED(j, 0, 246 * factor1, 255 * factor2, mLedConfig.brightness_scale);
              break;
      case 2: factor1 = 1.0 - ((float)(ind % LEDSSTRIP_MAN_MAX_STEP - 2 * LEDSSTRIP_MAN_EFFSTEP_BIN) / LEDSSTRIP_MAN_EFFSTEP_BIN);
              factor2 = (float)((int)(ind - LEDSSTRIP_MAN_EFFSTEP_BIN*2) % LEDSSTRIP_MAN_MAX_STEP) / LEDSSTRIP_MAN_EFFSTEP_BIN;
              Set_LED(j, 255 * factor2, 0, 255 * factor1, mLedConfig.brightness_scale);
              break;
    }
  }
  Send_Data();
  if(mEffStep >= LEDSSTRIP_MAN_MAX_STEP)
  {mEffStep = 0; return LEDSSTRIPMAN_ENDSTEP; }
  else mEffStep++;
  return LEDSSTRIPMAN_SUCCESS;
}

static void Set_LED(int pLEDnum, int pRed, int pGreen, int pBlue, float brightness_scale){
  if (pLEDnum < 0 || pLEDnum >= LEDSSTRIP_MAN_MAX_LEDS)
    return;

  mLEDData[pLEDnum].led.index = pLEDnum;
  mLEDData[pLEDnum].led.color.red = pRed * brightness_scale;
  mLEDData[pLEDnum].led.color.green = pGreen * brightness_scale;
  mLEDData[pLEDnum].led.color.blue = pBlue * brightness_scale;
}

static void Set_LED_ALL(int pRed, int pGreen, int pBlue, float brightness_scale){
  for (int i=0; i<LEDSSTRIP_MAN_MAX_LEDS; i++)
  {
    Set_LED(i, pRed, pGreen, pBlue, brightness_scale);
  }
}

static void Reset_LED (void)
{
  for (int i=0; i<LEDSSTRIP_MAN_MAX_LEDS; i++)
  {
    mLEDData[i].raw = 0;
    mLEDData[i].led.index = i;
  }
  Send_Data();
}

void Send_Data(void)
{
  uint32_t indx = 0;
  uint32_t color;
  for(int i = 0; i<LEDSSTRIP_MAN_MAX_LEDS; i++)
  {
    color = GET_COLOR_FROM_LED(mLEDData[i].led.color.blue, mLEDData[i].led.color.green, mLEDData[i].led.color.red);

    uint32_t mask = 1 << 23;
    for (int j = LEDSSTRIP_MAN_RGB_COLOR_LEN-1; j>=0; j--)
    {
      mPwmData[indx++] = (color & mask) ? PWM_LEDS_HIGH : PWM_LEDS_LOW;
      mask >>= 1;
    }
  }

  for(int i=0; i<LEDSSTRIP_MAN_ZERO_PADDING; i++)
  {
    mPwmData[indx] = 0;
    indx++;
  }
  LedstripDriver_Send(mPwmData, indx);
}

static void Set_Current_Event(void){
  if(mOnOffFlag.onoff){
    if(mOnOffFlag.breath){
      mLedConfig.effect = LED_MODE_FADE_IN;
    } else if(mOnOffFlag.blink){
      mLedConfig.effect = LED_MODE_BLINK;
    } else if(mOnOffFlag.rainbow){
      mLedConfig.effect = LED_MODE_RIGHT_RAINBOW;
    } else {
      mLedConfig.effect = LED_MODE_STATIC;
      Set_LED_ALL(mLedConfig.led_static.red, mLedConfig.led_static.green, mLedConfig.led_static.blue, mLedConfig.brightness_scale);
    }
  } else {
    mLedConfig.effect = LED_MODE_CLOSE;
  }
}

static void Set_OnOff_Value(eZigbeeOnOffType_t type, bool* ret) {
  switch (type)
  {
  case Zb_OnOff_Off:
    *ret = false;
    break;
  case Zb_OnOff_On:
    *ret = true;
    break;
  case Zb_OnOff_Toggle:
    *ret = !(*ret);
    break;
  default:
    break;
  }
}