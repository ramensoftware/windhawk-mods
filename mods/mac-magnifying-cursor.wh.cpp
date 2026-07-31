// ==WindhawkMod==
// @id           mac-magnifying-cursor
// @name         macOS magnifying cursor
// @description  Recreates the macOS "Shake to Find" feature by enlarging the cursor when rapidly moved.
// @version      1.4.3
// @github       https://github.com/alivca
// @author       Jaali
// @include      windhawk.exe
// @compilerOptions -luser32 -lgdi32
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
  $description: "Total mouse movement distance required to trigger the effect (recommended: 3500)."
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
#include <vector>
#include <cmath>
#include <atomic>
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

const DWORD g_cursorIds[13] = {
    OCR_NORMAL, OCR_IBEAM, OCR_WAIT, OCR_CROSS, OCR_UP,
    OCR_SIZENWSE, OCR_SIZENESW, OCR_SIZEWE, OCR_SIZENS,
    OCR_SIZEALL, OCR_NO, OCR_HAND, OCR_APPSTARTING
};

struct Settings {
    float maxScale       = 4.0f;
    float minScale       = 1.0f;
    float lerpSpeedUp    = 0.40f;
    float lerpSpeedDown  = 0.15f;
    float shakeThreshold = 3500.0f;
    int   shakeWindowMs  = 500;
} g_settings;

void LoadSettings() {
    int maxScalePct = Wh_GetIntSetting(L"maxScalePercent");
    if (maxScalePct > 0) {
        if (maxScalePct > 1000) maxScalePct = 1000;
        if (maxScalePct < 100) maxScalePct = 100;
        g_settings.maxScale = maxScalePct / 100.0f;
    }

    int thresh = Wh_GetIntSetting(L"shakeThreshold");
    if (thresh > 0) g_settings.shakeThreshold = static_cast<float>(thresh);

    int speedUp = Wh_GetIntSetting(L"lerpSpeedUpPercent");
    if (speedUp > 0) g_settings.lerpSpeedUp = speedUp / 100.0f;

    int speedDown = Wh_GetIntSetting(L"lerpSpeedDownPercent");
    if (speedDown > 0) g_settings.lerpSpeedDown = speedDown / 100.0f;
}

typedef int (WINAPI *pfn_GetObjectW)(HANDLE, int, LPVOID);
typedef HDC (WINAPI *pfn_CreateCompatibleDC)(HDC);
typedef HBITMAP (WINAPI *pfn_CreateDIBSection)(HDC, CONST BITMAPINFO *, UINT, VOID **, HANDLE, DWORD);
typedef HGDIOBJ (WINAPI *pfn_SelectObject)(HDC, HGDIOBJ);
typedef BOOL (WINAPI *pfn_DeleteObject)(HGDIOBJ);
typedef BOOL (WINAPI *pfn_DeleteDC)(HDC);
typedef BOOL (WINAPI *pfn_SetWindowBand)(HWND hWnd, HWND hwndInsertAfter, DWORD dwBand);
typedef BOOL (WINAPI *pfn_SetThreadDpiAwarenessContext)(HANDLE);

struct GdiApi {
    pfn_GetObjectW pGetObjectW = nullptr;
    pfn_CreateCompatibleDC pCreateCompatibleDC = nullptr;
    pfn_CreateDIBSection pCreateDIBSection = nullptr;
    pfn_SelectObject pSelectObject = nullptr;
    pfn_DeleteObject pDeleteObject = nullptr;
    pfn_DeleteDC pDeleteDC = nullptr;

    bool Init() {
        HMODULE hGdi = GetModuleHandleW(L"gdi32.dll");
        if (!hGdi) hGdi = LoadLibraryW(L"gdi32.dll");
        if (!hGdi) return false;

        pGetObjectW = (pfn_GetObjectW)GetProcAddress(hGdi, "GetObjectW");
        pCreateCompatibleDC = (pfn_CreateCompatibleDC)GetProcAddress(hGdi, "CreateCompatibleDC");
        pCreateDIBSection = (pfn_CreateDIBSection)GetProcAddress(hGdi, "CreateDIBSection");
        pSelectObject = (pfn_SelectObject)GetProcAddress(hGdi, "SelectObject");
        pDeleteObject = (pfn_DeleteObject)GetProcAddress(hGdi, "DeleteObject");
        pDeleteDC = (pfn_DeleteDC)GetProcAddress(hGdi, "DeleteDC");

        return pGetObjectW && pCreateCompatibleDC && pCreateDIBSection && 
               pSelectObject && pDeleteObject && pDeleteDC;
    }
} g_gdi;

struct PointTime {
    POINT pt;
    DWORD time;
};

struct ModState {
    HWND hwndOverlay = NULL;
    HANDLE hThread = NULL;
    std::atomic<bool> running{false};
    std::vector<PointTime> history;
    float currentScale = 1.0f;
    float targetScale = 1.0f;
    bool cursorHidden = false;
    HCURSOR hSavedCursor = NULL;
} g_state;

HCURSOR CreateBlankCursor() {
    int w = GetSystemMetrics(SM_CXCURSOR);
    int h = GetSystemMetrics(SM_CYCURSOR);
    if (w <= 0) w = 32;
    if (h <= 0) h = 32;
    std::vector<BYTE> andMask((w * h) / 8, 0xFF);
    std::vector<BYTE> xorMask((w * h) / 8, 0x00);
    return CreateCursor(GetModuleHandle(NULL), 0, 0, w, h, andMask.data(), xorMask.data());
}

void HideAllSystemCursors() {
    if (!g_state.cursorHidden) {
        HCURSOR hBlank = CreateBlankCursor();
        if (hBlank) {
            for (int i = 0; i < 13; ++i) {
                SetSystemCursor(CopyIcon(hBlank), g_cursorIds[i]);
            }
            DestroyIcon(hBlank);
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
    DWORD now = GetTickCount();

    POINT pt;
    GetCursorPos(&pt);

    g_state.history.push_back({ pt, now });

    while (!g_state.history.empty() && (now - g_state.history.front().time > static_cast<DWORD>(g_settings.shakeWindowMs))) {
        g_state.history.erase(g_state.history.begin());
    }

    float totalPath = 0.0f;
    if (g_state.history.size() >= 2) {
        for (size_t i = 1; i < g_state.history.size(); ++i) {
            totalPath += GetDistance(g_state.history[i - 1].pt, g_state.history[i].pt);
        }
        float netDisp = GetDistance(g_state.history.front().pt, g_state.history.back().pt);

        if (totalPath > g_settings.shakeThreshold && (totalPath / (netDisp + 1.0f) > 1.3f)) {
            g_state.targetScale = g_settings.maxScale;
        } else if (totalPath < 80.0f) {
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
                g_gdi.pGetObjectW(ii.hbmMask, sizeof(BITMAP), &bm);

                int baseWidth = bm.bmWidth;
                int baseHeight = ii.hbmColor ? bm.bmHeight : (bm.bmHeight / 2);
                if (baseWidth == 0) baseWidth = 32;
                if (baseHeight == 0) baseHeight = 32;

                int scaledW = static_cast<int>(baseWidth * g_state.currentScale);
                int scaledH = static_cast<int>(baseHeight * g_state.currentScale);

                int hotspotX = static_cast<int>(ii.xHotspot * g_state.currentScale);
                int hotspotY = static_cast<int>(ii.yHotspot * g_state.currentScale);

                int winX = pt.x - hotspotX;
                int winY = pt.y - hotspotY;

                HDC hdcScreen = GetDC(NULL);
                if (hdcScreen) {
                    HDC hdcMem = g_gdi.pCreateCompatibleDC(hdcScreen);
                    if (hdcMem) {
                        BITMAPINFO bmi = { 0 };
                        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                        bmi.bmiHeader.biWidth = scaledW;
                        bmi.bmiHeader.biHeight = -scaledH;
                        bmi.bmiHeader.biPlanes = 1;
                        bmi.bmiHeader.biBitCount = 32;
                        bmi.bmiHeader.biCompression = BI_RGB;

                        void* pBits = nullptr;
                        HBITMAP hbmMem = g_gdi.pCreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
                        if (hbmMem && pBits) {
                            HBITMAP hOldBm = static_cast<HBITMAP>(g_gdi.pSelectObject(hdcMem, hbmMem));
                            ZeroMemory(pBits, static_cast<size_t>(scaledW) * scaledH * 4);

                            DrawIconEx(hdcMem, 0, 0, g_state.hSavedCursor, scaledW, scaledH, 0, NULL, DI_NORMAL);

                            POINT ptZero = { 0, 0 };
                            SIZE sizeWin = { scaledW, scaledH };
                            POINT ptWin = { winX, winY };
                            BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

                            UpdateLayeredWindow(g_state.hwndOverlay, hdcScreen, &ptWin, &sizeWin, hdcMem, &ptZero, 0, &blend, ULW_ALPHA);

                            g_gdi.pSelectObject(hdcMem, hOldBm);
                            g_gdi.pDeleteObject(hbmMem);
                        }
                        g_gdi.pDeleteDC(hdcMem);
                    }
                    ReleaseDC(NULL, hdcScreen);
                }

                if (ii.hbmMask) g_gdi.pDeleteObject(ii.hbmMask);
                if (ii.hbmColor) g_gdi.pDeleteObject(ii.hbmColor);

                SetWindowPos(g_state.hwndOverlay, HWND_TOPMOST, winX, winY, scaledW, scaledH,
                             SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOOWNERZORDER);
            }
        }
    } else {
        ShowWindow(g_state.hwndOverlay, SW_HIDE);
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
        case WM_DESTROY:
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI CursorMonitorThread(LPVOID lpParam) {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    
    if (hUser32) {
        auto pSetDpi = (pfn_SetThreadDpiAwarenessContext)GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
        if (pSetDpi) pSetDpi((HANDLE)-4);
    }

    if (!g_gdi.Init()) return 0;

    const wchar_t* kClassName = L"WindhawkShakeCursorExclusiveOverlay";

    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = kClassName;

    UnregisterClass(kClassName, wc.hInstance);
    if (!RegisterClassEx(&wc)) {
        return 0;
    }

    g_state.hwndOverlay = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"", WS_POPUP,
        0, 0, 100, 100,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!g_state.hwndOverlay) {
        UnregisterClass(kClassName, wc.hInstance);
        return 0;
    }

    if (hUser32) {
        auto pSetWindowBand = (pfn_SetWindowBand)GetProcAddress(hUser32, "SetWindowBand");
        if (pSetWindowBand) {
            pSetWindowBand(g_state.hwndOverlay, NULL, 2);
        }
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

    UnregisterClass(kClassName, wc.hInstance);
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();

    SystemParametersInfoW(SPI_SETCURSORS, 0, NULL, 0);

    g_state.running = true;
    g_state.hThread = CreateThread(NULL, 0, CursorMonitorThread, NULL, 0, NULL);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    g_state.running = false;
    if (g_state.hwndOverlay) {
        PostMessage(g_state.hwndOverlay, WM_CLOSE, 0, 0);
    }
    if (g_state.hThread) {
        WaitForSingleObject(g_state.hThread, INFINITE);
        CloseHandle(g_state.hThread);
        g_state.hThread = NULL;
    }
    RestoreAllSystemCursors();
}

// === Обязательные точки входа Windhawk ===
BOOL Wh_ModInit() {
    return WhTool_ModInit();
}

void Wh_ModSettingsChanged() {
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    WhTool_ModUninit();
}
