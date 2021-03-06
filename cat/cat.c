// cat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Everything.h"

#define BUF_SIZE 0x200

static VOID CatFile(HANDLE, HANDLE);

INT _tmain(INT argc, LPTSTR argv[]) {
    HANDLE hInFile, hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    BOOL dashS;
    INT iArg, iFirstFile;
    iFirstFile = Options(argc, argv, _T("s"), &dashS, NULL);

    if (iFirstFile == argc) {
        CatFile(hStdIn, hStdOut);
    } else {
        for (iArg = iFirstFile; iArg < argc; ++iArg) {
            hInFile = CreateFile(argv[iArg], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hInFile == INVALID_HANDLE_VALUE) {
                if (!dashS)
                    ReportError(_T("Error: File does not exist."), 0, TRUE);
            } else {
                CatFile(hInFile, hStdOut);
                if (GetLastError() != 0 && !dashS)
                    ReportError(_T("Cat Error."), 0, TRUE);
                CloseHandle(hInFile);
            }
        }
    }
    return EXIT_SUCCESS;
}

static VOID CatFile(HANDLE hInFile, HANDLE hOutFile) {
    DWORD nIn, nOut;
    BYTE buffer[BUF_SIZE];
    HANDLE stdOutCon = GetStdHandle(STD_OUTPUT_HANDLE);
    __try {
        if (hOutFile == stdOutCon)
            while (ReadFile(hInFile, buffer, BUF_SIZE, &nIn, NULL) && (nIn != 0) && WriteConsole(stdOutCon, buffer, nIn, &nOut, NULL));
        else
            while (ReadFile(hInFile, buffer, BUF_SIZE, &nIn, NULL) && (nIn != 0) && WriteFile(hOutFile, buffer, nIn, &nOut, NULL));
        }
    __finally {
        CloseHandle(stdOutCon);
    }
    return;
}