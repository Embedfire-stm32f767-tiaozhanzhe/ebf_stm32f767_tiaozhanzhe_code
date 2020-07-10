/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   用V1.2.0版本库建的工程模板
  ******************************************************************
  * @attention
  *
  * 实验平台:野火  STM32F767开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32f7xx.h"
#include "main.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h"
#include "./sdmmc/bsp_sdmmc_sd.h"
#include "./flash/bsp_qspi_flash.h"
#include "./key/bsp_key.h" 
//#include "./delay/core_delay.h"  
/* FatFs includes component */
#include "ff.h"
#include "./flash/bsp_qspi_flash.h"
#include "./res_mgr/aux_data.h"
/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
char SDPath[4]; /* SD逻辑驱动器路径 */
extern FATFS sd_fs;	
FRESULT res_sd;                /* 文件操作结果 */

//要复制的文件路径，到aux_data.c修改
extern char src_dir[];
extern char dst_dir[];

static void WIFI_PDN_INIT(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStruct;
	/*使能引脚时钟*/	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	/*选择要控制的GPIO引脚*/															   
	GPIO_InitStruct.Pin = GPIO_PIN_13;	
	/*设置引脚的输出类型为推挽输出*/
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      
	/*设置引脚为上拉模式*/
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	/*设置引脚速率为高速 */   
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST; 
	/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	/*禁用WiFi模块*/
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);  
}
DIR dir; 
FIL fnew;													/* 文件对象 */
UINT fnum;            			  /* 文件成功读写数量 */
BYTE ReadBuffer[1024]={0};        /* 读缓冲区 */
BYTE WriteBuffer[] =              /* 写缓冲区*/
"欢迎使用野火STM32 F767开发板 今天是个好日子，新建文件系统测试文件\r\n";  

extern FATFS flash_fs;

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
uint8_t state = QSPI_ERROR;
int main(void)
{
	  FRESULT res = FR_OK;
    /* 系统时钟初始化成400MHz */
    SystemClock_Config();
    WIFI_PDN_INIT();
  
    LED_GPIO_Config();
    LED_BLUE;	
    /* 初始化USART1 配置模式为 115200 8-N-1 */
    UARTx_Config();	
  
    Key_GPIO_Config();
    QSPI_FLASH_Init();

    //在外部SD卡挂载文件系统，文件系统挂载时会对SD卡初始化
    res_sd = f_mount(&sd_fs,SD_ROOT,1);  

    if(res_sd != FR_OK)
    {
      printf("f_mount ERROR!请给开发板插入SD卡然后重新复位开发板!");
      LED_RED;
      while(1);
    }
  
#if 1    
    printf("\r\n 按一次KEY1开始烧写字库并复制文件到FLASH。 \r\n"); 
    printf("\r\n 注意该操作会把FLASH的原内容会被删除！！ \r\n"); 

    while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0);
    while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1); 
    printf("\r\n 正在进行整片擦除，时间很长，请耐心等候...\r\n"); 
    
    BSP_QSPI_Erase_Chip();
  //烧录数据到flash的非文件系统区域    
  res = burn_file_sd2flash(burn_data,AUX_MAX_NUM); 
  
  if(res == FR_OK)
  {
    printf("\r\n\r\n\r\n"); 

    //复制文件到FLASH的文件系统区域
    copy_file_sd2flash(src_dir,dst_dir);
    if(res == FR_OK)
    {
      printf("\r\n 所有数据已成功复制到FLASH！！！ \r\n");  
      LED_GREEN;
    }
    else
    {
      printf("\r\n 复制文件到FLASH失败(文件系统部分)，请复位重试！！ \r\n"); 
    }
  }
  else
  {
    printf("\r\n 拷贝数据到FLASH失败(非文件系统部分)，请复位重试！！ \r\n"); 
  }
  

#endif    
    /* 操作完成，停机 */
    while(1)
    {
    }
}
/**
  * @brief  System Clock 配置
  *         system Clock 配置如下: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 400000000 (CPU Clock)
	*            HCLK(Hz)             = 200000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  100MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  100MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  100MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  100MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 160
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
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
/****************************END OF FILE***************************/
