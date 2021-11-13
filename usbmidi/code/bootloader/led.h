#ifndef LEDS_H_
#define LEDS_H_

#include "stdint.h"
#include "stdbool.h"

#define kNUMBER_OF_LEDS 4

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t gate;
} Led_t;

bool ledupdate;
Led_t leds[kNUMBER_OF_LEDS];

void LED_Update();
void LED_Init();

void LED_SetRed();
void LED_SetOrange();
void LED_SetGreen();

#endif // LEDS_H_