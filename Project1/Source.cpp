#include <windows.h>
#include <iostream>
#include <string>
using namespace std; //������������ ������������ ��� std
//���
//#define cLibName L"C:\\Users\\max\\source\\repos\\lections_kos\\Debug\\Dll1.dll" //���
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0) //���� nCode, �� ���������� ���������
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	if (nCode < 0) //���� ������ �������
	{
		char slnfo[50];
		//������������� ����-��� � ������
		UINT uMap = MapVirtualKeyW((UINT)wParam, 2);
		if (uMap)
			//���� �������, ������� �������� wParam ��� ������
			sprintf_s(slnfo, 50, "nCode = %d, wParam: %c, lParam: %d\0", nCode, uMap, lParam);
		else
			//�����, ������� �������� wParam ��� �����
			sprintf_s(slnfo, 50, "nCode = %d, wParam: %d, lParam: %d\0", nCode, wParam, lParam);
		MessageBoxA(NULL, LPCSTR(slnfo), "Keyboard hook info", MB_OK);
		//������� �������� �� �����
	}
	return TRUE; //������������� ���������� ���������
}

int main()
{
	setlocale(LC_ALL, "ru");
	int nMenu; 	//���������� ����
	static HINSTANCE hLib = NULL; //���������� ���������� DLL
	static HHOOK hHook = NULL; 	//���������� ����
	static BOOL bHook = FALSE; // ���������� ��������� ����
	HMODULE hInstance = GetModuleHandle(NULL);

	do {
		//system("cls"); //�������� �����
		do { //����� ���� �� �����
			cout <<
				endl << "l. Set Hook" <<
				endl << "2. Unset Hook" <<
				endl << "0. Exit" <<
				endl << endl << ">";
			cin >> nMenu; //����� ������ ����
		} while ((nMenu < 0) || (nMenu > 2));
		switch (nMenu)
		{
		case 1: //��������� ����
			if (!bHook) //���� ��� �� ����������
			{
				hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInstance, NULL);
				//if (!hHook) //��������
				{
					MessageBoxA(GetActiveWindow(), "��� �� ����������!", "������", MB_OK | MB_ICONEXCLAMATION);
				//	return 3;
				}
				//else bHook = TRUE;
			}
			break;
		case 2: //������ ����
			if (bHook) //���� ��� ����������
			{
				if (UnhookWindowsHookEx(hHook)) //����� ���
				{
					bHook = false;
					if (hLib) //���������� ����������
						if (!FreeLibrary(hLib))
						{ //��������
							MessageBoxA(GetActiveWindow(), "���������� �� �����������!", "������", MB_OK |
								MB_ICONEXCLAMATION);
							return 4;
						}
				}
				else
				{
					MessageBoxA(GetActiveWindow(), "��� �� ����!", "������", MB_OK | MB_ICONEXCLAMATION);
					return 5;
				}
			}
				break;
		}//switch
	} while (nMenu);
	
	return 0;
}