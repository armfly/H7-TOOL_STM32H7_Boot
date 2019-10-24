/*
*********************************************************************************************************
*
*	ģ������ : MODBUS��վ�Ĵ�������
*	�ļ����� : modbus_register.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2019-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MODBUS_REGISTER_H
#define __MODBUS_REGISTER_H

uint8_t WriteRegValue_06H(uint16_t reg_addr, uint16_t reg_value);
uint8_t ReadRegValue_03H(uint16_t _reg_addr, uint16_t *_reg_value);
uint8_t ReadRegValue_04H(uint16_t _reg_addr, uint16_t *_reg_value);

uint8_t MODS_GetDIState(uint16_t _reg, uint8_t *_value);
uint8_t MODS_GetDOState(uint16_t _reg, uint8_t *_value);
uint8_t MODS_WriteRelay(uint16_t _reg, uint8_t _on);

extern uint8_t fSaveReq_06H;		/* ���������������06Hд�Ĵ������� */
extern uint8_t fResetReq_06H;		/* ��Ҫ��λCPU����Ϊ��������仯 */
extern uint8_t fSaveCalibParam;	/* ����У׼������������06H��10Hд�Ĵ������� */

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
