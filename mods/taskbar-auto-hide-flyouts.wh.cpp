// ==WindhawkMod==
// @id              taskbar-auto-hide-flyouts
// @name            Taskbar Auto-Hide Flyout Fix
// @description     Prevents the auto-hiding taskbar from disappearing while the Quick Settings, Notification Center, or calendar flyouts are open.
// @version         1.0.0
// @author          Zicronium
// @github          https://github.com/Prashant-modder
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -ldwmapi -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Auto-Hide Flyout Fix

This mod ensures that the Windows 11 auto-hiding taskbar stays reliably visible on screen whenever the system flyouts (Quick Settings, Notification Center, or Calendar) are active. 

It handles mouse actions as well as Win+A / Win+N hotkeys gracefully by monitoring window cloaking states and intercepting both the legacy TrayUI system and the modern WinRT ViewCoordinator layout controllers.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <dwmapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#define TIMER_REARM_ID 8821
#define TIMER_POLL_INTERVAL 250
#define kTrayUITimerHide 2

static HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTaskbar) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hTaskbar, &pid);
        if (pid == GetCurrentProcessId()) {
            return hTaskbar;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Advanced Flyout State Analysis (DWMWA_CLOAKED verification)
// ---------------------------------------------------------------------------
static bool IsTargetWindowActive(HWND hWnd) {
    if (!hWnd || !IsWindowVisible(hWnd)) return false;
    
    BOOL cloaked = FALSE;
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
        cloaked = FALSE;
    }
    return !cloaked; 
}

static bool AreShellFlyoutsOpen() {
    HWND hFlyout = nullptr;
    while ((hFlyout = FindWindowExW(nullptr, hFlyout, L"ControlCenterWindow", nullptr)) != nullptr) {
        if (IsTargetWindowActive(hFlyout)) return true;
    }
    
    HWND hUwpWindow = nullptr;
    while ((hUwpWindow = FindWindowExW(nullptr, hUwpWindow, L"Windows.UI.Core.CoreWindow", nullptr)) != nullptr) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hUwpWindow, &pid);
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProc) {
            WCHAR imgName[MAX_PATH];
            DWORD len = ARRAYSIZE(imgName);
            if (QueryFullProcessImageNameW(hProc, 0, imgName, &len)) {
                if (wcsstr(imgName, L"ShellExperienceHost.exe") != nullptr) {
                    if (IsTargetWindowActive(hUwpWindow)) {
                        CloseHandle(hProc);
                        return true;
                    }
                }
            }
            CloseHandle(hProc);
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Hook Layers: Subclass-Safe Rearming Timers
// ---------------------------------------------------------------------------
using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;

LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_TIMER && wParam == TIMER_REARM_ID) {
        if (!AreShellFlyoutsOpen()) {
            KillTimer(hWnd, TIMER_REARM_ID);
            SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);
        }
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void WINAPI TrayUI__Hide_Hook(void* pThis) {
    if (AreShellFlyoutsOpen()) {
        HWND hTaskbar = FindCurrentProcessTaskbarWnd();
        if (hTaskbar) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(hTaskbar, TaskbarSubclassProc, TIMER_REARM_ID);
            SetTimer(hTaskbar, TIMER_REARM_ID, TIMER_POLL_INTERVAL, nullptr);
        }
        return; 
    }
    TrayUI__Hide_Original(pThis);
}

using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool hide);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;

void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool hide) {
    if (hide && AreShellFlyoutsOpen()) {
        return;
    }
    CSecondaryTray__AutoHide_Original(pThis, hide);
}

// ---------------------------------------------------------------------------
// Modern Win11 Hook Layer: ViewCoordinator Layout Suppression
// ---------------------------------------------------------------------------
using ShouldTaskbarBeExpanded_t = bool(__cdecl*)(void* pThis);
ShouldTaskbarBeExpanded_t ShouldTaskbarBeExpanded_Original;

bool __cdecl ShouldTaskbarBeExpanded_Hook(void* pThis) {
    if (AreShellFlyoutsOpen()) {
        return true; 
    }
    return ShouldTaskbarBeExpanded_Original(pThis);
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hMod = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (hMod && lpLibFileName && (wcsstr(lpLibFileName, L"Taskbar.View.dll") || wcsstr(lpLibFileName, L"ExplorerExtensions.dll"))) {
        // Taskbar.View.dll
        WindhawkUtils::SYMBOL_HOOK hooks[] = {
            {
                { LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(void))" },
                &ShouldTaskbarBeExpanded_Original,
                ShouldTaskbarBeExpanded_Hook,
                true 
            }
        };
        WindhawkUtils::HookSymbols(hMod, hooks, ARRAYSIZE(hooks));
    }
    return hMod;
}

// ---------------------------------------------------------------------------
// Mod Framework Initializer Context
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    HMODULE hTaskbarDll = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hTaskbarDll) {
        // taskbar.dll
        WindhawkUtils::SYMBOL_HOOK hooks[] = {
            {
                { LR"(public: virtual void __cdecl TrayUI::_Hide(void))" },
                &TrayUI__Hide_Original,
                TrayUI__Hide_Hook,
                false 
            },
            {
                { LR"(public: void __cdecl CSecondaryTray::_AutoHide(bool))" },
                &CSecondaryTray__AutoHide_Original,
                CSecondaryTray__AutoHide_Hook,
                true
            }
        };
        WindhawkUtils::HookSymbols(hTaskbarDll, hooks, ARRAYSIZE(hooks));
    }

    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(hKernelBase, "LoadLibraryExW");
        if (pLoadLibraryExW) {
            WindhawkUtils::SetFunctionHook((void*)pLoadLibraryExW, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
        }
    }

    return TRUE;
}

void Wh_ModUninit() {
    HWND hTaskbar = FindCurrentProcessTaskbarWnd();
    if (hTaskbar) {
        KillTimer(hTaskbar, TIMER_REARM_ID);
        // FIX: Provided TaskbarSubclassProc to cleanly match the utility signature
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hTaskbar, TaskbarSubclassProc);
        SetTimer(hTaskbar, kTrayUITimerHide, 0, nullptr);
    }
}
