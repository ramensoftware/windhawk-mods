// ==WindhawkMod==
// @id              dimmer-overlay
// @name            Dimmer Overlay
// @description     Adjustable screen dimming overlay for Windows
// @version         1.2.7
// @author          ZXCITE
// @include         explorer.exe
// @compilerOptions -Wl,--export-all-symbols -luser32 -lgdi32
// @license         BSD-3-Clause
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dimmer Overlay with App Exclusions

A lightweight desktop dimming overlay for Windows. It does not attempt hardware
brightness control. It places a translucent black, click-through window over the
virtual desktop instead.

## Features

- Adjustable black-overlay opacity from 0% (no dimming) to 95% (near-black).
- Works across the full virtual desktop, including multiple monitors.
- Click-through and non-activating: normal mouse and keyboard interaction continues.
- Excludes applications by their exact executable path.
- Dimming returns automatically when an excluded application is minimized, closed,
  or no longer the foreground application.
- Ctrl+Alt+F11 toggles the overlay on and off as an emergency/recovery shortcut.
- No separate Dimmer application or tray app is required; the overlay runs in the first eligible Explorer process.

## Configure

Open the mod's **Settings** in Windhawk:

- **Enable dimming**: turn the overlay on or off.
- **Black overlay opacity**: the percentage of black applied. Higher values look darker.
- **Excluded executable paths**: add one full path to the application's `.exe` per item,
  for example `C:\Program Files\Mozilla Firefox\firefox.exe`.

Use the executable's actual path, not a Start-menu shortcut (`.lnk`). Paths are compared
case-insensitively after environment-variable expansion and basic path normalization.
When the active foreground process path matches an excluded entry, the overlay hides.
It returns when another application is active, or the excluded application is minimized
or closed.

## Important limitations

- This is a user-mode overlay, not hardware brightness control. It does not reduce the
  monitor backlight or its power usage.
- Exclusive-fullscreen games, protected video paths, UAC's secure desktop, and some
  hardware-accelerated presentation modes may bypass or appear above the overlay.
- Packaged/UWP applications can expose a host process (for example, `ApplicationFrameHost.exe`)
  as the foreground process. In those cases, excluding the packaged app's own executable
  path may not match; exclude the actual foreground host path if appropriate.
- The overlay runs inside the first eligible Explorer process. A named mutex prevents
  duplicate overlays if Windows has multiple Explorer processes.
- The emergency shortcut is fixed at **Ctrl+Alt+F11**. If another program has registered
  that hotkey first, Windhawk's mod log will note that the shortcut could not be registered.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Enabled: true
  $name: Enable dimming
  $description: Turns the overlay on or off. Ctrl+Alt+F11 also toggles it temporarily.
- DimAmount: 35
  $name: Black overlay opacity
  $description: 0 means no dimming; 95 is near-black. The maximum is capped below full black for safer recovery.
  #! $min: 0
  #! $max: 95
  #! $format: slider
- ExcludedApps: [""]
  $name: Excluded executable paths
  $description: Add one full executable path per list item. Replace the blank item with a path or add more items. Dimming is hidden only while that app is foreground.
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <cwchar>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

// Windhawk's loader requires the InternalWhModPtr data symbol and callback
// symbols to be visible in the compiled DLL. Explicit linker options in the
// metadata below ask the MinGW/LLD toolchain to export global symbols; the
// address reference in Wh_ModInit keeps the inline API pointer materialized.

namespace {

constexpr wchar_t kWindowClassName[] = L"WindhawkDimmerOverlayExclusions_3D872A1E";
constexpr UINT kPollIntervalMs = 125;
constexpr int kEmergencyHotkeyId = 0xD17;
constexpr UINT kEmergencyHotkeyModifiers = MOD_CONTROL | MOD_ALT | MOD_NOREPEAT;
constexpr UINT kEmergencyHotkeyVirtualKey = VK_F11;
constexpr int kMaxExcludedApps = 128;

struct Settings {
    bool enabled = true;
    int dimAmount = 35;
    std::vector<std::wstring> excludedApps;
};

std::mutex g_settingsMutex;
Settings g_settings;
HANDLE g_stopEvent = nullptr;
HANDLE g_workerThread = nullptr;
HANDLE g_instanceMutex = nullptr;
std::atomic<bool> g_manualEnabled{true};

std::wstring Trim(std::wstring value) {
    size_t first = 0;
    while (first < value.size() && iswspace(value[first])) {
        ++first;
    }

    size_t last = value.size();
    while (last > first && iswspace(value[last - 1])) {
        --last;
    }

    return value.substr(first, last - first);
}

std::wstring ExpandEnvironment(std::wstring value) {
    if (value.empty()) {
        return value;
    }

    DWORD needed = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);
    if (needed == 0 || needed > 32768) {
        return value;
    }

    std::vector<wchar_t> buffer(needed);
    DWORD written = ExpandEnvironmentStringsW(value.c_str(), buffer.data(), needed);
    if (written == 0 || written > needed) {
        return value;
    }

    return std::wstring(buffer.data());
}

std::wstring NormalizePath(std::wstring path) {
    path = Trim(std::move(path));
    if (path.size() >= 2 && path.front() == L'"' && path.back() == L'"') {
        path = path.substr(1, path.size() - 2);
    }
    path = Trim(std::move(path));
    if (path.empty()) {
        return path;
    }

    path = ExpandEnvironment(std::move(path));
    std::replace(path.begin(), path.end(), L'/', L'\\');

    DWORD needed = GetFullPathNameW(path.c_str(), 0, nullptr, nullptr);
    if (needed > 0 && needed < 32768) {
        std::vector<wchar_t> buffer(needed);
        DWORD written = GetFullPathNameW(path.c_str(), needed, buffer.data(), nullptr);
        if (written > 0 && written < needed) {
            path.assign(buffer.data(), written);
        }
    }

    std::replace(path.begin(), path.end(), L'/', L'\\');
    return path;
}

void LoadSettings() {
    Settings updated;
    updated.enabled = Wh_GetIntSetting(L"Enabled") != 0;
    updated.dimAmount = std::max(0, std::min(95, Wh_GetIntSetting(L"DimAmount")));

    for (int index = 0; index < kMaxExcludedApps; ++index) {
        PCWSTR rawPath = Wh_GetStringSetting(L"ExcludedApps[%d]", index);
        std::wstring path = rawPath ? rawPath : L"";
        Wh_FreeStringSetting(rawPath);

        path = NormalizePath(std::move(path));
        if (path.empty()) {
            break;
        }

        bool duplicate = false;
        for (const auto& existing : updated.excludedApps) {
            if (CompareStringOrdinal(existing.c_str(), -1, path.c_str(), -1, TRUE) == CSTR_EQUAL) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate) {
            updated.excludedApps.push_back(std::move(path));
        }
    }

    const int loadedEnabled = updated.enabled ? 1 : 0;
    const int loadedDimAmount = updated.dimAmount;
    const int loadedExclusionCount = static_cast<int>(updated.excludedApps.size());
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = std::move(updated);
    }

    OutputDebugStringW(L"[DimmerWH] Settings loaded\r\n");
    Wh_Log(L"Settings loaded: enabled=%d, dimAmount=%d, exclusions=%d; pid=%lu",
           loadedEnabled, loadedDimAmount, loadedExclusionCount, GetCurrentProcessId());
}

Settings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

bool IsExcludedForegroundProcess(const std::vector<std::wstring>& exclusions) {
    if (exclusions.empty()) {
        return false;
    }

    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(foregroundWindow, &processId);
    if (processId == 0) {
        return false;
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return false;
    }

    std::vector<wchar_t> pathBuffer(32768);
    DWORD pathLength = static_cast<DWORD>(pathBuffer.size());
    std::wstring processPath;
    if (QueryFullProcessImageNameW(process, 0, pathBuffer.data(), &pathLength)) {
        processPath.assign(pathBuffer.data(), pathLength);
    }
    CloseHandle(process);

    if (processPath.empty()) {
        return false;
    }

    processPath = NormalizePath(std::move(processPath));
    for (const auto& excludedPath : exclusions) {
        if (CompareStringOrdinal(processPath.c_str(), -1,
                                 excludedPath.c_str(), -1, TRUE) == CSTR_EQUAL) {
            return true;
        }
    }

    return false;
}

bool GetVirtualScreenRect(RECT* rect) {
    if (!rect) {
        return false;
    }

    rect->left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    rect->top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    rect->right = rect->left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    rect->bottom = rect->top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return rect->right > rect->left && rect->bottom > rect->top;
}

bool SameRect(const RECT& a, const RECT& b) {
    return a.left == b.left && a.top == b.top &&
           a.right == b.right && a.bottom == b.bottom;
}

LRESULT CALLBACK OverlayWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT paint{};
            HDC dc = BeginPaint(hwnd, &paint);
            FillRect(dc, &paint.rcPaint, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
            EndPaint(hwnd, &paint);
            return 0;
        }

        case WM_ERASEBKGND: {
            RECT clientRect{};
            GetClientRect(hwnd, &clientRect);
            FillRect(reinterpret_cast<HDC>(wParam), &clientRect,
                     reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
            return 1;
        }

        case WM_NCHITTEST:
            // The layered + transparent extended styles pass pointer input through.
            return HTTRANSPARENT;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_HOTKEY:
            if (wParam == kEmergencyHotkeyId) {
                bool enabled = !g_manualEnabled.load();
                g_manualEnabled.store(enabled);
                Wh_Log(L"Emergency toggle: overlay %ls", enabled ? L"enabled" : L"disabled");
                return 0;
            }
            break;

        case WM_DISPLAYCHANGE:
            // The worker refreshes the virtual-screen bounds on its next poll.
            return 0;

        default:
            break;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void UpdateOverlay(HWND hwnd, bool* visible, BYTE* appliedAlpha,
                   RECT* lastRect, bool* haveRect) {
    Settings settings = GetSettingsSnapshot();

    const bool manuallyEnabled = g_manualEnabled.load();
    const bool excluded = settings.enabled && settings.dimAmount > 0 &&
                          IsExcludedForegroundProcess(settings.excludedApps);
    const bool shouldBeVisible = settings.enabled && settings.dimAmount > 0 &&
                                 manuallyEnabled && !excluded;

    if (!shouldBeVisible) {
        if (*visible) {
            ShowWindow(hwnd, SW_HIDE);
            *visible = false;
        }
        return;
    }

    BYTE alpha = static_cast<BYTE>((settings.dimAmount * 255 + 50) / 100);
    if (alpha != *appliedAlpha) {
        if (!SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA)) {
            Wh_Log(L"SetLayeredWindowAttributes failed: %lu", GetLastError());
            if (*visible) {
                ShowWindow(hwnd, SW_HIDE);
                *visible = false;
            }
            return;
        }
        *appliedAlpha = alpha;
    }

    RECT screenRect{};
    if (!GetVirtualScreenRect(&screenRect)) {
        if (*visible) {
            ShowWindow(hwnd, SW_HIDE);
            *visible = false;
        }
        return;
    }

    int width = screenRect.right - screenRect.left;
    int height = screenRect.bottom - screenRect.top;
    BOOL positionResult = FALSE;

    if (!*visible || !*haveRect || !SameRect(screenRect, *lastRect)) {
        InvalidateRect(hwnd, nullptr, TRUE);
        positionResult = SetWindowPos(hwnd, HWND_TOPMOST,
                                      screenRect.left, screenRect.top, width, height,
                                      SWP_NOACTIVATE | SWP_SHOWWINDOW);
        if (positionResult) {
            *lastRect = screenRect;
            *haveRect = true;
            *visible = true;
        }
    } else {
        // Maintain topmost placement without forcing activation or changing bounds.
        positionResult = SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                                      SWP_NOMOVE | SWP_NOSIZE |
                                      SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }

    if (!positionResult) {
        Wh_Log(L"SetWindowPos failed: %lu", GetLastError());
        ShowWindow(hwnd, SW_HIDE);
        *visible = false;
        return;
    }

    if (*visible) {
        UpdateWindow(hwnd);
    }
}

DWORD WINAPI OverlayThreadProc(void*) {
    OutputDebugStringW(L"[DimmerWH] OverlayThreadProc entered\r\n");
    // This code runs inside the first eligible Explorer process. A named mutex in
    // Wh_ModInit enforces a single overlay even if multiple Explorer processes exist.
    if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
        return 0;
    }

    HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = OverlayWindowProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    windowClass.lpszClassName = kWindowClassName;

    ATOM registeredClass = RegisterClassExW(&windowClass);
    if (!registeredClass) {
        OutputDebugStringW(L"[DimmerWH] RegisterClassExW failed\r\n");
        Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
        return 0;
    }

    RECT screenRect{};
    if (!GetVirtualScreenRect(&screenRect)) {
        screenRect = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    }

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT |
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kWindowClassName, L"Dimmer Overlay", WS_POPUP,
        screenRect.left, screenRect.top,
        std::max(1L, screenRect.right - screenRect.left),
        std::max(1L, screenRect.bottom - screenRect.top),
        nullptr, nullptr, instance, nullptr);

    if (!hwnd) {
        OutputDebugStringW(L"[DimmerWH] CreateWindowExW failed\r\n");
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        if (registeredClass) {
            UnregisterClassW(kWindowClassName, instance);
        }
        return 0;
    }

    OutputDebugStringW(L"[DimmerWH] Overlay window created\r\n");

    bool hotkeyRegistered = RegisterHotKey(hwnd, kEmergencyHotkeyId,
                                           kEmergencyHotkeyModifiers,
                                           kEmergencyHotkeyVirtualKey) != FALSE;
    if (!hotkeyRegistered) {
        Wh_Log(L"Could not register Ctrl+Alt+F11; the hotkey may already be in use (error %lu).",
               GetLastError());
    }

    bool visible = false;
    BYTE appliedAlpha = 255;
    RECT lastRect{};
    bool haveRect = false;
    UpdateOverlay(hwnd, &visible, &appliedAlpha, &lastRect, &haveRect);
    OutputDebugStringW(visible ? L"[DimmerWH] Overlay initially visible\r\n" : L"[DimmerWH] Overlay initially hidden; inspect settings/foreground exclusion\r\n");
    Wh_Log(L"Dimmer overlay started. Emergency toggle: Ctrl+Alt+F11.");

    bool keepRunning = true;
    while (keepRunning) {
        DWORD waitResult = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE,
                                                     kPollIntervalMs, QS_ALLINPUT);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        if (waitResult == WAIT_OBJECT_0 + 1) {
            MSG message{};
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (message.message == WM_QUIT) {
                    keepRunning = false;
                    break;
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        } else if (waitResult == WAIT_FAILED) {
            Wh_Log(L"MsgWaitForMultipleObjects failed: %lu", GetLastError());
            break;
        }

        if (keepRunning) {
            UpdateOverlay(hwnd, &visible, &appliedAlpha, &lastRect, &haveRect);
        }
    }

    if (hotkeyRegistered) {
        UnregisterHotKey(hwnd, kEmergencyHotkeyId);
    }

    ShowWindow(hwnd, SW_HIDE);
    DestroyWindow(hwnd);
    if (registeredClass) {
        UnregisterClassW(kWindowClassName, instance);
    }

    Wh_Log(L"Dimmer overlay stopped.");
    return 0;
}

}  // namespace

__attribute__((dllexport)) BOOL Wh_ModInit() {
    // Keep the inline Windhawk API pointer materialized for the linker export.
#if !defined(WH_EDITING) && (defined(__clang__) || defined(__GNUC__))
    // The runtime build gets InternalWhModPtr from Windhawk's internal API
    // header. WH_EDITING intentionally hides that declaration from clangd.
    __asm__ __volatile__("" : : "r"(&InternalWhModPtr) : "memory");
#endif

    OutputDebugStringW(L"[DimmerWH] Wh_ModInit entered\r\n");
    Wh_Log(L"Wh_ModInit entered in Explorer process %lu", GetCurrentProcessId());

    DWORD sessionId = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) || sessionId == 0) {
        OutputDebugStringW(L"[DimmerWH] Not in an interactive session; skipping\r\n");
        return FALSE;
    }

    Wh_Log(L"Interactive session %lu confirmed", sessionId);

    // Windhawk can have several helper processes. Only one should own the overlay.
    SetLastError(ERROR_SUCCESS);
    g_instanceMutex = CreateMutexW(nullptr, FALSE,
        L"Local\\WindhawkDimmerOverlayExclusions_3D872A1E");
    if (!g_instanceMutex) {
        OutputDebugStringW(L"[DimmerWH] CreateMutexW failed\r\n");
        return FALSE;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        OutputDebugStringW(L"[DimmerWH] Another overlay instance already exists; skipping\r\n");
        CloseHandle(g_instanceMutex);
        g_instanceMutex = nullptr;
        return FALSE;
    }

    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        OutputDebugStringW(L"[DimmerWH] CreateEventW failed\r\n");
        Wh_Log(L"CreateEventW failed: %lu", GetLastError());
        CloseHandle(g_instanceMutex);
        g_instanceMutex = nullptr;
        return FALSE;
    }

    g_workerThread = CreateThread(nullptr, 0, OverlayThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        OutputDebugStringW(L"[DimmerWH] CreateThread failed\r\n");
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        CloseHandle(g_instanceMutex);
        g_instanceMutex = nullptr;
        return FALSE;
    }

    OutputDebugStringW(L"[DimmerWH] Overlay worker thread created\r\n");
    Wh_Log(L"Overlay worker thread created successfully");
    return TRUE;
}

__attribute__((dllexport)) void Wh_ModSettingsChanged() {
    LoadSettings();
    // The worker polls the settings snapshot; the next poll applies any changes.
}

__attribute__((dllexport)) void Wh_ModUninit() {
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_workerThread) {
        // The worker uses only a short timed wait and does not wait on this thread.
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }


    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_instanceMutex) {
        CloseHandle(g_instanceMutex);
        g_instanceMutex = nullptr;
    }
    OutputDebugStringW(L"[DimmerWH] Wh_ModUninit complete\r\n");
}

