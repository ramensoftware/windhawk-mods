// ==WindhawkMod==
// @id              remove-clock-cutout-background
// @name            Remove Legacy Clock Square Cutout
// @description     Removes the light square background behind the analog clock in StartAllBack and Date/Time dialogs with flicker-free OS composition.
// @version         3.1.0
// @author          awnaise
// @github          https://github.com/awnaise
// @include         explorer.exe
// @include         rundll32.exe
// @compilerOptions -lgdiplus -lcomctl32 -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Removes the light square background behind the analog clock in StartAllBack
and classic Windows Date/Time flyouts (timedate.cpl) using OS-level 
double buffering (WS_EX_COMPOSITED) and GDI+ sub-pixel anti-aliased corner 
masking with smart dark-mode dialog sampling.

### Screenshots

#### StartAllBack Clock Flyout

**Before:**
![Before Flyout](https://i.imgur.com/f4Ku2hE.png)

**After:**
![After Flyout](https://i.imgur.com/IYWKYWx.png)

#### Date & Time Dialog (timedate.cpl)

**Before:**
![Before Date and Time](https://i.imgur.com/xzkz1NF.png)

**After:**
![After Date and Time](https://i.imgur.com/ug6TxAf.png)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <windhawk_utils.h>
#include <mutex>
#include <unordered_map>
#include <vector>

using namespace Gdiplus;

static ULONG_PTR g_gdiplusToken = 0;

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t pfnCreateWindowExWOriginal = NULL;

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t pfnShowWindowOriginal = NULL;

thread_local bool g_bInMask = false;

// Global state tracking for subclassed windows: HWND -> added WS_EX_COMPOSITED flag
std::mutex g_clockWindowsMutex;
std::unordered_map<HWND, bool> g_clockWindows;

// Known clock window classes that should strictly be targeted
bool IsKnownClockClass(const wchar_t* szClass) {
    if (!szClass) return false;
    return (wcsicmp(szClass, L"ClockWndMain") == 0 ||
            wcsicmp(szClass, L"ClockCtl") == 0 ||
            wcsicmp(szClass, L"ClockWndClass") == 0);
}

// Targets analog clock controls (StartAllBack flyout + timedate.cpl ClockWndMain/ClockCtl)
bool IsAnalogClockControl(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;

    DWORD style = GetWindowLongW(hwnd, GWL_STYLE);
    if (!(style & WS_CHILD)) return false;

    wchar_t szClass[256] = {0};
    if (GetClassNameW(hwnd, szClass, 256) <= 0) return false;

    // Explicitly exclude non-analog digital time pickers and tray clock controls
    if (wcsstr(szClass, L"TrayClock") != NULL) return false;
    if (wcsstr(szClass, L"DigitalClock") != NULL) return false;
    if (wcsicmp(szClass, L"SysDateTimePick32") == 0) return false;

    // Strict class matching only to avoid painting over unrelated third-party controls
    return IsKnownClockClass(szClass);
}

// Dynamically samples the parent container's actual background pixel color
COLORREF GetParentBgColor(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return GetSysColor(COLOR_3DFACE);

    HWND hParent = GetParent(hwnd);
    HWND hRoot = GetAncestor(hwnd, GA_ROOT);
    if (!hParent) hParent = hwnd;
    if (!hRoot) hRoot = hParent;

    // 1. Try querying the parent control/dialog brush
    HDC hdcChild = GetDC(hwnd);
    if (hdcChild) {
        HBRUSH hbr = (HBRUSH)SendMessageW(hParent, WM_CTLCOLORSTATIC, (WPARAM)hdcChild, (LPARAM)hwnd);
        if (!hbr) hbr = (HBRUSH)SendMessageW(hParent, WM_CTLCOLORDLG, (WPARAM)hdcChild, (LPARAM)hParent);
        if (hbr) {
            LOGBRUSH lb{};
            if (GetObject(hbr, sizeof(lb), &lb) && lb.lbStyle == BS_SOLID) {
                if (lb.lbColor != RGB(255, 255, 255)) {
                    ReleaseDC(hwnd, hdcChild);
                    return lb.lbColor;
                }
            }
        }
        ReleaseDC(hwnd, hdcChild);
    }

    // 2. Sample safe empty background regions inside the dialog
    HDC hdcScreen = GetDC(NULL);
    if (hdcScreen) {
        RECT rcChild, rcParent, rcRoot;
        GetWindowRect(hwnd, &rcChild);
        GetWindowRect(hParent, &rcParent);
        GetWindowRect(hRoot, &rcRoot);

        // Detect if running in Dark Mode by sampling root window bottom margin
        COLORREF rootClr = GetPixel(hdcScreen, rcRoot.left + 20, rcRoot.bottom - 15);
        bool isDarkMode = false;
        if (rootClr != CLR_INVALID) {
            BYTE r = GetRValue(rootClr);
            BYTE g = GetGValue(rootClr);
            BYTE b = GetBValue(rootClr);
            if (r < 180 && g < 180 && b < 180) {
                isDarkMode = true;
            }
        }

        int w = rcChild.right - rcChild.left;

        POINT candidates[] = {
            { rcParent.right - 30, rcParent.bottom - 25 },
            { rcParent.left + 30,  rcParent.bottom - 25 },
            { rcChild.left + (w / 2), rcChild.bottom + 25 },
            { rcParent.right - 30, rcParent.top + 35 },
            { rcRoot.left + 20,    rcRoot.bottom - 15 }
        };

        for (const auto& pt : candidates) {
            if (PtInRect(&rcRoot, pt)) {
                COLORREF clr = GetPixel(hdcScreen, pt.x, pt.y);
                if (clr != CLR_INVALID) {
                    BYTE r = GetRValue(clr);
                    BYTE g = GetGValue(clr);
                    BYTE b = GetBValue(clr);

                    if (isDarkMode && (r > 200 && g > 200 && b > 200)) {
                        continue;
                    }

                    ReleaseDC(NULL, hdcScreen);
                    return clr;
                }
            }
        }

        ReleaseDC(NULL, hdcScreen);

        if (isDarkMode && rootClr != CLR_INVALID) {
            return rootClr;
        }
    }

    return GetSysColor(COLOR_3DFACE);
}

// Applies anti-aliased GDI+ corner mask
void ApplySmoothMaskToDC(HDC hdc, HWND hwnd, int w, int h) {
    if (!hdc || w <= 20 || h <= 20 || g_bInMask) return;
    g_bInMask = true;

    COLORREF bgClr = GetParentBgColor(hwnd);

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

    GraphicsPath path(FillModeAlternate);
    path.AddRectangle(Rect(0, 0, w, h));

    wchar_t szClass[256] = {0};
    GetClassNameW(hwnd, szClass, 256);

    REAL insetL = 0.5f;
    REAL insetT = 0.5f;
    REAL ew = (REAL)w;
    REAL eh = (REAL)h;

    // Handle timedate.cpl ClockWndMain (142x144 canvas with a 130x130 clock face anchored upper-left)
    if (wcsicmp(szClass, L"ClockWndMain") == 0) {
        UINT dpi = GetDpiForWindow(hwnd);
        if (dpi == 0) dpi = 96;
        int nominalFace = MulDiv(130, (int)dpi, 96);

        if (w >= nominalFace + 6 && h >= nominalFace + 6) {
            ew = (REAL)nominalFace;
            eh = (REAL)nominalFace;
        } else {
            ew = (REAL)w - 1.0f;
            eh = (REAL)h - 1.0f;
        }
    } else {
        bool isClassicClock = (wcsicmp(szClass, L"ClockCtl") == 0);
        insetL = isClassicClock ? 1.0f : 0.5f;
        insetT = isClassicClock ? 1.0f : 0.5f;
        REAL insetR = isClassicClock ? 1.0f : 1.8f; 
        REAL insetB = isClassicClock ? 1.0f : 1.8f; 

        ew = (REAL)w - (insetL + insetR);
        eh = (REAL)h - (insetT + insetB);
    }

    RectF ellipseRect(insetL, insetT, ew, eh);
    path.AddEllipse(ellipseRect);

    SolidBrush brush(Color(255, GetRValue(bgClr), GetGValue(bgClr), GetBValue(bgClr)));
    graphics.FillPath(&brush, &path);

    Pen pen(Color(255, GetRValue(bgClr), GetGValue(bgClr), GetBValue(bgClr)), 1.2f);
    graphics.DrawEllipse(&pen, ellipseRect);

    g_bInMask = false;
}

LRESULT CALLBACK ClockSubclassProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData
) {
    switch (uMsg) {
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            if (hdc) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                COLORREF bgClr = GetParentBgColor(hwnd);
                HBRUSH hbr = CreateSolidBrush(bgClr);
                if (hbr) {
                    FillRect(hdc, &rc, hbr);
                    DeleteObject(hbr);
                }
            }
            return TRUE;
        }

        case WM_PAINT: {
            LRESULT lRes = DefSubclassProc(hwnd, uMsg, wParam, lParam);

            RECT rc;
            if (GetClientRect(hwnd, &rc)) {
                int w = rc.right - rc.left;
                int h = rc.bottom - rc.top;
                HDC hdc = GetDC(hwnd);
                if (hdc) {
                    ApplySmoothMaskToDC(hdc, hwnd, w, h);
                    ReleaseDC(hwnd, hdc);
                }
            }
            return lRes;
        }

        case WM_PRINTCLIENT: {
            LRESULT res = DefSubclassProc(hwnd, uMsg, wParam, lParam);
            HDC hdc = (HDC)wParam;
            if (hdc) {
                RECT rc;
                if (GetClientRect(hwnd, &rc)) {
                    ApplySmoothMaskToDC(hdc, hwnd, rc.right - rc.left, rc.bottom - rc.top);
                }
            }
            return res;
        }

        case WM_NCDESTROY: {
            std::lock_guard<std::mutex> lock(g_clockWindowsMutex);
            g_clockWindows.erase(hwnd);
            break;
        }
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

void AttachIfClock(HWND hwnd) {
    if (!IsAnalogClockControl(hwnd)) return;

    DWORD exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    bool addedComposited = !(exStyle & WS_EX_COMPOSITED);

    {
        std::lock_guard<std::mutex> lock(g_clockWindowsMutex);
        if (!g_clockWindows.try_emplace(hwnd, addedComposited).second) return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, ClockSubclassProc, 0)) {
        std::lock_guard<std::mutex> lock(g_clockWindowsMutex);
        g_clockWindows.erase(hwnd);
        return;
    }

    if (addedComposited) {
        SetWindowLongW(hwnd, GWL_EXSTYLE, exStyle | WS_EX_COMPOSITED);
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

BOOL CALLBACK EnumFlyoutChildrenProc(HWND hChild, LPARAM lParam) {
    AttachIfClock(hChild);
    return TRUE;
}

void AttachToClockWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    AttachIfClock(hwnd);
    EnumChildWindows(hwnd, EnumFlyoutChildrenProc, 0);
}

HWND WINAPI HookedCreateWindowExW(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam
) {
    HWND hwnd = pfnCreateWindowExWOriginal(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );

    if (hwnd) {
        AttachToClockWindow(hwnd);
    }
    return hwnd;
}

BOOL WINAPI HookedShowWindow(HWND hWnd, int nCmdShow) {
    BOOL result = pfnShowWindowOriginal(hWnd, nCmdShow);

    if (nCmdShow != SW_HIDE && hWnd && IsWindow(hWnd)) {
        AttachToClockWindow(hWnd);
    }
    return result;
}

BOOL CALLBACK ResetWindowRegionsProc(HWND hwnd, LPARAM lParam) {
    if (GetWindowThreadProcessId(hwnd, NULL) == GetCurrentProcessId()) {
        AttachToClockWindow(hwnd);
    }
    return TRUE;
}

BOOL Wh_ModInit() {
    GdiplusStartupInput gdiplusStartupInput;
    if (GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL) != Ok) {
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(CreateWindowExW, HookedCreateWindowExW, &pfnCreateWindowExWOriginal);
    WindhawkUtils::SetFunctionHook(ShowWindow, HookedShowWindow, &pfnShowWindowOriginal);

    EnumWindows(ResetWindowRegionsProc, 0);
    Wh_Log(L"Clock mask mod initialized successfully");

    return TRUE;
}

void Wh_ModUninit() {
    std::vector<std::pair<HWND, bool>> windows;
    {
        std::lock_guard<std::mutex> lock(g_clockWindowsMutex);
        windows.assign(g_clockWindows.begin(), g_clockWindows.end());
        g_clockWindows.clear();
    }

    for (auto [hwnd, addedComposited] : windows) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ClockSubclassProc);
        if (addedComposited) {
            SetWindowLongW(hwnd, GWL_EXSTYLE,
                           GetWindowLongW(hwnd, GWL_EXSTYLE) & ~WS_EX_COMPOSITED);
        }
        InvalidateRect(hwnd, NULL, TRUE);
    }

    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}
