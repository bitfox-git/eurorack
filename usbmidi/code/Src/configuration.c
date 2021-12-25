#include "configuration.h"
#include "digital.h"
#include "analog.h"
#include "dac.h"
#include "led.h"
#include "stm32f3xx_hal.h"
#include <string.h>

Config_t config[16];
uint8_t configindex;

void Config_DisableOutput() {
  for (size_t i = 0; i < 4; i++)
  {
    Digital_Configure(&digital[i],DIGITAL_OFF,&config[configindex].digitalsetting[i]);
    Analog_Configure(&analog[i],ANALOG_OFF,&config[configindex].analogsetting[i]);
  }
}

void Config_EraseBitPage() {
  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef ferase;
  ferase.TypeErase = FLASH_TYPEERASE_PAGES;
  ferase.PageAddress = kBIT_ADDRESS;
  ferase.NbPages = 1;

  uint32_t pageerror = 0;

  HAL_FLASHEx_Erase(&ferase,&pageerror);

  HAL_FLASH_Lock();
}

void Config_EraseUserPage() {
  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef ferase;
  ferase.TypeErase = FLASH_TYPEERASE_PAGES;
  ferase.PageAddress = kUSER_ADDRESS;
  ferase.NbPages = 2;

  uint32_t pageerror = 0;

  HAL_FLASHEx_Erase(&ferase,&pageerror);

  HAL_FLASH_Lock();
}

void Config_EraseUpdatePage() {
  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef ferase;
  ferase.TypeErase = FLASH_TYPEERASE_PAGES;
  ferase.PageAddress = kUPDATE_ADDRESS;
  ferase.NbPages = 60;

  uint32_t pageerror = 0;

  HAL_FLASHEx_Erase(&ferase,&pageerror);

  HAL_FLASH_Lock();
}

void Config_FlashWord(uint32_t *Address, uint32_t Data) {
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, *Address, Data);
  *Address += 4;
}

void Config_FlashFloat(uint32_t *Address, float Data) {
  uint32_t translate;
  memcpy(&translate,&Data,4);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, *Address, translate);
  *Address += 4;
}

void Config_Configure() {
  LED_ChangeMode(LED_Preset);
  for (uint8_t i = 0; i < 4; i++) {
    Digital_Configure(&digital[i],config[configindex].digitalmode[i],&config[configindex].digitalsetting[i]);
    Analog_Configure(&analog[i],config[configindex].analogmode[i],&config[configindex].analogsetting[i]);
  }
}

void Config_Empty() {
  MidiSetting randomsetting;
  randomsetting.byte2 = 255;
  randomsetting.byte3 = 255;
  randomsetting.midichannel = 255;

  for (uint8_t i = 0; i < 4; i++) {
    Digital_Configure(&digital[i],DIGITAL_OFF,&randomsetting);
    Analog_Configure(&analog[i],ANALOG_OFF,&randomsetting);
  }
}

void Config_ProgramChange(uint8_t index) {
  if(index < 16) {
    configindex = index;
    Config_Configure();
  }
}

inline uint32_t Config_ReadWord(uint32_t *flashaddress) {
  uint32_t data = *(__IO uint32_t*)(*flashaddress);
  *flashaddress += 4;
  return data;
}

bool Config_ReadCalibrate() {
  uint32_t flashaddress = kUSER_ADDRESS;
  if(flashaddress == *(__IO uint32_t*)flashaddress) {
    flashaddress = kCALIB_ADDRESS;
    for (uint8_t i = 0; i < 4; i++)
    {
      dac.voltlow[i] = *(__IO float*)flashaddress;
      flashaddress += 4;
      dac.volthigh[i] = *(__IO float*)flashaddress;
      flashaddress += 4;
    }
    return true;
  } else {
    for (uint8_t i = 0; i < 4; i++) {
      dac.voltlow[i] = 8.19; // Calculated Constants from OPAMP resistors etc.
      dac.volthigh[i] = -5.69;
    }
    return false;
  }
}

bool Config_ReadConfig() {
  uint32_t flashaddress = kUSER_ADDRESS;
  if(flashaddress == *(__IO uint32_t*)flashaddress) {
    flashaddress = kCONFIG_ADDRESS;
    for (size_t i = 0; i < 16; i++)
    {
      for (uint8_t j = 0; j < 4; j++)
      {
        uint32_t data = Config_ReadWord(&flashaddress);
        config[i].analogmode[j] = data >> 24 & 0xFF;
        config[i].analogsetting[j].midichannel = data >> 16 & 0xFF;
        config[i].analogsetting[j].byte2 = data >> 8 & 0xFF;
        config[i].analogsetting[j].byte3 = data >> 0 & 0xFF;
        
        data = Config_ReadWord(&flashaddress);
        config[i].digitalmode[j] = data >> 24 & 0xFF;
        config[i].digitalsetting[j].midichannel = data >> 16 & 0xFF;
        config[i].digitalsetting[j].byte2 = data >> 8 & 0xFF;
        config[i].digitalsetting[j].byte3 = data >> 0 & 0xFF;

      }
    }  
    return true;

  } else {

    for (size_t i = 0; i < 16; i++) {
      for (size_t j = 0; j < 4; j++) {
        config[i].analogmode[j] = ANALOG_OFF;
        config[i].analogsetting[j].midichannel = 0;
        config[i].analogsetting[j].byte2 = 0;
        config[i].analogsetting[j].byte3 = 0;

        config[i].digitalmode[j] = DIGITAL_OFF;
        config[i].digitalsetting[j].midichannel = 0;
        config[i].digitalsetting[j].byte2 = 0;
        config[i].digitalsetting[j].byte3 = 0;
      }
    }
    return false;
  }
}

void Config_WriteUserData() { // Maybe make this write User stuffs ?
  Config_EraseUserPage();

  HAL_FLASH_Unlock();
  uint32_t flashaddress = kUSER_ADDRESS;
  Config_FlashWord(&flashaddress, flashaddress);
  for (size_t i = 0; i < 4; i++)
  {
    Config_FlashFloat(&flashaddress, dac.voltlow[i]);
    Config_FlashFloat(&flashaddress, dac.volthigh[i]);
  }
  if(flashaddress == kCONFIG_ADDRESS) { // check if deze grapjes kloppen
    flashaddress = kCONFIG_ADDRESS; 
  }
  for (size_t i = 0; i < 16; i++)
    {
      for (uint8_t j = 0; j < 4; j++)
      {
        uint8_t a = config[i].analogmode[j];
        uint8_t b = config[i].analogsetting[j].midichannel;
        uint8_t c = config[i].analogsetting[j].byte2;
        uint8_t d = config[i].analogsetting[j].byte3;
        uint32_t writeword = (a << 24 & 0xFF000000) | (b << 16 & 0x00FF0000) | (c << 24 & 0x0000FF00) | (d << 0 & 0x000000FF);
        Config_FlashWord(&flashaddress, writeword);
        
        a = config[i].digitalmode[j];
        b = config[i].digitalsetting[j].midichannel;
        c = config[i].digitalsetting[j].byte2;
        d = config[i].digitalsetting[j].byte3;
        writeword = (a << 24 & 0xFF000000) | (b << 16 & 0x00FF0000) | (c << 24 & 0x0000FF00) | (d << 0 & 0x000000FF);
        Config_FlashWord(&flashaddress, writeword);

      }
    } 
  HAL_FLASH_Lock();
}

uint32_t appflashaddress;
void Config_FlashInitWriteApp() {
  Config_DisableOutput();
  LED_ChangeMode(LED_Uploading);
  Config_EraseUpdatePage();
  HAL_FLASH_Unlock();
  appflashaddress = kUPDATE_ADDRESS;
}

void Config_FlashWriteAppWord(uint32_t data) {
  Config_FlashWord(&appflashaddress, data);
}

void Config_FlashEndWriteApp() {
  HAL_FLASH_Lock();
}

uint32_t Config_FlashVerifyWord(uint32_t *flashaddress) {
  return Config_ReadWord(flashaddress);
}

void Config_FlashResetAppBit() {
  uint32_t appbit = kBIT_ADDRESS;
  if(Config_ReadWord(&appbit) != 0xFFFFFFFF) {
    Config_EraseBitPage();
  }
  appbit = kUPDATE_ADDRESS;
  if(Config_ReadWord(&appbit) != 0xFFFFFFFF) {
    Config_EraseUpdatePage();
  }
}

void Config_FlashSetAppBit() {
  HAL_FLASH_Unlock();
  uint32_t appbit = kBIT_ADDRESS;
  Config_FlashWord(&appbit,appbit);
  HAL_FLASH_Lock();
  NVIC_SystemReset();
}

void Config_Init() {
  if(Config_ReadConfig() == false){
    // LED_ChangeMode(LED_NoConfig);
  }
  Config_ProgramChange(0);
}