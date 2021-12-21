#include "analog.h"
#include "digital.h"
#include "gpio.h"
#include "dac.h"
#include "poly.h"
#include "resources.h"
#include "math.h"

#define kENVELOPE_EXPO_SIZE (int)(sizeof(envelopeexpo)/sizeof(float)-1)


Analog_t analog[kANALOG_AMOUNT];
uint32_t randomthreshold[4] = {
  0xFFFFFFFF/140,
  0xFFFFFFFF/650,
  0xFFFFFFFF/2700,
  0xFFFFFFFF/8600,
};


// RANDOM UINT32 GENERATOR

uint32_t state = 0;
uint32_t customrand() {
  state = state * 1664525L + 1013904223L;
  return state;
}



void Analog_Init() {
  for (size_t i = 0; i < 4; i++) {
    analog[i].id = i;
    analog[i].phase = 1;
    Analog_Reset(&analog[i]);
  }
}

void Analog_Reset(Analog_t *ana) {
  Poly_Init();
  NoteStack_Clear(&ana->stack);
  ana->pedal = 0;
  ana->digitalstate = false;

  ana->oscnote = 0x45;
  if(digital[ana->id].voice == true) {
    ana->digitalvoice = true;
  } else {
    ana->digitalvoice = false;
  }
  if(ana->mode == ANALOG_CONSTANT) {
    Analog_ChangeNote(ana->settings.byte2,true,ana);
  }

  // else if(ana->mode == ANALOG_OFF) {
  //   DAC_InsertValue(0.0f,ana->id);
  // } 
  else {
    DAC_InsertValue(0.0f,ana->id);
  } 

  ana->trigger = 0;

  ana->oscillator = false;
  ana->envelope = false;
  ana->random = false;

  switch (ana->mode) {
    case ANALOG_OSC_NOISE:
    case ANALOG_MONO_OSC_SAW:
    case ANALOG_MONO_OSC_PULSE:
    case ANALOG_MONO_OSC_TRI:
    case ANALOG_MONO_OSC_SINE:
    case ANALOG_POLY_OSC_PULSE_DUO:
    case ANALOG_POLY_OSC_SAW_DUO:
    case ANALOG_POLY_OSC_TRI_DUO:
    case ANALOG_POLY_OSC_SINE_DUO:
    case ANALOG_POLY_OSC_PULSE_QUAD:
    case ANALOG_POLY_OSC_SAW_QUAD:
    case ANALOG_POLY_OSC_TRI_QUAD:
    case ANALOG_POLY_OSC_SINE_QUAD:
      ana->phase = 0;
      ana->osccounter = 0;
      ana->oscillator = true;
      break;
    
    case ANALOG_ENVELOPE_AD_EXPO:
    case ANALOG_ENVELOPE_AD_LINEAR:
    case ANALOG_ENVELOPE_AD_LOG:
    case ANALOG_ENVELOPE_ASR_EXPO:
    case ANALOG_ENVELOPE_ASR_LINEAR:
    case ANALOG_ENVELOPE_ASR_LOG:
    case ANALOG_ENVELOPE_VELOCITY_DECAY_EXPO:
    case ANALOG_ENVELOPE_VELOCITY_DECAY_LINEAR:
    case ANALOG_ENVELOPE_VELOCITY_DECAY_LOG:
      ana->envelope = true;
      ana->envelopeout = 0;
      ana->osccounter = 0;
      ana->oscnote = 0xFF;

    case ANALOG_DIGITAL_RANDOM:
    case ANALOG_RANDOM:
      ana->random = true;
      ana->osccounter = 0;

    default:
      break;
  }
}

void Analog_Configure(Analog_t *ana, Analog_Settings mode, MidiSetting *midiset) {
  ana->mode = mode;
  ana->settings.byte2 = midiset->byte2;
  ana->settings.byte3 = midiset->byte3;
  ana->settings.midichannel = midiset->midichannel;

  Analog_Reset(ana);
}

void Analog_DigitalValue(bool state, bool trigger, Analog_t *ana) {
  if(trigger) {
    ana->trigger = (kSAMPLE_RATE/1000);
  } else {
    ana->trigger = 0;
  }
  if(state) {
    DAC_InsertValue(5.0f,ana->id);
  } else {
    DAC_InsertValue(0.0f,ana->id);
  }
}

void Analog_DigitalValuePedal(bool state, bool trigger, Analog_t *ana) {
  if(trigger) {
    ana->trigger = (kSAMPLE_RATE/1000);
  } else {
    ana->trigger = 0;
  }

  if(ana->digitalstate == true && state == false) {
    ana->digitalstate = state;
    if(ana->pedal == false){
      if(state) {
        DAC_InsertValue(5.0f,ana->id);
      } else {
        DAC_InsertValue(0.0f,ana->id);
      }
    }
  } else{
    ana->digitalstate = state;
    if(state) {
      DAC_InsertValue(5.0f,ana->id);
    } else {
      DAC_InsertValue(0.0f,ana->id);
    }
  }
}

void Analog_DigitalVoice(bool state, Analog_t *ana) {
  if(ana->digitalvoice) {
    Digital_Voice(state, &digital[ana->id]);
  }
}

void Analog_NoteToDac(Analog_t *ana) {
  float value = (ana->oscnote-60) / 12.0f;
  value += (ana->pitchbend * ana->settings.byte2) / (12*8192.0f);
  DAC_InsertValue(value, ana->id);
}


void Analog_ChangeNote(uint8_t note, bool state, Analog_t *ana) {
  if(state) {
    ana->oscnote = note;
    Analog_NoteToDac(ana);
    Analog_DigitalVoice(true,ana);
  } else {
    Analog_DigitalVoice(false,ana);
  }
}

void Analog_CycleValue(Analog_t *ana) {
  DAC_Sendvalue(ana->buffer[ana->bufferindex],ana->id);
  ana->bufferindex++;
  ana->bufferindex %= kBLOCK_SIZE*2;
}

void Analog_TimerUpdate(Analog_t *ana) {
  if(ana->trigger) {
    ana->trigger--;
    if(ana->trigger == 0) {
      Analog_DigitalValue(false,false,ana);
    }
  } 
  if (ana->oscillator) {
    if(ana->osccounter == 0) {
      Analog_Oscillator(ana);
    }
    ana->osccounter++;
    ana->osccounter %= (kBLOCK_SIZE);
  } else if(ana->envelope) {
    if(ana->osccounter == 0) {
      Analog_Envelope(ana);
    }
    ana->osccounter++;
    ana->osccounter %= (kBLOCK_SIZE);
  } else if(ana->random) {
    if(ana->osccounter == 0) {
      Analog_Random(ana);
    }
    ana->osccounter++;
    ana->osccounter %= (kBLOCK_SIZE);
  }
}

void Analog_Random(Analog_t *ana) {
  switch(ana->mode) {
    case ANALOG_DIGITAL_RANDOM:
      if(customrand() < randomthreshold[ana->id]) {
        Analog_DigitalValue(true,true,ana);
      }
      break;

    case ANALOG_RANDOM:
      if(customrand() < randomthreshold[ana->id]) {
        DAC_InsertValue((customrand() / 429496729.0f) - 5.0f,ana->id);
      }
      break;

    default:
      break;
  }   
}


void Analog_Envelope(Analog_t *ana) {

  uint8_t tempindex = 0;
  if(ana->bufferindex < kBLOCK_SIZE) {
    tempindex = kBLOCK_SIZE;
  }

  for (size_t i = 0; i < kBLOCK_SIZE; i++) {
    switch(ana->mode) {
      case ANALOG_ENVELOPE_AD_EXPO:
      case ANALOG_ENVELOPE_AD_LINEAR:
      case ANALOG_ENVELOPE_AD_LOG:
        if(ana->oscnote == 0xFF) {
          if(ana->envelopeout <= 0) {
            ana->envelopeout = 0;
          } else {
            ana->envelopeout -= envelopetime[ana->settings.byte3];
          }
        } else {
          if(ana->envelopeout >= 8) {
            ana->oscnote = 0xFF;
            ana->envelopeout = 8;
          } else {
            ana->envelopeout += envelopetime[ana->settings.byte2];
          }
        }
        break;

      case ANALOG_ENVELOPE_ASR_EXPO:
      case ANALOG_ENVELOPE_ASR_LINEAR:
      case ANALOG_ENVELOPE_ASR_LOG:
        if(ana->oscnote == 0xFF) {
          if(ana->envelopeout <= 0) {
            ana->envelopeout = 0;
          } else {
            ana->envelopeout -= envelopetime[ana->settings.byte3];
          }
        } else {
          if(ana->envelopeout >= 8) {
            ana->envelopeout = 8;
          } else {
            ana->envelopeout += envelopetime[ana->settings.byte2];
          }
        }
        break;

      case ANALOG_ENVELOPE_VELOCITY_DECAY_EXPO:
      case ANALOG_ENVELOPE_VELOCITY_DECAY_LINEAR:
      case ANALOG_ENVELOPE_VELOCITY_DECAY_LOG:
        if(ana->envelopeout <= 0) {
          ana->envelopeout = 0;
        } else {
          ana->envelopeout -= envelopetime[ana->settings.byte2];
        }
        break;
      default:
        break;
    }
    switch(ana->mode) {
      case ANALOG_ENVELOPE_AD_EXPO:
      case ANALOG_ENVELOPE_ASR_EXPO:
        if(ana->envelopeout >= 8|| ana->envelopeout < 0) {
          ana->buffer[tempindex] = DAC_GetValue(ana->envelopeout,ana->id);
        } else {
          int tempout = ana->envelopeout*kENVELOPE_EXPO_SIZE/8;
          ana->buffer[tempindex] = DAC_GetValue(envelopeexpo[tempout],ana->id);
        }
        break;
      case ANALOG_ENVELOPE_VELOCITY_DECAY_EXPO:
        if(ana->envelopeout >= 8 || ana->envelopeout < 0) {
          ana->buffer[tempindex] = DAC_GetValue(ana->envelopeout,ana->id);
        } else {
          int tempout = ana->envelopeout*kENVELOPE_EXPO_SIZE/8;
          ana->buffer[tempindex] = DAC_GetValue((envelopeexpo[tempout]*ana->envelopesize)/127.0f,ana->id);
        }
        break;

      case ANALOG_ENVELOPE_ASR_LOG:
      case ANALOG_ENVELOPE_AD_LOG:
        if(ana->envelopeout >= 8 || ana->envelopeout < 0) {
          ana->buffer[tempindex] = DAC_GetValue(ana->envelopeout,ana->id);
        } else {
          int tempout =  kENVELOPE_EXPO_SIZE-((ana->envelopeout)*kENVELOPE_EXPO_SIZE/8);
          ana->buffer[tempindex] = DAC_GetValue(5.0f-envelopeexpo[tempout],ana->id);
        }
        break;
      case ANALOG_ENVELOPE_VELOCITY_DECAY_LOG:
        if(ana->envelopeout >= 8 || ana->envelopeout < 0) {
          ana->buffer[tempindex] = DAC_GetValue(ana->envelopeout,ana->id);
        } else {
          int tempout =  kENVELOPE_EXPO_SIZE-((ana->envelopeout)*kENVELOPE_EXPO_SIZE/8);
          ana->buffer[tempindex] = DAC_GetValue(((5.0f-envelopeexpo[tempout])*ana->envelopesize)/127.0f,ana->id);
        }
        break;

      case ANALOG_ENVELOPE_ASR_LINEAR:
      case ANALOG_ENVELOPE_AD_LINEAR:
        ana->buffer[tempindex] = DAC_GetValue(ana->envelopeout,ana->id);
        break;
      case ANALOG_ENVELOPE_VELOCITY_DECAY_LINEAR:
        ana->buffer[tempindex] = DAC_GetValue((ana->envelopeout*ana->envelopesize)/127.0f,ana->id);
        break;

      default:
        break;
    }


    tempindex++;
  }
}

void Analog_CalculateFrequency(Analog_t *ana) {
  float pitchbend = (ana->pitchbend * ana->settings.byte2) / (12*8192.0f);
  ana->oschz = exp2f((ana->oscnote-69.0f)/12 + pitchbend) * 440 * (1.0f/kSAMPLE_RATE);
}


void Analog_Oscillator(Analog_t *ana) {  
  uint8_t tempindex = 0;
  if(ana->bufferindex < kBLOCK_SIZE) {
    tempindex = kBLOCK_SIZE;
  }

  if(ana->oscnote == 0xFF && ana->digitalvoice == false) {
    for (size_t i = 0; i < kBLOCK_SIZE; i++)
    {
      ana->buffer[tempindex] = DAC_GetValue(0.0f,ana->id);
      tempindex++;
    }
  } else {
    float output;
    for (size_t i = 0; i < kBLOCK_SIZE; i++)
    {
        ana->phase += ana->oschz;
        while(ana->phase >= 1){
          ana->phase -= 1;
        }

        switch (ana->mode) {
          case ANALOG_MONO_OSC_PULSE:
          case ANALOG_POLY_OSC_PULSE_DUO:
          case ANALOG_POLY_OSC_PULSE_QUAD:
            if(ana->phase >= 0.5f) {
              output = 1.0f;
            } else {
              output = -1.0f;
            }
            break;

          case ANALOG_MONO_OSC_SAW:
          case ANALOG_POLY_OSC_SAW_DUO:
          case ANALOG_POLY_OSC_SAW_QUAD:
            output = ana->phase * 2 - 1;
            break;

          case ANALOG_MONO_OSC_TRI:
          case ANALOG_POLY_OSC_TRI_DUO:
          case ANALOG_POLY_OSC_TRI_QUAD:
            if(ana->phase >= 0.5f) {
              output = -ana->phase * 4 + 3;
            } else {
              output = ana->phase * 4 - 1;
            }
            break;

          case ANALOG_MONO_OSC_SINE:
          case ANALOG_POLY_OSC_SINE_DUO:
          case ANALOG_POLY_OSC_SINE_QUAD:
            output = sinelut[(int)(ana->phase*(sizeof(sinelut)/sizeof(float)))];
            break;

          case ANALOG_OSC_NOISE:
            output = (float)customrand() / (float)(0xFFFFFFFF*0.5f) - 1.0f;
            break;          

          default:
            output = 0;
            break;
        }
      
      ana->buffer[tempindex] = DAC_GetValue(output,ana->id);
      tempindex++;
    }
  }
}

void Analog_CC(MidiMessage *message, Analog_t *ana) {
  switch (ana->mode) {
    case ANALOG_CC_UNI:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->settings.byte2 == message->byte2) {
          float value = (float)message->byte3 / (127.0f/5.0f);
          DAC_InsertValue(value, ana->id);
        }
      }
      break;
    case ANALOG_CC_BI:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->settings.byte2 == message->byte2) {
          float value = (float)message->byte3 / (127.0f/2.5f) - 5.0f;
          DAC_InsertValue(value, ana->id);
        }
      }
      break;
    default:
      break;
  }
}

void Analog_NoteOn(MidiMessage *message, Analog_t *ana) {
  switch (ana->mode) {
    case ANALOG_MONO_OSC_PULSE:
    case ANALOG_MONO_OSC_SAW:
    case ANALOG_MONO_OSC_TRI:
    case ANALOG_MONO_OSC_SINE:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        NoteStack_NoteOn(message->byte2,message->byte3,&ana->stack);
        uint8_t tempnote = NoteStack_RecentNote(&ana->stack);
        if(tempnote != 0xFF) {
          ana->oscnote = tempnote;
          Analog_CalculateFrequency(ana);
          Analog_DigitalVoice(true,ana);
        } else {
          Analog_DigitalVoice(false,ana);
        }
      }
      break;

    case ANALOG_ENVELOPE_ASR_EXPO:
    case ANALOG_ENVELOPE_ASR_LINEAR:
    case ANALOG_ENVELOPE_ASR_LOG:
    case ANALOG_ENVELOPE_AD_EXPO:
    case ANALOG_ENVELOPE_AD_LINEAR:
    case ANALOG_ENVELOPE_AD_LOG:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        NoteStack_NoteOn(message->byte2,message->byte3,&ana->stack);
        uint8_t tempnote = NoteStack_RecentNote(&ana->stack);
        ana->oscnote = tempnote;
      }
      break;

    case ANALOG_ENVELOPE_VELOCITY_DECAY_EXPO:
    case ANALOG_ENVELOPE_VELOCITY_DECAY_LINEAR:
    case ANALOG_ENVELOPE_VELOCITY_DECAY_LOG:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        NoteStack_NoteOn(message->byte2,message->byte3,&ana->stack);
        uint8_t tempnote = NoteStack_RecentNote(&ana->stack);
        ana->oscnote = tempnote;
        ana->envelopeout = 5;
        ana->envelopesize = message->byte3;
      }
      break;

    case ANALOG_VOCT:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        NoteStack_NoteOn(message->byte2,message->byte3,&ana->stack);
        uint8_t note = NoteStack_RecentNote(&ana->stack);
        bool state = true;
        if(note == 0xFF) {
          state = false;
        }
        Analog_ChangeNote(note,state,ana);      
      }
      break;

    case ANALOG_VELOCITY_UNI:
    case ANALOG_KEYPRESSURE_UNI:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        NoteStack_NoteOn(message->byte2, message->byte3, &ana->stack);
        uint8_t vel = NoteStack_RecentVelocity(&ana->stack);
        DAC_InsertValue((5*vel)/127.0f,ana->id);
      }
      break;

    case ANALOG_VELOCITY_BI:
    case ANALOG_KEYPRESSURE_BI:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        NoteStack_NoteOn(message->byte2, message->byte3, &ana->stack);
        uint8_t vel = NoteStack_RecentVelocity(&ana->stack);
        DAC_InsertValue((10*vel)/127.0f - 5.0f,ana->id);
      }
      break;

    case ANALOG_POLY_DUO:
    case ANALOG_POLY_OSC_SAW_DUO:
    case ANALOG_POLY_OSC_PULSE_DUO:
    case ANALOG_POLY_OSC_TRI_DUO:
    case ANALOG_POLY_OSC_SINE_DUO:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->id == 0) {
          Poly_NoteOn(message->byte2, message->byte3, &poly[0]);
        } else if(ana->id == 2) {
          Poly_NoteOn(message->byte2, message->byte3, &poly[1]); 
        }
      }
      break;

    case ANALOG_POLY_QUAD:
    case ANALOG_POLY_OSC_SAW_QUAD:
    case ANALOG_POLY_OSC_PULSE_QUAD:
    case ANALOG_POLY_OSC_TRI_QUAD:
    case ANALOG_POLY_OSC_SINE_QUAD:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->id == 0) {
          Poly_NoteOn(message->byte2, message->byte3, &poly[0]);
        }
      }
      break;

    case ANALOG_VELOCITY_GATE:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->settings.byte2 == message->byte2 || ana->settings.byte2 == 255) {
          DAC_InsertValue((5*message->byte3)/127.0f,ana->id);
        }
      }
      break;

    case ANALOG_VELOCITY_TRIGGER:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->settings.byte2 == message->byte2 || ana->settings.byte2 == 255) {
          DAC_InsertValue((5*message->byte3)/127.0f,ana->id);
          ana->trigger = (kSAMPLE_RATE/1000);
        }
      }
      break;

    case ANALOG_DIGITAL_GATE:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->settings.byte2 == message->byte2 || ana->settings.byte2 == 255) {
          Analog_DigitalValuePedal(true,false,ana);
        }
      }
      break;

    case ANALOG_DIGITAL_TRIGGER:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->settings.byte2 == message->byte2 || ana->settings.byte2 == 255) {
          Analog_DigitalValue(true,true,ana);
        }
      }
      break;
      
    default:
      break;
  }
}

void Analog_NoteOff(MidiMessage *message, Analog_t *ana) {
  switch (ana->mode) {
    case ANALOG_MONO_OSC_PULSE:
    case ANALOG_MONO_OSC_SAW:
    case ANALOG_MONO_OSC_TRI:
    case ANALOG_MONO_OSC_SINE:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        NoteStack_NoteOff(message->byte2, &ana->stack);
        uint8_t tempnote = NoteStack_RecentNote(&ana->stack);
        if(tempnote == 0xFF && ana->pedal) {
          Analog_DigitalVoice(false,ana);
        } else {
          Analog_DigitalVoice(true,ana);
        } 
        if(tempnote != 0xFF) {
          ana->oscnote = tempnote;
          Analog_CalculateFrequency(ana);
        }
        break;
      }
      break;

    case ANALOG_ENVELOPE_ASR_EXPO:
    case ANALOG_ENVELOPE_ASR_LINEAR:
    case ANALOG_ENVELOPE_ASR_LOG:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        NoteStack_NoteOff(message->byte2, &ana->stack);
        uint8_t tempnote = NoteStack_RecentNote(&ana->stack);
        if(tempnote == 0xFF && ana->pedal) {
        } else {
          ana->oscnote = tempnote;
        }
      }
      break;

    case ANALOG_VOCT:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        NoteStack_NoteOff(message->byte2, &ana->stack);
        uint8_t note = NoteStack_RecentNote(&ana->stack);
        bool state = true;
        if(note == 0xFF) {
          state = false;
        }
        Analog_ChangeNote(note,state,ana);
      }
      break;

    case ANALOG_VELOCITY_UNI:
    case ANALOG_KEYPRESSURE_UNI:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        NoteStack_NoteOff(message->byte2, &ana->stack);
        if(NoteStack_RecentNote(&ana->stack) != 0xFF) {
          uint8_t vel = NoteStack_RecentVelocity(&ana->stack);
          DAC_InsertValue((5*vel)/127.0f,ana->id);
        } 
      }
      break;

    case ANALOG_VELOCITY_BI:
    case ANALOG_KEYPRESSURE_BI:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        NoteStack_NoteOff(message->byte2, &ana->stack);
        if(NoteStack_RecentNote(&ana->stack) != 0xFF) {
          uint8_t vel = NoteStack_RecentVelocity(&ana->stack);
          DAC_InsertValue((10*vel)/127.0f - 5.0f,ana->id);
        } 
      }
      break;

    case ANALOG_POLY_DUO:
    case ANALOG_POLY_OSC_SAW_DUO:
    case ANALOG_POLY_OSC_PULSE_DUO:
    case ANALOG_POLY_OSC_TRI_DUO:
    case ANALOG_POLY_OSC_SINE_DUO:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->id == 0) {
          Poly_NoteOff(message->byte2, &poly[0]);
        } else if(ana->id == 2) {
          Poly_NoteOff(message->byte2, &poly[1]); 
        }
      }
      break;

    case ANALOG_POLY_QUAD:
    case ANALOG_POLY_OSC_SAW_QUAD:
    case ANALOG_POLY_OSC_PULSE_QUAD:
    case ANALOG_POLY_OSC_TRI_QUAD:
    case ANALOG_POLY_OSC_SINE_QUAD:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->id == 0) {
          Poly_NoteOff(message->byte2, &poly[0]);
        }
      }
      break;
    
    case ANALOG_VELOCITY_GATE:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->settings.byte2 == message->byte2 || ana->settings.byte2 == 255) {
          Analog_DigitalValuePedal(false,false,ana);
        }
      }

    case ANALOG_DIGITAL_GATE:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        if(ana->settings.byte2 == message->byte2 || ana->settings.byte2 == 255) {
          Analog_DigitalValuePedal(false,false,ana);
        }
      }
      break;

    default:
      break;
  }
}

void Analog_PolyInput(uint8_t note, uint8_t velo, bool state, Analog_t *ana) {
  switch (ana->mode) {
    case ANALOG_POLY_DUO:
    case ANALOG_POLY_QUAD:
      Analog_ChangeNote(note,state,ana);
      break;


    case ANALOG_POLY_OSC_PULSE_DUO:
    case ANALOG_POLY_OSC_SAW_DUO:
    case ANALOG_POLY_OSC_TRI_DUO:
    case ANALOG_POLY_OSC_SINE_DUO:
    case ANALOG_POLY_OSC_PULSE_QUAD:
    case ANALOG_POLY_OSC_SAW_QUAD:
    case ANALOG_POLY_OSC_TRI_QUAD:
    case ANALOG_POLY_OSC_SINE_QUAD:
      if(ana->digitalvoice) {
        Analog_DigitalVoice(state,ana);
        if(state) {
          ana->oscnote = note;
          Analog_CalculateFrequency(ana);
        }
      } else {
        ana->digitalstate = state;
        if(state == false) {
          if(ana->pedal == 0) {
            ana->oscnote = 0xFF;
            Analog_CalculateFrequency(ana);
          }
        } else {
          ana->oscnote = note;
          Analog_CalculateFrequency(ana);
        }
      }
      
      break;

    default:
      break;
  }
}

void Analog_Clock(MidiMessage *message, Analog_t *ana) {
  switch (ana->mode)
  {
    case ANALOG_DIGITAL_CLOCK:
      if(message->byte1 == 0xF8) { // Midi message Clock
        Analog_DigitalValue(true,true,ana);
      }
      break;

    case ANALOG_DIGITAL_CLOCK_RESET:
      if(message->byte1 == 0xFA) { // Midi Message Running
        Analog_DigitalValue(true,true,ana);
      }
      break;
    case ANALOG_DIGITAL_CLOCK_RUN:
      if(message->byte1 == 0xFA) { // Midi Message Running
        Analog_DigitalValue(true,false,ana);
      } else if(message->byte1 == 0xFC) { // Midi Message stopped
        Analog_DigitalValue(false,false,ana);
      }
      break;
      
    default:
      break;
  }
}

void Analog_Aftertouch(MidiMessage *message, Analog_t *ana) {
  switch (ana->mode) {
    case ANALOG_AFTERTOUCH_UNI:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        DAC_InsertValue((5*message->byte2)/127.0f,ana->id);
      }
      break;

    case ANALOG_AFTERTOUCH_BI:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        DAC_InsertValue((10*message->byte2)/127.0f - 5.0f,ana->id);
      }
      break;
      
    default:
      break;
  }
}

void Analog_Pitchbend(MidiMessage *message, Analog_t *ana) {
  switch(ana->mode) {
    case ANALOG_POLY_OSC_SAW_DUO:
    case ANALOG_POLY_OSC_PULSE_DUO:
    case ANALOG_POLY_OSC_TRI_DUO:
    case ANALOG_POLY_OSC_SINE_DUO:
    case ANALOG_POLY_OSC_SAW_QUAD:
    case ANALOG_POLY_OSC_PULSE_QUAD:
    case ANALOG_POLY_OSC_TRI_QUAD:
    case ANALOG_POLY_OSC_SINE_QUAD:
    case ANALOG_MONO_OSC_PULSE:
    case ANALOG_MONO_OSC_SAW:
    case ANALOG_MONO_OSC_TRI:
    case ANALOG_MONO_OSC_SINE:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        ana->pitchbend = -8192 + message->byte2 + (message->byte3 << 7);
        Analog_CalculateFrequency(ana);
      }
      break;

    case ANALOG_VOCT:
    case ANALOG_POLY_DUO:
    case ANALOG_POLY_QUAD:
      if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
        ana->pitchbend = -8192 + message->byte2 + (message->byte3 << 7); 
        Analog_NoteToDac(ana);
      }
      break;

    default:
      break;
  }
}

void Analog_Pedal(MidiMessage *message, Analog_t *ana) {
  if(message->midichannel == ana->settings.midichannel || ana->settings.midichannel == 255) {
    ana->pedal = message->byte3;
    uint8_t tempnote = NoteStack_RecentNote(&ana->stack);
    switch(ana->mode) {
      case ANALOG_DIGITAL_GATE:
        if(ana->pedal == 0) {
          Analog_DigitalValue(ana->digitalstate,false,ana);
        }
        break;

      case ANALOG_ENVELOPE_ASR_EXPO:
      case ANALOG_ENVELOPE_ASR_LOG:
      case ANALOG_ENVELOPE_ASR_LINEAR:
        if(tempnote == 0xFF && ana->pedal == 0) {
          ana->oscnote = tempnote;
        }
        break;


      case ANALOG_POLY_OSC_PULSE_DUO:
      case ANALOG_POLY_OSC_SAW_DUO:
      case ANALOG_POLY_OSC_TRI_DUO:
      case ANALOG_POLY_OSC_SINE_DUO:
      case ANALOG_POLY_OSC_PULSE_QUAD:
      case ANALOG_POLY_OSC_SAW_QUAD:
      case ANALOG_POLY_OSC_TRI_QUAD:
      case ANALOG_POLY_OSC_SINE_QUAD:
      case ANALOG_MONO_OSC_PULSE:
      case ANALOG_MONO_OSC_SAW:
      case ANALOG_MONO_OSC_TRI:
      case ANALOG_MONO_OSC_SINE:
        if(ana->pedal == 0) {
          if(ana->digitalstate == 0) {
            ana->oscnote = 0xFF;
          }
        }

      default:
        break;
    }
  }
}