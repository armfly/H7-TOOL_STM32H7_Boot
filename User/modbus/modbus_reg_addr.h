/*
*********************************************************************************************************
*
*	ģ������ : MODBUS �Ĵ�����ַ���� ��Ӧ�ò㣩
*	�ļ����� : modbus_reg_addr.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MODBUS_REG_ADDR_H
#define __MODBUS_REG_ADDR_H



#define HARD_MODEL		0x0750	
#define BOOT_VERSION	0x0001		

#define REG03_RESET_TO_BOOT			0xFF7F	/* ����Ĵ�����1��λ����BOOT 2��λ����APP  3��λ */

/* BOOT �����õļĴ��� */
#define REG03_BOOT_CPU_ID0			0xFF80
#define REG03_BOOT_CPU_ID1			0xFF81
#define REG03_BOOT_CPU_ID2			0xFF82
#define REG03_BOOT_CPU_ID3			0xFF83
#define REG03_BOOT_CPU_ID4			0xFF84
#define REG03_BOOT_CPU_ID5			0xFF85
#define REG03_BOOT_HARD_VER			0xFF86	/* Ӳ���ͺţ�ֻ�������ڲ��ͺŴ��룬���ͺ�ȫ�ƣ�0xC200 ��ʾTC200*/
#define REG03_BOOT_SOFT_VER			0xFF87	/* Ӳ���汾��ֻ��������0x0100����ʾH1.00 */

#define REG03_BOOT_PROG_TYPE		0xFF88	/* APP��������: 0 = CPU�ڲ�Flash ; 1 = QSPI Flash */
#define REG03_BOOT_PROG_ADDR		0xFF89	/* APP�����ַ 32λ */
#define REG03_BOOT_PROG_SIZE		0xFF8B	/* APP���򳤶� 32λ */
/* ϵͳ���Ʋ���  
	1 - ֪ͨ��ʼ�������豸��ʼ����flash. ����ǰ��2���Ĵ������������ռ� 
	2 - ֪ͨ�豸�����������
	3 - ֪ͨϵͳ��λ
*/
#define REG03_BOOT_ERASE_SECTOR		0xFF8D	/* �������������0��ʼ��ÿ����128K */
#define REG03_BOOT_TRANS_END		0xFF8E	/* ������� 1 */

#define REG03_BOOT_STATUS			0xFFA0	/* �豸״̬�Ĵ���  0���У�1��ʾ��æ */
#define REG03_BOOT_CRC32_HIGH		0xFFA1	/* ������CRC32У�� - δ�� */
#define REG03_BOOT_CRC32_LOW		0xFFA2	/* ������CRC32У�� - δ�� */

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
