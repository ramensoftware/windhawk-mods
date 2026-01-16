// ==WindhawkMod==
// @id              taskbar-shadow-border
// @name            Taskbar Shadow & Border
// @description     Adds a customizable, floating shadow and border effect above the taskbar
// @version         1.0.0
// @author          Lockframe
// @github          https://github.com/Lockframe
// @include         windhawk.exe
// @compilerOptions -lgdi32 -luser32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Shadow & Border

This mod adds a customizable drop shadow and border above the taskbar.

![](https://i.imgur.com/sGw2BaC.png)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ShadowHeight: 20
  $name: Shadow Height (Light/Default)
  $description: Height in pixels when using Light Mode.
- ShadowOpacity: 60
  $name: Shadow Opacity (Light/Default)
  $description: Darkness (0-255) when using Light Mode.
- ShadowHeightDark: 20
  $name: Shadow Height (Dark Mode)
  $description: Height in pixels when using Dark Mode.
- ShadowOpacityDark: 120
  $name: Shadow Opacity (Dark Mode)
  $description: Darkness (0-255) when using Dark Mode.
- BorderWidth: 0
  $name: Border Top Width
  $description: Height of the solid border line (0 to disable).
- BorderColor: "#FF000000"
  $name: Border Color (Light)
  $description: Color in "#RRGGBB" or "#AARRGGBB" format for Light Mode.
- BorderColorDark: "#FFFFFFFF"
  $name: Border Color (Dark)
  $description: Color in "#RRGGBB" or "#AARRGGBB" format for Dark Mode.
- ShadowOffset: 0
  $name: Shadow Offset
  $description: Vertical offset. Positive moves up, negative moves down.
- ShadowAlwaysOnTop: false
  $name: Always Show on Top
  $description: If enabled, the shadow will overlay on top of maximized windows.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <atomic>
#include <strsafe.h>

// ----------------------------------------------------------------------------
// Globals
// ----------------------------------------------------------------------------
#define WM_UPDATE_SETTINGS   (WM_USER + 1)

HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;
WCHAR g_szClassName[64] = {0};
HWND g_hShadow = NULL;  
HWND g_hTaskbar = NULL; 

// Atomic Settings
std::atomic<int> g_shadowHeight(20);
std::atomic<int> g_shadowOpacity(60);
std::atomic<int> g_shadowHeightDark(20);
std::atomic<int> g_shadowOpacityDark(120);
std::atomic<int> g_shadowOffset(0);
std::atomic<bool> g_shadowAlwaysOnTop(false);
std::atomic<bool> g_isDarkMode(false);

std::atomic<int> g_borderWidth(0);
std::atomic<UINT32> g_borderColor(0xFF000000); 
std::atomic<UINT32> g_borderColorDark(0xFFFFFFFF);

// State Cache
struct ShadowState {
    int lastX, lastY, lastW, lastH; 
    int lastOpacity;
    int lastThemeMode; 
    UINT32 lastBorderColor;
    int lastBorderWidth;
    bool lastTopMost;
    bool lastHiddenByFullScreen;
} g_state = {0, 0, 0, 0, 0, -1, 0, 0, false, false};

// GDI Cache
struct GdiCache {
    HDC hdcMem;
    HBITMAP hBitmap;
    HBITMAP hOldBitmap;
    void* pBits;
    int width;
    int height;
} g_cache = {0};

// ----------------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------------
UINT32 HexToRGB(PCWSTR hex) {
    if (!hex || !*hex) return 0xFF000000;
    if (hex[0] == L'#') hex++;
    size_t len = wcslen(hex);
    unsigned int a = 255, r = 0, g = 0, b = 0;
    if (len >= 8) swscanf_s(hex, L"%02x%02x%02x%02x", &a, &r, &g, &b);
    else swscanf_s(hex, L"%02x%02x%02x", &r, &g, &b);
    return (a << 24) | (r << 16) | (g << 8) | b;
}

void UpdateDarkModeState() {
    HKEY hKey;
    DWORD value = 1; 
    DWORD size = sizeof(DWORD);
    LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, 
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 
        0, KEY_READ, &hKey);
    if (res == ERROR_SUCCESS) {
        RegQueryValueEx(hKey, L"SystemUsesLightTheme", NULL, NULL, (LPBYTE)&value, &size);
        RegCloseKey(hKey);
    }
    g_isDarkMode.store(value == 0);
}

// ----------------------------------------------------------------------------
// FullScreen Logic (Fuzzy + Anti-Freeze)
// ----------------------------------------------------------------------------
BOOL IsFullScreenAppActive(HWND hTaskbar) {
    HWND hFore = GetForegroundWindow();
    if (!hFore || hFore == GetShellWindow()) return FALSE;
    if (hFore == hTaskbar) return FALSE;

    // PATCH: If the app is frozen, skip expensive checks to prevent stalling
    if (IsHungAppWindow(hFore)) return FALSE;

    WCHAR szClass[256];
    if (GetClassName(hFore, szClass, 256)) {
        if (wcscmp(szClass, L"WorkerW") == 0 || wcscmp(szClass, L"Progman") == 0) return FALSE;
    }

    RECT rcFore;
    if (DwmGetWindowAttribute(hFore, DWMWA_EXTENDED_FRAME_BOUNDS, &rcFore, sizeof(rcFore)) != S_OK) {
        if (!GetWindowRect(hFore, &rcFore)) return FALSE;
    }
    
    HMONITOR hMonTaskbar = MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTOPRIMARY);
    HMONITOR hMonFore = MonitorFromWindow(hFore, MONITOR_DEFAULTTOPRIMARY);
    
    if (hMonTaskbar != hMonFore) return FALSE;

    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (!GetMonitorInfo(hMonFore, &mi)) return FALSE;

    // Fuzzy Tolerance (20px)
    const long TOLERANCE = 20;

    return (rcFore.left <= (mi.rcMonitor.left + TOLERANCE) && 
            rcFore.top <= (mi.rcMonitor.top + TOLERANCE) && 
            rcFore.right >= (mi.rcMonitor.right - TOLERANCE) && 
            rcFore.bottom >= (mi.rcMonitor.bottom - TOLERANCE));
}

// ----------------------------------------------------------------------------
// Visual Logic
// ----------------------------------------------------------------------------
void FreeGdiCache() {
    if (g_cache.hdcMem) {
        if (g_cache.hOldBitmap) SelectObject(g_cache.hdcMem, g_cache.hOldBitmap);
        if (g_cache.hBitmap) DeleteObject(g_cache.hBitmap);
        DeleteDC(g_cache.hdcMem);
    }
    memset(&g_cache, 0, sizeof(g_cache));
}

void DrawShadow(HWND hShadow, int width, int height, int opacity, int borderWidth, UINT32 borderColor) {
    if (!hShadow || width <= 0 || height <= 0) return;

    // GDI Cache: Only re-alloc if size changes
    if (width != g_cache.width || height != g_cache.height || !g_cache.hdcMem) {
        FreeGdiCache();

        HDC hdcScreen = GetDC(NULL);
        g_cache.hdcMem = CreateCompatibleDC(hdcScreen);
        
        BITMAPINFO bmi = {}; 
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = height; 
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        g_cache.hBitmap = CreateDIBSection(g_cache.hdcMem, &bmi, DIB_RGB_COLORS, &g_cache.pBits, NULL, 0);
        
        if (g_cache.hBitmap && g_cache.pBits) {
             g_cache.hOldBitmap = (HBITMAP)SelectObject(g_cache.hdcMem, g_cache.hBitmap);
             g_cache.width = width;
             g_cache.height = height;
        }
        ReleaseDC(NULL, hdcScreen);
    }

    if (g_cache.pBits) {
        UINT32* pixels = (UINT32*)g_cache.pBits;
        
        UINT8 bA = (borderColor >> 24) & 0xFF;
        UINT8 bR = (borderColor >> 16) & 0xFF;
        UINT8 bG = (borderColor >> 8) & 0xFF;
        UINT8 bB = (borderColor) & 0xFF;

        if (bA != 255) {
            bR = (bR * bA) / 255;
            bG = (bG * bA) / 255;
            bB = (bB * bA) / 255;
        }
        UINT32 premultipliedBorder = (bA << 24) | (bR << 16) | (bG << 8) | bB;

        int safeBorder = (borderWidth > height) ? height : borderWidth;
        int shadowTotalH = height - safeBorder;

        for (int y = 0; y < height; y++) {
            UINT32 pixelVal = 0;
            if (y < safeBorder) {
                pixelVal = premultipliedBorder;
            } else if (shadowTotalH > 0) {
                int shadowY = y - safeBorder; 
                int alpha = (opacity * (shadowTotalH - shadowY)) / shadowTotalH;
                if (alpha < 0) alpha = 0; if (alpha > 255) alpha = 255;
                pixelVal = (alpha << 24); 
            }
            for (int x = 0; x < width; x++) pixels[y * width + x] = pixelVal;
        }

        POINT ptSrc = {0, 0};
        SIZE wndSize = {width, height};
        BLENDFUNCTION blend = {0};
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = 255; 
        blend.AlphaFormat = AC_SRC_ALPHA;

        UpdateLayeredWindow(hShadow, NULL, NULL, &wndSize, g_cache.hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    }
}

void UpdateShadow(BOOL forceRedraw) {
    if (!g_hShadow) return;
    
    // PATCH: Self-Healing Parenting (Fixes Startup Race)
    HWND hCurrentParent = GetParent(g_hShadow);
    HWND hShell = GetShellWindow();
    if (hCurrentParent == NULL && hShell != NULL) {
        // We loaded before Shell was ready; attach now to fix Z-Order
        SetParent(g_hShadow, hShell);
    }

    if (!g_hTaskbar || !IsWindow(g_hTaskbar)) {
        g_hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
        if (!g_hTaskbar) return;
    }

    // 1. Full Screen Logic
    BOOL isFullScreen = IsFullScreenAppActive(g_hTaskbar);
    
    if (isFullScreen) {
        if (!g_state.lastHiddenByFullScreen) {
            ShowWindow(g_hShadow, SW_HIDE);
            g_state.lastHiddenByFullScreen = true;
        }
        return;
    }

    // 2. Visibility Logic
    RECT rcTaskbar;
    if (!GetWindowRect(g_hTaskbar, &rcTaskbar)) return;

    BOOL isVisible = IsWindowVisible(g_hTaskbar);
    HMONITOR hMon = MonitorFromWindow(g_hTaskbar, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (GetMonitorInfo(hMon, &mi)) {
        if (rcTaskbar.top >= mi.rcMonitor.bottom - 2) isVisible = FALSE;
    } else {
        if (rcTaskbar.top >= GetSystemMetrics(SM_CYSCREEN) - 2) isVisible = FALSE;
    }

    if (!isVisible) {
        if (!g_state.lastHiddenByFullScreen) { 
            ShowWindow(g_hShadow, SW_HIDE);
            g_state.lastHiddenByFullScreen = true;
        }
        return;
    }

    // 3. Settings & Position
    bool isDark = g_isDarkMode.load();
    int activeHeight = isDark ? g_shadowHeightDark.load() : g_shadowHeight.load();
    int activeOpacity = isDark ? g_shadowOpacityDark.load() : g_shadowOpacity.load();
    int activeBorderWidth = g_borderWidth.load();
    UINT32 activeBorderColor = isDark ? g_borderColorDark.load() : g_borderColor.load();
    int offset = g_shadowOffset.load();
    bool alwaysOnTop = g_shadowAlwaysOnTop.load();
    
    if (activeBorderWidth > activeHeight) activeHeight = activeBorderWidth;

    int tbWidth = rcTaskbar.right - rcTaskbar.left;
    if (tbWidth <= 0) return;

    int shadowTop = rcTaskbar.top - activeHeight - offset;
    int shadowLeft = rcTaskbar.left;
    
    // 4. Dirty Check
    if (!forceRedraw &&
        !g_state.lastHiddenByFullScreen && 
        g_state.lastX == shadowLeft &&
        g_state.lastY == shadowTop &&
        g_state.lastW == tbWidth &&
        g_state.lastH == activeHeight &&
        g_state.lastTopMost == alwaysOnTop &&
        g_state.lastBorderWidth == activeBorderWidth &&
        g_state.lastBorderColor == activeBorderColor &&
        g_state.lastOpacity == activeOpacity &&
        g_state.lastThemeMode == (int)isDark)
    {
        return;
    }

    g_state.lastHiddenByFullScreen = false;

    // 5. Update Window Position
    HWND zOrderTarget = alwaysOnTop ? g_hTaskbar : HWND_BOTTOM;
    UINT flags = SWP_NOACTIVATE | SWP_SHOWWINDOW;
    if (alwaysOnTop) flags |= SWP_NOOWNERZORDER; 

    SetWindowPos(g_hShadow, zOrderTarget, shadowLeft, shadowTop, tbWidth, activeHeight, flags);

    // Only Redraw if visuals changed
    if (forceRedraw || 
        g_state.lastW != tbWidth || 
        g_state.lastH != activeHeight || 
        g_state.lastOpacity != activeOpacity ||
        g_state.lastBorderWidth != activeBorderWidth ||
        g_state.lastBorderColor != activeBorderColor ||
        g_state.lastThemeMode != (int)isDark) 
    {
        DrawShadow(g_hShadow, tbWidth, activeHeight, activeOpacity, activeBorderWidth, activeBorderColor);
    }
        
    g_state.lastW = tbWidth;
    g_state.lastH = activeHeight;
    g_state.lastOpacity = activeOpacity;
    g_state.lastThemeMode = (int)isDark;
    g_state.lastX = shadowLeft;
    g_state.lastY = shadowTop;
    g_state.lastTopMost = alwaysOnTop;
    g_state.lastBorderWidth = activeBorderWidth;
    g_state.lastBorderColor = activeBorderColor;
}

// ----------------------------------------------------------------------------
// Window Procedure
// ----------------------------------------------------------------------------
LRESULT CALLBACK ShadowWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_NCHITTEST: return HTTRANSPARENT; 
        
        case WM_UPDATE_SETTINGS:
            UpdateDarkModeState();
            UpdateShadow(TRUE);
            return 0;

        case WM_DISPLAYCHANGE: 
            UpdateShadow(TRUE);
            return 0;

        case WM_TIMER:
            if (wParam == 1) UpdateShadow(FALSE);
            return 0;

        case WM_SETTINGCHANGE:
            if (lParam && wcscmp((LPCWSTR)lParam, L"ImmersiveColorSet") == 0) {
                UpdateDarkModeState();
                UpdateShadow(TRUE);
            }
            return 0;

        case WM_WINDOWPOSCHANGING: {
            WINDOWPOS* pPos = (WINDOWPOS*)lParam;
            if ((pPos->flags & SWP_HIDEWINDOW) && g_hTaskbar && IsWindowVisible(g_hTaskbar)) {
                if (!IsFullScreenAppActive(g_hTaskbar)) {
                     pPos->flags &= ~SWP_HIDEWINDOW;
                     pPos->flags |= SWP_SHOWWINDOW;
                }
            }
            return 0;
        }
            
        case WM_CLOSE: DestroyWindow(hwnd); return 0;
        case WM_DESTROY: 
            FreeGdiCache();
            KillTimer(hwnd, 1);
            g_hShadow = NULL; 
            PostQuitMessage(0); 
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ----------------------------------------------------------------------------
// Hooks
// ----------------------------------------------------------------------------
void CALLBACK ThreadWinEventProc(
    HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, 
    LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime
) {
    if (!g_hTaskbar || !IsWindow(g_hTaskbar)) g_hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);

    if (hwnd == g_hTaskbar && idObject == OBJID_WINDOW && idChild == CHILDID_SELF) {
        UpdateShadow(FALSE);
        return;
    }

    if (event == EVENT_SYSTEM_FOREGROUND) {
        if (g_hShadow) PostMessage(g_hShadow, WM_TIMER, 1, 0); 
    }
}

// ----------------------------------------------------------------------------
// Main Thread
// ----------------------------------------------------------------------------
DWORD WINAPI ShadowThreadProc(LPVOID lpParam) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = ShadowWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = g_szClassName;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (!RegisterClassEx(&wc)) return 1;

    HWND hShell = GetShellWindow(); 
    
    g_hShadow = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"TaskbarShadow",
        WS_POPUP, 0, 0, 10, 10,
        hShell, // Attempt to attach on create. If NULL, Self-Healing in UpdateShadow fixes it.
        NULL, wc.hInstance, NULL
    );

    if (g_hShadow) {
        SetTimer(g_hShadow, 1, 500, NULL);
        UpdateShadow(TRUE);
    }

    HWINEVENTHOOK hHookTaskbar = NULL;
    HWINEVENTHOOK hHookFocus = NULL;
    g_hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);

    if (g_hTaskbar) {
        DWORD pid = 0;
        DWORD threadId = GetWindowThreadProcessId(g_hTaskbar, &pid);
        if (pid != 0) {
            hHookTaskbar = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, NULL, ThreadWinEventProc, pid, threadId, WINEVENT_OUTOFCONTEXT);
        }
    }
    
    hHookFocus = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, ThreadWinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }

    if (hHookTaskbar) UnhookWinEvent(hHookTaskbar);
    if (hHookFocus) UnhookWinEvent(hHookFocus);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

// ----------------------------------------------------------------------------
// Lifecycle
// ----------------------------------------------------------------------------
void LoadSettings() {
    g_shadowHeight.store(Wh_GetIntSetting(L"ShadowHeight"));
    g_shadowOpacity.store(Wh_GetIntSetting(L"ShadowOpacity"));
    g_shadowHeightDark.store(Wh_GetIntSetting(L"ShadowHeightDark"));
    g_shadowOpacityDark.store(Wh_GetIntSetting(L"ShadowOpacityDark"));
    g_shadowOffset.store(Wh_GetIntSetting(L"ShadowOffset"));
    g_shadowAlwaysOnTop.store(Wh_GetIntSetting(L"ShadowAlwaysOnTop"));
    g_borderWidth.store(Wh_GetIntSetting(L"BorderWidth"));
    
    PCWSTR lightColor = Wh_GetStringSetting(L"BorderColor");
    g_borderColor.store(HexToRGB(lightColor));
    Wh_FreeStringSetting(lightColor);

    PCWSTR darkColor = Wh_GetStringSetting(L"BorderColorDark");
    g_borderColorDark.store(HexToRGB(darkColor));
    Wh_FreeStringSetting(darkColor);
}

BOOL WhTool_ModInit() {
    LoadSettings();
    UpdateDarkModeState(); 
    StringCchPrintf(g_szClassName, 64, L"WindhawkShadow_%u_%u", GetCurrentProcessId(), GetTickCount());
    g_hThread = CreateThread(NULL, 0, ShadowThreadProc, NULL, 0, &g_dwThreadId);
    return (g_hThread != NULL);
}

void WhTool_ModUninit() {
    if (g_dwThreadId != 0) {
        if (g_hShadow && IsWindow(g_hShadow)) PostMessage(g_hShadow, WM_CLOSE, 0, 0);
        else PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
        
        if (g_hThread) { WaitForSingleObject(g_hThread, 5000); CloseHandle(g_hThread); }
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hShadow && IsWindow(g_hShadow)) PostMessage(g_hShadow, WM_UPDATE_SETTINGS, 0, 0);
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
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

    if (isService) {
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
