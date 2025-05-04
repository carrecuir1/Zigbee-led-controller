#include "main.h"
#include <stdbool.h>

// Drivers
#include "cpudrv.h"
#include "gpiodrv.h"
#include "ledstripdrv.h"

// Peripherals
#include "cpu.h"
#include "ipcc.h"
#include "usart.h"
#include "lpuart.h"
#include "rtc.h"
#include "rng.h"

#include "dbg_trace.h"
#include "hw_conf.h"
#include "otp.h"
#include "stm32_seq.h"

#include "LED_Functions.h"
#include "retcode.h"

typedef struct sFlag_ms_counter{
  uint8_t Each1ms    :1;
  uint8_t Each10ms   :1;
  uint8_t Each25ms   :1;
  uint8_t Each50ms   :1;
  uint8_t Each100ms  :1;
  uint8_t Each250ms  :1;
  uint8_t Each500ms  :1;
  uint8_t Each1000ms :1;
} sFlag_ms_counter_t;

uint32_t mMsCounter = 0;
sFlag_ms_counter_t mFlagMsCounter = {0};

uint8_t brightness = 45;
uint8_t color_R, color_G, color_B = 0;
uint8_t time_Level = 1;

bool rainbow_Bool = false;
bool LED_bool = false;
bool breathe_Bool = false;
bool blink_Bool = false;

static int main_init();
static void timing_callback_routine();

static int main_init()
{
  if(IS_FAILURE(CPUDriver_Open()))
    return -1;

  if(IS_FAILURE(IPCC_Open()))
    return -1;

  if(IS_FAILURE(GPIODriver_Open()))
    return -1;

  if(IS_FAILURE(LedstripDriver_Open()))
    return -1;

  if(IS_FAILURE(LPUART_Open()))
    return -1;

  if(IS_FAILURE(USART_Open()))
    return -1;

  if(IS_FAILURE(RTC_Open()))
    return -1;
    
  if(IS_FAILURE(RNG_Open()))
    return -1;

  DbgOutputInit();
  DbgTraceInit();

  MX_APPE_Init();
}

int main(void)
{

  if(main_init() < 0)
    return -1;

  printf("Starting app\r\n");
  while (1)
  {
    /* USER CODE END WHILE */
    MX_APPE_Process();

    /* USER CODE BEGIN 3 */
    // check which bool is on so that the right function kan be turned on
    //  if(LED_bool)
    //  {
    //  	Set_LED(0, color_R, color_G, color_B);
    //  	Set_LED(1, color_R, color_G, color_B);
    //  	Set_LED(2, color_R, color_G, color_B);
    //  	Set_Brightness(brightness);
    //  	Send_To_LEDS();
    //  }else if(breathe_Bool)
    //  {
    //  	Fade_In_Effect(0, brightness, time_Level, 1, color_R, color_G, color_B);
    //  }else if(blink_Bool)
    //  {
    //  	Blink_Effect(time_Level, brightness, color_R, color_G, color_B);
    //  }else if(rainbow_Bool)
    //  {
    //  	Rainbow_Effect(brightness, time_Level);
    //  }else
    //  	Turn_Off_LEDS();
    Rainbow_Effect(brightness, time_Level);


  }
  /* USER CODE END 3 */
}

static void timing_callback_routine()
{
  if(mFlagMsCounter.Each1ms){
    mFlagMsCounter.Each1ms = 0;
    mCallbackMan.RunEach1ms();
  }

  if(mFlagMsCounter.Each10ms){
    mFlagMsCounter.Each10ms = 0;
    mCallbackMan.RunEach10ms();
  }

  if(mFlagMsCounter.Each25ms){
    mFlagMsCounter.Each25ms = 0;
    mCallbackMan.RunEach25ms();
  }

  if(mFlagMsCounter.Each50ms){
    mFlagMsCounter.Each50ms = 0;
    mCallbackMan.RunEach50ms();
  }

  if(mFlagMsCounter.Each100ms){
    mFlagMsCounter.Each100ms = 0;
    mCallbackMan.RunEach100ms();
  }

  if(mFlagMsCounter.Each250ms){
    mFlagMsCounter.Each250ms = 0;
    mCallbackMan.RunEach250ms();
    mGpioDrv.Toggle(GPIO_LED1);
  }

  if(mFlagMsCounter.Each500ms){
    mFlagMsCounter.Each500ms = 0;
    mCallbackMan.RunEach500ms();
    mGpioDrv.Toggle(GPIO_LED2);
  }

  if(mFlagMsCounter.Each1000ms){
    mFlagMsCounter.Each1000ms = 0;
    mCallbackMan.RunEach1000ms();
    mGpioDrv.Toggle(GPIO_LED3);
  }
}

// Called each time the systick is asserted (1ms)
void HAL_SYSTICK_Callback (void)
{
  mMsCounter++;

  // Each 1 mSeconds
  if ((mMsCounter % 1) == 0)
    mFlagMsCounter.Each1ms = 1;

  // Each 10 mSeconds
  if ((mMsCounter % 10) == 0)
    mFlagMsCounter.Each10ms = 1;

  // Each 25 mSeconds
  if ((mMsCounter % 25) == 0)
    mFlagMsCounter.Each25ms = 1;

  // Each 50 mSeconds
  if ((mMsCounter % 50) == 0)
    mFlagMsCounter.Each50ms = 1;

  // Each 100 mSeconds
  if ((mMsCounter % 100) == 0)
    mFlagMsCounter.Each100ms = 1;

  // Each 250 mSeconds
  if ((mMsCounter % 250) == 0)
    mFlagMsCounter.Each250ms = 1;

  // Each 500 mSeconds
  if ((mMsCounter % 500) == 0)
    mFlagMsCounter.Each500ms = 1;

  // Each 1000 mSeconds
  if ((mMsCounter % 1000) == 0)
    mFlagMsCounter.Each1000ms = 1;
}