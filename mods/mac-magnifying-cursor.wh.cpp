// ==WindhawkMod==
// @id           mac-magnifying-cursor
// @name         macOS magnifying cursor
// @description  Recreates the macOS "Shake to Find" feature by enlarging the cursor when rapidly moved.
// @version      1.4.5
// @github       https://github.com/alivca
// @author       Jaali
// @include      windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Recreates the macOS "Shake to Find" feature: rapidly shaking your mouse temporarily enlarges the cursor so you can instantly locate it on screen.
![Preview](https://raw.githubusercontent.com/alivca/windhawk-mods-gif/main/github.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxScalePercent: 400
  $name: Maximum size (%)
  $description: "How much the cursor enlarges (e.g. 400 = 4x scale)."
- shakeThreshold: 3500
  $name: Shake sensitivity threshold
  $description: "Total mouse movement distance required to trigger (lower = more sensitive, recommended: 3000-4000)."
- lerpSpeedUpPercent: 40
  $name: Enlarge speed (%)
  $description: "How fast the cursor expands (recommended: 20-40)."
- lerpSpeedDownPercent: 15
  $name: Shrink speed (%)
  $description: "How fast the cursor shrinks back (recommended: 10-20)."
*/
// ==/WindhawkModSettings==

#define OEMRESOURCE
#include <windows.h>
#include <shellapi.h>
#include <deque>
#include <vector>
#include <cmath>
#include <windhawk_api.h>

#ifndef OCR_NORMAL
#define OCR_NORMAL 32512
#endif
#ifndef OCR_IBEAM
#define OCR_IBEAM 32513
#endif
#ifndef OCR_WAIT
#define OCR_WAIT 32514
#endif
#ifndef OCR_CROSS
#define OCR_CROSS 32515
#endif
#ifndef OCR_UP
#define OCR_UP 32516
#endif
#ifndef OCR_SIZENWSE
#define OCR_SIZENWSE 32642
#endif
#ifndef OCR_SIZENESW
#define OCR_SIZENESW 32643
#endif
#ifndef OCR_SIZEWE
#define OCR_SIZEWE 32644
#endif
#ifndef OCR_SIZENS
#define OCR_SIZENS 32645
#endif
#ifndef OCR_SIZEALL
#define OCR_SIZEALL 32646
#endif
#ifndef OCR_NO
#define OCR_NO 32648
#endif
#ifndef OCR_HAND
#define OCR_HAND 32649
#endif
#ifndef OCR_APPSTARTING
#define OCR_APPSTARTING 32650
#endif
#ifndef OCR_HELP
#define OCR_HELP 32651
#endif

const DWORD g_cursorIds[] = {
    OCR_NORMAL, OCR_IBEAM, OCR_WAIT, OCR_CROSS, OCR_UP,
    OCR_SIZENWSE, OCR_SIZENESW, OCR_SIZEWE, OCR_SIZENS,
    OCR_SIZEALL, OCR_NO, OCR_HAND, OCR_APPSTARTING, OCR_HELP
};
const size_t kCursorCount = sizeof(g_cursorIds) / sizeof(g_cursorIds[0]);

struct Settings {
    float maxScale       = 4.0f;
    float minScale       = 1.0f;
    float lerpSpeedUp    = 0.40f;
    float lerpSpeedDown  = 0.15f;
    float shakeThreshold = 800.0f;
    int   shakeWindowMs  = 500;
} g_settings;

void LoadSettings() {
    int maxScalePct = Wh_GetIntSetting(L"maxScalePercent");
    if (maxScalePct < 100) maxScalePct = 400;
    if (maxScalePct > 1000) maxScalePct = 1000;
    g_settings.maxScale = maxScalePct / 100.0f;

    int thresh = Wh_GetIntSetting(L"shakeThreshold");
    g_settings.shakeThreshold = (thresh > 0) ? static_cast<float>(thresh) : 800.0f;

    int speedUp = Wh_GetIntSetting(L"lerpSpeedUpPercent");
    g_settings.lerpSpeedUp = (speedUp > 0 && speedUp <= 100) ? (speedUp / 100.0f) : 0.40f;

    int speedDown = Wh_GetIntSetting(L"lerpSpeedDownPercent");
    g_settings.lerpSpeedDown = (speedDown > 0 && speedDown <= 100) ? (speedDown / 100.0f) : 0.15f;
}

struct PointTime {
    POINT pt;
    ULONGLONG time;
};

struct ModState {
    HWND hwndOverlay = NULL;
    HANDLE hThread = NULL;
    DWORD dwThreadId = 0;
    std::deque<PointTime> history;
    float currentScale = 1.0f;
    float targetScale = 1.0f;
    bool cursorHidden = false;
    bool isVisible = false;
    HCURSOR hSavedCursor = NULL;
} g_state;

HCURSOR CreateBlankCursor() {
    int w = GetSystemMetrics(SM_CXCURSOR);
    int h = GetSystemMetrics(SM_CYCURSOR);
    if (w <= 0) w = 32;
    if (h <= 0) h = 32;

    size_t maskSize = (static_cast<size_t>(w + 7) / 8) * h;
    std::vector<BYTE> andMask(maskSize, 0xFF);
    std::vector<BYTE> xorMask(maskSize, 0x00);
    return CreateCursor(GetModuleHandle(NULL), 0, 0, w, h, andMask.data(), xorMask.data());
}

void HideAllSystemCursors() {
    if (!g_state.cursorHidden) {
        HCURSOR hBlank = CreateBlankCursor();
        if (hBlank) {
            for (size_t i = 0; i < kCursorCount; ++i) {
                HCURSOR hCopy = CopyIcon(hBlank);
                if (hCopy) {
                    if (!SetSystemCursor(hCopy, g_cursorIds[i])) {
                        DestroyIcon(hCopy);
                    }
                }
            }
            DestroyCursor(hBlank);
            g_state.cursorHidden = true;
        }
    }
}

void RestoreAllSystemCursors() {
    if (g_state.cursorHidden) {
        SystemParametersInfoW(SPI_SETCURSORS, 0, NULL, 0);
        g_state.cursorHidden = false;
    }
    if (g_state.hSavedCursor) {
        DestroyIcon(g_state.hSavedCursor);
        g_state.hSavedCursor = NULL;
    }
}

float GetDistance(POINT a, POINT b) {
    float dx = static_cast<float>(a.x - b.x);
    float dy = static_cast<float>(a.y - b.y);
    return std::sqrt(dx * dx + dy * dy);
}

void UpdateFrame() {
    ULONGLONG now = GetTickCount64();

    POINT pt;
    GetCursorPos(&pt);

    g_state.history.push_back({ pt, now });

    while (!g_state.history.empty() && (now - g_state.history.front().time > static_cast<ULONGLONG>(g_settings.shakeWindowMs))) {
        g_state.history.pop_front();
    }

    float totalPath = 0.0f;
    if (g_state.history.size() >= 2) {
        for (size_t i = 1; i < g_state.history.size(); ++i) {
            totalPath += GetDistance(g_state.history[i - 1].pt, g_state.history[i].pt);
        }
        float netDisp = GetDistance(g_state.history.front().pt, g_state.history.back().pt);

        float shakeRatio = totalPath / (netDisp + 1.0f);
        if (totalPath > g_settings.shakeThreshold && shakeRatio > 1.2f) {
            g_state.targetScale = g_settings.maxScale;
        } else if (totalPath < g_settings.shakeThreshold * 0.4f || shakeRatio <= 1.1f) {
            g_state.targetScale = g_settings.minScale;
        }
    }

    float lerpSpeed = (g_state.targetScale > g_state.currentScale) ? g_settings.lerpSpeedUp : g_settings.lerpSpeedDown;
    g_state.currentScale += (g_state.targetScale - g_state.currentScale) * lerpSpeed;

    if (g_state.currentScale > 1.02f) {
        if (!g_state.cursorHidden) {
            CURSORINFO ci = { sizeof(CURSORINFO) };
            if (GetCursorInfo(&ci) && (ci.flags & CURSOR_SHOWING) && ci.hCursor) {
                if (g_state.hSavedCursor) DestroyIcon(g_state.hSavedCursor);
                g_state.hSavedCursor = CopyIcon(ci.hCursor);
                if (g_state.hSavedCursor) {
                    HideAllSystemCursors();
                }
            }
        }

        if (g_state.hSavedCursor) {
            ICONINFO ii = { 0 };
            if (GetIconInfo(g_state.hSavedCursor, &ii)) {
                BITMAP bm = { 0 };
                GetObjectW(ii.hbmMask, sizeof(BITMAP), &bm);

                int baseWidth = bm.bmWidth;
                int baseHeight = ii.hbmColor ? bm.bmHeight : (bm.bmHeight / 2);
                if (baseWidth <= 0) baseWidth = 32;
                if (baseHeight <= 0) baseHeight = 32;

                int scaledW = static_cast<int>(baseWidth * g_state.currentScale);
                int scaledH = static_cast<int>(baseHeight * g_state.currentScale);

                int hotspotX = static_cast<int>(ii.xHotspot * g_state.currentScale);
                int hotspotY = static_cast<int>(ii.yHotspot * g_state.currentScale);

                int winX = pt.x - hotspotX;
                int winY = pt.y - hotspotY;

                HDC hdcScreen = GetDC(NULL);
                if (hdcScreen) {
                    HDC hdcMem = CreateCompatibleDC(hdcScreen);
                    if (hdcMem) {
                        BITMAPINFO bmi = { 0 };
                        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                        bmi.bmiHeader.biWidth = scaledW;
                        bmi.bmiHeader.biHeight = -scaledH;
                        bmi.bmiHeader.biPlanes = 1;
                        bmi.bmiHeader.biBitCount = 32;
                        bmi.bmiHeader.biCompression = BI_RGB;

                        void* pBits = nullptr;
                        HBITMAP hbmMem = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
                        if (hbmMem && pBits) {
                            HBITMAP hOldBm = static_cast<HBITMAP>(SelectObject(hdcMem, hbmMem));

                            DWORD* pPixels = static_cast<DWORD*>(pBits);
                            size_t pixelCount = static_cast<size_t>(scaledW) * scaledH;

                            for (size_t i = 0; i < pixelCount; ++i) {
                                pPixels[i] = 0x00000001;
                            }

                            DrawIconEx(hdcMem, 0, 0, g_state.hSavedCursor, scaledW, scaledH, 0, NULL, DI_NORMAL);

                            bool hasAlpha = false;
                            for (size_t i = 0; i < pixelCount; ++i) {
                                if ((pPixels[i] & 0xFF000000) != 0) {
                                    hasAlpha = true;
                                    break;
                                }
                            }

                            if (!hasAlpha) {
                                for (size_t i = 0; i < pixelCount; ++i) {
                                    if (pPixels[i] == 0x00000001) {
                                        pPixels[i] = 0x00000000;
                                    } else {
                                        pPixels[i] |= 0xFF000000;
                                    }
                                }
                            }

                            POINT ptZero = { 0, 0 };
                            SIZE sizeWin = { scaledW, scaledH };
                            POINT ptWin = { winX, winY };
                            BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

                            UpdateLayeredWindow(g_state.hwndOverlay, hdcScreen, &ptWin, &sizeWin, hdcMem, &ptZero, 0, &blend, ULW_ALPHA);

                            SelectObject(hdcMem, hOldBm);
                            DeleteObject(hbmMem);
                        }
                        DeleteDC(hdcMem);
                    }
                    ReleaseDC(NULL, hdcScreen);
                }

                if (ii.hbmMask) DeleteObject(ii.hbmMask);
                if (ii.hbmColor) DeleteObject(ii.hbmColor);

                SetWindowPos(g_state.hwndOverlay, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

                g_state.isVisible = true;
            }
        }
    } else {
        if (g_state.isVisible) {
            ShowWindow(g_state.hwndOverlay, SW_HIDE);
            g_state.isVisible = false;
        }
        RestoreAllSystemCursors();
    }
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == 1) {
                UpdateFrame();
            }
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_QUERYENDSESSION:
        case WM_ENDSESSION:
            RestoreAllSystemCursors();
            return TRUE;
        case WM_DESTROY:
            KillTimer(hwnd, 1);
            RestoreAllSystemCursors();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

typedef BOOL (WINAPI *pfn_SetThreadDpiAwarenessContext)(HANDLE);

DWORD WINAPI CursorMonitorThread(LPVOID lpParam) {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        auto pSetDpi = (pfn_SetThreadDpiAwarenessContext)GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
        if (pSetDpi) pSetDpi((HANDLE)-4);
    }

    const wchar_t* kClassName = L"WindhawkShakeCursorExclusiveOverlay";

    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = kClassName;

    if (!RegisterClassEx(&wc)) {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            return 0;
        }
    }

    g_state.hwndOverlay = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClassName, L"", WS_POPUP,
        0, 0, 1, 1,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (!g_state.hwndOverlay) {
        return 0;
    }

    SetTimer(g_state.hwndOverlay, 1, 16, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    RestoreAllSystemCursors();

    if (g_state.hwndOverlay) {
        DestroyWindow(g_state.hwndOverlay);
        g_state.hwndOverlay = NULL;
    }

    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_state.hThread = CreateThread(NULL, 0, CursorMonitorThread, NULL, 0, &g_state.dwThreadId);
    return g_state.hThread != NULL;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    if (g_state.hwndOverlay) {
        PostMessage(g_state.hwndOverlay, WM_CLOSE, 0, 0);
    } else if (g_state.dwThreadId != 0) {
        PostThreadMessage(g_state.dwThreadId, WM_QUIT, 0, 0);
    }

    if (g_state.hThread) {
        WaitForSingleObject(g_state.hThread, 2000);
        CloseHandle(g_state.hThread);
        g_state.hThread = NULL;
    }
    RestoreAllSystemCursors();
}
