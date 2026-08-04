#include "file_hooks.h"
#include "mod_manager.h"
#include "utils.h"
#include "MinHook.h"
#include <string>
#include <algorithm>
#include <unordered_set>
#include <eh.h>         // For _set_se_translator
#include <exception>    // For std::exception
#include <functional>   // For RAII guard
#include <windows.h>

CreateFileW_t g_OriginalCreateFileW = nullptr;
thread_local bool g_inHook = false;

static std::unordered_set<std::wstring> g_noOverrideCache;
static const size_t MAX_CACHE_SIZE = 1000;

// === Structured Exception Translator ===
class SEHException : public std::exception {
public:
    explicit SEHException(unsigned int code) : code_(code) {}
    unsigned int code() const { return code_; }
private:
    unsigned int code_;
};

void SETranslator(unsigned int code, EXCEPTION_POINTERS*) {
    throw SEHException(code);
}

// RAII helper to reset g_inHook safely
struct ResetInHookFlag {
    ~ResetInHookFlag() { g_inHook = false; }
};
// ======================================

bool IsRelativePath(const std::wstring& path) {
    if (path.length() >= 2 && path[1] == L':') return false;
    if (path.length() >= 2 && path[0] == L'\\' && path[1] == L'\\') return false;
    return true;
}

std::wstring NormalizePath(const std::wstring& path) {
    std::wstring normalized = path;
    std::replace(normalized.begin(), normalized.end(), L'/', L'\\');

    if (normalized.length() >= 2 && normalized[0] == L'.' && normalized[1] == L'\\') {
        normalized = normalized.substr(2);
    }

    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::towlower);
    return normalized;
}

HANDLE WINAPI HookedCreateFileW(
    LPCWSTR               lpFileName,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttributes,
    HANDLE                hTemplateFile
) {
    if (g_inHook) {
        return g_OriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
            lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }

    g_inHook = true;
    ResetInHookFlag _guard; // Will reset g_inHook on function exit

    _set_se_translator(SETranslator); // Enable SEH-to-C++ translation

    try {
        if (!lpFileName || !g_pModManager) {
            return g_OriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
                lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        }

        std::wstring requestedPath = lpFileName;

        if (!IsRelativePath(requestedPath)) {
            return g_OriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
                lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        }

        std::wstring normalizedPath = NormalizePath(requestedPath);

        if (g_noOverrideCache.find(normalizedPath) != g_noOverrideCache.end()) {
            return g_OriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
                lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        }

        if (g_pModManager->HasOverride(normalizedPath)) {
            std::wstring overridePath = g_pModManager->GetOverride(normalizedPath);

            LogMessage("OVERRIDE: " + WStringToString(normalizedPath) +
                " -> " + WStringToString(overridePath));

            return g_OriginalCreateFileW(overridePath.c_str(), dwDesiredAccess, dwShareMode,
                lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        }

        if (g_noOverrideCache.size() < MAX_CACHE_SIZE) {
            g_noOverrideCache.insert(normalizedPath);
        }

        return g_OriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
            lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }
    catch (const SEHException& e) {
        LogMessage("EXCEPTION in CreateFileW hook! SEH code: " + std::to_string(e.code()));
    }
    catch (const std::exception& e) {
        LogMessage(std::string("STL exception in CreateFileW hook: ") + e.what());
    }
    catch (...) {
        LogMessage("UNKNOWN exception in CreateFileW hook.");
    }

    // Fallback in case of exception
    return g_OriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
        lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

bool InitializeFileHooks() {
    LogMessage("Initializing file hooks...");

    if (MH_Initialize() != MH_OK) {
        LogMessage("ERROR: MinHook initialization failed!");
        return false;
    }

    if (MH_CreateHookApi(L"kernel32", "CreateFileW", &HookedCreateFileW,
        reinterpret_cast<LPVOID*>(&g_OriginalCreateFileW)) != MH_OK) {
        LogMessage("ERROR: Failed to create CreateFileW hook!");
        MH_Uninitialize();
        return false;
    }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        LogMessage("ERROR: Failed to enable hooks!");
        MH_Uninitialize();
        return false;
    }

    LogMessage("CreateFileW hook installed successfully.");
    return true;
}

void ShutdownFileHooks() {
    LogMessage("Shutting down file hooks...");
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
