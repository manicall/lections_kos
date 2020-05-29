#include "framework.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"

HWND hWndDialog;
HINSTANCE       ghInstance;   // Переменная для хранения хендела процесса                      
// Описание используемой оконной процедуры
BOOL CALLBACK   PviewDlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
// Главное приложение программы
int WINAPI    WinMain(HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow)
{
	MSG     msg;
	ghInstance = hInstance;
	// Создание  диалогового окна
	hWndDialog = CreateDialogParam(hInstance,
		MAKEINTRESOURCE(IDD_DIALOG1),
		NULL,
		(DLGPROC)PviewDlgProc,
		(LONG)0);

	// Стандартный цикл обработки сообщений приложения
	ShowWindow(hWndDialog, nCmdShow);
	UpdateWindow(hWndDialog);
	while (GetMessage(&msg, NULL, 0, 0))
		if (!IsDialogMessage(hWndDialog, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	DestroyWindow(hWndDialog);
	return 0;
}

RECT rect;
HDC hdc, hdcm;
int wmId, wmEvent;
PAINTSTRUCT ps; // структура для перерисовки окна
BOOL CALLBACK   PviewDlgProc(HWND    hWnd,
	UINT    wMsg,
	WPARAM  wParam,
	LPARAM  lParam)
{
	switch (wMsg)
	{
		// сообщение об инициализации диалоговой формы
	case WM_INITDIALOG:
	{
		DWORD dwCurentRecord = 0;
		DWORD NumberOfRecords = 0;
		LPVOID lpBuffer;
		DWORD dwRead = 0;
		DWORD dwNeed = 0;
		DWORD dwSize = sizeof(EVENTLOGRECORD);
		DWORD dwPos = 0;
		system("chcp 1251");
		HANDLE hEventLog = OpenBackupEventLog(NULL, L"C:\\Windows\\System32\\winevt\\Logs\\Application.evtx");
		if (!hEventLog)
			MessageBox(0, L"OpenEventLog", NULL, MB_OK | MB_ICONERROR);
		else if (!GetNumberOfEventLogRecords(hEventLog, &NumberOfRecords))
			MessageBox(0, L"GetNumberOfEventLogRecords", NULL, MB_OK | MB_ICONERROR);
		else
		{
			//Память под массив структур
			lpBuffer = new LPVOID[NumberOfRecords * dwSize * 8];
			if
				(
					!ReadEventLog
					(
						hEventLog,
						NULL,
						0,
						lpBuffer,
						NumberOfRecords * dwSize,
						&dwRead,
						&dwNeed
					)
					)
			{
				MessageBox(0, L"ReadEventLog", NULL, MB_OK | MB_ICONERROR);
				PostQuitMessage(0);;
			}

		}
	}			
	break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;


	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		
		default:
			return FALSE;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}
