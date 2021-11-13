#ifndef NOTE_STACK_H_
#define NOTE_STACK_H_

#include "stdint.h"
#include "stdbool.h"

#define kFREE_SLOT 0xFF
#define kNOTE_STACK_CAPACITY 16

typedef struct {
  uint8_t note;
  uint8_t velocity;
  uint8_t next_ptr;  // Base 1.
} NoteEntry;

typedef struct {
  uint8_t size;
  NoteEntry pool[kNOTE_STACK_CAPACITY + 1];  // First element is a dummy node!
  uint8_t root_ptr;  // Base 1.
  uint8_t sorted_ptr[kNOTE_STACK_CAPACITY + 1];  // Base 1.
} NoteStack;

void NoteStack_NoteOn(uint8_t note, uint8_t velocity, NoteStack *stack);
void NoteStack_NoteOff(uint8_t note, NoteStack *stack);
void NoteStack_Clear(NoteStack *stack);

uint8_t NoteStack_RecentNote(NoteStack *stack);
uint8_t NoteStack_RecentVelocity(NoteStack *stack);

  // const NoteEntry& played_note(uint8_t index) const {
  //   uint8_t current = root_ptr_;
  //   index = size_ - index - 1;
  //   for (uint8_t i = 0; i < index; ++i) {
  //     current = pool_[current].next_ptr;
  //   }
  //   return pool_[current];
  // }
  // const NoteEntry& sorted_note(uint8_t index) const {
  //   return pool_[sorted_ptr_[index]];
  // }
  // const NoteEntry& note(uint8_t index) const { return pool_[index]; }
  // NoteEntry* mutable_note(uint8_t index) { return &pool_[index]; }
  // const NoteEntry& dummy() const { return pool_[0]; }

#endif  // CVPAL_NOTE_STACK_H_
