#include <windows.h>
#include <iostream>
using namespace std; //использовать пространство имён std
//для
#define cLibName L"C:\\Users\\max\\source\\repos\\lections_kos\\Debug\\Dll1" //имя
int main()
{
	int nMenu; 	//переменная меню
	static HINSTANCE hLib = NULL; //дескриптор библиотеки DLL
	static HHOOK hHook = NULL; 	//дескриптор хука
	static BOOL bHook = FALSE; // переменная установки хука

	do {
		system("cls"); //очистить экран
		do { //вывод меню на экран
			cout << " Menu" <<
				endl <<
				endl << "l. Set Hook" <<
				endl << "2. Unset Hook" <<
				endl <<
				endl << "0. Exit" <<
				endl << endl << "Vash vybor? ";
			cin >> nMenu; //выбор пункта меню
		} while ((nMenu < 0) || (nMenu > 2));
		switch (nMenu)
		{
		case 1: //установка хука
			if (!bHook) //если хук не установлен
			{
				HOOKPROC hProc = NULL; //дескриптор фильтрующей функции
				//загрузить библиотеку DLL
				hLib = LoadLibrary(cLibName);
				if (!hLib) //проверка
				{
					MessageBoxA(GetActiveWindow(), "Библиотека не загружена!", "Ошибка", MB_OK | MB_ICONEXCLAMATION);
					return 1;
				}
				//Получить адрес фильтрующей функции
				hProc = (HOOKPROC)GetProcAddress(hLib, "HookProc");
				if (!hProc)
				{
					MessageBoxA(GetActiveWindow(), "Функция не найдена!", "Ошибка", MB_OK | MB_ICONEXCLAMATION);
					return 2;
				}
				cout << "ВСЕ НОРМ" << endl;
				//установить глобальный хук
				hHook = SetWindowsHookEx(WH_KEYBOARD, hProc, hLib, NULL);
				if (!hHook) //проверка
				{
					MessageBoxA(GetActiveWindow(), "Хук не установлен!", "Ошибка", MB_OK | MB_ICONEXCLAMATION);
					return 3;
				}
				else bHook = TRUE;
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
							MessageBoxA(GetActiveWindow(),
								"Библиотека не освобождена!", "Ошибка", MB_OK |
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