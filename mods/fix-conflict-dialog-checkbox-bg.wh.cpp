// ==WindhawkMod==
// @id              fix-conflict-dialog-checkbox-bg
// @name            Fix file dialogs for Classic theme
// @description     Fixes black backgrounds in shell dialogs (Classic Theme) under Win 11 24H2+
// @version         2.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         *
// @compilerOptions -lgdi32 -lcomctl32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix File Conflict Dialog Checkbox Background

Fixes black backgrounds in Explorer shell dialogs when Windows Classic
theme is active:
- dialog background (#32770)
- "Apply to all conflicts" checkbox in the file conflict dialog
- Static/Label controls (including icon labels) in shell dialogs
  such as file deletion

Only unthemed (under Classic theme) windows are affected. Themed windows are
ignored completely.

This bug has appeared in recent cumulative updates.

Before:

![before](https://i.imgur.com/jMGJOHd.png)

![before](https://i.imgur.com/asDIOSr.png)

After:

![after](https://i.imgur.com/pK0Z0u8.png)

![after](https://i.imgur.com/wZnlaci.png)

*/
// ==/WindhawkModReadme==

#include <uxtheme.h>
#include <algorithm>
#include <vector>
#include <mutex>
#include <windhawk_utils.h>

#ifndef SS_TYPEMASK
#define SS_TYPEMASK 0x0000001F
#endif

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t pOriginalCreateWindowExW = nullptr;

enum SubclassRole : DWORD_PTR
{
    kRoleDialog = 1,  // shell dialog: background + WM_CTLCOLOR* for its children
    kRoleParent = 2,  // intermediate parent: WM_CTLCOLOR* for its children
    kRoleStatic = 3,  // static control itself: WM_ERASEBKGND (icons)
};

static std::vector<HWND> g_subclassedWindows;
static std::mutex g_mutex;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static bool IsWindowClass(HWND hWnd, const wchar_t* expected)
{
    if (!hWnd)
        return false;
    wchar_t cls[64];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)))
        return false;
    return _wcsicmp(cls, expected) == 0;
}

// Dialogs created from shell templates have the shell module as hInstance.
// If some dialog is not fixed, add its module here.
static bool IsShellDialog(HWND hDlg)
{
    if (!IsWindowClass(hDlg, L"#32770"))
        return false;

    HMODULE hInst = (HMODULE)GetWindowLongPtrW(hDlg, GWLP_HINSTANCE);
    if (!hInst)
        return false;

    static const wchar_t* const kModules[] = {
        L"shell32.dll",
        L"windows.storage.dll",
    };
    for (const wchar_t* name : kModules)
    {
        if (hInst == GetModuleHandleW(name))
            return true;
    }
    return false;
}

// Walks the real parent chain (not owners) and returns the shell dialog, if any.
static HWND FindShellDialogAncestor(HWND hWnd)
{
    HWND hDesktop = GetDesktopWindow();
    for (HWND hCur = hWnd; hCur && hCur != hDesktop;
         hCur = GetAncestor(hCur, GA_PARENT))
    {
        if (IsShellDialog(hCur))
            return hCur;
    }
    return nullptr;
}

// Original v1.0: Button -> CtrlNotifySink -> DirectUIHWND -> shell dialog
static bool CheckHierarchy(HWND hWndParent)
{
    if (!IsWindowClass(hWndParent, L"CtrlNotifySink"))
        return false;

    HWND hDui = GetParent(hWndParent);
    if (!IsWindowClass(hDui, L"DirectUIHWND"))
        return false;

    return IsShellDialog(GetParent(hDui));
}

static bool IsCheckBox(HWND hCtrl)
{
    if (!IsWindowClass(hCtrl, L"Button"))
        return false;
    LONG_PTR type = GetWindowLongPtrW(hCtrl, GWL_STYLE) & BS_TYPEMASK;
    return type == BS_CHECKBOX || type == BS_AUTOCHECKBOX;
}

// Static controls that are not intentionally colored rects/frames.
static bool IsPlainStatic(HWND hCtrl)
{
    if (!IsWindowClass(hCtrl, L"Static"))
        return false;
    LONG_PTR type = GetWindowLongPtrW(hCtrl, GWL_STYLE) & SS_TYPEMASK;
    return !(type >= SS_BLACKRECT && type <= SS_WHITEFRAME);
}

static LRESULT ApplyFaceColors(HDC hdc)
{
    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
    SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
    return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
}

// ---------------------------------------------------------------------------
// Subclass procedure
// ---------------------------------------------------------------------------

static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR dwRefData)
{
    const SubclassRole role = (SubclassRole)dwRefData;

    switch (uMsg)
    {
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
        // Check boxes send WM_CTLCOLORSTATIC, push buttons send WM_CTLCOLORBTN.
        if (role == kRoleDialog || role == kRoleParent)
        {
            HWND hCtrl = (HWND)lParam;
            if (hCtrl &&
                (IsCheckBox(hCtrl) ||
                 (uMsg == WM_CTLCOLORSTATIC && IsPlainStatic(hCtrl))))
            {
                return ApplyFaceColors((HDC)wParam);
            }
        }
        break;

    case WM_CTLCOLORDLG:
        if (role == kRoleDialog)
            return ApplyFaceColors((HDC)wParam);
        break;

    case WM_ERASEBKGND:
        if (role == kRoleDialog || role == kRoleStatic)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
            FillRect((HDC)wParam, &rc, GetSysColorBrush(COLOR_BTNFACE));
            return 1;
        }
        break;

    case WM_NCDESTROY:
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = std::find(g_subclassedWindows.begin(),
                            g_subclassedWindows.end(), hWnd);
        if (it != g_subclassedWindows.end())
            g_subclassedWindows.erase(it);
        break;
    }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Subclassing
// ---------------------------------------------------------------------------

static bool SubclassWindow(HWND hWnd, SubclassRole role)
{
    if (!hWnd)
        return false;

    // Windows of other threads are skipped: subclassing them would need a
    // blocking SendMessage, which is not worth the deadlock risk.
    if (GetWindowThreadProcessId(hWnd, nullptr) != GetCurrentThreadId())
        return false;

    {
        std::lock_guard<std::mutex> lock(g_mutex);

        if (std::find(g_subclassedWindows.begin(), g_subclassedWindows.end(),
                      hWnd) != g_subclassedWindows.end())
        {
            return false;
        }

        // Same-thread window: SetWindowSubclassFromAnyThread takes the direct
        // SetWindowSubclass path and sends no messages, so it is safe to call
        // under the lock. Doing the check, the subclass call and the insertion
        // atomically also guarantees that Wh_ModUninit never sees a subclassed
        // window that is missing from the list.
        if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
                hWnd, SubclassProc, (DWORD_PTR)role))
        {
            return false;
        }

        g_subclassedWindows.push_back(hWnd);
    }

    InvalidateRect(hWnd, nullptr, TRUE);
    return true;
}

static BOOL CALLBACK EnumStaticProc(HWND hChild, LPARAM)
{
    if (IsPlainStatic(hChild))
        SubclassWindow(hChild, kRoleStatic);
    return TRUE;
}

static void SubclassShellDialog(HWND hDlg)
{
    if (SubclassWindow(hDlg, kRoleDialog))
        EnumChildWindows(hDlg, EnumStaticProc, 0);
}

// ---------------------------------------------------------------------------
// Hook
// ---------------------------------------------------------------------------

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
    LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = pOriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd || IsAppThemed())
        return hWnd;

    // 1. The dialog itself (it usually bypasses CreateWindowExW, so it is
    //    also caught when its first child is created).
    if (IsShellDialog(hWnd))
        SubclassShellDialog(hWnd);

    if (hWndParent && IsShellDialog(hWndParent))
        SubclassShellDialog(hWndParent);

    // 2. Static text / icons inside a shell dialog.
    if (hWndParent && IsPlainStatic(hWnd))
    {
        HWND hDlg = FindShellDialogAncestor(hWndParent);
        if (hDlg)
        {
            SubclassWindow(hWnd, kRoleStatic);
            if (hWndParent != hDlg)
                SubclassWindow(hWndParent, kRoleParent);
        }
    }

    // 3. "Apply to all conflicts" checkbox (original v1.0 logic).
    if (hWndParent && IsCheckBox(hWnd) && CheckHierarchy(hWndParent))
        SubclassWindow(hWndParent, kRoleParent);

    return hWnd;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    Wh_SetFunctionHook((void*)CreateWindowExW,
                       (void*)CreateWindowExW_Hook,
                       (void**)&pOriginalCreateWindowExW);

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    std::vector<HWND> hwnds;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        hwnds.swap(g_subclassedWindows);
    }

    for (HWND hWnd : hwnds)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SubclassProc);
}
