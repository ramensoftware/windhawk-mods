// ==WindhawkMod==
// @id              magnifier-headless
// @name            Magnifier Headless Mode
// @description     Blocks all Magnifier window creation, keeping zoom functionality with win+"-" and win+"+" keyboard shortcuts.
// @version         0.1.2
// @author          BCRTVKCS
// @github          https://github.com/bcrtvkcs
// @twitter         https://x.com/bcrtvkcs
// @homepage        https://grdigital.pro
// @include         magnify.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Blocks all Magnifier window creation, keeping zoom functionality with win+"-" and win+"+" keyboard shortcuts.
*/
// ==/WindhawkModReadme==

#include <windows.h>

// Global flag for clean shutdown
volatile BOOL g_shouldStop = FALSE;

// Function to check if window is magnifier by class name (more reliable)
BOOL IsMagnifierWindow(HWND hwnd) {
    wchar_t className[256] = {0};
    GetClassNameW(hwnd, className, 256);
    
    // Check for known Magnifier class names
    return (wcscmp(className, L"MagUIClass") == 0 ||
            wcscmp(className, L"ScreenMagnifierUIWnd") == 0);
}

// Hide magnifier windows by class name
void HideMagnifierWindows() {
    // Find by MagUIClass
    HWND hwnd = FindWindowW(L"MagUIClass", NULL);
    if (hwnd) {
        ShowWindow(hwnd, SW_HIDE);
    }
    
    // Find by ScreenMagnifierUIWnd
    hwnd = FindWindowW(L"ScreenMagnifierUIWnd", NULL);
    if (hwnd) {
        ShowWindow(hwnd, SW_HIDE);
    }
}

// Monitoring thread with proper shutdown
DWORD WINAPI MonitorThread(LPVOID lpParam) {
    while (!g_shouldStop) {
        HideMagnifierWindows();
        
        // Sleep with early exit check
        for (int i = 0; i < 50 && !g_shouldStop; i++) {
            Sleep(10); // Total 500ms but can exit early
        }
    }
    return 0;
}

HANDLE g_thread = NULL;

// ShowWindow hook to catch magnifier show attempts
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    // If it's a magnifier window trying to show, hide it instead
    if (IsMagnifierWindow(hWnd) && 
        (nCmdShow == SW_SHOW || nCmdShow == SW_RESTORE || nCmdShow == SW_MAXIMIZE)) {
        return ShowWindow_Original(hWnd, SW_HIDE);
    }
    
    return ShowWindow_Original(hWnd, nCmdShow);
}

// CreateWindowExW hook to catch magnifier window creation
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
) {
    HWND hwnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );
    
    // If created window is magnifier, hide it immediately
    if (hwnd && lpClassName) {
        if (wcscmp(lpClassName, L"MagUIClass") == 0 ||
            wcscmp(lpClassName, L"ScreenMagnifierUIWnd") == 0) {
            ShowWindow(hwnd, SW_HIDE);
        }
    }
    
    return hwnd;
}

// Mod initialization
BOOL Wh_ModInit() {
    g_shouldStop = FALSE;
    
    // Initial hide
    HideMagnifierWindows();
    
    // Create monitoring thread
    g_thread = CreateThread(NULL, 0, MonitorThread, NULL, 0, NULL);
    
    return TRUE;
}

// Proper cleanup without TerminateThread
void Wh_ModUninit() {
    // Signal thread to stop
    g_shouldStop = TRUE;
    
    // Wait for thread to finish properly
    if (g_thread) {
        WaitForSingleObject(g_thread, 2000); // Wait max 2 seconds
        CloseHandle(g_thread);
        g_thread = NULL;
    }
    
    // Restore magnifier windows by class name
    HWND hwnd = FindWindowW(L"MagUIClass", NULL);
    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
    }
    
    hwnd = FindWindowW(L"ScreenMagnifierUIWnd", NULL);
    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
    }
}

// Hook setup
BOOL Wh_ModSettingsInit() {
    return TRUE;
}

BOOL Wh_ModBeforeSymbolLoading() {
    // Hook ShowWindow
    if (!Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "ShowWindow"),
        (void*)ShowWindow_Hook,
        (void**)&ShowWindow_Original)) {
        return FALSE;
    }
    
    // Hook CreateWindowExW
    if (!Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "CreateWindowExW"),
        (void*)CreateWindowExW_Hook,
        (void**)&CreateWindowExW_Original)) {
        return FALSE;
    }
    
    return TRUE;
}