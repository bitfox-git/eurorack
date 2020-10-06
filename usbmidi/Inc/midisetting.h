#ifndef MIDISETTING_H_
#define MIDISETTING_H_

#include "stdint.h"

typedef struct {
  uint8_t midichannel;
  uint8_t byte2;
  uint8_t byte3;
} MidiSetting;

#endif // MIDISETTING_H_