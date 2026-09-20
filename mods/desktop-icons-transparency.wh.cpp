// ==WindhawkMod==
// @id              desktop-icons-transparency
// @name            Desktop Icons Transparency
// @description     Adjusts the opacity of desktop icons and text labels without darkening the wallpaper
// @version         1.0.0
// @author          malinkadev
// @github          https://github.com/zed712969-crypto
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32 -lmsimg32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icons Transparency
A lightweight mod that seamlessly adjusts the opacity (alpha channel) of Windows desktop icons, including their text labels and drop shadows.

### Features
- Synchronously fades icon images, labels, and text shadows.
- Does not darken wallpaper or create black background artifacts.
- Zero CPU and RAM overhead in idle.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- opacity: 50
  $name: Icon opacity (%)
  $description: From 0 (fully hidden) to 100 (fully visible).
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0A00
#define _WIN32_IE 0x0A00

#include <windows.h>
#include <commctrl.h>
#include <commoncontrols.h>

struct {
    int opacity;
} settings;

static DWORD g_desktopThreadId = 0;
static thread_local bool g_inDirectHook = false;
static thread_local bool g_inTextHook = false;

// Search for SysListView32 of the desktop
HWND GetDesktopListView() {
    HWND hProgman = FindWindowW(L"Progman", L"Program Manager");
    HWND hDefView = NULL;

    if (hProgman) {
        hDefView = FindWindowExW(hProgman, NULL, L"SHELLDLL_DefView", NULL);
    }

    if (!hDefView) {
        HWND hWorkerW = NULL;
        while ((hWorkerW = FindWindowExW(NULL, hWorkerW, L"WorkerW", NULL)) != NULL) {
            hDefView = FindWindowExW(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            if (hDefView) break;
        }
    }

    if (!hDefView) return NULL;
    return FindWindowExW(hDefView, NULL, L"SysListView32", NULL);
}

bool IsDesktopThread() {
    if (!g_desktopThreadId) {
        HWND hListView = GetDesktopListView();
        if (hListView) {
            g_desktopThreadId = GetWindowThreadProcessId(hListView, NULL);
        }
    }
    return (g_desktopThreadId != 0 && GetCurrentThreadId() == g_desktopThreadId);
}

// 1. Hook for ImageList_DrawIndirect
using ImageList_DrawIndirect_t = BOOL (WINAPI *)(IMAGELISTDRAWPARAMS* pimldp);
ImageList_DrawIndirect_t ImageList_DrawIndirect_Original = nullptr;

BOOL WINAPI ImageList_DrawIndirect_Hook(IMAGELISTDRAWPARAMS* pimldp) {
    if (pimldp && IsDesktopThread()) {
        if (settings.opacity <= 0) {
            return TRUE;
        }

        if (settings.opacity < 100) {
            pimldp->fState |= 0x00000008; // ILS_ALPHA
            pimldp->Frame = (DWORD)((settings.opacity * 255) / 100);
        }

        g_inDirectHook = true;
        BOOL result = ImageList_DrawIndirect_Original(pimldp);
        g_inDirectHook = false;
        return result;
    }
    return ImageList_DrawIndirect_Original(pimldp);
}

// 2. Hook for GdiAlphaBlend
using GdiAlphaBlend_t = BOOL (WINAPI *)(
    HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
    HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc,
    BLENDFUNCTION ftn
);
GdiAlphaBlend_t GdiAlphaBlend_Original = nullptr;

BOOL WINAPI GdiAlphaBlend_Hook(
    HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
    HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc,
    BLENDFUNCTION ftn
) {
    if (!g_inDirectHook && !g_inTextHook && IsDesktopThread()) {
        if (settings.opacity <= 0) {
            return TRUE;
        }

        if (settings.opacity < 100) {
            BYTE baseAlpha = ftn.SourceConstantAlpha ? ftn.SourceConstantAlpha : 255;
            ftn.SourceConstantAlpha = (BYTE)((baseAlpha * settings.opacity) / 100);
        }
    }
    return GdiAlphaBlend_Original(hdcDest, xoriginDest, yoriginDest, wDest, hDest,
                                 hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
}

// 3. Hook for DrawShadowText
using DrawShadowText_t = int (WINAPI *)(
    HDC hdc, LPCWSTR pszText, UINT cch, RECT *prc, DWORD dwFlags,
    COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset
);
DrawShadowText_t DrawShadowText_Original = nullptr;

int WINAPI DrawShadowText_Hook(
    HDC hdc, LPCWSTR pszText, UINT cch, RECT *prc, DWORD dwFlags,
    COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset
) {
    if (!IsDesktopThread() || g_inTextHook) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    if (dwFlags & DT_CALCRECT) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    if (settings.opacity >= 100) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    if (settings.opacity <= 0) {
        return (prc ? (prc->bottom - prc->top) : 1);
    }

    if (!prc) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    int pad = 4;
    int x = prc->left - pad;
    int y = prc->top - pad;
    int w = (prc->right - prc->left) + pad * 2;
    int h = (prc->bottom - prc->top) + pad * 2;

    if (w <= 0 || h <= 0) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    HDC hMemDC = CreateCompatibleDC(hdc);
    if (!hMemDC) {
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    HBITMAP hBmp = CreateCompatibleBitmap(hdc, w, h);
    if (!hBmp) {
        DeleteDC(hMemDC);
        return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText, crShadow, ixOffset, iyOffset);
    }

    HGDIOBJ hOldBmp = SelectObject(hMemDC, hBmp);
    HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    HGDIOBJ hOldFont = SelectObject(hMemDC, hFont);

    BitBlt(hMemDC, 0, 0, w, h, hdc, x, y, SRCCOPY);

    RECT localRect = { pad, pad, pad + (prc->right - prc->left), pad + (prc->bottom - prc->top) };

    g_inTextHook = true;
    int result = DrawShadowText_Original(hMemDC, pszText, cch, &localRect, dwFlags, crText, crShadow, ixOffset, iyOffset);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = (BYTE)((settings.opacity * 255) / 100);
    bf.AlphaFormat = 0;

    AlphaBlend(hdc, x, y, w, h, hMemDC, 0, 0, w, h, bf);
    g_inTextHook = false;

    SelectObject(hMemDC, hOldFont);
    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hMemDC);

    return result;
}

// 4. Hook for DrawTextW
using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t DrawTextW_Original = nullptr;

int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    if (!IsDesktopThread() || g_inTextHook) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    if (format & DT_CALCRECT) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    if (settings.opacity >= 100) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    if (settings.opacity <= 0) {
        return (lprc ? (lprc->bottom - lprc->top) : 1);
    }

    if (!lprc) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    int pad = 4;
    int x = lprc->left - pad;
    int y = lprc->top - pad;
    int w = (lprc->right - lprc->left) + pad * 2;
    int h = (lprc->bottom - lprc->top) + pad * 2;

    if (w <= 0 || h <= 0) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    HDC hMemDC = CreateCompatibleDC(hdc);
    if (!hMemDC) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    HBITMAP hBmp = CreateCompatibleBitmap(hdc, w, h);
    if (!hBmp) {
        DeleteDC(hMemDC);
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    HGDIOBJ hOldBmp = SelectObject(hMemDC, hBmp);
    HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    HGDIOBJ hOldFont = SelectObject(hMemDC, hFont);

    SetTextColor(hMemDC, GetTextColor(hdc));
    SetBkMode(hMemDC, TRANSPARENT);

    BitBlt(hMemDC, 0, 0, w, h, hdc, x, y, SRCCOPY);

    RECT localRect = { pad, pad, pad + (lprc->right - lprc->left), pad + (lprc->bottom - lprc->top) };

    g_inTextHook = true;
    int result = DrawTextW_Original(hMemDC, lpchText, cchText, &localRect, format);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = (BYTE)((settings.opacity * 255) / 100);
    bf.AlphaFormat = 0;

    AlphaBlend(hdc, x, y, w, h, hMemDC, 0, 0, w, h, bf);
    g_inTextHook = false;

    SelectObject(hMemDC, hOldFont);
    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hMemDC);

    return result;
}

// 5. Hook for ExtTextOutW
using ExtTextOutW_t = decltype(&ExtTextOutW);
ExtTextOutW_t ExtTextOutW_Original = nullptr;

BOOL WINAPI ExtTextOutW_Hook(
    HDC hdc, int x, int y, UINT options, const RECT *lprect,
    LPCWSTR lpString, UINT c, const INT *lpDx
) {
    if (IsDesktopThread() && !g_inTextHook) {
        if (settings.opacity <= 0) {
            return TRUE;
        }
    }
    return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
}

void RepaintDesktop() {
    HWND hListView = GetDesktopListView();
    if (!hListView || !IsWindow(hListView)) return;

    int count = (int)SendMessageW(hListView, LVM_GETITEMCOUNT, 0, 0);
    if (count > 0) {
        SendMessageW(hListView, LVM_REDRAWITEMS, 0, count - 1);
    }
    InvalidateRect(hListView, NULL, TRUE);
    UpdateWindow(hListView);
}

void LoadSettings() {
    settings.opacity = Wh_GetIntSetting(L"opacity");
    if (settings.opacity < 0) settings.opacity = 0;
    if (settings.opacity > 100) settings.opacity = 100;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Desktop Icons Transparency Init");

    LoadSettings();

    HWND hListView = GetDesktopListView();
    if (hListView) {
        g_desktopThreadId = GetWindowThreadProcessId(hListView, NULL);
    }

    // Hook comctl32.dll
    HMODULE hComCtl = GetModuleHandleW(L"comctl32.dll");
    if (hComCtl) {
        void* pDrawIndirect = (void*)GetProcAddress(hComCtl, "ImageList_DrawIndirect");
        if (pDrawIndirect) {
            Wh_SetFunctionHook(pDrawIndirect,
                               (void*)ImageList_DrawIndirect_Hook,
                               (void**)&ImageList_DrawIndirect_Original);
        }

        void* pDrawShadowText = (void*)GetProcAddress(hComCtl, "DrawShadowText");
        if (pDrawShadowText) {
            Wh_SetFunctionHook(pDrawShadowText,
                               (void*)DrawShadowText_Hook,
                               (void**)&DrawShadowText_Original);
        }
    }

    // Hook user32.dll
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        void* pDrawTextW = (void*)GetProcAddress(hUser32, "DrawTextW");
        if (pDrawTextW) {
            Wh_SetFunctionHook(pDrawTextW,
                               (void*)DrawTextW_Hook,
                               (void**)&DrawTextW_Original);
        }
    }

    // Hook gdi32full.dll / gdi32.dll
    HMODULE hGdi = GetModuleHandleW(L"gdi32full.dll");
    if (!hGdi) hGdi = GetModuleHandleW(L"gdi32.dll");
    if (hGdi) {
        void* pGdiAlphaBlend = (void*)GetProcAddress(hGdi, "GdiAlphaBlend");
        if (pGdiAlphaBlend) {
            Wh_SetFunctionHook(pGdiAlphaBlend,
                               (void*)GdiAlphaBlend_Hook,
                               (void**)&GdiAlphaBlend_Original);
        }

        void* pExtTextOutW = (void*)GetProcAddress(hGdi, "ExtTextOutW");
        if (pExtTextOutW) {
            Wh_SetFunctionHook(pExtTextOutW,
                               (void*)ExtTextOutW_Hook,
                               (void**)&ExtTextOutW_Original);
        }
    }

    RepaintDesktop();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Desktop Icons Transparency Uninit");
    RepaintDesktop();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Desktop Icons Transparency SettingsChanged");
    LoadSettings();
    RepaintDesktop();
}
