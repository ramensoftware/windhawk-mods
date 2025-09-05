// ==WindhawkMod==
// @id              magnifier-headless
// @name            Magnifier Headless Mode
// @description     Blocks all Magnifier window creation, keeping zoom functionality with win+"-" and win+"+" keyboard shortcuts.
// @version         0.1.0
// @author          BCRTVKCS
// @github          https://github.com/bcrtvkcs
// @twitter         https://x.com/bcrtvkcs
// @homepage        https://grdigital.pro
// @include         magnify.exe
// @exclude         ^(?!.*magnify.exe)
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Blocks all Magnifier window creation, keeping zoom functionality with win+"-" and win+"+" keyboard shortcuts.
*/
// ==/WindhawkModReadme==

#include <windows.h>

// Simple function to hide magnifier windows by exact title match only
void HideMagnifierByTitle() {
    // Only search for exact Magnifier window titles
    const wchar_t* exactTitles[] = {
        L"Büyüteç",
        L"Magnifier"
    };
    
    for (int i = 0; i < 2; i++) {
        HWND hwnd = FindWindowW(NULL, exactTitles[i]);
        if (hwnd) {
            ShowWindow(hwnd, SW_HIDE);
        }
    }
}

// Monitoring thread - only checks for magnifier titles
DWORD WINAPI MonitorThread(LPVOID lpParam) {
    while (true) {
        HideMagnifierByTitle();
        Sleep(500); // Check every 500ms
    }
    return 0;
}

HANDLE g_thread = NULL;

// Mod initialization - minimal approach
BOOL Wh_ModInit() {
    // Initial hide
    HideMagnifierByTitle();
    
    // Create simple monitoring thread
    g_thread = CreateThread(NULL, 0, MonitorThread, NULL, 0, NULL);
    
    return TRUE;
}

// Mod cleanup
void Wh_ModUninit() {
    if (g_thread) {
        TerminateThread(g_thread, 0);
        CloseHandle(g_thread);
        g_thread = NULL;
    }
    
    // Restore magnifier windows
    HWND hwnd = FindWindowW(NULL, L"Büyüteç");
    if (hwnd) ShowWindow(hwnd, SW_SHOW);
    
    hwnd = FindWindowW(NULL, L"Magnifier");
    if (hwnd) ShowWindow(hwnd, SW_SHOW);
}
