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
		inline bool �������� = false;

		inline bool ��ɱ��ʬ = false;

		inline bool ֲ����CD = false;

		inline bool ֲ�ﲻ�� = false;

		inline bool ��ʬ���� = false;

		inline bool ��̨���� = false;


	}
	namespace UI_2
	{
		inline bool ��ʼ�� = false;
		inline float �����ʼ���� = 0.0f;
		inline DWORD PID = 0;
		inline HANDLE g_processHandle = NULL;// ֲ���ս��ʬ�Ľ��̾��
		inline DWORD address = 0x00; // ֲ���ս��ʬ����ģ���ַ
		inline DWORD sunny_value = 0; 


		inline DWORD addressCD = 0x00488E76;
		inline DWORD addressZoom_1 = 0x00531319;
		inline DWORD addressWIndow = 0x004502C0;


	}
}
void function();



