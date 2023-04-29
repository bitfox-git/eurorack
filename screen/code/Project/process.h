#ifndef PROCESS_H_
#define PROCESS_H_

#include "stdint.h"
#include "configuration.h"

typedef struct {
  float pitch;
  float *buffer;
  uint8_t channel;
} Process_t;

void Process();

#endif // PROCESS_H_