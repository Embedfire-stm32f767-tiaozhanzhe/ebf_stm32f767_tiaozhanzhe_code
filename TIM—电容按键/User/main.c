/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   蜂鸣器例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F746 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f7xx.h"
#include "main.h"
#include "./beep/bsp_beep.h"
#include "./TouchPad/bsp_touchpad.h"
#include "./usart/bsp_usart.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    /* 系统时钟初始化成216 MHz */
    SystemClock_Config();
	/*串口初始化 */
	UARTx_Config();
	/*蜂鸣器端口初始化 */
	BEEP_GPIO_Config(); 
	/* 初始化电容按键 */
	TPAD_Init();
	/* 控制IO */
	while (1)
	{
		if(TPAD_Scan(0))
		{
			BEEP_ON;
			HAL_Delay(100);
			BEEP_OFF;
		}	
	}
}

/**
  * @brief  System Clock 配置
  *         System Clock 配置如下 : 
  *         System Clock source            = PLL (HSE)
  *         SYSCLK(Hz)                     = 216000000
  *         HCLK(Hz)                       = 216000000
  *         AHB Prescaler                  = 1
  *         APB1 Prescaler                 = 4
  *         APB2 Prescaler                 = 2
  *         HSE Frequency(Hz)              = 25000000
  *         PLL_M                          = 25
  *         PLL_N                          = 432
  *         PLL_P                          = 2
  *         PLL_Q                          = 9
  *         VDD(V)                         = 3.3
  *         Main regulator output voltage  = Scale1 mode
  *         Flash Latency(WS)              = 7
  * @param  无
  * @retval 无
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* 使能HSE，配置HSE为PLL的时钟源，配置PLL的各种分频因子M N P Q 
	 * PLLCLK = HSE/M*N/P = 25M / 25 *432 / 2 = 216M
	 */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* 激活 OverDrive 模式以达到216M频率  */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* 选择PLLCLK作为SYSCLK，并配置 HCLK, PCLK1 and PCLK2 的时钟分频因子 
	 * SYSCLK = PLLCLK     = 216M
	 * HCLK   = SYSCLK / 1 = 216M
	 * PCLK2  = SYSCLK / 2 = 108M
	 * PCLK1  = SYSCLK / 4 = 54M
	 */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 
  
	/* 在HAL_RCC_ClockConfig函数里面同时初始化好了系统定时器systick，配置为1ms中断一次 */
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }  
}
/*********************************************END OF FILE**********************/

