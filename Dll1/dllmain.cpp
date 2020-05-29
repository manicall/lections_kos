// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include <windows.h>
#include <stdio.h>
#include <string>
using namespace std; //использовать пространство имён std
int WINAPI DllMain(HINSTANCE hlnstance, DWORD fdReason, PVOID pvReserved)
{
    return TRUE;
}
extern "C" __declspec(dllexport)
