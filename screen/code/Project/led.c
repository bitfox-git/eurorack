#include "configuration.h"

#include "stdbool.h"
#include "stdint.h"

#include "tim.h"

void Led_Update() {
  bool led1 = (active_channel & 0b001);
  bool led2 = (active_channel & 0b010);
  bool led3 = (active_channel & 0b100);

  if(led1) {
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,(uint8_t)255);
  } else {
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,(uint8_t)0);
  }

  if(led2) {
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,(uint8_t)255);
  } else {
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,(uint8_t)0);
  }

  if(led3) {
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,(uint8_t)255);
  } else {
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,(uint8_t)0);
  }



}