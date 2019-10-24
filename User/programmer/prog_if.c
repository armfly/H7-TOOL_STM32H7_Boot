/*
*********************************************************************************************************
*
*	ģ������ : ������ӿ��ļ�
*	�ļ����� : prog_if.c
*	��    �� : V1.0
*	˵    �� : 
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2019-03-19  armfly  ��ʽ����
*
*	Copyright (C), 2019-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "param.h"
#include "modbus_slave.h"
#include "prog_if.h"

#define PG_INST_MAX_LEN  32 * 1024
static uint16_t s_prog_buf[PG_INST_MAX_LEN];	/* �ű��������� */
static uint32_t s_prog_len;						/* ���򳤶� */
static uint32_t s_prog_pc = 0;				/* ����ָ�� */
static uint8_t s_prog_param_len = 0;		/* ָ��������� */
static uint8_t s_prog_state = 0;			/* ָ��ִ��״̬��0��ʾȡcmd��1��ʾȡ������2��ʾ�ȴ�ִ�� */
static uint8_t s_prog_cmd = 0;
uint8_t s_prog_ack_buf[2 * 1024];
uint16_t s_prog_ack_len;

static uint8_t s_prog_run_flag = 0;
	
void PG_AnalyzeInst(void);
uint8_t PG_AnalyzeI2C(uint16_t _inst);

/*
*********************************************************************************************************
*	�� �� ��: PG_ReadInst
*	����˵��: ��ȡһ��ָ�������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t PG_ReadInst(void)
{
	uint16_t inst;
	
	if (s_prog_pc < PG_INST_MAX_LEN)
	{
		inst = s_prog_buf[s_prog_pc];
		s_prog_pc++;
		
		if (s_prog_pc >= s_prog_len)
		{
			s_prog_run_flag = 2;
		}
	}
	else
	{
		inst = 0;
	}
	
	return inst;
}

/*
*********************************************************************************************************
*	�� �� ��: PG_Run
*	����˵��: ����ָ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PG_Poll(void)
{
	if (s_prog_run_flag == 1)
	{
		PG_AnalyzeInst();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: PG_Stop
*	����˵��: ֹͣ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PG_Stop(void)
{
	s_prog_run_flag = 0;
	s_prog_pc = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: PG_WaitRunCompleted
*	����˵��: ִ��С���򣬲��ȴ�ָ�����
*	��    ��: _usTimeout,�ȴ�ʱ��ms�����65��.
*	�� �� ֵ: 0 ��ʾ��ʱ��1��ʾOK
*********************************************************************************************************
*/
uint8_t PG_WaitRunCompleted(uint16_t _usTimeout)
{
	int32_t time1;
	uint8_t re;
	
	s_prog_run_flag = 1;
	s_prog_pc = 0;
	s_prog_ack_len = 0;

	time1 = bsp_GetRunTime();
	while (1)
	{
		bsp_Idle();
		
		if (s_prog_run_flag == 2)
		{
			re = 1;
			break;
		}
		
		if (bsp_CheckRunTime(time1) > _usTimeout)
		{
			re = 0;
			break;
		}
	}
	return re;
}
/*
*********************************************************************************************************
*	�� �� ��: PG_Install
*	����˵��: ��װ����
*	��    ��: 
*		_addr : ƫ�Ƶ�ַ
*		_buf :  ������
*		_len :  ���򳤶�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PG_Install(uint16_t _addr, uint8_t *_buf, uint16_t _len, uint16_t _total_len)
{
	uint32_t i;
	
	for (i = 0; i < _len; i++)
	{
		if (_addr < PG_INST_MAX_LEN)
		{
			s_prog_buf[_addr++] = (_buf[2 * i] << 8) + _buf[2 * i + 1];
		}
	}
	
	s_prog_len = _total_len;
}
	
void PG_AnalyzeInst(void)
{
	uint16_t inst;
	
	if (s_prog_state == 0)
	{
		inst = PG_ReadInst();
		s_prog_cmd = inst;
	}
	else if (s_prog_state == 1)
	{
		inst = PG_ReadInst();
	}
	else if (s_prog_state == 2)
	{
		inst = 0;
	}
	
	switch (s_prog_cmd >> 8)
	{
		case DEV_SYS:
			break;
		
		case DEV_GPIO:
			break;
		
		case DEV_TIM:
			break;
		
		case DEV_DAC:
			break;
		
		case DEV_ADC:
			break;
		
		case DEV_I2C:
			PG_AnalyzeI2C(inst);
			break;
		
		case DEV_SPI:
			break;
		
		case DEV_UART:
			break;
		
		case DEV_485:
			break;
		
		case DEV_CAN:
			break;
		
		case DEV_SWD:
			break;
	}
}

uint8_t PG_AnalyzeI2C(uint16_t _inst)
{
	static uint16_t s_len = 0;
	static uint16_t s_cmd = 0;
	
	if (s_prog_state == 0)		/* ȡָ�� */
	{
		switch (_inst)
		{
			case I2C_START:
				i2c_Start();
				break;
			
			case I2C_STOP:
				i2c_Stop();
				break;
			
			case I2C_SEND_BYTE:
				s_prog_state = 1;
				s_prog_param_len = 1;
				s_len = 0;					
				break;

			case I2C_SEND_BYTES:
				s_prog_state = 1;
				s_prog_param_len = PG_ReadInst();	/* ȡ�����ֶ� */
				s_len = 0;					
				break;
			
			case I2C_READ_BYTES:
				s_prog_state = 1;
				s_prog_ack_len = PG_ReadInst();		/* ȡ�����ֶ� */
				s_len = 0;					
				break;
		}	
		s_cmd = _inst;
	}
	else if (s_prog_state == 1)		/* ִ��ָ�� */
	{
		switch (s_cmd)
		{
			case I2C_SEND_BYTE:				
				i2c_SendByte(_inst >> 8);
				i2c_WaitAck();
				s_prog_state = 2;	/* ִ����� */
				break;
			
			case I2C_SEND_BYTES:	/* ��״̬����� s_prog_param_len ���� */
				if (s_len < s_prog_param_len)	
				{
					i2c_SendByte(_inst >> 8);
					i2c_WaitAck();					
					s_len++;
				}
				else	/* ִ����� */
				{
					s_prog_state = 2;
				}
				break;
			
			case I2C_READ_BYTES:
				if (s_len <= s_prog_ack_len)
				{
					s_prog_ack_buf[s_len] = i2c_ReadByte();		/* ��1���ֽ� */
					s_len++;
					if (s_len == s_prog_ack_len)
					{
						i2c_Ack();	/* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
					}
					else
					{
						i2c_Ack();		/* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
					}
				}
				else
				{
					s_prog_state = 2;	/* �쳣��֮ */
				}
				break;				
		}
	}
	else if (s_prog_state == 2)
	{
		s_prog_state = 0;
//		switch (s_cmd)
//		{
//			case I2C_SEND_BYTE:	
//				/* �ȴ����ݴ������ */
//				s_prog_state = 0;
//				break;
//			
//			case I2C_SEND_BYTES:	
//				/* �ȴ����ݴ������ */
//				s_prog_state = 0;				
//				break;
//		}		
	}
	
	return 0;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/


