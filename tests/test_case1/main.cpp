#include <Windows.h>
#include <stdio.h>
#include <iostream>

void show_date()
{
    SYSTEMTIME SystemTime;
    GetSystemTime(&SystemTime);

    TCHAR pszDate[200];
    GetDateFormatA( LOCALE_USER_DEFAULT, DATE_LONGDATE, &SystemTime, NULL, pszDate, 200 );

    std::cout << "Date: " << pszDate << "\n";
}

int demo()
{
    show_date();
    std::cout << "Test Case 1 finished\n";
    return 0;
}

BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        demo();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
