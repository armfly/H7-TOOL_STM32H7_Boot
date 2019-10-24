/*
*********************************************************************************************************
*
*	模块名称 : MODBUS 寄存器地址定义 （应用层）
*	文件名称 : modbus_reg_addr.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2016-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MODBUS_REG_ADDR_H
#define __MODBUS_REG_ADDR_H



#define HARD_MODEL		0x0750	
#define BOOT_VERSION	0x0001		

#define REG03_RESET_TO_BOOT			0xFF7F	/* 特殊寄存器，1复位进入BOOT 2复位进入APP  3复位 */

/* BOOT 程序用的寄存器 */
#define REG03_BOOT_CPU_ID0			0xFF80
#define REG03_BOOT_CPU_ID1			0xFF81
#define REG03_BOOT_CPU_ID2			0xFF82
#define REG03_BOOT_CPU_ID3			0xFF83
#define REG03_BOOT_CPU_ID4			0xFF84
#define REG03_BOOT_CPU_ID5			0xFF85
#define REG03_BOOT_HARD_VER			0xFF86	/* 硬件型号（只读），内部型号代码，非型号全称，0xC200 表示TC200*/
#define REG03_BOOT_SOFT_VER			0xFF87	/* 硬件版本（只读），如0x0100，表示H1.00 */

#define REG03_BOOT_PROG_TYPE		0xFF88	/* APP程序类型: 0 = CPU内部Flash ; 1 = QSPI Flash */
#define REG03_BOOT_PROG_ADDR		0xFF89	/* APP程序地址 32位 */
#define REG03_BOOT_PROG_SIZE		0xFF8B	/* APP程序长度 32位 */
/* 系统控制参数  
	1 - 通知开始升级，设备开始擦除flash. 根据前面2个寄存器决定擦除空间 
	2 - 通知设备程序下载完毕
	3 - 通知系统复位
*/
#define REG03_BOOT_ERASE_SECTOR		0xFF8D	/* 擦除扇区，编号0开始，每扇区128K */
#define REG03_BOOT_TRANS_END		0xFF8E	/* 传送完毕 1 */

#define REG03_BOOT_STATUS			0xFFA0	/* 设备状态寄存器  0空闲，1表示正忙 */
#define REG03_BOOT_CRC32_HIGH		0xFFA1	/* 程序区CRC32校验 - 未用 */
#define REG03_BOOT_CRC32_LOW		0xFFA2	/* 程序区CRC32校验 - 未用 */

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
