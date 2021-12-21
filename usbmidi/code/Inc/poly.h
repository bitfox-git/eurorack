#ifndef POLY_H_
#define POLY_H_

#include "stdint.h"
#include "stdbool.h"

#define kMAX_POLYPHONY 4


typedef struct {
  uint8_t notes[kMAX_POLYPHONY];
  uint8_t velo[kMAX_POLYPHONY];
  uint8_t prio[kMAX_POLYPHONY];
  uint8_t size;
  uint8_t polyamount;
  uint8_t id;
} Poly_t;

extern Poly_t poly[2];

void Poly_Init();

void Poly_NoteOn(uint8_t note, uint8_t velocity, Poly_t *p);
void Poly_NoteOff(uint8_t note, Poly_t *p);

#endif // POLY_H_