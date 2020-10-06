/* Functional Description: Generic ring buffer library for deeply
   embedded system. See the unit tests for usage examples. */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <stdint.h>
#include <stdbool.h>

// RINGBUFFER_SIZE Should be a power of two
#define RINGBUFFER_SIZE 4096
 
typedef struct {
    uint32_t data[RINGBUFFER_SIZE]; /* block of memory or array of data */
    uint16_t tail;
    uint16_t head;
    uint16_t max; 
    bool full;

} RingBuffer_t;

RingBuffer_t midibuffer;

bool RingbufferPop(RingBuffer_t *b, uint32_t *data);
void RingbufferPut(RingBuffer_t * b, uint32_t const *data);
void RingbufferInit(RingBuffer_t *b);

#endif // RINGBUFFER_H_