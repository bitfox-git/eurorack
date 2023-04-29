#include "ssd1306.h"
#include "process.h"
#include "configuration.h"
#include "math.h"
#include "stdio.h"





void display_yin_trio(Process_t *process) {
  uint8_t add_x = 0;
  switch (process->channel) {
    case 0:
      ssd1306_FillPart(0,43,0,8,Black);
      ssd1306_DrawLinePulse(42,0,42,63,4,Black);
      ssd1306_DrawLinePulse(85,0,85,63,4,Black);
      break;
    case 1:
      ssd1306_FillPart(43,85,0,8,Black);
      add_x = 44;
      break;
    case 2:
      ssd1306_FillPart(86,128,0,8,Black);
      add_x = 87;
      break;
  }

  if(process->pitch == -1) {
    ssd1306_UpdateScreen();
    return;
  }

  char charconv[20];
  gcvt(process->pitch, 4, charconv);
  ssd1306_WriteString(charconv, Font_7x10, White,add_x+2,0);

  int totalcent = log2f(process->pitch/440.0f)*12000.0f;
  int notecent = (((totalcent+500) % 12000) + 12000) % 12000;

  // itoa(centlookup,charconv,10);
  char notenamelookup[12][2] = {"A ","Bb","B ","C ","Db","D ","Eb","E ","F ","Gb","G ","Ab"};
  ssd1306_SetCursor(add_x,16);
  ssd1306_WriteChar(notenamelookup[notecent/1000][0], Font_16x26, White);
  ssd1306_SetCursor(ssd1306_GetX(),(16+8));
  ssd1306_WriteChar(notenamelookup[notecent/1000][1], Font_11x18, White);

  int notediff = (notecent-500) % 1000;
  uint8_t x = 0;
  if(notediff > 500) {
    x = add_x+40-(1000 - notediff)/13;
    ssd1306_DrawLine(40+add_x,60,x,60,White);
    ssd1306_DrawLine(40+add_x,61,x,61,White);
    ssd1306_DrawLine(40+add_x,62,x,62,White);
    ssd1306_DrawLine(40+add_x,63,x,63,White);
  } else {
    x = add_x+notediff/13;
    ssd1306_DrawLine(add_x,60,x,60,White);
    ssd1306_DrawLine(add_x,61,x,61,White);
    ssd1306_DrawLine(add_x,62,x,62,White);
    ssd1306_DrawLine(add_x,63,x,63,White);
  }
  
  char centchar[6];
  itoa( (notecent%1000)-500, centchar, 10 );
  ssd1306_WriteString(centchar, Font_7x10, White, add_x+6,53);

  ssd1306_UpdateScreen();
}

void display_yin_duo(Process_t *process) {
  uint8_t add_x = 0;
  switch (config_active_channel) {
    case CONFIG_CHANNEL_DUO_1_2:
    case CONFIG_CHANNEL_DUO_1_3:
      if(process->channel == 0) {
        // display_yin_duo_left(process);
        ssd1306_FillPart(0,64,0,8,Black);
        ssd1306_DrawLinePulse(63,0,63,63,4,Black);
      } else {
        add_x = 66;
        ssd1306_FillPart(64,128,0,8,Black);

        // display_yin_duo_right(process);
      }
      
      break;
    case CONFIG_CHANNEL_DUO_2_3:
      if(process->channel == 1) {
        ssd1306_FillPart(0,64,0,8,Black);
        ssd1306_DrawLinePulse(63,0,63,63,4,Black);
        // display_yin_duo_left(process);
      } else {
        add_x = 66;
        ssd1306_FillPart(64,128,0,8,Black);
        // display_yin_duo_right(process);
      }
      break;
  }

  if(process->pitch == -1) {
    ssd1306_UpdateScreen();
    return;
  }

  char charconv[20];
  gcvt(process->pitch, 4, charconv);
  ssd1306_WriteString(charconv, Font_7x10, White,add_x,0);

  int totalcent = log2f(process->pitch/440.0f)*12000.0f;
  int notecent = (((totalcent+500) % 12000) + 12000) % 12000;

  // itoa(centlookup,charconv,10);
  char notenamelookup[12][2] = {"A ","Bb","B ","C ","Db","D ","Eb","E ","F ","Gb","G ","Ab"};
  ssd1306_SetCursor(add_x+8,16);
  ssd1306_WriteChar(notenamelookup[notecent/1000][0], Font_16x26, White);
  ssd1306_SetCursor(ssd1306_GetX(),(16+8));
  ssd1306_WriteChar(notenamelookup[notecent/1000][1], Font_11x18, White);

  int notediff = (notecent-500) % 1000;
  uint8_t x = 0;
  if(notediff > 500) {
    x = add_x+60-(1000 - notediff)/9;
    ssd1306_DrawLine(add_x+60,60,x,60,White);
    ssd1306_DrawLine(add_x+60,61,x,61,White);
    ssd1306_DrawLine(add_x+60,62,x,62,White);
    ssd1306_DrawLine(add_x+60,63,x,63,White);
  } else {
    x = add_x+notediff/9;
    ssd1306_DrawLine(add_x,60,x,60,White);
    ssd1306_DrawLine(add_x,61,x,61,White);
    ssd1306_DrawLine(add_x,62,x,62,White);
    ssd1306_DrawLine(add_x,63,x,63,White);
  }
  
  char centchar[6];
  itoa( (notecent%1000)-500, centchar, 10 );
  ssd1306_WriteString(centchar, Font_7x10, White, add_x+13,53);

  ssd1306_UpdateScreen();

}

void display_yin_solo(Process_t *process) {
  ssd1306_Fill(Black);

  if(process->pitch == -1) {
    ssd1306_UpdateScreen();
    return;
  }
  
  char charconv[20];
  gcvt(process->pitch, 4, charconv);
  ssd1306_WriteString(charconv, Font_7x10, White,0,0);

  int totalcent = log2f(process->pitch/440.0f)*12000.0f;
  int notecent = (((totalcent+500) % 12000) + 12000) % 12000;

  

  // itoa(centlookup,charconv,10);
  char notenamelookup[12][2] = {"A ","Bb","B ","C ","Db","D ","Eb","E ","F ","Gb","G ","Ab"};
  ssd1306_SetCursor(64,0);
  ssd1306_WriteDoubleChar(notenamelookup[notecent/1000][0], Font_16x26, White);
  ssd1306_SetCursor(ssd1306_GetX(),(52-12));
  ssd1306_WriteChar(notenamelookup[notecent/1000][1], Font_16x26, White);

  int notediff = (notecent-500) % 1000;
  uint8_t leftx = 63;
  uint8_t rightx = 64;
  if(notediff > 500) {
    leftx -= (1000 - notediff)/8;
  } else {
    rightx += notediff/8;
  }

  // uint8_t leftx = 63-(notediff / 8);
  // uint8_t rightx = 64+(notediff / 8);

  ssd1306_DrawLine(leftx,60,rightx,60,White);
  ssd1306_DrawLine(leftx,61,rightx,61,White);
  ssd1306_DrawLine(leftx,62,rightx,62,White);
  ssd1306_DrawLine(leftx,63,rightx,63,White);

  
  char centchar[6];
  itoa( (notecent%1000)-500, centchar, 10 );
  ssd1306_WriteString(centchar, Font_7x10, White, 30,53);

  ssd1306_UpdateScreen();
}

void Display_Init() {
  ssd1306_Init();
}

void Display_Update(Process_t *process) {
  switch (Config_GetMode())
  {
    case CONFIG_MODE_PITCH:
      switch (config_active_channel)
      {
        case CONFIG_CHANNEL_DUO_1_2:
        case CONFIG_CHANNEL_DUO_2_3:
        case CONFIG_CHANNEL_DUO_1_3:
          display_yin_duo(process);
          break;
        case CONFIG_CHANNEL_ONE:
        case CONFIG_CHANNEL_TWO:
        case CONFIG_CHANNEL_THREE:
          display_yin_solo(process);
          break;

        case CONFIG_CHANNEL_TRIO:
          display_yin_trio(process);
          break;
      }
      break;
    
    default:
      break;
  }
  

      // for (size_t i = 0; i < 128; i++)
      // {
      // ssd1306_DrawPixel(i,audiobuffer[i]*32+32,White);
      // }
      // ssd1306_UpdateScreen();
      // HAL_Delay(200);
      // ssd1306_Fill(Black);
      
      // uint32_t inittime = HAL_GetTick();
      // float convert = Yin_getPitch(input,1024,0.15,false,decimate);
      // uint32_t endtime = HAL_GetTick();

      

      // ssd1306_Fill(Black);
      
}