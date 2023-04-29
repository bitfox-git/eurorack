#ifndef FILTER_H_
#define FILTER_H_

#include "stdint.h"

void Filter_Decimate(int16_t *inputbuffer, float *outputbuffer, int n, int decimation, float* taps, float offset);

#define FILTER_NUMBER_OF_TAPS 19

extern float Filter_Taps_8[FILTER_NUMBER_OF_TAPS];

extern float Filter_Taps_3[FILTER_NUMBER_OF_TAPS];


#endif // FILTER_H_