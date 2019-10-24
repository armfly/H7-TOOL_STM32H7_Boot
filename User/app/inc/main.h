/*
*********************************************************************************************************
*
*	模块名称 : main模块
*	文件名称 : main.h
*	版    本 : V1.0
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _MAIN_H_
#define _MAIN_H_

/* STM32H750， FALASH 扇区大小为128KB， boot区需要占用128KB  */
#define APPLICATION_ADDRESS		0x08020000 		// 选择128K以后的区域作为APP程序区（2MB-128KB)
//#define APPLICATION_ADDRESS		0x08100000 	// 选择第2个BANK （1MB) 作为APP程序区
#define APPLICATION_SIZE		(2 * 1024 * 1024 - 128 * 1024)		// 应用程序最大容量	

/* UI颜色定义 */
#define CL_MAIN_BACK		CL_BUTTON_GREY
#define CL_MAIN_FONT		CL_GREY

typedef struct 
{
	uint32_t CPU_Sn[3];
	
	uint16_t BootSystem;
	uint16_t BootStatus;
	
	uint16_t ProgType;	/* APP程序类型: 0 = CPU内部Flash ; 1 = QSPI Flash */
	uint32_t ProgAddr;	/* APP程序地址 32位 */	
	uint32_t ProgSize;	/* APP程序长度 32位 */
	
	uint32_t FlashAddr;
	uint8_t ProgDatabuf[2048];
	uint16_t ProgDataLen;
	uint16_t WriteLen;
	
	uint8_t DispDir;
	
	uint8_t Mode;		/* 0表示STM32固件升级， 1表示ESP32 WiFi固件升级 */
}VAR_T;


extern VAR_T g_tVar;

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

