/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   ADC-多通道-DMA
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 746 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "main.h"  
#include "stm32f7xx.h"
#include "./adc/bsp_adc.h"
#include "./usart/bsp_usart.h"

// ADC转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue[RHEOSTAT_NOFCHANEL];

// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal[RHEOSTAT_NOFCHANEL]={0}; 


static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{   
    /* 配置系统时钟为216 MHz */
    SystemClock_Config();

    /* 初始化USART1 配置模式为 115200 8-N-1 */
    UARTx_Config();

    Rheostat_Init();
    while (1)
    {
		ADC_ConvertedValueLocal[0] =(float) ADC_ConvertedValue[0]/4096*(float)3.3;
		ADC_ConvertedValueLocal[1] =(float) ADC_ConvertedValue[1]/4096*(float)3.3;
		ADC_ConvertedValueLocal[2] =(float) ADC_ConvertedValue[2]/4096*(float)3.3;
		
		printf("\r\n CH1_PC3 value = %f V \r\n",ADC_ConvertedValueLocal[0]);
		printf("\r\n CH2_PA4 value = %f V \r\n",ADC_ConvertedValueLocal[1]);
		printf("\r\n CH3_PA6 value = %f V \r\n",ADC_ConvertedValueLocal[2]);
		
		printf("\r\n\r\n");
		Delay(0xffffff);  
    }   
}


/**
  * @brief  System Clock 配置
  *         system Clock 配置如下 : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
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

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
	if(ret != HAL_OK)
	{
		while(1) { ; }
	}  
}

/*********************************************END OF FILE**********************/

