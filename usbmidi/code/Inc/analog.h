#ifndef ANALOG_H_
#define ANALOG_H_

#include "stdint.h"
#include "stdbool.h"
#include "midimessage.h"
#include "midisetting.h"
#include "note_stack.h"


#define kANALOG_AMOUNT 4
#define kBLOCK_SIZE 6
#define kSAMPLE_RATE 11000
//   htim6.Init.Period = 72000000/(kSAMPLE_RATE*4) - 1;


typedef enum  {
  ANALOG_OFF=0x00,
  ANALOG_CONSTANT,

  ANALOG_VOCT,
  ANALOG_POLY_DUO,
  ANALOG_POLY_QUAD,

  ANALOG_CC_UNI,
  ANALOG_CC_BI,
  
  ANALOG_VELOCITY_UNI,
  ANALOG_VELOCITY_BI,

  ANALOG_AFTERTOUCH_UNI,
  ANALOG_AFTERTOUCH_BI,

  ANALOG_KEYPRESSURE_UNI,
  ANALOG_KEYPRESSURE_BI,

  ANALOG_OSC_NOISE,
  ANALOG_MONO_OSC_PULSE,
  ANALOG_MONO_OSC_SAW,
  ANALOG_MONO_OSC_TRI,
  ANALOG_MONO_OSC_SINE,
  ANALOG_POLY_OSC_PULSE_DUO,
  ANALOG_POLY_OSC_SAW_DUO,
  ANALOG_POLY_OSC_TRI_DUO,
  ANALOG_POLY_OSC_SINE_DUO,
  ANALOG_POLY_OSC_PULSE_QUAD,
  ANALOG_POLY_OSC_SAW_QUAD,
  ANALOG_POLY_OSC_TRI_QUAD,
  ANALOG_POLY_OSC_SINE_QUAD,

  ANALOG_DIGITAL_GATE,
  ANALOG_DIGITAL_TRIGGER,
  ANALOG_DIGITAL_CLOCK,
  ANALOG_DIGITAL_CLOCK_RESET,
  ANALOG_DIGITAL_CLOCK_RUN,
  ANALOG_DIGITAL_RANDOM,

  ANALOG_VELOCITY_GATE,
  ANALOG_VELOCITY_TRIGGER,

  ANALOG_ENVELOPE_AD_LINEAR,
  ANALOG_ENVELOPE_AD_EXPO,
  ANALOG_ENVELOPE_AD_LOG,

  ANALOG_ENVELOPE_ASR_LINEAR,
  ANALOG_ENVELOPE_ASR_EXPO,
  ANALOG_ENVELOPE_ASR_LOG,

  ANALOG_ENVELOPE_VELOCITY_DECAY_LINEAR,
  ANALOG_ENVELOPE_VELOCITY_DECAY_EXPO,
  ANALOG_ENVELOPE_VELOCITY_DECAY_LOG,
  
  // ADD NEW OPTIONS HERE

  ANALOG_RANDOM=0xFF,
} Analog_Settings;



typedef struct {
  MidiSetting settings;
  Analog_Settings mode;
  bool digitalvoice;
  
  uint8_t trigger;

  uint8_t oscnote;
  int pitchbend;
  uint8_t pedal;
  bool digitalstate;

  float oschz;
  bool oscillator;
  uint8_t osccounter;
  float phase;

  bool envelope;
  float envelopeout;
  uint8_t envelopesize;

  bool random;

  NoteStack stack;

  uint16_t buffer[kBLOCK_SIZE*2];
  uint8_t bufferindex;

  uint8_t id;

} Analog_t;


extern uint32_t randomthreshold[4];
extern Analog_t analog[kANALOG_AMOUNT];

void Analog_NoteOn(MidiMessage *message, Analog_t *ana);
void Analog_NoteOff(MidiMessage *message, Analog_t *ana);
void Analog_CC(MidiMessage *message, Analog_t *ana);
void Analog_Aftertouch(MidiMessage *message, Analog_t *ana);
void Analog_Pitchbend(MidiMessage *message, Analog_t *ana);
void Analog_Clock(MidiMessage *message, Analog_t *ana);
void Analog_Pedal(MidiMessage *message, Analog_t *ana);

uint32_t customrand();

void Analog_Reset(Analog_t *ana);

void Analog_Oscillator(Analog_t *ana);
void Analog_Random(Analog_t *ana);
void Analog_Envelope(Analog_t *ana);
void Analog_CycleValue(Analog_t *ana);

void Analog_PolyInput(uint8_t note, uint8_t velo, bool state, Analog_t *ana);

void Analog_TimerUpdate(Analog_t *dig);
void Analog_ChangeNote(uint8_t note, bool state, Analog_t *ana);


void Analog_Init();
void Analog_Configure(Analog_t *ana, Analog_Settings mode, MidiSetting *midiset);



#endif // ANALOG_H_