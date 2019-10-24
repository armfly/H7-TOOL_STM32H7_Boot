/*
*********************************************************************************************************
*
*	ģ������ : MODBUS��վͨ��ģ��
*	�ļ����� : modbus_slave.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2019-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MODBUS_SLAVE_H
#define __MODBUS_SLAVE_H

/* RTU Ӧ����� */
#define RSP_OK				0		/* �ɹ� */
#define RSP_ERR_CMD			0x01	/* ��֧�ֵĹ����� */
#define RSP_ERR_REG_ADDR	0x02	/* �Ĵ�����ַ���� */
#define RSP_ERR_VALUE		0x03	/* ����ֵ����� */
#define RSP_ERR_WRITE		0x04	/* д��ʧ�� */

#define ERR_PACKAGE			0x05	/* �Լ���������Ӧ�� */

#define RX_BUF_SIZE	     (2 * 1024)
#define TX_BUF_SIZE      (2 * 1024)

typedef struct
{
	uint8_t *RxBuf;
	uint16_t RxCount;
	uint8_t RxStatus;
	uint8_t RxNewFlag;

	uint8_t RspCode;

	uint8_t TxBuf[TX_BUF_SIZE];
	uint16_t TxCount;
	
	/*MODBUS TCPͷ��*/
	uint8_t TCP_Head[6];
	uint8_t TCP_Flag;
}MODS_T;

void uart_rx_isr(void);		/* �� stm8s_it.c �е��� */
uint8_t AnalyzeCmd(uint8_t *_DispBuf);
uint8_t MODS_Poll(uint8_t *_buf, uint16_t _len);
void MODS_SendAckErr(uint8_t _ucErrCode);
void MODS_SendWithCRC(uint8_t *_pBuf, uint16_t _ucLen);
void MODS_SendAckOk(void);

extern MODS_T g_tModS;

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
