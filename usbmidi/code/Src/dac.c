#include "dac.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_spi.h"
#include "spi.h"
#include "configuration.h"

typedef enum CalibrationStatus{Init=0,Uncalibrated, Calibrated}; // Maybe not needed ?

DAC_t dac;

const uint8_t channellookup[4] = {
  0b00010000,
  0b00010010,
  0b00010100,
  0b00010110,
};

void DAC_Init() {
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
  dac.status = Init;
  if(Config_ReadCalibrate() == false) {
    dac.status = Uncalibrated;
  } else {
    dac.status = Calibrated;
  }

  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);
  DAC_Calibrate();
  __HAL_SPI_ENABLE(&hspi2);
  DAC_InsertValue(0,0);
  DAC_InsertValue(0,1);
  DAC_InsertValue(0,2);
  DAC_InsertValue(0,3);
  dac.current = 0;
}

void DAC_Calibrate() {
  uint16_t totalsteps = kDAC_CALIB_HIGH - kDAC_CALIB_LOW; 
  for (uint8_t i = 0; i < kANALOGOUTPUTS; i++) {
    float tempscale = totalsteps / (dac.voltlow[i] - dac.volthigh[i]);
    float tempoffset = (kDAC_CALIB_HIGH / tempscale) + dac.volthigh[i];
    dac.caliboffset[i] = tempoffset;
    dac.calibscale[i] = tempscale;
  }
}

void DAC_InsertValue(const float value, const uint8_t channel) {
  dac.floatvalues[channel] = value;
  int compare = (int)((dac.caliboffset[channel] - value) * dac.calibscale[channel]);
  if(compare < 0) {
    compare = 0;
  } else if (compare > 65535) {
    compare = 65535;
  }
  dac.values[channel] = compare;
}

uint16_t DAC_GetValue(const float value, const uint8_t channel) {
  dac.floatvalues[channel] = value;
  int compare = (int)((dac.caliboffset[channel] - value) * dac.calibscale[channel]);
  if(compare < 0) {
    compare = 0;
  } else if (compare > 65535) {
    compare = 65535;
  }
  return (uint16_t)compare;
}

void DAC_Sendvalue(const uint16_t word, const uint8_t channel) {
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
  uint16_t part1 = channellookup[channel] << 8 | word >> 8;
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
  hspi2.Instance->DR = part1;
  hspi2.Instance->DR = word << 8;
}

void DAC_Send() {
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
  uint16_t word = dac.values[dac.current];
  uint16_t part1 = channellookup[dac.current] << 8 | word >> 8;
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
  hspi2.Instance->DR = part1;
  hspi2.Instance->DR = word << 8;
}

void DAC_Refresh() {
  DAC_Calibrate();
  for (size_t i = 0; i < 4; i++) {
    DAC_InsertValue(dac.floatvalues[i],i);
  }
}

