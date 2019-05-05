/**
  ******************************************************************************
  * @file    ap3216.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   AP3216C 驱动文件
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32F767 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./AP3216C/ap3216c.h"
#include "./usart/bsp_debug_usart.h"
#include "./i2c/i2c.h"


#define AP3216C_ERROR 		I2C_ERROR
#define AP3216C_INFO 		I2C_INFO
/**
  * @brief   写数据到AP3216C寄存器
  * @param   reg_add:寄存器地址
	* @param	 reg_data:要写入的数据
  * @retval  
  */
static void AP3216C_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
	Sensors_I2C_WriteRegister(AP3216C_ADDRESS,reg_add,1,&reg_dat);
}

/**
  * @brief   从AP3216C寄存器读取数据
  * @param   reg_add:寄存器地址
	* @param	 Read：存储数据的缓冲区
	* @param	 num：要读取的数据量
  * @retval  
  */
static void AP3216C_ReadData(uint8_t reg_add,unsigned char* Read,uint8_t num)
{
	Sensors_I2C_ReadRegister(AP3216C_ADDRESS,reg_add,num,Read);
}

/**
  * @brief   复位AP3216C传感器
  * @param   
  * @retval  
  */
void AP3216C_Reset(void)
{
  AP3216C_WriteReg(AP3216C_SYS_CONFIG, AP3216C_SW_RST_BIT);
}

/**
  * @brief   设置AP3216C环境光传感器的上下限阈值
  * @param   low_threshold：下限阈值
  * @param   high_threshold：上限阈值
  * @retval  
  */
void AP3216C_Set_ALS_Threshold(uint16_t low_threshold, uint16_t high_threshold)
{
  uint8_t resolution;
  double DR;
  
  AP3216C_ReadData(AP3216C_ALS_CONFIG, &resolution, 1);
  if((resolution >> 4) == AP3216C_ALS_RESOLUTION_036LUX_BIT)
  {
    DR = 0.36;
  }
  else if((resolution >> 4) == AP3216C_ALS_RESOLUTION_0089LUX_BIT)
  {
    DR = 0.089;
  }
  else if((resolution >> 4) == AP3216C_ALS_RESOLUTION_0022LUX_BIT)
  {
    DR = 0.022;
  }
  else if((resolution >> 4) == AP3216C_ALS_RESOLUTION_00056LUX_BIT)
  {
    DR = 0.0056;
  }
  
  high_threshold = (uint16_t)high_threshold / DR;
  low_threshold = (uint16_t)low_threshold / DR;
  
  AP3216C_WriteReg(AP3216C_ALS_LOW_THRESHOLD7_0, (low_threshold & 0xFF));
  AP3216C_WriteReg(AP3216C_ALS_LOW_THRESHOLD15_8, (low_threshold >> 8));
  AP3216C_WriteReg(AP3216C_ALS_HIGH_THRESHOLD7_0, (high_threshold & 0xFF));
  AP3216C_WriteReg(AP3216C_ALS_HIGH_THRESHOLD15_8, (high_threshold >> 8));
}

/**
  * @brief   设置AP3216C接近传感器的上下限阈值
  * @param   low_threshold：下限阈值
  * @param   high_threshold：上限阈值
  * @retval  
  */
void AP3216C_Set_PS_Threshold(uint16_t low_threshold, uint16_t high_threshold)
{
  if(low_threshold > 1020)
  {
    AP3216C_WriteReg(AP3216C_PS_LOW_THRESHOLD2_0, (low_threshold - 1020 & 0x03));
  }
  AP3216C_WriteReg(AP3216C_PS_LOW_THRESHOLD10_3, (low_threshold / 4));
  
  if(high_threshold > 1020)
  {
    AP3216C_WriteReg(AP3216C_PS_HIGH_THRESHOLD2_0, (high_threshold - 1020 & 0x03));
  }
  AP3216C_WriteReg(AP3216C_PS_HIGH_THRESHOLD10_3, (high_threshold / 4));
}

/**
  * @brief   配置AP3216C中断引脚
  * @param   
  * @param   
  * @retval  
  */
void AP3216C_INT_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 

  /*开启按键GPIO口的时钟*/
  AP_INT_GPIO_CLK_ENABLE();

  /* 选择按键1的引脚 */ 
  GPIO_InitStructure.Pin = AP_INT_GPIO_PIN;
  /* 设置引脚为输入模式 */ 
  GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    		
  /* 设置引脚不上拉也不下拉 */
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  /* 使用上面的结构体初始化按键 */
  HAL_GPIO_Init(AP_INT_GPIO_PORT, &GPIO_InitStructure); 
}

/**
  * @brief   读取AP3216C传感器的中断状态
  * @param   
  * @retval  INTstatus：第0位表示ALS中断，第1位表示PS中断
  */
uint8_t AP3216C_Get_INTStatus(void)
{
  uint8_t INTstatus;
  
  AP3216C_ReadData(AP3216C_INT_STATUS, &INTstatus, 1);
  
  return INTstatus;
}

/**
  * @brief   初始化AP3216C传感器
  * @param   
  * @retval  
  */
void AP3216C_Init(void)
{
  AP3216C_WriteReg(AP3216C_SYS_CONFIG, 0x00);//关闭所有功能
  AP3216C_WriteReg(AP3216C_SYS_CONFIG, AP3216C_SW_RST_BIT);//复位
  Delay(10);//复位后一定要延时10ms及以上，否则会出错
  AP3216C_WriteReg(AP3216C_SYS_CONFIG, AP3216C_ALS_PS_IR_ACTIVE_BIT);//开启所有功能
//  Delay(100);
  AP3216C_Set_ALS_Threshold(100, 1000);//环境光下限100，上限1000，超限触发中断
  AP3216C_Set_PS_Threshold(120, 600);//接近值下限120，上限600，超限触发中断
  AP3216C_INT_Config();
}

/**
  * @brief   读取AP3216C的环境光传感器数据
  * @param   
  * @retval  
  */
float AP3216C_ReadALS(void)
{
  uint8_t temp, buf[2];
  uint16_t ALS_RAW;
  float ALS_Data;
  
  AP3216C_ReadData(AP3216C_ALS_DATA_LOW, buf, 2);
  ALS_RAW = (buf[1] << 8) | buf[0];
    
  AP3216C_ReadData(AP3216C_ALS_CONFIG, &temp, 1);
  if((temp >> 4) == AP3216C_ALS_RESOLUTION_036LUX_BIT)
  {
    ALS_Data = ALS_RAW * 0.36;
  }
  else if((temp >> 4) == AP3216C_ALS_RESOLUTION_0089LUX_BIT)
  {
    ALS_Data = ALS_RAW * 0.089;
  }
  else if((temp >> 4) == AP3216C_ALS_RESOLUTION_0022LUX_BIT)
  {
    ALS_Data = ALS_RAW * 0.022;
  }
  else if((temp >> 4) == AP3216C_ALS_RESOLUTION_00056LUX_BIT)
  {
    ALS_Data = ALS_RAW * 0.0056;
  }
  return ALS_Data;
}

/**
  * @brief   读取AP3216C的接近传感器数据
  * @param   
  * @retval  
  */
uint16_t AP3216C_ReadPS(void)
{
  uint8_t buf[2];
  uint16_t proximity;
  
  AP3216C_ReadData(AP3216C_PS_DATA_LOW, buf, 2);
  
  if((buf[0] >> 6) || (buf[1] >> 6 ))
  {
    return proximity = 0xFFFF;//红外太强时接近传感器无效，返回0xFFFF
  }
  else
  {
    buf[0] = buf[0] & 0x0F; // PS Data LOW 4 bits
    buf[1] = buf[1] & 0x3F; // PS Data HIGH 6 bits
    proximity = (buf[1] << 4) | buf[0];
    proximity |= buf[1] >> 7;//最高位表示对象的位置，0表示远离，1表示靠近
    
    return proximity;
  }
}

/**
  * @brief   读取AP3216C的红外光传感器数据
  * @param   
  * @retval  
  */
uint16_t AP3216C_ReadIR(void)
{
  uint8_t buf[2];
  uint16_t IR_Data;
  
  AP3216C_ReadData(AP3216C_IR_DATA_LOW, buf, 2);
  buf[0] = buf[0] & 0x03; // IR Data LOW 2 bits
  IR_Data = (buf[1] << 2) | buf[0];
  
  return IR_Data;
}
