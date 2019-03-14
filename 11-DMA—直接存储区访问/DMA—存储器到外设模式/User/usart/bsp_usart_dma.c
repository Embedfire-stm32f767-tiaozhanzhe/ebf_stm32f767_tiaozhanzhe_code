/**
  ******************************************************************************
  * @file    bsp_usart_dma.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   重现c库printf函数到usart端口,使用DMA模式发送数据
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F767 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_usart_dma.h"

DMA_HandleTypeDef DMA_Handle;
UART_HandleTypeDef UartHandle;

 /**
  * @brief  USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void Debug_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
        
    DEBUG_USART_RX_GPIO_CLK_ENABLE();
    DEBUG_USART_TX_GPIO_CLK_ENABLE();
    
    /* 配置串口1时钟源*/
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    RCC_PeriphClkInit.Usart1ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
    /* 使能 UART 时钟 */
    DEBUG_USART_CLK_ENABLE();

    /**USART1 GPIO Configuration    
    PA9     ------> USART2_TX
    PA10    ------> USART2_RX 
    */
    /* 配置Tx引脚为复用功能  */
    GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = DEBUG_USART_TX_AF;
    HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
    
    /* 配置Rx引脚为复用功能 */
    GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
    GPIO_InitStruct.Alternate = DEBUG_USART_RX_AF;
    HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct); 
    
    /* 配置串DEBUG_USART 模式 */
    UartHandle.Instance = DEBUG_USART;
    UartHandle.Init.BaudRate = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    UartHandle.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&UartHandle);
}

///重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口DEBUG_USART */
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}

///重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		
	int ch;
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}


/**
  * @brief  USART1 TX DMA 配置，内存到外设(USART1->DR)
  * @param  无
  * @retval 无
  */
void USART_DMA_Config(void)
{

  /*开启DMA时钟*/
  DEBUG_USART_DMA_CLK_ENABLE();

  DMA_Handle.Instance = DEBUG_USART_DMA_STREAM;
  /*usart1 tx对应dma2，通道4，数据流7*/	
  DMA_Handle.Init.Channel = DEBUG_USART_DMA_CHANNEL;  
  /*方向：从内存到外设*/		
  DMA_Handle.Init.Direction= DMA_MEMORY_TO_PERIPH;	
  /*外设地址不增*/	    
  DMA_Handle.Init.PeriphInc = DMA_PINC_DISABLE; 
  /*内存地址自增*/
  DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;	
  /*外设数据单位*/	
  DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  /*内存数据单位 8bit*/
  DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;	
  /*DMA模式：不断循环*/
  DMA_Handle.Init.Mode = DMA_CIRCULAR;	 
  /*优先级：中*/	
  DMA_Handle.Init.Priority = DMA_PRIORITY_MEDIUM;      
  /*禁用FIFO*/
  DMA_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;        
  DMA_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;    
  /*存储器突发传输 16个节拍*/
  DMA_Handle.Init.MemBurst = DMA_MBURST_SINGLE;    
  /*外设突发传输 1个节拍*/
  DMA_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;    
  /*配置DMA2的数据流7*/		   
//  /* Deinitialize the stream for new transfer */
  HAL_DMA_DeInit(&DMA_Handle);
  /* Configure the DMA stream */
  HAL_DMA_Init(&DMA_Handle); 
  
   /* Associate the DMA handle */
  __HAL_LINKDMA(&UartHandle, hdmatx, DMA_Handle);

}


/*********************************************END OF FILE**********************/
