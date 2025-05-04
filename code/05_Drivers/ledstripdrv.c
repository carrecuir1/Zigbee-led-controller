#include "ledstripdrv.h"


retcode_t LedstripDriver_Open(void){
  retcode_t ret = LEDSTRIP_ERROR_INIT; // Assuming error
  if(IS_SUCCESS(PWM_Open(PWM_TYPE_LEDS))) ret = LEDSTRIP_SUCCESS;
  return ret;
}

retcode_t LedstripDriver_Send(uint16_t* pData, uint32_t pLength){
  retcode_t ret = LEDSTRIP_ERROR_SENDING; // Assuming error
  switch(PWM_Send(PWM_TYPE_LEDS, (uint8_t*)pData, pLength))
  {
    case PWM_SUCCESS: ret = LEDSTRIP_SUCCESS; break;
    case PWM_NOT_READY: ret = LEDSTRIP_NOT_READY; break;
    default: ret = LEDSTRIP_NOT_READY; break;
  }
  return ret;
}
