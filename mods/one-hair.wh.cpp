// ==WindhawkMod==
// @id              one-hair
// @name            One Hair
// @description     A hair on your screen. Always on top.
// @version         1.0
// @author          Acercandr0
// @github          https://github.com/Acercandr0
// @include         windhawk.exe
// @compilerOptions -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# One Hair

**The first mod designed to make your Windows experience slightly worse.**

One Hair places a single, slightly annoying hair on your screen. It’s always on top, and moves randomly.

## Why?
Why not?

## Features
- **One persistent hair** always on top, always there.
- **Configurable movement interval** – default: 5 minutes (or whatever you set).
- **Random positions** because a hair that stays still is boring.
- **Runs in its own process** won't crash your Explorer. (But it might annoy you.)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- moveInterval: 300
  $name: Move interval (seconds)
  $description: Seconds between each random jump (minimum 1, default 300)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <shellapi.h>
#include <strsafe.h>

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

static DWORD g_moveIntervalMs = 300000; // 300 segundos por defecto

// ─────────────────────────────────────────────
//  Globals
// ─────────────────────────────────────────────
static HWND   g_hwnd   = nullptr;
static HANDLE g_hThread = nullptr;
static HANDLE g_hWindowReady = nullptr;

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
    if (!hdcScr) {
        Wh_Log(L"RenderHair: GetDC failed");
        return;
    }
    HDC hdcBig = CreateCompatibleDC(hdcScr);
    HDC hdcOut = CreateCompatibleDC(hdcScr);

    DWORD* bigBits = nullptr;
    DWORD* outBits = nullptr;

    HBITMAP hBig    = CreateDIB32(hdcScr, SW, SH, &bigBits);
    HBITMAP hOut    = CreateDIB32(hdcScr, HAIR_W, HAIR_H, &outBits);
    if (!hBig || !hOut) {
        Wh_Log(L"RenderHair: CreateDIB32 failed");
        if (hBig) DeleteObject(hBig);
        if (hOut) DeleteObject(hOut);
        DeleteDC(hdcBig);
        DeleteDC(hdcOut);
        ReleaseDC(nullptr, hdcScr);
        return;
    }

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

    BOOL result = UpdateLayeredWindow(hwnd, hdcScr, &ptDst, &sz,
                                      hdcOut, &ptSrc, 0, &bf, ULW_ALPHA);
    if (!result) {
        Wh_Log(L"RenderHair: UpdateLayeredWindow failed, error=%d", GetLastError());
    }

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
    if (sw > HAIR_W * 2)
        posX = HAIR_W + rand() % (sw - HAIR_W * 2);
    else
        posX = 10;
    if (sh > HAIR_H * 2)
        posY = HAIR_H + rand() % (sh - HAIR_H * 2);
    else
        posY = 10;
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
        Wh_Log(L"WM_CREATE: window created, pos=%d,%d", posX, posY);
        RenderHair(hwnd, posX, posY);
        SetTimer(hwnd, TIMER_ID, g_moveIntervalMs, nullptr);
        if (g_hWindowReady)
            SetEvent(g_hWindowReady);
        return 0;

    case WM_TIMER:
        if (wp == TIMER_ID) {
            PickRandomPosition(posX, posY);
            Wh_Log(L"WM_TIMER: moving to %d,%d", posX, posY);
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
    if (!RegisterClassEx(&wc)) {
        Wh_Log(L"HairThreadProc: RegisterClassEx failed, error=%d", GetLastError());
        return 1;
    }

    g_hwnd = CreateWindowEx(
        WS_EX_LAYERED     |
        WS_EX_TRANSPARENT |
        WS_EX_TOPMOST     |
        WS_EX_TOOLWINDOW,
        L"OneHairOverlay",
        L"One Hair",
        WS_POPUP,
        0, 0, HAIR_W, HAIR_H,
        nullptr, nullptr, wc.hInstance, nullptr);

    if (!g_hwnd) {
        Wh_Log(L"HairThreadProc: CreateWindowEx failed, error=%d", GetLastError());
        return 1;
    }

    ShowWindow(g_hwnd, SW_SHOW);
    Wh_Log(L"HairThreadProc: window created successfully, HWND=0x%p", g_hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Wh_Log(L"HairThreadProc: exiting message loop");
    return 0;
}

// ─────────────────────────────────────────────
//  Tool mod implementation
// ─────────────────────────────────────────────
static void LoadSettings()
{
    int newInterval = Wh_GetIntSetting(L"moveInterval");
    if (newInterval < 1) newInterval = 1;
    g_moveIntervalMs = newInterval * 1000;
    Wh_Log(L"Settings loaded: interval=%d ms", g_moveIntervalMs);
}

BOOL WhTool_ModInit()
{
    Wh_Log(L"WhTool_ModInit called");

    LoadSettings();

    g_hWindowReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_hWindowReady) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_hThread = CreateThread(nullptr, 0, HairThreadProc, nullptr, 0, nullptr);
    if (!g_hThread) {
        Wh_Log(L"CreateThread failed");
        CloseHandle(g_hWindowReady);
        return FALSE;
    }

    WaitForSingleObject(g_hWindowReady, 5000);
    return TRUE;
}


void WhTool_ModSettingsChanged()
{
    LoadSettings();

    if (g_hwnd) {
        PostMessage(g_hwnd, WM_UPDATE_INTERVAL, 0, 0);
    } else {
        Wh_Log(L"g_hwnd is null, cannot update interval");
    }
}

void WhTool_ModUninit()
{
    Wh_Log(L"WhTool_ModUninit called");

    if (g_hwnd) {
        PostMessage(g_hwnd, WM_EXIT_THREAD, 0, 0);
    }

    if (g_hThread) {
        WaitForSingleObject(g_hThread, 5000);
        CloseHandle(g_hThread);
    }

    if (g_hWindowReady) CloseHandle(g_hWindowReady);
}

// ============================================================================
//  Tool mod launcher code (from Windhawk wiki — unmodified)
// ============================================================================
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}