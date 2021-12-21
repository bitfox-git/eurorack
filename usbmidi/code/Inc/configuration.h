#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "stdint.h"
#include "stdbool.h"
#include "midisetting.h"

#define kUSER_ADDRESS 0x08003000
#define kCALIB_ADDRESS kUSER_ADDRESS + 4
#define kCONFIG_ADDRESS (kCALIB_ADDRESS + sizeof(float)*8)

#define kBIT_ADDRESS 0x08002800
#define kUPDATE_ADDRESS 0x08022000

typedef struct {
  uint8_t analogmode[4];
  MidiSetting analogsetting[4];

  uint8_t digitalmode[4];
  MidiSetting digitalsetting[4];

}Config_t;

extern Config_t config[16];
extern uint8_t configindex;



bool Config_ReadCalibrate();
void Config_WriteCalibrate();


void Config_DisableOutput();

void Config_Configure();
void Config_ProgramChange(uint8_t index);
void Config_Empty();

void Config_FlashInitWriteApp();
void Config_FlashWriteAppWord(uint32_t data);
void Config_FlashEndWriteApp();
void Config_FlashSetAppBit();
void Config_FlashResetAppBit();
uint32_t Config_ReadWord(uint32_t *flashaddress);

void Config_WriteUserData();

void Config_Init();

#endif // CONFIGURATION_H_