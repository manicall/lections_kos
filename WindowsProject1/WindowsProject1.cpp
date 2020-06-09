#pragma warning(disable : 4996)
#include "framework.h"
#include <windowsx.h>
#include "resource.h"
#include <string>
#include <chrono>

using namespace std;
HWND hWndDialog;
HINSTANCE       ghInstance;
wstring pEventTypeNames[] = { L"Error", L"Warning", L"Informational", L"Audit Success", L"Audit Failure" };
int wmId, wmEvent;
HWND cb, lb;

class UserTimer {
	std::chrono::time_point<std::chrono::steady_clock> start, end;
public:
	UserTimer() {
		start = std::chrono::high_resolution_clock::now();
	}
	~UserTimer() {
		end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> duration = end - start;
		ListBox_AddString(lb, (L"Duration: " + to_wstring(duration.count()) + L" s").data());
	}
};

void output_records(const wstring& path);

DWORD GetEventTypeName(DWORD EventType)
{
	DWORD index = 0;

	switch (EventType)
	{
	case EVENTLOG_ERROR_TYPE:
		index = 0;
		break;
	case EVENTLOG_WARNING_TYPE:
		index = 1;
		break;
	case EVENTLOG_INFORMATION_TYPE:
		index = 2;
		break;
	case EVENTLOG_AUDIT_SUCCESS:
		index = 3;
		break;
	case EVENTLOG_AUDIT_FAILURE:
		index = 4;
		break;
	}

	return index;
}

void print_one_record(BYTE* rec) {

	EVENTLOGRECORD* pRecord = (EVENTLOGRECORD*)rec;
	std::wstring source_name((wchar_t*)(rec + sizeof(EVENTLOGRECORD)));
	int count = 0;
	ListBox_AddString(lb, (L"EventType: " + pEventTypeNames[GetEventTypeName(pRecord->EventType)]).data());
	ListBox_AddString(lb, (L"Provider: " + source_name).data());
	ListBox_AddString(lb, (L"Length: " + to_wstring(pRecord->Length)).data());
	ListBox_AddString(lb, (L"RecordNumber: " + to_wstring(pRecord->RecordNumber)).data());
	ListBox_AddString(lb, (L"--Strings-- [ " + to_wstring(pRecord->Length) + L" ]").data());
	

	for (wstring wstr = (wchar_t*)(rec + pRecord->StringOffset); count < pRecord->NumStrings; ++count) {
		ListBox_AddString(lb, (L"  -> " + wstr).data());
		wstr += wstr.length() + 1;
	}

	ListBox_AddString(lb, L"");
}

void print_records(BYTE* rec, DWORD rec_size) {
	PBYTE pRecord = rec;
	PBYTE pEndOfRecords = rec + rec_size;
	while (pRecord < pEndOfRecords)
	{
		print_one_record(pRecord);
		pRecord += ((PEVENTLOGRECORD)pRecord)->Length;
	}
}
                 

BOOL CALLBACK   PviewDlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);

int WINAPI    WinMain(HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow)
{
	MSG     msg;
	ghInstance = hInstance;

	hWndDialog = CreateDialogParam(hInstance,
		MAKEINTRESOURCE(IDD_DIALOG1),
		NULL,
		(DLGPROC)PviewDlgProc,
		(LONG)0);

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

BOOL CALLBACK   PviewDlgProc(HWND    hWnd,
	UINT    wMsg,
	WPARAM  wParam,
	LPARAM  lParam)
{
	switch (wMsg)
	{
	case WM_INITDIALOG:
	{
		
		cb = GetDlgItem(hWnd, IDC_COMBO1);
		lb = GetDlgItem(hWnd, IDC_LIST1);

		ComboBox_AddString(cb, L"C:\\Windows\\System32\\winevt\\Logs\\Application.evtx");
		ComboBox_AddString(cb, L"C:\\Windows\\System32\\winevt\\Logs\\Security.evtx");
		ComboBox_AddString(cb, L"C:\\Windows\\System32\\winevt\\Logs\\Setup.evtx");
		ComboBox_AddString(cb, L"C:\\Windows\\System32\\winevt\\Logs\\System.evtx");
		
	}			
	break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_COMBO1:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE: 
				ListBox_ResetContent(lb);
				switch (SendMessage(cb, CB_GETCURSEL, 0, 0))
				{
				case 0: 			
				{
					output_records(L"C:\\Windows\\System32\\winevt\\Logs\\Application.evtx");
					break;
				}
				case 1:
				{
					output_records(L"C:\\Windows\\System32\\winevt\\Logs\\Security.evtx");
					break;
				}
				case 2:
				{
					output_records(L"C:\\Windows\\System32\\winevt\\Logs\\Setup.evtx");
					break;
				}
				case 3:
				{
					output_records(L"C:\\Windows\\System32\\winevt\\Logs\\System.evtx");
					break;
				}		
				}
				break;
			default:
				break;
			}
		default:
			break;
		}
	default:
		return FALSE;
	}
	return TRUE;
}

void output_records(const wstring& path) {
	UserTimer t;
	const unsigned MAX_BUFF_SIZE = 0x10000;

	DWORD NumberOfRecords = 0;
	BYTE* pBuffer = new BYTE[MAX_BUFF_SIZE];
	DWORD dwBytesToRead = MAX_BUFF_SIZE;
	DWORD status = ERROR_SUCCESS;
	DWORD dwBytesRead = 0;
	DWORD dwMinimumBytesToRead = 0;

	HANDLE hEventLog = OpenEventLog(NULL, path.data());
	ListBox_AddString(lb, path.data());
	if (!hEventLog)
	{
		ListBox_AddString(lb, (L"OpenEventLogA [ " + to_wstring(GetLastError()) + L" ]\n").data());
	}
	if (!GetNumberOfEventLogRecords(hEventLog, &NumberOfRecords))
	{
		ListBox_AddString(lb, (L"GetNumberOfEventLogRecords [ " + to_wstring(GetLastError()) + L" ]\n").data());
	}

	ListBox_AddString(lb, (L"NumberOfRecords [ " + to_wstring(NumberOfRecords) + L" ]\n").data());
	int count = 0;
	while (ERROR_SUCCESS == status)
	{
		if (!ReadEventLog(hEventLog,
			EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ,
			0,
			pBuffer,
			dwBytesToRead,
			&dwBytesRead,
			&dwMinimumBytesToRead))
		{
			status = GetLastError();
			if (ERROR_INSUFFICIENT_BUFFER == status)
			{
				status = ERROR_SUCCESS;

				BYTE* pTemp = new BYTE[dwMinimumBytesToRead];
				delete[] pBuffer;

				pBuffer = pTemp;
				dwBytesToRead = dwMinimumBytesToRead;
			}
			else
			{
				if (ERROR_HANDLE_EOF != status)
				{
					if (hEventLog)
						CloseEventLog(hEventLog);
					delete[] pBuffer;
					return ;
					system("pause");
				}
			}
		}
		else
		{
			print_records(pBuffer, dwBytesRead);
			if (++count > 5) break;
		}
	}
	if (hEventLog)
		CloseEventLog(hEventLog);
	delete[] pBuffer;
}