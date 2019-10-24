#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

static int lua_udp_print(lua_State* L);

void lua_tcp_RegisterFun(void)
{
	//��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
    lua_register(g_Lua, "udp_print", lua_udp_print);	
}

/*
*********************************************************************************************************
*	�� �� ��: lua_udp_SendBytes
*	����˵��: ��UDP����һ�����ݡ�UDPĿ��IP�����һ�ν��յ���UDP������ȡ��Ҳ����ֻ�������һ��ͨ�ŵ�����
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
extern void udp_SendBuf(uint8_t *_buf, uint16_t _len, uint16_t _port);
static int lua_udp_print(lua_State* L)
{
	size_t len;
	const char *data;

	if (lua_type(L, 1) == LUA_TSTRING) 	/* �жϵ�1������ */
	{		
		data = luaL_checklstring(L, 1, &len); /* 1�ǲ�����λ�ã� len��string�ĳ��� */
		udp_SendBuf((uint8_t *)data, len, 777);
	}
	
	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		char buf[32];
		double num;
		
		len = 1;
		num = luaL_checknumber(L, 1);
		sprintf(buf, "%f", num);

		udp_SendBuf((uint8_t *)buf, strlen(buf), 777);
	}
		
	return 1;
}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
