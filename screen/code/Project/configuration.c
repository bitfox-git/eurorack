#include "configuration.h"

Config_Channel active_channel;

void Config_SetChannel(Config_Channel new_channel) {
  active_channel = new_channel;
}