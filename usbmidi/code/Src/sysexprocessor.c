#include "sysexprocessor.h"
#include "dac.h"
#include "main.h"
#include <string.h>
#include "configuration.h"
#include "usb_device.h"
#include "usbd_midi.h"
#include "usbd_midi_if.h"
#include "led.h"

#define kFIRMWARE_VERSION 0x02

// Private Variables
//Sysex
enum SysexEnum{Off, Initialization, MessageType, Configure, Calibrate, Update, EndUpdate, Finished, Failed};
uint8_t sysexstatus = Off;

const uint8_t sysexverify[5] = {0x00,0x46,0x47,0x57,0x00};
uint8_t sysexverifyindex;



// Helper Functions
uint8_t MidiBytestoByte(uint8_t *bufferarray, int *bufferindex) {
  uint8_t temp = bufferarray[*bufferindex] + (bufferarray[*bufferindex + 1] << 7 & 0x80);
  *bufferindex += 2;
  return temp;
}
uint16_t MidiBytestoHalfWord(uint8_t *bufferarray, int *bufferindex) { 
  uint16_t temp = bufferarray[*bufferindex] + (bufferarray[*bufferindex + 1] << 7 & 0x3F80)  + (bufferarray[*bufferindex + 2] << 14 & 0xC000);
  *bufferindex += 3;
  return temp;
}
uint32_t MidiBytestoWord(uint8_t *bufferarray, int *bufferindex) {
  uint32_t temp = bufferarray[*bufferindex] + (bufferarray[*bufferindex + 1] << 7 & 0x00003F80)  + (bufferarray[*bufferindex + 2] << 14 & 0x001FC000) 
                  + (bufferarray[*bufferindex + 3] << 21 & 0x0FE00000)  + (bufferarray[*bufferindex + 4] << 28 & 0xF0000000);
  *bufferindex += 5;
  return temp;
}
float MidiBytestoFloat(uint8_t *bufferarray, int *bufferindex) {
  uint32_t temp = bufferarray[*bufferindex] + (bufferarray[*bufferindex + 1] << 7 & 0x00003F80)  + (bufferarray[*bufferindex + 2] << 14 & 0x001FC000) 
                  + (bufferarray[*bufferindex + 3] << 21 & 0x0FE00000)  + (bufferarray[*bufferindex + 4] << 28 & 0xF0000000);
  *bufferindex += 5;
  float tempf;
  memcpy(&tempf, &temp, sizeof(uint32_t));
  return tempf;
}

void BytetoMidiBytes(uint8_t input, uint8_t *bufferarray, int *bufferindex) {
  bufferarray[*bufferindex] = input & 0x7F;
  *bufferindex += 1;
  bufferarray[*bufferindex] = (input >> 7) & 0x01;
  *bufferindex += 1;
}
void HalfWordtoMidiBytes(uint16_t input, uint8_t *bufferarray, int *bufferindex) {
  bufferarray[*bufferindex] = input & 0x7F;
  *bufferindex += 1;
  bufferarray[*bufferindex] = (input >> 7) & 0x7F;
  *bufferindex += 1;
  bufferarray[*bufferindex] = (input >> 14) & 0x03;
  *bufferindex += 1;
}
void WordtoMidiBytes(uint32_t input, uint8_t *bufferarray, int *bufferindex) {
  for (uint8_t i = 0; i < 4; i++) {
    bufferarray[*bufferindex] = (input >> i*7) & 0x7F;
    *bufferindex += 1;
  }
  bufferarray[*bufferindex] = (input >> 28) & 0x0F;
  *bufferindex += 1;
}
void FloatToMidiBytes(float input, uint8_t *bufferarray, int *bufferindex) {
  uint32_t tempuint;
  memcpy(&tempuint, &input, sizeof(uint32_t));
  for (uint8_t i = 0; i < 4; i++) {
    bufferarray[*bufferindex] = (tempuint >> i*7) & 0x7F;
    *bufferindex += 1;
  }
  bufferarray[*bufferindex] = (tempuint >> 28) & 0x0F;
  *bufferindex += 1;
}



uint8_t sysexflashbuf[5];
int sysexflashbufindex = 0;
void Sysex_FlashUpdate(uint8_t input) {
  sysexflashbuf[sysexflashbufindex] = input;
  sysexflashbufindex++;
  if(sysexflashbufindex >= 5) {
    sysexflashbufindex = 0;
    uint32_t data = MidiBytestoWord(sysexflashbuf,&sysexflashbufindex);
    sysexflashbufindex = 0;
    Config_FlashWriteAppWord(data);
  }
}

uint32_t sysexconfigupdate;
void Sysex_ConfigUpdate(uint8_t input) {
  sysexflashbuf[sysexflashbufindex] = input;
  sysexflashbufindex++;
  if(sysexflashbufindex >= 5) {
    sysexflashbufindex = 0;
    uint32_t data = MidiBytestoWord(sysexflashbuf,&sysexflashbufindex);
    sysexflashbufindex = 0;

    if(sysexconfigupdate%2 == 0) {
      config[sysexconfigupdate/8].analogmode[(sysexconfigupdate/2)%4] = data >> 24;
      config[sysexconfigupdate/8].analogsetting[(sysexconfigupdate/2)%4].midichannel = data >> 16;
      config[sysexconfigupdate/8].analogsetting[(sysexconfigupdate/2)%4].byte2 = data >> 8;
      config[sysexconfigupdate/8].analogsetting[(sysexconfigupdate/2)%4].byte3 = data >> 0;
    } else {
      config[sysexconfigupdate/8].digitalmode[(sysexconfigupdate/2)%4] = data >> 24;
      config[sysexconfigupdate/8].digitalsetting[(sysexconfigupdate/2)%4].midichannel = data >> 16;
      config[sysexconfigupdate/8].digitalsetting[(sysexconfigupdate/2)%4].byte2 = data >> 8;
      config[sysexconfigupdate/8].digitalsetting[(sysexconfigupdate/2)%4].byte3 = data >> 0;
    }

    sysexconfigupdate += 1;
  }
}

void Sysex_CalibrationUpdate(uint8_t input) {
  sysexflashbuf[sysexflashbufindex] = input;
  sysexflashbufindex++;
  if(sysexflashbufindex >= 5) {
    sysexflashbufindex = 0;
    volatile float data = MidiBytestoFloat(sysexflashbuf,&sysexflashbufindex);
    sysexflashbufindex = 0;

    if(sysexconfigupdate%2 == 0) {
      dac.voltlow[sysexconfigupdate/2] = data;
    } else {
      dac.volthigh[sysexconfigupdate/2] = data;
    }

    sysexconfigupdate += 1;
    if(sysexconfigupdate == 8) {
      DAC_Refresh();
    }
  }
}

void Sysex_ConfigCalibInit() {
  sysexflashbufindex = 0;
  sysexconfigupdate = 0;
}

inline bool Sysex_UsbTimeout() {
  uint32_t timeouttick = HAL_GetTick();
  while(USB_Tx_State) {
    if(HAL_GetTick() - timeouttick > 3) {
      return false;
    }
  }
  return true;
}

bool Sysex_VerifyApp() {
  for (size_t i = 0; i < 15; i++)
  {
    LED_UploadIncrement();
    uint32_t flashaddress = kUPDATE_ADDRESS + i*0x2000;
    uint32_t maxflash = kUPDATE_ADDRESS + i*0x2000 + 0x2000;

    uint8_t verifyoutput[15];
    int verifyindex = 0;
    //Sending First 6 bytes
    uint32_t data = Config_ReadWord(&flashaddress);
    verifyoutput[verifyindex] = 0xF0;
    verifyindex++;
    WordtoMidiBytes(data, verifyoutput, &verifyindex);
    
    Sysex_SendMidiMessage(0x04, verifyoutput[0], verifyoutput[1], verifyoutput[2]);
    if(Sysex_UsbTimeout() == false) 
      return false;
    Sysex_SendMidiMessage(0x04, verifyoutput[3], verifyoutput[4], verifyoutput[5]);
    if(Sysex_UsbTimeout() == false) 
      return false;

    while (flashaddress < maxflash)
    {
      verifyindex = 0;
      for (size_t i = 0; i < 3; i++) {
        if(flashaddress >= maxflash) {
          break;
        }
        data = Config_ReadWord(&flashaddress);
        WordtoMidiBytes(data,verifyoutput,&verifyindex);
      }
      for (size_t i = 0; i < 5; i++) {
        if(Sysex_UsbTimeout() == false) {
          return false;
        }
        Sysex_SendMidiMessage(0x04, verifyoutput[i*3], verifyoutput[i*3+1], verifyoutput[i*3+2]);
      }  
    }
    if(Sysex_UsbTimeout() == false){
      return false;
    }
    Sysex_SendMidiMessage(0x07,0x00,0x00,0xF7);
    HAL_Delay(2000);
  }
  

  return true;
}


void Sysex_Process(uint8_t input) {
  // leddriver.ChangeMode(leddriver.Connected);
  
  if(input == 0xF0) {
    if(sysexstatus == Off) {
      sysexstatus = Initialization;
      sysexverifyindex = 0;
    } else {
      sysexstatus = Failed;
    }
  } else if(input == 0xF7) { // Could also check for sysexstatus_ == finished for correct upload of message
    if(sysexstatus == EndUpdate) {
      Config_FlashEndWriteApp();
      LED_SwitchUpload();
      Sysex_VerifyApp();

    }
    if(sysexstatus == Configure || sysexstatus == Calibrate) {
      Config_Configure();
    }
    sysexstatus = Off;
  } 
  else if(input <= 127) 
  {
    switch(sysexstatus) {
      case Initialization:
        if(sysexverify[sysexverifyindex] == input) {
          sysexverifyindex++;
          if(sysexverifyindex == 5) {
            sysexstatus = MessageType;
          }
        } else {
          sysexstatus = Failed;
        }
        break;

      case MessageType:
        if(input == 0x00) { // Am i alive ?
          Sysex_SendMidiMessage(0x04,0xF0,kFIRMWARE_VERSION,0xF7);
          sysexstatus = Finished;
        }
        if(input == 0x74) { // ConfigurePush
          Config_WriteUserData();
          sysexstatus = Finished;
        }
        if(input == 0x75) { // ConfigurePush
          Sysex_ConfigCalibInit();
          Config_DisableOutput();
          sysexstatus = Configure;
        }
        if(input == 0x76) { // CalibratePush
          Sysex_ConfigCalibInit();
          sysexstatus = Calibrate;
        }
        if(input == 0x77) { 
          Config_FlashSetAppBit();
          sysexstatus = Finished;
        }
        if(input == 0x78) {
          sysexstatus = EndUpdate;
        }
        if(input == 0x79) {
          LED_UploadIncrement();
          sysexstatus = Update;
        }
        if(input == 0x7A) { // Write To Start Update App
          Config_FlashInitWriteApp();
          LED_UploadIncrement();
          sysexstatus = Update;
        }
        if(input == 0x7B) { // Load Current Program
          Config_Configure();
          sysexstatus = Finished;
        }
        if(input == 0x7C) { // Configure outputs off, set dac value to low
          Sysex_CalibrationLow();
          sysexstatus = Finished;
        }
        if(input == 0x7D) { // Configure outputs off, set dac value to low
          Sysex_CalibrationHigh();
          sysexstatus = Finished;
        }
        if(input == 0x7E ) { // Request Configuration Data
          Sysex_DumpConfiguration();
          sysexstatus = Finished;
        }
        if(input == 0x7F) { // Request Calibration Data
          Sysex_DumpCalibration();
          sysexstatus = Finished;
        }
        break;
      
      case Update:
      case EndUpdate:
        Sysex_FlashUpdate(input);
        break;

      case Calibrate:
        Sysex_CalibrationUpdate(input);
        break;
        
      case Configure:
        Sysex_ConfigUpdate(input);
        break;

      default:
        break;   
    }
  }
}

void Sysex_CalibrationLow() {
  Config_DisableOutput();
  LED_ChangeMode(LED_Fixed);
  LED_Color(255,0);
  for (size_t i = 0; i < 4; i++) {
    dac.values[i] = kDAC_CALIB_LOW;
  }
}

void Sysex_CalibrationHigh() {
  Config_DisableOutput();
  LED_ChangeMode(LED_Fixed);
  LED_Color(0,255);
  for (size_t i = 0; i < 4; i++) {
    dac.values[i] = kDAC_CALIB_HIGH;
  }
}

bool Sysex_DumpConfiguration() {
  uint8_t bufferoutput[40*16+2];
  int bufferindex = 0;

  bufferoutput[bufferindex] = 0xF0;
  bufferindex++;
  for (size_t i = 0; i < 16; i++)  {
    for (uint8_t j = 0; j < 4; j++) {
      uint8_t a = config[i].analogmode[j];
      uint8_t b = config[i].analogsetting[j].midichannel;
      uint8_t c = config[i].analogsetting[j].byte2;
      uint8_t d = config[i].analogsetting[j].byte3;
      uint32_t writeword = (a << 24 & 0xFF000000) | (b << 16 & 0x00FF0000) | (c << 8 & 0x0000FF00) | (d << 0 & 0x000000FF);
      WordtoMidiBytes(writeword,bufferoutput,&bufferindex);

      a = config[i].digitalmode[j];
      b = config[i].digitalsetting[j].midichannel;
      c = config[i].digitalsetting[j].byte2;
      d = config[i].digitalsetting[j].byte3;
      writeword = (a << 24 & 0xFF000000) | (b << 16 & 0x00FF0000) | (c << 8 & 0x0000FF00) | (d << 0 & 0x000000FF);
      WordtoMidiBytes(writeword,bufferoutput,&bufferindex);
    }
  }
  bufferoutput[bufferindex] = 0xF7;
  Sysex_SendMidiMessage(0x04, bufferoutput[0*3], bufferoutput[0*3+1], bufferoutput[0*3+2]);

  for (size_t i = 1; i < 213; i++) {
    Sysex_SendMidiMessage(0x04, bufferoutput[i*3], bufferoutput[i*3+1], bufferoutput[i*3+2]);  
    if(Sysex_UsbTimeout() == false) {
      return false;
    }
  }

  
  Sysex_SendMidiMessage(0x07, bufferoutput[639], bufferoutput[640], bufferoutput[641]);
  return true;
}



bool Sysex_DumpCalibration() {
  uint8_t bufferoutput[40+2];
  int bufferindex = 0;

  bufferoutput[bufferindex] = 0xF0;
  bufferindex++;
  for (size_t i = 0; i < 4; i++) {
    FloatToMidiBytes(dac.voltlow[i], bufferoutput, &bufferindex);
    FloatToMidiBytes(dac.volthigh[i], bufferoutput, &bufferindex);
  }
  bufferoutput[bufferindex] = 0xF7;

  for (size_t i = 0; i < 13; i++)
  {
    Sysex_SendMidiMessage(0x04, bufferoutput[i*3], bufferoutput[i*3+1], bufferoutput[i*3+2]);
    if(Sysex_UsbTimeout() == false) {
      return false;
    }
  }  
  Sysex_SendMidiMessage(0x07, bufferoutput[39], bufferoutput[40], bufferoutput[41]);
  return true;
}

//this function does not do any parameter checking and sends raw data.
void Sysex_SendMidiMessage(uint8_t usbmessagetype, uint8_t byte1, uint8_t byte2, uint8_t byte3) {
  if(hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
    uint8_t buffer[4];
    buffer[0] = usbmessagetype;
    buffer[1] = byte1;
    buffer[2] = byte2;
    buffer[3] = byte3;
    MIDI_DataTx(buffer, 4);
    USBD_MIDI_SendPacket();
  }
}