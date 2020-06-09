#ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501    // Change this to the appropriate value to target other versions of Windows.
#endif                        
#include <windows.h>
#include <stdio.h>
SERVICE_STATUS          longstartsvcStatus;
SERVICE_STATUS_HANDLE   longstartsvcStatusHandle;
VOID  __stdcall longstartsvcStart(DWORD argc, LPTSTR* argv);
VOID  __stdcall longstartsvcCtrlHandler(DWORD opcode);
DWORD longstartsvcInitialization(DWORD argc, LPTSTR* argv,
    DWORD* specificError);
VOID SvcDebugOut(LPSTR String, DWORD Status)
{
    CHAR  Buffer[1024];
    if (strlen(String) < 1000)
    {
        sprintf(Buffer, String, Status);
        OutputDebugStringA(Buffer);
    }
}
char ServiceName[] = "longstartsvc";
void usage(FILE* out) {
    fprintf(out, "usage: longstartsvc.exe [-install|-remove]");
}
void MyErrorExit(char* msg) {
    fprintf(stderr, "%s\n", msg);
    ExitProcess(1);
}
VOID CreateSampleService()
{
    char filename[MAX_PATH];
    SC_HANDLE schSCManager, schService;
    GetModuleFileName(NULL, filename, sizeof(filename) / sizeof(filename[0]));
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL)
        MyErrorExit("OpenSCManager");
    schService = CreateService(
        schSCManager,              // SCManager database 
        ServiceName,              // name of service 
        ServiceName,           // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        filename,        // service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 

    if (schService == NULL)
        MyErrorExit("CreateService");
    else
        printf("CreateService SUCCESS.\n");

    CloseServiceHandle(schService);
}
VOID DeleteSampleService()
{
    SC_HANDLE schSCManager, schService;
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL)
        MyErrorExit("OpenSCManager");
    schService = OpenService(
        schSCManager,       // SCManager database 
        ServiceName,       // name of service 
        DELETE);            // only need DELETE access 

    if (schService == NULL)
        MyErrorExit("OpenService");

    if (!DeleteService(schService))
        MyErrorExit("DeleteService");
    else
        printf("DeleteService SUCCESS\n");

    CloseServiceHandle(schService);
}
/* check strtol()/strtod() result */
static int strtox_check(char* nptr, char* endptr) {
    char dummys[2];
    /* Check for various possible errors */
    /* if ((errno == ERANGE && (*n == LONG_MAX || *n == LONG_MIN))
           || (errno != 0 && *n == 0)) { */
    if (errno != 0) {
        /*pdebug8("strtol(): failed\n");*/
        return errno;
    }
    /*pdebug8("strtol(): nptr: '%s', endptr: '%s'\n", nptr, endptr);*/
    if (nptr == endptr) {
        /*pdebug8("strtol(): No digits were found\n");*/
        return -1;
    }
    /* If we got here, strtol() successfully parsed a number */
    if (*endptr != '\0' && *endptr != '\n' && sscanf(endptr, "%1s", dummys) > 0) {
        /*pdebug8("strtol(): Further characters after number: %s\n", endptr);*/
        return -1;
    }
    return 0;
}
int main(int argc, char* argv[])
{
    SERVICE_TABLE_ENTRY   DispatchTable[] =
    {
        { ServiceName, longstartsvcStart      },
        { NULL,              NULL          }
    };
    if (argc > 2) {
        usage(stderr);
        return 1;
    }
    else if (argc == 2) {
        if (0 == strcmp(argv[1], "-install")) {
            CreateSampleService();
        }
        else if (0 == strcmp(argv[1], "-remove")) {
            DeleteSampleService();
        }
        else {
            usage(stderr);
            return 1;
        }
        return 0;
    }
    if (!StartServiceCtrlDispatcher(DispatchTable))
    {
        SvcDebugOut(" [longstartsvc] StartServiceCtrlDispatcher error = %d\n", GetLastError());
    }
    return 0;
}
VOID __stdcall longstartsvcCtrlHandler(DWORD Opcode)
{
    DWORD status;

    switch (Opcode)
    {
    case SERVICE_CONTROL_PAUSE:
        // Do whatever it takes to pause here. 
        longstartsvcStatus.dwCurrentState = SERVICE_PAUSED;
        break;

    case SERVICE_CONTROL_CONTINUE:
        // Do whatever it takes to continue here. 
        longstartsvcStatus.dwCurrentState = SERVICE_RUNNING;
        break;

    case SERVICE_CONTROL_STOP:
        // Do whatever it takes to stop here. 
        longstartsvcStatus.dwWin32ExitCode = 0;
        longstartsvcStatus.dwCurrentState = SERVICE_STOPPED;
        longstartsvcStatus.dwCheckPoint = 0;
        longstartsvcStatus.dwWaitHint = 0;

        if (!SetServiceStatus(longstartsvcStatusHandle,
            &longstartsvcStatus))
        {
            status = GetLastError();
            SvcDebugOut(" [longstartsvc] SetServiceStatus error %ld\n", status);
        }

        SvcDebugOut(" [longstartsvc] Leaving longstartsvc \n", 0);
        return;

    case SERVICE_CONTROL_INTERROGATE:
        // Fall through to send current status. 
        break;

    default:
        SvcDebugOut(" [longstartsvc] Unrecognized opcode %ld\n",
            Opcode);
    }

    // Send current status. 
    if (!SetServiceStatus(longstartsvcStatusHandle, &longstartsvcStatus))
    {
        status = GetLastError();
        SvcDebugOut(" [longstartsvc] SetServiceStatus error %ld\n", status);
    }
    return;
}
void __stdcall longstartsvcStart(DWORD argc, LPTSTR* argv)
{
    DWORD status;
    DWORD specificError;

    longstartsvcStatus.dwServiceType = SERVICE_WIN32;
    longstartsvcStatus.dwCurrentState = SERVICE_START_PENDING;
    longstartsvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
        SERVICE_ACCEPT_PAUSE_CONTINUE;
    longstartsvcStatus.dwWin32ExitCode = 0;
    longstartsvcStatus.dwServiceSpecificExitCode = 0;
    longstartsvcStatus.dwCheckPoint = 0;
    longstartsvcStatus.dwWaitHint = 120000;

    longstartsvcStatusHandle = RegisterServiceCtrlHandler(
        "longstartsvc",
        longstartsvcCtrlHandler);

    if (longstartsvcStatusHandle == (SERVICE_STATUS_HANDLE)0)
    {
        SvcDebugOut(" [longstartsvc] RegisterServiceCtrlHandler failed %d\n", GetLastError());
        return;
    }

    // Initialization code goes here. 
    status = longstartsvcInitialization(argc, argv, &specificError);

    // Handle error condition 
    if (status != NO_ERROR)
    {
        longstartsvcStatus.dwCurrentState = SERVICE_STOPPED;
        longstartsvcStatus.dwCheckPoint = 0;
        longstartsvcStatus.dwWaitHint = 0;
        longstartsvcStatus.dwWin32ExitCode = status;
        longstartsvcStatus.dwServiceSpecificExitCode = specificError;

        SetServiceStatus(longstartsvcStatusHandle, &longstartsvcStatus);
        return;
    }

    // Initialization complete - report running status. 
    longstartsvcStatus.dwCurrentState = SERVICE_RUNNING;
    longstartsvcStatus.dwCheckPoint = 0;
    longstartsvcStatus.dwWaitHint = 0;

    if (!SetServiceStatus(longstartsvcStatusHandle, &longstartsvcStatus))
    {
        status = GetLastError();
        SvcDebugOut(" [longstartsvc] SetServiceStatus error %ld\n", status);
    }

    // This is where the service does its work. 
    SvcDebugOut(" [longstartsvc] Returning the Main Thread \n", 0);

    return;
}

// Stub initialization function. 
DWORD longstartsvcInitialization(DWORD   argc, LPTSTR* argv,
    DWORD* specificError)
{
    while (longstartsvcStatus.dwWaitHint > 0) {
        if (longstartsvcStatus.dwWaitHint > 10000) {
            Sleep(10000);
            longstartsvcStatus.dwWaitHint -= 10000;
        }
        else {
            Sleep(longstartsvcStatus.dwWaitHint);
            longstartsvcStatus.dwWaitHint = 0;
        }
        longstartsvcStatus.dwCheckPoint++;
        SetServiceStatus(longstartsvcStatusHandle, &longstartsvcStatus);
    }
    return(0);
}