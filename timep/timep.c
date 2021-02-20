// timep.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Everything.h"

int _tmain(INT argc, LPTSTR argv[])
{
    STARTUPINFO startUp;
    PROCESS_INFORMATION procInfo;
    union {
        LONGLONG li;
        FILETIME ft;
    } createTime, exitTime, elapsedTime;
    FILETIME kernelTime, userTime;
    SYSTEMTIME elTiSys, keTiSys, usTiSys, startTimeSys;
    LPTSTR targv = SkipArg(GetCommandLine(), 1, argc, argv);

    GetStartupInfo(&startUp);
    GetSystemTime(&startTimeSys);

    if (CreateProcess(NULL, targv, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startUp, &procInfo) == TRUE) {
        HANDLE hProc = procInfo.hProcess;

        WaitForSingleObject(hProc, INFINITE);

        GetProcessTimes(hProc, &createTime.ft, &exitTime.ft, &kernelTime, &userTime);
        elapsedTime.li = exitTime.li - createTime.li;
        FileTimeToSystemTime(&elapsedTime.ft, &elTiSys);
        FileTimeToSystemTime(&kernelTime, &keTiSys);
        FileTimeToSystemTime(&userTime, &usTiSys);
        _tprintf(_T("\n"));
        _tprintf(_T("Real time: %02d:%02d:%02d:%03d\n"), elTiSys.wHour, elTiSys.wMinute, elTiSys.wSecond, elTiSys.wMilliseconds);
        _tprintf(_T("User time: %02d:%02d:%02d:%03d\n"), usTiSys.wHour, usTiSys.wMinute, usTiSys.wSecond, usTiSys.wMilliseconds);
        _tprintf(_T("Krnl time: %02d:%02d:%02d:%03d\n"), keTiSys.wHour, keTiSys.wMinute, keTiSys.wSecond, keTiSys.wMilliseconds);

        CloseHandle(procInfo.hThread);
        CloseHandle(procInfo.hProcess);
        CloseHandle(hProc);
        return EXIT_SUCCESS;
    } else {
        _tprintf(_T("Couldn't spawn process '%s'!\n"), targv);
        return EXIT_FAILURE;
    }
}
