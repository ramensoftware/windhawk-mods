// ==WindhawkMod==
// @id              one-hair
// @name            One Hair
// @description     A hair on your screen. Always on top.
// @version         1.0
// @author          Acercandr0
// @github          https://github.com/Acercandr0
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- moveInterval: 60
  $name: Move interval (seconds)
  $description: Seconds between each random jump (minimum 1)
*/

#include <windows.h>
#include <cstdlib>
#include <ctime>

// ─────────────────────────────────────────────
//  Config
// ─────────────────────────────────────────────
static const int   HAIR_W   = 60;
static const int   HAIR_H   = 230;
static const int   SS       = 4;
static const DWORD TIMER_ID = 1;
static const UINT  WM_UPDATE_INTERVAL = WM_APP + 1;
static const UINT  WM_EXIT_THREAD     = WM_APP + 2;

static const BYTE  HAIR_R = 35;
static const BYTE  HAIR_G = 22;
static const BYTE  HAIR_B = 10;
static const BYTE  HAIR_A = 240;

// Loaded from settings — default 4 seconds
static DWORD g_moveIntervalMs = 4000;

// ─────────────────────────────────────────────
//  Globals
// ─────────────────────────────────────────────
static HWND   g_hwnd   = nullptr;
static HANDLE g_hThread = nullptr;
static HANDLE g_hExitEvent = nullptr;

// ─────────────────────────────────────────────
//  DIB helper
// ─────────────────────────────────────────────
static HBITMAP CreateDIB32(HDC hdc, int w, int h, DWORD** ppBits)
{
    BITMAPINFO bmi              = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    return CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)ppBits, nullptr, 0);
}

// ─────────────────────────────────────────────
//  Render + push to layered window
// ─────────────────────────────────────────────
static void RenderHair(HWND hwnd, int posX, int posY)
{
    const int SW = HAIR_W * SS;
    const int SH = HAIR_H * SS;

    HDC hdcScr = GetDC(nullptr);
    HDC hdcBig = CreateCompatibleDC(hdcScr);
    HDC hdcOut = CreateCompatibleDC(hdcScr);

    DWORD* bigBits = nullptr;
    DWORD* outBits = nullptr;

    HBITMAP hBig    = CreateDIB32(hdcScr, SW, SH, &bigBits);
    HBITMAP hOut    = CreateDIB32(hdcScr, HAIR_W, HAIR_H, &outBits);
    HBITMAP hOldBig = (HBITMAP)SelectObject(hdcBig, hBig);
    HBITMAP hOldOut = (HBITMAP)SelectObject(hdcOut, hOut);

    RECT rc = { 0, 0, SW, SH };
    FillRect(hdcBig, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

    #define SX(v) ((LONG)((v) * SS))
    POINT bp[] = {
        { SX(5),  SX(5)   },   // P0 root
        { SX(5),  SX(6)   },   // C1
        { SX(24), SX(162) },   // C2 — main curve
        { SX(54), SX(222) },   // P1 mid
        { SX(54), SX(223) },   // C3
        { SX(55), SX(225) },   // C4
        { SX(55), SX(225) },   // P2 tip
    };
    #undef SX

    HPEN hPen    = CreatePen(PS_SOLID, 6, RGB(HAIR_R, HAIR_G, HAIR_B));
    HPEN hOldPen = (HPEN)SelectObject(hdcBig, hPen);
    SelectObject(hdcBig, GetStockObject(NULL_BRUSH));

    PolyBezier(hdcBig, bp, 7);

    SelectObject(hdcBig, hOldPen);
    DeleteObject(hPen);

    for (int y = 0; y < HAIR_H; y++) {
        for (int x = 0; x < HAIR_W; x++) {
            int totalLum = 0;
            for (int sy = 0; sy < SS; sy++)
                for (int sx = 0; sx < SS; sx++) {
                    DWORD px = bigBits[(y*SS + sy) * SW + (x*SS + sx)];
                    totalLum += (int)((px >> 16) & 0xFF);
                }

            int maxLum   = SS * SS * 255;
            int coverage = maxLum - totalLum;
            BYTE a  = (BYTE)((coverage * (int)HAIR_A) / maxLum);
            BYTE pr = (BYTE)((HAIR_R * a) / 255);
            BYTE pg = (BYTE)((HAIR_G * a) / 255);
            BYTE pb = (BYTE)((HAIR_B * a) / 255);

            outBits[y * HAIR_W + x] = ((DWORD)a  << 24)
                                     | ((DWORD)pr << 16)
                                     | ((DWORD)pg <<  8)
                                     |  (DWORD)pb;
        }
    }

    POINT ptSrc = { 0, 0 };
    POINT ptDst = { posX, posY };
    SIZE  sz    = { HAIR_W, HAIR_H };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    UpdateLayeredWindow(hwnd, hdcScr, &ptDst, &sz,
                        hdcOut, &ptSrc, 0, &bf, ULW_ALPHA);

    SelectObject(hdcBig, hOldBig);
    SelectObject(hdcOut, hOldOut);
    DeleteObject(hBig);
    DeleteObject(hOut);
    DeleteDC(hdcBig);
    DeleteDC(hdcOut);
    ReleaseDC(nullptr, hdcScr);
}

// ─────────────────────────────────────────────
//  Random position
// ─────────────────────────────────────────────
static void PickRandomPosition(int& posX, int& posY)
{
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    posX = HAIR_W + rand() % (sw - HAIR_W * 2);
    posY = HAIR_H + rand() % (sh - HAIR_H * 2);
}

// ─────────────────────────────────────────────
//  Window procedure
// ─────────────────────────────────────────────
static LRESULT CALLBACK HairWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static int posX = 0, posY = 0;

    switch (msg) {
    case WM_CREATE:
        srand((unsigned)time(nullptr));
        PickRandomPosition(posX, posY);
        RenderHair(hwnd, posX, posY);
        SetTimer(hwnd, TIMER_ID, g_moveIntervalMs, nullptr);
        return 0;

    case WM_TIMER:
        if (wp == TIMER_ID) {
            PickRandomPosition(posX, posY);
            RenderHair(hwnd, posX, posY);
        }
        return 0;

    case WM_UPDATE_INTERVAL:
        KillTimer(hwnd, TIMER_ID);
        SetTimer(hwnd, TIMER_ID, g_moveIntervalMs, nullptr);
        return 0;

    case WM_EXIT_THREAD:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

// ─────────────────────────────────────────────
//  Thread that owns the window and runs message loop
// ─────────────────────────────────────────────
static DWORD WINAPI HairThreadProc(LPVOID lpParam)
{
    WNDCLASSEX wc    = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = HairWndProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = L"OneHairOverlay";
    RegisterClassEx(&wc);

    g_hwnd = CreateWindowEx(
        WS_EX_LAYERED     |
        WS_EX_TRANSPARENT |
        WS_EX_TOPMOST     |
        WS_EX_TOOLWINDOW,
        L"OneHairOverlay",
        L"One Hair",
        WS_POPUP,
        0, 0, HAIR_W, HAIR_H,
        nullptr, nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!g_hwnd) {
        return 1;
    }

    ShowWindow(g_hwnd, SW_SHOWNA);

    // Create a manual-reset event for exit signaling
    g_hExitEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_hExitEvent) {
        DestroyWindow(g_hwnd);
        return 1;
    }

    MSG msg;
    HANDLE handles[2] = { g_hExitEvent, nullptr };

    while (true) {
        DWORD dwResult = MsgWaitForMultipleObjects(1, handles, FALSE, INFINITE, QS_ALLINPUT);

        if (dwResult == WAIT_OBJECT_0) {
            // Exit event signaled → break out of the loop
            break;
        }

        // Process all pending messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                // Quit message received → exit
                goto cleanup;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

cleanup:
    // Clean up
    if (g_hwnd) {
        DestroyWindow(g_hwnd);  // just in case it wasn't destroyed already
        g_hwnd = nullptr;
    }
    
    if (g_hExitEvent) {
        CloseHandle(g_hExitEvent);
        g_hExitEvent = nullptr;
    }
    
    return 0;
}

// ─────────────────────────────────────────────
//  Settings loader
// ─────────────────────────────────────────────
void Wh_ModSettingsChanged()
{
    int secs = Wh_GetIntSetting(L"moveInterval", 4);
    if (secs < 1) secs = 1;
    g_moveIntervalMs = (DWORD)(secs * 1000);

    if (g_hwnd) {
        PostMessage(g_hwnd, WM_UPDATE_INTERVAL, 0, 0);
    }
}

// ─────────────────────────────────────────────
//  Mod entry / exit
// ─────────────────────────────────────────────
BOOL Wh_ModInit()
{
    // Load settings before creating the window
    Wh_ModSettingsChanged();

    g_hThread = CreateThread(nullptr, 0, HairThreadProc, nullptr, 0, nullptr);
    if (!g_hThread) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit()
{
    // Signal the thread to exit
    if (g_hExitEvent) {
        SetEvent(g_hExitEvent);
    }

    // Post a message to the window to destroy itself (optional, but ensures cleanup)
    if (g_hwnd) {
        PostMessage(g_hwnd, WM_EXIT_THREAD, 0, 0);
    }

    // Wait for the thread to finish (max 5 seconds)
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 5000);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
}