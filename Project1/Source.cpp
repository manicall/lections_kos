#include <windows.h>
#include <iostream>
using namespace std; //������������ ������������ ��� std
//���
#define cLibName L"C:\\Users\\max\\source\\repos\\lections_kos\\Debug\\Dll1" //���
int main()
{
	int nMenu; 	//���������� ����
	static HINSTANCE hLib = NULL; //���������� ���������� DLL
	static HHOOK hHook = NULL; 	//���������� ����
	static BOOL bHook = FALSE; // ���������� ��������� ����

	do {
		system("cls"); //�������� �����
		do { //����� ���� �� �����
			cout << " Menu" <<
				endl <<
				endl << "l. Set Hook" <<
				endl << "2. Unset Hook" <<
				endl <<
				endl << "0. Exit" <<
				endl << endl << "Vash vybor? ";
			cin >> nMenu; //����� ������ ����
		} while ((nMenu < 0) || (nMenu > 2));
		switch (nMenu)
		{
		case 1: //��������� ����
			if (!bHook) //���� ��� �� ����������
			{
				HOOKPROC hProc = NULL; //���������� ����������� �������
				//��������� ���������� DLL
				hLib = LoadLibrary(cLibName);
				if (!hLib) //��������
				{
					MessageBoxA(GetActiveWindow(), "���������� �� ���������!", "������", MB_OK | MB_ICONEXCLAMATION);
					return 1;
				}
				//�������� ����� ����������� �������
				hProc = (HOOKPROC)GetProcAddress(hLib, "HookProc");
				if (!hProc)
				{
					MessageBoxA(GetActiveWindow(), "������� �� �������!", "������", MB_OK | MB_ICONEXCLAMATION);
					return 2;
				}
				cout << "��� ����" << endl;
				//���������� ���������� ���
				hHook = SetWindowsHookEx(WH_KEYBOARD, hProc, hLib, NULL);
				if (!hHook) //��������
				{
					MessageBoxA(GetActiveWindow(), "��� �� ����������!", "������", MB_OK | MB_ICONEXCLAMATION);
					return 3;
				}
				else bHook = TRUE;
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
							MessageBoxA(GetActiveWindow(),
								"���������� �� �����������!", "������", MB_OK |
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