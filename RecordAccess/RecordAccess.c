// RecordAccess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "RecordAccess.h"

#define _CRT_SECURE_NO_WARNINGS

INT _tmain(INT argc, PTCHAR argv[])
{
    HANDLE hFile;
    LARGE_INTEGER curPtr;
    DWORD openOption, nXfer, recNo;
    RECORD record;
    TCHAR string[STRING_SIZE], command, extra;
    OVERLAPPED ov = { 0, 0, 0, 0, NULL }, ovZero = { 0, 0, 0, 0, NULL };
    HEADER header = { 0, 0 };
    SYSTEMTIME currentTime;
    BOOLEAN headerChange, recordChange;

    if (argc == 1) {
        _tprintf(_T("usage: RecordAccess filename [r|w|d]\n"));
        return EXIT_SUCCESS;
    }

    openOption = ((argc > 2 && _ttoi(argv[2]) <= 0) || argc <= 2) ? OPEN_EXISTING : CREATE_ALWAYS;
    hFile = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE, 0, NULL, openOption, FILE_FLAG_RANDOM_ACCESS, NULL);

    if (argc >= 3 && _ttoi(argv[2]) > 0) {
        header.numRecords = _ttoi(argv[2]);
        WriteFile(hFile, &header, sizeof(header), &nXfer, &ovZero);
        curPtr.QuadPart = (LONGLONG)sizeof(RECORD) * _ttoi(argv[2]) + sizeof(HEADER);
        SetFilePointerEx(hFile, curPtr, NULL, FILE_BEGIN);
        SetEndOfFile(hFile);
        return EXIT_SUCCESS;
    }

    ReadFile(hFile, &header, sizeof(HEADER), &nXfer, &ovZero);

    while (TRUE) {
        headerChange = FALSE; recordChange = FALSE;
        _tprintf(_T("Enter r(ead)/w(rite)/d(elete)/q(uit) rec#\n"));
        _tscanf_s(_T("%c%u%c"), &command, 1, &recNo, &extra, 1);
        if (command == 'q') break;
        if (recNo >= header.numRecords) {
            _tprintf(_T("Record number os too large. Try again.\n"));
            continue;
        }
        else {
            curPtr.QuadPart = (LONGLONG)recNo * sizeof(RECORD) + sizeof(HEADER);
            ov.Offset = curPtr.LowPart;
            ov.OffsetHigh = curPtr.HighPart;
            ReadFile(hFile, &record, sizeof(RECORD), &nXfer, &ov);
            GetSystemTime(&currentTime);
            record.recordLastReferenceTime = currentTime;
            if (command == 'r' || command == 'd') {
                if (record.referenceCount == 0) {
                    _tprintf(_T("Record number %d is empty.\n"), recNo);
                    continue;
                }
                else {
                    _tprintf(_T("Record number %d. Reference count: %d \n"), recNo, record.referenceCount);
                    _tprintf(_T("Data: %s\n"), record.dataString);
                }
                if (command == 'd') {
                    record.referenceCount = 0;
                    header.numNonEmptyRecords -= 1;
                    headerChange = TRUE;
                    recordChange = TRUE;
                }
            }
            else if (command == 'w') {
                _tprintf(_T("Enter new data string for the record.\n"));
                _fgetts(string, sizeof(string), stdin);
                string[_tcslen(string) - 1] = _T('\0');
                if (record.referenceCount == 0) {
                    record.recordCreationTime = currentTime;
                    header.numNonEmptyRecords += 1;
                    headerChange = TRUE;
                }
                record.recordUpdateTime = currentTime;
                record.referenceCount += 1;
                _tcsncpy_s(record.dataString, STRING_SIZE, string, STRING_SIZE - 1);
                recordChange = TRUE;
            }
            else {
                _tprintf(_T("Command must start with r, w, or d. Try again.\n"));
            }
            if (recordChange)
                WriteFile(hFile, &record, sizeof(RECORD), &nXfer, &ov);
            if (headerChange)
                WriteFile(hFile, &header, sizeof(HEADER), &nXfer, &ovZero);

        }
    }
    _tprintf(_T("Computed number of non-empty records is: %d\n"), header.numNonEmptyRecords);
    ReadFile(hFile, &header, sizeof(HEADER), &nXfer, &ovZero);
    _tprintf(_T("File %s has %d non-empty records.\nCapacity: %d\n"), argv[1], header.numNonEmptyRecords, header.numRecords);
    CloseHandle(hFile);
    return EXIT_SUCCESS;
}
