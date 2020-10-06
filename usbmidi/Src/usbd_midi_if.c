/**
  ******************************************************************************
  * @file           : usbd_midi_if.c
  * @brief          :
  ******************************************************************************

    (CC at)2016 by D.F.Mac. @TripArts Music

*/

/* Includes ------------------------------------------------------------------*/
#include "usbd_midi_if.h"
#include "stm32f3xx_hal.h"

// basic midi rx/tx functions
static uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length);
uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length);


USBD_MIDI_ItfTypeDef USBD_Interface_fops_FS =
{
  MIDI_DataRx,
  MIDI_DataTx
};

static uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length){
  uint16_t cnt;
  uint16_t msgs = length / 4;
  uint16_t chk = length % 4;
  if(chk == 0){
    for(cnt = 0;cnt < msgs;cnt ++){
      RingbufferPut(&midibuffer,((uint32_t *)msg)+cnt);
    }
  }
  return 0;
}

uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length){
  if(length == 4) {
    uint32_t i = 0;
    while (i < length) {
      APP_Rx_Buffer[APP_Rx_ptr_in] = *(msg + i);
      APP_Rx_ptr_in++;
      i++;
      if (APP_Rx_ptr_in == APP_RX_DATA_SIZE) {
        APP_Rx_ptr_in = 0;
      }
    }
    return USBD_OK;
  }
  else {
    return USBD_FAIL;
  }
}