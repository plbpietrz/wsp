// pwd.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Everything.h"

#define DIRNAME_LEN (MAX_PATH + 2)
INT _tmain(INT argc, LPTSTR argv[])
{
    TCHAR pwdBuffer[DIRNAME_LEN];
    DWORD lenCurrDir;

    lenCurrDir = GetCurrentDirectory(DIRNAME_LEN, &pwdBuffer);
    if (lenCurrDir == 0)
        ReportError(_T("Problem checking current directory"), 1, TRUE);
    if (lenCurrDir > DIRNAME_LEN)
        ReportError(_T("Path too long"), 2, FALSE);
    PrintMsg(GetStdHandle(STD_OUTPUT_HANDLE), pwdBuffer);
    return EXIT_SUCCESS;
}
