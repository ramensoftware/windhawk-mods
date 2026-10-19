// ==WindhawkMod==
// @id              global-menu-bar
// @name            Global Menu Bar (macOS style)
// @description     A macOS-style global menu bar at the top of the screen that shows the menus of the focused window, tray icons, status icons and a clock
// @version         1.0.0
// @author          Rafael Mendes
// @github          https://github.com/RafaelMendes1407
// @include         *
// @exclude         conhost.exe
// @exclude         dwm.exe
// @exclude         backgroundTaskHost.exe
// @exclude         LockApp.exe
// @exclude         SearchHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         msedgewebview2.exe
// @exclude         windhawk.exe
// @exclude         windhawk-ui.exe
// @compilerOptions -lgdi32 -lshell32 -lversion -lpowrprof -lshcore -ladvapi32 -lcomctl32 -ldwmapi -ld2d1 -ldwrite -lwindowscodecs -lole32 -loleaut32 -luuid -lruntimeobject -lwlanapi -liphlpapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Global Menu Bar (macOS style)

Creates a bar fixed at the top of the monitors, like the macOS menu bar. It
follows the focused window and shows:

- **⊞ menu** (the equivalent of the Apple menu): About this PC, Settings,
  Task Manager, Sleep, Restart, Shut down, Lock, Sign out.
- **App name** in bold, with a window menu.
- **The menus of the active window** (File, Edit, View...). With a menu open,
  just hover another item to switch menus.
- **Menus of modern apps** (WinUI, UWP, Electron like VS Code...) read
  through UI Automation. When clicked, the menu opens inside the app itself.
- **Standard menus** for apps without a menu bar (Chrome, Terminal,
  Discord...): File, Edit, View, Window and Help, which send the common
  shortcuts (Ctrl+S, Ctrl+C, Ctrl+Z, F11...).
- **Tray icons** of apps (Discord, Steam, OneDrive...) in their own menu (ˇ)
  and/or directly on the bar. In the menu, left click opens the app and right
  click opens the icon's menu.
- **Status icons**: Wi-Fi/Ethernet, Bluetooth, volume (scroll the mouse to
  adjust), battery, search and quick settings.
- **Clock**: click it to open the notification center and the calendar.
- **System flyouts at the top**: quick settings (Wi-Fi, Bluetooth, volume),
  notifications and calendar open right below the bar, on the monitor whose
  bar was clicked.

## How it works
The mod runs in `explorer.exe` (the bar), in each app (a small agent) and in
`ShellHost.exe`/`ShellExperienceHost.exe` (to bring the flyouts to the top).
The agent opens the menu inside the app itself, so everything works like the
original menu, including menus drawn by the app. If the option is on, it also
hides the native menu inside the window.

## If an app breaks
Add the executable (e.g. `notepad++.exe`) to **Excluded apps**. The mod
stops touching that app and the bar only shows its name. Restart the app
after excluding it.

## Limitations
- Hiding the menu only works in apps with a classic Win32 menu. In modern
  apps the menu is part of the app's interface.
- With the menu hidden, the Alt key no longer opens the menu inside the
  window.
- On startup, the mod asks apps to register their tray icons again (the
  `TaskbarCreated` message). Some rare apps may duplicate the icon.
- The system Mica/Acrylic effects require Windows 11 22H2 or newer.
- The flyouts at the top depend on the internal structure of Windows 11 and
  may stop working after updates. Don't use it together with the Windows 11
  Notification Center Styler mod configured to change the same flyouts.
- Electron apps (VS Code etc.) only expose their menu after one or two
  seconds in focus; until then the bar shows the standard menus.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- monitors: all
  $name: Monitors
  $options:
  - all: All monitors
  - primary: Primary monitor only
- height: 28
  $name: Bar height
  $description: In pixels, at 100% scaling
- theme: auto
  $name: Theme
  $options:
  - auto: Follow Windows
  - dark: Dark
  - light: Light
- backdrop: acrylic
  $name: Background effect
  $options:
  - solid: Solid
  - acrylic: Acrylic
  - mica: Mica (Windows 11)
  - micaAlt: Mica Alt (Windows 11)
  - acrylicSystem: System acrylic (Windows 11)
- opacity: 55
  $name: Background color opacity (%)
  $description: Theme color applied over the effect. In solid mode, use 100 for an opaque bar
- reserveSpace: true
  $name: Reserve screen space
  $description: Maximized windows don't go under the bar
- hideNativeMenu: false
  $name: Hide the menu inside windows
  $description: Removes the classic menu bar from windows; the menu stays only on the global bar
- modernMenus: true
  $name: Show menus of modern apps
  $description: Uses UI Automation to read the menus of WinUI, UWP, Electron apps etc.
- standardMenus: true
  $name: Standard menus for apps without a menu
  $description: >-
    File, Edit, View, Window and Help with the common shortcuts (Ctrl+S,
    Ctrl+C, F11...) for apps that don't have a menu bar
- openMode: auto
  $name: Menu opening mode
  $options:
  - auto: Open the menus on the bar (macOS style)
  - native: Trigger the window's native menu
- trayMode: menu
  $name: Tray icons
  $options:
  - menu: In their own menu on the bar
  - inline: Directly on the bar
  - both: Both
  - off: Hidden
- flyoutsOnTop: true
  $name: Open system flyouts at the top
  $description: >-
    Quick settings (Wi-Fi, Bluetooth, volume), notifications and calendar
    open right below the bar
- flyoutsGap: 8
  $name: Space between the bar and the flyouts
  $description: In pixels, at 100% scaling
- flyoutsAnimateFromTop: true
  $name: Animate quick settings from the top down
- statusIcons:
  - wifi: true
    $name: Wi-Fi / network
  - bluetooth: true
    $name: Bluetooth
  - volume: true
    $name: Volume
  - battery: true
    $name: Battery
  - batteryPercent: true
    $name: Battery percentage
  - search: true
    $name: Search
  - controlCenter: true
    $name: Quick settings
  $name: Status icons
- showClock: true
  $name: Show clock
- dateFormat: "ddd d MMM"
  $name: Date format
  $description: GetDateFormatEx format; leave empty to hide
- timeFormat: "HH:mm"
  $name: Time format
  $description: GetTimeFormatEx format; leave empty to hide
- excludedApps: [""]
  $name: Excluded apps
  $description: >-
    Executable name (e.g. notepad++.exe). The mod doesn't touch these apps and
    the bar only shows their name
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#include <powrprof.h>
// windhawk_api.h already included windows.h; the order warning is harmless here.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-W#warnings"
#include <winsock2.h>
#pragma clang diagnostic pop
#include <iphlpapi.h>
#include <wlanapi.h>
// Otherwise the Win32 macro renames ID2D1RenderTarget::DrawText.
#undef DrawText
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <uiautomation.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <wrl/client.h>

#undef GetCurrentTime
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Radios.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.Provider.h>

#include <ocidl.h>
#include <xamlom.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <map>
#include <mutex>
#include <memory>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

////////////////////////////////////////////////////////////////////////////////
// Shared by both sides (bar in explorer and agent in apps)

constexpr WCHAR kBarClass[] = L"WindhawkGlobalMenuBar";
constexpr WCHAR kControllerClass[] = L"WindhawkGlobalMenuController";
constexpr WCHAR kPropMenu[] = L"WhGlobalMenu.HiddenMenu";

// Bar -> agent: open submenu wParam at position lParam (physical).
UINT g_msgPopup;
// Bar -> agent: wParam=1 hides the native menu, 0 restores it.
UINT g_msgHide;
// Agent -> bar (SendMessage): the mouse moved over the bar with a menu
// open. wParam=1 if it was a click. Returns 1 if the menu should close.
UINT g_msgMenuHover;
// Agent -> controller: the menu opened by the agent closed.
UINT g_msgMenuClosed;

void RegisterSharedMessages() {
    g_msgPopup = RegisterWindowMessageW(L"WhGlobalMenu.Popup");
    g_msgHide = RegisterWindowMessageW(L"WhGlobalMenu.Hide");
    g_msgMenuHover = RegisterWindowMessageW(L"WhGlobalMenu.MenuHover");
    g_msgMenuClosed = RegisterWindowMessageW(L"WhGlobalMenu.MenuClosed");
}

std::wstring AgentObjectName(DWORD pid) {
    return L"Local\\WhGlobalMenuAgent_" + std::to_wstring(pid);
}

std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

std::wstring ToLower(std::wstring s) {
    CharLowerBuffW(s.data(), (DWORD)s.size());
    return s;
}

std::wstring FileNameFromPath(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    return slash == std::wstring::npos ? path : path.substr(slash + 1);
}

std::vector<std::wstring> LoadExcludedApps() {
    std::vector<std::wstring> result;
    for (int i = 0; i < 64; i++) {
        PCWSTR value = Wh_GetStringSetting(L"excludedApps[%d]", i);
        std::wstring name = value ? value : L"";
        Wh_FreeStringSetting(value);
        if (!name.empty()) {
            result.push_back(ToLower(FileNameFromPath(name)));
        }
    }
    return result;
}

bool IsExcludedPath(const std::vector<std::wstring>& excluded,
                    const std::wstring& path) {
    if (path.empty()) {
        return false;
    }
    std::wstring name = ToLower(FileNameFromPath(path));
    return std::find(excluded.begin(), excluded.end(), name) != excluded.end();
}

std::wstring GetCurrentProcessPath() {
    WCHAR path[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    return std::wstring(path, len);
}

bool IsWindowOfClass(HWND hwnd, PCWSTR className) {
    WCHAR cls[64];
    return hwnd && GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
           !wcscmp(cls, className);
}

// System flyouts always open on the primary monitor. When a status item of
// another monitor's bar is clicked, explorer stores that monitor in a
// property of Shell_TrayWnd (with the time of the click), and ShellHost
// reads it from there when positioning quick settings.
constexpr WCHAR kFlyoutTargetProp[] = L"GlobalMenuBarFlyoutTarget";
constexpr DWORD kFlyoutTargetTimeout = 3000;

void SetFlyoutTarget(HMONITOR monitor) {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    MONITORINFO mi{sizeof(mi)};
    if (!tray || !GetMonitorInfoW(monitor, &mi)) {
        return;
    }
    int x = (mi.rcMonitor.left + mi.rcMonitor.right) / 2;
    int y = (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
    ULONG_PTR value = ((ULONG_PTR)GetTickCount() << 32) |
                      ((ULONG_PTR)(USHORT)(SHORT)x << 16) |
                      (USHORT)(SHORT)y;
    SetPropW(tray, kFlyoutTargetProp, (HANDLE)value);
}

// Monitor requested by a recent click on the bar, or nullptr.
HMONITOR GetFlyoutTarget() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    ULONG_PTR value = tray ? (ULONG_PTR)GetPropW(tray, kFlyoutTargetProp) : 0;
    if (!value || GetTickCount() - (DWORD)(value >> 32) > kFlyoutTargetTimeout) {
        return nullptr;
    }
    POINT pt{(SHORT)(USHORT)(value >> 16), (SHORT)(USHORT)value};
    return MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);
}

UINT MonitorDpi(HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        return 96;
    }
    return dpiY;
}

////////////////////////////////////////////////////////////////////////////////
// Agent: runs inside each app

namespace agent {

bool g_active;
bool g_hide;
std::vector<std::wstring> g_excluded;
HANDLE g_presence;

HWND g_trackHwnd;
HHOOK g_filterHook;

using GetMenu_t = HMENU(WINAPI*)(HWND);
using SetMenu_t = BOOL(WINAPI*)(HWND, HMENU);
using DispatchMessage_t = LRESULT(WINAPI*)(const MSG*);

GetMenu_t GetMenu_orig;
SetMenu_t SetMenu_orig;
DispatchMessage_t DispatchMessageW_orig;
DispatchMessage_t DispatchMessageA_orig;

// With the menu hidden, the app still sees its own menu.
HMENU WINAPI GetMenu_hook(HWND hwnd) {
    HMENU menu = GetMenu_orig(hwnd);
    if (!menu && g_active) {
        if (HANDLE saved = GetPropW(hwnd, kPropMenu)) {
            return (HMENU)saved;
        }
    }
    return menu;
}

BOOL WINAPI SetMenu_hook(HWND hwnd, HMENU menu) {
    if (g_active && GetPropW(hwnd, kPropMenu)) {
        if (!menu) {
            RemovePropW(hwnd, kPropMenu);
            return SetMenu_orig(hwnd, nullptr);
        }
        SetPropW(hwnd, kPropMenu, menu);
        return TRUE;
    }
    return SetMenu_orig(hwnd, menu);
}

void HideMenu(HWND hwnd) {
    HMENU menu = GetMenu_orig(hwnd);
    if (menu && IsMenu(menu)) {
        SetPropW(hwnd, kPropMenu, menu);
        SetMenu_orig(hwnd, nullptr);
    }
}

void RestoreMenu(HWND hwnd) {
    HMENU menu = (HMENU)GetPropW(hwnd, kPropMenu);
    if (!menu) {
        return;
    }
    RemovePropW(hwnd, kPropMenu);
    if (!GetMenu_orig(hwnd) && IsMenu(menu)) {
        SetMenu_orig(hwnd, menu);
    }
}

BOOL CALLBACK RestoreMenuEnumProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && GetPropW(hwnd, kPropMenu)) {
        RestoreMenu(hwnd);
    }
    return TRUE;
}

void RestoreAllMenus() {
    EnumWindows(RestoreMenuEnumProc, 0);
}

// During the menu: if the mouse moves over another item of the global bar,
// asks the bar whether to switch menus.
LRESULT CALLBACK MenuFilterProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == MSGF_MENU) {
        MSG* msg = (MSG*)lParam;
        if (msg->message == WM_MOUSEMOVE || msg->message == WM_LBUTTONDOWN) {
            HWND under = WindowFromPoint(msg->pt);
            if (IsWindowOfClass(under, kBarClass)) {
                POINT pt = msg->pt;
                LogicalToPhysicalPointForPerMonitorDPI(g_trackHwnd, &pt);
                DWORD_PTR result = 0;
                if (SendMessageTimeoutW(under, g_msgMenuHover,
                                        msg->message == WM_LBUTTONDOWN,
                                        MAKELPARAM(pt.x, pt.y),
                                        SMTO_ABORTIFHUNG, 200, &result) &&
                    result) {
                    EndMenu();
                    return TRUE;
                }
            }
        }
    }
    return CallNextHookEx(g_filterHook, code, wParam, lParam);
}

void NotifyMenuClosed() {
    if (HWND controller = FindWindowExW(HWND_MESSAGE, nullptr,
                                        kControllerClass, nullptr)) {
        PostMessageW(controller, g_msgMenuClosed, 0, 0);
    }
}

void ShowPopup(HWND hwnd, int pos, POINT pt) {
    HMENU menu = GetMenu_hook(hwnd);
    if (!menu || !IsMenu(menu)) {
        NotifyMenuClosed();
        return;
    }

    HMENU sub = GetSubMenu(menu, pos);
    if (!sub) {
        UINT id = GetMenuItemID(menu, pos);
        NotifyMenuClosed();
        if (id != (UINT)-1) {
            SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(id, 0), 0);
        }
        return;
    }

    PhysicalToLogicalPointForPerMonitorDPI(hwnd, &pt);
    SetForegroundWindow(hwnd);
    SendMessageW(hwnd, WM_INITMENU, (WPARAM)menu, 0);

    g_trackHwnd = hwnd;
    g_filterHook = SetWindowsHookExW(WH_MSGFILTER, MenuFilterProc, nullptr,
                                     GetCurrentThreadId());
    TrackPopupMenuEx(sub, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON, pt.x,
                     pt.y, hwnd, nullptr);
    if (g_filterHook) {
        UnhookWindowsHookEx(g_filterHook);
        g_filterHook = nullptr;
    }
    g_trackHwnd = nullptr;

    NotifyMenuClosed();
}

bool HandleAgentMessage(const MSG* msg) {
    if (!g_active || !msg || !msg->hwnd) {
        return false;
    }
    if (msg->message == g_msgPopup) {
        ShowPopup(msg->hwnd, (int)msg->wParam,
                  POINT{GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)});
        return true;
    }
    if (msg->message == g_msgHide) {
        if (msg->wParam && g_hide) {
            HideMenu(msg->hwnd);
        } else {
            RestoreMenu(msg->hwnd);
        }
        return true;
    }
    return false;
}

LRESULT WINAPI DispatchMessageW_hook(const MSG* msg) {
    if (HandleAgentMessage(msg)) {
        return 0;
    }
    return DispatchMessageW_orig(msg);
}

LRESULT WINAPI DispatchMessageA_hook(const MSG* msg) {
    if (HandleAgentMessage(msg)) {
        return 0;
    }
    return DispatchMessageA_orig(msg);
}

bool IsInteractiveProcess() {
    HWINSTA station = GetProcessWindowStation();
    USEROBJECTFLAGS flags{};
    DWORD needed = 0;
    return station &&
           GetUserObjectInformationW(station, UOI_FLAGS, &flags, sizeof(flags),
                                     &needed) &&
           (flags.dwFlags & WSF_VISIBLE);
}

void SetPresence(bool present) {
    if (present && !g_presence) {
        g_presence = CreateEventW(nullptr, TRUE, FALSE,
                                  AgentObjectName(GetCurrentProcessId()).c_str());
    } else if (!present && g_presence) {
        CloseHandle(g_presence);
        g_presence = nullptr;
    }
}

BOOL Init() {
    if (!IsInteractiveProcess()) {
        return FALSE;
    }

    g_excluded = LoadExcludedApps();
    if (IsExcludedPath(g_excluded, GetCurrentProcessPath())) {
        return FALSE;
    }

    RegisterSharedMessages();
    g_hide = Wh_GetIntSetting(L"hideNativeMenu") != 0;
    g_active = true;

    Wh_SetFunctionHook((void*)GetMenu, (void*)GetMenu_hook,
                       (void**)&GetMenu_orig);
    Wh_SetFunctionHook((void*)SetMenu, (void*)SetMenu_hook,
                       (void**)&SetMenu_orig);
    Wh_SetFunctionHook((void*)DispatchMessageW, (void*)DispatchMessageW_hook,
                       (void**)&DispatchMessageW_orig);
    Wh_SetFunctionHook((void*)DispatchMessageA, (void*)DispatchMessageA_hook,
                       (void**)&DispatchMessageA_orig);

    // Lets explorer (medium integrity) talk to elevated apps.
    ChangeWindowMessageFilter(g_msgPopup, MSGFLT_ADD);
    ChangeWindowMessageFilter(g_msgHide, MSGFLT_ADD);

    SetPresence(true);
    return TRUE;
}

void BeforeUninit() {
    SetPresence(false);
    RestoreAllMenus();
    g_active = false;
}

void SettingsChanged() {
    g_hide = Wh_GetIntSetting(L"hideNativeMenu") != 0;
    g_excluded = LoadExcludedApps();
    bool excluded = IsExcludedPath(g_excluded, GetCurrentProcessPath());

    if (excluded || !g_hide) {
        RestoreAllMenus();
    }
    g_active = !excluded;
    SetPresence(g_active);
}

}  // namespace agent

namespace flyouts {
bool OpenNotificationCenterOn(HMONITOR monitor);
}  // namespace flyouts

////////////////////////////////////////////////////////////////////////////////
// Bar: runs in explorer.exe

namespace bar {

enum class Theme { Auto, Dark, Light };
enum class Backdrop { Solid, Acrylic, Mica, MicaAlt, AcrylicSystem };

struct Settings {
    bool allMonitors = true;
    int height = 28;
    Theme theme = Theme::Auto;
    Backdrop backdrop = Backdrop::Acrylic;
    int opacity = 55;
    bool reserveSpace = true;
    bool hideNativeMenu = false;
    bool modernMenus = true;
    bool standardMenus = true;
    bool nativeMode = false;
    bool trayIcons = true;  // captures the icons (menu or directly on the bar)
    bool trayInline = false;
    bool trayMenu = true;
    bool wifi = true;
    bool bluetooth = true;
    bool volume = true;
    bool battery = true;
    bool batteryPercent = true;
    bool search = true;
    bool controlCenter = true;
    bool showClock = true;
    std::wstring dateFormat;
    std::wstring timeFormat;
    std::vector<std::wstring> excluded;
} g_settings;

void LoadSettings() {
    auto boolSetting = [](PCWSTR name) { return Wh_GetIntSetting(name) != 0; };

    g_settings.allMonitors = GetStringSetting(L"monitors") != L"primary";
    g_settings.height = std::clamp(Wh_GetIntSetting(L"height"), 20, 64);
    g_settings.opacity = std::clamp(Wh_GetIntSetting(L"opacity"), 0, 100);
    g_settings.reserveSpace = boolSetting(L"reserveSpace");
    g_settings.hideNativeMenu = boolSetting(L"hideNativeMenu");
    g_settings.modernMenus = boolSetting(L"modernMenus");
    g_settings.standardMenus = boolSetting(L"standardMenus");
    g_settings.nativeMode = GetStringSetting(L"openMode") == L"native";
    std::wstring trayMode = GetStringSetting(L"trayMode");
    g_settings.trayInline = trayMode == L"inline" || trayMode == L"both";
    g_settings.trayMenu = trayMode == L"menu" || trayMode == L"both";
    g_settings.trayIcons = g_settings.trayInline || g_settings.trayMenu;
    g_settings.wifi = boolSetting(L"statusIcons.wifi");
    g_settings.bluetooth = boolSetting(L"statusIcons.bluetooth");
    g_settings.volume = boolSetting(L"statusIcons.volume");
    g_settings.battery = boolSetting(L"statusIcons.battery");
    g_settings.batteryPercent = boolSetting(L"statusIcons.batteryPercent");
    g_settings.search = boolSetting(L"statusIcons.search");
    g_settings.controlCenter = boolSetting(L"statusIcons.controlCenter");
    g_settings.showClock = boolSetting(L"showClock");
    g_settings.dateFormat = GetStringSetting(L"dateFormat");
    g_settings.timeFormat = GetStringSetting(L"timeFormat");
    g_settings.excluded = LoadExcludedApps();

    std::wstring theme = GetStringSetting(L"theme");
    g_settings.theme = theme == L"dark"    ? Theme::Dark
                       : theme == L"light" ? Theme::Light
                                           : Theme::Auto;

    std::wstring backdrop = GetStringSetting(L"backdrop");
    g_settings.backdrop = backdrop == L"solid"           ? Backdrop::Solid
                          : backdrop == L"mica"          ? Backdrop::Mica
                          : backdrop == L"micaAlt"       ? Backdrop::MicaAlt
                          : backdrop == L"acrylicSystem" ? Backdrop::AcrylicSystem
                                                         : Backdrop::Acrylic;
}

// Internal controller messages.
constexpr UINT WM_C_RELOAD = WM_APP + 1;
constexpr UINT WM_C_TRAY = WM_APP + 2;
constexpr UINT WM_C_STATUS = WM_APP + 3;
constexpr UINT WM_C_UIA = WM_APP + 4;
constexpr UINT WM_C_RECREATE_BARS = WM_APP + 5;
constexpr UINT WM_C_THEME = WM_APP + 6;

constexpr UINT WM_BAR_APPBAR = WM_APP + 20;

constexpr UINT_PTR TIMER_TICK = 1;
constexpr UINT_PTR TIMER_WAIT_TASKBAR = 2;
constexpr UINT_PTR TIMER_RECREATE = 3;
constexpr UINT_PTR TIMER_ASYNC_CHECK = 4;
constexpr UINT_PTR TIMER_TOOLTIP = 5;
constexpr UINT_PTR TIMER_UIA_RETRY = 6;
constexpr UINT_PTR TIMER_SEND_KEYS = 7;

////////////////////////////////////////////////////////////////////////////////
// Worker thread: system status and UI Automation

namespace worker {

constexpr UINT WM_W_UIA_QUERY = WM_APP + 40;   // wParam=generation, lParam=HWND
constexpr UINT WM_W_UIA_INVOKE = WM_APP + 41;  // wParam=generation, lParam=index
constexpr UINT WM_W_POLL_NOW = WM_APP + 42;

struct StatusInfo {
    bool hasBattery = false;
    int batteryPercent = -1;
    bool charging = false;
    bool hasVolume = false;
    int volume = 0;
    bool muted = false;
    bool wifiConnected = false;
    bool ethernetConnected = false;
    int wifiSignal = -1;
    std::wstring wifiSsid;
    bool hasWifiRadio = false;
    bool wifiRadioOn = false;
    bool hasBluetooth = false;
    bool bluetoothOn = false;

    bool operator==(const StatusInfo&) const = default;
};

struct UiaResult {
    UINT generation;
    std::vector<std::wstring> names;
};

HANDLE g_thread;
DWORD g_threadId;
HANDLE g_readyEvent;
HWND g_controller;

ComPtr<IUIAutomation> g_uia;
ComPtr<IMMDeviceEnumerator> g_deviceEnumerator;
HANDLE g_wlan;
UINT g_uiaGeneration;
std::vector<ComPtr<IUIAutomationElement>> g_uiaItems;
StatusInfo g_lastStatus;
StatusInfo g_radios;
int g_pollCount;

void PollPower(StatusInfo& s) {
    SYSTEM_POWER_STATUS ps;
    if (GetSystemPowerStatus(&ps) && ps.BatteryFlag != 255 &&
        !(ps.BatteryFlag & 128)) {
        s.hasBattery = true;
        s.batteryPercent =
            ps.BatteryLifePercent <= 100 ? ps.BatteryLifePercent : -1;
        s.charging = (ps.BatteryFlag & 8) || ps.ACLineStatus == 1;
    }
}

void PollVolume(StatusInfo& s) {
    if (!g_deviceEnumerator) {
        CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                         IID_PPV_ARGS(&g_deviceEnumerator));
    }
    if (!g_deviceEnumerator) {
        return;
    }

    ComPtr<IMMDevice> device;
    if (FAILED(g_deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                           &device))) {
        return;
    }
    ComPtr<IAudioEndpointVolume> volume;
    if (FAILED(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                                nullptr, (void**)&volume))) {
        return;
    }

    float level = 0;
    BOOL muted = FALSE;
    volume->GetMasterVolumeLevelScalar(&level);
    volume->GetMute(&muted);
    s.hasVolume = true;
    s.volume = (int)(level * 100 + 0.5f);
    s.muted = muted;
}

void PollNetwork(StatusInfo& s) {
    ULONG flags = GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_SKIP_ANYCAST |
                  GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
    ULONG size = 16 * 1024;
    std::vector<BYTE> buffer(size);
    ULONG result = GetAdaptersAddresses(
        AF_UNSPEC, flags, nullptr, (PIP_ADAPTER_ADDRESSES)buffer.data(), &size);
    if (result == ERROR_BUFFER_OVERFLOW) {
        buffer.resize(size);
        result = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr,
                                      (PIP_ADAPTER_ADDRESSES)buffer.data(),
                                      &size);
    }
    if (result == NO_ERROR) {
        for (auto* a = (PIP_ADAPTER_ADDRESSES)buffer.data(); a; a = a->Next) {
            if (a->OperStatus != IfOperStatusUp || !a->FirstGatewayAddress ||
                a->IfType == IF_TYPE_SOFTWARE_LOOPBACK) {
                continue;
            }
            if (a->IfType == IF_TYPE_IEEE80211) {
                s.wifiConnected = true;
            } else {
                s.ethernetConnected = true;
            }
        }
    }

    // Signal and network name; may fail without location permission.
    if (!g_wlan) {
        DWORD version;
        if (WlanOpenHandle(2, nullptr, &version, &g_wlan) != ERROR_SUCCESS) {
            g_wlan = nullptr;
            return;
        }
    }
    PWLAN_INTERFACE_INFO_LIST list = nullptr;
    if (WlanEnumInterfaces(g_wlan, nullptr, &list) != ERROR_SUCCESS) {
        return;
    }
    for (DWORD i = 0; i < list->dwNumberOfItems; i++) {
        const WLAN_INTERFACE_INFO& info = list->InterfaceInfo[i];
        if (info.isState != wlan_interface_state_connected) {
            continue;
        }
        s.wifiConnected = true;
        DWORD dataSize = 0;
        PWLAN_CONNECTION_ATTRIBUTES attributes = nullptr;
        if (WlanQueryInterface(g_wlan, &info.InterfaceGuid,
                               wlan_intf_opcode_current_connection, nullptr,
                               &dataSize, (PVOID*)&attributes,
                               nullptr) == ERROR_SUCCESS &&
            attributes) {
            const auto& assoc = attributes->wlanAssociationAttributes;
            s.wifiSignal = (int)assoc.wlanSignalQuality;
            WCHAR ssid[64];
            int len = MultiByteToWideChar(
                CP_UTF8, 0, (const char*)assoc.dot11Ssid.ucSSID,
                (int)assoc.dot11Ssid.uSSIDLength, ssid, ARRAYSIZE(ssid) - 1);
            ssid[std::max(len, 0)] = L'\0';
            s.wifiSsid = ssid;
            WlanFreeMemory(attributes);
        }
        break;
    }
    WlanFreeMemory(list);
}

void PollRadios() {
    using namespace winrt::Windows::Devices::Radios;
    StatusInfo radios;
    try {
        auto list = Radio::GetRadiosAsync().get();
        for (const auto& radio : list) {
            bool on = radio.State() == RadioState::On;
            if (radio.Kind() == RadioKind::Bluetooth) {
                radios.hasBluetooth = true;
                radios.bluetoothOn |= on;
            } else if (radio.Kind() == RadioKind::WiFi) {
                radios.hasWifiRadio = true;
                radios.wifiRadioOn |= on;
            }
        }
    } catch (...) {
        return;
    }
    g_radios = radios;
}

void Poll(bool force) {
    // Radios are more expensive to query.
    if (force || g_pollCount++ % 4 == 0) {
        PollRadios();
    }

    StatusInfo s;
    PollPower(s);
    PollVolume(s);
    PollNetwork(s);
    s.hasBluetooth = g_radios.hasBluetooth;
    s.bluetoothOn = g_radios.bluetoothOn;
    s.hasWifiRadio = g_radios.hasWifiRadio;
    s.wifiRadioOn = g_radios.wifiRadioOn;

    if (force || !(s == g_lastStatus)) {
        g_lastStatus = s;
        auto* copy = new StatusInfo(s);
        if (!PostMessageW(g_controller, WM_C_STATUS, 0, (LPARAM)copy)) {
            delete copy;
        }
    }
}

// Requests type, id and name along with each element: one trip to the app's
// process per node instead of three.
ComPtr<IUIAutomationCacheRequest> CreateUiaCache() {
    ComPtr<IUIAutomationCacheRequest> cache;
    if (SUCCEEDED(g_uia->CreateCacheRequest(&cache))) {
        cache->AddProperty(UIA_ControlTypePropertyId);
        cache->AddProperty(UIA_AutomationIdPropertyId);
        cache->AddProperty(UIA_NamePropertyId);
    }
    return cache;
}

CONTROLTYPEID CachedControlType(IUIAutomationElement* element) {
    CONTROLTYPEID type = 0;
    element->get_CachedControlType(&type);
    return type;
}

template <typename F>
void ForEachChild(IUIAutomationTreeWalker* walker,
                  IUIAutomationCacheRequest* cache,
                  IUIAutomationElement* parent,
                  F&& callback) {
    ComPtr<IUIAutomationElement> child;
    walker->GetFirstChildElementBuildCache(parent, cache, &child);
    while (child) {
        if (!callback(child)) {
            return;
        }
        ComPtr<IUIAutomationElement> next;
        walker->GetNextSiblingElementBuildCache(child.Get(), cache, &next);
        child = next;
    }
}

// Breadth-first search with depth, node and time limits: the VS Code menu,
// for example, is at depth 11 and the Chromium/Electron tree can be
// huge.
ComPtr<IUIAutomationElement> FindMenuBar(IUIAutomationTreeWalker* walker,
                                         IUIAutomationCacheRequest* cache,
                                         IUIAutomationElement* root) {
    constexpr int kMaxDepth = 20;
    constexpr size_t kMaxNodes = 3000;
    constexpr ULONGLONG kTimeBudgetMs = 2500;
    ULONGLONG start = GetTickCount64();

    std::vector<std::pair<ComPtr<IUIAutomationElement>, int>> queue;
    queue.emplace_back(root, 0);
    for (size_t head = 0; head < queue.size(); head++) {
        if (head >= kMaxNodes || GetTickCount64() - start > kTimeBudgetMs) {
            break;
        }
        ComPtr<IUIAutomationElement> element = queue[head].first;
        int depth = queue[head].second;

        if (CachedControlType(element.Get()) == UIA_MenuBarControlTypeId) {
            BSTR id = nullptr;
            element->get_CachedAutomationId(&id);
            bool system = id && !wcscmp(id, L"SystemMenuBar");
            SysFreeString(id);
            if (!system) {
                return element;
            }
            continue;
        }

        if (depth >= kMaxDepth || queue.size() > kMaxNodes) {
            continue;
        }
        ForEachChild(walker, cache, element.Get(),
                     [&](const ComPtr<IUIAutomationElement>& child) {
                         queue.emplace_back(child, depth + 1);
                         return queue.size() <= kMaxNodes;
                     });
    }
    return nullptr;
}

void CollectMenuItems(IUIAutomationTreeWalker* walker,
                      IUIAutomationCacheRequest* cache,
                      IUIAutomationElement* parent,
                      int depth,
                      std::vector<std::wstring>& names) {
    ForEachChild(walker, cache, parent,
                 [&](const ComPtr<IUIAutomationElement>& child) {
                     if (CachedControlType(child.Get()) ==
                         UIA_MenuItemControlTypeId) {
                         BSTR name = nullptr;
                         child->get_CachedName(&name);
                         if (name && *name) {
                             names.push_back(name);
                             g_uiaItems.push_back(child);
                         }
                         SysFreeString(name);
                     } else if (depth < 2) {
                         // Some frameworks wrap the items in a panel.
                         CollectMenuItems(walker, cache, child.Get(), depth + 1,
                                          names);
                     }
                     return g_uiaItems.size() < 32;
                 });
}

void QueryUia(HWND hwnd, UINT generation) {
    g_uiaGeneration = generation;
    g_uiaItems.clear();

    auto* result = new UiaResult{generation, {}};
    ComPtr<IUIAutomationTreeWalker> walker;
    ComPtr<IUIAutomationCacheRequest> cache;
    ComPtr<IUIAutomationElement> root;
    if (g_uia && IsWindow(hwnd) &&
        SUCCEEDED(g_uia->get_ControlViewWalker(&walker)) && walker &&
        (cache = CreateUiaCache()) &&
        SUCCEEDED(g_uia->ElementFromHandleBuildCache(hwnd, cache.Get(),
                                                     &root)) &&
        root) {
        ComPtr<IUIAutomationElement> menuBar =
            FindMenuBar(walker.Get(), cache.Get(), root.Get());
        if (menuBar) {
            CollectMenuItems(walker.Get(), cache.Get(), menuBar.Get(), 0,
                             result->names);
        }
    }

    if (!PostMessageW(g_controller, WM_C_UIA, 0, (LPARAM)result)) {
        delete result;
    }
}

void InvokeUia(UINT generation, size_t index) {
    if (generation != g_uiaGeneration || index >= g_uiaItems.size()) {
        return;
    }
    IUIAutomationElement* element = g_uiaItems[index].Get();

    ComPtr<IUIAutomationExpandCollapsePattern> expand;
    if (SUCCEEDED(element->GetCurrentPatternAs(UIA_ExpandCollapsePatternId,
                                               IID_PPV_ARGS(&expand))) &&
        expand) {
        expand->Expand();
        return;
    }
    ComPtr<IUIAutomationInvokePattern> invoke;
    if (SUCCEEDED(element->GetCurrentPatternAs(UIA_InvokePatternId,
                                               IID_PPV_ARGS(&invoke))) &&
        invoke) {
        invoke->Invoke();
        return;
    }
    element->SetFocus();
}

DWORD WINAPI ThreadProc(void*) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    ComPtr<IUIAutomation> uia;
    if (FAILED(CoCreateInstance(CLSID_CUIAutomation8, nullptr,
                                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&uia)))) {
        CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(&uia));
    }
    g_uia = uia;
    ComPtr<IUIAutomation2> uia2;
    if (g_uia && SUCCEEDED(g_uia.As(&uia2))) {
        uia2->put_ConnectionTimeout(1500);
        uia2->put_TransactionTimeout(1500);
    }

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_readyEvent);

    SetTimer(nullptr, 0, 1500, nullptr);
    Poll(true);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        switch (msg.message) {
            case WM_TIMER:
                Poll(false);
                break;
            case WM_W_POLL_NOW:
                Poll(true);
                break;
            case WM_W_UIA_QUERY: {
                // Only the most recent query matters.
                MSG newer;
                while (PeekMessageW(&newer, nullptr, WM_W_UIA_QUERY,
                                    WM_W_UIA_QUERY, PM_REMOVE)) {
                    msg = newer;
                }
                QueryUia((HWND)msg.lParam, (UINT)msg.wParam);
                break;
            }
            case WM_W_UIA_INVOKE:
                InvokeUia((UINT)msg.wParam, (size_t)msg.lParam);
                break;
        }
    }

    g_uiaItems.clear();
    g_uia.Reset();
    g_deviceEnumerator.Reset();
    if (g_wlan) {
        WlanCloseHandle(g_wlan, nullptr);
        g_wlan = nullptr;
    }
    CoUninitialize();
    return 0;
}

void Start(HWND controller) {
    g_controller = controller;
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, &g_threadId);
    if (g_thread) {
        WaitForSingleObject(g_readyEvent, 5000);
    }
}

void Stop() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}

void Post(UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_thread) {
        PostThreadMessageW(g_threadId, msg, wParam, lParam);
    }
}

}  // namespace worker

////////////////////////////////////////////////////////////////////////////////
// State

enum class ItemKind {
    Start,
    App,
    Menu,
    UiaMenu,
    Standard,
    TrayMenu,
    Tray,
    Status,
    Clock
};
enum class StatusKind { Wifi, Bluetooth, Volume, Battery, Search, ControlCenter };

struct BarItem {
    ItemKind kind = ItemKind::Menu;
    std::wstring text;
    std::wstring tooltip;
    WCHAR glyph = 0;
    WCHAR mnemonic = 0;
    int pos = -1;
    bool enabled = true;
    bool dim = false;
    bool rightSide = false;
    StatusKind status = StatusKind::Wifi;
    HICON icon = nullptr;
    HWND trayHwnd = nullptr;
    UINT trayId = 0;
};

struct Bar {
    HWND hwnd = nullptr;
    HMONITOR monitor = nullptr;
    UINT dpi = 96;
    bool appbar = false;
    bool fullscreen = false;
    bool trackingMouse = false;
    bool inApplyPosition = false;
    int hot = -1;
    std::vector<RECT> rects;
    ComPtr<ID2D1HwndRenderTarget> target;
    ComPtr<ID2D1SolidColorBrush> brush;
    ComPtr<IDWriteTextFormat> fontNormal;
    ComPtr<IDWriteTextFormat> fontBold;
    ComPtr<IDWriteTextFormat> fontIcon;
    std::map<HICON, ComPtr<ID2D1Bitmap>> iconCache;
};

struct TrayIcon {
    HWND hwnd = nullptr;
    UINT id = 0;
    GUID guid{};
    bool hasGuid = false;
    UINT callback = 0;
    UINT version = 0;
    HICON icon = nullptr;
    std::wstring tip;
    bool hidden = false;
};

// Format of the WM_COPYDATA that Shell_NotifyIcon sends to Shell_TrayWnd.
// Handles are always 32 bits to work across architectures.
struct NotifyIconData32 {
    DWORD cbSize;
    DWORD hWnd;
    DWORD uID;
    DWORD uFlags;
    DWORD uCallbackMessage;
    DWORD hIcon;
    WCHAR szTip[128];
    DWORD dwState;
    DWORD dwStateMask;
    WCHAR szInfo[256];
    DWORD uVersion;
    WCHAR szInfoTitle[64];
    DWORD dwInfoFlags;
    GUID guidItem;
    DWORD hBalloonIcon;
};

struct TrayCopyData {
    DWORD signature;
    DWORD message;
    NotifyIconData32 nid;
};

struct TrayUpdate {
    DWORD message;
    UINT flags;
    TrayIcon data;
    DWORD state;
    DWORD stateMask;
};

struct Colors {
    D2D1_COLOR_F background;
    D2D1_COLOR_F text;
    D2D1_COLOR_F disabled;
    D2D1_COLOR_F hover;
    bool dark;
} g_colors;

HANDLE g_thread;
HANDLE g_readyEvent;
HANDLE g_mutex;

HWND g_controller;
HWND g_tooltip;
HWND g_trayWnd;
HWINEVENTHOOK g_winEventHook;
HHOOK g_msgFilterHook;
bool g_started;

ComPtr<ID2D1Factory> g_d2d;
ComPtr<IDWriteFactory> g_dwrite;
ComPtr<IWICImagingFactory> g_wic;
std::wstring g_iconFontFamily = L"Segoe MDL2 Assets";

std::vector<std::unique_ptr<Bar>> g_bars;
std::vector<BarItem> g_items;
std::vector<TrayIcon> g_tray;
worker::StatusInfo g_status;

HWND g_target;
DWORD g_targetPid;
std::wstring g_appName = L"Desktop";
std::wstring g_appPath;
bool g_targetExcluded;
bool g_targetHasAgent;

UINT g_uiaGeneration;
std::vector<std::wstring> g_uiaNames;
int g_uiaRetries;

// Open menu (synchronous: TrackPopupMenu in explorer; asynchronous: in the agent).
Bar* g_openBar;
int g_openIndex = -1;
bool g_openAsync;
ULONGLONG g_asyncTick;
DWORD g_asyncThread;
Bar* g_pendingBar;
int g_pendingIndex = -1;
bool g_itemsDirty;

Bar* g_tooltipBar;
int g_tooltipIndex = -1;

// Tray menu open: a right click on an item becomes a right click on the icon.
HMENU g_trayMenu;
int g_trayMenuRightClick = -1;

// Standard menu shortcuts, sent when the app gets the focus back.
std::vector<WORD> g_pendingKeys;
HWND g_pendingKeysTarget;

int Scale(const Bar& bar, int value) {
    return MulDiv(value, bar.dpi, 96);
}

Bar* BarFromHwnd(HWND hwnd) {
    for (auto& bar : g_bars) {
        if (bar->hwnd == hwnd) {
            return bar.get();
        }
    }
    return nullptr;
}

bool IsMenuKind(ItemKind kind) {
    return kind == ItemKind::Start || kind == ItemKind::App ||
           kind == ItemKind::Menu || kind == ItemKind::Standard ||
           kind == ItemKind::TrayMenu;
}

void InvalidateBars() {
    for (auto& bar : g_bars) {
        InvalidateRect(bar->hwnd, nullptr, FALSE);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Theme and effects

bool SystemUsesLightTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                 L"Personalize",
                 L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                 &size);
    return value != 0;
}

void UpdateColors() {
    bool light = g_settings.theme == Theme::Light ||
                 (g_settings.theme == Theme::Auto && SystemUsesLightTheme());
    float alpha = g_settings.opacity / 100.0f;
    if (light) {
        g_colors.background = D2D1::ColorF(0.96f, 0.96f, 0.96f, alpha);
        g_colors.text = D2D1::ColorF(0.08f, 0.08f, 0.08f);
        g_colors.disabled = D2D1::ColorF(0.08f, 0.08f, 0.08f, 0.45f);
        g_colors.hover = D2D1::ColorF(0, 0, 0, 0.10f);
        g_colors.dark = false;
    } else {
        g_colors.background = D2D1::ColorF(0.11f, 0.11f, 0.12f, alpha);
        g_colors.text = D2D1::ColorF(0.95f, 0.95f, 0.95f);
        g_colors.disabled = D2D1::ColorF(0.95f, 0.95f, 0.95f, 0.45f);
        g_colors.hover = D2D1::ColorF(1, 1, 1, 0.16f);
        g_colors.dark = true;
    }
}

struct AccentPolicy {
    INT accentState;
    INT accentFlags;
    INT gradientColor;
    INT animationId;
};

struct WindowCompositionAttribData {
    DWORD attrib;
    PVOID data;
    SIZE_T size;
};

constexpr DWORD WCA_ACCENT_POLICY = 19;
constexpr INT ACCENT_DISABLED = 0;
constexpr INT ACCENT_ENABLE_ACRYLICBLURBEHIND = 4;
constexpr DWORD kDwmUseImmersiveDarkMode = 20;
constexpr DWORD kDwmWindowCornerPreference = 33;
constexpr DWORD kDwmSystemBackdropType = 38;

void SetAccent(HWND hwnd, INT state, DWORD abgr) {
    using SetWindowCompositionAttribute_t =
        BOOL(WINAPI*)(HWND, WindowCompositionAttribData*);
    static auto setWindowCompositionAttribute =
        (SetWindowCompositionAttribute_t)GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (!setWindowCompositionAttribute) {
        return;
    }
    AccentPolicy policy{state, state ? 2 : 0, (INT)abgr, 0};
    WindowCompositionAttribData data{WCA_ACCENT_POLICY, &policy,
                                     sizeof(policy)};
    setWindowCompositionAttribute(hwnd, &data);
}

void ApplyBackdrop(Bar& bar) {
    HWND hwnd = bar.hwnd;

    // Makes DWM respect the window's alpha channel, without blur.
    DWM_BLURBEHIND blur{};
    blur.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    blur.fEnable = TRUE;
    blur.hRgnBlur = CreateRectRgn(0, 0, -1, -1);
    DwmEnableBlurBehindWindow(hwnd, &blur);
    DeleteObject(blur.hRgnBlur);

    BOOL dark = g_colors.dark;
    DwmSetWindowAttribute(hwnd, kDwmUseImmersiveDarkMode, &dark, sizeof(dark));
    UINT corner = 1;  // DWMWCP_DONOTROUND
    DwmSetWindowAttribute(hwnd, kDwmWindowCornerPreference, &corner,
                          sizeof(corner));

    UINT systemBackdrop = 1;  // DWMSBT_NONE
    switch (g_settings.backdrop) {
        case Backdrop::Mica:
            systemBackdrop = 2;
            break;
        case Backdrop::AcrylicSystem:
            systemBackdrop = 3;
            break;
        case Backdrop::MicaAlt:
            systemBackdrop = 4;
            break;
        default:
            break;
    }

    if (g_settings.backdrop == Backdrop::Acrylic) {
        D2D1_COLOR_F c = g_colors.background;
        DWORD abgr = (1u << 24) | ((DWORD)(c.b * 255) << 16) |
                     ((DWORD)(c.g * 255) << 8) | (DWORD)(c.r * 255);
        SetAccent(hwnd, ACCENT_ENABLE_ACRYLICBLURBEHIND, abgr);
    } else {
        SetAccent(hwnd, ACCENT_DISABLED, 0);
    }

    MARGINS margins{};
    if (systemBackdrop != 1) {
        margins = MARGINS{-1, -1, -1, -1};
    }
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    DwmSetWindowAttribute(hwnd, kDwmSystemBackdropType, &systemBackdrop,
                          sizeof(systemBackdrop));
    // DWM only draws the system effects on "active" windows.
    DefWindowProcW(hwnd, WM_NCACTIVATE, TRUE, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Direct2D / DirectWrite resources

void DetectIconFont() {
    ComPtr<IDWriteFontCollection> fonts;
    if (SUCCEEDED(g_dwrite->GetSystemFontCollection(&fonts, FALSE))) {
        UINT32 index;
        BOOL exists = FALSE;
        fonts->FindFamilyName(L"Segoe Fluent Icons", &index, &exists);
        if (exists) {
            g_iconFontFamily = L"Segoe Fluent Icons";
        }
    }
}

ComPtr<IDWriteTextFormat> CreateTextFormat(PCWSTR family,
                                           DWRITE_FONT_WEIGHT weight,
                                           float size) {
    ComPtr<IDWriteTextFormat> format;
    g_dwrite->CreateTextFormat(family, nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
                               DWRITE_FONT_STRETCH_NORMAL, size, L"", &format);
    if (format) {
        format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    return format;
}

void CreateFonts(Bar& bar) {
    float size = (float)Scale(bar, 13);
    bar.fontNormal = CreateTextFormat(L"Segoe UI Variable Text",
                                      DWRITE_FONT_WEIGHT_NORMAL, size);
    bar.fontBold = CreateTextFormat(L"Segoe UI Variable Text",
                                    DWRITE_FONT_WEIGHT_BOLD, size);
    bar.fontIcon = CreateTextFormat(g_iconFontFamily.c_str(),
                                    DWRITE_FONT_WEIGHT_NORMAL,
                                    (float)Scale(bar, 15));
}

float MeasureText(IDWriteTextFormat* format, const std::wstring& text) {
    if (!format || text.empty()) {
        return 0;
    }
    ComPtr<IDWriteTextLayout> layout;
    if (FAILED(g_dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(),
                                          format, 10000, 1000, &layout))) {
        return 0;
    }
    DWRITE_TEXT_METRICS metrics{};
    layout->GetMetrics(&metrics);
    return metrics.widthIncludingTrailingWhitespace;
}

bool EnsureRenderTarget(Bar& bar) {
    if (bar.target) {
        return true;
    }
    RECT client;
    GetClientRect(bar.hwnd, &client);
    auto props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96);
    auto hwndProps = D2D1::HwndRenderTargetProperties(
        bar.hwnd, D2D1::SizeU(std::max<LONG>(client.right, 1),
                              std::max<LONG>(client.bottom, 1)));
    if (FAILED(g_d2d->CreateHwndRenderTarget(props, hwndProps, &bar.target))) {
        return false;
    }
    bar.target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    bar.target->CreateSolidColorBrush(g_colors.text, &bar.brush);
    bar.iconCache.clear();
    return true;
}

ID2D1Bitmap* GetIconBitmap(Bar& bar, HICON icon) {
    auto it = bar.iconCache.find(icon);
    if (it != bar.iconCache.end()) {
        return it->second.Get();
    }

    ComPtr<ID2D1Bitmap> bitmap;
    ComPtr<IWICBitmap> wicBitmap;
    ComPtr<IWICFormatConverter> converter;
    if (g_wic && SUCCEEDED(g_wic->CreateBitmapFromHICON(icon, &wicBitmap)) &&
        SUCCEEDED(g_wic->CreateFormatConverter(&converter)) &&
        SUCCEEDED(converter->Initialize(
            wicBitmap.Get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0,
            WICBitmapPaletteTypeMedianCut))) {
        bar.target->CreateBitmapFromWicBitmap(converter.Get(), nullptr,
                                              &bitmap);
    }
    bar.iconCache[icon] = bitmap;
    return bitmap.Get();
}

////////////////////////////////////////////////////////////////////////////////
// Target window

enum class WindowType { Ignore, Desktop, App };

WindowType ClassifyWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || BarFromHwnd(hwnd) || hwnd == g_tooltip) {
        return WindowType::Ignore;
    }

    WCHAR cls[128];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return WindowType::Ignore;
    }

    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW")) {
        return WindowType::Desktop;
    }

    static const PCWSTR kIgnored[] = {
        L"#32768",  // popup menus
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"NotifyIconOverflowWindow",
        L"TopLevelWindowForOverflowXamlIsland",
        L"Windows.UI.Core.CoreWindow",  // Start, search, action center
        L"XamlExplorerHostIslandWindow",
        L"ForegroundStaging",
        L"MultitaskingViewFrame",
        L"TaskSwitcherWnd",
        L"Xaml_WindowedPopupClass",
        L"tooltips_class32",
        L"ControlCenterWindow",
    };
    for (PCWSTR ignored : kIgnored) {
        if (!wcscmp(cls, ignored)) {
            return WindowType::Ignore;
        }
    }

    return WindowType::App;
}

std::wstring GetFileDescription(const std::wstring& path) {
    DWORD handle;
    DWORD size = GetFileVersionInfoSizeW(path.c_str(), &handle);
    if (!size) {
        return {};
    }

    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoW(path.c_str(), 0, size, data.data())) {
        return {};
    }

    struct LangCodePage {
        WORD language;
        WORD codePage;
    }* translations;
    UINT len;
    if (!VerQueryValueW(data.data(), L"\\VarFileInfo\\Translation",
                        (void**)&translations, &len) ||
        len < sizeof(LangCodePage)) {
        return {};
    }

    WCHAR subBlock[64];
    swprintf(subBlock, ARRAYSIZE(subBlock),
             L"\\StringFileInfo\\%04x%04x\\FileDescription",
             translations[0].language, translations[0].codePage);

    WCHAR* description;
    if (VerQueryValueW(data.data(), subBlock, (void**)&description, &len) &&
        len > 1) {
        return description;
    }
    return {};
}

void UpdateAppInfo(HWND hwnd) {
    g_appPath.clear();
    g_targetPid = 0;
    GetWindowThreadProcessId(hwnd, &g_targetPid);

    if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                     g_targetPid)) {
        WCHAR path[MAX_PATH];
        DWORD size = ARRAYSIZE(path);
        if (QueryFullProcessImageNameW(process, 0, path, &size)) {
            g_appPath = path;
        }
        CloseHandle(process);
    }

    if (IsWindowOfClass(hwnd, L"ApplicationFrameWindow")) {
        // UWP apps: the process is ApplicationFrameHost, the title is the name.
        WCHAR title[256];
        GetWindowTextW(hwnd, title, ARRAYSIZE(title));
        g_appName = title;
        return;
    }

    g_appName = g_appPath.empty() ? L"" : GetFileDescription(g_appPath);
    if (g_appName.empty() && !g_appPath.empty()) {
        std::wstring name = FileNameFromPath(g_appPath);
        size_t dot = name.find_last_of(L'.');
        g_appName = dot == std::wstring::npos ? name : name.substr(0, dot);
    }
    if (g_appName.empty()) {
        WCHAR title[256];
        GetWindowTextW(hwnd, title, ARRAYSIZE(title));
        g_appName = title;
    }
}

bool AgentPresent(DWORD pid) {
    HANDLE event = OpenEventW(SYNCHRONIZE, FALSE, AgentObjectName(pid).c_str());
    if (event) {
        CloseHandle(event);
        return true;
    }
    return false;
}

// The menu may be hidden by the agent; in that case it's in a property.
HMENU GetTargetMenu() {
    if (!g_target || g_targetExcluded) {
        return nullptr;
    }
    HMENU menu = GetMenu(g_target);
    if (!menu) {
        menu = (HMENU)GetPropW(g_target, kPropMenu);
    }
    return menu && IsMenu(menu) ? menu : nullptr;
}

bool ShouldQueryUia() {
    if (!g_settings.modernMenus || !g_target || g_targetExcluded ||
        g_targetPid == GetCurrentProcessId() || GetTargetMenu()) {
        return false;
    }
    // Browsers don't have a menu bar, and querying their accessibility tree
    // is expensive.
    static const PCWSTR kSkip[] = {
        L"chrome.exe",  L"msedge.exe",  L"firefox.exe",
        L"brave.exe",   L"opera.exe",   L"vivaldi.exe",
    };
    std::wstring name = ToLower(FileNameFromPath(g_appPath));
    for (PCWSTR skip : kSkip) {
        if (name == skip) {
            return false;
        }
    }
    return true;
}

void RequestUia() {
    g_uiaGeneration++;
    g_uiaNames.clear();
    if (ShouldQueryUia()) {
        worker::Post(worker::WM_W_UIA_QUERY, g_uiaGeneration,
                     (LPARAM)g_target);
    }
}

void MaybeHideNativeMenu() {
    if (g_settings.hideNativeMenu && g_target && g_targetHasAgent &&
        GetMenu(g_target)) {
        PostMessageW(g_target, g_msgHide, 1, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Bar items

void ParseMenuText(const WCHAR* raw, std::wstring& text, WCHAR& mnemonic) {
    text.clear();
    mnemonic = 0;
    for (const WCHAR* p = raw; *p; p++) {
        if (*p == L'\t') {
            break;
        }
        if (*p == L'&') {
            if (p[1] == L'&') {
                text += L'&';
                p++;
            } else if (p[1] && !mnemonic) {
                mnemonic = p[1];
            }
            continue;
        }
        text += *p;
    }
}

std::wstring FormatDate(DWORD flags, PCWSTR format) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    WCHAR buffer[128];
    if (GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, flags, &st, format, buffer,
                        ARRAYSIZE(buffer), nullptr)) {
        return buffer;
    }
    return {};
}

std::wstring GetClockText() {
    std::wstring result;
    if (!g_settings.dateFormat.empty()) {
        result = FormatDate(0, g_settings.dateFormat.c_str());
    }
    if (!g_settings.timeFormat.empty()) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        WCHAR buffer[128];
        if (GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st,
                            g_settings.timeFormat.c_str(), buffer,
                            ARRAYSIZE(buffer))) {
            if (!result.empty()) {
                result += L"   ";
            }
            result += buffer;
        }
    }
    return result;
}

// Standard menus for apps without a menu bar: each command sends the common
// shortcut or performs an action on the window.
enum class StandardAction { Keys, Close, Minimize, Maximize, Topmost, WebHelp };

struct StandardCommand {
    PCWSTR text;  // nullptr = separator
    PCWSTR shortcut;
    std::vector<WORD> keys;
    StandardAction action = StandardAction::Keys;
};

struct StandardMenu {
    PCWSTR title;
    std::vector<StandardCommand> commands;
};

const std::vector<StandardMenu>& StandardMenus() {
    static const std::vector<StandardMenu> menus = {
        {L"File",
         {
             {L"New", L"Ctrl+N", {VK_CONTROL, 'N'}},
             {L"New Window", L"Ctrl+Shift+N", {VK_CONTROL, VK_SHIFT, 'N'}},
             {L"Open...", L"Ctrl+O", {VK_CONTROL, 'O'}},
             {nullptr, nullptr, {}},
             {L"Save", L"Ctrl+S", {VK_CONTROL, 'S'}},
             {L"Save As...", L"Ctrl+Shift+S", {VK_CONTROL, VK_SHIFT, 'S'}},
             {nullptr, nullptr, {}},
             {L"Print...", L"Ctrl+P", {VK_CONTROL, 'P'}},
             {nullptr, nullptr, {}},
             {L"Close Tab", L"Ctrl+W", {VK_CONTROL, 'W'}},
             {L"Close Window", L"Alt+F4", {}, StandardAction::Close},
         }},
        {L"Edit",
         {
             {L"Undo", L"Ctrl+Z", {VK_CONTROL, 'Z'}},
             {L"Redo", L"Ctrl+Y", {VK_CONTROL, 'Y'}},
             {nullptr, nullptr, {}},
             {L"Cut", L"Ctrl+X", {VK_CONTROL, 'X'}},
             {L"Copy", L"Ctrl+C", {VK_CONTROL, 'C'}},
             {L"Paste", L"Ctrl+V", {VK_CONTROL, 'V'}},
             {L"Select All", L"Ctrl+A", {VK_CONTROL, 'A'}},
             {nullptr, nullptr, {}},
             {L"Find...", L"Ctrl+F", {VK_CONTROL, 'F'}},
             {L"Replace...", L"Ctrl+H", {VK_CONTROL, 'H'}},
             {nullptr, nullptr, {}},
             {L"Emoji & Symbols", L"Win+.", {VK_LWIN, VK_OEM_PERIOD}},
         }},
        {L"View",
         {
             {L"Zoom In", L"Ctrl++", {VK_CONTROL, VK_OEM_PLUS}},
             {L"Zoom Out", L"Ctrl+-", {VK_CONTROL, VK_OEM_MINUS}},
             {L"Actual Size", L"Ctrl+0", {VK_CONTROL, '0'}},
             {nullptr, nullptr, {}},
             {L"Refresh", L"F5", {VK_F5}},
             {L"Enter Full Screen", L"F11", {VK_F11}},
         }},
        {L"Window",
         {
             {L"Minimize", L"", {}, StandardAction::Minimize},
             {L"Maximize / Restore", L"", {}, StandardAction::Maximize},
             {nullptr, nullptr, {}},
             {L"Snap Left", L"Win+←", {VK_LWIN, VK_LEFT}},
             {L"Snap Right", L"Win+→", {VK_LWIN, VK_RIGHT}},
             {L"Move to Other Monitor", L"Win+Shift+→",
              {VK_LWIN, VK_SHIFT, VK_RIGHT}},
             {nullptr, nullptr, {}},
             {L"Always on Top", L"", {}, StandardAction::Topmost},
             {nullptr, nullptr, {}},
             {L"Task View", L"Win+Tab", {VK_LWIN, VK_TAB}},
         }},
        {L"Help",
         {
             {L"App Help", L"F1", {VK_F1}},
             {L"Search Help on the Web", L"", {}, StandardAction::WebHelp},
         }},
    };
    return menus;
}

void AddMenuItems(std::vector<BarItem>& items) {
    HMENU menu = GetTargetMenu();
    if (menu) {
        bool windowEnabled = IsWindowEnabled(g_target);
        int count = std::min(GetMenuItemCount(menu), 64);
        for (int i = 0; i < count; i++) {
            MENUITEMINFOW mii{sizeof(mii)};
            mii.fMask = MIIM_FTYPE | MIIM_STATE;
            if (!GetMenuItemInfoW(menu, i, TRUE, &mii) ||
                (mii.fType & (MFT_SEPARATOR | MFT_OWNERDRAW | MFT_BITMAP))) {
                continue;
            }

            WCHAR buffer[256];
            if (GetMenuStringW(menu, i, buffer, ARRAYSIZE(buffer),
                               MF_BYPOSITION) <= 0) {
                continue;
            }

            BarItem item;
            item.kind = ItemKind::Menu;
            ParseMenuText(buffer, item.text, item.mnemonic);
            if (item.text.empty()) {
                continue;
            }
            item.pos = i;
            item.enabled = windowEnabled && !(mii.fState & MFS_DISABLED);
            items.push_back(item);
        }
        return;
    }

    for (size_t i = 0; i < g_uiaNames.size(); i++) {
        BarItem item;
        item.kind = ItemKind::UiaMenu;
        item.text = g_uiaNames[i];
        item.pos = (int)i;
        items.push_back(item);
    }

    if (g_uiaNames.empty() && g_settings.standardMenus && g_target &&
        !g_targetExcluded) {
        const auto& menus = StandardMenus();
        for (size_t i = 0; i < menus.size(); i++) {
            BarItem item;
            item.kind = ItemKind::Standard;
            item.text = menus[i].title;
            item.pos = (int)i;
            item.enabled = IsWindowEnabled(g_target);
            items.push_back(item);
        }
    }
}

void AddStatusItem(std::vector<BarItem>& items,
                   StatusKind kind,
                   WCHAR glyph,
                   std::wstring text,
                   std::wstring tooltip,
                   bool dim = false) {
    BarItem item;
    item.kind = ItemKind::Status;
    item.status = kind;
    item.glyph = glyph;
    item.text = std::move(text);
    item.tooltip = std::move(tooltip);
    item.dim = dim;
    item.rightSide = true;
    items.push_back(item);
}

bool IsTrayIconVisible(const TrayIcon& icon) {
    return !icon.hidden && icon.icon && IsWindow(icon.hwnd);
}

void AddRightItems(std::vector<BarItem>& items) {
    if (g_settings.trayMenu &&
        std::any_of(g_tray.begin(), g_tray.end(), IsTrayIconVisible)) {
        BarItem item;
        item.kind = ItemKind::TrayMenu;
        item.glyph = 0xE70D;  // ChevronDown
        item.tooltip = L"Tray icons";
        item.rightSide = true;
        items.push_back(item);
    }

    if (g_settings.trayInline) {
        for (const TrayIcon& icon : g_tray) {
            if (icon.hidden || !icon.icon) {
                continue;
            }
            BarItem item;
            item.kind = ItemKind::Tray;
            item.icon = icon.icon;
            item.tooltip = icon.tip;
            item.trayHwnd = icon.hwnd;
            item.trayId = icon.id;
            item.rightSide = true;
            items.push_back(item);
        }
    }

    const worker::StatusInfo& s = g_status;

    if (g_settings.bluetooth && s.hasBluetooth) {
        AddStatusItem(items, StatusKind::Bluetooth, 0xE702, L"",
                      s.bluetoothOn ? L"Bluetooth: on"
                                    : L"Bluetooth: off",
                      !s.bluetoothOn);
    }

    if (g_settings.wifi) {
        if (s.wifiConnected) {
            WCHAR glyph = 0xE701;
            if (s.wifiSignal >= 0) {
                glyph = s.wifiSignal >= 75   ? 0xE701
                        : s.wifiSignal >= 50 ? 0xE873
                        : s.wifiSignal >= 25 ? 0xE872
                                             : 0xE871;
            }
            std::wstring tip = L"Wi-Fi";
            if (!s.wifiSsid.empty()) {
                tip += L": " + s.wifiSsid;
            }
            if (s.wifiSignal >= 0) {
                tip += L" (" + std::to_wstring(s.wifiSignal) + L"%)";
            }
            AddStatusItem(items, StatusKind::Wifi, glyph, L"", tip);
        } else if (s.ethernetConnected) {
            AddStatusItem(items, StatusKind::Wifi, 0xE839, L"", L"Ethernet");
        } else if (s.hasWifiRadio && !s.wifiRadioOn) {
            AddStatusItem(items, StatusKind::Wifi, 0xE701, L"",
                          L"Wi-Fi off", true);
        } else {
            AddStatusItem(items, StatusKind::Wifi, 0xE701, L"",
                          L"Not connected", true);
        }
    }

    if (g_settings.volume && s.hasVolume) {
        WCHAR glyph = s.muted          ? 0xE74F
                      : s.volume == 0  ? 0xE992
                      : s.volume < 34  ? 0xE993
                      : s.volume < 67  ? 0xE994
                                       : 0xE995;
        AddStatusItem(items, StatusKind::Volume, glyph, L"",
                      s.muted ? L"Volume: muted"
                              : L"Volume: " + std::to_wstring(s.volume) + L"%");
    }

    if (g_settings.battery && s.hasBattery) {
        int level = s.batteryPercent < 0
                        ? 10
                        : std::clamp((s.batteryPercent + 5) / 10, 0, 10);
        WCHAR glyph = (WCHAR)((s.charging ? 0xEBAB : 0xEBA0) + level);
        std::wstring percent =
            s.batteryPercent < 0 ? L"" : std::to_wstring(s.batteryPercent) + L"%";
        std::wstring tip = L"Battery";
        if (!percent.empty()) {
            tip += L": " + percent;
        }
        if (s.charging) {
            tip += L" (charging)";
        }
        AddStatusItem(items, StatusKind::Battery, glyph,
                      g_settings.batteryPercent ? percent : L"", tip);
    }

    if (g_settings.search) {
        AddStatusItem(items, StatusKind::Search, 0xE721, L"", L"Search");
    }

    if (g_settings.controlCenter) {
        AddStatusItem(items, StatusKind::ControlCenter, 0xE91C, L"",
                      L"Quick settings");
    }

    if (g_settings.showClock) {
        BarItem item;
        item.kind = ItemKind::Clock;
        item.text = GetClockText();
        item.tooltip = FormatDate(DATE_LONGDATE, nullptr);
        item.rightSide = true;
        items.push_back(item);
    }
}

float ItemContentWidth(Bar& bar, const BarItem& item, float* glyphWidth) {
    float glyph = item.glyph
                      ? MeasureText(bar.fontIcon.Get(), std::wstring(1, item.glyph))
                      : 0;
    float text = MeasureText(
        item.kind == ItemKind::App ? bar.fontBold.Get() : bar.fontNormal.Get(),
        item.text);
    if (glyphWidth) {
        *glyphWidth = glyph;
    }
    float gap = glyph > 0 && text > 0 ? (float)Scale(bar, 5) : 0;
    return glyph + gap + text;
}

void LayoutBar(Bar& bar) {
    RECT client;
    GetClientRect(bar.hwnd, &client);
    int inset = Scale(bar, 3);
    int top = inset;
    int bottom = client.bottom - inset;

    bar.rects.assign(g_items.size(), RECT{});

    auto itemWidth = [&](const BarItem& item) {
        switch (item.kind) {
            case ItemKind::Start:
                return Scale(bar, 13) + 2 * Scale(bar, 10);
            case ItemKind::Tray:
                return Scale(bar, 16) + 2 * Scale(bar, 6);
            case ItemKind::Status:
            case ItemKind::TrayMenu:
                return (int)ItemContentWidth(bar, item, nullptr) +
                       2 * Scale(bar, 7);
            default:
                return (int)ItemContentWidth(bar, item, nullptr) +
                       2 * Scale(bar, 10);
        }
    };

    int right = client.right - Scale(bar, 8);
    for (int i = (int)g_items.size() - 1; i >= 0; i--) {
        if (!g_items[i].rightSide) {
            continue;
        }
        int width = itemWidth(g_items[i]);
        bar.rects[i] = RECT{right - width, top, right, bottom};
        right -= width;
    }

    // Items on the left that don't fit disappear, like in macOS.
    int x = Scale(bar, 8);
    int limit = right - Scale(bar, 16);
    for (size_t i = 0; i < g_items.size(); i++) {
        if (g_items[i].rightSide) {
            continue;
        }
        int width = itemWidth(g_items[i]);
        if (x + width > limit && g_items[i].kind != ItemKind::Start) {
            continue;
        }
        bar.rects[i] = RECT{x, top, x + width, bottom};
        x += width;
    }
}

void PruneIconCaches() {
    for (auto& bar : g_bars) {
        for (auto it = bar->iconCache.begin(); it != bar->iconCache.end();) {
            bool used = std::any_of(g_tray.begin(), g_tray.end(),
                                    [&](const TrayIcon& t) {
                                        return t.icon == it->first;
                                    });
            it = used ? std::next(it) : bar->iconCache.erase(it);
        }
    }
}

void RebuildItems() {
    if (g_openIndex >= 0) {
        g_itemsDirty = true;
        InvalidateBars();
        return;
    }
    g_itemsDirty = false;

    std::vector<BarItem> items;

    BarItem start;
    start.kind = ItemKind::Start;
    items.push_back(start);

    BarItem app;
    app.kind = ItemKind::App;
    app.text = g_appName;
    app.enabled = g_target != nullptr;
    items.push_back(app);

    AddMenuItems(items);
    AddRightItems(items);

    g_items = std::move(items);
    PruneIconCaches();
    for (auto& bar : g_bars) {
        LayoutBar(*bar);
        if (bar->hot >= (int)g_items.size()) {
            bar->hot = -1;
        }
        InvalidateRect(bar->hwnd, nullptr, FALSE);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Bar positions

void UpdateBarDpi(Bar& bar) {
    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(bar.monitor, MDT_EFFECTIVE_DPI, &dpiX,
                                   &dpiY)) &&
        dpiX != bar.dpi) {
        bar.dpi = dpiX;
        CreateFonts(bar);
    }
}

void ApplyPosition(Bar& bar) {
    if (bar.inApplyPosition) {
        return;
    }
    bar.inApplyPosition = true;

    MONITORINFO mi{sizeof(mi)};
    if (GetMonitorInfoW(bar.monitor, &mi)) {
        UpdateBarDpi(bar);
        int height = Scale(bar, g_settings.height);
        RECT rc{mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right,
                mi.rcMonitor.top + height};

        APPBARDATA abd{sizeof(abd)};
        abd.hWnd = bar.hwnd;
        if (g_settings.reserveSpace) {
            if (!bar.appbar) {
                abd.uCallbackMessage = WM_BAR_APPBAR;
                bar.appbar = SHAppBarMessage(ABM_NEW, &abd) != 0;
            }
            if (bar.appbar) {
                abd.uEdge = ABE_TOP;
                abd.rc = rc;
                SHAppBarMessage(ABM_QUERYPOS, &abd);
                abd.rc.bottom = abd.rc.top + height;
                SHAppBarMessage(ABM_SETPOS, &abd);
                rc = abd.rc;
            }
        } else if (bar.appbar) {
            SHAppBarMessage(ABM_REMOVE, &abd);
            bar.appbar = false;
        }

        UINT flags = SWP_NOACTIVATE | (bar.fullscreen ? 0 : SWP_SHOWWINDOW);
        SetWindowPos(bar.hwnd, HWND_TOPMOST, rc.left, rc.top,
                     rc.right - rc.left, rc.bottom - rc.top, flags);
    }

    bar.inApplyPosition = false;
}

bool IsFullscreenOn(HWND hwnd, const Bar& bar) {
    if (ClassifyWindow(hwnd) != WindowType::App ||
        MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) != bar.monitor) {
        return false;
    }
    if ((GetWindowLongW(hwnd, GWL_STYLE) & WS_CAPTION) == WS_CAPTION) {
        return false;
    }
    MONITORINFO mi{sizeof(mi)};
    RECT rc;
    if (!GetMonitorInfoW(bar.monitor, &mi) || !GetWindowRect(hwnd, &rc)) {
        return false;
    }
    return rc.left <= mi.rcMonitor.left && rc.top <= mi.rcMonitor.top &&
           rc.right >= mi.rcMonitor.right && rc.bottom >= mi.rcMonitor.bottom;
}

void UpdateFullscreen() {
    HWND foreground = GetForegroundWindow();
    WindowType type = ClassifyWindow(foreground);
    if (type == WindowType::Ignore) {
        return;
    }
    for (auto& bar : g_bars) {
        bool fullscreen = IsFullscreenOn(foreground, *bar);
        if (fullscreen == bar->fullscreen) {
            continue;
        }
        bar->fullscreen = fullscreen;
        if (fullscreen) {
            ShowWindow(bar->hwnd, SW_HIDE);
        } else {
            SetWindowPos(bar->hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_SHOWWINDOW);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Focus

void OnForeground(HWND hwnd) {
    WindowType type = ClassifyWindow(hwnd);
    if (type == WindowType::Ignore) {
        return;
    }

    HWND target = nullptr;
    if (type == WindowType::App) {
        // Dialogs and secondary windows show the menu of the owner window.
        target = hwnd;
        HWND root = GetAncestor(hwnd, GA_ROOTOWNER);
        if (root && root != hwnd && IsWindowVisible(root) &&
            ClassifyWindow(root) == WindowType::App) {
            target = root;
        }
    }

    UpdateFullscreen();

    bool changed = target != g_target;
    g_target = target;
    if (target) {
        UpdateAppInfo(target);
        g_targetExcluded = IsExcludedPath(g_settings.excluded, g_appPath);
        g_targetHasAgent = !g_targetExcluded && AgentPresent(g_targetPid);
    } else {
        g_appName = L"Desktop";
        g_appPath.clear();
        g_targetPid = 0;
        g_targetExcluded = false;
        g_targetHasAgent = false;
    }

    MaybeHideNativeMenu();
    if (changed) {
        g_uiaRetries = 0;
        RequestUia();
    }
    RebuildItems();
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG, LONG,
                           DWORD, DWORD) {
    if (event == EVENT_SYSTEM_FOREGROUND) {
        OnForeground(hwnd);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Tray

void ApplyTrayUpdate(TrayUpdate* update) {
    const TrayIcon& data = update->data;
    auto it = std::find_if(g_tray.begin(), g_tray.end(), [&](const TrayIcon& t) {
        if (data.hasGuid || t.hasGuid) {
            return data.hasGuid && t.hasGuid &&
                   IsEqualGUID(data.guid, t.guid);
        }
        return t.hwnd == data.hwnd && t.id == data.id;
    });

    switch (update->message) {
        case NIM_ADD:
        case NIM_MODIFY: {
            if (it == g_tray.end()) {
                TrayIcon icon;
                icon.hwnd = data.hwnd;
                icon.id = data.id;
                icon.guid = data.guid;
                icon.hasGuid = data.hasGuid;
                g_tray.push_back(icon);
                it = g_tray.end() - 1;
            }
            it->hwnd = data.hwnd;
            it->id = data.id;
            if (update->flags & NIF_MESSAGE) {
                it->callback = data.callback;
            }
            if (update->flags & NIF_ICON) {
                if (it->icon) {
                    DestroyIcon(it->icon);
                }
                it->icon = data.icon;
                update->data.icon = nullptr;
            }
            if (update->flags & NIF_TIP) {
                it->tip = data.tip;
            }
            if ((update->flags & NIF_STATE) &&
                (update->stateMask & NIS_HIDDEN)) {
                it->hidden = (update->state & NIS_HIDDEN) != 0;
            }
            break;
        }
        case NIM_DELETE:
            if (it != g_tray.end()) {
                if (it->icon) {
                    DestroyIcon(it->icon);
                }
                g_tray.erase(it);
            }
            break;
        case NIM_SETVERSION:
            if (it != g_tray.end()) {
                it->version = data.version;
            }
            break;
    }

    if (update->data.icon) {
        DestroyIcon(update->data.icon);
    }
}

void PruneDeadTrayIcons() {
    bool removed = false;
    for (auto it = g_tray.begin(); it != g_tray.end();) {
        if (!IsWindow(it->hwnd)) {
            if (it->icon) {
                DestroyIcon(it->icon);
            }
            it = g_tray.erase(it);
            removed = true;
        } else {
            ++it;
        }
    }
    if (removed) {
        RebuildItems();
    }
}

void ClearTray() {
    for (TrayIcon& icon : g_tray) {
        if (icon.icon) {
            DestroyIcon(icon.icon);
        }
    }
    g_tray.clear();
}

// Runs on the taskbar thread: only copies the data and forwards it.
LRESULT CALLBACK TraySubclassProc(HWND hwnd,
                                  UINT msg,
                                  WPARAM wParam,
                                  LPARAM lParam,
                                  DWORD_PTR) {
    if (msg == WM_COPYDATA) {
        auto* cds = (COPYDATASTRUCT*)lParam;
        if (cds && cds->dwData == 1 && cds->lpData &&
            cds->cbData >= offsetof(TrayCopyData, nid.szTip)) {
            TrayCopyData data{};
            memcpy(&data, cds->lpData,
                   std::min<size_t>(cds->cbData, sizeof(data)));
            HWND controller = g_controller;
            if (data.signature == 0x34753423 && controller) {
                Wh_Log(L"Tray: NIM %u, window %08X, id %u, flags %X",
                       data.message, data.nid.hWnd, data.nid.uID,
                       data.nid.uFlags);
                auto* update = new TrayUpdate{};
                update->message = data.message;
                update->flags = data.nid.uFlags;
                update->state = data.nid.dwState;
                update->stateMask = data.nid.dwStateMask;
                update->data.hwnd = (HWND)(ULONG_PTR)data.nid.hWnd;
                update->data.id = data.nid.uID;
                update->data.callback = data.nid.uCallbackMessage;
                update->data.version = data.nid.uVersion;
                if ((data.nid.uFlags & NIF_GUID) &&
                    cds->cbData >= offsetof(TrayCopyData, nid.hBalloonIcon)) {
                    update->data.hasGuid = true;
                    update->data.guid = data.nid.guidItem;
                }
                if ((data.nid.uFlags & NIF_ICON) && data.nid.hIcon) {
                    update->data.icon =
                        CopyIcon((HICON)(ULONG_PTR)data.nid.hIcon);
                }
                data.nid.szTip[ARRAYSIZE(data.nid.szTip) - 1] = L'\0';
                update->data.tip = data.nid.szTip;

                if (!PostMessageW(controller, WM_C_TRAY, 0, (LPARAM)update)) {
                    if (update->data.icon) {
                        DestroyIcon(update->data.icon);
                    }
                    delete update;
                }
            }
        }
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

void InstallTrayHook() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (g_trayWnd && g_trayWnd == tray && IsWindow(g_trayWnd)) {
        return;
    }
    // The taskbar was recreated (or hadn't been seen yet).
    g_trayWnd = nullptr;
    if (tray &&
        WindhawkUtils::SetWindowSubclassFromAnyThread(tray, TraySubclassProc,
                                                      0)) {
        g_trayWnd = tray;
    }
    static HWND lastLogged;
    if (tray != lastLogged) {
        lastLogged = tray;
        Wh_Log(L"Bandeja: Shell_TrayWnd %p, subclasse %s", tray,
               g_trayWnd ? L"instalada" : L"falhou");
    }
}

void RemoveTrayHook() {
    if (g_trayWnd) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_trayWnd,
                                                         TraySubclassProc);
        g_trayWnd = nullptr;
    }
}

// Asks apps to register their icons again, so the bar sees them.
void RequestTrayIcons() {
    PostMessageW(HWND_BROADCAST, RegisterWindowMessageW(L"TaskbarCreated"), 0,
                 0);
}

void SendTrayEvent(const TrayIcon& icon, UINT event, POINT anchor) {
    if (!icon.callback) {
        return;
    }
    if (icon.version >= 4) {
        PostMessageW(icon.hwnd, icon.callback, MAKEWPARAM(anchor.x, anchor.y),
                     MAKELPARAM(event, icon.id));
    } else {
        PostMessageW(icon.hwnd, icon.callback, icon.id, event);
    }
}

void ClickTrayIcon(HWND hwnd, UINT id, UINT button, bool doubleClick,
                   POINT anchor) {
    auto it = std::find_if(g_tray.begin(), g_tray.end(), [&](const TrayIcon& t) {
        return t.hwnd == hwnd && t.id == id;
    });
    if (it == g_tray.end() || !IsWindow(it->hwnd)) {
        return;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(it->hwnd, &pid);
    AllowSetForegroundWindow(pid);

    if (doubleClick) {
        SendTrayEvent(*it, WM_LBUTTONDBLCLK, anchor);
    } else if (button == WM_LBUTTONUP) {
        SendTrayEvent(*it, WM_LBUTTONDOWN, anchor);
        SendTrayEvent(*it, WM_LBUTTONUP, anchor);
        if (it->version >= 3) {
            SendTrayEvent(*it, NIN_SELECT, anchor);
        }
    } else {
        SendTrayEvent(*it, WM_RBUTTONDOWN, anchor);
        SendTrayEvent(*it, WM_RBUTTONUP, anchor);
        if (it->version >= 3) {
            SendTrayEvent(*it, WM_CONTEXTMENU, anchor);
        }
    }
}

void ClickTray(Bar& bar, int index, UINT button, bool doubleClick) {
    const BarItem& item = g_items[index];
    RECT rc = bar.rects[index];
    POINT anchor{(rc.left + rc.right) / 2, rc.bottom};
    ClientToScreen(bar.hwnd, &anchor);
    ClickTrayIcon(item.trayHwnd, item.trayId, button, doubleClick, anchor);
}

// Icon name for the tray menu: the first line of the tooltip or, without a
// tooltip, the description of the program that owns the icon.
std::wstring TrayIconTitle(const TrayIcon& icon) {
    std::wstring title = icon.tip.substr(0, icon.tip.find_first_of(L"\r\n"));
    if (title.empty()) {
        DWORD pid = 0;
        GetWindowThreadProcessId(icon.hwnd, &pid);
        if (HANDLE process =
                OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
            WCHAR path[MAX_PATH];
            DWORD size = ARRAYSIZE(path);
            if (QueryFullProcessImageNameW(process, 0, path, &size)) {
                title = GetFileDescription(path);
                if (title.empty()) {
                    title = FileNameFromPath(path);
                }
            }
            CloseHandle(process);
        }
    }
    if (title.size() > 60) {
        title = title.substr(0, 57) + L"...";
    }
    // "&" would be read as a mnemonic.
    std::wstring escaped;
    for (WCHAR c : title) {
        escaped += c;
        if (c == L'&') {
            escaped += L'&';
        }
    }
    return escaped.empty() ? L"(no name)" : escaped;
}

// 32 bpp bitmap with alpha, which Windows menus draw next to the text.
HBITMAP IconToMenuBitmap(HICON icon, int size) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bitmap =
        CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(dc);
        return nullptr;
    }
    HGDIOBJ old = SelectObject(dc, bitmap);
    DrawIconEx(dc, 0, 0, icon, size, size, 0, nullptr, DI_NORMAL);
    SelectObject(dc, old);
    DeleteDC(dc);

    // Old icons without an alpha channel would be invisible.
    auto* pixels = (DWORD*)bits;
    int count = size * size;
    if (std::none_of(pixels, pixels + count,
                     [](DWORD p) { return (p & 0xFF000000) != 0; })) {
        for (int i = 0; i < count; i++) {
            if (pixels[i]) {
                pixels[i] |= 0xFF000000;
            }
        }
    }
    return bitmap;
}

////////////////////////////////////////////////////////////////////////////////
// Actions

void RunShell(PCWSTR file, PCWSTR params = nullptr) {
    ShellExecuteW(nullptr, L"open", file, params, nullptr, SW_SHOWNORMAL);
}

bool IsExtendedKey(WORD key) {
    switch (key) {
        case VK_LEFT:
        case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
        case VK_LWIN:
        case VK_HOME:
        case VK_END:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_INSERT:
        case VK_DELETE:
            return true;
    }
    return false;
}

void SendKeys(const std::vector<WORD>& keys) {
    std::vector<INPUT> inputs;
    auto add = [&](WORD key, DWORD flags) {
        INPUT input{INPUT_KEYBOARD};
        input.ki.wVk = key;
        input.ki.dwFlags = flags | (IsExtendedKey(key) ? KEYEVENTF_EXTENDEDKEY : 0);
        inputs.push_back(input);
    };
    for (WORD key : keys) {
        add(key, 0);
    }
    for (auto it = keys.rbegin(); it != keys.rend(); ++it) {
        add(*it, KEYEVENTF_KEYUP);
    }
    SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
}

// The bar's menu had the focus; the shortcut can only go once the app is the
// foreground window again.
void SendKeysToTarget(HWND target, std::vector<WORD> keys) {
    g_pendingKeys = std::move(keys);
    g_pendingKeysTarget = target;
    SetForegroundWindow(target);
    SetTimer(g_controller, TIMER_SEND_KEYS, 80, nullptr);
}

void FlushPendingKeys() {
    KillTimer(g_controller, TIMER_SEND_KEYS);
    HWND foreground = GetForegroundWindow();
    if (!g_pendingKeys.empty() && foreground &&
        (foreground == g_pendingKeysTarget ||
         GetAncestor(foreground, GA_ROOTOWNER) == g_pendingKeysTarget)) {
        SendKeys(g_pendingKeys);
    }
    g_pendingKeys.clear();
    g_pendingKeysTarget = nullptr;
}

bool Confirm(PCWSTR text) {
    return MessageBoxW(nullptr, text, L"Global Menu Bar",
                       MB_YESNO | MB_ICONQUESTION | MB_TOPMOST |
                           MB_SETFOREGROUND) == IDYES;
}

void RunStatusAction(StatusKind kind) {
    switch (kind) {
        case StatusKind::Wifi:
            RunShell(L"ms-availablenetworks:");
            break;
        case StatusKind::Bluetooth:
            RunShell(L"ms-settings:bluetooth");
            break;
        case StatusKind::Volume:
        case StatusKind::ControlCenter:
            SendKeys({VK_LWIN, 'A'});
            break;
        case StatusKind::Battery:
            RunShell(L"ms-settings:batterysaver");
            break;
        case StatusKind::Search:
            SendKeys({VK_LWIN, 'S'});
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Menus

int HitTest(const Bar& bar, POINT pt) {
    RECT client;
    GetClientRect(bar.hwnd, &client);
    if (pt.y < client.top || pt.y >= client.bottom) {
        return -1;
    }
    // The whole bar height is clickable, including the screen edge.
    int count = (int)std::min(bar.rects.size(), g_items.size());
    for (int i = 0; i < count; i++) {
        const RECT& rc = bar.rects[i];
        if (rc.right > rc.left && pt.x >= rc.left && pt.x < rc.right) {
            return i;
        }
    }
    return -1;
}

// With a menu open, the mouse over another bar item switches menus.
bool HandleMenuHover(Bar* bar, POINT screenPt, bool click) {
    if (g_openIndex < 0) {
        return false;
    }
    POINT pt = screenPt;
    ScreenToClient(bar->hwnd, &pt);
    int hit = HitTest(*bar, pt);
    if (hit < 0 || !IsMenuKind(g_items[hit].kind) || !g_items[hit].enabled) {
        return false;
    }
    if (hit == g_openIndex && bar == g_openBar) {
        if (click) {
            g_pendingBar = nullptr;
            g_pendingIndex = -1;
            return true;
        }
        return false;
    }
    g_pendingBar = bar;
    g_pendingIndex = hit;
    return true;
}

LRESULT CALLBACK MsgFilterProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == MSGF_MENU) {
        MSG* msg = (MSG*)lParam;
        if (msg->message == WM_MOUSEMOVE || msg->message == WM_LBUTTONDOWN) {
            if (Bar* bar = BarFromHwnd(WindowFromPoint(msg->pt))) {
                if (HandleMenuHover(bar, msg->pt,
                                    msg->message == WM_LBUTTONDOWN)) {
                    EndMenu();
                    return TRUE;
                }
            }
        }
    }
    return CallNextHookEx(g_msgFilterHook, code, wParam, lParam);
}

// Right side items open the menu right-aligned.
bool g_menuAlignRight;

// notify: lets the menu send notifications (e.g. WM_MENURBUTTONUP) to the bar.
UINT TrackMenu(Bar& bar, HMENU menu, POINT pt, bool notify = false) {
    SetForegroundWindow(bar.hwnd);
    g_msgFilterHook = SetWindowsHookExW(WH_MSGFILTER, MsgFilterProc, nullptr,
                                        GetCurrentThreadId());
    UINT flags = TPM_RETURNCMD | TPM_TOPALIGN | TPM_LEFTBUTTON |
                 (notify ? 0 : TPM_NONOTIFY) |
                 (g_menuAlignRight ? TPM_RIGHTALIGN : TPM_LEFTALIGN);
    UINT cmd = (UINT)TrackPopupMenuEx(menu, flags, pt.x, pt.y, bar.hwnd,
                                      nullptr);
    if (g_msgFilterHook) {
        UnhookWindowsHookEx(g_msgFilterHook);
        g_msgFilterHook = nullptr;
    }
    PostMessageW(bar.hwnd, WM_NULL, 0, 0);
    return cmd;
}

// Commands of the mod's own menus (⊞ and the app menu).
enum : UINT {
    CMD_ABOUT = 1,
    CMD_SETTINGS,
    CMD_TASKMGR,
    CMD_SLEEP,
    CMD_RESTART,
    CMD_SHUTDOWN,
    CMD_LOCK,
    CMD_LOGOFF,
    CMD_OPEN_LOCATION = 100,
    CMD_MINIMIZE,
    CMD_MAXIMIZE,
    CMD_TOPMOST,
    CMD_EXCLUDE,
    CMD_CLOSE,
};

void ShowStartMenu(Bar& bar, POINT pt) {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, CMD_ABOUT, L"About This PC");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, CMD_SETTINGS, L"Settings...");
    AppendMenuW(menu, MF_STRING, CMD_TASKMGR, L"Task Manager...");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, CMD_SLEEP, L"Sleep");
    AppendMenuW(menu, MF_STRING, CMD_RESTART, L"Restart...");
    AppendMenuW(menu, MF_STRING, CMD_SHUTDOWN, L"Shut Down...");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, CMD_LOCK, L"Lock Screen");
    AppendMenuW(menu, MF_STRING, CMD_LOGOFF, L"Sign Out...");

    UINT cmd = TrackMenu(bar, menu, pt);
    DestroyMenu(menu);

    switch (cmd) {
        case CMD_ABOUT:
            RunShell(L"ms-settings:about");
            break;
        case CMD_SETTINGS:
            RunShell(L"ms-settings:");
            break;
        case CMD_TASKMGR:
            RunShell(L"taskmgr.exe");
            break;
        case CMD_SLEEP:
            SetSuspendState(FALSE, FALSE, FALSE);
            break;
        case CMD_RESTART:
            if (Confirm(L"Restart the computer now?")) {
                ShellExecuteW(nullptr, L"open", L"shutdown.exe", L"/r /t 0",
                              nullptr, SW_HIDE);
            }
            break;
        case CMD_SHUTDOWN:
            if (Confirm(L"Shut down the computer now?")) {
                ShellExecuteW(nullptr, L"open", L"shutdown.exe", L"/s /t 0",
                              nullptr, SW_HIDE);
            }
            break;
        case CMD_LOCK:
            LockWorkStation();
            break;
        case CMD_LOGOFF:
            if (Confirm(L"Sign out now?")) {
                ExitWindowsEx(EWX_LOGOFF, 0);
            }
            break;
    }
}

void ShowAppMenu(Bar& bar, POINT pt) {
    HWND target = g_target;
    if (!target || !IsWindow(target)) {
        return;
    }

    bool topmost = GetWindowLongW(target, GWL_EXSTYLE) & WS_EX_TOPMOST;
    std::wstring exeName = FileNameFromPath(g_appPath);
    std::wstring closeText = L"Close " + g_appName;

    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, g_appPath.empty() ? MF_GRAYED : MF_STRING,
                CMD_OPEN_LOCATION, L"Open File Location");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, CMD_MINIMIZE, L"Minimize");
    AppendMenuW(menu, MF_STRING, CMD_MAXIMIZE,
                IsZoomed(target) ? L"Restore" : L"Maximize");
    AppendMenuW(menu, MF_STRING | (topmost ? MF_CHECKED : 0), CMD_TOPMOST,
                L"Always on Top");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    if (!exeName.empty() && !g_targetExcluded) {
        std::wstring excludeText = L"Exclude " + exeName + L" from the global bar...";
        AppendMenuW(menu, MF_STRING, CMD_EXCLUDE, excludeText.c_str());
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    }
    AppendMenuW(menu, MF_STRING, CMD_CLOSE, closeText.c_str());

    UINT cmd = TrackMenu(bar, menu, pt);
    DestroyMenu(menu);

    switch (cmd) {
        case CMD_OPEN_LOCATION: {
            std::wstring params = L"/select,\"" + g_appPath + L"\"";
            RunShell(L"explorer.exe", params.c_str());
            break;
        }
        case CMD_MINIMIZE:
            PostMessageW(target, WM_SYSCOMMAND, SC_MINIMIZE, 0);
            break;
        case CMD_MAXIMIZE:
            SetForegroundWindow(target);
            PostMessageW(target, WM_SYSCOMMAND,
                         IsZoomed(target) ? SC_RESTORE : SC_MAXIMIZE, 0);
            break;
        case CMD_TOPMOST:
            SetWindowPos(target, topmost ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0,
                         0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            SetForegroundWindow(target);
            break;
        case CMD_EXCLUDE: {
            std::wstring text =
                L"To exclude " + exeName +
                L", add the name to \"Excluded apps\" in the mod's settings "
                L"in Windhawk and restart the app.\n\nCopy \"" +
                exeName + L"\" to the clipboard?";
            if (Confirm(text.c_str()) && OpenClipboard(bar.hwnd)) {
                EmptyClipboard();
                size_t bytes = (exeName.size() + 1) * sizeof(WCHAR);
                if (HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes)) {
                    memcpy(GlobalLock(mem), exeName.c_str(), bytes);
                    GlobalUnlock(mem);
                    if (!SetClipboardData(CF_UNICODETEXT, mem)) {
                        GlobalFree(mem);
                    }
                }
                CloseClipboard();
            }
            break;
        }
        case CMD_CLOSE:
            PostMessageW(target, WM_SYSCOMMAND, SC_CLOSE, 0);
            break;
        default:
            SetForegroundWindow(target);
            break;
    }
}

void ShowStandardMenu(Bar& bar, int index, POINT pt) {
    HWND target = g_target;
    const auto& menus = StandardMenus();
    if (!target || !IsWindow(target) || index < 0 ||
        index >= (int)menus.size()) {
        return;
    }
    const auto& commands = menus[index].commands;
    bool topmost = GetWindowLongW(target, GWL_EXSTYLE) & WS_EX_TOPMOST;

    HMENU menu = CreatePopupMenu();
    for (size_t i = 0; i < commands.size(); i++) {
        const StandardCommand& command = commands[i];
        if (!command.text) {
            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            continue;
        }
        std::wstring text = command.text;
        if (command.shortcut && *command.shortcut) {
            text += L"\t";
            text += command.shortcut;
        }
        UINT flags = MF_STRING;
        if (command.action == StandardAction::Topmost && topmost) {
            flags |= MF_CHECKED;
        }
        AppendMenuW(menu, flags, i + 1, text.c_str());
    }

    UINT cmd = TrackMenu(bar, menu, pt);
    DestroyMenu(menu);
    if (!cmd || cmd > commands.size() || !IsWindow(target)) {
        SetForegroundWindow(target);
        return;
    }

    const StandardCommand& command = commands[cmd - 1];
    switch (command.action) {
        case StandardAction::Keys:
            SendKeysToTarget(target, command.keys);
            break;
        case StandardAction::Close:
            PostMessageW(target, WM_SYSCOMMAND, SC_CLOSE, 0);
            break;
        case StandardAction::Minimize:
            PostMessageW(target, WM_SYSCOMMAND, SC_MINIMIZE, 0);
            break;
        case StandardAction::Maximize:
            SetForegroundWindow(target);
            PostMessageW(target, WM_SYSCOMMAND,
                         IsZoomed(target) ? SC_RESTORE : SC_MAXIMIZE, 0);
            break;
        case StandardAction::Topmost:
            SetWindowPos(target, topmost ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0,
                         0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            SetForegroundWindow(target);
            break;
        case StandardAction::WebHelp: {
            std::wstring query = g_appName;
            for (WCHAR& c : query) {
                if (c == L' ') {
                    c = L'+';
                }
            }
            std::wstring url =
                L"https://www.bing.com/search?q=" + query + L"+help";
            RunShell(url.c_str());
            break;
        }
    }
}

void ShowTrayMenu(Bar& bar, POINT pt) {
    struct Entry {
        HWND hwnd;
        UINT id;
    };
    std::vector<Entry> entries;
    std::vector<HBITMAP> bitmaps;
    int iconSize = GetSystemMetricsForDpi(SM_CXSMICON, bar.dpi);

    HMENU menu = CreatePopupMenu();
    for (const TrayIcon& icon : g_tray) {
        if (!IsTrayIconVisible(icon)) {
            continue;
        }
        entries.push_back({icon.hwnd, icon.id});
        UINT id = (UINT)entries.size();
        AppendMenuW(menu, MF_STRING, id, TrayIconTitle(icon).c_str());
        if (HBITMAP bitmap = IconToMenuBitmap(icon.icon, iconSize)) {
            bitmaps.push_back(bitmap);
            MENUITEMINFOW mii{sizeof(mii)};
            mii.fMask = MIIM_BITMAP;
            mii.hbmpItem = bitmap;
            SetMenuItemInfoW(menu, id, FALSE, &mii);
        }
    }

    g_trayMenu = menu;
    g_trayMenuRightClick = -1;
    UINT cmd = TrackMenu(bar, menu, pt, true);
    g_trayMenu = nullptr;
    DestroyMenu(menu);
    for (HBITMAP bitmap : bitmaps) {
        DeleteObject(bitmap);
    }

    POINT cursor;
    GetCursorPos(&cursor);
    if (g_trayMenuRightClick >= 0 &&
        g_trayMenuRightClick < (int)entries.size()) {
        const Entry& entry = entries[g_trayMenuRightClick];
        ClickTrayIcon(entry.hwnd, entry.id, WM_RBUTTONUP, false, cursor);
    } else if (cmd >= 1 && cmd <= entries.size()) {
        const Entry& entry = entries[cmd - 1];
        ClickTrayIcon(entry.hwnd, entry.id, WM_LBUTTONUP, false, cursor);
    }
    g_trayMenuRightClick = -1;
}

// Asks the app to update states (enabled/checked) and populate dynamic
// submenus, as it would if the menu were opened in its own window.
void PrepareSubMenu(HWND target, HMENU menu, int pos, int depth) {
    SendMessageTimeoutW(target, WM_INITMENUPOPUP, (WPARAM)menu,
                        MAKELPARAM(pos, FALSE), SMTO_ABORTIFHUNG, 300,
                        nullptr);
    if (depth >= 2) {
        return;
    }
    int count = std::min(GetMenuItemCount(menu), 128);
    for (int i = 0; i < count; i++) {
        if (HMENU sub = GetSubMenu(menu, i)) {
            PrepareSubMenu(target, sub, i, depth + 1);
        }
    }
}

// Owner-drawn items would send WM_MEASUREITEM/WM_DRAWITEM to the bar, which
// can't draw them.
bool MenuNeedsOwnerDraw(HMENU menu) {
    int count = std::min(GetMenuItemCount(menu), 128);
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii{sizeof(mii)};
        mii.fMask = MIIM_FTYPE | MIIM_BITMAP;
        if (GetMenuItemInfoW(menu, i, TRUE, &mii) &&
            ((mii.fType & MFT_OWNERDRAW) || mii.hbmpItem == HBMMENU_CALLBACK)) {
            return true;
        }
    }
    return false;
}

void OpenNativeMenu(HWND target, const BarItem& item) {
    SetForegroundWindow(target);
    if (item.mnemonic) {
        PostMessageW(target, WM_SYSCOMMAND, SC_KEYMENU,
                     (LPARAM)towlower(item.mnemonic));
        return;
    }
    PostMessageW(target, WM_SYSCOMMAND, SC_KEYMENU, 0);
    for (int i = 0; i < item.pos; i++) {
        PostMessageW(target, WM_KEYDOWN, VK_RIGHT, 0);
    }
    PostMessageW(target, WM_KEYDOWN, VK_DOWN, 0);
}

// Returns true if the menu was opened by the agent (closes asynchronously).
bool ShowWindowMenu(Bar& bar, const BarItem& item, POINT pt) {
    HWND target = g_target;
    HMENU top = GetTargetMenu();
    if (!top) {
        return false;
    }

    bool hidden = !GetMenu(target);

    if (g_targetHasAgent && (!g_settings.nativeMode || hidden) &&
        PostMessageW(target, g_msgPopup, item.pos, MAKELPARAM(pt.x, pt.y))) {
        g_asyncThread = GetWindowThreadProcessId(target, nullptr);
        return true;
    }

    HMENU sub = GetSubMenu(top, item.pos);
    if (!sub) {
        // Command item directly on the menu bar.
        UINT id = GetMenuItemID(top, item.pos);
        SetForegroundWindow(target);
        if (id != (UINT)-1) {
            PostMessageW(target, WM_COMMAND, MAKEWPARAM(id, 0), 0);
        }
        return false;
    }

    if (g_settings.nativeMode && !hidden) {
        OpenNativeMenu(target, item);
        return false;
    }

    SendMessageTimeoutW(target, WM_INITMENU, (WPARAM)top, 0, SMTO_ABORTIFHUNG,
                        300, nullptr);
    PrepareSubMenu(target, sub, item.pos, 0);

    if (MenuNeedsOwnerDraw(sub) && !hidden) {
        OpenNativeMenu(target, item);
        return false;
    }

    UINT cmd = TrackMenu(bar, sub, pt);
    SetForegroundWindow(target);
    if (cmd) {
        PostMessageW(target, WM_COMMAND, MAKEWPARAM(cmd, 0), 0);
    }
    return false;
}

void FinishMenus() {
    if (g_itemsDirty) {
        RebuildItems();
    } else {
        InvalidateBars();
    }
}

void OpenItem(Bar* bar, int index) {
    while (bar && index >= 0 && index < (int)g_items.size()) {
        BarItem item = g_items[index];

        g_openBar = bar;
        g_openIndex = index;
        g_pendingBar = nullptr;
        g_pendingIndex = -1;
        InvalidateBars();
        UpdateWindow(bar->hwnd);

        RECT client;
        GetClientRect(bar->hwnd, &client);
        g_menuAlignRight = item.rightSide;
        POINT pt{item.rightSide ? bar->rects[index].right
                                : bar->rects[index].left,
                 client.bottom};
        ClientToScreen(bar->hwnd, &pt);

        bool async = false;
        switch (item.kind) {
            case ItemKind::Start:
                ShowStartMenu(*bar, pt);
                break;
            case ItemKind::App:
                ShowAppMenu(*bar, pt);
                break;
            case ItemKind::Menu:
                async = ShowWindowMenu(*bar, item, pt);
                break;
            case ItemKind::Standard:
                ShowStandardMenu(*bar, item.pos, pt);
                break;
            case ItemKind::TrayMenu:
                ShowTrayMenu(*bar, pt);
                break;
            default:
                break;
        }
        g_menuAlignRight = false;

        if (async) {
            g_openAsync = true;
            g_asyncTick = GetTickCount64();
            SetTimer(g_controller, TIMER_ASYNC_CHECK, 250, nullptr);
            return;
        }

        g_openBar = nullptr;
        g_openIndex = -1;
        bar = g_pendingBar;
        index = g_pendingIndex;
        g_pendingBar = nullptr;
        g_pendingIndex = -1;
    }

    FinishMenus();
}

void OnAsyncMenuClosed() {
    if (!g_openAsync) {
        return;
    }
    g_openAsync = false;
    KillTimer(g_controller, TIMER_ASYNC_CHECK);

    g_openBar = nullptr;
    g_openIndex = -1;
    Bar* bar = g_pendingBar;
    int index = g_pendingIndex;
    g_pendingBar = nullptr;
    g_pendingIndex = -1;

    if (bar && index >= 0) {
        OpenItem(bar, index);
    } else {
        FinishMenus();
    }
}

bool ThreadHasVisibleMenu(DWORD threadId) {
    for (HWND menu = FindWindowExW(nullptr, nullptr, L"#32768", nullptr); menu;
         menu = FindWindowExW(nullptr, menu, L"#32768", nullptr)) {
        if (IsWindowVisible(menu) &&
            GetWindowThreadProcessId(menu, nullptr) == threadId) {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Tooltip

void HideTooltip() {
    KillTimer(g_controller, TIMER_TOOLTIP);
    if (g_tooltip && g_tooltipBar) {
        TOOLINFOW ti{};
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
        ti.hwnd = g_controller;
        ti.uId = 1;
        SendMessageW(g_tooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
    }
    g_tooltipBar = nullptr;
    g_tooltipIndex = -1;
}

void ScheduleTooltip(Bar* bar, int index) {
    HideTooltip();
    if (bar && index >= 0 && !g_items[index].tooltip.empty() &&
        g_openIndex < 0) {
        g_tooltipBar = bar;
        g_tooltipIndex = index;
        SetTimer(g_controller, TIMER_TOOLTIP, 600, nullptr);
    }
}

void ShowTooltip() {
    KillTimer(g_controller, TIMER_TOOLTIP);
    Bar* bar = g_tooltipBar;
    int index = g_tooltipIndex;
    if (!g_tooltip || !BarFromHwnd(bar ? bar->hwnd : nullptr) || index < 0 ||
        index >= (int)g_items.size() || bar->hot != index) {
        return;
    }

    TOOLINFOW ti{};
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
    ti.hwnd = g_controller;
    ti.uId = 1;
    ti.lpszText = (LPWSTR)g_items[index].tooltip.c_str();
    SendMessageW(g_tooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);

    RECT rc = bar->rects[index];
    POINT pt{rc.left, rc.bottom + Scale(*bar, 6)};
    ClientToScreen(bar->hwnd, &pt);
    SendMessageW(g_tooltip, TTM_TRACKPOSITION, 0, MAKELPARAM(pt.x, pt.y));
    SendMessageW(g_tooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
}

void CreateTooltip() {
    INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_BAR_CLASSES};
    InitCommonControlsEx(&icc);

    g_tooltip = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                                TOOLTIPS_CLASSW, nullptr,
                                WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 0, 0,
                                0, 0, nullptr, nullptr,
                                (HINSTANCE)&__ImageBase, nullptr);
    if (!g_tooltip) {
        return;
    }
    TOOLINFOW ti{};
    ti.cbSize = TTTOOLINFOW_V2_SIZE;
    ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd = g_controller;
    ti.uId = 1;
    ti.lpszText = (LPWSTR)L"";
    SendMessageW(g_tooltip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
}

////////////////////////////////////////////////////////////////////////////////
// Drawing

void DrawWindowsLogo(Bar& bar, const D2D1_RECT_F& rc, D2D1_COLOR_F color) {
    float size = (float)Scale(bar, 13);
    float gap = std::max(1.0f, (float)Scale(bar, 1));
    float square = (size - gap) / 2;
    float left = (rc.left + rc.right - size) / 2;
    float top = (rc.top + rc.bottom - size) / 2;

    bar.brush->SetColor(color);
    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 2; col++) {
            float x = left + col * (square + gap);
            float y = top + row * (square + gap);
            bar.target->FillRectangle(D2D1::RectF(x, y, x + square, y + square),
                                      bar.brush.Get());
        }
    }
}

void DrawText(Bar& bar,
              IDWriteTextFormat* format,
              const std::wstring& text,
              const D2D1_RECT_F& rc,
              D2D1_COLOR_F color) {
    bar.brush->SetColor(color);
    bar.target->DrawText(text.c_str(), (UINT32)text.size(), format, rc,
                         bar.brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void PaintItem(Bar& bar, int index) {
    const BarItem& item = g_items[index];
    const RECT& r = bar.rects[index];
    if (r.right <= r.left) {
        return;
    }
    D2D1_RECT_F rc = D2D1::RectF((float)r.left, (float)r.top, (float)r.right,
                                 (float)r.bottom);

    bool clickable = item.enabled;
    bool highlight = (index == g_openIndex && &bar == g_openBar) ||
                     (g_openIndex < 0 && index == bar.hot && clickable);
    if (highlight) {
        float radius = (float)Scale(bar, 5);
        bar.brush->SetColor(g_colors.hover);
        bar.target->FillRoundedRectangle(D2D1::RoundedRect(rc, radius, radius),
                                         bar.brush.Get());
    }

    D2D1_COLOR_F color =
        item.enabled && !item.dim ? g_colors.text : g_colors.disabled;

    switch (item.kind) {
        case ItemKind::Start:
            DrawWindowsLogo(bar, rc, color);
            return;

        case ItemKind::Tray: {
            ID2D1Bitmap* bitmap = GetIconBitmap(bar, item.icon);
            if (bitmap) {
                float size = (float)Scale(bar, 16);
                float x = (rc.left + rc.right - size) / 2;
                float y = (rc.top + rc.bottom - size) / 2;
                bar.target->DrawBitmap(
                    bitmap, D2D1::RectF(x, y, x + size, y + size), 1.0f,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
            return;
        }

        default:
            break;
    }

    if (item.glyph) {
        float glyphWidth = 0;
        float total = ItemContentWidth(bar, item, &glyphWidth);
        float left = (rc.left + rc.right - total) / 2;
        DrawText(bar, bar.fontIcon.Get(), std::wstring(1, item.glyph),
                 D2D1::RectF(left, rc.top, left + glyphWidth, rc.bottom),
                 color);
        if (!item.text.empty()) {
            float textLeft = left + glyphWidth + Scale(bar, 5);
            DrawText(bar, bar.fontNormal.Get(), item.text,
                     D2D1::RectF(textLeft, rc.top, left + total + 1, rc.bottom),
                     color);
        }
        return;
    }

    IDWriteTextFormat* format = item.kind == ItemKind::App
                                    ? bar.fontBold.Get()
                                    : bar.fontNormal.Get();
    DrawText(bar, format, item.text, rc, color);
}

void Paint(Bar& bar) {
    if (!EnsureRenderTarget(bar) || !bar.fontNormal) {
        return;
    }

    bar.target->BeginDraw();
    bar.target->SetTransform(D2D1::Matrix3x2F::Identity());
    bar.target->Clear(g_colors.background);

    for (int i = 0; i < (int)g_items.size() && i < (int)bar.rects.size(); i++) {
        PaintItem(bar, i);
    }

    if (bar.target->EndDraw() == D2DERR_RECREATE_TARGET) {
        bar.iconCache.clear();
        bar.brush.Reset();
        bar.target.Reset();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Bar window

void OnBarClick(Bar& bar, int hit, UINT msg) {
    if (hit < 0 || hit >= (int)g_items.size()) {
        return;
    }
    const BarItem& item = g_items[hit];
    HideTooltip();
    if (msg == WM_LBUTTONUP && item.kind == ItemKind::Status) {
        SetFlyoutTarget(bar.monitor);
    }

    switch (item.kind) {
        case ItemKind::Start:
        case ItemKind::App:
        case ItemKind::Menu:
        case ItemKind::Standard:
        case ItemKind::TrayMenu:
            if ((msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) &&
                item.enabled && g_openIndex < 0) {
                OpenItem(&bar, hit);
            }
            break;

        case ItemKind::UiaMenu:
            if (msg == WM_LBUTTONDOWN && g_target) {
                SetForegroundWindow(g_target);
                worker::Post(worker::WM_W_UIA_INVOKE, g_uiaGeneration,
                             item.pos);
            }
            break;

        case ItemKind::Tray:
            if (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP) {
                ClickTray(bar, hit, msg, false);
            } else if (msg == WM_LBUTTONDBLCLK) {
                ClickTray(bar, hit, msg, true);
            }
            break;

        case ItemKind::Status:
            if (msg == WM_LBUTTONUP) {
                RunStatusAction(item.status);
            }
            break;

        case ItemKind::Clock:
            // Win+N always opens on the primary monitor; on the others, the
            // clock of that monitor's (hidden) taskbar is triggered.
            if (msg == WM_LBUTTONUP &&
                !flyouts::OpenNotificationCenterOn(bar.monitor)) {
                SendKeys({VK_LWIN, 'N'});
            }
            break;
    }
}

LRESULT CALLBACK BarWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                            LPARAM lParam) {
    Bar* bar = (Bar*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (msg == WM_NCCREATE) {
        bar = (Bar*)((CREATESTRUCTW*)lParam)->lpCreateParams;
        bar->hwnd = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)bar);
    }
    if (!bar) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    if (msg == g_msgMenuHover && g_msgMenuHover) {
        return HandleMenuHover(
            bar, POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)},
            wParam != 0);
    }

    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_NCACTIVATE:
            return DefWindowProcW(hwnd, msg, TRUE, lParam);

        case WM_MOUSEMOVE: {
            if (!bar->trackingMouse) {
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                bar->trackingMouse = true;
            }
            int hit = HitTest(
                *bar, POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            if (hit != bar->hot) {
                bar->hot = hit;
                ScheduleTooltip(bar, hit);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            bar->trackingMouse = false;
            bar->hot = -1;
            HideTooltip();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONUP:
            OnBarClick(*bar,
                       HitTest(*bar, POINT{GET_X_LPARAM(lParam),
                                           GET_Y_LPARAM(lParam)}),
                       msg);
            return 0;

        case WM_MOUSEWHEEL: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hwnd, &pt);
            int hit = HitTest(*bar, pt);
            if (hit >= 0 && g_items[hit].kind == ItemKind::Status &&
                g_items[hit].status == StatusKind::Volume) {
                SendKeys({(WORD)(GET_WHEEL_DELTA_WPARAM(wParam) > 0
                                     ? VK_VOLUME_UP
                                     : VK_VOLUME_DOWN)});
                worker::Post(worker::WM_W_POLL_NOW, 0, 0);
            }
            return 0;
        }

        case WM_RBUTTONDOWN:
        case WM_CONTEXTMENU:
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            Paint(*bar);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_SIZE:
            if (bar->target) {
                bar->target->Resize(D2D1::SizeU(LOWORD(lParam), HIWORD(lParam)));
            }
            LayoutBar(*bar);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_MEASUREITEM:
        case WM_DRAWITEM:
            return FALSE;

        case WM_MENURBUTTONUP:
            if (g_trayMenu && (HMENU)lParam == g_trayMenu) {
                MENUITEMINFOW mii{sizeof(mii)};
                mii.fMask = MIIM_ID;
                if (GetMenuItemInfoW(g_trayMenu, (UINT)wParam, TRUE, &mii) &&
                    mii.wID >= 1) {
                    g_trayMenuRightClick = (int)mii.wID - 1;
                    EndMenu();
                }
            }
            return 0;

        case WM_WINDOWPOSCHANGED:
            if (bar->appbar) {
                APPBARDATA abd{sizeof(abd)};
                abd.hWnd = hwnd;
                SHAppBarMessage(ABM_WINDOWPOSCHANGED, &abd);
            }
            break;

        case WM_ACTIVATE:
            if (bar->appbar) {
                APPBARDATA abd{sizeof(abd)};
                abd.hWnd = hwnd;
                SHAppBarMessage(ABM_ACTIVATE, &abd);
            }
            break;

        case WM_BAR_APPBAR:
            if (wParam == ABN_POSCHANGED) {
                ApplyPosition(*bar);
            }
            return 0;

        case WM_SETTINGCHANGE:
            if (lParam && !wcscmp((PCWSTR)lParam, L"ImmersiveColorSet")) {
                PostMessageW(g_controller, WM_C_THEME, 0, 0);
            }
            break;

        case WM_DISPLAYCHANGE:
            PostMessageW(g_controller, WM_C_RECREATE_BARS, 0, 0);
            return 0;

        case WM_DPICHANGED:
            ApplyPosition(*bar);
            return 0;

        case WM_DESTROY:
            if (bar->appbar) {
                APPBARDATA abd{sizeof(abd)};
                abd.hWnd = hwnd;
                SHAppBarMessage(ABM_REMOVE, &abd);
                bar->appbar = false;
            }
            bar->iconCache.clear();
            bar->brush.Reset();
            bar->target.Reset();
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK CreateBarForMonitor(HMONITOR monitor, HDC, LPRECT, LPARAM) {
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(monitor, &mi) ||
        (!g_settings.allMonitors && !(mi.dwFlags & MONITORINFOF_PRIMARY))) {
        return TRUE;
    }

    auto bar = std::make_unique<Bar>();
    bar->monitor = monitor;
    bar->dpi = 0;
    UpdateBarDpi(*bar);

    HWND hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kBarClass,
        L"Global Menu Bar", WS_POPUP, mi.rcMonitor.left, mi.rcMonitor.top, 0,
        0, nullptr, nullptr, (HINSTANCE)&__ImageBase, bar.get());
    if (!hwnd) {
        return TRUE;
    }

    ApplyBackdrop(*bar);
    g_bars.push_back(std::move(bar));
    ApplyPosition(*g_bars.back());
    return TRUE;
}

void DestroyBars() {
    HideTooltip();
    g_openBar = g_pendingBar = nullptr;
    g_openIndex = g_pendingIndex = -1;
    g_openAsync = false;
    for (auto& bar : g_bars) {
        DestroyWindow(bar->hwnd);
    }
    g_bars.clear();
}

void CreateBars() {
    DestroyBars();
    EnumDisplayMonitors(nullptr, nullptr, CreateBarForMonitor, 0);
    RebuildItems();
    UpdateFullscreen();
}

////////////////////////////////////////////////////////////////////////////////
// Controller

// The taskbar exists and belongs to this process.
bool IsOurTaskbarReady() {
    DWORD pid = 0;
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    return tray && GetWindowThreadProcessId(tray, &pid) &&
           pid == GetCurrentProcessId();
}

void StartBars() {
    g_started = true;
    CreateBars();
    InstallTrayHook();
    if (g_settings.trayIcons) {
        RequestTrayIcons();
    }
    OnForeground(GetForegroundWindow());
}

void ReloadSettings() {
    bool hadTray = g_settings.trayIcons;
    LoadSettings();
    UpdateColors();
    if (g_started) {
        CreateBars();
        if (g_settings.trayIcons && !hadTray) {
            RequestTrayIcons();
        }
        g_target = nullptr;
        OnForeground(GetForegroundWindow());
    }
}

LRESULT CALLBACK ControllerWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam) {
    if (msg == g_msgMenuClosed && g_msgMenuClosed) {
        OnAsyncMenuClosed();
        return 0;
    }

    switch (msg) {
        case WM_CREATE:
            g_controller = hwnd;
            CreateTooltip();
            g_winEventHook = SetWinEventHook(
                EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
            SetTimer(hwnd, TIMER_TICK, 1000, nullptr);
            if (IsOurTaskbarReady()) {
                StartBars();
            } else {
                SetTimer(hwnd, TIMER_WAIT_TASKBAR, 500, nullptr);
            }
            return 0;

        case WM_TIMER:
            switch (wParam) {
                case TIMER_WAIT_TASKBAR:
                    if (IsOurTaskbarReady()) {
                        KillTimer(hwnd, TIMER_WAIT_TASKBAR);
                        StartBars();
                    }
                    break;

                case TIMER_TICK:
                    if (!g_started) {
                        break;
                    }
                    if (g_target && !IsWindow(g_target)) {
                        g_target = nullptr;
                        OnForeground(GetForegroundWindow());
                    }
                    // Some apps (e.g. MDI) put the menu back; hides it again.
                    MaybeHideNativeMenu();
                    UpdateFullscreen();
                    if (g_settings.trayIcons &&
                        g_trayWnd != FindWindowW(L"Shell_TrayWnd", nullptr)) {
                        InstallTrayHook();
                        if (g_trayWnd) {
                            RequestTrayIcons();
                        }
                    }
                    PruneDeadTrayIcons();
                    RebuildItems();
                    break;

                case TIMER_RECREATE:
                    if (g_openIndex >= 0) {
                        break;
                    }
                    KillTimer(hwnd, TIMER_RECREATE);
                    CreateBars();
                    break;

                case TIMER_ASYNC_CHECK:
                    if (g_openAsync &&
                        GetTickCount64() - g_asyncTick > 1500 &&
                        !ThreadHasVisibleMenu(g_asyncThread)) {
                        OnAsyncMenuClosed();
                    }
                    break;

                case TIMER_TOOLTIP:
                    ShowTooltip();
                    break;

                case TIMER_SEND_KEYS:
                    FlushPendingKeys();
                    break;

                case TIMER_UIA_RETRY:
                    KillTimer(hwnd, TIMER_UIA_RETRY);
                    RequestUia();
                    break;
            }
            return 0;

        case WM_C_RELOAD:
            ReloadSettings();
            return 0;

        case WM_C_THEME:
            UpdateColors();
            for (auto& bar : g_bars) {
                ApplyBackdrop(*bar);
            }
            InvalidateBars();
            return 0;

        case WM_C_RECREATE_BARS:
            SetTimer(hwnd, TIMER_RECREATE, 800, nullptr);
            return 0;

        case WM_C_TRAY: {
            auto* update = (TrayUpdate*)lParam;
            ApplyTrayUpdate(update);
            delete update;
            if (g_settings.trayIcons) {
                RebuildItems();
            }
            return 0;
        }

        case WM_C_STATUS: {
            auto* status = (worker::StatusInfo*)lParam;
            g_status = *status;
            delete status;
            RebuildItems();
            return 0;
        }

        case WM_C_UIA: {
            auto* result = (worker::UiaResult*)lParam;
            if (result->generation == g_uiaGeneration) {
                g_uiaNames = std::move(result->names);
                // Apps still loading, and Chromium/Electron, which only builds
                // the accessibility tree after the first access, may not
                // expose the menu right away.
                if (g_uiaNames.empty() && g_uiaRetries++ < 4) {
                    SetTimer(hwnd, TIMER_UIA_RETRY, 1200, nullptr);
                }
                RebuildItems();
            }
            delete result;
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, TIMER_TICK);
            KillTimer(hwnd, TIMER_WAIT_TASKBAR);
            KillTimer(hwnd, TIMER_RECREATE);
            KillTimer(hwnd, TIMER_ASYNC_CHECK);
            KillTimer(hwnd, TIMER_TOOLTIP);
            KillTimer(hwnd, TIMER_UIA_RETRY);
            KillTimer(hwnd, TIMER_SEND_KEYS);
            RemoveTrayHook();
            if (g_winEventHook) {
                UnhookWinEvent(g_winEventHook);
                g_winEventHook = nullptr;
            }
            DestroyBars();
            if (g_tooltip) {
                DestroyWindow(g_tooltip);
                g_tooltip = nullptr;
            }
            g_controller = nullptr;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI BarThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    LoadSettings();
    UpdateColors();
    RegisterSharedMessages();
    ChangeWindowMessageFilter(g_msgMenuHover, MSGFLT_ADD);
    ChangeWindowMessageFilter(g_msgMenuClosed, MSGFLT_ADD);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, g_d2d.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        (IUnknown**)g_dwrite.GetAddressOf());
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                     IID_PPV_ARGS(&g_wic));

    HINSTANCE instance = (HINSTANCE)&__ImageBase;
    bool ok = g_d2d && g_dwrite;
    if (ok) {
        DetectIconFont();

        WNDCLASSEXW wc{sizeof(wc)};
        wc.style = CS_DBLCLKS;
        wc.lpfnWndProc = BarWndProc;
        wc.hInstance = instance;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kBarClass;
        RegisterClassExW(&wc);

        WNDCLASSEXW cc{sizeof(cc)};
        cc.lpfnWndProc = ControllerWndProc;
        cc.hInstance = instance;
        cc.lpszClassName = kControllerClass;
        RegisterClassExW(&cc);

        // The worker thread needs the controller to post results.
        HWND controller =
            CreateWindowExW(0, kControllerClass, nullptr, 0, 0, 0, 0, 0,
                            HWND_MESSAGE, nullptr, instance, nullptr);
        ok = controller != nullptr;
        if (ok) {
            worker::Start(controller);
            if (g_started) {
                RequestUia();
            }
        }
    }
    SetEvent(g_readyEvent);

    if (ok) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    } else {
        Wh_Log(L"Failed to start the bar");
    }

    worker::Stop();

    // Discards results that arrived after the end of the loop.
    MSG msg;
    while (PeekMessageW(&msg, nullptr, WM_C_TRAY, WM_C_UIA, PM_REMOVE)) {
    }

    ClearTray();
    g_items.clear();
    g_wic.Reset();
    g_dwrite.Reset();
    g_d2d.Reset();

    UnregisterClassW(kBarClass, instance);
    UnregisterClassW(kControllerClass, instance);
    CoUninitialize();
    return 0;
}

// With "Launch folder windows in a separate process", folder windows run in
// their own explorer.exe (/factory,... -Embedding). The bar must stay in the
// taskbar's process, the only one that can read the tray
// icons.
bool IsShellExplorer() {
    std::wstring commandLine = ToLower(GetCommandLineW());
    return commandLine.find(L"/factory") == std::wstring::npos &&
           commandLine.find(L"-embedding") == std::wstring::npos &&
           commandLine.find(L"/separate") == std::wstring::npos;
}

BOOL Init() {
    if (!IsShellExplorer()) {
        Wh_Log(L"Folder windows process, the bar stays in the taskbar "
               L"process");
        return TRUE;
    }

    // There's only one taskbar process per session. The mutex is still
    // created so that earlier versions of the mod, which may still be
    // running in the folders process during the update, don't create a
    // second bar; if it already exists, it's one of those versions, which
    // will be unloaded right after.
    g_mutex = CreateMutexW(nullptr, FALSE, L"Local\\WindhawkGlobalMenuBar");

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, BarThread, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        CloseHandle(g_mutex);
        g_mutex = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Uninit() {
    if (g_thread) {
        WaitForSingleObject(g_readyEvent, INFINITE);
        if (HWND controller = g_controller) {
            PostMessageW(controller, WM_CLOSE, 0, 0);
        }
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }

    if (g_mutex) {
        CloseHandle(g_mutex);
        g_mutex = nullptr;
    }
}

void SettingsChanged() {
    if (HWND controller = g_controller) {
        PostMessageW(controller, WM_C_RELOAD, 0, 0);
    }
}

}  // namespace bar

////////////////////////////////////////////////////////////////////////////////
// System flyouts at the top: runs in ShellHost.exe (quick settings) and
// in ShellExperienceHost.exe (notifications and calendar).
//
// The windows of these flyouts take the whole work area height and the
// content is aligned at the bottom; changing the alignment of a few XAML
// elements is enough. The elements are found with XAML Diagnostics, the
// same technique as the Windows 11 Notification Center Styler mod (m417z),
// and the changes follow the recipe published in
// https://github.com/ramensoftware/windhawk-mods/issues/1053#issuecomment-2405461863

namespace flyouts {

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;

enum class Target { ShellExperienceHost, ShellHost, Explorer };

Target g_target;
bool g_animateFromTop;
int g_barHeight = 28;
int g_gap = 8;
std::atomic<bool> g_tapInjected;
std::atomic<bool> g_unloading;

// With more than one UI thread, XAML's composition diagnostics corrupt the
// heap. They're turned off by answering "1" to the DisableCompositionDiag
// value, read only once inside AdviseVisualTreeChange.
thread_local bool g_reportCompositionDiagAsDisabled;

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)&GetCurrentModuleHandle, &module)) {
        return nullptr;
    }
    return module;
}

// Original value of each changed property, to restore when unloading.
struct Change {
    winrt::weak_ref<wux::FrameworkElement> element;
    bool verticalAlignment = false;
    wux::VerticalAlignment originalVerticalAlignment{};
    wux::VerticalAlignment wantedVerticalAlignment{};
    int64_t verticalAlignmentToken = 0;
    wux::FrameworkElement::Loaded_revoker loadedRevoker;
    wux::FrameworkElement::SizeChanged_revoker sizeChangedRevoker;
    bool transform = false;
    wux::Media::Transform originalTransform{nullptr};
    wf::Point originalTransformOrigin{};
    bool margin = false;
    wux::Thickness originalMargin{};
    wux::Thickness wantedMargin{};
    int64_t marginToken = 0;
    bool maxHeight = false;
    double originalMaxHeight = 0;
    bool rows = false;
    std::vector<wux::GridLength> originalRows;
};

// State of each UI thread; only the thread itself touches its own.
struct ThreadState {
    std::vector<Change> changes;
    std::vector<InstanceHandle> pendingReleases;
    winrt::Windows::System::DispatcherQueueTimer releaseTimer{nullptr};
    winrt::event_token releaseTimerToken{};
    bool releaseArmed = false;
};

std::mutex g_threadsMutex;
std::map<DWORD, ThreadState> g_threads;

ThreadState& CurrentThreadState() {
    std::lock_guard<std::mutex> lock(g_threadsMutex);
    return g_threads[GetCurrentThreadId()];
}

Change& RecordChange(const wux::FrameworkElement& element) {
    auto& changes = CurrentThreadState().changes;
    for (Change& change : changes) {
        if (auto existing = change.element.get(); existing && existing == element) {
            return change;
        }
    }
    std::erase_if(changes, [](const Change& c) { return !c.element.get(); });
    Change change;
    change.element = winrt::make_weak(element);
    changes.push_back(std::move(change));
    return changes.back();
}

void SetVerticalAlignment(const wux::FrameworkElement& element,
                          wux::VerticalAlignment alignment) {
    Change& change = RecordChange(element);
    change.wantedVerticalAlignment = alignment;
    if (!change.verticalAlignment) {
        change.verticalAlignment = true;
        change.originalVerticalAlignment = element.VerticalAlignment();
        // The flyout itself sets the alignment back to the bottom when opening
        // (it adapts to the taskbar position), so the change is reapplied
        // whenever the value changes.
        change.verticalAlignmentToken = element.RegisterPropertyChangedCallback(
            wux::FrameworkElement::VerticalAlignmentProperty(),
            [](const wux::DependencyObject& sender,
               const wux::DependencyProperty&) {
                if (g_unloading) {
                    return;
                }
                auto fe = sender.try_as<wux::FrameworkElement>();
                if (!fe) {
                    return;
                }
                for (const Change& c : CurrentThreadState().changes) {
                    if (c.verticalAlignment && c.element.get() == fe) {
                        if (fe.VerticalAlignment() != c.wantedVerticalAlignment) {
                            fe.VerticalAlignment(c.wantedVerticalAlignment);
                        }
                        break;
                    }
                }
            });
    }
    element.VerticalAlignment(alignment);
}

// Rotating the page and the inner grid by 180° keeps the content in the same
// position, but the entrance animation now comes from the top.
void SetRotated(const wux::FrameworkElement& element) {
    Change& change = RecordChange(element);
    if (!change.transform) {
        change.transform = true;
        change.originalTransform = element.RenderTransform();
        change.originalTransformOrigin = element.RenderTransformOrigin();
    }
    wux::Media::RotateTransform rotate;
    rotate.Angle(180);
    element.RenderTransform(rotate);
    element.RenderTransformOrigin(wf::Point{0.5f, 0.5f});
}

void SetMaxHeight(const wux::FrameworkElement& element, double maxHeight) {
    Change& change = RecordChange(element);
    if (!change.maxHeight) {
        change.maxHeight = true;
        change.originalMaxHeight = element.MaxHeight();
    }
    element.MaxHeight(maxHeight);
}

void SetRowsAuto(const wux::FrameworkElement& element) {
    auto grid = element.try_as<wux::Controls::Grid>();
    if (!grid) {
        return;
    }
    auto rows = grid.RowDefinitions();
    Change& change = RecordChange(element);
    if (!change.rows) {
        change.rows = true;
        for (const auto& row : rows) {
            change.originalRows.push_back(row.Height());
        }
    }
    for (const auto& row : rows) {
        row.Height(wux::GridLengthHelper::Auto());
    }
}

double WorkAreaHeightDip() {
    HMONITOR monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{sizeof(mi)};
    UINT dpiX = 96, dpiY = 96;
    if (!GetMonitorInfoW(monitor, &mi)) {
        return 800;
    }
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    return (mi.rcWork.bottom - mi.rcWork.top) * 96.0 / dpiY;
}

std::wstring ClassNameOf(const wux::FrameworkElement& element) {
    return element ? std::wstring(winrt::get_class_name(element)) : L"";
}

wux::FrameworkElement ParentOf(const wux::FrameworkElement& element) {
    if (!element) {
        return nullptr;
    }
    return wux::Media::VisualTreeHelper::GetParent(element)
        .try_as<wux::FrameworkElement>();
}

bool IsPageType(std::wstring_view type) {
    return type == L"ControlCenter.ControlCenterPage" ||
           type == L"ActionCenter.NotificationCenterPage";
}

// Sticks the content to the top: it stays at the top and everything between
// it and the page stretches to fill the whole window.
bool IsTargetWindow(HWND hwnd);

BOOL CALLBACK FindThreadPanelProc(HWND hwnd, LPARAM param) {
    RECT rc;
    if (IsTargetWindow(hwnd) && GetWindowRect(hwnd, &rc) &&
        rc.right - rc.left > 1 && rc.bottom - rc.top > 1) {
        *(HWND*)param = hwnd;
        return FALSE;
    }
    return TRUE;
}

// How far the content must move down, in DIPs, to sit below the bar with
// the configured spacing. If the window already starts below the bar, it
// only moves down as much as needed.
double TopOffsetDip() {
    HWND hwnd = nullptr;
    EnumThreadWindows(GetCurrentThreadId(), FindThreadPanelProc,
                      (LPARAM)&hwnd);
    if (!hwnd) {
        return g_barHeight + g_gap;
    }
    RECT rc;
    GetWindowRect(hwnd, &rc);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);
    UINT dpi = GetDpiForWindow(hwnd);
    if (!dpi) {
        dpi = 96;
    }
    double windowTopDip = (rc.top - mi.rcMonitor.top) * 96.0 / dpi;
    double offset = std::max(0.0, g_barHeight + g_gap - windowTopDip);
    Wh_Log(L"Flyout window: (%d,%d)-(%d,%d), dpi %u, offset %.1f",
           rc.left, rc.top, rc.right, rc.bottom, dpi, offset);
    return offset;
}

Change& TrackMargin(const wux::FrameworkElement& element);

void FixRootContent(const wux::FrameworkElement& rootContent,
                    const wux::FrameworkElement& page) {
    bool controlCenter =
        ClassNameOf(page) == L"ControlCenter.ControlCenterPage";
    if (controlCenter) {
        SetVerticalAlignment(rootContent, wux::VerticalAlignment::Top);
    } else {
        // The Notification Center only draws the area the content took at the
        // bottom; with the content only at the top, the flyout shows up cut in
        // the middle of the screen. Stretched to the bottom, with the rows set
        // to "Auto", the cards stay at the top and the drawn area covers the whole window.
        SetVerticalAlignment(rootContent, wux::VerticalAlignment::Stretch);
        SetRowsAuto(rootContent);
    }
    for (auto ancestor = ParentOf(rootContent); ancestor && ancestor != page;
         ancestor = ParentOf(ancestor)) {
        SetVerticalAlignment(ancestor, wux::VerticalAlignment::Stretch);
    }
    SetVerticalAlignment(page, wux::VerticalAlignment::Stretch);

    // The flyout window starts at the top of the monitor, under the bar; the
    // content moves down by the bar height plus the spacing.
    double offset = TopOffsetDip();
    Change& change = TrackMargin(rootContent);
    change.wantedMargin = change.originalMargin;
    change.wantedMargin.Top += offset;
    rootContent.Margin(change.wantedMargin);

    Wh_Log(L"Content moved to the top (%s), top margin %.1f",
           controlCenter ? L"quick settings" : L"notifications",
           change.wantedMargin.Top);
}

// Stores the original margin and starts keeping wantedMargin: the flyout may
// reset the margin when opening.
Change& TrackMargin(const wux::FrameworkElement& element) {
    Change& change = RecordChange(element);
    if (!change.margin) {
        change.margin = true;
        change.originalMargin = element.Margin();
        change.wantedMargin = change.originalMargin;
        change.marginToken = element.RegisterPropertyChangedCallback(
            wux::FrameworkElement::MarginProperty(),
            [](const wux::DependencyObject& sender,
               const wux::DependencyProperty&) {
                if (g_unloading) {
                    return;
                }
                auto fe = sender.try_as<wux::FrameworkElement>();
                if (!fe) {
                    return;
                }
                for (const Change& c : CurrentThreadState().changes) {
                    if (c.margin && c.element.get() == fe) {
                        if (fe.Margin() != c.wantedMargin) {
                            fe.Margin(c.wantedMargin);
                        }
                        break;
                    }
                }
            });
    }
    return change;
}

wux::FrameworkElement FindByName(const wux::DependencyObject& root,
                                 std::wstring_view name,
                                 int depth) {
    int count = wux::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = wux::Media::VisualTreeHelper::GetChild(root, i);
        if (auto fe = child.try_as<wux::FrameworkElement>();
            fe && fe.Name() == name) {
            return fe;
        }
        if (depth > 0) {
            if (auto found = FindByName(child, name, depth - 1)) {
                return found;
            }
        }
    }
    return nullptr;
}

void FixPage(const wux::FrameworkElement& page) {
    if (g_unloading) {
        return;
    }
    try {
        if (auto rootContent = FindByName(page, L"RootContent", 8)) {
            FixRootContent(rootContent, page);
        } else {
            Wh_Log(L"RootContent not found in %s",
                   ClassNameOf(page).c_str());
        }
    } catch (...) {
        Wh_Log(L"Error: %08X", winrt::to_hresult());
    }
}

// When the page is reported, its children may not exist yet; redoes the
// search every time it's loaded (the flyout is recreated when opening).
void WatchPage(const wux::FrameworkElement& page) {
    Change& change = RecordChange(page);
    if (!change.loadedRevoker) {
        change.loadedRevoker = page.Loaded(
            winrt::auto_revoke,
            [](const wf::IInspectable& sender, const wux::RoutedEventArgs&) {
                if (auto fe = sender.try_as<wux::FrameworkElement>()) {
                    FixPage(fe);
                }
            });
        // The window may be positioned after Loaded; recomputes the offset
        // when the size changes.
        change.sizeChangedRevoker = page.SizeChanged(
            winrt::auto_revoke,
            [](const wf::IInspectable& sender, const wux::SizeChangedEventArgs&) {
                if (auto fe = sender.try_as<wux::FrameworkElement>()) {
                    FixPage(fe);
                }
            });
    }
    FixPage(page);
}

// Explorer: the clock buttons (Notification Center) of each taskbar, with
// the queue of each one's thread.
struct ClockButton {
    winrt::weak_ref<wux::FrameworkElement> element;
    winrt::Windows::System::DispatcherQueue queue{nullptr};
};
std::mutex g_clockButtonsMutex;
std::vector<ClockButton> g_clockButtons;

void RememberClockButton(const wux::FrameworkElement& element) {
    auto queue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    if (!queue) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_clockButtonsMutex);
    std::erase_if(g_clockButtons,
                  [](const ClockButton& b) { return !b.element.get(); });
    for (const ClockButton& b : g_clockButtons) {
        if (b.element.get() == element) {
            return;
        }
    }
    g_clockButtons.push_back({winrt::make_weak(element), queue});
    Wh_Log(L"Taskbar clock found (%d)",
           (int)g_clockButtons.size());
}

// Triggers the element through its automation pattern or the one of the
// first descendant that has one (OmniButton doesn't create its own peer; the
// IconView inside it does, and the peer itself implements IInvokeProvider).
bool InvokeElement(const wux::DependencyObject& node, int depth) {
    namespace peers = winrt::Windows::UI::Xaml::Automation::Peers;
    namespace provider = winrt::Windows::UI::Xaml::Automation::Provider;
    if (auto element = node.try_as<wux::UIElement>()) {
        if (auto peer =
                peers::FrameworkElementAutomationPeer::CreatePeerForElement(
                    element)) {
            if (auto invoke = peer.GetPattern(peers::PatternInterface::Invoke)) {
                if (auto winrtInvoke =
                        invoke.try_as<provider::IInvokeProvider>()) {
                    winrtInvoke.Invoke();
                } else if (auto comInvoke =
                               invoke.try_as<::IInvokeProvider>()) {
                    comInvoke->Invoke();
                } else if (auto peerInvoke =
                               peer.try_as<provider::IInvokeProvider>()) {
                    peerInvoke.Invoke();
                } else {
                    return false;
                }
                Wh_Log(L"Clock triggered (Invoke) in %s",
                       winrt::get_class_name(node).c_str());
                return true;
            }
            if (auto toggle = peer.GetPattern(peers::PatternInterface::Toggle)) {
                toggle.as<provider::IToggleProvider>().Toggle();
                Wh_Log(L"Clock triggered (Toggle) in %s",
                       winrt::get_class_name(node).c_str());
                return true;
            }
        }
    }
    if (depth <= 0) {
        return false;
    }
    int count = wux::Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; i++) {
        if (InvokeElement(wux::Media::VisualTreeHelper::GetChild(node, i),
                          depth - 1)) {
            return true;
        }
    }
    return false;
}

BOOL CALLBACK FindMonitorTaskbarProc(HWND hwnd, LPARAM param) {
    auto* data = (std::pair<HMONITOR, HWND>*)param;
    if (IsWindowOfClass(hwnd, L"Shell_SecondaryTrayWnd") &&
        MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) == data->first) {
        data->second = hwnd;
        return FALSE;
    }
    return TRUE;
}

// Triggers the clock of the monitor's secondary taskbar: Windows opens the
// Notification Center on that monitor, with the right size for its DPI.
// Returns false on the primary monitor or if there's no button.
bool OpenNotificationCenterOn(HMONITOR monitor) {
    std::pair<HMONITOR, HWND> data{monitor, nullptr};
    EnumWindows(FindMonitorTaskbarProc, (LPARAM)&data);
    RECT rc;
    if (!data.second || !GetWindowRect(data.second, &rc)) {
        return false;
    }
    // The bar's XAML root is as wide as the bar, in DIPs.
    double width = (rc.right - rc.left) * 96.0 / MonitorDpi(monitor);
    std::vector<ClockButton> buttons;
    {
        std::lock_guard<std::mutex> lock(g_clockButtonsMutex);
        buttons = g_clockButtons;
    }
    bool queued = false;
    for (const ClockButton& button : buttons) {
        queued |= button.queue.TryEnqueue([weak = button.element, width] {
            try {
                auto element = weak.get();
                auto root = element ? element.XamlRoot() : nullptr;
                if (root && std::abs(root.Size().Width - width) < 2) {
                    if (!InvokeElement(element, 8)) {
                        Wh_Log(L"Clock without an automation pattern");
                    }
                }
            } catch (...) {
                Wh_Log(L"Error triggering the clock: %08X",
                       winrt::to_hresult());
            }
        });
    }
    return queued;
}

void ApplyToElement(const wux::FrameworkElement& element,
                    std::wstring_view type) {
    if (g_unloading) {
        return;
    }

    if (g_target == Target::Explorer) {
        if (element.Name() == L"NotificationCenterButton") {
            RememberClockButton(element);
        }
        return;
    }

    std::wstring name(element.Name());
    auto parent = ParentOf(element);
    std::wstring parentType = ClassNameOf(parent);

    // Quick settings (Win+A).
    if (type == L"ControlCenter.ControlCenterPage") {
        SetVerticalAlignment(element, wux::VerticalAlignment::Stretch);
        if (g_animateFromTop) {
            SetRotated(element);
        }
        WatchPage(element);
        return;
    }
    if (name == L"RootGrid" && parentType == L"ControlCenter.ControlCenterPage") {
        SetVerticalAlignment(element, wux::VerticalAlignment::Stretch);
        if (g_animateFromTop) {
            SetRotated(element);
        }
        return;
    }

    if (type == L"ActionCenter.NotificationCenterPage") {
        SetVerticalAlignment(element, wux::VerticalAlignment::Stretch);
        WatchPage(element);
        return;
    }

    if (name == L"RootContent") {
        // The path to the page changes between Windows versions; walks up until found.
        auto ancestor = parent;
        for (int i = 0; ancestor && i < 8; i++) {
            if (IsPageType(ClassNameOf(ancestor))) {
                FixRootContent(element, ancestor);
                break;
            }
            ancestor = ParentOf(ancestor);
        }
        return;
    }

    // With the rows set to "Auto", the notification list needs a limit so
    // the calendar stays visible.
    if (name == L"NotificationCenterGrid") {
        SetMaxHeight(element, std::max(200.0, WorkAreaHeightDip() * 0.55));
    }
}

void RestoreCurrentThread() {
    auto& changes = CurrentThreadState().changes;
    for (Change& change : changes) {
        auto element = change.element.get();
        if (!element) {
            continue;
        }
        try {
            change.loadedRevoker.revoke();
            change.sizeChangedRevoker.revoke();
            if (change.verticalAlignment) {
                element.UnregisterPropertyChangedCallback(
                    wux::FrameworkElement::VerticalAlignmentProperty(),
                    change.verticalAlignmentToken);
                element.VerticalAlignment(change.originalVerticalAlignment);
            }
            if (change.transform) {
                element.RenderTransform(change.originalTransform);
                element.RenderTransformOrigin(change.originalTransformOrigin);
            }
            if (change.margin) {
                element.UnregisterPropertyChangedCallback(
                    wux::FrameworkElement::MarginProperty(), change.marginToken);
                element.Margin(change.originalMargin);
            }
            if (change.maxHeight) {
                element.MaxHeight(change.originalMaxHeight);
            }

            if (change.rows) {
                if (auto grid = element.try_as<wux::Controls::Grid>()) {
                    auto rows = grid.RowDefinitions();
                    for (uint32_t i = 0;
                         i < rows.Size() && i < change.originalRows.size();
                         i++) {
                        rows.GetAt(i).Height(change.originalRows[i]);
                    }
                }
            }
        } catch (...) {
            Wh_Log(L"Error restoring: %08X", winrt::to_hresult());
        }
    }
    changes.clear();
}

// Interface that XAML Diagnostics implements and xamlom.h doesn't declare.
// UnregisterInstance releases the reference the diagnostics keep of each
// reported element; without it, the elements would leak.
constexpr GUID IID_IXamlDiagnosticsTestHooks = {
    0x735941a2, 0x3ee3, 0x495a, {0x8d, 0xa9, 0x97, 0x26, 0x27, 0x00, 0x30, 0x75}};

struct IXamlDiagnosticsTestHooks : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE UnregisterInstance(InstanceHandle handle) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetDispatcherQueueForObject(
        InstanceHandle handle,
        void** dispatcherQueue) = 0;
};

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
   public:
    explicit VisualTreeWatcher(winrt::com_ptr<IUnknown> site);

    void UnadviseVisualTreeChange() {
        HRESULT hr = m_diagnostics.as<IVisualTreeService3>()
                         ->UnadviseVisualTreeChange(this);
        if (FAILED(hr)) {
            Wh_Log(L"UnadviseVisualTreeChange failed: %08X", hr);
        }
    }

    void ReleaseReference(InstanceHandle handle) {
        if (m_testHooks) {
            m_testHooks->UnregisterInstance(handle);
        }
    }

    winrt::com_ptr<IXamlDiagnostics> m_diagnostics;
    winrt::com_ptr<IXamlDiagnosticsTestHooks> m_testHooks;

   private:
    HRESULT STDMETHODCALLTYPE
    OnVisualTreeChange(ParentChildRelation relation,
                       VisualElement element,
                       VisualMutationType mutationType) override;

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle,
                                                    VisualElementState,
                                                    LPCWSTR) noexcept override {
        return S_OK;
    }
};

winrt::com_ptr<VisualTreeWatcher> g_watcher;

DWORD WINAPI AdviseThreadProc(LPVOID param) {
    auto* watcher = (VisualTreeWatcher*)param;
    g_reportCompositionDiagAsDisabled = true;
    HRESULT hr = watcher->m_diagnostics.as<IVisualTreeService3>()
                     ->AdviseVisualTreeChange(watcher);
    g_reportCompositionDiagAsDisabled = false;
    watcher->Release();
    if (FAILED(hr)) {
        Wh_Log(L"AdviseVisualTreeChange failed: %08X", hr);
    }
    return 0;
}

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
    : m_diagnostics(site.as<IXamlDiagnostics>()) {
    m_diagnostics->QueryInterface(IID_IXamlDiagnosticsTestHooks,
                                  m_testHooks.put_void());

    // Calling AdviseVisualTreeChange from here sometimes hangs the app; the
    // Styler does it on a new thread for the same reason.
    AddRef();
    HANDLE thread = CreateThread(nullptr, 0, AdviseThreadProc, this, 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        Release();
    }
}

void DrainReleases() {
    ThreadState& state = CurrentThreadState();
    state.releaseArmed = false;
    auto pending = std::move(state.pendingReleases);
    state.pendingReleases.clear();
    if (!g_watcher) {
        return;
    }
    std::sort(pending.begin(), pending.end());
    pending.erase(std::unique(pending.begin(), pending.end()), pending.end());
    for (InstanceHandle handle : pending) {
        g_watcher->ReleaseReference(handle);
    }
}

// Notifications arrive from inside XAML's own walk over the tree; releasing
// references there would destroy elements in the middle of it. That's why
// the releases go to a timer of the thread's dispatcher.
void QueueRelease(InstanceHandle handle) {
    if (!handle) {
        return;
    }
    ThreadState& state = CurrentThreadState();
    state.pendingReleases.push_back(handle);
    if (state.releaseArmed) {
        return;
    }
    try {
        if (!state.releaseTimer) {
            auto queue =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (!queue) {
                return;
            }
            state.releaseTimer = queue.CreateTimer();
            state.releaseTimer.IsRepeating(false);
            state.releaseTimer.Interval(std::chrono::milliseconds{1000});
            state.releaseTimerToken = state.releaseTimer.Tick(
                [](const winrt::Windows::System::DispatcherQueueTimer&,
                   const wf::IInspectable&) { DrainReleases(); });
        }
        state.releaseTimer.Start();
        state.releaseArmed = true;
    } catch (...) {
        Wh_Log(L"Timer error: %08X", winrt::to_hresult());
    }
}

void StopReleasesCurrentThread() {
    ThreadState& state = CurrentThreadState();
    if (state.releaseTimer) {
        try {
            state.releaseTimer.Stop();
            state.releaseTimer.Tick(state.releaseTimerToken);
        } catch (...) {
        }
        state.releaseTimer = nullptr;
    }
    DrainReleases();
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation relation,
                                              VisualElement element,
                                              VisualMutationType mutationType) try {
    if (mutationType == Add) {
        try {
            wf::IInspectable inspectable;
            winrt::check_hresult(m_diagnostics->GetIInspectableFromHandle(
                element.Handle,
                reinterpret_cast<::IInspectable**>(winrt::put_abi(inspectable))));
            if (auto frameworkElement =
                    inspectable.try_as<wux::FrameworkElement>()) {
                ApplyToElement(frameworkElement,
                               element.Type ? element.Type : L"");
            }
        } catch (...) {
            Wh_Log(L"Erro: %08X", winrt::to_hresult());
        }
        QueueRelease(element.Handle);
        QueueRelease(relation.Parent);
    } else if (mutationType == Remove) {
        QueueRelease(element.Handle);
    }
    return S_OK;
} catch (...) {
    // Returning an error makes XAML stop sending notifications.
    return S_OK;
}

class Tap : public winrt::implements<Tap, IObjectWithSite, winrt::non_agile> {
   public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) override try {
        if (g_watcher) {
            g_watcher->UnadviseVisualTreeChange();
            g_watcher = nullptr;
        }
        m_site.copy_from(site);
        if (m_site) {
            // Undoes the reference to the DLL that InitializeXamlDiagnosticsEx took.
            FreeLibrary(GetCurrentModuleHandle());
            g_watcher = winrt::make_self<VisualTreeWatcher>(m_site);
        }
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** site) noexcept override {
        return m_site.as(riid, site);
    }

   private:
    winrt::com_ptr<IUnknown> m_site;
};

// {6F0C4B5E-2B7A-4C8E-9A43-1D2E7F5B9C31}
constexpr CLSID CLSID_GlobalMenuTap = {
    0x6f0c4b5e, 0x2b7a, 0x4c8e, {0x9a, 0x43, 0x1d, 0x2e, 0x7f, 0x5b, 0x9c, 0x31}};

struct TapFactory
    : winrt::implements<TapFactory, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer,
                                             REFIID riid,
                                             void** object) override try {
        *object = nullptr;
        if (outer) {
            return CLASS_E_NOAGGREGATION;
        }
        return winrt::make<Tap>().as(riid, object);
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override { return S_OK; }
};

void InjectTapOnce() {
    if (g_unloading || g_tapInjected.exchange(true)) {
        return;
    }

    HMODULE module = GetCurrentModuleHandle();
    WCHAR location[MAX_PATH];
    DWORD len = module ? GetModuleFileNameW(module, location, ARRAYSIZE(location))
                       : 0;
    HMODULE xaml = LoadLibraryExW(L"Windows.UI.Xaml.dll", nullptr,
                                  LOAD_LIBRARY_SEARCH_SYSTEM32);
    using InitializeXamlDiagnosticsEx_t =
        HRESULT(WINAPI*)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, CLSID, LPCWSTR);
    auto initialize =
        xaml ? (InitializeXamlDiagnosticsEx_t)GetProcAddress(
                   xaml, "InitializeXamlDiagnosticsEx")
             : nullptr;
    if (!len || len == ARRAYSIZE(location) || !initialize) {
        Wh_Log(L"XAML Diagnostics unavailable");
        return;
    }

    // There's no way to know the name of the free connection; tries until one is found.
    HRESULT hr = E_FAIL;
    for (int i = 1; i <= 10000; i++) {
        WCHAR connection[64];
        swprintf(connection, ARRAYSIZE(connection), L"VisualDiagConnection%d", i);
        hr = initialize(connection, GetCurrentProcessId(), L"", location,
                        CLSID_GlobalMenuTap, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }
    if (FAILED(hr)) {
        Wh_Log(L"InitializeXamlDiagnosticsEx failed: %08X", hr);
    } else {
        Wh_Log(L"XAML Diagnostics connected");
    }
}

// In ShellHost, depending on the version, quick settings live in a
// ControlCenterWindow or in a CoreWindow.
bool IsTargetWindow(HWND hwnd) {
    return IsWindowOfClass(hwnd, L"Windows.UI.Core.CoreWindow") ||
           (g_target == Target::ShellHost &&
            IsWindowOfClass(hwnd, L"ControlCenterWindow"));
}

// Outside the primary monitor, ShellHost limits quick settings with a region
// stuck to the bottom, and the top of the content (which the mod put at the
// top) gets cut. The region now starts at the top of the window.
using NtUserSetWindowRgn_t = int(WINAPI*)(HWND, HRGN, BOOL);
NtUserSetWindowRgn_t NtUserSetWindowRgn_orig;

int WINAPI NtUserSetWindowRgn_hook(HWND hwnd, HRGN rgn, BOOL redraw) {
    RECT box, rc;
    if (!g_unloading && rgn && IsTargetWindow(hwnd) &&
        GetRgnBox(rgn, &box) != ERROR && box.top > 0 &&
        GetWindowRect(hwnd, &rc) && box.bottom >= rc.bottom - rc.top - 4 &&
        box.bottom > box.top) {
        MONITORINFO mi{sizeof(mi)};
        GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);
        UINT dpi = GetDpiForWindow(hwnd);
        if (!dpi) {
            dpi = 96;
        }
        double windowTopDip = (rc.top - mi.rcMonitor.top) * 96.0 / dpi;
        double topDip = std::max(0.0, g_barHeight + g_gap - windowTopDip) + 8;
        int bottom = std::min<int>(
            rc.bottom - rc.top,
            (box.bottom - box.top) + MulDiv((int)std::ceil(topDip), dpi, 96));
        if (HRGN moved = CreateRectRgn(0, 0, rc.right - rc.left, bottom)) {
            Wh_Log(L"Quick settings region: %d..%d -> 0..%d",
                   box.top, box.bottom, bottom);
            DeleteObject(rgn);
            rgn = moved;
        }
    }
    return NtUserSetWindowRgn_orig(hwnd, rgn, redraw);
}

// Plan B for versions where the flyout window has the size of the content
// instead of the screen height: mirrors the position from bottom to top.
using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_orig;

BOOL WINAPI SetWindowPos_hook(HWND hwnd,
                              HWND insertAfter,
                              int x,
                              int y,
                              int cx,
                              int cy,
                              UINT flags) {
    if (g_unloading || (flags & SWP_NOMOVE) || !IsTargetWindow(hwnd)) {
        return SetWindowPos_orig(hwnd, insertAfter, x, y, cx, cy, flags);
    }

    int width = cx, height = cy;
    if (flags & SWP_NOSIZE) {
        RECT rc;
        GetWindowRect(hwnd, &rc);
        width = rc.right - rc.left;
        height = rc.bottom - rc.top;
    }
    RECT target{x, y, x + width, y + height};
    MONITORINFO mi{sizeof(mi)};
    if (width > 1 && height > 1 &&
        GetMonitorInfoW(MonitorFromRect(&target, MONITOR_DEFAULTTONEAREST),
                        &mi)) {
        const RECT& work = mi.rcWork;
        int gap = work.bottom - (y + height);
        if (height < (work.bottom - work.top) - 4 && gap >= -4 && gap <= 96) {
            UINT dpi = GetDpiForWindow(hwnd);
            int newY = std::max<int>(work.top, mi.rcMonitor.top +
                                                   MulDiv(g_barHeight, dpi, 96)) +
                       MulDiv(g_gap, dpi, 96);
            Wh_Log(L"Flyout moved from y=%d to y=%d", y, newY);
            y = newY;
        }
    }

    // Takes quick settings to the monitor of the click on the bar, at the
    // same position relative to the work area, with the size converted to
    // that monitor's DPI.
    MONITORINFO src{sizeof(src)}, dst{sizeof(dst)};
    HMONITOR source = MonitorFromRect(&target, MONITOR_DEFAULTTONEAREST);
    HMONITOR destination = GetFlyoutTarget();
    if (g_target == Target::ShellHost && !(flags & SWP_NOSIZE) && width > 1 &&
        height > 1 && destination && destination != source &&
        GetMonitorInfoW(source, &src) && GetMonitorInfoW(destination, &dst)) {
        double scale =
            (double)MonitorDpi(destination) / MonitorDpi(source);
        int right = (int)((src.rcWork.right - (x + cx)) * scale);
        int top = (int)((y - src.rcWork.top) * scale);
        cx = (int)(cx * scale);
        y = dst.rcWork.top + top;
        cy = std::min<int>((int)(cy * scale), dst.rcWork.bottom - y);
        x = dst.rcWork.right - right - cx;
        Wh_Log(L"Quick settings moved to (%d,%d) %dx%d", x, y, cx,
               cy);
    }
    return SetWindowPos_orig(hwnd, insertAfter, x, y, cx, cy, flags);
}

BOOL CALLBACK CollectTargetWindowsProc(HWND hwnd, LPARAM param) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && IsTargetWindow(hwnd)) {
        ((std::vector<HWND>*)param)->push_back(hwnd);
    }
    return TRUE;
}

std::vector<HWND> GetTargetWindows() {
    std::vector<HWND> windows;
    EnumWindows(CollectTargetWindowsProc, (LPARAM)&windows);
    return windows;
}

using ThreadProc_t = void (*)();

UINT RunFromWindowThreadMsg() {
    static UINT msg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return msg;
}

LRESULT CALLBACK RunFromWindowThreadHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == RunFromWindowThreadMsg()) {
            ((ThreadProc_t)cwp->lParam)();
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Runs proc on the window's thread, synchronously.
void RunFromWindowThread(HWND hwnd, ThreadProc_t proc) {
    DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (!threadId) {
        return;
    }
    if (threadId == GetCurrentThreadId()) {
        proc();
        return;
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunFromWindowThreadHook,
                                   nullptr, threadId);
    if (!hook) {
        return;
    }
    SendMessageW(hwnd, RunFromWindowThreadMsg(), 0, (LPARAM)proc);
    UnhookWindowsHookEx(hook);
}

// Asynchronous version: ControlCenterWindow doesn't have XAML yet when it's created.
UINT RunLaterMsg() {
    static UINT msg = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThreadLater_" WH_MOD_ID);
    return msg;
}

HHOOK g_runLaterHook;

LRESULT CALLBACK RunLaterHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* msg = (const MSG*)lParam;
        if (wParam == PM_REMOVE && msg->message == RunLaterMsg()) {
            HHOOK hook = g_runLaterHook;
            g_runLaterHook = nullptr;
            ((ThreadProc_t)msg->lParam)();
            if (hook) {
                UnhookWindowsHookEx(hook);
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void RunFromWindowThreadLater(HWND hwnd, ThreadProc_t proc) {
    DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (!threadId || g_runLaterHook) {
        return;
    }
    g_runLaterHook =
        SetWindowsHookExW(WH_GETMESSAGE, RunLaterHook, nullptr, threadId);
    if (g_runLaterHook && !PostMessageW(hwnd, RunLaterMsg(), 0, (LPARAM)proc)) {
        UnhookWindowsHookEx(g_runLaterHook);
        g_runLaterHook = nullptr;
    }
}

void OnWindowCreated(HWND hwnd, LPCWSTR className) {
    if (((ULONG_PTR)className & ~(ULONG_PTR)0xFFFF) == 0) {
        return;  // atom, not a name
    }
    Wh_Log(L"Window created: %s", className);
    if (g_target == Target::ShellExperienceHost &&
        !_wcsicmp(className, L"Windows.UI.Core.CoreWindow")) {
        InjectTapOnce();
    } else if (g_target == Target::ShellHost &&
               (!_wcsicmp(className, L"ControlCenterWindow") ||
                !_wcsicmp(className, L"Windows.UI.Core.CoreWindow"))) {
        RunFromWindowThreadLater(hwnd, InjectTapOnce);
    }
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD,
                                           LPCWSTR,
                                           LPCWSTR,
                                           DWORD,
                                           int,
                                           int,
                                           int,
                                           int,
                                           HWND,
                                           HMENU,
                                           HINSTANCE,
                                           PVOID,
                                           DWORD);
CreateWindowInBand_t CreateWindowInBand_orig;

HWND WINAPI CreateWindowInBand_hook(DWORD exStyle,
                                    LPCWSTR className,
                                    LPCWSTR windowName,
                                    DWORD style,
                                    int x,
                                    int y,
                                    int width,
                                    int height,
                                    HWND parent,
                                    HMENU menu,
                                    HINSTANCE instance,
                                    PVOID param,
                                    DWORD band) {
    HWND hwnd = CreateWindowInBand_orig(exStyle, className, windowName, style,
                                        x, y, width, height, parent, menu,
                                        instance, param, band);
    if (hwnd) {
        OnWindowCreated(hwnd, className);
    }
    return hwnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD,
                                             LPCWSTR,
                                             LPCWSTR,
                                             DWORD,
                                             int,
                                             int,
                                             int,
                                             int,
                                             HWND,
                                             HMENU,
                                             HINSTANCE,
                                             PVOID,
                                             DWORD,
                                             DWORD);
CreateWindowInBandEx_t CreateWindowInBandEx_orig;

HWND WINAPI CreateWindowInBandEx_hook(DWORD exStyle,
                                      LPCWSTR className,
                                      LPCWSTR windowName,
                                      DWORD style,
                                      int x,
                                      int y,
                                      int width,
                                      int height,
                                      HWND parent,
                                      HMENU menu,
                                      HINSTANCE instance,
                                      PVOID param,
                                      DWORD band,
                                      DWORD typeFlags) {
    HWND hwnd = CreateWindowInBandEx_orig(exStyle, className, windowName, style,
                                          x, y, width, height, parent, menu,
                                          instance, param, band, typeFlags);
    if (hwnd) {
        OnWindowCreated(hwnd, className);
    }
    return hwnd;
}

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
RegOpenKeyExW_t RegOpenKeyExW_orig;

LSTATUS WINAPI RegOpenKeyExW_hook(HKEY key,
                                  LPCWSTR subKey,
                                  DWORD options,
                                  REGSAM sam,
                                  PHKEY result) {
    LSTATUS status = RegOpenKeyExW_orig(key, subKey, options, sam, result);
    if (status == ERROR_SUCCESS || !g_reportCompositionDiagAsDisabled ||
        key != HKEY_LOCAL_MACHINE || !subKey ||
        _wcsicmp(subKey, L"Software\\Microsoft\\XAML\\Debug") != 0) {
        return status;
    }
    // The key usually doesn't exist, and the value is only read if it opens.
    return RegOpenKeyExW_orig(HKEY_LOCAL_MACHINE, L"Software\\Microsoft",
                              options, sam, result);
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_orig;

LSTATUS WINAPI RegQueryValueExW_hook(HKEY key,
                                     LPCWSTR valueName,
                                     LPDWORD reserved,
                                     LPDWORD type,
                                     LPBYTE data,
                                     LPDWORD dataSize) {
    if (!g_reportCompositionDiagAsDisabled || !valueName ||
        _wcsicmp(valueName, L"DisableCompositionDiag") != 0) {
        return RegQueryValueExW_orig(key, valueName, reserved, type, data,
                                     dataSize);
    }
    if (type) {
        *type = REG_DWORD;
    }
    if (data && (!dataSize || *dataSize < sizeof(DWORD))) {
        if (dataSize) {
            *dataSize = sizeof(DWORD);
        }
        return ERROR_MORE_DATA;
    }
    if (data) {
        *(DWORD*)data = 1;
    }
    if (dataSize) {
        *dataSize = sizeof(DWORD);
    }
    return ERROR_SUCCESS;
}

// In explorer, XAML Diagnostics is only used to find the taskbars'
// clocks.
void InitExplorer() {
    g_target = Target::Explorer;
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (kernelBase) {
        if (void* p = (void*)GetProcAddress(kernelBase, "RegOpenKeyExW")) {
            Wh_SetFunctionHook(p, (void*)RegOpenKeyExW_hook,
                               (void**)&RegOpenKeyExW_orig);
        }
        if (void* p = (void*)GetProcAddress(kernelBase, "RegQueryValueExW")) {
            Wh_SetFunctionHook(p, (void*)RegQueryValueExW_hook,
                               (void**)&RegQueryValueExW_orig);
        }
    }
}

bool IsFlyoutProcess() {
    std::wstring name = ToLower(FileNameFromPath(GetCurrentProcessPath()));
    return name == L"shellexperiencehost.exe" || name == L"shellhost.exe";
}

BOOL Init() {
    if (!Wh_GetIntSetting(L"flyoutsOnTop")) {
        return FALSE;
    }
    g_animateFromTop = Wh_GetIntSetting(L"flyoutsAnimateFromTop") != 0;
    g_barHeight = std::clamp(Wh_GetIntSetting(L"height"), 20, 64);
    g_gap = std::clamp(Wh_GetIntSetting(L"flyoutsGap"), 0, 200);
    g_target = ToLower(FileNameFromPath(GetCurrentProcessPath())) ==
                       L"shellhost.exe"
                   ? Target::ShellHost
                   : Target::ShellExperienceHost;

    HMODULE user32 =
        LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32) {
        if (void* p = (void*)GetProcAddress(user32, "CreateWindowInBand")) {
            Wh_SetFunctionHook(p, (void*)CreateWindowInBand_hook,
                               (void**)&CreateWindowInBand_orig);
        }
        if (void* p = (void*)GetProcAddress(user32, "CreateWindowInBandEx")) {
            Wh_SetFunctionHook(p, (void*)CreateWindowInBandEx_hook,
                               (void**)&CreateWindowInBandEx_orig);
        }
        if (void* p = (void*)GetProcAddress(user32, "SetWindowPos")) {
            Wh_SetFunctionHook(p, (void*)SetWindowPos_hook,
                               (void**)&SetWindowPos_orig);
        }
    }
    HMODULE win32u = GetModuleHandleW(L"win32u.dll");
    if (win32u && g_target == Target::ShellHost) {
        if (void* p = (void*)GetProcAddress(win32u, "NtUserSetWindowRgn")) {
            Wh_SetFunctionHook(p, (void*)NtUserSetWindowRgn_hook,
                               (void**)&NtUserSetWindowRgn_orig);
        }

    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (kernelBase) {
        if (void* p = (void*)GetProcAddress(kernelBase, "RegOpenKeyExW")) {
            Wh_SetFunctionHook(p, (void*)RegOpenKeyExW_hook,
                               (void**)&RegOpenKeyExW_orig);
        }
        if (void* p = (void*)GetProcAddress(kernelBase, "RegQueryValueExW")) {
            Wh_SetFunctionHook(p, (void*)RegQueryValueExW_hook,
                               (void**)&RegQueryValueExW_orig);
        }
    }
    return TRUE;
}

BOOL CALLBACK FindAnyThreadWindowProc(HWND hwnd, LPARAM param) {
    *(HWND*)param = hwnd;
    return FALSE;
}

void AfterInit() {
    if (g_target == Target::Explorer) {
        if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
            RunFromWindowThread(tray, InjectTapOnce);
        }
        return;
    }

    // If the process already has XAML loaded (the flyout was opened before
    // the mod loaded, or the flyout window is permanent in this version),
    // connects now, through the thread of one of its windows.
    std::vector<HWND> windows = GetTargetWindows();
    bool xaml = GetModuleHandleW(L"Windows.UI.Xaml.dll") != nullptr;
    Wh_Log(L"Windows found: %d, XAML %s", (int)windows.size(),
           xaml ? L"loaded" : L"not loaded");
    if (xaml && !windows.empty()) {
        RunFromWindowThread(windows.front(), InjectTapOnce);
    }
}

void Uninit() {
    g_unloading = true;

    if (g_watcher) {
        g_watcher->UnadviseVisualTreeChange();
    }

    // Undoes the changes on each thread that touched some element.
    std::vector<DWORD> threadIds;
    {
        std::lock_guard<std::mutex> lock(g_threadsMutex);
        for (const auto& [threadId, state] : g_threads) {
            threadIds.push_back(threadId);
        }
    }
    for (DWORD threadId : threadIds) {
        HWND hwnd = nullptr;
        EnumThreadWindows(threadId, FindAnyThreadWindowProc, (LPARAM)&hwnd);
        if (hwnd) {
            RunFromWindowThread(hwnd, [] {
                StopReleasesCurrentThread();
                RestoreCurrentThread();
            });
        }
    }
    g_watcher = nullptr;

    {
        std::lock_guard<std::mutex> lock(g_clockButtonsMutex);
        g_clockButtons.clear();
    }
    std::lock_guard<std::mutex> lock(g_threadsMutex);
    g_threads.clear();
}

}  // namespace flyouts

enum class ProcessRole { Explorer, Flyouts, Agent };
ProcessRole g_role;

bool IsExplorerProcess() {
    return ToLower(FileNameFromPath(GetCurrentProcessPath())) == L"explorer.exe";
}

}  // namespace

// COM entry point that XAML Diagnostics uses to create the TAP.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
__declspec(dllexport) STDAPI DllGetClassObject(REFCLSID rclsid,
                                               REFIID riid,
                                               LPVOID* ppv) try {
    *ppv = nullptr;
    if (rclsid != flyouts::CLSID_GlobalMenuTap) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    return winrt::make<flyouts::TapFactory>().as(riid, ppv);
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport) STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}
#pragma clang diagnostic pop

////////////////////////////////////////////////////////////////////////////////
// Notification Center region: runs in explorer.
//
// Explorer limits the Notification Center window (ShellExperienceHost)
// with SetWindowRgn to the area the content takes at the bottom, and redoes
// it on every opening. With the content moved to the top by the "flyouts"
// part, the notifications and the date got cut and only the calendar showed,
// in the middle of the screen. The region now starts at the top of the window.

namespace panelregion {

using SetWindowRgn_t = decltype(&SetWindowRgn);
SetWindowRgn_t SetWindowRgn_orig;
int g_barHeight = 28;
int g_gap = 8;

bool IsNotificationPanel(HWND hwnd) {
    if (!IsWindowOfClass(hwnd, L"Windows.UI.Core.CoreWindow")) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }
    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    bool result = QueryFullProcessImageNameW(process, 0, path, &size) &&
                  ToLower(FileNameFromPath(path)) == L"shellexperiencehost.exe";
    CloseHandle(process);
    return result;
}

int WINAPI SetWindowRgn_hook(HWND hwnd, HRGN rgn, BOOL redraw) {
    RECT box, rc;
    if (rgn && GetRgnBox(rgn, &box) != ERROR && box.top > 0 &&
        box.bottom > box.top && GetWindowRect(hwnd, &rc) &&
        IsNotificationPanel(hwnd)) {
        MONITORINFO mi{sizeof(mi)};
        GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                        &mi);
        UINT dpi = GetDpiForWindow(hwnd);
        if (!dpi) {
            dpi = 96;
        }
        // The same offset the "flyouts" part applies to the content, plus the
        // RootGrid margin (8).
        double windowTopDip = (rc.top - mi.rcMonitor.top) * 96.0 / dpi;
        double topDip =
            std::max(0.0, g_barHeight + g_gap - windowTopDip) + 8;
        int height = rc.bottom - rc.top;
        int bottom = std::min(height, (int)(box.bottom - box.top) +
                                          MulDiv((int)std::ceil(topDip),
                                                 dpi, 96));
        if (HRGN moved = CreateRectRgn(box.left, 0, box.right, bottom)) {
            Wh_Log(L"Notification Center region: %d..%d -> 0..%d", box.top,
                   box.bottom, bottom);
            DeleteObject(rgn);
            rgn = moved;
        }
    }
    return SetWindowRgn_orig(hwnd, rgn, redraw);
}

void Init() {
    if (!Wh_GetIntSetting(L"flyoutsOnTop")) {
        return;
    }
    g_barHeight = std::clamp(Wh_GetIntSetting(L"height"), 20, 64);
    g_gap = std::clamp(Wh_GetIntSetting(L"flyoutsGap"), 0, 200);
    Wh_SetFunctionHook((void*)SetWindowRgn, (void*)SetWindowRgn_hook,
                       (void**)&SetWindowRgn_orig);
}

}  // namespace panelregion

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (IsExplorerProcess()) {
        g_role = ProcessRole::Explorer;
        if (!bar::Init()) {
            return FALSE;
        }
        panelregion::Init();
        flyouts::InitExplorer();
        return TRUE;
    }
    if (flyouts::IsFlyoutProcess()) {
        g_role = ProcessRole::Flyouts;
        return flyouts::Init();
    }
    g_role = ProcessRole::Agent;
    return agent::Init();
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_role == ProcessRole::Flyouts || g_role == ProcessRole::Explorer) {
        flyouts::AfterInit();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    if (g_role == ProcessRole::Agent) {
        agent::BeforeUninit();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_role == ProcessRole::Explorer) {
        bar::Uninit();
        flyouts::Uninit();
    } else if (g_role == ProcessRole::Flyouts) {
        flyouts::Uninit();
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    switch (g_role) {
        case ProcessRole::Explorer:
            bar::SettingsChanged();
            break;
        case ProcessRole::Flyouts:
            // Reloading reapplies everything: XAML Diagnostics reports the whole
            // tree again when connecting.
            *bReload = TRUE;
            break;
        case ProcessRole::Agent:
            agent::SettingsChanged();
            break;
    }
    return TRUE;
}
