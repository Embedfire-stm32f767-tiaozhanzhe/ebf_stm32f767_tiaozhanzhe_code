/**
  ******************************************************************************
  * @file    bsp_advance_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   高级控制定时器互补输出范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F767 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_lptim.h"

LPTIM_HandleTypeDef  LPTIM_Handle;
/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void LPTIM_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启定时器相关的GPIO外设时钟*/
	LPTIM1_OUT_GPIO_CLK_ENABLE();

	/* 定时器功能引脚初始化 */															   
	GPIO_InitStructure.Pin = LPTIM1_OUT_PIN;	
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;    
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW; 	
	GPIO_InitStructure.Alternate = LPTIM1_OUT_AF;
	HAL_GPIO_Init(LPTIM1_OUT_GPIO_PORT, &GPIO_InitStructure);	
}

/**
  * @brief  配置TIM输出PWM
  * @param  无
  * @retval 无
  */
static void LPTIM_Mode_Config(void)
{
	RCC_PeriphCLKInitTypeDef        RCC_PeriphCLKInitStruct;	
	uint32_t PeriodValue;
	uint32_t PulseValue;

	/* 选择LSE时钟作为LPTIM时钟源 */
	RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LPTIM1;
	RCC_PeriphCLKInitStruct.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_LSE;  
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);	
	// 开启LPTIM1时钟
	__LPTIM1_CLK_ENABLE(); 
	/* 定义定时器的句柄即确定定时器寄存器的基地址*/
	LPTIM_Handle.Instance = LPTIM1;
	// 高级控制定时器时钟源LPTIM_CLK = LSE=32.768KHz 
	LPTIM_Handle.Init.Clock.Source    = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
	// 定时器时钟分频系数
	LPTIM_Handle.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;  	
	// 定时器计数源，内部
	LPTIM_Handle.Init.CounterSource   = LPTIM_COUNTERSOURCE_INTERNAL;
	// 触发源，软件触发
	LPTIM_Handle.Init.Trigger.Source  = LPTIM_TRIGSOURCE_SOFTWARE; 
	// 定时器输出极性
	LPTIM_Handle.Init.OutputPolarity  = LPTIM_OUTPUTPOLARITY_HIGH;
	// 定时器更新方式
	LPTIM_Handle.Init.UpdateMode      = LPTIM_UPDATE_IMMEDIATE;
	// 初始化定时器LPTIM
	HAL_LPTIM_Init(&LPTIM_Handle);

	/*PWM模式配置*/
	/*当定时器从0计数到99，即为100次，为一个定时周期PWM周期，32.768KHz/100 = 327.68Hz*/
	PeriodValue = 100-1;
	/*PWM脉冲为周期一半即50% */
	PulseValue = 50-1;
	HAL_LPTIM_PWM_Start(&LPTIM_Handle, PeriodValue, PulseValue);
}

/**
  * @brief  低功耗定时器在低功耗模式输出PWM
  * @param  无
  * @retval 无
  */
void LPTIM_PWM_OUT(void)
{
	LPTIM_GPIO_Config();	
	
	LPTIM_Mode_Config();
	/* 进入低功耗模式 */
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	/* 等待系统被按键KEY2唤醒，退出低功耗模式后停止输出PWM */
    HAL_LPTIM_PWM_Stop(&LPTIM_Handle);
}

/*********************************************END OF FILE**********************/
