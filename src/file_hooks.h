#pragma once

#include <windows.h>

bool InitializeFileHooks();
void ShutdownFileHooks();

typedef HANDLE(WINAPI* CreateFileW_t)(
    LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

extern CreateFileW_t g_OriginalCreateFileW;