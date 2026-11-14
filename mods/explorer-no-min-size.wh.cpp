// ==WindhawkMod==
// @id              explorer-no-min-size
// @name            Explorer No Minimum Window Size
// @description     Removes the minimum window size restriction in File Explorer
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lcomctl32
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

static bool IsExplorerWindow(HWND hwnd)
{
    if (!hwnd) return false;
    wchar_t cls[256] = {};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) return false;
    return wcscmp(cls, L"CabinetWClass") == 0 || wcscmp(cls, L"ExploreWClass") == 0;
}

static LRESULT CALLBACK SubclassWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    if (msg == WM_GETMINMAXINFO)
    {
        LRESULT result = DefSubclassProc(hwnd, msg, wParam, lParam);
        MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
        mmi->ptMinTrackSize.x = 1;
        mmi->ptMinTrackSize.y = 1;
        return result;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static void SubclassWindow(HWND hwnd)
{
    if (!IsExplorerWindow(hwnd)) return;
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, SubclassWndProc, 0))
        Wh_Log(L"Subclassed HWND %p", hwnd);
}

static void UnsubclassWindow(HWND hwnd)
{
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, SubclassWndProc);
}

static BOOL CALLBACK EnumSubclass(HWND hwnd, LPARAM)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) SubclassWindow(hwnd);
    return TRUE;
}

static BOOL CALLBACK EnumUnsubclass(HWND hwnd, LPARAM)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) UnsubclassWindow(hwnd);
    return TRUE;
}

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t originalCreateWindowExW = nullptr;

HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hwnd = originalCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd) SubclassWindow(hwnd);
    return hwnd;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"init");
    Wh_SetFunctionHook(
        reinterpret_cast<void*>(CreateWindowExW),
        reinterpret_cast<void*>(CreateWindowExWHook),
        reinterpret_cast<void**>(&originalCreateWindowExW));
    EnumWindows(EnumSubclass, 0);
    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"uninit");
    EnumWindows(EnumUnsubclass, 0);
}

void Wh_ModSettingsChanged() {}
