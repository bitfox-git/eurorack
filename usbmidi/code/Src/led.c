#include "led.h"
#include "tim.h"
#include "dac.h"
#include "stm32f3xx_hal_tim.h"
#include "digital.h"
#include "configuration.h"

bool ledupdate;
uint8_t ledupdatecounter;
Led_t leds[kNUMBER_OF_LEDS];

LED_ModeEnum ledmode;
uint16_t ledtimeout;


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
  if(ledupdate) { 
    switch(ledmode) {
      case LED_Default:
        for (size_t i = 0; i < 4; i++)
        {
          if(dac.floatvalues[i] >= 0.0f) {
            leds[i].g = dac.floatvalues[i]*30 + (digital[i].state*90);
            leds[i].r = 0;
          } else {
            leds[i].g = 0;
            leds[i].r = dac.floatvalues[i]*-30 + (digital[i].state*90);
          }
        }
        break;

      case LED_Preset:
        ledtimeout -= 16;
        if(ledtimeout == 0) {
          ledmode = LED_Default;
        }
        if(configindex < 8) {
          if(configindex < 4) {
            for (size_t i = 0; i < 4; i++)
            { 
              leds[i].r = 0;
              if(configindex%4 == i) {
                leds[i].g = 255*((ledtimeout/7)%2);
              } else {
                leds[i].g = 0;
              }
            }
          } else {
            for (size_t i = 0; i < 4; i++)
            { 
              leds[i].r = 0;
              if(configindex%4 == i) {
                leds[i].g = 0;
              } else {
                leds[i].g = 255*((ledtimeout/7)%2);
              }
            }
          }
        } else {
          if(configindex < 12) {
            for (size_t i = 0; i < 4; i++)
            { 
              leds[i].g = 0;
              if(configindex%4 == i) {
                leds[i].r = 255*((ledtimeout/7)%2);
              } else {
                leds[i].r = 0;
              }
            }
          } else {
            for (size_t i = 0; i < 4; i++)
            { 
              leds[i].g = 0;
              if(configindex%4 == i) {
                leds[i].r = 0;
              } else {
                leds[i].r = 255*((ledtimeout/7)%2);
              }
            }
          }
        }
        break;

      case LED_Connected:
        ledtimeout -= 8;
        if(ledtimeout == 0) {
          ledmode = LED_Default;
        }
        for (size_t i = 0; i < 4; i++) {
          leds[i].r = -ledtimeout;
          leds[i].g = -ledtimeout; 
        }
        break;

      case LED_Disconnected:
        ledtimeout -= 8;
        if(ledtimeout == 0) {
          ledmode = LED_Default;
        }
        for (size_t i = 0; i < 4; i++) {
          leds[i].r = ledtimeout;
          leds[i].g = ledtimeout; 
        }
        break;

      case LED_Uploading:
        break;

      case LED_Fixed:
        break;
  
    }

    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,leds[0].r);
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,leds[0].g);

    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,leds[1].r);
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,leds[1].g);

    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,leds[2].r);
    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,leds[2].g);

    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,leds[3].r);
    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,leds[3].g);

    ledupdate = false;
  }
}




void LED_Color(uint8_t g, uint8_t r) {
  for (size_t i = 0; i < 4; i++)
  {
    leds[i].g = g;
    leds[i].r = r;
  }
}

void LED_Gate(uint8_t channel) {
  leds[channel].gate = 20;
}

uint8_t ledinc = 0;
uint8_t uploadswitch;
void LED_ChangeMode(LED_ModeEnum mode) {
  ledmode = mode;
  if(mode == LED_Uploading) {
    ledinc = 0;
    uploadswitch = 0;
  }
  if(mode == LED_Connected || mode == LED_Disconnected || mode == LED_Preset) {
    ledtimeout = 1024;
  }
}

void LED_SwitchUpload() {
  uploadswitch = 255;
}

void LED_UploadIncrement() {
  for (size_t i = 0; i < 4; i++)
  {
    uint8_t value = uploadswitch;
    if(ledinc == i) {
      value = ~uploadswitch;
    }
    leds[i].g = value;
    leds[i].r = value;
  }

  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,leds[0].r);
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,leds[0].g);

  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,leds[1].r);
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,leds[1].g);

  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,leds[2].r);
  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,leds[2].g);

  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,leds[3].r);
  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,leds[3].g);

  ledinc++;
  ledinc %= 4;
}