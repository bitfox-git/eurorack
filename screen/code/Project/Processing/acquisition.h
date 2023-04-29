#ifndef ACQUISITION_H_
#define ACQUISITION_H_

#include "stdint.h"
#include "stdbool.h"

void Acquisition_Init();
uint8_t Acquisition_Start();

void Acquisition_Switch();

int16_t* Acquisition_DMABuffer();
float* Acquisition_AudioBuffer();
uint8_t Acquisition_Channel();
bool Acquisition_Ready();

#endif // ACQUISITION_H_