
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "spi.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <math.h>

#include "lcd5110.h"
#include "stm32f3_discovery_accelerometer.h"
#include "stm32f3_discovery_gyroscope.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
LCD5110_display lcd1;
LCD5110_display lcd2;
//LCD5110_display lcd3;

void print_lcd(LCD5110_display* lcd, char msg[]) {
  LCD5110_clear_scr(lcd);
  LCD5110_set_cursor(2, 2, lcd);
  LCD5110_print(msg, BLACK, lcd);
  LCD5110_refresh(lcd);
}

typedef struct {
    double x;
    double y;
    double z;
} POINT_3D;

typedef struct {
    POINT_3D point;
    double length;
    double max_length;
    POINT_3D max_point;
} DATA_VECTOR;

void data_vector_count_length(DATA_VECTOR* data_vector) {
    double x = data_vector->point.x;
    double y = data_vector->point.y;
    double z = data_vector->point.z;
    double new_length = sqrt(x*x + y*y + z*z);
    if (new_length > data_vector->max_length) {
        data_vector->max_length = new_length;
        data_vector->max_point = data_vector->point;
    }
    data_vector->length = new_length;
}

DATA_VECTOR accelero_data;
DATA_VECTOR gyro_data;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_SPI2_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  lcd1.hw_conf.spi_handle = &hspi2;
  lcd1.hw_conf.spi_cs_pin = LCD1_CS_Pin;
  lcd1.hw_conf.spi_cs_port = LCD1_CS_GPIO_Port;
  lcd1.hw_conf.rst_pin = LCD1_RST_Pin;
  lcd1.hw_conf.rst_port = LCD1_RST_GPIO_Port;
  lcd1.hw_conf.dc_pin = LCD1_DC_Pin;
  lcd1.hw_conf.dc_port = LCD1_DC_GPIO_Port;
  lcd1.def_scr = lcd5110_def_scr;

  LCD5110_init(&lcd1.hw_conf, LCD5110_INVERTED_MODE, 0x40, 2, 3);

  lcd2.hw_conf.spi_handle = &hspi3;
  lcd2.hw_conf.spi_cs_pin = LCD2_CS_Pin;
  lcd2.hw_conf.spi_cs_port = LCD2_CS_GPIO_Port;
  lcd2.hw_conf.rst_pin = LCD2_RST_Pin;
  lcd2.hw_conf.rst_port = LCD2_RST_GPIO_Port;
  lcd2.hw_conf.dc_pin = LCD2_DC_Pin;
  lcd2.hw_conf.dc_port = LCD2_DC_GPIO_Port;
  lcd2.def_scr = lcd5110_def_scr;

  LCD5110_init(&lcd2.hw_conf, LCD5110_INVERTED_MODE, 0x40, 2, 3);

//  lcd3.hw_conf.spi_handle = &hspi1;
//  lcd3.hw_conf.spi_cs_pin = LCD3_CS_Pin;
//  lcd3.hw_conf.spi_cs_port = LCD3_CS_GPIO_Port;
//  lcd3.hw_conf.rst_pin = LCD3_RST_Pin;
//  lcd3.hw_conf.rst_port = LCD3_RST_GPIO_Port;
//  lcd3.hw_conf.dc_pin = LCD3_DC_Pin;
//  lcd3.hw_conf.dc_port = LCD3_DC_GPIO_Port;
//  lcd3.def_scr = lcd5110_def_scr;
//
//  LCD5110_init(&lcd3.hw_conf, LCD5110_INVERTED_MODE, 0x40, 2, 3);

  accelero_data.max_length = -INFINITY;
  gyro_data.max_length = -INFINITY;

  if (BSP_ACCELERO_Init() != HAL_OK)
  {
      /* Initialization Error */
      print_lcd(&lcd1, "Error initializing HAL.");
      while(1){}
  }
  if (BSP_GYRO_Init() != HAL_OK) {
      print_lcd(&lcd2, "Error initializing HAL.");
      while(1){}
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int16_t buffer_accelero[3] = {0};
  float buffer_gyroscope[3] = {0};
//  print_lcd(&lcd2, "sosi pisos");
  while (1)
  {
//  read data from accelerometer
      BSP_ACCELERO_GetXYZ(buffer_accelero);
      accelero_data.point.x = (double)(buffer_accelero[0] >> 4) / 1000.0;
      accelero_data.point.y = (double)(buffer_accelero[1] >> 4) / 1000.0;
      accelero_data.point.z = (double)(buffer_accelero[2] >> 4) / 1000.0;

      data_vector_count_length(&accelero_data);

//  print it
      LCD5110_printf(&lcd1, BLACK, "cur:    max:\n"
                                  "%7.3f %6.3f\n"
                                  "%7.3f %6.3f\n"
                                  "%7.3f %6.3f\n"
                                  "len: %g\n",
                     accelero_data.point.x,
                     accelero_data.max_point.x,
                     accelero_data.point.y,
                     accelero_data.max_point.y,
                     accelero_data.point.z,
                     accelero_data.max_point.z,
                     accelero_data.length
      );


//  read data from gyroscope
      BSP_GYRO_GetXYZ(buffer_gyroscope);
      gyro_data.point.x = (double)(buffer_gyroscope[0]) * 8.75 / 1000;
      gyro_data.point.y = (double)(buffer_gyroscope[1]) * 8.75 / 1000;
      gyro_data.point.z = (double)(buffer_gyroscope[2]) * 8.75 / 1000;

      data_vector_count_length(&gyro_data);

//  print it
      LCD5110_printf(&lcd2, BLACK, "cur:    max:\n"
                                   "%6.1f %6.1f\n"
                                   "%6.1f %6.1f\n"
                                   "%6.1f %6.1f\n"
                                   "len: %g\n",
                     gyro_data.point.x,
                     gyro_data.max_point.x,
                     gyro_data.point.y,
                     gyro_data.max_point.y,
                     gyro_data.point.z,
                     gyro_data.max_point.z,
                     gyro_data.length
      );

      HAL_Delay(1000);

//      break;
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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
