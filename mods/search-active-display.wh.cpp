// ==WindhawkMod==
// @id              search-active-display
// @name            Search on Active Display
// @description     Opens Win+S search on the monitor where the mouse cursor is located, or in a custom monitor of choice
// @version         1.1.0
// @author          ereinaimer
// @github          https://github.com/ereinaimer
// @include         SearchHost.exe
// @architecture    x86-64
// @compilerOptions -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Search on Active Display

Opens the Windows Search (Win+S) on the monitor where the mouse cursor is
located, or on a specific monitor of choice.

Works for both Win+S shortcut and taskbar search icon click.

## How it works

The mod hooks MonitorFromWindow and MonitorFromRect inside SearchHost.exe.
When SearchHost queries which monitor its CoreWindow is on, the mod returns
the target monitor (where the cursor is, or a fixed monitor). It also
physically repositions the CoreWindow to the correct monitor coordinates
so the DWM compositor renders it there.

## Selecting a monitor

Set the **Monitor** setting to 0 to follow the mouse cursor, or to a
specific monitor number (1, 2, 3...). You can also use a monitor interface
name for stable identification across reboots.

## Supported Windows Builds

- Windows 11 22H2 and 24H2.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- monitor: 0
  $name: Monitor
  $description: >-
    The monitor number that the search will appear on. Set to zero to use
    the monitor where the mouse cursor is located.
- monitorInterfaceName: ""
  $name: Monitor interface name
  $description: >-
    If not empty, the given monitor interface name (can also be an interface
    name substring) will be used instead of the monitor number. Can be useful if
    the monitor numbers change often. To see all available interface names, set
    any interface name, enable mod logs, open the search and look for "Found
    display device" messages.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <shellscalingapi.h>

struct {
    int monitor;
    WindhawkUtils::StringSetting monitorInterfaceName;
} g_settings;

struct MonitorSearchContext {
    int targetId;
    int currentId;
    PCWSTR targetInterface;
    HMONITOR result;
};

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor, LPARAM dwData) {
    auto* ctx = reinterpret_cast<MonitorSearchContext*>(dwData);
    
    if (!ctx->targetInterface) {
        if (ctx->currentId == ctx->targetId) {
            ctx->result = hMonitor;
            return FALSE;
        }
        ctx->currentId++;
        return TRUE;
    }
    
    MONITORINFOEXW mi;
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMonitor, &mi)) {
        DISPLAY_DEVICEW dd;
        dd.cb = sizeof(dd);
        if (EnumDisplayDevicesW(mi.szDevice, 0, &dd, EDD_GET_DEVICE_INTERFACE_NAME)) {
            Wh_Log(L"Found display device %s, interface name: %s", mi.szDevice, dd.DeviceID);
            if (wcsstr(dd.DeviceID, ctx->targetInterface)) {
                ctx->result = hMonitor;
                return FALSE;
            }
        }
    }
    return TRUE;
}

HMONITOR GetTargetMonitor() {
    if (g_settings.monitorInterfaceName.get()[0] != L'\0') {
        MonitorSearchContext ctx = {0, 0, g_settings.monitorInterfaceName.get(), nullptr};
        EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&ctx));
        return ctx.result;
    }
    
    if (g_settings.monitor == 0) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            return MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        }
    } else if (g_settings.monitor >= 1) {
        MonitorSearchContext ctx = {g_settings.monitor - 1, 0, nullptr, nullptr};
        EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&ctx));
        return ctx.result;
    }
    
    return nullptr;
}

// Win32 API hooks in SearchHost.exe

using MonitorFromWindow_t = decltype(&MonitorFromWindow);
MonitorFromWindow_t MonitorFromWindow_Original;

using MonitorFromRect_t = decltype(&MonitorFromRect);
MonitorFromRect_t MonitorFromRect_Original;

// CoreWindow detection and repositioning

bool IsCoreWindow(HWND hwnd) {
    WCHAR className[256] = {0};
    GetClassName(hwnd, className, ARRAYSIZE(className));
    return _wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0;
}

// Translate a window rect from its current monitor to the target monitor,
// preserving relative position within the monitor's work area and scaling by DPI.
void MoveWindowToMonitor(HWND hwnd, HMONITOR targetMonitor) {
    RECT windowRect;
    if (!GetWindowRect(hwnd, &windowRect)) {
        return;
    }

    HMONITOR currentMonitor = MonitorFromRect_Original 
        ? MonitorFromRect_Original(&windowRect, MONITOR_DEFAULTTONEAREST) 
        : MonitorFromRect(&windowRect, MONITOR_DEFAULTTONEAREST);

    if (currentMonitor == targetMonitor) {
        return;  // Already on the right monitor
    }

    MONITORINFO currentMi{.cbSize = sizeof(MONITORINFO)};
    MONITORINFO targetMi{.cbSize = sizeof(MONITORINFO)};
    GetMonitorInfo(currentMonitor, &currentMi);
    GetMonitorInfo(targetMonitor, &targetMi);

    // Get DPI for both monitors to handle scaling
    UINT dpiCurrentX = 96, dpiCurrentY = 96;
    UINT dpiTargetX = 96, dpiTargetY = 96;
    
    if (FAILED(GetDpiForMonitor(currentMonitor, MDT_EFFECTIVE_DPI, &dpiCurrentX, &dpiCurrentY))) {
        dpiCurrentX = dpiCurrentY = 96;
    }
    if (FAILED(GetDpiForMonitor(targetMonitor, MDT_EFFECTIVE_DPI, &dpiTargetX, &dpiTargetY))) {
        dpiTargetX = dpiTargetY = 96;
    }

    // Current window dimensions
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    // Scale dimensions based on DPI ratio
    int targetWidth = MulDiv(windowWidth, dpiTargetX, dpiCurrentX);
    int targetHeight = MulDiv(windowHeight, dpiTargetY, dpiCurrentY);

    // Compute offset from the bottom of the current monitor's work area
    int offsetBottom = currentMi.rcWork.bottom - windowRect.bottom;
    int targetOffsetBottom = MulDiv(offsetBottom, dpiTargetY, dpiCurrentY);

    // Compute horizontal offset from the center of the current monitor's work area
    int currentCenterX = (currentMi.rcWork.left + currentMi.rcWork.right) / 2;
    int offsetCenter = ((windowRect.left + windowRect.right) / 2) - currentCenterX;
    int targetOffsetCenter = MulDiv(offsetCenter, dpiTargetX, dpiCurrentX);

    // Apply the scaled offsets to the target monitor's work area
    int targetCenterXScreen = (targetMi.rcWork.left + targetMi.rcWork.right) / 2;
    int newX = targetCenterXScreen + targetOffsetCenter - (targetWidth / 2);
    int newY = targetMi.rcWork.bottom - targetOffsetBottom - targetHeight;

    SetWindowPos(hwnd, nullptr, newX, newY, targetWidth, targetHeight,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

HMONITOR WINAPI MonitorFromWindow_Hook(HWND hwnd, DWORD dwFlags) {
    HMONITOR original = MonitorFromWindow_Original(hwnd, dwFlags);
    if (!original) {
        return original;
    }

    if (IsCoreWindow(hwnd)) {
        HMONITOR target = GetTargetMonitor();
        if (target && target != original) {
            // We hook the query API instead of SetWindowPos because UWP's XAML layout engine
            // rigidly enforces its own layout coordinates and aggressively overrides SetWindowPos hooks,
            // causing the window to snap back to the primary monitor. Spoofing the monitor query 
            // prevents the layout engine from fighting the manual MoveWindowToMonitor translation.
            //
            // A re-entrancy guard cannot be used here. During SetWindowPos, the layout engine
            // synchronously queries MonitorFromWindow to validate the new bounds. If the query
            // spoof is guarded, the engine will see the primary monitor, reject the change,
            // and snap the window back.
            MoveWindowToMonitor(hwnd, target);
            return target;
        }
    }

    return original;
}

HMONITOR WINAPI MonitorFromRect_Hook(LPCRECT lprc, DWORD dwFlags) {
    HMONITOR original = MonitorFromRect_Original(lprc, dwFlags);
    if (!original) {
        return original;
    }

    if (!lprc) {
        return original;
    }

    // Check if this rect belongs to a search window by checking its size.
    // The search window is typically a large centered flyout (800x750ish).
    // We override for any rect that the search host queries.
    int width = lprc->right - lprc->left;
    int height = lprc->bottom - lprc->top;
    
    // Only query target monitor and override for rects that are clearly a window rect (not tiny UI elements)
    if (width > 200 && height > 200) {
        HMONITOR target = GetTargetMonitor();
        if (target && target != original) {
            return target;
        }
    }

    return original;
}

// Windhawk callbacks

void LoadSettings() {
    g_settings.monitor = Wh_GetIntSetting(L"monitor");
    g_settings.monitorInterfaceName =
        WindhawkUtils::StringSetting::make(L"monitorInterfaceName");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Search on Active Display initialized");
    LoadSettings();

    WCHAR processFileName[MAX_PATH];
    GetModuleFileName(NULL, processFileName, ARRAYSIZE(processFileName));
    PCWSTR processName = wcsrchr(processFileName, L'\\');
    if (processName) {
        processName++;
    } else {
        processName = processFileName;
    }

    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    if (!user32Module) {
        Wh_Log(L"Couldn't get user32.dll");
        return FALSE;
    }


    if (_wcsicmp(processName, L"SearchHost.exe") == 0) {
        WindhawkUtils::SetFunctionHook(
            (void*)GetProcAddress(user32Module, "MonitorFromWindow"),
            (void*)MonitorFromWindow_Hook,
            (void**)&MonitorFromWindow_Original);

        WindhawkUtils::SetFunctionHook(
            (void*)GetProcAddress(user32Module, "MonitorFromRect"),
            (void*)MonitorFromRect_Hook,
            (void**)&MonitorFromRect_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}