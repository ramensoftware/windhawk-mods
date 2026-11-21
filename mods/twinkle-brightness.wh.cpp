// ==WindhawkMod==
// @id              twinkle-brightness
// @name            Twinkle Brightness
// @description     Brightness sliders for every display in the system tray, with hotkeys, an on-screen indicator, time-based schedules, idle dimming and mouse wheel control
// @version         1.1.0
// @author          isousluer
// @github          https://github.com/isousluer
// @homepage        https://github.com/isousluer/twinkle-brightness
// @include         windhawk.exe
// @compilerOptions -ldxva2 -lgdiplus -ldwmapi -lshcore -lgdi32 -luser32 -lshell32 -lole32 -loleaut32 -lwbemuuid -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// Source code is published under The MIT License.
// https://github.com/isousluer/twinkle-brightness

// ==WindhawkModReadme==
/*
# Twinkle Brightness

Control the brightness of **all your displays** - external monitors and laptop
screens - from a tray icon, the keyboard or the mouse wheel. Inspired by the
excellent [Twinkle Tray](https://github.com/xanderfrangos/twinkle-tray) app,
re-imagined as a lightweight Windhawk mod with no Electron runtime.

![Screenshot](https://raw.githubusercontent.com/isousluer/twinkle-brightness/main/assets/screenshot-panel.png)

## Features

- **Tray panel:** click the sun icon in the notification area to get a
  brightness slider for every display. The panel follows the Windows
  light/dark theme and accent color.
- **Link displays:** the chain button in the panel links the displays. While
  linked, one slider sets all displays to the same level, and the hotkeys and
  the mouse wheel change all of them. While unlinked, each display has its own
  slider, the hotkeys change the display with the active window and the mouse
  wheel changes the display under the cursor.
- **Hotkeys:** bind any key combination to increase, decrease or set the
  brightness, turn the displays off, or open the panel. A hotkey can also
  target a specific display.
- **On-screen indicator:** a small overlay shows the new level when you use a
  hotkey or the mouse wheel, with one row per display when they are at
  different levels.
- **Mouse wheel:** scroll over the tray icon (or anywhere on the taskbar) to
  change the brightness.
- **Schedule:** change the brightness automatically at given times of the day,
  with a smooth transition.
- **Idle dimming:** dim the displays after a period of inactivity and restore
  them as soon as you touch the mouse or keyboard.
- **Per-display options:** custom names, hiding displays, and min/max
  brightness to normalize displays that are brighter or darker than others.
- **Tray menu:** refresh displays, turn displays off, pause automatic
  brightness, and copy display names/IDs for use in the settings.

## How it works

- **External monitors** are controlled with **DDC/CI** (VCP code `0x10`), the
  same channel the monitor's own on-screen menu uses. DDC/CI must be enabled in
  the monitor's menu. Some docks, adapters, KVM switches and VGA/DVI
  connections do not pass DDC/CI through.
- **Laptop and tablet screens** are controlled through **WMI**, like the
  Windows brightness slider.

The mod runs in its own dedicated `windhawk.exe` process (Windhawk "tool mod")
and does not inject code into Explorer or any other program.

## Hotkey format

Combine `Ctrl`, `Alt`, `Shift` and `Win` with one key, separated by `+`, for
example `Ctrl+Alt+Up`, `Win+Shift+PageDown` or `Ctrl+Alt+F1`. Supported keys:
`A`-`Z`, `0`-`9`, `F1`-`F24`, `Up`, `Down`, `Left`, `Right`, `PageUp`,
`PageDown`, `Home`, `End`, `Insert`, `Delete`, `Space`, `Plus`, `Minus`,
`NumpadPlus`, `NumpadMinus`, `Numpad0`-`Numpad9`.

## Choosing displays in the settings

Hotkeys, schedule entries and per-display options accept a part of a display's
**name** (as shown in the panel) or **ID**. Right-click the tray icon and choose
**Copy display info** to copy the names and IDs of all connected displays.
Leave the field empty to target all displays (for hotkeys: to follow the
link button).

## Troubleshooting

- **A monitor says "not supported":** enable DDC/CI in its on-screen menu, use a
  direct HDMI/DisplayPort/USB-C cable instead of a dock or adapter, then choose
  **Refresh displays** in the tray menu.
- **A monitor stops responding after sleep:** choose **Refresh displays**, or
  turn the monitor off and on with its power button.
- **A hotkey does nothing:** another program may already use that combination.
  Check the Windhawk log for "RegisterHotKey failed". The default
  `Ctrl+Alt+Up`/`Ctrl+Alt+Down` are also used by Visual Studio Code (add a
  cursor) and some Intel graphics drivers (rotate the screen); change them in
  the settings if you need those shortcuts.
- **The AMD Radeon software can interfere** with DDC/CI brightness. Disable
  "Custom Colors" there if brightness changes have no effect.

This mod is not affiliated with Twinkle Tray or its author.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: auto
  $name: Panel theme
  $options:
  - auto: Follow Windows
  - dark: Dark
  - light: Light
- showOsd: true
  $name: Show the on-screen indicator
  $description: Shows the new brightness level when you use a hotkey or the mouse wheel
- hotkeys:
  - - keys: Ctrl+Alt+Up
      $name: Key combination
      $description: For example Ctrl+Alt+Up, Win+Shift+PageDown or Ctrl+Alt+F1. Leave empty to disable this entry.
    - action: increase
      $name: Action
      $options:
      - increase: Increase brightness
      - decrease: Decrease brightness
      - set: Set brightness
      - turnOff: Turn off displays
      - panel: Open the brightness panel
    - value: 10
      $name: Amount (%)
      $description: Step for increase/decrease, or the level for "Set brightness"
    - display: ""
      $name: Display
      $description: Part of the display's name or ID. Leave empty to follow the panel's link button, which changes all displays when it's on and the display with the active window when it's off.
  - - keys: Ctrl+Alt+Down
    - action: decrease
    - value: 10
    - display: ""
  $name: Hotkeys
- wheel:
  - mode: trayIcon
    $name: Change brightness with the mouse wheel
    $options:
    - off: Disabled
    - trayIcon: Over the tray icon
    - taskbar: Anywhere on the taskbar
  - modifier: none
    $name: Required modifier key
    $options:
    - none: None
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
  - step: 2
    $name: Step per wheel notch (%)
    $description: The wheel changes all displays while the panel's link button is on, and the display under the mouse cursor while it's off.
  $name: Mouse wheel
- schedule:
  - - time: ""
      $name: Time
      $description: 24-hour time, for example 07:30 or 21:00. Leave empty to disable this entry.
    - brightness: 50
      $name: Brightness (%)
    - display: ""
      $name: Display
      $description: Part of the display's name or ID. Leave empty for all displays.
  $name: Brightness schedule
  $description: Changes the brightness automatically at the given times of the day
- scheduleOptions:
  - transitionSeconds: 30
    $name: Transition duration (seconds)
    $description: How long the change to a scheduled level takes. 0 changes it instantly.
  - applyOnStartup: true
    $name: Apply the latest entry on startup and after sleep
  $name: Schedule options
- idle:
  - enabled: false
    $name: Dim the displays when idle
  - minutes: 5
    $name: Idle time (minutes)
  - brightness: 10
    $name: Dimmed brightness (%)
    $description: Displays that are already darker than this are left alone
  - ignoreFullscreen: true
    $name: Don't dim while a full-screen app is running
    $description: For example while watching a video or giving a presentation
  $name: Idle dimming
- displays:
  - - match: ""
      $name: Display name or ID
      $description: Part of the display's name or ID. Use "Copy display info" in the tray menu to find it.
    - name: ""
      $name: Custom name
    - minBrightness: 0
      $name: Minimum brightness (%)
      $description: Hardware level used when the slider is at 0%. Use with the maximum to normalize displays.
    - maxBrightness: 100
      $name: Maximum brightness (%)
      $description: Hardware level used when the slider is at 100%
    - hide: false
      $name: Hide this display
  $name: Per-display options
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>

#include <dwmapi.h>
#include <gdiplus.h>
#include <lowlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <sddl.h>
#include <wbemidl.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <cmath>
#include <condition_variable>
#include <cwctype>
#include <map>
#include <mutex>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Constants

constexpr UINT WM_APP_TRAY = WM_APP + 1;
constexpr UINT WM_APP_SNAPSHOT = WM_APP + 2;  // wParam: 1 = full enumeration
constexpr UINT WM_APP_SETTINGS = WM_APP + 3;
constexpr UINT WM_APP_SHUTDOWN = WM_APP + 4;
constexpr UINT WM_APP_WHEEL = WM_APP + 5;

constexpr UINT_PTR TIMER_REENUM = 1;
constexpr UINT_PTR TIMER_SCHEDULE = 2;
constexpr UINT_PTR TIMER_IDLE = 3;
constexpr UINT_PTR TIMER_FADE = 4;
constexpr UINT_PTR TIMER_TOOLTIP = 5;
constexpr UINT_PTR TIMER_MONITOR_OFF = 7;

constexpr UINT_PTR TIMER_OSD_HIDE = 1;
constexpr UINT_PTR TIMER_OSD_FADE = 2;

constexpr UINT TRAY_ICON_ID = 1;
// {7A3C9E52-4B1D-4F8A-9C6E-2D5B8F1A0E37}
constexpr GUID TRAY_ICON_GUID = {
    0x7a3c9e52,
    0x4b1d,
    0x4f8a,
    {0x9c, 0x6e, 0x2d, 0x5b, 0x8f, 0x1a, 0x0e, 0x37}};

enum MenuId : UINT {
    MENU_REFRESH = 1,
    MENU_TURN_OFF,
    MENU_PAUSE,
    MENU_COPY_INFO,
};

enum class HotkeyAction { Increase, Decrease, Set, TurnOff, Panel };
enum class WheelMode { Off, TrayIcon, Taskbar };
enum class ThemeMode { Auto, Dark, Light };

////////////////////////////////////////////////////////////////////////////////
// Settings

struct HotkeyConfig {
    UINT modifiers = 0;
    UINT vk = 0;
    HotkeyAction action = HotkeyAction::Increase;
    int value = 10;
    std::wstring display;
    std::wstring text;
};

struct ScheduleEntry {
    int minuteOfDay = 0;
    int brightness = 50;
    std::wstring display;
};

struct DisplayConfig {
    std::wstring match;
    std::wstring name;
    int minBrightness = 0;
    int maxBrightness = 100;
    bool hide = false;
};

struct Settings {
    ThemeMode theme = ThemeMode::Auto;
    bool showOsd = true;
    std::vector<HotkeyConfig> hotkeys;
    WheelMode wheelMode = WheelMode::TrayIcon;
    int wheelModifier = 0;  // VK code, 0 = none
    int wheelStep = 2;
    std::vector<ScheduleEntry> schedule;
    int transitionSeconds = 30;
    bool applyScheduleOnStartup = true;
    bool idleEnabled = false;
    int idleMinutes = 5;
    int idleBrightness = 10;
    bool idleIgnoreFullscreen = true;
    std::vector<DisplayConfig> displays;
};

Settings g_settings;

////////////////////////////////////////////////////////////////////////////////
// Helpers

std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = (wchar_t)towlower(c);
    }
    return s;
}

std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos) {
        return L"";
    }
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}

bool ContainsI(const std::wstring& haystack, const std::wstring& needle) {
    if (needle.empty()) {
        return false;
    }
    return ToLower(haystack).find(ToLower(needle)) != std::wstring::npos;
}

std::wstring GetStringSetting(const std::wstring& name) {
    PCWSTR value = Wh_GetStringSetting(name.c_str());
    std::wstring result = value;
    Wh_FreeStringSetting(value);
    return Trim(result);
}

int GetIntSetting(const std::wstring& name) {
    return Wh_GetIntSetting(name.c_str());
}

std::wstring IndexedName(PCWSTR array, int index, PCWSTR field) {
    return std::wstring(array) + L"[" + std::to_wstring(index) + L"]." + field;
}

int ClampPercent(int v) {
    return std::clamp(v, 0, 100);
}

UINT KeyNameToVk(const std::wstring& name) {
    if (name.empty()) {
        return 0;
    }
    if (name.size() == 1) {
        wchar_t c = (wchar_t)towupper(name[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            return c;
        }
    }
    if (name.size() >= 2 && name[0] == L'f' && iswdigit(name[1])) {
        int n = _wtoi(name.c_str() + 1);
        if (n >= 1 && n <= 24) {
            return VK_F1 + n - 1;
        }
    }
    if (name.size() == 7 && name.rfind(L"numpad", 0) == 0 &&
        iswdigit(name[6])) {
        return VK_NUMPAD0 + (name[6] - L'0');
    }

    static const struct {
        PCWSTR name;
        UINT vk;
    } keys[] = {
        {L"up", VK_UP},
        {L"down", VK_DOWN},
        {L"left", VK_LEFT},
        {L"right", VK_RIGHT},
        {L"pageup", VK_PRIOR},
        {L"pgup", VK_PRIOR},
        {L"pagedown", VK_NEXT},
        {L"pgdn", VK_NEXT},
        {L"home", VK_HOME},
        {L"end", VK_END},
        {L"insert", VK_INSERT},
        {L"ins", VK_INSERT},
        {L"delete", VK_DELETE},
        {L"del", VK_DELETE},
        {L"space", VK_SPACE},
        {L"plus", VK_OEM_PLUS},
        {L"=", VK_OEM_PLUS},
        {L"minus", VK_OEM_MINUS},
        {L"-", VK_OEM_MINUS},
        {L"comma", VK_OEM_COMMA},
        {L"period", VK_OEM_PERIOD},
        {L"numpadplus", VK_ADD},
        {L"add", VK_ADD},
        {L"numpadminus", VK_SUBTRACT},
        {L"subtract", VK_SUBTRACT},
        {L"pause", VK_PAUSE},
        {L"scrolllock", VK_SCROLL},
    };
    for (const auto& key : keys) {
        if (name == key.name) {
            return key.vk;
        }
    }
    return 0;
}

bool ParseHotkey(const std::wstring& text, UINT* modifiers, UINT* vk) {
    *modifiers = 0;
    *vk = 0;
    size_t start = 0;
    while (start <= text.size()) {
        size_t plus = text.find(L'+', start);
        std::wstring token = ToLower(Trim(text.substr(
            start, plus == std::wstring::npos ? std::wstring::npos
                                              : plus - start)));
        if (token == L"ctrl" || token == L"control") {
            *modifiers |= MOD_CONTROL;
        } else if (token == L"alt") {
            *modifiers |= MOD_ALT;
        } else if (token == L"shift") {
            *modifiers |= MOD_SHIFT;
        } else if (token == L"win" || token == L"windows" ||
                   token == L"meta") {
            *modifiers |= MOD_WIN;
        } else if (!token.empty()) {
            if (*vk) {
                return false;
            }
            *vk = KeyNameToVk(token);
            if (!*vk) {
                return false;
            }
        }
        if (plus == std::wstring::npos) {
            break;
        }
        start = plus + 1;
    }
    return *vk != 0;
}

int ParseTime(const std::wstring& text) {
    int h = -1, m = -1;
    if (swscanf_s(text.c_str(), L"%d:%d", &h, &m) != 2) {
        return -1;
    }
    if (h < 0 || h > 23 || m < 0 || m > 59) {
        return -1;
    }
    return h * 60 + m;
}

void LoadSettings() {
    Settings s;

    std::wstring theme = GetStringSetting(L"theme");
    s.theme = theme == L"dark"    ? ThemeMode::Dark
              : theme == L"light" ? ThemeMode::Light
                                  : ThemeMode::Auto;
    s.showOsd = GetIntSetting(L"showOsd") != 0;

    for (int i = 0; i < 32; i++) {
        std::wstring keys = GetStringSetting(IndexedName(L"hotkeys", i, L"keys"));
        if (keys.empty()) {
            continue;
        }
        HotkeyConfig hk;
        hk.text = keys;
        if (!ParseHotkey(keys, &hk.modifiers, &hk.vk)) {
            Wh_Log(L"Invalid hotkey: %s", keys.c_str());
            continue;
        }
        std::wstring action =
            GetStringSetting(IndexedName(L"hotkeys", i, L"action"));
        hk.action = action == L"decrease" ? HotkeyAction::Decrease
                    : action == L"set"     ? HotkeyAction::Set
                    : action == L"turnOff" ? HotkeyAction::TurnOff
                    : action == L"panel"   ? HotkeyAction::Panel
                                           : HotkeyAction::Increase;
        hk.value = GetIntSetting(IndexedName(L"hotkeys", i, L"value"));
        hk.display = GetStringSetting(IndexedName(L"hotkeys", i, L"display"));
        s.hotkeys.push_back(std::move(hk));
    }

    std::wstring wheelMode = GetStringSetting(L"wheel.mode");
    s.wheelMode = wheelMode == L"off"       ? WheelMode::Off
                  : wheelMode == L"taskbar" ? WheelMode::Taskbar
                                            : WheelMode::TrayIcon;
    std::wstring modifier = GetStringSetting(L"wheel.modifier");
    s.wheelModifier = modifier == L"ctrl"    ? VK_CONTROL
                      : modifier == L"shift" ? VK_SHIFT
                      : modifier == L"alt"   ? VK_MENU
                                             : 0;
    s.wheelStep = std::clamp(GetIntSetting(L"wheel.step"), 1, 50);

    for (int i = 0; i < 32; i++) {
        std::wstring time =
            GetStringSetting(IndexedName(L"schedule", i, L"time"));
        if (time.empty()) {
            continue;
        }
        ScheduleEntry entry;
        entry.minuteOfDay = ParseTime(time);
        if (entry.minuteOfDay < 0) {
            Wh_Log(L"Invalid schedule time: %s", time.c_str());
            continue;
        }
        entry.brightness = ClampPercent(
            GetIntSetting(IndexedName(L"schedule", i, L"brightness")));
        entry.display =
            GetStringSetting(IndexedName(L"schedule", i, L"display"));
        s.schedule.push_back(std::move(entry));
    }
    s.transitionSeconds =
        std::clamp(GetIntSetting(L"scheduleOptions.transitionSeconds"), 0,
                   3600);
    s.applyScheduleOnStartup =
        GetIntSetting(L"scheduleOptions.applyOnStartup") != 0;

    s.idleEnabled = GetIntSetting(L"idle.enabled") != 0;
    s.idleMinutes = std::clamp(GetIntSetting(L"idle.minutes"), 1, 1440);
    s.idleBrightness = ClampPercent(GetIntSetting(L"idle.brightness"));
    s.idleIgnoreFullscreen = GetIntSetting(L"idle.ignoreFullscreen") != 0;

    for (int i = 0; i < 32; i++) {
        std::wstring match =
            GetStringSetting(IndexedName(L"displays", i, L"match"));
        if (match.empty()) {
            continue;
        }
        DisplayConfig d;
        d.match = match;
        d.name = GetStringSetting(IndexedName(L"displays", i, L"name"));
        d.minBrightness = ClampPercent(
            GetIntSetting(IndexedName(L"displays", i, L"minBrightness")));
        d.maxBrightness = ClampPercent(
            GetIntSetting(IndexedName(L"displays", i, L"maxBrightness")));
        if (d.maxBrightness <= d.minBrightness) {
            d.minBrightness = 0;
            d.maxBrightness = 100;
        }
        d.hide = GetIntSetting(IndexedName(L"displays", i, L"hide")) != 0;
        s.displays.push_back(std::move(d));
    }

    g_settings = std::move(s);
}

////////////////////////////////////////////////////////////////////////////////
// Display model shared between the hardware worker and the UI

enum class Method { None, Ddc, Wmi };

struct DisplayInfo {
    std::wstring key;   // Unique and stable while the display is connected
    std::wstring id;    // Hardware ID, e.g. DEL41B8#5&2a2b3c&0&UID4353
    std::wstring name;  // Friendly name, e.g. DELL U2720Q
    HMONITOR monitor = nullptr;
    RECT rect{};
    bool internal = false;
    Method method = Method::None;
    int raw = -1;  // Hardware brightness 0-100, -1 if unknown
};

std::atomic<HWND> g_hwndMain;

////////////////////////////////////////////////////////////////////////////////
// Hardware worker. DDC/CI and WMI calls are slow (tens to hundreds of
// milliseconds), so they run on a dedicated thread. Brightness requests are
// coalesced per display: only the latest requested level is applied.

namespace hw {

struct HwDisplay {
    DisplayInfo info;
    HANDLE physical = nullptr;
    DWORD vcpMax = 100;
    std::wstring wmiNormalized;
    std::wstring wmiPath;
};

std::mutex g_mutex;
std::condition_variable g_cv;
bool g_requestEnumerate = false;
bool g_requestRefresh = false;
bool g_requestStop = false;
bool g_busy = false;
std::condition_variable g_idleCv;
std::map<std::wstring, int> g_pendingSet;
std::vector<DisplayInfo> g_snapshot;

// Worker thread only.
std::vector<HwDisplay> g_displays;
IWbemServices* g_wmi = nullptr;

constexpr BYTE VCP_BRIGHTNESS = 0x10;

bool DdcGet(HANDLE handle, DWORD* current, DWORD* maximum) {
    for (int attempt = 0; attempt < 3; attempt++) {
        MC_VCP_CODE_TYPE type;
        if (GetVCPFeatureAndVCPFeatureReply(handle, VCP_BRIGHTNESS, &type,
                                            current, maximum) &&
            *maximum > 0) {
            return true;
        }
        Sleep(40);
    }
    return false;
}

bool DdcSet(HANDLE handle, DWORD value) {
    for (int attempt = 0; attempt < 2; attempt++) {
        if (SetVCPFeature(handle, VCP_BRIGHTNESS, value)) {
            return true;
        }
        Sleep(50);
    }
    return false;
}

IWbemServices* WmiConnect() {
    IWbemLocator* locator = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WbemLocator, nullptr,
                                CLSCTX_INPROC_SERVER, IID_IWbemLocator,
                                (void**)&locator))) {
        return nullptr;
    }
    IWbemServices* services = nullptr;
    BSTR ns = SysAllocString(L"ROOT\\WMI");
    HRESULT hr = locator->ConnectServer(ns, nullptr, nullptr, nullptr, 0,
                                        nullptr, nullptr, &services);
    SysFreeString(ns);
    locator->Release();
    if (FAILED(hr)) {
        return nullptr;
    }
    CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                      RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
                      nullptr, EOAC_NONE);
    return services;
}

template <typename F>
void WmiForEach(PCWSTR query, F callback) {
    if (!g_wmi) {
        return;
    }
    BSTR language = SysAllocString(L"WQL");
    BSTR queryStr = SysAllocString(query);
    IEnumWbemClassObject* enumerator = nullptr;
    HRESULT hr = g_wmi->ExecQuery(
        language, queryStr,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr,
        &enumerator);
    SysFreeString(language);
    SysFreeString(queryStr);
    if (FAILED(hr) || !enumerator) {
        return;
    }
    while (true) {
        IWbemClassObject* object = nullptr;
        ULONG returned = 0;
        if (enumerator->Next(5000, 1, &object, &returned) != WBEM_S_NO_ERROR ||
            returned == 0) {
            break;
        }
        callback(object);
        object->Release();
    }
    enumerator->Release();
}

std::wstring WmiGetString(IWbemClassObject* object, PCWSTR name) {
    VARIANT v;
    VariantInit(&v);
    std::wstring result;
    if (SUCCEEDED(object->Get(name, 0, &v, nullptr, nullptr)) &&
        v.vt == VT_BSTR && v.bstrVal) {
        result = v.bstrVal;
    }
    VariantClear(&v);
    return result;
}

int WmiGetInt(IWbemClassObject* object, PCWSTR name) {
    VARIANT v;
    VariantInit(&v);
    int result = -1;
    if (SUCCEEDED(object->Get(name, 0, &v, nullptr, nullptr))) {
        if (v.vt == VT_UI1) {
            result = v.bVal;
        } else if (v.vt == VT_I4) {
            result = v.lVal;
        }
    }
    VariantClear(&v);
    return result;
}

// "DISPLAY\BOE0812\4&2fa5ae1a&0&UID8388688_0" -> "boe0812#4&2fa5ae1a&0&uid8388688"
std::wstring NormalizeWmiInstance(std::wstring s) {
    s = ToLower(s);
    if (s.rfind(L"display\\", 0) == 0) {
        s = s.substr(8);
    }
    size_t underscore = s.rfind(L'_');
    if (underscore != std::wstring::npos) {
        s = s.substr(0, underscore);
    }
    std::replace(s.begin(), s.end(), L'\\', L'#');
    return s;
}

// "\\?\DISPLAY#DEL41B8#5&2a2b3c&0&UID4353#{e6f07b5f-...}" -> "DEL41B8#5&2a2b3c&0&UID4353"
std::wstring IdFromDevicePath(const std::wstring& path) {
    std::vector<std::wstring> parts;
    size_t start = 0;
    while (true) {
        size_t hash = path.find(L'#', start);
        parts.push_back(path.substr(
            start, hash == std::wstring::npos ? std::wstring::npos
                                              : hash - start));
        if (hash == std::wstring::npos) {
            break;
        }
        start = hash + 1;
    }
    if (parts.size() >= 3) {
        return parts[1] + L"#" + parts[2];
    }
    return path;
}

std::map<std::wstring, int> WmiReadBrightness() {
    std::map<std::wstring, int> result;
    WmiForEach(L"SELECT InstanceName, CurrentBrightness FROM WmiMonitorBrightness",
               [&](IWbemClassObject* object) {
                   result[NormalizeWmiInstance(
                       WmiGetString(object, L"InstanceName"))] =
                       WmiGetInt(object, L"CurrentBrightness");
               });
    return result;
}

bool WmiSet(const std::wstring& path, int percent) {
    if (!g_wmi || path.empty()) {
        return false;
    }

    bool ok = false;
    BSTR className = SysAllocString(L"WmiMonitorBrightnessMethods");
    BSTR methodName = SysAllocString(L"WmiSetBrightness");
    BSTR objectPath = SysAllocString(path.c_str());
    IWbemClassObject* classObject = nullptr;
    IWbemClassObject* inParamsDef = nullptr;
    IWbemClassObject* inParams = nullptr;

    if (SUCCEEDED(g_wmi->GetObject(className, 0, nullptr, &classObject,
                                   nullptr)) &&
        SUCCEEDED(classObject->GetMethod(methodName, 0, &inParamsDef,
                                         nullptr)) &&
        SUCCEEDED(inParamsDef->SpawnInstance(0, &inParams))) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = 0;
        inParams->Put(L"Timeout", 0, &v, 0);
        v.vt = VT_UI1;
        v.bVal = (BYTE)ClampPercent(percent);
        inParams->Put(L"Brightness", 0, &v, 0);
        ok = SUCCEEDED(g_wmi->ExecMethod(objectPath, methodName, 0, nullptr,
                                         inParams, nullptr, nullptr));
    }

    if (inParams) {
        inParams->Release();
    }
    if (inParamsDef) {
        inParamsDef->Release();
    }
    if (classObject) {
        classObject->Release();
    }
    SysFreeString(className);
    SysFreeString(methodName);
    SysFreeString(objectPath);
    return ok;
}

struct TargetInfo {
    std::wstring gdiName;
    std::wstring name;
    std::wstring id;
    bool internal = false;
};

std::vector<TargetInfo> QueryTargets() {
    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;
    LONG result;
    do {
        UINT32 pathCount = 0, modeCount = 0;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount,
                                        &modeCount) != ERROR_SUCCESS) {
            return {};
        }
        paths.resize(pathCount);
        modes.resize(modeCount);
        result = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount,
                                    paths.data(), &modeCount, modes.data(),
                                    nullptr);
        paths.resize(pathCount);
    } while (result == ERROR_INSUFFICIENT_BUFFER);

    if (result != ERROR_SUCCESS) {
        return {};
    }

    std::vector<TargetInfo> targets;
    for (const auto& path : paths) {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
        source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        source.header.size = sizeof(source);
        source.header.adapterId = path.sourceInfo.adapterId;
        source.header.id = path.sourceInfo.id;
        if (DisplayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS) {
            continue;
        }

        DISPLAYCONFIG_TARGET_DEVICE_NAME target{};
        target.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        target.header.size = sizeof(target);
        target.header.adapterId = path.targetInfo.adapterId;
        target.header.id = path.targetInfo.id;
        if (DisplayConfigGetDeviceInfo(&target.header) != ERROR_SUCCESS) {
            continue;
        }

        TargetInfo info;
        info.gdiName = source.viewGdiDeviceName;
        info.name = target.monitorFriendlyDeviceName;
        info.id = IdFromDevicePath(target.monitorDevicePath);
        auto tech = target.outputTechnology;
        info.internal =
            tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
            tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
            tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED;
        targets.push_back(std::move(info));
    }
    return targets;
}

BOOL CALLBACK EnumMonitorsProc(HMONITOR monitor, HDC, LPRECT, LPARAM param) {
    ((std::vector<HMONITOR>*)param)->push_back(monitor);
    return TRUE;
}

void DestroyDisplays() {
    for (auto& d : g_displays) {
        if (d.physical) {
            DestroyPhysicalMonitor(d.physical);
        }
    }
    g_displays.clear();
}

void Enumerate() {
    DestroyDisplays();

    if (!g_wmi) {
        g_wmi = WmiConnect();
    }

    struct WmiInstance {
        std::wstring normalized;
        std::wstring path;
        bool used = false;
    };
    std::vector<WmiInstance> wmiInstances;
    WmiForEach(L"SELECT * FROM WmiMonitorBrightnessMethods",
               [&](IWbemClassObject* object) {
                   WmiInstance instance;
                   instance.normalized = NormalizeWmiInstance(
                       WmiGetString(object, L"InstanceName"));
                   instance.path = WmiGetString(object, L"__PATH");
                   if (!instance.path.empty()) {
                       wmiInstances.push_back(std::move(instance));
                   }
               });
    std::map<std::wstring, int> wmiBrightness;
    if (!wmiInstances.empty()) {
        wmiBrightness = WmiReadBrightness();
    }

    std::vector<TargetInfo> targets = QueryTargets();
    std::vector<HMONITOR> monitors;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsProc,
                        (LPARAM)&monitors);

    for (HMONITOR monitor : monitors) {
        MONITORINFOEXW mi{};
        mi.cbSize = sizeof(mi);
        if (!GetMonitorInfoW(monitor, &mi)) {
            continue;
        }

        std::vector<const TargetInfo*> monitorTargets;
        for (const auto& t : targets) {
            if (_wcsicmp(t.gdiName.c_str(), mi.szDevice) == 0) {
                monitorTargets.push_back(&t);
            }
        }

        std::vector<PHYSICAL_MONITOR> physical;
        DWORD physicalCount = 0;
        if (GetNumberOfPhysicalMonitorsFromHMONITOR(monitor, &physicalCount) &&
            physicalCount > 0) {
            physical.resize(physicalCount);
            if (!GetPhysicalMonitorsFromHMONITOR(monitor, physicalCount,
                                                 physical.data())) {
                physical.clear();
            }
        }

        size_t count = std::max<size_t>(
            {physical.size(), monitorTargets.size(), (size_t)1});
        for (size_t i = 0; i < count; i++) {
            HwDisplay d;
            d.info.monitor = monitor;
            d.info.rect = mi.rcMonitor;

            const TargetInfo* target =
                i < monitorTargets.size()
                    ? monitorTargets[i]
                    : (monitorTargets.empty() ? nullptr
                                              : monitorTargets.back());
            if (target) {
                d.info.name = target->name;
                d.info.id = target->id;
                d.info.internal = target->internal;
            }
            if (i < physical.size()) {
                d.physical = physical[i].hPhysicalMonitor;
                if (d.info.name.empty() && !d.info.internal) {
                    d.info.name = physical[i].szPhysicalMonitorDescription;
                }
            }
            if (d.info.id.empty()) {
                d.info.id = mi.szDevice;
            }
            if (d.info.name.empty()) {
                d.info.name =
                    d.info.internal ? L"Built-in display" : L"Display";
            }
            d.info.key = d.info.id + L"|" + std::to_wstring(i);

            int wmiIndex = -1;
            std::wstring idLower = ToLower(d.info.id);
            for (size_t j = 0; j < wmiInstances.size(); j++) {
                if (!wmiInstances[j].used &&
                    wmiInstances[j].normalized == idLower) {
                    wmiIndex = (int)j;
                    break;
                }
            }
            if (wmiIndex < 0 && d.info.internal) {
                for (size_t j = 0; j < wmiInstances.size(); j++) {
                    if (!wmiInstances[j].used) {
                        wmiIndex = (int)j;
                        break;
                    }
                }
            }

            auto useWmi = [&]() {
                auto& instance = wmiInstances[wmiIndex];
                instance.used = true;
                d.info.method = Method::Wmi;
                d.wmiNormalized = instance.normalized;
                d.wmiPath = instance.path;
                auto it = wmiBrightness.find(instance.normalized);
                d.info.raw = it != wmiBrightness.end() ? it->second : -1;
            };

            if (wmiIndex >= 0 && d.info.internal) {
                useWmi();
            } else {
                DWORD current, maximum;
                if (d.physical && DdcGet(d.physical, &current, &maximum)) {
                    d.info.method = Method::Ddc;
                    d.vcpMax = maximum;
                    d.info.raw = ClampPercent(
                        (int)std::lround(current * 100.0 / maximum));
                } else if (wmiIndex >= 0) {
                    useWmi();
                }
            }

            Wh_Log(L"Display: %s [%s] method=%d brightness=%d",
                   d.info.name.c_str(), d.info.id.c_str(),
                   (int)d.info.method, d.info.raw);
            g_displays.push_back(std::move(d));
        }
    }

    // Make duplicate names distinguishable.
    std::map<std::wstring, int> nameCounts;
    for (const auto& d : g_displays) {
        nameCounts[d.info.name]++;
    }
    std::map<std::wstring, int> nameIndex;
    for (auto& d : g_displays) {
        if (nameCounts[d.info.name] > 1) {
            int n = ++nameIndex[d.info.name];
            d.info.name += L" (" + std::to_wstring(n) + L")";
        }
    }
}

void Refresh() {
    std::map<std::wstring, int> wmiBrightness;
    bool wmiRead = false;
    for (auto& d : g_displays) {
        if (d.info.method == Method::Ddc) {
            DWORD current, maximum;
            if (DdcGet(d.physical, &current, &maximum)) {
                d.vcpMax = maximum;
                d.info.raw =
                    ClampPercent((int)std::lround(current * 100.0 / maximum));
            }
        } else if (d.info.method == Method::Wmi) {
            if (!wmiRead) {
                wmiBrightness = WmiReadBrightness();
                wmiRead = true;
            }
            auto it = wmiBrightness.find(d.wmiNormalized);
            if (it != wmiBrightness.end()) {
                d.info.raw = it->second;
            }
        }
    }
}

bool Apply(HwDisplay& d, int raw) {
    bool ok = false;
    if (d.info.method == Method::Ddc) {
        ok = DdcSet(d.physical,
                    (DWORD)std::lround(raw * (double)d.vcpMax / 100.0));
    } else if (d.info.method == Method::Wmi) {
        ok = WmiSet(d.wmiPath, raw);
    }
    if (ok) {
        d.info.raw = raw;
    } else {
        Wh_Log(L"Setting brightness failed: %s", d.info.name.c_str());
    }
    return ok;
}

// Returns the requests that failed, so they can be retried after the
// displays are enumerated again (handles go stale after sleep or a display
// change).
std::map<std::wstring, int> ApplyAll(const std::map<std::wstring, int>& sets) {
    std::map<std::wstring, int> failed;
    for (const auto& [key, raw] : sets) {
        for (auto& d : g_displays) {
            if (d.info.key == key) {
                if (!Apply(d, raw)) {
                    failed[key] = raw;
                }
                break;
            }
        }
    }
    return failed;
}

void Publish(bool enumerated) {
    std::vector<DisplayInfo> infos;
    for (const auto& d : g_displays) {
        infos.push_back(d.info);
    }
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_snapshot = std::move(infos);
    }
    if (HWND hwnd = g_hwndMain) {
        PostMessage(hwnd, WM_APP_SNAPSHOT, enumerated ? 1 : 0, 0);
    }
}

DWORD WINAPI WorkerThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    while (true) {
        bool enumerate, refresh, stop;
        std::map<std::wstring, int> sets;
        {
            std::unique_lock<std::mutex> lock(g_mutex);
            g_cv.wait(lock, [] {
                return g_requestEnumerate || g_requestRefresh ||
                       g_requestStop || !g_pendingSet.empty();
            });
            enumerate = g_requestEnumerate;
            refresh = g_requestRefresh;
            stop = g_requestStop;
            sets.swap(g_pendingSet);
            g_requestEnumerate = false;
            g_requestRefresh = false;
            g_busy = true;
        }

        std::map<std::wstring, int> failed = ApplyAll(sets);
        // A failed write usually means stale handles (after sleep or a display
        // change), so enumerate again and retry. Rate-limited, so that a
        // monitor that answers reads but rejects writes doesn't cause an
        // enumeration for every slider movement.
        static DWORD lastFailureEnumerate;
        if (!failed.empty() && !stop &&
            GetTickCount() - lastFailureEnumerate > 10000) {
            lastFailureEnumerate = GetTickCount();
            enumerate = true;
        }

        if (enumerate) {
            Enumerate();
            ApplyAll(failed);
            Publish(true);
        } else if (refresh) {
            Refresh();
            Publish(false);
        }

        if (stop) {
            std::map<std::wstring, int> rest;
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                rest.swap(g_pendingSet);
            }
            ApplyAll(rest);
            break;
        }

        {
            std::lock_guard<std::mutex> lock(g_mutex);
            g_busy = false;
        }
        g_idleCv.notify_all();
    }

    DestroyDisplays();
    if (g_wmi) {
        g_wmi->Release();
        g_wmi = nullptr;
    }
    CoUninitialize();
    return 0;
}

void RequestEnumerate() {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_requestEnumerate = true;
    g_cv.notify_one();
}

void RequestRefresh() {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_requestRefresh = true;
    g_cv.notify_one();
}

void RequestSet(const std::wstring& key, int raw) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_pendingSet[key] = ClampPercent(raw);
    g_cv.notify_one();
}

void RequestStop() {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_requestStop = true;
    g_cv.notify_one();
}

std::vector<DisplayInfo> TakeSnapshot() {
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_snapshot;
}

// Waits until all queued brightness changes were written, e.g. before the
// session ends.
bool WaitUntilIdle(DWORD timeoutMs) {
    std::unique_lock<std::mutex> lock(g_mutex);
    return g_idleCv.wait_for(lock, std::chrono::milliseconds(timeoutMs), [] {
        return !g_busy && g_pendingSet.empty() && !g_requestEnumerate &&
               !g_requestRefresh;
    });
}

}  // namespace hw

////////////////////////////////////////////////////////////////////////////////
// UI state (UI thread only)

struct UiDisplay {
    DisplayInfo info;
    std::wstring label;
    int minBrightness = 0;
    int maxBrightness = 100;
    bool hidden = false;
    int value = -1;  // Slider value 0-100, -1 if unknown
    DWORD lastSetTick = 0;

    bool Controllable() const {
        return !hidden && info.method != Method::None;
    }
};

struct Fade {
    std::wstring key;
    int from;
    int to;
    DWORD start;
    DWORD duration;
};

HINSTANCE g_module;
HANDLE g_uiThread;
HANDLE g_workerThread;
HANDLE g_uiReadyEvent;
bool g_isWin11;
ULONG_PTR g_gdiplusToken;
std::wstring g_fontFamily = L"Segoe UI";

std::vector<UiDisplay> g_displays;
std::vector<DisplayInfo> g_lastSnapshot;
bool g_haveSnapshot;
std::vector<Fade> g_fades;
bool g_paused;
// The panel's link button: when on, all displays share one slider and move
// together, and hotkeys and the wheel change all of them. Saved in storage.
bool g_linked = true;
void SetLinked(bool linked);
bool IsTaskbarWindow(HWND hwnd);
bool g_idleDimmed;
std::map<std::wstring, int> g_idleRestore;
void SaveIdleRestore();
long long g_lastScheduleCheck;
bool g_reenumPending;
bool g_resumeCatchUp;
int g_wheelAccumulator;
int g_registeredHotkeys;
std::wstring g_storagePrefix;

UINT g_taskbarCreatedMsg;
HICON g_trayIcon;
bool g_trayUseGuid = true;
bool g_trayAdded;

// The low-level mouse hook runs on its own thread, so that it always answers
// within LowLevelHooksTimeout even when the UI thread waits for Explorer. It
// reads this configuration, which the UI thread keeps up to date.
struct WheelHookConfig {
    WheelMode mode = WheelMode::Off;
    int modifier = 0;
    RECT trayIconRect{};
};
std::mutex g_wheelHookMutex;
WheelHookConfig g_wheelHookConfig;
HANDLE g_hookThread;
DWORD g_hookThreadId;

HWND g_hwndPanel;
HWND g_hwndOsd;

void UpdatePanel();
void ShowPanel();
void HidePanel();
void ShowOsd(const std::vector<UiDisplay*>& targets, HMONITOR monitor);
void UpdateTrayTip();

int RawToValue(const UiDisplay& d, int raw) {
    if (raw < 0) {
        return -1;
    }
    int range = d.maxBrightness - d.minBrightness;
    if (range <= 0) {
        return raw;
    }
    return ClampPercent(
        (int)std::lround((raw - d.minBrightness) * 100.0 / range));
}

int ValueToRaw(const UiDisplay& d, int value) {
    return ClampPercent((int)std::lround(
        d.minBrightness + (d.maxBrightness - d.minBrightness) * value / 100.0));
}

UiDisplay* FindDisplay(const std::wstring& key) {
    for (auto& d : g_displays) {
        if (d.info.key == key) {
            return &d;
        }
    }
    return nullptr;
}

std::vector<UiDisplay*> ControllableDisplays() {
    std::vector<UiDisplay*> result;
    for (auto& d : g_displays) {
        if (d.Controllable()) {
            result.push_back(&d);
        }
    }
    return result;
}

std::vector<UiDisplay*> MatchDisplays(const std::wstring& match) {
    std::vector<UiDisplay*> result;
    for (auto& d : g_displays) {
        if (d.Controllable() &&
            (match.empty() || ContainsI(d.label, match) ||
             ContainsI(d.info.name, match) || ContainsI(d.info.id, match))) {
            result.push_back(&d);
        }
    }
    return result;
}

bool IsFading(const std::wstring& key) {
    return std::any_of(g_fades.begin(), g_fades.end(),
                       [&](const Fade& f) { return f.key == key; });
}

void CancelFade(const std::wstring& key) {
    std::erase_if(g_fades, [&](const Fade& f) { return f.key == key; });
}

void OnValuesChanged() {
    if (g_hwndPanel && IsWindowVisible(g_hwndPanel)) {
        InvalidateRect(g_hwndPanel, nullptr, FALSE);
    }
    SetTimer(g_hwndMain, TIMER_TOOLTIP, 300, nullptr);
}

void SetDisplayValue(UiDisplay& d, int value, bool byUser) {
    if (!d.Controllable()) {
        return;
    }
    if (byUser) {
        CancelFade(d.info.key);
    }
    value = ClampPercent(value);
    d.lastSetTick = GetTickCount();
    if (value == d.value) {
        return;
    }
    d.value = value;
    hw::RequestSet(d.info.key, ValueToRaw(d, value));
    OnValuesChanged();
}

void StartFade(UiDisplay& d, int to, DWORD durationMs) {
    CancelFade(d.info.key);
    to = ClampPercent(to);
    if (durationMs == 0 || d.value < 0 || d.value == to) {
        SetDisplayValue(d, to, false);
        return;
    }
    g_fades.push_back({d.info.key, d.value, to, GetTickCount(), durationMs});
    SetTimer(g_hwndMain, TIMER_FADE, 100, nullptr);
}

void OnFadeTimer() {
    DWORD now = GetTickCount();
    std::vector<Fade> fades = g_fades;
    for (const auto& f : fades) {
        UiDisplay* d = FindDisplay(f.key);
        double t = std::min(1.0, (now - f.start) / (double)f.duration);
        if (d) {
            int value = (int)std::lround(f.from + (f.to - f.from) * t);
            if (value != d->value) {
                d->value = value;
                d->lastSetTick = now;
                hw::RequestSet(d->info.key, ValueToRaw(*d, value));
                OnValuesChanged();
            }
        }
        if (t >= 1.0 || !d) {
            std::erase_if(g_fades, [&](const Fade& x) {
                return x.key == f.key && x.start == f.start;
            });
        }
    }
    if (g_fades.empty()) {
        KillTimer(g_hwndMain, TIMER_FADE);
    }
}

void RebuildDisplays(const std::vector<DisplayInfo>& snapshot,
                     bool fromHardware) {
    std::map<std::wstring, UiDisplay> old;
    for (const auto& d : g_displays) {
        old[d.info.key] = d;
    }

    DWORD now = GetTickCount();
    std::vector<UiDisplay> displays;
    for (const auto& info : snapshot) {
        UiDisplay d;
        d.info = info;
        d.label = info.name;
        for (const auto& cfg : g_settings.displays) {
            if (ContainsI(info.name, cfg.match) ||
                ContainsI(info.id, cfg.match)) {
                if (!cfg.name.empty()) {
                    d.label = cfg.name;
                }
                d.minBrightness = cfg.minBrightness;
                d.maxBrightness = cfg.maxBrightness;
                d.hidden = cfg.hide;
                break;
            }
        }

        int hardwareValue = RawToValue(d, info.raw);
        auto it = old.find(info.key);
        if (it != old.end()) {
            d.lastSetTick = it->second.lastSetTick;
            bool recent = now - d.lastSetTick < 2000 || IsFading(info.key);
            d.value = (fromHardware && !recent && hardwareValue >= 0)
                          ? hardwareValue
                          : it->second.value;
            if (d.value < 0) {
                d.value = hardwareValue;
            }
        } else {
            d.value = hardwareValue;
        }
        displays.push_back(std::move(d));
    }

    std::stable_sort(displays.begin(), displays.end(),
                     [](const UiDisplay& a, const UiDisplay& b) {
                         if (a.info.rect.left != b.info.rect.left) {
                             return a.info.rect.left < b.info.rect.left;
                         }
                         return a.info.rect.top < b.info.rect.top;
                     });

    g_displays = std::move(displays);
    std::erase_if(g_fades,
                  [](const Fade& f) { return FindDisplay(f.key) == nullptr; });
}

////////////////////////////////////////////////////////////////////////////////
// Theme

struct Theme {
    Gdiplus::Color background;
    Gdiplus::Color footer;
    Gdiplus::Color text;
    Gdiplus::Color secondaryText;
    Gdiplus::Color track;
    Gdiplus::Color thumb;
    Gdiplus::Color border;
    Gdiplus::Color hover;
    Gdiplus::Color accent;
    bool dark;
};

DWORD ReadRegDword(PCWSTR path, PCWSTR name, DWORD defaultValue) {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, path, name, RRF_RT_REG_DWORD, nullptr,
                     &value, &size) == ERROR_SUCCESS) {
        return value;
    }
    return defaultValue;
}

bool IsSystemDark() {
    return ReadRegDword(
               L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
               L"SystemUsesLightTheme", 0) == 0;
}

Gdiplus::Color GetAccentColor(bool dark) {
    BYTE palette[32];
    DWORD size = sizeof(palette);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
                     L"AccentPalette", RRF_RT_REG_BINARY, nullptr, palette,
                     &size) == ERROR_SUCCESS &&
        size >= sizeof(palette)) {
        // Entries are RGBA: Light3, Light2, Light1, Base, Dark1, Dark2, Dark3.
        int i = dark ? 1 : 4;
        return Gdiplus::Color(255, palette[i * 4], palette[i * 4 + 1],
                              palette[i * 4 + 2]);
    }
    DWORD color;
    BOOL opaque;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        return Gdiplus::Color(255, (color >> 16) & 0xff, (color >> 8) & 0xff,
                              color & 0xff);
    }
    return Gdiplus::Color(255, 0, 120, 212);
}

Theme GetTheme() {
    bool dark = g_settings.theme == ThemeMode::Dark ||
                (g_settings.theme == ThemeMode::Auto && IsSystemDark());
    Theme t;
    t.dark = dark;
    t.accent = GetAccentColor(dark);
    if (dark) {
        t.background = Gdiplus::Color(255, 44, 44, 44);
        t.footer = Gdiplus::Color(255, 32, 32, 32);
        t.text = Gdiplus::Color(255, 255, 255, 255);
        t.secondaryText = Gdiplus::Color(255, 200, 200, 200);
        t.track = Gdiplus::Color(255, 140, 140, 140);
        t.thumb = Gdiplus::Color(255, 69, 69, 69);
        t.border = Gdiplus::Color(255, 64, 64, 64);
        t.hover = Gdiplus::Color(255, 58, 58, 58);
    } else {
        t.background = Gdiplus::Color(255, 249, 249, 249);
        t.footer = Gdiplus::Color(255, 238, 238, 238);
        t.text = Gdiplus::Color(255, 27, 27, 27);
        t.secondaryText = Gdiplus::Color(255, 96, 96, 96);
        t.track = Gdiplus::Color(255, 135, 135, 135);
        t.thumb = Gdiplus::Color(255, 255, 255, 255);
        t.border = Gdiplus::Color(255, 220, 220, 220);
        t.hover = Gdiplus::Color(255, 226, 226, 226);
    }
    return t;
}

////////////////////////////////////////////////////////////////////////////////
// Drawing helpers

void RoundRectPath(Gdiplus::GraphicsPath* path,
                   const Gdiplus::RectF& r,
                   float radius) {
    float d = std::min({radius * 2, r.Width, r.Height});
    if (d <= 0) {
        path->AddRectangle(r);
        return;
    }
    path->AddArc(r.X, r.Y, d, d, 180, 90);
    path->AddArc(r.X + r.Width - d, r.Y, d, d, 270, 90);
    path->AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0, 90);
    path->AddArc(r.X, r.Y + r.Height - d, d, d, 90, 90);
    path->CloseFigure();
}

void FillRoundRect(Gdiplus::Graphics& g,
                   const Gdiplus::Color& color,
                   const Gdiplus::RectF& r,
                   float radius) {
    Gdiplus::GraphicsPath path;
    RoundRectPath(&path, r, radius);
    Gdiplus::SolidBrush brush(color);
    g.FillPath(&brush, &path);
}

void DrawSun(Gdiplus::Graphics& g,
             float cx,
             float cy,
             float size,
             const Gdiplus::Color& color) {
    Gdiplus::SolidBrush brush(color);
    float r = size * 0.2f;
    g.FillEllipse(&brush, cx - r, cy - r, r * 2, r * 2);
    Gdiplus::Pen pen(color, std::max(1.0f, size / 12.0f));
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);
    for (int i = 0; i < 8; i++) {
        double a = i * 3.14159265358979 / 4;
        float r1 = size * 0.32f, r2 = size * 0.46f;
        g.DrawLine(&pen, cx + (float)cos(a) * r1, cy + (float)sin(a) * r1,
                   cx + (float)cos(a) * r2, cy + (float)sin(a) * r2);
    }
}

void DrawLabel(Gdiplus::Graphics& g,
              const std::wstring& text,
              const Gdiplus::RectF& rect,
              float size,
              const Gdiplus::Color& color,
              Gdiplus::StringAlignment align,
              PCWSTR family = nullptr,
              int style = Gdiplus::FontStyleRegular) {
    Gdiplus::Font font(family ? family : g_fontFamily.c_str(), size, style,
                       Gdiplus::UnitPixel);
    Gdiplus::StringFormat format;
    format.SetAlignment(align);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
    Gdiplus::SolidBrush brush(color);
    g.DrawString(text.c_str(), (INT)text.size(), &font, rect, &format, &brush);
}

void DrawSlider(Gdiplus::Graphics& g,
                const RECT& track,
                int value,
                const Theme& t,
                float scale,
                bool active) {
    float cy = (track.top + track.bottom) / 2.0f;
    float left = (float)track.left, right = (float)track.right;
    float h = 4 * scale;
    FillRoundRect(g, t.track, Gdiplus::RectF(left, cy - h / 2, right - left, h),
                  h / 2);
    float x = left + (right - left) * std::max(0, value) / 100.0f;
    if (x > left) {
        FillRoundRect(g, t.accent, Gdiplus::RectF(left, cy - h / 2, x - left, h),
                      h / 2);
    }
    float outer = 10 * scale;
    Gdiplus::SolidBrush thumbBrush(t.thumb);
    g.FillEllipse(&thumbBrush, x - outer, cy - outer, outer * 2, outer * 2);
    Gdiplus::Pen borderPen(t.border, 1);
    g.DrawEllipse(&borderPen, x - outer, cy - outer, outer * 2, outer * 2);
    float inner = (active ? 7 : 5) * scale;
    Gdiplus::SolidBrush accentBrush(t.accent);
    g.FillEllipse(&accentBrush, x - inner, cy - inner, inner * 2, inner * 2);
}

UINT GetMonitorDpi(HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        return 96;
    }
    return dpiX;
}

////////////////////////////////////////////////////////////////////////////////
// Tray icon

NOTIFYICONDATAW BaseNotifyIconData() {
    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwndMain;
    nid.uID = TRAY_ICON_ID;
    if (g_trayUseGuid) {
        nid.uFlags |= NIF_GUID;
        nid.guidItem = TRAY_ICON_GUID;
    }
    return nid;
}

HICON CreateSunIcon() {
    UINT dpi = GetDpiForSystem();
    int size = GetSystemMetricsForDpi(SM_CXSMICON, dpi);
    Gdiplus::Bitmap bitmap(size, size, PixelFormat32bppARGB);
    {
        Gdiplus::Graphics g(&bitmap);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        Gdiplus::Color color = IsSystemDark()
                                   ? Gdiplus::Color(255, 255, 255, 255)
                                   : Gdiplus::Color(255, 0, 0, 0);
        DrawSun(g, size / 2.0f, size / 2.0f, (float)size, color);
    }
    HICON icon = nullptr;
    bitmap.GetHICON(&icon);
    return icon;
}

std::wstring BuildTrayTip() {
    std::wstring tip = L"Twinkle Brightness";
    for (auto* d : ControllableDisplays()) {
        tip += L"\n" + d->label + L": " +
               (d->value >= 0 ? std::to_wstring(d->value) + L"%" : L"?");
    }
    if (g_paused) {
        tip += L"\n(automatic brightness paused)";
    }
    if (tip.size() > 127) {
        tip = tip.substr(0, 124) + L"...";
    }
    return tip;
}

void AddTrayIcon() {
    if (g_trayIcon) {
        DestroyIcon(g_trayIcon);
    }
    g_trayIcon = CreateSunIcon();

    NOTIFYICONDATAW nid = BaseNotifyIconData();
    nid.uFlags |= NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
    nid.uCallbackMessage = WM_APP_TRAY;
    nid.hIcon = g_trayIcon;
    wcsncpy_s(nid.szTip, BuildTrayTip().c_str(), _TRUNCATE);

    g_trayAdded = Shell_NotifyIconW(NIM_ADD, &nid);
    if (!g_trayAdded && g_trayUseGuid) {
        // A stale icon with the same GUID may still be registered, or the
        // GUID may be bound to another executable path.
        Shell_NotifyIconW(NIM_DELETE, &nid);
        g_trayAdded = Shell_NotifyIconW(NIM_ADD, &nid);
        if (!g_trayAdded) {
            g_trayUseGuid = false;
            nid.uFlags &= ~NIF_GUID;
            g_trayAdded = Shell_NotifyIconW(NIM_ADD, &nid);
        }
    }
    if (!g_trayAdded) {
        Wh_Log(L"Shell_NotifyIcon(NIM_ADD) failed");
        return;
    }

    nid.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &nid);
}

void UpdateTrayIconImage() {
    if (!g_trayAdded) {
        return;
    }
    HICON old = g_trayIcon;
    g_trayIcon = CreateSunIcon();
    NOTIFYICONDATAW nid = BaseNotifyIconData();
    nid.uFlags |= NIF_ICON;
    nid.hIcon = g_trayIcon;
    Shell_NotifyIconW(NIM_MODIFY, &nid);
    if (old) {
        DestroyIcon(old);
    }
}

void UpdateTrayTip() {
    if (!g_trayAdded) {
        return;
    }
    NOTIFYICONDATAW nid = BaseNotifyIconData();
    nid.uFlags |= NIF_TIP | NIF_SHOWTIP;
    wcsncpy_s(nid.szTip, BuildTrayTip().c_str(), _TRUNCATE);
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

void RemoveTrayIcon() {
    if (g_trayAdded) {
        NOTIFYICONDATAW nid = BaseNotifyIconData();
        Shell_NotifyIconW(NIM_DELETE, &nid);
        g_trayAdded = false;
    }
    if (g_trayIcon) {
        DestroyIcon(g_trayIcon);
        g_trayIcon = nullptr;
    }
}

bool GetTrayIconRect(RECT* rect) {
    NOTIFYICONIDENTIFIER nii{};
    nii.cbSize = sizeof(nii);
    nii.hWnd = g_hwndMain;
    nii.uID = TRAY_ICON_ID;
    if (g_trayUseGuid) {
        nii.guidItem = TRAY_ICON_GUID;
    }
    return SUCCEEDED(Shell_NotifyIconGetRect(&nii, rect));
}

////////////////////////////////////////////////////////////////////////////////
// Brightness panel

struct PanelRow {
    int display;  // Index into g_displays, -1 = all displays
    RECT rect;
    RECT track;
    bool supported;
};

std::vector<PanelRow> g_rows;
int g_dragRow = -1;
int g_focusRow = -1;
int g_hoverButton = -1;  // 0 = turn off, 1 = refresh, 2 = link
constexpr int PANEL_BUTTONS = 3;
RECT g_buttons[PANEL_BUTTONS];
UINT g_panelDpi = 96;
int g_panelWidth;
int g_panelHeight;
int g_footerTop;
DWORD g_panelHiddenTick;

int S(int dip) {
    return MulDiv(dip, (int)g_panelDpi, 96);
}

bool CanLink() {
    return ControllableDisplays().size() >= 2;
}

void LayoutPanel() {
    g_rows.clear();
    g_panelWidth = S(360);
    int y = S(12);
    int rowHeight = S(60);

    auto addRow = [&](int display, bool supported) {
        PanelRow row{};
        row.display = display;
        row.supported = supported;
        row.rect = {0, y, g_panelWidth, y + rowHeight};
        int cy = y + S(42);
        row.track = {S(56), cy - S(2), g_panelWidth - S(64), cy + S(2)};
        g_rows.push_back(row);
        y += rowHeight;
    };

    if (g_linked && CanLink()) {
        addRow(-1, true);
    } else {
        for (size_t i = 0; i < g_displays.size(); i++) {
            if (!g_displays[i].hidden) {
                addRow((int)i, g_displays[i].info.method != Method::None);
            }
        }
    }
    if (g_rows.empty()) {
        y += rowHeight;  // "No displays" message
    }
    y += S(8);

    g_footerTop = y;
    g_panelHeight = y + S(48);
    int buttonSize = S(32);
    int top = g_footerTop + S(8);
    g_buttons[1] = {g_panelWidth - S(16) - buttonSize, top,
                    g_panelWidth - S(16), top + buttonSize};
    g_buttons[0] = {g_buttons[1].left - S(4) - buttonSize, top,
                    g_buttons[1].left - S(4), top + buttonSize};
    g_buttons[2] = {g_buttons[0].left - S(4) - buttonSize, top,
                    g_buttons[0].left - S(4), top + buttonSize};
    if (!CanLink()) {
        SetRectEmpty(&g_buttons[2]);
    }

    if (g_focusRow >= (int)g_rows.size()) {
        g_focusRow = -1;
    }
}

int RowValue(const PanelRow& row) {
    if (row.display >= 0) {
        return g_displays[row.display].value;
    }
    int sum = 0, count = 0;
    for (auto* d : ControllableDisplays()) {
        if (d->value >= 0) {
            sum += d->value;
            count++;
        }
    }
    return count ? (int)std::lround(sum / (double)count) : -1;
}

void SetRowValue(const PanelRow& row, int value) {
    if (row.display >= 0) {
        SetDisplayValue(g_displays[row.display], value, true);
    } else {
        for (auto* d : ControllableDisplays()) {
            SetDisplayValue(*d, value, true);
        }
    }
}

void PaintPanel(HDC hdc) {
    RECT client;
    GetClientRect(g_hwndPanel, &client);
    int w = client.right, h = client.bottom;

    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bitmap = CreateCompatibleBitmap(hdc, w, h);
    HGDIOBJ oldBitmap = SelectObject(mem, bitmap);

    {
        Gdiplus::Graphics g(mem);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
        Theme t = GetTheme();
        float scale = g_panelDpi / 96.0f;

        g.Clear(t.background);

        if (g_rows.empty()) {
            DrawLabel(g, L"No displays found",
                     Gdiplus::RectF(0, (float)S(12), (float)w, (float)S(60)),
                     (float)S(14), t.secondaryText,
                     Gdiplus::StringAlignmentCenter);
        }

        for (size_t i = 0; i < g_rows.size(); i++) {
            const PanelRow& row = g_rows[i];
            std::wstring label = row.display >= 0
                                     ? g_displays[row.display].label
                                     : L"All displays";
            DrawLabel(g, label,
                     Gdiplus::RectF((float)S(20), (float)row.rect.top + S(6),
                                    (float)(w - S(40)), (float)S(22)),
                     (float)S(14), t.text, Gdiplus::StringAlignmentNear,
                     nullptr, Gdiplus::FontStyleBold);

            float cy = (row.track.top + row.track.bottom) / 2.0f;
            if (!row.supported) {
                DrawLabel(g, L"Brightness control is not available",
                         Gdiplus::RectF((float)S(20), cy - S(11),
                                        (float)(w - S(40)), (float)S(22)),
                         (float)S(13), t.secondaryText,
                         Gdiplus::StringAlignmentNear);
                continue;
            }

            DrawSun(g, (float)S(32), cy, (float)S(20), t.secondaryText);
            int value = RowValue(row);
            bool active = g_dragRow == (int)i || g_focusRow == (int)i;
            DrawSlider(g, row.track, value, t, scale, active);
            DrawLabel(g, value >= 0 ? std::to_wstring(value) : L"-",
                     Gdiplus::RectF((float)row.track.right + S(12), cy - S(11),
                                    (float)(w - S(20) - row.track.right - S(12)),
                                    (float)S(22)),
                     (float)S(14), t.text, Gdiplus::StringAlignmentFar);
        }

        // Footer
        Gdiplus::SolidBrush footerBrush(t.footer);
        g.FillRectangle(&footerBrush, 0, g_footerTop, w, h - g_footerTop);
        Gdiplus::Pen footerLine(t.border, 1);
        g.DrawLine(&footerLine, 0, g_footerTop, w, g_footerTop);
        DrawLabel(g, g_paused ? L"Automatic brightness paused" : L"Brightness",
                 Gdiplus::RectF((float)S(20), (float)g_footerTop,
                                (float)((CanLink() ? g_buttons[2].left
                                                   : g_buttons[0].left) -
                                        S(28)),
                                (float)(h - g_footerTop)),
                 (float)S(12), t.secondaryText, Gdiplus::StringAlignmentNear);

        // Power, Refresh, Link
        static const PCWSTR glyphs[PANEL_BUTTONS] = {L"\xE7E8", L"\xE72C",
                                                     L"\xE71B"};
        for (int i = 0; i < PANEL_BUTTONS; i++) {
            const RECT& b = g_buttons[i];
            if (IsRectEmpty(&b)) {
                continue;
            }
            Gdiplus::RectF rect((float)b.left, (float)b.top,
                                (float)(b.right - b.left),
                                (float)(b.bottom - b.top));
            if (g_hoverButton == i) {
                FillRoundRect(g, t.hover, rect, (float)S(4));
            }
            bool on = i == 2 && g_linked;
            if (on) {
                // Pressed look, so the state shows even with a gray accent.
                FillRoundRect(g, t.hover, rect, (float)S(4));
            }
            DrawLabel(g, glyphs[i], rect, (float)S(16), on ? t.accent : t.text,
                     Gdiplus::StringAlignmentCenter, L"Segoe MDL2 Assets");
        }

        if (!g_isWin11) {
            Gdiplus::Pen border(t.border, 1);
            g.DrawRectangle(&border, 0, 0, w - 1, h - 1);
        }
    }

    BitBlt(hdc, 0, 0, w, h, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(mem);
}

void PositionPanel() {
    RECT iconRect{};
    POINT anchor;
    if (GetTrayIconRect(&iconRect)) {
        anchor = {(iconRect.left + iconRect.right) / 2,
                  (iconRect.top + iconRect.bottom) / 2};
    } else {
        GetCursorPos(&anchor);
    }
    HMONITOR monitor = MonitorFromPoint(anchor, MONITOR_DEFAULTTONEAREST);
    g_panelDpi = GetMonitorDpi(monitor);
    LayoutPanel();

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(monitor, &mi);
    RECT work = mi.rcWork;

    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    UINT edge = ABE_BOTTOM;
    if (SHAppBarMessage(ABM_GETTASKBARPOS, &abd)) {
        edge = abd.uEdge;
    }

    int margin = S(12);
    int x = work.right - g_panelWidth - margin;
    int y = work.bottom - g_panelHeight - margin;
    if (edge == ABE_TOP) {
        y = work.top + margin;
    } else if (edge == ABE_LEFT) {
        x = work.left + margin;
    }

    SetWindowPos(g_hwndPanel, HWND_TOPMOST, x, y, g_panelWidth, g_panelHeight,
                 SWP_NOACTIVATE);
}

void ApplyPanelFrame() {
    BOOL dark = GetTheme().dark;
    DwmSetWindowAttribute(g_hwndPanel, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */,
                          &dark, sizeof(dark));
    if (g_isWin11) {
        int corner = 2;  // DWMWCP_ROUND
        DwmSetWindowAttribute(g_hwndPanel,
                              33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &corner,
                              sizeof(corner));
    }
}

void ShowPanel() {
    hw::RequestRefresh();
    g_focusRow = -1;
    g_hoverButton = -1;
    ApplyPanelFrame();
    PositionPanel();
    ShowWindow(g_hwndPanel, SW_SHOW);
    SetForegroundWindow(g_hwndPanel);
    SetFocus(g_hwndPanel);
    InvalidateRect(g_hwndPanel, nullptr, FALSE);
}

void HidePanel() {
    if (!IsWindowVisible(g_hwndPanel)) {
        return;
    }
    if (GetCapture() == g_hwndPanel) {
        ReleaseCapture();
    }
    g_dragRow = -1;
    ShowWindow(g_hwndPanel, SW_HIDE);
    g_panelHiddenTick = GetTickCount();
}

void TogglePanel() {
    if (IsWindowVisible(g_hwndPanel)) {
        HidePanel();
    } else if (GetTickCount() - g_panelHiddenTick > 400) {
        // The panel was not just closed by clicking the tray icon.
        ShowPanel();
    }
}

void UpdatePanel() {
    if (!g_hwndPanel || !IsWindowVisible(g_hwndPanel)) {
        return;
    }
    if (GetCapture() == g_hwndPanel) {
        ReleaseCapture();
    }
    g_dragRow = -1;
    PositionPanel();
    InvalidateRect(g_hwndPanel, nullptr, FALSE);
}

int HitTestRow(POINT pt, bool trackOnly) {
    for (size_t i = 0; i < g_rows.size(); i++) {
        const PanelRow& row = g_rows[i];
        if (!row.supported) {
            continue;
        }
        if (trackOnly) {
            int cy = (row.track.top + row.track.bottom) / 2;
            if (pt.x >= row.track.left - S(12) &&
                pt.x <= row.track.right + S(12) && pt.y >= cy - S(14) &&
                pt.y <= cy + S(14)) {
                return (int)i;
            }
        } else if (PtInRect(&row.rect, pt)) {
            return (int)i;
        }
    }
    return -1;
}

void DragTo(int x) {
    if (g_dragRow < 0 || g_dragRow >= (int)g_rows.size()) {
        return;
    }
    const PanelRow& row = g_rows[g_dragRow];
    int width = std::max(1, (int)(row.track.right - row.track.left));
    int value =
        ClampPercent((int)std::lround((x - row.track.left) * 100.0 / width));
    SetRowValue(row, value);
}

void AdjustRow(int rowIndex, int delta) {
    if (rowIndex < 0 || rowIndex >= (int)g_rows.size() ||
        !g_rows[rowIndex].supported) {
        return;
    }
    int value = RowValue(g_rows[rowIndex]);
    SetRowValue(g_rows[rowIndex], (value < 0 ? 50 : value) + delta);
}

void MoveFocus(int direction) {
    int count = (int)g_rows.size();
    if (count == 0) {
        return;
    }
    int i = g_focusRow;
    for (int n = 0; n < count; n++) {
        i = (i + direction + count) % count;
        if (g_rows[i].supported) {
            g_focusRow = i;
            break;
        }
    }
    InvalidateRect(g_hwndPanel, nullptr, FALSE);
}

void TurnOffDisplays() {
    // Delay so that releasing the mouse button or hotkey does not wake the
    // displays again.
    SetTimer(g_hwndMain, TIMER_MONITOR_OFF, 500, nullptr);
}

LRESULT CALLBACK PanelWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                HidePanel();
            }
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            PaintPanel(hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int row = HitTestRow(pt, true);
            if (row >= 0) {
                g_dragRow = row;
                g_focusRow = row;
                SetCapture(hwnd);
                DragTo(pt.x);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (g_dragRow >= 0) {
                DragTo(pt.x);
                return 0;
            }
            int hover = -1;
            for (int i = 0; i < PANEL_BUTTONS; i++) {
                if (PtInRect(&g_buttons[i], pt)) {
                    hover = i;
                }
            }
            if (hover != g_hoverButton) {
                g_hoverButton = hover;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_hoverButton != -1) {
                g_hoverButton = -1;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONUP: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (g_dragRow >= 0) {
                ReleaseCapture();
                return 0;
            }
            if (PtInRect(&g_buttons[0], pt)) {
                HidePanel();
                TurnOffDisplays();
            } else if (PtInRect(&g_buttons[1], pt)) {
                hw::RequestEnumerate();
            } else if (PtInRect(&g_buttons[2], pt)) {
                SetLinked(!g_linked);
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            g_dragRow = -1;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_MOUSEWHEEL: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hwnd, &pt);
            int row = HitTestRow(pt, false);
            if (row < 0) {
                row = g_focusRow;
            }
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            AdjustRow(row, (delta > 0 ? 1 : -1) * g_settings.wheelStep);
            return 0;
        }

        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE:
                    HidePanel();
                    break;
                case VK_UP:
                case VK_TAB:
                    MoveFocus(wParam == VK_TAB && GetKeyState(VK_SHIFT) >= 0
                                  ? 1
                                  : -1);
                    break;
                case VK_DOWN:
                    MoveFocus(1);
                    break;
                case VK_RIGHT:
                    AdjustRow(g_focusRow, 1);
                    break;
                case VK_LEFT:
                    AdjustRow(g_focusRow, -1);
                    break;
                case VK_PRIOR:
                    AdjustRow(g_focusRow, 10);
                    break;
                case VK_NEXT:
                    AdjustRow(g_focusRow, -10);
                    break;
                case VK_HOME:
                    AdjustRow(g_focusRow, -100);
                    break;
                case VK_END:
                    AdjustRow(g_focusRow, 100);
                    break;
            }
            return 0;

        case WM_DPICHANGED:
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// On-screen indicator

int g_osdAlpha;

struct OsdItem {
    std::wstring label;
    int value;
};

constexpr size_t OSD_MAX_ROWS = 6;

// When the targets are at different levels, the indicator shows one row per
// display (up to OSD_MAX_ROWS) instead of a single, misleading value.
std::vector<OsdItem> BuildOsdItems(const std::vector<UiDisplay*>& targets) {
    bool sameValue = std::all_of(
        targets.begin(), targets.end(),
        [&](const UiDisplay* d) { return d->value == targets[0]->value; });
    if (targets.size() == 1 || sameValue) {
        std::wstring label;
        if (targets.size() == 1) {
            label = targets[0]->label;
        } else if (targets.size() == ControllableDisplays().size()) {
            label = L"All displays";
        } else {
            label = std::to_wstring(targets.size()) + L" displays";
        }
        return {{label, targets[0]->value}};
    }

    std::vector<OsdItem> items;
    for (size_t i = 0; i < targets.size() && i < OSD_MAX_ROWS; i++) {
        items.push_back({targets[i]->label, targets[i]->value});
    }
    return items;
}

void GetOsdSize(size_t rows, UINT dpi, int* w, int* h) {
    auto px = [dpi](int dip) { return MulDiv(dip, (int)dpi, 96); };
    if (rows <= 1) {
        *w = px(280);
        *h = px(60);
    } else {
        *w = px(370);
        *h = px(20 + 30 * (int)rows);
    }
}

void PaintOsd(Gdiplus::Bitmap& canvas,
              UINT dpi,
              const std::vector<OsdItem>& items);

void ShowOsd(const std::vector<UiDisplay*>& targets, HMONITOR monitor) {
    if (!g_settings.showOsd || targets.empty() || !g_hwndOsd) {
        return;
    }

    std::vector<OsdItem> items = BuildOsdItems(targets);

    if (!monitor) {
        monitor = targets.size() == 1
                      ? targets[0]->info.monitor
                      : MonitorFromWindow(GetForegroundWindow(),
                                          MONITOR_DEFAULTTOPRIMARY);
    }
    UINT dpi = GetMonitorDpi(monitor);
    auto px = [dpi](int dip) { return MulDiv(dip, (int)dpi, 96); };

    int w, h;
    GetOsdSize(items.size(), dpi, &w, &h);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(monitor, &mi);
    POINT pos = {(mi.rcWork.left + mi.rcWork.right - w) / 2,
                 mi.rcWork.bottom - h - px(48)};
    SIZE size = {w, h};

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    HBITMAP dib = CreateDIBSection(screen, &bmi, DIB_RGB_COLORS, &bits,
                                   nullptr, 0);
    if (!dib) {
        DeleteDC(mem);
        ReleaseDC(nullptr, screen);
        return;
    }
    HGDIOBJ oldBitmap = SelectObject(mem, dib);

    {
        Gdiplus::Bitmap canvas(w, h, w * 4, PixelFormat32bppPARGB, (BYTE*)bits);
        PaintOsd(canvas, dpi, items);
    }

    g_osdAlpha = 255;
    POINT zero = {0, 0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwndOsd, screen, &pos, &size, mem, &zero, 0, &blend,
                        ULW_ALPHA);
    SetWindowPos(g_hwndOsd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

    SelectObject(mem, oldBitmap);
    DeleteObject(dib);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);

    KillTimer(g_hwndOsd, TIMER_OSD_FADE);
    SetTimer(g_hwndOsd, TIMER_OSD_HIDE, 1500, nullptr);
}

void DrawOsdBar(Gdiplus::Graphics& g,
                const Theme& t,
                float left,
                float right,
                float cy,
                int value,
                float scale) {
    float th = 4 * scale;
    FillRoundRect(g, t.track, Gdiplus::RectF(left, cy - th / 2, right - left, th),
                  th / 2);
    if (value > 0) {
        FillRoundRect(g, t.accent,
                      Gdiplus::RectF(left, cy - th / 2,
                                     (right - left) * value / 100.0f, th),
                      th / 2);
    }
}

void PaintOsd(Gdiplus::Bitmap& canvas,
              UINT dpi,
              const std::vector<OsdItem>& items) {
    auto px = [dpi](int dip) { return MulDiv(dip, (int)dpi, 96); };
    float scale = dpi / 96.0f;
    int w = (int)canvas.GetWidth(), h = (int)canvas.GetHeight();

    Gdiplus::Graphics g(&canvas);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    g.Clear(Gdiplus::Color(0, 0, 0, 0));
    Theme t = GetTheme();

    Gdiplus::RectF box(0.5f, 0.5f, w - 1.0f, h - 1.0f);
    FillRoundRect(g, t.background, box, 8 * scale);
    Gdiplus::GraphicsPath borderPath;
    RoundRectPath(&borderPath, box, 8 * scale);
    Gdiplus::Pen borderPen(t.border, 1);
    g.DrawPath(&borderPen, &borderPath);

    DrawSun(g, (float)px(30), h / 2.0f, (float)px(24), t.text);

    auto valueText = [](int value) {
        return value >= 0 ? std::to_wstring(value) : std::wstring(L"-");
    };

    if (items.size() == 1) {
        const OsdItem& item = items[0];
        DrawLabel(g, item.label,
                  Gdiplus::RectF((float)px(54), (float)px(6),
                                 (float)(w - px(54) - px(64)), (float)px(22)),
                  (float)px(14), t.text, Gdiplus::StringAlignmentNear, nullptr,
                  Gdiplus::FontStyleBold);
        DrawOsdBar(g, t, (float)px(54), (float)(w - px(64)), (float)px(38),
                   item.value, scale);
        DrawLabel(g, valueText(item.value),
                  Gdiplus::RectF((float)(w - px(60)), 0, (float)px(44),
                                 (float)h),
                  (float)px(18), t.text, Gdiplus::StringAlignmentFar, nullptr,
                  Gdiplus::FontStyleBold);
        return;
    }

    // One row per display: name, bar, value.
    for (size_t i = 0; i < items.size(); i++) {
        float top = (float)px(10 + 30 * (int)i);
        float cy = top + px(15);
        DrawLabel(g, items[i].label,
                  Gdiplus::RectF((float)px(54), top, (float)px(140),
                                 (float)px(30)),
                  (float)px(14), t.text, Gdiplus::StringAlignmentNear, nullptr,
                  Gdiplus::FontStyleBold);
        DrawOsdBar(g, t, (float)px(200), (float)(w - px(56)), cy,
                   items[i].value, scale);
        DrawLabel(g, valueText(items[i].value),
                  Gdiplus::RectF((float)(w - px(52)), top, (float)px(36),
                                 (float)px(30)),
                  (float)px(14), t.text, Gdiplus::StringAlignmentFar, nullptr,
                  Gdiplus::FontStyleBold);
    }
}

LRESULT CALLBACK OsdWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER) {
        if (wParam == TIMER_OSD_HIDE) {
            KillTimer(hwnd, TIMER_OSD_HIDE);
            SetTimer(hwnd, TIMER_OSD_FADE, 16, nullptr);
        } else if (wParam == TIMER_OSD_FADE) {
            g_osdAlpha -= 32;
            if (g_osdAlpha <= 0) {
                KillTimer(hwnd, TIMER_OSD_FADE);
                ShowWindow(hwnd, SW_HIDE);
            } else {
                BLENDFUNCTION blend = {AC_SRC_OVER, 0, (BYTE)g_osdAlpha,
                                       AC_SRC_ALPHA};
                UpdateLayeredWindow(hwnd, nullptr, nullptr, nullptr, nullptr,
                                    nullptr, 0, &blend, ULW_ALPHA);
            }
        }
        return 0;
    }
    if (msg == WM_NCHITTEST) {
        return HTTRANSPARENT;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Hotkeys and mouse wheel

void RegisterHotkeys() {
    for (int i = 1; i <= g_registeredHotkeys; i++) {
        UnregisterHotKey(g_hwndMain, i);
    }
    g_registeredHotkeys = (int)g_settings.hotkeys.size();
    for (int i = 0; i < g_registeredHotkeys; i++) {
        const auto& hk = g_settings.hotkeys[i];
        if (!RegisterHotKey(g_hwndMain, i + 1, hk.modifiers, hk.vk)) {
            Wh_Log(L"RegisterHotKey failed for %s (error %u)", hk.text.c_str(),
                   GetLastError());
        }
    }
}

void UnregisterHotkeys() {
    for (int i = 1; i <= g_registeredHotkeys; i++) {
        UnregisterHotKey(g_hwndMain, i);
    }
    g_registeredHotkeys = 0;
}

// The monitor of the active window, or the one under the cursor when the
// desktop or the taskbar is active.
HMONITOR ActiveMonitor() {
    HWND fg = GetForegroundWindow();
    WCHAR className[64] = L"";
    if (fg) {
        GetClassNameW(fg, className, ARRAYSIZE(className));
    }
    if (!fg || wcscmp(className, L"Progman") == 0 ||
        wcscmp(className, L"WorkerW") == 0 || IsTaskbarWindow(fg)) {
        POINT pt;
        GetCursorPos(&pt);
        return MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    }
    return MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
}

// What a hotkey without a display setting, or the mouse wheel, changes: all
// displays while they are linked, otherwise the display on the monitor.
std::vector<UiDisplay*> DefaultTargets(HMONITOR monitor) {
    if (g_linked) {
        return ControllableDisplays();
    }
    std::vector<UiDisplay*> result;
    for (auto* d : ControllableDisplays()) {
        if (d->info.monitor == monitor) {
            result.push_back(d);
        }
    }
    return result;
}

// Changes the displays by delta, or sets them to setValue (>= 0). Linked
// displays move as one, from their average level, so they end up equal.
void ChangeDisplays(const std::vector<UiDisplay*>& targets,
                    int delta,
                    int setValue,
                    bool linked) {
    int linkedBase = 50;
    if (linked) {
        int sum = 0, count = 0;
        for (auto* d : targets) {
            if (d->value >= 0) {
                sum += d->value;
                count++;
            }
        }
        if (count) {
            linkedBase = (int)std::lround(sum / (double)count);
        }
    }
    for (auto* d : targets) {
        int current = linked ? linkedBase : (d->value < 0 ? 50 : d->value);
        SetDisplayValue(*d, setValue >= 0 ? setValue : current + delta, true);
    }
}

void OnHotkey(int id) {
    if (id < 1 || id > (int)g_settings.hotkeys.size()) {
        return;
    }
    const HotkeyConfig& hk = g_settings.hotkeys[id - 1];
    switch (hk.action) {
        case HotkeyAction::TurnOff:
            TurnOffDisplays();
            return;
        case HotkeyAction::Panel:
            if (IsWindowVisible(g_hwndPanel)) {
                HidePanel();
            } else {
                ShowPanel();
            }
            return;
        default:
            break;
    }

    int delta = hk.action == HotkeyAction::Increase   ? hk.value
                : hk.action == HotkeyAction::Decrease ? -hk.value
                                                      : 0;
    int setValue = hk.action == HotkeyAction::Set ? hk.value : -1;

    if (!hk.display.empty()) {
        std::vector<UiDisplay*> targets = MatchDisplays(hk.display);
        ChangeDisplays(targets, delta, setValue, false);
        ShowOsd(targets, nullptr);
        return;
    }
    HMONITOR monitor = ActiveMonitor();
    std::vector<UiDisplay*> targets = DefaultTargets(monitor);
    ChangeDisplays(targets, delta, setValue, g_linked);
    ShowOsd(targets, g_linked ? nullptr : monitor);
}

// The taskbar, or the notification area's overflow flyout.
bool IsTaskbarWindow(HWND hwnd) {
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) {
        return false;
    }
    WCHAR className[64];
    if (!GetClassNameW(root, className, ARRAYSIZE(className))) {
        return false;
    }
    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
           wcscmp(className, L"NotifyIconOverflowWindow") == 0 ||
           wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") == 0;
}

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && wParam == WM_MOUSEWHEEL) {
        auto* info = (MSLLHOOKSTRUCT*)lParam;
        WheelHookConfig config;
        {
            std::lock_guard<std::mutex> lock(g_wheelHookMutex);
            config = g_wheelHookConfig;
        }
        bool inArea = false;
        if (config.mode == WheelMode::TrayIcon) {
            // Also check the window, so that a window covering the icon's
            // position (e.g. a full-screen video) keeps its wheel events.
            inArea = PtInRect(&config.trayIconRect, info->pt) &&
                     IsTaskbarWindow(WindowFromPoint(info->pt));
        } else if (config.mode == WheelMode::Taskbar) {
            inArea = IsTaskbarWindow(WindowFromPoint(info->pt));
        }
        bool modifierOk = config.modifier == 0 ||
                          (GetAsyncKeyState(config.modifier) & 0x8000);
        HWND main = g_hwndMain;
        if (inArea && modifierOk && main) {
            PostMessage(main, WM_APP_WHEEL,
                        (WPARAM)(short)HIWORD(info->mouseData),
                        MAKELPARAM(info->pt.x, info->pt.y));
            return 1;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID readyEvent) {
    // Create the message queue before signaling, so that StopHookThread's
    // WM_QUIT can't get lost.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    HHOOK hook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, g_module, 0);
    SetEvent((HANDLE)readyEvent);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx failed (error %u)", GetLastError());
        return 1;
    }
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        DispatchMessageW(&msg);
    }
    UnhookWindowsHookEx(hook);
    return 0;
}

void StopHookThread() {
    if (!g_hookThread) {
        return;
    }
    PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_hookThread, 3000);
    CloseHandle(g_hookThread);
    g_hookThread = nullptr;
    g_hookThreadId = 0;
}

void UpdateTrayIconRectForWheel() {
    RECT rect{};
    if (g_settings.wheelMode != WheelMode::TrayIcon ||
        !GetTrayIconRect(&rect)) {
        SetRectEmpty(&rect);
    }
    std::lock_guard<std::mutex> lock(g_wheelHookMutex);
    g_wheelHookConfig.trayIconRect = rect;
}

void OnWheel(int delta, POINT pt) {
    g_wheelAccumulator += delta;
    int steps = g_wheelAccumulator / WHEEL_DELTA;
    g_wheelAccumulator %= WHEEL_DELTA;
    if (steps == 0) {
        return;
    }

    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    std::vector<UiDisplay*> targets = DefaultTargets(monitor);
    ChangeDisplays(targets, steps * g_settings.wheelStep, -1, g_linked);
    ShowOsd(targets, monitor);
}

void UpdateMouseHook() {
    {
        std::lock_guard<std::mutex> lock(g_wheelHookMutex);
        g_wheelHookConfig.mode = g_settings.wheelMode;
        g_wheelHookConfig.modifier = g_settings.wheelModifier;
    }
    // The tray icon's rect is refreshed from the icon's WM_MOUSEMOVE
    // notifications, TaskbarCreated and display changes, not by polling.
    UpdateTrayIconRectForWheel();

    bool wanted = g_settings.wheelMode != WheelMode::Off;
    if (wanted && !g_hookThread) {
        HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        g_hookThread =
            CreateThread(nullptr, 0, HookThread, ready, 0, &g_hookThreadId);
        if (g_hookThread) {
            HANDLE handles[] = {ready, g_hookThread};
            WaitForMultipleObjects(2, handles, FALSE, 5000);
        }
        CloseHandle(ready);
    } else if (!wanted) {
        StopHookThread();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Schedule and idle dimming

long long LocalMinutesNow() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);
    ULARGE_INTEGER u;
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    return (long long)(u.QuadPart / 600000000ULL);
}

void ApplyScheduledValue(UiDisplay& d, int target, DWORD durationMs) {
    if (g_idleDimmed) {
        // Don't brighten dimmed displays; apply the level when the user is
        // back instead.
        if (target > g_settings.idleBrightness) {
            g_idleRestore[d.info.key] = target;
            SaveIdleRestore();
            return;
        }
        g_idleRestore.erase(d.info.key);
        SaveIdleRestore();
    }
    StartFade(d, target, durationMs);
}

void CheckSchedule(bool startup) {
    if (g_reenumPending) {
        return;  // Wait for valid display handles, e.g. after resume.
    }

    long long now = LocalMinutesNow();
    long long dayStart = now - now % 1440;
    // Startup and resume from sleep are catch-ups: only the latest entry
    // matters, and only if "apply on startup and after sleep" is on.
    bool catchUp = startup || g_resumeCatchUp || g_lastScheduleCheck == 0 ||
                   now - g_lastScheduleCheck > 1440;
    g_resumeCatchUp = false;
    long long last = g_lastScheduleCheck;
    g_lastScheduleCheck = now;

    if (g_settings.schedule.empty() || g_paused) {
        return;
    }
    if (catchUp && !g_settings.applyScheduleOnStartup) {
        return;
    }

    struct Hit {
        long long time;
        const ScheduleEntry* entry;
    };
    std::vector<Hit> hits;
    for (const auto& entry : g_settings.schedule) {
        long long t = dayStart + entry.minuteOfDay;
        if (t > now) {
            t -= 1440;
        }
        if (catchUp || (t > last && t <= now)) {
            hits.push_back({t, &entry});
        }
    }
    std::stable_sort(hits.begin(), hits.end(), [](const Hit& a, const Hit& b) {
        return a.time < b.time;
    });

    DWORD duration = catchUp ? 1000 : g_settings.transitionSeconds * 1000;
    for (const auto& hit : hits) {
        Wh_Log(L"Schedule: %02d:%02d -> %d%%", hit.entry->minuteOfDay / 60,
               hit.entry->minuteOfDay % 60, hit.entry->brightness);
        for (auto* d : MatchDisplays(hit.entry->display)) {
            ApplyScheduledValue(*d, hit.entry->brightness, duration);
        }
    }
}

// Arms a one-shot timer for the next schedule entry instead of polling.
// Re-armed after it fires, on settings changes, time changes and after
// resume.
void ArmScheduleTimer() {
    if (g_settings.schedule.empty() || !g_haveSnapshot) {
        KillTimer(g_hwndMain, TIMER_SCHEDULE);
        return;
    }
    SYSTEMTIME st;
    GetLocalTime(&st);
    long long msOfDay =
        ((st.wHour * 60LL + st.wMinute) * 60 + st.wSecond) * 1000 +
        st.wMilliseconds;
    long long next = LLONG_MAX;
    for (const auto& entry : g_settings.schedule) {
        long long delta = entry.minuteOfDay * 60000LL - msOfDay;
        if (delta <= 0) {
            delta += 24 * 60 * 60000LL;
        }
        next = std::min(next, delta);
    }
    // Fire just after the minute starts, so that CheckSchedule sees it.
    SetTimer(g_hwndMain, TIMER_SCHEDULE, (UINT)(next + 500), nullptr);
}

void RunScheduleCheck(bool startup) {
    CheckSchedule(startup);
    ArmScheduleTimer();
}

bool IsFullscreenAppRunning() {
    QUERY_USER_NOTIFICATION_STATE state;
    if (FAILED(SHQueryUserNotificationState(&state))) {
        return false;
    }
    return state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN ||
           state == QUNS_PRESENTATION_MODE;
}

// The levels from before idle dimming are also kept in the mod's storage, so
// that they can be restored at the next start if the session ends while the
// displays are dimmed (restart for updates, shutdown, crash). All users'
// tool processes share the storage, so the value names include the user SID.
std::wstring StorageName(PCWSTR name, int index = -1) {
    std::wstring result = g_storagePrefix + name;
    if (index >= 0) {
        result += std::to_wstring(index);
    }
    return result;
}

void SaveIdleRestore() {
    int oldCount =
        Wh_GetIntValue(StorageName(L"idleRestoreCount").c_str(), 0);
    int n = 0;
    for (const auto& [key, value] : g_idleRestore) {
        Wh_SetStringValue(StorageName(L"idleRestoreKey", n).c_str(),
                          key.c_str());
        Wh_SetIntValue(StorageName(L"idleRestoreValue", n).c_str(), value);
        n++;
    }
    Wh_SetIntValue(StorageName(L"idleRestoreCount").c_str(), n);
    for (int i = n; i < oldCount; i++) {
        Wh_DeleteValue(StorageName(L"idleRestoreKey", i).c_str());
        Wh_DeleteValue(StorageName(L"idleRestoreValue", i).c_str());
    }
}

void ClearSavedIdleRestore() {
    int count = Wh_GetIntValue(StorageName(L"idleRestoreCount").c_str(), 0);
    for (int i = 0; i < count; i++) {
        Wh_DeleteValue(StorageName(L"idleRestoreKey", i).c_str());
        Wh_DeleteValue(StorageName(L"idleRestoreValue", i).c_str());
    }
    if (count) {
        Wh_DeleteValue(StorageName(L"idleRestoreCount").c_str());
    }
}

// Called after the first enumeration: undoes a dim that outlived the
// previous session.
void RestoreSavedIdleLevels() {
    int count = Wh_GetIntValue(StorageName(L"idleRestoreCount").c_str(), 0);
    for (int i = 0; i < count && i < 64; i++) {
        WCHAR key[512];
        Wh_GetStringValue(StorageName(L"idleRestoreKey", i).c_str(), key,
                          ARRAYSIZE(key));
        int value =
            Wh_GetIntValue(StorageName(L"idleRestoreValue", i).c_str(), -1);
        UiDisplay* d = FindDisplay(key);
        if (d && value >= 0) {
            Wh_Log(L"Restoring %s to %d%% after an interrupted idle dim",
                   d->label.c_str(), value);
            SetDisplayValue(*d, value, false);
        }
    }
    ClearSavedIdleRestore();
}

// keepBackup: leave the saved levels in storage; the caller clears them once
// the writes are known to have reached the displays.
void SetLinked(bool linked) {
    g_linked = linked;
    Wh_SetIntValue(StorageName(L"linked").c_str(), linked ? 1 : 0);
    if (linked) {
        // Bring the displays together, like a click on the shared slider.
        ChangeDisplays(ControllableDisplays(), 0, -1, true);
    }
    UpdatePanel();
}

void RestoreFromIdle(bool immediate, bool keepBackup = false) {
    if (!g_idleDimmed) {
        return;
    }
    g_idleDimmed = false;
    if (!keepBackup) {
        ClearSavedIdleRestore();
    }
    for (const auto& [key, value] : g_idleRestore) {
        if (UiDisplay* d = FindDisplay(key)) {
            if (immediate) {
                CancelFade(key);
                SetDisplayValue(*d, value, false);
            } else {
                StartFade(*d, value, 300);
            }
        }
    }
    g_idleRestore.clear();
}

// The idle timer doesn't poll: while the displays aren't dimmed it fires when
// the idle time could first be reached. Only while dimmed does it check every
// second for input, to restore quickly.
void ArmIdleTimer(DWORD delayMs) {
    SetTimer(g_hwndMain, TIMER_IDLE, std::max<DWORD>(delayMs, 1000), nullptr);
}

void OnIdleTimer() {
    if (!g_settings.idleEnabled) {
        KillTimer(g_hwndMain, TIMER_IDLE);
        RestoreFromIdle(false);
        return;
    }
    if (!g_haveSnapshot) {
        // Wait for the first enumeration, so that the levels saved by an
        // interrupted dim are restored before they could be overwritten.
        ArmIdleTimer(2000);
        return;
    }

    LASTINPUTINFO lii{sizeof(lii)};
    if (!GetLastInputInfo(&lii)) {
        ArmIdleTimer(60000);
        return;
    }
    DWORD idleMs = GetTickCount() - lii.dwTime;
    DWORD thresholdMs = (DWORD)g_settings.idleMinutes * 60000;

    if (g_idleDimmed) {
        if (idleMs < 1500 || g_paused) {
            RestoreFromIdle(false);
            ArmIdleTimer(thresholdMs);
        } else {
            ArmIdleTimer(1000);
        }
        return;
    }

    if (g_paused) {
        ArmIdleTimer(thresholdMs);
        return;
    }
    if (idleMs < thresholdMs) {
        ArmIdleTimer(thresholdMs - idleMs);
        return;
    }
    if (g_settings.idleIgnoreFullscreen && IsFullscreenAppRunning()) {
        ArmIdleTimer(30000);
        return;
    }

    ArmIdleTimer(1000);
    g_idleDimmed = true;
    g_idleRestore.clear();
    for (auto* d : ControllableDisplays()) {
        int current = d->value;
        for (const auto& f : g_fades) {
            if (f.key == d->info.key) {
                current = f.to;  // Restore to where a transition was heading.
            }
        }
        if (current > g_settings.idleBrightness) {
            g_idleRestore[d->info.key] = current;
            StartFade(*d, g_settings.idleBrightness, 2000);
        }
    }
    SaveIdleRestore();
    Wh_Log(L"Idle: dimmed %d display(s)", (int)g_idleRestore.size());
}

////////////////////////////////////////////////////////////////////////////////
// Tray menu

void CopyDisplayInfo() {
    std::wstring text;
    for (const auto& d : g_displays) {
        PCWSTR method = d.info.method == Method::Ddc   ? L"DDC/CI"
                        : d.info.method == Method::Wmi ? L"WMI"
                                                       : L"not supported";
        text += d.info.name + L"\t" + d.info.id + L"\t" + method + L"\r\n";
    }
    if (text.empty() || !OpenClipboard(g_hwndMain)) {
        return;
    }
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(WCHAR);
    if (HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes)) {
        void* data = GlobalLock(mem);
        if (!data) {
            GlobalFree(mem);
            CloseClipboard();
            return;
        }
        memcpy(data, text.c_str(), bytes);
        GlobalUnlock(mem);
        if (!SetClipboardData(CF_UNICODETEXT, mem)) {
            GlobalFree(mem);
        }
    }
    CloseClipboard();
}

void ShowTrayMenu(POINT pt) {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, MENU_REFRESH, L"Refresh displays");
    AppendMenuW(menu, MF_STRING, MENU_TURN_OFF, L"Turn off displays");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | (g_paused ? MF_CHECKED : 0), MENU_PAUSE,
                L"Pause automatic brightness");
    AppendMenuW(menu, MF_STRING, MENU_COPY_INFO, L"Copy display info");

    SetForegroundWindow(g_hwndMain);
    UINT cmd = TrackPopupMenuEx(menu,
                                TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON |
                                    (GetSystemMetrics(SM_MENUDROPALIGNMENT)
                                         ? TPM_RIGHTALIGN
                                         : TPM_LEFTALIGN),
                                pt.x, pt.y, g_hwndMain, nullptr);
    PostMessage(g_hwndMain, WM_NULL, 0, 0);
    DestroyMenu(menu);

    switch (cmd) {
        case MENU_REFRESH:
            hw::RequestEnumerate();
            break;
        case MENU_TURN_OFF:
            TurnOffDisplays();
            break;
        case MENU_PAUSE:
            g_paused = !g_paused;
            if (g_paused) {
                g_fades.clear();
                KillTimer(g_hwndMain, TIMER_FADE);
                RestoreFromIdle(false);
            }
            UpdateTrayTip();
            if (IsWindowVisible(g_hwndPanel)) {
                InvalidateRect(g_hwndPanel, nullptr, FALSE);
            }
            break;
        case MENU_COPY_INFO:
            CopyDisplayInfo();
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Main window

void ApplySettings() {
    RegisterHotkeys();
    UpdateMouseHook();
    if (g_settings.idleEnabled) {
        ArmIdleTimer(1000);
    } else {
        KillTimer(g_hwndMain, TIMER_IDLE);
        RestoreFromIdle(false);
    }
    ArmScheduleTimer();
    RebuildDisplays(g_lastSnapshot, false);
    UpdatePanel();
    UpdateTrayTip();
}

void Shutdown() {
    RestoreFromIdle(true);
    g_fades.clear();
    KillTimer(g_hwndMain, TIMER_FADE);
    UnregisterHotkeys();
    StopHookThread();
    RemoveTrayIcon();
    if (g_hwndPanel) {
        DestroyWindow(g_hwndPanel);
        g_hwndPanel = nullptr;
    }
    if (g_hwndOsd) {
        DestroyWindow(g_hwndOsd);
        g_hwndOsd = nullptr;
    }
    HWND main = g_hwndMain;
    g_hwndMain = nullptr;
    DestroyWindow(main);
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg) {
        g_trayAdded = false;
        AddTrayIcon();
        UpdateMouseHook();
        return 0;
    }

    switch (msg) {
        case WM_APP_TRAY:
            switch (LOWORD(lParam)) {
                case NIN_SELECT:
                case NIN_KEYSELECT:
                    TogglePanel();
                    break;
                case WM_CONTEXTMENU:
                    HidePanel();
                    ShowTrayMenu({GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam)});
                    break;
                case WM_MOUSEMOVE:
                    // The icon may have moved (taskbar resized, icons
                    // reordered): refresh the rectangle the wheel hook uses.
                    if (g_settings.wheelMode == WheelMode::TrayIcon) {
                        POINT pt{GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam)};
                        RECT rect;
                        {
                            std::lock_guard<std::mutex> lock(g_wheelHookMutex);
                            rect = g_wheelHookConfig.trayIconRect;
                        }
                        if (!PtInRect(&rect, pt)) {
                            UpdateTrayIconRectForWheel();
                        }
                    }
                    break;
            }
            return 0;

        case WM_APP_SNAPSHOT: {
            bool enumerated = wParam != 0;
            g_lastSnapshot = hw::TakeSnapshot();
            RebuildDisplays(g_lastSnapshot, true);
            UpdatePanel();
            UpdateTrayTip();
            if (enumerated) {
                g_reenumPending = false;
            }
            if (!g_haveSnapshot) {
                g_haveSnapshot = true;
                RestoreSavedIdleLevels();
                RunScheduleCheck(true);
            } else if (enumerated) {
                // Entries skipped while re-enumerating, and the catch-up
                // after resume.
                RunScheduleCheck(false);
            }
            return 0;
        }

        case WM_QUERYENDSESSION:
            // Don't leave the displays dimmed when the user signs out or
            // shuts down while idle.
            if (g_idleDimmed) {
                RestoreFromIdle(true, true);
                if (hw::WaitUntilIdle(3000)) {
                    ClearSavedIdleRestore();
                }
            }
            return TRUE;

        case WM_TIMECHANGE:
            RunScheduleCheck(false);
            return 0;

        case WM_APP_SETTINGS:
            LoadSettings();
            ApplySettings();
            return 0;

        case WM_APP_SHUTDOWN:
            Shutdown();
            return 0;

        case WM_APP_WHEEL:
            OnWheel((int)(short)wParam,
                    {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            return 0;

        case WM_HOTKEY:
            OnHotkey((int)wParam);
            return 0;

        case WM_TIMER:
            switch (wParam) {
                case TIMER_REENUM:
                    KillTimer(hwnd, TIMER_REENUM);
                    hw::RequestEnumerate();
                    break;
                case TIMER_SCHEDULE:
                    RunScheduleCheck(false);
                    break;
                case TIMER_IDLE:
                    OnIdleTimer();
                    break;
                case TIMER_FADE:
                    OnFadeTimer();
                    break;
                case TIMER_TOOLTIP:
                    KillTimer(hwnd, TIMER_TOOLTIP);
                    UpdateTrayTip();
                    break;
                case TIMER_MONITOR_OFF:
                    KillTimer(hwnd, TIMER_MONITOR_OFF);
                    DefWindowProcW(hwnd, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
                    break;
            }
            return 0;

        case WM_DISPLAYCHANGE:
            g_reenumPending = true;
            SetTimer(hwnd, TIMER_REENUM, 2000, nullptr);
            UpdateTrayIconRectForWheel();
            break;

        case WM_POWERBROADCAST:
            if (wParam == PBT_APMRESUMEAUTOMATIC ||
                wParam == PBT_APMRESUMESUSPEND) {
                g_reenumPending = true;
                g_resumeCatchUp = true;
                SetTimer(hwnd, TIMER_REENUM, 3000, nullptr);
            }
            return TRUE;

        case WM_SETTINGCHANGE:
            if (lParam &&
                wcscmp((PCWSTR)lParam, L"ImmersiveColorSet") == 0) {
                UpdateTrayIconImage();
                if (IsWindowVisible(g_hwndPanel)) {
                    ApplyPanelFrame();
                    InvalidateRect(g_hwndPanel, nullptr, FALSE);
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool IsWindows11() {
    using RtlGetVersion_t = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);
    auto rtlGetVersion = (RtlGetVersion_t)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
    RTL_OSVERSIONINFOW info{sizeof(info)};
    return rtlGetVersion && rtlGetVersion(&info) == 0 &&
           info.dwBuildNumber >= 22000;
}

// The prefix for the mod's storage value names: the current user's SID.
std::wstring GetStoragePrefix() {
    std::wstring prefix;
    HANDLE token;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        BYTE buffer[256];
        DWORD size;
        if (GetTokenInformation(token, TokenUser, buffer, sizeof(buffer),
                                &size)) {
            PWSTR sid;
            if (ConvertSidToStringSidW(((TOKEN_USER*)buffer)->User.Sid, &sid)) {
                prefix = std::wstring(sid) + L"_";
                LocalFree(sid);
            }
        }
        CloseHandle(token);
    }
    return prefix;
}

DWORD WINAPI UiThread(LPVOID) {
    g_storagePrefix = GetStoragePrefix();
    g_linked = Wh_GetIntValue(StorageName(L"linked").c_str(), 1) != 0;

    Gdiplus::GdiplusStartupInput gdiplusInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr);
    {
        Gdiplus::FontFamily family(L"Segoe UI Variable Text");
        if (family.IsAvailable()) {
            g_fontFamily = L"Segoe UI Variable Text";
        }
    }

    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = g_module;
    wc.lpszClassName = L"TwinkleBrightness_Main";
    RegisterClassExW(&wc);

    wc.lpfnWndProc = PanelWndProc;
    wc.style = CS_DROPSHADOW;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"TwinkleBrightness_Panel";
    RegisterClassExW(&wc);

    wc.lpfnWndProc = OsdWndProc;
    wc.style = 0;
    wc.lpszClassName = L"TwinkleBrightness_Osd";
    RegisterClassExW(&wc);

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    HWND main = CreateWindowExW(WS_EX_TOOLWINDOW, L"TwinkleBrightness_Main",
                                L"Twinkle Brightness", WS_POPUP, 0, 0, 0, 0,
                                nullptr, nullptr, g_module, nullptr);
    g_hwndMain = main;
    g_hwndPanel = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"TwinkleBrightness_Panel",
        L"Brightness", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, g_module,
        nullptr);
    g_hwndOsd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT |
                                    WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                                    WS_EX_NOACTIVATE,
                                L"TwinkleBrightness_Osd", L"", WS_POPUP, 0, 0,
                                0, 0, nullptr, nullptr, g_module, nullptr);

    // Let Explorer's TaskbarCreated broadcast reach us even if Windhawk runs
    // elevated.
    ChangeWindowMessageFilterEx(main, g_taskbarCreatedMsg, MSGFLT_ALLOW,
                                nullptr);

    SetEvent(g_uiReadyEvent);

    if (!main || !g_hwndPanel) {
        Wh_Log(L"Creating windows failed");
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        return 1;
    }

    AddTrayIcon();
    ApplySettings();
    hw::RequestEnumerate();

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    Gdiplus::GdiplusShutdown(g_gdiplusToken);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Tool mod callbacks

BOOL WhTool_ModInit() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&WhTool_ModInit, &g_module);
    g_isWin11 = IsWindows11();
    LoadSettings();

    g_workerThread = CreateThread(nullptr, 0, hw::WorkerThread, nullptr, 0,
                                  nullptr);
    g_uiReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiThread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    if (!g_workerThread || !g_uiThread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }
    WaitForSingleObject(g_uiReadyEvent, 10000);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (HWND hwnd = g_hwndMain) {
        PostMessage(hwnd, WM_APP_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (HWND hwnd = g_hwndMain) {
        SendMessageTimeoutW(hwnd, WM_APP_SHUTDOWN, 0, 0, SMTO_NORMAL, 5000,
                            nullptr);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 5000);
        CloseHandle(g_uiThread);
    }
    hw::RequestStop();
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
    }
    if (g_uiReadyEvent) {
        CloseHandle(g_uiReadyEvent);
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
