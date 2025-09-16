#pragma once  
#include "../PVZCheater.h"
void Menu();
namespace YEAH
{
	inline bool ini_open = true;
	inline float window_width = 500;
	inline float window_height = 600;
	namespace UI_1
	{
		inline bool 无限阳光 = false;

		inline bool 秒杀僵尸 = false;

		inline bool 植物无CD = false;

		inline bool 植物不死 = false;

		inline bool 僵尸不死 = false;

		inline bool 后台运行 = false;


	}
	namespace UI_2
	{
		inline bool 初始化 = false;
		inline float 程序初始进度 = 0.0f;
		inline DWORD PID = 0;
		inline HANDLE g_processHandle = NULL;// 植物大战僵尸的进程句柄
		inline DWORD address = 0x00; // 植物大战僵尸的主模块地址
		inline DWORD sunny_value = 0; 


		inline DWORD addressCD = 0x00488E76;
		inline DWORD addressZoom_1 = 0x00531319;
		inline DWORD addressWIndow = 0x004502C0;


	}
}
void function();



