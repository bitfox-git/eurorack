/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "sdadc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stm32f3xx_hal_sdadc.h"
#include "ssd1306.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f373xc.h"

#include "yin.h"
#include "stdio.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define USE_HAL_SDADC_REGISTER_CALLBACKS
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
float audiobuffer[MAXIMUM_SAMPLESIZE];
uint32_t dmainput[MAXIMUM_SAMPLESIZE];
// kiss_fft_cpx testInput[testsize];
// kiss_fft_cpx testOutput[testsize];
uint8_t floatindex;
bool ready;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SDADC1_Init();
  MX_SDADC2_Init();
  MX_SDADC3_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM13_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
  // __HAL_TIM_ENABLE(&htim13);
  __HAL_TIM_ENABLE(&htim4);
  HAL_TIM_PWM_Start_IT(&htim13,TIM_CHANNEL_1);

  // HAL_SDADC_ConfigChannel(&hsdadc2,SDADC_CHANNEL_6,SDADC_CONTINUOUS_CONV_ON);
  // HAL_SDADC_ConfigChannel(&hsdadc3,SDADC_CHANNEL_6,SDADC_CONTINUOUS_CONV_ON);

  ready = false;

  HAL_SDADC_CalibrationStart(&hsdadc1,SDADC_CALIBRATION_SEQ_1);
  HAL_SDADC_PollForCalibEvent(&hsdadc1,HAL_MAX_DELAY);
  // HAL_SDADC_SelectInjectedExtTrigger(&hsdadc1,)
  // HAL_SDADC_InjectedConfigChannel
  // HAL_SDADC_ConfigChannel(&hsdadc1,SDADC_CHANNEL_6,SDADC_CONTINUOUS_CONV_ON);
  // HAL_SDADC_SelectRegularTrigger(&hsdadc1,SDADC_SOFTWARE_TRIGGER);
  HAL_SDADC_InjectedStart_DMA(&hsdadc1, &dmainput,MAXIMUM_SAMPLESIZE*2);

  // uint8_t data = 0;
  // ssd1306_Fill(Black);
  // ssd1306_UpdateScreen();
  // float randommem[4098];

  // float dcx = 0;
  // float dcy = 0;


  // HAL_SDADC_Start(&hsdadc1);
  // HAL_SDADC_PollForConversion(&hsdadc1, HAL_MAX_DELAY);
  // HAL_SDADC_Start_IT()
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(ready == true) {
      for (size_t i = 0; i < MAXIMUM_SAMPLESIZE; i++)
      {
        // audiobuffer[i] = (1/(-32768.0f*2))*((signed short)(dmainput[i]>>16)) ;
        audiobuffer[i] = (1/(-32768.0f*2))*((signed short)(dmainput[i]>>16)) ;
      }
      
      ssd1306_Fill(Black);

      // for (size_t i = 0; i < 128; i++)
      // {
      //   ssd1306_DrawPixel(i,audiobuffer[i]*32+32,White);
      // }
      // ssd1306_UpdateScreen();
      // HAL_Delay(200);
      // ssd1306_Fill(Black);
      
      uint32_t inittime = HAL_GetTick();
      float convert = Yin_getPitch(&audiobuffer,MAXIMUM_SAMPLESIZE,0.15,false);
      uint32_t endtime = HAL_GetTick();



      // ssd1306_Fill(Black);
      char charconv[20];
      gcvt(convert, 5, charconv);
      ssd1306_WriteString(charconv, Font_7x10, White,0,0);
    
      // convert = Yin_getProbability();
      // gcvt(convert, 5, charconv);
      // ssd1306_WriteString(charconv, Font_7x10, White,0,21);

      volatile int totalcent = log2f(convert/440.0f)*12000.0f;
      
      volatile int notecent = (((totalcent+500) % 12000) + 12000) % 12000;



      // itoa(centlookup,charconv,10);
      char notenamelookup[12][2] = {"A ","Bb","B ","C ","Db","D ","Eb","E ","F ","Gb","G ","Ab"};
      ssd1306_SetCursor(0,50);
      ssd1306_WriteChar(notenamelookup[notecent/1000][0], Font_7x10, White);
      ssd1306_WriteChar(notenamelookup[notecent/1000][1], Font_7x10, White);

      ssd1306_UpdateScreen();
      ready = false;
      // HAL_Delay(200);
      HAL_SDADC_InjectedStart_DMA(&hsdadc1, &dmainput,MAXIMUM_SAMPLESIZE*2);
    }



    // for (int i = 0; i < MAXIMUM_SAMPLESIZE; i++)
    // {
    //   HAL_SDADC_Start(&hsdadc1);
    //   HAL_SDADC_PollForConversion(&hsdadc1,HAL_MAX_DELAY);
    //   int number = (signed short)HAL_SDADC_GetValue(&hsdadc1);
    //   float outputfloat = number / 32768.0f;

    //   //DC BLOCKER
    //   float x = outputfloat;
    //   outputfloat = dcy = dcy * 0.995 + x - dcx;
    //   dcx = x;

    //   audiobuffer[i] = outputfloat;
    // }

    

    // // Hardware Test code "_"

    // ssd1306_Fill(Black);
    // if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_SET) {
    // }
    // if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET) {
    // }
    // if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9) == GPIO_PIN_SET) {
    // }

    // data++;
    // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,data);
    // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,data);
    // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,data);
    // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,data);
    // __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,data);
    // __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,data);
    // __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,data);





    

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_SDADC;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.SdadcClockSelection = RCC_SDADCSYSCLK_DIV12;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_PWREx_EnableSDADC(PWR_SDADC_ANALOG1);
  HAL_PWREx_EnableSDADC(PWR_SDADC_ANALOG2);
  HAL_PWREx_EnableSDADC(PWR_SDADC_ANALOG3);
}

/* USER CODE BEGIN 4 */
void HAL_SDADC_InjectedConvCpltCallback(SDADC_HandleTypeDef *hsdadc) {
  ready =true;
  HAL_SDADC_InjectedStop_DMA(&hsdadc1);   
}

void HAL_SDADC_ConvCpltCallback(SDADC_HandleTypeDef *hsdadc) {
  // testInput[floatindex] = HAL_SDADC_GetValue(hsdadc);
  // ready = true;
  // HAL_SDADC_Start_IT(hsdadc);
}

void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  if(htim == &htim13) {
    ready = false;
  }
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_TIM_TriggerCallback could be implemented in the user file
   */
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
