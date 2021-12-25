#include "button.h"
#include "tim.h"
#include "stm32f3xx_hal_tim.h"
#include "configuration.h"
#include "stdbool.h"

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
  uint8_t debounce;
  Button_State state;
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_Pin;

} Button_t;

Button_t button1 = {0,Button_Off,GPIOE,GPIO_PIN_8};
Button_t button2 = {0,Button_Off,GPIOA,GPIO_PIN_5};
Button_t button3 = {0,Button_Off,GPIOA,GPIO_PIN_3};
Button_t encoder = {0,Button_Off,GPIOE,GPIO_PIN_9};

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

void buttonstate(Button_t *b) {
  if(b->state == Button_Off) {
    if(b->debounce == 0xFF) {
      b->state = Button_Pressed;
      changeactivechannel();
    }
  } else {
    if(b->debounce == 0x00) {
      b->state = Button_Off;
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

// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//   // HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
//   // HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4 | GPIO_PIN_6,GPIO_PIN_RESET);
//   bool but1 = (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8) == GPIO_PIN_RESET);
//   bool but2 = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET);
//   bool but3 = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_RESET);

//   switch (GPIO_Pin)
//   {
//     case BUTTON_1:
//       if(but2 && but3) {
//         Config_SetChannel(CONFIG_CHANNEL_TRIO);
//       } else if(but2) {
//         Config_SetChannel(CONFIG_CHANNEL_DUO_1_2);
//       } else if(but3) {
//         Config_SetChannel(CONFIG_CHANNEL_DUO_1_3);
//       } else {
//         Config_SetChannel(CONFIG_CHANNEL_ONE);
//       }
//       break;
//     case BUTTON_2:
//       if(but1 && but3) {
//         Config_SetChannel(CONFIG_CHANNEL_TRIO);
//       } else if(but1) {
//         Config_SetChannel(CONFIG_CHANNEL_DUO_1_2);
//       } else if(but3) {
//         Config_SetChannel(CONFIG_CHANNEL_DUO_2_3);
//       } else {
//         Config_SetChannel(CONFIG_CHANNEL_TWO);
//       }
//       break;
//     case BUTTON_3:
//       if(but1 && but2) {
//         Config_SetChannel(CONFIG_CHANNEL_TRIO);
//       } else if(but1) {
//         Config_SetChannel(CONFIG_CHANNEL_DUO_1_3);
//       } else if(but2) {
//         Config_SetChannel(CONFIG_CHANNEL_DUO_2_3);
//       } else {
//         Config_SetChannel(CONFIG_CHANNEL_THREE);
//       }
//       break;
      
//     case BUTTON_ENC:

//       break;

//     default:
//       break;
//   }
  // if(GPIO_Pin == BUTTON_1) {
  //   // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,(uint8_t)255);
  // }
  // if(GPIO_Pin == BUTTON_2) {
  //   if(HAL_GPIO_ReadPin(GPIO))
  //   // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,(uint8_t)0);
  //   // HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
  // }
  // if(GPIO_Pin == BUTTON_3) {
  //     // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,(uint8_t)10);
  //     // HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
  // }
  
// }