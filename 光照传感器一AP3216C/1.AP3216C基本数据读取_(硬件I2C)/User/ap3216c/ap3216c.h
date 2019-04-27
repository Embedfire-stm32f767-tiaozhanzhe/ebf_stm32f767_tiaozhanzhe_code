#ifndef __AP3216C_H
#define __AP3216C_H
#include "stm32f7xx.h"

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

void AP3216C_WriteReg(uint8_t reg_add,uint8_t reg_dat);
void AP3216C_ReadData(uint8_t reg_add,unsigned char* Read,uint8_t num);

void AP3216CReadALS(uint16_t *alsData);
void AP3216CReadPS(uint16_t *psData);
void AP3216CReadIR(uint16_t *irData);
void AP3216C_ReturnTemp(float*Temperature);
void AP3216C_Init(void);


#endif  /*__AP3216C*/
