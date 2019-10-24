/*
*********************************************************************************************************
*
*	模块名称 : DS18B20 驱动模块(1-wire 数字温度传感器）
*	文件名称 : bsp_ds18b20.h
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_DS18B20_H
#define _BSP_DS18B20_H

void bsp_InitDS18B20(void);
int16_t DS18B20_ReadTempReg(void);
int16_t DS18B20_ReadTempByID(uint8_t *_id);
uint8_t DS18B20_ReadID(uint8_t *_id);
uint8_t DS18B20_ReadTempRegCRC(int16_t *read_temp);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
