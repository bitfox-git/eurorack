#include "poly.h"
#include "analog.h"


void Poly_CheckVoices(Poly_t *p){
  uint8_t maxpoly = 0;
  if(p->id == 0) {
    uint8_t maxcheck = 4;
    if(analog[0].mode == ANALOG_POLY_DUO || analog[0].mode == ANALOG_POLY_OSC_PULSE_DUO || analog[0].mode == ANALOG_POLY_OSC_SAW_DUO 
                                        || analog[0].mode == ANALOG_POLY_OSC_TRI_DUO || analog[0].mode == ANALOG_POLY_OSC_SINE_DUO) {
      maxcheck = 2;
    }

    for (uint8_t i = 0; i < maxcheck; i++) { 
      if (analog[i].mode == ANALOG_POLY_QUAD || analog[i].mode == ANALOG_POLY_DUO || analog[i].oscillator) {
        maxpoly += 1;
      } else {
        break;
      }
    }
  } else {
    for (uint8_t i = p->id*2; i < p->id*2+2; i++) { 
      if (analog[i].mode == ANALOG_POLY_DUO || analog[i].oscillator) {
        maxpoly += 1;
      } else {
        break;
      }
    }
  }
  p->polyamount = maxpoly;
}

void Poly_Init() {
  poly[0].id = 0;
  poly[1].id = 1;
  for (uint8_t i = 0; i < kMAX_POLYPHONY; ++i) {
    poly[0].prio[i] = i;
    poly[1].prio[i] = i;
    poly[0].notes[i] = 0xFF;
    poly[0].velo[i] = 0xFF;
    poly[1].notes[i] = 0xFF;
    poly[1].velo[i] = 0xFF;
  }
  Poly_CheckVoices(&poly[0]);
  Poly_CheckVoices(&poly[1]);
}

void Poly_NoteOn(uint8_t note, uint8_t velocity, Poly_t *p) {
  if(p->polyamount == 0) { 
    Poly_CheckVoices(p);
  }

  // Retrigger the note that already plays ????? This shouldn't happen. ( maybe on string controllers ? )
  for (uint8_t i = 0; i < p->polyamount; ++i) {
    if (p->notes[i] == note) {
      return;
    }
  }
  
  // If there are inactive voices simply add
  if(p->size != p->polyamount) {
    for (uint8_t i = 0; i < p->polyamount; i++) {
      if(p->prio[i] == p->size) {
        p->notes[i] = note;
        p->velo[i] = velocity;
        p->size++;

        Analog_PolyInput(note,velocity,true,&analog[p->id*2+i]);
        return;
      }
    }  
  } else { // If full replace prio 0
    for (uint8_t i = 0; i < p->polyamount; i++) {
      if(p->prio[i] == 0) {
        p->notes[i] = note;
        p->velo[i] = velocity;
        for (uint8_t i = 0; i < p->polyamount; i++)
        {
          p->prio[i]--;
          p->prio[i] %= p->polyamount;
        }
        Analog_PolyInput(note,velocity,true,&analog[p->id*2+i]);
        return;
      }
    }  
  }
}

void Poly_NoteOff(uint8_t note, Poly_t *p) {
  for (uint8_t i = 0; i < p->polyamount; ++i) {
    if (p->notes[i] == note) {
      p->notes[i] = 0xFF;
      uint8_t check = p->prio[i];
      for (uint8_t j = 0; j < p->polyamount; j++)
      {
        if(i != j && check < p->prio[j]) {
          p->prio[j]--;
        }
      }
      p->prio[i] = p->polyamount-1;
      p->size--;
      Analog_PolyInput(0xFF,0xFF,false,&analog[p->id*2+i]);
    }
  }
}


