#include "callbackmanager.h"

static sTimingCallback_t mEach1msCallbacks    = {0};       // Callback each 1ms
static sTimingCallback_t mEach10msCallbacks   = {0};       // Callback each 10ms
static sTimingCallback_t mEach25msCallbacks   = {0};       // Callback each 25ms
static sTimingCallback_t mEach50msCallbacks   = {0};       // Callback each 50ms
static sTimingCallback_t mEach100msCallbacks  = {0};       // Callback each 100ms
static sTimingCallback_t mEach250msCallbacks  = {0};       // Callback each 250ms
static sTimingCallback_t mEach500msCallbacks  = {0};       // Callback each 500ms
static sTimingCallback_t mEach1000msCallbacks = {0};       // Callback each 1000ms

void Callback_RegisterEach1ms(void_callback_t callback){
  if(mEach1msCallbacks.nbr == CALLBACK_MAX)
    return;
  mEach1msCallbacks.funcArr[mEach1msCallbacks.nbr] = callback;
  mEach1msCallbacks.nbr++;
}

void Callback_RunEach1ms(void){
  for(uint8_t i = 0; i < mEach1msCallbacks.nbr;i++)
    mEach1msCallbacks.funcArr[i]();
}

void Callback_RegisterEach10ms(void_callback_t callback){
  if(mEach10msCallbacks.nbr == CALLBACK_MAX)
    return;
  mEach10msCallbacks.funcArr[mEach10msCallbacks.nbr] = callback;
  mEach10msCallbacks.nbr++;
}

void Callback_RunEach10ms(void){
  for(uint8_t i = 0; i < mEach10msCallbacks.nbr;i++)
    mEach10msCallbacks.funcArr[i]();
}

void Callback_RegisterEach25ms(void_callback_t callback){
  if(mEach25msCallbacks.nbr == CALLBACK_MAX)
    return;
  mEach25msCallbacks.funcArr[mEach25msCallbacks.nbr] = callback;
  mEach25msCallbacks.nbr++;
}

void Callback_RunEach25ms(void){
  for(uint8_t i = 0; i < mEach25msCallbacks.nbr;i++)
    mEach25msCallbacks.funcArr[i]();
}

void Callback_RegisterEach50ms(void_callback_t callback){
  if(mEach50msCallbacks.nbr == CALLBACK_MAX)
    return;
  mEach50msCallbacks.funcArr[mEach50msCallbacks.nbr] = callback;
  mEach50msCallbacks.nbr++;
}

void Callback_RunEach50ms(void){
  for(uint8_t i = 0; i < mEach50msCallbacks.nbr;i++)
    mEach50msCallbacks.funcArr[i]();
}

void Callback_RegisterEach100ms(void_callback_t callback){
  if(mEach100msCallbacks.nbr == CALLBACK_MAX)
    return;
  mEach100msCallbacks.funcArr[mEach100msCallbacks.nbr] = callback;
  mEach100msCallbacks.nbr++;
}

void Callback_RunEach100ms(void){
  for(uint8_t i = 0; i < mEach100msCallbacks.nbr;i++)
    mEach100msCallbacks.funcArr[i]();
}

void Callback_RegisterEach250ms(void_callback_t callback){
  if(mEach250msCallbacks.nbr == CALLBACK_MAX)
    return;
  mEach250msCallbacks.funcArr[mEach250msCallbacks.nbr] = callback;
  mEach250msCallbacks.nbr++;
}

void Callback_RunEach250ms(void){
  for(uint8_t i = 0; i < mEach250msCallbacks.nbr;i++)
    mEach250msCallbacks.funcArr[i]();
}

void Callback_RegisterEach500ms(void_callback_t callback){
  if(mEach500msCallbacks.nbr == CALLBACK_MAX)
    return;
  mEach500msCallbacks.funcArr[mEach500msCallbacks.nbr] = callback;
  mEach500msCallbacks.nbr++;
}

void Callback_RunEach500ms(void){
  for(uint8_t i = 0; i < mEach500msCallbacks.nbr;i++)
    mEach500msCallbacks.funcArr[i]();
}

void Callback_RegisterEach1000ms(void_callback_t callback){
  if(mEach1000msCallbacks.nbr == CALLBACK_MAX)
    return;
  mEach1000msCallbacks.funcArr[mEach1000msCallbacks.nbr] = callback;
  mEach1000msCallbacks.nbr++;
}

void Callback_RunEach1000ms(void){
  for(uint8_t i = 0; i < mEach1000msCallbacks.nbr;i++)
    mEach1000msCallbacks.funcArr[i]();
}
