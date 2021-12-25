#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "button.h"

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

extern Config_Channel active_channel;

void Config_SetChannel(Config_Channel new_channel);


#endif // CONFIGURATION_H_