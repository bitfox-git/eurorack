#include "configuration.h"
#include "sdadc.h"
#include "acquisition.h"

Config_Mode mode;
Config_Channel config_active_channel = 1;
Channel_t channel[3];

Config_Mode Config_GetMode() {
  return mode;
}


void Config_Init() {
  for (uint8_t i = 0; i < 3; i++)
  {
    channel[i].samplerate = CONFIG_SAMPLE_SLOW;
    channel[i].offset = 0.0f;
  }
  channel[0].hsdadc = &hsdadc1;
  channel[1].hsdadc = &hsdadc2;
  channel[2].hsdadc = &hsdadc3;
  Config_SetChannel(CONFIG_CHANNEL_ONE);
}


void Config_SetChannel(Config_Channel new_channel) {
  config_active_channel = new_channel;
  channel[0].active = config_active_channel & CONFIG_CHANNEL_ONE;
  channel[1].active = config_active_channel & CONFIG_CHANNEL_TWO;
  channel[2].active = config_active_channel & CONFIG_CHANNEL_THREE;

  Acquisition_Switch();
}

void Config_SetChannelSpeed(uint8_t id) {
  channel[id].samplerate = (channel[id].samplerate == CONFIG_SAMPLE_SLOW) ? CONFIG_SAMPLE_FAST : CONFIG_SAMPLE_SLOW;
}