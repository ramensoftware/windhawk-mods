// ==WindhawkMod==
// @id              taskbar-left-speed-meter
// @name            Taskbar Left Speed Meter
// @description     Displays real-time Internet download and upload speed on the bottom-left corner of the taskbar with custom colors and fonts.
// @version         1.1.0
// @author          RAKIBUL ISLAM SANI
// @github          https://github.com/rakibulislamsani
// @include         windhawk.exe
// @compilerOptions -DWIN32_LEAN_AND_MEAN -liphlpapi -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Left Speed Meter

A dedicated, lightweight network traffic monitor docked on the bottom-left corner of the Windows taskbar (specifically tailored for centered taskbars in Windows 11).

### Comparison with existing mods
- **net-speed-taskbar:** Offers comprehensive chart and minimal layouts anywhere along the taskbar. In contrast, this mod focuses strictly on a simple, lightweight text readout anchored at the bottom-left corner, adding deep font customization (arbitrary font families, bold styling) and custom HEX color selection.
- **taskbar-system-info:** Displays multi-resource readouts (CPU/GPU/RAM/Network) together. If you only want a standalone network speed monitor without system resource counters, this mod provides that focused experience.

### Features
- Real-time download and upload speed monitoring.
- Deep visual styling via the Settings tab:
  - Custom arbitrary text color (HEX code support).
  - Custom font family (e.g. Segoe UI, Consolas, Arial, Tahoma).
  - Toggleable bold text styling.
  - Custom font size with automatic per-monitor DPI scaling.
  - Horizontal margin offset adjustments.
- Auto-hides when applications go fullscreen (videos, games) or when the taskbar auto-hides.
- Click-through transparent overlay that never blocks underlying taskbar elements.
- Runs in an isolated dedicated tool process for zero risk to shell stability.

### Placement Note
On Windows 11 with centered taskbar icons, this docks in the empty bottom-left corner. If you have the Widgets (Weather) icon enabled at the left corner, adjust the `Left Margin (X)` setting to position the meter neatly alongside it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- textColor: "#00FF7F"
  $name: Text Color (HEX)
  $description: "HEX color code (e.g. #00FF7F for Neon Green, #00E5FF for Cyan, #FFFFFF for White, #000000 for Black)"
- fontName: "Segoe UI"
  $name: Font Name
  $description: "Font family name (e.g. Segoe UI, Consolas, Arial, Tahoma, Calibri)"
- isBold: true
  $name: Bold Text
  $description: "Enable bold styling for the speed text"
- fontSize: 13
  $name: Font Size
  $description: "Base text font size (Auto-scaled by monitor DPI. Default: 13)"
- posX: 20
  $name: Left Margin (X)
  $description: "Horizontal offset in pixels from the left corner (Default: 20)"
- hideOnFullscreen: true
  $name: Hide on Fullscreen / Taskbar Auto-Hide
  $description: "Automatically hide the speed meter when a window is fullscreen or the taskbar is hidden"
*/
// ==/WindhawkModSettings==

#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <windows.h>
#include <shellapi.h>
#include <windhawk_utils.h>
#include <atomic>
#include <stdlib.h>
#include <stdio.h>

#define TIMER_ID 1001
#define WM_REFRESH_SETTINGS (WM_APP + 1)
#define TRANSPARENT_COLOR RGB(255, 0, 255)

std::atomic<HWND> hOverlayWnd{NULL};
HANDLE hThread = NULL;
DWORD dwThreadId = 0;
std::atomic<bool> bRunning{true};

COLORREF g_textColor = RGB(0, 255, 127);
wchar_t g_fontName[64] = L"Segoe UI";
BOOL g_isBold = TRUE;
int g_fontSize = 13;
int g_posX = 20;
BOOL g_hideOnFullscreen = TRUE;

ULONG64 prevInBytes = 0, prevOutBytes = 0;
ULONGLONG prevTickCount = 0;
bool hasBaseline = false;
wchar_t displayText[128] = L"▼ 0.0 KB/s\n▲ 0.0 KB/s";

HINSTANCE GetCurrentModuleHandle() {
    HINSTANCE hInst = NULL;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&GetCurrentModuleHandle, &hInst);
    return hInst;
}

UINT GetTaskbarDpi(HWND hWnd) {
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (hUser32) {
        typedef UINT(WINAPI * GetDpiForWindow_t)(HWND);
        GetDpiForWindow_t pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(hUser32, "GetDpiForWindow");
        if (pGetDpiForWindow && hWnd) {
            return pGetDpiForWindow(hWnd);
        }
    }
    HDC hdc = GetDC(NULL);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
    return dpi ? dpi : 96;
}

COLORREF HexToColorRef(PCWSTR hexColor) {
    if (!hexColor || wcslen(hexColor) < 6) return RGB(0, 255, 127);
    const wchar_t* p = hexColor;
    if (*p == L'#') p++;
    unsigned int r = 0, g = 0, b = 0;
    if (swscanf(p, L"%02x%02x%02x", &r, &g, &b) == 3) {
        COLORREF color = RGB(r, g, b);
        if (color == TRANSPARENT_COLOR) {
            color = RGB(255, 0, 254);
        }
        return color;
    }
    return RGB(0, 255, 127);
}

void LoadSettings() {
    WindhawkUtils::StringSetting colorSetting(L"textColor");
    g_textColor = HexToColorRef(colorSetting.get());

    WindhawkUtils::StringSetting fontSetting(L"fontName");
    if (fontSetting.get() && wcslen(fontSetting.get()) > 0) {
        wcsncpy_s(g_fontName, 64, fontSetting.get(), _TRUNCATE);
    } else {
        wcsncpy_s(g_fontName, 64, L"Segoe UI", _TRUNCATE);
    }

    g_isBold = Wh_GetIntSetting(L"isBold") != 0;

    g_fontSize = Wh_GetIntSetting(L"fontSize");
    if (g_fontSize <= 0) g_fontSize = 13;

    g_posX = Wh_GetIntSetting(L"posX");
    if (g_posX < 0) g_posX = 20;

    g_hideOnFullscreen = Wh_GetIntSetting(L"hideOnFullscreen") != 0;
}

void FetchNetworkTraffic(ULONG64 *inBytes, ULONG64 *outBytes) {
    *inBytes = 0;
    *outBytes = 0;
    PMIB_IF_TABLE2 pTable = NULL;

    if (GetIfTable2(&pTable) == NO_ERROR) {
        for (ULONG i = 0; i < pTable->NumEntries; i++) {
            const MIB_IF_ROW2& row = pTable->Table[i];
            if (row.InterfaceAndOperStatusFlags.HardwareInterface &&
                !row.InterfaceAndOperStatusFlags.FilterInterface &&
                row.Type != MIB_IF_TYPE_LOOPBACK) {
                *inBytes += row.InOctets;
                *outBytes += row.OutOctets;
            }
        }
        FreeMibTable(pTable);
    }
}

BOOL ShouldHideMeter(HWND hTaskbar) {
    if (!g_hideOnFullscreen) return FALSE;

    if (!hTaskbar || !IsWindowVisible(hTaskbar)) return TRUE;

    RECT rcTaskbar;
    GetWindowRect(hTaskbar, &rcTaskbar);
    HMONITOR hMon = MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfo(hMon, &mi)) {
        if (rcTaskbar.top >= mi.rcMonitor.bottom - 4 || (rcTaskbar.bottom - rcTaskbar.top) <= 5) {
            return TRUE;
        }
    }

    HWND hFore = GetForegroundWindow();
    if (hFore && hFore != hTaskbar && hFore != GetDesktopWindow()) {
        HMONITOR hForeMon = MonitorFromWindow(hFore, MONITOR_DEFAULTTONEAREST);
        if (hForeMon == hMon) {
            RECT rcFore;
            GetWindowRect(hFore, &rcFore);
            if (EqualRect(&rcFore, &mi.rcMonitor)) {
                wchar_t className[256];
                GetClassName(hFore, className, 256);
                if (wcscmp(className, L"Progman") != 0 && wcscmp(className, L"WorkerW") != 0) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

void UpdatePosition(HWND hTaskbar) {
    HWND hWnd = hOverlayWnd.load();
    if (!hTaskbar || !hWnd) return;

    RECT rcTaskbar;
    GetWindowRect(hTaskbar, &rcTaskbar);

    UINT dpi = GetTaskbarDpi(hTaskbar);
    int width = MulDiv(140, dpi, 96);
    int height = MulDiv(40, dpi, 96);
    int posXScaled = MulDiv(g_posX, dpi, 96);

    int x = rcTaskbar.left + posXScaled;
    int y = rcTaskbar.top + ((rcTaskbar.bottom - rcTaskbar.top - height) / 2);

    SetWindowPos(hWnd, HWND_TOPMOST, x, y, width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void RefreshSpeed() {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    HWND hWnd = hOverlayWnd.load();

    if (!hTaskbar) {
        hasBaseline = false;
        if (hWnd && IsWindowVisible(hWnd)) {
            ShowWindow(hWnd, SW_HIDE);
        }
        return;
    }

    ULONG64 curIn = 0, curOut = 0;
    FetchNetworkTraffic(&curIn, &curOut);
    ULONGLONG curTick = GetTickCount64();

    if (!hasBaseline) {
        prevInBytes = curIn;
        prevOutBytes = curOut;
        prevTickCount = curTick;
        hasBaseline = true;
    }

    ULONGLONG elapsedMs = curTick - prevTickCount;
    if (elapsedMs == 0) elapsedMs = 1;

    double downSpeed = 0.0;
    double upSpeed = 0.0;

    if (curIn >= prevInBytes) {
        downSpeed = (double)(curIn - prevInBytes) / (elapsedMs / 1000.0) / 1024.0;
    }
    if (curOut >= prevOutBytes) {
        upSpeed = (double)(curOut - prevOutBytes) / (elapsedMs / 1000.0) / 1024.0;
    }

    prevInBytes = curIn;
    prevOutBytes = curOut;
    prevTickCount = curTick;

    if (ShouldHideMeter(hTaskbar)) {
        if (hWnd && IsWindowVisible(hWnd)) {
            ShowWindow(hWnd, SW_HIDE);
        }
        return;
    } else {
        if (hWnd && !IsWindowVisible(hWnd)) {
            ShowWindow(hWnd, SW_SHOWNOACTIVATE);
        }
    }

    wchar_t downStr[64], upStr[64];
    if (downSpeed >= 1024.0)
        swprintf(downStr, 64, L"▼ %.2f MB/s", downSpeed / 1024.0);
    else
        swprintf(downStr, 64, L"▼ %.1f KB/s", downSpeed);

    if (upSpeed >= 1024.0)
        swprintf(upStr, 64, L"▲ %.2f MB/s", upSpeed / 1024.0);
    else
        swprintf(upStr, 64, L"▲ %.1f KB/s", upSpeed);

    swprintf(displayText, 128, L"%s\n%s", downStr, upStr);

    UpdatePosition(hTaskbar);
    if (hWnd) {
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

LRESULT CALLBACK OverlayProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == TIMER_ID) {
                RefreshSpeed();
            }
            return 0;

        case WM_REFRESH_SETTINGS:
            LoadSettings();
            RefreshSpeed();
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rc;
            GetClientRect(hWnd, &rc);

            HBRUSH hTransBrush = CreateSolidBrush(TRANSPARENT_COLOR);
            FillRect(hdc, &rc, hTransBrush);
            DeleteObject(hTransBrush);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, g_textColor);

            HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
            UINT dpi = GetTaskbarDpi(hTaskbar);
            int scaledFontSize = MulDiv(g_fontSize, dpi, 96);

            int weight = g_isBold ? FW_BOLD : FW_NORMAL;
            HFONT hFont = CreateFont(scaledFontSize, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     NONANTIALIASED_QUALITY, DEFAULT_PITCH, g_fontName);
            HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

            DrawText(hdc, displayText, -1, &rc, DT_LEFT | DT_TOP);

            SelectObject(hdc, oldFont);
            DeleteObject(hFont);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_NCHITTEST:
            return HTTRANSPARENT;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

DWORD WINAPI OverlayThreadProc(LPVOID lpParam) {
    HINSTANCE hInst = GetCurrentModuleHandle();

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"WindhawkTaskbarLeftMeterWindow";
    
    if (!RegisterClass(&wc)) {
        Wh_Log(L"RegisterClass failed: %u", GetLastError());
        return 0;
    }

    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    UINT dpi = GetTaskbarDpi(hTaskbar);
    int width = MulDiv(140, dpi, 96);
    int height = MulDiv(40, dpi, 96);

    HWND createdWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"",
        WS_POPUP, 0, 0, width, height,
        NULL, NULL, hInst, NULL
    );

    hOverlayWnd.store(createdWnd);

    if (createdWnd) {
        SetLayeredWindowAttributes(createdWnd, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
        FetchNetworkTraffic(&prevInBytes, &prevOutBytes);
        prevTickCount = GetTickCount64();
        hasBaseline = true;
        RefreshSpeed();
        SetTimer(createdWnd, TIMER_ID, 1000, NULL);
    }

    MSG msg;
    while (bRunning.load() && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    HWND currentWnd = hOverlayWnd.exchange(NULL);
    if (currentWnd) {
        KillTimer(currentWnd, TIMER_ID);
        DestroyWindow(currentWnd);
    }

    UnregisterClass(wc.lpszClassName, hInst);
    return 0;
}

// Tool Mod callbacks
BOOL WhTool_ModInit() {
    LoadSettings();
    bRunning.store(true);
    hThread = CreateThread(NULL, 0, OverlayThreadProc, NULL, 0, &dwThreadId);
    return (hThread != NULL);
}

void WhTool_ModSettingsChanged() {
    HWND hWnd = hOverlayWnd.load();
    if (hWnd) {
        PostMessage(hWnd, WM_REFRESH_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    bRunning.store(false);
    if (dwThreadId) {
        PostThreadMessage(dwThreadId, WM_QUIT, 0, 0);
    }
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        hThread = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
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
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
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
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") /
                       sizeof(WCHAR)) -
                      1];
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
