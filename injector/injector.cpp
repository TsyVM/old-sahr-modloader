#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>

DWORD GetProcessIdByName(const std::wstring& processName) {
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;
    
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (processName == entry.szExeFile) {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        } while (Process32NextW(snapshot, &entry));
    }
    
    CloseHandle(snapshot);
    return 0;
}

bool InjectDLL(DWORD processId, const std::wstring& dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) {
        std::wcerr << L"Failed to open process. Error: " << GetLastError() << std::endl;
        return false;
    }
    
    SIZE_T pathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID pRemotePath = VirtualAllocEx(hProcess, NULL, pathSize, MEM_COMMIT, PAGE_READWRITE);
    if (!pRemotePath) {
        std::wcerr << L"Failed to allocate memory. Error: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }
    
    if (!WriteProcessMemory(hProcess, pRemotePath, dllPath.c_str(), pathSize, NULL)) {
        std::wcerr << L"Failed to write memory. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }
    
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    LPVOID pLoadLibraryW = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryW");
    
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)pLoadLibraryW, pRemotePath, 0, NULL);
    
    if (!hThread) {
        std::wcerr << L"Failed to create remote thread. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }
    
    WaitForSingleObject(hThread, INFINITE);
    
    VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    
    return true;
}

int wmain(int argc, wchar_t* argv[]) {
    std::wcout << L"=== SHAR ModLoader Injector ===" << std::endl;
    std::wcout << L"Version 1.0.0" << std::endl;
    std::wcout << std::endl;
    
    if (argc < 2) {
        std::wcout << L"Usage: SHARInjector.exe <path_to_SHARModLoader.dll>" << std::endl;
        std::wcout << L"Example: SHARInjector.exe \"SHARModLoader.dll\"" << std::endl;
        std::wcout << std::endl;
        std::wcout << L"The DLL will be injected into Simpsons.exe" << std::endl;
        return 1;
    }
    
    std::wstring dllPath = argv[1];
    
    if (GetFileAttributesW(dllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        std::wcerr << L"ERROR: DLL not found: " << dllPath << std::endl;
        return 1;
    }
    
    wchar_t fullPath[MAX_PATH];
    GetFullPathNameW(dllPath.c_str(), MAX_PATH, fullPath, NULL);
    dllPath = fullPath;
    
    std::wcout << L"DLL Path: " << dllPath << std::endl;
    std::wcout << L"Searching for Simpsons.exe..." << std::endl;
    
    DWORD pid = GetProcessIdByName(L"Simpsons.exe");
    
    if (pid == 0) {
        std::wcerr << L"ERROR: Simpsons.exe not found!" << std::endl;
        std::wcerr << L"Please start the game first, then run this injector." << std::endl;
        return 1;
    }
    
    std::wcout << L"Found Simpsons.exe (PID: " << pid << L")" << std::endl;
    std::wcout << L"Injecting DLL..." << std::endl;
    
    if (InjectDLL(pid, dllPath)) {
        std::wcout << std::endl;
        std::wcout << L"SUCCESS! ModLoader injected successfully." << std::endl;
        std::wcout << L"Check ModLoader.log in the game directory for details." << std::endl;
        return 0;
    } else {
        std::wcerr << std::endl;
        std::wcerr << L"FAILED to inject DLL." << std::endl;
        std::wcerr << L"Try running this program as Administrator." << std::endl;
        return 1;
    }
}