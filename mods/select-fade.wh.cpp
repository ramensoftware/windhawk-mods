// ==WindhawkMod==
// @id              select-fade
// @name            SelectFade
// @description     Replaces the system marquee with a custom-drawn rectangle that fades out smoothly on release.
// @version         2.5
// @author          434e5b58
// @github          https://github.com/434e5b58
// @license         MIT
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -lcomctl32 -ladvapi32 -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# SelectFade

Replaces the default desktop selection rectangle with a fully custom-drawn overlay that fades out smoothly.

## How it works

* **Custom Drawing:** The mod disables the standard system marquee on the desktop and draws its own selection box using a layered window.
* **Live Preview:** While you drag, the rectangle updates in real time. The border always uses your system's accent color, and the fill (optional) can be customized.
* **Smooth Fade-Out:** When you release the mouse button, the rectangle doesn't just disappear. It smoothly fades out in place, creating a polished, modern feel.
* **Settings:** Toggle the fill on/off, set a custom fill color, enable or disable the fade animation, and adjust its speed and duration.

## Limitations

* **Desktop Only:** This mod is designed for the desktop. It **does not work in File Explorer windows** because modern Explorer uses DirectUI, which bypasses the standard drawing methods used here.

## Credits

Based on the original [Desktop Marquee Fade](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/desktop-marquee-fade.wh.cpp) mod by Twilight0Sparkle (MIT License).

![Preview](https://i.imgur.com/XXXXXXX.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LiveEnableFill: false
  $name: Fill while dragging
  $name:ru-RU: Живая заливка
  $description: Fill the selection rectangle while dragging.
  $description:ru-RU: Включить заливку во время перетаскивания.
- LiveFillColor: ''
  $name: Fill color
  $name:ru-RU: Цвет живой заливки
  $description: HEX color for the fill. Empty = system accent.
  $description:ru-RU: HEX-код цвета заливки. Пусто = системный акцент.
- AnimEnabled: true
  $name: Enable fade animation
  $name:ru-RU: Включить анимацию
  $description: If disabled, the rectangle disappears instantly on release.
  $description:ru-RU: Если выключить, рамка просто исчезнет после отпускания.
- AnimSpeed: 100
  $name: Animation speed (%)
  $name:ru-RU: Скорость анимации (%)
  $description: Fade speed (50-500).
  $description:ru-RU: Скорость исчезновения (50-500).
- AnimDuration: 300
  $name: Animation duration (ms)
  $name:ru-RU: Длительность анимации (мс)
  $description: Fade duration (100-2000).
  $description:ru-RU: Время исчезновения (100-2000).
*/
// ==/WindhawkModSettings==

#include <map>
#include <atomic>
#include <algorithm>
#include <cmath>
#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winreg.h>

#ifdef _WIN64
#    define THISCALL  __cdecl
#    define STHISCALL L"__cdecl"
#else
#    define THISCALL  __thiscall
#    define STHISCALL L"__thiscall"
#endif

COLORREF g_clrLiveBorder = RGB(255, 0, 255);
COLORREF g_clrLiveFill   = RGB(0, 0, 0);
bool     g_bLiveEnableFill = false;

std::atomic<bool>  g_settingAnimEnabled{true};
std::atomic<int>   g_settingSpeed{100};
std::atomic<int>   g_settingDuration{300};

HWND   g_hManagerWnd = NULL;
HHOOK  g_hMouseHook  = NULL;
HANDLE g_hThread     = NULL;
DWORD  g_dwThreadId  = 0;
HINSTANCE g_hModInstance = NULL;

POINT g_dragStart = {0};
POINT g_dragCurrent = {0};
bool  g_isDragging = false;
bool  g_isStartPointValid = false;
bool  g_updatePending = false;

HWND g_hCachedDesktopTopLevel  = NULL;
HWND g_hCachedSysListView32    = NULL;
HWND g_hLiveOverlay = NULL;
std::atomic<HWND> g_hDesktopListViewAtomic{NULL};

#define TIMER_ID_FADE        1
#define WM_PRECHECK_START    (WM_USER + 2)
#define WM_UPDATE_LIVE       (WM_USER + 3)
#define WM_STOP_LIVE         (WM_USER + 4)

struct AnimState { float alpha; float decrement; };
std::map<HWND, AnimState> g_animations;

thread_local HWND g_tlsCurrentListView = NULL;

COLORREF GetSystemAccentColor() {
    HKEY hKey;
    COLORREF sysColor = RGB(255, 0, 255); 
    DWORD dwColor = 0;
    DWORD dwSize = sizeof(dwColor);

    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"AccentColorMenu", nullptr, nullptr, (LPBYTE)&dwColor, &dwSize) == ERROR_SUCCESS) {
            sysColor = RGB((dwColor >> 0) & 0xFF, (dwColor >> 8) & 0xFF, (dwColor >> 16) & 0xFF);
            RegCloseKey(hKey);
            return sysColor;
        }
        RegCloseKey(hKey);
    }

    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"ColorizationColor", nullptr, nullptr, (LPBYTE)&dwColor, &dwSize) == ERROR_SUCCESS) {
            sysColor = RGB((dwColor >> 0) & 0xFF, (dwColor >> 8) & 0xFF, (dwColor >> 16) & 0xFF);
        }
        RegCloseKey(hKey);
    }
    return sysColor;
}

DWORD ParseHexColor(PCWSTR hexStr) {
    if (!hexStr) return 0;
    const wchar_t* p = hexStr;
    while (*p == L' ' || *p == L'\t') p++;
    if      (p[0] == L'0' && (p[1] == L'x' || p[1] == L'X')) p += 2;
    else if (p[0] == L'#') p += 1;
    return wcstoul(p, nullptr, 16);
}

DWORD ParseHexColorOrDefault(PCWSTR hexStr, DWORD defaultValue) {
    if (!hexStr || hexStr[0] == L'\0') return defaultValue;
    return ParseHexColor(hexStr);
}

void LoadSettings() {
    g_bLiveEnableFill = Wh_GetIntSetting(L"LiveEnableFill") != 0;
    
    WindhawkUtils::StringSetting liveFillStr = WindhawkUtils::StringSetting::make(L"LiveFillColor");
    DWORD liveHex = ParseHexColorOrDefault(liveFillStr.get(), 0);
    g_clrLiveFill = (liveHex == 0) ? GetSystemAccentColor() : RGB((liveHex >> 16) & 0xFF, (liveHex >> 8) & 0xFF, liveHex & 0xFF);
    
    g_clrLiveBorder = GetSystemAccentColor();

    g_settingAnimEnabled.store(Wh_GetIntSetting(L"AnimEnabled") != 0);
    g_settingSpeed.store(std::max(50, std::min(500, Wh_GetIntSetting(L"AnimSpeed"))));
    g_settingDuration.store(std::max(100, std::min(2000, Wh_GetIntSetting(L"AnimDuration"))));
}

void PaintMarqueeDIB(HWND hwnd, int width, int height, int alphaBorder, int alphaFill, int borderThickness) {
    if (width <= 0 || height <= 0) return;

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem    = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void*   pBits     = nullptr;
    HBITMAP hBitmap   = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    if (pBits) {
        COLORREF fillCol   = g_clrLiveFill;
        COLORREF borderCol = g_clrLiveBorder;

        BYTE fillR = GetRValue(fillCol),   fillG = GetGValue(fillCol),   fillB = GetBValue(fillCol);
        BYTE borR  = GetRValue(borderCol), borG  = GetGValue(borderCol), borB  = GetBValue(borderCol);

        int bt = std::max(1, std::min(borderThickness, std::min(width, height) / 2));

        DWORD* pixels = (DWORD*)pBits;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bool isBorder = (x < bt || x >= width - bt || y < bt || y >= height - bt);
                if (isBorder) {
                    BYTE r = (borR * alphaBorder) / 255;
                    BYTE g = (borG * alphaBorder) / 255;
                    BYTE b = (borB * alphaBorder) / 255;
                    pixels[y * width + x] = ((DWORD)alphaBorder << 24) | (r << 16) | (g << 8) | b;
                } else {
                    if (g_bLiveEnableFill) {
                        BYTE r = (fillR * alphaFill) / 255;
                        BYTE g = (fillG * alphaFill) / 255;
                        BYTE b = (fillB * alphaFill) / 255;
                        pixels[y * width + x] = ((DWORD)alphaFill << 24) | (r << 16) | (g << 8) | b;
                    } else {
                        pixels[y * width + x] = 0x00000000;
                    }
                }
            }
        }

        POINT ptSrc = {0, 0};
        SIZE szWnd = {width, height};
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, hdcScreen, NULL, &szWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    }

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER && wParam == TIMER_ID_FADE) {
        auto it = g_animations.find(hwnd);
        if (it != g_animations.end()) {
            AnimState& st = it->second;
            st.alpha -= st.decrement;
            if (st.alpha <= 0.0f) {
                KillTimer(hwnd, TIMER_ID_FADE);
                g_animations.erase(it);
                DestroyWindow(hwnd);
            } else {
                BLENDFUNCTION blend = {AC_SRC_OVER, 0, (BYTE)st.alpha, AC_SRC_ALPHA};
                UpdateLayeredWindow(hwnd, NULL, NULL, NULL, NULL, NULL, 0, &blend, ULW_ALPHA);
            }
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

HWND GetRealDesktopListView(HWND* outDesktopTopLevel) {
    HWND hProgman = FindWindowW(L"Progman", L"Program Manager");
    HWND hDesktopDefView = FindWindowExW(hProgman, NULL, L"SHELLDLL_DefView", NULL);

    if (hDesktopDefView) {
        if (outDesktopTopLevel) *outDesktopTopLevel = hProgman;
    } else {
        HWND hWorkerW = NULL;
        while ((hWorkerW = FindWindowExW(NULL, hWorkerW, L"WorkerW", NULL)) != NULL) {
            hDesktopDefView = FindWindowExW(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            if (hDesktopDefView) {
                if (outDesktopTopLevel) *outDesktopTopLevel = hWorkerW;
                break;
            }
        }
    }
    if (!hDesktopDefView) return NULL;
    return FindWindowExW(hDesktopDefView, NULL, L"SysListView32", L"FolderView");
}

LRESULT CALLBACK ManagerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_PRECHECK_START) {
        POINT* ppt = (POINT*)lParam;
        g_hCachedSysListView32 = GetRealDesktopListView(&g_hCachedDesktopTopLevel);
        g_hDesktopListViewAtomic.store(g_hCachedSysListView32);
        bool isValid = false;

        if (g_hCachedSysListView32 && g_hCachedDesktopTopLevel) {
            RECT rcDesktop;
            GetWindowRect(g_hCachedSysListView32, &rcDesktop);
            if (PtInRect(&rcDesktop, *ppt)) {
                HWND hwndAt = WindowFromPoint(*ppt);
                HWND hRootAt = hwndAt ? GetAncestor(hwndAt, GA_ROOT) : NULL;
                if (hRootAt == g_hCachedDesktopTopLevel) {
                    DWORD sysListViewPid = 0;
                    GetWindowThreadProcessId(g_hCachedSysListView32, &sysListViewPid);
                    if (sysListViewPid == GetCurrentProcessId()) {
                        LVHITTESTINFO lvhti;
                        ZeroMemory(&lvhti, sizeof(lvhti));
                        lvhti.pt = *ppt;
                        ScreenToClient(g_hCachedSysListView32, &lvhti.pt);
                        DWORD_PTR dwResult = 0;
                        bool isOnIcon = false;
                        if (SendMessageTimeoutW(g_hCachedSysListView32, LVM_HITTEST, 0, (LPARAM)&lvhti, SMTO_ABORTIFHUNG | SMTO_NORMAL, 50, &dwResult)) {
                            isOnIcon = ((int)dwResult != -1);
                        }
                        isValid = !isOnIcon;
                    }
                }
            }
        }
        g_isStartPointValid = isValid;
        delete ppt;
        return 0;
    }

    if (msg == WM_UPDATE_LIVE) {
        g_updatePending = false;
        if (!g_isStartPointValid) return 0;

        RECT rc{
            std::min(g_dragStart.x, g_dragCurrent.x),
            std::min(g_dragStart.y, g_dragCurrent.y),
            std::max(g_dragStart.x, g_dragCurrent.x),
            std::max(g_dragStart.y, g_dragCurrent.y)
        };
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
        if (w <= 0 || h <= 0) return 0;

        if (!g_hLiveOverlay) {
            g_hLiveOverlay = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                L"FadeMarqueeOverlayClass", NULL, WS_POPUP, 0, 0, 0, 0,
                NULL, NULL, g_hModInstance, NULL);
            if (g_hLiveOverlay) {
                HWND hAbove = g_hCachedDesktopTopLevel ? GetWindow(g_hCachedDesktopTopLevel, GW_HWNDPREV) : NULL;
                SetWindowPos(g_hLiveOverlay, hAbove ? hAbove : HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            }
        }

        if (g_hLiveOverlay) {
            UINT dpi = GetDpiForWindow(g_hLiveOverlay);
            if (dpi == 0) dpi = 96;
            int borderThickness = std::max(1, (int)lround((double)dpi / 96.0));

            SetWindowPos(g_hLiveOverlay, NULL, rc.left, rc.top, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
            PaintMarqueeDIB(g_hLiveOverlay, w, h, 255, 90, borderThickness);
        }
        return 0;
    }

    if (msg == WM_STOP_LIVE) {
        if (g_hLiveOverlay) {
            if (g_settingAnimEnabled.load()) {
                int speed = g_settingSpeed.load();
                int duration = g_settingDuration.load();
                int actualDuration = (duration * 100) / speed;
                if (actualDuration < 16) actualDuration = 16;

                AnimState st;
                st.alpha = 255.0f;
                st.decrement = 255.0f / ((float)actualDuration / 16.0f);
                g_animations[g_hLiveOverlay] = st;
                SetTimer(g_hLiveOverlay, TIMER_ID_FADE, 16, NULL);
                g_hLiveOverlay = NULL;
            } else {
                DestroyWindow(g_hLiveOverlay);
                g_hLiveOverlay = NULL;
            }
        }
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;
        if (wParam == WM_LBUTTONDOWN) {
            g_dragStart = pMouse->pt;
            g_dragCurrent = pMouse->pt;
            g_isDragging = true;
            g_isStartPointValid = false;
            POINT* ppt = new POINT{pMouse->pt.x, pMouse->pt.y};
            if (!PostMessageW(g_hManagerWnd, WM_PRECHECK_START, 0, (LPARAM)ppt)) {
                delete ppt;
            }
        } else if (wParam == WM_MOUSEMOVE && g_isDragging) {
            g_dragCurrent = pMouse->pt;
            if (!g_updatePending) {
                g_updatePending = true;
                PostMessageW(g_hManagerWnd, WM_UPDATE_LIVE, 0, 0);
            }
        } else if (wParam == WM_LBUTTONUP && g_isDragging) {
            g_isDragging = false;
            PostMessageW(g_hManagerWnd, WM_STOP_LIVE, 0, 0);
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

LRESULT (THISCALL *CListView_WndProc_orig)(void *, HWND, UINT, WPARAM, LPARAM);
LRESULT THISCALL CListView_WndProc_hook(void *pThis, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HWND prev = g_tlsCurrentListView;
    g_tlsCurrentListView = hwnd;
    LRESULT result = CListView_WndProc_orig(pThis, hwnd, msg, wParam, lParam);
    g_tlsCurrentListView = prev;
    return result;
}

void (THISCALL *CLVDrawManager__PaintMarquee_orig)(void *, HDC, void *, RECT *);
void THISCALL CLVDrawManager__PaintMarquee_hook(void *pThis, HDC hdc, void *pv, RECT *prc) {
    HWND hDesktop = g_hDesktopListViewAtomic.load();
    if (hDesktop != NULL && g_tlsCurrentListView == hDesktop) {
        return;
    }
    CLVDrawManager__PaintMarquee_orig(pThis, hdc, pv, prc);
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    HINSTANCE hInst = g_hModInstance;

    WNDCLASSW wcm = {0};
    wcm.lpfnWndProc = ManagerWndProc;
    wcm.hInstance = hInst;
    wcm.lpszClassName = L"FadeMarqueeManagerClass";
    if (!RegisterClassW(&wcm)) return 0;

    WNDCLASSW wco = {0};
    wco.lpfnWndProc = OverlayWndProc;
    wco.hInstance = hInst;
    wco.lpszClassName = L"FadeMarqueeOverlayClass";
    if (!RegisterClassW(&wco)) {
        UnregisterClassW(L"FadeMarqueeManagerClass", hInst);
        return 0;
    }

    g_hManagerWnd = CreateWindowExW(0, L"FadeMarqueeManagerClass", NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInst, NULL);
    if (!g_hManagerWnd) {
        UnregisterClassW(L"FadeMarqueeManagerClass", hInst);
        UnregisterClassW(L"FadeMarqueeOverlayClass", hInst);
        return 0;
    }

    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc, hInst, 0);
    if (!g_hMouseHook) {
        DestroyWindow(g_hManagerWnd);
        UnregisterClassW(L"FadeMarqueeManagerClass", hInst);
        UnregisterClassW(L"FadeMarqueeOverlayClass", hInst);
        return 0;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);

    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_PRECHECK_START && msg.lParam) {
            delete (POINT*)msg.lParam;
        }
    }

    if (g_hMouseHook) UnhookWindowsHookEx(g_hMouseHook);
    if (g_hManagerWnd) DestroyWindow(g_hManagerWnd);

    if (g_hLiveOverlay) {
        DestroyWindow(g_hLiveOverlay);
        g_hLiveOverlay = NULL;
    }

    for (auto const& pair : g_animations) {
        KillTimer(pair.first, TIMER_ID_FADE);
        DestroyWindow(pair.first);
    }
    g_animations.clear();

    UnregisterClassW(L"FadeMarqueeManagerClass", hInst);
    UnregisterClassW(L"FadeMarqueeOverlayClass", hInst);

    return 0;
}

BOOL Wh_ModInit() {
    PCWSTR cmdLine = GetCommandLineW();
    if (cmdLine && (wcsstr(cmdLine, L"/factory") || wcsstr(cmdLine, L"-Embedding"))) return FALSE;
    HWND hShell = GetShellWindow();
    if (hShell) {
        DWORD shellPid = 0;
        GetWindowThreadProcessId(hShell, &shellPid);
        if (shellPid != 0 && shellPid != GetCurrentProcessId()) return FALSE;
    }

    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&Wh_ModInit, &g_hModInstance);

    LoadSettings();

    HMODULE hComCtl = LoadLibraryExW(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hComCtl) {
        const WindhawkUtils::SYMBOL_HOOK comctl32DllHooks[] = {
            { { L"private: void " STHISCALL " CLVDrawManager::_PaintMarquee(struct HDC__ *,void *,struct tagRECT const *)" },
              &CLVDrawManager__PaintMarquee_orig, CLVDrawManager__PaintMarquee_hook, false },
            { { 
                #ifdef _WIN64
                L"public: __int64 __cdecl CListView::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
                #else
                L"public: long __thiscall CListView::WndProc(struct HWND__ *,unsigned int,unsigned int,long)"
                #endif
              },
              &CListView_WndProc_orig, CListView_WndProc_hook, false }
        };
        WindhawkUtils::HookSymbols(hComCtl, comctl32DllHooks, ARRAYSIZE(comctl32DllHooks));
    }

    g_hThread = CreateThread(NULL, 0, WorkerThreadProc, NULL, 0, &g_dwThreadId);
    return g_hThread != NULL;
}

void Wh_ModSettingsChanged() { LoadSettings(); }
void Wh_ModUninit() {
    if (g_dwThreadId) PostThreadMessageW(g_dwThreadId, WM_QUIT, 0, 0);
    if (g_hThread) { WaitForSingleObject(g_hThread, INFINITE); CloseHandle(g_hThread); }
}
