/*
*********************************************************************************************************
*
*	ģ������ : DAC8562ģ����Խ���
*	�ļ����� : form_dac8562.c
*	��    �� : V1.0
*	˵    �� : ����������DAC8562ģ�顣
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2014-10-15 armfly  �׷�
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "form_dac8562.h"
#include "math.h"

/* �������ṹ */
typedef struct
{
	FONT_T FontBlack;	/* ��ɫ */
	FONT_T FontBlue;	/* ��ɫ */
	FONT_T FontBtn;		/* ��ť������ */
	FONT_T FontBox;		/* ������������ */

	GROUP_T Box1;

	BUTTON_T BtnRet;

	BUTTON_T Btn1;
	BUTTON_T Btn2;
	BUTTON_T Btn3;
	BUTTON_T Btn4;
	BUTTON_T Btn5;
	BUTTON_T Btn6;
	BUTTON_T Btn7;
	BUTTON_T Btn8;
	BUTTON_T Btn9;
	BUTTON_T Btn10;
	BUTTON_T Btn11;
	BUTTON_T Btn12;
	BUTTON_T Btn13;
	BUTTON_T Btn14;
	BUTTON_T Btn15;

	LABEL_T Label1;
	LABEL_T Label2;
	LABEL_T Label3;
	LABEL_T Label4;
	LABEL_T Label5;
	LABEL_T Label6;

	uint16_t DacValue;	/* DAC���� */
	int32_t Voltage;	/* �����ѹ */
}Form8562_T;

/* ���屳��ɫ */
#define FORM_BACK_COLOR		CL_BTN_FACE

/* �������ʹ�С */
#define BOX1_X	5
#define BOX1_Y	8
#define BOX1_H	(g_LcdHeight - 53)
#define BOX1_W	(g_LcdWidth -  2 * BOX1_X)
#define BOX1_T	"DAC8562 ˫·DACģ��(16bit, -10V -> +10V)"

#define BTN1_H	32
#define BTN1_W	105

/* ��1�а�ť���� */
#define	BTN1_X	(BOX1_X + 10)
#define	BTN1_Y	(BOX1_Y + 20)
#define	BTN1_T	"-10V"

	#define BTN2_H	BTN1_H
	#define BTN2_W	BTN1_W
	#define	BTN2_X	(BTN1_X +  BTN1_W + 10)
	#define	BTN2_Y	BTN1_Y
	#define	BTN2_T	"0V"

		#define BTN3_H	BTN1_H
		#define BTN3_W	BTN1_W
		#define	BTN3_X	(BTN1_X + 2 * (BTN1_W + 10))
		#define	BTN3_Y	BTN1_Y
		#define	BTN3_T	"+10V"

/* ��2�а�ť���� */
#define BTN4_H	BTN1_H
#define BTN4_W	BTN1_W
#define	BTN4_X	BTN1_X
#define	BTN4_Y	(BTN1_Y + BTN1_H + 10)
#define	BTN4_T	"DACֵ+1"

	#define BTN5_H	BTN1_H
	#define BTN5_W	BTN1_W
	#define	BTN5_X	(BTN1_X +  1 * (BTN1_W + 10))
	#define	BTN5_Y	BTN4_Y
	#define	BTN5_T	"DACֵ-1"

		#define BTN6_H	BTN1_H
		#define BTN6_W	BTN1_W
		#define	BTN6_X	(BTN1_X +  2 * (BTN1_W + 10))
		#define	BTN6_Y	BTN4_Y
		#define	BTN6_T	"DACֵ+100"

			#define BTN7_H	BTN1_H
			#define BTN7_W	BTN1_W
			#define	BTN7_X	(BTN1_X +  3 * (BTN1_W + 10))
			#define	BTN7_Y	BTN4_Y
			#define	BTN7_T	"DACֵ-100"

/* ��3�а�ť���� */
#define BTN8_H	BTN1_H
#define BTN8_W	BTN1_W
#define	BTN8_X	BTN1_X
#define	BTN8_Y	(BTN1_Y + 2 * (BTN1_H + 10))
#define	BTN8_T	"��ѹ+1mV"

	#define BTN9_H	BTN1_H
	#define BTN9_W	BTN1_W
	#define	BTN9_X	(BTN1_X +  1 * (BTN1_W + 10))
	#define	BTN9_Y	BTN8_Y
	#define	BTN9_T	"��ѹ-1mV"

		#define BTN10_H	BTN1_H
		#define BTN10_W	BTN1_W
		#define	BTN10_X	(BTN1_X +  2 * (BTN1_W + 10))
		#define	BTN10_Y	BTN8_Y
		#define	BTN10_T	"��ѹ+100mV"

			#define BTN11_H	BTN1_H
			#define BTN11_W	BTN1_W
			#define	BTN11_X	(BTN1_X +  3 * (BTN1_W + 10))
			#define	BTN11_Y	BTN8_Y
			#define	BTN11_T	"��ѹ-100mV"

/* ��4�а�ť���� */
#define BTN12_H	BTN1_H
#define BTN12_W	BTN1_W
#define	BTN12_X	BTN1_X
#define	BTN12_Y	(BTN1_Y + 3 * (BTN1_H + 10))
#define	BTN12_T	"ͬ�����Ҳ�"

	#define BTN13_H	BTN1_H
	#define BTN13_W	BTN1_W
	#define	BTN13_X	(BTN1_X +  1 * (BTN1_W + 10))
	#define	BTN13_Y	BTN12_Y
	#define	BTN13_T	"2·90������"

		#define BTN14_H	BTN1_H
		#define BTN14_W	BTN1_W
		#define	BTN14_X	(BTN1_X +  2 * (BTN1_W + 10))
		#define	BTN14_Y	BTN12_Y
		#define	BTN14_T	"2·180������"

			#define BTN15_H	BTN1_H
			#define BTN15_W	BTN1_W
			#define	BTN15_X	(BTN1_X +  3 * (BTN1_W + 10))
			#define	BTN15_Y	BTN12_Y
			#define	BTN15_T	"ֹͣ�������"

#define	LBL1_X	BOX1_X + 5
#define	LBL1_Y	190
#define	LBL1_T "��ǰDACֵ:"

#define	LBL2_X	LBL1_X + 85
#define	LBL2_Y	LBL1_Y
#define	LBL2_T ""

#define	LBL3_X	LBL1_X
#define	LBL3_Y	LBL1_Y + 20
#define	LBL3_T " �����ѹ:"

#define	LBL4_X	LBL3_X + 85
#define	LBL4_Y	LBL3_Y
#define	LBL4_T " "


/* ��ť */
/* ���ذ�ť������(��Ļ���½�) */
#define BTN_RET_H	32
#define BTN_RET_W	80
#define	BTN_RET_X	(g_LcdWidth - BTN_RET_W - 8)
#define	BTN_RET_Y	(g_LcdHeight - BTN_RET_H - 4)
#define	BTN_RET_T	"����"

static void InitForm8562(void);
static void DispForm8562(void);

static void DispDacValue(void);
static void DispDacVoltage(void);
static void MakeSinTable(uint16_t *_pBuf, uint16_t _usSamples, uint16_t _usBottom, uint16_t _usTop);

Form8562_T *Form8562;

#define DAC_OUT_FREQ	10000		/* DAC �������Ƶ�� 10KHz */
#define WAVE_SAMPLES	200			/* ÿ������������ Խ���η���Խϸ�壬����������Ƶ�ʻή�� */

static uint16_t s_WaveBuf[WAVE_SAMPLES];
static uint16_t s_WavePos1, s_WavePos2;
/*
*********************************************************************************************************
*	�� �� ��: InitForm8562
*	����˵��: ��ʼ���ؼ�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitForm8562(void)
{
	/* ������������ */
	Form8562->FontBox.FontCode = FC_ST_16;
	Form8562->FontBox.BackColor = CL_BTN_FACE;	/* �ͱ���ɫ��ͬ */
	Form8562->FontBox.FrontColor = CL_BLACK;
	Form8562->FontBox.Space = 0;

	/* ����1 ���ھ�ֹ��ǩ */
	Form8562->FontBlack.FontCode = FC_ST_16;
	Form8562->FontBlack.BackColor = CL_MASK;		/* ͸��ɫ */
	Form8562->FontBlack.FrontColor = CL_BLACK;
	Form8562->FontBlack.Space = 0;

	/* ����2 ���ڱ仯������ */
	Form8562->FontBlue.FontCode = FC_ST_16;
	Form8562->FontBlue.BackColor = CL_BTN_FACE;
	Form8562->FontBlue.FrontColor = CL_BLUE;
	Form8562->FontBlue.Space = 0;

	/* ��ť���� */
	Form8562->FontBtn.FontCode = FC_ST_16;
	Form8562->FontBtn.BackColor = CL_MASK;		/* ͸������ */
	Form8562->FontBtn.FrontColor = CL_BLACK;
	Form8562->FontBtn.Space = 0;

	/* ����� */
	Form8562->Box1.Left = BOX1_X;
	Form8562->Box1.Top = BOX1_Y;
	Form8562->Box1.Height = BOX1_H;
	Form8562->Box1.Width = BOX1_W;
	Form8562->Box1.pCaption = BOX1_T;
	Form8562->Box1.Font = &Form8562->FontBox;

	/* ��ǩ */
	Form8562->Label1.Left = LBL1_X;
	Form8562->Label1.Top = LBL1_Y;
	Form8562->Label1.MaxLen = 0;
	Form8562->Label1.pCaption = LBL1_T;
	Form8562->Label1.Font = &Form8562->FontBlack;

	Form8562->Label2.Left = LBL2_X;
	Form8562->Label2.Top = LBL2_Y;
	Form8562->Label2.MaxLen = 0;
	Form8562->Label2.pCaption = LBL2_T;
	Form8562->Label2.Font = &Form8562->FontBlue;

	Form8562->Label3.Left = LBL3_X;
	Form8562->Label3.Top = LBL3_Y;
	Form8562->Label3.MaxLen = 0;
	Form8562->Label3.pCaption = LBL3_T;
	Form8562->Label3.Font = &Form8562->FontBlack;

	Form8562->Label4.Left = LBL4_X;
	Form8562->Label4.Top = LBL4_Y;
	Form8562->Label4.MaxLen = 0;
	Form8562->Label4.pCaption = LBL4_T;
	Form8562->Label4.Font = &Form8562->FontBlue;

	/* ��ť */
	Form8562->BtnRet.Left = BTN_RET_X;
	Form8562->BtnRet.Top = BTN_RET_Y;
	Form8562->BtnRet.Height = BTN_RET_H;
	Form8562->BtnRet.Width = BTN_RET_W;
	Form8562->BtnRet.pCaption = BTN_RET_T;
	Form8562->BtnRet.Font = &Form8562->FontBtn;
	Form8562->BtnRet.Focus = 0;

	Form8562->Btn1.Left = BTN1_X;
	Form8562->Btn1.Top = BTN1_Y;
	Form8562->Btn1.Height = BTN1_H;
	Form8562->Btn1.Width = BTN1_W;
	Form8562->Btn1.pCaption = BTN1_T;
	Form8562->Btn1.Font = &Form8562->FontBtn;
	Form8562->Btn1.Focus = 0;

	Form8562->Btn2.Left = BTN2_X;
	Form8562->Btn2.Top = BTN2_Y;
	Form8562->Btn2.Height = BTN2_H;
	Form8562->Btn2.Width = BTN2_W;
	Form8562->Btn2.pCaption = BTN2_T;
	Form8562->Btn2.Font = &Form8562->FontBtn;
	Form8562->Btn2.Focus = 0;

	Form8562->Btn3.Left = BTN3_X;
	Form8562->Btn3.Top = BTN3_Y;
	Form8562->Btn3.Height = BTN3_H;
	Form8562->Btn3.Width = BTN3_W;
	Form8562->Btn3.pCaption = BTN3_T;
	Form8562->Btn3.Font = &Form8562->FontBtn;
	Form8562->Btn3.Focus = 0;

	Form8562->Btn4.Left = BTN4_X;
	Form8562->Btn4.Top = BTN4_Y;
	Form8562->Btn4.Height = BTN4_H;
	Form8562->Btn4.Width = BTN4_W;
	Form8562->Btn4.pCaption = BTN4_T;
	Form8562->Btn4.Font = &Form8562->FontBtn;
	Form8562->Btn4.Focus = 0;

	Form8562->Btn5.Left = BTN5_X;
	Form8562->Btn5.Top = BTN5_Y;
	Form8562->Btn5.Height = BTN5_H;
	Form8562->Btn5.Width = BTN5_W;
	Form8562->Btn5.pCaption = BTN5_T;
	Form8562->Btn5.Font = &Form8562->FontBtn;
	Form8562->Btn5.Focus = 0;

	Form8562->Btn6.Left = BTN6_X;
	Form8562->Btn6.Top = BTN6_Y;
	Form8562->Btn6.Height = BTN6_H;
	Form8562->Btn6.Width = BTN6_W;
	Form8562->Btn6.pCaption = BTN6_T;
	Form8562->Btn6.Font = &Form8562->FontBtn;
	Form8562->Btn6.Focus = 0;

	Form8562->Btn7.Left = BTN7_X;
	Form8562->Btn7.Top = BTN7_Y;
	Form8562->Btn7.Height = BTN7_H;
	Form8562->Btn7.Width = BTN7_W;
	Form8562->Btn7.pCaption = BTN7_T;
	Form8562->Btn7.Font = &Form8562->FontBtn;
	Form8562->Btn7.Focus = 0;

	Form8562->Btn8.Left = BTN8_X;
	Form8562->Btn8.Top = BTN8_Y;
	Form8562->Btn8.Height = BTN8_H;
	Form8562->Btn8.Width = BTN8_W;
	Form8562->Btn8.pCaption = BTN8_T;
	Form8562->Btn8.Font = &Form8562->FontBtn;
	Form8562->Btn8.Focus = 0;

	Form8562->Btn9.Left = BTN9_X;
	Form8562->Btn9.Top = BTN9_Y;
	Form8562->Btn9.Height = BTN9_H;
	Form8562->Btn9.Width = BTN9_W;
	Form8562->Btn9.pCaption = BTN9_T;
	Form8562->Btn9.Font = &Form8562->FontBtn;
	Form8562->Btn9.Focus = 0;

	Form8562->Btn10.Left = BTN10_X;
	Form8562->Btn10.Top = BTN10_Y;
	Form8562->Btn10.Height = BTN10_H;
	Form8562->Btn10.Width = BTN10_W;
	Form8562->Btn10.pCaption = BTN10_T;
	Form8562->Btn10.Font = &Form8562->FontBtn;
	Form8562->Btn10.Focus = 0;

	Form8562->Btn11.Left = BTN11_X;
	Form8562->Btn11.Top = BTN11_Y;
	Form8562->Btn11.Height = BTN11_H;
	Form8562->Btn11.Width = BTN11_W;
	Form8562->Btn11.pCaption = BTN11_T;
	Form8562->Btn11.Font = &Form8562->FontBtn;
	Form8562->Btn11.Focus = 0;

	Form8562->Btn12.Left = BTN12_X;
	Form8562->Btn12.Top = BTN12_Y;
	Form8562->Btn12.Height = BTN12_H;
	Form8562->Btn12.Width = BTN12_W;
	Form8562->Btn12.pCaption = BTN12_T;
	Form8562->Btn12.Font = &Form8562->FontBtn;
	Form8562->Btn12.Focus = 0;

	Form8562->Btn13.Left = BTN13_X;
	Form8562->Btn13.Top = BTN13_Y;
	Form8562->Btn13.Height = BTN13_H;
	Form8562->Btn13.Width = BTN13_W;
	Form8562->Btn13.pCaption = BTN13_T;
	Form8562->Btn13.Font = &Form8562->FontBtn;
	Form8562->Btn13.Focus = 0;

	Form8562->Btn14.Left = BTN14_X;
	Form8562->Btn14.Top = BTN14_Y;
	Form8562->Btn14.Height = BTN14_H;
	Form8562->Btn14.Width = BTN14_W;
	Form8562->Btn14.pCaption = BTN14_T;
	Form8562->Btn14.Font = &Form8562->FontBtn;
	Form8562->Btn14.Focus = 0;

	Form8562->Btn15.Left = BTN15_X;
	Form8562->Btn15.Top = BTN15_Y;
	Form8562->Btn15.Height = BTN15_H;
	Form8562->Btn15.Width = BTN15_W;
	Form8562->Btn15.pCaption = BTN15_T;
	Form8562->Btn15.Font = &Form8562->FontBtn;
	Form8562->Btn15.Focus = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: FormMainDAC8562
*	����˵��: DAC8562����������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void FormMainDAC8562(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t ucTouch;		/* �����¼� */
	uint8_t fQuit = 0;
	int16_t tpX, tpY;
	Form8562_T form;
	uint8_t fDispVolt = 1;

	Form8562 = &form;

	InitForm8562();
	DispForm8562();

	bsp_InitDAC8562();

	MakeSinTable(s_WaveBuf, WAVE_SAMPLES, 0, 65535);

	Form8562->DacValue = 0;

	/* ����������ѭ���� */
	while (fQuit == 0)
	{
		bsp_Idle();

		if (fDispVolt)
		{
			fDispVolt = 0;

			bsp_SetTIMforInt(TIM7, 0, 0, 0); 	/* �ر����ڲ��η����Ķ�ʱ�� */
			
			DAC8562_SetDacData(0, Form8562->DacValue);
			DAC8562_SetDacData(1, Form8562->DacValue);

			DispDacValue();
			DispDacVoltage();
		}

		ucTouch = TOUCH_GetKey(&tpX, &tpY);	/* ��ȡ�����¼� */
		if (ucTouch != TOUCH_NONE)
		{
			switch (ucTouch)
			{
				case TOUCH_DOWN:		/* ���ʰ����¼� */
					if (LCD_ButtonTouchDown(&Form8562->BtnRet, tpX, tpY))
					{
						//fQuit = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn1, tpX, tpY))
					{
						Form8562->DacValue = 0;
						fDispVolt = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn2, tpX, tpY))
					{
						Form8562->DacValue = 32767;
						fDispVolt = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn3, tpX, tpY))
					{
						Form8562->DacValue = 65535;
						fDispVolt = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn4, tpX, tpY))
					{
						Form8562->DacValue++;
						fDispVolt = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn5, tpX, tpY))
					{
						Form8562->DacValue--;
						fDispVolt = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn6, tpX, tpY))
					{
						Form8562->DacValue += 100;
						fDispVolt = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn7, tpX, tpY))
					{
						Form8562->DacValue -= 100;
						fDispVolt = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn8, tpX, tpY))
					{
						/* +1mV */
						Form8562->Voltage = DAC8562_DacToVoltage(Form8562->DacValue);
						Form8562->Voltage += 10;
						Form8562->DacValue = DAC8562_VoltageToDac(Form8562->Voltage);
						fDispVolt = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn9, tpX, tpY))
					{
						/* -1mV */
						Form8562->Voltage = DAC8562_DacToVoltage(Form8562->DacValue);
						Form8562->Voltage -= 10;
						Form8562->DacValue = DAC8562_VoltageToDac(Form8562->Voltage);
						fDispVolt = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn10, tpX, tpY))
					{
						/* +100mV */
						Form8562->Voltage = DAC8562_DacToVoltage(Form8562->DacValue);
						Form8562->Voltage += 1000;
						Form8562->DacValue = DAC8562_VoltageToDac(Form8562->Voltage);
						fDispVolt = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn11, tpX, tpY))
					{
						/* -100mV */
						Form8562->Voltage = DAC8562_DacToVoltage(Form8562->DacValue);
						Form8562->Voltage -= 1000;
						Form8562->DacValue = DAC8562_VoltageToDac(Form8562->Voltage);
						fDispVolt = 1;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn12, tpX, tpY))
					{
						bsp_SetTIMforInt(TIM7, DAC_OUT_FREQ, 0, 0);
						s_WavePos1 = 0;					/* ����1��ǰ 0�� */
						s_WavePos2 = 0;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn13, tpX, tpY))
					{
						bsp_SetTIMforInt(TIM7, DAC_OUT_FREQ, 0, 0);
						s_WavePos1 = WAVE_SAMPLES / 4;	/* ����1��ǰ 90�� */
						s_WavePos2 = 0;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn14, tpX, tpY))
					{
						bsp_SetTIMforInt(TIM7, DAC_OUT_FREQ, 0, 0);
						s_WavePos1 = WAVE_SAMPLES / 2;	/* ����1��ǰ 180�� */
						s_WavePos2 = 0;
					}
					else if (LCD_ButtonTouchDown(&Form8562->Btn15, tpX, tpY))
					{
						bsp_SetTIMforInt(TIM7, 0, 0, 0); 	/* �ر����ڲ��η����Ķ�ʱ�� */
						fDispVolt = 1;
					}
					break;

				case TOUCH_RELEASE:		/* �����ͷ��¼� */
					if (LCD_ButtonTouchRelease(&Form8562->BtnRet, tpX, tpY))
					{
						fQuit = 1;	/* ���� */
					}
					else
					{
						LCD_ButtonTouchRelease(&Form8562->BtnRet, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn1, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn2, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn3, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn4, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn5, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn6, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn7, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn8, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn9, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn10, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn11, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn12, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn13, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn14, tpX, tpY);
						LCD_ButtonTouchRelease(&Form8562->Btn15, tpX, tpY);
					}
					break;
			}
		}

		/* �������¼� */
		ucKeyCode = bsp_GetKey();
		if (ucKeyCode > 0)
		{
			/* �м����� */
			switch (ucKeyCode)
			{
				case KEY_DOWN_K1:		/* K1�� + 1*/
					break;

				case KEY_DOWN_K2:		/* K2�� - 1 */
					break;

				case KEY_DOWN_K3:		/* K3�� - ��0 */
					break;

				case JOY_DOWN_U:		/* ҡ��UP������ */
					break;

				case JOY_DOWN_D:		/* ҡ��DOWN������ */
					break;

				case JOY_DOWN_L:		/* ҡ��LEFT������ */
					break;

				case JOY_DOWN_R:		/* ҡ��RIGHT������ */
					break;

				case JOY_DOWN_OK:		/* ҡ��OK������ */
					break;

				default:
					break;
			}
		}
	}

	bsp_SetTIMforInt(TIM7, 0, 0, 0); 	/* �ر����ڲ��η����Ķ�ʱ�� */
}

/*
*********************************************************************************************************
*	�� �� ��: DispForm8562
*	����˵��: ��ʾ���еľ�̬�ؼ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispForm8562(void)
{
	LCD_ClrScr(CL_BTN_FACE);

	/* ����� */
	LCD_DrawGroupBox(&Form8562->Box1);

	LCD_DrawLabel(&Form8562->Label1);
	LCD_DrawLabel(&Form8562->Label2);
	LCD_DrawLabel(&Form8562->Label3);
	LCD_DrawLabel(&Form8562->Label4);

	/* ��ť */
	LCD_DrawButton(&Form8562->Btn1);
	LCD_DrawButton(&Form8562->Btn2);
	LCD_DrawButton(&Form8562->Btn3);
	LCD_DrawButton(&Form8562->Btn4);
	LCD_DrawButton(&Form8562->Btn5);
	LCD_DrawButton(&Form8562->Btn6);
	LCD_DrawButton(&Form8562->Btn5);
	LCD_DrawButton(&Form8562->Btn6);
	LCD_DrawButton(&Form8562->Btn7);
	LCD_DrawButton(&Form8562->Btn8);
	LCD_DrawButton(&Form8562->Btn9);
	LCD_DrawButton(&Form8562->Btn10);
	LCD_DrawButton(&Form8562->Btn11);
	LCD_DrawButton(&Form8562->Btn12);
	LCD_DrawButton(&Form8562->Btn13);
	LCD_DrawButton(&Form8562->Btn14);
	LCD_DrawButton(&Form8562->Btn15);
	LCD_DrawButton(&Form8562->BtnRet);
}

/*
*********************************************************************************************************
*	�� �� ��: DispDacValue
*	����˵��: ��ʾ��ǰDAC��ֵ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispDacValue(void)
{
	char buf[10];

	sprintf(buf, "%d", Form8562->DacValue);

	Form8562->Label2.pCaption = buf;
	LCD_DrawLabel(&Form8562->Label2);
}

/*
*********************************************************************************************************
*	�� �� ��: DispDacVoltage
*	����˵��: ��ʾ��ѹ
*	��    ��: ��
*			  _
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispDacVoltage(void)
{
	char buf[10];

	Form8562->Voltage = DAC8562_DacToVoltage(Form8562->DacValue);

	sprintf(buf, "%d.%04dV", Form8562->Voltage / 10000, (abs(Form8562->Voltage) % 10000));

	Form8562->Label4.pCaption = buf;
	LCD_DrawLabel(&Form8562->Label4);
}


/*
*********************************************************************************************************
*	�� �� ��: MakeSinTable
*	����˵��: ����������Ҳ�����
*	��    ��: _pBuf : Ŀ�껺����
*			  _usSamples : ÿ�����ڵ������� ���������32��������ż����
*			 _usBottom : ����ֵ
*			 _usTop : ����ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MakeSinTable(uint16_t *_pBuf, uint16_t _usSamples, uint16_t _usBottom, uint16_t _usTop)
{
	uint16_t i;
	uint16_t mid;	/* ��ֵ */
	uint16_t att;	/* ���� */

	mid = (_usBottom + _usTop) / 2;	/* 0λ��ֵ */
	att = (_usTop - _usBottom) / 2;  	/* ���Ҳ����ȣ����ֵ����2 */

	for (i = 0; i < _usSamples; i++)
	{
		_pBuf[i] = mid + (int32_t)(att * sin((i * 2 * 3.14159) / _usSamples));
	}
}

/*
*********************************************************************************************************
*	�� �� ��: TIM7_IRQHandler
*	����˵��: TIM7 �жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TIM7_IRQHandler(void)
{
	uint16_t dac;

	if (READ_BIT(TIM7->SR, TIM_IT_UPDATE) != 0)
	{		
		CLEAR_BIT(TIM7->SR, TIM_IT_UPDATE);

		/* 3.5��������������������ڷ���SPI����оƬTSC2046���������� */
		if (bsp_SpiBusBusy())
		{
			return;
		}

		/* ����1 */
		dac = s_WaveBuf[s_WavePos1++];
		if (s_WavePos1 >= WAVE_SAMPLES)
		{
			s_WavePos1 = 0;
		}
		DAC8562_SetDacData(0, dac);		/* �ı��1ͨ�� DAC�����ѹ */

		/* ����1 */
		dac = s_WaveBuf[s_WavePos2++];
		if (s_WavePos2 >= WAVE_SAMPLES)
		{
			s_WavePos2 = 0;
		}
		DAC8562_SetDacData(1, dac);		/* �ı��2ͨ�� DAC�����ѹ */
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
