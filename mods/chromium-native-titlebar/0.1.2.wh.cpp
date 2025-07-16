// ==WindhawkMod==
// @id              chromium-native-titlebar
// @name            Native titelebars in Chromium-based browsers
// @description     Restore native titlebars in Edge and Chrome
// @version         0.1.2
// @author          Anixx
// @github          https://github.com/Anixx
// @include         chrome.exe
// @include         msedge.exe
// @include         chromium.exe
// @include         supermium.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
**! IMPORTANT !** This mod is outdated. It is recommended to use the mod
**Titlebar For Everyone** by Ingran121 instead in all cases.

This is a variation of the Chromium Frame Blackout Fix mod by Ingan121.
The difference from the original mod is in intercepting also WM_NCCALCSIZE in addition
to WM_NCPAINT message.
Since the effect of this mod is unpredictable on other applications, by default only
some Chromium-based browsers are injected.

![Edge](https://i.imgur.com/DQGNOtH.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>

LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if ((uMsg == WM_NCPAINT) || (uMsg == WM_NCCALCSIZE))  {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK InitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    // Subclass all relevant windows belonging to this process
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);
        if (wcsncmp(className, L"Chrome_WidgetWin_", 17) == 0) {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, 1)) {
                Wh_Log(L"Subclassed %p", hWnd);
            }
        }
    }
    return TRUE;
}

BOOL CALLBACK UninitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    // Unsubclass all windows belonging to this process
    if (pid == GetCurrentProcessId()) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SubclassProc);
    }
    return TRUE;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_original;
HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd != NULL) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);
        if (wcsncmp(className, L"Chrome_WidgetWin_", 17) == 0) {
            if (dwStyle & WS_CAPTION) {
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, 0)) {
                    Wh_Log(L"Subclassed %p", hWnd);
                }
            }
        }
    }
    return hWnd;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    #ifdef _WIN64
        Wh_Log(L"Init - x86_64");
    #else
        Wh_Log(L"Init - x86");
    #endif

    // Check if this process is auxilliary process by checking if the arguments contain --type=
    LPWSTR args = GetCommandLineW();
    if (wcsstr(args, L"--type=") != NULL) {
        Wh_Log(L"Auxilliary process detected, skipping");
        return FALSE;
    }

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_hook,
                       (void**)&CreateWindowExW_original);

    EnumWindows(InitEnumWindowsProc, 0);
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    EnumWindows(UninitEnumWindowsProc, 0);
}
