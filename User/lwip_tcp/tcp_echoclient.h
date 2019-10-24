/**
  ******************************************************************************
  * @file    tcp_echoclient.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013 
  * @brief   Header file for tcp_echoclient.c
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TCP_ECHOCLIENT_H__
#define __TCP_ECHOCLIENT_H__

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#define TCP_Tx_SIZE		64
#define TCP_Rx_SIZE		2048 + 10

/* �û�TCP����/���ͽṹ�� */
typedef struct
{
	uint8_t TcpState;			/* TCP����״̬ 0��ʾû���ӣ�1��ʾ������ */
	uint8_t TxData[TCP_Tx_SIZE];	/* TCP�������ݻ����� */
	uint16_t TxCount;				/* TCP���͵����ݳ��� */
	uint8_t RxData[TCP_Rx_SIZE];	/* TCP�������ݻ����� */
}TCP_USER_T;

/* ECHO protocol states */
enum echoclient_states
{
  ES_NOT_CONNECTED = 0,
  ES_CONNECTED,
  ES_RECEIVED,
  ES_CLOSING,
};

/* structure to be passed as argument to the tcp callbacks */
struct echoclient
{
  enum echoclient_states state; /* connection status */
  struct tcp_pcb *pcb;          /* pointer on the current tcp_pcb */
  struct pbuf *p_tx;            /* pointer on pbuf to be transmitted */
};

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void tcp_echoclient_connect(void);

void tcp_echoclient_connection_close(struct tcp_pcb *tpcb, struct echoclient * es);
void tcp_client_usersent(struct tcp_pcb *_tpcb);		/* �û��������� */
extern struct tcp_pcb *echoclient_pcb;

extern TCP_USER_T g_tClient;
extern uint8_t g_fTcpState;		/* TCP���� */

#endif /* __TCP_ECHOCLIENT_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
