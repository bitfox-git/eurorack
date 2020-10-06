#include "digital.h"
#include "gpio.h"
#include "analog.h"


void Digital_Init() {
  digital[0].gpiopin = GPIO_PIN_3;
  digital[0].id = 0;
  digital[1].gpiopin = GPIO_PIN_5;
  digital[1].id = 1;
  digital[2].gpiopin = GPIO_PIN_2;
  digital[2].id = 2;
  digital[3].gpiopin = GPIO_PIN_1;
  digital[3].id = 3;
}

void Digital_Reset(Digital_t *dig) {
  dig->trigger = 0;
  dig->voice = false;
  dig->random = false;
  dig->pedal = 0;
  dig->state = false;
  Digital_ChangeValue(false,false,dig);

  switch (dig->mode) {
    case DIGITAL_VOICE_GATE:
    case DIGITAL_VOICE_TRIGGER:
      dig->voice = true;
      break;
    case DIGITAL_RANDOM:
      dig->random = true;

    default:
      break;
  }
}

void Digital_Configure(Digital_t *dig, Digital_Settings mode, MidiSetting *midiset){
  dig->mode = mode;
  dig->settings.byte2 = midiset->byte2;
  dig->settings.byte3 = midiset->byte3;
  dig->settings.midichannel = midiset->midichannel;

  Digital_Reset(dig);
}

void Digital_NoteOn(MidiMessage *message, Digital_t *dig) {
  switch (dig->mode) {
    case DIGITAL_TRIGGER:
      if(message->midichannel == dig->settings.midichannel || dig->settings.midichannel == 255) {
        if(dig->settings.byte2 == message->byte2 || dig->settings.byte2 == 255) {
          Digital_ChangeValue(true,true,dig);
        }
      }
      break;
    
    case DIGITAL_GATE:
      if(message->midichannel == dig->settings.midichannel || dig->settings.midichannel == 255) {
        if(dig->settings.byte2 == message->byte2 || dig->settings.byte2 == 255) {
          Digital_ChangeValuePedal(true,false,dig);
        }
      }
      break;
      
    default:
      break;
  }
}

void Digital_NoteOff(MidiMessage *message, Digital_t *dig) {
  switch (dig->mode) {    
    case DIGITAL_GATE:
      if(message->midichannel == dig->settings.midichannel || dig->settings.midichannel == 255) {
        if(dig->settings.byte2 == message->byte2 || dig->settings.byte2 == 255) {
          Digital_ChangeValuePedal(false,false,dig);
        }
      }
    break;

    default:
      break;
  }
}

void Digital_ChangeValue(bool state, bool trigger, Digital_t *dig) {
  if(trigger) {
    dig->trigger = (kSAMPLE_RATE/1000);
  } else {
    dig->trigger = 0;
  }
  dig->state = state;
  HAL_GPIO_WritePin(GPIOA,dig->gpiopin,state);
}

void Digital_ChangeValuePedal(bool state, bool trigger, Digital_t *dig) {
  if(trigger) {
    dig->trigger = (kSAMPLE_RATE/1000);
  } else {
    dig->trigger = 0;
  }
  if(dig->state == true && state == false) {
    dig->state = state;
    if(dig->pedal == false){
      HAL_GPIO_WritePin(GPIOA,dig->gpiopin,state);
    }
  } else{
    dig->state = state;
    HAL_GPIO_WritePin(GPIOA,dig->gpiopin,state);
  }
}

void Digital_TimerUpdate(Digital_t *dig) {
  if(dig->trigger) {
    dig->trigger--;
    if(dig->trigger == 0) {
      dig->state = false;
      Digital_ChangeValue(false,false,dig);
    }
  }
  if(dig->random) {
    if(dig->randomcounter == 0) {
      Digital_Random(dig);
    }
    dig->randomcounter++;
    dig->randomcounter %= (kBLOCK_SIZE);
  }
}

void Digital_Random(Digital_t *dig) {
  if(dig->mode == DIGITAL_RANDOM) {
    if(customrand() < randomthreshold[dig->id]) {
      Digital_ChangeValue(true,true,dig);
    }
  }
}   

void Digital_Clock(MidiMessage *message, Digital_t *dig) {
  switch (dig->mode)
  {
    case DIGITAL_CLOCK:
      if(message->byte1 == 0xF8) { // Midi message Clock
        if(dig->counter == 0) {
          Digital_ChangeValue(true,true,dig);
        }
        dig->counter++;
        dig->counter %= dig->settings.byte2;
      } else if ( message->byte1 == 0xFA) {
        dig->counter = 0;
      }
      break;

    case DIGITAL_CLOCK_RESET:
      if(message->byte1 == 0xFA) { // Midi Message Running
        Digital_ChangeValue(true,true,dig);
      }
      break;
    case DIGITAL_CLOCK_RUN:
      if(message->byte1 == 0xFA) { // Midi Message Running
        Digital_ChangeValue(true,false,dig);
      }
      else if(message->byte1 == 0xFC) { // Midi Message Running
        Digital_ChangeValue(false,false,dig);
      }
      break;
      
    default:
      break;
  }
}

void Digital_Voice(bool state, Digital_t *dig) {
  switch (dig->mode)
  {
    case DIGITAL_VOICE_TRIGGER:
      if(state){ 
        Digital_ChangeValue(true,true,dig);
      }
      break;
    case DIGITAL_VOICE_GATE:
      Digital_ChangeValuePedal(state,false,dig);
      break;
    
    default:
      break;
  }
}

void Digital_Pedal(MidiMessage *message, Digital_t *dig) {
  if(message->midichannel == dig->settings.midichannel || dig->settings.midichannel == 255) {
    dig->pedal = message->byte3;
    switch(dig->mode) {
      case DIGITAL_VOICE_GATE:
      case DIGITAL_GATE:
        if(dig->pedal == 0) {
          HAL_GPIO_WritePin(GPIOA,dig->gpiopin,dig->state);
        }
        break;
              
      default:
        break;
    }
  }
}