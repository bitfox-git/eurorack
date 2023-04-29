#include "acquisition.h"

#include "sdadc.h"
#include "tim.h"
#include "configuration.h"

#define MAXIMUM_SAMPLESIZE 1024

int16_t dmainput[MAXIMUM_SAMPLESIZE];
float audiobuffer[MAXIMUM_SAMPLESIZE];

bool dmaready = false;
Channel_t convertchannel;
uint8_t channelindex;

void Acquisition_Init() {
  // Conversion Timers
  HAL_TIM_PWM_Start(&htim19,TIM_CHANNEL_2); // sdadc
  HAL_TIM_PWM_Start(&htim19,TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim19,TIM_CHANNEL_4);


  // Calibration
  HAL_SDADC_CalibrationStart(&hsdadc1,SDADC_CALIBRATION_SEQ_1);
  HAL_SDADC_PollForCalibEvent(&hsdadc1,HAL_MAX_DELAY);
  HAL_SDADC_CalibrationStart(&hsdadc2,SDADC_CALIBRATION_SEQ_1);
  HAL_SDADC_PollForCalibEvent(&hsdadc2,HAL_MAX_DELAY);
  HAL_SDADC_CalibrationStart(&hsdadc3,SDADC_CALIBRATION_SEQ_1);
  HAL_SDADC_PollForCalibEvent(&hsdadc3,HAL_MAX_DELAY);

  Acquisition_Start();
}


int16_t* Acquisition_DMABuffer() {
  return dmainput;
} 
float* Acquisition_AudioBuffer() {
  return audiobuffer;
}
uint8_t Acquisition_Channel() {
  return channelindex;
}

bool Acquisition_Ready() {
  return dmaready;
}






void nextchannel() {
  if(config_active_channel == CONFIG_CHANNEL_EMPTY) {
    return;
  }
  while(1) {
    channelindex++;
    channelindex %= 3;
    if(channel[channelindex].active) {
      break;
    }
  }
  convertchannel = channel[channelindex];
}


uint8_t Acquisition_Start() {
  uint8_t ret;
  switch (convertchannel.samplerate) {
    case CONFIG_SAMPLE_FAST:
      ret = 1;
      break;
    case CONFIG_SAMPLE_SLOW:
      ret = 4;
      break;
  }

  nextchannel();
  dmaready = false;

  for (size_t i = 0; i < MAXIMUM_SAMPLESIZE; i++) {
    audiobuffer[i] = ((float)(1/(-32768.0f))*(dmainput[i]))-convertchannel.offset;
  }

  switch (convertchannel.samplerate) {
    case CONFIG_SAMPLE_FAST:
      __HAL_RCC_SDADC_CONFIG(RCC_SDADCSYSCLK_DIV12);
      break;
    case CONFIG_SAMPLE_SLOW:
      __HAL_RCC_SDADC_CONFIG(RCC_SDADCSYSCLK_DIV48);
      break;
  }

  HAL_SDADC_InjectedStart_DMA(convertchannel.hsdadc, &dmainput,MAXIMUM_SAMPLESIZE);

  return ret;
}

void HAL_SDADC_InjectedConvCpltCallback(SDADC_HandleTypeDef *hsdadc) {
  dmaready = true;
  HAL_SDADC_InjectedStop_DMA(hsdadc);   
}

void Acquisition_Switch() { // TODO: Could lead to rush conditions with above function ???
  HAL_SDADC_InjectedStop_DMA(convertchannel.hsdadc);  
  nextchannel();
  HAL_SDADC_InjectedStart_DMA(convertchannel.hsdadc, &dmainput,MAXIMUM_SAMPLESIZE);
}
