/*
*********************************************************************************************************
*
*	ģ������ : MODBUS�ӻ�ģ��
*	�ļ����� : tcp_MODS_slave.c
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "modbus_reg_addr.h"
#include "modbus_register.h"
#include "main.h"

/*
*********************************************************************************************************
*	�� �� ��: ReadRegValue_03H
*	����˵��: ��ȡ���ּĴ�����ֵ
*	��    ��: reg_addr �Ĵ�����ַ
*			  reg_value ��żĴ������
*	�� �� ֵ: 1��ʾOK 0��ʾ����
*********************************************************************************************************
*/
uint8_t ReadRegValue_03H(uint16_t _RegAddr, uint16_t *_RegValue)
{
	uint16_t value;
		
	/* modbusΪ���ģʽ�������ȴ���16λ���󴫵�16λ */
	switch (_RegAddr)
	{
		case REG03_BOOT_CPU_ID0:             /* MCU��ţ�ֻ���� */
			value = g_tVar.CPU_Sn[0];	
			break;
		
		case REG03_BOOT_CPU_ID1: 
			value = g_tVar.CPU_Sn[0] >> 16;
			break;
		
		case REG03_BOOT_CPU_ID2: 
			value = g_tVar.CPU_Sn[1];
			break;
		
		case REG03_BOOT_CPU_ID3: 
			value = g_tVar.CPU_Sn[1] >> 16;
			break;
		
		case REG03_BOOT_CPU_ID4: 
			value = g_tVar.CPU_Sn[2];
			break;
		
		case REG03_BOOT_CPU_ID5: 
			value = g_tVar.CPU_Sn[2] >> 16;
			break;	
				
		case REG03_BOOT_HARD_VER:
			value = HARD_MODEL;
			break;

		case REG03_BOOT_SOFT_VER:
			value = BOOT_VERSION;
			break;		

		case REG03_BOOT_PROG_TYPE:		/* APP��������: 0 = CPU�ڲ�Flash ; 1 = QSPI Flash */
			value = g_tVar.ProgType;
			break;
		
		case REG03_BOOT_PROG_ADDR:		/* APP�����ַ 32λ */
			value = g_tVar.ProgAddr;
			break;
		
		case REG03_BOOT_PROG_SIZE:		/* APP���򳤶� 32λ */
			value = g_tVar.ProgSize;
			break;
			
		case REG03_BOOT_STATUS:		/* �豸״̬�Ĵ���  0���У�1��ʾ��æ */
			value = g_tVar.BootStatus;
			break;
		
		case REG03_BOOT_CRC32_HIGH:	/* ������CRC32У�� - δ�� */
			break;
		
		case REG03_BOOT_CRC32_LOW:	/* ������CRC32У�� - δ�� */
			break;
		
		default:
			return 0;		/* �����쳣������ 0 */	
	}

	*_RegValue = value;
	return 1;		/* ��ȡ�ɹ� */
}

/*
*********************************************************************************************************
*	�� �� ��: WriteRegValue_06H
*	����˵��: ��ȡ���ּĴ�����ֵ	д�����Ĵ���
*	��    ��: reg_addr �Ĵ�����ַ
*			  reg_value �Ĵ���ֵ
*	�� �� ֵ: 1��ʾOK 0��ʾ����
*********************************************************************************************************
*/
uint8_t WriteRegValue_06H(uint16_t _RegAddr, uint16_t _RegValue)
{		
	uint8_t err = 0;
	
	switch (_RegAddr)
	{						
		case REG03_RESET_TO_BOOT:
			if (_RegValue == 2)
			{
				/* ��λ����BOOT ���� */
			}
			else if (_RegValue == 1)
			{
				/* ��λ����APP */
				bsp_PutMsg(MSG_RESET_TO_APP, 0);
			}			
			break;
		
		case REG03_BOOT_PROG_TYPE:		/* APP��������: 0 = CPU�ڲ�Flash ; 1 = QSPI Flash */
			g_tVar.ProgType = _RegValue;
			break;
		
		case REG03_BOOT_PROG_ADDR:		/* APP�����ַ 32λ */
			g_tVar.ProgAddr = _RegValue << 16;
			break;
		
		case REG03_BOOT_PROG_ADDR + 1:	
			g_tVar.ProgAddr += _RegValue;
			break;
		
		case REG03_BOOT_PROG_SIZE:			/* APP���򳤶� 32λ */
			g_tVar.ProgSize = _RegValue << 16;
			break;
		
		case REG03_BOOT_PROG_SIZE + 1:
			g_tVar.ProgSize += _RegValue;
			bsp_PutMsg(MSG_FILE_IFNO, 0);	/* ��ʾ�ļ���Ϣ */
			break;
		
		case REG03_BOOT_ERASE_SECTOR:	/* ��������, 128KB */
			{
				if (_RegValue < 15)
				{				
					if (bsp_EraseCpuFlash(APPLICATION_ADDRESS + _RegValue * 128 * 1024) != 0)
					{
						err = 1;
					}
				}
				else
				{
					err = 1;
				}
				if (err == 0)
				{
					bsp_PutMsg(MSG_ERASE_SECTOR_OK, _RegValue);	/* �����ɹ� */
				}
				else
				{
					bsp_PutMsg(MSG_ERASE_SECTOR_ERR, _RegValue);	/* ����ʧ�� */
				}
			}
			break;
		
		case REG03_BOOT_TRANS_END:	/* �ļ��Ѵ������ - �豸�ƺ������һ������ */
			if (_RegValue == 1)
			{
				if (bsp_WriteCpuFlash(g_tVar.FlashAddr, g_tVar.ProgDatabuf, g_tVar.ProgDataLen) == 0)		/* д���һ������(�ȶ�ȡ������µ�1K���ݣ���д��) */
				{
					g_tVar.WriteLen = g_tVar.ProgDataLen;
					bsp_PutMsg(MSG_TRANS_FILE, 100);	/* ���� 100% */
					break;
				}
				else
				{
					bsp_PutMsg(MSG_TRANS_ERROR, g_tVar.FlashAddr);	/* ʧ�� */
				}	
			}
			break;

		case REG03_BOOT_STATUS:		/* �豸״̬�Ĵ���  0���У�1��ʾ��æ */
			g_tVar.BootStatus = _RegValue; 
			break;
		
		case REG03_BOOT_CRC32_HIGH:	/* ������CRC32У�� - δ�� */
			break;
		
		case REG03_BOOT_CRC32_LOW:	/* ������CRC32У�� - δ�� */
			break;
		
		default:
			err = 1;	/* �����쳣������ 0 */
			break;
	}

	if (err == 0)
	{
		return 1;		/* �ɹ� */
	}
	return 0;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
