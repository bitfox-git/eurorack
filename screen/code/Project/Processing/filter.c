#include "filter.h"
#include "stdint.h"
#include "stdbool.h"



typedef struct {
  float history[FILTER_NUMBER_OF_TAPS];
  uint8_t index;
} Filter_t;

Filter_t filter;


void Filter_Decimate(int16_t *inputbuffer, float *outputbuffer, int n, int decimation, float* taps, float offset) {
  for (uint8_t i = 0; i < FILTER_NUMBER_OF_TAPS; i++) {
    filter.history[i] = 0;
  }
  filter.index = 0;

  float acc;
  for (int i = 0; i < n; i++) {
    float value = (float)inputbuffer[i];
    filter.history[filter.index] = ((1/(-32768.0f))*value) - offset;
    uint8_t index = filter.index ,j; 
    acc = 0;
    for(j = 0; j < FILTER_NUMBER_OF_TAPS; ++j) {
      index = index != 0 ? index-1 : FILTER_NUMBER_OF_TAPS-1;
      acc += filter.history[index] * taps[j];
    };

    if(i%=decimation == 0) {
      outputbuffer[i/decimation] = acc;
    }    
    filter.index++;
    filter.index %= FILTER_NUMBER_OF_TAPS;
  }
}


float Filter_Taps_8[FILTER_NUMBER_OF_TAPS] = {
-0.023498584007711640,
-0.013349966070291007,
0.002602384124422894,
0.023294272392655184,
0.046942153013936172,
0.071244916281039058,
0.093667951914802122,
0.111770057962719646,
0.123526432657048726,
0.127600763462757755,
0.123526432657048726,
0.111770057962719646,
0.093667951914802122,
0.071244916281039058,
0.046942153013936172,
0.023294272392655184,
0.002602384124422894,
-0.013349966070291007,
-0.023498584007711640,
};

float Filter_Taps_3[FILTER_NUMBER_OF_TAPS] = {
0.012185274115133395,
0.036578976430581153,
0.029133209176338368,
-0.012347809006484746,
-0.056665581466771670,
-0.057385628747676665,
0.012445949032027066,
0.134777876436262267,
0.251532543163589473,
0.299490381734002498,
0.251532543163589473,
0.134777876436262267,
0.012445949032027066,
-0.057385628747676665,
-0.056665581466771670,
-0.012347809006484746,
0.029133209176338368,
0.036578976430581153,
0.012185274115133395,
};
