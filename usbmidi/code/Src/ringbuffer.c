/* Functional Description: Generic ring buffer library for deeply
   embedded system. See the unit tests for usage examples. */

#include "ringbuffer.h"
#include "stm32f3xx_it.h"

RingBuffer_t midibuffer;

bool RingbufferPop(RingBuffer_t *b, uint32_t *data) {
  if(!(!b->full && (b->head == b->tail))) { // Check if Ringbuffer is empty 
    *data = b->data[b->tail];
    b->full = false;
    b->tail++;
    b->tail %= RINGBUFFER_SIZE;
    return true;
  } 
  return false;
}

void RingbufferPut(RingBuffer_t *b, uint32_t const *data_element) {       
  if(b->full) {
    return;
		// b->tail++;
	}
  b->data[b->head] = *data_element;
	b->head++;
  b->head %= RINGBUFFER_SIZE;
	b->full = (b->head == b->tail);
}

void RingbufferInit(RingBuffer_t *b) {
    if (b) {
        b->head = 0;
        b->tail = 0;
        b->max = RINGBUFFER_SIZE;
        for (uint32_t i = 0; i < RINGBUFFER_SIZE; i++)
        {
          b->data[i] = 0x00000000;
        }
    }
    return;
}