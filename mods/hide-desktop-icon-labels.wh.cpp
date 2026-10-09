// ==WindhawkMod==
// @id              hide-desktop-icon-labels
// @name            Hide desktop icon labels
// @description     Hides text labels under Windows desktop icons.
// @version         1.5
// @author          akumy
// @github          https://github.com/Akumy-01
// @include         explorer.exe
// @compilerOptions -lcomctl32 -luxtheme -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide desktop icon labels

This mod hides the text labels under desktop icons without renaming the items.
It can optionally show a hidden caption while the mouse is over the icon. It
targets only the desktop's `SysListView32` control inside `explorer.exe`, so
File Explorer folder views are left unchanged.

The desktop still reserves the normal label area. This keeps icon positions and
selection behavior stable, while the label text itself is not drawn.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideLabels: true
  $name: Hide labels
  $description: Hide text labels under desktop icons.
- showCaptionOnMouseOver: true
  $name: Show caption on mouseover
  $description: Temporarily show the hovered desktop icon caption.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <uxtheme.h>

struct Settings {
    bool hideLabels;
    bool showCaptionOnMouseOver;
};

Settings g_settings;
thread_local int g_desktopListViewPaintDepth;
thread_local HWND g_currentDesktopListView;
thread_local int g_allowedDesktopTextDrawDepth;
HWND g_hoveredListView;
int g_hoveredItem = -1;
bool g_trackingMouseLeave;

const WCHAR kDesktopListViewSubclassProp[] =
    L"Windhawk.HideDesktopIconLabels.Subclassed";

struct EnumDesktopListViewsContext {
    void (*callback)(HWND);
};

class ScopedAllowedDesktopTextDraw {
   public:
    explicit ScopedAllowedDesktopTextDraw(bool enabled) : m_enabled(enabled) {
        if (m_enabled) {
            g_allowedDesktopTextDrawDepth++;
        }
    }

    ~ScopedAllowedDesktopTextDraw() {
        if (m_enabled && g_allowedDesktopTextDrawDepth > 0) {
            g_allowedDesktopTextDrawDepth--;
        }
    }

   private:
    bool m_enabled;
};

using BeginPaint_t = decltype(&BeginPaint);
using EndPaint_t = decltype(&EndPaint);
using DrawTextW_t = decltype(&DrawTextW);
using DrawTextExW_t = decltype(&DrawTextExW);
using TextOutW_t = decltype(&TextOutW);
using ExtTextOutW_t = decltype(&ExtTextOutW);
using DrawThemeText_t = decltype(&DrawThemeText);
using DrawThemeTextEx_t = decltype(&DrawThemeTextEx);
using DrawShadowText_t = decltype(&DrawShadowText);

BeginPaint_t BeginPaint_Original;
EndPaint_t EndPaint_Original;
DrawTextW_t DrawTextW_Original;
DrawTextExW_t DrawTextExW_Original;
TextOutW_t TextOutW_Original;
ExtTextOutW_t ExtTextOutW_Original;
DrawThemeText_t DrawThemeText_Original;
DrawThemeTextEx_t DrawThemeTextEx_Original;
DrawShadowText_t DrawShadowText_Original;

void LoadSettings() {
    g_settings.hideLabels = Wh_GetIntSetting(L"hideLabels");
    g_settings.showCaptionOnMouseOver =
        Wh_GetIntSetting(L"showCaptionOnMouseOver");
}

bool IsClassName(HWND hWnd, PCWSTR expectedClassName) {
    WCHAR className[64];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, expectedClassName) == 0;
}

bool IsDesktopRootWindow(HWND hWnd) {
    return IsClassName(hWnd, L"Progman") || IsClassName(hWnd, L"WorkerW");
}

bool IsDesktopListView(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || !IsClassName(hWnd, L"SysListView32")) {
        return false;
    }

    HWND hDefView = GetParent(hWnd);
    if (!hDefView || !IsClassName(hDefView, L"SHELLDLL_DefView")) {
        return false;
    }

    HWND hRoot = GetAncestor(hDefView, GA_ROOT);
    return IsDesktopRootWindow(hRoot);
}

bool GetListViewItemRect(HWND hWnd, int item, int code, RECT* rc) {
    if (item < 0 || !rc) {
        return false;
    }

    rc->left = code;
    return SendMessageW(hWnd, LVM_GETITEMRECT, item, reinterpret_cast<LPARAM>(rc));
}

void InvalidateListViewItemLabel(HWND hWnd, int item) {
    RECT rc;
    if (IsWindow(hWnd) && GetListViewItemRect(hWnd, item, LVIR_LABEL, &rc)) {
        InflateRect(&rc, 3, 3);
        InvalidateRect(hWnd, &rc, TRUE);
        UpdateWindow(hWnd);
    }
}

int HitTestListViewItem(HWND hWnd, POINT ptClient) {
    LVHITTESTINFO hitTest = {};
    hitTest.pt = ptClient;

    int item = static_cast<int>(
        SendMessageW(hWnd, LVM_HITTEST, 0, reinterpret_cast<LPARAM>(&hitTest)));

    if (item < 0 || !(hitTest.flags & LVHT_ONITEM)) {
        return -1;
    }

    return item;
}

void UpdateHoveredListViewItem(HWND hWnd, int item) {
    if (g_hoveredListView == hWnd && g_hoveredItem == item) {
        return;
    }

    if (g_hoveredListView) {
        InvalidateListViewItemLabel(g_hoveredListView, g_hoveredItem);
    }

    g_hoveredListView = hWnd;
    g_hoveredItem = item;

    if (g_hoveredListView) {
        InvalidateListViewItemLabel(g_hoveredListView, g_hoveredItem);
    }
}

LRESULT CALLBACK DesktopListViewSubclassProc(HWND hWnd,
                                             UINT uMsg,
                                             WPARAM wParam,
                                             LPARAM lParam,
                                             DWORD_PTR dwRefData) {
    UNREFERENCED_PARAMETER(dwRefData);

    switch (uMsg) {
        case WM_MOUSEMOVE: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            UpdateHoveredListViewItem(hWnd, HitTestListViewItem(hWnd, pt));

            if (!g_trackingMouseLeave) {
                TRACKMOUSEEVENT trackMouseEvent = {
                    sizeof(trackMouseEvent),
                    TME_LEAVE,
                    hWnd,
                    0,
                };

                if (TrackMouseEvent(&trackMouseEvent)) {
                    g_trackingMouseLeave = true;
                }
            }

            break;
        }

        case WM_MOUSELEAVE:
            g_trackingMouseLeave = false;
            if (g_hoveredListView == hWnd) {
                UpdateHoveredListViewItem(nullptr, -1);
            }
            break;

        case WM_NCDESTROY:
            if (g_hoveredListView == hWnd) {
                g_hoveredListView = nullptr;
                g_hoveredItem = -1;
            }

            RemovePropW(hWnd, kDesktopListViewSubclassProp);
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void EnsureDesktopListViewSubclass(HWND hWnd) {
    if (!g_settings.showCaptionOnMouseOver ||
        GetPropW(hWnd, kDesktopListViewSubclassProp)) {
        return;
    }

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, DesktopListViewSubclassProc, 0)) {
        SetPropW(hWnd, kDesktopListViewSubclassProp, reinterpret_cast<HANDLE>(1));
    }
}

void RemoveDesktopListViewSubclass(HWND hWnd) {
    if (GetPropW(hWnd, kDesktopListViewSubclassProp)) {
        RemovePropW(hWnd, kDesktopListViewSubclassProp);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hWnd, DesktopListViewSubclassProc);
    }
}

void InvalidateDesktopListView(HWND hWnd) {
    if (IsWindow(hWnd)) {
        InvalidateRect(hWnd, nullptr, FALSE);
    }
}

BOOL CALLBACK EnumDesktopListViewsProc(HWND hWnd, LPARAM lParam) {
    if (!IsDesktopRootWindow(hWnd)) {
        return TRUE;
    }

    HWND hDefView = FindWindowExW(hWnd, nullptr, L"SHELLDLL_DefView", nullptr);
    if (!hDefView) {
        return TRUE;
    }

    HWND hListView = FindWindowExW(hDefView, nullptr, L"SysListView32", nullptr);
    if (hListView) {
        auto context = reinterpret_cast<EnumDesktopListViewsContext*>(lParam);
        context->callback(hListView);
    }

    return TRUE;
}

void ForEachDesktopListView(void (*callback)(HWND)) {
    EnumDesktopListViewsContext context = {callback};
    EnumWindows(EnumDesktopListViewsProc, reinterpret_cast<LPARAM>(&context));
}

HWND GetDesktopListViewForDc(HDC hdc) {
    if (g_desktopListViewPaintDepth > 0 &&
        IsDesktopListView(g_currentDesktopListView)) {
        return g_currentDesktopListView;
    }

    HWND hWnd = WindowFromDC(hdc);
    return IsDesktopListView(hWnd) ? hWnd : nullptr;
}

bool ShouldHideDesktopIconText(HDC hdc,
                               const RECT* textRect = nullptr,
                               const POINT* textPt = nullptr) {
    UNREFERENCED_PARAMETER(textRect);
    UNREFERENCED_PARAMETER(textPt);

    if (g_allowedDesktopTextDrawDepth > 0) {
        return false;
    }

    if (!g_settings.hideLabels) {
        return false;
    }

    return GetDesktopListViewForDc(hdc) != nullptr;
}

int RectHeight(const RECT* rc) {
    if (!rc) {
        return 0;
    }

    LONG height = rc->bottom - rc->top;
    return height > 0 ? static_cast<int>(height) : 0;
}

int DrawReadableTextInRect(HDC hdc,
                           LPCWSTR text,
                           int textLength,
                           const RECT* textRect,
                           UINT format) {
    if (!textRect) {
        return 0;
    }

    ScopedAllowedDesktopTextDraw allow(true);
    int savedDc = SaveDC(hdc);
    SetBkMode(hdc, TRANSPARENT);

    UINT drawFormat = format & ~DT_CALCRECT;
    RECT rc;
    int result = 0;

    static const POINT offsets[] = {
        {-1, -1}, {0, -1}, {1, -1}, {-1, 0},
        {1, 0},   {-1, 1}, {0, 1},  {1, 1},
    };

    SetTextColor(hdc, RGB(0, 0, 0));
    for (const POINT& offset : offsets) {
        rc = *textRect;
        OffsetRect(&rc, offset.x, offset.y);
        if (DrawTextW_Original) {
            DrawTextW_Original(hdc, text, textLength, &rc, drawFormat);
        } else {
            DrawTextW(hdc, text, textLength, &rc, drawFormat);
        }
    }

    rc = *textRect;
    SetTextColor(hdc, RGB(255, 255, 255));
    if (DrawTextW_Original) {
        result = DrawTextW_Original(hdc, text, textLength, &rc, drawFormat);
    } else {
        result = DrawTextW(hdc, text, textLength, &rc, drawFormat);
    }

    if (savedDc) {
        RestoreDC(hdc, savedDc);
    }

    return result;
}

HFONT CreateOpaqueIconTitleFont(HWND hWnd) {
    LOGFONTW logFont = {};
    if (SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(logFont), &logFont,
                              0)) {
        logFont.lfQuality = NONANTIALIASED_QUALITY;
        return CreateFontIndirectW(&logFont);
    }

    HFONT hFont = reinterpret_cast<HFONT>(SendMessageW(hWnd, WM_GETFONT, 0, 0));
    if (hFont && GetObjectW(hFont, sizeof(logFont), &logFont) == sizeof(logFont)) {
        logFont.lfQuality = NONANTIALIASED_QUALITY;
        return CreateFontIndirectW(&logFont);
    }

    return nullptr;
}

bool GetListViewItemText(HWND hWnd, int item, PWSTR text, int textLength) {
    if (item < 0 || !text || textLength <= 0) {
        return false;
    }

    text[0] = L'\0';

    LVITEMW lvItem = {};
    lvItem.mask = LVIF_TEXT;
    lvItem.iItem = item;
    lvItem.pszText = text;
    lvItem.cchTextMax = textLength;

    return SendMessageW(hWnd, LVM_GETITEMTEXTW, item,
                        reinterpret_cast<LPARAM>(&lvItem)) > 0;
}

void DrawHoveredListViewCaption(HWND hWnd) {
    if (!g_settings.hideLabels || !g_settings.showCaptionOnMouseOver ||
        g_hoveredListView != hWnd || g_hoveredItem < 0) {
        return;
    }

    RECT labelRect;
    if (!GetListViewItemRect(hWnd, g_hoveredItem, LVIR_LABEL, &labelRect)) {
        return;
    }

    WCHAR text[1024];
    if (!GetListViewItemText(hWnd, g_hoveredItem, text, ARRAYSIZE(text))) {
        return;
    }

    HDC hdc = GetDC(hWnd);
    if (!hdc) {
        return;
    }

    int savedDc = SaveDC(hdc);
    IntersectClipRect(hdc, labelRect.left - 3, labelRect.top - 3,
                      labelRect.right + 3, labelRect.bottom + 3);

    HFONT hFont = CreateOpaqueIconTitleFont(hWnd);
    if (hFont) {
        SelectObject(hdc, hFont);
    }

    UINT format = DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_EDITCONTROL;
    DrawReadableTextInRect(hdc, text, -1, &labelRect, format);

    if (savedDc) {
        RestoreDC(hdc, savedDc);
    }

    if (hFont) {
        DeleteObject(hFont);
    }

    ReleaseDC(hWnd, hdc);
}

HDC WINAPI BeginPaint_Hook(HWND hWnd, LPPAINTSTRUCT lpPaint) {
    HDC hdc = BeginPaint_Original(hWnd, lpPaint);

    if (IsDesktopListView(hWnd)) {
        g_desktopListViewPaintDepth++;
        g_currentDesktopListView = hWnd;
        EnsureDesktopListViewSubclass(hWnd);
    }

    return hdc;
}

BOOL WINAPI EndPaint_Hook(HWND hWnd, const PAINTSTRUCT* lpPaint) {
    bool isDesktopListView = IsDesktopListView(hWnd);
    BOOL result = EndPaint_Original(hWnd, lpPaint);

    if (isDesktopListView && g_desktopListViewPaintDepth > 0) {
        g_desktopListViewPaintDepth--;
        if (g_desktopListViewPaintDepth == 0) {
            g_currentDesktopListView = nullptr;
        }
    }

    if (isDesktopListView) {
        DrawHoveredListViewCaption(hWnd);
    }

    return result;
}

int WINAPI DrawTextW_Hook(HDC hdc,
                          LPCWSTR lpchText,
                          int cchText,
                          LPRECT lprc,
                          UINT format) {
    if (ShouldHideDesktopIconText(hdc, lprc) && !(format & DT_CALCRECT)) {
        return RectHeight(lprc);
    }

    ScopedAllowedDesktopTextDraw allow(GetDesktopListViewForDc(hdc) != nullptr);
    return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
}

int WINAPI DrawTextExW_Hook(HDC hdc,
                            LPWSTR lpchText,
                            int cchText,
                            LPRECT lprc,
                            UINT format,
                            LPDRAWTEXTPARAMS lpdtp) {
    if (ShouldHideDesktopIconText(hdc, lprc) && !(format & DT_CALCRECT)) {
        return RectHeight(lprc);
    }

    ScopedAllowedDesktopTextDraw allow(GetDesktopListViewForDc(hdc) != nullptr);
    return DrawTextExW_Original(hdc, lpchText, cchText, lprc, format, lpdtp);
}

BOOL WINAPI TextOutW_Hook(HDC hdc,
                          int x,
                          int y,
                          LPCWSTR lpString,
                          int c) {
    POINT pt = {x, y};
    if (ShouldHideDesktopIconText(hdc, nullptr, &pt)) {
        return TRUE;
    }

    ScopedAllowedDesktopTextDraw allow(GetDesktopListViewForDc(hdc) != nullptr);
    return TextOutW_Original(hdc, x, y, lpString, c);
}

BOOL WINAPI ExtTextOutW_Hook(HDC hdc,
                             int x,
                             int y,
                             UINT options,
                             const RECT* lprect,
                             LPCWSTR lpString,
                             UINT c,
                             const INT* lpDx) {
    POINT pt = {x, y};
    if (ShouldHideDesktopIconText(hdc, lprect, &pt)) {
        return TRUE;
    }

    ScopedAllowedDesktopTextDraw allow(GetDesktopListViewForDc(hdc) != nullptr);
    return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
}

HRESULT WINAPI DrawThemeText_Hook(HTHEME hTheme,
                                  HDC hdc,
                                  int iPartId,
                                  int iStateId,
                                  LPCWSTR pszText,
                                  int cchText,
                                  DWORD dwTextFlags,
                                  DWORD dwTextFlags2,
                                  LPCRECT pRect) {
    if (ShouldHideDesktopIconText(hdc, pRect) && !(dwTextFlags & DT_CALCRECT)) {
        return S_OK;
    }

    ScopedAllowedDesktopTextDraw allow(GetDesktopListViewForDc(hdc) != nullptr);
    return DrawThemeText_Original(hTheme, hdc, iPartId, iStateId, pszText,
                                  cchText, dwTextFlags, dwTextFlags2, pRect);
}

HRESULT WINAPI DrawThemeTextEx_Hook(HTHEME hTheme,
                                    HDC hdc,
                                    int iPartId,
                                    int iStateId,
                                    LPCWSTR pszText,
                                    int cchText,
                                    DWORD dwTextFlags,
                                    LPRECT pRect,
                                    const DTTOPTS* pOptions) {
    if (ShouldHideDesktopIconText(hdc, pRect) && !(dwTextFlags & DT_CALCRECT)) {
        return S_OK;
    }

    ScopedAllowedDesktopTextDraw allow(GetDesktopListViewForDc(hdc) != nullptr);
    return DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, pszText,
                                    cchText, dwTextFlags, pRect, pOptions);
}

int WINAPI DrawShadowText_Hook(HDC hdc,
                               LPCWSTR pszText,
                               UINT cch,
                               RECT* prc,
                               DWORD dwFlags,
                               COLORREF crText,
                               COLORREF crShadow,
                               int ixOffset,
                               int iyOffset) {
    if (ShouldHideDesktopIconText(hdc, prc) && !(dwFlags & DT_CALCRECT)) {
        return RectHeight(prc);
    }

    ScopedAllowedDesktopTextDraw allow(GetDesktopListViewForDc(hdc) != nullptr);
    return DrawShadowText_Original(hdc, pszText, cch, prc, dwFlags, crText,
                                   crShadow, ixOffset, iyOffset);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    if (g_settings.showCaptionOnMouseOver) {
        ForEachDesktopListView(EnsureDesktopListViewSubclass);
    }

    if (!WindhawkUtils::SetFunctionHook(BeginPaint, BeginPaint_Hook,
                                        &BeginPaint_Original)) {
        Wh_Log(L"Failed to hook BeginPaint");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(EndPaint, EndPaint_Hook,
                                        &EndPaint_Original)) {
        Wh_Log(L"Failed to hook EndPaint");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DrawTextW, DrawTextW_Hook,
                                        &DrawTextW_Original)) {
        Wh_Log(L"Failed to hook DrawTextW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DrawTextExW, DrawTextExW_Hook,
                                        &DrawTextExW_Original)) {
        Wh_Log(L"Failed to hook DrawTextExW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(ExtTextOutW, ExtTextOutW_Hook,
                                        &ExtTextOutW_Original)) {
        Wh_Log(L"Failed to hook ExtTextOutW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(TextOutW, TextOutW_Hook,
                                        &TextOutW_Original)) {
        Wh_Log(L"Failed to hook TextOutW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DrawThemeText, DrawThemeText_Hook,
                                        &DrawThemeText_Original)) {
        Wh_Log(L"Failed to hook DrawThemeText");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DrawThemeTextEx, DrawThemeTextEx_Hook,
                                        &DrawThemeTextEx_Original)) {
        Wh_Log(L"Failed to hook DrawThemeTextEx");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(DrawShadowText, DrawShadowText_Hook,
                                        &DrawShadowText_Original)) {
        Wh_Log(L"Failed to hook DrawShadowText");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
    ForEachDesktopListView(RemoveDesktopListViewSubclass);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();

    if (g_settings.showCaptionOnMouseOver) {
        ForEachDesktopListView(EnsureDesktopListViewSubclass);
    }

    UpdateHoveredListViewItem(nullptr, -1);
    ForEachDesktopListView(InvalidateDesktopListView);
}
