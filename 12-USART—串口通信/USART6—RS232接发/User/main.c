/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   使用RS232串口，中断接收测试，演示接收数据到buff，并进行简单命令处理。
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 767 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f7xx.h"
#include "main.h"
#include "./usart/bsp_rs232_usart.h"

volatile uint8_t Rxflag=0;
uint8_t ucTemp;
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	uint8_t ucaRxBuf[256];
	uint16_t usRxCount=0; 
    
    /* 配置系统时钟为216 MHz */
    SystemClock_Config();
	
    /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
    RS232_USART_Config();

	/*调用printf函数，因为重定向了fputc，printf的内容会输出到串口*/
	printf("\r\nPrintf方式输出：这是一个串口中断接收回显实验 \r\n");	

	
	/*自定义函数方式*/
	Usart_SendString( RS232_USART, (uint8_t *)"自定义函数输出：这是一个串口中断接收回显实验\n" );
	Usart_SendString( RS232_USART, (uint8_t *)"输入数据并以回车键结束\n" );
	
	/*STM32串口接收到字符后会进入stm32f4xx_it.c文件的中断服务函数，
	*接收该数据，并标记Rxflag标志位。*/

  while(1)
	{	
		/* 
			接收DEBUG_USART口的数据，分析并处理 
			可以将此段代码封装为一个函数，在主程序其它流程调用
		*/
		if(Rxflag)
		{
			if (usRxCount < sizeof(ucaRxBuf))
			{
				ucaRxBuf[usRxCount++] = ucTemp;
			}
			else
			{
				usRxCount = 0;
			}
			
			/* 简单的通信协议，遇到回车换行符认为1个命令帧，可自行加其它判断实现自定义命令 */
			/* 遇到换行字符，认为接收到一个命令 */
			if (ucTemp == 0x0A)	/* 换行字符 */
			{		
				/*检测到有回车字符就把数据返回给上位机*/
				HAL_UART_Transmit( &UartHandle, (uint8_t *)ucaRxBuf,usRxCount,1000 );
				usRxCount = 0;
			}
			Rxflag=0;
		}
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

