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

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

static bool IsExplorerWindow(HWND hwnd)
{
    if (!hwnd)
        return false;

    wchar_t cls[256] = {};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)))
        return false;

    return wcscmp(cls, L"CabinetWClass") == 0 ||
           wcscmp(cls, L"ExploreWClass") == 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Per-window subclassing
// ─────────────────────────────────────────────────────────────────────────────

static const wchar_t* kPropName = L"WH_OrigWndProc";

static LRESULT CALLBACK SubclassWndProc(HWND hwnd, UINT msg,
                                         WPARAM wParam, LPARAM lParam)
{
    WNDPROC original = reinterpret_cast<WNDPROC>(
        GetPropW(hwnd, kPropName));

    if (msg == WM_GETMINMAXINFO)
    {
        LRESULT result = original
            ? CallWindowProcW(original, hwnd, msg, wParam, lParam)
            : DefWindowProcW(hwnd, msg, wParam, lParam);

        MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
        mmi->ptMinTrackSize.x = 1;
        mmi->ptMinTrackSize.y = 1;

        return result;
    }

    if (msg == WM_NCDESTROY)
    {
        if (original)
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC,
                              reinterpret_cast<LONG_PTR>(original));
        RemovePropW(hwnd, kPropName);
    }

    return original
        ? CallWindowProcW(original, hwnd, msg, wParam, lParam)
        : DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void SubclassWindow(HWND hwnd)
{
    if (!IsExplorerWindow(hwnd))
        return;
    if (GetPropW(hwnd, kPropName))
        return; // already subclassed

    LONG_PTR original = SetWindowLongPtrW(
        hwnd, GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(SubclassWndProc));

    SetPropW(hwnd, kPropName, reinterpret_cast<HANDLE>(original));
    Wh_Log(L"Subclassed HWND %p", hwnd);
}

static void UnsubclassWindow(HWND hwnd)
{
    WNDPROC original = reinterpret_cast<WNDPROC>(
        GetPropW(hwnd, kPropName));
    if (!original)
        return;

    SetWindowLongPtrW(hwnd, GWLP_WNDPROC,
                      reinterpret_cast<LONG_PTR>(original));
    RemovePropW(hwnd, kPropName);
    Wh_Log(L"Unsubclassed HWND %p", hwnd);
}

// ─────────────────────────────────────────────────────────────────────────────
// EnumWindows callbacks  (plain functions – no captures, convertible to WNDENUMPROC)
// ─────────────────────────────────────────────────────────────────────────────

static BOOL CALLBACK EnumSubclass(HWND hwnd, LPARAM /*lParam*/)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId())
        SubclassWindow(hwnd);
    return TRUE;
}

static BOOL CALLBACK EnumUnsubclass(HWND hwnd, LPARAM /*lParam*/)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId())
        UnsubclassWindow(hwnd);
    return TRUE;
}

// ─────────────────────────────────────────────────────────────────────────────
// CreateWindowExW hook – catch windows as they are created
// ─────────────────────────────────────────────────────────────────────────────

using CreateWindowExW_t = HWND(WINAPI *)(
    DWORD, LPCWSTR, LPCWSTR, DWORD,
    int, int, int, int,
    HWND, HMENU, HINSTANCE, LPVOID);

CreateWindowExW_t originalCreateWindowExW = nullptr;

HWND WINAPI CreateWindowExWHook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hwnd = originalCreateWindowExW(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hwnd)
        SubclassWindow(hwnd);

    return hwnd;
}

// ─────────────────────────────────────────────────────────────────────────────
// Windhawk entry points
// ─────────────────────────────────────────────────────────────────────────────

BOOL Wh_ModInit()
{
    Wh_Log(L"Explorer No Minimum Window Size – init");

    Wh_SetFunctionHook(
        reinterpret_cast<void*>(CreateWindowExW),
        reinterpret_cast<void*>(CreateWindowExWHook),
        reinterpret_cast<void**>(&originalCreateWindowExW));

    // Subclass windows that are already open.
    EnumWindows(EnumSubclass, 0);

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Explorer No Minimum Window Size – uninit");
    EnumWindows(EnumUnsubclass, 0);
}
