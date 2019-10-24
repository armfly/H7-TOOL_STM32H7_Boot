/*
*********************************************************************************************************
*
*	ģ������ : lwip Э��ջ�û��ӿ�
*	�ļ����� : lwip_user.c
*	��    �� : V1.0
*	˵    �� : �ṩWEB���������ܡ�
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-12-05  armfly  ��ʽ����
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "stdio.h"
#include "bsp.h"
#include "num_pad.h"
#include "param.h"

/* for LwIP */
//#include "netconf.h"
//#include "main_lwip.h"
//#include "httpd_w.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"

#include "ethernetif.h"
#include "app_ethernet.h"
#include "http_cgi_ssi.h"

#include "tcp_echoserver.h"
#include "lwip_user.h"
#include "net_udp.h"

static uint8_t s_init_lwip_ok = 0;

struct netif gnetif;

static void Netif_Config(void)
{
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;

	#if LWIP_DHCP 
		ip_addr_set_zero_ip4(&ipaddr);
		ip_addr_set_zero_ip4(&netmask);
		ip_addr_set_zero_ip4(&gw);
	#else	
		/* IP address default setting */
		IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
		IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
		IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3); 
	#endif

	/* add the network interface */    
	netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

	/*  Registers the default network interface */
	netif_set_default(&gnetif);

	ethernet_link_status_updated(&gnetif);

	#if LWIP_NETIF_LINK_CALLBACK  
		netif_set_link_callback(&gnetif, ethernet_link_status_updated);
	#endif
}

/*
*********************************************************************************************************
*	�� �� ��: lwip_start
*	����˵��: ����lwip_start,  �������������ȫ�ֱ��� g_tParam.lwip_ip, g_tParam.lwip_net_mask, 
*			  g_tParam.lwip_gateway 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void lwip_start(void)
{
	/* Initialize the LwIP stack */
	lwip_init();

	/* Configure the Network interface */
	Netif_Config();

	/* Http webserver Init */
	http_server_init();
	
	/* tcp server init */
	tcp_echoserver_init();
	
	udp_server_init();	/* ����UDP���� */
	
	s_init_lwip_ok = 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lwip_pro
*	����˵��: lwip ��ѯ�����뵽��ѭ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void lwip_pro(void)
{
	if (s_init_lwip_ok == 0)
	{
		return;
	}
	
    /* Read a received packet from the Ethernet buffers and send it 
       to the lwIP for handling */
    ethernetif_input(&gnetif);

    /* Handle timeouts */
    sys_check_timeouts();

#if LWIP_NETIF_LINK_CALLBACK     
    Ethernet_Link_Periodic_Handle(&gnetif);
#endif
    
#if LWIP_DHCP   
    DHCP_Periodic_Handle(&gnetif);
#endif 
}



/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
