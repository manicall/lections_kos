// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include <windows.h>
#include <stdio.h>

extern "C" __declspec(dllexport)
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0) //если nCode, не продолжать обработку
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    if (lParam & 0x80000000) //если нажата клавиша
    {
        char slnfo[50];
        //преобразовать скан-код в символ
        UINT uMap = MapVirtualKeyW((UINT)wParam, 2);
        if (uMap)
            //если удалось, вывести параметр wParam как символ
            sprintf_s(slnfo, 50, "nCode = %d, wParam: %c, lParam: %d\0", nCode, uMap, lParam);
        else
            //иначе, вывести параметр wParam как число
            sprintf_s(slnfo, 50, "nCode = %d, wParam: %d, lParam: %d\0", nCode, wParam, lParam);
        //вывести собщение на экран
        MessageBoxA(NULL, LPCSTR(slnfo), "Keyboard hook info", MB_OK);
    }
    return TRUE; //заблокировать дальнейшую обработку
}

int WINAPI DllMain(HINSTANCE hlnstance, DWORD fdReason, PVOID pvReserved)
{
    return TRUE;
}
