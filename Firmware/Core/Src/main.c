/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define XTAL_FREQ 25000000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

Si5351_ConfigTypeDef si1;
uint32_t channel0Freq;
uint32_t channel1Freq;
uint8_t selectedChannel;
uint8_t currentChar;
uint8_t currentMenu;

char textBuffer[16];
char editBuffer[16];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
static void MX_SI_Init(void);
static void MX_SCR_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void SetFrequency(uint8_t channel, uint32_t desiredFrequency)
{

	uint32_t pllFreq;
	uint32_t l;
	float f;
	uint8_t mult;
	uint32_t num;
	uint32_t denom;
	uint32_t divider;
	Si5351_CLKRDivTypeDef rDivider = CLK_R_Div1;

	if(desiredFrequency == 0)
	{
		si1.CLK[channel].CLK_Enable = OFF;
		Si5351_Init(&si1);
		if(channel == 0)
			channel0Freq = 0;
		else
			channel1Freq = 0;

		return;
	}

	if(desiredFrequency < 1000000)
	{
		rDivider = CLK_R_Div64;
		divider = 900000000 / (desiredFrequency * 64);

		if (divider % 2)
			divider--;

		pllFreq = divider * desiredFrequency * 64;
	}
	else
	{
		divider = 900000000 / desiredFrequency;

		if (divider % 2)
			divider--;

		pllFreq = divider * desiredFrequency;
	}

	mult = pllFreq / XTAL_FREQ;
	l = pllFreq % XTAL_FREQ;

	if(l == 0)
	{
		num = 0;
		denom = 1;
	}
	else
	{
		f = l;


		f /= XTAL_FREQ;
		f *= 1048575;
		num = f;
		denom = 1048575;
	}

	float finalFreq = (25000000 * (float)(mult + (float)num / (float)denom)) / (divider);

	if(rDivider != CLK_R_Div1)
		finalFreq /= 64;

	si1.PLL[channel].PLL_Multiplier_Integer = mult; //800Mhz
	si1.PLL[channel].PLL_Multiplier_Numerator = num;
	si1.PLL[channel].PLL_Multiplier_Denominator = denom;
	si1.MS[channel].MS_Divider_Integer = divider;
	si1.CLK[channel].CLK_R_Div = rDivider;
	si1.CLK[channel].CLK_Enable = ON;

	Si5351_Init(&si1);
	Si5351_PLLReset(&si1, channel);

	if(channel == 0)
		channel0Freq = finalFreq;
	else
		channel1Freq = finalFreq;
}

void SelectOsc(uint8_t oscChannel)
{

	selectedChannel = oscChannel;
	if(oscChannel == 0)
	{
		ssd1306_SetCursor(0, 10);
		ssd1306_WriteString(">", Font_6x8, White);
		ssd1306_SetCursor(0, 30);
		ssd1306_WriteString(" ", Font_6x8, White);
	}
	else
	{
		ssd1306_SetCursor(0, 10);
		ssd1306_WriteString(" ", Font_6x8, White);
		ssd1306_SetCursor(0, 30);
		ssd1306_WriteString(">", Font_6x8, White);
	}

	ssd1306_UpdateScreen();
}

void ShowCurrentOscConfig()
{
	size_t freqLen;

	ssd1306_Fill(Black);

	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("-MultiOsc-", Font_6x8, White);

	ssd1306_SetCursor(0, 10);
	ssd1306_WriteString("  OSC 1:", Font_6x8, White);

	memset(textBuffer, 0, 16);
	sprintf(textBuffer, "%lu", channel0Freq);
	freqLen = strlen(textBuffer);

	ssd1306_SetCursor((10 - freqLen) * 6, 20);
	ssd1306_WriteString(textBuffer, Font_6x8, White);

	ssd1306_SetCursor(0, 30);
	ssd1306_WriteString("  OSC 2:", Font_6x8, White);

	memset(textBuffer, 0, 16);
	sprintf(textBuffer, "%lu", channel1Freq);
	freqLen = strlen(textBuffer);

	ssd1306_SetCursor((10 - freqLen) * 6, 40);
	ssd1306_WriteString(textBuffer, Font_6x8, White);

	SelectOsc(0);
}

uint8_t DebounceButton(GPIO_TypeDef* Port, uint16_t Pin)
{
	if(!HAL_GPIO_ReadPin(Port, Pin))
	{
		while(!HAL_GPIO_ReadPin(Port, Pin))
			HAL_Delay(1);

		HAL_Delay(50);

		return 1;
	}

	return 0;
}

void IncrementChar()
{
	uint8_t charIndex = 8 - currentChar;

	editBuffer[charIndex]++;

	if(editBuffer[charIndex] > 57)
		editBuffer[charIndex] = 48;

	ssd1306_SetCursor(6, 16);
	ssd1306_WriteString(editBuffer, Font_6x8, White);

	ssd1306_UpdateScreen();
}

void DecrementChar()
{
	uint8_t charIndex = 8 - currentChar;

	editBuffer[charIndex]--;

	if(editBuffer[charIndex] < 48)
		editBuffer[charIndex] = 57;

	ssd1306_SetCursor(6, 16);
	ssd1306_WriteString(editBuffer, Font_6x8, White);

	ssd1306_UpdateScreen();
}

void SelectChar(uint8_t charNumber)
{
	currentChar = charNumber;

	memset(textBuffer, ' ', 10);
	textBuffer[10] = 0;
	textBuffer[9 - charNumber] = '^';

	ssd1306_SetCursor(0, 24);
	ssd1306_WriteString(textBuffer, Font_6x8, White);

	ssd1306_UpdateScreen();
}

void ShowEditOscConfig()
{

	size_t freqLen;
	uint32_t currentFreq = selectedChannel? channel1Freq : channel0Freq;

	if(currentFreq == 0)
	{
		memset(editBuffer, 48, 10);
		editBuffer[9] = 0;
	}
	else
	{
		sprintf(editBuffer, "%lu", currentFreq);
		freqLen = strlen(editBuffer);
		memset(editBuffer, 48, 9);
		sprintf(&editBuffer[9 - freqLen], "%lu", currentFreq);
	}

	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);

	if(selectedChannel == 0)
		ssd1306_WriteString("SET OSC 1:", Font_6x8, White);
	else
		ssd1306_WriteString("SET OSC 2:", Font_6x8, White);

	ssd1306_SetCursor(6, 16);
	ssd1306_WriteString(editBuffer, Font_6x8, White);

	SelectChar(0);
}

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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  MX_SI_Init();
  MX_SCR_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);


	  if(currentMenu == 0)
	  {
		  if(DebounceButton(BTN_UP_GPIO_Port, BTN_UP_Pin) || DebounceButton(BTN_DOWN_GPIO_Port, BTN_DOWN_Pin))
		  {
			  if(selectedChannel == 0)
				  SelectOsc(1);
			  else
				  SelectOsc(0);
		  }
		  else if(DebounceButton(BTN_ENTER_GPIO_Port, BTN_ENTER_Pin))
		  {
			  currentMenu = 1;
			  ShowEditOscConfig();
		  }
	  }
	  else
	  {
		  if(DebounceButton(BTN_LEFT_GPIO_Port, BTN_LEFT_Pin))
		  {
			  currentChar++;
			  if(currentChar > 8)
				  currentChar = 0;

			  SelectChar(currentChar);
		  }
		  else if(DebounceButton(BTN_RIGHT_GPIO_Port, BTN_RIGHT_Pin))
		  {
			  currentChar--;
			  if(currentChar > 8)
				  currentChar = 8;

			  SelectChar(currentChar);
		  }
		  else if(DebounceButton(BTN_UP_GPIO_Port, BTN_UP_Pin))
		  {
			  IncrementChar();
		  }
		  else if (DebounceButton(BTN_DOWN_GPIO_Port, BTN_DOWN_Pin))
		  {
			  DecrementChar();
		  }
		  else if(DebounceButton(BTN_ENTER_GPIO_Port, BTN_ENTER_Pin))
		  {
			  uint32_t newFreq = (unsigned)atol(editBuffer);

			  if(newFreq < 7000)
				  newFreq = 0;

			  SetFrequency(selectedChannel, newFreq);
			  currentMenu = 0;
			  ShowCurrentOscConfig();
		  }
	  }

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
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
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OLED_RESET_PIN_GPIO_Port, OLED_RESET_PIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_UP_Pin BTN_DOWN_Pin BTN_LEFT_Pin BTN_RIGHT_Pin */
  GPIO_InitStruct.Pin = BTN_UP_Pin|BTN_DOWN_Pin|BTN_LEFT_Pin|BTN_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_ENTER_Pin */
  GPIO_InitStruct.Pin = BTN_ENTER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BTN_ENTER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OLED_RESET_PIN_Pin */
  GPIO_InitStruct.Pin = OLED_RESET_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OLED_RESET_PIN_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

static void MX_SI_Init(void)
{
	Si5351_StructInit(&si1, &hi2c1);
	si1.OSC.OSC_XTAL_Load = XTAL_Load_10_pF;

	si1.PLL[0].PLL_Clock_Source = PLL_Clock_Source_XTAL;
	si1.PLL[0].PLL_Multiplier_Integer = 32; //800Mhz
	si1.MS[0].MS_Clock_Source = MS_Clock_Source_PLLA;
	si1.MS[0].MS_Divider_Integer = 80; //10Mhz
	si1.CLK[0].CLK_R_Div = CLK_R_Div1;
	si1.CLK[0].CLK_Enable = OFF;

	si1.PLL[1].PLL_Clock_Source = PLL_Clock_Source_XTAL;
	si1.PLL[1].PLL_Multiplier_Integer = 32; //800Mhz
	si1.MS[1].MS_Clock_Source = MS_Clock_Source_PLLB;
	si1.MS[1].MS_Divider_Integer = 50; //10Mhz
	si1.CLK[1].CLK_R_Div = CLK_R_Div16; //1Mhz
	si1.CLK[1].CLK_Enable = OFF;

	si1.CLK[2].CLK_Enable = OFF;

	Si5351_Init(&si1);

	channel0Freq = 0;
	channel1Freq = 0;
}

static void MX_SCR_Init(void)
{
	ssd1306_Init();
	ShowCurrentOscConfig();
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
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
