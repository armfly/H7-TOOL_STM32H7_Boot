/*
*********************************************************************************************************
*
*	ģ������ : ���������̼���
*	�ļ����� : modbus_iap.c
*	��    �� : V1.0
*	˵    �� : 
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "main.h"
#include "modbus_iap.h"
#include "param.h"
#include "modbus_slave.h"

uint32_t g_FlashAddr = 0;			/* CPU��ǰд��ĵ�ַ */
static uint8_t s_Databuf[1024];	/* ��������buf�����յ�1024���ֽ�һ����д��cpu flash */
uint32_t g_DataLen = 0;			/* ���һ������ */

uint8_t g_Upgrade = 0;
uint8_t g_Erase = 0;			/* APPӦ�������������־ */

/*
*********************************************************************************************************
*	�� �� ��: WriteRegValue
*	����˵��: ��ȡ���ּĴ�����ֵ
*	��    ��: reg_addr �Ĵ�����ַ
*			  reg_value �Ĵ���ֵ
*	�� �� ֵ: 1��ʾOK 0��ʾ����
*********************************************************************************************************
*/
extern uint8_t g_fReset;
extern uint8_t g_fBaud;
uint8_t IAP_Write06H(uint16_t reg_addr, uint16_t reg_value)
{
	switch (reg_addr)
	{
		case BOOT_UPGRADE_FLAG:			/* ����������־�Ĵ��� */
			g_Upgrade = reg_value;
			if (reg_value == 0)			/* �������� */
			{
				if (bsp_WriteCpuFlash(g_FlashAddr, s_Databuf, g_DataLen) == 0)		/* д���һ������(�ȶ�ȡ������µ�1K���ݣ���д��) */
				{
					//DispMessage("���������ɹ���");
					g_tParam.UpgradeFlag = 0x55AAA55A;
					return 1;
				}
				else
				{
					return 0;
				}
			}
			break;
		
		case BOOT_BAUD:					/* �����ʼĴ��� */	
			break;
			
		case SYS_RESET:					/* ϵͳ��λ */
//			FLASH_EraseSector(ADDR_FLASH_SECTOR_3, VoltageRange_3);
//			FLASH_EraseSector(ADDR_FLASH_SECTOR_4, VoltageRange_3);
//			FLASH_EraseSector(ADDR_FLASH_SECTOR_5, VoltageRange_3);
//			FLASH_EraseSector(ADDR_FLASH_SECTOR_6, VoltageRange_3);
//			FLASH_EraseSector(ADDR_FLASH_SECTOR_7, VoltageRange_3);
//			FLASH_EraseSector(ADDR_FLASH_SECTOR_8, VoltageRange_3);
//			FLASH_EraseSector(ADDR_FLASH_SECTOR_9, VoltageRange_3);
//			FLASH_EraseSector(ADDR_FLASH_SECTOR_10, VoltageRange_3);
//			FLASH_EraseSector(ADDR_FLASH_SECTOR_11, VoltageRange_3);
			break;
		
		default:
			return 0;		/* �����쳣������ 0 */
	}

	return 1;		/* �ɹ� */
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_15H
*	����˵��: д�ļ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
extern uint8_t g_Erase;
void MODS_15H(void)
{
	/*
		��������:
			11 �ӻ���ַ
			15 ������
			00 �������ݳ���
			01 ������x���ο�����
			00 ������x���ļ���
			01 ������x����¼��
			9A ������x����¼����
			9B ������x����¼����
			18 У����ֽ�
			FC У����ֽ�
	
		�ӻ���Ӧ:
			11 �ӻ���ַ
			15 ������
			00 �������ݳ���
			01 ������x���ο�����
			00 ������x���ļ���
			01 ������x����¼��
			9A ������x����¼����
			9B ������x����¼����
			18 У����ֽ�
			FC У����ֽ�
*/
	uint8_t i;
//	uint8_t DateLen;		/* �������ݳ��� */
//	uint8_t Type;			/* �ο����� */
//	uint16_t FileID;		/* �ļ��� */
	uint16_t RecordID;		/* ��¼�� */
	uint16_t RecordLen;		/* ��¼���� */
	uint32_t Packet;		/* �ڼ������� */
	static uint16_t s_LenBak;	/* ��¼֮ǰ�����ݳ��ȣ����������֮ǰ�Ĳ�ͬ������Ϊ�����һ������,��Ҫд�� */
	uint32_t Cpu_Offset;	/* CPU��ַƫ�� */
//	char buf[50];
	
	g_tModS.RspCode = RSP_OK;	
	
//	DateLen = g_tModS.RxBuf[2];
//	Type = g_tModS.RxBuf[3];
//	FileID = BEBufToUint16(&g_tModS.RxBuf[4]); 		/* ������x���ļ��� */
	RecordID = BEBufToUint16(&g_tModS.RxBuf[6]); 		/* ������x����¼�� */
	RecordLen = BEBufToUint16(&g_tModS.RxBuf[8]); 	/* ������x����¼���� */
	
	if (RecordID == 0)				/* ��һ�����ݣ��Ͱ�flashд���ַ��Ϊ����ַ,ͬʱ����Ӧ�������� */
	{
		//g_FlashAddr = APPLICATION_ADDRESS;
		g_Erase = 1;
		s_LenBak = RecordLen;		/* ��1�������ݳ��ȣ���Ϊ���յ�ÿ���ĳ��� */
	}
	
	Packet = RecordID + 1;
	Cpu_Offset = RecordID * s_LenBak / 1024 * 1024;			/* CPUд���ƫ�Ƶ�ַ */
	g_FlashAddr = APP_BUF_ADDR + Cpu_Offset;			/* ��һ������д���λ�ã�1024�������� */
	
	memcpy(&s_Databuf[(RecordID * s_LenBak) % 1024], &g_tModS.RxBuf[10], RecordLen);		/* ���1K������д��CPU flash */
		
	if ((Packet * s_LenBak) % 1024 != 0)					/* �жϵ�ǰ���ݰ��Ƿ�����1K�������� */
	{	
		g_DataLen = ((RecordID * s_LenBak) % 1024) + RecordLen;		/* ��¼��ǰ��Ҫд��İ��� */
	}
	else													/* ����1K������������ʱ�ſ�ʼ��1K����д��CPU flash */
	{	
		if (bsp_WriteCpuFlash(g_FlashAddr, s_Databuf, 1024) == 0)		/* ÿ��д��1024���ֽ� */
		{
//			sprintf(buf, "���ȣ�%d%%", RecordID * s_LenBak * 100 % 1024);
//			DispMessage(buf);
			//DispMessage("����������...");
			g_tModS.RspCode = RSP_OK;
		}
		else
		{
			g_tModS.RspCode = RSP_ERR_WRITE;				/* д��ʧ�� */
			goto err_ret;
		}
	}

	
err_ret:
	if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
	{
		g_tModS.TxCount = 0;
		for (i = 0; i < 10; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[i];	/* Ӧ�����ݰ� */
		}
		MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/


