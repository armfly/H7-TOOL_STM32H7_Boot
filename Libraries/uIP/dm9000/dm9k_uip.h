/****************************************************************************
* Copyright (C), 2009-2010, www.armfly.com  ����������
*
* �ļ���: dm9k_uip.c
* ���ݼ���: Davicom DM9000A uP NIC fast Ethernet driver for uIP.
*
* �ļ���ʷ:
* �汾��  ����       ����    ˵��
* v0.1    2010-01-18 armfly  �������ļ�
*
*/

#ifndef _DM9K_UIP_H_
#define _DM9K_UIP_H_

#include <inttypes.h>

/* DM9000 REGISTER LIST */
#define DM9000_REG_NCR        0x00
#define DM9000_REG_NSR        0x01
#define DM9000_REG_TCR        0x02
#define DM9000_REG_TSR1       0x03
#define DM9000_REG_TSR2       0x04
#define DM9000_REG_RCR        0x05
#define DM9000_REG_RSR        0x06
#define DM9000_REG_ROCR       0x07
#define DM9000_REG_BPTR       0x08
#define DM9000_REG_FCTR       0x09
#define DM9000_REG_FCR        0x0A
#define DM9000_REG_EPCR       0x0B
#define DM9000_REG_EPAR       0x0C
#define DM9000_REG_EPDRL      0x0D
#define DM9000_REG_EPDRH      0x0E
#define DM9000_REG_WAR        0x0F
#define DM9000_REG_PAR        0x10
#define DM9000_REG_MAR        0x16
#define DM9000_REG_GPCR       0x1E
#define DM9000_REG_GPR        0x1F
#define DM9000_REG_VID_L      0x28
#define DM9000_REG_VID_H      0x29
#define DM9000_REG_PID_L      0x2A
#define DM9000_REG_PID_H      0x2B
#define DM9000_REG_CHIPR      0x2C
#define DM9000_REG_TCR2       0x2D
#define DM9000_REG_OTCR       0x2E
#define DM9000_REG_SMCR       0x2F
#define DM9000_REG_ETXCSR     0x30
#define DM9000_REG_TCSCR      0x31
#define DM9000_REG_RCSCSR     0x32
#define DM9000_REG_MRCMDX     0xF0
#define DM9000_REG_MRCMD      0xF2
#define DM9000_REG_MRRL       0xF4
#define DM9000_REG_MRRH       0xF5
#define DM9000_REG_MWCMDX     0xF6
#define DM9000_REG_MWCMD      0xF8
#define DM9000_REG_MWRL       0xFA
#define DM9000_REG_MWRH       0xFB
#define DM9000_REG_TXPLL      0xFC
#define DM9000_REG_TXPLH      0xFD
#define DM9000_REG_ISR        0xFE
#define DM9000_REG_IMR        0xFF


/* ��غ����� */
#define DM9000A_ID_OK       0x0A469000

#define DM9000_BYTE_MODE      0x01
#define DM9000_WORD_MODE      0x00
#define DM9000_PHY            0x40
#define DM9000_PKT_RDY        0x01
#define DM9000_PKT_NORDY      0x00
#define DM9000_REG_RESET      0x03

#define DM9000_RX_INTR        0x01                /* �����ж��ж� bit */
#define DM9000_TX_INTR        0x02                /* �����ж��ж� bit */
#define DM9000_OVERFLOW_INTR  0x04                /* �ڴ�����ж��ж� bit */
#define DM9000_LINK_CHANG     0x20                /* ���ӱ䶯�ж��ж� bit */

#define DM9000_PHY_ON        0x00                /* �趨 PHY ���� */
#define DM9000_PHY_OFF        0x01                /* �趨 PHY �ر� */
#define DM9000_RCR_SET        0x31                /* �趨 ���չ��� (���� CRC �� ������) */
#define DM9000_TCR_SET        0x01                /* �趨 ���͹��� */
#define DM9000_RCR_OFF        0x00                /* �趨 ���չ��ܹعر����� */
#define DM9000_BPTR_SET       0x37                /* �趨 Back Pressure �������� */
#define DM9000_FCTR_SET       0x38                /* �趨 Flow Control �������� */
#define DM9000_TCR2_SET       0x80                /* ���� LED ��ʾģʽ */
#define DM9000_OTCR_SET       0x80                /* ���� DM9000 ����Ƶ�� 0x80 = 100Mhz */
#define DM9000_ETXCSR_SET     0x83                /* ���� Early Tramsmit �������� */
#define DM9000_FCR_SET        0x28                /* ���� �������ع������� */
#define DM9000_TCSCR_SET      0x07                /* �趨 CHECKSUM �������� ���� */
#define DM9000_RCSCSR_SET     0x03                /* �趨 CHECKSUM ���ռ�� ���� */
#define DM9000_IMR_SET        0x81                /* �趨 �����ж�ʹ�� �������� */
#define DM9000_IMR_OFF        0x80                /* �趨 �ر��ж�ʹ�� �������� */


/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void etherdev_init(void);
void etherdev_send(uint8_t *p_char, uint16_t length);
uint16_t etherdev_read(uint8_t *p_char);
uint16_t dm9k_receive_packet(uint8_t *_uip_buf);
unsigned short etherdev_poll(void);
void etherdev_chkmedia(void);
uint32_t dm9k_ReadID(void);

void dm9k_debug_test(void);

#endif
