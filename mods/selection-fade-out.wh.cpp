// ==WindhawkMod==
// @id              selection-fade-out
// @name            Selection Marquee Fade-Out (macOS Style)
// @description     Replaces the selection marquee on the Desktop and in File Explorer with a customizable, flicker-free macOS-style fade-out selection.
// @version         1.0.0
// @author          flashpoint563
// @github          https://github.com/flashpoint563-del
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32 -ldwmapi -luxtheme
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Selection Marquee Fade-Out
Replaces the standard selection rectangle on the Windows 11 Desktop and in File Explorer with a smooth, seamless marquee featuring a macOS-style fade-out animation.

## Features:
- Completely seamless, flicker-free transition on both the Desktop and in File Explorer.
- Custom color (HEX) and opacity (%) settings apply to both the live selection while dragging and the fade-out animation.
- Supports rapid consecutive selections synchronized with the monitor's refresh rate (VSync / DwmFlush).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration: 300
  $name: Fade-out duration (ms)
  $description: Time it takes for the selection marquee to completely fade away after releasing the left mouse button (range 50-2000).
- opacity: 28
  $name: Fill opacity (%)
  $description: Opacity of the inner fill (0-100%). Applies to both the active selection and the fade-out animation. Default is 28%.
- useAccentColor: true
  $name: Use default Windows selection color
  $description: When enabled, uses the system Windows selection color.
- customColorHex: "#FFFFFF"
  $name: Custom color (HEX)
  $description: Color in #RRGGBB format (e.g., #FFFFFF or #0078D7). Used when the default Windows color is disabled.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <vector>
#include <deque>
#include <algorithm>
#include <mutex>
#include <cwctype>

struct ModSettings {
    int duration;
    int opacityPercent;
    bool useAccentColor;
    BYTE red;
    BYTE green;
    BYTE blue;
} g_settings;

struct OverlayVisual {
    RECT rect;
    BYTE fillR, fillG, fillB, fillAlpha;
    BYTE borderR, borderG, borderB, borderAlpha;
};

struct LiveState {
    bool active = false;
    bool dirty = false;
    bool commitToFade = false;
    bool cancel = false;
    int fadeDuration = 300;
    OverlayVisual visual = {};
};

struct FadeRequest {
    OverlayVisual visual;
    int duration;
};

struct ActiveFade {
    HWND hwnd;
    LARGE_INTEGER startTime;
    int duration;
};

constexpr size_t POOL_SIZE = 6;
const wchar_t* OVERLAY_CLASS = L"WH_SelectionFadeOverlay_v12";

std::mutex g_stateMutex;
LiveState g_liveState;
std::deque<FadeRequest> g_pendingFades;

HANDLE g_animThread = nullptr;
HANDLE g_wakeEvent = nullptr;
HANDLE g_stopEvent = nullptr;

// Current selection state in the window thread
thread_local bool t_isSelecting = false;
thread_local bool t_marqueeDetected = false;
thread_local bool t_inAlphaBlend = false;
thread_local POINT t_startPt = {0, 0};
thread_local HWND t_targetWnd = nullptr;
thread_local HPAINTBUFFER t_suppressPaintBuffer = nullptr;

// Original hooked functions
using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t DispatchMessageW_Original = nullptr;

using AlphaBlend_t = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
AlphaBlend_t GdiAlphaBlend_Original = nullptr;
AlphaBlend_t MsimgAlphaBlend_Original = nullptr;

using BufferedPaintSetAlpha_t = HRESULT(WINAPI*)(HPAINTBUFFER, const RECT*, BYTE);
BufferedPaintSetAlpha_t BufferedPaintSetAlpha_Original = nullptr;

using EndBufferedPaint_t = HRESULT(WINAPI*)(HPAINTBUFFER, BOOL);
EndBufferedPaint_t EndBufferedPaint_Original = nullptr;

using FrameRect_t = decltype(&FrameRect);
FrameRect_t FrameRect_Original = nullptr;

inline DWORD MakePremultipliedBGRA(BYTE r, BYTE g, BYTE b, BYTE a) {
    BYTE pr = (BYTE)((r * a) / 255);
    BYTE pg = (BYTE)((g * a) / 255);
    BYTE pb = (BYTE)((b * a) / 255);
    return ((DWORD)a << 24) | ((DWORD)pr << 16) | ((DWORD)pg << 8) | (DWORD)pb;
}

void ParseHexColor(PCWSTR hexStr, BYTE& r, BYTE& g, BYTE& b) {
    r = 255;
    g = 255;
    b = 255;
    if (!hexStr) return;

    while (*hexStr && iswspace(*hexStr)) ++hexStr;
    if (*hexStr == L'#') ++hexStr;
    if (wcslen(hexStr) < 6) return;

    auto hexVal = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        return -1;
    };

    int v[6];
    for (int i = 0; i < 6; ++i) {
        v[i] = hexVal(hexStr[i]);
        if (v[i] < 0) return;
    }

    r = (BYTE)((v[0] << 4) | v[1]);
    g = (BYTE)((v[2] << 4) | v[3]);
    b = (BYTE)((v[4] << 4) | v[5]);
}

void LoadSettings() {
    g_settings.duration = std::clamp(Wh_GetIntSetting(L"duration"), 50, 2000);
    g_settings.opacityPercent = std::clamp(Wh_GetIntSetting(L"opacity"), 0, 100);
    g_settings.useAccentColor = Wh_GetIntSetting(L"useAccentColor") != 0;

    PCWSTR hexStr = Wh_GetStringSetting(L"customColorHex");
    ParseHexColor(hexStr, g_settings.red, g_settings.green, g_settings.blue);
    Wh_FreeStringSetting(hexStr);
}

bool IsDesktopOrExplorerFileArea(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;

    HWND curr = hwnd;
    bool isExplorerOrDesktopRoot = false;
    bool hasFileViewInChain = false;

    while (curr) {
        wchar_t cls[128] = {0};
        GetClassNameW(curr, cls, 128);

        if (wcscmp(cls, L"SysListView32") == 0 ||
            wcscmp(cls, L"DirectUIHWND") == 0 ||
            wcscmp(cls, L"UIItemsView") == 0 ||
            wcscmp(cls, L"SHELLDLL_DefView") == 0 ||
            wcsstr(cls, L"InputSite") != nullptr ||
            wcsstr(cls, L"DesktopChildSiteBridge") != nullptr) {
            hasFileViewInChain = true;
        }

        if (wcscmp(cls, L"CabinetWClass") == 0 ||
            wcscmp(cls, L"ExploreWClass") == 0 ||
            wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0) {
            isExplorerOrDesktopRoot = true;
            break;
        }

        curr = GetParent(curr);
    }

    return isExplorerOrDesktopRoot && hasFileViewInChain;
}

OverlayVisual BuildOverlayVisual(HWND targetWnd, POINT startPt, POINT endPt) {
    RECT rc;
    rc.left = std::min(startPt.x, endPt.x);
    rc.top = std::min(startPt.y, endPt.y);
    rc.right = std::max(startPt.x, endPt.x);
    rc.bottom = std::max(startPt.y, endPt.y);

    if (IsWindow(targetWnd)) {
        HWND root = GetAncestor(targetWnd, GA_ROOT);
        if (root) {
            wchar_t rootCls[64] = {0};
            GetClassNameW(root, rootCls, 64);
            if (wcscmp(rootCls, L"CabinetWClass") == 0 || wcscmp(rootCls, L"ExploreWClass") == 0) {
                RECT wndRc;
                if (GetWindowRect(targetWnd, &wndRc)) {
                    IntersectRect(&rc, &rc, &wndRc);
                }
            }
        }
    }

    BYTE fillAlpha = (BYTE)std::clamp((g_settings.opacityPercent * 255 + 50) / 100, 0, 255);
    BYTE borderAlpha = (BYTE)std::clamp((int)(fillAlpha * 3.6f), 0, 255);

    OverlayVisual vis = {};
    vis.rect = rc;
    vis.fillAlpha = fillAlpha;
    vis.borderAlpha = borderAlpha;

    if (g_settings.useAccentColor) {
        COLORREF fillCol = GetSysColor(COLOR_HOTLIGHT);
        COLORREF borderCol = GetSysColor(COLOR_HIGHLIGHT);
        vis.fillR = GetRValue(fillCol);
        vis.fillG = GetGValue(fillCol);
        vis.fillB = GetBValue(fillCol);
        vis.borderR = GetRValue(borderCol);
        vis.borderG = GetGValue(borderCol);
        vis.borderB = GetBValue(borderCol);
    } else {
        vis.fillR = vis.borderR = g_settings.red;
        vis.fillG = vis.borderG = g_settings.green;
        vis.fillB = vis.borderB = g_settings.blue;
    }

    return vis;
}

void UpdateLiveOverlay(HWND targetWnd, POINT startPt, POINT currPt) {
    OverlayVisual vis = BuildOverlayVisual(targetWnd, startPt, currPt);

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (g_liveState.commitToFade) {
            g_pendingFades.push_back({g_liveState.visual, g_liveState.fadeDuration});
            g_liveState.commitToFade = false;
        }
        g_liveState.active = true;
        g_liveState.cancel = false;
        g_liveState.visual = vis;
        g_liveState.dirty = true;
    }
    SetEvent(g_wakeEvent);
}

void CommitLiveOverlayToFade(HWND targetWnd, POINT startPt, POINT endPt) {
    OverlayVisual vis = BuildOverlayVisual(targetWnd, startPt, endPt);
    bool validSize = (vis.rect.right - vis.rect.left) >= 4 && (vis.rect.bottom - vis.rect.top) >= 4;

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (validSize) {
            g_liveState.visual = vis;
            g_liveState.active = false;
            g_liveState.dirty = true;
            g_liveState.commitToFade = true;
            g_liveState.cancel = false;
            g_liveState.fadeDuration = g_settings.duration;
        } else {
            g_liveState.active = false;
            g_liveState.dirty = false;
            g_liveState.commitToFade = false;
            g_liveState.cancel = true;
        }
    }
    SetEvent(g_wakeEvent);
}

void CancelLiveOverlay() {
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_liveState.active = false;
        g_liveState.dirty = false;
        g_liveState.commitToFade = false;
        g_liveState.cancel = true;
    }
    SetEvent(g_wakeEvent);
}

HWND CreatePooledOverlayWindow() {
    return CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        OVERLAY_CLASS,
        L"",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
    );
}

bool RenderVisualToWindow(HWND hwnd, const OverlayVisual& vis, bool ensureVisible) {
    int width = vis.rect.right - vis.rect.left;
    int height = vis.rect.bottom - vis.rect.top;
    if (width <= 0 || height <= 0) {
        ShowWindow(hwnd, SW_HIDE);
        return false;
    }

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits = nullptr;
    HBITMAP hbm = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
    if (!hbm || !pvBits) {
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);
        return false;
    }

    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);

    DWORD fillPixel = MakePremultipliedBGRA(vis.fillR, vis.fillG, vis.fillB, vis.fillAlpha);
    DWORD borderPixel = MakePremultipliedBGRA(vis.borderR, vis.borderG, vis.borderB, vis.borderAlpha);
    DWORD* pixels = (DWORD*)pvBits;

    for (int y = 0; y < height; ++y) {
        DWORD* row = pixels + y * width;
        if (y == 0 || y == height - 1) {
            std::fill_n(row, width, borderPixel);
        } else {
            row[0] = borderPixel;
            if (width > 2) {
                std::fill_n(row + 1, width - 2, fillPixel);
            }
            if (width > 1) {
                row[width - 1] = borderPixel;
            }
        }
    }

    POINT ptSrc = {0, 0};
    POINT ptDst = {vis.rect.left, vis.rect.top};
    SIZE sizeWnd = {width, height};

    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbm);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);

    if (ensureVisible) {
        SetWindowPos(hwnd, HWND_TOPMOST, vis.rect.left, vis.rect.top, width, height,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSENDCHANGING);
    }
    return true;
}

DWORD WINAPI AnimationThreadProc(LPVOID) {
    WNDCLASSEXW wcOverlay = {};
    wcOverlay.cbSize = sizeof(WNDCLASSEXW);
    wcOverlay.lpfnWndProc = DefWindowProcW;
    wcOverlay.hInstance = GetModuleHandleW(nullptr);
    wcOverlay.lpszClassName = OVERLAY_CLASS;
    RegisterClassExW(&wcOverlay);

    std::vector<HWND> windowPool;
    std::vector<ActiveFade> activeFades;

    for (size_t i = 0; i < POOL_SIZE; ++i) {
        HWND wnd = CreatePooledOverlayWindow();
        if (wnd) windowPool.push_back(wnd);
    }

    auto acquireWindow = [&]() -> HWND {
        if (!windowPool.empty()) {
            HWND w = windowPool.back();
            windowPool.pop_back();
            return w;
        }
        return CreatePooledOverlayWindow();
    };

    auto releaseWindow = [&](HWND w) {
        if (!w) return;
        ShowWindow(w, SW_HIDE);
        if (windowPool.size() < POOL_SIZE) {
            windowPool.push_back(w);
        } else {
            DestroyWindow(w);
        }
    };

    HWND liveHwnd = acquireWindow();
    bool liveVisible = false;

    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    HANDLE waitHandles[2] = { g_stopEvent, g_wakeEvent };

    while (true) {
        if (activeFades.empty()) {
            DWORD waitRes = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
            if (waitRes == WAIT_OBJECT_0) {
                break;
            }
        } else {
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
                break;
            }
        }

        LiveState localLive;
        std::deque<FadeRequest> localPending;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            localLive = g_liveState;
            g_liveState.dirty = false;
            g_liveState.commitToFade = false;
            g_liveState.cancel = false;
            localPending.swap(g_pendingFades);
        }

        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);

        for (const auto& req : localPending) {
            HWND wnd = acquireWindow();
            if (wnd && RenderVisualToWindow(wnd, req.visual, true)) {
                activeFades.push_back({wnd, now, req.duration});
            } else {
                releaseWindow(wnd);
            }
        }

        if (localLive.cancel) {
            if (liveVisible && liveHwnd) {
                ShowWindow(liveHwnd, SW_HIDE);
                liveVisible = false;
            }
        } else if (localLive.commitToFade) {
            if (!liveHwnd) liveHwnd = acquireWindow();
            if (liveHwnd && RenderVisualToWindow(liveHwnd, localLive.visual, !liveVisible)) {
                activeFades.push_back({liveHwnd, now, localLive.fadeDuration});
                liveHwnd = acquireWindow();
                liveVisible = false;
            } else if (liveVisible && liveHwnd) {
                ShowWindow(liveHwnd, SW_HIDE);
                liveVisible = false;
            }
        } else if (localLive.active && localLive.dirty) {
            if (!liveHwnd) liveHwnd = acquireWindow();
            if (liveHwnd) {
                liveVisible = RenderVisualToWindow(liveHwnd, localLive.visual, !liveVisible);
            }
        }

        if (activeFades.empty()) {
            continue;
        }

        if (FAILED(DwmFlush())) {
            Sleep(8);
        }

        QueryPerformanceCounter(&now);

        for (auto it = activeFades.begin(); it != activeFades.end();) {
            double elapsedMs = (double)(now.QuadPart - it->startTime.QuadPart) * 1000.0 / (double)freq.QuadPart;
            if (elapsedMs >= (double)it->duration) {
                releaseWindow(it->hwnd);
                it = activeFades.erase(it);
            } else {
                float t = (float)(elapsedMs / (double)it->duration);
                float eased = (1.0f - t) * (1.0f - t);
                BYTE constAlpha = (BYTE)(255.0f * eased);

                BLENDFUNCTION blend = {};
                blend.BlendOp = AC_SRC_OVER;
                blend.BlendFlags = 0;
                blend.SourceConstantAlpha = constAlpha;
                blend.AlphaFormat = AC_SRC_ALPHA;

                UpdateLayeredWindow(it->hwnd, nullptr, nullptr, nullptr, nullptr, nullptr, 0, &blend, ULW_ALPHA);
                ++it;
            }
        }
    }

    if (liveHwnd) DestroyWindow(liveHwnd);
    for (auto& f : activeFades) DestroyWindow(f.hwnd);
    for (HWND wnd : windowPool) DestroyWindow(wnd);
    UnregisterClassW(OVERLAY_CLASS, GetModuleHandleW(nullptr));
    return 0;
}

bool TryInterceptMarqueeAlphaBlend(
    int wDest, int hDest, int wSrc, int hSrc, BLENDFUNCTION ftn
) {
    if (!t_isSelecting || t_inAlphaBlend) {
        return false;
    }

    bool isMarqueeBlend = false;
    if (ftn.BlendOp == AC_SRC_OVER) {
        if (ftn.AlphaFormat == 0 &&
            ftn.SourceConstantAlpha >= 20 &&
            ftn.SourceConstantAlpha <= 180) {
            isMarqueeBlend = true;
        } else if (ftn.AlphaFormat == AC_SRC_ALPHA &&
                   ftn.SourceConstantAlpha >= 20 &&
                   ftn.SourceConstantAlpha <= 110) {
            isMarqueeBlend = true;
        } else if (ftn.AlphaFormat == AC_SRC_ALPHA &&
                   ftn.SourceConstantAlpha == 255 &&
                   wSrc <= 2 && hSrc <= 2 &&
                   wDest >= 4 && hDest >= 4) {
            isMarqueeBlend = true;
        }
    }

    if (isMarqueeBlend) {
        t_marqueeDetected = true;

        POINT currPt;
        if (GetCursorPos(&currPt)) {
            UpdateLiveOverlay(t_targetWnd, t_startPt, currPt);
        }
        return true;
    }

    return false;
}

BOOL WINAPI GdiAlphaBlend_Hook(
    HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
    HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc,
    BLENDFUNCTION ftn
) {
    if (TryInterceptMarqueeAlphaBlend(wDest, hDest, wSrc, hSrc, ftn)) {
        return TRUE;
    }

    t_inAlphaBlend = true;
    BOOL res = GdiAlphaBlend_Original(
        hdcDest, xoriginDest, yoriginDest, wDest, hDest,
        hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn
    );
    t_inAlphaBlend = false;
    return res;
}

BOOL WINAPI MsimgAlphaBlend_Hook(
    HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
    HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc,
    BLENDFUNCTION ftn
) {
    if (TryInterceptMarqueeAlphaBlend(wDest, hDest, wSrc, hSrc, ftn)) {
        return TRUE;
    }

    t_inAlphaBlend = true;
    BOOL res = MsimgAlphaBlend_Original(
        hdcDest, xoriginDest, yoriginDest, wDest, hDest,
        hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn
    );
    t_inAlphaBlend = false;
    return res;
}

HRESULT WINAPI BufferedPaintSetAlpha_Hook(HPAINTBUFFER hBufferedPaint, const RECT* prc, BYTE alpha) {
    if (t_isSelecting && alpha >= 20 && alpha <= 180) {
        t_marqueeDetected = true;

        POINT currPt;
        if (GetCursorPos(&currPt)) {
            UpdateLiveOverlay(t_targetWnd, t_startPt, currPt);
        }

        if (prc == nullptr) {
            t_suppressPaintBuffer = hBufferedPaint;
            return S_OK;
        } else {
            return BufferedPaintSetAlpha_Original(hBufferedPaint, prc, 0);
        }
    }

    return BufferedPaintSetAlpha_Original(hBufferedPaint, prc, alpha);
}

HRESULT WINAPI EndBufferedPaint_Hook(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget) {
    if (t_suppressPaintBuffer && hBufferedPaint == t_suppressPaintBuffer) {
        t_suppressPaintBuffer = nullptr;
        return EndBufferedPaint_Original(hBufferedPaint, FALSE);
    }
    return EndBufferedPaint_Original(hBufferedPaint, fUpdateTarget);
}

int WINAPI FrameRect_Hook(HDC hDC, const RECT* lprc, HBRUSH hbr) {
    if (t_isSelecting && t_marqueeDetected && lprc &&
        (lprc->right - lprc->left) >= 4 &&
        (lprc->bottom - lprc->top) >= 4) {
        return 1;
    }

    return FrameRect_Original(hDC, lprc, hbr);
}

LRESULT WINAPI DispatchMessageW_Hook(const MSG* lpMsg) {
    if (!lpMsg) {
        return DispatchMessageW_Original(lpMsg);
    }

    if (lpMsg->message == WM_LBUTTONDOWN) {
        if (IsDesktopOrExplorerFileArea(lpMsg->hwnd)) {
            t_isSelecting = true;
            t_marqueeDetected = false;
            t_suppressPaintBuffer = nullptr;
            t_startPt = lpMsg->pt;
            t_targetWnd = lpMsg->hwnd;
        }

        LRESULT res = DispatchMessageW_Original(lpMsg);

        if (t_isSelecting && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0) {
            bool hadMarquee = t_marqueeDetected;
            HWND target = t_targetWnd;
            POINT startPt = t_startPt;

            t_isSelecting = false;
            t_marqueeDetected = false;
            t_suppressPaintBuffer = nullptr;
            t_targetWnd = nullptr;

            POINT endPt;
            if (!GetCursorPos(&endPt)) {
                DWORD msgPos = GetMessagePos();
                endPt = { GET_X_LPARAM(msgPos), GET_Y_LPARAM(msgPos) };
            }

            if (hadMarquee) {
                CommitLiveOverlayToFade(target, startPt, endPt);
            } else {
                CancelLiveOverlay();
            }
        }

        return res;
    }

    if (lpMsg->message == WM_MOUSEMOVE) {
        if (t_isSelecting && t_marqueeDetected) {
            UpdateLiveOverlay(t_targetWnd, t_startPt, lpMsg->pt);
        }
        return DispatchMessageW_Original(lpMsg);
    }

    if (lpMsg->message == WM_LBUTTONUP) {
        bool wasSelecting = t_isSelecting;
        bool hadMarquee = t_marqueeDetected;
        HWND target = t_targetWnd;
        POINT startPt = t_startPt;
        POINT endPt = lpMsg->pt;

        if (wasSelecting) {
            t_isSelecting = false;
            t_marqueeDetected = false;
            t_suppressPaintBuffer = nullptr;
            t_targetWnd = nullptr;
        }

        LRESULT res = DispatchMessageW_Original(lpMsg);

        if (wasSelecting) {
            if (hadMarquee) {
                CommitLiveOverlayToFade(target, startPt, endPt);
            } else {
                CancelLiveOverlay();
            }
        }

        return res;
    }

    if (lpMsg->message == WM_CAPTURECHANGED || lpMsg->message == WM_CANCELMODE) {
        if (t_isSelecting && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0) {
            t_isSelecting = false;
            t_marqueeDetected = false;
            t_suppressPaintBuffer = nullptr;
            t_targetWnd = nullptr;
            CancelLiveOverlay();
        }
    }

    return DispatchMessageW_Original(lpMsg);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Selection Marquee Fade-Out Init (v1.9.2)");
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_animThread = CreateThread(nullptr, 0, AnimationThreadProc, nullptr, 0, nullptr);

    HMODULE gdiFull = GetModuleHandleW(L"gdi32full.dll");
    void* pGdiAlphaBlend = gdiFull ? (void*)GetProcAddress(gdiFull, "GdiAlphaBlend") : nullptr;
    if (!pGdiAlphaBlend) {
        HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
        pGdiAlphaBlend = gdi32 ? (void*)GetProcAddress(gdi32, "GdiAlphaBlend") : nullptr;
    }
    if (pGdiAlphaBlend) {
        Wh_SetFunctionHook(pGdiAlphaBlend, (void*)GdiAlphaBlend_Hook, (void**)&GdiAlphaBlend_Original);
    }

    HMODULE msimg32 = LoadLibraryW(L"msimg32.dll");
    void* pMsimgAlphaBlend = msimg32 ? (void*)GetProcAddress(msimg32, "AlphaBlend") : nullptr;
    if (pMsimgAlphaBlend && pMsimgAlphaBlend != pGdiAlphaBlend) {
        Wh_SetFunctionHook(pMsimgAlphaBlend, (void*)MsimgAlphaBlend_Hook, (void**)&MsimgAlphaBlend_Original);
    }

    HMODULE uxtheme = LoadLibraryW(L"uxtheme.dll");
    if (uxtheme) {
        void* pSetAlpha = (void*)GetProcAddress(uxtheme, "BufferedPaintSetAlpha");
        void* pEndPaint = (void*)GetProcAddress(uxtheme, "EndBufferedPaint");
        if (pSetAlpha) {
            Wh_SetFunctionHook(pSetAlpha, (void*)BufferedPaintSetAlpha_Hook, (void**)&BufferedPaintSetAlpha_Original);
        }
        if (pEndPaint) {
            Wh_SetFunctionHook(pEndPaint, (void*)EndBufferedPaint_Hook, (void**)&EndBufferedPaint_Original);
        }
    }

    Wh_SetFunctionHook((void*)FrameRect, (void*)FrameRect_Hook, (void**)&FrameRect_Original);
    Wh_SetFunctionHook((void*)DispatchMessageW, (void*)DispatchMessageW_Hook, (void**)&DispatchMessageW_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Selection Marquee Fade-Out Uninit");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_animThread) {
        WaitForSingleObject(g_animThread, 2000);
        CloseHandle(g_animThread);
        g_animThread = nullptr;
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}