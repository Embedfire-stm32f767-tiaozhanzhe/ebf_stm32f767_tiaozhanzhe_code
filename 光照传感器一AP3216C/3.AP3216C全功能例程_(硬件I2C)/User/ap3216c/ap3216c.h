#ifndef __AP3216C_H
#define __AP3216C_H
#include "stm32f7xx.h"

//中断引脚定义
/*******************************************************/

#define AP_INT_GPIO_PORT                GPIOE
#define AP_INT_GPIO_CLK_ENABLE()        __GPIOE_CLK_ENABLE();
#define AP_INT_GPIO_PIN                 GPIO_PIN_3

/* 读中断引脚状态 */
#define AP_INT_Read()    HAL_GPIO_ReadPin(AP_INT_GPIO_PORT, AP_INT_GPIO_PIN)

/*******************************************************/

// AP3216C, Standard address 0x1E
#define AP3216C_ADDRESS                 0x3C
#define AP3216C_WHO_AM_I                0x75

#define AP3216C_SYS_CONFIG              0x00
#define AP3216C_INT_STATUS              0x01
#define AP3216C_INT_CLEAR_MANNER        0x02
#define AP3216C_IR_DATA_LOW             0x0A
#define AP3216C_IR_DATA_HIGH            0x0B
#define AP3216C_ALS_DATA_LOW            0x0C
#define AP3216C_ALS_DATA_HIGH           0x0D
#define AP3216C_PS_DATA_LOW             0x0C
#define AP3216C_PS_DATA_HIGH            0x0F

#define AP3216C_ALS_CONFIG              0x10
#define AP3216C_ALS_CALIBRAE            0x19
#define AP3216C_ALS_LOW_THRESHOLD7_0    0x1A
#define AP3216C_ALS_LOW_THRESHOLD15_8   0x1B
#define AP3216C_ALS_HIGH_THRESHOLD7_0   0x1C
#define AP3216C_ALS_HIGH_THRESHOLD15_8  0x1D

#define AP3216C_PS_CONFIG               0x20
#define AP3216C_PS_LED_DRIVER           0x21
#define AP3216C_PS_INT_FORM             0x22
#define AP3216C_PS_MEAN_TIME            0x23 
#define AP3216C_PS_LED_WAITING_TIME     0x24
#define AP3216C_PS_CALIBRATION_L        0x28
#define AP3216C_PS_CALIBRATION_H        0x29
#define AP3216C_PS_LOW_THRESHOLD2_0     0x2A
#define AP3216C_PS_LOW_THRESHOLD10_3    0x2B
#define AP3216C_PS_HIGH_THRESHOLD2_0    0x2C
#define AP3216C_PS_HIGH_THRESHOLD10_3   0x2D

#define AP3216C_PWR_DOWN_BIT          0
#define AP3216C_ALS_ACTIVE_BIT        1
#define AP3216C_PS_IR_ACTIVE_BIT      2
#define AP3216C_ALS_PS_IR_ACTIVE_BIT  3
#define AP3216C_SW_RST_BIT            4
#define AP3216C_ALS_ONCE_BIT          5
#define AP3216C_PS_IR_ONCE_BIT        6
#define AP3216C_ALS_PS_IR_ONCE_BIT    7

#define AP3216C_ALS_INT_STATUS_BIT    0
#define AP3216C_PS_INT_STATUS_BIT     1
#define AP3216C_INT_CLEAR_MANNER_BIT  0

#define AP3216C_ALS_RESOLUTION_036LUX_BIT   0
#define AP3216C_ALS_RESOLUTION_0089LUX_BIT  1
#define AP3216C_ALS_RESOLUTION_0022LUX_BIT  2
#define AP3216C_ALS_RESOLUTION_00056LUX_BIT 3

#define AP3216C_ALS_INT_FILTER_1_BIT  0
#define AP3216C_ALS_INT_FILTER_4_BIT  1
#define AP3216C_ALS_INT_FILTER_8_BIT  2
#define AP3216C_ALS_INT_FILTER_12_BIT 3
#define AP3216C_ALS_INT_FILTER_16_BIT 4
#define AP3216C_ALS_INT_FILTER_60_BIT 5

#define AP3216C_PS_IR_INTERGRATION_TIME_1T_BIT 0
#define AP3216C_PS_IR_INTERGRATION_TIME_2T_BIT 1
#define AP3216C_PS_IR_INTERGRATION_TIME_15T_BIT 14
#define AP3216C_PS_IR_INTERGRATION_TIME_16T_BIT 15

#define AP3216C_PS_GAIN_X1_BIT 0
#define AP3216C_PS_GAIN_X2_BIT 1
#define AP3216C_PS_GAIN_X4_BIT 2
#define AP3216C_PS_GAIN_X8_BIT 3

#define AP3216C_PS_INT_FILTER_1_BIT 0
#define AP3216C_PS_INT_FILTER_2_BIT 1
#define AP3216C_PS_INT_FILTER_4_BIT 2
#define AP3216C_PS_INT_FILTER_8_BIT 3

#define AP3216C_PS_LED_PULSE_0_BIT 0
#define AP3216C_PS_LED_PULSE_1_BIT 1
#define AP3216C_PS_LED_PULSE_2_BIT 2
#define AP3216C_PS_LED_PULSE_3_BIT 3

#define AP3216C_PS_LED_DRIVER_RATIO_0167_BIT 0
#define AP3216C_PS_LED_DRIVER_RATIO_0333_BIT 1
#define AP3216C_PS_LED_DRIVER_RATIO_0667_BIT 2
#define AP3216C_PS_LED_DRIVER_RATIO_1000_BIT 3

#define AP3216C_PS_INT_MOD_BIT 0

static void AP3216C_WriteReg(uint8_t reg_add,uint8_t reg_dat);
static void AP3216C_ReadData(uint8_t reg_add,unsigned char* Read,uint8_t num);

void AP3216C_Set_ALS_Threshold(uint16_t low_threshold, uint16_t high_threshold);
void AP3216C_Set_PS_Threshold(uint16_t low_threshold, uint16_t high_threshold);
uint8_t AP3216C_Get_INTStatus(void);
void AP3216C_INT_Config(void);

float AP3216C_ReadALS(void);
uint16_t AP3216C_ReadPS(void);
uint16_t AP3216C_ReadIR(void);
void AP3216C_ReturnTemp(float*Temperature);
void AP3216C_Init(void);


#endif  /*__AP3216C*/
