#include "configuration.h"

#include "stdbool.h"
#include "stdint.h"

#include "tim.h"

#include "ledlut.h"

#define FAST_BLINK_SPEED 801
#define SLOW_BLINK_SPEED 251

uint16_t ledcounter;
uint16_t ledcounter;
uint16_t ledcounter;

void Led_Init() {
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1); // led pwm
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);


  HAL_TIM_PWM_Start_IT(&htim14,TIM_CHANNEL_1);

  Led_ChannelUpdate();
}



void Led_ChannelUpdate() {
  bool led1 = (config_active_channel & 0b001);
  bool led2 = (config_active_channel & 0b010);
  bool led3 = (config_active_channel & 0b100);

  ledcounter++;
  ledcounter %= sizeof(slowledlut);
  if(led1) {
    uint8_t val = (channel[0].samplerate == CONFIG_SAMPLE_SLOW) ? slowledlut[ledcounter] : fastledlut[ledcounter];
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,(uint8_t)50+val);
  } else {
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,(uint8_t)0);
  }

  if(led2) {
    uint8_t val = (channel[1].samplerate == CONFIG_SAMPLE_SLOW) ? slowledlut[ledcounter] : fastledlut[ledcounter];
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,(uint8_t)50+val);
  } else {
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,(uint8_t)0);
  }

  if(led3) {
    uint8_t val = (channel[2].samplerate == CONFIG_SAMPLE_SLOW) ? slowledlut[ledcounter] : fastledlut[ledcounter];
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,(uint8_t)50+val);
  } else {
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,(uint8_t)0);
  }
}