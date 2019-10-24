#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

static int lua_I2C_Start(lua_State* L);
static int lua_I2C_Stop(lua_State* L);
static int lua_I2C_SendBytes(lua_State* L);
static int lua_I2C_ReciveBytes(lua_State* L);

void lua_i2c_RegisterFun(void)
{
	//��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
    lua_register(g_Lua, "i2c_start", lua_I2C_Start);
    lua_register(g_Lua, "i2c_stop", lua_I2C_Stop);
	lua_register(g_Lua, "i2c_send", lua_I2C_SendBytes);
	lua_register(g_Lua, "i2c_recive", lua_I2C_ReciveBytes);
}

static int lua_I2C_Start(lua_State* L)
{
	i2c_Start();
	return 1;
}

static int lua_I2C_Stop(lua_State* L)
{
	i2c_Stop();
	return 1;
}

/*
	��ʽ1: i2c_send(buf);
	��ʽ2: i2c_send(32);	
*/
static int lua_I2C_SendBytes(lua_State* L)
{
	size_t i;
	size_t len;
	const char *data;
	uint8_t buf[1];
	int re;

	if (lua_type(L, 1) == LUA_TSTRING) 	/* �жϵ�1������ */
	{		
		data = luaL_checklstring(L, 1, &len); /* 1�ǲ�����λ�ã� len��string�ĳ��� */		
	}
	
	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		len = 1;
		buf[0] = luaL_checknumber(L, 1);
		
		data = (const char *)buf;
	}
	
	re = 1;
	for (i = 0; i < len; i++)
	{
		i2c_SendByte(data[i]);	
		if (i2c_WaitAck() != 0)
		{
			i2c_Stop();
			re = 0;
			break;
		}		
	}
	lua_pushnumber(L, re);	/* ����ֵ */
	return 1;
}

/*
Lua��C++��bufferʱ��ʹ��string���У���C++�ģ�tolua++��ʹ����������ȡbuffer
        size_t ld;
        const char *data = luaL_checklstring(tolua_S, 2, &ld); // 2�ǲ�����λ�ã� ld��buffer�ĳ���
C++��Lua��bufferʱ����C++������ʹ��������봫��buffer
            LuaStack *stack = LuaEngine::getInstance()->getLuaStack();
            stack->

*/
// lua_I2C_ReciveBytes(2);
static int lua_I2C_ReciveBytes(lua_State* L)
{
	size_t len;
	size_t i;
	
	len = luaL_checknumber(L, 1);
	if (len == 0 || len > LUA_READ_LEN_MAX)
	{
		lua_pushnumber(L, 0);	/* ����ֵ */
		return 0;
	}

	for (i = 0; i < len; i++)
	{
		s_lua_read_buf[i] = i2c_ReadByte();	/* ��1���ֽ� */

		/* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
		if (i != len - 1)
		{
			i2c_Ack();	/* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
		}
		else
		{
			i2c_NAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		}
	}
	
	//lua_pushnumber(L, 1);	/* ����ֵ */
//	strcpy(s_lua_read_buf, "s_lua_read_buf");
	lua_pushlstring(L, (char *)s_lua_read_buf, len); 
	return 1;
}







