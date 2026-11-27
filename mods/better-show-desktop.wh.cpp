// ==WindhawkMod==
// @id              better-show-desktop
// @name            BetterShowDesktop (Smart minimize & restore)
// @description     A smarter Show Desktop that remembers exactly which windows it minimized, so you can open new windows and still restore the original ones later
// @version         1.0.0
// @author          AngryPavel
// @github          https://github.com/AngryPavel
// @license         GPL-3.0-only
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -luuid -luser32 -ldwmapi -ladvapi32 -lruntimeobject -lversion
// ==/WindhawkMod==
//
// This source code is licensed under the GNU General Public License v3.0.
//
// The Windows 11 Show Desktop XAML element detection and SystemTray module
// loading/hooking approach are based on:
// "Aero Peek on 'Show desktop' button hover" by m417z.
// https://windhawk.net/mods/taskbar-show-desktop-button-aero-peek

// ==WindhawkModReadme==
/*
# BetterShowDesktop

BetterShowDesktop replaces the fragile Windows Show Desktop toggle with a
stateful minimize/restore action.

The key difference is that the mod remembers **exactly which windows it
minimized**.

With the built-in Windows Show Desktop behavior, opening another window after
showing the desktop effectively breaks the previous toggle state. Pressing
Show Desktop again may simply minimize the newly opened windows instead of
restoring the windows you originally hid.

BetterShowDesktop keeps its own restore state.

You can:

1. Minimize your current windows.
2. Open one or several new windows.
3. Click again.
4. Restore only the windows that were minimized originally, while leaving the
   newly opened windows alone.

The custom action only affects the current virtual desktop, so windows on
other virtual desktops remain untouched.

## Features

* Remembers exactly which windows were minimized by the mod.
* Opening new windows does not destroy the restore state.
* Restores only the original minimized windows and leaves newly opened windows
  untouched.
* Works independently on each virtual desktop and never minimizes windows on
  other virtual desktops.
* Windows that were already manually minimized are never claimed by the mod.
* Maximized windows return maximized.
* Custom action can be assigned to left or right click.
* The other mouse button can optionally keep the original Windows Show Desktop
  action.

## Default controls

* Left click: per-virtual-desktop minimize/restore.
* Right click: original Windows Show Desktop.

These controls can be changed in the mod settings.

## Restore state

Restore state is kept in memory inside Explorer. Restarting Explorer, disabling
the mod, or reloading it clears the state.

## Compatibility

Designed for Windows 11 with the modern taskbar.

The mod uses the public `IVirtualDesktopManager` COM interface to determine
which virtual desktop a window belongs to.

Explorer's `CurrentVirtualDesktop` registry value is used to identify the
currently active virtual desktop because the public interface doesn't expose
a direct GetCurrentDesktopId method.

## Credits

The Windows 11 taskbar element detection and SystemTray module hook pattern are
based on **Aero Peek on "Show desktop" button hover** by m417z.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- customActionButton: left
  $name: Per-desktop action button
  $description: >-
    Mouse button used for the per-virtual-desktop minimize/restore action.
  $options:
    - left: Left click
    - right: Right click

- enableDefaultActionOnOtherButton: true
  $name: Enable Windows Show Desktop on other button
  $description: >-
    Enable the original Windows Show Desktop action on the other mouse button.
    If disabled, the other mouse button does nothing on the Show Desktop area.

- stateTimeoutSeconds: 60
  $name: Restore state timeout (seconds)
  $description: >-
    How long minimized windows remain eligible for restore.
    Set to 0 to keep restore state until it is used or explicitly cleared.

- desktopSwitchRetryDelayMs: 50
  $name: Desktop switch verification delay (ms)
  $description: >-
    Advanced. Delay before rechecking the active virtual desktop immediately
    after a desktop switch.

- minimizeVerificationDelayMs: 50
  $name: Minimize verification delay (ms)
  $description: >-
    Advanced. Delay before checking which windows actually became minimized.
    Only successfully minimized windows are stored in restore state.

- restoreSettleDelayMs: 30
  $name: Restore settle delay (ms)
  $description: >-
    Advanced. Delay after restoring windows before repairing their Z-order.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <dwmapi.h>
#include <shobjidl.h>
#include <shldisp.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <cwctype>
#include <functional>
#include <iterator>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#undef GetCurrentTime
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

constexpr DWORD kDwmwaCloaked = 14;

constexpr int kSwShowMaximized = 3;
constexpr int kSwShowNoActivate = 4;
constexpr int kSwShowMinNoActive = 7;

constexpr UINT kGaRootOwner = 3;

constexpr UINT kSwpNoSize = 0x0001;
constexpr UINT kSwpNoMove = 0x0002;
constexpr UINT kSwpNoActivate = 0x0010;
constexpr UINT kSwpNoOwnerZOrder = 0x0200;

const wchar_t* const kExcludedClasses[] = {
    L"Progman",
    L"WorkerW",
    L"Shell_TrayWnd",
    L"Shell_SecondaryTrayWnd",
};

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

struct ModSettings {
    std::atomic<bool> customActionOnRight{false};
    std::atomic<bool> enableDefaultActionOnOtherButton{true};

    std::atomic<int> stateTimeoutSeconds{60};
    std::atomic<int> desktopSwitchRetryDelayMs{50};
    std::atomic<int> minimizeVerificationDelayMs{50};
    std::atomic<int> restoreSettleDelayMs{30};
} g_settings;

int ClampSetting(
    int value,
    int minValue,
    int maxValue) {

    if (value < minValue) {
        return minValue;
    }

    if (value > maxValue) {
        return maxValue;
    }

    return value;
}

void LoadSettings() {
    PCWSTR customActionButton =
        Wh_GetStringSetting(
            L"customActionButton");

    bool customActionOnRight = false;

    if (customActionButton) {
        customActionOnRight =
            _wcsicmp(
                customActionButton,
                L"right") == 0;

        Wh_FreeStringSetting(
            customActionButton);
    }

    g_settings.customActionOnRight.store(
        customActionOnRight);

    g_settings
        .enableDefaultActionOnOtherButton
        .store(
            Wh_GetIntSetting(
                L"enableDefaultActionOnOtherButton") != 0);

    g_settings.stateTimeoutSeconds.store(
        ClampSetting(
            Wh_GetIntSetting(
                L"stateTimeoutSeconds"),
            0,
            86400));

    g_settings
        .desktopSwitchRetryDelayMs
        .store(
            ClampSetting(
                Wh_GetIntSetting(
                    L"desktopSwitchRetryDelayMs"),
                0,
                1000));

    g_settings
        .minimizeVerificationDelayMs
        .store(
            ClampSetting(
                Wh_GetIntSetting(
                    L"minimizeVerificationDelayMs"),
                0,
                1000));

    g_settings
        .restoreSettleDelayMs
        .store(
            ClampSetting(
                Wh_GetIntSetting(
                    L"restoreSettleDelayMs"),
                0,
                1000));

    Wh_Log(
        L"Settings: custom=%s, nativeOther=%d, timeout=%d, minimizeVerify=%d",
        customActionOnRight ? L"right" : L"left",
        g_settings.enableDefaultActionOnOtherButton.load() ? 1 : 0,
        g_settings.stateTimeoutSeconds.load(),
        g_settings.minimizeVerificationDelayMs.load());
}

// -----------------------------------------------------------------------------
// GUID helpers
// -----------------------------------------------------------------------------

bool GuidEqual(
    const GUID& a,
    const GUID& b) {

    return std::memcmp(
               &a,
               &b,
               sizeof(GUID)) == 0;
}

bool GuidIsNull(
    const GUID& guid) {

    static const GUID nullGuid{};

    return GuidEqual(
        guid,
        nullGuid);
}

struct GuidLess {
    bool operator()(
        const GUID& a,
        const GUID& b) const {

        return std::memcmp(
                   &a,
                   &b,
                   sizeof(GUID)) < 0;
    }
};

std::wstring GuidToString(
    const GUID& guid) {

    wchar_t buffer[64]{};

    if (!StringFromGUID2(
            guid,
            buffer,
            ARRAYSIZE(buffer))) {

        return L"<invalid-guid>";
    }

    return buffer;
}

// -----------------------------------------------------------------------------
// COM initialization
// -----------------------------------------------------------------------------

class ScopedComApartment {
public:
    ScopedComApartment() {
        hr_ = CoInitializeEx(
            nullptr,
            COINIT_APARTMENTTHREADED);

        shouldUninitialize_ =
            SUCCEEDED(hr_);
    }

    ~ScopedComApartment() {
        if (shouldUninitialize_) {
            CoUninitialize();
        }
    }

    bool IsUsable() const {
        return SUCCEEDED(hr_) ||
               hr_ == RPC_E_CHANGED_MODE;
    }

    HRESULT Result() const {
        return hr_;
    }

private:
    HRESULT hr_ = E_FAIL;
    bool shouldUninitialize_ = false;
};

// -----------------------------------------------------------------------------
// Process helpers
// -----------------------------------------------------------------------------

DWORD GetWindowProcessId(
    HWND hwnd) {

    DWORD pid = 0;

    GetWindowThreadProcessId(
        hwnd,
        &pid);

    return pid;
}

std::wstring GetProcessImagePath(
    DWORD pid) {

    if (!pid) {
        return {};
    }

    HANDLE process =
        OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION,
            FALSE,
            pid);

    if (!process) {
        return {};
    }

    wchar_t buffer[32768]{};
    DWORD size = ARRAYSIZE(buffer);

    std::wstring result;

    if (QueryFullProcessImageNameW(
            process,
            0,
            buffer,
            &size)) {

        result.assign(
            buffer,
            size);
    }

    CloseHandle(process);

    return result;
}

std::wstring ToLower(
    std::wstring value) {

    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](wchar_t c) {
            return static_cast<wchar_t>(
                std::towlower(c));
        });

    return value;
}

std::wstring GetPathFileName(
    const std::wstring& path) {

    size_t pos =
        path.find_last_of(
            L"\\/");

    if (pos ==
        std::wstring::npos) {

        return path;
    }

    return path.substr(
        pos + 1);
}

bool IsWindhawkUiWindow(
    HWND hwnd) {

    DWORD pid =
        GetWindowProcessId(
            hwnd);

    std::wstring path =
        GetProcessImagePath(
            pid);

    if (path.empty()) {
        return false;
    }

    std::wstring lowerPath =
        ToLower(path);

    std::wstring fileName =
        ToLower(
            GetPathFileName(path));

    // Current/lightweight Windhawk UI.
    if (fileName ==
            L"windhawk.exe" ||
        fileName ==
            L"windhawk-ui.exe") {

        return true;
    }

    // Windhawk 1.x UI.
    if (fileName ==
            L"vscodium.exe" &&
        lowerPath.find(
            L"\\windhawk\\") !=
            std::wstring::npos) {

        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------
// Window helpers
// -----------------------------------------------------------------------------

std::wstring GetWindowClassName(
    HWND hwnd) {

    wchar_t buffer[256]{};

    int length =
        GetClassNameW(
            hwnd,
            buffer,
            ARRAYSIZE(buffer));

    if (length <= 0) {
        return {};
    }

    return std::wstring(
        buffer,
        length);
}

bool IsExcludedWindowClass(
    const std::wstring& className) {

    for (const wchar_t* excluded :
         kExcludedClasses) {

        if (_wcsicmp(
                className.c_str(),
                excluded) == 0) {

            return true;
        }
    }

    return false;
}

bool IsCloaked(
    HWND hwnd) {

    DWORD cloaked = 0;

    HRESULT hr =
        DwmGetWindowAttribute(
            hwnd,
            kDwmwaCloaked,
            &cloaked,
            sizeof(cloaked));

    return SUCCEEDED(hr) &&
           cloaked != 0;
}

bool IsAltTabWindow(
    HWND hwnd) {

    HWND root =
        GetAncestor(
            hwnd,
            kGaRootOwner);

    if (!root) {
        root = hwnd;
    }

    HWND walk = root;

    while (true) {
        HWND popup =
            GetLastActivePopup(
                walk);

        if (!popup ||
            popup == walk) {

            break;
        }

        if (IsWindowVisible(
                popup)) {

            walk = popup;
            break;
        }

        walk = popup;
    }

    return walk == hwnd;
}

std::optional<GUID>
GetWindowDesktopId(
    IVirtualDesktopManager* desktopManager,
    HWND hwnd) {

    GUID guid{};

    HRESULT hr =
        desktopManager
            ->GetWindowDesktopId(
                hwnd,
                &guid);

    if (FAILED(hr) ||
        GuidIsNull(guid)) {

        return std::nullopt;
    }

    return guid;
}

bool IsWindowOnCurrentDesktop(
    IVirtualDesktopManager* desktopManager,
    HWND hwnd) {

    BOOL isCurrent = FALSE;

    HRESULT hr =
        desktopManager
            ->IsWindowOnCurrentVirtualDesktop(
                hwnd,
                &isCurrent);

    return SUCCEEDED(hr) &&
           isCurrent;
}

bool IsManageableWindow(
    HWND hwnd,
    IVirtualDesktopManager* desktopManager) {

    if (!IsWindow(hwnd)) {
        return false;
    }

    if (!IsWindowVisible(hwnd)) {
        return false;
    }

    const std::wstring className =
        GetWindowClassName(hwnd);

    if (className.empty() ||
        IsExcludedWindowClass(
            className)) {

        return false;
    }

    if (IsCloaked(hwnd)) {
        return false;
    }

    LONG_PTR exStyle =
        GetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE);

    // Windhawk UI has historically used unusual window/process structure.
    // Treat its visible main window as a normal app window rather than letting
    // the generic Alt-Tab/tool-window heuristic accidentally discard it.
    bool windhawkUi =
        IsWindhawkUiWindow(
            hwnd);

    if (!windhawkUi) {
        if ((exStyle &
             WS_EX_TOOLWINDOW) &&
            !(exStyle &
              WS_EX_APPWINDOW)) {

            return false;
        }

        if (!IsAltTabWindow(
                hwnd) &&
            !(exStyle &
              WS_EX_APPWINDOW)) {

            return false;
        }
    }

    if (!IsWindowOnCurrentDesktop(
            desktopManager,
            hwnd)) {

        return false;
    }

    return true;
}

struct EnumWindowsContext {
    IVirtualDesktopManager* desktopManager;
    std::vector<HWND>* result;
};

BOOL CALLBACK
EnumManageableWindowsProc(
    HWND hwnd,
    LPARAM lParam) {

    auto* context =
        reinterpret_cast<
            EnumWindowsContext*>(
                lParam);

    if (IsManageableWindow(
            hwnd,
            context->desktopManager)) {

        context->result
            ->push_back(hwnd);
    }

    return TRUE;
}

std::vector<HWND>
GetCurrentManageableWindows(
    IVirtualDesktopManager* desktopManager) {

    std::vector<HWND> result;

    EnumWindowsContext context{
        .desktopManager =
            desktopManager,
        .result =
            &result,
    };

    EnumWindows(
        EnumManageableWindowsProc,
        reinterpret_cast<LPARAM>(
            &context));

    return result;
}

// -----------------------------------------------------------------------------
// Current virtual desktop ID
// -----------------------------------------------------------------------------

std::optional<GUID>
ReadGuidFromRegistryKey(
    const std::wstring& path) {

    HKEY key = nullptr;

    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            path.c_str(),
            0,
            KEY_QUERY_VALUE,
            &key) !=
        ERROR_SUCCESS) {

        return std::nullopt;
    }

    GUID guid{};
    DWORD type = 0;
    DWORD size = sizeof(guid);

    LONG result =
        RegQueryValueExW(
            key,
            L"CurrentVirtualDesktop",
            nullptr,
            &type,
            reinterpret_cast<BYTE*>(
                &guid),
            &size);

    RegCloseKey(key);

    if (result != ERROR_SUCCESS ||
        type != REG_BINARY ||
        size < sizeof(guid) ||
        GuidIsNull(guid)) {

        return std::nullopt;
    }

    return guid;
}

std::optional<GUID>
ReadCurrentDesktopFromRegistry() {

    if (auto result =
            ReadGuidFromRegistryKey(
                L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops")) {

        return result;
    }

    DWORD sessionId = 0;

    if (!ProcessIdToSessionId(
            GetCurrentProcessId(),
            &sessionId)) {

        return std::nullopt;
    }

    wchar_t path[256]{};

    swprintf_s(
        path,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops",
        sessionId);

    return ReadGuidFromRegistryKey(
        path);
}

std::optional<GUID>
GetCurrentDesktopId(
    IVirtualDesktopManager* desktopManager,
    const std::vector<HWND>& currentWindows) {

    auto registryId =
        ReadCurrentDesktopFromRegistry();

    if (registryId) {
        bool foundAnyWindowDesktopId =
            false;

        bool registryMatchesAWindow =
            false;

        for (HWND hwnd :
             currentWindows) {

            auto windowId =
                GetWindowDesktopId(
                    desktopManager,
                    hwnd);

            if (!windowId) {
                continue;
            }

            foundAnyWindowDesktopId =
                true;

            if (GuidEqual(
                    *windowId,
                    *registryId)) {

                registryMatchesAWindow =
                    true;

                break;
            }
        }

        if (foundAnyWindowDesktopId &&
            !registryMatchesAWindow) {

            int delayMs =
                g_settings
                    .desktopSwitchRetryDelayMs
                    .load();

            if (delayMs > 0) {
                Sleep(delayMs);
            }

            if (auto second =
                    ReadCurrentDesktopFromRegistry()) {

                registryId =
                    second;
            }
        }

        return registryId;
    }

    std::map<
        GUID,
        int,
        GuidLess>
        counts;

    for (HWND hwnd :
         currentWindows) {

        if (auto desktopId =
                GetWindowDesktopId(
                    desktopManager,
                    hwnd)) {

            counts[
                *desktopId]++;
        }
    }

    if (!counts.empty()) {
        auto best =
            counts.begin();

        for (auto it =
                 std::next(
                     counts.begin());
             it != counts.end();
             ++it) {

            if (it->second >
                best->second) {

                best = it;
            }
        }

        return best->first;
    }

    HWND foreground =
        GetForegroundWindow();

    if (foreground &&
        IsWindowOnCurrentDesktop(
            desktopManager,
            foreground)) {

        return GetWindowDesktopId(
            desktopManager,
            foreground);
    }

    return std::nullopt;
}

// -----------------------------------------------------------------------------
// Toggle state
// -----------------------------------------------------------------------------

struct WindowState {
    HWND hwnd = nullptr;
    DWORD pid = 0;
    std::wstring className;
    bool wasMaximized = false;
};

struct DesktopState {
    std::chrono::steady_clock::time_point
        createdAt;

    std::vector<WindowState>
        windows;
};

std::map<
    GUID,
    DesktopState,
    GuidLess>
    g_desktopStates;

std::mutex g_toggleMutex;

bool SameWindow(
    const WindowState& item) {

    if (!IsWindow(
            item.hwnd)) {

        return false;
    }

    if (GetWindowProcessId(
            item.hwnd) !=
        item.pid) {

        return false;
    }

    return
        GetWindowClassName(
            item.hwnd) ==
        item.className;
}

bool IsRestoreCandidate(
    const WindowState& item,
    IVirtualDesktopManager* desktopManager,
    const GUID& currentDesktopId) {

    if (!SameWindow(item)) {
        return false;
    }

    if (!IsIconic(
            item.hwnd)) {

        return false;
    }

    if (!IsWindowOnCurrentDesktop(
            desktopManager,
            item.hwnd)) {

        return false;
    }

    auto desktopId =
        GetWindowDesktopId(
            desktopManager,
            item.hwnd);

    if (desktopId &&
        !GuidEqual(
            *desktopId,
            currentDesktopId)) {

        return false;
    }

    return true;
}

bool IsStateExpired(
    const DesktopState& state) {

    int timeoutSeconds =
        g_settings
            .stateTimeoutSeconds
            .load();

    if (timeoutSeconds <= 0) {
        return false;
    }

    auto age =
        std::chrono::steady_clock::now() -
        state.createdAt;

    return age >
           std::chrono::seconds(
               timeoutSeconds);
}

void PruneExpiredStates() {
    if (g_settings
            .stateTimeoutSeconds
            .load() <= 0) {

        return;
    }

    for (auto it =
             g_desktopStates.begin();
         it !=
             g_desktopStates.end();) {

        if (IsStateExpired(
                it->second)) {

            it =
                g_desktopStates.erase(
                    it);
        } else {
            ++it;
        }
    }
}

void ClearAllCustomState() {
    std::lock_guard<std::mutex>
        lock(g_toggleMutex);

    size_t stateCount =
        g_desktopStates.size();

    g_desktopStates.clear();

    Wh_Log(
        L"Cleared custom restore state for %zu desktop(s)",
        stateCount);
}

// -----------------------------------------------------------------------------
// Minimize
// -----------------------------------------------------------------------------

void MinimizeCurrentWindows(
    IVirtualDesktopManager* desktopManager,
    const GUID& currentDesktopId) {

    const auto windows =
        GetCurrentManageableWindows(
            desktopManager);

    std::vector<WindowState>
        targets;

    for (HWND hwnd :
         windows) {

        // Never claim windows which were already manually minimized.
        if (IsIconic(hwnd)) {
            continue;
        }

        auto desktopId =
            GetWindowDesktopId(
                desktopManager,
                hwnd);

        if (desktopId &&
            !GuidEqual(
                *desktopId,
                currentDesktopId)) {

            continue;
        }

        WindowState item;

        item.hwnd = hwnd;
        item.pid =
            GetWindowProcessId(hwnd);
        item.className =
            GetWindowClassName(hwnd);
        item.wasMaximized =
            IsZoomed(hwnd) != FALSE;

        targets.push_back(
            std::move(item));
    }

    // Important:
    // no targets -> absolutely no restore state.
    if (targets.empty()) {
        g_desktopStates.erase(
            currentDesktopId);

        Wh_Log(
            L"No windows to minimize; no state created");

        return;
    }

    Wh_Log(
        L"Requesting minimize for %zu window(s) on desktop %s",
        targets.size(),
        GuidToString(
            currentDesktopId)
            .c_str());

    for (const WindowState& item :
         targets) {

        if (IsWindow(
                item.hwnd)) {

            ShowWindowAsync(
                item.hwnd,
                kSwShowMinNoActive);
        }
    }

    int verifyDelay =
        g_settings
            .minimizeVerificationDelayMs
            .load();

    if (verifyDelay > 0) {
        Sleep(verifyDelay);
    }

    // Only windows which ACTUALLY became minimized are allowed into state.
    std::vector<WindowState>
        actuallyMinimized;

    for (const WindowState& item :
         targets) {

        if (!SameWindow(item)) {
            continue;
        }

        if (!IsIconic(
                item.hwnd)) {

            continue;
        }

        if (!IsWindowOnCurrentDesktop(
                desktopManager,
                item.hwnd)) {

            continue;
        }

        auto desktopId =
            GetWindowDesktopId(
                desktopManager,
                item.hwnd);

        if (desktopId &&
            !GuidEqual(
                *desktopId,
                currentDesktopId)) {

            continue;
        }

        actuallyMinimized.push_back(
            item);
    }

    if (actuallyMinimized.empty()) {
        g_desktopStates.erase(
            currentDesktopId);

        Wh_Log(
            L"No window was actually minimized; no state created");

        return;
    }

    DesktopState state;

    state.createdAt =
        std::chrono::steady_clock::now();

    state.windows =
        std::move(
            actuallyMinimized);

    size_t storedCount =
        state.windows.size();

    g_desktopStates[
        currentDesktopId] =
        std::move(state);

    Wh_Log(
        L"Stored restore state for %zu actually minimized window(s)",
        storedCount);
}

// -----------------------------------------------------------------------------
// Restore
// -----------------------------------------------------------------------------

bool RestoreWindows(
    const DesktopState& desktopState,
    IVirtualDesktopManager* desktopManager,
    const GUID& currentDesktopId) {

    std::vector<
        const WindowState*>
        candidates;

    for (const WindowState& item :
         desktopState.windows) {

        if (IsRestoreCandidate(
                item,
                desktopManager,
                currentDesktopId)) {

            candidates.push_back(
                &item);
        }
    }

    if (candidates.empty()) {
        return false;
    }

    std::vector<HWND>
        candidateHwnds;

    candidateHwnds.reserve(
        candidates.size());

    for (const WindowState* item :
         candidates) {

        candidateHwnds.push_back(
            item->hwnd);
    }

    // Windows which stayed visible must remain above restored windows.
    std::vector<HWND>
        protectedWindows;

    for (HWND hwnd :
         GetCurrentManageableWindows(
             desktopManager)) {

        bool candidate =
            false;

        for (HWND candidateHwnd :
             candidateHwnds) {

            if (candidateHwnd ==
                hwnd) {

                candidate = true;
                break;
            }
        }

        if (candidate ||
            IsIconic(hwnd)) {

            continue;
        }

        auto desktopId =
            GetWindowDesktopId(
                desktopManager,
                hwnd);

        if (desktopId &&
            !GuidEqual(
                *desktopId,
                currentDesktopId)) {

            continue;
        }

        protectedWindows.push_back(
            hwnd);
    }

    HWND oldForeground =
        GetForegroundWindow();

    std::vector<HWND>
        restored;

    Wh_Log(
        L"Restoring %zu window(s) on desktop %s",
        candidates.size(),
        GuidToString(
            currentDesktopId)
            .c_str());

    for (const WindowState* item :
         candidates) {

        if (!IsWindow(
                item->hwnd)) {

            continue;
        }

        if (item->wasMaximized) {
            ShowWindowAsync(
                item->hwnd,
                kSwShowMaximized);
        } else {
            ShowWindowAsync(
                item->hwnd,
                kSwShowNoActivate);
        }

        restored.push_back(
            item->hwnd);
    }

    int settleDelay =
        g_settings
            .restoreSettleDelayMs
            .load();

    if (settleDelay > 0) {
        Sleep(settleDelay);
    }

    std::vector<HWND>
        validProtected;

    for (HWND hwnd :
         protectedWindows) {

        if (IsWindow(hwnd) &&
            IsWindowVisible(hwnd) &&
            !IsIconic(hwnd)) {

            validProtected.push_back(
                hwnd);
        }
    }

    if (!validProtected.empty()) {
        // EnumWindows enumerates top-to-bottom.
        HWND anchor =
            validProtected.back();

        for (HWND hwnd :
             restored) {

            if (!IsWindow(hwnd)) {
                continue;
            }

            SetWindowPos(
                hwnd,
                anchor,
                0,
                0,
                0,
                0,
                kSwpNoMove |
                    kSwpNoSize |
                    kSwpNoActivate |
                    kSwpNoOwnerZOrder);
        }
    }

    if (oldForeground &&
        IsWindow(oldForeground) &&
        !IsIconic(oldForeground)) {

        SetForegroundWindow(
            oldForeground);
    }

    return true;
}

// -----------------------------------------------------------------------------
// Custom per-desktop toggle
// -----------------------------------------------------------------------------

void ToggleCurrentDesktopWindows() {
    std::unique_lock<std::mutex>
        lock(
            g_toggleMutex,
            std::try_to_lock);

    if (!lock.owns_lock()) {
        Wh_Log(
            L"Toggle already running; click ignored");

        return;
    }

    ScopedComApartment com;

    if (!com.IsUsable()) {
        Wh_Log(
            L"CoInitializeEx failed: 0x%08X",
            static_cast<unsigned>(
                com.Result()));

        return;
    }

    IVirtualDesktopManager*
        desktopManager =
            nullptr;

    HRESULT hr =
        CoCreateInstance(
            __uuidof(
                VirtualDesktopManager),
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(
                IVirtualDesktopManager),
            reinterpret_cast<void**>(
                &desktopManager));

    if (FAILED(hr) ||
        !desktopManager) {

        Wh_Log(
            L"Failed to create IVirtualDesktopManager: 0x%08X",
            static_cast<unsigned>(
                hr));

        return;
    }

    auto releaseManager =
        [&]() {
            if (desktopManager) {
                desktopManager
                    ->Release();

                desktopManager =
                    nullptr;
            }
        };

    const auto currentWindows =
        GetCurrentManageableWindows(
            desktopManager);

    const auto currentDesktopId =
        GetCurrentDesktopId(
            desktopManager,
            currentWindows);

    if (!currentDesktopId) {
        Wh_Log(
            L"Couldn't determine current virtual desktop");

        releaseManager();
        return;
    }

    PruneExpiredStates();

    auto stateIt =
        g_desktopStates.find(
            *currentDesktopId);

    if (stateIt ==
        g_desktopStates.end()) {

        MinimizeCurrentWindows(
            desktopManager,
            *currentDesktopId);

        releaseManager();
        return;
    }

    bool candidatesExist =
        false;

    for (const WindowState& item :
         stateIt->second.windows) {

        if (IsRestoreCandidate(
                item,
                desktopManager,
                *currentDesktopId)) {

            candidatesExist = true;
            break;
        }
    }

    if (candidatesExist) {
        DesktopState state =
            std::move(
                stateIt->second);

        g_desktopStates.erase(
            stateIt);

        RestoreWindows(
            state,
            desktopManager,
            *currentDesktopId);

        releaseManager();
        return;
    }

    // Existing state is no longer a usable toggle state.
    g_desktopStates.erase(
        stateIt);

    MinimizeCurrentWindows(
        desktopManager,
        *currentDesktopId);

    releaseManager();
}

// -----------------------------------------------------------------------------
// Native Windows Show Desktop
// -----------------------------------------------------------------------------

bool InvokeNativeShowDesktop() {
    ScopedComApartment com;

    if (!com.IsUsable()) {
        Wh_Log(
            L"Native Show Desktop: COM initialization failed: 0x%08X",
            static_cast<unsigned>(
                com.Result()));

        return false;
    }

    CLSID shellClsid{};

    HRESULT hr =
        CLSIDFromProgID(
            L"Shell.Application",
            &shellClsid);

    if (FAILED(hr)) {
        Wh_Log(
            L"CLSIDFromProgID(Shell.Application) failed: 0x%08X",
            static_cast<unsigned>(
                hr));

        return false;
    }

    IShellDispatch4* shell =
        nullptr;

    hr =
        CoCreateInstance(
            shellClsid,
            nullptr,
            CLSCTX_INPROC_SERVER |
                CLSCTX_LOCAL_SERVER,
            __uuidof(
                IShellDispatch4),
            reinterpret_cast<void**>(
                &shell));

    if (FAILED(hr) ||
        !shell) {

        Wh_Log(
            L"Failed to create Shell.Application: 0x%08X",
            static_cast<unsigned>(
                hr));

        return false;
    }

    hr =
        shell->ToggleDesktop();

    shell->Release();

    if (FAILED(hr)) {
        Wh_Log(
            L"Shell.ToggleDesktop failed: 0x%08X",
            static_cast<unsigned>(
                hr));

        return false;
    }

    Wh_Log(
        L"Native Windows Show Desktop executed");

    return true;
}

void ExecuteNativeShowDesktop() {
    // Deliberately clear ALL desktop states.
    //
    // Native Show Desktop can change window state outside our own tracking.
    // Keeping old per-desktop restore entries after that is more dangerous than
    // losing them.
    ClearAllCustomState();

    InvokeNativeShowDesktop();
}

// -----------------------------------------------------------------------------
// Show Desktop XAML element detection
// -----------------------------------------------------------------------------

FrameworkElement EnumParentElements(
    FrameworkElement element,
    std::function<bool(
        FrameworkElement)>
        callback) {

    auto parent =
        element;

    while (true) {
        parent =
            Media::VisualTreeHelper
                ::GetParent(parent)
                .try_as<
                    FrameworkElement>();

        if (!parent) {
            return nullptr;
        }

        if (callback(parent)) {
            return parent;
        }
    }
}

FrameworkElement GetParentElementByName(
    FrameworkElement element,
    PCWSTR name) {

    return EnumParentElements(
        element,
        [name](
            FrameworkElement parent) {

            return parent.Name() ==
                   name;
        });
}

bool IsChildOfElementByName(
    FrameworkElement element,
    PCWSTR name) {

    return !!GetParentElementByName(
        element,
        name);
}

bool IsShowDesktopIconView(
    void* pThis) {

    FrameworkElement element =
        nullptr;

    ((IUnknown*)pThis)
        ->QueryInterface(
            winrt::guid_of<
                FrameworkElement>(),
            winrt::put_abi(
                element));

    if (!element) {
        return false;
    }

    return
        winrt::get_class_name(
            element) ==
            L"SystemTray.IconView" &&
        IsChildOfElementByName(
            element,
            L"ShowDesktopStack");
}

// -----------------------------------------------------------------------------
// Input handling
// -----------------------------------------------------------------------------

constexpr UINT
    kCustomWorkerMessage =
        WM_APP + 0x4D1;

constexpr UINT
    kNativeWorkerMessage =
        WM_APP + 0x4D2;

std::atomic<bool>
    g_pointerOverShowDesktop{
        false};

enum class CapturedClick : int {
    None = 0,

    CustomLeft,
    CustomRight,

    NativeLeft,
    NativeRight,

    DisabledLeft,
    DisabledRight,
};

std::atomic<CapturedClick>
    g_capturedClick{
        CapturedClick::None};

POINT g_capturePoint{};
bool g_captureMoved = false;

HANDLE g_mouseHookThread =
    nullptr;

HANDLE g_workerThread =
    nullptr;

HANDLE g_mouseHookReadyEvent =
    nullptr;

HANDLE g_workerReadyEvent =
    nullptr;

DWORD g_mouseHookThreadId =
    0;

DWORD g_workerThreadId =
    0;

HHOOK g_lowLevelMouseHook =
    nullptr;

bool IsCapturedLeft(
    CapturedClick click) {

    return
        click ==
            CapturedClick::CustomLeft ||
        click ==
            CapturedClick::NativeLeft ||
        click ==
            CapturedClick::DisabledLeft;
}

bool IsCapturedRight(
    CapturedClick click) {

    return
        click ==
            CapturedClick::CustomRight ||
        click ==
            CapturedClick::NativeRight ||
        click ==
            CapturedClick::DisabledRight;
}

bool IsCustomClick(
    CapturedClick click) {

    return
        click ==
            CapturedClick::CustomLeft ||
        click ==
            CapturedClick::CustomRight;
}

bool IsNativeClick(
    CapturedClick click) {

    return
        click ==
            CapturedClick::NativeLeft ||
        click ==
            CapturedClick::NativeRight;
}

CapturedClick GetActionForButton(
    bool rightButton) {

    bool customOnRight =
        g_settings
            .customActionOnRight
            .load();

    if (rightButton ==
        customOnRight) {

        return rightButton
            ? CapturedClick::CustomRight
            : CapturedClick::CustomLeft;
    }

    if (g_settings
            .enableDefaultActionOnOtherButton
            .load()) {

        return rightButton
            ? CapturedClick::NativeRight
            : CapturedClick::NativeLeft;
    }

    return rightButton
        ? CapturedClick::DisabledRight
        : CapturedClick::DisabledLeft;
}

// -----------------------------------------------------------------------------
// Worker thread
// -----------------------------------------------------------------------------

DWORD WINAPI WorkerThreadProc(
    LPVOID) {

    g_workerThreadId =
        GetCurrentThreadId();

    MSG msg{};

    // Force creation of the thread message queue.
    PeekMessageW(
        &msg,
        nullptr,
        WM_USER,
        WM_USER,
        PM_NOREMOVE);

    Wh_Log(
        L"Worker thread started: %lu",
        g_workerThreadId);

    if (g_workerReadyEvent) {
        SetEvent(
            g_workerReadyEvent);
    }

    while (GetMessageW(
               &msg,
               nullptr,
               0,
               0) > 0) {

        switch (msg.message) {
            case kCustomWorkerMessage:
                Wh_Log(
                    L"Executing per-desktop action");

                ToggleCurrentDesktopWindows();
                break;

            case kNativeWorkerMessage:
                Wh_Log(
                    L"Executing native Show Desktop action");

                ExecuteNativeShowDesktop();
                break;
        }
    }

    Wh_Log(
        L"Worker thread stopped");

    g_workerThreadId =
        0;

    return 0;
}

// -----------------------------------------------------------------------------
// Low-level mouse hook
// -----------------------------------------------------------------------------

LRESULT CALLBACK LowLevelMouseProc(
    int nCode,
    WPARAM wParam,
    LPARAM lParam) {

    if (nCode < 0 ||
        nCode != HC_ACTION) {

        return CallNextHookEx(
            g_lowLevelMouseHook,
            nCode,
            wParam,
            lParam);
    }

    const auto* mouse =
        reinterpret_cast<
            const MSLLHOOKSTRUCT*>(
                lParam);

    if (!mouse) {
        return CallNextHookEx(
            g_lowLevelMouseHook,
            nCode,
            wParam,
            lParam);
    }

    // Don't interfere with input synthesized by other software.
    if (mouse->flags &
        LLMHF_INJECTED) {

        return CallNextHookEx(
            g_lowLevelMouseHook,
            nCode,
            wParam,
            lParam);
    }

    CapturedClick captured =
        g_capturedClick.load();

    // Track actual movement while a button is captured.
    //
    // XAML can occasionally clear hover state while a right button is being
    // pressed. A normal click with no real mouse movement is still considered
    // valid even if that happens.
    if (wParam ==
            WM_MOUSEMOVE &&
        captured !=
            CapturedClick::None) {

        int dx =
            static_cast<int>(
                mouse->pt.x -
                g_capturePoint.x);

        int dy =
            static_cast<int>(
                mouse->pt.y -
                g_capturePoint.y);

        int dragX =
            std::max(
                1,
                GetSystemMetrics(
                    SM_CXDRAG));

        int dragY =
            std::max(
                1,
                GetSystemMetrics(
                    SM_CYDRAG));

        if (std::abs(dx) >=
                dragX ||
            std::abs(dy) >=
                dragY) {

            g_captureMoved =
                true;
        }
    }

    // -------------------------------------------------------------------------
    // Button down
    // -------------------------------------------------------------------------

    bool leftDown =
        wParam ==
        WM_LBUTTONDOWN;

    bool rightDown =
        wParam ==
        WM_RBUTTONDOWN;

    if (leftDown ||
        rightDown) {

        if (!g_pointerOverShowDesktop
                 .load()) {

            return CallNextHookEx(
                g_lowLevelMouseHook,
                nCode,
                wParam,
                lParam);
        }

        bool rightButton =
            rightDown;

        CapturedClick click =
            GetActionForButton(
                rightButton);

        g_capturePoint =
            mouse->pt;

        g_captureMoved =
            false;

        g_capturedClick.store(
            click);

        Wh_Log(
            L"Show Desktop %s DOWN captured, action=%s",
            rightButton
                ? L"RIGHT"
                : L"LEFT",
            IsCustomClick(click)
                ? L"custom"
                : IsNativeClick(click)
                    ? L"native"
                    : L"disabled");

        // Always swallow our left/right button events on this area.
        //
        // Native behavior is invoked directly through Shell.ToggleDesktop,
        // therefore the taskbar itself doesn't need to receive this click.
        return 1;
    }

    // -------------------------------------------------------------------------
    // Button up
    // -------------------------------------------------------------------------

    bool leftUp =
        wParam ==
        WM_LBUTTONUP;

    bool rightUp =
        wParam ==
        WM_RBUTTONUP;

    if (leftUp ||
        rightUp) {

        captured =
            g_capturedClick.load();

        bool matchingButton =
            (leftUp &&
             IsCapturedLeft(
                 captured)) ||
            (rightUp &&
             IsCapturedRight(
                 captured));

        if (!matchingButton) {
            return CallNextHookEx(
                g_lowLevelMouseHook,
                nCode,
                wParam,
                lParam);
        }

        g_capturedClick.store(
            CapturedClick::None);

        bool stillOver =
            g_pointerOverShowDesktop
                .load();

        // If the pointer didn't really move, treat this as a valid ordinary
        // click even if WinUI/XAML temporarily cleared hover while the button
        // was pressed.
        bool validClick =
            stillOver ||
            !g_captureMoved;

        Wh_Log(
            L"Show Desktop %s UP captured, action=%s, over=%d, moved=%d, valid=%d",
            rightUp
                ? L"RIGHT"
                : L"LEFT",
            IsCustomClick(captured)
                ? L"custom"
                : IsNativeClick(captured)
                    ? L"native"
                    : L"disabled",
            stillOver ? 1 : 0,
            g_captureMoved ? 1 : 0,
            validClick ? 1 : 0);

        if (!validClick) {
            return 1;
        }

        if (g_workerThreadId ==
            0) {

            Wh_Log(
                L"Worker thread unavailable");

            return 1;
        }

        if (IsCustomClick(
                captured)) {

            if (!PostThreadMessageW(
                    g_workerThreadId,
                    kCustomWorkerMessage,
                    0,
                    0)) {

                Wh_Log(
                    L"Failed to post custom action: %lu",
                    GetLastError());
            }
        } else if (
            IsNativeClick(
                captured)) {

            if (!PostThreadMessageW(
                    g_workerThreadId,
                    kNativeWorkerMessage,
                    0,
                    0)) {

                Wh_Log(
                    L"Failed to post native action: %lu",
                    GetLastError());
            }
        }

        // Disabled = deliberately do nothing.
        return 1;
    }

    return CallNextHookEx(
        g_lowLevelMouseHook,
        nCode,
        wParam,
        lParam);
}

// -----------------------------------------------------------------------------
// Mouse hook thread
// -----------------------------------------------------------------------------

DWORD WINAPI MouseHookThreadProc(
    LPVOID) {

    g_mouseHookThreadId =
        GetCurrentThreadId();

    MSG msg{};

    PeekMessageW(
        &msg,
        nullptr,
        WM_USER,
        WM_USER,
        PM_NOREMOVE);

    MEMORY_BASIC_INFORMATION
        memoryInfo{};

    HINSTANCE module =
        nullptr;

    if (VirtualQuery(
            reinterpret_cast<LPCVOID>(
                &LowLevelMouseProc),
            &memoryInfo,
            sizeof(memoryInfo))) {

        module =
            reinterpret_cast<HINSTANCE>(
                memoryInfo.AllocationBase);
    }

    g_lowLevelMouseHook =
        SetWindowsHookExW(
            WH_MOUSE_LL,
            LowLevelMouseProc,
            module,
            0);

    if (!g_lowLevelMouseHook) {
        Wh_Log(
            L"SetWindowsHookExW(WH_MOUSE_LL) failed: %lu",
            GetLastError());

        if (g_mouseHookReadyEvent) {
            SetEvent(
                g_mouseHookReadyEvent);
        }

        g_mouseHookThreadId =
            0;

        return 0;
    }

    Wh_Log(
        L"WH_MOUSE_LL installed on thread %lu",
        g_mouseHookThreadId);

    if (g_mouseHookReadyEvent) {
        SetEvent(
            g_mouseHookReadyEvent);
    }

    while (GetMessageW(
               &msg,
               nullptr,
               0,
               0) > 0) {
    }

    if (g_lowLevelMouseHook) {
        UnhookWindowsHookEx(
            g_lowLevelMouseHook);

        g_lowLevelMouseHook =
            nullptr;
    }

    Wh_Log(
        L"Mouse hook thread stopped");

    g_mouseHookThreadId =
        0;

    return 0;
}

// -----------------------------------------------------------------------------
// Input infrastructure
// -----------------------------------------------------------------------------

void StopInputThreads();

bool StartInputThreads() {
    g_pointerOverShowDesktop.store(
        false);

    g_capturedClick.store(
        CapturedClick::None);

    g_captureMoved =
        false;

    // Start worker first.
    g_workerReadyEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_workerReadyEvent) {
        Wh_Log(
            L"CreateEvent(worker) failed: %lu",
            GetLastError());

        return false;
    }

    g_workerThread =
        CreateThread(
            nullptr,
            0,
            WorkerThreadProc,
            nullptr,
            0,
            nullptr);

    if (!g_workerThread) {
        Wh_Log(
            L"CreateThread(worker) failed: %lu",
            GetLastError());

        StopInputThreads();

        return false;
    }

    if (WaitForSingleObject(
            g_workerReadyEvent,
            2000) !=
        WAIT_OBJECT_0) {

        Wh_Log(
            L"Worker thread initialization timed out");

        StopInputThreads();

        return false;
    }

    g_mouseHookReadyEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_mouseHookReadyEvent) {
        Wh_Log(
            L"CreateEvent(mouse hook) failed: %lu",
            GetLastError());

        StopInputThreads();

        return false;
    }

    g_mouseHookThread =
        CreateThread(
            nullptr,
            0,
            MouseHookThreadProc,
            nullptr,
            0,
            nullptr);

    if (!g_mouseHookThread) {
        Wh_Log(
            L"CreateThread(mouse hook) failed: %lu",
            GetLastError());

        StopInputThreads();

        return false;
    }

    if (WaitForSingleObject(
            g_mouseHookReadyEvent,
            2000) !=
        WAIT_OBJECT_0) {

        Wh_Log(
            L"Mouse hook initialization timed out");

        StopInputThreads();

        return false;
    }

    if (!g_lowLevelMouseHook) {
        Wh_Log(
            L"WH_MOUSE_LL wasn't installed");

        StopInputThreads();

        return false;
    }

    return true;
}

void StopInputThreads() {
    g_pointerOverShowDesktop.store(
        false);

    g_capturedClick.store(
        CapturedClick::None);

    if (g_mouseHookThreadId) {
        PostThreadMessageW(
            g_mouseHookThreadId,
            WM_QUIT,
            0,
            0);
    }

    if (g_workerThreadId) {
        PostThreadMessageW(
            g_workerThreadId,
            WM_QUIT,
            0,
            0);
    }

    if (g_mouseHookThread) {
        WaitForSingleObject(
            g_mouseHookThread,
            INFINITE);

        CloseHandle(
            g_mouseHookThread);

        g_mouseHookThread =
            nullptr;
    }

    if (g_workerThread) {
        WaitForSingleObject(
            g_workerThread,
            INFINITE);

        CloseHandle(
            g_workerThread);

        g_workerThread =
            nullptr;
    }

    if (g_mouseHookReadyEvent) {
        CloseHandle(
            g_mouseHookReadyEvent);

        g_mouseHookReadyEvent =
            nullptr;
    }

    if (g_workerReadyEvent) {
        CloseHandle(
            g_workerReadyEvent);

        g_workerReadyEvent =
            nullptr;
    }

    g_mouseHookThreadId =
        0;

    g_workerThreadId =
        0;
}

// -----------------------------------------------------------------------------
// IconView pointer hooks
// -----------------------------------------------------------------------------

using IconView_OnPointerMoved_t =
    int(WINAPI*)(
        void* pThis,
        void* pArgs);

IconView_OnPointerMoved_t
    IconView_OnPointerMoved_Original;

int WINAPI
IconView_OnPointerMoved_Hook(
    void* pThis,
    void* pArgs) {

    if (IsShowDesktopIconView(
            pThis)) {

        if (!g_pointerOverShowDesktop
                 .exchange(true)) {

            Wh_Log(
                L"Pointer entered Show Desktop button");
        }
    }

    return
        IconView_OnPointerMoved_Original(
            pThis,
            pArgs);
}

using IconView_OnPointerExited_t =
    int(WINAPI*)(
        void* pThis,
        void* pArgs);

IconView_OnPointerExited_t
    IconView_OnPointerExited_Original;

int WINAPI
IconView_OnPointerExited_Hook(
    void* pThis,
    void* pArgs) {

    if (IsShowDesktopIconView(
            pThis)) {

        if (g_pointerOverShowDesktop
                .exchange(false)) {

            Wh_Log(
                L"Pointer exited Show Desktop button");
        }
    }

    return
        IconView_OnPointerExited_Original(
            pThis,
            pArgs);
}

// -----------------------------------------------------------------------------
// SystemTray symbol hooks
// -----------------------------------------------------------------------------

std::atomic<bool>
    g_systemTrayModuleHooked{
        false};

bool HookSystemTraySymbols(
    HMODULE module) {

    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {

        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::IconView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"
            },
            &IconView_OnPointerMoved_Original,
            IconView_OnPointerMoved_Hook,
        },

        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::IconView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"
            },
            &IconView_OnPointerExited_Original,
            IconView_OnPointerExited_Hook,
        },
    };

    if (!HookSymbols(
            module,
            symbolHooks,
            ARRAYSIZE(
                symbolHooks))) {

        Wh_Log(
            L"HookSymbols failed");

        return false;
    }

    return true;
}

VS_FIXEDFILEINFO*
GetModuleVersionInfo(
    HMODULE module,
    UINT* ptrLength) {

    void* fixedInfo =
        nullptr;

    UINT length =
        0;

    HRSRC resource =
        FindResourceW(
            module,
            MAKEINTRESOURCE(
                VS_VERSION_INFO),
            RT_VERSION);

    if (resource) {
        HGLOBAL global =
            LoadResource(
                module,
                resource);

        if (global) {
            void* data =
                LockResource(
                    global);

            if (data) {
                if (!VerQueryValueW(
                        data,
                        L"\\",
                        &fixedInfo,
                        &length) ||
                    length == 0) {

                    fixedInfo =
                        nullptr;

                    length =
                        0;
                }
            }
        }
    }

    if (ptrLength) {
        *ptrLength =
            length;
    }

    return reinterpret_cast<
        VS_FIXEDFILEINFO*>(
            fixedInfo);
}

HMODULE
GetSystemTrayModuleHandle() {

    HMODULE module =
        GetModuleHandleW(
            L"SystemTray.dll");

    if (!module) {
        module =
            GetModuleHandleW(
                L"Taskbar.View.dll");

        if (module) {
            VS_FIXEDFILEINFO*
                versionInfo =
                    GetModuleVersionInfo(
                        module,
                        nullptr);

            WORD moduleMajor =
                versionInfo
                    ? HIWORD(
                          versionInfo
                              ->dwFileVersionMS)
                    : 0;

            // Newer taskbar builds moved SystemTray types out of
            // Taskbar.View.dll.
            if (!moduleMajor ||
                moduleMajor >=
                    2604) {

                module =
                    nullptr;
            }
        }
    }

    if (!module) {
        module =
            GetModuleHandleW(
                L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfSystemTray(
    HMODULE module,
    LPCWSTR libraryName) {

    if (!g_systemTrayModuleHooked &&
        GetSystemTrayModuleHandle() ==
            module &&
        !g_systemTrayModuleHooked
             .exchange(true)) {

        Wh_Log(
            L"Loaded %s",
            libraryName);

        if (HookSystemTraySymbols(
                module)) {

            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t =
    decltype(
        &LoadLibraryExW);

LoadLibraryExW_t
    LoadLibraryExW_Original;

HMODULE WINAPI
LoadLibraryExW_Hook(
    LPCWSTR libraryName,
    HANDLE file,
    DWORD flags) {

    HMODULE module =
        LoadLibraryExW_Original(
            libraryName,
            file,
            flags);

    if (module) {
        HandleLoadedModuleIfSystemTray(
            module,
            libraryName);
    }

    return module;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(
        L"Initializing BetterShowDesktop");

    LoadSettings();

    if (HMODULE systemTrayModule =
            GetSystemTrayModuleHandle()) {

        g_systemTrayModuleHooked.store(
            true);

        if (!HookSystemTraySymbols(
                systemTrayModule)) {

            return FALSE;
        }
    } else {
        Wh_Log(
            L"System tray module isn't loaded yet");

        HMODULE kernelBase =
            GetModuleHandleW(
                L"kernelbase.dll");

        if (!kernelBase) {
            Wh_Log(
                L"kernelbase.dll isn't loaded");

            return FALSE;
        }

        auto loadLibraryExW =
            reinterpret_cast<
                decltype(
                    &LoadLibraryExW)>(
                        GetProcAddress(
                            kernelBase,
                            "LoadLibraryExW"));

        if (!loadLibraryExW) {
            Wh_Log(
                L"Couldn't find LoadLibraryExW");

            return FALSE;
        }

        WindhawkUtils::
            SetFunctionHook(
                loadLibraryExW,
                LoadLibraryExW_Hook,
                &LoadLibraryExW_Original);
    }

    if (!StartInputThreads()) {
        Wh_Log(
            L"Failed to initialize input interception");

        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(
        L"After init");

    if (!g_systemTrayModuleHooked
             .load()) {

        if (HMODULE systemTrayModule =
                GetSystemTrayModuleHandle()) {

            if (!g_systemTrayModuleHooked
                     .exchange(true)) {

                Wh_Log(
                    L"Found system tray module after init");

                if (HookSystemTraySymbols(
                        systemTrayModule)) {

                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(
        L"Before uninit");

    StopInputThreads();

    {
        std::lock_guard<std::mutex>
            lock(g_toggleMutex);

        g_desktopStates.clear();
    }
}

void Wh_ModUninit() {
    Wh_Log(
        L"BetterShowDesktop uninitialized");
}

void Wh_ModSettingsChanged() {
    Wh_Log(
        L"Settings changed");

    LoadSettings();
}