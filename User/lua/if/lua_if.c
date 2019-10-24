#include "lua_if.h"
#include "bsp.h"
#include "param.h"

/* 
	lua ���ӵ��Դ���ķ���:

lobject.c�ļ�:
	const char *luaO_pushfstring (lua_State *L, const char *fmt, ...) {
	#if 1		
	  printf("%s", msg);
	#endif

ldebug.c �ļ� luaG_runerror ����������printf

lauxlib.c �ļ� luaL_error����������printf

ldo.c �ļ� luaD_throw ����	printf("\r\nthrow errcode=%d\r\n", errcode);

lua.h ����������
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRGCMM	5
#define LUA_ERRERR	6

*/

/*
	luaconf.h �ļ��Ը���������Ĵ��� ȱʡ64λ������˫���ȸ���
		default configuration for 64-bit Lua ('long long' and 'double')
*/

lua_State *g_Lua = 0;

char s_lua_prog_buf[LUA_PROG_LEN_MAX];
uint32_t s_lua_prog_len;
uint32_t s_lua_func_init_idx;
uint32_t s_lua_func_write_idx;
uint32_t s_lua_func_read_idx;

uint8_t s_lua_read_buf[LUA_READ_LEN_MAX];
uint8_t s_lua_read_len;

static void lua_RegisterFunc(void);

void exit(int status)
{
	;
}

int system(const char *cmd)
{
	return 0;
}

/* time_t : date/time in unix secs past 1-Jan-70 */
time_t time(time_t *_t)
{
	/* ���´��������ڣ� https://blog.csdn.net/qq_29350001/article/details/87637350 */
	#define xMINUTE (60)		  /* 1�ֵ����� */
	#define xHOUR   (60*xMINUTE)  /* 1Сʱ������ */
	#define xDAY    (24*xHOUR   ) /* 1������� */
	#define xYEAR   (365*xDAY   ) /* 1������� */

	/* ��localtime��UTC+8����ʱ�䣩תΪUNIX TIME����1970��1��1��Ϊ��� */
	static unsigned int  month[12] =
	{
		/*01��*/xDAY*(0),
		/*02��*/xDAY*(31),
		/*03��*/xDAY*(31+28),
		/*04��*/xDAY*(31+28+31),
		/*05��*/xDAY*(31+28+31+30),
		/*06��*/xDAY*(31+28+31+30+31),
		/*07��*/xDAY*(31+28+31+30+31+30),
		/*08��*/xDAY*(31+28+31+30+31+30+31),
		/*09��*/xDAY*(31+28+31+30+31+30+31+31),
		/*10��*/xDAY*(31+28+31+30+31+30+31+31+30),
		/*11��*/xDAY*(31+28+31+30+31+30+31+31+30+31),
		/*12��*/xDAY*(31+28+31+30+31+30+31+31+30+31+30)
	};
	unsigned int  seconds = 0;
	unsigned int  year = 0;
  
	RTC_ReadClock();
	year = g_tRTC.Year - 1970;						/* ������2100��ǧ������� */
	seconds = xYEAR * year + xDAY * ((year + 1) /4);  /* ǰ�����ȥ������ */
	seconds += month[g_tRTC.Mon - 1];      			/* ���Ͻ��걾�¹�ȥ������ */
	if ((g_tRTC.Mon > 2) && (((year + 2) % 4) == 0))		/* 2008��Ϊ���� */
	seconds += xDAY;            					/* �����1������ */
	seconds += xDAY * (g_tRTC.Day-1); 	/* ���ϱ����ȥ������ */
	seconds += xHOUR * g_tRTC.Hour;		/* ���ϱ�Сʱ��ȥ������ */
	seconds += xMINUTE * g_tRTC.Min;		/* ���ϱ����ӹ�ȥ������ */
	seconds += g_tRTC.Sec;				/* ���ϵ�ǰ����<br>��seconds -= 8 * xHOUR; */
	*_t = seconds;
	return *_t;
}

/*
print(\"Hello,I am lua!\\n--this is newline printf\")
function foo()
  local i = 0
  local sum = 1
    while i <= 10 do
         sum = sum * 2
         i = i + 1
    end
return sum
 end
print(\"sum =\", foo())
print(\"and sum = 2^11 =\", 2 ^ 11)
print(\"exp(200) =\", math.exp(200))
*/
const char lua_test[] = { 
    "print(\"Hello,I am lua!\\n--this is newline printf\")\n"
    "function foo()\n"
    "  local i = 0\n"
    "  local sum = 1\n"
    "  while i <= 10 do\n"
    "    sum = sum * 2\n"
    "    i = i + 1\n"
    "  end\n"
    "return sum\n"
    "end\n"
    "print(\"sum =\", foo())\n"
    "print(\"and sum = 2^11 =\", 2 ^ 11)\n"
    "print(\"exp(200) =\", math.exp(200))\n"
};

void lua_Test(void)
{
	luaL_dostring(g_Lua, lua_test); /* ����Lua�ű� */
	
	luaL_dostring(g_Lua, "print(add_f(1.0, 9.09))\n print(sub_f(20.1,19.01))");
}

/*
*********************************************************************************************************
*	�� �� ��: lua_Init
*	����˵��: ��ʼ��lua�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void lua_Init(void)
{    
    g_Lua = luaL_newstate(); 		/* ����Lua���л��� */
    luaL_openlibs(g_Lua);
    luaopen_base(g_Lua);
	
	lua_RegisterFunc();		/* ע��c��������lua���� */
}

/* �ر��ͷ�Lua */
void lua_DeInit(void)
{	
    lua_close(g_Lua);				/* �ͷ��ڴ� */
	g_Lua = 0;
}

void lua_DownLoad(uint32_t _addr, uint8_t *_buf, uint32_t _len, uint32_t _total_len)
{
	uint32_t i;
	
	for (i = 0; i < _len; i++)
	{
		if (_addr < LUA_PROG_LEN_MAX)
		{
			s_lua_prog_buf[_addr + i] = _buf[i];
		}
	}
	
	s_lua_prog_len = _total_len;
	s_lua_prog_buf[s_lua_prog_len] = 0;
	
	if (g_Lua > 0)
	{
		lua_DeInit();
	}
	lua_Init();
	
	//luaL_dostring(g_Lua, s_lua_prog_buf);
}

void lua_Poll(void)
{
	if (g_tVar.LuaRunOnce == 1)
	{
		g_tVar.LuaRunOnce = 0;
		luaL_dostring(g_Lua, s_lua_prog_buf);
	}
}

void lua_DoInit(void)
{
	luaL_dostring(g_Lua, "init()");
}

// ͨ��д�ļ�
uint8_t lua_66H_Write(uint32_t _addr, uint8_t *_buf, uint32_t _len)
{
    uint8_t re;
	
	lua_getglobal(g_Lua, "write");    // ������ջ ��ȡlua����write
	lua_pushinteger(g_Lua, _addr); 	
	lua_pushlstring(g_Lua, (char *)_buf, _len); 
	lua_pushinteger(g_Lua, _len); 
	
	lua_pcall(g_Lua, 3, 1, 0);
	/*
		lua_pcall(lua_State *L,int nargs,int nresults,int errfunc)
		1
		nargs ��������
		nresults ����ֵ����
		errFunc ����������0��ʾ�ޣ���ʾ����������ջ�е�����
	*/
	re = lua_tonumber(g_Lua, -1);
	lua_pop(g_Lua, 1);
	return re;
}

// ͨ�Ŷ��ļ�
uint8_t lua_67H_Read(uint32_t _addr, uint8_t *_buf, uint32_t _len)
{
    uint8_t re = 0;
	uint32_t i;
	
	lua_getglobal(g_Lua, "read");    // ������ջ ��ȡlua����write
	lua_pushinteger(g_Lua, _addr); 	
	lua_pushinteger(g_Lua, _len); 
	
	lua_pcall(g_Lua, 2, 1, 0);
	/*
		lua_pcall(lua_State *L,int nargs,int nresults,int errfunc)
		1
		nargs ��������
		nresults ����ֵ����
		errFunc ����������0��ʾ�ޣ���ʾ����������ջ�е�����
	*/
	
	for (i = 0; i < _len; i++)
	{
		_buf[i] = s_lua_read_buf[i];
	}

	return re;
}

/*
*********************************************************************************************************
*	�� �� ��: beep
*	����˵��: �������� lua����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int beep(lua_State* L)
{
    //���ջ�еĲ����Ƿ�Ϸ���1��ʾLua����ʱ�ĵ�һ������(������)���������ơ�
    //���Lua�����ڵ���ʱ���ݵĲ�����Ϊnumber���ú�����������ֹ�����ִ�С�
    double op1 = luaL_checknumber(L, 1);
    double op2 = luaL_checknumber(L, 2);
	
	BEEP_KeyTone();
	
    //�������Ľ��ѹ��ջ�С�����ж������ֵ��������������ѹ��ջ�С�
    //lua_pushnumber(L, op1 + op2);
	
    //����ֵ������ʾ��C�����ķ���ֵ��������ѹ��ջ�еķ���ֵ������
    return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: delayus
*	����˵��: ΢���ӳ� lua����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int delayus(lua_State* L)
{
    uint32_t n = luaL_checknumber(L,1);

	bsp_DelayUS(n);
    return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: delayms
*	����˵��: �����ӳ� lua����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int delayms(lua_State* L)
{
    uint32_t n = luaL_checknumber(L,1);

	bsp_DelayMS(n);
    return 0;
}


/*
*********************************************************************************************************
*	�� �� ��: printhex
*	����˵��: ��ӡhex��ʽ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
extern uint8_t USBCom_SendBuf(int _Port, uint8_t *_Buf, uint16_t _Len);
extern void udp_SendBuf(uint8_t *_buf, uint16_t _len, uint16_t _port);
static int printhex(lua_State* L)
{
	if (lua_type(L, 1) == LUA_TSTRING) 	/* �жϵ�1������ */
	{		
		const char *data;
		size_t len;
		
		data = luaL_checklstring(L, 1, &len); /* 1�ǲ�����λ�ã� len��string�ĳ��� */
		#if 0
			udp_SendBuf((uint8_t *)data, len, 777);
		#else	
			USBCom_SendBuf(1, (uint8_t *)data, len);
		#endif		
	}

	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		char buf[32];
		uint32_t num;
		
		num = luaL_checknumber(L, 1);
		sprintf(buf, "%X\r\n", num);

		#if 0
			udp_SendBuf(buf, 1, 777);
		#else	
			USBCom_SendBuf(1, (uint8_t *)buf, strlen(buf));
		#endif
	}	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: write_clock
*	����˵��: дʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int write_clock(lua_State* L)
{
	uint16_t year;
	uint8_t mon, day, hour, min, sec;
	
	if (lua_type(L, 1) == LUA_TNUMBER) /* �жϵ�1������ */
	{
		year = luaL_checknumber(L, 1);
	}
	if (lua_type(L, 2) == LUA_TNUMBER)
	{
		mon = luaL_checknumber(L, 2);
	}
	if (lua_type(L, 3) == LUA_TNUMBER)
	{
		day = luaL_checknumber(L, 3);
	}
	if (lua_type(L, 4) == LUA_TNUMBER)
	{
		hour = luaL_checknumber(L, 4);
	}
	if (lua_type(L, 5) == LUA_TNUMBER)
	{
		min = luaL_checknumber(L, 5);
	}
	if (lua_type(L, 6) == LUA_TNUMBER)
	{
		sec = luaL_checknumber(L, 6);
	}	
	
	RTC_WriteClock(year, mon, day, hour, min, sec);
    return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: read_clock
*	����˵��: дʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int read_clock(lua_State* L)
{
	static uint8_t rtc_buf[8];
	
	RTC_ReadClock();

	rtc_buf[0] = g_tRTC.Year >> 8;
	rtc_buf[1] = g_tRTC.Year;
	rtc_buf[2] = g_tRTC.Mon;
	rtc_buf[3] = g_tRTC.Day;
	rtc_buf[4] = g_tRTC.Hour;
	rtc_buf[5] = g_tRTC.Min;
	rtc_buf[6] = g_tRTC.Sec;
	rtc_buf[7] = g_tRTC.Week;
	
	lua_pushlstring(L, (const char *)rtc_buf, 8); 
    return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_RegisterFunc
*	����˵��: ע��lua�ɵ��õ�c����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void lua_RegisterFunc(void)
{
	//��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
    lua_register(g_Lua, "beep", beep);
    lua_register(g_Lua, "delayus", delayus);
	lua_register(g_Lua, "delayms", delayms);
	lua_register(g_Lua, "printhex", printhex);
	lua_register(g_Lua, "write_clock", write_clock);
	lua_register(g_Lua, "read_clock", read_clock);
	
	
	/* ע��ӿں��� */
	lua_gpio_RegisterFun();	
	lua_i2c_RegisterFun();
	lua_spi_RegisterFun();
	lua_tim_RegisterFun();
	lua_tcp_RegisterFun();
	lua_qspi_RegisterFun();
	lua_fatfs_RegisterFun();
}


