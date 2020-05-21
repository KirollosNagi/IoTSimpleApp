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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
uint8_t hexToAscii(uint8_t n)//4-bit hex value converted to an ascii character
{
 if (n>=0 && n<=9) n = n + '0';
 else n = n - 10 + 'A';
 return n;
}
uint8_t* htod(uint8_t h[])
{
    if(h[0]>='0'&&h[0]<='9')
        h[0]-='0';
    else
        h[0]=h[0]-'A'+10;
    if(h[1]>='0' && h[1]<='9')
        h[1]-='0';
    else
        h[1]=h[1]-'A'+10;
    int d=h[0]*16 +h[1];
    char t[]={'0','0','0'};
    int i=0;
    while(d>0)
    {
        t[2-i] = d%10+'0';
        d=d/10;
        i++;
    }
    return t;
}
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t rxdata;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	if (HAL_I2C_IsDeviceReady(&hi2c1, 0xD0, 10, HAL_MAX_DELAY) == HAL_OK)
	{
		for (int i = 1; i<=10;i++) // indicator of ready device
		{
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
			HAL_Delay(250);
		}
	}
	else
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
		HAL_Delay(250);
	}
//////////////////////////////////////////////////////////////////////////
	//STM sends a command to get ip
	uint8_t command[]={'!'};
	HAL_UART_Transmit(&huart1,command,sizeof(command),10);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	int timebool=0;
	//get temp
	uint8_t control [2], status[2], Tupper[2], Tlower[2];
	status[0]	= 0x0F;
	control[0]	= 0x0E;
	Tupper[0] = 0x11;
	Tlower[0] = 0x12;
	uint8_t temp1[2],*temp2,nsign='-';
	uint8_t tempout[] = {'0','0','0','.','0','0','\r','\n'};
	//get clock
	uint8_t secbuffer [2], minbuffer [2], hourbuffer [2];
	uint8_t dowbuffer [2],datebuffer[2],monthbuffer[2],yearbuffer[2];
	// seconds
	secbuffer[0] = 0x00; //register address
	/*initialization only
	secbuffer[1] = 0x00; //0 seconds
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, secbuffer, 2, 10);
	*/
	// minutes
	minbuffer[0] = 0x01; //register address
	/*initialization only
	minbuffer[1] = 0x17; // 15 mins
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, minbuffer, 2, 10);
	*/
	// hours
	hourbuffer[0] = 0x02; //register address
	/*initialization only
	hourbuffer[1] = 0x05;	// 5 am
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, hourbuffer, 2, 10);
	*/
	//DOW 
	dowbuffer[0] = 0x03; //register address
	/*initialization only
	dowbuffer[1] = 0x05; // Thursday
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, dowbuffer, 2, 10);
	*/
	//date
	datebuffer[0] = 0x04; //register address
	/*initialization only
	datebuffer[1] = 0x21; // 21 may
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, datebuffer, 2, 10);
	*/
	//month
	monthbuffer[0] = 0x05; //register address
	/*initialization only
	monthbuffer[1] = 0x05; //MAY
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, monthbuffer, 2, 10);
	*/
	//year
	yearbuffer[0] = 0x06; //register address
	/*initialization only
	yearbuffer[1] = 0x20;	// 2020
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, yearbuffer, 2, 10);
	*/
	//Receive via I2C and forward to UART
	uint8_t newline[]={'\r', '\n'};
	uint8_t fnewline[]={'F','\r', '\n'};
	uint8_t outf[] ={':', '\t'};
	uint8_t out[] = {0,0,':',0,0,':',0,0,' ',' ','1','\t',0,0,'/',0,0,'/',0,0,'\r','\n'};
 	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
	uint8_t cc1='P';
	uint8_t cc2='E';
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, status, 1, 10);		//0xD0 is write
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, status+1, 1, 10);	//0xD1 is read
		if((status[1]&0x04) == 0x00)
		{
			HAL_I2C_Master_Transmit(&hi2c1, 0xD0, control, 1, 10);
			HAL_I2C_Master_Receive(&hi2c1, 0xD1, control+1, 1, 10);
			control[1] |= 0x20; 		//set CONV bit (5th)
			HAL_I2C_Master_Transmit(&hi2c1, 0xD0, control, 2, 10);
			HAL_I2C_Master_Transmit(&hi2c1, 0xD0, Tupper, 1, 10);
			HAL_I2C_Master_Receive(&hi2c1, 0xD1, Tupper+1, 1, 10);
			HAL_I2C_Master_Transmit(&hi2c1, 0xD0, Tlower, 1, 10);
			HAL_I2C_Master_Receive(&hi2c1, 0xD1, Tlower+1, 1, 10);
			temp1[0] = hexToAscii(Tupper[1] >> 4);
			temp1[1] = hexToAscii(Tupper[1] & 0x0F);
			temp2 = htod(temp1);
			tempout[0]=temp2[0];
			tempout[1]=temp2[1];
			tempout[2]=temp2[2];
			switch(Tlower[1]) 
			{
				case 0x00:
					tempout[4] = '0';
					tempout[5] = '0';
					break;
				case 0x40:
					tempout[4] = '2';
					tempout[5] = '5';
					break;
				case 0x80:
					tempout[4] = '5';
					tempout[5] = '0';
					break;
				case 0xc0:
					tempout[4] = '7';
					tempout[5] = '5';
					break;
			}
			
		}
		//get year
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, yearbuffer, 1, 10);
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, yearbuffer+1, 1, 10);
		out[18] = hexToAscii(yearbuffer[1] >> 4 );
		out[19] = hexToAscii(yearbuffer[1] & 0x0F );
		//get month
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, monthbuffer, 1, 10);
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, monthbuffer+1, 1, 10);
		out[15] = hexToAscii((monthbuffer[1] >> 4)& 0x01 );
		out[16] = hexToAscii(monthbuffer[1] & 0x0F );
		//get date
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, datebuffer, 1, 10);
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, datebuffer+1, 1, 10);
		out[12] = hexToAscii(datebuffer[1] >> 4);
		out[13] = hexToAscii(datebuffer[1] & 0x0F );
		//get day of week
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, dowbuffer, 1, 10);
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, dowbuffer+1, 1, 10);
		out[10] = hexToAscii(dowbuffer[1] & 0x0F );
		//send seconds register address 00h to read from
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, secbuffer, 1, 10);
		//read data of register 00h to secbuffer[1]
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, secbuffer+1, 1, 10);
		//prepare UART output
		out[6] = hexToAscii(secbuffer[1] >> 4 );
		out[7] = hexToAscii(secbuffer[1] & 0x0F );
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, minbuffer, 1, 10);
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, minbuffer+1, 1, 10);
		out[3] = hexToAscii(minbuffer[1] >> 4 );
		out[4] = hexToAscii(minbuffer[1] & 0x0F );
		HAL_I2C_Master_Transmit(&hi2c1, 0xD0, hourbuffer, 1, 10);
		HAL_I2C_Master_Receive(&hi2c1, 0xD1, hourbuffer+1, 1, 10);
		out[0] = hexToAscii((hourbuffer[1] >> 4)&0x01);
		out[1] = hexToAscii(hourbuffer[1] & 0x0F);
		// transmit time to UART
		if(rxdata=='N')
		{
			HAL_UART_Transmit(&huart2,newline, sizeof(newline), 10);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
			rxdata=' ';
		}
		if(rxdata=='M')
		{
			timebool=1;
		}
		else if(rxdata =='F')
		{
			HAL_UART_Transmit(&huart2,fnewline, sizeof(fnewline), 10);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
			rxdata=' ';
		}
		else if (rxdata == 'P')
		{
			
			HAL_UART_Transmit(&huart2,outf, sizeof(outf), 10);
			HAL_UART_Transmit(&huart1, &cc1, sizeof(cc1),10);
			if((Tupper[1] & 0x80) != 0x00)
			{
				HAL_UART_Transmit(&huart2,(uint8_t*) &nsign, 1, 100);
				HAL_UART_Transmit(&huart1,(uint8_t*) &nsign, 1, 100);
			}
			if(tempout[0]=='0')
			{
				HAL_UART_Transmit(&huart2,tempout+1, sizeof(tempout)-1, 10);
				HAL_UART_Transmit(&huart1,tempout+1, sizeof(tempout)-3, 10);
			}
			else
			{
				HAL_UART_Transmit(&huart2,tempout, sizeof(tempout), 10);
				HAL_UART_Transmit(&huart1,tempout, sizeof(tempout)-2, 10);

			}
			HAL_UART_Transmit(&huart1,command,sizeof(command),10);
			rxdata=' ';
			timebool=0;
		}
		else if (rxdata == 'E' && timebool)
		{
			HAL_UART_Transmit(&huart1, &cc2, sizeof(cc2),10);
			HAL_UART_Transmit(&huart2,outf, sizeof(outf), 10);
			HAL_UART_Transmit(&huart2,out, sizeof(out), 10);
			HAL_UART_Transmit(&huart1,out, sizeof(out)-2, 10);
			HAL_UART_Transmit(&huart1,command,sizeof(command),10);
			rxdata=' ';
			timebool=0;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
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
  hi2c1.Init.Timing = 0x00000E14;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
