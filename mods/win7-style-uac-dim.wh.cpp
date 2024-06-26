// ==WindhawkMod==
// @id              win7-style-uac-dim
// @name            Windows 7 Style UAC Dim
// @description     Restores the desktop screenshot on the User Account Control screen
// @version         1.0.1
// @author          aubymori
// @github          https://github.com/aubymori
// @include         consent.exe
// @compilerOptions -lgdi32 -lgdiplus
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Style UAC Dim
This mod restores the full desktop screenshot on the background of the secure desktop
for UAC. Since Windows 8, it has only displayed the wallpaper.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/win7-uac-dim-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/win7-uac-dim-after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <gdiplus.h>

using namespace Gdiplus;

int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);

HBITMAP g_hbDesktop = NULL;

void (*OnPaint_orig)(HWND) = nullptr;
void OnPaint_hook(HWND hWnd)
{
    PAINTSTRUCT ps = { 0 };
    HDC hDC = BeginPaint(hWnd, &ps);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hbOld = (HBITMAP)SelectObject(hMemDC, g_hbDesktop);

    BitBlt(
        hDC,
        0, 0,
        cx, cy,
        hMemDC,
        0, 0,
        SRCCOPY
    );

    SelectObject(hMemDC, hbOld);
    EndPaint(hWnd, &ps);
}

const WindhawkUtils::SYMBOL_HOOK hooks[] = {
    {
        {
            L"void __cdecl OnPaint(struct HWND__ *)"
        },
        &OnPaint_orig,
        OnPaint_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    /* Take a screenshot of the desktop */
    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    g_hbDesktop = CreateCompatibleBitmap(hScreenDC, cx, cy);
    SelectObject(hMemDC, g_hbDesktop);
    BitBlt(
        hMemDC,
        0, 0,
        cx, cy,
        hScreenDC,
        x, y,
        SRCCOPY
    );

    /* Regular GDI is hard to alpha blend in */
    GdiplusStartupInput gsi;
    ULONG_PTR ulToken;
    GdiplusStartup(&ulToken, &gsi, NULL);

    /* Put black overlay on the desktop screenshot */
    Bitmap bm(g_hbDesktop, NULL);
    Graphics gfx(&bm);
    Color overlay(175, 0, 0, 0);
    SolidBrush brush(overlay);
    gfx.FillRectangle(&brush, 0, 0, cx, cy);

    /* Delete original screenshot and replace with the dimmed one */
    DeleteObject(g_hbDesktop);
    bm.GetHBITMAP(Color(0, 0, 0), &g_hbDesktop);

    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);

    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        hooks,
        ARRAYSIZE(hooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in consent.exe");
        return FALSE;
    }

    return TRUE;
}