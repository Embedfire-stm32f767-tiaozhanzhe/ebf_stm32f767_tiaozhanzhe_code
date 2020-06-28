/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   Ethernet_UDP_Client
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 767 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "main.h"  
#include "stm32f7xx.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h"
#include "lwip/init.h"
#include "app_ethernet.h"
#include "ethernetif.h"
#include "lwip/timeouts.h"
#include "./BSP/key/bsp_key.h"
#include "udp_echoclient.h"

KEY Key1,Key2;
extern __IO uint8_t EthLinkStatus;

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{ 
	/* Enable I-Cache */
	SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();
	
    /* 配置系统时钟为216 MHz */
    SystemClock_Config();

    /* 初始化RGB彩灯 */
    LED_GPIO_Config();

    /* 初始化USART1 配置模式为 115200 8-N-1 */
    UARTx_Config();
	
    Key_GPIO_Config();
	KeyCreate(&Key1,GetPinStateOfKey1);
	KeyCreate(&Key2,GetPinStateOfKey2);
	
    /* 初始化LwIP协议栈*/
    lwip_init();
	
	printf("LAN8720A Ethernet Demo\n");
    printf("LwIP版本：%s\n",LWIP_VERSION_STRING);

    /* 网络接口配置 */
    Netif_Config();
    /* 报告用户网络连接状态 */
    User_notification(&gnetif);
#ifdef USE_DHCP
    printf("使用DHCP服务获取IP地址\r\n");
#else 
    printf("使用静态IP地址\r\n");
	//IP地址和端口可在main.h文件修改，或者使用DHCP服务自动获取IP(需要路由器支持) 
	printf("本地IP和端口: %d.%d.%d.%d:%d\n",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3,UDP_CLIENT_PORT);   
#endif
    
    printf("远端IP和端口: %d.%d.%d.%d:%d\n",DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3,UDP_SERVER_PORT);

    printf("    KEY1: 启动UDP连接\n");
    printf("    KEY2: 断开UDP连接\n");  
	while (1)
	{ 
		Key_RefreshState(&Key1);//刷新按键状态
		Key_RefreshState(&Key2);//刷新按键状态
		if(Key_AccessTimes(&Key1,KEY_ACCESS_READ)!=0)//按键被按下过
		{
			LED2_ON;
			if (EthLinkStatus == 0)
			{
                printf("Connect to udp server\n");
				/* Connect to udp server */ 
				udp_echoclient_connect();
			}
			Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
		}
		if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0)//按键被按下过
		{
			LED2_OFF;
			udp_echoclient_disconnect();
			Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
		}  		
        /* 从以太网缓冲区中读取数据包，交给LwIP 处理 */
        ethernetif_input(&gnetif);
        /* 处理 LwIP 超时 */
        sys_check_timeouts();
		
#ifdef USE_DHCP
        /* 处理DHCP的周期定时器 */
        DHCP_Periodic_Handle(&gnetif);
#endif 		
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
void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

/*********************************************END OF FILE**********************/

