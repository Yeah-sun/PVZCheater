#include "UI.h"
#include <tlhelp32.h>
void WriteMemory(void* value, DWORD valueSize, ...)
{
	if (value == NULL || valueSize == 0 || YEAH::UI_2::g_processHandle == NULL) return;

	DWORD tempValue = 0;

	va_list addresses; //
	va_start(addresses, valueSize);
	DWORD offset = 0;
	DWORD lastAddress = 0;
	while ((offset = va_arg(addresses, DWORD)) != -1)
	{
		lastAddress = tempValue + offset;
		::ReadProcessMemory(YEAH::UI_2::g_processHandle, (LPCVOID)lastAddress, &tempValue, sizeof(DWORD), NULL);
	}
	va_end(addresses);
	::WriteProcessMemory(YEAH::UI_2::g_processHandle, (LPVOID)lastAddress, value, valueSize, NULL);
}
void WriteMemory(void* value, DWORD valueSize, DWORD address) {
	WriteMemory(value, valueSize, address, -1);
}


void function()
{
	/*-----------------------------------------------------------------------------*/
	static bool flag = false;
	if (YEAH::UI_2::初始化 && !flag)//
	{
		HWND hwnd = ::FindWindow(NULL, TEXT("植物大战僵尸中文版"));
		if (hwnd != NULL)
		{
			GetWindowThreadProcessId(hwnd, &YEAH::UI_2::PID);// 获取进程 ID
			if (YEAH::UI_2::PID != 0) // 打开进程
			{
				if (YEAH::UI_2::g_processHandle != NULL) { CloseHandle(YEAH::UI_2::g_processHandle); }
				YEAH::UI_2::g_processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, YEAH::UI_2::PID);
				if (YEAH::UI_2::g_processHandle == NULL) { DWORD errorCode = GetLastError(); }
			}
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, YEAH::UI_2::PID);
			if (hSnapshot == INVALID_HANDLE_VALUE)  return ;
			MODULEENTRY32 me32;
			me32.dwSize = sizeof(me32);
			if (Module32First(hSnapshot, &me32)) {
				YEAH::UI_2::address = (DWORD)reinterpret_cast<uintptr_t>(me32.modBaseAddr);
			}
			CloseHandle(hSnapshot);
		}
		YEAH::UI_2::程序初始进度 = 98;
		flag = true;
	}
	/*-----------------------------------------------------------------------------*/
	if (YEAH::UI_1::植物不死)//
	{
		// 如果需要植物不死
		//BYTE data1[] = { 0x90, 0x90, 0x90, 0x90 };
		//WriteMemory(data1, sizeof(data1), address1);
	}
	else
	{
		//BYTE data1[] = { 0x83, 0x46, 0x40, 0xFC };
		//WriteMemory(data1, sizeof(data1), address1);
	}
	/*-----------------------------------------------------------------------------*/
	if (YEAH::UI_1::后台运行)//
	{
		BYTE data[] = { 0xC3 };
		WriteMemory(data, sizeof(data), YEAH::UI_2::addressWIndow);
	}
	else
	{
		BYTE data[] = { 0x55 };
		WriteMemory(data, sizeof(data), YEAH::UI_2::addressWIndow);
	}

	/*-----------------------------------------------------------------------------*/
	if (YEAH::UI_1::僵尸不死)//
	{
		BYTE data1[] = { 0xC7,0x85,0xC8,0x00,0x00,0x00,     0x01,0x00,0x00,0x00,0x90 };
		WriteMemory(data1, sizeof(data1), YEAH::UI_2::addressZoom_1);

	}
	else if (YEAH::UI_1::秒杀僵尸)//
	{
		BYTE data1[] = { 0xC7,0x85,0xC8,0x00,0x00,0x00,     0x00,0x00,0x00,0x00,0x90 };
		WriteMemory(data1, sizeof(data1), YEAH::UI_2::addressZoom_1);
	}
	else
	{
		BYTE data1[] = { 0x89,0xBD,0xC8,0x00,0x00,0x00,     0xE8,0xEC,0xC3,0xFF,0xFF };
		WriteMemory(data1, sizeof(data1), YEAH::UI_2::addressZoom_1);
	}
	/*-----------------------------------------------------------------------------*/
	if (YEAH::UI_1::植物无CD)//
	{

		BYTE data[] = { 0x01 };
		WriteMemory(data, sizeof(data), YEAH::UI_2::addressCD);
	}
	else
	{
		BYTE data[] = { 0x00 };
		WriteMemory(data, sizeof(data), YEAH::UI_2::addressCD);
	}
	/*-----------------------------------------------------------------------------*/
	if (YEAH::UI_1::无限阳光)//
	{
		DWORD value = 9999;
		WriteMemory(&value, sizeof(value), YEAH::UI_2::address + 0x2A9EC0, 0x768, 0x5560, -1);
	}
	/*-----------------------------------------------------------------------------*/
}
