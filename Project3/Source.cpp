//#define DEBUG

#include <Windows.h>
#include <iostream>
using namespace std;
#ifndef DEBUG



void MyErrorExit(const char* msg) {
    cout << msg << endl;
    cout << "Код ошибки: " << GetLastError() << endl;
    system("pause");
    ExitProcess(1);
}

VOID CreateSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService);
DWORD StartSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService);

int main() {
    setlocale(LC_ALL, "ru");
    SC_HANDLE schSCManager, schService;
    schSCManager = OpenSCManager(
        NULL,                    // локальная машина
        NULL,                    // имя базы (ServicesActive)
        SC_MANAGER_ALL_ACCESS);  // полный доступ
    if (schSCManager == NULL) MyErrorExit("OpenSCManager");
    CreateSampleService(schSCManager, schService);
    StartSampleService(schSCManager, schService);
    system("pause");
}

VOID CreateSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService)
{
    wchar_t ServiceName[] = L"Пользовательская служба";
    LPCTSTR lpszBinaryPathName = L"C:\\Users\\max\\Desktop\\LD_service.exe";
    LPCTSTR lpszDisplayName;
    schService = CreateService(
        schSCManager,              // база данныйх SCManager
        L"Sample_Srv",             // имя сервиса
        ServiceName,               // имя, которое будет показываться
        SERVICE_ALL_ACCESS,        // уровень доступа
        SERVICE_WIN32_OWN_PROCESS, // сервис будет запускаться
                                   // в собственном процессе
        SERVICE_DEMAND_START,      // будет запускаться "вручную"
        SERVICE_ERROR_NORMAL,      // стандартный режим
                                   // протоколирования ошибок
        lpszBinaryPathName,        // путь к сервису
        NULL,                      // сервис не принадлежит группе
        NULL,                      // --""--
        NULL,                      // нет зависимости
        NULL,                      // С правами системы
        NULL);                     // без пароля

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
        schSCManager,           // база сервисов SCM
        L"Sample_Srv",          // имя сервиса
        SERVICE_ALL_ACCESS);

    if (schService == NULL) MyErrorExit("OpenService");
    if (!StartService(
        schService,  // дескриптор сервиса
        0,           // количество аргументов
        NULL))       // нет аргументов
    {
        MyErrorExit("StartService");
    }
    else
    {
        printf("Service start pending.\n");
    }
    // Проверяем состояние сервиса до тех пор, пока он не запустится.
    if (!QueryServiceStatus(
        schService,   // дескриптор сервиса
        &ssStatus))  // адрес структуры с информацией о сервисе
    {
        MyErrorExit("QueryServiceStatus");
    }

    // Сохраняем счётчик тиков и начальную точку.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        // Не нужно ожидать дольше, чем приблизительное время,
        // необходимое для старта. Обычно ожидать лучше всего одну
        // десятую этого значения, но не более одно секунды
        // и никак не более 10 секунд.

        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // Снова проверяем статус.

        if (!QueryServiceStatus(
            schService,   // дескриптор сервиса
            &ssStatus))  // адрес структуры
            break;

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // Сервис в процесса старта.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                // Время до старта не изменилось
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
        printf("\nСервис на запущен. \n");
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
    cout << "1) Создать сервис" << endl;
    cout << "2) Удалить сервис" << endl;
    cout << "3) Запустить сервис" << endl;
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
        NULL,                    // локальная машина
        NULL,                    // имя базы (ServicesActive)
        SC_MANAGER_ALL_ACCESS);  // полный доступ
    while (true) {
        switch (getAnswer()) {
        case CREATESERVICE: CreateSampleService(schSCManager, schService); break;
        case DELETESERVICE: DeleteSampleService(schSCManager, schService); break;
        case STARTSERVICE: StartSampleService(schSCManager, schService);  break;
        default: cout << "Неизвестная команда, программа будет завершена. "; system("pause"); return 2;
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
        schSCManager,              // база данныйх SCManager
        L"Sample_Srv",             // имя сервиса
        ServiceName,               // имя, которое будет показываться
        SERVICE_ALL_ACCESS,        // уровень доступа
        SERVICE_WIN32_OWN_PROCESS, // сервис будет запускаться
                                   // в собственном процессе
        SERVICE_DEMAND_START,      // будет запускаться "вручную"
        SERVICE_ERROR_NORMAL,      // стандартный режим
                                   // протоколирования ошибок
        lpszBinaryPathName,        // путь к сервису
        NULL,                      // сервис не принадлежит группе
        NULL,                      // --""--
        NULL,                      // нет зависимости
        NULL,                      // С правами системы
        NULL);                     // без пароля

    if (schService == NULL)
        MyErrorExit("CreateService");
    else
        printf("CreateService SUCCESS.\n");

    CloseServiceHandle(schService);
}

VOID DeleteSampleService(SC_HANDLE schSCManager, SC_HANDLE& schService)
{
    schSCManager = OpenSCManager(
        NULL,                    // локальная машина
        NULL,                    // имя базы (ServicesActive)
        SC_MANAGER_ALL_ACCESS);  // полный доступ
    if (schSCManager == NULL)
        MyErrorExit("OpenSCManager");
    schService = OpenService(
        schSCManager,       // база данных SCManager
        L"Sample_Srv",      // имя сервиса
        DELETE);            // нужно только удалить
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
        schSCManager,           // база сервисов SCM
        L"Sample_Srv",          // имя сервиса
        SERVICE_ALL_ACCESS);

    if (schService == NULL)
    {
        MyErrorExit("OpenService");
    }

    if (!StartService(
        schService,  // дескриптор сервиса
        0,           // количество аргументов
        NULL))       // нет аргументов
    {
        MyErrorExit("StartService");
    }
    else
    {
        printf("Service start pending.\n");
    }

    // Проверяем состояние сервиса до тех пор, пока он не запустится.

    if (!QueryServiceStatus(
        schService,   // дескриптор сервиса
        &ssStatus))  // адрес структуры с информацией о сервисе
    {
        MyErrorExit("QueryServiceStatus");
    }

    // Сохраняем счётчик тиков и начальную точку.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        // Не нужно ожидать дольше, чем приблизительное время,
        // необходимое для старта. Обычно ожидать лучше всего одну
        // десятую этого значения, но не более одно секунды
        // и никак не более 10 секунд.

        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // Снова проверяем статус.

        if (!QueryServiceStatus(
            schService,   // дескриптор сервиса
            &ssStatus))  // адрес структуры
            break;

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // Сервис в процесса старта.

            dwStartTickCount = GetTickCount();
                dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                // Время до старта не изменилось
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
        printf("\nСервис на запущен. \n");
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
