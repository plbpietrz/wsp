#pragma once
#include "pch.h"
#ifndef RECORD_ACCESS
#define RECORD_ACCESS

#define STRING_SIZE 256

typedef struct _RECORD {
    DWORD referenceCount;
    SYSTEMTIME recordCreationTime;
    SYSTEMTIME recordLastReferenceTime;
    SYSTEMTIME recordUpdateTime;
    TCHAR dataString[STRING_SIZE];
} RECORD;

typedef struct _HEADER {
    DWORD numRecords;
    DWORD numNonEmptyRecords;
} HEADER;

#endif // !RECORD_ACCESS
