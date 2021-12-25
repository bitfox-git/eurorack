#include "led.h"
#include "tim.h"
#include "stm32f3xx_hal_tim.h"

void LED_SetRed() {
  for (uint8_t i = 0; i < 4; i++)
  {
    leds[i].r = 255;
    leds[i].g = 0;
  }
  LED_Update();
}
void LED_SetOrange() {
  for (uint8_t i = 0; i < 4; i++)
  {
    leds[i].r = 255;
    leds[i].g = 255;
  }
  LED_Update();
}
void LED_SetGreen() {
  for (uint8_t i = 0; i < 4; i++)
  {
    leds[i].r = 0;
    leds[i].g = 255;
  }
  LED_Update();
}


void LED_Init() {
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
  LED_Update();
}

void LED_Update() {
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,leds[0].r);
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,leds[0].g);

  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,leds[1].r);
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,leds[1].g);

  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,leds[2].r);
  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,leds[2].g);

  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,leds[3].r);
  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,leds[3].g);
}
