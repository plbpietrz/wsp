/* DllGetVersion function implementation */
#pragma warning (disable: 4273)

#include "Everything.h"

#if 0
// An alternative. Locate with GetProcAddress
declspec (dllexport) _HRESULT CALLBACK DllGetVersion(DLLVERSIONINFO *pdvi)
{
    /* pdvi->cbSize = sizeof(DLLVERSIONINFO); This should be filled in by the caller */
    pdvi->dwMajorVersion = 4;
    pdvi->dwMinorVersion = 0;
    pdvi->dwBuildNumber  = 1;  /* Increment every time the implementation changes */
    pdvi->dwPlatformID = DLLVER_PLATFORM_WINDOWS; /* No platform restrictions */
    return NOERROR;
}
#endif

typedef BOOL (WINAPI *LPFNGetVersionEx)(LPOSVERSIONINFO);

BOOL WindowsVersionOK (DWORD MajorVerRequired, DWORD MinorVerRequired)
{
    /* Can this Windows version run this application */
    OSVERSIONINFO OurVersion = { 0 };

/*
typedef struct _OSVERSIONINFO{ 
    DWORD dwOSVersionInfoSize; 
    DWORD dwMajorVersion; 
    DWORD dwMinorVersion; 
    DWORD dwBuildNumber; 
    DWORD dwPlatformId; 
    TCHAR szCSDVersion[ 128 ]; 
} OSVERSIONINFO; 
*/ 

    /* Determine version information.  */
    /* The version number to test for is an NT version number */
    /* 9x systems must be ruled out explicitely */
    /* See the OSVERSIONINFO description in MSDN for Windows 9x */
    OurVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    HMODULE kernel32Handle = GetModuleHandle(_T("kernel32.dll"));
#ifdef UNICODE
    LPFNGetVersionEx getVersionEx = (LPFNGetVersionEx)GetProcAddress(kernel32Handle, "GetVersionExW");
#else
    LPFNGetVersionEx getVersionEx = (LPFNGetVersionEx)GetProcAddress(kernel32Handle, "GetVersionExA");
#endif // UNICODE

    if (!getVersionEx) {
        ReportError(_T("Cannot get GetVersionEx address!"), 1, TRUE);
    }

    if (!getVersionEx(&OurVersion))
        ReportError (_T("Cannot get OS Version info."), 1, TRUE);

    return ( (OurVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
               ((OurVersion.dwMajorVersion > MajorVerRequired) ||
                (OurVersion.dwMajorVersion >= MajorVerRequired && 
                 OurVersion.dwMinorVersion >= MinorVerRequired) ));
}

