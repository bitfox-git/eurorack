#ifndef MIDIMESSAGE_H_
#define MIDIMESSAGE_H_

#include "stdint.h"

typedef struct {
  uint8_t cablenum;
  uint8_t messagetype;
  uint8_t byte1;
  uint8_t midichannel;
  uint8_t byte2;
  uint8_t byte3;
} MidiMessage;

#endif // MIDIMESSAGE_H_