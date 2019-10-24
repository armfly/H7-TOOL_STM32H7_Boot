#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

static int lua_SetTVCC(lua_State* L);
static int lua_GpioCfg(lua_State* L);
static int lua_GpioWrite(lua_State* L);
static int lua_GpioRead(lua_State* L);

/*
*********************************************************************************************************
*	�� �� ��: lua_GpioCfg
*	����˵��: ����GPIO����
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void lua_gpio_RegisterFun(void)
{
	//��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
    lua_register(g_Lua, "gpio_cfg",  lua_GpioCfg);		/* gpio_cfg(0, 1) */
	lua_register(g_Lua, "gpio_write", lua_GpioWrite);	/* gpio_write(0, 1) */
	lua_register(g_Lua, "gpio_read", lua_GpioRead);	/* gpio_write(0, 1) */
	
	lua_register(g_Lua, "set_tvcc", lua_SetTVCC);	/* */
	
}

/*
*********************************************************************************************************
*	�� �� ��: lua_GpioCfg
*	����˵��: ����GPIO����
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_GpioCfg(lua_State* L)
{
	uint8_t _no;
	uint8_t _dir;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		_no = luaL_checknumber(L, 1);
	}
	else
	{
		return 1;
	}

	if (lua_type(L, 2) == LUA_TNUMBER) /* �жϵ�2������ */
	{
		_dir = luaL_checknumber(L, 2);
	}
	else
	{
		return 1;
	}
	
	if (_dir == 0)
	{
		EIO_ConfigPort(_no, ES_GPIO_IN);
	}
	else if (_dir == 1)
	{
		EIO_ConfigPort(_no, ES_GPIO_OUT);
	}	
	else if (_dir == 2)
	{
		EIO_ConfigPort(_no, ES_FMC_OUT);
	}
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_GpioWrite
*	����˵��: ����GPIO�����ƽ
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_GpioWrite(lua_State* L)
{
	uint8_t _no;
	uint8_t _value;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		_no = luaL_checknumber(L, 1);
	}
	else
	{
		return 1;
	}

	if (lua_type(L, 2) == LUA_TNUMBER) /* �жϵ�2������ */
	{
		_value = luaL_checknumber(L, 2);
	}
	else
	{
		return 1;
	}
	
	if (_value == 0)
	{
		EIO_SetOutLevel(_no, 0);
	}
	else if (_value == 1)
	{
		EIO_SetOutLevel(_no, 1);
	}	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_GpioRead
*	����˵��: ��ȡGPIO�����ƽ
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_GpioRead(lua_State* L)
{
	uint8_t _no;
	uint8_t _value;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		_no = luaL_checknumber(L, 1);
	}
	else
	{
		return 0;
	}

	if (EIO_GetInputLevel(_no) == 0)
	{
		lua_pushnumber(L, 0);	/* ����ֵ */
	}
	else
	{
		lua_pushnumber(L, 1);	/* ����ֵ */
	}
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_SetTVCC
*	����˵��: ����TVCC��ѹ
*	��    ��: ����IO��ѹ. ��λ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_SetTVCC(lua_State* L)
{
	float volt;

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		volt = luaL_checknumber(L, 1);
		bsp_SetTVCC(volt * 1000);
	}
	return 1;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
