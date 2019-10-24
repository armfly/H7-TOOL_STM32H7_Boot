#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio_dma.h"


FATFS   g_lua_fs;
FIL 	g_lua_file1;

char luaDiskPath[4]; /* ����FatFS ����·�� */

static int lua_f_mount(lua_State* L);
static int lua_f_dir(lua_State* L);
void ViewDir(char *_path);

void lua_fatfs_RegisterFun(void)
{
	//��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
    lua_register(g_Lua, "f_init", lua_f_mount);	
	lua_register(g_Lua, "f_dir", lua_f_dir);
}

/*
*********************************************************************************************************
*	�� �� ��: lua_udp_SendBytes
*	����˵��: ��UDP����һ�����ݡ�UDPĿ��IP�����һ�ν��յ���UDP������ȡ��Ҳ����ֻ�������һ��ͨ�ŵ�����
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_f_mount(lua_State* L)
{
	FATFS_LinkDriver(&SD_Driver, luaDiskPath);
	
	/* �����ļ�ϵͳ */
	if (f_mount(&g_lua_fs, luaDiskPath, 0) != FR_OK)
	{
		printf("f_mount�ļ�ϵͳʧ��");
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: lua_f_dir
*	����˵��: 
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int lua_f_dir(lua_State* L)
{
	const char *data;
	uint32_t len;
	
	if (lua_type(L, 1) == LUA_TSTRING) 	/* �жϵ�1������ */
	{		
		data = luaL_checklstring(L, 1, &len); /* 1�ǲ�����λ�ã� len��string�ĳ��� */		
	}
	
	ViewDir((char *)data);
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: ViewDir
*	����˵��: ��ʾ��Ŀ¼�µ��ļ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ViewDir(char *_path)
{
	/* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */
	FRESULT result;
	DIR DirInf;
	FILINFO FileInf;
	uint32_t cnt = 0;

	/* �򿪸��ļ��� - �������Ҫ f_closedir  */
	result = f_opendir(&DirInf, _path); /* 1: ��ʾ�̷� */
	if (result != FR_OK)
	{
		printf("�򿪸�Ŀ¼ʧ�� (%d)\r\n", result);
		return;
	}

	printf("\r\n��ǰĿ¼��%s\r\n", _path);

	/* ��ȡ��ǰ�ļ����µ��ļ���Ŀ¼ */
	printf("����        |  �ļ���С | ���ļ��� | ���ļ���\r\n");
	for (cnt = 0; ;cnt++)
	{
		result = f_readdir(&DirInf,&FileInf); 		/* ��ȡĿ¼��������Զ����� */
		if (result != FR_OK || FileInf.fname[0] == 0)
		{
			break;
		}

		if (FileInf.fname[0] == '.')
		{
			continue;
		}

		/* �ж����ļ�������Ŀ¼ */
		if (FileInf.fattrib & AM_DIR)
		{
			printf("(0x%02d)Ŀ¼  ", FileInf.fattrib);
		}
		else
		{
			printf("(0x%02d)�ļ�  ", FileInf.fattrib);
		}

		/* ��ӡ�ļ���С, ���4G */
		printf(" %10d", FileInf.fsize);

		printf("  %s |", FileInf.altname);	/* ���ļ��� */

		printf("  %s\r\n", (char *)FileInf.fname);	/* ���ļ��� */
	}
	
	f_closedir(&DirInf);	/*���رմ򿪵�Ŀ¼ */
}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
