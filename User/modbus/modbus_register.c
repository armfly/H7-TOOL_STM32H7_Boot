/*
*********************************************************************************************************
*
*	模块名称 : MODBUS从机模块
*	文件名称 : tcp_MODS_slave.c
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "modbus_reg_addr.h"
#include "modbus_register.h"
#include "main.h"

/*
*********************************************************************************************************
*	函 数 名: ReadRegValue_03H
*	功能说明: 读取保持寄存器的值
*	形    参: reg_addr 寄存器地址
*			  reg_value 存放寄存器结果
*	返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
uint8_t ReadRegValue_03H(uint16_t _RegAddr, uint16_t *_RegValue)
{
	uint16_t value;
		
	/* modbus为大端模式，所以先传高16位，后传低16位 */
	switch (_RegAddr)
	{
		case REG03_BOOT_CPU_ID0:             /* MCU序号（只读） */
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

		case REG03_BOOT_PROG_TYPE:		/* APP程序类型: 0 = CPU内部Flash ; 1 = QSPI Flash */
			value = g_tVar.ProgType;
			break;
		
		case REG03_BOOT_PROG_ADDR:		/* APP程序地址 32位 */
			value = g_tVar.ProgAddr;
			break;
		
		case REG03_BOOT_PROG_SIZE:		/* APP程序长度 32位 */
			value = g_tVar.ProgSize;
			break;
			
		case REG03_BOOT_STATUS:		/* 设备状态寄存器  0空闲，1表示正忙 */
			value = g_tVar.BootStatus;
			break;
		
		case REG03_BOOT_CRC32_HIGH:	/* 程序区CRC32校验 - 未用 */
			break;
		
		case REG03_BOOT_CRC32_LOW:	/* 程序区CRC32校验 - 未用 */
			break;
		
		default:
			return 0;		/* 参数异常，返回 0 */	
	}

	*_RegValue = value;
	return 1;		/* 读取成功 */
}

/*
*********************************************************************************************************
*	函 数 名: WriteRegValue_06H
*	功能说明: 读取保持寄存器的值	写单个寄存器
*	形    参: reg_addr 寄存器地址
*			  reg_value 寄存器值
*	返 回 值: 1表示OK 0表示错误
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
				/* 复位进入BOOT 升级 */
			}
			else if (_RegValue == 1)
			{
				/* 复位进入APP */
				bsp_PutMsg(MSG_RESET_TO_APP, 0);
			}			
			break;
		
		case REG03_BOOT_PROG_TYPE:		/* APP程序类型: 0 = CPU内部Flash ; 1 = QSPI Flash */
			g_tVar.ProgType = _RegValue;
			break;
		
		case REG03_BOOT_PROG_ADDR:		/* APP程序地址 32位 */
			g_tVar.ProgAddr = _RegValue << 16;
			break;
		
		case REG03_BOOT_PROG_ADDR + 1:	
			g_tVar.ProgAddr += _RegValue;
			break;
		
		case REG03_BOOT_PROG_SIZE:			/* APP程序长度 32位 */
			g_tVar.ProgSize = _RegValue << 16;
			break;
		
		case REG03_BOOT_PROG_SIZE + 1:
			g_tVar.ProgSize += _RegValue;
			bsp_PutMsg(MSG_FILE_IFNO, 0);	/* 显示文件信息 */
			break;
		
		case REG03_BOOT_ERASE_SECTOR:	/* 擦除扇区, 128KB */
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
					bsp_PutMsg(MSG_ERASE_SECTOR_OK, _RegValue);	/* 擦除成功 */
				}
				else
				{
					bsp_PutMsg(MSG_ERASE_SECTOR_ERR, _RegValue);	/* 擦除失败 */
				}
			}
			break;
		
		case REG03_BOOT_TRANS_END:	/* 文件已传送完毕 - 设备善后处理最好一个扇区 */
			if (_RegValue == 1)
			{
				if (bsp_WriteCpuFlash(g_tVar.FlashAddr, g_tVar.ProgDatabuf, g_tVar.ProgDataLen) == 0)		/* 写最后一包数据(先读取，组成新的1K数据，再写入) */
				{
					g_tVar.WriteLen = g_tVar.ProgDataLen;
					bsp_PutMsg(MSG_TRANS_FILE, 100);	/* 进度 100% */
					break;
				}
				else
				{
					bsp_PutMsg(MSG_TRANS_ERROR, g_tVar.FlashAddr);	/* 失败 */
				}	
			}
			break;

		case REG03_BOOT_STATUS:		/* 设备状态寄存器  0空闲，1表示正忙 */
			g_tVar.BootStatus = _RegValue; 
			break;
		
		case REG03_BOOT_CRC32_HIGH:	/* 程序区CRC32校验 - 未用 */
			break;
		
		case REG03_BOOT_CRC32_LOW:	/* 程序区CRC32校验 - 未用 */
			break;
		
		default:
			err = 1;	/* 参数异常，返回 0 */
			break;
	}

	if (err == 0)
	{
		return 1;		/* 成功 */
	}
	return 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
