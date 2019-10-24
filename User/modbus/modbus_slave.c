/*
*********************************************************************************************************
*
*	ģ������ : MODBUS�ӻ�ģ��
*	�ļ����� : modbus_slave.c
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "main.h"
#include "modbus_slave.h"
#include "modbus_reg_addr.h"
#include "modbus_register.h"

#define RS485_ADDR		1

#define UPGRADE_PACKET_LEN	1024

static void MODS_AnalyzeApp(void);

//static void MODS_RxTimeOut(void);

static void MODS_03H(void);;
static void MODS_06H(void);
static void MODS_10H(void);
static void MODS_15H(void);
	
void MODS_ReciveNew(uint8_t _byte);

MODS_T g_tModS;

/*
*********************************************************************************************************
*	�� �� ��: MODS_Poll
*	����˵��: �������ݰ�. �����������������á�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t MODS_Poll(uint8_t *_buf, uint16_t _len)
{
	uint16_t addr;
	uint16_t crc1;

	g_tModS.RxBuf = _buf;
	g_tModS.RxCount = _len;
	
	g_tModS.TxCount = 0;
	//*_AckBuf = g_tModS.TxBuf;

	if (g_tModS.RxCount < 4)
	{
		goto err_ret;
	}

	if (g_tModS.TCP_Flag == 0)
	{
		/* ����CRCУ��� */
		crc1 = CRC16_Modbus(g_tModS.RxBuf, g_tModS.RxCount);
		if (crc1 != 0)
		{
			#if 0	/* ��ʽ������CRC�쳣�ģ�����Ҫ�������� */
			
			#else	/* ���Գ���ԭʼ���ݷ��� */
				memcpy(g_tModS.TxBuf, g_tModS.RxBuf, g_tModS.RxCount);
				g_tModS.TxCount = g_tModS.RxCount;
			#endif
			//MODS_SendAckErr(ERR_PACKAGE);		/* ��������Ӧ�� */
			goto err_ret;
		}
	}
	else
	{
		g_tModS.RxCount += 2;
	}

	/* վ��ַ (1�ֽڣ� */
	addr = g_tModS.RxBuf[0];	/* ��1�ֽ� վ�� */
	if (addr != RS485_ADDR && addr != 0xF4)
	{
		goto err_ret;
	}

	/* ����Ӧ�ò�Э�� */
	MODS_AnalyzeApp();
	g_tModS.RxCount = 0;	/* ��������������������´�֡ͬ�� */
	return 1;
	
err_ret:
	g_tModS.RxCount = 0;	/* ��������������������´�֡ͬ�� */
	return 0;
}

#if 0

/*
*********************************************************************************************************
*	�� �� ��: MODS_ReciveNew
*	����˵��: ���ڽ����жϷ���������ñ����������յ�һ���ֽ�ʱ��ִ��һ�α�������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODS_ReciveNew(uint8_t _byte)
{
	#if 1
		if (g_tModS.RxCount < RX_BUF_SIZE)
		{
			g_tModS.RxBuf[g_tModS.RxCount++] = _byte;
		}	
	#else
		/*
			3.5���ַ���ʱ������ֻ������RTUģʽ���棬��ΪRTUģʽû�п�ʼ���ͽ�������
			�������ݰ�֮��ֻ�ܿ�ʱ���������֣�Modbus�����ڲ�ͬ�Ĳ������£����ʱ���ǲ�һ���ģ�
			���Ծ���3.5���ַ���ʱ�䣬�����ʸߣ����ʱ������С�������ʵͣ����ʱ������Ӧ�ʹ�

			4800  = 7.297ms
			9600  = 3.646ms
			19200  = 1.771ms
			38400  = 0.885ms
		*/
		uint32_t timeout;
		
		g_rtu_timeout = 0;
		
		timeout = 35000000 / g_tParam.Baud;		/* ���㳬ʱʱ�䣬��λus */
		
		/* Ӳ����ʱ�жϣ���ʱ����us Ӳ����ʱ��1����ADC, ��ʱ��2����Modbus */
		bsp_StartHardTimer(2, timeout, (void *)MODS_RxTimeOut);

		if (g_tModS.RxCount < RX_BUF_SIZE)
		{
			g_tModS.RxBuf[g_tModS.RxCount++] = _byte;
		}
	#endif
}

///*
//*********************************************************************************************************
//*	�� �� ��: MODS_RxTimeOut
//*	����˵��: ����3.5���ַ�ʱ���ִ�б������� ����ȫ�ֱ��� g_rtu_timeout = 1; ֪ͨ������ʼ���롣
//*	��    ��: ��
//*	�� �� ֵ: ��
//*********************************************************************************************************
//*/
//static void MODS_RxTimeOut(void)
//{
//	g_rtu_timeout = 1;
//}
#endif

/*
*********************************************************************************************************
*	�� �� ��: MODS_SendWithCRC
*	����˵��: ����һ������, �Զ�׷��2�ֽ�CRC
*	��    ��: _pBuf ���ݣ�
*			  _ucLen ���ݳ��ȣ�����CRC��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODS_SendWithCRC(uint8_t *_pBuf, uint16_t _ucLen)
{
	uint16_t crc;
	uint8_t buf[TX_BUF_SIZE];

	memcpy(buf, _pBuf, _ucLen);
	crc = CRC16_Modbus(_pBuf, _ucLen);
	buf[_ucLen++] = crc >> 8;
	buf[_ucLen++] = crc;

	/* ������̫������,���ݴ���g_tModS.TxBuf��,�ں������淢�� */
	memcpy(g_tModS.TxBuf, buf, _ucLen);
	g_tModS.TxCount = _ucLen;
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_SendAckErr
*	����˵��: ���ʹ���Ӧ��
*	��    ��: _ucErrCode : �������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODS_SendAckErr(uint8_t _ucErrCode)
{
	uint8_t txbuf[3];

	txbuf[0] = g_tModS.RxBuf[0];			/* 485��ַ */
	txbuf[1] = g_tModS.RxBuf[1] | 0x80;	/* �쳣�Ĺ����� */
	txbuf[2] = _ucErrCode;						/* �������(01,02,03,04) */

	MODS_SendWithCRC(txbuf, 3);
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_SendAckOk
*	����˵��: ������ȷ��Ӧ��.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODS_SendAckOk(void)
{
	uint8_t txbuf[6];
	uint8_t i;

	for (i = 0; i < 6; i++)
	{
		txbuf[i] = g_tModS.RxBuf[i];
	}
	MODS_SendWithCRC(txbuf, 6);
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_AnalyzeApp
*	����˵��: ����Ӧ�ò�Э��
*	��    ��:
*		     _DispBuf  �洢����������ʾ����ASCII�ַ�����0����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_AnalyzeApp(void)
{
	switch (g_tModS.RxBuf[1])			/* ��2���ֽ� ������ */
	{	
		case 0x03:	/* ��ȡ1�������������ּĴ��� ��һ���������ּĴ�����ȡ�õ�ǰ�Ķ�����ֵ*/
			MODS_03H();
			break;

		case 0x06:	/* д�����������ּĴ��� (�洢��CPU��FLASH�У���EEPROM�еĲ���)*/
			MODS_06H();			
			break;
			
		case 0x10:	/* д����������ּĴ��� (�洢��CPU��FLASH�У���EEPROM�еĲ���)*/
			MODS_10H();
			break;
		
		case 0x15:	/* д�ļ��Ĵ��� */
			MODS_15H();
			break;
		
		default:
			g_tModS.RspCode = RSP_ERR_CMD;
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
			break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_03H
*	����˵��: ��ȡ���ּĴ��� ��һ���������ּĴ�����ȡ�õ�ǰ�Ķ�����ֵ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_03H(void)
{
	uint16_t regaddr;
	uint16_t num;
	uint16_t value;
	uint16_t i;
	
	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	regaddr = BEBufToUint16(&g_tModS.RxBuf[2]); 
	num = BEBufToUint16(&g_tModS.RxBuf[4]);
	if (num > (TX_BUF_SIZE - 5) / 2)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;		
	}

err_ret:
	if (g_tModS.RxBuf[0] != 0x00)	/* 00�㲥��ַ��Ӧ��, FF��ַӦ��RS485_ADDR */
	{	
		if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
		{
			g_tModS.TxCount = 0;
			g_tModS.TxBuf[g_tModS.TxCount++] = RS485_ADDR;
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
			g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;
			
			{
				for (i = 0; i < num; i++)
				{
					if (ReadRegValue_03H(regaddr++, &value) == 0)
					{
						g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
						goto err_ret;
					}
					g_tModS.TxBuf[g_tModS.TxCount++] = value >> 8;
					g_tModS.TxBuf[g_tModS.TxCount++] = value;
				}
			}
			
			MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);
		
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_06H
*	����˵��: д�����Ĵ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_06H(void)
{

	/*
		д���ּĴ�����ע��06ָ��ֻ�ܲ����������ּĴ�����16ָ��������õ����������ּĴ���

		��������:
			11 �ӻ���ַ
			06 ������
			00 �Ĵ�����ַ���ֽ�
			01 �Ĵ�����ַ���ֽ�
			00 ����1���ֽ�
			01 ����1���ֽ�
			9A CRCУ����ֽ�
			9B CRCУ����ֽ�

		�ӻ���Ӧ:
			11 �ӻ���ַ
			06 ������
			00 �Ĵ�����ַ���ֽ�
			01 �Ĵ�����ַ���ֽ�
			00 ����1���ֽ�
			01 ����1���ֽ�
			1B CRCУ����ֽ�
			5A	CRCУ����ֽ�

		����:
			01 06 30 06 00 25  A710    ---- ������������Ϊ 2.5
			01 06 30 06 00 10  6707    ---- ������������Ϊ 1.0


			01 06 30 1B 00 00  F6CD    ---- SMA �˲�ϵ�� = 0 �ر��˲�
			01 06 30 1B 00 01  370D    ---- SMA �˲�ϵ�� = 1
			01 06 30 1B 00 02  770C    ---- SMA �˲�ϵ�� = 2
			01 06 30 1B 00 05  36CE    ---- SMA �˲�ϵ�� = 5

			01 06 30 07 00 01  F6CB    ---- ����ģʽ�޸�Ϊ T1
			01 06 30 07 00 02  B6CA    ---- ����ģʽ�޸�Ϊ T2

			01 06 31 00 00 00  8736    ---- ������ӿ��¼��
			01 06 31 01 00 00  D6F6    ---- �����澯��¼��

*/

	uint16_t reg;
	uint16_t value;
//	uint8_t i;
	
	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* �Ĵ���ֵ */

	if (WriteRegValue_06H(reg, value) == 0)
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_10H
*	����˵��: ����д����Ĵ���.  �����ڸ�дʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_10H(void)
{
	/*
		�ӻ���ַΪ11H�����ּĴ�������ʵ��ַΪ0001H���Ĵ����Ľ�����ַΪ0002H���ܹ�����2���Ĵ�����
		���ּĴ���0001H������Ϊ000AH�����ּĴ���0002H������Ϊ0102H��

		��������:
			11 �ӻ���ַ
			10 ������
			00 �Ĵ�����ʼ��ַ���ֽ�
			01 �Ĵ�����ʼ��ַ���ֽ�
			00 �Ĵ����������ֽ�
			02 �Ĵ����������ֽ�
			04 �ֽ���
			00 ����1���ֽ�
			0A ����1���ֽ�
			01 ����2���ֽ�
			02 ����2���ֽ�
			C6 CRCУ����ֽ�
			F0 CRCУ����ֽ�

		�ӻ���Ӧ:
			11 �ӻ���ַ
			06 ������
			00 �Ĵ�����ַ���ֽ�
			01 �Ĵ�����ַ���ֽ�
			00 ����1���ֽ�
			01 ����1���ֽ�
			1B CRCУ����ֽ�
			5A	CRCУ����ֽ�

		����:
			01 10 30 00 00 06 0C  07 DE  00 0A  00 01  00 08  00 0C  00 00     389A    ---- дʱ�� 2014-10-01 08:12:00
			01 10 30 00 00 06 0C  07 DF  00 01  00 1F  00 17  00 3B  00 39     5549    ---- дʱ�� 2015-01-31 23:59:57

	*/
	uint16_t reg_addr;
	uint16_t reg_num;
//	uint8_t byte_num;
	uint16_t value;
	uint8_t i;
	uint8_t *_pBuf;	
	
	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount < 11)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	reg_addr = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	reg_num = BEBufToUint16(&g_tModS.RxBuf[4]);	/* �Ĵ������� */
//	byte_num = g_tModS.RxBuf[6];	/* ������������ֽ��� */
	_pBuf = &g_tModS.RxBuf[7];

	for (i = 0; i < reg_num; i++)
	{
		value = BEBufToUint16(_pBuf);
		
		if (WriteRegValue_06H(reg_addr + i, value) == 0)
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
			break;
		}
		
		_pBuf += 2;
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)			/* ��ȷӦ�� */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_15H
*	����˵��: д�ļ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_15H(void)
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
		s_LenBak = RecordLen;		/* ��1�������ݳ��ȣ���Ϊ���յ�ÿ���ĳ��� */
	}
	
	Packet = RecordID + 1;
	Cpu_Offset = RecordID * s_LenBak / 1024 * 1024;			/* CPUд���ƫ�Ƶ�ַ */
	g_tVar.FlashAddr = APPLICATION_ADDRESS + Cpu_Offset;	/* ��һ������д���λ�ã�1024�������� */
	
	memcpy(&g_tVar.ProgDatabuf[(RecordID * s_LenBak) % 1024], &g_tModS.RxBuf[10], RecordLen);		/* ���1K������д��CPU flash */
	
	if ((Packet * s_LenBak) % 1024 != 0)					/* �жϵ�ǰ���ݰ��Ƿ�����1K�������� */
	{	
		g_tVar.ProgDataLen = ((RecordID * s_LenBak) % 1024) + RecordLen;		/* ��¼��ǰ��Ҫд��İ��� */
	}
	else													/* ����1K������������ʱ�ſ�ʼ��1K����д��CPU flash */
	{	
		if (bsp_WriteCpuFlash(g_tVar.FlashAddr, g_tVar.ProgDatabuf, 1024) == 0)		/* ÿ��д��1024���ֽ� */
		{
			g_tVar.WriteLen = 1024;
			bsp_PutMsg(MSG_TRANS_FILE, RecordID * s_LenBak * 100  / g_tVar.ProgSize);	/* ������Ϣ�����ݽ��� */
			g_tModS.RspCode = RSP_OK;
		}
		else
		{
			bsp_PutMsg(MSG_TRANS_ERROR, g_tVar.FlashAddr);
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
