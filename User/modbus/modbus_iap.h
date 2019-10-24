/*
*********************************************************************************************************
*
*	ģ������ : RS485 MODEBUS ͨ��ģ��
*	�ļ����� : modbus_rs485.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MODBUS_IAP_H_
#define __MODBUS_IAP_H_


/* 06H boot�����ʼĴ��� �� ���������Ĵ��� */
#define SYS_RESET			0x9100
#define BOOT_BAUD			0x9101
#define BOOT_UPGRADE_FLAG	0x9102

void MODS_15H(void);
uint8_t IAP_Write06H(uint16_t reg_addr, uint16_t reg_value);

#endif


