#ifndef __ADVANCE_TIM_H
#define	__ADVANCE_TIM_H

#include "stm32f7xx.h"


//Òý½Å¶¨Òå
/*******************************************************/
#define LPTIM1_OUT_PIN            		GPIO_PIN_13                 
#define LPTIM1_OUT_GPIO_PORT            GPIOD                      
#define LPTIM1_OUT_GPIO_CLK_ENABLE()    __GPIOD_CLK_ENABLE()
#define LPTIM1_OUT_AF	                GPIO_AF3_LPTIM1                 


extern TIM_HandleTypeDef TIM_TimeBaseStructure;

void LPTIM_PWM_OUT(void);

#endif /* __ADVANCE_TIM_H */

