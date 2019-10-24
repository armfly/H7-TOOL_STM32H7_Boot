/*
*********************************************************************************************************
*
*	ģ������ : lua�ӿ�ģ��
*	�ļ����� : lua_if.h
*	��    �� : V1.0
*	˵    �� : ��
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2015-04-25 armfly  ST�̼���汾 V2.1.0
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __LUA_IF_H
#define __LUA_IF_H

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if_i2c.h"
#include "lua_if_gpio.h"
#include "lua_if_spi.h"
#include "lua_if_qspi.h"
#include "lua_if_tcp_udp.h"
#include "lua_if_tim.h"
#include "lua_if_fatfs.h"

#define LUA_PROG_LEN_MAX	(32 * 1024)

#define LUA_READ_LEN_MAX	(2 * 1024)

void lua_Test(void);
void lua_Init(void);
void lua_DeInit(void);
void lua_DownLoad(uint32_t _addr, uint8_t *_buf, uint32_t _len, uint32_t _total_len);
void lua_Run(void);
uint8_t lua_66H_Write(uint32_t _addr, uint8_t *_buf, uint32_t _len);
uint8_t lua_67H_Read(uint32_t _addr, uint8_t *_buf, uint32_t _len);

extern lua_State *g_Lua;

extern uint8_t s_lua_read_buf[LUA_READ_LEN_MAX];
extern uint8_t s_lua_read_len;

extern char s_lua_prog_buf[LUA_PROG_LEN_MAX];
extern uint32_t s_lua_prog_len;

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
