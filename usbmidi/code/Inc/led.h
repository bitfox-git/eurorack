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

typedef enum  {
  LED_Default,
  LED_Preset,
  LED_Connected,
  LED_Disconnected,
  LED_Uploading,
  LED_Fixed
} LED_ModeEnum;

bool ledupdate;
uint8_t ledupdatecounter;
Led_t leds[kNUMBER_OF_LEDS];

LED_ModeEnum ledmode;
uint16_t ledtimeout;


void LED_Gate(uint8_t channel);
void LED_CV(uint8_t channel);
void LED_Update();
void LED_Init();

void LED_ChangeMode(LED_ModeEnum mode);

void LED_UploadIncrement();
void LED_SwitchUpload();

void LED_Color(uint8_t g, uint8_t r);

#endif // LEDS_H_