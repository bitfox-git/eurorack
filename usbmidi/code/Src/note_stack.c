#include "note_stack.h"

uint8_t NoteStack_RecentNote(NoteStack *stack) {
  return stack->pool[stack->root_ptr].note;
}
uint8_t NoteStack_RecentVelocity(NoteStack *stack) {
  return stack->pool[stack->root_ptr].velocity;
}

void NoteStack_NoteOn(uint8_t note, uint8_t velocity, NoteStack *stack) {
  // Remove the note from the list first (in case it is already here).
  NoteStack_NoteOff(note, stack);
  // In case of saturation, remove the least recently played note from the
  // stack.
  if (stack->size == kNOTE_STACK_CAPACITY) {
    uint8_t least_recent_note;
    for (uint8_t i = 1; i <= kNOTE_STACK_CAPACITY; ++i) {
      if (stack->pool[i].next_ptr == 0) {
        least_recent_note = stack->pool[i].note;
      }
    }
    NoteStack_NoteOff(least_recent_note, stack);
  }
  // Now we are ready to insert the new note. Find a free slot to insert it.
  uint8_t free_slot;
  for (uint8_t i = 1; i <= kNOTE_STACK_CAPACITY; ++i) {
    if (stack->pool[i].note == kFREE_SLOT) {
      free_slot = i;
      break;
    }
  }
  stack->pool[free_slot].next_ptr = stack->root_ptr;
  stack->pool[free_slot].note = note;
  stack->pool[free_slot].velocity = velocity;
  stack->root_ptr = free_slot;

  // The last step consists in inserting the note in the sorted list.
  for (uint8_t i = 0; i < stack->size; ++i) {
    if (stack->pool[stack->sorted_ptr[i]].note > note) {
      for (uint8_t j = stack->size; j > i; --j) {
        stack->sorted_ptr[j] = stack->sorted_ptr[j - 1];
      }
      stack->sorted_ptr[i] = free_slot;
      free_slot = 0;
      break;
    }
  }
  if (free_slot) {
    stack->sorted_ptr[stack->size] = free_slot;
  }
  stack->size++;
}

void NoteStack_NoteOff(uint8_t note, NoteStack *stack) {
  uint8_t current = stack->root_ptr;
  uint8_t previous = 0;
  while (current) {
    if (stack->pool[current].note == note) {
      break;
    }
    previous = current;
    current = stack->pool[current].next_ptr;
  }
  if (current) {
    if (previous) {
      stack->pool[previous].next_ptr = stack->pool[current].next_ptr;
    } else {
      stack->root_ptr = stack->pool[current].next_ptr;
    }
    for (uint8_t i = 0; i < stack->size; ++i) {
      if (stack->sorted_ptr[i] == current) {
        for (uint8_t j = i; j < stack->size - 1; ++j) {
          stack->sorted_ptr[j] = stack->sorted_ptr[j + 1];
        }
        break;
      }
    }
    stack->pool[current].next_ptr = 0;
    stack->pool[current].note = kFREE_SLOT;
    stack->pool[current].velocity = 0;
    stack->size--;
  }
}

void NoteStack_Clear(NoteStack *stack) {
  stack->size = 0;
  stack->root_ptr = 0;
  for (uint8_t i = 0; i <= kNOTE_STACK_CAPACITY; ++i) {
    stack->pool[i].note = kFREE_SLOT;
  }
}

// const NoteEntry& most_recent_note(NoteStack *stack) const { return pool_[root_ptr_]; }
// const NoteEntry& least_recent_note(NoteStack *stack) const {
//   uint8_t current = root_ptr_;
//   while (current && pool_[current].next_ptr) {
//     current = pool_[current].next_ptr;
//   }
//   return pool_[current];
// }
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