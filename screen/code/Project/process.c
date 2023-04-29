#include "configuration.h"
#include "process.h"
#include "display.h"
#include "acquisition.h"
#include "yin.h"

Process_t data;

void process_yin() {
  float* input = Acquisition_AudioBuffer();
  data.buffer = input;
  data.channel = Acquisition_Channel();
  uint8_t decimate = Acquisition_Start();
  data.pitch = Yin_getPitch(input,1024,0.15,false,decimate);
}



void Process() {
  if(Acquisition_Ready()) {
    switch (Config_GetMode())
    {
      case CONFIG_MODE_PITCH:
        process_yin();
        break;
      
      default:
        break;
    }
    Display_Update(&data);
  }
}
