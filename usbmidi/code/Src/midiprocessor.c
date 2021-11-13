#include "midiprocessor.h"
#include "sysexprocessor.h"
#include "main.h"
#include "dac.h"
#include "led.h"
#include "digital.h"
#include "analog.h"
#include "configuration.h"

void MIDI_InputMessage(uint32_t input) {
  
  MidiMessage midimessage;
  // uint32_t comes in "wrong" endian format
  midimessage.messagetype = input & 0x0F;
  midimessage.cablenum = (input >> 4) & 0x0F;
  midimessage.byte1 = (input >> 8) & 0xFF;
  midimessage.byte2 = (input >> 16) & 0xFF;
  midimessage.byte3 = (input >> 24) & 0xFF;
  midimessage.midichannel = midimessage.byte1 & 0x0F;
  
  switch(midimessage.messagetype) {
    // case 0x0:
    // case 0x1:
    //   break;

    // case 0x2: //MTC SongSelect
    //   break;
    
    // case 0x3: //SSP
    //   break; 

    case 0x4: // SysEx starts or continues
      Sysex_Process(midimessage.byte1);
      Sysex_Process(midimessage.byte2);
      Sysex_Process(midimessage.byte3);
      break;


    case 0x5: // Single-byte System Common Message or SysEx ends with following single byte.
      if(midimessage.byte1 == 0xF7) {
        Sysex_Process(midimessage.byte1);
      }
      else { // Process the single byte System Common Message

      }
      break;

    case 0x6: // SysEx ends with following two bytes
      Sysex_Process(midimessage.byte1);
      Sysex_Process(midimessage.byte2);
      break;
      
    case 0x7: // SysEx ends with following three bytes.
      Sysex_Process(midimessage.byte1);
      Sysex_Process(midimessage.byte2);
      Sysex_Process(midimessage.byte3);
      break;

    case 0x8: // Note Off
      for (size_t i = 0; i < 4; i++) {
        Analog_NoteOff(&midimessage,&analog[i]);
        Digital_NoteOff(&midimessage,&digital[i]);
      }
      break;

    case 0x9: // Note On
      for (size_t i = 0; i < 4; i++) {
        Analog_NoteOn(&midimessage,&analog[i]);
        Digital_NoteOn(&midimessage,&digital[i]);
      }
      break;

    case 0xA: // Poly-KeyPress
      break;

    case 0xB: // CC
      if(midimessage.byte2 == 0) {
        break;
      } 
      else if(midimessage.byte2 < 120) {
          if(midimessage.byte2 == 64) {
            for (size_t i = 0; i < 4; i++) {
              Analog_Pedal(&midimessage,&analog[i]);
              Digital_Pedal(&midimessage,&digital[i]);
            }
          }
          for (size_t i = 0; i < 4; i++) {
            Analog_CC(&midimessage,&analog[i]);
          }
      } else if(midimessage.byte2 < 124 && midimessage.byte2 != 122){
        for (size_t i = 0; i < 4; i++) {
          Digital_Reset(&digital[i]);
          Analog_Reset(&analog[i]);
        }
      }
      
      break;

    case 0xC: // Program Change
      Config_ProgramChange(midimessage.byte2 & 0x7F);
      break;
    
    case 0xD: // Channel Pressure
      for (size_t i = 0; i < 4; i++) {
        Analog_Aftertouch(&midimessage, &analog[i]);
      }
      break;

    case 0xE: // Pitchbend
      for (size_t i = 0; i < 4; i++) {
        Analog_Pitchbend(&midimessage, &analog[i]);
      }
      break;


    case 0xF: // SingleByte System Real Time Messages
      for (size_t i = 0; i < 4; i++){
        Analog_Clock(&midimessage,&analog[i]);
        Digital_Clock(&midimessage,&digital[i]);
      }
      break;
    default: 
      break;
  }
}
