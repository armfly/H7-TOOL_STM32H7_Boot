/*
*********************************************************************************************************
*
*	模块名称 : MODBUS从机模块
*	文件名称 : modbus_slave.c
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
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
*	函 数 名: MODS_Poll
*	功能说明: 解析数据包. 在主程序中轮流调用。
*	形    参: 无
*	返 回 值: 无
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
		/* 计算CRC校验和 */
		crc1 = CRC16_Modbus(g_tModS.RxBuf, g_tModS.RxCount);
		if (crc1 != 0)
		{
			#if 0	/* 正式程序丢弃CRC异常的，并需要处理连包 */
			
			#else	/* 调试程序，原始数据返回 */
				memcpy(g_tModS.TxBuf, g_tModS.RxBuf, g_tModS.RxCount);
				g_tModS.TxCount = g_tModS.RxCount;
			#endif
			//MODS_SendAckErr(ERR_PACKAGE);		/* 发送连包应答 */
			goto err_ret;
		}
	}
	else
	{
		g_tModS.RxCount += 2;
	}

	/* 站地址 (1字节） */
	addr = g_tModS.RxBuf[0];	/* 第1字节 站号 */
	if (addr != RS485_ADDR && addr != 0xF4)
	{
		goto err_ret;
	}

	/* 分析应用层协议 */
	MODS_AnalyzeApp();
	g_tModS.RxCount = 0;	/* 必须清零计数器，方便下次帧同步 */
	return 1;
	
err_ret:
	g_tModS.RxCount = 0;	/* 必须清零计数器，方便下次帧同步 */
	return 0;
}

#if 0

/*
*********************************************************************************************************
*	函 数 名: MODS_ReciveNew
*	功能说明: 串口接收中断服务程序会调用本函数。当收到一个字节时，执行一次本函数。
*	形    参: 无
*	返 回 值: 无
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
			3.5个字符的时间间隔，只是用在RTU模式下面，因为RTU模式没有开始符和结束符，
			两个数据包之间只能靠时间间隔来区分，Modbus定义在不同的波特率下，间隔时间是不一样的，
			所以就是3.5个字符的时间，波特率高，这个时间间隔就小，波特率低，这个时间间隔相应就大

			4800  = 7.297ms
			9600  = 3.646ms
			19200  = 1.771ms
			38400  = 0.885ms
		*/
		uint32_t timeout;
		
		g_rtu_timeout = 0;
		
		timeout = 35000000 / g_tParam.Baud;		/* 计算超时时间，单位us */
		
		/* 硬件定时中断，定时精度us 硬件定时器1用于ADC, 定时器2用于Modbus */
		bsp_StartHardTimer(2, timeout, (void *)MODS_RxTimeOut);

		if (g_tModS.RxCount < RX_BUF_SIZE)
		{
			g_tModS.RxBuf[g_tModS.RxCount++] = _byte;
		}
	#endif
}

///*
//*********************************************************************************************************
//*	函 数 名: MODS_RxTimeOut
//*	功能说明: 超过3.5个字符时间后执行本函数。 设置全局变量 g_rtu_timeout = 1; 通知主程序开始解码。
//*	形    参: 无
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//static void MODS_RxTimeOut(void)
//{
//	g_rtu_timeout = 1;
//}
#endif

/*
*********************************************************************************************************
*	函 数 名: MODS_SendWithCRC
*	功能说明: 发送一串数据, 自动追加2字节CRC
*	形    参: _pBuf 数据；
*			  _ucLen 数据长度（不带CRC）
*	返 回 值: 无
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

	/* 发送以太网数据,数据存在g_tModS.TxBuf中,在函数外面发送 */
	memcpy(g_tModS.TxBuf, buf, _ucLen);
	g_tModS.TxCount = _ucLen;
}

/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckErr
*	功能说明: 发送错误应答
*	形    参: _ucErrCode : 错误代码
*	返 回 值: 无
*********************************************************************************************************
*/
void MODS_SendAckErr(uint8_t _ucErrCode)
{
	uint8_t txbuf[3];

	txbuf[0] = g_tModS.RxBuf[0];			/* 485地址 */
	txbuf[1] = g_tModS.RxBuf[1] | 0x80;	/* 异常的功能码 */
	txbuf[2] = _ucErrCode;						/* 错误代码(01,02,03,04) */

	MODS_SendWithCRC(txbuf, 3);
}

/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckOk
*	功能说明: 发送正确的应答.
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: MODS_AnalyzeApp
*	功能说明: 分析应用层协议
*	形    参:
*		     _DispBuf  存储解析到的显示数据ASCII字符串，0结束
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_AnalyzeApp(void)
{
	switch (g_tModS.RxBuf[1])			/* 第2个字节 功能码 */
	{	
		case 0x03:	/* 读取1个或多个参数保持寄存器 在一个或多个保持寄存器中取得当前的二进制值*/
			MODS_03H();
			break;

		case 0x06:	/* 写单个参数保持寄存器 (存储在CPU的FLASH中，或EEPROM中的参数)*/
			MODS_06H();			
			break;
			
		case 0x10:	/* 写多个参数保持寄存器 (存储在CPU的FLASH中，或EEPROM中的参数)*/
			MODS_10H();
			break;
		
		case 0x15:	/* 写文件寄存器 */
			MODS_15H();
			break;
		
		default:
			g_tModS.RspCode = RSP_ERR_CMD;
			MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
			break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_03H
*	功能说明: 读取保持寄存器 在一个或多个保持寄存器中取得当前的二进制值
*	形    参: 无
*	返 回 值: 无
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
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	regaddr = BEBufToUint16(&g_tModS.RxBuf[2]); 
	num = BEBufToUint16(&g_tModS.RxBuf[4]);
	if (num > (TX_BUF_SIZE - 5) / 2)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;		
	}

err_ret:
	if (g_tModS.RxBuf[0] != 0x00)	/* 00广播地址不应答, FF地址应答RS485_ADDR */
	{	
		if (g_tModS.RspCode == RSP_OK)			/* 正确应答 */
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
						g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
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
			MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_06H
*	功能说明: 写单个寄存器
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_06H(void)
{

	/*
		写保持寄存器。注意06指令只能操作单个保持寄存器，16指令可以设置单个或多个保持寄存器

		主机发送:
			11 从机地址
			06 功能码
			00 寄存器地址高字节
			01 寄存器地址低字节
			00 数据1高字节
			01 数据1低字节
			9A CRC校验高字节
			9B CRC校验低字节

		从机响应:
			11 从机地址
			06 功能码
			00 寄存器地址高字节
			01 寄存器地址低字节
			00 数据1高字节
			01 数据1低字节
			1B CRC校验高字节
			5A	CRC校验低字节

		例子:
			01 06 30 06 00 25  A710    ---- 触发电流设置为 2.5
			01 06 30 06 00 10  6707    ---- 触发电流设置为 1.0


			01 06 30 1B 00 00  F6CD    ---- SMA 滤波系数 = 0 关闭滤波
			01 06 30 1B 00 01  370D    ---- SMA 滤波系数 = 1
			01 06 30 1B 00 02  770C    ---- SMA 滤波系数 = 2
			01 06 30 1B 00 05  36CE    ---- SMA 滤波系数 = 5

			01 06 30 07 00 01  F6CB    ---- 测试模式修改为 T1
			01 06 30 07 00 02  B6CA    ---- 测试模式修改为 T2

			01 06 31 00 00 00  8736    ---- 擦除浪涌记录区
			01 06 31 01 00 00  D6F6    ---- 擦除告警记录区

*/

	uint16_t reg;
	uint16_t value;
//	uint8_t i;
	
	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* 寄存器值 */

	if (WriteRegValue_06H(reg, value) == 0)
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)			/* 正确应答 */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_10H
*	功能说明: 连续写多个寄存器.  进用于改写时钟
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_10H(void)
{
	/*
		从机地址为11H。保持寄存器的其实地址为0001H，寄存器的结束地址为0002H。总共访问2个寄存器。
		保持寄存器0001H的内容为000AH，保持寄存器0002H的内容为0102H。

		主机发送:
			11 从机地址
			10 功能码
			00 寄存器起始地址高字节
			01 寄存器起始地址低字节
			00 寄存器数量高字节
			02 寄存器数量低字节
			04 字节数
			00 数据1高字节
			0A 数据1低字节
			01 数据2高字节
			02 数据2低字节
			C6 CRC校验高字节
			F0 CRC校验低字节

		从机响应:
			11 从机地址
			06 功能码
			00 寄存器地址高字节
			01 寄存器地址低字节
			00 数据1高字节
			01 数据1低字节
			1B CRC校验高字节
			5A	CRC校验低字节

		例子:
			01 10 30 00 00 06 0C  07 DE  00 0A  00 01  00 08  00 0C  00 00     389A    ---- 写时钟 2014-10-01 08:12:00
			01 10 30 00 00 06 0C  07 DF  00 01  00 1F  00 17  00 3B  00 39     5549    ---- 写时钟 2015-01-31 23:59:57

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
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	reg_addr = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	reg_num = BEBufToUint16(&g_tModS.RxBuf[4]);	/* 寄存器个数 */
//	byte_num = g_tModS.RxBuf[6];	/* 后面的数据体字节数 */
	_pBuf = &g_tModS.RxBuf[7];

	for (i = 0; i < reg_num; i++)
	{
		value = BEBufToUint16(_pBuf);
		
		if (WriteRegValue_06H(reg_addr + i, value) == 0)
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
			break;
		}
		
		_pBuf += 2;
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)			/* 正确应答 */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_15H
*	功能说明: 写文件
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_15H(void)
{
	/*
		主机发送:
			11 从机地址
			15 功能码
			00 请求数据长度
			01 子请求x，参考类型
			00 子请求x，文件号
			01 子请求x，记录号
			9A 子请求x，记录长度
			9B 子请求x，记录数据
			18 校验高字节
			FC 校验低字节
	
		从机响应:
			11 从机地址
			15 功能码
			00 请求数据长度
			01 子请求x，参考类型
			00 子请求x，文件号
			01 子请求x，记录号
			9A 子请求x，记录长度
			9B 子请求x，记录数据
			18 校验高字节
			FC 校验低字节
*/
	uint8_t i;
//	uint8_t DateLen;		/* 请求数据长度 */
//	uint8_t Type;			/* 参考类型 */
//	uint16_t FileID;		/* 文件号 */
	uint16_t RecordID;		/* 记录号 */
	uint16_t RecordLen;		/* 记录长度 */
	uint32_t Packet;		/* 第几包数据 */
	static uint16_t s_LenBak;	/* 记录之前的数据长度，如果长度与之前的不同，则认为是最后一包数据,需要写入 */
	uint32_t Cpu_Offset;	/* CPU地址偏移 */
//	char buf[50];
	
	g_tModS.RspCode = RSP_OK;	
	
//	DateLen = g_tModS.RxBuf[2];
//	Type = g_tModS.RxBuf[3];
//	FileID = BEBufToUint16(&g_tModS.RxBuf[4]); 		/* 子请求x，文件号 */
	RecordID = BEBufToUint16(&g_tModS.RxBuf[6]); 		/* 子请求x，记录号 */
	RecordLen = BEBufToUint16(&g_tModS.RxBuf[8]); 	/* 子请求x，记录长度 */
	
	if (RecordID == 0)				/* 第一包数据，就把flash写入地址设为基地址,同时擦除应用区代码 */
	{
		s_LenBak = RecordLen;		/* 第1包的数据长度，认为是收到每包的长度 */
	}
	
	Packet = RecordID + 1;
	Cpu_Offset = RecordID * s_LenBak / 1024 * 1024;			/* CPU写入的偏移地址 */
	g_tVar.FlashAddr = APPLICATION_ADDRESS + Cpu_Offset;	/* 下一包数据写入的位置，1024的整数倍 */
	
	memcpy(&g_tVar.ProgDatabuf[(RecordID * s_LenBak) % 1024], &g_tModS.RxBuf[10], RecordLen);		/* 组成1K数据再写入CPU flash */
	
	if ((Packet * s_LenBak) % 1024 != 0)					/* 判断当前数据包是否满足1K的整数倍 */
	{	
		g_tVar.ProgDataLen = ((RecordID * s_LenBak) % 1024) + RecordLen;		/* 记录当前需要写入的包长 */
	}
	else													/* 满足1K的整数倍，此时才开始将1K数据写入CPU flash */
	{	
		if (bsp_WriteCpuFlash(g_tVar.FlashAddr, g_tVar.ProgDatabuf, 1024) == 0)		/* 每次写入1024个字节 */
		{
			g_tVar.WriteLen = 1024;
			bsp_PutMsg(MSG_TRANS_FILE, RecordID * s_LenBak * 100  / g_tVar.ProgSize);	/* 发送消息，传递进度 */
			g_tModS.RspCode = RSP_OK;
		}
		else
		{
			bsp_PutMsg(MSG_TRANS_ERROR, g_tVar.FlashAddr);
			g_tModS.RspCode = RSP_ERR_WRITE;				/* 写入失败 */
			goto err_ret;
		}
	}

	
err_ret:
	if (g_tModS.RspCode == RSP_OK)			/* 正确应答 */
	{
		g_tModS.TxCount = 0;
		for (i = 0; i < 10; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[i];	/* 应答数据包 */
		}
		MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
