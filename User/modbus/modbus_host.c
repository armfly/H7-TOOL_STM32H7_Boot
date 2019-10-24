/*
*********************************************************************************************************
*
*	ģ������ : MODSBUSͨ�ų��� ��������
*	�ļ����� : modbus_host.c
*	��    �� : V1.0
*	˵    �� : ����ͨ�ų���ͨ��Э�����MODBUS
*	�޸ļ�¼ :
*		�汾��  ����        ����    ˵��
*       V1.0   2015-04-18 �޸�Э��
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "param.h"
#include "main.h"
#include "main_run.h"
#include "tcp_modbus_host.h"
#include "tcp_echoclient.h"

/* ����ÿ���ӻ��ļ�����ֵ */

MODH_T g_tModH;
uint8_t g_modh_timeout = 0;

//static void MODH_RxTimeOut(void);
static void MODH_AnalyzeApp(void);

static void MODH_Read_01H(void);
static void MODH_Read_02H(void);
static void MODH_Read_03H(void);
static void MODH_Read_04H(void);
static void MODH_Read_05H(void);
static void MODH_Read_06H(void);
static void MODH_Read_10H(void);
//static void MODH_Read_30H(void);

#if 0
/*
*********************************************************************************************************
*	�� �� ��: MODH_SendPacket
*	����˵��: �������ݰ� COM1��
*	��    ��: _buf : ���ݻ�����
*			  _len : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODH_SendPacket(uint8_t *_buf, uint16_t _len)
{
	RS485_SendBuf1(_buf, _len);
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: MODH_SendAckWithCRC
*	����˵��: ����Ӧ��,�Զ���CRC.  
*	��    ��: �ޡ����������� g_tModH.TxBuf[], [g_tModH.TxCount
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODH_SendAckWithCRC(void)
{
	uint16_t crc;
	
	crc = CRC16_Modbus(g_tModH.TxBuf, g_tModH.TxCount);
	g_tModH.TxBuf[g_tModH.TxCount++] = crc >> 8;
	g_tModH.TxBuf[g_tModH.TxCount++] = crc;	
	
	memcpy(g_tClient.TxData, g_tModH.TxBuf, g_tModH.TxCount);	/* Ҫ���͵����ݷ���TCP�ͻ��˷��ͻ����� */
	g_tClient.TxCount = g_tModH.TxCount;		/* TCP��Ҫ���͵����ݳ��� */
	
	//MODH_SendPacket(g_tModH.TxBuf, g_tModH.TxCount);
	tcp_client_usersent(echoclient_pcb);		/* �û�����g_tClient.TxData�е����� */
}

/*
*********************************************************************************************************
*	�� �� ��: MODH_AnalyzeApp
*	����˵��: ����Ӧ�ò�Э�顣����Ӧ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODH_AnalyzeApp(void)
{	
	switch (g_tModH.RxBuf[1])			/* ��2���ֽ� ������ */
	{
		case 0x01:	/* ��ȡ��Ȧ״̬����ӦԶ�̿���D01/D02/D03�� */
			MODH_Read_01H();
			break;

		case 0x02:	/* ��ȡ����״̬����ӦX0��X7�� */
			MODH_Read_02H();
			break;

		case 0x03:	/* ��ȡ���ּĴ��� ��һ���������ּĴ�����ȡ�õ�ǰ�Ķ�����ֵ */
			MODH_Read_03H();
			break;

		case 0x04:	/* ��ȡ����Ĵ�������ӦA01/A02�� �� */
			MODH_Read_04H();
			break;

		case 0x05:	/* ǿ�Ƶ���Ȧ����ӦY1/Y2�� */
			MODH_Read_05H();
			break;

		case 0x06:	/* д�����Ĵ���  */
			MODH_Read_06H();
			break;		

		case 0x10:	/* д����Ĵ���  */
			MODH_Read_10H();
			break;
		
		default:
			break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODH_Send10H
*	����˵��: ����10Hָ�����д������ּĴ���. ���һ��֧��23���Ĵ�����
*	��    ��: _addr : ��վ��ַ
*			  _reg : �Ĵ������
*			  _num : �Ĵ�������n (ÿ���Ĵ���2���ֽ�) ֵ��
*			  _buf : n���Ĵ��������ݡ����� = 2 * n
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODH_Send10H(uint8_t _addr, uint16_t _reg, uint8_t _num, uint8_t *_buf)
{
	uint16_t i;
	
	g_tModH.TxCount = 0;
	g_tModH.TxBuf[g_tModH.TxCount++] = _addr;		/* ��վ��ַ */
	g_tModH.TxBuf[g_tModH.TxCount++] = 0x10;		/* ��վ��ַ */	
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg >> 8;	/* �Ĵ������ ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg;		/* �Ĵ������ ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _num >> 8;	/* �Ĵ������� ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _num;		/* �Ĵ������� ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = 2 * _num;	/* �����ֽ��� */
	
	for (i = 0; i < 2 * _num; i++)
	{
		if (g_tModH.TxCount > H_RX_BUF_SIZE - 3)
		{
			return;		/* ���ݳ������������ȣ�ֱ�Ӷ��������� */
		}
		g_tModH.TxBuf[g_tModH.TxCount++] = _buf[i];		/* ��������ݳ��� */
	}
	
	MODH_SendAckWithCRC();	/* �������ݣ��Զ���CRC */
}

/*
*********************************************************************************************************
*	�� �� ��: MODH_Send05H
*	����˵��: ����05Hָ�дǿ�õ���Ȧ
*	��    ��: _addr : ��վ��ַ
*			  _reg : �Ĵ������
*			  _value : �Ĵ���ֵ,2�ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODH_Send05H(uint8_t _addr, uint16_t _reg, uint16_t _value)
{
	g_tModH.TxCount = 0;
	g_tModH.TxBuf[g_tModH.TxCount++] = _addr;			/* ��վ��ַ */
	g_tModH.TxBuf[g_tModH.TxCount++] = 0x05;			/* ������ */	
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg >> 8;		/* �Ĵ������ ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg;			/* �Ĵ������ ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _value >> 8;	/* �Ĵ���ֵ ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _value;		/* �Ĵ���ֵ ���ֽ� */
	
	MODH_SendAckWithCRC();	/* �������ݣ��Զ���CRC */

	g_tModH.fAck05H = 0;		/* ����յ��ӻ���Ӧ���������־����Ϊ1 */
}

/*
*********************************************************************************************************
*	�� �� ��: MODH_Send06H
*	����˵��: ����06Hָ�д1�����ּĴ���
*	��    ��: _addr : ��վ��ַ
*			  _reg : �Ĵ������
*			  _value : �Ĵ���ֵ,2�ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODH_Send06H(uint8_t _addr, uint16_t _reg, uint16_t _value)
{
	g_tModH.TxCount = 0;
	g_tModH.TxBuf[g_tModH.TxCount++] = _addr;			/* ��վ��ַ */
	g_tModH.TxBuf[g_tModH.TxCount++] = 0x06;			/* ������ */	
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg >> 8;		/* �Ĵ������ ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg;			/* �Ĵ������ ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _value >> 8;	/* �Ĵ���ֵ ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _value;		/* �Ĵ���ֵ ���ֽ� */
	
	MODH_SendAckWithCRC();	/* �������ݣ��Զ���CRC */
	
	g_tModH.fAck06H = 0;		/* ����յ��ӻ���Ӧ���������־����Ϊ1 */
}

/*
*********************************************************************************************************
*	�� �� ��: MODH_Send03H
*	����˵��: ����03Hָ���ѯ1���������ּĴ���
*	��    ��: _addr : ��վ��ַ
*			  _reg : �Ĵ������
*			  _num : �Ĵ�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODH_Send03H(uint8_t _addr, uint16_t _reg, uint16_t _num)
{
	g_tModH.TxCount = 0;
	g_tModH.TxBuf[g_tModH.TxCount++] = _addr;		/* ��վ��ַ */
	g_tModH.TxBuf[g_tModH.TxCount++] = 0x03;		/* ������ */	
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg >> 8;	/* �Ĵ������ ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _reg;		/* �Ĵ������ ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _num >> 8;	/* �Ĵ������� ���ֽ� */
	g_tModH.TxBuf[g_tModH.TxCount++] = _num;		/* �Ĵ������� ���ֽ� */
	
	MODH_SendAckWithCRC();	/* �������ݣ��Զ���CRC */
	g_tModH.fAck03H = 0;	/* ����ձ�־ */
	g_tModH.RegNum = _num;	/* �Ĵ������� */
	g_tModH.Reg03H = _reg;		/* ����03Hָ���еļĴ�����ַ�������Ӧ�����ݽ��з��� */	
}


#if 0
/*
*********************************************************************************************************
*	�� �� ��: MODH_ReciveNew
*	����˵��: ���ڽ����жϷ���������ñ����������յ�һ���ֽ�ʱ��ִ��һ�α�������
*	��    ��: 
*	�� �� ֵ: 1 ��ʾ������
*********************************************************************************************************
*/
void MODH_ReciveNew(uint8_t _data)
{
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

	g_modh_timeout = 0;
	
	
	timeout = 48000000 / g_tParam.HBaud485;		/* ���㳬ʱʱ�䣬��λus 35000000*/
	
	//timeout = 35000000 / g_tParam.Baud485;		/* ���㳬ʱʱ�䣬��λus 35000000*/
	
	/* Ӳ����ʱ�жϣ���ʱ����us Ӳ����ʱ��2����MODBUS�ӻ�, ��ʱ��3����MODBUS�ӻ�����*/
	bsp_StartHardTimer(3, timeout, (void *)MODH_RxTimeOut);

	if (g_tModH.RxCount < H_RX_BUF_SIZE)
	{
		g_tModH.RxBuf[g_tModH.RxCount++] = _data;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODH_RxTimeOut
*	����˵��: ����3.5���ַ�ʱ���ִ�б������� ����ȫ�ֱ��� g_rtu_timeout = 1; ֪ͨ������ʼ���롣
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODH_RxTimeOut(void)
{
	g_modh_timeout = 1;
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: MODH_Poll
*	����˵��: ���տ�����ָ��. 1ms ��Ӧʱ�䡣
*	��    ��: ��
*	�� �� ֵ: 0 ��ʾ������ 1��ʾ�յ���ȷ����
*********************************************************************************************************
*/
void MODH_Poll(void)
{	
	uint16_t crc1;
	
	if (g_modh_timeout == 0)
	{
		/* û�г�ʱ���������ա���Ҫ���� g_tModH.RxCount */
		return ;
	}

	/* �յ�����
		05 06 00 88 04 57 3B70 (8 �ֽ�)
			05    :  ��������ĺ�վ��
			06    :  ָ��
			00 88 :  �����������ʾ�Ĵ���
			04 57 :  ����,,,ת���� 10 ������ 1111.��λ��ǰ,
			3B70  :  �����ֽ� CRC ��	��05�� 57��У��
	*/
	g_modh_timeout = 0;

	if (g_tModH.RxCount < 4)
	{
		goto err_ret;
	}

	/* ����CRCУ��� */
	crc1 = CRC16_Modbus(g_tModH.RxBuf, g_tModH.RxCount);
	if (crc1 != 0)
	{
		goto err_ret;
	}
	
	/* ����Ӧ�ò�Э�� */
	MODH_AnalyzeApp();

err_ret:
	g_tModH.RxCount = 0;	/* ��������������������´�֡ͬ�� */
}

static void MODH_Read_01H(void)
{
	;
}

static void MODH_Read_02H(void)
{
	;
}

static void MODH_Read_04H(void)
{
	;
}

static void MODH_Read_05H(void)
{
	if (g_tModH.RxCount > 0)
	{
		if (g_tModH.RxBuf[0] == SlaveAddr)			//g_tParam.HAddr485
		{
			g_tModH.fAck05H = 1;		/* ���յ�Ӧ�� */
		}
	};
}

/*
*********************************************************************************************************
*	�� �� ��: MODH_Read_06H
*	����˵��: ����06Hָ���Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODH_Read_06H(void)
{
	if (g_tModH.RxCount > 0)
	{
		if (g_tModH.RxBuf[0] == SlaveAddr)		//g_tParam.HAddr485
		{
			g_tModH.fAck06H = 1;		/* ���յ�Ӧ�� */
		}
	}
}


/*
*********************************************************************************************************
*	�� �� ��: MODH_Read_03H
*	����˵��: ����03Hָ���Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODH_Read_03H(void)
{
	#if 0
	uint8_t bytes;
	uint8_t *p;
	
	if (g_tModH.RxCount > 0)
	{
		//BEEP_KeyTone();

		bytes = g_tModH.RxBuf[2];	/* ���ݳ��� �ֽ��� */				
		switch (g_tModH.Reg03H)
		{
			case REG_P01:
				if (bytes == 32)
				{
					p = &g_tModH.RxBuf[3];	
					
					g_tVar.P01 = BEBufToUint16(p); p += 2;	/* �Ĵ��� */	
					g_tVar.P02 = BEBufToUint16(p); p += 2;	/* �Ĵ��� */	
					g_tVar.P03 = BEBufToUint16(p); p += 2;
					g_tVar.P04 = BEBufToUint16(p); p += 2;
					g_tVar.P05 = BEBufToUint16(p); p += 2;
					g_tVar.P06 = BEBufToUint16(p); p += 2;
					g_tVar.P07 = BEBufToUint16(p); p += 2;
					g_tVar.P08 = BEBufToUint16(p); p += 2;
					g_tVar.P09 = BEBufToUint16(p); p += 2;
					
					g_tVar.P10 = BEBufToUint16(p); p += 2;	
					g_tVar.P11 = BEBufToUint16(p); p += 2;
					g_tVar.P12 = BEBufToUint16(p); p += 2;
					g_tVar.P13 = BEBufToUint16(p); p += 2;
					g_tVar.P14 = BEBufToUint16(p); p += 2;
					g_tVar.P15 = BEBufToUint16(p); p += 2;
					g_tVar.P16 = BEBufToUint16(p); p += 2;
				//	bsp_PutMsg(MSG_READ_FINISH, 0);			/* ����ʱ��ȡһ�����ݣ�������Ϣ��ʾ��ȡ��� */
					g_tModH.fAck03H = 1;
				}
				break;
			
			case REG_P01 + 16:
				if (bytes == 32)
				{
					p = &g_tModH.RxBuf[3];	
					
					g_tVar.P17 = BEBufToUint16(p); p += 2;
					g_tVar.P18 = BEBufToUint16(p); p += 2;
					g_tVar.P19 = BEBufToUint16(p); p += 2;
						
					g_tVar.P20 = BEBufToUint16(p); p += 2;
					g_tVar.P21 = BEBufToUint16(p); p += 2;
					g_tVar.P22 = BEBufToUint16(p); p += 2;
					g_tVar.P23 = BEBufToUint16(p); p += 2;
					g_tVar.P24 = BEBufToUint16(p); p += 2;
					g_tVar.P25 = BEBufToUint16(p); p += 2;
					g_tVar.P26 = BEBufToUint16(p); p += 2;
					g_tVar.P27 = BEBufToUint16(p); p += 2;
					g_tVar.P28 = BEBufToUint16(p); p += 2;
					g_tVar.P29 = BEBufToUint16(p); p += 2;

					g_tVar.P30 = BEBufToUint16(p); p += 2;
					g_tVar.P31 = BEBufToUint16(p); p += 2;
					g_tVar.P32 = BEBufToUint16(p); p += 2;

					g_tModH.fAck03H = 2;
				}
				break;
				
			case REG_P01 + 32:
				if (bytes == 32)
				{
					p = &g_tModH.RxBuf[3];	
					
					g_tVar.P33 = BEBufToUint16(p); p += 2;
					g_tVar.P34 = BEBufToUint16(p); p += 2;
					g_tVar.P35 = BEBufToUint16(p); p += 2;
					g_tVar.P36 = BEBufToUint16(p); p += 2;
					g_tVar.P37 = BEBufToUint16(p); p += 2;
					g_tVar.P38 = BEBufToUint16(p); p += 2;
					g_tVar.P39 = BEBufToUint16(p); p += 2;

					g_tVar.P40 = BEBufToUint16(p); p += 2;
					g_tVar.P41 = BEBufToUint16(p); p += 2;
					g_tVar.P42 = BEBufToUint16(p); p += 2;
					g_tVar.P43 = BEBufToUint16(p); p += 2;
					g_tVar.P44 = BEBufToUint16(p); p += 2;
					g_tVar.P45 = BEBufToUint16(p); p += 2;
					g_tVar.P46 = BEBufToUint16(p); p += 2;
					g_tVar.P47 = BEBufToUint16(p); p += 2;
					g_tVar.P48 = BEBufToUint16(p); p += 2;

					g_tModH.fAck03H = 3;
				}
				break;
		
			case REG_P01 + 48:
				if (bytes == 32)
				{
					p = &g_tModH.RxBuf[3];	
					
					g_tVar.P49 = BEBufToUint16(p); p += 2;	

					g_tVar.P50 = BEBufToUint16(p); p += 2;
					g_tVar.P51 = BEBufToUint16(p); p += 2;
					g_tVar.P52 = BEBufToUint16(p); p += 2;
					g_tVar.P53 = BEBufToUint16(p); p += 2;
					g_tVar.P54 = BEBufToUint16(p); p += 2;
					g_tVar.P55 = BEBufToUint16(p); p += 2;
					g_tVar.P56 = BEBufToUint16(p); p += 2;
					g_tVar.P57 = BEBufToUint16(p); p += 2;
					g_tVar.P58 = BEBufToUint16(p); p += 2;
					g_tVar.P59 = BEBufToUint16(p); p += 2;
						
					g_tVar.P60 = BEBufToUint16(p); p += 2;
					g_tVar.P61 = BEBufToUint16(p); p += 2;
					g_tVar.P62 = BEBufToUint16(p); p += 2;
					g_tVar.P63 = BEBufToUint16(p); p += 2;
					g_tVar.P64 = BEBufToUint16(p); p += 2;

					g_tModH.fAck03H = 4;
				}
				break;	
				
			case REG_H40:				/* �����д��� */
				if (bytes == 28)		/* ɨ��� */
				{
					p = &g_tModH.RxBuf[3];	
					
					g_tVar.H40 = BEBufToUint32(p); p += 4;	
					g_tVar.H42 = BEBufToUint32(p); p += 4;
					g_tVar.H44 = BEBufToUint16(p); p += 2;
					g_tVar.H45 = BEBufToUint16(p); p += 2;
					g_tVar.H46 = BEBufToUint16(p); p += 2;
					g_tVar.H47 = BEBufToUint16(p); p += 2;
					g_tVar.H48 = BEBufToUint16(p); p += 2;
					g_tVar.H49 = BEBufToUint16(p); p += 2;
					g_tVar.H4A = BEBufToUint16(p); p += 2;
					g_tVar.H4B = BEBufToUint16(p); p += 2;
					
					g_tVar.H4C = BEBufToUint16(p); p += 2;	
					g_tVar.H4D = BEBufToUint16(p); p += 2;

					g_tModH.fAck03H = 5;
				}
			
			#if 0
				if (bytes == 4)
				{
					p = &g_tModH.RxBuf[3];	
					g_tVar.H40 = BEBufToUint32(p);
					g_tModH.fAck03H = 1;
				}
			#endif
				break;
				
			case REG_H44:				/* ��������,��ѯ�������ϱ��Ĺ���ʱ���Զ����Խ��淵�����˵� */
				if (bytes == 2)
				{
					p = &g_tModH.RxBuf[3];	
					g_tVar.H44 = BEBufToUint16(p);
					
					g_tModH.fAck03H = 1;
				}
				break;	
				
		#if 0			
			case REG_H4A:				/* ����״̬��ѯ */
				if (bytes == 2)
				{
					p = &g_tModH.RxBuf[3];	
					g_tVar.H4A = BEBufToUint16(p);
					g_tModH.fAck03H = 1;
				}
				break;

			case REG_H42:			 	/* ά�������еĴ��� */
				if (bytes == 4)
				{
					p = &g_tModH.RxBuf[3];	
					g_tVar.H42 = BEBufToUint32(p);
				}
				break;
			
			case REG_H47:				/* ϵͳ��ѯ1 */
				break;
				
		    case REG_H49:				/* ϵͳ��ѯ2 */
				break;
			
			case REG_H48:				/* ϵͳ��ѯ3 */
				break;
			
			case REG_H4D:				/* ϵͳ��ѯ4 */
				break;
			
			case REG_H4C:				/* ϵͳ��ѯ5 */
				break;
			
			case REG_P16:				/* ϵͳ��ѯ6 */
				break;
			
			case REG_P18:				/* ϵͳ��ѯ7 */
				break;
			
			case REG_H4B:				/* ϵͳ��ѯ8 */
				break;
			
			case REG_P63: 				/* ϵͳ�汾 */
				if (bytes == 4)
				{
					p = &g_tModH.RxBuf[3];	
					g_tVar.P63 = BEBufToUint16(p); p += 2;
					g_tVar.P64 = BEBufToUint16(p);
				}
		#endif
		}
	}
	#endif
}

/*
*********************************************************************************************************
*	�� �� ��: MODH_Read_10H
*	����˵��: ����10Hָ���Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODH_Read_10H(void)
{
	/*
		10Hָ���Ӧ��:
			�ӻ���ַ                11
			������                  10
			�Ĵ�����ʼ��ַ���ֽ�	00
			�Ĵ�����ʼ��ַ���ֽ�    01
			�Ĵ����������ֽ�        00
			�Ĵ����������ֽ�        02
			CRCУ����ֽ�           12
			CRCУ����ֽ�           98
	*/
	//uint8_t addr;
	//uint8_t bytes;
	//uint8_t reg;

	if (g_tModH.RxCount > 0)
	{
//		BEEP_KeyTone();
		
		/* ������Ϣ. ��Ϣ���ǼĴ�����ַ������ */
		#if 0
		{
			uint8_t buf[4];
			
			buf[0] = g_tModH.RxBuf[0];	/* �ӻ���ַ */
			buf[1] = g_tModH.RxBuf[5];	/* �Ĵ������� */
			buf[2] = g_tModH.RxBuf[2];	/* �Ĵ�����ַ��16λ */
			buf[3] = g_tModH.RxBuf[3];	/* �Ĵ�����ַ��16λ */
			bsp_PutMsg(MSG_RX_10H, (uint32_t)buf);
		}
		#endif
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODH_WriteParam
*	����˵��: ��������. ͨ������06Hָ��ʵ�֣�����֮�󣬵ȴ��ӻ�Ӧ��ѭ��10��д����
*	��    ��: ��
*	�� �� ֵ: 1 ��ʾ�ɹ���0 ��ʾʧ�ܣ�ͨ�ų�ʱ�򱻾ܾ���
*********************************************************************************************************
*/
extern uint8_t g_fTcpState;
uint8_t MODH_WriteParam(uint16_t _reg, uint16_t _value)
{
	int32_t time1;
	uint8_t i;
	
	for (i = 0; i < 3; i++)
	{
		MODH_Send06H (SLAVE_ADDR, _reg, _value);
		/* ��TCP״̬��־����Ϊ�ǽ��յ�����״̬ */
		g_tClient.TcpState = g_fTcpState;		/* 2016-10-17 add by xd */
		time1 = bsp_GetRunTime();	/* ��¼����͵�ʱ�� */
		
		while (1)
		{
			bsp_Idle();
			
			/* ��ʱ���� 150ms ������Ϊ�쳣,  100ms��ʱ�ᳬʱ������û�н��յ���PC���Ӧ��ʱ������ʮ��һ�ٶม�� */
			if (bsp_CheckRunTime(time1) > 200)		//100
			{
				break;
			}
			
			if (g_tClient.TcpState == 2)
			{
				break;
			}
		}
		
		if (g_tClient.TcpState == 2)
		{
			break;
		}
	}
	
	if (g_tClient.TcpState == 2)		/* ���յ����� */
	{
		return 1;
	}
	else
	{
		return 0;		/* ͨ�ų�ʱ�� */
	}
}


#if 0
/*
*********************************************************************************************************
*	�� �� ��: MODH_Read_30H
*	����˵��: ����30Hָ���Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODH_Read_30H(void)
{
	uint8_t addr;
	uint8_t bytes;
	
	if (g_tModH.RxCount > 0)
	{
		//BEEP_KeyTone();
		
		addr = g_tModH.RxBuf[0];
		bytes = g_tModH.RxBuf[2];
#if 0		
		if (g_tModH.Reg30H == SREG_STATUS)	/* ������ѯ״̬�� - ʵʱ���� - ����Ա������ */
		{		
			if ((addr < DEV_NUM_MAX) && (addr > 0) && (addr != ADDR_FF) && (bytes >= 10))
			{
				addr--;

				g_tVar.Dev[addr].CommErr = 0;	/* ͨ�Ŵ������� */
				
				g_tVar.Dev[addr].Status = BEBufToUint16(&g_tModH.RxBuf[3]);

				if (bytes >= 4)
				{
					g_tVar.Dev[addr].CountNow = BEBufToUint16(&g_tModH.RxBuf[5]);
				}
				if (bytes >= 6)
				{			
					g_tVar.Dev[addr].GH[0] = BEBufToUint16(&g_tModH.RxBuf[7]);
				}
				if (bytes >= 8)
				{				
					g_tVar.Dev[addr].GH[1] = BEBufToUint16(&g_tModH.RxBuf[9]);
				}
				if (bytes >= 10)		
				{
					g_tVar.Dev[addr].GH[2] = BEBufToUint16(&g_tModH.RxBuf[11]);
				}
				
				bsp_PutMsg(MSG_433M_OK, addr + 1);	/* 433M ͨ��OK, ֪ͨ��ؽ���ˢ��ͼ�� */		
			}
		}
#endif
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODH_WriteParam_05H
*	����˵��: ��������. ͨ������05Hָ��ʵ�֣�����֮�󣬵ȴ��ӻ�Ӧ��
*	��    ��: ��
*	�� �� ֵ: 1 ��ʾ�ɹ���0 ��ʾʧ�ܣ�ͨ�ų�ʱ�򱻾ܾ���
*********************************************************************************************************
*/
uint8_t MODH_WriteParam_05H(uint16_t _reg, uint16_t _value)
{
	int32_t time1;
	uint8_t i;

	for (i = 0; i < 10; i++)
	{
		MODH_Send05H (SlaveAddr, _reg, _value);
		time1 = bsp_GetRunTime();	/* ��¼����͵�ʱ�� */
		
		g_LastTime = bsp_GetRunTime();			/* 300ms */
		while (1)
		{
			bsp_Idle();
			
			/* ��ʱ���� 150ms ������Ϊ�쳣 */
			if (bsp_CheckRunTime(time1) > 150)		//100
			{
				g_LastTime = bsp_GetRunTime();
				break;	/* ͨ�ų�ʱ�� */
			}
			
			if (g_tModH.fAck05H > 0)
			{
				break;
			}
		}
		
		if (g_tModH.fAck05H > 0)
		{
			break;
		}
	}
	
	if (g_tModH.fAck05H == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/* ��һ���Ĵ��� */
uint8_t MODH_ReadParam_03H(uint16_t _reg, uint16_t _num)
{
	int32_t time1;
	uint8_t i;
	
	for (i = 0; i < 10; i++)
	{
		MODH_Send03H (SlaveAddr, _reg, _num);
		time1 = bsp_GetRunTime();	/* ��¼����͵�ʱ�� */
		
		g_LastTime = bsp_GetRunTime();			/* 300ms */
		while (1)
		{
			bsp_Idle();
			
			/* ��ʱ���� 150ms ������Ϊ�쳣 */
			if (bsp_CheckRunTime(time1) > 150)		//100
			{
				g_LastTime = bsp_GetRunTime();
				break;	/* ͨ�ų�ʱ�� */
			}
			
			if (g_tModH.fAck03H > 0)
			{
				break;
			}
		}
		
		if (g_tModH.fAck03H > 0)
		{
			break;
		}
	}
	
	if (g_tModH.fAck03H == 0)
	{
		return 0;
	}
	else 
	{
		return 1;	/* д��03H�����ɹ� */
	}
}

/* ��64���Ĵ����Ĳ���,��10�� */
uint8_t MODH_ReadParam_03H_16(uint16_t _reg, uint16_t _num)
{
	int32_t time1;
	uint8_t i;
	uint8_t State = 0;
	
	while (1)
	{	
		for (i = 0; i < 10; i++)
		{
			/* ����ĵ�ַ�Ͷ�0x55,���������16��������32�ֽڣ�*/
			MODH_Send03H (SlaveAddr, _reg + State * _num, _num);		/* ���Ͳ�ѯ���� */
			
			time1 = bsp_GetRunTime();	/* ��¼����͵�ʱ�� */
			
			g_LastTime = bsp_GetRunTime();			/* 300ms */
			while (1)
			{
				bsp_Idle();
				
				/* ��ʱ���� 200ms ������Ϊ�쳣 */
				if (bsp_CheckRunTime(time1) > 200)		//200
				{
					g_LastTime = bsp_GetRunTime();
					break;	/* ͨ�ų�ʱ�� */
				}
				
				if (g_tModH.fAck03H > 0)
				{
					State++;
					break;
				}
			}
			
			if (g_tModH.fAck03H > 0)
			{
				break;
			}
		}
		
		if (g_tModH.fAck03H == 0)
		{
			return 0;
		}
		
		if (State == 4)
		{
			return 1;
		}
	}
}

/* ��ѯ��14������ */
uint8_t MODH_CycleRead_03H(uint16_t _reg)
{
	int32_t time1;
	
	/* ����ĵ�ַ�Ͷ�0x55,���������16��������32�ֽڣ�*/
	MODH_Send03H (SlaveAddr, _reg, 14);		/* ���Ͳ�ѯ���� */
	
	time1 = bsp_GetRunTime();	/* ��¼����͵�ʱ�� */
	
	g_LastTime = bsp_GetRunTime();		/* 300ms */
	while (1)
	{
		bsp_Idle();
		
		/* ��ʱ���� 200ms ������Ϊ�쳣 */
		if (bsp_CheckRunTime(time1) > 200)	
		{
			g_LastTime = bsp_GetRunTime();
			return 0;	/* ͨ�ų�ʱ�� */
		}
		
		if (g_tModH.fAck03H > 0)
		{
			break;
		}
	}
	
	return 1;
}


/*
*********************************************************************************************************
*	�� �� ��: MODH_AllowRun_05H
*	����˵��: �����ֹ����ר��05ָ�������bsp_Idle(); ���ý��������
*	��    ��: ��
*	�� �� ֵ: 1 ��ʾ�ɹ���0 ��ʾʧ�ܣ�ͨ�ų�ʱ�򱻾ܾ���
*********************************************************************************************************
*/
uint8_t MODH_AllowRun_05H(uint16_t _reg, uint16_t _value)
{
	int32_t time1;
	uint8_t i;

	for (i = 0; i < 10; i++)
	{
		MODH_Send05H (SlaveAddr, _reg, _value);
		time1 = bsp_GetRunTime();	/* ��¼����͵�ʱ�� */
	
		g_LastTime = bsp_GetRunTime();
		while (1)
		{
			//bsp_Idle();
			MODH_Poll();
			MODS_Poll();
			
			/* ��ʱ���� 150ms ������Ϊ�쳣 */
			if (bsp_CheckRunTime(time1) > 150)		//100
			{
				g_LastTime = bsp_GetRunTime();
				break;	/* ͨ�ų�ʱ�� */
			}
			
			if (g_tModH.fAck05H > 0)
			{
				break;
			}
		}
		
		if (g_tModH.fAck05H > 0)
		{
			break;
		}
	}
	
	if (g_tModH.fAck05H == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

