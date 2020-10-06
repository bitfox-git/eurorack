#ifndef DIGITAL_H_
#define DIGITAL_H_

#include "stdint.h"
#include "stdbool.h"
#include "midimessage.h"
#include "midisetting.h"

#define kDIGITAL_AMOUNT 4

typedef enum  {
  DIGITAL_OFF=0x00,

  DIGITAL_GATE,
  DIGITAL_TRIGGER,

  DIGITAL_CLOCK,
  DIGITAL_CLOCK_RESET,
  DIGITAL_CLOCK_RUN,

  DIGITAL_VOICE_GATE,
  DIGITAL_VOICE_TRIGGER,

  DIGITAL_RANDOM=0xFF,
}Digital_Settings;

typedef struct {
  bool state;

  bool voice;

  bool random;
  uint8_t randomcounter;
  
  MidiSetting settings;
  Digital_Settings mode;

  uint8_t trigger;

  uint8_t gpiopin;
  uint8_t id;

  uint8_t counter;

  uint8_t pedal;

}Digital_t;

Digital_t digital[kDIGITAL_AMOUNT];

void Digital_NoteOn(MidiMessage *message, Digital_t *dig);
void Digital_NoteOff(MidiMessage *message, Digital_t *dig);
void Digital_Clock(MidiMessage *message, Digital_t *dig);
void Digital_Pedal(MidiMessage *message, Digital_t *dig);

void Digital_Reset(Digital_t *dig);

void Digital_Voice(bool state, Digital_t *dig);

void Digital_TimerUpdate(Digital_t *dig);
void Digital_Random(Digital_t *dig);

void Digital_ChangeValue(bool state, bool trigger, Digital_t *dig);
void Digital_ChangeValuePedal(bool state, bool trigger, Digital_t *dig);


void Digital_Init();
void Digital_Configure(Digital_t *dig, Digital_Settings mode, MidiSetting *midiset);

#endif // DIGITAL_H_