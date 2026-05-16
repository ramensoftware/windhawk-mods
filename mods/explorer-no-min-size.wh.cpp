// ==WindhawkMod==
// @id              explorer-no-min-size
// @name            Explorer No Minimum Window Size
// @description     Removes the minimum window size restriction in File Explorer, allowing windows to be resized freely.
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer No Minimum Window Size

Removes the minimum window size restriction in Windows File Explorer.
By default, Explorer prevents windows from being resized below a certain
width and height. With this mod you can resize Explorer windows to any size.

![screenshot](https://i.imgur.com/eE0Hmr8.png)

*/
// ==/WindhawkModReadme==
#include <windhawk_utils.h>
#include <windows.h>

static bool IsExplorerWindow(HWND hwnd) {
    if (!hwnd) return false;
    wchar_t cls[256];
    if (!GetClassNameW(hwnd, cls, 256)) return false;
    return wcscmp(cls, L"CabinetWClass") == 0 || wcscmp(cls, L"ExploreWClass") == 0;
}

static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR) {
    if (msg == WM_GETMINMAXINFO) {
        LRESULT r = DefSubclassProc(hwnd, msg, wp, lp);
        MINMAXINFO* m = (MINMAXINFO*)lp;
        m->ptMinTrackSize.x = 1;
        m->ptMinTrackSize.y = 1;
        return r;
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static void Sub(HWND hwnd) {
    if (IsExplorerWindow(hwnd))
        WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, SubclassProc, 0);
}

static void Unsub(HWND hwnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, SubclassProc);
}

static BOOL CALLBACK EnumSub(HWND hwnd, LPARAM) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) Sub(hwnd);
    return TRUE;
}

static BOOL CALLBACK EnumUnsub(HWND hwnd, LPARAM) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) Unsub(hwnd);
    return TRUE;
}

typedef HWND(WINAPI* CWE_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CWE_t oCWE;

HWND WINAPI hCWE(DWORD a, LPCWSTR b, LPCWSTR c, DWORD d, int e, int f, int g, int h, HWND i, HMENU j, HINSTANCE k, LPVOID l) {
    HWND hwnd = oCWE(a, b, c, d, e, f, g, h, i, j, k, l);
    if (hwnd) Sub(hwnd);
    return hwnd;
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)hCWE, (void**)&oCWE);
    EnumWindows(EnumSub, 0);
    return TRUE;
}

void Wh_ModUninit() {
    EnumWindows(EnumUnsub, 0);
}

void Wh_ModSettingsChanged() {}
