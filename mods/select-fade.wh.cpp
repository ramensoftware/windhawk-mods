// ==WindhawkMod==
// @github https://github.com/434e5b58
// @id              select-fade
// @name            SelectFade
// @description     Replaces the system marquee with a custom-drawn rectangle that fades out smoothly on release.
// @version         0.1
// @author          434e5b58
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -lcomctl32 -ladvapi32 -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# SelectFade

Replaces the default desktop selection rectangle with a fully custom-drawn overlay that fades out smoothly.

## How it works

* **Custom Drawing:** The mod disables the standard system marquee and draws its own selection box using a layered window.
* **Live Preview:** While you drag, the rectangle updates in real time. The border always uses your system's accent color, and the fill (optional) can be customized.
* **Smooth Fade-Out:** When you release the mouse button, the rectangle doesn't just disappear. It smoothly fades out in place, creating a polished, modern feel.
* **Settings:** Toggle the fill on/off, set a custom fill color, enable or disable the fade animation, and adjust its speed and duration.

## Limitations

* **Desktop Only:** This mod is designed for the desktop. It **does not work in File Explorer windows** because modern Explorer uses DirectUI, which bypasses the standard drawing methods used here.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LiveEnableFill: false
  $name: Живая заливка
  $description: Включить заливку во время перетаскивания.
- LiveFillColor: ''
  $name: Цвет живой заливки
  $description: HEX-код цвета заливки. Пусто = системный акцент.
- AnimEnabled: true
  $name: Включить анимацию
  $description: Если выключить, рамка просто исчезнет после отпускания.
- AnimSpeed: 100
  $name: Скорость анимации (%)
  $description: Скорость исчезновения (50-500).
- AnimDuration: 300
  $name: Длительность анимации (мс)
  $description: Время исчезновения (100-2000).
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

POINT g_dragStart = {0};
POINT g_dragCurrent = {0};
std::atomic<bool> g_isDragging{false};
std::atomic<bool> g_isStartPointValid{false};

HWND g_hCachedDesktopTopLevel  = NULL;
HWND g_hCachedSysListView32    = NULL;
HWND g_hLiveOverlay = NULL;

#define TIMER_ID_FADE        1
#define WM_START_LIVE        (WM_USER + 1)
#define WM_UPDATE_LIVE       (WM_USER + 2)
#define WM_STOP_LIVE         (WM_USER + 3)
#define WM_PRECHECK_START    (WM_USER + 4)

struct AnimState { float alpha; float decrement; RECT rect; };
std::map<HWND, AnimState> g_animations;

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
                // Перерисовываем DIB с новой альфой
                int w = st.rect.right - st.rect.left;
                int h = st.rect.bottom - st.rect.top;
                UINT dpi = GetDpiForWindow(hwnd);
                if (dpi == 0) dpi = 96;
                int borderThickness = std::max(1, (int)lround((double)dpi / 96.0));
                PaintMarqueeDIB(hwnd, w, h, (int)st.alpha, (int)st.alpha, borderThickness);
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
        g_isStartPointValid.store(isValid);
        delete ppt;
        return 0;
    }

    if (msg == WM_START_LIVE) {
        if (!g_isStartPointValid.load()) return 0;

        if (g_hLiveOverlay) {
            DestroyWindow(g_hLiveOverlay);
            g_hLiveOverlay = NULL;
        }

        g_hLiveOverlay = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            L"FadeMarqueeOverlayClass", NULL, WS_POPUP, 0, 0, 0, 0,
            NULL, NULL, GetModuleHandle(NULL), NULL);

        if (g_hLiveOverlay) {
            HWND hAbove = GetWindow(g_hCachedDesktopTopLevel, GW_HWNDPREV);
            SetWindowPos(g_hLiveOverlay, hAbove ? hAbove : HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
        return 0;
    }

    if (msg == WM_UPDATE_LIVE) {
        if (!g_hLiveOverlay) return 0;

        RECT* prc = (RECT*)lParam;
        int w = prc->right - prc->left;
        int h = prc->bottom - prc->top;
        if (w <= 0 || h <= 0) { delete prc; return 0; }

        UINT dpi = GetDpiForWindow(g_hLiveOverlay);
        if (dpi == 0) dpi = 96;
        int borderThickness = std::max(1, (int)lround((double)dpi / 96.0));

        SetWindowPos(g_hLiveOverlay, NULL, prc->left, prc->top, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
        PaintMarqueeDIB(g_hLiveOverlay, w, h, 255, 90, borderThickness);

        delete prc;
        return 0;
    }

    if (msg == WM_STOP_LIVE) {
        if (g_hLiveOverlay) {
            if (g_settingAnimEnabled.load()) {
                int speed = g_settingSpeed.load();
                int duration = g_settingDuration.load();
                int actualDuration = (duration * 100) / speed;
                if (actualDuration < 16) actualDuration = 16;

                RECT rc;
                GetWindowRect(g_hLiveOverlay, &rc);

                AnimState st;
                st.alpha = 255.0f;
                st.decrement = 255.0f / ((float)actualDuration / 16.0f);
                st.rect = rc;
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
            g_isDragging.store(true);
            g_isStartPointValid.store(false);
            POINT* ppt = new POINT{pMouse->pt.x, pMouse->pt.y};
            PostMessageW(g_hManagerWnd, WM_PRECHECK_START, 0, (LPARAM)ppt);
        } else if (wParam == WM_MOUSEMOVE && g_isDragging.load()) {
            g_dragCurrent = pMouse->pt;
            RECT* prc = new RECT{
                std::min(g_dragStart.x, g_dragCurrent.x),
                std::min(g_dragStart.y, g_dragCurrent.y),
                std::max(g_dragStart.x, g_dragCurrent.x),
                std::max(g_dragStart.y, g_dragCurrent.y)
            };
            PostMessageW(g_hManagerWnd, WM_UPDATE_LIVE, 0, (LPARAM)prc);
            if (!g_hLiveOverlay) {
                PostMessageW(g_hManagerWnd, WM_START_LIVE, 0, 0);
            }
        } else if (wParam == WM_LBUTTONUP && g_isDragging.load()) {
            g_isDragging.store(false);
            PostMessageW(g_hManagerWnd, WM_STOP_LIVE, 0, 0);
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

void (THISCALL *CLVDrawManager__PaintMarquee_orig)(void *, HDC, void *, RECT *);
void THISCALL CLVDrawManager__PaintMarquee_hook(void *pThis, HDC hdc, void *pv, RECT *prc) {
    // Отключаем системную отрисовку — оригинал не вызываем
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    WNDCLASSW wcm = {0, ManagerWndProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"FadeMarqueeManagerClass"};
    RegisterClassW(&wcm);
    WNDCLASSW wco = {0, OverlayWndProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"FadeMarqueeOverlayClass"};
    RegisterClassW(&wco);

    g_hManagerWnd = CreateWindowExW(0, L"FadeMarqueeManagerClass", NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, GetModuleHandle(NULL), NULL);
    g_hMouseHook  = SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc, GetModuleHandle(NULL), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);

    if (g_hMouseHook) UnhookWindowsHookEx(g_hMouseHook);
    if (g_hManagerWnd) DestroyWindow(g_hManagerWnd);
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

    LoadSettings();

    HMODULE hComCtl = LoadLibraryExW(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hComCtl) {
        const WindhawkUtils::SYMBOL_HOOK comctl32DllHooks[] = {
            { { L"private: void " STHISCALL " CLVDrawManager::_PaintMarquee(struct HDC__ *,void *,struct tagRECT const *)" },
              &CLVDrawManager__PaintMarquee_orig, CLVDrawManager__PaintMarquee_hook, false }
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
