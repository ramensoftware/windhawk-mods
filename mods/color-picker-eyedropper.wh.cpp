// ==WindhawkMod==
// @id              color-picker-eyedropper
// @name            Color Picker / Eyedropper Tool
// @description     System-wide color picker overlay
// @version         1.0.1
// @author          Naeru
// @github          https://github.com/nathanaeru
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lmsimg32 -ldwmapi -luxtheme -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Color Picker / Eyedropper Tool for Windows 11

A lightweight, system-wide color picker overlay mod for [Windhawk](https://windhawk.net/), inspired by [Microsoft PowerToys](https://github.com/microsoft/powertoys) Color Picker. It allows you to select any pixel on your screen via a customizable global hotkey, view a magnified pixel grid loupe, and automatically obtain the color value in **HEX**, **RGB**, **HSL**, and **CMYK** formats with one-click clipboard copying.

![Color Picker mod in action](https://raw.githubusercontent.com/nathanaeru/color-picker-eyedropper/refs/heads/main/demo.gif)

## Features

- **System-Wide Screen Freeze & Overlay**: Freezes the screen upon activation to prevent accidental click-throughs onto underlying windows.
- **Magnified Loupe Cursor**: Displays a magnified preview grid with a centered crosshair and a live color preview badge that follows your cursor.
- **Multi-Format Support**: Automatically converts and displays captured colors in:
  - **HEX** (e.g., `#FF5733` or `ff5733`)
  - **RGB** (e.g., `rgb(255, 87, 51)`)
  - **HSL** (e.g., `hsl(11, 100%, 60%)`)
  - **CMYK** (e.g., `cmyk(0%, 66%, 80%, 0%)`)
- **Native Windows 11 Dark Mode Support**: The result pop-up window automatically adapts to your system theme (Light or Dark mode).
- **Modern UI Styling**: Clean, dialog-style UI using Segoe UI font, smooth rounded corners, drop-shadow effects, and single-click copy buttons.
- **Customizable Mod Settings**: Directly configure hotkeys and color formatting preferences within the Windhawk UI.


## Hotkey & Controls

- **Activate Eyedropper**: Default hotkey is `Win + Alt + C` (customizable in Windhawk settings).
- **Pick Color**: **Left-Click** anywhere on the screen to capture the pixel color and open the copy window.
- **Cancel Eyedropper**: Press **Escape** or **Right-Click** to exit picking mode without selecting a color.


## Configuration Settings

You can customize the mod settings directly from the Windhawk Mod Manager UI:

| Setting | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| **Hotkey Key** | String | `C` | Single alphanumeric or symbol character used with `Win + Alt`. |
| **Include '#' Prefix in HEX** | Boolean | `true` | Choose whether HEX values include the `#` symbol (`#CDCDCD` vs `CDCDCD`). |
| **Uppercase HEX Characters** | Boolean | `true` | Toggle uppercase vs. lowercase HEX characters (`#ABCDEF` vs `#abcdef`). |

## Dependencies & Technical Details

- **Target Process**: `windhawk.exe` (runs as a Windhawk tool mod)
- **Libraries Linked**: `-lgdi32`, `-lmsimg32`, `-ldwmapi`, `-luxtheme`, `-lshcore`
- **Architecture**: Pure Win32 GDI & User32 APIs (no heavy external GUI dependencies).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- vkCode: C
  $name: Hotkey Key
  $description: Single alphanumeric or symbol character used with Win+Alt

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
#include <windows.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <uxtheme.h>

#define HOTKEY_ID 1001
#define WM_UPDATE_HOTKEY (WM_USER + 1)
#define WM_PICK_COMPLETE (WM_USER + 2)
#define WM_PICK_CANCEL   (WM_USER + 3)

#define IDC_COPY_HEX 2001
#define IDC_COPY_RGB 2002
#define IDC_COPY_HSL 2003
#define IDC_COPY_CMYK 2004
#define IDC_LABEL_HEX 2011
#define IDC_EDIT_HEX 2012
#define IDC_LABEL_RGB 2021
#define IDC_EDIT_RGB 2022
#define IDC_LABEL_HSL 2031
#define IDC_EDIT_HSL 2032
#define IDC_LABEL_CMYK 2041
#define IDC_EDIT_CMYK 2042

static HWND g_hwndOverlay = NULL;
static HWND g_hwndResult = NULL;
static HWND g_hwndHotkey = NULL;
static HHOOK g_hMouseHook = NULL;
static HHOOK g_hKeyboardHook = NULL;
static HANDLE g_hHotkeyThread = NULL;
static HANDLE g_hHotkeyReadyEvent = NULL;
static HINSTANCE g_hInstance = NULL;
static HFONT g_hResultFont = NULL;

static BOOL g_isPicking = FALSE;
static BOOL g_isDarkMode = FALSE;
static COLORREF g_selectedColor = 0;
static POINT g_lastCursorPos = {0, 0};

static wchar_t g_szHex[32] = {0};
static wchar_t g_szRgb[32] = {0};
static wchar_t g_szHsl[32] = {0};
static wchar_t g_szCmyk[32] = {0};

static UINT g_hotkeyVk = 0x43;
static UINT g_hotkeyKeyModifiers = 0;

static HDC g_hdcMemScreen = NULL;
static HBITMAP g_hbmScreen = NULL;
static HBITMAP g_hbmOldScreen = NULL;
static int g_vscreenX = 0, g_vscreenY = 0, g_vscreenW = 0, g_vscreenH = 0;

static BOOL g_hexPrefix = TRUE;
static BOOL g_hexUppercase = TRUE;
static UINT g_overlayDpi = USER_DEFAULT_SCREEN_DPI;

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

static HBRUSH g_hDarkBgBrush = NULL;
static HBRUSH g_hDarkControlBrush = NULL;

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
    PCWSTR szVk = Wh_GetStringSetting(L"vkCode");
    wchar_t ch = szVk[0];
    BOOL validCharacter = (ch != L'\0' && szVk[1] == L'\0' && ch >= L'!' && ch <= L'~');
    Wh_FreeStringSetting(szVk);

    if (!validCharacter) {
        g_hotkeyVk = 0x43;
        g_hotkeyKeyModifiers = 0;
    } else if ((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')) {
        g_hotkeyVk = (ch >= L'a' && ch <= L'z') ? (UINT)(ch - L'a' + L'A') : (UINT)ch;
        g_hotkeyKeyModifiers = 0;
    } else {
        SHORT key = VkKeyScanW(ch);
        if (key == -1) {
            g_hotkeyVk = 0x43;
            g_hotkeyKeyModifiers = 0;
        } else {
            g_hotkeyVk = LOBYTE(key);
            BYTE keyModifiers = HIBYTE(key);
            g_hotkeyKeyModifiers = 0;
            if (keyModifiers & 1) g_hotkeyKeyModifiers |= MOD_SHIFT;
            if (keyModifiers & 2) g_hotkeyKeyModifiers |= MOD_CONTROL;
            if (keyModifiers & 4) g_hotkeyKeyModifiers |= MOD_ALT;
        }
    }

    g_hexPrefix = Wh_GetIntSetting(L"hexPrefix");
    g_hexUppercase = Wh_GetIntSetting(L"hexUppercase");
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ResultWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HotkeyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void StopColorPicker();

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
        s = (l > 0.5f) ? (delta / (2.0f - maxVal - minVal)) : (delta / (maxVal + minVal));
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
    if (!OpenClipboard(hwnd)) return;
    EmptyClipboard();
    size_t len = (wcslen(text) + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    if (hMem) {
        void* pMem = GlobalLock(hMem);
        if (pMem) {
            memcpy(pMem, text, len);
            GlobalUnlock(hMem);
            if (!SetClipboardData(CF_UNICODETEXT, hMem)) {
                GlobalFree(hMem);
            }
        } else {
            GlobalFree(hMem);
        }
    }
    CloseClipboard();
}

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

static int ScaleForDpi(int value, UINT dpi) {
    return MulDiv(value, dpi, 96);
}

static int MinInt(int a, int b) {
    return a < b ? a : b;
}

static int MaxInt(int a, int b) {
    return a > b ? a : b;
}

static void LayoutResultWindow(HWND hwnd, UINT dpi) {
    const int ids[] = {
        IDC_LABEL_HEX, IDC_EDIT_HEX, IDC_COPY_HEX,
        IDC_LABEL_RGB, IDC_EDIT_RGB, IDC_COPY_RGB,
        IDC_LABEL_HSL, IDC_EDIT_HSL, IDC_COPY_HSL,
        IDC_LABEL_CMYK, IDC_EDIT_CMYK, IDC_COPY_CMYK,
    };
    const int positions[][4] = {
        {20, 80, 50, 20}, {80, 77, 180, 24}, {270, 76, 70, 26},
        {20, 115, 50, 20}, {80, 112, 180, 24}, {270, 111, 70, 26},
        {20, 150, 50, 20}, {80, 147, 180, 24}, {270, 146, 70, 26},
        {20, 185, 50, 20}, {80, 182, 180, 24}, {270, 181, 70, 26},
    };

    for (size_t i = 0; i < ARRAYSIZE(ids); i++) {
        HWND child = GetDlgItem(hwnd, ids[i]);
        if (child) {
            MoveWindow(child, ScaleForDpi(positions[i][0], dpi),
                       ScaleForDpi(positions[i][1], dpi),
                       ScaleForDpi(positions[i][2], dpi),
                       ScaleForDpi(positions[i][3], dpi), TRUE);
        }
    }

    NONCLIENTMETRICSW ncm = { sizeof(NONCLIENTMETRICSW) };
    if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi)) {
        HFONT newFont = CreateFontIndirectW(&ncm.lfMessageFont);
        if (newFont) {
            HFONT oldFont = g_hResultFont;
            g_hResultFont = newFont;
            EnumChildWindows(hwnd, SetChildFontProc, (LPARAM)newFont);
            if (oldFont) {
                DeleteObject(oldFont);
            }
        }
    }
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

            CreateWindowW(L"STATIC", L"HEX:", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_LABEL_HEX, NULL, NULL);
            CreateWindowW(L"EDIT", g_szHex, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY, 0, 0, 0, 0, hwnd, (HMENU)IDC_EDIT_HEX, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Copy", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_COPY_HEX, NULL, NULL);

            CreateWindowW(L"STATIC", L"RGB:", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_LABEL_RGB, NULL, NULL);
            CreateWindowW(L"EDIT", g_szRgb, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY, 0, 0, 0, 0, hwnd, (HMENU)IDC_EDIT_RGB, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Copy", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_COPY_RGB, NULL, NULL);

            CreateWindowW(L"STATIC", L"HSL:", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_LABEL_HSL, NULL, NULL);
            CreateWindowW(L"EDIT", g_szHsl, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY, 0, 0, 0, 0, hwnd, (HMENU)IDC_EDIT_HSL, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Copy", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_COPY_HSL, NULL, NULL);

            CreateWindowW(L"STATIC", L"CMYK:", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_LABEL_CMYK, NULL, NULL);
            CreateWindowW(L"EDIT", g_szCmyk, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY, 0, 0, 0, 0, hwnd, (HMENU)IDC_EDIT_CMYK, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Copy", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_COPY_CMYK, NULL, NULL);

            UINT dpi = GetDpiForWindow(hwnd);
            LayoutResultWindow(hwnd, dpi);

            g_isDarkMode = IsDarkModeEnabled();
            if (g_isDarkMode) {
                BOOL useDarkMode = TRUE;
                DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));

                g_hDarkBgBrush = CreateSolidBrush(RGB(32, 32, 32));
                g_hDarkControlBrush = CreateSolidBrush(RGB(45, 45, 45));

                EnumChildWindows(hwnd, SetChildThemeProc, 0);
            }
            break;
        }
        case WM_DPICHANGED: {
            UINT dpi = HIWORD(wParam);
            RECT* suggested = (RECT*)lParam;
            SetWindowPos(hwnd, NULL, suggested->left, suggested->top,
                         suggested->right - suggested->left,
                         suggested->bottom - suggested->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            LayoutResultWindow(hwnd, dpi);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (g_isDarkMode && g_hDarkBgBrush) {
                FillRect(hdc, &rc, g_hDarkBgBrush);
            } else {
                FillRect(hdc, &rc, (HBRUSH)(COLOR_BTNFACE + 1));
            }
            return 1;
        }
        case WM_CTLCOLORSTATIC: {
            if (g_isDarkMode && g_hDarkBgBrush && g_hDarkControlBrush) {
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
            if (g_isDarkMode && g_hDarkControlBrush) {
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

            COLORREF frameColor = g_isDarkMode ? RGB(70, 70, 70) : RGB(180, 180, 180);

            UINT dpi = GetDpiForWindow(hwnd);
            int x = ScaleForDpi(20, dpi);
            int y = ScaleForDpi(15, dpi);
            int width = ScaleForDpi(321, dpi);
            int height = ScaleForDpi(41, dpi);
            int corner = ScaleForDpi(8, dpi);
            HRGN hRgn = CreateRoundRectRgn(x, y, x + width, y + height, corner, corner);
            HBRUSH hBrush = CreateSolidBrush(g_selectedColor);
            FillRgn(hdc, hRgn, hBrush);
            DeleteObject(hBrush);

            HPEN hFramePen = CreatePen(PS_SOLID, 1, frameColor);
            HGDIOBJ hOldPen = SelectObject(hdc, hFramePen);
            HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            RoundRect(hdc, x, y, x + width, y + height, corner, corner);

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

BOOL RegisterWindowClasses() {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = ResultWndProc;
    wc.hInstance = g_hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"WindhawkColorResultWnd";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (!RegisterClassW(&wc)) return FALSE;

    WNDCLASSW wcOverlay = {0};
    wcOverlay.lpfnWndProc = OverlayWndProc;
    wcOverlay.hInstance = g_hInstance;
    wcOverlay.lpszClassName = L"WindhawkColorPickerOverlay";
    wcOverlay.hCursor = LoadCursor(NULL, IDC_CROSS);
    if (!RegisterClassW(&wcOverlay)) {
        UnregisterClassW(L"WindhawkColorResultWnd", g_hInstance);
        return FALSE;
    }

    WNDCLASSW wcHotkey = {0};
    wcHotkey.lpfnWndProc = HotkeyWndProc;
    wcHotkey.hInstance = g_hInstance;
    wcHotkey.lpszClassName = L"WindhawkColorPickerHotkeyWnd";
    if (!RegisterClassW(&wcHotkey)) {
        UnregisterClassW(L"WindhawkColorPickerOverlay", g_hInstance);
        UnregisterClassW(L"WindhawkColorResultWnd", g_hInstance);
        return FALSE;
    }
    return TRUE;
}

void UnregisterWindowClasses() {
    UnregisterClassW(L"WindhawkColorPickerHotkeyWnd", g_hInstance);
    UnregisterClassW(L"WindhawkColorPickerOverlay", g_hInstance);
    UnregisterClassW(L"WindhawkColorResultWnd", g_hInstance);
}

void ShowResultWindow() {
    if (g_hwndResult) {
        DestroyWindow(g_hwndResult);
        g_hwndResult = NULL;
    }

    const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
    const DWORD exStyle = WS_EX_TOPMOST | WS_EX_DLGMODALFRAME;
    HMONITOR monitor = MonitorFromPoint(g_lastCursorPos, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = USER_DEFAULT_SCREEN_DPI;
    UINT dpiY = USER_DEFAULT_SCREEN_DPI;
    UINT dpi = SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) ? dpiX : GetDpiForSystem();
    RECT windowRect = {0, 0, ScaleForDpi(360, dpi), ScaleForDpi(220, dpi)};
    AdjustWindowRectExForDpi(&windowRect, style, FALSE, exStyle, dpi);

    MONITORINFO monitorInfo = {sizeof(monitorInfo)};
    RECT workArea = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        workArea = monitorInfo.rcWork;
    }

    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    g_hwndResult = CreateWindowExW(
        exStyle,
        L"WindhawkColorResultWnd",
        L"Color Picker - Selected Color",
        style,
        workArea.left + ((workArea.right - workArea.left) - width) / 2,
        workArea.top + ((workArea.bottom - workArea.top) - height) / 2,
        width, height,
        NULL, NULL, g_hInstance, NULL
    );
}

static RECT GetLoupeRect(POINT cursor) {
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info = {sizeof(info)};
    RECT bounds = {g_vscreenX, g_vscreenY, g_vscreenX + g_vscreenW, g_vscreenY + g_vscreenH};
    if (GetMonitorInfoW(monitor, &info)) {
        bounds = info.rcMonitor;
    }
    UINT dpiX = USER_DEFAULT_SCREEN_DPI;
    UINT dpiY = USER_DEFAULT_SCREEN_DPI;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        g_overlayDpi = dpiX;
    }
    UINT dpi = g_overlayDpi ? g_overlayDpi : USER_DEFAULT_SCREEN_DPI;
    int width = ScaleForDpi(132, dpi);
    int height = ScaleForDpi(160, dpi);
    int offset = ScaleForDpi(15, dpi);

    int left = cursor.x + offset;
    int top = cursor.y + offset;
    if (left + width > bounds.right) left = cursor.x - offset - width;
    if (top + height > bounds.bottom) top = cursor.y - offset - height;
    left = MaxInt(bounds.left, MinInt(left, bounds.right - width));
    top = MaxInt(bounds.top, MinInt(top, bounds.bottom - height));

    RECT result = {left, top, left + width, top + height};
    return result;
}

static void InvalidateLoupe(HWND hwnd, POINT oldPt, POINT newPt) {
    RECT rc1 = GetLoupeRect(oldPt);
    RECT rc2 = GetLoupeRect(newPt);
    OffsetRect(&rc1, -g_vscreenX, -g_vscreenY);
    OffsetRect(&rc2, -g_vscreenX, -g_vscreenY);
    InflateRect(&rc1, 15, 15);
    InflateRect(&rc2, 15, 15);
    InvalidateRect(hwnd, &rc1, FALSE);
    InvalidateRect(hwnd, &rc2, FALSE);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            g_overlayDpi = GetDpiForWindow(hwnd);
            return 0;
        case WM_DPICHANGED:
            g_overlayDpi = HIWORD(wParam);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (g_hdcMemScreen) {
                int paintWidth = ps.rcPaint.right - ps.rcPaint.left;
                int paintHeight = ps.rcPaint.bottom - ps.rcPaint.top;
                BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, paintWidth, paintHeight, g_hdcMemScreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

                RECT box = GetLoupeRect(g_lastCursorPos);
                int boxX = box.left - g_vscreenX;
                int boxY = box.top - g_vscreenY;
                int boxW = box.right - box.left;
                int boxH = box.bottom - box.top;
                int dpi = (int)g_overlayDpi;
                int blurRadius = ScaleForDpi(8, dpi);
                int shadowW = boxW + blurRadius * 2;
                int shadowH = boxH + blurRadius * 2;

                HDC hdcShadow = CreateCompatibleDC(hdc);
                BITMAPINFO bmi = {{0}};
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = shadowW;
                bmi.bmiHeader.biHeight = -shadowH;
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;

                DWORD* bits = NULL;
                HBITMAP hbmShadow = CreateDIBSection(hdcShadow, &bmi, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
                if (hbmShadow && bits) {
                    HBITMAP oldShadow = (HBITMAP)SelectObject(hdcShadow, hbmShadow);
                    int roundRadius = ScaleForDpi(14, dpi);
                    for (int y = 0; y < shadowH; y++) {
                        for (int x = 0; x < shadowW; x++) {
                            int px = x - blurRadius;
                            int py = y - blurRadius;
                            int cx = MaxInt(0, abs(px - boxW / 2) - (boxW / 2 - roundRadius));
                            int cy = MaxInt(0, abs(py - boxH / 2) - (boxH / 2 - roundRadius));
                            float distance = sqrtf((float)(cx * cx + cy * cy)) - roundRadius;
                            BYTE alpha = 0;
                            if (distance <= 0) {
                                alpha = 65;
                            } else if (distance < blurRadius) {
                                float factor = 1.0f - distance / blurRadius;
                                alpha = (BYTE)(65.0f * factor * factor);
                            }
                            bits[y * shadowW + x] = (DWORD)alpha << 24;
                        }
                    }
                    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                    int shadowOffset = ScaleForDpi(3, dpi);
                    AlphaBlend(hdc, boxX - blurRadius + shadowOffset, boxY - blurRadius + shadowOffset, shadowW, shadowH, hdcShadow, 0, 0, shadowW, shadowH, blend);
                    SelectObject(hdcShadow, oldShadow);
                    DeleteObject(hbmShadow);
                }
                DeleteDC(hdcShadow);

                int corner = ScaleForDpi(14, dpi);
                HRGN clip = CreateRoundRectRgn(boxX, boxY, boxX + boxW + 1, boxY + boxH + 1, corner, corner);
                SelectClipRgn(hdc, clip);

                int relX = g_lastCursorPos.x - g_vscreenX;
                int relY = g_lastCursorPos.y - g_vscreenY;
                int gridSize = ScaleForDpi(132, dpi);
                int sourceSize = ScaleForDpi(11, dpi);
                StretchBlt(hdc, boxX, boxY, gridSize, gridSize, g_hdcMemScreen, relX - sourceSize / 2, relY - sourceSize / 2, sourceSize, sourceSize, SRCCOPY);
                COLORREF color = GetPixel(g_hdcMemScreen, relX, relY);

                HPEN blackPen = CreatePen(PS_SOLID, ScaleForDpi(1, dpi), RGB(0, 0, 0));
                HPEN whitePen = CreatePen(PS_SOLID, ScaleForDpi(1, dpi), RGB(255, 255, 255));
                HGDIOBJ oldPen = SelectObject(hdc, blackPen);
                HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
                int centerX = boxX + gridSize / 2;
                int centerY = boxY + gridSize / 2;
                int marker = ScaleForDpi(7, dpi);
                Rectangle(hdc, centerX - marker, centerY - marker, centerX + marker, centerY + marker);
                SelectObject(hdc, whitePen);
                marker = ScaleForDpi(8, dpi);
                Rectangle(hdc, centerX - marker, centerY - marker, centerX + marker, centerY + marker);

                RECT textRect = {boxX, boxY + gridSize, boxX + boxW, boxY + boxH};
                HBRUSH bgBrush = CreateSolidBrush(color);
                FillRect(hdc, &textRect, bgBrush);
                DeleteObject(bgBrush);

                BYTE r = GetRValue(color);
                BYTE g = GetGValue(color);
                BYTE b = GetBValue(color);
                wchar_t hex[16];
                FormatHexColor(color, hex, ARRAYSIZE(hex));
                SetTextColor(hdc, (r * 0.299 + g * 0.587 + b * 0.114 > 128) ? RGB(0, 0, 0) : RGB(255, 255, 255));
                SetBkMode(hdc, TRANSPARENT);
                HFONT font = CreateFontW(-ScaleForDpi(10, dpi), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
                HGDIOBJ oldFont = font ? SelectObject(hdc, font) : NULL;
                DrawTextW(hdc, hex, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                if (font) {
                    SelectObject(hdc, oldFont);
                    DeleteObject(font);
                }

                SelectObject(hdc, oldPen);
                SelectObject(hdc, oldBrush);
                DeleteObject(blackPen);
                DeleteObject(whitePen);
                SelectClipRgn(hdc, NULL);

                HPEN borderPen = CreatePen(PS_SOLID, ScaleForDpi(1, dpi), RGB(180, 180, 180));
                oldPen = SelectObject(hdc, borderPen);
                oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
                RoundRect(hdc, boxX, boxY, boxX + boxW, boxY + boxH, corner, corner);
                SelectObject(hdc, oldPen);
                SelectObject(hdc, oldBrush);
                DeleteObject(borderPen);
                DeleteObject(clip);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

void StopColorPicker() {
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
        if (g_hbmOldScreen) {
            SelectObject(g_hdcMemScreen, g_hbmOldScreen);
        }
        DeleteDC(g_hdcMemScreen);
        g_hdcMemScreen = NULL;
    }
    if (g_hbmScreen) {
        DeleteObject(g_hbmScreen);
        g_hbmScreen = NULL;
    }
    g_hbmOldScreen = NULL;
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_isPicking && g_hwndOverlay) {
        const MSLLHOOKSTRUCT* mouse = (const MSLLHOOKSTRUCT*)lParam;
        if (wParam == WM_MOUSEMOVE) {
            POINT previous = g_lastCursorPos;
            POINT current = mouse->pt;
            if (current.x != previous.x || current.y != previous.y) {
                g_lastCursorPos = current;
                InvalidateLoupe(g_hwndOverlay, previous, current);
            }
        } else if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN) {
            return 1;
        } else if (wParam == WM_LBUTTONUP) {
            g_lastCursorPos = mouse->pt;
            int x = g_lastCursorPos.x - g_vscreenX;
            int y = g_lastCursorPos.y - g_vscreenY;
            if (g_hdcMemScreen && x >= 0 && x < g_vscreenW && y >= 0 && y < g_vscreenH) {
                g_selectedColor = GetPixel(g_hdcMemScreen, x, y);
            }
            PostMessageW(g_hwndHotkey, WM_PICK_COMPLETE, 0, 0);
            return 1;
        } else if (wParam == WM_RBUTTONUP) {
            PostMessageW(g_hwndHotkey, WM_PICK_CANCEL, 0, 0);
            return 1;
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_isPicking && g_hwndHotkey && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        const KBDLLHOOKSTRUCT* key = (const KBDLLHOOKSTRUCT*)lParam;
        if (key->vkCode == VK_ESCAPE) {
            PostMessageW(g_hwndHotkey, WM_PICK_CANCEL, 0, 0);
            return 1;
        }
    }
    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

void StartColorPicker() {
    if (g_isPicking) return;

    GetCursorPos(&g_lastCursorPos);
    g_vscreenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    g_vscreenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    g_vscreenW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    g_vscreenH = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HDC screen = GetDC(NULL);
    if (!screen) return;

    g_hdcMemScreen = CreateCompatibleDC(screen);
    g_hbmScreen = CreateCompatibleBitmap(screen, g_vscreenW, g_vscreenH);
    if (!g_hdcMemScreen || !g_hbmScreen) {
        ReleaseDC(NULL, screen);
        StopColorPicker();
        return;
    }

    g_hbmOldScreen = (HBITMAP)SelectObject(g_hdcMemScreen, g_hbmScreen);
    if (!g_hbmOldScreen || !BitBlt(g_hdcMemScreen, 0, 0, g_vscreenW, g_vscreenH, screen, g_vscreenX, g_vscreenY, SRCCOPY)) {
        ReleaseDC(NULL, screen);
        StopColorPicker();
        return;
    }
    ReleaseDC(NULL, screen);

    g_isPicking = TRUE;
    g_hwndOverlay = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"WindhawkColorPickerOverlay", NULL, WS_POPUP | WS_VISIBLE,
        g_vscreenX, g_vscreenY, g_vscreenW, g_vscreenH,
        NULL, NULL, g_hInstance, NULL
    );

    if (!g_hwndOverlay) {
        StopColorPicker();
        return;
    }

    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, g_hInstance, 0);
    g_hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, g_hInstance, 0);

    InvalidateRect(g_hwndOverlay, NULL, FALSE);
}

LRESULT CALLBACK HotkeyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_HOTKEY && wParam == HOTKEY_ID) {
        StartColorPicker();
        return 0;
    }
    if (uMsg == WM_PICK_COMPLETE) {
        StopColorPicker();
        ShowResultWindow();
        return 0;
    }
    if (uMsg == WM_PICK_CANCEL) {
        StopColorPicker();
        return 0;
    }
    if (uMsg == WM_UPDATE_HOTKEY) {
        UnregisterHotKey(hwnd, HOTKEY_ID);
        LoadHotkeySettings();
        UINT modifiers = MOD_WIN | MOD_ALT | g_hotkeyKeyModifiers | MOD_NOREPEAT;
        RegisterHotKey(hwnd, HOTKEY_ID, modifiers, g_hotkeyVk);
        return 0;
    }
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI HotkeyThreadProc(LPVOID lpParam) {
    g_hwndHotkey = CreateWindowExW(0, L"WindhawkColorPickerHotkeyWnd", NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, g_hInstance, NULL);
    if (!g_hwndHotkey) {
        SetEvent(g_hHotkeyReadyEvent);
        return 1;
    }

    LoadHotkeySettings();
    UINT modifiers = MOD_WIN | MOD_ALT | g_hotkeyKeyModifiers | MOD_NOREPEAT;
    RegisterHotKey(g_hwndHotkey, HOTKEY_ID, modifiers, g_hotkeyVk);
    SetEvent(g_hHotkeyReadyEvent);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnregisterHotKey(g_hwndHotkey, HOTKEY_ID);
    StopColorPicker();
    if (g_hwndResult) {
        DestroyWindow(g_hwndResult);
        g_hwndResult = NULL;
    }
    g_hwndHotkey = NULL;
    return 0;
}

BOOL WhTool_ModInit() {
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&WhTool_ModInit, &g_hInstance)) {
        return FALSE;
    }
    if (!RegisterWindowClasses()) {
        return FALSE;
    }

    g_hHotkeyReadyEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_hHotkeyReadyEvent) {
        UnregisterWindowClasses();
        return FALSE;
    }

    g_hHotkeyThread = CreateThread(NULL, 0, HotkeyThreadProc, NULL, 0, NULL);
    if (!g_hHotkeyThread) {
        CloseHandle(g_hHotkeyReadyEvent);
        g_hHotkeyReadyEvent = NULL;
        UnregisterWindowClasses();
        return FALSE;
    }

    WaitForSingleObject(g_hHotkeyReadyEvent, INFINITE);
    CloseHandle(g_hHotkeyReadyEvent);
    g_hHotkeyReadyEvent = NULL;

    if (!g_hwndHotkey) {
        WaitForSingleObject(g_hHotkeyThread, INFINITE);
        CloseHandle(g_hHotkeyThread);
        g_hHotkeyThread = NULL;
        UnregisterWindowClasses();
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_hwndHotkey) {
        PostMessageW(g_hwndHotkey, WM_UPDATE_HOTKEY, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_hwndHotkey) {
        PostMessageW(g_hwndHotkey, WM_CLOSE, 0, 0);
    }
    if (g_hHotkeyThread) {
        WaitForSingleObject(g_hHotkeyThread, INFINITE);
        CloseHandle(g_hHotkeyThread);
        g_hHotkeyThread = NULL;
    }
    UnregisterWindowClasses();
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) return FALSE;

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 || wcscmp(argv[i], L"-service-start") == 0 || wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) return FALSE;

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) ExitProcess(1);
        if (GetLastError() == ERROR_ALREADY_EXISTS) ExitProcess(1);
        if (!WhTool_ModInit()) ExitProcess(1);

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) return FALSE;

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) return;
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
        WINBOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW = (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) return;

    STARTUPINFO si{.cb = sizeof(STARTUPINFO), .dwFlags = STARTF_FORCEOFFFEEDBACK};
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr)) {
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModUninit();
    ExitProcess(0);
}