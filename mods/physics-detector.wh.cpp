// ==WindhawkMod==
// @id              physics-detector
// @name            Taskbar Auto-Hide Fine Tuner for Flyouts
// @description     Prevents the auto-hiding taskbar from hiding while the cursor is over the panel or while the Quick Settings/Notification flyout is actively open.
// @version         1.0.0
// @author          Zicronium
// @github          https://github.com/Prashant-modder
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Auto-Hide Fine Tuner for Flyouts

This mod improves the stock Windows 11 auto-hide behavior by preventing the taskbar from disappearing under two specific conditions:
1. When the cursor is actively hovering over the primary taskbar workspace.
2. When the Quick Settings or Notification Center flyouts are open (supporting both mouse activation and Win+A / Win+N keyboard shortcuts).

## Compatibility
- Windows 11 only.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;

// Clean, on-demand utility to check if the shell Flyout window is actively rendering on screen
static bool IsShellFlyoutOpen() {
    HWND hFlyout = FindWindowW(L"ControlCenterWindow", nullptr);
    return (hFlyout && IsWindowVisible(hFlyout));
}

// Determines if the mouse cursor is physically resting over the main taskbar container window
static bool IsMouseOverTaskbar() {
    POINT pt;
    if (!GetCursorPos(&pt)) return false;

    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) return false;

    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (!hRoot) return false;

    WCHAR cls[256]; 
    if (GetClassNameW(hRoot, cls, ARRAYSIZE(cls)) > 0) {
        if (wcscmp(cls, L"Shell_TrayWnd") == 0 || 
            wcscmp(cls, L"TopLevelWindowForOverflowXamlIsland") == 0) {
            return true;
        }
    }
    return false;
}

// The core engine interceptor hook
void WINAPI TrayUI__Hide_Hook(void* pThis) {
    // If a flyout is open or the user is interacting with the taskbar, bypass the hide trigger completely
    if (IsShellFlyoutOpen() || IsMouseOverTaskbar()) {
        return; 
    }

    // Otherwise, allow Windows to safely proceed with hiding the bar layout
    TrayUI__Hide_Original(pThis);
}

BOOL Wh_ModInit() {
    HMODULE hTaskbarDll = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hTaskbarDll) {
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK taskbar_dll_hooks[] = {
        {
            {
                LR"(public: virtual void __cdecl TrayUI::_Hide(void))",
                LR"(public: void __cdecl TrayUI::_Hide(void))",
            },
            &TrayUI__Hide_Original,
            TrayUI__Hide_Hook,
            false // Changed to false: This hook is strictly required for operation
        }
    };

    if (!WindhawkUtils::HookSymbols(hTaskbarDll, taskbar_dll_hooks, ARRAYSIZE(taskbar_dll_hooks))) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    // Zero state management required on unload since we dropped background thread allocations entirely
}
