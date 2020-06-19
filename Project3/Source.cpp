//#define DEBUG

#include <Windows.h>
#include <iostream>
using namespace std;
#ifndef DEBUG



void MyErrorExit(const char* msg) {
    cout << msg << endl;
    cout << "��� ������: " << GetLastError() << endl;
    system("pause");
    ExitProcess(1);
}

VOID CreateSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService);
DWORD StartSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService);

int main() {
    setlocale(LC_ALL, "ru");
    SC_HANDLE schSCManager, schService;
    schSCManager = OpenSCManager(
        NULL,                    // ��������� ������
        NULL,                    // ��� ���� (ServicesActive)
        SC_MANAGER_ALL_ACCESS);  // ������ ������
    if (schSCManager == NULL) MyErrorExit("OpenSCManager");
    CreateSampleService(schSCManager, schService);
    StartSampleService(schSCManager, schService);
    system("pause");
}

VOID CreateSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService)
{
    wchar_t ServiceName[] = L"���������������� ������";
    LPCTSTR lpszBinaryPathName = L"C:\\Users\\max\\Desktop\\LD_service.exe";
    LPCTSTR lpszDisplayName;
    schService = CreateService(
        schSCManager,              // ���� ������� SCManager
        L"Sample_Srv",             // ��� �������
        ServiceName,               // ���, ������� ����� ������������
        SERVICE_ALL_ACCESS,        // ������� �������
        SERVICE_WIN32_OWN_PROCESS, // ������ ����� �����������
                                   // � ����������� ��������
        SERVICE_DEMAND_START,      // ����� ����������� "�������"
        SERVICE_ERROR_NORMAL,      // ����������� �����
                                   // ���������������� ������
        lpszBinaryPathName,        // ���� � �������
        NULL,                      // ������ �� ����������� ������
        NULL,                      // --""--
        NULL,                      // ��� �����������
        NULL,                      // � ������� �������
        NULL);                     // ��� ������

    if (schService == NULL) MyErrorExit("CreateService");
    else printf("CreateService SUCCESS.\n");
    CloseServiceHandle(schService);
}
DWORD StartSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService)
{
    SERVICE_STATUS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwStatus;

    schService = OpenService(
        schSCManager,           // ���� �������� SCM
        L"Sample_Srv",          // ��� �������
        SERVICE_ALL_ACCESS);

    if (schService == NULL) MyErrorExit("OpenService");
    if (!StartService(
        schService,  // ���������� �������
        0,           // ���������� ����������
        NULL))       // ��� ����������
    {
        MyErrorExit("StartService");
    }
    else
    {
        printf("Service start pending.\n");
    }
    // ��������� ��������� ������� �� ��� ���, ���� �� �� ����������.
    if (!QueryServiceStatus(
        schService,   // ���������� �������
        &ssStatus))  // ����� ��������� � ����������� � �������
    {
        MyErrorExit("QueryServiceStatus");
    }

    // ��������� ������� ����� � ��������� �����.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        // �� ����� ������� ������, ��� ��������������� �����,
        // ����������� ��� ������. ������ ������� ����� ����� ����
        // ������� ����� ��������, �� �� ����� ���� �������
        // � ����� �� ����� 10 ������.

        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // ����� ��������� ������.

        if (!QueryServiceStatus(
            schService,   // ���������� �������
            &ssStatus))  // ����� ���������
            break;

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // ������ � �������� ������.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                // ����� �� ������ �� ����������
                break;
            }
        }
    }
    if (ssStatus.dwCurrentState == SERVICE_RUNNING)
    {
        printf("StartService SUCCESS.\n");
        dwStatus = NO_ERROR;
    }
    else
    {
        printf("\n������ �� �������. \n");
        printf("  Current State: %d\n", ssStatus.dwCurrentState);
        printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
        printf("  Service Specific Exit Code: %d\n",
            ssStatus.dwServiceSpecificExitCode);
        printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
        printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
        dwStatus = GetLastError();
    }
    CloseServiceHandle(schService);
    return dwStatus;
}
#endif 

#ifdef DEBUG
#include <Windows.h>
#include <iostream>
using namespace std;

wchar_t ServiceName[] = L"longstartsvc";
enum { CREATESERVICE, DELETESERVICE, STARTSERVICE };
int getAnswer() {
    cout << "===========MENU===========" << endl;
    cout << "1) ������� ������" << endl;
    cout << "2) ������� ������" << endl;
    cout << "3) ��������� ������" << endl;
    int a; cin >> a;
    return a - 1;
}
void MyErrorExit(const char* msg) {
    cout << msg << endl;
    cout << GetLastError() << endl;
    system("pause");
    ExitProcess(1);
}

VOID CreateSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService);
VOID DeleteSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService);
DWORD StartSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService);

int main() {
    setlocale(LC_ALL, "ru");
    SC_HANDLE schSCManager, schService;
    schSCManager = OpenSCManager(
        NULL,                    // ��������� ������
        NULL,                    // ��� ���� (ServicesActive)
        SC_MANAGER_ALL_ACCESS);  // ������ ������
    while (true) {
        switch (getAnswer()) {
        case CREATESERVICE: CreateSampleService(schSCManager, schService); break;
        case DELETESERVICE: DeleteSampleService(schSCManager, schService); break;
        case STARTSERVICE: StartSampleService(schSCManager, schService);  break;
        default: cout << "����������� �������, ��������� ����� ���������. "; system("pause"); return 2;
        }
    }
    system("pause");
}

VOID CreateSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService)
{
    

    if (schSCManager == NULL)
        MyErrorExit("OpenSCManager");

    LPCTSTR lpszBinaryPathName = L"C:\\Users\\max\\Desktop\\LD_service.exe";
    LPCTSTR lpszDisplayName;
    schService = CreateService(
        schSCManager,              // ���� ������� SCManager
        L"Sample_Srv",             // ��� �������
        ServiceName,               // ���, ������� ����� ������������
        SERVICE_ALL_ACCESS,        // ������� �������
        SERVICE_WIN32_OWN_PROCESS, // ������ ����� �����������
                                   // � ����������� ��������
        SERVICE_DEMAND_START,      // ����� ����������� "�������"
        SERVICE_ERROR_NORMAL,      // ����������� �����
                                   // ���������������� ������
        lpszBinaryPathName,        // ���� � �������
        NULL,                      // ������ �� ����������� ������
        NULL,                      // --""--
        NULL,                      // ��� �����������
        NULL,                      // � ������� �������
        NULL);                     // ��� ������

    if (schService == NULL)
        MyErrorExit("CreateService");
    else
        printf("CreateService SUCCESS.\n");

    CloseServiceHandle(schService);
}

VOID DeleteSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService)
{
    schSCManager = OpenSCManager(
        NULL,                    // ��������� ������
        NULL,                    // ��� ���� (ServicesActive)
        SC_MANAGER_ALL_ACCESS);  // ������ ������
    if (schSCManager == NULL)
        MyErrorExit("OpenSCManager");
    schService = OpenService(
        schSCManager,       // ���� ������ SCManager
        L"Sample_Srv",      // ��� �������
        DELETE);            // ����� ������ �������
    if (schService == NULL)
        MyErrorExit("OpenService");

    if (!DeleteService(schService))
        MyErrorExit("DeleteService");
    else
        printf("DeleteService SUCCESS\n");
    CloseServiceHandle(schService);
}

DWORD StartSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService)
{
    SERVICE_STATUS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwStatus;

    schService = OpenService(
        schSCManager,           // ���� �������� SCM
        L"Sample_Srv",          // ��� �������
        SERVICE_ALL_ACCESS);

    if (schService == NULL)
    {
        MyErrorExit("OpenService");
    }

    if (!StartService(
        schService,  // ���������� �������
        0,           // ���������� ����������
        NULL))       // ��� ����������
    {
        MyErrorExit("StartService");
    }
    else
    {
        printf("Service start pending.\n");
    }

    // ��������� ��������� ������� �� ��� ���, ���� �� �� ����������.

    if (!QueryServiceStatus(
        schService,   // ���������� �������
        &ssStatus))  // ����� ��������� � ����������� � �������
    {
        MyErrorExit("QueryServiceStatus");
    }

    // ��������� ������� ����� � ��������� �����.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        // �� ����� ������� ������, ��� ��������������� �����,
        // ����������� ��� ������. ������ ������� ����� ����� ����
        // ������� ����� ��������, �� �� ����� ���� �������
        // � ����� �� ����� 10 ������.

        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // ����� ��������� ������.

        if (!QueryServiceStatus(
            schService,   // ���������� �������
            &ssStatus))  // ����� ���������
            break;

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // ������ � �������� ������.

            dwStartTickCount = GetTickCount();
                dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                // ����� �� ������ �� ����������
                break;
            }
        }
    }

    if (ssStatus.dwCurrentState == SERVICE_RUNNING)
    {
        printf("StartService SUCCESS.\n");
        dwStatus = NO_ERROR;
    }
    else
    {
        printf("\n������ �� �������. \n");
        printf("  Current State: %d\n", ssStatus.dwCurrentState);
        printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
        printf("  Service Specific Exit Code: %d\n",
            ssStatus.dwServiceSpecificExitCode);
        printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
        printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
        dwStatus = GetLastError();
    }

    CloseServiceHandle(schService);
    return dwStatus;
}
#endif
