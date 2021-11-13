/**
  ******************************************************************************
  * @file           : usbd_midi_if.h
  * @brief          : Header for usbd_midi_if file.
  ******************************************************************************
*/

#ifndef USBD_MIDI_IF_H_
#define USBD_MIDI_IF_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_midi.h"
#include "usbd_desc.h"
#include "ringbuffer.h"

extern USBD_MIDI_ItfTypeDef  USBD_Interface_fops_FS;

extern uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length);

// Remember to call this function to actually send the packet
extern void USBD_MIDI_SendPacket(void);

#ifdef __cplusplus
}
#endif
  
#endif /* USBD_MIDI_IF_H_ */
