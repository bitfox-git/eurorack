#include "button.h"
#include "tim.h"
#include "stm32f3xx_hal_tim.h"
#include "configuration.h"
#include "stdbool.h"
#include "led.h"

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim14) {
    Button_Scanner();
    Led_ChannelUpdate();
  }  
}


typedef enum {
  BUTTON_1 = GPIO_PIN_8,
  BUTTON_2 = GPIO_PIN_5,
  BUTTON_3 = GPIO_PIN_3, 
  BUTTON_ENC = GPIO_PIN_9,
} Button_Pins;

typedef enum {
  Button_Off = 0,
  Button_Pressed,
  // Button_Released // Maybe not needed
} Button_State;



typedef struct {
  uint8_t id;
  uint8_t debounce;
  Button_State state;
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_Pin;
  uint8_t longpress;
} Button_t;

Button_t button1 = {0,0,Button_Off,GPIOE,GPIO_PIN_8};
Button_t button2 = {1,0,Button_Off,GPIOA,GPIO_PIN_5};
Button_t button3 = {2,0,Button_Off,GPIOA,GPIO_PIN_3};
Button_t encoder = {9,0,Button_Off,GPIOE,GPIO_PIN_9};

void debounce(Button_t *b) {
  b->debounce = b->debounce << 1;
  if(!(b->GPIOx->IDR & b->GPIO_Pin)) {
    b->debounce += 1;
  }
}

void changeactivechannel() {
  uint8_t buttons = 0;
  if(button1.state == Button_Pressed) {
    buttons += 0b001;
  }
  if(button2.state == Button_Pressed) {
    buttons += 0b010;
  }
  if(button3.state == Button_Pressed) {
    buttons += 0b100;
  }
  Config_SetChannel((Config_Channel)buttons);
}

void resetlongpress() {
  button1.longpress = 0;
  button2.longpress = 0;
  button3.longpress = 0;
}

void buttonstate(Button_t *b) {
  if(b->state == Button_Off) {
    if(b->debounce == 0xFF) {
      b->state = Button_Pressed;
      resetlongpress();
      changeactivechannel();
    }
  } else {
    if(b->debounce == 0x00) {
      b->state = Button_Off;
    }

    if(b->longpress < 200) {
      b->longpress++;
    } else if (b->longpress == 200) {
      b->longpress++;
      Config_SetChannelSpeed(b->id);
    }
  }
}

void encoderstate(Button_t *b) {
  if(b->state == Button_Off) {
    if(b->debounce == 0xFF) {
      b->state = Button_Pressed;

      // TODO: Encoder pressed, menu.h change
    }
  } else {
    if(b->debounce == 0x00) {
      b->state = Button_Off;
    }
  }
}



void Button_Scanner() {
  debounce(&button1);
  debounce(&button2);
  debounce(&button3);
  debounce(&encoder);

  buttonstate(&button1);
  buttonstate(&button2);
  buttonstate(&button3);
  encoderstate(&encoder);
}