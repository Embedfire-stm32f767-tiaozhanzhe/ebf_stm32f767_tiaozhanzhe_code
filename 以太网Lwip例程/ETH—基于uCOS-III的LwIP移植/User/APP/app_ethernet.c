/**
  ******************************************************************************
  * @file    LwIP/LwIP_TCP_Echo_Client/Src/app_ethernet.c 
  * @author  MCD Application Team
  * @brief   Ethernet specefic module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "lwip/opt.h"
#include "main.h"
#include "lwip/dhcp.h"
#include "app_ethernet.h"
#include "ethernetif.h"
#include "netif/etharp.h"
#include "./Bsp/led/bsp_led.h"
#include  <os_app_hooks.h>
#include "lwip/tcpip.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define USE_DEBUG

#ifdef USE_DHCP
#define MAX_DHCP_TRIES  4
uint32_t DHCPfineTimer = 0;
__IO uint8_t DHCP_state = DHCP_OFF;
#endif
__IO uint8_t EthLinkStatus = 0;

/**
  * @brief  建立网络接口
  * @param  None
  * @retval None
  */
void Netif_Config(struct netif *netif)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
#ifdef USE_DHCP
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else
  IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
  IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
#endif /* USE_DHCP */
  /* 添加网络接口*/    
  netif_add(netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  
  /* 注册默认网络接口 */
  netif_set_default(netif);
  
//  pxNetIf = netif;
  if (netif_is_link_up(netif))
  {
    /* 当netif完全配置时，必须调用此函数 */
    netif_set_up(netif);
  }
  else
  {
    /* 当netif链接断开时，必须调用此函数 */
    netif_set_down(netif);
  }
  
}
/**
  * @brief  通知用户有关网络接口配置状态
  * @param  netif: 网络接口
  * @retval None
  */
void User_notification(struct netif *netif) 
{
  if (netif_is_up(netif))
  {
	LED_GREEN; 
#ifdef USE_DHCP
    /* 更新DHCP状态机 */
    DHCP_state = DHCP_START;
#else    
	printf("Static IP: %d.%d.%d.%d\n",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
    printf("NETMASK  : %d.%d.%d.%d\n",NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
    printf("Gateway  : %d.%d.%d.%d\n",GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
#endif /* USE_DHCP */    
	  
  }
  else
  {
    LED_RED;
	EthLinkStatus = 1; 
#ifdef USE_DHCP
    /* 更新DHCP状态机 */
    DHCP_state = DHCP_LINK_DOWN;
#endif  /* USE_DHCP */
    printf ("The network cable is not connected \n");
	while(1){}
  }

}
#ifdef USE_DHCP
/**
  * @brief  DHCP进程处理
  * @param  None
  * @retval None
  */
void LwIP_DHCP_task(void const * argument)
{
	struct netif *netif = (struct netif *) argument;
	OS_ERR  os_err;
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;
	struct dhcp *dhcp;   
#ifdef USE_DEBUG 
	uint8_t iptxt[20];
#endif
	while(1)
	{	
		switch (DHCP_state)
		{
			case DHCP_START:
			{
				ip_addr_set_zero_ip4(&netif->ip_addr);
				ip_addr_set_zero_ip4(&netif->netmask);
				ip_addr_set_zero_ip4(&netif->gw);
				DHCP_state = DHCP_WAIT_ADDRESS;
				dhcp_start(netif);
#ifdef USE_DEBUG
				printf("  状态：寻找DHCP服务器 ...\n");
#endif
			}
			break;

			case DHCP_WAIT_ADDRESS:
			{
				if (dhcp_supplied_address(netif)) 
				{
					DHCP_state = DHCP_ADDRESS_ASSIGNED;

					LED_BLUE; 
#ifdef USE_DEBUG 
					sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
					printf ("DHCP服务器分配的IP地址: %s\n", iptxt);        
#endif
				}
				else
				{
					dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

					/* DHCP 超时 */
					if (dhcp->tries > MAX_DHCP_TRIES)
					{
						DHCP_state = DHCP_TIMEOUT;

						/* 停止DHCP */
						dhcp_stop(netif);

						/* 使用静态地址 */
						IP_ADDR4(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
						IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
						IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
						netif_set_addr(netif, &ipaddr, &netmask, &gw);

						LED_RED;
#ifdef USE_DEBUG     
						sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
						printf("DHCP 超时 !! \n");
						printf("静态IP地址: %s\n", iptxt);   

#endif
					}
				}
			}
			break;
			case DHCP_LINK_DOWN:
			{
				/* 停止DHCP */
				dhcp_stop(netif);
				DHCP_state = DHCP_OFF; 
			}
			break;
			default: break;
		}
		OSTimeDlyHMSM( 0u, 0u, 0u, 250u,OS_OPT_TIME_HMSM_STRICT,&os_err);
	}
}
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
