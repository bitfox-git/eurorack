/* ---------------------------------
// DAC8565



--------------------------------- */


#ifndef DAC_H_
#define DAC_H_

#include <stdint.h>
#include "stdbool.h"

#define kANALOGOUTPUTS 4

#define kDAC_CALIB_LOW 0x5FBE // Should be Three Volt
#define kDAC_CALIB_HIGH 0x970A // Should be Zero Volt


typedef struct {
  uint16_t values[kANALOGOUTPUTS];

  float floatvalues[kANALOGOUTPUTS];

  // Calibration
  uint8_t status;
  float voltlow[kANALOGOUTPUTS];
  float volthigh[kANALOGOUTPUTS];
  float calibscale[kANALOGOUTPUTS];
  float caliboffset[kANALOGOUTPUTS];

  //SPI OutputBuffer
  uint8_t current;

} DAC_t; 

extern DAC_t dac;

void DAC_Init();
void DAC_InsertValue(const float, const uint8_t);
uint16_t DAC_GetValue(const float value, const uint8_t channel);
void DAC_Sendvalue(const uint16_t word, const uint8_t channel);
void DAC_Send();

void DAC_Calibrate();

void DAC_Refresh();

#endif // DAC_H_