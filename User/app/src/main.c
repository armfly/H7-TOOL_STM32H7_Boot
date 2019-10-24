/*
*********************************************************************************************************
*
*	模块名称 : 主程序
*	文件名称 : main.c
*	版    本 : V1.3
*	说    明 : 
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-01-01 armfly  正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"		/* printf函数定向输出到串口，所以必须包含这个文件 */
#include "main.h"
#include "usbd_user.h"

typedef void (*pFunction)(void);

//__attribute__((zero_init)) uint32_t g_JumpInit;		/* 不会被编译器初始化为0，注意noinit后面打钩 */
//uint32_t g_JumpInit;		/* 不会被编译器初始化为0，注意noinit后面打钩 */

#define g_JumpInit *(uint32_t *)0x20000000

void status_Upgrade(void);
static uint8_t CheckAppOk(void);
static void JumpToApp(void);
static void DispInfo(char *_str, uint8_t _line);
static void DispErrInfo(char *_str, uint8_t _line);
static void DispBootVersion(void);

uint8_t s_MainStatus;

VAR_T g_tVar;

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参: 无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{		
#if 1
	if (g_JumpInit == 0x11223344)	/* 软件复位后再进入APP，提供一个干净的CPU环境给APP */
	{
		g_JumpInit = 0;		
		
		JumpToApp();	/* 去执行APP程序 */
	}
	else if (g_JumpInit == 0x5AA51234)	/* APP请求升级固件, 直接进入固件升级状态 */
	{
		g_JumpInit = 0;
		s_MainStatus = 2;	
	}
	else
	{
		s_MainStatus = 0;	/* 去检测上电时是否有键按下 */
	}
#endif
	
	bsp_Init();		/* 其中不包括LCD初始化 */
	
	bsp_GetCpuID(g_tVar.CPU_Sn);	/* 读取CPU ID */
	
	while (1)
	{
		switch (s_MainStatus)
		{
			case 0:
				bsp_DelayMS(100);	/* 等待100ms，检测是否有按键按下 */
				if (bsp_GetKeyState(KID_S) || bsp_GetKeyState(KID_C))
				{
					bsp_ClearKey();
					/* 按下S键或C键上电，则进入固件升级界面 */
					s_MainStatus = 2;
				}			
				else
				{
					s_MainStatus = 1; 
				}
				break;
				
			case 1:
				if (CheckAppOk())		/* 有程序则跳转 */
				{
					g_JumpInit = 0x11223344;
					
					NVIC_SystemReset();	/* 复位CPU */
				}	
				else	/* 没有程序，直接进入固件升级界面 */
				{
					s_MainStatus = 2;
				}
				break;
				
			case 2:		/* 固件升级界面 */		
				LCD_InitHard();		/* boot只有在需要固件升级时才初始化LCD */
				status_Upgrade();
				break;
			
			default:
				;
				break;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: status_Upgrade
*	功能说明: USB串口升级固件状态
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void status_Upgrade(void)
{
	uint8_t ucKeyCode;		/* 按键代码 */
	uint8_t fRefresh;
	
	g_tVar.Mode = 0;
	usbd_OpenCDC(8);		/* 启用USB虚拟串口 */
	DispBootVersion();		/* 显示固件版本 */
	
	fRefresh = 1;
	while (s_MainStatus == 2)
	{
		bsp_Idle();

		if (fRefresh)	/* 刷新整个界面 */
		{
			fRefresh = 0;
		}

		{
			MSG_T msg;
			char buf[100];			
			
			if (bsp_GetMsg(&msg))
			{
				switch (msg.MsgCode)
				{
					case MSG_FILE_IFNO:
						{							
							if (g_tVar.ProgType == 0)
							{
								DispInfo("目标: CPU Flash", 0);
							}
							else if (g_tVar.ProgType == 1)
							{
								DispInfo("目标: QSPI Flash", 0);
							}
							else
							{
								DispInfo("目标: 未知", 0);
							}							
							sprintf(buf, "目标地址: 0x%08X", g_tVar.ProgAddr);
							DispInfo(buf, 1);
							sprintf(buf, "文件大小: %dB", g_tVar.ProgSize);
							DispInfo(buf, 2);														
						}						
						break;
						
					case MSG_ERASE_SECTOR_OK:
//						sprintf(buf, "擦除扇区成功: %d", msg.MsgParam);
//						DispInfo(buf, 4);						
						break;

					case MSG_ERASE_SECTOR_ERR:
						sprintf(buf, "擦除扇区失败: %d", msg.MsgParam);
						DispErrInfo(buf, 4);						
						break;
					
					case MSG_TRANS_FILE:	/* 传输文件 */
						{
							char buf[128];
							
							DispInfo("开始传输数据", 3);
							sprintf(buf, "进度: %3d%%  0x%08X, %4d",msg.MsgParam, g_tVar.FlashAddr, g_tVar.WriteLen);
							DispInfo(buf, 4);
						}
						break;
					
					case MSG_TRANS_ERROR:	/* 写flash文件失败 */
						{
							char buf[128];
							
							sprintf(buf, "写Flash失败: %08X, %4d", g_tVar.FlashAddr, g_tVar.WriteLen);
							DispErrInfo(buf, 3);
						}
						break;
						
					case MSG_RESET_TO_APP:	/* 复位CPU跳到APP */
						bsp_DelayMS(100);
						g_JumpInit = 0x11223344;
						NVIC_SystemReset();	/* 复位CPU */
						break;
						
					default:
						break;
				}
			}
		}
		
		ucKeyCode = bsp_GetKey();	/* 读取键值, 无键按下时返回 KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{
			/* 有键按下 */
			switch (ucKeyCode)
			{
				case  KEY_DOWN_S:		/* S键按下 */								
					break;

				case  KEY_LONG_S:	/* S键长按 */									
					break;				

				case  KEY_UP_C:		/* C键下 */	
					break;

				case  KEY_LONG_C:	/* C键释放 */
					break;					
				
				default:
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: DispInfo
*	功能说明: 显示升级提示信息
*	形    参: _str : 显示内容
*			  _line : 行号 0-8
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispInfo(char *_str, uint8_t _line)
{
	FONT_T tFont;		/* 定义字体结构体变量 */

	/* 设置字体参数 */
	{
		tFont.FontCode = FC_ST_16;	/* 字体代码 16点阵 */
		tFont.FrontColor = CL_MAIN_BACK;	/* 字体颜色 */
		tFont.BackColor = CL_MAIN_FONT;	/* 文字背景颜色 */
		tFont.Space = 0;				/* 文字间距，单位 = 像素 */
	}
		
	LCD_DispStrEx(5, 50 + 25 * _line, _str, &tFont, 200, 0);
}

/*
*********************************************************************************************************
*	函 数 名: DispErrInfo
*	功能说明: 显示升级错误信息。红色显示。
*	形    参: _str : 显示内容
*			  _line : 行号 0-8
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispErrInfo(char *_str, uint8_t _line)
{
	FONT_T tFont;		/* 定义字体结构体变量 */

	/* 设置字体参数 */
	{
		tFont.FontCode = FC_ST_16;	/* 字体代码 16点阵 */
		tFont.FrontColor = CL_RED;	/* 字体颜色 */
		tFont.BackColor = CL_MAIN_BACK;	/* 文字背景颜色 */
		tFont.Space = 0;				/* 文字间距，单位 = 像素 */
	}
		
	LCD_DispStrEx(5, 50 + 25 * _line, _str, &tFont, 200, 0);
}

/*
*********************************************************************************************************
*	函 数 名: DispBootVersion
*	功能说明: 显示Boot固件版本。 固件版本内嵌在向量表 startup_stm32h743xx.s
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispBootVersion(void)
{
	FONT_T tFont;		/* 定义字体结构体变量 */
	char buf[32];
	uint32_t ver;

	LCD_ClrScr(CL_MAIN_BACK);  	/* 清屏，背景蓝色 */
	
	/* 设置字体参数 */
	{
		tFont.FontCode = FC_ST_16;	/* 字体代码 16点阵 */
		tFont.FrontColor = CL_MAIN_FONT;	/* 字体颜色 */
		tFont.BackColor = CL_MAIN_BACK;	/* 文字背景颜色 */
		tFont.Space = 0;				/* 文字间距，单位 = 像素 */
	}
	ver = *(uint32_t *)(0x08000000 + 28);
	sprintf(buf, "H7-TOOL Boot Version: %X.%02X", ver >> 8, ver & 0xFF);
	LCD_DispStr(5, 2, buf, &tFont);
	
	LCD_DispStrEx(5, 2 + 20, "STM32 APP 固件升级", &tFont, 230, 0);
}

/*
*********************************************************************************************************
*	函 数 名: CheckAppOk
*	功能说明: 检查APP程序是否有效。
*	形    参: 无
*	返 回 值: 返回1表示OK，0表示失败
*********************************************************************************************************
*/
static uint8_t CheckAppOk(void)
{
#if 1	
	uint32_t StackAddr;
	
	//StackAddr = *(__IO uint32_t*)APPLICATION_ADDRESS + 128 * 1024;
	StackAddr = *(__IO uint32_t*)APPLICATION_ADDRESS + 128 * 1024;
	
	// 0x2408EDF8   0x20020000
	if ((StackAddr & 0x2FF00000) == 0x24000000 || (StackAddr & 0x2FF00000) == 0x20000000)
	{
		return 1;
	}
	return 0;
#else		/* 检查APP校验（是否合法程序） 未启用 */
	uint32_t ProgLen = *(uint32_t *)0x08008010;		/* app程序有效长度 */
	uint32_t ProgCrc = *(uint32_t *)0x08008014;		/* app程序CRC校验 */
	uint16_t crc1, crc2;
	uint32_t flash_size;
	
	flash_size = bsp_GetFlashSize();	/* falsh 容量 */
	
	if (ProgLen > flash_size - 8 * 1024)
	{
		return 0;
	}
	
	crc1 = CRC16_Modbus((uint8_t *)(0x08002000 + 0x100), ProgLen / 2);
	crc2 = CRC16_Modbus((uint8_t *)(0x08002000 + ProgLen / 2), ProgLen / 2);
	
	if (crc1 == (ProgCrc >> 16) && crc2 == (ProgCrc & 0xFFFF))
	{
		return 1;
	}
	else
	{
		return 0;
	}
#endif
}


/*
*********************************************************************************************************
*	函 数 名: JumpToApp
*	功能说明: 跳转到APP程序 0x0800 2000
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void JumpToApp(void)
{
	pFunction Jump_To_Application;
	uint32_t StackAddr;
	uint32_t ResetVector;
	
	/* 关中断 */
	DISABLE_INT();
	
	SCB_DisableDCache();
	
	StackAddr = *(__IO uint32_t*)APPLICATION_ADDRESS;			//0x08002000;
	ResetVector = *(__IO uint32_t*)(APPLICATION_ADDRESS + 4);	//0x08002004;
	
	/* Initialize user application's Stack Pointer */
	__set_MSP(StackAddr);		/* 设置主栈指针 */

	/* 设置中断向量表地址 */
//	*(uint32_t *)0xE000ED08 = StackAddr;

	/* 函数指针赋值 */
	Jump_To_Application = (pFunction)ResetVector;		
	Jump_To_Application();
}


/*
*********************************************************************************************************
*	函 数 名: DebugWriteCpuFlash
*	功能说明: 调试写CPU flash功能
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DebugWriteCpuFlash(void)
{
	uint32_t addr = 0x08100000;
	uint16_t i;
	uint16_t j;
	uint8_t buf0[1024];
	uint8_t buf1[1024];
	uint8_t err = 0;
	
	for (i = 0; i < 1024; i++)
	{
		buf0[i] = i;
	}
	
	addr = 0x08100000;
	err = 0;
	for (i = 0; i < 8; i++)
	{
		if (bsp_EraseCpuFlash(addr + i * 128 * 1024) != 0)
		{
			err = 1;
			break;
		}
		
		for (j = 0; j < 128; j++)
		{
			if (bsp_WriteCpuFlash(addr + j * 1024, buf0, 1024) != 0)
			{
				err = 2;
				break;
			}
		}
		
		for (j = 0; j < 128; j++)
		{
			memcpy(buf1, (uint8_t *)(addr + j * 1024), 1024);
			
			if (memcmp(buf0, buf1, 1024) != 0)
			{
				err = 3;
				break;
			}
		}			
		
		if (err != 0)
		{
			break;
		}
	}
	
	while(1);
}

#if 0	// 垃圾代码暂存
					if (g_tVar.ProgType == 0)		/* 写CPU Flash */
						{
							DispInfo("开始擦除CPU Flash", 3);
	
							bsp_DelayMS(200);	/* 等待应答给PC的数据发送完毕 */
							if (g_tVar.ProgAddr >= APPLICATION_ADDRESS && 
								g_tVar.ProgSize > 1024 &&
								g_tVar.ProgAddr + g_tVar.ProgSize < CPU_FLASH_BASE_ADDR + CPU_FLASH_SIZE)
							{
								uint8_t i;
								uint8_t n;
								uint8_t err = 0;
								
								n = (g_tVar.ProgSize + CPU_FLASH_SECTOR_SIZE - 1) / CPU_FLASH_SECTOR_SIZE;	/* 需要擦除的扇区个数 */
								
								/* 第1个扇区128K是boot程序，后面15个扇区用于应用程序 */
								for (i = 0; i < n; i++)
								{
									if (bsp_EraseCpuFlash(APPLICATION_ADDRESS + i * 128 * 1024) != 0)
									{
										/* 失败 */
										err = 1;
										break;
									}
									else
									{
										{
											char buf[128];
											
											sprintf(buf, "擦除扇区成功: %d", i);
											DispInfo(buf, 4);
										}
									}
								}	
								if (err == 1)
								{
									{
										char buf[128];
										
										sprintf(buf, "擦除扇区失败: %d", i);
										DispInfo(buf, 4);
									}
								}
								else
								{
									g_tVar.BootStatus = 0;	/* 擦除完毕 - PC机读取该标志判断是否结束 */
								}
							}		
							else
							{
								if (g_tVar.ProgSize < 1024)
								{
									DispErrInfo("程序大小不正确", 3);
								}
								else
								{
									DispErrInfo("目标地址不正确", 3);
								}
							}
						}
						else if (g_tVar.ProgType == 1)	/* 写QSPI Flash */
						{
							;	/* 暂时用不到 */
							DispErrInfo("不支持的文件类型", 3);
						}
#endif
	
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
