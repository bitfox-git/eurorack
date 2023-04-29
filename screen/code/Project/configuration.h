#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "button.h"
#include "stdint.h"
#include "stdbool.h"
#include "sdadc.h"

typedef enum {
  CONFIG_MODE_PITCH,
  CONFIG_MODE_FREEWAVEFORM,
  CONFIG_MODE_SYNCWAVEFORM,
  CONFIG_MODE_FFT,
  CONFIG_MODE_SLOWSCOPE,
} Config_Mode;

typedef enum {
  CONFIG_CHANNEL_EMPTY = 0b000,
  CONFIG_CHANNEL_ONE = 0b001,
  CONFIG_CHANNEL_TWO = 0b010,
  CONFIG_CHANNEL_THREE = 0b100,
  CONFIG_CHANNEL_DUO_1_2 = 0b011,
  CONFIG_CHANNEL_DUO_1_3 = 0b101,
  CONFIG_CHANNEL_DUO_2_3 = 0b110,
  CONFIG_CHANNEL_TRIO = 0b111,
} Config_Channel;

typedef enum {
  CONFIG_SAMPLE_SLOW,
  CONFIG_SAMPLE_FAST,
} Config_SampleRate;

typedef struct {
  bool active;
  float offset;
  Config_SampleRate samplerate;
  SDADC_HandleTypeDef *hsdadc
} Channel_t;


extern Config_Channel config_active_channel;
extern Channel_t channel[3];

void Config_Init();

void Config_GetCalibration(Config_Channel c);
void Config_SetChannel(Config_Channel new_channel);
void Config_SetChannelSpeed(uint8_t channel_id);

Config_Mode Config_GetMode();

#endif // CONFIGURATION_H_