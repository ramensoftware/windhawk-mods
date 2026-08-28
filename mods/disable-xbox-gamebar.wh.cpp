// ==WindhawkMod==
// @id              disable-xbox-gamebar
// @name            Disable Xbox Game Bar
// @description     This mod is configurable to disable the Xbox Game Bar and the Feedback Hub.
// @version         1.0
// @github          https://github.com/Jules1Hase
// @author          Jules_Hase
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Xbox Game Bar

This mod is configurable to disable the Xbox Game Bar and the Feedback Hub.

Currently, it does not hide the dimming effect when opening the Feedback Hub.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- blockGameBar: true
  $name: Block Xbox Game Bar
  $description: Block Xbox Game Bar components (xboxgamingoverlay, gameinput, gamebarservices)
- blockFeedbackHub: true
  $name: Block Feedback Hub
  $description: Block Microsoft Feedback Hub
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <wingdi.h>
#include <winuser.h>

struct {
    BOOL blockGameBar;
    BOOL blockFeedbackHub;
} settings;

static const WCHAR* g_blockedExes[] = {
    L"xboxgamingoverlay",
    L"gameinput",
    L"gamebarservices",
    L"gamebar",
    L"xboxcapture",
    L"gameinputservice",
    L"feedbackhub",
    L"pilotshubapp",
};

static const WCHAR* g_blockedDlls[] = {
    L"xboxgamingoverlay.dll",
    L"gameinput.dll",
    L"gamebar.dll",
    L"xboxcapture.dll",
    L"feedbackhub.dll",
    L"pilotshubapp.dll",
};

static const WCHAR* g_blockedPaths[] = {
    L"microsoft.xboxgamingoverlay",
    L"microsoft gameinput",
    L"gameinput\\x64",
    L"windowsapps\\microsoft.xboxgamingoverlay",
    L"gameinput",
    L"xboxgamingoverlay",
    L"feedbackhub",
    L"windowsapps\\*feedbackhub*",
    L"windowsapps\\microsoft.windowsfeedbackhub",
    L"pilotshubapp",
};

BOOL ShouldBlock(WCHAR* name, BOOL isProcess) {
    if (!name) return FALSE;

    WCHAR nameCopy[MAX_PATH * 2];
    wcscpy_s(nameCopy, name);
    _wcslwr(nameCopy);

    if (settings.blockGameBar) {
        const WCHAR* gamebarNames[] = { L"xboxgamingoverlay", L"gameinput", L"gamebarservices", L"gamebar", L"xboxcapture", L"gameinputservice" };
        for (int i = 0; i < ARRAYSIZE(gamebarNames); i++) {
            if (wcsstr(nameCopy, gamebarNames[i])) return TRUE;
        }
    }

    if (settings.blockFeedbackHub) {
        const WCHAR* feedbackNames[] = { L"feedbackhub", L"feedback", L"pilotshubapp", L"pilotshub" };
        for (int i = 0; i < ARRAYSIZE(feedbackNames); i++) {
            if (wcsstr(nameCopy, feedbackNames[i])) return TRUE;
        }
    }

    return FALSE;
}

BOOL IsBlockedProcess(WCHAR* processName) {
    if (!processName) return FALSE;
    WCHAR nameCopy[MAX_PATH];
    wcscpy_s(nameCopy, processName);
    _wcslwr(nameCopy);

    for (int i = 0; i < ARRAYSIZE(g_blockedExes); i++) {
        if (wcsstr(nameCopy, g_blockedExes[i])) {
            return ShouldBlock(processName, TRUE);
        }
    }
    return FALSE;
}

BOOL IsBlockedDll(WCHAR* dllName) {
    if (!dllName) return FALSE;
    WCHAR nameCopy[MAX_PATH];
    wcscpy_s(nameCopy, dllName);
    _wcslwr(nameCopy);

    for (int i = 0; i < ARRAYSIZE(g_blockedDlls); i++) {
        if (wcsstr(nameCopy, g_blockedDlls[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL IsBlockedPath(WCHAR* fullPath) {
    if (!fullPath) return FALSE;
    
    WCHAR pathCopy[MAX_PATH * 2];
    wcscpy_s(pathCopy, fullPath);
    _wcslwr(pathCopy);

    for (int i = 0; i < ARRAYSIZE(g_blockedPaths); i++) {
        if (wcsstr(pathCopy, g_blockedPaths[i])) {
            if (wcsstr(pathCopy, L"feedbackhub") || wcsstr(pathCopy, L"feedback")) {
                return ShouldBlock(fullPath, FALSE);
            }
            if (wcsstr(pathCopy, L"gameinput") || wcsstr(pathCopy, L"xboxgamingoverlay") || wcsstr(pathCopy, L"gamebar")) {
                return ShouldBlock(fullPath, FALSE);
            }
            return TRUE;
        }
    }
    return FALSE;
}

void KillGameBarProcesses() {
    if (!settings.blockGameBar && !settings.blockFeedbackHub) {
        return;
    }

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    PROCESSENTRY32W pe32 = {0};
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return;
    }

    do {
        if (IsBlockedProcess(pe32.szExeFile)) {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ProcessID);
            if (hProcess) {
                Wh_Log(L"Terminating: %s (PID: %u)", pe32.szExeFile, pe32.th32ProcessID);
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
        }
    } while (Process32NextW(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
}

using LoadLibraryW_t = HMODULE(WINAPI*)(LPCWSTR);
LoadLibraryW_t LoadLibraryW_Original;

HMODULE WINAPI LoadLibraryW_Hook(LPCWSTR lpLibFileName) {
    if (lpLibFileName && IsBlockedDll((WCHAR*)lpLibFileName)) {
        Wh_Log(L"Blocking LoadLibraryW: %ls", lpLibFileName);
        SetLastError(ERROR_DLL_NOT_FOUND);
        return NULL;
    }
    if (lpLibFileName && IsBlockedPath((WCHAR*)lpLibFileName)) {
        Wh_Log(L"Blocking LoadLibraryW (path): %ls", lpLibFileName);
        SetLastError(ERROR_DLL_NOT_FOUND);
        return NULL;
    }
    return LoadLibraryW_Original(lpLibFileName);
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    if (lpLibFileName && IsBlockedDll((WCHAR*)lpLibFileName)) {
        Wh_Log(L"Blocking LoadLibraryExW: %ls", lpLibFileName);
        SetLastError(ERROR_DLL_NOT_FOUND);
        return NULL;
    }
    if (lpLibFileName && IsBlockedPath((WCHAR*)lpLibFileName)) {
        Wh_Log(L"Blocking LoadLibraryExW (path): %ls", lpLibFileName);
        SetLastError(ERROR_DLL_NOT_FOUND);
        return NULL;
    }
    return LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
}

using LoadLibraryA_t = HMODULE(WINAPI*)(LPCSTR);
LoadLibraryA_t LoadLibraryA_Original;

HMODULE WINAPI LoadLibraryA_Hook(LPCSTR lpLibFileName) {
    if (lpLibFileName) {
        char nameCopy[MAX_PATH];
        strcpy_s(nameCopy, lpLibFileName);
        _strlwr(nameCopy);

        for (int i = 0; i < ARRAYSIZE(g_blockedDlls); i++) {
            char dllName[MAX_PATH];
            wcstombs(dllName, g_blockedDlls[i], MAX_PATH);
            if (strstr(nameCopy, dllName)) {
                Wh_Log(L"Blocking LoadLibraryA: %S", lpLibFileName);
                SetLastError(ERROR_DLL_NOT_FOUND);
                return NULL;
            }
        }
    }
    return LoadLibraryA_Original(lpLibFileName);
}

using CreateProcessW_t = BOOL(WINAPI*)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
CreateProcessW_t CreateProcessW_Original;

BOOL WINAPI CreateProcessW_Hook(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags,
                                LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
                                LPPROCESS_INFORMATION lpProcessInformation) {
    WCHAR searchPath[MAX_PATH * 2] = {0};

    if (lpApplicationName && IsBlockedPath((WCHAR*)lpApplicationName)) {
        Wh_Log(L"Blocking CreateProcessW (app): %ls", lpApplicationName);
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }

    if (lpApplicationName && IsBlockedProcess((WCHAR*)lpApplicationName)) {
        Wh_Log(L"Blocking CreateProcessW (name): %ls", lpApplicationName);
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }

    if (lpCommandLine) {
        WCHAR cmdLineCopy[32768];
        wcscpy_s(cmdLineCopy, lpCommandLine);
        WCHAR* p = cmdLineCopy;
        WCHAR* context = NULL;
        
        WCHAR* token = wcstok_s(p, L" \t\"", &context);
        while (token) {
            if (IsBlockedPath(token) || IsBlockedProcess(token)) {
                Wh_Log(L"Blocking CreateProcessW (cmdline): %ls", token);
                SetLastError(ERROR_ACCESS_DENIED);
                return FALSE;
            }

            if (GetFullPathNameW(token, ARRAYSIZE(searchPath), searchPath, NULL) && IsBlockedPath(searchPath)) {
                Wh_Log(L"Blocking CreateProcessW (path): %ls", searchPath);
                SetLastError(ERROR_ACCESS_DENIED);
                return FALSE;
            }

            token = wcstok_s(NULL, L" \t\"", &context);
        }
    }

    return CreateProcessW_Original(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                                   bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                                   lpStartupInfo, lpProcessInformation);
}

using CreateProcessAsUserW_t = BOOL(WINAPI*)(HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
CreateProcessAsUserW_t CreateProcessAsUserW_Original;

BOOL WINAPI CreateProcessAsUserW_Hook(HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
                                      LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                      BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
                                      LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
                                      LPPROCESS_INFORMATION lpProcessInformation) {
    if (lpApplicationName && (IsBlockedPath((WCHAR*)lpApplicationName) || IsBlockedProcess((WCHAR*)lpApplicationName))) {
        Wh_Log(L"Blocking CreateProcessAsUserW: %ls", lpApplicationName);
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }

    return CreateProcessAsUserW_Original(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes,
                                        lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
                                        lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

using ShellExecuteExW_t = BOOL(WINAPI*)(LPSHELLEXECUTEINFOW);
ShellExecuteExW_t ShellExecuteExW_Original;

BOOL WINAPI ShellExecuteExW_Hook(LPSHELLEXECUTEINFOW lpExecInfo) {
    if (lpExecInfo && lpExecInfo->lpFile) {
        WCHAR fileCopy[MAX_PATH * 2];
        wcscpy_s(fileCopy, lpExecInfo->lpFile);
        _wcslwr(fileCopy);

        if (IsBlockedPath(fileCopy) || IsBlockedProcess(fileCopy)) {
            Wh_Log(L"Blocking ShellExecuteExW: %ls", fileCopy);
            SetLastError(ERROR_ACCESS_DENIED);
            return FALSE;
        }
    }

    return ShellExecuteExW_Original(lpExecInfo);
}

using WinExec_t = UINT(WINAPI*)(LPCSTR, UINT);
WinExec_t WinExec_Original;

UINT WINAPI WinExec_Hook(LPCSTR lpCmdLine, UINT uCmdShow) {
    if (lpCmdLine) {
        char cmdLineCopy[32768];
        strcpy_s(cmdLineCopy, lpCmdLine);
        _strlwr(cmdLineCopy);

        for (int i = 0; i < ARRAYSIZE(g_blockedExes); i++) {
            char exeName[MAX_PATH];
            wcstombs(exeName, g_blockedExes[i], MAX_PATH);
            if (strstr(cmdLineCopy, exeName)) {
                Wh_Log(L"Blocking WinExec: %S", lpCmdLine);
                SetLastError(ERROR_ACCESS_DENIED);
                return 0;
            }
        }
    }

    return WinExec_Original(lpCmdLine, uCmdShow);
}

using RegOpenKeyExW_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR, DWORD, REGSAM, HKEY*);
RegOpenKeyExW_t RegOpenKeyExW_Original;

LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, HKEY* phkResult) {
    if (lpSubKey) {
        WCHAR subKeyCopy[512];
        wcscpy_s(subKeyCopy, lpSubKey);
        _wcslwr(subKeyCopy);

        if (wcsstr(subKeyCopy, L"gamebar") || wcsstr(subKeyCopy, L"xbox") || 
            wcsstr(subKeyCopy, L"gameinput") || wcsstr(subKeyCopy, L"gamingoverlay") ||
            wcsstr(subKeyCopy, L"feedbackhub") || wcsstr(subKeyCopy, L"feedback")) {
            Wh_Log(L"Blocking registry open: %ls", lpSubKey);
            if (phkResult) *phkResult = NULL;
            return ERROR_ACCESS_DENIED;
        }
    }

    return RegOpenKeyExW_Original(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

using RegCreateKeyExW_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR, DWORD, LPWSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, HKEY*, LPDWORD);
RegCreateKeyExW_t RegCreateKeyExW_Original;

LSTATUS WINAPI RegCreateKeyExW_Hook(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions,
                                   REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, HKEY* phkResult,
                                   LPDWORD lpdwDisposition) {
    if (lpSubKey) {
        WCHAR subKeyCopy[512];
        wcscpy_s(subKeyCopy, lpSubKey);
        _wcslwr(subKeyCopy);

        if (wcsstr(subKeyCopy, L"gamebar") || wcsstr(subKeyCopy, L"xbox") ||
            wcsstr(subKeyCopy, L"gameinput") || wcsstr(subKeyCopy, L"gamingoverlay") ||
            wcsstr(subKeyCopy, L"feedbackhub") || wcsstr(subKeyCopy, L"feedback")) {
            Wh_Log(L"Blocking registry create: %ls", lpSubKey);
            if (phkResult) *phkResult = NULL;
            return ERROR_ACCESS_DENIED;
        }
    }

    return RegCreateKeyExW_Original(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired,
                                    lpSecurityAttributes, phkResult, lpdwDisposition);
}

using RegisterHotKey_t = BOOL(WINAPI*)(int, UINT, UINT, UINT);
RegisterHotKey_t RegisterHotKey_Original;

using SetWindowsHookExW_t = HHOOK(WINAPI*)(int, HOOKPROC, HINSTANCE, DWORD);
SetWindowsHookExW_t SetWindowsHookExW_Original;

using SetWindowsHookExA_t = HHOOK(WINAPI*)(int, HOOKPROC, HINSTANCE, DWORD);
SetWindowsHookExA_t SetWindowsHookExA_Original;

using AttachThreadInput_t = BOOL(WINAPI*)(DWORD, DWORD, BOOL);
AttachThreadInput_t AttachThreadInput_Original;

using RegisterRawInputDevices_t = BOOL(WINAPI*)(PCRAWINPUTDEVICE, UINT, UINT);
RegisterRawInputDevices_t RegisterRawInputDevices_Original;

using SendInput_t = UINT(WINAPI*)(int, LPINPUT, int);
SendInput_t SendInput_Original;

using GetAsyncKeyState_t = SHORT(WINAPI*)(int);
GetAsyncKeyState_t GetAsyncKeyState_Original;

using GetKeyState_t = SHORT(WINAPI*)(int);
GetKeyState_t GetKeyState_Original;

using GetKeyboardState_t = BOOL(WINAPI*)(PBYTE);
GetKeyboardState_t GetKeyboardState_Original;

using PeekMessageW_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT, UINT);
PeekMessageW_t PeekMessageW_Original;

using GetMessageW_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT);
GetMessageW_t GetMessageW_Original;

using TranslateMessage_t = BOOL(WINAPI*)(const MSG*);
TranslateMessage_t TranslateMessage_Original;

using DispatchMessageW_t = LRESULT(WINAPI*)(const MSG*);
DispatchMessageW_t DispatchMessageW_Original;

BOOL WINAPI RegisterHotKey_Hook(int hWnd, UINT id, UINT fsModifiers, UINT vk) {
    if (vk == 0x47 && (fsModifiers == MOD_WIN || fsModifiers == (MOD_NOREPEAT | MOD_WIN))) {
        if (settings.blockGameBar) {
            Wh_Log(L"Blocking Win+G hotkey (Game Bar blocked)");
            SetLastError(ERROR_ACCESS_DENIED);
            return FALSE;
        }
    }
    if (vk == 0x58 && fsModifiers == (MOD_NOREPEAT | MOD_WIN | MOD_ALT)) {
        if (settings.blockGameBar) {
            Wh_Log(L"Blocking Win+Alt+X hotkey (Game Bar blocked)");
            SetLastError(ERROR_ACCESS_DENIED);
            return FALSE;
        }
    }
    if (vk == 0x46 && (fsModifiers == MOD_WIN || fsModifiers == (MOD_NOREPEAT | MOD_WIN))) {
        if (settings.blockFeedbackHub) {
            Wh_Log(L"Blocking Win+F hotkey (Feedback Hub blocked)");
            SetLastError(ERROR_ACCESS_DENIED);
            return FALSE;
        }
    }
    return RegisterHotKey_Original(hWnd, id, fsModifiers, vk);
}

HHOOK WINAPI SetWindowsHookExW_Hook(int idHook, HOOKPROC lpfn, HINSTANCE hMod, DWORD dwThreadId) {
    Wh_Log(L"SetWindowsHookExW: idHook=%d, threadId=%u", idHook, dwThreadId);
    return SetWindowsHookExW_Original(idHook, lpfn, hMod, dwThreadId);
}

HHOOK WINAPI SetWindowsHookExA_Hook(int idHook, HOOKPROC lpfn, HINSTANCE hMod, DWORD dwThreadId) {
    Wh_Log(L"SetWindowsHookExA: idHook=%d, threadId=%u", idHook, dwThreadId);
    return SetWindowsHookExA_Original(idHook, lpfn, hMod, dwThreadId);
}

BOOL WINAPI AttachThreadInput_Hook(DWORD idAttach, DWORD idAttachTo, BOOL fAttach) {
    Wh_Log(L"AttachThreadInput: attach=%u to=%u attach=%d", idAttach, idAttachTo, fAttach);
    return AttachThreadInput_Original(idAttach, idAttachTo, fAttach);
}

BOOL WINAPI RegisterRawInputDevices_Hook(PCRAWINPUTDEVICE pDevices, UINT uiDevices, UINT cbSize) {
    Wh_Log(L"RegisterRawInputDevices: devices=%u, size=%u", uiDevices, cbSize);
    if (pDevices) {
        for (UINT i = 0; i < uiDevices; i++) {
            Wh_Log(L"  Device[%u]: usUsagePage=0x%X, usUsage=0x%X, dwFlags=0x%X",
                   i, pDevices[i].usUsagePage, pDevices[i].usUsage, pDevices[i].dwFlags);
        }
    }
    return RegisterRawInputDevices_Original(pDevices, uiDevices, cbSize);
}

UINT WINAPI SendInput_Hook(int nInputs, LPINPUT pInputs, int cbSize) {
    return SendInput_Original(nInputs, pInputs, cbSize);
}

SHORT WINAPI GetAsyncKeyState_Hook(int vKey) {
    return GetAsyncKeyState_Original(vKey);
}

SHORT WINAPI GetKeyState_Hook(int vKey) {
    return GetKeyState_Original(vKey);
}

BOOL WINAPI GetKeyboardState_Hook(PBYTE lpKeyState) {
    return GetKeyboardState_Original(lpKeyState);
}

BOOL WINAPI PeekMessageW_Hook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
    BOOL result = PeekMessageW_Original(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

    if (result && lpMsg && settings.blockFeedbackHub) {
        if (lpMsg->message == WM_HOTKEY) {
            UINT vk = HIWORD(lpMsg->lParam);
            UINT mods = LOWORD(lpMsg->lParam);
            Wh_Log(L"PeekMessageW: WM_HOTKEY vk=0x%X, mods=0x%X, hWnd=0x%X", vk, mods, (DWORD)(ULONG_PTR)lpMsg->hwnd);

            if (vk == 0x46 && (mods & MOD_WIN)) {
                Wh_Log(L"PeekMessageW: Blocking Win+F hotkey");
                lpMsg->message = WM_NULL;
                lpMsg->wParam = 0;
                lpMsg->lParam = 0;
                return TRUE;
            }
        }
    }

    return result;
}

BOOL WINAPI GetMessageW_Hook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax) {
    BOOL result = GetMessageW_Original(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);

    if (result && lpMsg && lpMsg->message != WM_NULL && settings.blockFeedbackHub) {
        if (lpMsg->message == WM_HOTKEY) {
            UINT vk = HIWORD(lpMsg->lParam);
            UINT mods = LOWORD(lpMsg->lParam);

            if (vk == 0x46 && (mods & MOD_WIN)) {
                Wh_Log(L"GetMessageW: Blocking Win+F hotkey");
                lpMsg->message = WM_NULL;
                lpMsg->wParam = 0;
                lpMsg->lParam = 0;
                return TRUE;
            }
        }
    }

    return result;
}

BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    if (lpMsg && settings.blockFeedbackHub) {
        if (lpMsg->message == WM_HOTKEY) {
            UINT vk = HIWORD(lpMsg->lParam);
            UINT mods = LOWORD(lpMsg->lParam);

            if (vk == 0x46 && (mods & MOD_WIN)) {
                Wh_Log(L"TranslateMessage: Blocking Win+F hotkey");
                return TRUE;
            }
        }
    }

    return TranslateMessage_Original(lpMsg);
}

LRESULT WINAPI DispatchMessageW_Hook(const MSG* lpMsg) {
    if (lpMsg && settings.blockFeedbackHub) {
        if (lpMsg->message == WM_HOTKEY) {
            UINT vk = HIWORD(lpMsg->lParam);
            UINT mods = LOWORD(lpMsg->lParam);

            if (vk == 0x46 && (mods & MOD_WIN)) {
                Wh_Log(L"DispatchMessageW: Blocking Win+F hotkey");
                return 0;
            }
        }
    }

    return DispatchMessageW_Original(lpMsg);
}

using UnregisterHotKey_t = BOOL(WINAPI*)(int, UINT);
UnregisterHotKey_t UnregisterHotKey_Original;

BOOL WINAPI UnregisterHotKey_Hook(int hWnd, UINT id) {
    return UnregisterHotKey_Original(hWnd, id);
}

using RegSetValueExW_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR, DWORD, DWORD, const BYTE*, DWORD);
RegSetValueExW_t RegSetValueExW_Original;

LSTATUS WINAPI RegSetValueExW_Hook(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE* lpData, DWORD cbData) {
    if (lpValueName) {
        WCHAR nameCopy[256];
        wcscpy_s(nameCopy, lpValueName);
        _wcslwr(nameCopy);

        if (wcsstr(nameCopy, L"gamebar") || wcsstr(nameCopy, L"xbox") ||
            wcsstr(nameCopy, L"gamingoverlay") || wcsstr(nameCopy, L"gameinput")) {
            Wh_Log(L"Blocking RegSetValueEx: %ls", lpValueName);
            return ERROR_ACCESS_DENIED;
        }
    }
    return RegSetValueExW_Original(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

using RegDeleteKeyW_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR);
RegDeleteKeyW_t RegDeleteKeyW_Original;

using RegDeleteValueW_t = LSTATUS(WINAPI*)(HKEY, LPCWSTR);
RegDeleteValueW_t RegDeleteValueW_Original;

using SHGetPropertyStoreForWindow_t = HRESULT(WINAPI*)(HWND, REFIID, void**);
SHGetPropertyStoreForWindow_t SHGetPropertyStoreForWindow_Original;

void Wh_ModInit() {
    Wh_Log(L"Disable Xbox Game Bar initialized");

    settings.blockGameBar = Wh_GetIntSetting(L"blockGameBar");
    settings.blockFeedbackHub = Wh_GetIntSetting(L"blockFeedbackHub");

    Wh_Log(L"Settings: blockGameBar=%d, blockFeedbackHub=%d", settings.blockGameBar, settings.blockFeedbackHub);

    Wh_SetFunctionHook((void*)LoadLibraryW, (void*)LoadLibraryW_Hook, (void**)&LoadLibraryW_Original);
    Wh_SetFunctionHook((void*)LoadLibraryExW, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
    Wh_SetFunctionHook((void*)LoadLibraryA, (void*)LoadLibraryA_Hook, (void**)&LoadLibraryA_Original);
    Wh_SetFunctionHook((void*)CreateProcessW, (void*)CreateProcessW_Hook, (void**)&CreateProcessW_Original);
    Wh_SetFunctionHook((void*)CreateProcessAsUserW, (void*)CreateProcessAsUserW_Hook, (void**)&CreateProcessAsUserW_Original);
    Wh_SetFunctionHook((void*)ShellExecuteExW, (void*)ShellExecuteExW_Hook, (void**)&ShellExecuteExW_Original);
    Wh_SetFunctionHook((void*)WinExec, (void*)WinExec_Hook, (void**)&WinExec_Original);
    Wh_SetFunctionHook((void*)RegOpenKeyExW, (void*)RegOpenKeyExW_Hook, (void**)&RegOpenKeyExW_Original);
    Wh_SetFunctionHook((void*)RegCreateKeyExW, (void*)RegCreateKeyExW_Hook, (void**)&RegCreateKeyExW_Original);
    Wh_SetFunctionHook((void*)RegSetValueExW, (void*)RegSetValueExW_Hook, (void**)&RegSetValueExW_Original);
    Wh_SetFunctionHook((void*)RegisterHotKey, (void*)RegisterHotKey_Hook, (void**)&RegisterHotKey_Original);
    Wh_SetFunctionHook((void*)UnregisterHotKey, (void*)UnregisterHotKey_Hook, (void**)&UnregisterHotKey_Original);
    Wh_SetFunctionHook((void*)SetWindowsHookExW, (void*)SetWindowsHookExW_Hook, (void**)&SetWindowsHookExW_Original);
    Wh_SetFunctionHook((void*)SetWindowsHookExA, (void*)SetWindowsHookExA_Hook, (void**)&SetWindowsHookExA_Original);
    Wh_SetFunctionHook((void*)AttachThreadInput, (void*)AttachThreadInput_Hook, (void**)&AttachThreadInput_Original);
    Wh_SetFunctionHook((void*)RegisterRawInputDevices, (void*)RegisterRawInputDevices_Hook, (void**)&RegisterRawInputDevices_Original);
    Wh_SetFunctionHook((void*)SendInput, (void*)SendInput_Hook, (void**)&SendInput_Original);
    Wh_SetFunctionHook((void*)GetAsyncKeyState, (void*)GetAsyncKeyState_Hook, (void**)&GetAsyncKeyState_Original);
    Wh_SetFunctionHook((void*)GetKeyState, (void*)GetKeyState_Hook, (void**)&GetKeyState_Original);
    Wh_SetFunctionHook((void*)GetKeyboardState, (void*)GetKeyboardState_Hook, (void**)&GetKeyboardState_Original);
    Wh_SetFunctionHook((void*)PeekMessageW, (void*)PeekMessageW_Hook, (void**)&PeekMessageW_Original);
    Wh_SetFunctionHook((void*)GetMessageW, (void*)GetMessageW_Hook, (void**)&GetMessageW_Original);
    Wh_SetFunctionHook((void*)TranslateMessage, (void*)TranslateMessage_Hook, (void**)&TranslateMessage_Original);
    Wh_SetFunctionHook((void*)DispatchMessageW, (void*)DispatchMessageW_Hook, (void**)&DispatchMessageW_Original);

    KillGameBarProcesses();
}

void Wh_ModUninit() {
    Wh_Log(L"Disable Xbox Game Bar uninitialized");
}

void Wh_ModSettingsChanged() {
    settings.blockGameBar = Wh_GetIntSetting(L"blockGameBar");
    settings.blockFeedbackHub = Wh_GetIntSetting(L"blockFeedbackHub");

    Wh_Log(L"Settings changed: blockGameBar=%d, blockFeedbackHub=%d", settings.blockGameBar, settings.blockFeedbackHub);

    KillGameBarProcesses();
}