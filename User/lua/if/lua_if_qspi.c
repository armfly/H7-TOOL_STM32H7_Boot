#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

static int qspi_readid(lua_State* L);
static int qspi_write(lua_State* L);
static int qspi_read(lua_State* L);
static int qspi_erase4k(lua_State* L);

void lua_qspi_RegisterFun(void)
{
	//��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
    lua_register(g_Lua, "qspi_readid", qspi_readid);	
	lua_register(g_Lua, "qspi_read", qspi_read);	
	lua_register(g_Lua, "qspi_write", qspi_write);
	lua_register(g_Lua, "qspi_erase4k", qspi_erase4k);
}

/*
*********************************************************************************************************
*	�� �� ��: qspi_readid
*	����˵��: ��оƬID
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int qspi_readid(lua_State* L)
{
	uint32_t id;
	
	id = QSPI_ReadID();
	lua_pushnumber(L, id);	/* ����ֵ */
		
	return 1;
}


/*
*********************************************************************************************************
*	�� �� ��: qspi_erase4k
*	����˵��: ����һ��������4KB
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int qspi_erase4k(lua_State* L)
{
	uint32_t addr;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�2������ */
	{
		addr = luaL_checknumber(L, 1);
		
		addr = addr / 4096;
		addr = addr * 4096;
	}

	QSPI_EraseSector(addr);
	lua_pushnumber(L, 1);	/* �ɹ� */

	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: qspi_write
*	����˵��: дһ�����ݣ�С��256�ֽ�
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int qspi_write(lua_State* L)
{
	size_t len;
	const char *data;
	uint32_t addr;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�2������ */
	{
		addr = luaL_checknumber(L, 1);
	}
	
	if (lua_type(L, 2) == LUA_TSTRING) 	/* �жϵ�1������ */
	{		
		data = luaL_checklstring(L, 2, &len); /* 1�ǲ�����λ�ã� len��string�ĳ��� */		
	}
	
	if (len > QSPI_PAGE_SIZE)
	{
		lua_pushnumber(L, 0);	/* ���� */
		return 1;
	}
	
	if (QSPI_WriteBuffer((uint8_t *)data, addr, len) == 0)
	{
		lua_pushnumber(L, 0);	/* ���� */
	}
	else
	{
		lua_pushnumber(L, 1);	/* �ɹ� */
	}
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: qspi_read
*	����˵��: ��һ�����ݣ��ɴ���256�ֽ�
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int qspi_read(lua_State* L)
{
	uint32_t addr;
	uint32_t num;

	if (lua_type(L, 1) == LUA_TNUMBER) 	/* �жϵ�1������ */
	{		
		addr = luaL_checknumber(L, 1); /* 1�ǲ�����λ�ã� len��string�ĳ��� */		
	}
	
	if (lua_type(L, 2) == LUA_TNUMBER) /* �жϵ�2������ */
	{
		num = luaL_checknumber(L, 2);
		
		memset(s_lua_read_buf, 0, num);
		
	}
	
	if (num > LUA_READ_LEN_MAX)
	{
		return 0;
	}
		
	QSPI_ReadBuffer(s_lua_read_buf, addr, num);

	lua_pushlstring(L, (char *)s_lua_read_buf, num); 
	return 1;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
