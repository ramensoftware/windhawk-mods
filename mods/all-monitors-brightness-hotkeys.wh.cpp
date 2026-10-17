// ==WindhawkMod==
// @id              all-monitors-brightness-hotkeys
// @name            Brightness Hotkeys for All Monitors
// @description     Change the brightness of every display with keyboard shortcuts: external monitors via DDC/CI, laptop screens via WMI, and a software-dimming fallback for the rest
// @version         1.0
// @author          Priyanshu Yadav
// @github          https://github.com/PriyanshuGeTRekT
// @homepage        https://github.com/PriyanshuGeTRekT/Universal-Monitor-Brightness-control
// @include         windhawk.exe
// @compilerOptions -ldxva2 -lole32 -loleaut32 -lwbemuuid -lgdi32 -luser32 -lshell32 -ldwmapi -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Brightness Hotkeys for All Monitors

Change the brightness of **every** display with two keyboard shortcuts, from
any app. That includes external monitors, not just the laptop screen. All
displays move together by the chosen step, and each keeps its own level.

The default shortcuts are **Ctrl + Alt + Up** (brighter) and
**Ctrl + Alt + Down** (dimmer). Hold them to keep going.

![The popup](https://raw.githubusercontent.com/PriyanshuGeTRekT/Universal-Monitor-Brightness-control/main/docs/images/windhawk-popup.png)

## How each display is controlled

- **External monitors** use **DDC/CI** (VCP code `0x10`), the same channel the
  monitor's own on-screen menu uses. If a monitor doesn't respond, turn on
  *DDC/CI* in its on-screen menu. Some docks and adapters don't pass DDC/CI
  through.
- **Laptop and other built-in screens** use **WMI**, the same as the brightness
  keys.
- **Anything else** (TVs, DisplayLink docks, monitors with DDC/CI turned off)
  can use **software dimming**: a click-through dark overlay that is left out
  of screenshots and screen sharing. It darkens the picture but doesn't lower
  the backlight. It can be turned off in the settings.

## The popup

A small popup in the corner shows every display's new level. It never takes
focus and clicks go through it.

While a game runs in **exclusive full screen**, the popup isn't shown by
default, because any window appearing over such a game knocks it out of full
screen. The brightness still changes. Games in borderless full screen look like
normal windows to Windows, so they still get the popup unless *Show popup* is
set to *Never*.

## Settings

- **Brighter / Dimmer shortcut**, for example `Ctrl+Alt+Up`. Modifiers: `Ctrl`,
  `Alt`, `Shift`, `Win`. Keys: `A`–`Z`, `0`–`9`, `F1`–`F24`, `Up`, `Down`,
  `Left`, `Right`, `PageUp`, `PageDown`, `Home`, `End`, `Insert`, `Delete`,
  `Space`, `Plus`, `Minus`. Leave a shortcut empty to turn it off. A shortcut
  that another program already uses is skipped; the mod's log says which.
- **Change per press** in percent.
- **Show popup**: *Except over full-screen games*, *Always* or *Never*.
- **Popup duration** in milliseconds.
- **Software dimming** for displays without hardware brightness control.

## Resource use

The mod runs in its own Windhawk process and doesn't inject into other
programs. Between key presses both of its threads are blocked: no timers, no
polling. Monitor handles are released 3 seconds after the last press.

A standalone version with a tray-icon slider is available at
[Universal-Monitor-Brightness-control](https://github.com/PriyanshuGeTRekT/Universal-Monitor-Brightness-control).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- BrighterHotkey: "Ctrl+Alt+Up"
  $name: Brighter shortcut
  $description: >-
    For example Ctrl+Alt+Up. Modifiers: Ctrl, Alt, Shift, Win. Keys: A-Z, 0-9,
    F1-F24, Up, Down, Left, Right, PageUp, PageDown, Home, End, Insert, Delete,
    Space, Plus, Minus. Leave empty to turn it off.
- DimmerHotkey: "Ctrl+Alt+Down"
  $name: Dimmer shortcut
  $description: Same format as the brighter shortcut.
- Step: 5
  $name: Change per press (%)
  $description: How much each press changes the brightness, from 1 to 50.
- Popup: exceptGames
  $name: Show popup
  $description: >-
    Shows each display's new level in the corner. Exclusive full-screen games
    drop out of full screen when a window appears over them, so by default the
    popup is skipped while one is running.
  $options:
  - exceptGames: Except over full-screen games
  - always: Always
  - never: Never
- PopupDuration: 1500
  $name: Popup duration (ms)
- SoftwareDimming: true
  $name: Software dimming for other displays
  $description: >-
    Dim displays that support neither DDC/CI nor WMI (TVs, DisplayLink docks,
    monitors with DDC/CI off) with a click-through dark overlay.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <dwmapi.h>
#include <lowlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <wbemidl.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class PopupMode { ExceptGames, Always, Never };

struct Settings {
    UINT upMods = 0, upVk = 0, downMods = 0, downVk = 0;
    int step = 5;
    PopupMode popup = PopupMode::ExceptGames;
    int popupMs = 1500;
    bool softwareDimming = true;
};

Settings g_settings;  // UI thread only

std::wstring Lower(std::wstring s) {
    for (auto& c : s) {
        c = (wchar_t)towlower(c);
    }
    return s;
}

std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t");
    if (a == std::wstring::npos) {
        return L"";
    }
    size_t b = s.find_last_not_of(L" \t");
    return s.substr(a, b - a + 1);
}

// Parses "Ctrl+Alt+Up". An empty string is valid and means "no shortcut".
bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    *mods = 0;
    *vk = 0;
    std::vector<std::wstring> parts;
    size_t start = 0;
    while (true) {
        size_t plus = text.find(L'+', start);
        std::wstring part = Trim(text.substr(
            start, plus == std::wstring::npos ? std::wstring::npos
                                              : plus - start));
        if (!part.empty()) {
            parts.push_back(Lower(part));
        }
        if (plus == std::wstring::npos) {
            break;
        }
        start = plus + 1;
    }
    if (parts.empty()) {
        return true;
    }

    for (size_t i = 0; i + 1 < parts.size(); i++) {
        const auto& p = parts[i];
        if (p == L"ctrl" || p == L"control") {
            *mods |= MOD_CONTROL;
        } else if (p == L"alt") {
            *mods |= MOD_ALT;
        } else if (p == L"shift") {
            *mods |= MOD_SHIFT;
        } else if (p == L"win" || p == L"windows") {
            *mods |= MOD_WIN;
        } else {
            return false;
        }
    }

    const std::wstring& key = parts.back();
    if (key.size() == 1 && key[0] >= L'a' && key[0] <= L'z') {
        *vk = 'A' + (key[0] - L'a');
    } else if (key.size() == 1 && key[0] >= L'0' && key[0] <= L'9') {
        *vk = '0' + (key[0] - L'0');
    } else if (key.size() >= 2 && key[0] == L'f' && iswdigit(key[1])) {
        int n = _wtoi(key.c_str() + 1);
        if (n >= 1 && n <= 24) {
            *vk = VK_F1 + n - 1;
        }
    } else {
        static const struct {
            const wchar_t* name;
            UINT vk;
        } keys[] = {
            {L"up", VK_UP},          {L"down", VK_DOWN},
            {L"left", VK_LEFT},      {L"right", VK_RIGHT},
            {L"pageup", VK_PRIOR},   {L"pgup", VK_PRIOR},
            {L"pagedown", VK_NEXT},  {L"pgdn", VK_NEXT},
            {L"home", VK_HOME},      {L"end", VK_END},
            {L"insert", VK_INSERT},  {L"ins", VK_INSERT},
            {L"delete", VK_DELETE},  {L"del", VK_DELETE},
            {L"space", VK_SPACE},    {L"plus", VK_OEM_PLUS},
            {L"minus", VK_OEM_MINUS}, {L"numplus", VK_ADD},
            {L"numminus", VK_SUBTRACT}, {L"pause", VK_PAUSE},
            {L"scrolllock", VK_SCROLL},
        };
        for (const auto& k : keys) {
            if (key == k.name) {
                *vk = k.vk;
                break;
            }
        }
    }
    return *vk != 0;
}

void LoadSettings() {
    Settings s;
    for (auto [name, mods, vk] :
         {std::tuple{L"BrighterHotkey", &s.upMods, &s.upVk},
          std::tuple{L"DimmerHotkey", &s.downMods, &s.downVk}}) {
        PCWSTR text = Wh_GetStringSetting(name);
        if (!ParseHotkey(text, mods, vk)) {
            Wh_Log(L"Can't understand the %s setting \"%s\"", name, text);
            *mods = *vk = 0;
        }
        Wh_FreeStringSetting(text);
    }
    s.step = std::clamp(Wh_GetIntSetting(L"Step"), 1, 50);
    PCWSTR popup = Wh_GetStringSetting(L"Popup");
    if (wcscmp(popup, L"always") == 0) {
        s.popup = PopupMode::Always;
    } else if (wcscmp(popup, L"never") == 0) {
        s.popup = PopupMode::Never;
    }
    Wh_FreeStringSetting(popup);
    s.popupMs = std::clamp(Wh_GetIntSetting(L"PopupDuration"), 300, 10000);
    s.softwareDimming = Wh_GetIntSetting(L"SoftwareDimming") != 0;
    g_settings = s;
}

// True while a game runs Direct3D in exclusive full-screen mode, the same
// signal Windows uses to hold back its own notifications. Any window appearing
// over such a game makes it leave full screen.
bool FullscreenGame() {
    QUERY_USER_NOTIFICATION_STATE state;
    return SUCCEEDED(SHQueryUserNotificationState(&state)) &&
           state == QUNS_RUNNING_D3D_FULL_SCREEN;
}

////////////////////////////////////////////////////////////////////////////////
// Displays (worker thread)
//
// DDC/CI and WMI calls block for tens of milliseconds, so they run on a worker
// thread that sleeps on a condition variable between key presses.

constexpr UINT WM_APP_ROWS = WM_APP + 1;
constexpr UINT WM_APP_SETTINGS = WM_APP + 2;
constexpr BYTE kVcpBrightness = 0x10;

enum class Kind { Wmi, Ddc, Soft };

struct Display {
    Kind kind;
    std::wstring name;
    std::wstring device;  // GDI device name, e.g. \\.\DISPLAY2
    RECT rect{};
    int value = 100;
    HANDLE physical = nullptr;  // DDC/CI
    DWORD max = 0;              // DDC/CI
    std::wstring key;           // DDC/CI: device + physical index
};

// What the popup and the UI thread need to know about a display.
struct Row {
    std::wstring name;
    int value;
    bool soft;
    std::wstring device;
    RECT rect;
};

// A monitor that has answered DDC/CI before, with its last known levels. Kept
// across releases so that one missed reply (monitors often ignore a read right
// after a write) can't turn it into a software-dimmed display.
struct Known {
    std::wstring key;
    DWORD max;
    int value;
};

template <typename F>
bool Retry(F f) {
    for (int attempt = 0; attempt < 4; attempt++) {
        if (attempt > 0) {
            Sleep(40);
        }
        if (f()) {
            return true;
        }
    }
    return false;
}

class Worker {
   public:
    void Start(HWND notify) {
        notify_ = notify;
        thread_ = std::thread([this] { Run(); });
    }

    void Nudge(int delta) {
        {
            std::lock_guard lock(mutex_);
            pending_ += delta;
        }
        cv_.notify_one();
    }

    void Stop() {
        {
            std::lock_guard lock(mutex_);
            stop_ = true;
        }
        cv_.notify_one();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

   private:
    void Run() {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        std::unique_lock lock(mutex_);
        while (true) {
            auto ready = [this] { return pending_ != 0 || stop_; };
            if (displays_.empty()) {
                cv_.wait(lock, ready);
            } else if (!cv_.wait_for(lock, std::chrono::seconds(3), ready)) {
                // Idle: give the monitor handles and WMI connection back.
                lock.unlock();
                Release();
                lock.lock();
                continue;
            }
            if (stop_) {
                break;
            }
            int delta = pending_;
            pending_ = 0;
            lock.unlock();

            if (displays_.empty()) {
                Enumerate();
            }
            for (auto& d : displays_) {
                if (d.kind != Kind::Soft) {
                    int v = std::clamp(d.value + delta, 0, 100);
                    if (v != d.value) {
                        SetLevel(d, v);
                    }
                }
            }
            auto* rows = new std::vector<Row>();
            for (const auto& d : displays_) {
                rows->push_back({d.name, d.value, d.kind == Kind::Soft,
                                 d.device, d.rect});
            }
            if (!PostMessage(notify_, WM_APP_ROWS, (WPARAM)delta,
                             (LPARAM)rows)) {
                delete rows;
            }
            lock.lock();
        }
        lock.unlock();
        Release();
        CoUninitialize();
    }

    void Enumerate() {
        struct Target {
            std::wstring gdi, name;
            bool internal;
        };
        std::vector<Target> targets;
        UINT32 numPaths = 0, numModes = 0;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &numPaths,
                                        &numModes) == ERROR_SUCCESS) {
            std::vector<DISPLAYCONFIG_PATH_INFO> paths(numPaths);
            std::vector<DISPLAYCONFIG_MODE_INFO> modes(numModes);
            if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &numPaths,
                                   paths.data(), &numModes, modes.data(),
                                   nullptr) == ERROR_SUCCESS) {
                for (UINT32 i = 0; i < numPaths; i++) {
                    DISPLAYCONFIG_SOURCE_DEVICE_NAME src{};
                    src.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
                    src.header.size = sizeof(src);
                    src.header.adapterId = paths[i].sourceInfo.adapterId;
                    src.header.id = paths[i].sourceInfo.id;
                    DISPLAYCONFIG_TARGET_DEVICE_NAME tgt{};
                    tgt.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
                    tgt.header.size = sizeof(tgt);
                    tgt.header.adapterId = paths[i].targetInfo.adapterId;
                    tgt.header.id = paths[i].targetInfo.id;
                    if (DisplayConfigGetDeviceInfo(&src.header) != 0 ||
                        DisplayConfigGetDeviceInfo(&tgt.header) != 0) {
                        continue;
                    }
                    auto tech = tgt.outputTechnology;
                    bool internal =
                        tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
                        tech ==
                            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
                        tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED ||
                        tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS;
                    targets.push_back({src.viewGdiDeviceName,
                                       tgt.monitorFriendlyDeviceName,
                                       internal});
                }
            }
        }

        std::vector<std::pair<HMONITOR, MONITORINFOEXW>> monitors;
        EnumDisplayMonitors(
            nullptr, nullptr,
            [](HMONITOR hm, HDC, LPRECT, LPARAM lp) -> BOOL {
                MONITORINFOEXW mi{};
                mi.cbSize = sizeof(mi);
                if (GetMonitorInfoW(hm, &mi)) {
                    ((std::vector<std::pair<HMONITOR, MONITORINFOEXW>>*)lp)
                        ->push_back({hm, mi});
                }
                return TRUE;
            },
            (LPARAM)&monitors);
        std::sort(monitors.begin(), monitors.end(), [](auto& a, auto& b) {
            return a.second.rcMonitor.left < b.second.rcMonitor.left;
        });

        std::vector<Display> internal, external;
        for (auto& [hm, mi] : monitors) {
            std::wstring dev = mi.szDevice;
            auto t = std::find_if(targets.begin(), targets.end(),
                                  [&](auto& t) { return t.gdi == dev; });
            if (t != targets.end() && t->internal) {
                internal.push_back(
                    {Kind::Soft, L"Built-in display", dev, mi.rcMonitor});
                continue;
            }
            std::wstring name = t != targets.end() && !t->name.empty()
                                    ? t->name
                                    : L"Display " +
                                          std::to_wstring(external.size() + 1);
            if (!AddDdc(hm, dev, name, mi.rcMonitor, external)) {
                external.push_back({Kind::Soft, name, dev, mi.rcMonitor});
            }
        }

        int wmiValue = 100;
        if (!internal.empty() && ConnectWmi() && GetWmi(&wmiValue)) {
            Display d = internal[0];
            d.kind = Kind::Wmi;
            d.value = wmiValue;
            displays_.push_back(d);
        } else {
            displays_.insert(displays_.end(), internal.begin(), internal.end());
        }
        displays_.insert(displays_.end(), external.begin(), external.end());
        for (const auto& d : displays_) {
            Wh_Log(L"%s: %s, %d%%", d.name.c_str(),
                   d.kind == Kind::Wmi   ? L"WMI"
                   : d.kind == Kind::Ddc ? L"DDC/CI"
                                         : L"software dimming",
                   d.value);
        }
    }

    bool AddDdc(HMONITOR hm,
                const std::wstring& dev,
                const std::wstring& name,
                RECT rect,
                std::vector<Display>& out) {
        DWORD count = 0;
        if (!GetNumberOfPhysicalMonitorsFromHMONITOR(hm, &count) ||
            count == 0) {
            return false;
        }
        std::vector<PHYSICAL_MONITOR> physical(count);
        if (!GetPhysicalMonitorsFromHMONITOR(hm, count, physical.data())) {
            return false;
        }
        size_t before = out.size();
        for (DWORD i = 0; i < count; i++) {
            HANDLE h = physical[i].hPhysicalMonitor;
            std::wstring key = dev + L"#" + std::to_wstring(i);
            DWORD cur = 0, max = 0;
            bool read = Retry([&] {
                return GetVCPFeatureAndVCPFeatureReply(h, kVcpBrightness,
                                                       nullptr, &cur, &max) &&
                       max > 0;
            });
            auto known = std::find_if(known_.begin(), known_.end(),
                                      [&](auto& k) { return k.key == key; });
            int value;
            if (read) {
                value = std::min(100, (int)((cur * 100 + max / 2) / max));
                if (known != known_.end()) {
                    known->max = max;
                    known->value = value;
                } else {
                    known_.push_back({key, max, value});
                }
            } else if (known != known_.end()) {
                max = known->max;
                value = known->value;
            } else {
                DestroyPhysicalMonitor(h);
                continue;
            }
            Display d{Kind::Ddc,
                      count > 1 ? name + L" (" + std::to_wstring(i + 1) + L")"
                                : name,
                      dev, rect, value};
            d.physical = h;
            d.max = max;
            d.key = key;
            out.push_back(d);
        }
        return out.size() > before;
    }

    void SetLevel(Display& d, int value) {
        d.value = value;
        if (d.kind == Kind::Wmi) {
            SetWmi(value);
        } else if (d.kind == Kind::Ddc) {
            DWORD raw = (value * d.max + 50) / 100;
            if (Retry([&] {
                    return SetVCPFeature(d.physical, kVcpBrightness, raw) !=
                           FALSE;
                })) {
                for (auto& k : known_) {
                    if (k.key == d.key) {
                        k.value = value;
                    }
                }
            }
        }
    }

    // WMI: WmiMonitorBrightness / WmiMonitorBrightnessMethods in root\WMI.

    IWbemClassObject* First(const wchar_t* query) {
        IEnumWbemClassObject* e = nullptr;
        BSTR lang = SysAllocString(L"WQL");
        BSTR q = SysAllocString(query);
        HRESULT hr = wmi_->ExecQuery(
            lang, q, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            nullptr, &e);
        SysFreeString(lang);
        SysFreeString(q);
        if (FAILED(hr)) {
            return nullptr;
        }
        IWbemClassObject* obj = nullptr;
        ULONG n = 0;
        e->Next(WBEM_INFINITE, 1, &obj, &n);
        e->Release();
        return n == 1 ? obj : nullptr;
    }

    bool ConnectWmi() {
        IWbemLocator* locator = nullptr;
        if (FAILED(CoCreateInstance(CLSID_WbemLocator, nullptr,
                                    CLSCTX_INPROC_SERVER, IID_IWbemLocator,
                                    (void**)&locator))) {
            return false;
        }
        BSTR ns = SysAllocString(L"ROOT\\WMI");
        HRESULT hr = locator->ConnectServer(ns, nullptr, nullptr, nullptr, 0,
                                            nullptr, nullptr, &wmi_);
        SysFreeString(ns);
        locator->Release();
        if (FAILED(hr)) {
            return false;
        }
        CoSetProxyBlanket(wmi_, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                          RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
                          nullptr, EOAC_NONE);

        IWbemClassObject* inst = First(
            L"SELECT __PATH FROM WmiMonitorBrightnessMethods WHERE "
            L"Active=TRUE");
        if (!inst) {
            return false;
        }
        VARIANT path;
        VariantInit(&path);
        if (SUCCEEDED(inst->Get(L"__PATH", 0, &path, nullptr, nullptr)) &&
            path.vt == VT_BSTR) {
            wmiPath_ = SysAllocString(path.bstrVal);
        }
        VariantClear(&path);
        inst->Release();

        IWbemClassObject* cls = nullptr;
        BSTR className = SysAllocString(L"WmiMonitorBrightnessMethods");
        hr = wmi_->GetObject(className, 0, nullptr, &cls, nullptr);
        SysFreeString(className);
        if (SUCCEEDED(hr)) {
            IWbemClassObject* in = nullptr;
            if (SUCCEEDED(cls->GetMethod(L"WmiSetBrightness", 0, &in,
                                         nullptr)) &&
                in) {
                in->SpawnInstance(0, &wmiParams_);
                in->Release();
            }
            cls->Release();
        }
        if (wmiParams_) {
            VARIANT timeout;
            timeout.vt = VT_I4;
            timeout.lVal = 1;
            wmiParams_->Put(L"Timeout", 0, &timeout, 0);
        }
        return wmiPath_ && wmiParams_;
    }

    bool GetWmi(int* value) {
        IWbemClassObject* obj = First(
            L"SELECT CurrentBrightness FROM WmiMonitorBrightness WHERE "
            L"Active=TRUE");
        if (!obj) {
            return false;
        }
        VARIANT v;
        VariantInit(&v);
        bool ok = SUCCEEDED(
            obj->Get(L"CurrentBrightness", 0, &v, nullptr, nullptr));
        if (ok) {
            *value = v.vt == VT_UI1 ? v.bVal : v.vt == VT_I4 ? v.lVal : 100;
        }
        VariantClear(&v);
        obj->Release();
        return ok;
    }

    void SetWmi(int value) {
        if (!wmiParams_) {
            return;
        }
        VARIANT b;
        b.vt = VT_UI1;
        b.bVal = (BYTE)value;
        wmiParams_->Put(L"Brightness", 0, &b, 0);
        BSTR method = SysAllocString(L"WmiSetBrightness");
        wmi_->ExecMethod(wmiPath_, method, 0, nullptr, wmiParams_, nullptr,
                         nullptr);
        SysFreeString(method);
    }

    void Release() {
        for (auto& d : displays_) {
            if (d.physical) {
                DestroyPhysicalMonitor(d.physical);
            }
        }
        displays_.clear();
        if (wmiParams_) {
            wmiParams_->Release();
            wmiParams_ = nullptr;
        }
        if (wmiPath_) {
            SysFreeString(wmiPath_);
            wmiPath_ = nullptr;
        }
        if (wmi_) {
            wmi_->Release();
            wmi_ = nullptr;
        }
    }

    HWND notify_ = nullptr;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    int pending_ = 0;
    bool stop_ = false;

    std::vector<Display> displays_;
    std::vector<Known> known_;
    IWbemServices* wmi_ = nullptr;
    BSTR wmiPath_ = nullptr;
    IWbemClassObject* wmiParams_ = nullptr;
};

Worker g_worker;

////////////////////////////////////////////////////////////////////////////////
// Software dimming (UI thread)

struct Dim {
    std::wstring device;
    HWND hwnd;
    int value;
};

std::vector<Dim> g_dims;
HWND g_popup;

constexpr wchar_t kPopupClass[] = L"WindhawkBrightnessHotkeys_Popup";
constexpr wchar_t kDimClass[] = L"WindhawkBrightnessHotkeys_Dim";
// Overlay opacity at 0% (out of 255), so a screen never goes fully black.
constexpr int kDimMaxAlpha = 220;

int DimValue(const std::wstring& device) {
    for (const auto& d : g_dims) {
        if (d.device == device) {
            return d.value;
        }
    }
    return 100;
}

void SetDim(const std::wstring& device, RECT rc, int value) {
    auto it = std::find_if(g_dims.begin(), g_dims.end(),
                           [&](auto& d) { return d.device == device; });
    if (value >= 100) {
        // Fully bright: no overlay at all, so nothing to composite.
        if (it != g_dims.end()) {
            DestroyWindow(it->hwnd);
            g_dims.erase(it);
        }
        return;
    }
    if (it == g_dims.end()) {
        HWND h = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            kDimClass, L"", WS_POPUP, rc.left, rc.top, rc.right - rc.left,
            rc.bottom - rc.top, nullptr, nullptr, GetModuleHandle(nullptr),
            nullptr);
        if (!h) {
            return;
        }
        // Keep screenshots and screen sharing undimmed.
        SetWindowDisplayAffinity(h, WDA_EXCLUDEFROMCAPTURE);
        g_dims.push_back({device, h, 100});
        it = g_dims.end() - 1;
    }
    it->value = value;
    SetLayeredWindowAttributes(it->hwnd, 0,
                               (BYTE)((100 - value) * kDimMaxAlpha / 100),
                               LWA_ALPHA);
    ShowWindow(it->hwnd, SW_SHOWNOACTIVATE);
    if (IsWindowVisible(g_popup)) {
        SetWindowPos(g_popup, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

void ClearDims() {
    for (const auto& d : g_dims) {
        DestroyWindow(d.hwnd);
    }
    g_dims.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Popup (UI thread)

constexpr int kHotkeyUp = 1;
constexpr int kHotkeyDown = 2;
constexpr UINT_PTR kTimerHide = 1;

std::vector<Row> g_rows;
UINT g_dpi = 96;

int Px(int dip) {
    return MulDiv(dip, g_dpi, 96);
}

constexpr int kWidth = 300, kPad = 16, kRowH = 46, kTop = 12, kBottom = 10;

SIZE PopupSize() {
    int n = std::max<int>(1, (int)g_rows.size());
    return {Px(kWidth), Px(kTop + kRowH * n + kBottom)};
}

DWORD RegDword(const wchar_t* key, const wchar_t* value, DWORD fallback) {
    DWORD data = 0, size = sizeof(data);
    return RegGetValue(HKEY_CURRENT_USER, key, value, RRF_RT_REG_DWORD,
                       nullptr, &data, &size) == ERROR_SUCCESS
               ? data
               : fallback;
}

void PaintPopup(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
    HGDIOBJ oldBmp = SelectObject(mem, bmp);

    bool dark = RegDword(
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                    L"Personalize",
                    L"SystemUsesLightTheme", 1) == 0;
    // AccentPalette holds 8 RGBA swatches, light3 to dark3. Windows 11 uses
    // light2 for controls in dark mode and dark1 in light mode.
    COLORREF accent = dark ? RGB(0x4C, 0xC2, 0xFF) : RGB(0x00, 0x5F, 0xB8);
    BYTE palette[32];
    DWORD size = sizeof(palette);
    if (RegGetValue(HKEY_CURRENT_USER,
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
                    L"Accent",
                    L"AccentPalette", RRF_RT_REG_BINARY, nullptr, palette,
                    &size) == ERROR_SUCCESS &&
        size == sizeof(palette)) {
        int i = dark ? 4 : 16;
        accent = RGB(palette[i], palette[i + 1], palette[i + 2]);
    }
    COLORREF bg = dark ? RGB(0x2C, 0x2C, 0x2C) : RGB(0xF3, 0xF3, 0xF3);
    COLORREF text = dark ? RGB(0xFF, 0xFF, 0xFF) : RGB(0x1A, 0x1A, 0x1A);
    COLORREF sub = dark ? RGB(0xCF, 0xCF, 0xCF) : RGB(0x5F, 0x5F, 0x5F);
    COLORREF track = dark ? RGB(0x5A, 0x5A, 0x5A) : RGB(0xC4, 0xC4, 0xC4);

    HBRUSH bgBrush = CreateSolidBrush(bg);
    FillRect(mem, &rc, bgBrush);
    DeleteObject(bgBrush);

    HFONT small = CreateFont(-Px(12), 0, 0, 0, FW_NORMAL, 0, 0, 0,
                             DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0,
                             L"Segoe UI");
    HFONT big = CreateFont(-Px(14), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
                           DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0,
                           L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(mem, small);
    SetBkMode(mem, TRANSPARENT);

    if (g_rows.empty()) {
        SelectObject(mem, big);
        SetTextColor(mem, sub);
        DrawText(mem, L"Brightness", -1, &rc,
                 DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    HGDIOBJ oldPen = SelectObject(mem, GetStockObject(NULL_PEN));
    for (size_t i = 0; i < g_rows.size(); i++) {
        const Row& r = g_rows[i];
        int top = Px(kTop + kRowH * (int)i);
        RECT name{Px(kPad), top, rc.right - Px(kPad), top + Px(18)};
        std::wstring label = r.soft ? r.name + L" · software" : r.name;
        SelectObject(mem, small);
        SetTextColor(mem, sub);
        DrawText(mem, label.c_str(), -1, &name,
                 DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

        int barL = Px(kPad), barR = rc.right - Px(kPad + 48);
        int barY = top + Px(28), barH = Px(4);
        int fill = barL + (barR - barL) * r.value / 100;
        HBRUSH trackBrush = CreateSolidBrush(track);
        SelectObject(mem, trackBrush);
        RoundRect(mem, barL, barY, barR, barY + barH, barH, barH);
        HBRUSH accentBrush = CreateSolidBrush(accent);
        SelectObject(mem, accentBrush);
        if (fill > barL) {
            RoundRect(mem, barL, barY, std::max(fill, barL + barH), barY + barH,
                      barH, barH);
        }
        SelectObject(mem, GetStockObject(NULL_BRUSH));
        DeleteObject(trackBrush);
        DeleteObject(accentBrush);

        RECT pct{barR + Px(8), top + Px(18), rc.right - Px(kPad),
                 top + Px(40)};
        std::wstring value = std::to_wstring(r.value) + L"%";
        SelectObject(mem, big);
        SetTextColor(mem, text);
        DrawText(mem, value.c_str(), -1, &pct,
                 DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    }
    SelectObject(mem, oldPen);
    SelectObject(mem, oldFont);
    DeleteObject(small);
    DeleteObject(big);

    BitBlt(hdc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
    EndPaint(hwnd, &ps);
}

// Shows the popup in the corner next to the taskbar of the monitor under the
// cursor, like the Windows flyouts, without taking focus.
void PlacePopup(bool show) {
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR hm = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfo(hm, &mi);
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(hm, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    g_dpi = dpiX;

    SIZE size = PopupSize();
    int margin = Px(12);
    RECT wa = mi.rcWork, mr = mi.rcMonitor;
    int x = wa.left > mr.left ? wa.left + margin : wa.right - size.cx - margin;
    int y = wa.top > mr.top ? wa.top + margin : wa.bottom - size.cy - margin;
    SetWindowPos(g_popup, HWND_TOPMOST, x, y, size.cx, size.cy,
                 SWP_NOACTIVATE | (show ? SWP_SHOWWINDOW : 0));
    InvalidateRect(g_popup, nullptr, FALSE);
}

void RegisterHotkeys() {
    UnregisterHotKey(g_popup, kHotkeyUp);
    UnregisterHotKey(g_popup, kHotkeyDown);
    const struct {
        int id;
        UINT mods, vk;
        const wchar_t* name;
    } keys[] = {
        {kHotkeyUp, g_settings.upMods, g_settings.upVk, L"Brighter"},
        {kHotkeyDown, g_settings.downMods, g_settings.downVk, L"Dimmer"},
    };
    for (const auto& k : keys) {
        if (k.vk && !RegisterHotKey(g_popup, k.id, k.mods, k.vk)) {
            Wh_Log(L"%s shortcut is already used by another program (error "
                   L"%u)",
                   k.name, GetLastError());
        }
    }
}

void OnShortcut(int delta) {
    g_worker.Nudge(delta);
    bool show = g_settings.popup == PopupMode::Always ||
                (g_settings.popup == PopupMode::ExceptGames &&
                 !FullscreenGame());
    if (show) {
        if (!IsWindowVisible(g_popup)) {
            PlacePopup(true);
        }
        SetTimer(g_popup, kTimerHide, g_settings.popupMs, nullptr);
    }
}

void OnRows(std::vector<Row>* rows, int delta) {
    bool game = FullscreenGame();
    for (Row& r : *rows) {
        if (!r.soft) {
            continue;
        }
        r.value = DimValue(r.device);
        // An overlay appearing over an exclusive full-screen game would knock
        // it out of full screen, and wouldn't be visible there anyway.
        if (delta != 0 && g_settings.softwareDimming && !game) {
            r.value = std::clamp(r.value + delta, 0, 100);
            SetDim(r.device, r.rect, r.value);
        }
    }
    bool resized = rows->size() != g_rows.size();
    g_rows = std::move(*rows);
    delete rows;
    if (IsWindowVisible(g_popup)) {
        if (resized) {
            PlacePopup(false);
        } else {
            InvalidateRect(g_popup, nullptr, FALSE);
        }
    }
}

LRESULT CALLBACK PopupProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_HOTKEY:
            if (wp == kHotkeyUp) {
                OnShortcut(g_settings.step);
            } else if (wp == kHotkeyDown) {
                OnShortcut(-g_settings.step);
            }
            return 0;
        case WM_APP_ROWS:
            OnRows((std::vector<Row>*)lp, (int)wp);
            return 0;
        case WM_APP_SETTINGS:
            LoadSettings();
            RegisterHotkeys();
            if (!g_settings.softwareDimming) {
                ClearDims();
            }
            return 0;
        case WM_TIMER:
            if (wp == kTimerHide) {
                KillTimer(hwnd, kTimerHide);
                ShowWindow(hwnd, SW_HIDE);
            }
            return 0;
        case WM_DISPLAYCHANGE:
            // Monitor layout changed: drop overlays; they're recreated on the
            // next press with fresh positions.
            ClearDims();
            return 0;
        case WM_NCHITTEST:
            return HTTRANSPARENT;  // clicks go through the popup
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            PaintPopup(hwnd);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT CALLBACK DimProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCHITTEST) {
        return HTTRANSPARENT;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

std::thread g_uiThread;
HANDLE g_uiReady;

void UiThread() {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HINSTANCE inst = GetModuleHandle(nullptr);
    WNDCLASS popupClass{};
    popupClass.lpfnWndProc = PopupProc;
    popupClass.hInstance = inst;
    popupClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    popupClass.lpszClassName = kPopupClass;
    RegisterClass(&popupClass);
    WNDCLASS dimClass{};
    dimClass.lpfnWndProc = DimProc;
    dimClass.hInstance = inst;
    dimClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    dimClass.lpszClassName = kDimClass;
    RegisterClass(&dimClass);

    g_popup = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kPopupClass,
        L"Brightness", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    if (g_popup) {
        DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
        DwmSetWindowAttribute(g_popup, DWMWA_WINDOW_CORNER_PREFERENCE, &corner,
                              sizeof(corner));
        LoadSettings();
        RegisterHotkeys();
        g_worker.Start(g_popup);
    }
    SetEvent(g_uiReady);
    if (!g_popup) {
        return;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    g_worker.Stop();
    ClearDims();
    UnregisterClass(kPopupClass, inst);
    UnregisterClass(kDimClass, inst);
}

////////////////////////////////////////////////////////////////////////////////
// Tool mod callbacks

BOOL WhTool_ModInit() {
    g_uiReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_uiThread = std::thread(UiThread);
    WaitForSingleObject(g_uiReady, INFINITE);
    CloseHandle(g_uiReady);
    if (!g_popup) {
        Wh_Log(L"Couldn't create the popup window");
        g_uiThread.join();
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    PostMessage(g_popup, WM_APP_SETTINGS, 0, 0);
}

void WhTool_ModUninit() {
    PostMessage(g_popup, WM_CLOSE, 0, 0);
    if (g_uiThread.joinable()) {
        g_uiThread.join();
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
