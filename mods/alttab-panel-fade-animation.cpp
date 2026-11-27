// ==WindhawkMod==
// @id              alttab-panel-fade-animation
// @name            Alt+Tab panel Fade Animation
// @description     Fade-in and fade-out animation for Alt+Tab
// @version         4.4.0
// @author          mintxup
// @github          https://github.com/windhawk-Alt-Tab-panel-Fade-animation/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lcomctl32 -lversion -luuid -lwinmm -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt+Tab Fade Animation

Adds fade-in and fade-out animation to the Windows Alt+Tab switcher.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fadeInDuration: 120
  $name: "Fade-In Duration (ms)"
- fadeOutDuration: 100
  $name: "Fade-Out Duration (ms)"
- enableLogging: false
  $name: "Enable Debug Logging"
*/
// ==/WindhawkModSettings==

#ifndef WH_MOD_ID
#define WH_MOD_ID L"alttab-fade-animation"
#endif

#include <windows.h>
#include <commctrl.h>
#include <timeapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <atomic>
#include <cmath>
#include <memory>

#define WM_USER_START_GHOST_DISSOLVE (WM_USER + 101)
#define WM_USER_CANCEL_GHOST         (WM_USER + 102)

struct {
    int fadeInDuration;
    int fadeOutDuration;
    int cornerCutoutSize;
    bool cutoutTaskbar;
    bool enableLogging;
} g_settings;

void LogDebug(const wchar_t* format, ...) {
    if (!g_settings.enableLogging) return;
    wchar_t buf[1024];
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(buf, _countof(buf), _TRUNCATE, format, args);
    va_end(args);
    Wh_Log(L"[FadeAnim] %s", buf);
}

std::atomic<HWND> g_activeAltTabHwnd{nullptr};
std::atomic<LONG> g_fadeInToken{0};
std::atomic<LONG> g_ghostToken{0};
std::atomic<BYTE> g_currentFadeInAlpha{0};
std::atomic<BYTE> g_ghostStartAlpha{255};
std::atomic<bool> g_isDismissing{false};

HWND g_hGhostWnd = nullptr;
HANDLE g_hGhostThread = nullptr;
CRITICAL_SECTION g_ghostCs;
HBITMAP g_hCurrentGhostBmp = nullptr;
int g_ghostWidth = 0;
int g_ghostHeight = 0;

LRESULT CALLBACK GhostWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EnterCriticalSection(&g_ghostCs);
            if (g_hCurrentGhostBmp) {
                HDC hdcMem = CreateCompatibleDC(hdc);
                HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, g_hCurrentGhostBmp);
                BitBlt(hdc, 0, 0, g_ghostWidth, g_ghostHeight, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdcMem, hOld);
                DeleteDC(hdcMem);
            }
            LeaveCriticalSection(&g_ghostCs);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_USER_START_GHOST_DISSOLVE: {
            LONG myToken = (LONG)wParam;
            int duration = (int)lParam;
            if (duration <= 0) duration = 100;
            BYTE startAlpha = g_ghostStartAlpha.load();

            ULONGLONG startTick = GetTickCount64();
            while (g_ghostToken == myToken && IsWindow(hWnd)) {
                ULONGLONG elapsed = GetTickCount64() - startTick;
                float progress = (float)elapsed / (float)duration;
                if (progress >= 1.0f) progress = 1.0f;

                float ease = (1.0f - progress) * (1.0f - progress);
                BYTE alpha = (BYTE)((float)startAlpha * ease);

                SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);

                if (progress >= 1.0f) break;
                Sleep(5);
            }

            if (g_ghostToken == myToken && IsWindow(hWnd)) {
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
                ShowWindow(hWnd, SW_HIDE);
            }
            return 0;
        }

        case WM_USER_CANCEL_GHOST: {
            if (IsWindow(hWnd)) {
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
                ShowWindow(hWnd, SW_HIDE);
            }
            return 0;
        }

        default:
            return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
}

DWORD WINAPI GhostThreadProc(LPVOID lpParam) {
    HANDLE hReadyEvent = (HANDLE)lpParam;

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = GhostWndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"WindhawkAltTabGhostWindow";
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    RegisterClassExW(&wc);

    g_hGhostWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        L"WindhawkAltTabGhostWindow",
        L"AltTabGhost",
        WS_POPUP,
        0, 0, 0, 0,
        NULL, NULL, GetModuleHandleW(NULL), NULL
    );

    SetLayeredWindowAttributes(g_hGhostWnd, 0, 0, LWA_ALPHA);

    SetEvent(hReadyEvent);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hGhostWnd && IsWindow(g_hGhostWnd)) {
        DestroyWindow(g_hGhostWnd);
        g_hGhostWnd = nullptr;
    }
    UnregisterClassW(L"WindhawkAltTabGhostWindow", GetModuleHandleW(NULL));
    return 0;
}

void CancelGhost() {
    ++g_ghostToken;
    if (g_hGhostWnd && IsWindow(g_hGhostWnd)) {
        ShowWindow(g_hGhostWnd, SW_HIDE);
        PostMessageW(g_hGhostWnd, WM_USER_CANCEL_GHOST, 0, 0);
    }
}

bool IsAltTabCandidateClass(PCWSTR className) {
    if (!className) return false;
    return (_wcsicmp(className, L"XamlExplorerHostIslandWindow") == 0 ||
            _wcsicmp(className, L"XamlExplorerHostWindow") == 0 ||
            _wcsicmp(className, L"MultitaskingViewFrame") == 0 ||
            _wcsicmp(className, L"Shell_InputSwitchTopLevelWindow") == 0);
}

bool IsAltTabWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) return false;

    WCHAR cls[128];
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) == 0) return false;

    return IsAltTabCandidateClass(cls);
}

struct FadeInParams {
    HWND hWnd;
    LONG token;
    int duration;
};

DWORD WINAPI FadeInThreadProc(LPVOID lpParam) {
    std::unique_ptr<FadeInParams> p(static_cast<FadeInParams*>(lpParam));
    HWND hWnd = p->hWnd;
    LONG token = p->token;
    int duration = p->duration > 0 ? p->duration : 120;

    ULONGLONG start = GetTickCount64();

    while (g_fadeInToken == token && IsWindow(hWnd) && !g_isDismissing.load()) {
        ULONGLONG elapsed = GetTickCount64() - start;
        float progress = (float)elapsed / (float)duration;
        if (progress >= 1.0f) progress = 1.0f;

        float ease = 1.0f - powf(1.0f - progress, 3.0f);
        BYTE alpha = (BYTE)(ease * 255.0f);
        g_currentFadeInAlpha = alpha;

        SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);

        if (progress >= 1.0f) break;
        Sleep(5);
    }

    if (g_fadeInToken == token && IsWindow(hWnd) && !g_isDismissing.load()) {
        g_currentFadeInAlpha = 255;
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        LogDebug(L"Fade-In complete at 255 (HWND: 0x%p)", hWnd);
    }

    return 0;
}

void StartFadeIn(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;

    CancelGhost();

    g_isDismissing = false;
    LONG token = ++g_fadeInToken;
    g_activeAltTabHwnd = hWnd;
    g_currentFadeInAlpha = 0;

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    LONG_PTR newExStyle = (exStyle & ~WS_EX_TRANSPARENT) | WS_EX_LAYERED;
    if (exStyle != newExStyle) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, newExStyle);
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);

    LogDebug(L"StartFadeIn (HWND: 0x%p, Duration: %d ms)", hWnd, g_settings.fadeInDuration);

    auto params = std::make_unique<FadeInParams>();
    params->hWnd = hWnd;
    params->token = token;
    params->duration = g_settings.fadeInDuration;

    HANDLE hThread = CreateThread(nullptr, 0, FadeInThreadProc, params.release(), 0, nullptr);
    if (hThread) {
        CloseHandle(hThread);
    }
}

void CaptureAndTriggerSynchronousOverlay(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;

    RECT rc;
    if (!GetWindowRect(hWnd, &rc) || (rc.right <= rc.left) || (rc.bottom <= rc.top)) return;

    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, w, h);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBmp);

    BitBlt(hdcMem, 0, 0, w, h, hdcScreen, rc.left, rc.top, SRCCOPY | CAPTUREBLT);

    SelectObject(hdcMem, hOldBmp);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    BYTE startAlpha = g_currentFadeInAlpha.load();
    if (startAlpha < 30) startAlpha = 255;
    g_ghostStartAlpha = startAlpha;

    EnterCriticalSection(&g_ghostCs);
    if (g_hCurrentGhostBmp) {
        DeleteObject(g_hCurrentGhostBmp);
    }
    g_hCurrentGhostBmp = hBmp;
    g_ghostWidth = w;
    g_ghostHeight = h;
    LeaveCriticalSection(&g_ghostCs);

    if (g_hGhostWnd && IsWindow(g_hGhostWnd)) {
        LONG token = ++g_ghostToken;

        SetWindowPos(g_hGhostWnd, HWND_TOPMOST, rc.left, rc.top, w, h,
                     SWP_SHOWWINDOW | SWP_NOACTIVATE);

        HRGN hRgnFull = CreateRectRgn(0, 0, w, h);
        if (hRgnFull) {
            int cornerSize = g_settings.cornerCutoutSize;
            if (cornerSize > 0) {
                HRGN hTL = CreateRectRgn(0, 0, cornerSize, cornerSize);
                HRGN hTR = CreateRectRgn(w - cornerSize, 0, w, cornerSize);
                HRGN hBL = CreateRectRgn(0, h - cornerSize, cornerSize, h);
                HRGN hBR = CreateRectRgn(w - cornerSize, h - cornerSize, w, h);

                if (hTL) { CombineRgn(hRgnFull, hRgnFull, hTL, RGN_DIFF); DeleteObject(hTL); }
                if (hTR) { CombineRgn(hRgnFull, hRgnFull, hTR, RGN_DIFF); DeleteObject(hTR); }
                if (hBL) { CombineRgn(hRgnFull, hRgnFull, hBL, RGN_DIFF); DeleteObject(hBL); }
                if (hBR) { CombineRgn(hRgnFull, hRgnFull, hBR, RGN_DIFF); DeleteObject(hBR); }
            }

            if (g_settings.cutoutTaskbar) {
                HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
                if (hTaskbar && IsWindowVisible(hTaskbar)) {
                    RECT tbScreen = {0};
                    if (GetWindowRect(hTaskbar, &tbScreen)) {
                        int tbLeft = (tbScreen.left - rc.left > 0) ? (tbScreen.left - rc.left) : 0;
                        int tbTop = (tbScreen.top - rc.top > 0) ? (tbScreen.top - rc.top) : 0;
                        int tbRight = (tbScreen.right - rc.left < w) ? (tbScreen.right - rc.left) : w;
                        int tbBottom = (tbScreen.bottom - rc.top < h) ? (tbScreen.bottom - rc.top) : h;

                        if (tbRight > tbLeft && tbBottom > tbTop) {
                            HRGN hTB = CreateRectRgn(tbLeft, tbTop, tbRight, tbBottom);
                            if (hTB) {
                                CombineRgn(hRgnFull, hRgnFull, hTB, RGN_DIFF);
                                DeleteObject(hTB);
                            }
                        }
                    }
                }
            }

            SetWindowRgn(g_hGhostWnd, hRgnFull, TRUE);
        } else {
            SetWindowRgn(g_hGhostWnd, NULL, TRUE);
        }

        SetLayeredWindowAttributes(g_hGhostWnd, 0, startAlpha, LWA_ALPHA);
        InvalidateRect(g_hGhostWnd, NULL, FALSE);
        UpdateWindow(g_hGhostWnd);

        PostMessageW(g_hGhostWnd, WM_USER_START_GHOST_DISSOLVE, (WPARAM)token, (LPARAM)g_settings.fadeOutDuration);
        LogDebug(L"Option B overlay displayed: Rect [%d, %d, %d, %d], Alpha: %d",
                 rc.left, rc.top, w, h, startAlpha);
    }
}

using XamlAltTabViewHost_v_DismissView_t = void(WINAPI*)(void* pThis);
XamlAltTabViewHost_v_DismissView_t XamlAltTabViewHost_v_DismissView_Original = nullptr;

void WINAPI XamlAltTabViewHost_v_DismissView_Hook(void* pThis) {
    LogDebug(L"XamlAltTabViewHost::v_DismissView called! pThis: 0x%p", pThis);

    g_isDismissing = true;
    ++g_fadeInToken;

    HWND hWnd = g_activeAltTabHwnd.load();
    if (hWnd) {
        CaptureAndTriggerSynchronousOverlay(hWnd);
    }

    XamlAltTabViewHost_v_DismissView_Original(pThis);

    g_activeAltTabHwnd = nullptr;
    g_isDismissing = false;
}

using CAltTabViewHost_v_DismissView_t = void(WINAPI*)(void* pThis);
CAltTabViewHost_v_DismissView_t CAltTabViewHost_v_DismissView_Original = nullptr;

void WINAPI CAltTabViewHost_v_DismissView_Hook(void* pThis) {
    LogDebug(L"CAltTabViewHost::v_DismissView called! pThis: 0x%p", pThis);

    g_isDismissing = true;
    ++g_fadeInToken;

    HWND hWnd = g_activeAltTabHwnd.load();
    if (hWnd) {
        CaptureAndTriggerSynchronousOverlay(hWnd);
    }

    CAltTabViewHost_v_DismissView_Original(pThis);

    g_activeAltTabHwnd = nullptr;
    g_isDismissing = false;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original = nullptr;

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (!IsAltTabWindow(hWnd)) {
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    LogDebug(L"ShowWindow(HWND: 0x%p, Cmd: %d)", hWnd, nCmdShow);

    if (nCmdShow == SW_SHOWNA || nCmdShow == SW_SHOW || nCmdShow == SW_SHOWNORMAL) {
        g_activeAltTabHwnd = hWnd;

        LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, (exStyle & ~WS_EX_TRANSPARENT) | WS_EX_LAYERED);
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

        SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);

        BOOL res = ShowWindow_Original(hWnd, nCmdShow);
        StartFadeIn(hWnd);
        return res;
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

void LoadSettings() {
    g_settings.fadeInDuration = Wh_GetIntSetting(L"fadeInDuration");
    if (g_settings.fadeInDuration <= 0) g_settings.fadeInDuration = 120;

    g_settings.fadeOutDuration = Wh_GetIntSetting(L"fadeOutDuration");
    if (g_settings.fadeOutDuration <= 0) g_settings.fadeOutDuration = 100;

    g_settings.cornerCutoutSize = Wh_GetIntSetting(L"cornerCutoutSize");
    if (g_settings.cornerCutoutSize < 0) g_settings.cornerCutoutSize = 48;

    g_settings.cutoutTaskbar = Wh_GetIntSetting(L"cutoutTaskbar") != 0;

    g_settings.enableLogging = Wh_GetIntSetting(L"enableLogging") != 0;
}

BOOL Wh_ModInit() {
    timeBeginPeriod(1);
    InitializeCriticalSection(&g_ghostCs);
    LoadSettings();

    Wh_Log(L"Alt+Tab Fade Animation Initialized");

    HANDLE hReadyEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_hGhostThread = CreateThread(NULL, 0, GhostThreadProc, hReadyEvent, 0, nullptr);
    if (hReadyEvent) {
        WaitForSingleObject(hReadyEvent, 1000);
        CloseHandle(hReadyEvent);
    }

    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Original);

    HMODULE twinui = LoadLibrary(L"twinui.pcshell.dll");
    if (twinui) {
        WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
            {
                {
                    LR"(?v_DismissView@XamlAltTabViewHost@@MEAAXXZ)",
                    LR"(protected: virtual void __cdecl XamlAltTabViewHost::v_DismissView(void))"
                },
                &XamlAltTabViewHost_v_DismissView_Original,
                XamlAltTabViewHost_v_DismissView_Hook,
                true,
            },
            {
                {
                    LR"(?v_DismissView@CAltTabViewHost@@MEAAXXZ)",
                    LR"(protected: virtual void __cdecl CAltTabViewHost::v_DismissView(void))"
                },
                &CAltTabViewHost_v_DismissView_Original,
                CAltTabViewHost_v_DismissView_Hook,
                true,
            },
        };

        if (WindhawkUtils::HookSymbols(twinui, symbolHooks, ARRAYSIZE(symbolHooks))) {
            Wh_Log(L"Successfully hooked XamlAltTabViewHost::v_DismissView symbols!");
        } else {
            Wh_Log(L"Warning: Could not hook v_DismissView symbols in twinui.pcshell.dll");
        }
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    LogDebug(L"Settings updated: In=%d ms, Out=%d ms, Cutout=%d px",
             g_settings.fadeInDuration, g_settings.fadeOutDuration, g_settings.cornerCutoutSize);
}

void Wh_ModUninit() {
    timeEndPeriod(1);

    CancelGhost();
    if (g_hGhostWnd && IsWindow(g_hGhostWnd)) {
        PostMessageW(g_hGhostWnd, WM_QUIT, 0, 0);
    }
    if (g_hGhostThread) {
        WaitForSingleObject(g_hGhostThread, 500);
        CloseHandle(g_hGhostThread);
        g_hGhostThread = nullptr;
    }

    EnterCriticalSection(&g_ghostCs);
    if (g_hCurrentGhostBmp) {
        DeleteObject(g_hCurrentGhostBmp);
        g_hCurrentGhostBmp = nullptr;
    }
    LeaveCriticalSection(&g_ghostCs);
    DeleteCriticalSection(&g_ghostCs);

    HWND hWnd = g_activeAltTabHwnd.load();
    if (hWnd && IsWindow(hWnd)) {
        LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
    }

    Wh_Log(L"Alt+Tab Fade Animation uninitialized.");
}
