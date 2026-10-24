/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile int i=0;//初始占空比
volatile uint16_t flag=0;
uint8_t mode=0;//用来存放模式
uint8_t rx_data;//存放收到的字符
volatile int duty;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	if (huart==&huart1)
	{
		if(rx_data=='A')//识别收到的字符
		{
			mode=1;//改变模式
			
			flag=0;//错误提示的标志
			
		}
		else if(rx_data=='M')//识别收到的字符
		{
			mode=2;
			
			flag=0;
		}
		
		
		 
		
			else if(rx_data=='+' && mode==2)//识别收到的字符
			{
				i=i+20;//每次占空比增加20
				if(i>=100)//把i锁定在最大值
	            {
		        i=100;
	            }
				duty=i*100/(htim2.Init.Period+1);
				 __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,i);//改变PWM的值
	             OLED_ShowNum(40,0,duty,3,16);//输出结果
	             OLED_ShowChar(64,0,'%',16);
				flag=0;//改变错误提示的标志
			}
			else if(rx_data=='-' && mode==2)//识别收到的字符
			{
				i=i-20;
				if(i<=0)//把i锁定在最小值
	            {
		        i=0;
	            }
				duty=i*100/(htim2.Init.Period+1);
				 __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,i);//改变PWM
	             OLED_ShowNum(40,0,duty,3,16);
	             OLED_ShowChar(64,0,'%',16);//OLED显示结果
				flag=0;//改变错误提示的标志
			}
			else flag=1;
			
			
	
	}
	HAL_UART_Receive_IT(&huart1,&rx_data,1);//开启UART接收中断
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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);//开启PWM
  OLED_Clear(0x00);//OLED清屏
  OLED_ShowString(0,0,(uint8_t*)"Duty:",16);
  HAL_UART_Receive_IT(&huart1,&rx_data,1);
  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,i);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  OLED_ShowString(0,0,(uint8_t*)"Duty:",16);
	  if(flag==1)
	  {
		 OLED_ShowString(0,10,(uint8_t*)"Input Invalid",16); //若输入错误弹出提示
	  }
	  if(flag==0)
	  {
		 OLED_ShowString(0,10,(uint8_t*)"             ",16); //输入正确覆盖提示
	  }
	  if(mode==1)
		{
			if(i<20)//锁定占空比范围
			{
				i=20;
			}
			if(i>80)//锁定占空比范围
			{
				i=80;
			}
			for(;i<=80 && i>=20;)
			{
				
				
			    for(;i<80;i++)//占空比平滑增加
				{
					HAL_Delay(20);
					duty=i*100/(htim2.Init.Period+1);
				     __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,i);//OLED显示占空比
	                 OLED_ShowNum(40,0,duty,3,16);
	                 OLED_ShowChar(64,0,'%',16);
					if(flag==1)
	                {
		                OLED_ShowString(0,10,(uint8_t*)"Input Invalid",16); //提示错误
	                }
	                if(flag==0)
	                {
		                OLED_ShowString(0,10,(uint8_t*)"             ",16); //消去错误提示
	                }
					if(mode!=1)
					break;//当改变模式则跳出死循环
				}
				if(i==80)
				{
				    for(;i>20;i--)
					{
						HAL_Delay(20);
						duty=i*100/(htim2.Init.Period+1);
					     __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,i);
	                     OLED_ShowNum(40,0,duty,3,16);
	                     OLED_ShowChar(64,0,'%',16);//OLED显示结果
						if(flag==1)
	                    {
		                    OLED_ShowString(0,10,(uint8_t*)"Input Invalid",16); //提示错误
	                    }
	                    if(flag==0)
	                    {
		                    OLED_ShowString(0,10,(uint8_t*)"             ",16); //消除错误提示
	                    }
						if(mode!=1)
					    break;
					}
				}
			}
					
		}



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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
