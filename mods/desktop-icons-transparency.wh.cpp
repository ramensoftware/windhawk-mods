// ==WindhawkMod==
// @id              desktop-icons-transparency
// @name            Desktop Icons Transparency
// @description     Adjusts the opacity of desktop icons and text labels without darkening the wallpaper
// @version         1.0.0
// @author          zed712969-crypto
// @github          https://github.com/zed712969-crypto
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32 -lmsimg32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icons Transparency
A lightweight mod that adjusts the opacity (alpha channel) of Windows desktop icons, including their text labels and drop shadows.

Unlike **Transparent Desktop Icons with Spotlight**, this mod applies a fixed opacity directly at draw time using native alpha blending — without creating DirectComposition overlays, duplicate wallpaper layers, or spotlight hover effects.

### Features
- Synchronously fades icon images, labels, and text shadows.
- Does not darken wallpaper or cause black background artifacts.
- Scoped strictly to desktop repainting: does not affect taskbar, tooltips, context menus, Explorer folder windows, or icon renaming.
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
#include <algorithm>
#include <windhawk_utils.h>

struct {
    int opacity;
} settings;

static thread_local bool g_inDesktopPaint = false;
static thread_local bool g_inDirectHook = false;
static thread_local bool g_inTextHook = false;

static HWND g_hDesktopListView = NULL;

// Subclass procedure: activates hooks strictly during desktop rendering
LRESULT CALLBACK DesktopListSubclass(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData
) {
    if (uMsg == WM_PAINT || uMsg == WM_PRINTCLIENT) {
        g_inDesktopPaint = true;
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        g_inDesktopPaint = false;
        return result;
    }

    if (uMsg == WM_NCDESTROY) {
        if (hWnd == g_hDesktopListView) {
            g_hDesktopListView = NULL;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Checks if window is a valid desktop shell root
bool IsDesktopParent(HWND hWnd) {
    if (!hWnd) return false;
    if (hWnd == GetShellWindow()) return true;

    WCHAR className[64] = {0};
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        if (_wcsicmp(className, L"Progman") == 0 || _wcsicmp(className, L"WorkerW") == 0) {
            return true;
        }
    }
    return false;
}

// Locate SysListView32 belonging to desktop in current Explorer process
HWND FindDesktopListView() {
    HWND hShell = GetShellWindow();
    if (hShell) {
        HWND hDefView = FindWindowExW(hShell, NULL, L"SHELLDLL_DefView", NULL);
        if (hDefView) {
            HWND hList = FindWindowExW(hDefView, NULL, L"SysListView32", NULL);
            if (hList) return hList;
        }
    }

    HWND hProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hProgman && hProgman != hShell) {
        HWND hDefView = FindWindowExW(hProgman, NULL, L"SHELLDLL_DefView", NULL);
        if (hDefView) {
            HWND hList = FindWindowExW(hDefView, NULL, L"SysListView32", NULL);
            if (hList) return hList;
        }
    }

    HWND hWorkerW = NULL;
    while ((hWorkerW = FindWindowExW(NULL, hWorkerW, L"WorkerW", NULL)) != NULL) {
        HWND hDefView = FindWindowExW(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
        if (hDefView) {
            HWND hList = FindWindowExW(hDefView, NULL, L"SysListView32", NULL);
            if (hList) return hList;
        }
    }

    return NULL;
}

HWND GetDesktopListView() {
    HWND hListView = FindDesktopListView();
    if (hListView) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hListView, &pid);
        if (pid == GetCurrentProcessId()) {
            return hListView;
        }
    }
    return NULL;
}

// 1. Hook for ImageList_DrawIndirect
using ImageList_DrawIndirect_t = decltype(&ImageList_DrawIndirect);
ImageList_DrawIndirect_t ImageList_DrawIndirect_Original = nullptr;

BOOL WINAPI ImageList_DrawIndirect_Hook(IMAGELISTDRAWPARAMS* pimldp) {
    if (pimldp && g_inDesktopPaint) {
        if (settings.opacity <= 0) {
            return TRUE;
        }
        if (settings.opacity >= 100) {
            return ImageList_DrawIndirect_Original(pimldp);
        }

        IMAGELISTDRAWPARAMS params = {};
        memcpy(&params, pimldp, std::min<DWORD>(pimldp->cbSize, sizeof(params)));
        params.cbSize = sizeof(params);

        if (!(params.fState & ILS_ALPHA)) {
            params.Frame = 255;
        }
        params.fState |= ILS_ALPHA;
        params.Frame = (DWORD)((params.Frame * settings.opacity) / 100);

        g_inDirectHook = true;
        BOOL result = ImageList_DrawIndirect_Original(&params);
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
    if (!g_inDirectHook && !g_inTextHook && g_inDesktopPaint) {
        if (settings.opacity <= 0) {
            return TRUE;
        }

        if (settings.opacity < 100) {
            ftn.SourceConstantAlpha = (BYTE)((ftn.SourceConstantAlpha * settings.opacity) / 100);
        }
    }
    return GdiAlphaBlend_Original(hdcDest, xoriginDest, yoriginDest, wDest, hDest,
                                 hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
}

// 3. Hook for DrawShadowText
using DrawShadowText_t = decltype(&DrawShadowText);
DrawShadowText_t DrawShadowText_Original = nullptr;

int WINAPI DrawShadowText_Hook(
    HDC hdc, LPCWSTR pszText, UINT cch, RECT *prc, DWORD dwFlags,
    COLORREF crText, COLORREF crShadow, int ixOffset, int iyOffset
) {
    if (!g_inDesktopPaint || g_inTextHook) {
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
    if (!g_inDesktopPaint || g_inTextHook) {
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
    if (g_inDesktopPaint && !g_inTextHook) {
        if (settings.opacity <= 0) {
            return TRUE;
        }
    }
    return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
}

// 6. Hook for CreateWindowExW (strictly target desktop list view)
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

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
    HWND hWnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );

    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (_wcsicmp(lpClassName, L"SysListView32") == 0 && hWndParent) {
            WCHAR parentClass[64] = {0};
            if (GetClassNameW(hWndParent, parentClass, ARRAYSIZE(parentClass)) &&
                _wcsicmp(parentClass, L"SHELLDLL_DefView") == 0) {
                HWND hGrandParent = GetAncestor(hWndParent, GA_PARENT);
                if (IsDesktopParent(hGrandParent)) {
                    DWORD pid = 0;
                    GetWindowThreadProcessId(hWnd, &pid);
                    if (pid == GetCurrentProcessId()) {
                        g_hDesktopListView = hWnd;
                        WindhawkUtils::SetWindowSubclassFromAnyThread(
                            hWnd, DesktopListSubclass, 0
                        );
                    }
                }
            }
        }
    }

    return hWnd;
}

void RepaintDesktop() {
    HWND hListView = g_hDesktopListView ? g_hDesktopListView : GetDesktopListView();
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

    WindhawkUtils::SetFunctionHook(
        CreateWindowExW,
        CreateWindowExW_Hook,
        &CreateWindowExW_Original
    );

    WindhawkUtils::SetFunctionHook(
        ImageList_DrawIndirect,
        ImageList_DrawIndirect_Hook,
        &ImageList_DrawIndirect_Original
    );

    WindhawkUtils::SetFunctionHook(
        DrawShadowText,
        DrawShadowText_Hook,
        &DrawShadowText_Original
    );

    WindhawkUtils::SetFunctionHook(
        DrawTextW,
        DrawTextW_Hook,
        &DrawTextW_Original
    );

    WindhawkUtils::SetFunctionHook(
        ExtTextOutW,
        ExtTextOutW_Hook,
        &ExtTextOutW_Original
    );

    HMODULE hGdi = GetModuleHandleW(L"gdi32full.dll");
    if (!hGdi) hGdi = GetModuleHandleW(L"gdi32.dll");
    if (hGdi) {
        void* pGdiAlphaBlend = (void*)GetProcAddress(hGdi, "GdiAlphaBlend");
        if (pGdiAlphaBlend) {
            WindhawkUtils::SetFunctionHook(
                (GdiAlphaBlend_t)pGdiAlphaBlend,
                GdiAlphaBlend_Hook,
                &GdiAlphaBlend_Original
            );
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"Desktop Icons Transparency AfterInit");

    HWND hListView = GetDesktopListView();
    if (hListView) {
        g_hDesktopListView = hListView;
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hListView, DesktopListSubclass, 0
        );
        RepaintDesktop();
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Desktop Icons Transparency Uninit");

    if (g_hDesktopListView && IsWindow(g_hDesktopListView)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_hDesktopListView, DesktopListSubclass
        );
    }
    RepaintDesktop();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Desktop Icons Transparency SettingsChanged");
    LoadSettings();
    RepaintDesktop();
}
