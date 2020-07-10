#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f7xx_hal.h"

/* 使能DHCP*/
#define USE_DHCP  

/*远端IP地址 */
#define DEST_IP_ADDR0   (uint8_t)192
#define DEST_IP_ADDR1   (uint8_t)168
#define DEST_IP_ADDR2   (uint8_t)1
#define DEST_IP_ADDR3   (uint8_t)198
/*远端端口 */
#define DEST_PORT       (uint8_t)7
 
/*本机静态IP地址 */
#define IP_ADDR0   (uint8_t) 192
#define IP_ADDR1   (uint8_t) 168
#define IP_ADDR2   (uint8_t) 1
#define IP_ADDR3   (uint8_t) 122
   
/*子网掩码*/
#define NETMASK_ADDR0   (uint8_t) 255
#define NETMASK_ADDR1   (uint8_t) 255
#define NETMASK_ADDR2   (uint8_t) 255
#define NETMASK_ADDR3   (uint8_t) 0

/*网关地址*/
#define GW_ADDR0   (uint8_t) 192
#define GW_ADDR1   (uint8_t) 168
#define GW_ADDR2   (uint8_t) 1
#define GW_ADDR3   (uint8_t) 1 

static void SystemClock_Config(void);
#endif /* __MAIN_H */

