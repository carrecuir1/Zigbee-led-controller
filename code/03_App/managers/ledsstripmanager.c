// #include "ledsstripmanager.h"
// #include "math.h"

// #define LEDSSTRIP_MAN_MAX_LEDS      8
// #define LEDSSTRIP_MAN_RGB_COLOR_LEN 24
// #define LEDSSTRIP_MAN_ZERO_PADDING  50
// #define LEDSSTRIP_MAN_BRIGHTNESS    0
// #define LEDSSTRIP_MAN_MAX_STEP      300
// #define LEDSSTRIP_MAN_NB_LEDS_RAIN  8
// #define LEDSSTRIP_MAN_EFFSTEP_BIN   (LEDSSTRIP_MAN_MAX_STEP/3.0)
// #define LEDSSTRIP_MAN_IND_STEP      LEDSSTRIP_MAN_MAX_STEP/LEDSSTRIP_MAN_NB_LEDS_RAIN
// #define PI                          3.14159265

// uint16_t mEffStep = 0;
// #warning  "THOL : Remove these member variables into something more complex... Structure"
// uint8_t mLEDData[LEDSSTRIP_MAN_MAX_LEDS][4];
// uint8_t mLEDMod[LEDSSTRIP_MAN_MAX_LEDS][4];
// uint16_t pwmData[(LEDSSTRIP_MAN_RGB_COLOR_LEN*LEDSSTRIP_MAN_MAX_LEDS)+LEDSSTRIP_MAN_ZERO_PADDING];

// static retcode_t Rainbow_Effect_Left(void);
// static retcode_t Rainbow_Effect_Right(void);
// static void Set_LED(int pLEDnum, int pRed, int pGreen, int pBlue);
// static void Reset_LED (void);
// static void Set_Brightness(int pBrightness);
// static void Send_Data(void);

// static void LedsStripManager_Each10ms(void);

// // Callbacks
// static void LedsStripManager_Each10ms(){
//   Rainbow_Effect_Right();
// }

// retcode_t LEDSSTRIPManager_Init(void){

//   Callback_RegisterEach10ms(LedsStripManager_Each10ms);

//   Reset_LED();
//   return LEDSSTRIPMAN_SUCCESS;
// }

// // Private Functions
// static retcode_t Rainbow_Effect_Left() {
//   float factor1, factor2;
//   uint16_t ind;
//   for(uint16_t j=0;j<LEDSSTRIP_MAN_MAX_LEDS;j++) {
//     ind = mEffStep + j * 1.625;
//     switch((int)((ind % LEDSSTRIP_MAN_MAX_STEP) / 4.333333333333333)) {
//       case 0: factor1 = 1.0 - ((float)(ind % LEDSSTRIP_MAN_MAX_STEP - 0 * 4.333333333333333) / 4.333333333333333);
//               factor2 = (float)((int)(ind - 0) % LEDSSTRIP_MAN_MAX_STEP) / 4.333333333333333;
//               /************ chnaged here *********/
//               Set_LED(j, 255 * factor1 + 0 * factor2, 0 * factor1 + 246 * factor2, 0 * factor1 + 0 * factor2);
//               break;
//       case 1: factor1 = 1.0 - ((float)(ind % LEDSSTRIP_MAN_MAX_STEP - 1 * 4.333333333333333) / 4.333333333333333);
//               factor2 = (float)((int)(ind - 4.333333333333333) % LEDSSTRIP_MAN_MAX_STEP) / 4.333333333333333;
//               Set_LED(j, 0 * factor1 + 0 * factor2, 246 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2);
//               break;
//       case 2: factor1 = 1.0 - ((float)(ind % LEDSSTRIP_MAN_MAX_STEP - 2 * 4.333333333333333) / 4.333333333333333);
//               factor2 = (float)((int)(ind - 8.666666666666666) % LEDSSTRIP_MAN_MAX_STEP) / 4.333333333333333;
//               Set_LED(j, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2);
//               break;
//     }
//   }
//   Send_Data();
//   if(mEffStep >= LEDSSTRIP_MAN_MAX_STEP) {mEffStep=0; return LEDSSTRIPMAN_ENDSTEP; }
//   else mEffStep++;
//   return LEDSSTRIPMAN_SUCCESS;
// }

// static retcode_t Rainbow_Effect_Right() {
//   float factor1, factor2;
//   uint16_t ind;
//   // Do this for every leds inside the strips
//   for(uint16_t j=0;j<LEDSSTRIP_MAN_MAX_LEDS;j++) {
//     ind = LEDSSTRIP_MAN_MAX_STEP - (int16_t)(mEffStep - j * LEDSSTRIP_MAN_IND_STEP) % LEDSSTRIP_MAN_MAX_STEP;
//     switch((int)((ind % LEDSSTRIP_MAN_MAX_STEP) / LEDSSTRIP_MAN_EFFSTEP_BIN)) {
//       case 0: factor1 = 1.0 - ((float)(ind % LEDSSTRIP_MAN_MAX_STEP - 0 * LEDSSTRIP_MAN_EFFSTEP_BIN) / LEDSSTRIP_MAN_EFFSTEP_BIN);
//               factor2 = (float)((int)(ind - 0) % LEDSSTRIP_MAN_MAX_STEP) / LEDSSTRIP_MAN_EFFSTEP_BIN;
//               Set_LED(j, 255 * factor1 + 0 * factor2, 0 * factor1 + 246 * factor2, 0 * factor1 + 0 * factor2);
//               break;
//       case 1: factor1 = 1.0 - ((float)(ind % LEDSSTRIP_MAN_MAX_STEP - 1 * LEDSSTRIP_MAN_EFFSTEP_BIN) / LEDSSTRIP_MAN_EFFSTEP_BIN);
//               factor2 = (float)((int)(ind - LEDSSTRIP_MAN_EFFSTEP_BIN) % LEDSSTRIP_MAN_MAX_STEP) / LEDSSTRIP_MAN_EFFSTEP_BIN;
//               Set_LED(j, 0 * factor1 + 0 * factor2, 246 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2);
//               break;
//       case 2: factor1 = 1.0 - ((float)(ind % LEDSSTRIP_MAN_MAX_STEP - 2 * LEDSSTRIP_MAN_EFFSTEP_BIN) / LEDSSTRIP_MAN_EFFSTEP_BIN);
//               factor2 = (float)((int)(ind - LEDSSTRIP_MAN_EFFSTEP_BIN*2) % LEDSSTRIP_MAN_MAX_STEP) / LEDSSTRIP_MAN_EFFSTEP_BIN;
//               Set_LED(j, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2);
//               break;
//     }
//   }
//   Send_Data();
//   if(mEffStep >= LEDSSTRIP_MAN_MAX_STEP)
//   {mEffStep = 0; return LEDSSTRIPMAN_ENDSTEP; }
//   else mEffStep++;
//   return LEDSSTRIPMAN_SUCCESS;
// }

// static void Set_LED(int pLEDnum, int pRed, int pGreen, int pBlue){
//   if(0<= pLEDnum && pLEDnum < LEDSSTRIP_MAN_MAX_LEDS){
//     mLEDData[pLEDnum][0] = pLEDnum;
//     mLEDData[pLEDnum][1] = pRed;
//     mLEDData[pLEDnum][2] = pGreen;
//     mLEDData[pLEDnum][3] = pBlue;
//   }
// }

// static void Reset_LED (void)
// {
//   for (int i=0; i<LEDSSTRIP_MAN_MAX_LEDS; i++)
//   {
//     mLEDData[i][0] = i;
//     mLEDData[i][1] = 0;
//     mLEDData[i][2] = 0;
//     mLEDData[i][3] = 0;
//   }
//   Send_Data();
// }

// static void Set_Brightness(int pBrightness)
// {
// #if USE_BRIGHTNESS
//   if(pBrightness>LEDSSTRIP_MAN_BRIGHTNESS) pBrightness = LEDSSTRIP_MAN_BRIGHTNESS;
//   for(int i=0; i<MAX_LED; i++)
//   {
//     mLEDMod[i][0] = mLEDData[i][0];
//     for (int j=1; j<4; j++)
//     {
//       float angle = 90-pBrightness;
//       angle = angle*PI/180;
//       mLEDMod[i][j] = (mLEDData[i][j])/tan(angle);
//     }
//   }
// #endif
// }

// void Send_Data(void)
// {
//   uint32_t indx = 0;
//   uint32_t color;
//   for(int i = 0; i<LEDSSTRIP_MAN_MAX_LEDS; i++)
//   {
//   #if USE_BRIGHTNESS
//     color = ((mLEDMod[i][1]<<16) | (mLEDMod[i][2]<<8) | (mLEDMod[i][3]));
//   #else
//     color = ((mLEDData[i][1]<<16) | (mLEDData[i][2]<<8) | (mLEDData[i][3]));
//   #endif

//     for (int j = LEDSSTRIP_MAN_RGB_COLOR_LEN-1; j>=0; j--)
//     {
//       if(color&(1<<j))  pwmData[indx] = PWM_LEDS_HIGH;
//       else pwmData[indx] = PWM_LEDS_LOW;
//       indx++;
//     }
//   }

//   for(int i=0; i<LEDSSTRIP_MAN_ZERO_PADDING; i++)
//   {
//     pwmData[indx] = 0;
//     indx++;
//   }
//   LedstripDriver_Send(pwmData, indx);
// }
