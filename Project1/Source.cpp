#include <windows.h>
#include <iostream>
#include <string>
using namespace std; //использовать пространство имён std
//для
//#define cLibName L"C:\\Users\\max\\source\\repos\\lections_kos\\Debug\\Dll1.dll" //имя
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0) //если nCode, не продолжать обработку
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	if (nCode < 0) //если нажата клавиша
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
		MessageBoxA(NULL, LPCSTR(slnfo), "Keyboard hook info", MB_OK);
		//вывести собщение на экран
	}
	return TRUE; //заблокировать дальнейшую обработку
}

int main()
{
	setlocale(LC_ALL, "ru");
	int nMenu; 	//переменная меню
	static HINSTANCE hLib = NULL; //дескриптор библиотеки DLL
	static HHOOK hHook = NULL; 	//дескриптор хука
	static BOOL bHook = FALSE; // переменная установки хука
	HMODULE hInstance = GetModuleHandle(NULL);

	do {
		//system("cls"); //очистить экран
		do { //вывод меню на экран
			cout <<
				endl << "l. Set Hook" <<
				endl << "2. Unset Hook" <<
				endl << "0. Exit" <<
				endl << endl << ">";
			cin >> nMenu; //выбор пункта меню
		} while ((nMenu < 0) || (nMenu > 2));
		switch (nMenu)
		{
		case 1: //установка хука
			if (!bHook) //если хук не установлен
			{
				hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInstance, NULL);
				//if (!hHook) //проверка
				{
					MessageBoxA(GetActiveWindow(), "Хук не установлен!", "Ошибка", MB_OK | MB_ICONEXCLAMATION);
				//	return 3;
				}
				//else bHook = TRUE;
			}
			break;
		case 2: //снятие хука
			if (bHook) //если хук установлен
			{
				if (UnhookWindowsHookEx(hHook)) //снять хук
				{
					bHook = false;
					if (hLib) //освободить библиотеку
						if (!FreeLibrary(hLib))
						{ //проверка
							MessageBoxA(GetActiveWindow(), "Библиотека не освобождена!", "Ошибка", MB_OK |
								MB_ICONEXCLAMATION);
							return 4;
						}
				}
				else
				{
					MessageBoxA(GetActiveWindow(), "Хук не снят!", "Ошибка", MB_OK | MB_ICONEXCLAMATION);
					return 5;
				}
			}
				break;
		}//switch
	} while (nMenu);
	
	return 0;
}