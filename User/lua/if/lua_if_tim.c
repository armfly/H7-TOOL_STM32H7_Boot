#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

static int lua_udp_print(lua_State* L);

void lua_tim_RegisterFun(void)
{
	//将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    lua_register(g_Lua, "udp_print", lua_udp_print);	
}

/*
*********************************************************************************************************
*	函 数 名: lua_udp_SendBytes
*	功能说明: 向UDP发送一包数据。UDP目标IP由最后一次接收到的UDP包中提取，也就是只发给最后一次通信的主机
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
extern void udp_SendBuf(uint8_t *_buf, uint16_t _len, uint16_t _port);
static int lua_udp_print(lua_State* L)
{
	size_t len;
	const char *data;

	if (lua_type(L, 1) == LUA_TSTRING) 	/* 判断第1个参数 */
	{		
		data = luaL_checklstring(L, 1, &len); /* 1是参数的位置， len是string的长度 */
		udp_SendBuf((uint8_t *)data, len, 777);
	}
	
	if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
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


/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
