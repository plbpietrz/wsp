// findw.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

VOID FindFilesInDirectory(LPTSTR szBaseDirectory, LPTSTR szFile) {
    TCHAR szFullPath[MAX_PATH];
    WIN32_FIND_DATA sFindData = { 0 };

    // traverse directories with DFS
    PathCombine(szFullPath, szBaseDirectory, _T("*"));
    HANDLE hFindFile = FindFirstFile(szFullPath, &sFindData);
    if (hFindFile != INVALID_HANDLE_VALUE) {
        do {
            if (sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (_tcscmp(sFindData.cFileName, _T(".")) && _tcscmp(sFindData.cFileName, _T(".."))) {
                    PathCombine(szFullPath, szBaseDirectory, sFindData.cFileName);
                    FindFilesInDirectory(szFullPath, szFile);
                }
                else
                    continue;
            }
        } while (FindNextFile(hFindFile, &sFindData));
        FindClose(hFindFile);
    }

    // when done with recursive directories checking, go through the files
    PathCombine(szFullPath, szBaseDirectory, szFile);
    hFindFile = FindFirstFile(szFullPath, &sFindData);
    if (hFindFile != INVALID_HANDLE_VALUE) {
        do {
            if (!(sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                PathCombine(szFullPath, szBaseDirectory, sFindData.cFileName);
                _tprintf(_T("%s\n"), szFullPath);
            }
        } while (FindNextFile(hFindFile, &sFindData));
        FindClose(hFindFile);
    }
}

INT _tmain(INT argc, LPTSTR argv[]) {
    
    if (argc < 3) {
        _tprintf(_T("Usage: findw <path> <file_name>\n"));
        return EXIT_SUCCESS;
    }
    TCHAR szCurrentWorkDir[MAX_PATH];
    GetCurrentDirectory(sizeof(szCurrentWorkDir), szCurrentWorkDir);
    
    // construct the actual search starting directory based on 1st input argument
    TCHAR szSearchDir[MAX_PATH];
    if (PathIsRelative(argv[1]))
        PathCombine(szSearchDir, szCurrentWorkDir, argv[1]);
    else
        _tcscpy_s(szSearchDir, sizeof(szSearchDir)/sizeof(TCHAR), argv[1]);
    
    WIN32_FIND_DATA sFindData = { 0 };
    HANDLE hFindFile = FindFirstFile(szSearchDir, &sFindData);
    do {
        if (_tcscmp(sFindData.cFileName, _T(".")) && _tcscmp(sFindData.cFileName, _T("..")))
            FindFilesInDirectory(szSearchDir, argv[2]);
    } while (FindNextFile(hFindFile, &sFindData));
    FindClose(hFindFile);
        
    return EXIT_SUCCESS;
}
