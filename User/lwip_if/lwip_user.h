/*
*********************************************************************************************************
*
*	ģ������ : lwipЭ��ջӦ�ýӿ�
*	�ļ����� : lwip_user.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LWIP_USER
#define _LWIP_USER

#if 1	
	/* IP��ַ */
	#define IP_ADDR0   g_tParam.LocalIPAddr[0]
	#define IP_ADDR1   g_tParam.LocalIPAddr[1]
	#define IP_ADDR2   g_tParam.LocalIPAddr[2]
	#define IP_ADDR3   g_tParam.LocalIPAddr[3]
	
	/* �������� */
	#define NETMASK_ADDR0   g_tParam.NetMask[0]
	#define NETMASK_ADDR1   g_tParam.NetMask[1]
	#define NETMASK_ADDR2   g_tParam.NetMask[2]
	#define NETMASK_ADDR3   g_tParam.NetMask[3]

	/* ���� */
	#define GW_ADDR0   g_tParam.Gateway[0]
	#define GW_ADDR1   g_tParam.Gateway[1]
	#define GW_ADDR2   g_tParam.Gateway[2]
	#define GW_ADDR3   g_tParam.Gateway[3]
#else
	/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
	#define IP_ADDR0   ((uint8_t) 192U)
	#define IP_ADDR1   ((uint8_t) 168U)
	#define IP_ADDR2   ((uint8_t) 1U)
	#define IP_ADDR3   ((uint8_t) 86U)
	   
	/*NETMASK*/
	#define NETMASK_ADDR0   ((uint8_t) 255U)
	#define NETMASK_ADDR1   ((uint8_t) 255U)
	#define NETMASK_ADDR2   ((uint8_t) 255U)
	#define NETMASK_ADDR3   ((uint8_t) 0U)

	/*Gateway Address*/
	#define GW_ADDR0   ((uint8_t) 192U)
	#define GW_ADDR1   ((uint8_t) 168U)
	#define GW_ADDR2   ((uint8_t) 1U)
	#define GW_ADDR3   ((uint8_t) 1U) 
#endif

void lwip_start(void);
void lwip_pro(void);


#endif


