#pragma warning(disable : 4996)
#include <windows.h>
#include <ctime>
#include <iostream>
#include <string>
using namespace std;
//EVENTLOGRECORD{
//....
//DWORD  DataOffset;    // Offset from beginning of record
// Then follow:
//
// WCHAR SourceName[]
// WCHAR Computername[]
// SID   UserSid
// WCHAR Strings[]
// BYTE  Data[]
// CHAR  Pad[]
// DWORD Length;

string pEventTypeNames[] = { "Error", "Warning", "Informational", "Audit Success", "Audit Failure" };

DWORD GetEventTypeName(DWORD EventType);

std::string conv_time(DWORD dwTime) {
    std::time_t result = dwTime;
    return std::asctime(std::localtime(&result));
}

void print_one_record(BYTE* rec) {

    EVENTLOGRECORD* pRecord = (EVENTLOGRECORD*)rec;

    std::cout << "EventType: " << pEventTypeNames[GetEventTypeName(pRecord->EventType)] << std::endl;

    std::string source_name((char*)(rec + sizeof(EVENTLOGRECORD)));
    std::string computer_name((char*)(rec + sizeof(EVENTLOGRECORD) + source_name.length() + 1));

    std::cout << "Provider: " << source_name << std::endl;
    std::cout << "ComputerName: " << computer_name << std::endl;
    cout << "EventID: " << pRecord->EventID << endl;
    std::cout << "Length: " << pRecord->Length << std::endl;
    std::cout << "RecordNumber: " << pRecord->RecordNumber << std::endl;
    std::cout << "TimeGenerated: " << conv_time(pRecord->TimeGenerated) << std::endl;

    int count = 0;
    std::cout << "--Strings-- [ " << pRecord->Length << " ]" << std::endl;
    for (char* str = (char*)(rec + pRecord->StringOffset); count < pRecord->NumStrings; ++count) {
        std::cout << "  -> " << str << std::endl;
        str += strlen(str) + 1;
    }
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

int main()
{
    const unsigned MAX_BUFF_SIZE = 0x10000;

    DWORD NumberOfRecords = 0;
    BYTE* pBuffer = new BYTE[MAX_BUFF_SIZE];
    DWORD dwBytesToRead = MAX_BUFF_SIZE;
    DWORD status = ERROR_SUCCESS;

    DWORD dwBytesRead = 0;
    DWORD dwMinimumBytesToRead = 0;

    HANDLE hEventLog = OpenEventLogA(NULL, "Security.evtx");

    setlocale(LC_ALL, "Russian");
    if (!hEventLog)
    {
        std::cout << "OpenBackupEventLogA [ " << std::hex << GetLastError() << " ]" << std::endl;
        system("pause");
        return 0;
    }

    if (!GetNumberOfEventLogRecords(hEventLog, &NumberOfRecords))
    {
        std::cout << "GetNumberOfEventLogRecords [ " << std::hex << GetLastError() << " ]" << std::endl;
        system("pause");
        return 0;
    }

    std::cout << "NumberOfRecords [ " << NumberOfRecords << " ]" << std::endl;

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
                    return 0;
                    system("pause");
                }
            }
        }
        else
        {

            print_records(pBuffer, dwBytesRead);
        }
    }

    if (hEventLog)
        CloseEventLog(hEventLog);

    delete[] pBuffer;
    system("pause");
    return 0;
}

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