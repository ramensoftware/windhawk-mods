// ==WindhawkMod==
// @id           mac-magnifying-cursor
// @name         macOS magnifying cursor
// @description  macOS magnifying cursor
// @version      1.2.1
// @author       Jaali
// @include      explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
macOS magnifying cursor
*/
// ==/WindhawkModReadme==

#define OEMRESOURCE
#include <windows.h>
#include <vector>
#include <cmath>
#include <windhawk_api.h>

#ifndef OCR_NORMAL
#define OCR_NORMAL 32512
#endif


typedef int (WINAPI *pfn_GetObjectW)(HANDLE, int, LPVOID);
typedef HDC (WINAPI *pfn_CreateCompatibleDC)(HDC);
typedef HBITMAP (WINAPI *pfn_CreateDIBSection)(HDC, CONST BITMAPINFO *, UINT, VOID **, HANDLE, DWORD);
typedef HGDIOBJ (WINAPI *pfn_SelectObject)(HDC, HGDIOBJ);
typedef BOOL (WINAPI *pfn_DeleteObject)(HGDIOBJ);
typedef BOOL (WINAPI *pfn_DeleteDC)(HDC);

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


struct Settings {
    float maxScale       = 4.0f;
    float minScale       = 1.0f;
    float lerpSpeedUp    = 0.40f;
    float lerpSpeedDown  = 0.15f;
    float shakeThreshold = 4500.0f;
    int   shakeWindowMs  = 500;
} g_settings;


constexpr float MAX_SCALE        = 4.0f;   // Максимальный размер курсора (3.5x)
constexpr float MIN_SCALE        = 1.0f;   // Обычный размер (1x)
constexpr float LERP_SPEED_UP    = 0.30f;  // Скорость плавного увеличения
constexpr float LERP_SPEED_DOWN  = 0.15f;  // Скорость плавного уменьшения
constexpr int   SHAKE_WINDOW_MS  = 500;    // Окно анализа движений (мс)
constexpr float SHAKE_THRESHOLD  = 4500.0f; // Порог чувствительности

void LoadSettings() {
    int maxScalePct = Wh_GetIntSetting(L"maxScalePercent");
    if (maxScalePct > 0) g_settings.maxScale = maxScalePct / 100.0f;

    int thresh = Wh_GetIntSetting(L"shakeThreshold");
    if (thresh > 0) g_settings.shakeThreshold = static_cast<float>(thresh);

    int speedUp = Wh_GetIntSetting(L"lerpSpeedUpPercent");
    if (speedUp > 0) g_settings.lerpSpeedUp = speedUp / 100.0f;

    int speedDown = Wh_GetIntSetting(L"lerpSpeedDownPercent");
    if (speedDown > 0) g_settings.lerpSpeedDown = speedDown / 100.0f;
}

struct PointTime {
    POINT pt;
    DWORD time;
};

struct ModState {
    HWND hwndOverlay = NULL;
    HANDLE hThread = NULL;
    bool running = false;
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

void HideSystemCursor() {
    if (!g_state.cursorHidden) {
        HCURSOR hBlank = CreateBlankCursor();
        if (hBlank) {
            SetSystemCursor(hBlank, OCR_NORMAL);
            g_state.cursorHidden = true;
        }
    }
}

void RestoreSystemCursor() {
    if (g_state.cursorHidden) {
        SystemParametersInfoW(SPI_SETCURSORS, 0, NULL, 0);
        g_state.cursorHidden = false;
    }
    if (g_state.hSavedCursor) {
        DestroyIcon(g_state.hSavedCursor);
        g_state.hSavedCursor = NULL;
    }
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

float GetDistance(POINT a, POINT b) {
    float dx = static_cast<float>(a.x - b.x);
    float dy = static_cast<float>(a.y - b.y);
    return std::sqrt(dx * dx + dy * dy);
}

DWORD WINAPI CursorMonitorThread(LPVOID lpParam) {
    if (!g_gdi.Init()) return 0;

    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkShakeCursorExclusiveOverlay";
    RegisterClassEx(&wc);

    g_state.hwndOverlay = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"", WS_POPUP,
        0, 0, 100, 100,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!g_state.hwndOverlay) return 0;

    DWORD lastTime = GetTickCount();

    while (g_state.running) {
        DWORD now = GetTickCount();
        DWORD dt = now - lastTime;
        if (dt < 16) {
            Sleep(16 - dt);
            now = GetTickCount();
        }
        lastTime = now;

        POINT pt;
        GetCursorPos(&pt);

        g_state.history.push_back({ pt, now });

        while (!g_state.history.empty() && (now - g_state.history.front().time > SHAKE_WINDOW_MS)) {
            g_state.history.erase(g_state.history.begin());
        }

        float totalPath = 0.0f;
        if (g_state.history.size() >= 2) {
            for (size_t i = 1; i < g_state.history.size(); ++i) {
                totalPath += GetDistance(g_state.history[i - 1].pt, g_state.history[i].pt);
            }
            float netDisp = GetDistance(g_state.history.front().pt, g_state.history.back().pt);

            if (totalPath > SHAKE_THRESHOLD && (totalPath / (netDisp + 1.0f) > 1.3f)) {
                g_state.targetScale = MAX_SCALE;
            } else if (totalPath < 80.0f) {
                g_state.targetScale = MIN_SCALE;
            }
        }

        float lerpSpeed = (g_state.targetScale > g_state.currentScale) ? LERP_SPEED_UP : LERP_SPEED_DOWN;
        g_state.currentScale += (g_state.targetScale - g_state.currentScale) * lerpSpeed;

        if (g_state.currentScale > 1.02f) {
            if (!g_state.cursorHidden) {
                CURSORINFO ci = { sizeof(CURSORINFO) };
                if (GetCursorInfo(&ci) && (ci.flags & CURSOR_SHOWING)) {
                    g_state.hSavedCursor = CopyIcon(ci.hCursor);
                }
                HideSystemCursor();
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
                    HDC hdcMem = g_gdi.pCreateCompatibleDC(hdcScreen);

                    BITMAPINFO bmi = { 0 };
                    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    bmi.bmiHeader.biWidth = scaledW;
                    bmi.bmiHeader.biHeight = -scaledH;
                    bmi.bmiHeader.biPlanes = 1;
                    bmi.bmiHeader.biBitCount = 32;
                    bmi.bmiHeader.biCompression = BI_RGB;

                    void* pBits = nullptr;
                    HBITMAP hbmMem = g_gdi.pCreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
                    HBITMAP hOldBm = static_cast<HBITMAP>(g_gdi.pSelectObject(hdcMem, hbmMem));

                    ZeroMemory(pBits, scaledW * scaledH * 4);

                    DrawIconEx(hdcMem, 0, 0, g_state.hSavedCursor, scaledW, scaledH, 0, NULL, DI_NORMAL);

                    POINT ptZero = { 0, 0 };
                    SIZE sizeWin = { scaledW, scaledH };
                    POINT ptWin = { winX, winY };
                    
                    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

                    UpdateLayeredWindow(g_state.hwndOverlay, hdcScreen, &ptWin, &sizeWin, hdcMem, &ptZero, 0, &blend, ULW_ALPHA);

                    g_gdi.pSelectObject(hdcMem, hOldBm);
                    g_gdi.pDeleteObject(hbmMem);
                    g_gdi.pDeleteDC(hdcMem);
                    ReleaseDC(NULL, hdcScreen);

                    if (ii.hbmMask) g_gdi.pDeleteObject(ii.hbmMask);
                    if (ii.hbmColor) g_gdi.pDeleteObject(ii.hbmColor);

                    ShowWindow(g_state.hwndOverlay, SW_SHOWNOACTIVATE);
                }
            }
        } else {
            ShowWindow(g_state.hwndOverlay, SW_HIDE);
            RestoreSystemCursor();
        }
    }

    RestoreSystemCursor();

    if (g_state.hwndOverlay) {
        DestroyWindow(g_state.hwndOverlay);
    }
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

BOOL Wh_ModInit() {
    g_state.running = true;
    g_state.hThread = CreateThread(NULL, 0, CursorMonitorThread, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    g_state.running = false;
    if (g_state.hThread) {
        WaitForSingleObject(g_state.hThread, 1000);
        CloseHandle(g_state.hThread);
        g_state.hThread = NULL;
    }
    RestoreSystemCursor();
}