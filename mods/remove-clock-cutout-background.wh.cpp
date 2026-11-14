// ==WindhawkMod==
// @id         remove-clock-cutout-background
// @name       Remove Legacy Clock Square Cutout & Animated Custom Image
// @description    Removes the light square background behind the analog clock in StartAllBack and Date/Time dialogs, with full animated GIF and PNG replacement support.
// @version        3.2.1
// @author         awnaise
// @github         https://github.com/awnaise
// @include        explorer.exe
// @include        rundll32.exe
// @compilerOptions -lgdiplus -lcomctl32 -lgdi32 -luser32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- customImagePath: ""
  $name: "Custom Clock Image (Path to PNG or Animated GIF)"
  $description: "Absolute path to a static image or animated GIF (PNG, JPEG, BMP, etc.) to replace the clock face. Leave empty to only remove the square cutout background."
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
Removes the light square background behind the analog clock in StartAllBack
and classic Windows Date/Time flyouts (timedate.cpl) using OS-level 
double buffering (WS_EX_COMPOSITED) and GDI+ sub-pixel anti-aliased corner 
masking with smart dark-mode dialog sampling, plus full animated GIF and PNG replacement support.

### Transparent PNG Support

Now with transparent PNG support!

**Before:**
![Before Transparent PNG](https://i.imgur.com/aC8dRN0.png)

**After:**
![After Transparent PNG](https://i.imgur.com/KoSYtFz.png)

### Custom Image / GIF Demonstration

![Custom Image Demo](https://i.imgur.com/qO1AHCG.gif)

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
#include <shlwapi.h>
#include <windhawk_utils.h>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <optional>

using namespace Gdiplus;

static ULONG_PTR g_gdiplusToken = 0;

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t pfnCreateWindowExWOriginal = NULL;

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t pfnShowWindowOriginal = NULL;

thread_local bool g_bInMask = false;

// State tracking for subclassed windows and border management
struct ClockWindowState {
    bool addedComposited;
    bool removedBorder;
};

std::mutex g_clockWindowsMutex;
std::unordered_map<HWND, ClockWindowState> g_clockWindows;

// Cached image structure for animated GIF support
struct CachedImage {
    std::unique_ptr<Image> img;
    UINT frameCount = 1;
    std::vector<UINT> delays;
};

// Safe destruction under process exit using [[clang::no_destroy]]
[[clang::no_destroy]] std::optional<std::unordered_map<std::wstring, CachedImage>> g_imageCache{std::in_place};
std::mutex g_imageMutex;

// Per-window animation state
struct WindowAnimState {
    std::wstring imagePath;
    UINT currentFrame = 0;
};

std::unordered_map<HWND, WindowAnimState> g_windowAnims;
std::mutex g_windowAnimsMutex;

// Helper to toggle and refresh window borders cleanly
void SetBorder(HWND hwnd, bool enable) {
    DWORD style = GetWindowLongW(hwnd, GWL_STYLE);
    SetWindowLongW(hwnd, GWL_STYLE,
                   enable ? (style | WS_BORDER) : (style & ~WS_BORDER));
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOACTIVATE);
}

// Retrieve or load cached image from memory stream to prevent file locking
CachedImage* GetCachedImage(const wchar_t* path) {
    std::lock_guard<std::mutex> lock(g_imageMutex);
    auto it = g_imageCache->find(path);
    if (it != g_imageCache->end()) {
        return &it->second;
    }

    HANDLE file = CreateFileW(path, GENERIC_READ,
                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                            nullptr, OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE) return nullptr;

    std::vector<BYTE> data;
    LARGE_INTEGER size{};
    DWORD read = 0;
    if (GetFileSizeEx(file, &size) && size.QuadPart > 0 && size.QuadPart < 64 * 1024 * 1024) {
        data.resize((size_t)size.QuadPart);
        if (!ReadFile(file, data.data(), (DWORD)data.size(), &read, nullptr) || read != data.size()) {
            data.clear();
        }
    }
    CloseHandle(file);
    if (data.empty()) return nullptr;

    IStream* stream = SHCreateMemStream(data.data(), (UINT)data.size());
    if (!stream) return nullptr;

    auto newImg = std::make_unique<Image>(stream);
    stream->Release();

    if (newImg->GetLastStatus() != Ok) {
        return nullptr;
    }

    CachedImage ci;
    UINT dimensionCount = newImg->GetFrameDimensionsCount();
    if (dimensionCount > 0) {
        std::vector<GUID> dimensionIDs(dimensionCount);
        newImg->GetFrameDimensionsList(dimensionIDs.data(), dimensionCount);
        
        bool hasTimeDimension = false;
        for (const auto& guid : dimensionIDs) {
            if (guid == FrameDimensionTime) {
                hasTimeDimension = true;
                break;
            }
        }

        if (hasTimeDimension) {
            ci.frameCount = newImg->GetFrameCount(&FrameDimensionTime);
            if (ci.frameCount > 1) {
                UINT propertySize = newImg->GetPropertyItemSize(PropertyTagFrameDelay);
                if (propertySize > 0) {
                    PropertyItem* pItem = (PropertyItem*)malloc(propertySize);
                    if (pItem) {
                        newImg->GetPropertyItem(PropertyTagFrameDelay, propertySize, pItem);
                        UINT* delays = (UINT*)pItem->value;
                        UINT numDelays = propertySize / sizeof(UINT);
                        for (UINT i = 0; i < ci.frameCount && i < numDelays; ++i) {
                            UINT delay = delays[i] * 10;
                            if (delay < 30) delay = 100;
                            ci.delays.push_back(delay);
                        }
                        free(pItem);
                    }
                }
            }
        }
    }

    while (ci.delays.size() < ci.frameCount) {
        ci.delays.push_back(100);
    }

    ci.img = std::move(newImg);
    (*g_imageCache)[path] = std::move(ci);
    return &(*g_imageCache)[path];
}

bool IsKnownClockClass(const wchar_t* szClass) {
    if (!szClass) return false;
    return (wcsicmp(szClass, L"ClockWndMain") == 0 ||
            wcsicmp(szClass, L"ClockCtl") == 0 ||
            wcsicmp(szClass, L"ClockWndClass") == 0);
}

bool IsAnalogClockControl(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;

    DWORD style = GetWindowLongW(hwnd, GWL_STYLE);
    if (!(style & WS_CHILD)) return false;

    wchar_t szClass[256] = {0};
    if (GetClassNameW(hwnd, szClass, 256) <= 0) return false;

    if (wcsstr(szClass, L"TrayClock") != NULL) return false;
    if (wcsstr(szClass, L"DigitalClock") != NULL) return false;
    if (wcsicmp(szClass, L"SysDateTimePick32") == 0) return false;

    return IsKnownClockClass(szClass);
}

COLORREF GetParentBgColor(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return GetSysColor(COLOR_3DFACE);

    HWND hParent = GetParent(hwnd);
    HWND hRoot = GetAncestor(hwnd, GA_ROOT);
    if (!hParent) hParent = hwnd;
    if (!hRoot) hRoot = hParent;

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

    HDC hdcScreen = GetDC(NULL);
    if (hdcScreen) {
        RECT rcChild, rcParent, rcRoot;
        GetWindowRect(hwnd, &rcChild);
        GetWindowRect(hParent, &rcParent);
        GetWindowRect(hRoot, &rcRoot);

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

void ApplySmoothMaskToDC(HDC hdc, HWND hwnd, int w, int h) {
    if (!hdc || w <= 20 || h <= 20 || g_bInMask) return;
    g_bInMask = true;

    PCWSTR customImagePath = Wh_GetStringSetting(L"customImagePath");
    bool renderedCustomImage = false;

    if (customImagePath && customImagePath[0] != L'\0') {
        CachedImage* cached = GetCachedImage(customImagePath);
        if (cached && cached->img) {
            COLORREF bgClr = GetParentBgColor(hwnd);

            Graphics graphics(hdc);
            graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            graphics.SetSmoothingMode(SmoothingModeAntiAlias);

            SolidBrush bgBrush(Color(255, GetRValue(bgClr), GetGValue(bgClr), GetBValue(bgClr)));
            graphics.FillRectangle(&bgBrush, 0, 0, w, h);

            if (cached->frameCount > 1) {
                UINT frameIndex = 0;
                {
                    std::lock_guard<std::mutex> lock(g_windowAnimsMutex);
                    auto& animState = g_windowAnims[hwnd];
                    if (animState.imagePath != customImagePath) {
                        animState.imagePath = customImagePath;
                        animState.currentFrame = 0;
                    }
                    frameIndex = animState.currentFrame % cached->frameCount;
                }

                GUID frameDimension = FrameDimensionTime;
                cached->img->SelectActiveFrame(&frameDimension, frameIndex);

                graphics.DrawImage(cached->img.get(), 0, 0, w, h);
                renderedCustomImage = true;

                UINT delay = cached->delays[frameIndex];
                SetTimer(hwnd, 9999, delay, NULL);
            } else {
                graphics.DrawImage(cached->img.get(), 0, 0, w, h);
                renderedCustomImage = true;
            }
        }
    }
    Wh_FreeStringSetting((PWSTR)customImagePath);

    if (!renderedCustomImage) {
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
    }

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
            PCWSTR customImagePath = Wh_GetStringSetting(L"customImagePath");
            bool hasCustomImage = (customImagePath && customImagePath[0] != L'\0');
            Wh_FreeStringSetting((PWSTR)customImagePath);

            if (hasCustomImage) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                if (hdc) {
                    RECT rc;
                    GetClientRect(hwnd, &rc);
                    ApplySmoothMaskToDC(hdc, hwnd, rc.right - rc.left, rc.bottom - rc.top);
                    EndPaint(hwnd, &ps);
                }
                return 0;
            } else {
                LRESULT lRes = DefSubclassProc(hwnd, uMsg, wParam, lParam);
                RECT rc;
                if (GetClientRect(hwnd, &rc)) {
                    HDC hdc = GetDC(hwnd);
                    if (hdc) {
                        ApplySmoothMaskToDC(hdc, hwnd, rc.right - rc.left, rc.bottom - rc.top);
                        ReleaseDC(hwnd, hdc);
                    }
                }
                return lRes;
            }
        }

        case WM_PRINTCLIENT: {
            PCWSTR customImagePath = Wh_GetStringSetting(L"customImagePath");
            bool hasCustomImage = (customImagePath && customImagePath[0] != L'\0');
            Wh_FreeStringSetting((PWSTR)customImagePath);

            if (hasCustomImage) {
                HDC hdc = (HDC)wParam;
                if (hdc) {
                    RECT rc;
                    GetClientRect(hwnd, &rc);
                    ApplySmoothMaskToDC(hdc, hwnd, rc.right - rc.left, rc.bottom - rc.top);
                }
                return 0;
            } else {
                LRESULT res = DefSubclassProc(hwnd, uMsg, wParam, lParam);
                HDC hdc = (HDC)wParam;
                if (hdc) {
                    RECT rc;
                    GetClientRect(hwnd, &rc);
                    ApplySmoothMaskToDC(hdc, hwnd, rc.right - rc.left, rc.bottom - rc.top);
                }
                return res;
            }
        }

        case WM_TIMER: {
            if (wParam == 9999) {
                PCWSTR customImagePath = Wh_GetStringSetting(L"customImagePath");
                if (customImagePath && customImagePath[0] != L'\0') {
                    CachedImage* cached = GetCachedImage(customImagePath);
                    if (cached && cached->frameCount > 1) {
                        std::lock_guard<std::mutex> lock(g_windowAnimsMutex);
                        auto& animState = g_windowAnims[hwnd];
                        animState.currentFrame = (animState.currentFrame + 1) % cached->frameCount;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                }
                Wh_FreeStringSetting((PWSTR)customImagePath);
                return 0;
            }
            break;
        }

        case WM_NCDESTROY: {
            KillTimer(hwnd, 9999);
            {
                std::lock_guard<std::mutex> lock1(g_clockWindowsMutex);
                g_clockWindows.erase(hwnd);
            }
            {
                std::lock_guard<std::mutex> lock2(g_windowAnimsMutex);
                g_windowAnims.erase(hwnd);
            }
            break;
        }
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

void AttachIfClock(HWND hwnd) {
    if (!IsAnalogClockControl(hwnd)) return;

    DWORD style = GetWindowLongW(hwnd, GWL_STYLE);
    DWORD exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    ClockWindowState state{
        .addedComposited = !(exStyle & WS_EX_COMPOSITED),
        .removedBorder = (style & WS_BORDER) != 0,
    };

    {
        std::lock_guard<std::mutex> lock(g_clockWindowsMutex);
        if (!g_clockWindows.try_emplace(hwnd, state).second) return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, ClockSubclassProc, 0)) {
        std::lock_guard<std::mutex> lock(g_clockWindowsMutex);
        g_clockWindows.erase(hwnd);
        return;
    }

    if (state.addedComposited) {
        SetWindowLongW(hwnd, GWL_EXSTYLE, exStyle | WS_EX_COMPOSITED);
    }

    if (state.removedBorder) {
        SetBorder(hwnd, false);
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

    (void)dwExStyle;
    (void)lpWindowName;
    (void)dwStyle;
    (void)X;
    (void)Y;
    (void)nWidth;
    (void)nHeight;
    (void)hWndParent;
    (void)hMenu;
    (void)hInstance;
    (void)lpParam;

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
    Wh_Log(L"Clock animated GIF mod initialized successfully");

    return TRUE;
}

void Wh_ModSettingsChanged() {
    std::lock_guard<std::mutex> lock(g_imageMutex);
    g_imageCache->clear();
}

void Wh_ModUninit() {
    std::vector<std::pair<HWND, ClockWindowState>> windows;
    {
        std::lock_guard<std::mutex> lock(g_clockWindowsMutex);
        windows.assign(g_clockWindows.begin(), g_clockWindows.end());
        g_clockWindows.clear();
    }

    auto cleanup = [](const auto& pair) {
        HWND hwnd = pair.first;
        ClockWindowState state = pair.second;
        KillTimer(hwnd, 9999);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, ClockSubclassProc);
        if (state.addedComposited) {
            SetWindowLongW(hwnd, GWL_EXSTYLE,
                           GetWindowLongW(hwnd, GWL_EXSTYLE) & ~WS_EX_COMPOSITED);
        }
        if (state.removedBorder) {
            SetBorder(hwnd, true);
        }
        InvalidateRect(hwnd, NULL, TRUE);
    };

    for (const auto& item : windows) {
        cleanup(item);
    }

    {
        std::lock_guard<std::mutex> lockImg(g_imageMutex);
        g_imageCache.reset();
    }

    {
        std::lock_guard<std::mutex> lock(g_windowAnimsMutex);
        g_windowAnims.clear();
    }

    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}
