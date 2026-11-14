// ==WindhawkMod==
// @id              color-picker-eyedropper
// @name            Color Picker / Eyedropper Tool
// @description     System-wide color picker overlay
// @version         1.0.0
// @author          Naeru
// @github          https://github.com/nathanaeru
// @include         explorer.exe
// @compilerOptions -lgdi32 -lmsimg32 -ldwmapi -luxtheme
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Color Picker / Eyedropper Tool for Windows 11

A lightweight, system-wide color picker overlay mod for [Windhawk](https://windhawk.net/), inspired by [Microsoft PowerToys](https://github.com/microsoft/powertoys) Color Picker. It allows you to select any pixel on your screen via a customizable global hotkey, view a magnified pixel grid loupe, and automatically obtain the color value in **HEX**, **RGB**, **HSL**, and **CMYK** formats with one-click clipboard copying.

![Color Picker mod in action](https://raw.githubusercontent.com/nathanaeru/color-picker-eyedropper/refs/heads/main/demo.gif)

## Features

- **System-Wide Screen Freeze & Overlay**: Freezes the screen upon activation to prevent accidental click-throughs onto underlying windows.
- **Magnified Loupe Cursor**: Displays a 5x magnified preview grid with a centered crosshair and a live color preview badge that follows your cursor.
- **Multi-Format Support**: Automatically converts and displays captured colors in:
  - **HEX** (e.g., `#FF5733` or `ff5733`)
  - **RGB** (e.g., `rgb(255, 87, 51)`)
  - **HSL** (e.g., `hsl(11, 100%, 60%)`)
  - **CMYK** (e.g., `cmyk(0%, 66%, 80%, 0%)`)
- **Native Windows 11 Dark Mode Support**: The result pop-up window automatically adapts to your system theme (Light or Dark mode).
- **Modern UI Styling**: Clean, dialog-style UI using Segoe UI font, smooth rounded corners, drop-shadow effects, and single-click copy buttons.
- **Customizable Mod Settings**: Directly configure hotkeys and color formatting preferences within the Windhawk UI.


## Hotkey & Controls

- **Activate Eyedropper**: Default hotkey is `Alt + C` (customizable in Windhawk settings).
- **Pick Color**: **Left-Click** anywhere on the screen to capture the pixel color and open the copy window.
- **Cancel Eyedropper**: Press **Escape** or **Right-Click** to exit picking mode without selecting a color.


## Configuration Settings

You can customize the mod settings directly from the Windhawk Mod Manager UI:

| Setting | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| **Hotkey Key** | String | `C` | The main hotkey key character (e.g., `C`, `X`, `V`, `1`). |
| **Alt Key Modifier** | Boolean | `true` | Include `Alt` in the activation shortcut. |
| **Windows Key Modifier** | Boolean | `false` | Include `Win` key in the activation shortcut. |
| **Include '#' Prefix in HEX** | Boolean | `true` | Choose whether HEX values include the `#` symbol (`#CDCDCD` vs `CDCDCD`). |
| **Uppercase HEX Characters** | Boolean | `true` | Toggle uppercase vs. lowercase HEX characters (`#ABCDEF` vs `#abcdef`). |

## Dependencies & Technical Details

- **Target Process**: `explorer.exe`
- **Libraries Linked**: `-lgdi32`, `-lmsimg32`, `-ldwmapi`, `-luxtheme`
- **Architecture**: Pure Win32 GDI & User32 APIs (no heavy external GUI dependencies).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modAlt: true
  $name: Alt Key Modifier
  $description: Use Alt key as hotkey modifier

- modWin: false
  $name: Windows Key Modifier
  $description: Use Windows key as hotkey modifier

- vkCode: C
  $name: Hotkey Key
  $description: Single key character (e.g. C, X, V, 1)

- hexPrefix: true
  $name: Include '#' Prefix in HEX
  $description: Include '#' prefix in HEX color formatting (e.g. '#FF0000' vs 'FF0000')

- hexUppercase: true
  $name: Uppercase HEX Characters
  $description: Format HEX string with uppercase letters (e.g. 'ABCDEF' vs 'abcdef')
*/
// ==/WindhawkModSettings==

#include <math.h>
#include <stdio.h>
#include <windhawk_api.h>
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <uxtheme.h>

#define HOTKEY_ID 1001
#define WM_UPDATE_HOTKEY (WM_USER + 1)

#define IDC_COPY_HEX 2001
#define IDC_COPY_RGB 2002
#define IDC_COPY_HSL 2003
#define IDC_COPY_CMYK 2004

static HHOOK g_hMouseHook = NULL;
static HHOOK g_hKeyboardHook = NULL;
static HWND g_hwndOverlay = NULL;
static HWND g_hwndResult = NULL;
static HWND g_hwndHotkey = NULL;
static HANDLE g_hHotkeyThread = NULL;
static HINSTANCE g_hInstance = NULL;
static HFONT g_hResultFont = NULL;

static BOOL g_isPicking = FALSE;
static COLORREF g_selectedColor = 0;
static POINT g_lastCursorPos = {0, 0};

static wchar_t g_szHex[32] = {0};
static wchar_t g_szRgb[32] = {0};
static wchar_t g_szHsl[32] = {0};
static wchar_t g_szCmyk[32] = {0};

static UINT g_hotkeyModifiers = MOD_ALT;
static UINT g_hotkeyVk = 0x43;

static HDC g_hdcMemScreen = NULL;
static HBITMAP g_hbmScreen = NULL;
static HBITMAP g_hbmOldScreen = NULL;
static int g_vscreenX = 0, g_vscreenY = 0, g_vscreenW = 0, g_vscreenH = 0;

static BOOL g_hexPrefix = TRUE;
static BOOL g_hexUppercase = TRUE;

void FormatHexColor(COLORREF color, wchar_t* outBuf, size_t bufSize) {
    BYTE r = GetRValue(color);
    BYTE g = GetGValue(color);
    BYTE b = GetBValue(color);

    const wchar_t* fmt;
    if (g_hexPrefix) {
        fmt = g_hexUppercase ? L"#%02X%02X%02X" : L"#%02x%02x%02x";
    } else {
        fmt = g_hexUppercase ? L"%02X%02X%02X" : L"%02x%02x%02x";
    }

    swprintf_s(outBuf, bufSize, fmt, r, g, b);
}

void LoadHotkeySettings() {
    UINT mods = 0;
    if (Wh_GetIntSetting(L"modWin")) mods |= MOD_WIN;
    if (Wh_GetIntSetting(L"modAlt")) mods |= MOD_ALT;
    g_hotkeyModifiers = mods;

    PCWSTR szVk = Wh_GetStringSetting(L"vkCode");
    if (szVk && szVk[0] != L'\0') {
        wchar_t ch = szVk[0];
        if (ch >= L'a' && ch <= L'z') {
            g_hotkeyVk = (UINT)(ch - L'a' + 'A');
        } else {
            g_hotkeyVk = (UINT)ch;
        }
        Wh_FreeStringSetting(szVk);
    } else {
        g_hotkeyVk = 0x43;
    }

    g_hexPrefix = Wh_GetIntSetting(L"hexPrefix");
    g_hexUppercase = Wh_GetIntSetting(L"hexUppercase");
}

void UpdateRegisteredHotkey() {
    if (g_hwndHotkey) {
        UnregisterHotKey(g_hwndHotkey, HOTKEY_ID);
        RegisterHotKey(g_hwndHotkey, HOTKEY_ID, g_hotkeyModifiers, g_hotkeyVk);
    }
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd,
                                UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam);
LRESULT CALLBACK ResultWndProc(HWND hwnd,
                               UINT uMsg,
                               WPARAM wParam,
                               LPARAM lParam);

void RGBtoHSL(COLORREF color, int* outH, int* outS, int* outL) {
    BYTE r = GetRValue(color);
    BYTE g = GetGValue(color);
    BYTE b = GetBValue(color);

    float rf = r / 255.0f;
    float gf = g / 255.0f;
    float bf = b / 255.0f;

    float maxVal = (rf > gf) ? ((rf > bf) ? rf : bf) : ((gf > bf) ? gf : bf);
    float minVal = (rf < gf) ? ((rf < bf) ? rf : bf) : ((gf < bf) ? gf : bf);
    float delta = maxVal - minVal;

    float h = 0.0f;
    float s = 0.0f;
    float l = (maxVal + minVal) / 2.0f;

    if (delta != 0.0f) {
        s = (l > 0.5f) ? (delta / (2.0f - maxVal - minVal))
                       : (delta / (maxVal + minVal));
        if (maxVal == rf) {
            h = (gf - bf) / delta + (gf < bf ? 6.0f : 0.0f);
        } else if (maxVal == gf) {
            h = (bf - rf) / delta + 2.0f;
        } else {
            h = (rf - gf) / delta + 4.0f;
        }
        h /= 6.0f;
    }

    *outH = (int)(h * 360.0f + 0.5f);
    *outS = (int)(s * 100.0f + 0.5f);
    *outL = (int)(l * 100.0f + 0.5f);
}

void RGBtoCMYK(COLORREF color, int* outC, int* outM, int* outY, int* outK) {
    BYTE r = GetRValue(color);
    BYTE g = GetGValue(color);
    BYTE b = GetBValue(color);

    float rf = r / 255.0f;
    float gf = g / 255.0f;
    float bf = b / 255.0f;

    float maxVal = (rf > gf) ? ((rf > bf) ? rf : bf) : ((gf > bf) ? gf : bf);
    float k = 1.0f - maxVal;

    if (k >= 0.9999f) {
        *outC = 0;
        *outM = 0;
        *outY = 0;
        *outK = 100;
        return;
    }

    float c = (1.0f - rf - k) / (1.0f - k);
    float m = (1.0f - gf - k) / (1.0f - k);
    float y = (1.0f - bf - k) / (1.0f - k);

    *outC = (int)(c * 100.0f + 0.5f);
    *outM = (int)(m * 100.0f + 0.5f);
    *outY = (int)(y * 100.0f + 0.5f);
    *outK = (int)(k * 100.0f + 0.5f);
}

void CopyToClipboard(HWND hwnd, const wchar_t* text) {
    if (!OpenClipboard(hwnd))
        return;
    EmptyClipboard();
    size_t len = (wcslen(text) + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    if (hMem) {
        void* pMem = GlobalLock(hMem);
        if (pMem) {
            memcpy(pMem, text, len);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        }
    }
    CloseClipboard();
}

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

static HBRUSH g_hDarkBgBrush = NULL;
static HBRUSH g_hDarkControlBrush = NULL;

static BOOL IsDarkModeEnabled() {
    DWORD data = 1;
    DWORD size = sizeof(data);
    LONG res = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_REG_DWORD,
        NULL,
        &data,
        &size
    );
    return (res == ERROR_SUCCESS) && (data == 0);
}

static BOOL CALLBACK SetChildFontProc(HWND hChild, LPARAM lParam) {
    SendMessageW(hChild, WM_SETFONT, (WPARAM)lParam, TRUE);
    return TRUE;
}

static BOOL CALLBACK SetChildThemeProc(HWND hChild, LPARAM lParam) {
    SetWindowTheme(hChild, L"DarkMode_Explorer", NULL);
    return TRUE;
}

LRESULT CALLBACK ResultWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            BYTE r = GetRValue(g_selectedColor);
            BYTE g = GetGValue(g_selectedColor);
            BYTE b = GetBValue(g_selectedColor);

            int h = 0, s = 0, l = 0;
            int c = 0, m = 0, y = 0, k = 0;
            RGBtoHSL(g_selectedColor, &h, &s, &l);
            RGBtoCMYK(g_selectedColor, &c, &m, &y, &k);

            FormatHexColor(g_selectedColor, g_szHex, 32);
            swprintf_s(g_szRgb, 32, L"rgb(%d, %d, %d)", r, g, b);
            swprintf_s(g_szHsl, 32, L"hsl(%d, %d%%, %d%%)", h, s, l);
            swprintf_s(g_szCmyk, 32, L"cmyk(%d%%, %d%%, %d%%, %d%%)", c, m, y, k);

            CreateWindowW(L"STATIC", L"HEX:", WS_CHILD | WS_VISIBLE, 20, 80, 50, 20, hwnd, NULL, NULL, NULL);
            CreateWindowW(L"EDIT", g_szHex, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY, 80, 77, 180, 24, hwnd, NULL, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Copy", WS_CHILD | WS_VISIBLE, 270, 76, 70, 26, hwnd, (HMENU)IDC_COPY_HEX, NULL, NULL);

            CreateWindowW(L"STATIC", L"RGB:", WS_CHILD | WS_VISIBLE, 20, 115, 50, 20, hwnd, NULL, NULL, NULL);
            CreateWindowW(L"EDIT", g_szRgb, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY, 80, 112, 180, 24, hwnd, NULL, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Copy", WS_CHILD | WS_VISIBLE, 270, 111, 70, 26, hwnd, (HMENU)IDC_COPY_RGB, NULL, NULL);

            CreateWindowW(L"STATIC", L"HSL:", WS_CHILD | WS_VISIBLE, 20, 150, 50, 20, hwnd, NULL, NULL, NULL);
            CreateWindowW(L"EDIT", g_szHsl, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY, 80, 147, 180, 24, hwnd, NULL, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Copy", WS_CHILD | WS_VISIBLE, 270, 146, 70, 26, hwnd, (HMENU)IDC_COPY_HSL, NULL, NULL);

            CreateWindowW(L"STATIC", L"CMYK:", WS_CHILD | WS_VISIBLE, 20, 185, 50, 20, hwnd, NULL, NULL, NULL);
            CreateWindowW(L"EDIT", g_szCmyk, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY, 80, 182, 180, 24, hwnd, NULL, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Copy", WS_CHILD | WS_VISIBLE, 270, 181, 70, 26, hwnd, (HMENU)IDC_COPY_CMYK, NULL, NULL);

            NONCLIENTMETRICSW ncm = { sizeof(NONCLIENTMETRICSW) };
            SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, 0);
            g_hResultFont = CreateFontIndirectW(&ncm.lfMessageFont);

            EnumChildWindows(hwnd, SetChildFontProc, (LPARAM)g_hResultFont);

            if (IsDarkModeEnabled()) {
                BOOL useDarkMode = TRUE;
                DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));

                g_hDarkBgBrush = CreateSolidBrush(RGB(32, 32, 32));
                g_hDarkControlBrush = CreateSolidBrush(RGB(45, 45, 45));

                EnumChildWindows(hwnd, SetChildThemeProc, 0);
            }

            break;
        }
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (IsDarkModeEnabled() && g_hDarkBgBrush) {
                FillRect(hdc, &rc, g_hDarkBgBrush);
            } else {
                FillRect(hdc, &rc, (HBRUSH)(COLOR_BTNFACE + 1));
            }
            return 1;
        }
        case WM_CTLCOLORSTATIC: {
            if (IsDarkModeEnabled() && g_hDarkBgBrush && g_hDarkControlBrush) {
                HDC hdcChild = (HDC)wParam;
                HWND hwndChild = (HWND)lParam;

                wchar_t szClassName[32] = {0};
                GetClassNameW(hwndChild, szClassName, 32);

                if (_wcsicmp(szClassName, L"STATIC") == 0) {
                    SetTextColor(hdcChild, RGB(240, 240, 240));
                    SetBkColor(hdcChild, RGB(32, 32, 32));
                    return (INT_PTR)g_hDarkBgBrush;
                } else {
                    SetTextColor(hdcChild, RGB(240, 240, 240));
                    SetBkColor(hdcChild, RGB(45, 45, 45));
                    return (INT_PTR)g_hDarkControlBrush;
                }
            }
            break;
        }
        case WM_CTLCOLOREDIT: {
            if (IsDarkModeEnabled() && g_hDarkControlBrush) {
                HDC hdcChild = (HDC)wParam;
                SetTextColor(hdcChild, RGB(240, 240, 240));
                SetBkColor(hdcChild, RGB(45, 45, 45));
                return (INT_PTR)g_hDarkControlBrush;
            }
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            BOOL isDark = IsDarkModeEnabled();
            COLORREF frameColor = isDark ? RGB(70, 70, 70) : RGB(180, 180, 180);

            HRGN hRgn = CreateRoundRectRgn(20, 15, 341, 56, 8, 8);
            HBRUSH hBrush = CreateSolidBrush(g_selectedColor);
            FillRgn(hdc, hRgn, hBrush);
            DeleteObject(hBrush);

            HPEN hFramePen = CreatePen(PS_SOLID, 1, frameColor);
            HGDIOBJ hOldPen = SelectObject(hdc, hFramePen);
            HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            RoundRect(hdc, 20, 15, 341, 56, 8, 8);

            SelectObject(hdc, hOldPen);
            SelectObject(hdc, hOldBrush);
            DeleteObject(hFramePen);
            DeleteObject(hRgn);

            EndPaint(hwnd, &ps);
            break;
        }
        case WM_COMMAND: {
            WORD id = LOWORD(wParam);
            if (id == IDC_COPY_HEX) CopyToClipboard(hwnd, g_szHex);
            else if (id == IDC_COPY_RGB) CopyToClipboard(hwnd, g_szRgb);
            else if (id == IDC_COPY_HSL) CopyToClipboard(hwnd, g_szHsl);
            else if (id == IDC_COPY_CMYK) CopyToClipboard(hwnd, g_szCmyk);
            break;
        }
        case WM_DESTROY: {
            if (g_hResultFont) {
                DeleteObject(g_hResultFont);
                g_hResultFont = NULL;
            }
            if (g_hDarkBgBrush) {
                DeleteObject(g_hDarkBgBrush);
                g_hDarkBgBrush = NULL;
            }
            if (g_hDarkControlBrush) {
                DeleteObject(g_hDarkControlBrush);
                g_hDarkControlBrush = NULL;
            }
            g_hwndResult = NULL;
            break;
        }
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void RegisterWindowClasses() {
    HINSTANCE hInst = g_hInstance;

    WNDCLASSW wc = {0};
    if (!GetClassInfoW(hInst, L"WindhawkColorResultWnd", &wc)) {
        wc.lpfnWndProc = ResultWndProc;
        wc.hInstance = hInst;
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszClassName = L"WindhawkColorResultWnd";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);
    }

    WNDCLASSW wcOverlay = {0};
    if (!GetClassInfoW(hInst, L"WindhawkColorPickerOverlay", &wcOverlay)) {
        wcOverlay.lpfnWndProc = OverlayWndProc;
        wcOverlay.hInstance = hInst;
        wcOverlay.lpszClassName = L"WindhawkColorPickerOverlay";
        wcOverlay.hCursor = LoadCursor(NULL, IDC_CROSS);
        RegisterClassW(&wcOverlay);
    }
}

void ShowResultWindow() {
    if (g_hwndResult) {
        DestroyWindow(g_hwndResult);
        g_hwndResult = NULL;
    }

    g_hwndResult = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_DLGMODALFRAME,
        L"WindhawkColorResultWnd",
        L"Color Picker - Selected Color",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        (GetSystemMetrics(SM_CXSCREEN) - 375) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - 260) / 2,
        375, 260,
        NULL, NULL, g_hInstance, NULL
    );

    SetWindowPos(g_hwndResult, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            SetTimer(hwnd, 1, 16, NULL);
            break;
        }
        case WM_TIMER: {
            if (wParam == 1 && g_isPicking) {
                POINT pt;
                GetCursorPos(&pt);
                if (pt.x != g_lastCursorPos.x || pt.y != g_lastCursorPos.y) {
                    g_lastCursorPos = pt;
                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
                }
            }
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdcDst = BeginPaint(hwnd, &ps);

            if (g_hdcMemScreen) {
                HDC hdcMem = CreateCompatibleDC(hdcDst);
                HBITMAP hbmMem = CreateCompatibleBitmap(hdcDst, g_vscreenW, g_vscreenH);
                HBITMAP hbmOldMem = (HBITMAP)SelectObject(hdcMem, hbmMem);

                BitBlt(hdcMem, 0, 0, g_vscreenW, g_vscreenH, g_hdcMemScreen, 0, 0, SRCCOPY);

                int relX = g_lastCursorPos.x - g_vscreenX;
                int relY = g_lastCursorPos.y - g_vscreenY;

                int boxX = relX + 15;
                int boxY = relY + 15;
                if (boxX + 132 > g_vscreenW) boxX = relX - 147;
                if (boxY + 160 > g_vscreenH) boxY = relY - 175;

                int shadowOffsetX = 3;
                int shadowOffsetY = 4;
                int blurRadius = 8;
                int shadowW = 133 + blurRadius * 2;
                int shadowH = 161 + blurRadius * 2;

                HDC hdcShadow = CreateCompatibleDC(hdcMem);
                BITMAPINFO bmi = {{0}};
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = shadowW;
                bmi.bmiHeader.biHeight = -shadowH;
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;

                DWORD* pBits = NULL;
                HBITMAP hbmShadow = CreateDIBSection(hdcShadow, &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
                if (hbmShadow && pBits) {
                    HBITMAP hbmOldShadow = (HBITMAP)SelectObject(hdcShadow, hbmShadow);

                    for (int y = 0; y < shadowH; y++) {
                        for (int x = 0; x < shadowW; x++) {
                            int px = x - blurRadius;
                            int py = y - blurRadius;

                            int cx = fmax(0, abs(px - 66) - (66 - 14));
                            int cy = fmax(0, abs(py - 80) - (80 - 14));
                            float dist = sqrtf((float)(cx * cx + cy * cy)) - 14.0f;

                            BYTE alpha = 0;
                            if (dist <= 0) {
                                alpha = 65;
                            } else if (dist < (float)blurRadius) {
                                float factor = 1.0f - (dist / (float)blurRadius);
                                alpha = (BYTE)(65.0f * factor * factor);
                            }

                            pBits[y * shadowW + x] = ((DWORD)alpha << 24);
                        }
                    }

                    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                    AlphaBlend(hdcMem, boxX - blurRadius + shadowOffsetX, boxY - blurRadius + shadowOffsetY, shadowW, shadowH, hdcShadow, 0, 0, shadowW, shadowH, bf);

                    SelectObject(hdcShadow, hbmOldShadow);
                    DeleteObject(hbmShadow);
                }
                DeleteDC(hdcShadow);

                HRGN hRgn = CreateRoundRectRgn(boxX, boxY, boxX + 133, boxY + 161, 14, 14);
                SelectClipRgn(hdcMem, hRgn);

                StretchBlt(hdcMem, boxX, boxY, 132, 132, g_hdcMemScreen, relX - 5, relY - 5, 11, 11, SRCCOPY);

                COLORREF currColor = GetPixel(g_hdcMemScreen, relX, relY);

                HPEN hBlackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                HPEN hWhitePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
                HGDIOBJ hOldPen = SelectObject(hdcMem, hBlackPen);
                HGDIOBJ hOldBrush = SelectObject(hdcMem, GetStockObject(NULL_BRUSH));

                Rectangle(hdcMem, boxX + 59, boxY + 59, boxX + 73, boxY + 73);
                SelectObject(hdcMem, hWhitePen);
                Rectangle(hdcMem, boxX + 58, boxY + 58, boxX + 74, boxY + 74);

                RECT rcText = {boxX, boxY + 132, boxX + 132, boxY + 160};
                HBRUSH hBgBrush = CreateSolidBrush(currColor);
                FillRect(hdcMem, &rcText, hBgBrush);
                DeleteObject(hBgBrush);

                BYTE r = GetRValue(currColor);
                BYTE g = GetGValue(currColor);
                BYTE b = GetBValue(currColor);
                wchar_t szHex[16];
                FormatHexColor(currColor, szHex, 16);

                COLORREF textColor = (r * 0.299 + g * 0.587 + b * 0.114 > 128) ? RGB(0, 0, 0) : RGB(255, 255, 255);
                SetTextColor(hdcMem, textColor);
                SetBkMode(hdcMem, TRANSPARENT);
                DrawTextW(hdcMem, szHex, -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                SelectObject(hdcMem, hOldPen);
                SelectObject(hdcMem, hOldBrush);
                DeleteObject(hBlackPen);
                DeleteObject(hWhitePen);

                SelectClipRgn(hdcMem, NULL);

                HPEN hBorderPen = CreatePen(PS_SOLID, 1, RGB(180, 180, 180));
                hOldPen = SelectObject(hdcMem, hBorderPen);
                hOldBrush = SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
                RoundRect(hdcMem, boxX, boxY, boxX + 133, boxY + 161, 14, 14);

                SelectObject(hdcMem, hOldPen);
                SelectObject(hdcMem, hOldBrush);
                DeleteObject(hBorderPen);
                DeleteObject(hRgn);

                BitBlt(hdcDst, 0, 0, g_vscreenW, g_vscreenH, hdcMem, 0, 0, SRCCOPY);

                SelectObject(hdcMem, hbmOldMem);
                DeleteObject(hbmMem);
                DeleteDC(hdcMem);
            }

            EndPaint(hwnd, &ps);
            break;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY:
            KillTimer(hwnd, 1);
            break;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void StopColorPicker() {
    if (!g_isPicking) return;
    g_isPicking = FALSE;

    if (g_hMouseHook) {
        UnhookWindowsHookEx(g_hMouseHook);
        g_hMouseHook = NULL;
    }
    if (g_hKeyboardHook) {
        UnhookWindowsHookEx(g_hKeyboardHook);
        g_hKeyboardHook = NULL;
    }
    if (g_hwndOverlay) {
        DestroyWindow(g_hwndOverlay);
        g_hwndOverlay = NULL;
    }
    if (g_hdcMemScreen) {
        SelectObject(g_hdcMemScreen, g_hbmOldScreen);
        DeleteObject(g_hbmScreen);
        DeleteDC(g_hdcMemScreen);
        g_hdcMemScreen = NULL;
        g_hbmScreen = NULL;
        g_hbmOldScreen = NULL;
    }
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_isPicking) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;
        if (wParam == WM_LBUTTONDOWN) {
            if (g_hdcMemScreen) {
                int relX = pMouse->pt.x - g_vscreenX;
                int relY = pMouse->pt.y - g_vscreenY;
                g_selectedColor = GetPixel(g_hdcMemScreen, relX, relY);
            }

            StopColorPicker();
            ShowResultWindow();
            return 1;
        } else if (wParam == WM_RBUTTONDOWN) {
            StopColorPicker();
            return 1;
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_isPicking) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        if (pKey->vkCode == VK_ESCAPE) {
            StopColorPicker();
            return 1;
        }
    }
    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

void StartColorPicker() {
    if (g_isPicking) return;
    g_isPicking = TRUE;

    GetCursorPos(&g_lastCursorPos);
    RegisterWindowClasses();

    g_vscreenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    g_vscreenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    g_vscreenW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    g_vscreenH = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HDC hdcScreen = GetDC(NULL);
    g_hdcMemScreen = CreateCompatibleDC(hdcScreen);
    g_hbmScreen = CreateCompatibleBitmap(hdcScreen, g_vscreenW, g_vscreenH);
    g_hbmOldScreen = (HBITMAP)SelectObject(g_hdcMemScreen, g_hbmScreen);
    BitBlt(g_hdcMemScreen, 0, 0, g_vscreenW, g_vscreenH, hdcScreen, g_vscreenX, g_vscreenY, SRCCOPY);
    ReleaseDC(NULL, hdcScreen);

    g_hwndOverlay = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"WindhawkColorPickerOverlay",
        NULL,
        WS_POPUP | WS_VISIBLE,
        g_vscreenX, g_vscreenY, g_vscreenW, g_vscreenH,
        NULL, NULL, g_hInstance, NULL
    );

    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, g_hInstance, 0);
    if (!g_hMouseHook) {
        g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
    }

    g_hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, g_hInstance, 0);
    if (!g_hKeyboardHook) {
        g_hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    }
}

LRESULT CALLBACK HotkeyWndProc(HWND hwnd,
                               UINT uMsg,
                               WPARAM wParam,
                               LPARAM lParam) {
    if (uMsg == WM_HOTKEY && wParam == HOTKEY_ID) {
        StartColorPicker();
        return 0;
    }
    if (uMsg == WM_UPDATE_HOTKEY) {
        UnregisterHotKey(hwnd, HOTKEY_ID);
        RegisterHotKey(hwnd, HOTKEY_ID, g_hotkeyModifiers | MOD_NOREPEAT,
                       g_hotkeyVk);
        return 0;
    }
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI HotkeyThreadProc(LPVOID lpParam) {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = HotkeyWndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"WindhawkColorPickerHotkeyWnd";
    RegisterClassW(&wc);

    g_hwndHotkey =
        CreateWindowExW(0, L"WindhawkColorPickerHotkeyWnd", NULL, 0, 0, 0, 0, 0,
                        HWND_MESSAGE, NULL, g_hInstance, NULL);

    LoadHotkeySettings();
    RegisterHotKey(g_hwndHotkey, HOTKEY_ID, g_hotkeyModifiers | MOD_NOREPEAT,
                   g_hotkeyVk);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnregisterHotKey(g_hwndHotkey, HOTKEY_ID);
    return 0;
}

BOOL Wh_ModInit() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&Wh_ModInit, &g_hInstance);
    RegisterWindowClasses();
    g_hHotkeyThread = CreateThread(NULL, 0, HotkeyThreadProc, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadHotkeySettings();
    if (g_hwndHotkey) {
        PostMessageW(g_hwndHotkey, WM_UPDATE_HOTKEY, 0, 0);
    }
}

void Wh_ModUninit() {
    StopColorPicker();
    if (g_hwndHotkey) {
        PostMessageW(g_hwndHotkey, WM_CLOSE, 0, 0);
    }
    if (g_hHotkeyThread) {
        WaitForSingleObject(g_hHotkeyThread, 1000);
        CloseHandle(g_hHotkeyThread);
        g_hHotkeyThread = NULL;
    }
    if (g_hwndResult) {
        DestroyWindow(g_hwndResult);
    }
}