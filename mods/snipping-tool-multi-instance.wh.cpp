// ==WindhawkMod==
// @id              snipping-tool-multi-instance
// @name            Snipping Tool Multiple Instances
// @description     Allow running multiple instances of legacy Snipping Tool (snippingtool.exe) instead of focusing existing ones.
// @version         1.0.4.1
// @author          TheShadyRainbow4
// @github          https://github.com/theshadyrainbow4
// @homepage        https://main.elitesoftwaretech.cc
// @include         snippingtool.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
**Snipping Tool Multiple Instances**

This Windhawk mod allows running multiple concurrent instances of the legacy Snipping Tool (`snippingtool.exe`).

### How it works
Normally, when launching `snippingtool.exe` while another instance is already open, the application detects the existing named mutex (`Microsoft-Windows-TabletPC-SnippingTool-InitializingMutex`) or an existing window and brings the running instance to the foreground before exiting.

This mod hooks `CreateMutexW`, `CreateMutexExW`, `OpenMutexW`, `FindWindowW`, and `FindWindowExW` inside `snippingtool.exe` so each newly launched process creates its own independent mutex and window without detecting existing instances.

## Works great when paired with my other mod Snipping tool escape quit!
*/
// ==/WindhawkModReadme==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <windows.h>

#ifndef WH_MOD
#define WH_MOD
#endif
#if __has_include(<windhawk_api.h>)
#include <windhawk_api.h>
#endif
#ifndef WH_MOD_ID
#define WH_MOD_ID L"snipping-tool-multi-instance"
#endif

static bool IsSnippingMutex(LPCWSTR name) {
    if (!name) return false;
    return (wcsstr(name, L"SnippingTool") != nullptr ||
            wcsstr(name, L"InitializingMutex") != nullptr ||
            wcsstr(name, L"Snipper") != nullptr);
}

static bool IsSnippingMutexA(LPCSTR name) {
    if (!name) return false;
    return (strstr(name, "SnippingTool") != nullptr ||
            strstr(name, "InitializingMutex") != nullptr ||
            strstr(name, "Snipper") != nullptr);
}

static bool IsSnippingWindow(LPCWSTR cls, LPCWSTR title) {
    if (cls && (wcsstr(cls, L"Snipper") != nullptr || wcsstr(cls, L"SnippingTool") != nullptr)) return true;
    if (title && wcsstr(title, L"Snipping Tool") != nullptr) return true;
    return false;
}

// ---------------- CreateMutexW Hook ----------------
using CreateMutexW_t = HANDLE(WINAPI*)(LPSECURITY_ATTRIBUTES, BOOL, LPCWSTR);
static CreateMutexW_t pfnCreateMutexW = nullptr;

static HANDLE WINAPI CreateMutexW_Hook(
    LPSECURITY_ATTRIBUTES lpMutexAttributes,
    BOOL bInitialOwner,
    LPCWSTR lpName
) {
    if (IsSnippingMutex(lpName)) {
        Wh_Log(L"Intercepted CreateMutexW for: %ls", lpName);
        return pfnCreateMutexW(lpMutexAttributes, bInitialOwner, nullptr);
    }
    return pfnCreateMutexW(lpMutexAttributes, bInitialOwner, lpName);
}

// ---------------- CreateMutexExW Hook ----------------
using CreateMutexExW_t = HANDLE(WINAPI*)(LPSECURITY_ATTRIBUTES, LPCWSTR, DWORD, DWORD);
static CreateMutexExW_t pfnCreateMutexExW = nullptr;

static HANDLE WINAPI CreateMutexExW_Hook(
    LPSECURITY_ATTRIBUTES lpMutexAttributes,
    LPCWSTR lpName,
    DWORD dwFlags,
    DWORD dwDesiredAccess
) {
    if (IsSnippingMutex(lpName)) {
        Wh_Log(L"Intercepted CreateMutexExW for: %ls", lpName);
        return pfnCreateMutexExW(lpMutexAttributes, nullptr, dwFlags, dwDesiredAccess);
    }
    return pfnCreateMutexExW(lpMutexAttributes, lpName, dwFlags, dwDesiredAccess);
}

// ---------------- OpenMutexW Hook ----------------
using OpenMutexW_t = HANDLE(WINAPI*)(DWORD, BOOL, LPCWSTR);
static OpenMutexW_t pfnOpenMutexW = nullptr;

static HANDLE WINAPI OpenMutexW_Hook(
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    LPCWSTR lpName
) {
    if (IsSnippingMutex(lpName)) {
        Wh_Log(L"Intercepted OpenMutexW for: %ls", lpName);
        SetLastError(ERROR_FILE_NOT_FOUND);
        return nullptr;
    }
    return pfnOpenMutexW(dwDesiredAccess, bInheritHandle, lpName);
}

// ---------------- CreateMutexA Hook ----------------
using CreateMutexA_t = HANDLE(WINAPI*)(LPSECURITY_ATTRIBUTES, BOOL, LPCSTR);
static CreateMutexA_t pfnCreateMutexA = nullptr;

static HANDLE WINAPI CreateMutexA_Hook(
    LPSECURITY_ATTRIBUTES lpMutexAttributes,
    BOOL bInitialOwner,
    LPCSTR lpName
) {
    if (IsSnippingMutexA(lpName)) {
        Wh_Log(L"Intercepted CreateMutexA for: %S", lpName);
        return pfnCreateMutexA(lpMutexAttributes, bInitialOwner, nullptr);
    }
    return pfnCreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
}

// ---------------- CreateMutexExA Hook ----------------
using CreateMutexExA_t = HANDLE(WINAPI*)(LPSECURITY_ATTRIBUTES, LPCSTR, DWORD, DWORD);
static CreateMutexExA_t pfnCreateMutexExA = nullptr;

static HANDLE WINAPI CreateMutexExA_Hook(
    LPSECURITY_ATTRIBUTES lpMutexAttributes,
    LPCSTR lpName,
    DWORD dwFlags,
    DWORD dwDesiredAccess
) {
    if (IsSnippingMutexA(lpName)) {
        Wh_Log(L"Intercepted CreateMutexExA for: %S", lpName);
        return pfnCreateMutexExA(lpMutexAttributes, nullptr, dwFlags, dwDesiredAccess);
    }
    return pfnCreateMutexExA(lpMutexAttributes, lpName, dwFlags, dwDesiredAccess);
}

// ---------------- OpenMutexA Hook ----------------
using OpenMutexA_t = HANDLE(WINAPI*)(DWORD, BOOL, LPCSTR);
static OpenMutexA_t pfnOpenMutexA = nullptr;

static HANDLE WINAPI OpenMutexA_Hook(
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    LPCSTR lpName
) {
    if (IsSnippingMutexA(lpName)) {
        Wh_Log(L"Intercepted OpenMutexA for: %S", lpName);
        SetLastError(ERROR_FILE_NOT_FOUND);
        return nullptr;
    }
    return pfnOpenMutexA(dwDesiredAccess, bInheritHandle, lpName);
}

// ---------------- FindWindowW Hook ----------------
using FindWindowW_t = HWND(WINAPI*)(LPCWSTR, LPCWSTR);
static FindWindowW_t pfnFindWindowW = nullptr;

static HWND WINAPI FindWindowW_Hook(
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName
) {
    if (IsSnippingWindow(lpClassName, lpWindowName)) {
        Wh_Log(L"Intercepted FindWindowW for class: %ls, title: %ls",
               lpClassName ? lpClassName : L"(null)",
               lpWindowName ? lpWindowName : L"(null)");
        SetLastError(ERROR_FILE_NOT_FOUND);
        return nullptr;
    }
    return pfnFindWindowW(lpClassName, lpWindowName);
}

// ---------------- FindWindowExW Hook ----------------
using FindWindowExW_t = HWND(WINAPI*)(HWND, HWND, LPCWSTR, LPCWSTR);
static FindWindowExW_t pfnFindWindowExW = nullptr;

static HWND WINAPI FindWindowExW_Hook(
    HWND hwndParent,
    HWND hwndChildAfter,
    LPCWSTR lpszClass,
    LPCWSTR lpszWindow
) {
    if (IsSnippingWindow(lpszClass, lpszWindow)) {
        Wh_Log(L"Intercepted FindWindowExW for class: %ls, title: %ls",
               lpszClass ? lpszClass : L"(null)",
               lpszWindow ? lpszWindow : L"(null)");
        SetLastError(ERROR_FILE_NOT_FOUND);
        return nullptr;
    }
    return pfnFindWindowExW(hwndParent, hwndChildAfter, lpszClass, lpszWindow);
}

// ---------------- Mod Initialization ----------------
BOOL Wh_ModInit() {
    Wh_Log(L"Snipping Tool Multiple Instances Mod Init");

    Wh_SetFunctionHook((void*)CreateMutexW, (void*)CreateMutexW_Hook, (void**)&pfnCreateMutexW);
    Wh_SetFunctionHook((void*)CreateMutexExW, (void*)CreateMutexExW_Hook, (void**)&pfnCreateMutexExW);
    Wh_SetFunctionHook((void*)OpenMutexW, (void*)OpenMutexW_Hook, (void**)&pfnOpenMutexW);

    Wh_SetFunctionHook((void*)CreateMutexA, (void*)CreateMutexA_Hook, (void**)&pfnCreateMutexA);
    Wh_SetFunctionHook((void*)CreateMutexExA, (void*)CreateMutexExA_Hook, (void**)&pfnCreateMutexExA);
    Wh_SetFunctionHook((void*)OpenMutexA, (void*)OpenMutexA_Hook, (void**)&pfnOpenMutexA);

    Wh_SetFunctionHook((void*)FindWindowW, (void*)FindWindowW_Hook, (void**)&pfnFindWindowW);
    Wh_SetFunctionHook((void*)FindWindowExW, (void*)FindWindowExW_Hook, (void**)&pfnFindWindowExW);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Snipping Tool Multiple Instances Mod Uninit");
}
