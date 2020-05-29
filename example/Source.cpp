#pragma warning(disable : 4996)
#include <time.h>
#include <stdio.h>
#include <windows.h>

tm* ConvDWORD2Time(DWORD dwTime);
void ShowRECORD(PVOID lpBuffer, DWORD* dwPos);

int main()
{
    DWORD dwCurentRecord = 0;
    DWORD NumberOfRecords = 0;
    LPVOID lpBuffer;
    DWORD dwRead = 0;
    DWORD dwNeed = 0;
    DWORD dwSize = sizeof(EVENTLOGRECORD);
    DWORD dwPos = 0;
    system("chcp 1251");
    HANDLE hEventLog = OpenEventLog(NULL, L"C:\\Windows\\System32\\winevt\\Logs\\Application.evtx");
    if (!hEventLog)
        MessageBox(0, L"OpenEventLog", NULL, MB_OK | MB_ICONERROR);
    else
        if (!GetNumberOfEventLogRecords(hEventLog, &NumberOfRecords))
            MessageBox(0, L"GetNumberOfEventLogRecords", NULL, MB_OK | MB_ICONERROR);
        else
        {
            printf("HANDLE hEventLog      : 0x%p\n", hEventLog);
            printf("DWORD NumberOfRecords : %u\n", NumberOfRecords);
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
                return 1;
            }
            else
                for
                    (dwPos = 0; 0 < dwRead; dwRead = dwRead - dwPos)
                {
                    ShowRECORD(((char*)lpBuffer + dwPos), &dwPos);
                    system("pause");
                }
        }
    if (hEventLog)
        CloseHandle(hEventLog);
    system("pause");
    return 0;
}

tm* ConvDWORD2Time(DWORD dwTime)
{
    time_t t = (time_t)dwTime;
    return localtime(&t);
}

void ShowRECORD(PVOID lpBuffer, DWORD* dwPos)
{

    tm* ptm;
    EVENTLOGRECORD pRECORD = *(EVENTLOGRECORD*)lpBuffer;
    printf("Length       : %d\n", pRECORD.Length);
    printf("RecordNumber : %d\n", pRECORD.RecordNumber);
    printf("TimeGenerated: %s", asctime(ConvDWORD2Time(pRECORD.TimeGenerated)));
    printf("TimeWritten  : %s", asctime(ConvDWORD2Time(pRECORD.TimeWritten)));
    printf("EventID      : %d\n", pRECORD.EventID);
    printf("EventType    : ");
    switch (pRECORD.EventType)
    {
    case EVENTLOG_ERROR_TYPE:
        printf("Error event\n");
        break;
    case EVENTLOG_AUDIT_FAILURE:
        printf("Failure Audit event\n");
        break;
    case EVENTLOG_AUDIT_SUCCESS:
        printf("Success Audit event\n");
        break;
    case EVENTLOG_INFORMATION_TYPE:
        printf("Information event\n");
        break;
    case EVENTLOG_WARNING_TYPE:
        printf("Warning event\n");
        break;
    }
    printf("NumStrings   : %d\n", pRECORD.NumStrings);
    printf("StringOffset : %d\n", pRECORD.StringOffset);
    printf("UserSidLength: %d\n", pRECORD.UserSidLength);
    printf("UserSidOffset: %d\n", pRECORD.UserSidOffset);
    printf("DataLength   : %d\n", pRECORD.DataLength);
    printf("DataOffset   : %d\n", pRECORD.DataOffset);
    printf("DataEntry    : %s\n", (char*)lpBuffer + pRECORD.StringOffset);
    printf("\n");

    (*dwPos) = pRECORD.Length;
}