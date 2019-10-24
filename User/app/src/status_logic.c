/*
*********************************************************************************************************
*
*	模块名称 : 逻辑分析仪主程序
*	文件名称 : status_logic.c
*	版    本 : V1.0
*	说    明 : 逻辑分析仪功能、示波器、虚拟串口、虚拟CAN、I2C控制器、SPI控制器、PWM控制器、GPIO控制
*				DAC控制、电流检测、编码器输入、频率计.
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2018-12-06 armfly  正式发布
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "param.h"
#include "main.h"
#include "status_logic.h"

#define FORM_BACK_COLOR	CL_BLUE

/*
*********************************************************************************************************
*	函 数 名: status_LogicMain
*	功能说明: 逻辑分析仪状态.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void status_LogicMain(void)
{
	uint8_t ucKeyCode;		/* 按键代码 */
	uint8_t fRefresh;
	FONT_T tFont;		/* 定义字体结构体变量 */

	/* 设置字体参数 */
	{
		tFont.FontCode = FC_ST_16;	/* 字体代码 16点阵 */
		tFont.FrontColor = CL_BLACK;	/* 字体颜色 */
		tFont.BackColor = FORM_BACK_COLOR;	/* 文字背景颜色 */
		tFont.Space = 0;				/* 文字间距，单位 = 像素 */

		LCD_ClrScr(FORM_BACK_COLOR);  	/* 清屏，背景蓝色 */

		LCD_DispStr(5, 3, "逻辑分析仪示波器", &tFont);
	}

	fRefresh = 1;
	while (g_MainStatus == MS_LOGIC)
	{
		bsp_Idle();

		if (fRefresh)	/* 刷新整个界面 */
		{
			fRefresh = 0;
		}

		ucKeyCode = bsp_GetKey();	/* 读取键值, 无键按下时返回 KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{
			/* 有键按下 */
			switch (ucKeyCode)
			{
				case  KEY_DOWN_S:		/* S键 */
					break;

				case  KEY_DOWN_C:		/* C键 */
					break;
				
				default:
					break;
			}
		}
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
