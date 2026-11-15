// ==WindhawkMod==
// @id           mac-magnifying-cursor
// @name         macOS magnifying cursor
// @description  Recreates the macOS "Shake to Find" feature by enlarging the cursor when rapidly moved.
// @version      1.4.7
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
- shakeThreshold: 2500
  $name: Shake sensitivity threshold
  $description: "Total mouse movement distance required to trigger (lower = more sensitive, recommended: 2500-3500)."
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
#include <atomic>
#include <windhawk_api.h>

#ifndef OCR_NORMAL
#define OCR_NORMAL 32512
#define OCR_IBEAM 32513
#define OCR_WAIT 32514
#define OCR_CROSS 32515
#define OCR_UP 32516
#define OCR_SIZENWSE 32642
#define OCR_SIZENESW 32643
#define OCR_SIZEWE 32644
#define OCR_SIZENS 32645
#define OCR_SIZEALL 32646
#define OCR_NO 32648
#define OCR_HAND 32649
#define OCR_APPSTARTING 32650
#define OCR_HELP 32651
#endif

const DWORD g_cursorIds[] = {
    OCR_NORMAL, OCR_IBEAM, OCR_WAIT, OCR_CROSS, OCR_UP,
    OCR_SIZENWSE, OCR_SIZENESW, OCR_SIZEWE, OCR_SIZENS,
    OCR_SIZEALL, OCR_NO, OCR_HAND, OCR_APPSTARTING, OCR_HELP
};
const size_t kCursorCount = sizeof(g_cursorIds) / sizeof(g_cursorIds[0]);

struct Settings {
    std::atomic<float> maxScale{4.0f};
    std::atomic<float> lerpSpeedUp{0.40f};
    std::atomic<float> lerpSpeedDown{0.15f};
    std::atomic<float> shakeThreshold{800.0f};
    std::atomic<int>   shakeWindowMs{500};
} g_settings;

void LoadSettings() {
    int maxScalePct = Wh_GetIntSetting(L"maxScalePercent");
    if (maxScalePct < 100) maxScalePct = 100;
    if (maxScalePct > 1000) maxScalePct = 1000;
    g_settings.maxScale.store(maxScalePct / 100.0f);

    int thresh = Wh_GetIntSetting(L"shakeThreshold");
    g_settings.shakeThreshold.store((thresh > 0) ? static_cast<float>(thresh) : 800.0f);

    int speedUp = Wh_GetIntSetting(L"lerpSpeedUpPercent");
    g_settings.lerpSpeedUp.store((speedUp > 0 && speedUp <= 100) ? (speedUp / 100.0f) : 0.40f);

    int speedDown = Wh_GetIntSetting(L"lerpSpeedDownPercent");
    g_settings.lerpSpeedDown.store((speedDown > 0 && speedDown <= 100) ? (speedDown / 100.0f) : 0.15f);
}

struct PointTime {
    POINT pt;
    ULONGLONG time;
};

struct SystemCursorBackup {
    DWORD id;
    HCURSOR hOriginalCopy;
};

struct ModState {
    HWND hwndOverlay = NULL;
    HANDLE hThread = NULL;
    DWORD dwThreadId = 0;
    HANDLE hThreadReadyEvent = NULL;
    std::deque<PointTime> history;
    float currentScale = 1.0f;
    float targetScale = 1.0f;
    bool cursorHidden = false;
    bool isVisible = false;
    std::vector<SystemCursorBackup> sysCursorBackups;
    UINT currentTimerInterval = 40;
} g_state;

LONG WINAPI RestoreCursorsOnCrash(EXCEPTION_POINTERS*) {
    SystemParametersInfoW(SPI_SETCURSORS, 0, NULL, 0);
    return EXCEPTION_CONTINUE_SEARCH;
}

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

void BackupAndHideAllSystemCursors() {
    if (g_state.cursorHidden) return;

    for (auto& item : g_state.sysCursorBackups) {
        if (item.hOriginalCopy) DestroyIcon(item.hOriginalCopy);
    }
    g_state.sysCursorBackups.clear();

    // 1. Сохраняем копии оригинальных иконок всех системных курсоров
    for (size_t i = 0; i < kCursorCount; ++i) {
        DWORD id = g_cursorIds[i];
        HCURSOR hSys = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(id), IMAGE_CURSOR, 0, 0, LR_SHARED);
        if (hSys) {
            HCURSOR hCopy = CopyIcon(hSys);
            if (hCopy) {
                g_state.sysCursorBackups.push_back({ id, hCopy });
            }
        }
    }

    // 2. Скрываем все системные курсоры
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

void RestoreAllSystemCursors() {
    if (g_state.cursorHidden) {
        SystemParametersInfoW(SPI_SETCURSORS, 0, NULL, 0);
        g_state.cursorHidden = false;
    }
    for (auto& item : g_state.sysCursorBackups) {
        if (item.hOriginalCopy) DestroyIcon(item.hOriginalCopy);
    }
    g_state.sysCursorBackups.clear();
}

HCURSOR GetActiveDrawableCursor(HCURSOR hActiveSys) {
    if (!hActiveSys) return NULL;

    // Сравниваем хэндл активного курсора со стандартными ID системных курсоров
    for (const auto& item : g_state.sysCursorBackups) {
        HCURSOR hCurrentSysHandle = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(item.id), IMAGE_CURSOR, 0, 0, LR_SHARED);
        if (hActiveSys == hCurrentSysHandle) {
            return item.hOriginalCopy;
        }
    }

    // Если у приложения кастомная иконка курсора — берем напрямую
    return hActiveSys;
}

bool IsFullscreenOrGameActive() {
    static ULONGLONG lastCheck = 0;
    static bool cachedResult = false;
    ULONGLONG now = GetTickCount64();

    if (now - lastCheck < 500) {
        return cachedResult;
    }
    lastCheck = now;

    QUERY_USER_NOTIFICATION_STATE state;
    if (SHQueryUserNotificationState(&state) == S_OK) {
        if (state == QUNS_RUNNING_D3D_FULL_SCREEN ||
            state == QUNS_BUSY ||
            state == QUNS_PRESENTATION_MODE) {
            cachedResult = true;
            return true;
        }
    }

    cachedResult = false;
    return false;
}

void SetMonitorTimerInterval(UINT intervalMs) {
    if (g_state.currentTimerInterval != intervalMs && g_state.hwndOverlay) {
        SetTimer(g_state.hwndOverlay, 1, intervalMs, NULL);
        g_state.currentTimerInterval = intervalMs;
    }
}

float GetDistance(POINT a, POINT b) {
    float dx = static_cast<float>(a.x - b.x);
    float dy = static_cast<float>(a.y - b.y);
    return std::sqrt(dx * dx + dy * dy);
}

void UpdateFrame() {
    ULONGLONG now = GetTickCount64();

    POINT pt{};
    if (!GetCursorPos(&pt) || IsFullscreenOrGameActive()) {
        g_state.history.clear();
        g_state.targetScale = 1.0f;
    } else {
        g_state.history.push_back({ pt, now });

        float shakeThreshold = g_settings.shakeThreshold.load();
        float maxScale = g_settings.maxScale.load();
        int shakeWindowMs = g_settings.shakeWindowMs.load();

        while (!g_state.history.empty() && (now - g_state.history.front().time > static_cast<ULONGLONG>(shakeWindowMs))) {
            g_state.history.pop_front();
        }

        float totalPath = 0.0f;
        if (g_state.history.size() >= 2) {
            for (size_t i = 1; i < g_state.history.size(); ++i) {
                totalPath += GetDistance(g_state.history[i - 1].pt, g_state.history[i].pt);
            }
            float netDisp = GetDistance(g_state.history.front().pt, g_state.history.back().pt);

            float shakeRatio = totalPath / (netDisp + 1.0f);
            if (totalPath > shakeThreshold && shakeRatio > 1.2f) {
                g_state.targetScale = maxScale;
            } else if (totalPath < shakeThreshold * 0.4f || shakeRatio <= 1.1f) {
                g_state.targetScale = 1.0f;
            }
        }
    }

    float lerpSpeed = (g_state.targetScale > g_state.currentScale) ? g_settings.lerpSpeedUp.load() : g_settings.lerpSpeedDown.load();
    g_state.currentScale += (g_state.targetScale - g_state.currentScale) * lerpSpeed;

    if (g_state.currentScale > 1.001f || g_state.targetScale > 1.001f) {
        SetMonitorTimerInterval(16);
    } else {
        SetMonitorTimerInterval(40);
    }

    if (g_state.currentScale > 1.02f) {
        BackupAndHideAllSystemCursors();

        CURSORINFO ci = { sizeof(CURSORINFO) };
        if (GetCursorInfo(&ci) && (ci.flags & CURSOR_SHOWING) && ci.hCursor) {
            HCURSOR hCursorToDraw = GetActiveDrawableCursor(ci.hCursor);
            if (hCursorToDraw) {
                ICONINFO ii = { 0 };
                if (GetIconInfo(hCursorToDraw, &ii)) {
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

                                DrawIconEx(hdcMem, 0, 0, hCursorToDraw, scaledW, scaledH, 0, NULL, DI_NORMAL);

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

                    if (!g_state.isVisible) {
                        SetWindowPos(g_state.hwndOverlay, HWND_TOPMOST, 0, 0, 0, 0,
                                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
                        g_state.isVisible = true;
                    }
                }
            }
        }
    } else {
        if (g_state.isVisible) {
            ShowWindow(g_state.hwndOverlay, SW_HIDE);
            g_state.isVisible = false;
        }
        if (g_state.cursorHidden) {
            RestoreAllSystemCursors();
        }
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
            g_state.hwndOverlay = NULL;
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
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = kClassName;

    if (!RegisterClassEx(&wc)) {
        if (g_state.hThreadReadyEvent) SetEvent(g_state.hThreadReadyEvent);
        return 0;
    }

    g_state.hwndOverlay = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClassName, L"", WS_POPUP,
        0, 0, 1, 1,
        NULL, NULL, hInstance, NULL
    );

    if (!g_state.hwndOverlay) {
        UnregisterClass(kClassName, hInstance);
        if (g_state.hThreadReadyEvent) SetEvent(g_state.hThreadReadyEvent);
        return 0;
    }

    SetTimer(g_state.hwndOverlay, 1, g_state.currentTimerInterval, NULL);

    if (g_state.hThreadReadyEvent) {
        SetEvent(g_state.hThreadReadyEvent);
    }

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

    UnregisterClass(kClassName, hInstance);
    return 0;
}

BOOL WhTool_ModInit() {
    SetUnhandledExceptionFilter(RestoreCursorsOnCrash);
    LoadSettings();

    g_state.hThreadReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!g_state.hThreadReadyEvent) return FALSE;

    g_state.hThread = CreateThread(NULL, 0, CursorMonitorThread, NULL, 0, &g_state.dwThreadId);
    if (!g_state.hThread) {
        CloseHandle(g_state.hThreadReadyEvent);
        g_state.hThreadReadyEvent = NULL;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    if (g_state.hThreadReadyEvent) {
        WaitForSingleObject(g_state.hThreadReadyEvent, INFINITE);
    }

    if (g_state.hwndOverlay) {
        PostMessage(g_state.hwndOverlay, WM_CLOSE, 0, 0);
    }

    if (g_state.hThread) {
        WaitForSingleObject(g_state.hThread, INFINITE);
        CloseHandle(g_state.hThread);
        g_state.hThread = NULL;
    }

    if (g_state.hThreadReadyEvent) {
        CloseHandle(g_state.hThreadReadyEvent);
        g_state.hThreadReadyEvent = NULL;
    }

    RestoreAllSystemCursors();
}

// ============================================================================
// Windhawk Tool Mod Launcher Boilerplate
// ============================================================================

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

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
            Wh_Log(L"Tool mod process already exists");
            CloseHandle(g_toolModProcessMutex);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            Wh_Log(L"WhTool_ModInit failed");
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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
    DWORD length = GetModuleFileName(nullptr, currentProcessPath,
                                     ARRAYSIZE(currentProcessPath));
    if (length == 0 || length == ARRAYSIZE(currentProcessPath)) {
        Wh_Log(L"GetModuleFileName failed");
        return;
    }

    WCHAR commandLine[MAX_PATH * 2];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
              WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
    }
    if (!kernelModule) {
        Wh_Log(L"GetModuleHandle failed");
        return;
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hNewToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"GetProcAddress failed");
        return;
    }

    STARTUPINFO si = {sizeof(si)};
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi = {0};
    if (pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                nullptr, nullptr, &si, &pi, nullptr)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        Wh_Log(L"CreateProcessInternalW failed");
    }
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
