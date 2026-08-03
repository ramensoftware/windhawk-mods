// ==WindhawkMod==
// @id              taskbar-auto-hide-flyouts
// @name            Taskbar Auto-Hide Flyout Fix
// @description     Prevents the auto-hiding taskbar from disappearing while the Quick Settings, Notification Center, or calendar flyouts are open.
// @version         1.0.0
// @author          Zicronium
// @github          https://github.com
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -ldwmapi
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

// ---------------------------------------------------------------------------
// Advanced Flyout State Analysis (DWMWA_CLOAKED verification)
// ---------------------------------------------------------------------------
static bool IsTargetWindowActive(HWND hWnd) {
    if (!hWnd || !IsWindowVisible(hWnd)) return false;
    
    BOOL cloaked = FALSE;
    HRESULT hr = DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (SUCCEEDED(hr) && cloaked) {
        return false; // Window is rendered but hidden/cloaked by the OS shell
    }
    return true; 
}

static bool AreShellFlyoutsOpen() {
    // Check 24H2+ architecture (ShellHost modern panels)
    HWND hFlyout = nullptr;
    while ((hFlyout = FindWindowExW(nullptr, hFlyout, L"ControlCenterWindow", nullptr)) != nullptr) {
        if (IsTargetWindowActive(hFlyout)) return true;
    }
    
    // Check 21H2-23H2 legacy architecture (ShellExperienceHost generic UWP panels)
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
// Legcy Hook Layer: TrayUI Hiding Controls
// ---------------------------------------------------------------------------
using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;

static VOID CALLBACK RearmTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    if (!AreShellFlyoutsOpen()) {
        KillTimer(hWnd, TIMER_REARM_ID);
        // Safely trigger standard system taskbar hide sequence dispatch
        SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);
    }
}

void WINAPI TrayUI__Hide_Hook(void* pThis) {
    if (AreShellFlyoutsOpen()) {
        HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (hTaskbar) {
            // Set a low-overhead tracking loop to re-arm hiding once panels close safely
            SetTimer(hTaskbar, TIMER_REARM_ID, TIMER_POLL_INTERVAL, RearmTimerProc);
        }
        return; // Suppress legacy hide invocation frame
    }
    TrayUI__Hide_Original(pThis);
}

// ---------------------------------------------------------------------------
// Modern Win11 Hook Layer: ViewCoordinator Layout Suppression
// ---------------------------------------------------------------------------
using UpdateIsExpanded_t = void(__cdecl*)(void* pThis, bool isExpanded);
UpdateIsExpanded_t UpdateIsExpanded_Original;

void __cdecl UpdateIsExpanded_Hook(void* pThis, bool isExpanded) {
    if (!isExpanded && AreShellFlyoutsOpen()) {
        // Force layout engine to retain active/expanded visibility state parameters
        UpdateIsExpanded_Original(pThis, true);
        
        HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (hTaskbar) {
            SetTimer(hTaskbar, TIMER_REARM_ID, TIMER_POLL_INTERVAL, RearmTimerProc);
        }
        return;
    }
    UpdateIsExpanded_Original(pThis, isExpanded);
}

// Runtime loader hook to safely trap dynamic runtime module streaming parameters
using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hMod = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (hMod && lpLibFileName && (wcsstr(lpLibFileName, L"Taskbar.View.dll") || wcsstr(lpLibFileName, L"ExplorerExtensions.dll"))) {
        WindhawkUtils::SYMBOL_HOOK viewHooks[] = {
            {
                { LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(bool))" },
                &UpdateIsExpanded_Original,
                UpdateIsExpanded_Hook,
                true // True because dynamic targets might contain varying decoration signatures across updates
            }
        };
        WindhawkUtils::HookSymbols(hMod, viewHooks, ARRAYSIZE(viewHooks));
    }
    return hMod;
}

// ---------------------------------------------------------------------------
// Mod Framework Initializer Context
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    HMODULE hTaskbarDll = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hTaskbarDll) {
        WindhawkUtils::SYMBOL_HOOK legacyHooks[] = {
            {
                {
                    LR"(public: virtual void __cdecl TrayUI::_Hide(void))",
                    LR"(public: void __cdecl TrayUI::_Hide(void))",
                },
                &TrayUI__Hide_Original,
                TrayUI__Hide_Hook,
                false // Required asset tracking anchor
            }
        };
        WindhawkUtils::HookSymbols(hTaskbarDll, legacyHooks, ARRAYSIZE(legacyHooks));
    }

    // Attach dynamic module loader hooks to securely track late-loaded modern layout dependencies
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        WindhawkUtils::SYMBOL_HOOK loaderHooks[] = {
            {
                { LR"(lSystem.LoadLibraryExW)" },
                &LoadLibraryExW_Original,
                LoadLibraryExW_Hook,
                true
            }
        };
        WindhawkUtils::HookSymbols(hKernelBase, loaderHooks, ARRAYSIZE(loaderHooks));
    }

    return TRUE;
}

void Wh_ModUninit() {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTaskbar) {
        KillTimer(hTaskbar, TIMER_REARM_ID);
    }
}
