// ==WindhawkMod==
// @id              traffic-monitor-taskbar
// @name            TrafficMonitor for Taskbar
// @description     Network speed, CPU, memory, GPU and daily traffic shown inside the Windows 11 taskbar, next to the tray (TrafficMonitor-style), with a Fluent settings panel and traffic history.
// @description:es-ES  Velocidad de red, CPU, memoria, GPU y tráfico diario dentro de la barra de tareas de Windows 11, junto a la bandeja (estilo TrafficMonitor), con un panel de ajustes Fluent.
// @version         1.0.5
// @author          xSHIMURAx
// @github          https://github.com/xSHIMURAx
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -luser32 -lwindowsapp -lpdh -ldwmapi -lshell32 -lgdi32 -lcomdlg32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# TrafficMonitor for Taskbar

Shows live system usage **inside** the Windows 11 taskbar (it is a XAML element of the
taskbar itself, not a separate window), so it moves by itself when tray icons appear or
disappear.

![TrafficMonitor for Taskbar](https://raw.githubusercontent.com/xSHIMURAx/TrafficMonitor-for-Taskbar/main/Screenshot/Traffic%20Monitor.png)

## Settings panel

**Right-click the monitor in the taskbar and choose "Settings"** (or turn on the
*Open the settings panel* switch below, save, and turn it off again).

The panel has the same sections as the original Python *TrafficMonitor* app:

* **Monitor** – shown items (drag the handle on the left of a row to change their order), MEM values, which monitor(s) it is shown on, anchor, network connection.
* **Appearance** – skin and size, spacing and height, font / bold / text effect / MEM
  separator, units and decimals.
* **Colors** – color of every part (label, number, unit), color alerts and color profiles
  (with automatic switching when Windows changes between dark and light theme).
* **General** – update interval, panel language, acrylic background of the panel, export / import settings.

Changes are a live preview; press **Save** to keep them. The panel keeps its settings in the
mod's own storage, so Windhawk's own settings page only has the switch to open the panel.

### Import / export
*Export settings* writes the same `TrafficMonitor_ajustes.json` file as the Python app, and
*Import settings* reads it, so you can bring your configuration (colors, profiles, units,
sizes…) from the original app. Options that only make sense for a floating window (drag,
hide on full screen, start with Windows) are ignored: the monitor is part of
the taskbar here.

## Features
* Download / upload speed (bytes or bits, auto / KB / MB units, 0-2 decimals, or automatic).
* CPU usage, memory (percent, used, available, or several at once), GPU usage and today's
  traffic.
* Two skins: **Classic** (colored text labels) and **Modern** (vector icons).
* Text effect: none, shadow or outline.
* Alerts: a value turns a custom color when it goes above a limit.
* Tooltip on hover: traffic today / last 7 days / this month, CPU, memory, CPU frequency, GPU.
* Traffic history (list and calendar views, like the original app): **double-click the monitor** or
  right-click it and choose *Traffic history*. Daily traffic is kept for up to ten years; the panel's
  *History* tab can import the original app's `traffic_history.json` and export it again.
* Choose the network connection ("auto" picks the busiest real adapter and ignores
  VPNs, virtual switches and filter drivers, which would count the same bytes twice).

## Notes
* Windows 11 only (it injects into the XAML tree of the taskbar). It uses internal
  `taskbar.dll` symbols, so a big Windows update can break it until the mod is updated.
* Choose where it is shown: first monitor, second monitor or both (Monitor tab, *Show on*). On a
  second taskbar that has no system tray the monitor goes to the left edge of that taskbar.
* GPU usage is read through Windows performance counters (PDH) from inside explorer; no
  external process is started.
* If the mod does not start, open Windhawk's log: usually the symbol download for
  `taskbar.dll` failed. Exit Windhawk from the tray, open it again and wait until
  "Loading symbols" finishes.

## Related mods
Other Windhawk mods that show system information in the taskbar, in case one of them fits you
better: **Taskbar System Info** (`taskbar-system-info`), **Taskbar Clock Customization**
(`taskbar-clock-customization`), **Taskbar Network Speed Indicator** (`net-speed-taskbar`) and
**Taskbar Live HUD** (`taskbar-live-hud-radar`). This mod is a port of the TrafficMonitor app:
its focus is network traffic, with daily history, import / export of the original app's files,
color alerts and profiles, and two skins.

## Credits
The technique used to find the taskbar's XAML tree and insert an element next to the
system tray, and the look of the settings window (XAML Islands), are based on
**Taskbar Fluent Media Player** by Salyts (MIT license).
The measurements, formats, settings and look follow the original Python *TrafficMonitor* script.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- openPanel: false
  $name: Open the settings panel
  $description: Turn this on and save to open the settings panel (turn it off afterwards). You can also right-click the monitor in the taskbar and choose "Settings".
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <unknwn.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include <windows.h>
#include <commdlg.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <propsys.h>
#include <propkey.h>
#include <pdh.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <windhawk_utils.h>

// Constants from pdhmsg.h (not included by pdh.h)
#ifndef PDH_MORE_DATA
#define PDH_MORE_DATA ((PDH_STATUS)0x800007D2L)
#endif
#ifndef PDH_CSTATUS_VALID_DATA
#define PDH_CSTATUS_VALID_DATA ((DWORD)0x00000000L)
#endif
#ifndef PDH_CSTATUS_NEW_DATA
#define PDH_CSTATUS_NEW_DATA ((DWORD)0x00000001L)
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
namespace Shapes = winrt::Windows::UI::Xaml::Shapes;
using winrt::Windows::UI::Color;
using winrt::Windows::UI::Text::FontWeights;

// ===========================================================================
// Settings model
//
// Every setting is a string in one flat map (`SMap`). The settings panel edits
// that map; `BuildSettings` turns it into the `Settings` struct the taskbar
// element is built from. The map is kept in the mod's own storage (Save in the
// panel), so Windhawk's own settings page only carries the "open panel" switch.
// ===========================================================================

enum Item { I_DOWN = 0, I_UP, I_CPU, I_MEM, I_GPU, I_TOTAL, I_COUNT };

static const wchar_t* const kItemKey[I_COUNT] = {L"down", L"up", L"cpu", L"mem", L"gpu", L"total"};
static const wchar_t* const kShowKey[I_COUNT] = {L"showDown", L"showUp",  L"showCpu",
                                                  L"showMem",  L"showGpu", L"showTotal"};
static const wchar_t* const kRoleKey[3] = {L"label", L"num", L"unit"};
// Order the original app lists the parts in (its Colors tab and alerts).
static const int kPanelOrder[I_COUNT] = {I_DOWN, I_UP, I_CPU, I_GPU, I_MEM, I_TOTAL};
// Order of the items in the taskbar when the user has not dragged them (it differs per skin).
static const int kDefaultOrderClassic[I_COUNT] = {I_DOWN, I_UP, I_CPU, I_MEM, I_GPU, I_TOTAL};
static const int kDefaultOrderModern[I_COUNT] = {I_UP, I_DOWN, I_CPU, I_GPU, I_MEM, I_TOTAL};
static const double kAlertLimitDefault[I_COUNT] = {10, 5, 80, 85, 90, 5};

struct Alert {
    bool on = false;
    double limit = 0;
    std::wstring color;
};

struct Settings {
    std::wstring position = L"tray_left";
    std::wstring monitor = L"primary";  // primary | secondary | both
    std::wstring skin = L"classic";
    bool show[I_COUNT] = {true, true, false, true, false, false};
    int order[I_COUNT] = {I_DOWN, I_UP, I_CPU, I_MEM, I_GPU, I_TOTAL};  // left to right
    bool memField[3] = {false, true, true};  // percent, used, available
    std::wstring memSep = L"\u2022";
    std::wstring todayLabel = L"TODAY";
    int intervalMs = 1000;
    std::wstring iface = L"auto";
    bool speedBits = false;
    std::wstring speedUnit = L"auto";
    int decimals = -1;  // -1 = automatic
    bool hideUnits = false;
    std::wstring fontFamily;
    double fontSize = 13;
    bool bold = true;
    std::wstring effect = L"none";  // none | shadow | outline
    std::wstring effectColor = L"#000000";
    int itemGap = 5;
    int marginLeft = 0;
    int marginRight = 8;
    int offsetY = 1;
    std::wstring color[I_COUNT][3];  // label, number, unit ("" = the skin's own color)
    Alert alert[I_COUNT];
};

using SMap = std::map<std::wstring, std::wstring>;

static const double kBaseFontPx = 13.0;  // 100 % size

static std::wstring MGet(const SMap& m, const std::wstring& k) {
    auto it = m.find(k);
    return it == m.end() ? std::wstring() : it->second;
}
static bool MBool(const SMap& m, const std::wstring& k) { return MGet(m, k) == L"1"; }
static long MInt(const SMap& m, const std::wstring& k, long def) {
    std::wstring s = MGet(m, k);
    if (s.empty()) return def;
    wchar_t* end = nullptr;
    long v = wcstol(s.c_str(), &end, 10);
    return end == s.c_str() ? def : v;
}
static double MDbl(const SMap& m, const std::wstring& k, double def) {
    std::wstring s = MGet(m, k);
    if (s.empty()) return def;
    for (auto& ch : s)
        if (ch == L',') ch = L'.';
    wchar_t* end = nullptr;
    double v = wcstod(s.c_str(), &end);
    return end == s.c_str() ? def : v;
}

static std::wstring KCol(int item, int role) {
    return std::wstring(L"col.") + kItemKey[item] + L"." + kRoleKey[role];
}
static std::wstring KAlert(int item, const wchar_t* field) {
    return std::wstring(L"alert.") + kItemKey[item] + L"." + field;
}

static bool IsHex6(const std::wstring& s) {
    if (s.size() != 7 || s[0] != L'#') return false;
    for (size_t i = 1; i < 7; i++)
        if (!iswxdigit(s[i])) return false;
    return true;
}
static std::wstring LowerStr(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}
static std::wstring NormHex(const std::wstring& s) { return IsHex6(s) ? LowerStr(s) : std::wstring(); }

static const SMap& Defaults() {
    static const SMap d = [] {
        SMap m;
        m[L"monitor"] = L"primary";  // primary | secondary | both
        m[L"position"] = L"tray_left";  // tray_left | tray_before_clock | tray_after_clock | taskbar_left_edge
        m[L"skin"] = L"classic";
        m[L"order"] = L"";  // item keys separated by commas; empty = the skin's own order
        m[L"panelAcrylic"] = L"1";  // acrylic background of the settings panel (the panel only)
        m[L"panelAlpha"] = L"50";   // tint opacity of that background, 20-100 %
        for (int i = 0; i < I_COUNT; i++) m[kShowKey[i]] = (i == I_DOWN || i == I_UP || i == I_MEM) ? L"1" : L"0";
        m[L"memPercent"] = L"0";
        m[L"memUsed"] = L"1";
        m[L"memAvail"] = L"1";
        m[L"memSep"] = L"\u2022";
        m[L"todayLabel"] = L"TODAY";
        m[L"interval"] = L"1000";
        m[L"iface"] = L"auto";
        m[L"speedBits"] = L"0";
        m[L"speedUnit"] = L"auto";
        m[L"decimals"] = L"-1";
        m[L"hideUnits"] = L"0";
        m[L"font"] = L"";
        m[L"bold"] = L"1";
        m[L"effect"] = L"none";
        m[L"effectColor"] = L"#000000";
        m[L"sizeClassic"] = L"100";
        m[L"sizeModern"] = L"95";
        m[L"gap"] = L"5";
        m[L"yoff"] = L"1";
        m[L"trayGap"] = L"8";
        for (int i = 0; i < I_COUNT; i++) {
            for (int r = 0; r < 3; r++) m[KCol(i, r)] = L"";
            m[KAlert(i, L"on")] = L"0";
            m[KAlert(i, L"limit")] = std::to_wstring((long)kAlertLimitDefault[i]);
            m[KAlert(i, L"color")] = L"#ff3b30";
        }
        m[L"profileCount"] = L"0";
        m[L"profileAuto"] = L"0";
        m[L"profileDark"] = L"";
        m[L"profileLight"] = L"";
        m[L"lang"] = L"auto";
        return m;
    }();
    return d;
}

// The order the items are shown in: the saved "order" list (item keys separated by commas), and
// any item missing from it after those, in the skin's own order. Empty or invalid = the skin's order.
static std::vector<int> EffectiveOrder(const SMap& m) {
    const bool modern = MGet(m, L"skin") == L"modern";
    std::vector<int> out;
    bool used[I_COUNT] = {};
    const std::wstring list = MGet(m, L"order");
    for (size_t pos = 0; pos <= list.size();) {
        size_t end = list.find(L',', pos);
        if (end == std::wstring::npos) end = list.size();
        const std::wstring key = list.substr(pos, end - pos);
        pos = end + 1;
        for (int i = 0; i < I_COUNT; i++) {
            if (!used[i] && key == kItemKey[i]) {
                used[i] = true;
                out.push_back(i);
            }
        }
    }
    for (int item : modern ? kDefaultOrderModern : kDefaultOrderClassic)
        if (!used[item]) out.push_back(item);
    return out;
}

static Settings BuildSettings(const SMap& m) {
    Settings c;
    c.monitor = MGet(m, L"monitor");
    if (c.monitor != L"secondary" && c.monitor != L"both") c.monitor = L"primary";
    c.position = MGet(m, L"position");
    if (c.position != L"tray_before_clock" && c.position != L"tray_after_clock" &&
        c.position != L"taskbar_left_edge")
        c.position = L"tray_left";
    c.skin = MGet(m, L"skin") == L"modern" ? L"modern" : L"classic";
    {
        const std::vector<int> o = EffectiveOrder(m);
        for (int i = 0; i < I_COUNT; i++) c.order[i] = o[i];
    }
    for (int i = 0; i < I_COUNT; i++) c.show[i] = MBool(m, kShowKey[i]);
    c.memField[0] = MBool(m, L"memPercent");
    c.memField[1] = MBool(m, L"memUsed");
    c.memField[2] = MBool(m, L"memAvail");
    if (!c.memField[0] && !c.memField[1] && !c.memField[2]) c.memField[0] = true;
    c.memSep = MGet(m, L"memSep");
    c.todayLabel = MGet(m, L"todayLabel");
    c.intervalMs = (int)std::clamp(MInt(m, L"interval", 1000), 250L, 10000L);
    c.iface = MGet(m, L"iface");
    if (c.iface.empty()) c.iface = L"auto";
    c.speedBits = MBool(m, L"speedBits");
    c.speedUnit = MGet(m, L"speedUnit");
    if (c.speedUnit != L"k" && c.speedUnit != L"m") c.speedUnit = L"auto";
    long d = MInt(m, L"decimals", -1);
    c.decimals = (d >= 0 && d <= 2) ? (int)d : -1;
    c.hideUnits = MBool(m, L"hideUnits");
    c.fontFamily = MGet(m, L"font");
    c.bold = MBool(m, L"bold");
    c.effect = MGet(m, L"effect");
    if (c.effect != L"shadow" && c.effect != L"outline") c.effect = L"none";
    c.effectColor = NormHex(MGet(m, L"effectColor"));
    if (c.effectColor.empty()) c.effectColor = L"#000000";
    const bool modern = c.skin == L"modern";
    long pct = std::clamp(MInt(m, modern ? L"sizeModern" : L"sizeClassic", modern ? 95 : 100), 50L, 200L);
    c.fontSize = std::max(8.0, std::round(kBaseFontPx * (double)pct / 100.0 * 4.0) / 4.0);
    c.itemGap = (int)std::clamp(MInt(m, L"gap", 5), -10L, 80L);
    c.marginLeft = 0;
    c.marginRight = (int)std::clamp(MInt(m, L"trayGap", 8), 0L, 80L);
    c.offsetY = (int)std::clamp(MInt(m, L"yoff", 1), -30L, 30L);
    for (int i = 0; i < I_COUNT; i++) {
        for (int r = 0; r < 3; r++) c.color[i][r] = NormHex(MGet(m, KCol(i, r)));
        c.alert[i].on = MBool(m, KAlert(i, L"on"));
        c.alert[i].limit = std::clamp(MDbl(m, KAlert(i, L"limit"), kAlertLimitDefault[i]), 0.0, 1e6);
        c.alert[i].color = NormHex(MGet(m, KAlert(i, L"color")));
        if (c.alert[i].color.empty()) c.alert[i].color = L"#ff3b30";
    }
    return c;
}

// ---- Panel language (the original app is in Spanish) ----
static bool g_uiEs = true;
static const wchar_t* TR(const wchar_t* es, const wchar_t* en) { return g_uiEs ? es : en; }
static bool ResolveUiEs(const SMap& m) {
    std::wstring lang = MGet(m, L"lang");
    if (lang == L"es") return true;
    if (lang == L"en") return false;
    return (GetUserDefaultUILanguage() & 0xFF) == 0x0A;  // LANG_SPANISH
}

// ---- Live values and saved values ----
static std::mutex g_cfgMtx;
static SMap g_vals;   // what the taskbar shows now (the panel's live preview included)
static SMap g_saved;  // what is in the mod's storage

static SMap SnapshotVals() {
    std::lock_guard<std::mutex> lk(g_cfgMtx);
    return g_vals;
}
static std::wstring ValGet(const std::wstring& k) {
    std::lock_guard<std::mutex> lk(g_cfgMtx);
    return MGet(g_vals, k);
}
static void ValSet(const std::wstring& k, const std::wstring& v) {
    std::lock_guard<std::mutex> lk(g_cfgMtx);
    g_vals[k] = v;
}

// ---- Storage ----
// "TM1;" then "key=value;" per setting, in printable characters only: portable
// Windhawk keeps mod storage in an INI file, which would turn line breaks into
// spaces and trim a value's outer spaces and quotes.
static constexpr const wchar_t* kStoreName = L"SavedSettingsV1";  // + "Chunks", + chunk index
static constexpr size_t kStoreChunk = 8000;
static constexpr const wchar_t* kStoreTag = L"TM1;";

static std::wstring EscapeSaved(const std::wstring& v) {
    std::wstring out;
    for (wchar_t c : v) {
        switch (c) {
            case L'\\': out += L"\\\\"; break;
            case L';': out += L"\\s"; break;
            case L'=': out += L"\\e"; break;
            case L' ': out += L"\\_"; break;
            case L'"': out += L"\\q"; break;
            default:
                if (c < 0x20) {
                    static const wchar_t* hex = L"0123456789abcdef";
                    out += L"\\x";
                    for (int sh = 12; sh >= 0; sh -= 4) out += hex[((unsigned)c >> sh) & 0xF];
                } else {
                    out += c;
                }
        }
    }
    return out;
}
static std::wstring UnescapeSaved(const std::wstring& v) {
    std::wstring out;
    for (size_t i = 0; i < v.size(); i++) {
        if (v[i] != L'\\' || i + 1 >= v.size()) {
            out += v[i];
            continue;
        }
        const wchar_t n = v[++i];
        switch (n) {
            case L's': out += L';'; break;
            case L'e': out += L'='; break;
            case L'_': out += L' '; break;
            case L'q': out += L'"'; break;
            case L'x':
                if (i + 4 < v.size()) {
                    out += (wchar_t)wcstoul(v.substr(i + 1, 4).c_str(), nullptr, 16);
                    i += 4;
                }
                break;
            default: out += n;  // \\ and anything unknown
        }
    }
    return out;
}
static std::wstring SerializeMap(const SMap& m) {
    std::wstring text = kStoreTag;
    for (const auto& kv : m) text += EscapeSaved(kv.first) + L"=" + EscapeSaved(kv.second) + L";";
    return text;
}
static SMap ParseMap(const std::wstring& text) {
    SMap m;
    const size_t tag = wcslen(kStoreTag);
    if (text.compare(0, tag, kStoreTag) != 0) return m;
    for (size_t pos = tag; pos < text.size();) {
        size_t end = text.find(L';', pos);
        if (end == std::wstring::npos) end = text.size();
        const std::wstring entry = text.substr(pos, end - pos);
        pos = end + 1;
        const size_t eq = entry.find(L'=');
        if (eq != std::wstring::npos && eq > 0)
            m[UnescapeSaved(entry.substr(0, eq))] = UnescapeSaved(entry.substr(eq + 1));
    }
    return m;
}

static std::wstring ReadStoredText(const std::wstring& prefix) {
    std::wstring text;
    const int chunks = (int)std::clamp(Wh_GetIntValue((prefix + L"Chunks").c_str(), 0), 0, 1000);
    for (int i = 0; i < chunks; i++) {
        std::vector<wchar_t> buf(kStoreChunk + 2, L'\0');
        Wh_GetStringValue((prefix + std::to_wstring(i)).c_str(), buf.data(), (int)buf.size());
        text += buf.data();
    }
    return text;
}
static bool WriteStoredText(const std::wstring& prefix, const std::wstring& text) {
    const std::wstring countName = prefix + L"Chunks";
    const int oldChunks = (int)std::clamp(Wh_GetIntValue(countName.c_str(), 0), 0, 1000);
    int chunks = 0;
    for (size_t at = 0; at < text.size(); at += kStoreChunk, chunks++)
        if (!Wh_SetStringValue((prefix + std::to_wstring(chunks)).c_str(), text.substr(at, kStoreChunk).c_str()))
            return false;
    if (!Wh_SetIntValue(countName.c_str(), chunks)) return false;
    for (int i = chunks; i < oldChunks; i++) Wh_DeleteValue((prefix + std::to_wstring(i)).c_str());
    return true;
}

// Defaults first, then whatever was saved.
static SMap MergeOverDefaults(const SMap& saved) {
    SMap m = Defaults();
    for (const auto& kv : saved) m[kv.first] = kv.second;
    return m;
}

static void LoadStore() {
    SMap saved = ParseMap(ReadStoredText(kStoreName));
    std::lock_guard<std::mutex> lk(g_cfgMtx);
    g_vals = MergeOverDefaults(saved);
    g_saved = g_vals;
    g_uiEs = ResolveUiEs(g_vals);
}

// Writes `m` to storage and remembers it as the saved state.
static bool SaveStore(const SMap& m) {
    if (!WriteStoredText(kStoreName, SerializeMap(m))) return false;
    std::lock_guard<std::mutex> lk(g_cfgMtx);
    g_saved = m;
    return true;
}

// ---- Color profiles ----
// A profile keeps the part colors, the alert colors and the text effect (like the
// original app). Stored as "profile.<n>" = a serialized map, plus "profileCount".
struct Profile {
    std::wstring name;
    SMap data;
};
static constexpr size_t kProfileMax = 30, kProfileNameMax = 30;

static std::vector<Profile> LoadProfiles(const SMap& m) {
    std::vector<Profile> out;
    long n = std::clamp(MInt(m, L"profileCount", 0), 0L, (long)kProfileMax);
    for (long i = 0; i < n; i++) {
        SMap d = ParseMap(MGet(m, L"profile." + std::to_wstring(i)));
        Profile p;
        p.name = MGet(d, L"name");
        if (p.name.empty()) continue;
        d.erase(L"name");
        p.data = std::move(d);
        out.push_back(std::move(p));
    }
    return out;
}
static void StoreProfiles(SMap& m, const std::vector<Profile>& ps) {
    for (auto it = m.begin(); it != m.end();) {
        if (it->first.compare(0, 8, L"profile.") == 0) it = m.erase(it);
        else ++it;
    }
    m[L"profileCount"] = std::to_wstring(ps.size());
    for (size_t i = 0; i < ps.size(); i++) {
        SMap d = ps[i].data;
        d[L"name"] = ps[i].name;
        m[L"profile." + std::to_wstring(i)] = SerializeMap(d);
    }
}
static SMap CurrentProfileData(const SMap& m) {
    SMap d;
    for (int i = 0; i < I_COUNT; i++) {
        for (int r = 0; r < 3; r++) d[KCol(i, r)] = NormHex(MGet(m, KCol(i, r)));
        d[KAlert(i, L"color")] = MGet(m, KAlert(i, L"color"));
    }
    d[L"effect"] = MGet(m, L"effect");
    d[L"effectColor"] = MGet(m, L"effectColor");
    return d;
}
static void ApplyProfileData(SMap& m, const SMap& d) {
    for (int i = 0; i < I_COUNT; i++) {
        for (int r = 0; r < 3; r++) m[KCol(i, r)] = NormHex(MGet(d, KCol(i, r)));
        auto it = d.find(KAlert(i, L"color"));
        if (it != d.end() && IsHex6(it->second)) m[KAlert(i, L"color")] = LowerStr(it->second);
    }
    auto e = d.find(L"effect");
    if (e != d.end()) m[L"effect"] = e->second;
    auto c = d.find(L"effectColor");
    if (c != d.end()) m[L"effectColor"] = c->second;
}

// ===========================================================================
// JSON (the original app's TrafficMonitor_ajustes.json)
// ===========================================================================

struct JV {
    enum Type { Null, Bool, Num, Str, Arr, Obj } t = Null;
    bool b = false;
    double n = 0;
    std::wstring s;
    std::vector<JV> a;           // Arr
    std::vector<std::wstring> k;  // Obj keys, parallel to `a`
    const JV* Get(const std::wstring& key) const {
        if (t != Obj) return nullptr;
        for (size_t i = 0; i < k.size(); i++)
            if (k[i] == key) return &a[i];
        return nullptr;
    }
};

struct JsonParser {
    const std::wstring& s;
    size_t i = 0;
    bool ok = true;
    int depth = 0;
    explicit JsonParser(const std::wstring& str) : s(str) {}

    void Ws() {
        while (i < s.size() && (s[i] == L' ' || s[i] == L'\t' || s[i] == L'\n' || s[i] == L'\r')) i++;
    }
    bool Lit(const wchar_t* w) {
        size_t n = wcslen(w);
        if (s.compare(i, n, w) == 0) {
            i += n;
            return true;
        }
        return false;
    }
    std::wstring Str() {
        std::wstring out;
        i++;  // opening quote
        while (i < s.size() && s[i] != L'"') {
            wchar_t c = s[i++];
            if (c != L'\\') {
                out += c;
                continue;
            }
            if (i >= s.size()) break;
            wchar_t e = s[i++];
            switch (e) {
                case L'n': out += L'\n'; break;
                case L't': out += L'\t'; break;
                case L'r': out += L'\r'; break;
                case L'b': out += L'\b'; break;
                case L'f': out += L'\f'; break;
                case L'u':
                    if (i + 4 <= s.size()) {
                        out += (wchar_t)wcstoul(s.substr(i, 4).c_str(), nullptr, 16);
                        i += 4;
                    } else {
                        ok = false;
                    }
                    break;
                default: out += e;  // \" \\ \/
            }
        }
        if (i >= s.size()) ok = false;
        else i++;  // closing quote
        return out;
    }
    JV Value() {
        JV v;
        Ws();
        if (i >= s.size() || depth > 64) {
            ok = false;
            return v;
        }
        wchar_t c = s[i];
        if (c == L'{') {
            v.t = JV::Obj;
            i++;
            depth++;
            Ws();
            if (i < s.size() && s[i] == L'}') {
                i++;
            } else {
                while (ok) {
                    Ws();
                    if (i >= s.size() || s[i] != L'"') {
                        ok = false;
                        break;
                    }
                    std::wstring key = Str();
                    Ws();
                    if (i >= s.size() || s[i] != L':') {
                        ok = false;
                        break;
                    }
                    i++;
                    JV val = Value();
                    v.k.push_back(std::move(key));
                    v.a.push_back(std::move(val));
                    Ws();
                    if (i < s.size() && s[i] == L',') {
                        i++;
                        continue;
                    }
                    if (i < s.size() && s[i] == L'}') {
                        i++;
                        break;
                    }
                    ok = false;
                }
            }
            depth--;
        } else if (c == L'[') {
            v.t = JV::Arr;
            i++;
            depth++;
            Ws();
            if (i < s.size() && s[i] == L']') {
                i++;
            } else {
                while (ok) {
                    v.a.push_back(Value());
                    Ws();
                    if (i < s.size() && s[i] == L',') {
                        i++;
                        continue;
                    }
                    if (i < s.size() && s[i] == L']') {
                        i++;
                        break;
                    }
                    ok = false;
                }
            }
            depth--;
        } else if (c == L'"') {
            v.t = JV::Str;
            v.s = Str();
        } else if (Lit(L"true")) {
            v.t = JV::Bool;
            v.b = true;
        } else if (Lit(L"false")) {
            v.t = JV::Bool;
        } else if (Lit(L"null")) {
        } else {
            wchar_t* end = nullptr;
            double d = wcstod(s.c_str() + i, &end);
            if (end == s.c_str() + i) {
                ok = false;
            } else {
                v.t = JV::Num;
                v.n = d;
                i = (size_t)(end - s.c_str());
            }
        }
        return v;
    }
};

static bool ParseJson(const std::wstring& text, JV& out) {
    JsonParser p(text);
    out = p.Value();
    p.Ws();
    return p.ok && p.i == text.size();
}

static bool JBool(const JV* v, bool def) { return v && v->t == JV::Bool ? v->b : def; }
static double JNum(const JV* v, double def) { return v && v->t == JV::Num ? v->n : def; }
static std::wstring JStr(const JV* v, const std::wstring& def) { return v && v->t == JV::Str ? v->s : def; }

static std::wstring B01(bool b) { return b ? L"1" : L"0"; }
static std::wstring NumStr(double v) {
    char b[40];
    snprintf(b, sizeof b, "%.6g", v);
    return std::wstring(b, b + strlen(b));
}

// What the original app's file may hold, as it understands it.
static const wchar_t* const kPyKeys[] = {L"cpu", L"mem", L"mem_fields", L"gpu", L"total", L"skin", L"size",
                                          L"size_classic", L"colors", L"anchor", L"monitor", L"interval", L"alerts",
                                          L"speed_bits", L"speed_unit", L"decimals", L"hide_units", L"font",
                                          L"bold", L"effect", L"effect_color", L"sep", L"gap", L"y_off",
                                          L"tray_gap", L"tray_gaps", L"profiles", L"profile_auto",
                                          L"profile_dark", L"profile_light"};

// Applies an exported file to `m`. Returns false if the file isn't one of the app's.
static bool ApplyPythonJson(const JV& rootIn, SMap& m) {
    const JV* root = &rootIn;
    if (const JV* st = root->Get(L"settings"))
        if (st->t == JV::Obj) root = st;
    if (root->t != JV::Obj) return false;
    bool any = false;
    for (auto* k : kPyKeys)
        if (root->Get(k)) any = true;
    if (!any) return false;

    auto clampSet = [&](const wchar_t* key, const JV* v, double lo, double hi) {
        if (v && v->t == JV::Num) m[key] = std::to_wstring((long)std::clamp(std::round(v->n), lo, hi));
    };
    auto boolSet = [&](const wchar_t* key, const JV* v) {
        if (v && v->t == JV::Bool) m[key] = B01(v->b);
    };

    boolSet(L"showCpu", root->Get(L"cpu"));
    boolSet(L"showMem", root->Get(L"mem"));
    boolSet(L"showGpu", root->Get(L"gpu"));
    boolSet(L"showTotal", root->Get(L"total"));
    if (const JV* mf = root->Get(L"mem_fields")) {
        if (mf->t == JV::Arr) {
            bool p = false, u = false, a = false;
            for (const JV& e : mf->a) {
                if (e.t != JV::Str) continue;
                if (e.s == L"percent") p = true;
                if (e.s == L"used") u = true;
                if (e.s == L"avail") a = true;
            }
            if (p || u || a) {
                m[L"memPercent"] = B01(p);
                m[L"memUsed"] = B01(u);
                m[L"memAvail"] = B01(a);
            }
        }
    }
    std::wstring skin = JStr(root->Get(L"skin"), L"");
    if (skin == L"classic" || skin == L"modern") m[L"skin"] = skin;
    clampSet(L"sizeModern", root->Get(L"size"), 50, 200);
    clampSet(L"sizeClassic", root->Get(L"size_classic"), 50, 200);
    {
        std::wstring mon = JStr(root->Get(L"monitor"), L"");
        if (mon == L"primary" || mon == L"secondary" || mon == L"both") m[L"monitor"] = mon;
    }
    std::wstring anchor = JStr(root->Get(L"anchor"), L"");
    if (anchor == L"left") m[L"position"] = L"taskbar_left_edge";
    else if (anchor == L"tray" && MGet(m, L"position").compare(0, 5, L"tray_") != 0) m[L"position"] = L"tray_left";
    if (const JV* iv = root->Get(L"interval")) {
        if (iv->t == JV::Num) {
            double v = iv->n;
            m[L"interval"] = v <= 750 ? L"500" : v <= 1500 ? L"1000" : L"2000";
        }
    }
    boolSet(L"speedBits", root->Get(L"speed_bits"));
    std::wstring unit = JStr(root->Get(L"speed_unit"), L"");
    if (unit == L"auto" || unit == L"k" || unit == L"m") m[L"speedUnit"] = unit;
    clampSet(L"decimals", root->Get(L"decimals"), -1, 2);
    boolSet(L"hideUnits", root->Get(L"hide_units"));
    if (const JV* f = root->Get(L"font"))
        if (f->t == JV::Str) m[L"font"] = f->s;
    boolSet(L"bold", root->Get(L"bold"));
    std::wstring eff = JStr(root->Get(L"effect"), L"");
    if (eff == L"none" || eff == L"shadow" || eff == L"outline") m[L"effect"] = eff;
    if (IsHex6(JStr(root->Get(L"effect_color"), L""))) m[L"effectColor"] = LowerStr(JStr(root->Get(L"effect_color"), L""));
    if (const JV* sp = root->Get(L"sep"))
        if (sp->t == JV::Str) m[L"memSep"] = sp->s.substr(0, 3);
    clampSet(L"gap", root->Get(L"gap"), -10, 80);
    clampSet(L"yoff", root->Get(L"y_off"), -30, 30);
    {
        const JV* tg = root->Get(L"tray_gaps");
        const JV* first = tg ? tg->Get(L"0") : nullptr;
        clampSet(L"trayGap", first ? first : root->Get(L"tray_gap"), 0, 80);
    }
    if (const JV* cols = root->Get(L"colors")) {
        if (cols->t == JV::Obj) {
            for (int i = 0; i < I_COUNT; i++) {
                const JV* it = cols->Get(kItemKey[i]);
                for (int r = 0; r < 3; r++) {
                    std::wstring c = JStr(it ? it->Get(kRoleKey[r]) : nullptr, L"");
                    m[KCol(i, r)] = NormHex(c);
                }
            }
        }
    }
    if (const JV* al = root->Get(L"alerts")) {
        if (al->t == JV::Obj) {
            for (int i = 0; i < I_COUNT; i++) {
                const JV* a = al->Get(kItemKey[i]);
                if (!a) continue;
                if (const JV* on = a->Get(L"on")) boolSet(KAlert(i, L"on").c_str(), on);
                if (const JV* lim = a->Get(L"limit"))
                    if (lim->t == JV::Num && lim->n >= 0 && lim->n <= 1e6) m[KAlert(i, L"limit")] = NumStr(lim->n);
                std::wstring c = NormHex(JStr(a->Get(L"color"), L""));
                if (!c.empty()) m[KAlert(i, L"color")] = c;
            }
        }
    }
    if (const JV* pr = root->Get(L"profiles")) {
        if (pr->t == JV::Obj) {
            std::vector<Profile> list;
            for (size_t n = 0; n < pr->k.size() && list.size() < kProfileMax; n++) {
                const JV& p = pr->a[n];
                if (p.t != JV::Obj || pr->k[n].empty()) continue;
                Profile out;
                out.name = pr->k[n].substr(0, kProfileNameMax);
                if (const JV* c = p.Get(L"colors")) {
                    for (int i = 0; i < I_COUNT; i++) {
                        const JV* it = c->Get(kItemKey[i]);
                        for (int r = 0; r < 3; r++) {
                            std::wstring col = NormHex(JStr(it ? it->Get(kRoleKey[r]) : nullptr, L""));
                            out.data[KCol(i, r)] = col;
                        }
                    }
                }
                if (const JV* a = p.Get(L"alerts")) {
                    for (int i = 0; i < I_COUNT; i++) {
                        std::wstring col = NormHex(JStr(a->Get(kItemKey[i]), L""));
                        if (!col.empty()) out.data[KAlert(i, L"color")] = col;
                    }
                }
                std::wstring pe = JStr(p.Get(L"effect"), L"");
                if (pe == L"none" || pe == L"shadow" || pe == L"outline") out.data[L"effect"] = pe;
                std::wstring pc = NormHex(JStr(p.Get(L"effect_color"), L""));
                if (!pc.empty()) out.data[L"effectColor"] = pc;
                list.push_back(std::move(out));
            }
            StoreProfiles(m, list);
        }
    }
    boolSet(L"profileAuto", root->Get(L"profile_auto"));
    if (const JV* d = root->Get(L"profile_dark"))
        if (d->t == JV::Str) m[L"profileDark"] = d->s;
    if (const JV* l = root->Get(L"profile_light"))
        if (l->t == JV::Str) m[L"profileLight"] = l->s;
    return true;
}

static std::wstring JsonEsc(const std::wstring& s) {
    std::wstring o = L"\"";
    for (wchar_t c : s) {
        switch (c) {
            case L'"': o += L"\\\""; break;
            case L'\\': o += L"\\\\"; break;
            case L'\n': o += L"\\n"; break;
            case L'\r': o += L"\\r"; break;
            case L'\t': o += L"\\t"; break;
            default:
                if (c < 0x20) {
                    static const wchar_t* hex = L"0123456789abcdef";
                    o += L"\\u";
                    for (int sh = 12; sh >= 0; sh -= 4) o += hex[((unsigned)c >> sh) & 0xF];
                } else {
                    o += c;
                }
        }
    }
    return o + L"\"";
}

// The file the original app exports (same keys), so it can be imported there too.
static std::wstring BuildPythonJson(const SMap& m, const std::wstring& exportedAt) {
    std::wstring o = L"{\n  \"app\": \"TrafficMonitor\",\n  \"version\": 1,\n  \"exported\": " + JsonEsc(exportedAt) +
                     L",\n  \"settings\": {\n";
    auto line = [&](const std::wstring& k, const std::wstring& v) { o += L"    \"" + k + L"\": " + v + L",\n"; };
    auto jb = [](bool b) { return std::wstring(b ? L"true" : L"false"); };
    line(L"cpu", jb(MBool(m, L"showCpu")));
    line(L"mem", jb(MBool(m, L"showMem")));
    {
        std::wstring arr = L"[";
        const wchar_t* names[3] = {L"percent", L"used", L"avail"};
        const wchar_t* keys[3] = {L"memPercent", L"memUsed", L"memAvail"};
        bool first = true;
        for (int i = 0; i < 3; i++) {
            if (!MBool(m, keys[i])) continue;
            arr += (first ? L"" : L", ") + std::wstring(L"\"") + names[i] + L"\"";
            first = false;
        }
        line(L"mem_fields", arr + L"]");
    }
    line(L"gpu", jb(MBool(m, L"showGpu")));
    line(L"total", jb(MBool(m, L"showTotal")));
    line(L"skin", JsonEsc(MGet(m, L"skin")));
    line(L"size", std::to_wstring(MInt(m, L"sizeModern", 95)));
    line(L"size_classic", std::to_wstring(MInt(m, L"sizeClassic", 100)));
    {
        std::wstring c = L"{";
        bool firstItem = true;
        for (int i = 0; i < I_COUNT; i++) {
            std::wstring inner;
            for (int r = 0; r < 3; r++) {
                std::wstring v = NormHex(MGet(m, KCol(i, r)));
                if (v.empty()) continue;
                inner += (inner.empty() ? L"" : L", ") + std::wstring(L"\"") + kRoleKey[r] + L"\": " + JsonEsc(v);
            }
            if (inner.empty()) continue;
            c += (firstItem ? L"" : L", ") + std::wstring(L"\"") + kItemKey[i] + L"\": {" + inner + L"}";
            firstItem = false;
        }
        line(L"colors", c + L"}");
    }
    line(L"anchor", JsonEsc(MGet(m, L"position") == L"taskbar_left_edge" ? L"left" : L"tray"));
    line(L"monitor", JsonEsc(MGet(m, L"monitor").empty() ? std::wstring(L"primary") : MGet(m, L"monitor")));
    line(L"interval", std::to_wstring(MInt(m, L"interval", 1000)));
    {
        std::wstring a = L"{";
        for (int i = 0; i < I_COUNT; i++) {
            a += (i ? L", " : L"") + std::wstring(L"\"") + kItemKey[i] + L"\": {\"on\": " +
                 jb(MBool(m, KAlert(i, L"on"))) + L", \"limit\": " +
                 NumStr(MDbl(m, KAlert(i, L"limit"), kAlertLimitDefault[i])) + L", \"color\": " +
                 JsonEsc(NormHex(MGet(m, KAlert(i, L"color"))).empty() ? L"#ff3b30" : NormHex(MGet(m, KAlert(i, L"color")))) +
                 L"}";
        }
        line(L"alerts", a + L"}");
    }
    line(L"speed_bits", jb(MBool(m, L"speedBits")));
    line(L"speed_unit", JsonEsc(MGet(m, L"speedUnit")));
    line(L"decimals", std::to_wstring(MInt(m, L"decimals", -1)));
    line(L"hide_units", jb(MBool(m, L"hideUnits")));
    line(L"font", JsonEsc(MGet(m, L"font")));
    line(L"bold", jb(MBool(m, L"bold")));
    line(L"effect", JsonEsc(MGet(m, L"effect")));
    line(L"effect_color", JsonEsc(MGet(m, L"effectColor")));
    line(L"sep", JsonEsc(MGet(m, L"memSep")));
    line(L"gap", std::to_wstring(MInt(m, L"gap", 5)));
    line(L"y_off", std::to_wstring(MInt(m, L"yoff", 1)));
    line(L"tray_gap", std::to_wstring(MInt(m, L"trayGap", 8)));
    line(L"tray_gaps", L"{\"0\": " + std::to_wstring(MInt(m, L"trayGap", 8)) + L"}");
    {
        std::wstring p = L"{";
        std::vector<Profile> ps = LoadProfiles(m);
        for (size_t n = 0; n < ps.size(); n++) {
            const SMap& d = ps[n].data;
            std::wstring cols = L"{";
            bool firstItem = true;
            for (int i = 0; i < I_COUNT; i++) {
                std::wstring inner;
                for (int r = 0; r < 3; r++) {
                    std::wstring v = NormHex(MGet(d, KCol(i, r)));
                    if (v.empty()) continue;
                    inner += (inner.empty() ? L"" : L", ") + std::wstring(L"\"") + kRoleKey[r] + L"\": " + JsonEsc(v);
                }
                if (inner.empty()) continue;
                cols += (firstItem ? L"" : L", ") + std::wstring(L"\"") + kItemKey[i] + L"\": {" + inner + L"}";
                firstItem = false;
            }
            cols += L"}";
            std::wstring als = L"{";
            bool firstAl = true;
            for (int i = 0; i < I_COUNT; i++) {
                std::wstring v = NormHex(MGet(d, KAlert(i, L"color")));
                if (v.empty()) continue;
                als += (firstAl ? L"" : L", ") + std::wstring(L"\"") + kItemKey[i] + L"\": " + JsonEsc(v);
                firstAl = false;
            }
            als += L"}";
            p += (n ? L", " : L"") + JsonEsc(ps[n].name) + L": {\"colors\": " + cols + L", \"alerts\": " + als +
                 L", \"effect\": " + JsonEsc(MGet(d, L"effect").empty() ? L"none" : MGet(d, L"effect")) +
                 L", \"effect_color\": " + JsonEsc(MGet(d, L"effectColor").empty() ? L"#000000" : MGet(d, L"effectColor")) + L"}";
        }
        line(L"profiles", p + L"}");
    }
    line(L"profile_auto", jb(MBool(m, L"profileAuto")));
    line(L"profile_dark", JsonEsc(MGet(m, L"profileDark")));
    o += L"    \"profile_light\": " + JsonEsc(MGet(m, L"profileLight")) + L"\n  }\n}\n";
    return o;
}

// ===========================================================================
// Shared state
// ===========================================================================

struct Sample {
    double downBps = 0, upBps = 0;
    double cpu = 0;
    double gpu = -1;  // -1 = not available
    double memUsed = 0, memTotal = 0, memPct = 0;
    double cpuMHz = 0;
    uint64_t todayUp = 0, todayDown = 0;
    uint64_t weekBytes = 0, monthBytes = 0;
};

static std::atomic<bool> g_unloading{false};
static std::atomic<int> g_intervalMs{1000};
static std::atomic<bool> g_wantGpu{false};

static std::mutex g_sampleMtx;
static Sample g_sample;

static std::mutex g_ifaceMtx;
static std::wstring g_iface = L"auto";

static HANDLE g_stopEvent = nullptr;
// Never destroyed by the CRT: Windhawk does not call Wh_ModUninit when Explorer itself exits
// (restart, sign-out, shutdown), and a std::thread that is still joinable when its global
// destructor runs calls std::terminate(). They are joined and reset in Wh_ModUninit.
[[clang::no_destroy]] static std::optional<std::thread> g_worker;
[[clang::no_destroy]] static std::optional<std::thread> g_initThread;

// UI-thread-only state (the taskbar thread)
static Settings g_cfg;
static HWND g_taskbarWnd = nullptr;

// ===========================================================================
// Run code on the taskbar's thread (technique from Taskbar Fluent Media Player)
// ===========================================================================

using WindowThreadProc = void (*)(void*);

// With `wait` the call blocks until the taskbar thread has run `proc` (SendMessage, no timeout).
// It is used on the unload path: giving up after a timeout would leave this mod's timer and event
// handlers alive in Explorer after Windhawk frees the mod's code, which crashes Explorer. It is
// safe there because the panel, worker and init threads are already joined and the caller holds
// no lock that the taskbar thread needs.
static bool RunFromWindowThread(HWND hWnd, WindowThreadProc proc, void* param,
                                bool wait = false) {
    static const UINT RUN_FROM_THREAD_MSG =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct Payload {
        WindowThreadProc proc;
        void* param;
    };

    static std::mutex s_payloadMtx;
    static std::set<const void*> s_livePayloads;

    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;

    if (tid == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM w, LPARAM l) CALLBACK -> LRESULT {
            if (code == HC_ACTION) {
                auto* cwp = reinterpret_cast<const CWPSTRUCT*>(l);
                if (cwp->message == RUN_FROM_THREAD_MSG) {
                    auto* p = reinterpret_cast<Payload*>(cwp->lParam);
                    bool ours = false;
                    {
                        std::lock_guard<std::mutex> lk(s_payloadMtx);
                        ours = s_livePayloads.erase(p) == 1;
                    }
                    if (ours) p->proc(p->param);
                }
            }
            return CallNextHookEx(nullptr, code, w, l);
        },
        nullptr, tid);

    if (!hook) return false;

    Payload pay{proc, param};
    {
        std::lock_guard<std::mutex> lk(s_payloadMtx);
        s_livePayloads.insert(&pay);
    }
    // With a timeout: if the taskbar thread is busy or already shutting down (explorer
    // restarting), this must not block forever. When it times out, the payload is still
    // in the live set and is removed below, so the hook never runs it after `pay` is gone.
    if (wait) {
        SendMessageW(hWnd, RUN_FROM_THREAD_MSG, 0, reinterpret_cast<LPARAM>(&pay));
    } else {
        DWORD_PTR sendResult = 0;
        SendMessageTimeoutW(hWnd, RUN_FROM_THREAD_MSG, 0, reinterpret_cast<LPARAM>(&pay),
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 3000, &sendResult);
    }
    bool ran = false;
    {
        std::lock_guard<std::mutex> lk(s_payloadMtx);
        ran = s_livePayloads.erase(&pay) == 0;
    }
    UnhookWindowsHookEx(hook);
    return ran;
}

static HWND FindTaskbarWnd() {
    HWND h = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!h) return nullptr;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    return pid == GetCurrentProcessId() ? h : nullptr;
}

// ===========================================================================
// Getting the taskbar's XamlRoot (technique from Taskbar Fluent Media Player)
// ===========================================================================

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
using Std_Ref_Decref_t = void(WINAPI*)(void*);
using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);

static CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
static CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;
static TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
static Std_Ref_Decref_t Std_Ref_Decref_Original = nullptr;
static TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;
static void* CTaskBand_ITaskListWndSite_vftable = nullptr;
static void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

static bool IsReadableMemoryRange(const void* address, size_t size) {
    if (!address || size == 0) return false;
    MEMORY_BASIC_INFORMATION memory{};
    if (!VirtualQuery(address, &memory, sizeof(memory)) ||
        memory.State != MEM_COMMIT ||
        (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }
    const auto start = reinterpret_cast<uintptr_t>(address);
    const auto regionStart = reinterpret_cast<uintptr_t>(memory.BaseAddress);
    const auto regionEnd = regionStart + memory.RegionSize;
    return start >= regionStart && start <= regionEnd &&
           size <= regionEnd - start;
}

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    // The first taskbar and the ones on other monitors use different classes
    // (CTaskBand vs CSecondaryTaskBand) with different vtables.
    wchar_t clsBuf[64] = {};
    GetClassNameW(hTaskbarWnd, clsBuf, ARRAYSIZE(clsBuf));
    const bool isSecondary = _wcsicmp(clsBuf, L"Shell_SecondaryTrayWnd") == 0;

    HWND hTaskSwWnd = isSecondary ? FindWindowExW(hTaskbarWnd, nullptr, L"WorkerW", nullptr)
                                  : (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) return nullptr;

    void* taskBand = (void*)GetWindowLongPtrW(hTaskSwWnd, 0);
    if (!taskBand) return nullptr;

    void* const expectedVftable =
        isSecondary ? CSecondaryTaskBand_ITaskListWndSite_vftable : CTaskBand_ITaskListWndSite_vftable;
    auto const getTaskbarHost =
        isSecondary ? CSecondaryTaskBand_GetTaskbarHost_Original : CTaskBand_GetTaskbarHost_Original;
    if (!expectedVftable || !getTaskbarHost) return nullptr;

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0;; i++) {
        if (!IsReadableMemoryRange(taskBandForTaskListWndSite, sizeof(void*)))
            return nullptr;
        if (*(void**)taskBandForTaskListWndSite == expectedVftable)
            break;
        if (i == 20) return nullptr;
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(taskBandForTaskListWndSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0]) return nullptr;

    auto releaseHost = [&]() {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
    };

    // Offset of the taskbar's IUnknown inside TaskbarHost, read from the first instructions of
    // TaskbarHost::FrameHeight. The x86-64 and ARM64 patterns are known. Windhawk loads the x86-64
    // build into a native ARM64 Explorer too (Explorer is a predefined shell process, so the mod is
    // compiled for ARM64 there). On any other architecture the offset is never guessed.
    size_t taskbarElementIUnknownOffset = 0x10;
#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b && b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp  fp, lr, [sp, #-0x10]!
        // fd030091 mov  fp, sp
        // 080c41f8 ldr  x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p && p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD && p[2] == 0x910003FD &&
            (p[3] & 0xFFFFFE00) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern");
        }
    }
#else
    Wh_Log(L"Unsupported architecture: can't find the taskbar element");
    releaseHost();
    return nullptr;
#endif

    // Never call QueryInterface through a pointer that is not valid (a wrong offset would
    // otherwise crash Explorer): the object and its vtable must be readable.
    void* const* elemSlot =
        (void* const*)((BYTE*)taskbarHostSharedPtr[0] + taskbarElementIUnknownOffset);
    if (!IsReadableMemoryRange(elemSlot, sizeof(void*))) {
        releaseHost();
        return nullptr;
    }
    auto* taskbarElementIUnknown = (IUnknown*)*elemSlot;
    if (!taskbarElementIUnknown || !IsReadableMemoryRange(taskbarElementIUnknown, sizeof(void*)) ||
        !IsReadableMemoryRange(*(void**)taskbarElementIUnknown, 3 * sizeof(void*))) {
        Wh_Log(L"TaskbarHost element pointer is not valid");
        releaseHost();
        return nullptr;
    }

    FrameworkElement taskbarElement{nullptr};
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));
    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    releaseHost();
    return result;
}

// ===========================================================================
// XAML tree helpers
// ===========================================================================

static FrameworkElement FindChildByName(FrameworkElement const& root,
                                        std::wstring_view name, int depth = 32) {
    if (!root || depth == 0) return nullptr;
    int n = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; ++i) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (child.Name() == name) return child;
        if (auto found = FindChildByName(child, name, depth - 1)) return found;
    }
    return nullptr;
}

static FrameworkElement FindChildByClassName(FrameworkElement const& parent,
                                             const wchar_t* className,
                                             int depth = 32) {
    if (!parent || depth <= 0) return nullptr;
    int n = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (winrt::get_class_name(child) == className) return child;
        if (auto found = FindChildByClassName(child, className, depth - 1))
            return found;
    }
    return nullptr;
}

static Grid FindTaskbarRootGrid(FrameworkElement const& root) {
    FrameworkElement taskbarFrame = nullptr;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto c = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (c && winrt::get_class_name(c) == L"Taskbar.TaskbarFrame") {
            taskbarFrame = c;
            break;
        }
    }
    if (!taskbarFrame) return nullptr;
    auto rootGrid = FindChildByName(taskbarFrame, L"RootGrid");
    return rootGrid ? rootGrid.try_as<Grid>() : nullptr;
}

// The tray's direct child that contains `elem`.
static FrameworkElement TrayDirectChild(Panel const& tray,
                                        FrameworkElement const& elem) {
    winrt::Windows::UI::Xaml::DependencyObject cur = elem;
    while (cur) {
        auto parent = VisualTreeHelper::GetParent(cur);
        if (parent == tray) return cur.try_as<FrameworkElement>();
        cur = parent;
    }
    return nullptr;
}

// Grid column or StackPanel index next to `anchor`; -1 without an anchor.
static int TraySlot(Panel const& tray, FrameworkElement const& anchor, bool after) {
    if (!anchor) return -1;
    if (auto stack = tray.try_as<StackPanel>()) {
        uint32_t i = 0;
        if (!stack.Children().IndexOf(anchor, i)) return -1;
        return (int)i + (after ? 1 : 0);
    }
    return Grid::GetColumn(anchor) + (after ? Grid::GetColumnSpan(anchor) : 0);
}

static void InsertGridColumn(Grid const& grid, int col) {
    ColumnDefinition newCol;
    newCol.Width({1.0, GridUnitType::Auto});
    if (col >= (int)grid.ColumnDefinitions().Size()) {
        grid.ColumnDefinitions().Append(newCol);
    } else {
        grid.ColumnDefinitions().InsertAt(col, newCol);
        for (uint32_t i = 0; i < grid.Children().Size(); ++i) {
            auto child = grid.Children().GetAt(i).try_as<FrameworkElement>();
            if (child) {
                int childCol = Grid::GetColumn(child);
                if (childCol >= col) Grid::SetColumn(child, childCol + 1);
            }
        }
    }
}

// ===========================================================================
// Number formatting (same rules as the Python TrafficMonitor)
// ===========================================================================

static std::wstring FmtNum(double v, int dec) {
    wchar_t buf[64];
    _snwprintf(buf, 63, L"%.*f", dec, v);
    buf[63] = 0;
    return buf;
}

// Bytes: base 1024 (KB/s). Bits: base 1000 (Kb/s).
static void FmtSpeed(double bps, bool bits, const std::wstring& fixed, int dec,
                     std::wstring& num, std::wstring& unit) {
    double v, base;
    const wchar_t* units[3];
    if (bits) {
        v = bps * 8 / 1000;
        base = 1000;
        units[0] = L"Kb/s"; units[1] = L"Mb/s"; units[2] = L"Gb/s";
    } else {
        v = bps / 1024;
        base = 1024;
        units[0] = L"KB/s"; units[1] = L"MB/s"; units[2] = L"GB/s";
    }
    int i = 0;
    if (fixed == L"m") {
        v /= base;
        i = 1;
    } else if (fixed == L"auto") {
        while (i < 2 && v >= 1000) {
            v /= base;
            i++;
        }
    }
    int d = dec >= 0 ? dec : (v < 10 ? 2 : v < 100 ? 1 : 0);
    num = FmtNum(v, d);
    unit = units[i];
}

static void FmtSize(double bytes, int dec, std::wstring& num, std::wstring& unit) {
    static const wchar_t* units[] = {L"KB", L"MB", L"GB", L"TB"};
    double v = bytes / 1024;
    int i = 0;
    while (i < 3 && v >= 1000) {
        v /= 1024;
        i++;
    }
    int d = dec >= 0 ? dec : (v < 10 ? 2 : v < 100 ? 1 : 0);
    num = FmtNum(v, d);
    unit = units[i];
}

static std::wstring FmtSizeText(double bytes) {
    std::wstring n, u;
    FmtSize(bytes, -1, n, u);
    return n + L" " + u;
}

// ===========================================================================
// Data sampling (worker thread): network, CPU, memory, CPU frequency, GPU
// ===========================================================================

// Same layout as the Windows MIB_IF_ROW2 (1352 bytes); declared here so the mod
// needs no winsock headers. GetIfTable2 is loaded dynamically.
struct IfRow2 {
    ULONG64 InterfaceLuid;
    ULONG InterfaceIndex;
    BYTE InterfaceGuid[16];
    WCHAR Alias[257];
    WCHAR Description[257];
    ULONG PhysicalAddressLength;
    BYTE PhysicalAddress[32];
    BYTE PermanentPhysicalAddress[32];
    ULONG Mtu;
    ULONG Type;
    int TunnelType;
    int MediaType;
    int PhysicalMediumType;
    int AccessType;
    int DirectionType;
    BYTE InterfaceAndOperStatusFlags;
    int OperStatus;
    int AdminStatus;
    int MediaConnectState;
    BYTE NetworkGuid[16];
    int ConnectionType;
    ULONG64 TransmitLinkSpeed, ReceiveLinkSpeed;
    ULONG64 InOctets, InUcastPkts, InNUcastPkts, InDiscards, InErrors,
        InUnknownProtos, InUcastOctets, InMulticastOctets, InBroadcastOctets;
    ULONG64 OutOctets, OutUcastPkts, OutNUcastPkts, OutDiscards, OutErrors,
        OutUcastOctets, OutMulticastOctets, OutBroadcastOctets, OutQLen;
};
static_assert(sizeof(IfRow2) == 1352, "IfRow2 layout");

struct IfTable2 {
    ULONG NumEntries;
    IfRow2 Table[1];
};

using GetIfTable2_t = ULONG(WINAPI*)(IfTable2**);
using FreeMibTable_t = void(WINAPI*)(PVOID);
static GetIfTable2_t pGetIfTable2 = nullptr;
static FreeMibTable_t pFreeMibTable = nullptr;

struct NetRow {
    std::wstring alias;
    ULONG64 in = 0, out = 0;
    bool up = false;
};
using NetMap = std::unordered_map<ULONG64, NetRow>;

static std::wstring Lower(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}

static bool IsVirtualName(const std::wstring& alias, const std::wstring& desc) {
    static const wchar_t* words[] = {
        L"loopback", L"pseudo", L"vethernet", L"virtualbox", L"vmware", L"vmnet",
        L"bluetooth", L"isatap", L"teredo", L"docker", L"wsl",
        L"packet scheduler", L"lightweight filter", L"-wfp", L"-qos"};
    std::wstring a = Lower(alias), d = Lower(desc);
    for (auto* w : words)
        if (a.find(w) != std::wstring::npos || d.find(w) != std::wstring::npos)
            return true;
    return false;
}

struct NetSnapshot {
    NetMap rows;
    std::map<ULONG64, bool> candidate;  // real adapter (for "auto")
};

static bool ReadNet(NetSnapshot& snap) {
    snap.rows.clear();
    snap.candidate.clear();
    if (!pGetIfTable2) return false;
    IfTable2* t = nullptr;
    if (pGetIfTable2(&t) != 0 || !t) return false;
    for (ULONG i = 0; i < t->NumEntries; i++) {
        const IfRow2& r = t->Table[i];
        NetRow n;
        n.alias = r.Alias;
        n.in = r.InOctets;
        n.out = r.OutOctets;
        n.up = r.OperStatus == 1;  // IfOperStatusUp
        snap.rows[r.InterfaceLuid] = n;
        // 24 = loopback, 131 = tunnel
        snap.candidate[r.InterfaceLuid] =
            n.up && r.Type != 24 && r.Type != 131 &&
            !IsVirtualName(r.Alias, r.Description);
    }
    if (pFreeMibTable) pFreeMibTable(t);
    return true;
}

struct PpiEntry {
    ULONG Number, MaxMhz, CurrentMhz, MhzLimit, MaxIdleState, CurrentIdleState;
};
using CallNtPowerInformation_t = LONG(WINAPI*)(int, PVOID, ULONG, PVOID, ULONG);
static CallNtPowerInformation_t pCallNtPower = nullptr;

static double ReadCpuMHz() {
    if (!pCallNtPower) return 0;
    SYSTEM_INFO si{};
    GetSystemInfo(&si);
    DWORD n = si.dwNumberOfProcessors ? si.dwNumberOfProcessors : 1;
    std::vector<PpiEntry> buf(n);
    // 11 = ProcessorInformation
    if (pCallNtPower(11, nullptr, 0, buf.data(), (ULONG)(buf.size() * sizeof(PpiEntry))) == 0)
        return (double)buf[0].CurrentMhz;
    return 0;
}

// GPU usage through the "GPU Engine" performance counters (like Task Manager):
// utilization summed per engine type, then the highest type.
struct GpuPdh {
    PDH_HQUERY q = nullptr;
    PDH_HCOUNTER c = nullptr;
    std::vector<BYTE> buf;
    bool failed = false;

    bool Open() {
        if (q) return true;
        if (failed) return false;
        if (PdhOpenQueryW(nullptr, 0, &q) != ERROR_SUCCESS) {
            q = nullptr;
            failed = true;
            return false;
        }
        if (PdhAddEnglishCounterW(q, L"\\GPU Engine(*)\\Utilization Percentage", 0, &c) !=
            ERROR_SUCCESS) {
            PdhCloseQuery(q);
            q = nullptr;
            failed = true;
            return false;
        }
        PdhCollectQueryData(q);
        return true;
    }
    void Close() {
        if (q) PdhCloseQuery(q);
        q = nullptr;
        c = nullptr;
    }
    double Read() {
        if (!Open()) return -1;
        if (PdhCollectQueryData(q) != ERROR_SUCCESS) return -1;
        DWORD size = 0, count = 0;
        PDH_STATUS st = PdhGetFormattedCounterArrayW(c, PDH_FMT_DOUBLE, &size, &count, nullptr);
        if (st != PDH_MORE_DATA || size == 0) return -1;
        buf.resize(size);
        auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buf.data());
        st = PdhGetFormattedCounterArrayW(c, PDH_FMT_DOUBLE, &size, &count, items);
        if (st != ERROR_SUCCESS) return -1;
        std::map<std::wstring, double> byType;
        for (DWORD i = 0; i < count; i++) {
            if (!items[i].szName) continue;
            if (items[i].FmtValue.CStatus != PDH_CSTATUS_VALID_DATA &&
                items[i].FmtValue.CStatus != PDH_CSTATUS_NEW_DATA)
                continue;
            std::wstring name = items[i].szName;
            size_t p = name.find(L"engtype_");
            if (p == std::wstring::npos) continue;
            byType[name.substr(p + 8)] += items[i].FmtValue.doubleValue;
        }
        if (byType.empty()) return -1;
        double best = 0;
        for (auto& kv : byType) best = std::max(best, kv.second);
        return std::min(100.0, best);
    }
};

// ---- History (traffic per day, kept in the mod's own storage) ----

struct DayBytes {
    uint64_t up = 0, down = 0;
};
static std::map<std::wstring, DayBytes> g_hist;  // guarded by g_histMtx (worker + settings panel)
static std::mutex g_histMtx;
static constexpr size_t kHistDays = 3660;  // about ten years

static std::wstring DateStr(int daysAgo) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);
    ULARGE_INTEGER u;
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    u.QuadPart -= (ULONGLONG)daysAgo * 864000000000ULL;
    ft.dwLowDateTime = u.LowPart;
    ft.dwHighDateTime = u.HighPart;
    FileTimeToSystemTime(&ft, &st);
    wchar_t buf[16];
    _snwprintf(buf, 15, L"%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
    buf[15] = 0;
    return buf;
}

static void LoadHistory() {
    // New storage is chunked ("hist"); older versions kept one string ("history", 90 days).
    std::wstring s = ReadStoredText(L"hist");
    if (s.empty()) {
        std::vector<wchar_t> buf(8192, L'\0');
        Wh_GetStringValue(L"history", buf.data(), (int)buf.size() - 1);
        s = buf.data();
    }
    std::map<std::wstring, DayBytes> loaded;
    size_t pos = 0;
    while (pos < s.size()) {
        size_t end = s.find(L';', pos);
        if (end == std::wstring::npos) end = s.size();
        std::wstring rec = s.substr(pos, end - pos);
        pos = end + 1;
        size_t c1 = rec.find(L','), c2 = rec.find(L',', c1 == std::wstring::npos ? 0 : c1 + 1);
        if (c1 == std::wstring::npos || c2 == std::wstring::npos) continue;
        DayBytes d;
        d.up = _wcstoui64(rec.substr(c1 + 1, c2 - c1 - 1).c_str(), nullptr, 10);
        d.down = _wcstoui64(rec.substr(c2 + 1).c_str(), nullptr, 10);
        loaded[rec.substr(0, c1)] = d;
    }
    std::lock_guard<std::mutex> lk(g_histMtx);
    g_hist = std::move(loaded);
}

static void SaveHistory() {
    std::wstring out;
    {
        std::lock_guard<std::mutex> lk(g_histMtx);
        while (g_hist.size() > kHistDays) g_hist.erase(g_hist.begin());
        for (auto& kv : g_hist) {
            out += kv.first + L"," + std::to_wstring(kv.second.up) + L"," +
                   std::to_wstring(kv.second.down) + L";";
        }
    }
    if (WriteStoredText(L"hist", out)) Wh_DeleteValue(L"history");  // old single-string copy
}

static std::map<std::wstring, DayBytes> HistorySnapshot() {
    std::lock_guard<std::mutex> lk(g_histMtx);
    return g_hist;
}

// Merges days into the history; for each day the larger value wins, so importing the same
// file twice (or a file that overlaps what is already here) never double-counts.
static int HistoryMerge(const std::map<std::wstring, DayBytes>& in) {
    int changed = 0;
    std::lock_guard<std::mutex> lk(g_histMtx);
    for (auto& kv : in) {
        DayBytes& d = g_hist[kv.first];
        const uint64_t up = std::max(d.up, kv.second.up), down = std::max(d.down, kv.second.down);
        if (up != d.up || down != d.down) changed++;
        d.up = up;
        d.down = down;
    }
    return changed;
}

static void WorkerProc() {
    GpuPdh gpu;
    NetSnapshot prev, cur;
    bool havePrev = false;
    auto prevTime = std::chrono::steady_clock::now();
    ULONGLONG prevIdle = 0, prevKernel = 0, prevUser = 0;
    bool haveCpu = false;
    ULONGLONG lastSave = GetTickCount64();
    LoadHistory();

    auto ft = [](const FILETIME& f) {
        return ((ULONGLONG)f.dwHighDateTime << 32) | f.dwLowDateTime;
    };

    for (;;) {
        Sample s;

        // --- network ---
        std::wstring ifaceWanted;
        {
            std::lock_guard<std::mutex> lk(g_ifaceMtx);
            ifaceWanted = g_iface;
        }
        auto now = std::chrono::steady_clock::now();
        double dt = std::chrono::duration<double>(now - prevTime).count();
        if (dt < 0.05) dt = 0.05;
        prevTime = now;
        uint64_t dUp = 0, dDown = 0;
        if (ReadNet(cur)) {
            bool autoMode = _wcsicmp(ifaceWanted.c_str(), L"auto") == 0;
            std::map<ULONG64, std::pair<uint64_t, uint64_t>> deltas;
            for (auto& kv : cur.rows) {
                const NetRow& r = kv.second;
                if (autoMode) {
                    if (!cur.candidate[kv.first]) continue;
                } else if (_wcsicmp(r.alias.c_str(), ifaceWanted.c_str()) != 0) {
                    continue;
                }
                auto it = prev.rows.find(kv.first);
                if (!havePrev || it == prev.rows.end()) continue;
                uint64_t du = r.out >= it->second.out ? r.out - it->second.out : 0;
                uint64_t dd = r.in >= it->second.in ? r.in - it->second.in : 0;
                deltas[kv.first] = {du, dd};
            }
            if (autoMode && deltas.size() > 1) {
                // VPNs, bridges... count the same bytes again: use only the busiest one.
                ULONG64 best = 0;
                uint64_t bestSum = 0;
                bool any = false;
                for (auto& kv : deltas) {
                    uint64_t sum = kv.second.first + kv.second.second;
                    if (!any || sum > bestSum) {
                        best = kv.first;
                        bestSum = sum;
                        any = true;
                    }
                }
                dUp = deltas[best].first;
                dDown = deltas[best].second;
            } else {
                for (auto& kv : deltas) {
                    dUp += kv.second.first;
                    dDown += kv.second.second;
                }
            }
            std::swap(prev, cur);
            havePrev = true;
        }
        s.upBps = dUp / dt;
        s.downBps = dDown / dt;

        // --- history ---
        {
            std::lock_guard<std::mutex> hl(g_histMtx);
            std::wstring today = DateStr(0);
            DayBytes& day = g_hist[today];
            day.up += dUp;
            day.down += dDown;
            s.todayUp = day.up;
            s.todayDown = day.down;
            std::wstring weekStart = DateStr(6);
            std::wstring monthPrefix = today.substr(0, 8);
            for (auto& kv : g_hist) {
                if (kv.first >= weekStart) s.weekBytes += kv.second.up + kv.second.down;
                if (kv.first.compare(0, 8, monthPrefix) == 0)
                    s.monthBytes += kv.second.up + kv.second.down;
            }
        }

        // --- CPU ---
        FILETIME fi, fk, fu;
        if (GetSystemTimes(&fi, &fk, &fu)) {
            ULONGLONG idle = ft(fi), kern = ft(fk), user = ft(fu);
            if (haveCpu) {
                ULONGLONG di = idle - prevIdle, dk = kern - prevKernel, du = user - prevUser;
                ULONGLONG total = dk + du;  // kernel time already includes idle time
                if (total > 0)
                    s.cpu = std::clamp((double)(total - di) * 100.0 / (double)total, 0.0, 100.0);
            }
            prevIdle = idle;
            prevKernel = kern;
            prevUser = user;
            haveCpu = true;
        }

        // --- memory ---
        MEMORYSTATUSEX ms{};
        ms.dwLength = sizeof(ms);
        if (GlobalMemoryStatusEx(&ms)) {
            s.memTotal = (double)ms.ullTotalPhys;
            s.memUsed = (double)(ms.ullTotalPhys - ms.ullAvailPhys);
            s.memPct = ms.ullTotalPhys ? s.memUsed * 100.0 / (double)ms.ullTotalPhys : 0;
        }
        s.cpuMHz = ReadCpuMHz();

        // --- GPU ---
        if (g_wantGpu) s.gpu = gpu.Read();
        else gpu.Close();

        {
            std::lock_guard<std::mutex> lk(g_sampleMtx);
            g_sample = s;
        }

        if (GetTickCount64() - lastSave > 60000) {
            SaveHistory();
            lastSave = GetTickCount64();
        }

        if (WaitForSingleObject(g_stopEvent, (DWORD)g_intervalMs.load()) != WAIT_TIMEOUT) break;
    }

    SaveHistory();
    gpu.Close();
}

// ===========================================================================
// UI (always on the taskbar's thread)
// ===========================================================================

struct Slot {
    TextBlock tb{nullptr};
    std::vector<TextBlock> fx;  // shadow / outline copies drawn behind `tb`
    int kind = 1;               // 0 label, 1 number, 2 unit, 3 separator
    int field = 0;              // MEM: 0 percent, 1 used, 2 available
    Color base{};
    std::wstring last;
    double minW = 0;                                       // reserved width of a number slot
    std::function<double(const std::wstring&)> measure;    // text width with this slot's font
};

struct IconShape {
    Shapes::Path path{nullptr};
    bool stroke = false;
};

struct ItemUi {
    int item = 0;
    std::vector<Slot> slots;
    std::vector<IconShape> icon;
    Color iconBase{};
    Color alertColor{};
    bool alerting = false;
};

// The settings panel (defined further down; it runs on its own thread).
static constexpr int kPageHistory = 4;  // index of the History page in the settings panel
namespace xs {
static void Show(int page = -1);  // page: which tab to open (-1 = the one that was open)
static void NotifyRefresh();
}  // namespace xs

// The original app's own colors for each part (what shows until a color is chosen).
static const wchar_t* DefaultPartColor(bool modern, int item, int role) {
    static const wchar_t* classicLabel[I_COUNT] = {L"#00FF46", L"#57FFFF", L"#FF4200",
                                                   L"#FF57FF", L"#FFD400", L"#A58CFF"};
    static const wchar_t* classicValue[I_COUNT] = {L"#00FF46", L"#00FFFF", L"#FF4200",
                                                   L"#FFFFFF", L"#FFD400", L"#A58CFF"};
    static const wchar_t* modernIcon[I_COUNT] = {L"#33cc95", L"#79cde1", L"#f2b924",
                                                 L"#a187f1", L"#6bd968", L"#f08ac8"};
    static const bool modernMuted[I_COUNT] = {true, true, false, false, false, true};
    if (modern) {
        if (role == 0) return modernIcon[item];
        if (role == 1) return L"#dae0e8";
        return modernMuted[item] ? L"#8f9db2" : L"#dae0e8";
    }
    return role == 0 ? classicLabel[item] : classicValue[item];
}

// One of these per taskbar that carries a monitor (first monitor, second monitor, or both).
// Everything in it is only touched from the thread of that taskbar's window.
struct Tb {
    HWND wnd = nullptr;
    bool primary = false;  // Shell_TrayWnd (false: Shell_SecondaryTrayWnd)
    FrameworkElement elem{nullptr};
    Panel parent{nullptr};
    FrameworkElement rootContent{nullptr};
    bool addedColumn = false;
    std::vector<ItemUi> items;
    ToolTip tip{nullptr};
    std::wstring tipText;
    unsigned tick = 0;
};
static std::vector<Tb*> g_tbs;  // changed only on the primary taskbar's thread
// Released on the taskbar thread by StopTimer(). Not destroyed automatically: at process exit
// the XAML core is already gone and the destructor would release it on the wrong thread.
[[clang::no_destroy]] static DispatcherTimer g_timer{nullptr};
static winrt::event_token g_tickToken{};
static unsigned g_tick = 0;

static bool ParseHex(std::wstring s, Color& out) {
    while (!s.empty() && iswspace(s.back())) s.pop_back();
    while (!s.empty() && iswspace(s.front())) s.erase(s.begin());
    if (!s.empty() && s[0] == L'#') s.erase(s.begin());
    if (s.size() != 6) return false;
    for (wchar_t ch : s)
        if (!iswxdigit(ch)) return false;
    unsigned v = wcstoul(s.c_str(), nullptr, 16);
    out = Color{255, (uint8_t)(v >> 16), (uint8_t)(v >> 8), (uint8_t)v};
    return true;
}

static Color ColorOr(const std::wstring& setting, const wchar_t* fallback) {
    Color c{};
    if (ParseHex(setting, c)) return c;
    ParseHex(fallback, c);
    return c;
}

static double MeasureText(const std::wstring& s, FontFamily const& ff, double fs, bool bold) {
    TextBlock t;
    t.FontFamily(ff);
    t.FontSize(fs);
    t.FontWeight(bold ? FontWeights::Bold() : FontWeights::Normal());
    t.Text(s);
    const float inf = std::numeric_limits<float>::infinity();
    t.Measure({inf, inf});
    return t.DesiredSize().Width;
}

static Shapes::Path MakePath(const std::wstring& data, bool stroke, double thick) {
    std::wstring xaml =
        L"<Path xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' "
        L"StrokeStartLineCap='Round' StrokeEndLineCap='Round' StrokeLineJoin='Round' "
        L"StrokeThickness='" + std::to_wstring(stroke ? thick : 0.0) + L"' Data='" + data + L"'/>";
    return winrt::Windows::UI::Xaml::Markup::XamlReader::Load(xaml).as<Shapes::Path>();
}

// Icons in the reference pixels of the original design (see the Python script).
struct IconDef {
    double w, h;
    struct P { const wchar_t* data; bool stroke; double thick; };
    std::vector<P> parts;
};

static IconDef GetIconDef(int item) {
    switch (item) {
        case I_UP:
            return {39, 44, {{L"M19.5,3.7 L19.5,40.3 M3.7,19.5 L19.5,3.7 L35.3,19.5", true, 7.2}}};
        case I_DOWN:
            return {39, 44, {{L"M19.5,3.7 L19.5,40.3 M3.7,24.5 L19.5,40.3 L35.3,24.5", true, 7.2}}};
        case I_CPU:
            return {50, 50, {
                {L"M15.7,9.2 L34.3,9.2 A6.5,6.5 0 0 1 40.8,15.7 L40.8,34.3 A6.5,6.5 0 0 1 34.3,40.8 "
                 L"L15.7,40.8 A6.5,6.5 0 0 1 9.2,34.3 L9.2,15.7 A6.5,6.5 0 0 1 15.7,9.2 Z", true, 4.4},
                {L"M15.5,15.5 L34.5,15.5 L34.5,34.5 L15.5,34.5 Z", false, 0},
                {L"M14.8,2.2 L14.8,9.2 M25,2.2 L25,9.2 M35.2,2.2 L35.2,9.2 "
                 L"M14.8,40.8 L14.8,47.8 M25,40.8 L25,47.8 M35.2,40.8 L35.2,47.8 "
                 L"M2.2,14.8 L9.2,14.8 M2.2,25 L9.2,25 M2.2,35.2 L9.2,35.2 "
                 L"M40.8,14.8 L47.8,14.8 M40.8,25 L47.8,25 M40.8,35.2 L47.8,35.2", true, 4.4}}};
        case I_MEM:
            return {57, 38, {
                {L"F0 M5,0 L52,0 A5,5 0 0 1 57,5 L57,25.6 L0,25.6 L0,5 A5,5 0 0 1 5,0 Z "
                 L"M12.9,7 L19.5,7 L19.5,19 L12.9,19 Z M25.2,7 L31.8,7 L31.8,19 L25.2,19 Z "
                 L"M38,7 L44.6,7 L44.6,19 L38,19 Z", false, 0},
                {L"F0 M0,27.8 L57,27.8 L57,35.4 A2.6,2.6 0 0 1 54.4,38 L2.6,38 A2.6,2.6 0 0 1 0,35.4 Z "
                 L"M8.2,33.4 L11,33.4 L11,38 L8.2,38 Z M20.6,33.4 L23.4,33.4 L23.4,38 L20.6,38 Z "
                 L"M33.3,33.4 L36.1,33.4 L36.1,38 L33.3,38 Z M45.9,33.4 L48.7,33.4 L48.7,38 L45.9,38 Z",
                 false, 0}}};
        case I_GPU:
            return {57, 38, {
                {L"M6.7,3 L50.3,3 A4.5,4.5 0 0 1 54.8,7.5 L54.8,24.5 A4.5,4.5 0 0 1 50.3,29 "
                 L"L6.7,29 A4.5,4.5 0 0 1 2.2,24.5 L2.2,7.5 A4.5,4.5 0 0 1 6.7,3 Z", true, 4.4},
                {L"M12.5,16 A6.5,6.5 0 1 0 25.5,16 A6.5,6.5 0 1 0 12.5,16 Z "
                 L"M34,12 L47,12 M34,20 L43,20", true, 3.6},
                {L"M7,32 L19,32 L19,36.5 L7,36.5 Z M26,32 L38,32 L38,36.5 L26,36.5 Z", false, 0}}};
        default:  // I_TOTAL
            return {50, 50, {
                {L"M4,26 L16,26 L16,46 L4,46 Z M19,14 L31,14 L31,46 L19,46 Z "
                 L"M34,3 L46,3 L46,46 L34,46 Z", false, 0}}};
    }
}

static void PaintIcon(ItemUi& ui, Color c) {
    SolidColorBrush b(c);
    for (auto& s : ui.icon) {
        if (s.stroke) s.path.Stroke(b);
        else s.path.Fill(b);
    }
}

static FrameworkElement MakeIcon(ItemUi& ui, double em, Color col) {
    IconDef def = GetIconDef(ui.item);
    Canvas canvas;
    canvas.Width(def.w);
    canvas.Height(def.h);
    for (auto& p : def.parts) {
        Shapes::Path path = MakePath(p.data, p.stroke, p.thick);
        canvas.Children().Append(path);
        ui.icon.push_back({path, p.stroke});
    }
    PaintIcon(ui, col);
    Viewbox vb;
    double k = em / 50.5;
    vb.Width(std::max(1.0, def.w * k));
    vb.Height(std::max(1.0, def.h * k));
    vb.Stretch(Stretch::Uniform);
    vb.VerticalAlignment(VerticalAlignment::Center);
    vb.Child(canvas);
    return vb;
}


static std::vector<int> MemFields(const Settings& c) {
    std::vector<int> f;
    for (int i = 0; i < 3; i++)
        if (c.memField[i]) f.push_back(i);
    if (f.empty()) f.push_back(0);
    return f;
}

static void SetSlotColor(Slot& s, Color c) {
    s.tb.Foreground(SolidColorBrush(c));
}

static void SetSlotText(Slot& s, const std::wstring& text) {
    s.tb.Text(text);
    for (auto& f : s.fx) f.Text(text);
}

static void ApplyAlert(ItemUi& ui, bool on) {
    if (ui.alerting == on) return;
    ui.alerting = on;
    for (auto& s : ui.slots) SetSlotColor(s, on ? ui.alertColor : s.base);
    PaintIcon(ui, on ? ui.alertColor : ui.iconBase);
}

// windhawk.exe, found from the windhawk.dll loaded in this process
// (<Windhawk>\Engine\<version>\<arch>\windhawk.dll), so portable installs work too.
static std::wstring FindWindhawkExe() {
    auto isFile = [](const std::wstring& path) {
        DWORD attrs = GetFileAttributesW(path.c_str());
        return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
    };
    wchar_t path[MAX_PATH]{};
    HMODULE engine = GetModuleHandleW(L"windhawk.dll");
    if (engine && GetModuleFileNameW(engine, path, MAX_PATH)) {
        std::wstring dir = path;
        for (int up = 0; up < 4; up++) {
            const size_t slash = dir.find_last_of(L'\\');
            if (slash == std::wstring::npos) break;
            dir.resize(slash);
            if (isFile(dir + L"\\windhawk.exe")) return dir + L"\\windhawk.exe";
        }
    }
    wchar_t fallback[MAX_PATH]{};
    if (ExpandEnvironmentStringsW(L"%ProgramFiles%\\Windhawk\\windhawk.exe", fallback, MAX_PATH) &&
        isFile(fallback))
        return fallback;
    return {};
}

static void OpenWindhawk() {
    const std::wstring exe = FindWindhawkExe();
    if (exe.empty()) {
        Wh_Log(L"OpenWindhawk: windhawk.exe not found");
        return;
    }
    std::wstring cmd = L"\"" + exe + L"\"";
    STARTUPINFOW si{sizeof(si)};
    PROCESS_INFORMATION pi{};
    if (CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

static MenuFlyoutItem MakeMenuItem(const wchar_t* glyph, const wchar_t* label, std::function<void()> onClick) {
    MenuFlyoutItem item;
    item.Text(winrt::hstring(label));
    try {
        FontIcon icon;
        icon.Glyph(glyph);
        icon.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
        item.Icon(icon);
    } catch (...) {
    }
    item.Click([onClick](winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&) {
        if (g_unloading) return;
        try {
            onClick();
        } catch (...) {
            Wh_Log(L"Menu item: exception ignored");
        }
    });
    return item;
}

// Right-click on the monitor: same context menu as Taskbar Fluent Media Player
// (opens above the element, "Open Windhawk" and "Settings" at the bottom).
static void ShowMonitorMenu(FrameworkElement const& target) {
    if (!target || g_unloading) return;
    try {
        MenuFlyout menu;
        try {
            menu.Placement(winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutPlacementMode::Top);
        } catch (...) {
        }
        menu.Items().Append(MakeMenuItem(L"\uE81C", TR(L"Historial de tráfico", L"Traffic history"),
                                         []() { xs::Show(kPageHistory); }));
        // Puzzle piece (mods); the gear is the Settings item right below.
        menu.Items().Append(MakeMenuItem(L"\uEA86", TR(L"Abrir Windhawk", L"Open Windhawk"),
                                         []() { OpenWindhawk(); }));
        menu.Items().Append(MakeMenuItem(L"\uE115", TR(L"Configuración", L"Settings"),
                                         []() { xs::Show(); }));
        menu.ShowAt(target);
    } catch (...) {
        Wh_Log(L"ShowMonitorMenu: exception ignored");
    }
}

static void BuildUi(Tb& t, const Settings& c) {
    t.items.clear();
    const bool modern = c.skin == L"modern";
    const double fs = c.fontSize;
    FontFamily ff(c.fontFamily.empty() ? (modern ? L"Consolas" : L"Segoe UI")
                                       : c.fontFamily.c_str());

    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatusEx(&ms);
    const bool bigMem = ms.ullTotalPhys >= (10ULL << 30);

    // Text effect: copies of the text drawn behind it, shifted by one pixel.
    std::vector<std::pair<double, double>> fxOffsets;
    if (c.effect == L"shadow") {
        fxOffsets = {{1, 1}};
    } else if (c.effect == L"outline") {
        fxOffsets = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
    }
    const Color fxColor = ColorOr(c.effectColor, L"#000000");

    Grid root;
    root.Name(L"TrafficMonitorMod");
    root.Background(SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
    root.VerticalAlignment(VerticalAlignment::Center);
    root.HorizontalAlignment(HorizontalAlignment::Left);
    root.Margin({(double)c.marginLeft, c.offsetY > 0 ? c.offsetY * 2.0 : 0.0,
                 (double)c.marginRight, c.offsetY < 0 ? -c.offsetY * 2.0 : 0.0});

    StackPanel row;
    row.Orientation(Orientation::Horizontal);
    row.VerticalAlignment(VerticalAlignment::Center);

    static const wchar_t* classicLabel[I_COUNT] = {L"\u2193:", L"\u2191:", L"CPU:",
                                                   L"MEM:",    L"GPU:",    nullptr};

    bool first = true;
    for (int item : c.order) {
        if (!c.show[item]) continue;
        ItemUi ui;
        ui.item = item;

        const Color labelCol = ColorOr(c.color[item][0], DefaultPartColor(modern, item, 0));
        const Color numCol = ColorOr(c.color[item][1], DefaultPartColor(modern, item, 1));
        const Color unitCol = ColorOr(c.color[item][2], DefaultPartColor(modern, item, 2));
        ui.iconBase = labelCol;
        ui.alertColor = ColorOr(c.alert[item].color, L"#ff3b30");

        StackPanel box;
        box.Orientation(Orientation::Horizontal);
        box.VerticalAlignment(VerticalAlignment::Center);
        if (!first) box.Margin({(double)c.itemGap, 0, 0, 0});
        first = false;

        auto addSlot = [&](int kind, int field, const std::wstring& tmpl, Color col,
                           bool ralign, double leftMargin, double rightMargin,
                           const std::wstring& fixedText) {
            const double minW = tmpl.empty() ? 0.0 : MeasureText(tmpl, ff, fs, c.bold);
            auto make = [&](Color fg) {
                TextBlock txt;
                txt.FontFamily(ff);
                txt.FontSize(fs);
                txt.FontWeight(c.bold ? FontWeights::Bold() : FontWeights::Normal());
                txt.VerticalAlignment(VerticalAlignment::Center);
                txt.Foreground(SolidColorBrush(fg));
                txt.Margin({leftMargin, 0, rightMargin, 0});
                if (ralign) txt.TextAlignment(TextAlignment::Right);
                if (minW > 0) txt.MinWidth(minW);
                if (!fixedText.empty()) txt.Text(fixedText);
                return txt;
            };
            Slot s;
            s.tb = make(col);
            s.kind = kind;
            s.field = field;
            s.base = col;
            if (kind == 1 && minW > 0) {
                s.minW = minW;
                s.measure = [ff, fs, bold = c.bold](const std::wstring& t) {
                    return MeasureText(t, ff, fs, bold);
                };
            }
            if (!fixedText.empty()) s.last = fixedText;
            if (!fxOffsets.empty()) {
                Grid host;
                for (auto& d : fxOffsets) {
                    TextBlock fx = make(fxColor);
                    fx.IsHitTestVisible(false);
                    TranslateTransform tt;
                    tt.X(d.first);
                    tt.Y(d.second);
                    fx.RenderTransform(tt);
                    host.Children().Append(fx);
                    s.fx.push_back(fx);
                }
                host.Children().Append(s.tb);
                box.Children().Append(host);
            } else {
                box.Children().Append(s.tb);
            }
            ui.slots.push_back(s);
        };

        // icon (modern) or text label (classic)
        if (modern) {
            auto icon = MakeIcon(ui, fs, labelCol);
            icon.Margin({0, 0, fs * 0.5, 0});
            box.Children().Append(icon);
        } else {
            std::wstring lbl = item == I_TOTAL ? c.todayLabel : classicLabel[item];
            if (!lbl.empty() && lbl.back() != L':' && item == I_TOTAL) lbl += L":";
            addSlot(0, 0, L"", labelCol, false, 0, 3, lbl);
        }

        const std::wstring speedUnitTmpl = c.speedBits ? L"Mb/s" : L"MB/s";
        std::wstring speedNumTmpl;
        if (c.speedUnit == L"k") speedNumTmpl = L"88888";
        else if (c.decimals < 0) speedNumTmpl = L"8.88";
        else {
            speedNumTmpl = L"888";
            if (c.decimals > 0) speedNumTmpl += L"." + std::wstring(c.decimals, L'8');
        }
        const std::wstring totalNumTmpl =
            c.decimals < 0 ? L"8.88"
                           : L"888" + (c.decimals > 0 ? L"." + std::wstring(c.decimals, L'8') : L"");

        if (item == I_DOWN || item == I_UP) {
            addSlot(1, 0, speedNumTmpl, numCol, true, 0, 0, L"");
            if (!c.hideUnits) addSlot(2, 0, speedUnitTmpl, unitCol, false, 3, 0, L"");
        } else if (item == I_CPU || item == I_GPU) {
            addSlot(1, 0, L"88", numCol, true, 0, 0, L"");
            if (!c.hideUnits) addSlot(2, 0, L"%", unitCol, false, modern ? 0 : 3, 0, L"");
        } else if (item == I_TOTAL) {
            addSlot(1, 0, totalNumTmpl, numCol, true, 0, 0, L"");
            if (!c.hideUnits) addSlot(2, 0, L"GB", unitCol, false, 3, 0, L"");
        } else {  // memory
            std::vector<int> fields = MemFields(c);
            for (size_t k = 0; k < fields.size(); k++) {
                int f = fields[k];
                if (k > 0) {
                    if (!c.memSep.empty()) addSlot(3, 0, L"", labelCol, false, 4, 4, c.memSep);  // same gap on both sides of the separator
                }
                std::wstring tmpl = f == 0 ? L"88"
                                    : c.decimals >= 0
                                        ? L"88" + (c.decimals > 0 ? L"." + std::wstring(c.decimals, L'8') : L"")
                                        : (bigMem ? L"88.88" : L"8.88");
                // Values after the first one reserve only the narrowest width, so there is no
                // blank space between the separator and the number (same gap on both sides).
                if (k > 0)
                    tmpl = f == 0 ? L"8"
                           : c.decimals >= 0
                               ? L"8" + (c.decimals > 0 ? L"." + std::wstring(c.decimals, L'8') : L"")
                               : L"8.88";
                addSlot(1, f, tmpl, numCol, true, 0, 0, L"");
                if (!c.hideUnits)
                    addSlot(2, f, f == 0 ? L"%" : L"GB", unitCol, false,
                            (modern && f == 0) ? 0 : 3, 0, L"");
            }
        }

        row.Children().Append(box);
        t.items.push_back(std::move(ui));
    }

    root.Children().Append(row);

    ToolTip tip;
    tip.Content(winrt::box_value(winrt::hstring(L"TrafficMonitor")));
    ToolTipService::SetToolTip(root, tip);
    t.tip = tip;
    t.tipText.clear();

    // Right click: the context menu with the settings panel (same wiring as the
    // Taskbar Fluent Media Player: capture on press, act on release inside the element).
    root.PointerPressed([](winrt::Windows::Foundation::IInspectable const& sender,
                           winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
        if (auto elem = sender.try_as<UIElement>()) elem.CapturePointer(e.Pointer());
    });
    root.PointerReleased([](winrt::Windows::Foundation::IInspectable const& sender,
                            winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e) {
        bool inside = false;
        if (auto elem = sender.try_as<UIElement>()) {
            elem.ReleasePointerCapture(e.Pointer());
            try {
                auto pos = e.GetCurrentPoint(elem).Position();
                auto size = elem.RenderSize();
                inside = pos.X >= 0 && pos.X <= size.Width && pos.Y >= 0 && pos.Y <= size.Height;
            } catch (...) {
                inside = false;
            }
        }
        if (g_unloading || !inside) return;
        try {
            auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
            if (kind == winrt::Windows::UI::Input::PointerUpdateKind::RightButtonReleased) {
                if (auto fe = sender.try_as<FrameworkElement>()) ShowMonitorMenu(fe);
            }
        } catch (...) {
            Wh_Log(L"PointerReleased: exception ignored");
        }
    });

    // Double click: the traffic history (like the original app).
    root.DoubleTapped([](winrt::Windows::Foundation::IInspectable const&,
                         winrt::Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs const&) {
        if (!g_unloading) xs::Show(kPageHistory);
    });

    t.elem = root;
}

static void UpdateValues(Tb& t) {
    if (!t.elem) return;
    Sample S;
    {
        std::lock_guard<std::mutex> lk(g_sampleMtx);
        S = g_sample;
    }
    const Settings& c = g_cfg;

    for (auto& ui : t.items) {
        bool alert = false;
        bool firstNum = true;
        for (auto& s : ui.slots) {
            if (s.kind == 0 || s.kind == 3) continue;
            const bool isFirstNum = s.kind == 1 && firstNum;
            if (s.kind == 1) firstNum = false;
            std::wstring num, unit;
            switch (ui.item) {
                case I_DOWN:
                    FmtSpeed(S.downBps, c.speedBits, c.speedUnit, c.decimals, num, unit);
                    break;
                case I_UP:
                    FmtSpeed(S.upBps, c.speedBits, c.speedUnit, c.decimals, num, unit);
                    break;
                case I_CPU:
                    num = FmtNum(S.cpu, 0);
                    unit = L"%";
                    break;
                case I_GPU:
                    num = S.gpu < 0 ? L"--" : FmtNum(S.gpu, 0);
                    unit = L"%";
                    break;
                case I_TOTAL: {
                    std::wstring n, u;
                    FmtSize((double)(S.todayUp + S.todayDown), c.decimals, n, u);
                    num = n;
                    unit = u;
                    break;
                }
                default: {  // memory
                    if (s.field == 0) {
                        num = FmtNum(S.memPct, 0);
                        unit = L"%";
                    } else {
                        double gb = (s.field == 1 ? S.memUsed : S.memTotal - S.memUsed) /
                                    (1024.0 * 1024.0 * 1024.0);
                        num = FmtNum(gb, c.decimals >= 0 ? c.decimals : 2);
                        unit = L"GB";
                    }
                }
            }
            const std::wstring& text = s.kind == 1 ? num : unit;
            if (text != s.last) {
                SetSlotText(s, text);
                s.last = text;
                // Classic skin: the number sits right next to its label. The unused part of the
                // reserved width moves to the left of the label, so the total width is unchanged.
                if (isFirstNum && s.measure && !ui.slots.empty() && ui.slots[0].kind == 0) {
                    const double slack = std::max(0.0, s.minW - s.measure(text));
                    Slot& lab = ui.slots[0];
                    lab.tb.Margin({slack, 0, 3, 0});
                    for (auto& f : lab.fx) f.Margin({slack, 0, 3, 0});
                    s.tb.MinWidth(0);
                    for (auto& f : s.fx) f.MinWidth(0);
                }
            }
        }

        const Alert& a = c.alert[ui.item];
        if (a.on) {
            switch (ui.item) {
                case I_DOWN: alert = S.downBps / (1024.0 * 1024.0) >= a.limit; break;
                case I_UP:   alert = S.upBps / (1024.0 * 1024.0) >= a.limit; break;
                case I_CPU:  alert = S.cpu >= a.limit; break;
                case I_GPU:  alert = S.gpu >= 0 && S.gpu >= a.limit; break;
                case I_MEM:  alert = S.memPct >= a.limit; break;
                case I_TOTAL:
                    alert = (double)(S.todayUp + S.todayDown) / (1024.0 * 1024.0 * 1024.0) >= a.limit;
                    break;
            }
        }
        ApplyAlert(ui, alert);
    }

    // tooltip (same lines as the original app)
    const std::wstring na = TR(L"N/D", L"N/A");
    std::wstring tip = std::wstring(TR(L"Tráfico usado hoy: ", L"Traffic today: ")) +
                       FmtSizeText((double)(S.todayUp + S.todayDown)) +
                       TR(L"\n  (Subida: ", L"\n  (Upload: ") + FmtSizeText((double)S.todayUp) +
                       TR(L" / Bajada: ", L" / Download: ") + FmtSizeText((double)S.todayDown) + L")\n" +
                       TR(L"Últimos 7 días: ", L"Last 7 days: ") + FmtSizeText((double)S.weekBytes) + L"\n" +
                       TR(L"Este mes: ", L"This month: ") + FmtSizeText((double)S.monthBytes) + L"\n" +
                       TR(L"Uso de CPU: ", L"CPU usage: ") + FmtNum(S.cpu, 0) + L" %\n" +
                       TR(L"Uso de memoria: ", L"Memory: ") + FmtNum(S.memUsed / 1073741824.0, 2) +
                       L" GB / " + FmtNum(S.memTotal / 1073741824.0, 2) + L" GB\n" +
                       TR(L"Frecuencia de CPU: ", L"CPU frequency: ") +
                       (S.cpuMHz > 0 ? FmtNum(S.cpuMHz / 1000.0, 2) + L" GHz" : na);
    if (c.show[I_GPU])
        tip += std::wstring(L"\n") + TR(L"Uso de GPU: ", L"GPU usage: ") +
               (S.gpu >= 0 ? FmtNum(S.gpu, 0) + L" %" : na);
    if (tip != t.tipText && t.tip) {
        t.tip.Content(winrt::box_value(winrt::hstring(tip)));
        t.tipText = tip;
    }
}

static void RemoveElement(Tb& t) {
    if (!t.elem) return;
    try {
        if (t.parent) {
            if (auto grid = t.parent.try_as<Grid>()) {
                int col = Grid::GetColumn(t.elem);
                uint32_t idx = 0;
                if (grid.Children().IndexOf(t.elem, idx)) grid.Children().RemoveAt(idx);
                if (t.addedColumn && col >= 0 && col < (int)grid.ColumnDefinitions().Size()) {
                    for (uint32_t i = 0; i < grid.Children().Size(); ++i) {
                        auto child = grid.Children().GetAt(i).try_as<FrameworkElement>();
                        if (child) {
                            int cc = Grid::GetColumn(child);
                            if (cc > col) Grid::SetColumn(child, cc - 1);
                        }
                    }
                    grid.ColumnDefinitions().RemoveAt(col);
                }
            } else {
                uint32_t idx = 0;
                if (t.parent.Children().IndexOf(t.elem, idx)) t.parent.Children().RemoveAt(idx);
            }
        }
    } catch (...) {
        Wh_Log(L"RemoveElement: exception ignored");
    }
    t.elem = nullptr;
    t.parent = nullptr;
    t.rootContent = nullptr;
    t.addedColumn = false;
    t.items.clear();
    t.tip = nullptr;
}

// Forget the element without touching XAML (the tree was rebuilt).
static void ForgetElement(Tb& t) {
    t.elem = nullptr;
    t.parent = nullptr;
    t.rootContent = nullptr;
    t.addedColumn = false;
    t.items.clear();
    t.tip = nullptr;
}

static bool TryInject(Tb& t) {
    if (!t.wnd || !IsWindow(t.wnd)) return false;

    auto xamlRoot = GetTaskbarXamlRoot(t.wnd);
    if (!xamlRoot) return false;
    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    if (!content) return false;

    const std::wstring& pos = g_cfg.position;
    bool edge = pos == L"taskbar_left_edge";
    FrameworkElement trayFrame{nullptr};
    if (!edge) {
        trayFrame = FindChildByName(content, L"SystemTrayFrameGrid");
        // A second taskbar may have no system tray: put the monitor at its left edge instead.
        if (!trayFrame && !t.primary) edge = true;
    }

    if (!edge) {
        if (!trayFrame) return false;
        auto trayGrid = trayFrame.try_as<Grid>();
        auto trayStack = trayFrame.try_as<StackPanel>();
        if (!trayGrid && !trayStack) return false;
        Panel tray = trayGrid ? Panel(trayGrid) : Panel(trayStack);

        auto anchorOf = [&](const wchar_t* name) {
            return TrayDirectChild(tray, FindChildByName(tray, name));
        };
        auto clock = [&] {
            auto cl = anchorOf(L"NotificationCenterButton");
            return cl ? cl : anchorOf(L"ClockButton");
        };

        int slot = -1;
        if (pos == L"tray_left") {
            slot = 0;
        } else if (pos == L"tray_before_clock") {
            slot = TraySlot(tray, clock(), false);
        } else if (pos == L"tray_after_clock") {
            // "after the clock" = just before the Show Desktop button
            slot = TraySlot(tray, anchorOf(L"ShowDesktopStack"), false);
            if (slot < 0) slot = TraySlot(tray, clock(), true);
        }
        if (slot < 0) slot = 0;  // the anchor isn't on this build: start of the tray

        BuildUi(t, g_cfg);
        if (trayGrid) {
            InsertGridColumn(trayGrid, slot);
            Grid::SetColumn(t.elem, slot);
            trayGrid.Children().Append(t.elem);
            t.addedColumn = true;
        } else {
            int n = (int)trayStack.Children().Size();
            if (slot < 0 || slot > n) slot = n;
            if (slot >= n) trayStack.Children().Append(t.elem);
            else trayStack.Children().InsertAt((uint32_t)slot, t.elem);
            t.addedColumn = false;
        }
        t.parent = tray;
    } else {
        auto rootGrid = FindTaskbarRootGrid(content);
        if (!rootGrid) return false;
        BuildUi(t, g_cfg);
        Grid::SetColumn(t.elem, 0);
        Canvas::SetZIndex(t.elem, 1);
        rootGrid.Children().Append(t.elem);
        t.parent = rootGrid;
        t.addedColumn = false;
    }
    t.rootContent = content;
    t.tipText.clear();
    UpdateValues(t);
    Wh_Log(L"TrafficMonitor placed (%s)", pos.c_str());
    return true;
}

static bool StillPlaced(Tb& t) {
    if (!t.elem) return false;
    try {
        return t.elem.XamlRoot() && VisualTreeHelper::GetParent(t.elem);
    } catch (...) {
        return false;
    }
}


// ---- Color profiles that follow Windows' dark / light theme (like the original app) ----

static bool IsTaskbarLightTheme() {
    DWORD v = 0, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &v, &sz) != ERROR_SUCCESS)
        return false;
    return v != 0;
}

static void ApplyRuntimeSettings(const Settings& c);
static void ResetAllTaskbars();

// On the taskbar thread. When the theme changed (or on the first check) and the automatic
// switch is on, the profile for that theme is applied and kept.
static int g_lastThemeLight = -1;
static void CheckThemeProfile() {
    const bool light = IsTaskbarLightTheme();
    if (g_lastThemeLight == (int)light) return;
    g_lastThemeLight = light ? 1 : 0;
    SMap cur = SnapshotVals();
    if (!MBool(cur, L"profileAuto")) return;
    const std::wstring name = MGet(cur, light ? L"profileLight" : L"profileDark");
    if (name.empty()) return;
    for (const Profile& p : LoadProfiles(cur)) {
        if (p.name != name) continue;
        {
            std::lock_guard<std::mutex> lk(g_cfgMtx);
            ApplyProfileData(g_vals, p.data);
        }
        SMap saved;
        {
            std::lock_guard<std::mutex> lk(g_cfgMtx);
            saved = g_saved;
        }
        ApplyProfileData(saved, p.data);
        SaveStore(saved);
        g_cfg = BuildSettings(SnapshotVals());
        ApplyRuntimeSettings(g_cfg);
        ResetAllTaskbars();  // the next tick builds them again
        xs::NotifyRefresh();
        Wh_Log(L"Theme changed: profile \"%s\" applied", name.c_str());
        return;
    }
}

static void OnTick();

static void TickOne(Tb& t) {  // on the thread of t.wnd
    t.tick++;
    if (t.elem && !StillPlaced(t)) ForgetElement(t);

    // Every ~3 s make sure the taskbar tree is still the one we're in.
    if (t.elem && t.tick % 12 == 0) {
        auto xamlRoot = GetTaskbarXamlRoot(t.wnd);
        auto content = xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
        if (content && t.rootContent && content != t.rootContent) {
            Wh_Log(L"Taskbar tree changed, re-injecting");
            ForgetElement(t);
        }
    }

    if (!t.elem) {
        if (t.tick <= 80 || t.tick % 8 == 0) TryInject(t);
    } else {
        UpdateValues(t);
    }
}

// Runs fn(t) on the thread that owns t's window (inline when it is the current one).
template <typename F>
static void OnTbThread(Tb* t, F fn, bool wait = false) {
    struct Call {
        Tb* t;
        F* fn;
    } call{t, &fn};
    RunFromWindowThread(
        t->wnd,
        [](void* p) {
            auto* c = static_cast<Call*>(p);
            try {
                (*c->fn)(*c->t);
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"Taskbar task error: %08X", (unsigned)e.code().value);
            } catch (...) {
                Wh_Log(L"Taskbar task: exception ignored");
            }
        },
        &call, wait);
}

static void ResetAllTaskbars() {
    g_tick = 0;
    for (Tb* t : std::vector<Tb*>(g_tbs)) {
        OnTbThread(t, [](Tb& x) {
            RemoveElement(x);
            x.tick = 0;
        });
    }
}

static void CollectTaskbarWindows(HWND& primary, std::vector<HWND>& secondary) {
    struct Ctx {
        HWND* primary;
        std::vector<HWND>* secondary;
    } ctx{&primary, &secondary};
    EnumWindows(
        [](HWND h, LPARAM lp) CALLBACK -> BOOL {
            auto* c = reinterpret_cast<Ctx*>(lp);
            DWORD pid = 0;
            wchar_t cls[64] = {};
            if (GetWindowThreadProcessId(h, &pid) && pid == GetCurrentProcessId() &&
                GetClassNameW(h, cls, ARRAYSIZE(cls))) {
                if (_wcsicmp(cls, L"Shell_TrayWnd") == 0) *c->primary = h;
                else if (_wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) c->secondary->push_back(h);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));
}

// Creates / drops one Tb per taskbar that should carry the monitor ("Show on" setting).
static void SyncTaskbars() {
    HWND prim = nullptr;
    std::vector<HWND> sec;
    CollectTaskbarWindows(prim, sec);
    if (prim) g_taskbarWnd = prim;

    std::vector<HWND> want;
    const std::wstring& mode = g_cfg.monitor;
    if (mode == L"secondary" && !sec.empty()) {
        want = sec;
    } else {  // primary, both, or "second monitor" with only one monitor connected
        if (prim) want.push_back(prim);
        if (mode == L"both") want.insert(want.end(), sec.begin(), sec.end());
    }

    // Drop the ones that are no longer wanted (or whose window is gone).
    for (size_t i = 0; i < g_tbs.size();) {
        Tb* t = g_tbs[i];
        const bool keep = IsWindow(t->wnd) && std::find(want.begin(), want.end(), t->wnd) != want.end();
        if (keep) {
            i++;
            continue;
        }
        if (IsWindow(t->wnd)) OnTbThread(t, [](Tb& x) { RemoveElement(x); });
        else ForgetElement(*t);
        g_tbs.erase(g_tbs.begin() + i);
        if (!t->elem) delete t;  // otherwise its thread did not answer: leak instead of releasing XAML here
    }
    // Add the missing ones.
    for (HWND h : want) {
        bool have = false;
        for (Tb* t : g_tbs)
            if (t->wnd == h) have = true;
        if (have) continue;
        Tb* t = new Tb();
        t->wnd = h;
        t->primary = h == prim;
        g_tbs.push_back(t);
        Wh_Log(L"Taskbar added (%s)", t->primary ? L"first monitor" : L"second monitor");
    }
}

static void OnTick() {
    if (g_unloading) return;
    try {
        g_tick++;
        if (g_tick % 8 == 1) SyncTaskbars();
        if (g_tick % 12 == 6 || g_lastThemeLight < 0) CheckThemeProfile();
        for (Tb* t : std::vector<Tb*>(g_tbs)) {
            if (g_unloading) return;
            OnTbThread(t, [](Tb& x) { TickOne(x); });
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"OnTick error: %08X", (unsigned)e.code().value);
    } catch (...) {
        Wh_Log(L"OnTick: exception ignored");
    }
}

static void StopTimer() {
    if (!g_timer) return;
    try {
        g_timer.Stop();
        if (g_tickToken.value) g_timer.Tick(g_tickToken);
    } catch (...) {
    }
    g_tickToken = {};
    g_timer = nullptr;
}

// Returns false (and leaves no timer) when XAML is not ready yet, e.g. while Explorer is still
// starting up: the caller retries later instead of letting the exception reach Explorer.
static bool StartTimer() {
    StopTimer();
    try {
        g_timer = DispatcherTimer();
        g_timer.Interval(winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(250)});
        g_tickToken = g_timer.Tick([](winrt::Windows::Foundation::IInspectable const&,
                                      winrt::Windows::Foundation::IInspectable const&) { OnTick(); });
        g_timer.Start();
        return true;
    } catch (...) {
        Wh_Log(L"StartTimer: XAML not ready yet");
        g_tickToken = {};
        g_timer = nullptr;
        return false;
    }
}

static void ApplyRuntimeSettings(const Settings& c) {
    g_intervalMs = c.intervalMs;
    g_wantGpu = c.show[I_GPU];
    {
        std::lock_guard<std::mutex> lk(g_ifaceMtx);
        g_iface = c.iface;
    }
}

// ---- entry points that run on the taskbar thread ----

static void StartOnThread() {
    g_tick = 0;
    g_lastThemeLight = -1;
    if (!StartTimer()) return;
    try {
        SyncTaskbars();
    } catch (...) {
    }
}

static void ReloadOnThread(void* p) {
    std::unique_ptr<Settings> s(static_cast<Settings*>(p));
    g_cfg = *s;
    ApplyRuntimeSettings(g_cfg);
    ResetAllTaskbars();  // the next tick builds them again with the new settings
}

static void StopOnThread() {
    StopTimer();
    for (Tb* t : std::vector<Tb*>(g_tbs))
        if (IsWindow(t->wnd)) OnTbThread(t, [](Tb& x) { RemoveElement(x); }, true);
    // The Tb structs are not deleted: if a taskbar thread could not run the removal, its XAML
    // objects must not be released from here. They are empty (or intentionally leaked).
    g_tbs.clear();
}

// Explorer rebuilt the taskbar: everything we held in the first taskbar's tree is dead.
static void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (g_unloading) return;
    // Explorer is still starting up: the init thread starts the monitor once XAML is ready.
    if (!g_timer) return;
    Wh_Log(L"TrayUI::StartTaskbar called, re-initializing");
    HWND h = FindTaskbarWnd();
    if (!h) return;
    g_taskbarWnd = h;
    for (Tb* t : g_tbs)
        if (t->primary) ForgetElement(*t);
    StartOnThread();
}

static bool HookTaskbarDllSymbols() {
    HMODULE h = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!h) return false;
    // One HookSymbols call for the whole module: every extra call invalidates the symbol cache and
    // slows down Explorer's startup. The secondary-taskbar symbols are only needed for the monitor
    // on a second taskbar, so they are optional: if they are missing the first taskbar still works.
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &Std_Ref_Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable, nullptr, true},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original, nullptr, true},
    };
    if (!WindhawkUtils::HookSymbols(h, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) return false;

    if (!CSecondaryTaskBand_ITaskListWndSite_vftable || !CSecondaryTaskBand_GetTaskbarHost_Original)
        Wh_Log(L"CSecondaryTaskBand symbols not found: the monitor can't be shown on a second taskbar");
    return true;
}

// Sends the current values (the panel's live preview included) to the taskbar.
static void PushSettingsToTaskbar() {
    auto* s = new Settings(BuildSettings(SnapshotVals()));
    HWND h = FindTaskbarWnd();
    if (!h) h = g_taskbarWnd;
    if (!h || !IsWindow(h)) {
        delete s;
        return;
    }
    if (!RunFromWindowThread(h, [](void* p) { ReloadOnThread(p); }, s)) delete s;
}

// ###########################################################################
// Settings panel: a WinUI-style window (XAML Islands on its own thread), the
// same recipe as Taskbar Fluent Media Player's settings window. Sections follow
// the original TrafficMonitor app: Monitor, Appearance, Colors, General.
// ###########################################################################
namespace xs {

namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
using winrt::hstring;
using winrt::Windows::Foundation::IInspectable;

[[clang::no_destroy]] static wuxh::WindowsXamlManager s_mgr{nullptr};
[[clang::no_destroy]] static wuxh::DesktopWindowXamlSource s_src{nullptr};
[[clang::no_destroy]] static wuxc::Grid s_root{nullptr};
[[clang::no_destroy]] static wuxc::StackPanel s_pagePanel{nullptr};
[[clang::no_destroy]] static wuxc::ScrollViewer s_pageScroll{nullptr};
[[clang::no_destroy]] static wuxc::StackPanel s_navItems{nullptr};
[[clang::no_destroy]] static wux::DispatcherTimer s_applyTimer{nullptr};
[[clang::no_destroy]] static wuxc::Button s_saveBtn{nullptr};
[[clang::no_destroy]] static wuxc::ControlTemplate s_tplBtn[2][2] = {{nullptr, nullptr}, {nullptr, nullptr}};  // [subtle][unused]
[[clang::no_destroy]] static wuxc::ControlTemplate s_tplText{nullptr};
[[clang::no_destroy]] static wuxm::SolidColorBrush s_accent{nullptr};
[[clang::no_destroy]] static wuxm::SolidColorBrush s_accentHover{nullptr};
[[clang::no_destroy]] static wuxm::SolidColorBrush s_accentPressed{nullptr};
[[clang::no_destroy]] static wuxm::SolidColorBrush s_accentFg{nullptr};

static HWND s_hwnd = nullptr, s_island = nullptr;
static HANDLE s_thread = nullptr;
static DWORD s_threadId = 0;
static std::mutex s_launchMtx;
static int s_sel = 0;
static std::atomic<int> s_reqPage{-1};  // tab asked for by Show(), consumed by the panel
static bool s_light = false;
static std::wstring s_profSel, s_profName;  // Colors page: chosen profile and name box
static wchar_t s_class[64] = {};
static HINSTANCE s_module = nullptr;
static bool s_classRegistered = false;
static HBRUSH s_eraseLight = nullptr, s_eraseDark = nullptr;
static const UINT WM_XS_REFRESH = WM_APP + 7;

static HINSTANCE ThisModule() {
    HMODULE hm = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&ThisModule), &hm);
    return hm;
}

static bool IsSystemLight() {
    DWORD v = 0, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_DWORD, nullptr, &v, &sz) != ERROR_SUCCESS)
        return false;
    return v != 0;
}

static wuxm::SolidColorBrush Br(BYTE a, BYTE r, BYTE g, BYTE b) {
    return wuxm::SolidColorBrush(Color{a, r, g, b});
}
static wuxm::SolidColorBrush BrC(Color c) { return wuxm::SolidColorBrush(c); }

static std::wstring HexOf(Color c) {
    wchar_t b[10];
    _snwprintf(b, 10, L"#%02x%02x%02x", c.R, c.G, c.B);
    return b;
}
static std::wstring XamlColor(Color c) {
    wchar_t b[12];
    _snwprintf(b, 12, L"#%02X%02X%02X%02X", c.A, c.R, c.G, c.B);
    return b;
}
static Color Shade(Color c, double delta) {
    auto ch = [delta](int v) -> BYTE {
        double t = delta >= 0 ? v + (255 - v) * delta : v * (1.0 + delta);
        return (BYTE)std::clamp(t, 0.0, 255.0);
    };
    return Color{255, ch(c.R), ch(c.G), ch(c.B)};
}

static void RefreshAccent() {
    Color c{255, 0, 120, 212};
    try {
        winrt::Windows::UI::ViewManagement::UISettings ui;
        c = ui.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Accent);
        // Like Windows 11 itself: controls use Light 2 of the accent in the dark theme and Dark 1 in the
        // light one. With a dark system accent (e.g. dark gray) the plain accent made the switches
        // and the Save button almost invisible.
        c = ui.GetColorValue(s_light ? winrt::Windows::UI::ViewManagement::UIColorType::AccentDark1
                                     : winrt::Windows::UI::ViewManagement::UIColorType::AccentLight2);
    } catch (...) {
    }
    {
        const double l0 = 0.299 * c.R + 0.587 * c.G + 0.114 * c.B;
        if (!s_light && l0 < 110.0) c = Shade(c, 0.45);  // still too dark on a dark panel: lighten it
    }
    s_accent = BrC(c);
    s_accentHover = BrC(Shade(c, -0.12));
    s_accentPressed = BrC(Shade(c, -0.24));
    const double lum = 0.299 * c.R + 0.587 * c.G + 0.114 * c.B;
    s_accentFg = lum > 150.0 ? Br(255, 0, 0, 0) : Br(255, 255, 255, 255);
}

struct FColors {
    Color stroke, strongStroke, fill, fillHover, fillPressed, subtleHover, subtlePressed, card, cardStroke;
};
static const FColors& F() {
    static const FColors dark{Color{0x12, 255, 255, 255}, Color{0x8B, 255, 255, 255}, Color{0x0F, 255, 255, 255},
                              Color{0x15, 255, 255, 255}, Color{0x08, 255, 255, 255}, Color{0x0F, 255, 255, 255},
                              Color{0x0A, 255, 255, 255}, Color{0x0D, 255, 255, 255}, Color{0x18, 255, 255, 255}};
    static const FColors light{Color{0x0F, 0, 0, 0},       Color{0x72, 0, 0, 0},       Color{0xB3, 255, 255, 255},
                               Color{0x80, 0xF9, 0xF9, 0xF9}, Color{0x4D, 0xF9, 0xF9, 0xF9}, Color{0x09, 0, 0, 0},
                               Color{0x06, 0, 0, 0},       Color{0xB3, 255, 255, 255}, Color{0x14, 0, 0, 0}};
    return s_light ? light : dark;
}

static std::wstring Replace(std::wstring s, const std::wstring& from, const std::wstring& to) {
    for (size_t i = s.find(from); i != std::wstring::npos; i = s.find(from, i + to.size())) s.replace(i, from.size(), to);
    return s;
}
static wuxc::ControlTemplate LoadTemplate(const std::wstring& xaml) {
    try {
        return wux::Markup::XamlReader::Load(hstring{xaml}).as<wuxc::ControlTemplate>();
    } catch (...) {
        return nullptr;
    }
}

static const wchar_t* const kNs =
    L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
    L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\"";

static wuxc::ControlTemplate ButtonTemplate(const std::wstring& hover, const std::wstring& pressed) {
    std::wstring x =
        LR"(<ControlTemplate NS TargetType="Button"><Grid Background="Transparent"><VisualStateManager.VisualStateGroups><VisualStateGroup x:Name="CommonStates"><VisualState x:Name="Normal"/><VisualState x:Name="PointerOver"><VisualState.Setters><Setter Target="Bd.Background" Value="HOVER"/></VisualState.Setters></VisualState><VisualState x:Name="Pressed"><VisualState.Setters><Setter Target="Bd.Background" Value="PRESSED"/><Setter Target="Cp.Opacity" Value="0.79"/></VisualState.Setters></VisualState><VisualState x:Name="Disabled"><VisualState.Setters><Setter Target="Cp.Opacity" Value="0.36"/></VisualState.Setters></VisualState></VisualStateGroup></VisualStateManager.VisualStateGroups><Border x:Name="Bd" Background="{TemplateBinding Background}" BorderBrush="{TemplateBinding BorderBrush}" BorderThickness="{TemplateBinding BorderThickness}" CornerRadius="{TemplateBinding CornerRadius}"><ContentPresenter x:Name="Cp" Margin="{TemplateBinding Padding}" HorizontalAlignment="{TemplateBinding HorizontalContentAlignment}" VerticalAlignment="{TemplateBinding VerticalContentAlignment}" Foreground="{TemplateBinding Foreground}"/></Border></Grid></ControlTemplate>)";
    x = Replace(x, L"NS", kNs);
    x = Replace(x, L"HOVER", hover);
    x = Replace(x, L"PRESSED", pressed);
    return LoadTemplate(x);
}

enum class Look { Standard, Subtle, Accent };

static void StyleButton(wuxc::Button const& b, Look look = Look::Standard) {
    const auto& f = F();
    if (look == Look::Accent) {
        if (!s_accent) RefreshAccent();
        auto tpl = ButtonTemplate(XamlColor(s_accentHover.Color()), XamlColor(s_accentPressed.Color()));
        if (tpl) b.Template(tpl);
        b.Background(s_accent);
        b.Foreground(s_accentFg);
        b.BorderThickness(wux::Thickness{0, 0, 0, 0});
        b.Padding(wux::Thickness{11, 6, 11, 7});
    } else {
        const bool subtle = look == Look::Subtle;
        auto& tpl = s_tplBtn[subtle ? 1 : 0][0];
        if (!tpl) tpl = ButtonTemplate(XamlColor(subtle ? f.subtleHover : f.fillHover),
                                       XamlColor(subtle ? f.subtlePressed : f.fillPressed));
        if (tpl) b.Template(tpl);
        if (subtle) {
            b.Background(Br(0, 0, 0, 0));
            b.BorderThickness(wux::Thickness{0, 0, 0, 0});
        } else {
            b.Background(BrC(f.fill));
            b.BorderBrush(BrC(f.stroke));
            b.BorderThickness(wux::Thickness{1, 1, 1, 1});
        }
    }
    b.CornerRadius(wux::CornerRadius{4, 4, 4, 4});
    b.HorizontalContentAlignment(wux::HorizontalAlignment::Center);
    b.VerticalContentAlignment(wux::VerticalAlignment::Center);
}

static void StyleTextBox(wuxc::TextBox const& tb) {
    const auto& f = F();
    if (!s_tplText) {
        std::wstring x =
            LR"(<ControlTemplate NS TargetType="TextBox"><Border Background="{TemplateBinding Background}" BorderBrush="{TemplateBinding BorderBrush}" BorderThickness="{TemplateBinding BorderThickness}" CornerRadius="4"><ScrollViewer x:Name="ContentElement" Margin="{TemplateBinding Padding}" HorizontalScrollMode="Disabled" VerticalScrollMode="Disabled" HorizontalScrollBarVisibility="Hidden" VerticalScrollBarVisibility="Hidden" ZoomMode="Disabled" IsTabStop="False" VerticalAlignment="Center"/></Border></ControlTemplate>)";
        s_tplText = LoadTemplate(Replace(x, L"NS", kNs));
    }
    if (s_tplText) tb.Template(s_tplText);
    tb.MinHeight(32);
    tb.Padding(wux::Thickness{10, 0, 8, 0});
    tb.Background(BrC(f.fill));
    tb.BorderBrush(BrC(f.strongStroke));
    tb.BorderThickness(wux::Thickness{1, 1, 1, 1});
    tb.GotFocus([](IInspectable const& s, wux::RoutedEventArgs const&) {
        auto t = s.as<wuxc::TextBox>();
        t.BorderBrush(s_accent ? s_accent : Br(255, 0, 120, 212));
        t.BorderThickness(wux::Thickness{1, 1, 1, 2});
    });
    tb.LostFocus([](IInspectable const& s, wux::RoutedEventArgs const&) {
        auto t = s.as<wuxc::TextBox>();
        t.BorderBrush(BrC(F().strongStroke));
        t.BorderThickness(wux::Thickness{1, 1, 1, 1});
    });
}

// ---- Editing: values live in g_vals, the taskbar follows after a short delay ----

static void ScheduleApply() {
    if (!s_applyTimer) return;
    s_applyTimer.Stop();
    s_applyTimer.Start();
}
static void SetKey(const std::wstring& k, const std::wstring& v) {
    ValSet(k, v);
    ScheduleApply();
}
static std::wstring L2(const wchar_t* es, const wchar_t* en) { return TR(es, en); }

static void RebuildPage();
static void RebuildAll();
static void RefreshPanelBackdrop();

// ---- Controls ----

static wuxc::TextBlock Text(const std::wstring& t, double size = 14, double opacity = 1.0, bool semibold = false) {
    wuxc::TextBlock tb;
    tb.Text(hstring{t});
    tb.FontSize(size);
    tb.Opacity(opacity);
    tb.TextWrapping(wux::TextWrapping::Wrap);
    if (semibold) tb.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    return tb;
}

static wuxc::FontIcon Glyph(const wchar_t* g, double size = 16) {
    wuxc::FontIcon i;
    i.Glyph(hstring{g});
    i.FontSize(size);
    i.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    return i;
}

static wuxc::Button TextButton(const std::wstring& label, std::function<void()> on, Look look = Look::Standard) {
    wuxc::Button b;
    StyleButton(b, look);
    wuxc::TextBlock t;
    t.Text(hstring{label});
    t.TextWrapping(wux::TextWrapping::Wrap);
    b.Content(t);
    if (look != Look::Accent) b.Padding(wux::Thickness{11, 5, 11, 6});
    b.Click([on](IInspectable const&, wux::RoutedEventArgs const&) { on(); });
    return b;
}

static wuxc::Button IconButton(const wchar_t* glyph, const std::wstring& tip, std::function<void()> on) {
    wuxc::Button b;
    StyleButton(b);
    b.Content(Glyph(glyph, 14));
    b.Padding(wux::Thickness{0, 6, 0, 6});
    b.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    wuxc::ToolTipService::SetToolTip(b, winrt::box_value(hstring{tip}));
    b.Click([on](IInspectable const&, wux::RoutedEventArgs const&) { on(); });
    return b;
}

using Choice = std::pair<std::wstring, std::wstring>;  // value, label

// A button that opens a list; `onPick(index)` runs when one is chosen.
static wuxc::Button MakeDropdown(std::vector<std::wstring> labels, int sel, std::function<void(int)> onPick,
                                 double minWidth = 200) {
    auto labelsP = std::make_shared<std::vector<std::wstring>>(std::move(labels));
    auto cur = std::make_shared<int>(labelsP->empty() ? 0 : std::clamp(sel, 0, (int)labelsP->size() - 1));

    wuxc::Grid content;
    for (auto w : {wux::GridUnitType::Star, wux::GridUnitType::Auto}) {
        wuxc::ColumnDefinition col;
        col.Width(wux::GridLength{1.0, w});
        content.ColumnDefinitions().Append(col);
    }
    wuxc::TextBlock text;
    text.Text(hstring{labelsP->empty() ? std::wstring() : (*labelsP)[*cur]});
    text.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    text.VerticalAlignment(wux::VerticalAlignment::Center);
    auto chevron = Glyph(L"\uE70D", 10);
    chevron.Opacity(0.7);
    chevron.Margin(wux::Thickness{10, 0, 0, 0});
    wuxc::Grid::SetColumn(chevron, 1);
    content.Children().Append(text);
    content.Children().Append(chevron);

    wuxc::Button btn;
    StyleButton(btn);
    btn.Content(content);
    btn.Padding(wux::Thickness{11, 5, 10, 6});
    btn.MinWidth(minWidth);
    btn.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);

    wuxc::Flyout fly;
    fly.Placement(wuxp::FlyoutPlacementMode::BottomEdgeAlignedLeft);
    wux::Style presenter(winrt::xaml_typename<wuxc::FlyoutPresenter>());
    presenter.Setters().Append(wux::Setter(wuxc::Control::PaddingProperty(), winrt::box_value(wux::Thickness{4, 4, 4, 4})));
    presenter.Setters().Append(wux::Setter(wuxc::Control::CornerRadiusProperty(), winrt::box_value(wux::CornerRadius{8, 8, 8, 8})));
    presenter.Setters().Append(wux::Setter(wux::FrameworkElement::MinWidthProperty(), winrt::box_value(0.0)));
    fly.FlyoutPresenterStyle(presenter);

    winrt::weak_ref<wuxc::Button> weakBtn = btn;
    winrt::weak_ref<wuxc::Flyout> weakFly = fly;
    winrt::weak_ref<wuxc::TextBlock> weakText = text;
    // `fly` is captured by value on purpose: the button's Click handler is what keeps the flyout
    // alive (a weak reference alone dies when this function returns, and nothing would open).
    btn.Click([=](IInspectable const&, wux::RoutedEventArgs const&) {
        auto b = weakBtn.get();
        wuxc::Flyout f = fly;
        if (!b || !f) return;
        wuxc::StackPanel list;
        list.Spacing(2);
        for (int i = 0; i < (int)labelsP->size(); i++) {
            const bool current = i == *cur;
            wuxc::Grid line;
            for (auto w : {wux::GridUnitType::Auto, wux::GridUnitType::Star}) {
                wuxc::ColumnDefinition col;
                col.Width(wux::GridLength{1.0, w});
                line.ColumnDefinitions().Append(col);
            }
            wuxc::Border pill;
            pill.Width(3);
            pill.Height(16);
            pill.Margin(wux::Thickness{0, 0, 10, 0});
            pill.CornerRadius(wux::CornerRadius{1.5, 1.5, 1.5, 1.5});
            pill.Background(current && s_accent ? s_accent : Br(0, 0, 0, 0));
            pill.VerticalAlignment(wux::VerticalAlignment::Center);
            wuxc::TextBlock label;
            label.Text(hstring{(*labelsP)[i]});
            label.TextTrimming(wux::TextTrimming::CharacterEllipsis);
            label.VerticalAlignment(wux::VerticalAlignment::Center);
            wuxc::Grid::SetColumn(label, 1);
            line.Children().Append(pill);
            line.Children().Append(label);
            wuxc::Button item;
            StyleButton(item, Look::Subtle);
            item.Content(line);
            item.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
            item.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
            item.Padding(wux::Thickness{8, 7, 12, 8});
            if (current) item.Background(s_light ? Br(12, 0, 0, 0) : Br(16, 255, 255, 255));
            item.Click([=](IInspectable const&, wux::RoutedEventArgs const&) {
                if (auto ff = weakFly.get()) ff.Hide();
                if (i == *cur) return;
                *cur = i;
                if (auto t = weakText.get()) t.Text(hstring{(*labelsP)[i]});
                onPick(i);
            });
            list.Children().Append(item);
        }
        wuxc::ScrollViewer scroll;
        scroll.Content(list);
        scroll.MaxHeight(360);
        scroll.Width(std::max(b.ActualWidth() - 8.0, 120.0));
        scroll.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
        f.Content(scroll);
        f.ShowAt(b);
    });
    return btn;
}

// A dropdown over a list of (value, label) choices tied to a setting.
static wuxc::Button ChoiceCtl(const std::wstring& key, const std::vector<Choice>& ch, double minWidth = 200,
                              std::function<void()> after = nullptr) {
    const std::wstring cur = ValGet(key);
    std::vector<std::wstring> labels;
    int sel = 0;
    for (size_t i = 0; i < ch.size(); i++) {
        labels.push_back(ch[i].second);
        if (ch[i].first == cur) sel = (int)i;
    }
    return MakeDropdown(labels, sel, [key, ch, after](int i) {
        SetKey(key, ch[i].first);
        if (after) after();
    }, minWidth);
}

static wuxc::ToggleSwitch ToggleCtl(const std::wstring& key, std::function<void(bool)> after = nullptr) {
    wuxc::ToggleSwitch ts;
    ts.IsOn(ValGet(key) == L"1");
    ts.OnContent(winrt::box_value(hstring{L""}));
    ts.OffContent(winrt::box_value(hstring{L""}));
    ts.MinWidth(0);
    ts.HorizontalAlignment(wux::HorizontalAlignment::Right);
    ts.Toggled([key, after](IInspectable const& s, wux::RoutedEventArgs const&) {
        const bool on = s.as<wuxc::ToggleSwitch>().IsOn();
        SetKey(key, on ? L"1" : L"0");
        if (after) after(on);
    });
    return ts;
}

static wuxc::StackPanel SliderCtl(const std::wstring& key, int lo, int hi, const wchar_t* suffix, int def) {
    wuxc::StackPanel sp;
    sp.Orientation(wuxc::Orientation::Horizontal);
    sp.Spacing(12);
    wuxc::Slider sl;
    sl.Minimum(lo);
    sl.Maximum(hi);
    sl.StepFrequency(1);
    sl.Width(220);
    sl.IsThumbToolTipEnabled(false);
    long v = (long)MInt(SnapshotVals(), key, def);
    sl.Value((double)std::clamp<long>(v, lo, hi));
    auto label = Text(std::to_wstring((long)sl.Value()) + suffix, 14);
    label.MinWidth(54);
    label.TextAlignment(wux::TextAlignment::Right);
    label.VerticalAlignment(wux::VerticalAlignment::Center);
    std::wstring suf = suffix;
    sl.ValueChanged([key, label, suf](IInspectable const&, wuxp::RangeBaseValueChangedEventArgs const& e) {
        long nv = (long)std::lround(e.NewValue());
        label.Text(hstring{std::to_wstring(nv) + suf});
        if (ValGet(key) != std::to_wstring(nv)) SetKey(key, std::to_wstring(nv));
    });
    sp.Children().Append(sl);
    sp.Children().Append(label);
    return sp;
}

static wuxc::TextBox TextCtl(const std::wstring& key, int maxLen, double width, std::function<void()> after = nullptr) {
    wuxc::TextBox tb;
    StyleTextBox(tb);
    tb.Text(hstring{ValGet(key)});
    tb.MaxLength(maxLen);
    tb.Width(width);
    tb.TextChanged([key, after](IInspectable const& s, wuxc::TextChangedEventArgs const&) {
        std::wstring t{s.as<wuxc::TextBox>().Text()};
        if (t != ValGet(key)) {
            SetKey(key, t);
            if (after) after();
        }
    });
    return tb;
}

// A number box: only a valid number in range is accepted.
static wuxc::TextBox NumCtl(const std::wstring& key, double lo, double hi, double width) {
    wuxc::TextBox tb;
    StyleTextBox(tb);
    tb.Text(hstring{ValGet(key)});
    tb.Width(width);
    tb.MaxLength(9);
    tb.TextChanged([key, lo, hi](IInspectable const& s, wuxc::TextChangedEventArgs const&) {
        std::wstring t{s.as<wuxc::TextBox>().Text()};
        for (auto& ch : t)
            if (ch == L',') ch = L'.';
        wchar_t* end = nullptr;
        double v = wcstod(t.c_str(), &end);
        if (t.empty() || end == t.c_str() || *end != 0 || v < lo || v > hi) return;
        if (MDbl(SnapshotVals(), key, -1) != v) SetKey(key, t);
    });
    return tb;
}

// A color swatch that opens a picker. `def` is shown while the setting is empty.
static wuxc::Button ColorCtl(const std::wstring& key, const std::wstring& def, bool allowReset = true) {
    auto current = [key, def]() {
        Color c{};
        std::wstring v = NormHex(ValGet(key));
        if (!ParseHex(v.empty() ? def : v, c)) ParseHex(L"#808080", c);
        return c;
    };
    wuxc::Border dot;
    dot.Width(48);
    dot.Height(18);
    dot.CornerRadius(wux::CornerRadius{3, 3, 3, 3});
    dot.Background(BrC(current()));
    dot.BorderBrush(BrC(F().strongStroke));
    dot.BorderThickness(wux::Thickness{1, 1, 1, 1});
    wuxc::Button btn;
    StyleButton(btn);
    btn.Content(dot);
    btn.Padding(wux::Thickness{6, 4, 6, 4});

    wuxc::Flyout fly;
    wuxc::StackPanel body;
    body.Spacing(8);
    wuxc::ColorPicker cp;
    cp.IsAlphaEnabled(false);
    cp.IsMoreButtonVisible(false);
    cp.IsHexInputVisible(true);
    cp.Color(current());
    cp.ColorChanged([key, dot](wuxc::ColorPicker const&, wuxc::ColorChangedEventArgs const& e) {
        SetKey(key, HexOf(e.NewColor()));
        dot.Background(BrC(e.NewColor()));
    });
    body.Children().Append(cp);
    if (allowReset) {
        winrt::weak_ref<wuxc::Flyout> weakFly = fly;
        body.Children().Append(TextButton(L2(L"Original", L"Default"), [key, def, dot, cp, weakFly]() {
            SetKey(key, L"");
            Color c{};
            ParseHex(def, c);
            dot.Background(BrC(c));
            if (auto f = weakFly.get()) f.Hide();
        }));
    }
    fly.Content(body);
    winrt::weak_ref<wuxc::Button> weakBtn = btn;
    btn.Click([fly, weakBtn](IInspectable const&, wux::RoutedEventArgs const&) {
        if (auto b = weakBtn.get()) fly.ShowAt(b);
    });
    return btn;
}

// ---- Rows and cards ----

static wuxc::Grid RowOf(const std::wstring& title, const std::wstring& desc, wux::FrameworkElement const& ctrl) {
    wuxc::Grid g;
    for (auto w : {wux::GridUnitType::Star, wux::GridUnitType::Auto}) {
        wuxc::ColumnDefinition col;
        col.Width(wux::GridLength{1.0, w});
        g.ColumnDefinitions().Append(col);
    }
    g.MinHeight(36);
    g.Padding(wux::Thickness{4, 3, 4, 3});
    wuxc::StackPanel left;
    left.VerticalAlignment(wux::VerticalAlignment::Center);
    left.Margin(wux::Thickness{0, 0, 16, 0});
    left.Children().Append(Text(title, 14));
    if (!desc.empty()) left.Children().Append(Text(desc, 12, 0.6));
    g.Children().Append(left);
    if (ctrl) {
        ctrl.VerticalAlignment(wux::VerticalAlignment::Center);
        if (ctrl.try_as<wuxc::ToggleSwitch>()) ctrl.Margin(wux::Thickness{0, -4, 0, -4});
        wuxc::Grid::SetColumn(ctrl, 1);
        g.Children().Append(ctrl);
    }
    return g;
}

static wuxc::StackPanel BeginCard(const std::wstring& section) {
    if (!section.empty()) {
        auto h = Text(section, 14, 1.0, true);
        h.Margin(wux::Thickness{0, 12, 0, 4});
        s_pagePanel.Children().Append(h);
    } else {
        s_pagePanel.Children().Append(wuxc::Border());
    }
    wuxc::Border card;
    card.CornerRadius(wux::CornerRadius{8, 8, 8, 8});
    card.Background(BrC(F().card));
    card.BorderBrush(BrC(F().cardStroke));
    card.BorderThickness(wux::Thickness{1, 1, 1, 1});
    card.Padding(wux::Thickness{12, 2, 12, 2});
    wuxc::StackPanel inner;
    card.Child(inner);
    s_pagePanel.Children().Append(card);
    return inner;
}

static void AddRow(wuxc::StackPanel const& card, wux::UIElement const& row) {
    if (card.Children().Size() > 0) {
        wuxc::Border line;
        line.Height(1);
        line.Background(BrC(F().stroke));
        card.Children().Append(line);
    }
    card.Children().Append(row);
}

static void AddNote(wuxc::StackPanel const& card, const std::wstring& note) {
    auto t = Text(note, 12, 0.6);
    t.Margin(wux::Thickness{4, 5, 4, 7});
    card.Children().Append(t);
}

static void ResetKeys(const std::vector<std::wstring>& keys) {
    for (auto& k : keys) {
        auto it = Defaults().find(k);
        ValSet(k, it == Defaults().end() ? L"" : it->second);
    }
    ScheduleApply();
    RebuildPage();
}

static void PageTitle(const std::wstring& t) {
    auto h = Text(t, 24, 1.0, true);
    h.Margin(wux::Thickness{0, 0, 0, 0});
    s_pagePanel.Children().Append(h);
}

// ---- Pages ----

static std::vector<Choice> NetChoices() {
    std::vector<Choice> v;
    v.push_back({L"auto", L2(L"Automática", L"Automatic")});
    std::map<std::wstring, bool> names;  // name -> is up
    NetSnapshot snap;
    if (ReadNet(snap)) {
        for (auto& kv : snap.rows) {
            if (IsVirtualName(kv.second.alias, L"")) continue;
            bool& up = names[kv.second.alias];
            up = up || kv.second.up;
        }
    }
    const std::wstring cur = ValGet(L"iface");
    if (cur != L"auto" && !cur.empty() && !names.count(cur)) v.push_back({cur, cur + L2(L" (no encontrada)", L" (not found)")});
    for (auto& kv : names)
        v.push_back({kv.first, kv.first + (kv.second ? L"" : L2(L" (desconectada)", L" (disconnected)"))});
    return v;
}

static int CALLBACK FontEnumProc(const LOGFONTW* lf, const TEXTMETRICW*, DWORD, LPARAM lp) {
    auto* set = reinterpret_cast<std::set<std::wstring>*>(lp);
    if (lf->lfFaceName[0] != L'@') set->insert(lf->lfFaceName);
    return 1;
}
static std::vector<std::wstring> InstalledFonts() {
    std::set<std::wstring> names;
    if (HDC dc = GetDC(nullptr)) {
        LOGFONTW lf{};
        lf.lfCharSet = DEFAULT_CHARSET;
        EnumFontFamiliesExW(dc, &lf, reinterpret_cast<FONTENUMPROCW>(FontEnumProc), reinterpret_cast<LPARAM>(&names), 0);
        ReleaseDC(nullptr, dc);
    }
    std::vector<std::wstring> v(names.begin(), names.end());
    std::sort(v.begin(), v.end(), [](const std::wstring& a, const std::wstring& b) { return LowerStr(a) < LowerStr(b); });
    return v;
}

// ---- A list whose rows are reordered by dragging the handle on their left ----
// Pointer events with pointer capture, so it does not depend on OLE drag and drop inside the
// XAML island. The card holds the rows at even child positions (a separator sits between them).

// Plain data only, and the card only weakly: the handlers live inside the card, so they must not
// keep it alive.
struct DragState {
    std::vector<int> order;  // items, top to bottom
    winrt::weak_ref<wuxc::StackPanel> card;
    int from = -1;      // row being dragged, -1 = none
    double startY = 0;  // pointer position (relative to the card) when the drag started
    double pitch = 39;  // row height plus separator
};

static wuxc::Grid DragRow(wuxc::StackPanel const& card, int i) {
    return card.Children().GetAt((uint32_t)(2 * i)).as<wuxc::Grid>();
}
static void DragOffset(wuxc::Grid const& row, double y) {
    if (auto tt = row.RenderTransform().try_as<wuxm::TranslateTransform>()) tt.Y(y);
}
static int DragTarget(DragState const& d, double dy) {
    return std::clamp(d.from + (int)std::lround(dy / d.pitch), 0, (int)d.order.size() - 1);
}
// The dragged row follows the pointer; the rows it passes over make room for it.
static void DragLayout(DragState const& d, wuxc::StackPanel const& card, double dy) {
    const int n = (int)d.order.size();
    const int to = DragTarget(d, dy);
    for (int i = 0; i < n; i++) {
        double off = 0;
        if (i == d.from) off = std::clamp(dy, -d.from * d.pitch, (n - 1 - d.from) * d.pitch);
        else if (d.from < to && i > d.from && i <= to) off = -d.pitch;
        else if (d.from > to && i >= to && i < d.from) off = d.pitch;
        DragOffset(DragRow(card, i), off);
    }
}
static void DragEnd(DragState const& d, wuxc::StackPanel const& card) {
    for (int i = 0; i < (int)d.order.size(); i++) {
        auto row = DragRow(card, i);
        DragOffset(row, 0);
        row.Opacity(1.0);
        wuxc::Canvas::SetZIndex(row, 0);
    }
}

static wuxc::Border DragHandle() {
    wuxc::Border h;
    h.Width(30);
    h.Margin(wux::Thickness{0, 0, 6, 0});
    h.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));  // to receive the pointer
    auto g = Text(L"\u22EE\u22EE", 16, 0.55);
    g.TextWrapping(wux::TextWrapping::NoWrap);
    g.HorizontalAlignment(wux::HorizontalAlignment::Center);
    g.VerticalAlignment(wux::VerticalAlignment::Center);
    h.Child(g);
    wuxc::ToolTipService::SetToolTip(
        h, winrt::box_value(hstring{L2(L"Arrastra para cambiar el orden", L"Drag to change the order")}));
    return h;
}

// handle | title | control, with a transform so the row can move while it is dragged.
static wuxc::Grid ReorderableRow(const std::wstring& title, wuxc::Border const& handle,
                                 wux::FrameworkElement const& ctrl) {
    wuxc::Grid g;
    for (auto w : {wux::GridUnitType::Auto, wux::GridUnitType::Star, wux::GridUnitType::Auto}) {
        wuxc::ColumnDefinition col;
        col.Width(wux::GridLength{1.0, w});
        g.ColumnDefinitions().Append(col);
    }
    g.MinHeight(36);
    g.Padding(wux::Thickness{4, 3, 4, 3});
    g.RenderTransform(wuxm::TranslateTransform());
    g.Children().Append(handle);
    auto t = Text(title, 14);
    t.VerticalAlignment(wux::VerticalAlignment::Center);
    t.Margin(wux::Thickness{0, 0, 16, 0});
    wuxc::Grid::SetColumn(t, 1);
    g.Children().Append(t);
    ctrl.VerticalAlignment(wux::VerticalAlignment::Center);
    if (ctrl.try_as<wuxc::ToggleSwitch>()) ctrl.Margin(wux::Thickness{0, -4, 0, -4});
    wuxc::Grid::SetColumn(ctrl, 2);
    g.Children().Append(ctrl);
    return g;
}

static void WireDrag(wuxc::Border const& handle, std::shared_ptr<DragState> d, int index) {
    handle.PointerPressed([d, index](IInspectable const& s, wux::Input::PointerRoutedEventArgs const& e) {
        auto card = d->card.get();
        if (!card) return;
        auto pt = e.GetCurrentPoint(card);
        if (!pt.Properties().IsLeftButtonPressed()) return;
        d->from = index;
        d->startY = pt.Position().Y;
        auto row = DragRow(card, index);
        if (row.ActualHeight() > 0) d->pitch = row.ActualHeight() + 1;
        wuxc::Canvas::SetZIndex(row, 10);
        row.Opacity(0.85);
        s.as<wux::UIElement>().CapturePointer(e.Pointer());
        e.Handled(true);
    });
    handle.PointerMoved([d](IInspectable const&, wux::Input::PointerRoutedEventArgs const& e) {
        if (d->from < 0) return;
        auto card = d->card.get();
        if (!card) return;
        DragLayout(*d, card, e.GetCurrentPoint(card).Position().Y - d->startY);
        e.Handled(true);
    });
    handle.PointerReleased([d](IInspectable const& s, wux::Input::PointerRoutedEventArgs const& e) {
        if (d->from < 0) return;
        auto card = d->card.get();
        const int from = d->from;
        const int to = card ? DragTarget(*d, e.GetCurrentPoint(card).Position().Y - d->startY) : from;
        d->from = -1;
        s.as<wux::UIElement>().ReleasePointerCapture(e.Pointer());
        e.Handled(true);
        if (!card) return;
        DragEnd(*d, card);
        if (to == from) return;
        std::vector<int> order = d->order;
        const int item = order[from];
        order.erase(order.begin() + from);
        order.insert(order.begin() + to, item);
        std::wstring v;
        for (int it : order) {
            if (!v.empty()) v += L',';
            v += kItemKey[it];
        }
        SetKey(L"order", v);
        RebuildPage();
    });
    auto cancel = [d](IInspectable const&, wux::Input::PointerRoutedEventArgs const&) {
        if (d->from < 0) return;
        d->from = -1;
        if (auto card = d->card.get()) DragEnd(*d, card);
    };
    handle.PointerCanceled(cancel);
    handle.PointerCaptureLost(cancel);
}

static void PageMonitor() {
    PageTitle(L2(L"Monitor", L"Monitor"));
    auto card = BeginCard(L2(L"Elementos mostrados", L"Shown items"));
    struct R { const wchar_t* es; const wchar_t* en; };
    static const R rows[I_COUNT] = {  // indexed by Item
        {L"Bajada (↓)", L"Download speed (↓)"}, {L"Subida (↑)", L"Upload speed (↑)"},
        {L"CPU", L"CPU"},                         {L"MEM (memoria)", L"MEM (memory)"},
        {L"GPU", L"GPU"},                         {L"Tráfico de hoy", L"Today's traffic"}};
    auto drag = std::make_shared<DragState>();
    drag->order = EffectiveOrder(SnapshotVals());
    drag->card = winrt::make_weak(card);
    for (int i = 0; i < (int)drag->order.size(); i++) {
        const int item = drag->order[i];
        const bool mem = item == I_MEM;
        auto handle = DragHandle();
        WireDrag(handle, drag, i);
        AddRow(card, ReorderableRow(L2(rows[item].es, rows[item].en), handle,
                                    ToggleCtl(kShowKey[item], [mem](bool) { if (mem) RebuildPage(); })));
    }
    AddRow(card, RowOf(L2(L"Restablecer orden", L"Reset order"), L"",
                       TextButton(L2(L"Restablecer", L"Reset"), []() { ResetKeys({L"order"}); })));
    AddNote(card, L2(L"Arrastra ⋮⋮ para cambiar el orden en la barra de tareas.",
                     L"Drag ⋮⋮ to change the order in the taskbar."));

    const bool memOn = ValGet(L"showMem") == L"1";
    card = BeginCard(L2(L"Valores de MEM", L"MEM values"));
    struct M { const wchar_t* key; const wchar_t* es; const wchar_t* en; };
    const M mems[] = {{L"memPercent", L"Porcentaje usado", L"Percent used"},
                      {L"memUsed", L"Memoria usada", L"Memory used"},
                      {L"memAvail", L"Memoria disponible", L"Memory available"}};
    for (const M& m : mems) {
        std::wstring key = m.key;
        auto tg = ToggleCtl(key, [key](bool on) {
            if (!on) {
                SMap v = SnapshotVals();
                if (MGet(v, L"memPercent") != L"1" && MGet(v, L"memUsed") != L"1" && MGet(v, L"memAvail") != L"1") {
                    SetKey(key, L"1");  // at least one value stays
                    RebuildPage();
                }
            }
        });
        tg.IsEnabled(memOn);
        AddRow(card, RowOf(L2(m.es, m.en), L"", tg));
    }
    AddNote(card, L2(L"Si eliges varios, se separan con el separador de MEM (pestaña Aspecto).",
                     L"If you pick several, they are joined with the MEM separator (Appearance tab)."));

    card = BeginCard(L2(L"Posición", L"Position"));
    AddRow(card, RowOf(L2(L"Mostrar en", L"Show on"), L"",
                       ChoiceCtl(L"monitor", {{L"primary", L2(L"Primer monitor", L"First monitor")},
                                              {L"secondary", L2(L"Segundo monitor", L"Second monitor")},
                                              {L"both", L2(L"Ambos monitores", L"Both monitors")}})));
    AddRow(card, RowOf(L2(L"Anclar a", L"Anchor to"), L"",
                       ChoiceCtl(L"position", {{L"tray_left", L2(L"Junto a la bandeja", L"Next to the tray")},
                                               {L"tray_before_clock", L2(L"Antes del reloj", L"Before the clock")},
                                               {L"tray_after_clock", L2(L"Después del reloj", L"After the clock")},
                                               {L"taskbar_left_edge", L2(L"Izquierda de la barra", L"Left edge of the taskbar")}})));
    AddNote(card, L2(L"El monitor es parte de la barra de tareas: se mueve solo al fijar o quitar iconos de la bandeja.",
                     L"The monitor is part of the taskbar: it moves by itself when tray icons appear or disappear."));

    card = BeginCard(L2(L"Red", L"Network"));
    AddRow(card, RowOf(L2(L"Conexión de red", L"Network connection"),
                       L2(L"Automática usa la conexión real que más tráfico mueve.", L"Automatic uses the busiest real connection."),
                       ChoiceCtl(L"iface", NetChoices(), 240)));
}

static void PageAppearance() {
    const bool modern = ValGet(L"skin") == L"modern";
    PageTitle(L2(L"Aspecto", L"Appearance"));
    auto card = BeginCard(L2(L"Skin y tamaño", L"Skin and size"));
    AddRow(card, RowOf(L"Skin", L"", ChoiceCtl(L"skin", {{L"classic", L2(L"Clásico", L"Classic")}, {L"modern", L2(L"Moderno", L"Modern")}}, 160,
                                               []() { RebuildPage(); })));
    AddRow(card, RowOf(modern ? L2(L"Tamaño de la skin moderna", L"Modern skin size") : L2(L"Tamaño de la skin clásica", L"Classic skin size"),
                       L2(L"100 % = tamaño base. Cada skin recuerda el suyo.", L"100 % = base size. Each skin remembers its own."),
                       SliderCtl(modern ? L"sizeModern" : L"sizeClassic", 50, 200, L" %", modern ? 95 : 100)));
    AddRow(card, RowOf(L2(L"Restablecer tamaño", L"Reset size"), L"",
                       TextButton(L2(L"Restablecer", L"Reset"), [modern]() { ResetKeys({modern ? L"sizeModern" : L"sizeClassic"}); })));

    card = BeginCard(L2(L"Espaciado y altura", L"Spacing and height"));
    AddRow(card, RowOf(L2(L"Separación entre valores", L"Space between values"), L"px", SliderCtl(L"gap", -10, 80, L" px", 5)));
    AddRow(card, RowOf(L2(L"Altura", L"Height"), L2(L"Negativo = sube, positivo = baja.", L"Negative = up, positive = down."),
                       SliderCtl(L"yoff", -30, 30, L" px", 1)));
    AddRow(card, RowOf(L2(L"Distancia a la bandeja", L"Distance to the tray"), L"px", SliderCtl(L"trayGap", 0, 80, L" px", 8)));
    AddRow(card, RowOf(L2(L"Restablecer espaciado", L"Reset spacing"), L"",
                       TextButton(L2(L"Restablecer", L"Reset"), []() { ResetKeys({L"gap", L"yoff", L"trayGap"}); })));

    card = BeginCard(L2(L"Texto", L"Text"));
    {
        std::vector<std::wstring> fonts = InstalledFonts();
        std::vector<std::wstring> labels{L2(L"(predeterminada)", L"(default)")};
        int sel = 0;
        const std::wstring cur = ValGet(L"font");
        for (size_t i = 0; i < fonts.size(); i++) {
            labels.push_back(fonts[i]);
            if (fonts[i] == cur) sel = (int)i + 1;
        }
        AddRow(card, RowOf(L2(L"Fuente", L"Font"), L"",
                           MakeDropdown(labels, sel, [fonts](int i) { SetKey(L"font", i == 0 ? L"" : fonts[i - 1]); }, 240)));
    }
    AddRow(card, RowOf(L2(L"Negrita", L"Bold"), L"", ToggleCtl(L"bold")));
    AddRow(card, RowOf(L2(L"Efecto del texto", L"Text effect"), L2(L"Sombra o contorno detrás del texto.", L"Shadow or outline behind the text."),
                       ChoiceCtl(L"effect", {{L"none", L2(L"Ninguno", L"None")}, {L"shadow", L2(L"Sombra", L"Shadow")}, {L"outline", L2(L"Contorno", L"Outline")}}, 160)));
    AddRow(card, RowOf(L2(L"Color del efecto", L"Effect color"), L"", ColorCtl(L"effectColor", L"#000000", false)));
    {
        wuxc::StackPanel sp;
        sp.Orientation(wuxc::Orientation::Horizontal);
        sp.Spacing(6);
        auto tb = TextCtl(L"memSep", 3, 64);
        sp.Children().Append(tb);
        for (const wchar_t* p : {L"/", L"·", L"|", L"-", L"•"}) {
            std::wstring pv = p;
            auto b = TextButton(pv, [tb, pv]() { tb.Text(hstring{pv}); });
            b.MinWidth(36);
            sp.Children().Append(b);
        }
        auto none = TextButton(L2(L"Ninguno", L"None"), [tb]() { tb.Text(hstring{L""}); });
        sp.Children().Append(none);
        AddRow(card, RowOf(L2(L"Separador de MEM", L"MEM separator"), L2(L"Hasta 3 caracteres.", L"Up to 3 characters."), sp));
    }
    if (!modern)
        AddRow(card, RowOf(L2(L"Etiqueta del tráfico de hoy", L"Today's traffic label"), L"", TextCtl(L"todayLabel", 12, 120)));
    AddRow(card, RowOf(L2(L"Restablecer texto", L"Reset text"), L"",
                       TextButton(L2(L"Restablecer", L"Reset"), []() {
                           ResetKeys({L"font", L"bold", L"effect", L"effectColor", L"memSep", L"todayLabel"});
                       })));

    card = BeginCard(L2(L"Unidades", L"Units"));
    AddRow(card, RowOf(L2(L"Velocidad en", L"Speed in"), L"",
                       ChoiceCtl(L"speedBits", {{L"0", L2(L"Bytes (KB/s, MB/s)", L"Bytes (KB/s, MB/s)")}, {L"1", L2(L"Bits (Kb/s, Mb/s)", L"Bits (Kb/s, Mb/s)")}}, 200)));
    AddRow(card, RowOf(L2(L"Unidad", L"Unit"), L"",
                       ChoiceCtl(L"speedUnit", {{L"auto", L2(L"Automática", L"Automatic")}, {L"k", L2(L"Siempre K (KB/s o Kb/s)", L"Always K (KB/s or Kb/s)")},
                                                {L"m", L2(L"Siempre M (MB/s o Mb/s)", L"Always M (MB/s or Mb/s)")}}, 220)));
    AddRow(card, RowOf(L2(L"Decimales", L"Decimals"), L2(L"También afectan a MEM (GB) y al tráfico de hoy.", L"They also affect MEM (GB) and today's traffic."),
                       ChoiceCtl(L"decimals", {{L"-1", L2(L"Automático (3 cifras)", L"Automatic (3 digits)")}, {L"0", L"0"}, {L"1", L"1"}, {L"2", L"2"}}, 200)));
    AddRow(card, RowOf(L2(L"Ocultar las unidades (solo el número)", L"Hide units (number only)"), L"", ToggleCtl(L"hideUnits")));
    AddRow(card, RowOf(L2(L"Restablecer unidades", L"Reset units"), L"",
                       TextButton(L2(L"Restablecer", L"Reset"), []() { ResetKeys({L"speedBits", L"speedUnit", L"decimals", L"hideUnits"}); })));
}

static const wchar_t* ItemName(int item) {
    switch (item) {
        case I_DOWN: return TR(L"Bajada", L"Download");
        case I_UP: return TR(L"Subida", L"Upload");
        case I_CPU: return L"CPU";
        case I_GPU: return L"GPU";
        case I_MEM: return L"MEM";
        default: return TR(L"Hoy", L"Today");
    }
}

static void PageColors() {
    const bool modern = ValGet(L"skin") == L"modern";
    PageTitle(L2(L"Colores", L"Colors"));

    // ---- color of each part ----
    auto card = BeginCard(L2(L"Colores de cada parte", L"Color of each part"));
    {
        wuxc::Grid head;
        for (int i = 0; i < 5; i++) {
            wuxc::ColumnDefinition col;
            col.Width(wux::GridLength{i == 0 ? 1.0 : (i == 4 ? 1.0 : 1.0), i == 0 ? wux::GridUnitType::Star : wux::GridUnitType::Auto});
            head.ColumnDefinitions().Append(col);
        }
        head.Padding(wux::Thickness{4, 8, 4, 2});
        const std::wstring names[3] = {L2(L"Flecha / etiqueta", L"Arrow / label"), L2(L"Número", L"Number"), L2(L"Unidad", L"Unit")};
        for (int r = 0; r < 3; r++) {
            auto t = Text(names[r], 12, 0.7);
            t.Width(96);
            t.TextAlignment(wux::TextAlignment::Center);
            wuxc::Grid::SetColumn(t, r + 1);
            head.Children().Append(t);
        }
        card.Children().Append(head);
    }
    for (int idx = 0; idx < I_COUNT; idx++) {
        const int item = kPanelOrder[idx];
        wuxc::Grid g;
        for (int i = 0; i < 5; i++) {
            wuxc::ColumnDefinition col;
            col.Width(wux::GridLength{1.0, i == 0 ? wux::GridUnitType::Star : wux::GridUnitType::Auto});
            g.ColumnDefinitions().Append(col);
        }
        g.Padding(wux::Thickness{4, 4, 4, 4});
        auto name = Text(ItemName(item), 14);
        name.VerticalAlignment(wux::VerticalAlignment::Center);
        g.Children().Append(name);
        for (int r = 0; r < 3; r++) {
            auto sw = ColorCtl(KCol(item, r), DefaultPartColor(modern, item, r));
            sw.Width(96);
            sw.Margin(wux::Thickness{0, 0, 0, 0});
            wuxc::Grid::SetColumn(sw, r + 1);
            g.Children().Append(sw);
        }
        auto orig = TextButton(L2(L"Original", L"Default"), [item]() {
            ResetKeys({KCol(item, 0), KCol(item, 1), KCol(item, 2)});
        });
        orig.Margin(wux::Thickness{8, 0, 0, 0});
        wuxc::Grid::SetColumn(orig, 4);
        g.Children().Append(orig);
        AddRow(card, g);
    }
    AddNote(card, L2(L"Flecha / etiqueta: ↓ ↑, CPU:, MEM: y el separador de MEM (en la skin moderna, los iconos).",
                     L"Arrow / label: ↓ ↑, CPU:, MEM: and the MEM separator (the icons in the modern skin)."));
    AddRow(card, RowOf(L2(L"Restablecer todos los colores", L"Reset all colors"), L"", TextButton(L2(L"Restablecer todo", L"Reset all"), []() {
               std::vector<std::wstring> keys;
               for (int i = 0; i < I_COUNT; i++)
                   for (int r = 0; r < 3; r++) keys.push_back(KCol(i, r));
               ResetKeys(keys);
           })));

    // ---- alerts ----
    card = BeginCard(L2(L"Alertas de color", L"Color alerts"));
    {
        auto t = Text(L2(L"Cambia el color del valor cuando supera el límite.", L"The value changes color when it goes above the limit."), 12, 0.7);
        t.Margin(wux::Thickness{4, 10, 4, 6});
        card.Children().Append(t);
    }
    const wchar_t* units[I_COUNT] = {L"MB/s", L"MB/s", L"%", L"%", L"%", L"GB"};
    for (int idx = 0; idx < I_COUNT; idx++) {
        const int item = kPanelOrder[idx];
        wuxc::Grid g;
        const double widths[5] = {0, 0, 80, 40, 0};
        for (int i = 0; i < 5; i++) {
            wuxc::ColumnDefinition col;
            col.Width(wux::GridLength{1.0, i == 0 ? wux::GridUnitType::Star : wux::GridUnitType::Auto});
            g.ColumnDefinitions().Append(col);
        }
        (void)widths;
        g.Padding(wux::Thickness{4, 4, 4, 4});
        auto name = Text(ItemName(item), 14);
        name.VerticalAlignment(wux::VerticalAlignment::Center);
        g.Children().Append(name);
        auto on = ToggleCtl(KAlert(item, L"on"));
        wuxc::Grid::SetColumn(on, 1);
        auto above = Text(L2(L"mayor que", L"above"), 12, 0.7);
        above.VerticalAlignment(wux::VerticalAlignment::Center);
        above.Margin(wux::Thickness{8, 0, 8, 0});
        wuxc::StackPanel mid;
        mid.Orientation(wuxc::Orientation::Horizontal);
        mid.VerticalAlignment(wux::VerticalAlignment::Center);
        mid.Children().Append(above);
        mid.Children().Append(NumCtl(KAlert(item, L"limit"), 0, 1e6, 84));
        auto unit = Text(units[item], 12, 0.7);
        unit.Margin(wux::Thickness{6, 0, 10, 0});
        unit.VerticalAlignment(wux::VerticalAlignment::Center);
        unit.MinWidth(34);
        mid.Children().Append(unit);
        wuxc::Grid::SetColumn(mid, 2);
        auto sw = ColorCtl(KAlert(item, L"color"), L"#ff3b30", false);
        wuxc::Grid::SetColumn(sw, 3);
        g.Children().Append(on);
        g.Children().Append(mid);
        g.Children().Append(sw);
        AddRow(card, g);
    }
    AddNote(card, L2(L"Se aplica al número y a la unidad. Hoy = tráfico del día.", L"Applies to the number and the unit. Today = traffic of the day."));
    AddRow(card, RowOf(L2(L"Restablecer alertas", L"Reset alerts"), L"", TextButton(L2(L"Restablecer alertas", L"Reset alerts"), []() {
               std::vector<std::wstring> keys;
               for (int i = 0; i < I_COUNT; i++)
                   for (const wchar_t* f : {L"on", L"limit", L"color"}) keys.push_back(KAlert(i, f));
               ResetKeys(keys);
           })));

    // ---- profiles ----
    card = BeginCard(L2(L"Perfiles de color", L"Color profiles"));
    {
        auto t = Text(L2(L"Un perfil guarda los colores de cada parte, los de las alertas y el efecto del texto. Ajústalos arriba, ponle un nombre y guárdalo.",
                         L"A profile keeps the part colors, the alert colors and the text effect. Set them above, name it and save it."), 12, 0.7);
        t.Margin(wux::Thickness{4, 10, 4, 6});
        card.Children().Append(t);
    }
    std::vector<Profile> profiles = LoadProfiles(SnapshotVals());
    std::vector<std::wstring> names;
    for (auto& p : profiles) names.push_back(p.name);
    if (std::find(names.begin(), names.end(), s_profSel) == names.end()) s_profSel = names.empty() ? L"" : names[0];
    {
        int sel = 0;
        for (size_t i = 0; i < names.size(); i++)
            if (names[i] == s_profSel) sel = (int)i;
        wuxc::StackPanel sp;
        sp.Orientation(wuxc::Orientation::Horizontal);
        sp.Spacing(8);
        sp.Children().Append(MakeDropdown(names.empty() ? std::vector<std::wstring>{L2(L"(ninguno)", L"(none)")} : names, sel,
                                          [names](int i) { if (i < (int)names.size()) { s_profSel = names[i]; s_profName = names[i]; } }, 200));
        sp.Children().Append(TextButton(L2(L"Aplicar", L"Apply"), [profiles]() {
            for (auto& p : profiles)
                if (p.name == s_profSel) {
                    { std::lock_guard<std::mutex> lk(g_cfgMtx); ApplyProfileData(g_vals, p.data); }
                    ScheduleApply();
                    RebuildPage();
                    return;
                }
        }));
        sp.Children().Append(TextButton(L2(L"Eliminar", L"Delete"), [profiles]() {
            if (s_profSel.empty()) return;
            if (MessageBoxW(s_hwnd, (L2(L"¿Eliminar el perfil «", L"Delete the profile \"") + s_profSel + L2(L"»?", L"\"?")).c_str(),
                            L2(L"Eliminar perfil", L"Delete profile").c_str(), MB_YESNO | MB_ICONQUESTION) != IDYES) return;
            std::vector<Profile> left;
            for (auto& p : profiles) if (p.name != s_profSel) left.push_back(p);
            { std::lock_guard<std::mutex> lk(g_cfgMtx);
              StoreProfiles(g_vals, left);
              for (const wchar_t* k : {L"profileDark", L"profileLight"}) if (g_vals[k] == s_profSel) g_vals[k] = L""; }
            RebuildPage();
        }));
        AddRow(card, RowOf(L2(L"Perfiles guardados", L"Saved profiles"), L"", sp));
    }
    {
        wuxc::StackPanel sp;
        sp.Orientation(wuxc::Orientation::Horizontal);
        sp.Spacing(8);
        wuxc::TextBox nb;
        StyleTextBox(nb);
        nb.Width(180);
        nb.MaxLength((int)kProfileNameMax);
        nb.Text(hstring{s_profName});
        nb.TextChanged([](IInspectable const& s, wuxc::TextChangedEventArgs const&) { s_profName = std::wstring{s.as<wuxc::TextBox>().Text()}; });
        sp.Children().Append(nb);
        sp.Children().Append(TextButton(L2(L"Guardar actuales", L"Save current"), [profiles]() {
            std::wstring name = s_profName;
            while (!name.empty() && name.back() == L' ') name.pop_back();
            if (name.empty()) {
                MessageBoxW(s_hwnd, L2(L"Escribe un nombre para el perfil.", L"Type a name for the profile.").c_str(),
                            L2(L"Perfiles de color", L"Color profiles").c_str(), MB_OK | MB_ICONWARNING);
                return;
            }
            std::vector<Profile> list = profiles;
            auto it = std::find_if(list.begin(), list.end(), [&](const Profile& p) { return p.name == name; });
            if (it != list.end()) {
                if (MessageBoxW(s_hwnd, (L2(L"Ya existe el perfil «", L"The profile \"") + name + L2(L"». ¿Reemplazarlo con los colores actuales?", L"\" exists. Replace it with the current colors?")).c_str(),
                                L2(L"Perfiles de color", L"Color profiles").c_str(), MB_YESNO | MB_ICONQUESTION) != IDYES) return;
            } else if (list.size() >= kProfileMax) {
                MessageBoxW(s_hwnd, L2(L"Máximo 30 perfiles. Elimina alguno.", L"At most 30 profiles. Delete one.").c_str(),
                            L2(L"Perfiles de color", L"Color profiles").c_str(), MB_OK | MB_ICONWARNING);
                return;
            }
            Profile np;
            np.name = name;
            np.data = CurrentProfileData(SnapshotVals());
            if (it != list.end()) *it = np; else list.push_back(np);
            { std::lock_guard<std::mutex> lk(g_cfgMtx); StoreProfiles(g_vals, list); }
            s_profSel = name;
            RebuildPage();
        }));
        AddRow(card, RowOf(L2(L"Nombre del perfil", L"Profile name"), L"", sp));
    }
    AddRow(card, RowOf(L2(L"Cambiar automáticamente cuando Windows cambie de tema", L"Switch automatically when Windows changes theme"),
                       L2(L"El tema se lee de la barra de tareas de Windows.", L"The theme is read from the Windows taskbar."), ToggleCtl(L"profileAuto")));
    {
        std::vector<Choice> ch{{L"", L2(L"(ninguno)", L"(none)")}};
        for (auto& n : names) ch.push_back({n, n});
        AddRow(card, RowOf(L2(L"Con tema oscuro usar", L"With dark theme use"), L"", ChoiceCtl(L"profileDark", ch, 200)));
        AddRow(card, RowOf(L2(L"Con tema claro usar", L"With light theme use"), L"", ChoiceCtl(L"profileLight", ch, 200)));
    }
}

// ---- Files: export / import the original app's JSON ----

static bool ReadFileUtf8(const std::wstring& path, std::wstring& out) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER sz{};
    std::string bytes;
    bool ok = GetFileSizeEx(h, &sz) && sz.QuadPart < (8 << 20);
    if (ok) {
        bytes.resize((size_t)sz.QuadPart);
        DWORD got = 0;
        ok = ReadFile(h, bytes.data(), (DWORD)bytes.size(), &got, nullptr) && got == bytes.size();
    }
    CloseHandle(h);
    if (!ok) return false;
    size_t skip = (bytes.size() >= 3 && (unsigned char)bytes[0] == 0xEF && (unsigned char)bytes[1] == 0xBB && (unsigned char)bytes[2] == 0xBF) ? 3 : 0;
    int n = MultiByteToWideChar(CP_UTF8, 0, bytes.data() + skip, (int)(bytes.size() - skip), nullptr, 0);
    out.assign((size_t)std::max(n, 0), L'\0');
    if (n > 0) MultiByteToWideChar(CP_UTF8, 0, bytes.data() + skip, (int)(bytes.size() - skip), out.data(), n);
    return true;
}

static bool WriteFileUtf8(const std::wstring& path, const std::wstring& text) {
    int n = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0, nullptr, nullptr);
    std::string bytes((size_t)std::max(n, 0), '\0');
    if (n > 0) WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), bytes.data(), n, nullptr, nullptr);
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    DWORD wrote = 0;
    bool ok = WriteFile(h, bytes.data(), (DWORD)bytes.size(), &wrote, nullptr) && wrote == bytes.size();
    CloseHandle(h);
    return ok;
}

static void DoExport() {
    wchar_t file[MAX_PATH] = L"TrafficMonitor_ajustes.json";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = s_hwnd;
    ofn.lpstrFilter = L"TrafficMonitor (*.json)\0*.json\0*.*\0*.*\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"json";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    if (!GetSaveFileNameW(&ofn)) return;
    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t when[32];
    _snwprintf(when, 32, L"%04d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    const bool ok = WriteFileUtf8(file, BuildPythonJson(SnapshotVals(), when));
    MessageBoxW(s_hwnd, ok ? L2(L"Ajustes exportados correctamente.", L"Settings exported.").c_str()
                           : L2(L"No se pudo guardar el archivo.", L"The file couldn't be saved.").c_str(),
                L2(L"Exportar ajustes", L"Export settings").c_str(), MB_OK | (ok ? MB_ICONINFORMATION : MB_ICONERROR));
}

static void DoImport() {
    wchar_t file[MAX_PATH] = L"";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = s_hwnd;
    ofn.lpstrFilter = L"TrafficMonitor (*.json)\0*.json\0*.*\0*.*\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (!GetOpenFileNameW(&ofn)) return;
    const std::wstring title = L2(L"Importar ajustes", L"Import settings");
    std::wstring text;
    JV root;
    if (!ReadFileUtf8(file, text) || !ParseJson(text, root)) {
        MessageBoxW(s_hwnd, L2(L"No se pudo leer el archivo.", L"The file couldn't be read.").c_str(), title.c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    SMap m = SnapshotVals();
    if (!ApplyPythonJson(root, m)) {
        MessageBoxW(s_hwnd, L2(L"Ese archivo no parece ser de ajustes de TrafficMonitor.", L"That file doesn't look like TrafficMonitor settings.").c_str(),
                    title.c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    if (MessageBoxW(s_hwnd, L2(L"Se reemplazarán tus ajustes actuales (colores, perfiles, tamaño, unidades…) por los del archivo.\n\n¿Continuar?",
                               L"Your current settings (colors, profiles, size, units…) will be replaced by the file's.\n\nContinue?").c_str(),
                    title.c_str(), MB_YESNO | MB_ICONQUESTION) != IDYES) return;
    {
        std::lock_guard<std::mutex> lk(g_cfgMtx);
        g_vals = m;
    }
    SaveStore(m);
    g_uiEs = ResolveUiEs(m);
    PushSettingsToTaskbar();
    RebuildAll();
}

static void DoResetAll() {
    if (MessageBoxW(s_hwnd, L2(L"Se restablecerán todos los ajustes (los perfiles de color se conservan).\n\n¿Continuar?",
                               L"All settings will be reset (color profiles are kept).\n\nContinue?").c_str(),
                    L2(L"Restablecer todo", L"Reset all").c_str(), MB_YESNO | MB_ICONQUESTION) != IDYES) return;
    SMap cur = SnapshotVals();
    SMap m = Defaults();
    for (auto& kv : cur)
        if (kv.first.compare(0, 7, L"profile") == 0 || kv.first == L"lang") m[kv.first] = kv.second;
    { std::lock_guard<std::mutex> lk(g_cfgMtx); g_vals = m; }
    g_uiEs = ResolveUiEs(m);
    PushSettingsToTaskbar();
    RebuildAll();
}

static void PageGeneral() {
    PageTitle(L2(L"General", L"General"));
    auto card = BeginCard(L2(L"Actualización", L"Update"));
    AddRow(card, RowOf(L2(L"Intervalo de actualización", L"Update interval"), L"",
                       ChoiceCtl(L"interval", {{L"500", L2(L"0,5 segundos", L"0.5 seconds")}, {L"1000", L2(L"1 segundo (normal)", L"1 second (normal)")},
                                               {L"2000", L2(L"2 segundos", L"2 seconds")}}, 200)));
    AddRow(card, RowOf(L2(L"Idioma del panel", L"Panel language"), L"",
                       ChoiceCtl(L"lang", {{L"auto", L2(L"Automático", L"Automatic")}, {L"es", L"Español"}, {L"en", L"English"}}, 200, []() {
                           g_uiEs = ResolveUiEs(SnapshotVals());
                           RebuildAll();
                       })));

    card = BeginCard(L2(L"Fondo del panel", L"Panel background"));
    {
        const bool acrylic = ValGet(L"panelAcrylic") == L"1";
        AddRow(card, RowOf(L2(L"Efecto de fondo", L"Background effect"), L"",
                           MakeDropdown({L2(L"Acrílico", L"Acrylic"), L2(L"Ninguno", L"None")}, acrylic ? 0 : 1,
                                        [](int i) {
                                            ValSet(L"panelAcrylic", i == 0 ? L"1" : L"0");
                                            RefreshPanelBackdrop();
                                            RebuildPage();
                                        }, 200)));
        // Same slider as the others, but it only changes the panel, so the taskbar is not rebuilt.
        wuxc::StackPanel sp;
        sp.Orientation(wuxc::Orientation::Horizontal);
        sp.Spacing(12);
        wuxc::Slider sl;
        sl.Minimum(20);
        sl.Maximum(100);
        sl.StepFrequency(1);
        sl.Width(220);
        sl.IsThumbToolTipEnabled(false);
        sl.IsEnabled(acrylic);
        sl.Value((double)std::clamp<long>(MInt(SnapshotVals(), L"panelAlpha", 50), 20, 100));
        auto label = Text(std::to_wstring((long)sl.Value()) + L" %", 14);
        label.MinWidth(54);
        label.TextAlignment(wux::TextAlignment::Right);
        label.VerticalAlignment(wux::VerticalAlignment::Center);
        sl.ValueChanged([label](IInspectable const&, wuxp::RangeBaseValueChangedEventArgs const& e) {
            const long nv = (long)std::lround(e.NewValue());
            label.Text(hstring{std::to_wstring(nv) + L" %"});
            if (ValGet(L"panelAlpha") != std::to_wstring(nv)) {
                ValSet(L"panelAlpha", std::to_wstring(nv));
                RefreshPanelBackdrop();
            }
        });
        sp.Children().Append(sl);
        sp.Children().Append(label);
        AddRow(card, RowOf(L2(L"Opacidad del fondo", L"Background opacity"),
                           L2(L"Más alto = más oscuro y menos transparente.", L"Higher = darker and less transparent."), sp));
    }

    card = BeginCard(L2(L"Copia de ajustes", L"Settings backup"));
    AddNote(card, L2(L"Lleva tu configuración (colores, perfiles, tamaño, unidades…) a otra PC o desde la app original de TrafficMonitor. "
                     L"Usa el mismo archivo TrafficMonitor_ajustes.json.",
                     L"Move your settings (colors, profiles, size, units…) to another PC or from the original TrafficMonitor app. "
                     L"It uses the same TrafficMonitor_ajustes.json file."));
    {
        wuxc::StackPanel sp;
        sp.Orientation(wuxc::Orientation::Horizontal);
        sp.Spacing(8);
        sp.Children().Append(TextButton(L2(L"Exportar ajustes…", L"Export settings…"), []() { DoExport(); }));
        sp.Children().Append(TextButton(L2(L"Importar ajustes…", L"Import settings…"), []() { DoImport(); }));
        auto row = RowOf(L2(L"Exportar / importar", L"Export / import"), L"", sp);
        AddRow(card, row);
    }
    AddRow(card, RowOf(L2(L"Restablecer todos los ajustes", L"Reset all settings"), L"",
                       TextButton(L2(L"Restablecer todo", L"Reset all"), []() { DoResetAll(); })));

    card = BeginCard(L2(L"Acerca de", L"About"));
    AddNote(card, L2(L"Basado en el TrafficMonitor original. Estas opciones de la app original no aplican aquí porque el monitor es parte de la barra de tareas: "
                     L"arrastrar con el mouse, ocultar con apps a pantalla completa (la barra se oculta sola), iniciar con Windows (lo hace Windhawk).",
                     L"Based on the original TrafficMonitor. These options of the original app don't apply here because the monitor is part of the taskbar: "
                     L"dragging with the mouse, hiding on full-screen apps (the taskbar hides itself), starting with Windows (Windhawk does that)."));
}

// ---- History page (list view + calendar view, like the original app) ----

static int DaysFromCivil(int y, int m, int d) {
    y -= m <= 2;
    const int era = (y >= 0 ? y : y - 399) / 400;
    const int yoe = y - era * 400;
    const int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}
static void CivilFromDays(int z, int& y, int& m, int& d) {
    z += 719468;
    const int era = (z >= 0 ? z : z - 146096) / 146097;
    const int doe = z - era * 146097;
    const int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    y = yoe + era * 400;
    const int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const int mp = (5 * doy + 2) / 153;
    d = doy - (153 * mp + 2) / 5 + 1;
    m = mp < 10 ? mp + 3 : mp - 9;
    y += (m <= 2);
}
static int WeekdaySun(int days) {  // 0 = Sunday
    const int w = (days + 4) % 7;
    return w < 0 ? w + 7 : w;
}
static int DaysInMonth(int y, int m) {
    static const int dm[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) return 29;
    return dm[m - 1];
}
static bool ParseDate(const std::wstring& s, int& y, int& m, int& d) {
    if (s.size() != 10 || s[4] != L'-' || s[7] != L'-') return false;
    for (int i : {0, 1, 2, 3, 5, 6, 8, 9})
        if (s[i] < L'0' || s[i] > L'9') return false;
    y = _wtoi(s.substr(0, 4).c_str());
    m = _wtoi(s.substr(5, 2).c_str());
    d = _wtoi(s.substr(8, 2).c_str());
    return y >= 1970 && m >= 1 && m <= 12 && d >= 1 && d <= DaysInMonth(y, m);
}
static std::wstring DateKey(int y, int m, int d) {
    wchar_t b[16];
    _snwprintf(b, 15, L"%04d-%02d-%02d", y, m, d);
    b[15] = 0;
    return b;
}
static const wchar_t* WeekdayName(int wd) {  // 0 = Sunday
    static const wchar_t* es[] = {L"dom", L"lun", L"mar", L"mié", L"jue", L"vie", L"sáb"};
    static const wchar_t* en[] = {L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"};
    return g_uiEs ? es[wd] : en[wd];
}

static constexpr uint64_t kGB = 1024ULL * 1024ULL * 1024ULL;
static const int kPeriodDays[] = {1, 7, 30, 91, 365};  // day, week, month, quarter, year

static int PeriodStart(int days, int kind) {
    if (kind == 0) return days;
    if (kind == 1) return days - WeekdaySun(days);  // weeks start on Sunday
    int y, m, d;
    CivilFromDays(days, y, m, d);
    if (kind == 2) return DaysFromCivil(y, m, 1);
    if (kind == 3) return DaysFromCivil(y, 3 * ((m - 1) / 3) + 1, 1);
    return DaysFromCivil(y, 1, 1);
}

static std::wstring PeriodLabel(int start, int kind) {
    int y, m, d;
    CivilFromDays(start, y, m, d);
    wchar_t b[64];
    if (kind == 0) {
        _snwprintf(b, 63, L"%04d/%02d/%02d (%s)", y, m, d, WeekdayName(WeekdaySun(start)));
    } else if (kind == 1) {
        int y2, m2, d2;
        CivilFromDays(start + 6, y2, m2, d2);
        _snwprintf(b, 63, L"%04d/%02d/%02d ~ %04d/%02d/%02d", y, m, d, y2, m2, d2);
    } else if (kind == 2) {
        _snwprintf(b, 63, L"%04d/%02d", y, m);
    } else if (kind == 3) {
        _snwprintf(b, 63, L"%d %c%d", y, g_uiEs ? L'T' : L'Q', (m - 1) / 3 + 1);
    } else {
        _snwprintf(b, 63, L"%d", y);
    }
    b[63] = 0;
    return b;
}

struct HistRow {
    int start;
    uint64_t up, down;
};
// Newest first.
static std::vector<HistRow> AggregateHistory(const std::map<std::wstring, DayBytes>& data, int kind) {
    std::map<int, std::pair<uint64_t, uint64_t>> acc;
    for (auto& kv : data) {
        int y, m, d;
        if (!ParseDate(kv.first, y, m, d)) continue;
        auto& a = acc[PeriodStart(DaysFromCivil(y, m, d), kind)];
        a.first += kv.second.up;
        a.second += kv.second.down;
    }
    std::vector<HistRow> rows;
    for (auto it = acc.rbegin(); it != acc.rend(); ++it) rows.push_back({it->first, it->second.first, it->second.second});
    return rows;
}

static Color Hx(uint32_t rgb) { return Color{255, (BYTE)(rgb >> 16), (BYTE)(rgb >> 8), (BYTE)rgb}; }

// Bar color of the list view: 10 GB/day = yellow, 100 GB/day = red.
static uint32_t ListLevelColor(uint64_t total, int kind) {
    const uint64_t days = (uint64_t)kPeriodDays[kind];
    if (total >= 100 * kGB * days) return 0xfb6353;
    if (total >= 10 * kGB * days) return 0xfcd53f;
    return 0x77bb4a;
}

struct CalLevel {
    uint64_t limit;  // 0 = no limit
    uint32_t color;
    const wchar_t* label;
};
static const CalLevel kCalLevels[] = {{1 * kGB, 0x19b5ea, L"0 ~ 1 GB"},
                                      {10 * kGB, 0x77bb4a, L"1 GB ~ 10 GB"},
                                      {100 * kGB, 0xfcd53f, L"10 GB ~ 100 GB"},
                                      {1024 * kGB, 0xfb6353, L"100 GB ~ 1 TB"},
                                      {0, 0xa31515, L"1 TB ~"}};
static uint32_t CalColor(uint64_t total) {
    for (const CalLevel& l : kCalLevels)
        if (l.limit == 0 || total < l.limit) return l.color;
    return 0xa31515;
}

static int s_hTab = 0;     // 0 list, 1 calendar
static int s_hView = 0;    // day, week, month, quarter, year
static int s_hLog = 1;     // 0 linear, 1 logarithmic
static int s_hRows = 60;   // rows shown in the list
static int s_calY = 0, s_calM = 0;
static std::wstring s_calSel;

static wuxc::Grid HistGrid() {
    wuxc::Grid g;
    const double w[] = {0, 84, 84, 96, 120};
    for (int i = 0; i < 5; i++) {
        wuxc::ColumnDefinition col;
        if (i == 0) {
            col.Width(wux::GridLength{1.0, wux::GridUnitType::Star});
            col.MinWidth(150);
        } else {
            col.Width(wux::GridLength{w[i], wux::GridUnitType::Pixel});
        }
        g.ColumnDefinitions().Append(col);
    }
    return g;
}

static void PutCell(wuxc::Grid const& g, int col, const std::wstring& t, bool right, double opacity = 1.0, bool semibold = false) {
    auto tb = Text(t, 13, opacity, semibold);
    tb.TextWrapping(wux::TextWrapping::NoWrap);
    tb.VerticalAlignment(wux::VerticalAlignment::Center);
    if (right) {
        tb.TextAlignment(wux::TextAlignment::Right);
        tb.HorizontalAlignment(wux::HorizontalAlignment::Right);
        tb.Margin(wux::Thickness{0, 0, 12, 0});
    }
    wuxc::Grid::SetColumn(tb, col);
    g.Children().Append(tb);
}

static wuxc::Button GlyphBtn(const wchar_t* glyph, const std::wstring& tip, std::function<void()> on) {
    auto b = IconButton(glyph, tip, std::move(on));
    b.HorizontalAlignment(wux::HorizontalAlignment::Left);
    b.Padding(wux::Thickness{12, 6, 12, 6});
    return b;
}

static void DoImportHistory() {
    wchar_t file[MAX_PATH] = L"";
    wchar_t dir[MAX_PATH] = L"";
    if (GetEnvironmentVariableW(L"APPDATA", dir, MAX_PATH)) wcscat_s(dir, L"\\TrafficMonitor");
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = s_hwnd;
    ofn.lpstrFilter = L"traffic_history (*.json)\0*.json\0*.*\0*.*\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = dir[0] ? dir : nullptr;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (!GetOpenFileNameW(&ofn)) return;
    const std::wstring title = L2(L"Importar historial", L"Import history");
    std::wstring text;
    JV root;
    if (!ReadFileUtf8(file, text) || !ParseJson(text, root) || root.t != JV::Obj) {
        MessageBoxW(s_hwnd, L2(L"No se pudo leer el archivo.", L"The file couldn't be read.").c_str(), title.c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    std::map<std::wstring, DayBytes> in;
    for (size_t i = 0; i < root.k.size() && i < root.a.size(); i++) {
        const JV& v = root.a[i];
        int y, m, d;
        if (v.t != JV::Arr || v.a.size() < 2 || v.a[0].t != JV::Num || v.a[1].t != JV::Num) continue;
        if (!ParseDate(root.k[i], y, m, d)) continue;
        DayBytes b;
        b.up = (uint64_t)std::max(0.0, v.a[0].n);
        b.down = (uint64_t)std::max(0.0, v.a[1].n);
        in[root.k[i]] = b;
    }
    if (in.empty()) {
        MessageBoxW(s_hwnd, L2(L"Ese archivo no parece ser un historial de TrafficMonitor (traffic_history.json).",
                               L"That file doesn't look like a TrafficMonitor history (traffic_history.json).").c_str(),
                    title.c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    const int changed = HistoryMerge(in);
    SaveHistory();
    wchar_t msg[256];
    _snwprintf(msg, 255, L2(L"Se leyeron %d días; %d se añadieron o se actualizaron.", L"%d days read; %d added or updated.").c_str(),
               (int)in.size(), changed);
    msg[255] = 0;
    MessageBoxW(s_hwnd, msg, title.c_str(), MB_OK | MB_ICONINFORMATION);
    RebuildPage();
}

static void DoExportHistory() {
    wchar_t file[MAX_PATH] = L"traffic_history.json";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = s_hwnd;
    ofn.lpstrFilter = L"traffic_history (*.json)\0*.json\0*.*\0*.*\0\0";
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"json";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    if (!GetSaveFileNameW(&ofn)) return;
    auto data = HistorySnapshot();
    std::wstring out = L"{\n";
    size_t n = 0;
    for (auto& kv : data) {
        out += L"  \"" + kv.first + L"\": [" + std::to_wstring(kv.second.up) + L", " + std::to_wstring(kv.second.down) + L"]";
        out += (++n < data.size()) ? L",\n" : L"\n";
    }
    out += L"}\n";
    const bool ok = WriteFileUtf8(file, out);
    MessageBoxW(s_hwnd, ok ? L2(L"Historial exportado correctamente.", L"History exported.").c_str()
                           : L2(L"No se pudo guardar el archivo.", L"The file couldn't be saved.").c_str(),
                L2(L"Exportar historial", L"Export history").c_str(), MB_OK | (ok ? MB_ICONINFORMATION : MB_ICONERROR));
}

static void HistoryListView(const std::map<std::wstring, DayBytes>& data) {
    static const wchar_t* const es[] = {L"Por día", L"Por semana", L"Por mes", L"Por trimestre", L"Por año"};
    static const wchar_t* const en[] = {L"By day", L"By week", L"By month", L"By quarter", L"By year"};
    std::vector<std::wstring> viewLabels;
    for (int i = 0; i < 5; i++) viewLabels.push_back(L2(es[i], en[i]));

    auto opts = BeginCard(L"");
    AddRow(opts, RowOf(L2(L"Tipo de vista", L"View type"), L"",
                       MakeDropdown(viewLabels, s_hView, [](int i) { s_hView = i; s_hRows = 60; RebuildPage(); }, 180)));
    AddRow(opts, RowOf(L2(L"Escala de la figura", L"Bar scale"), L"",
                       MakeDropdown({L2(L"Lineal", L"Linear"), L2(L"Logarítmica", L"Logarithmic")}, s_hLog,
                                    [](int i) { s_hLog = i; RebuildPage(); }, 180)));

    auto card = BeginCard(L"");
    {
        auto g = HistGrid();
        g.Margin(wux::Thickness{0, 8, 0, 8});
        PutCell(g, 0, L2(L"Fecha", L"Date"), false, 0.7, true);
        PutCell(g, 1, L2(L"Subida", L"Upload"), true, 0.7, true);
        PutCell(g, 2, L2(L"Bajada", L"Download"), true, 0.7, true);
        PutCell(g, 3, L2(L"Total", L"Total"), true, 0.7, true);
        PutCell(g, 4, L2(L"Figura", L"Chart"), false, 0.7, true);
        card.Children().Append(g);
        wuxc::Border line;
        line.Height(1);
        line.Background(BrC(F().stroke));
        card.Children().Append(line);
    }

    const auto rows = AggregateHistory(data, s_hView);
    if (rows.empty()) {
        auto t = Text(L2(L"Aún no hay datos.", L"No data yet."), 13, 0.6);
        t.Margin(wux::Thickness{4, 14, 4, 14});
        card.Children().Append(t);
        return;
    }
    uint64_t vmax = 1;
    for (auto& r : rows) vmax = std::max(vmax, r.up + r.down);
    const double f0 = 0.2 * (double)kGB * kPeriodDays[s_hView];  // base of the logarithmic scale
    const int shown = std::min<int>((int)rows.size(), s_hRows);
    for (int i = 0; i < shown; i++) {
        const HistRow& r = rows[i];
        const uint64_t tot = r.up + r.down;
        auto g = HistGrid();
        g.MinHeight(32);
        PutCell(g, 0, PeriodLabel(r.start, s_hView), false);
        PutCell(g, 1, FmtSizeText((double)r.up), true);
        PutCell(g, 2, FmtSizeText((double)r.down), true);
        PutCell(g, 3, FmtSizeText((double)tot), true);
        double frac;
        if (s_hLog) {
            if ((double)tot <= f0) frac = 0.0;
            else if ((double)vmax <= f0) frac = 1.0;
            else frac = std::log10((double)tot / f0) / std::log10((double)vmax / f0);
        } else {
            frac = (double)tot / (double)vmax;
        }
        wuxc::Border bar;
        bar.Width(4.0 + std::clamp(frac, 0.0, 1.0) * 104.0);
        bar.Height(10);
        bar.CornerRadius(wux::CornerRadius{3, 3, 3, 3});
        bar.Background(BrC(Hx(ListLevelColor(tot, s_hView))));
        bar.HorizontalAlignment(wux::HorizontalAlignment::Left);
        bar.VerticalAlignment(wux::VerticalAlignment::Center);
        wuxc::Grid::SetColumn(bar, 4);
        g.Children().Append(bar);
        card.Children().Append(g);
    }
    if ((int)rows.size() > shown) {
        const int left = (int)rows.size() - shown;
        auto more = TextButton(L2(L"Mostrar más", L"Show more") + L" (" + std::to_wstring(left) + L")",
                               []() { s_hRows += 100; RebuildPage(); });
        more.HorizontalAlignment(wux::HorizontalAlignment::Left);
        more.Margin(wux::Thickness{0, 8, 0, 10});
        card.Children().Append(more);
    }
}

static void HistoryCalendarView(const std::map<std::wstring, DayBytes>& data) {
    SYSTEMTIME now;
    GetLocalTime(&now);
    if (s_calY == 0) {
        s_calY = now.wYear;
        s_calM = now.wMonth;
        s_calSel = DateKey(now.wYear, now.wMonth, now.wDay);
    }
    int minY = now.wYear, maxY = now.wYear;
    for (auto& kv : data) {
        int y, m, d;
        if (!ParseDate(kv.first, y, m, d)) continue;
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
    }
    maxY += 5;
    minY = std::min(minY, s_calY);
    maxY = std::max(maxY, s_calY);

    // Month / year selectors
    auto opts = BeginCard(L"");
    {
        auto yrs = std::make_shared<std::vector<int>>();
        std::vector<std::wstring> ylabels;
        for (int y = minY; y <= maxY; y++) {
            yrs->push_back(y);
            ylabels.push_back(std::to_wstring(y));
        }
        std::vector<std::wstring> mlabels;
        for (int m = 1; m <= 12; m++) mlabels.push_back(std::to_wstring(m));

        wuxc::StackPanel bar;
        bar.Orientation(wuxc::Orientation::Horizontal);
        bar.Spacing(8);
        bar.Margin(wux::Thickness{0, 8, 0, 8});
        auto lbl = [](const std::wstring& t) {
            auto tb = Text(t, 14);
            tb.VerticalAlignment(wux::VerticalAlignment::Center);
            return tb;
        };
        bar.Children().Append(lbl(L2(L"Año:", L"Year:")));
        bar.Children().Append(MakeDropdown(ylabels, s_calY - minY, [yrs](int i) { s_calY = (*yrs)[i]; RebuildPage(); }, 90));
        bar.Children().Append(lbl(L2(L"Mes:", L"Month:")));
        bar.Children().Append(MakeDropdown(mlabels, s_calM - 1, [](int i) { s_calM = i + 1; RebuildPage(); }, 80));
        bar.Children().Append(GlyphBtn(L"\uE76B", L2(L"Mes anterior", L"Previous month"), []() {
            if (--s_calM < 1) { s_calM = 12; s_calY--; }
            RebuildPage();
        }));
        bar.Children().Append(GlyphBtn(L"\uE76C", L2(L"Mes siguiente", L"Next month"), []() {
            if (++s_calM > 12) { s_calM = 1; s_calY++; }
            RebuildPage();
        }));
        bar.Children().Append(GlyphBtn(L"\uE787", L2(L"Hoy", L"Today"), []() {
            SYSTEMTIME t;
            GetLocalTime(&t);
            s_calY = t.wYear;
            s_calM = t.wMonth;
            s_calSel = DateKey(t.wYear, t.wMonth, t.wDay);
            RebuildPage();
        }));
        opts.Children().Append(bar);
    }

    // Month grid
    auto card = BeginCard(L"");
    {
        const double headH = 30, cellH = 62;
        wuxc::Grid g;
        g.Margin(wux::Thickness{0, 10, 0, 10});
        for (int c = 0; c < 7; c++) {
            wuxc::ColumnDefinition col;
            col.Width(wux::GridLength{1.0, wux::GridUnitType::Star});
            g.ColumnDefinitions().Append(col);
        }
        for (int r = 0; r < 7; r++) {
            wuxc::RowDefinition row;
            row.Height(wux::GridLength{r == 0 ? headH : cellH, wux::GridUnitType::Pixel});
            g.RowDefinitions().Append(row);
        }
        const Color gap = s_light ? Color{255, 243, 243, 243} : Color{255, 32, 32, 32};
        for (int c = 0; c < 7; c++) {
            wuxc::Border head;
            head.Background(BrC(Hx((c == 0 || c == 6) ? 0xd9534f : (c % 2 ? 0x0b69b5 : 0x1c7fd1))));
            head.BorderBrush(BrC(gap));
            head.BorderThickness(wux::Thickness{1, 1, 1, 1});
            auto t = Text(WeekdayName(c), 13, 1.0, true);
            t.Foreground(Br(255, 255, 255, 255));
            t.HorizontalAlignment(wux::HorizontalAlignment::Center);
            t.VerticalAlignment(wux::VerticalAlignment::Center);
            head.Child(t);
            wuxc::Grid::SetColumn(head, c);
            g.Children().Append(head);
        }
        const int first = DaysFromCivil(s_calY, s_calM, 1);
        const int wd0 = WeekdaySun(first);
        const int dim = DaysInMonth(s_calY, s_calM);
        for (int idx = 0; idx < 42; idx++) {
            const int day = idx - wd0 + 1;
            const int r = idx / 7, c = idx % 7;
            const bool weekend = c == 0 || c == 6;
            uint32_t bg, fg;
            if (s_light) {
                if (weekend) { bg = (r % 2 == 0) ? 0xfbe9e9 : 0xf6dede; fg = 0xb03a3a; }
                else { bg = (c % 2) ? 0xe3f0fc : 0xd6e9fa; fg = 0x0b3d6b; }
            } else {
                if (weekend) { bg = (r % 2 == 0) ? 0x3a2528 : 0x33211f; fg = 0xff9a9a; }
                else { bg = (c % 2) ? 0x1d2b38 : 0x22313f; fg = 0x9cc9f2; }
            }
            wuxc::Border cell;
            cell.Background(BrC(Hx(bg)));
            cell.BorderBrush(BrC(gap));
            cell.BorderThickness(wux::Thickness{1, 1, 1, 1});
            wuxc::Grid::SetRow(cell, r + 1);
            wuxc::Grid::SetColumn(cell, c);
            if (day >= 1 && day <= dim) {
                const std::wstring key = DateKey(s_calY, s_calM, day);
                wuxc::StackPanel sp;
                sp.HorizontalAlignment(wux::HorizontalAlignment::Center);
                sp.VerticalAlignment(wux::VerticalAlignment::Center);
                auto dn = Text(std::to_wstring(day), 14);
                dn.Foreground(BrC(Hx(fg)));
                dn.HorizontalAlignment(wux::HorizontalAlignment::Center);
                sp.Children().Append(dn);
                auto it = data.find(key);
                if (it != data.end()) {
                    const uint64_t tot = it->second.up + it->second.down;
                    auto tt = Text(FmtSizeText((double)tot), 11);
                    tt.Foreground(BrC(s_light ? Color{255, 51, 51, 51} : Color{255, 208, 208, 208}));
                    tt.TextWrapping(wux::TextWrapping::NoWrap);
                    tt.HorizontalAlignment(wux::HorizontalAlignment::Center);
                    sp.Children().Append(tt);
                    wuxc::Border dot;
                    dot.Width(10);
                    dot.Height(9);
                    dot.Margin(wux::Thickness{0, 2, 0, 0});
                    dot.Background(BrC(Hx(CalColor(tot))));
                    dot.HorizontalAlignment(wux::HorizontalAlignment::Center);
                    sp.Children().Append(dot);
                }
                cell.Child(sp);
                if (key == s_calSel) {
                    cell.BorderBrush(BrC(Hx(0xd9534f)));
                    cell.BorderThickness(wux::Thickness{2, 2, 2, 2});
                }
                cell.Tapped([key](IInspectable const&, wux::Input::TappedRoutedEventArgs const&) {
                    s_calSel = key;
                    RebuildPage();
                });
            }
            g.Children().Append(cell);
        }
        card.Children().Append(g);

        // Legend
        wuxc::StackPanel legend;
        legend.Orientation(wuxc::Orientation::Horizontal);
        legend.Spacing(16);
        legend.Margin(wux::Thickness{4, 0, 4, 10});
        for (const CalLevel& l : kCalLevels) {
            wuxc::StackPanel item;
            item.Orientation(wuxc::Orientation::Horizontal);
            item.Spacing(6);
            wuxc::Border sw;
            sw.Width(12);
            sw.Height(12);
            sw.CornerRadius(wux::CornerRadius{2, 2, 2, 2});
            sw.Background(BrC(Hx(l.color)));
            sw.VerticalAlignment(wux::VerticalAlignment::Center);
            item.Children().Append(sw);
            auto t = Text(l.label, 12, 0.8);
            t.VerticalAlignment(wux::VerticalAlignment::Center);
            t.TextWrapping(wux::TextWrapping::NoWrap);
            item.Children().Append(t);
            legend.Children().Append(item);
        }
        card.Children().Append(legend);
    }

    // Month total and the selected day
    {
        auto sum = BeginCard(L"");
        const std::wstring prefix = DateKey(s_calY, s_calM, 1).substr(0, 8);
        uint64_t tu = 0, td = 0;
        for (auto& kv : data)
            if (kv.first.compare(0, 8, prefix) == 0) { tu += kv.second.up; td += kv.second.down; }
        auto line1 = Text(L2(L"Tráfico total del mes: ", L"Total traffic this month: ") + FmtSizeText((double)(tu + td)) +
                              L2(L"   (Subida: ", L"   (Upload: ") + FmtSizeText((double)tu) +
                              L2(L" / Bajada: ", L" / Download: ") + FmtSizeText((double)td) + L")", 13);
        line1.Margin(wux::Thickness{4, 10, 4, 4});
        sum.Children().Append(line1);
        auto it = data.find(s_calSel);
        std::wstring detail = s_calSel + L":  ";
        if (it != data.end())
            detail += L2(L"Subida ", L"Upload ") + FmtSizeText((double)it->second.up) + L2(L"  ·  Bajada ", L"  ·  Download ") +
                      FmtSizeText((double)it->second.down) + L"  ·  Total " + FmtSizeText((double)(it->second.up + it->second.down));
        else
            detail += L2(L"sin datos", L"no data");
        auto line2 = Text(detail, 13, 0.75);
        line2.Margin(wux::Thickness{4, 0, 4, 10});
        sum.Children().Append(line2);
    }
}

static void PageHistory() {
    PageTitle(L2(L"Historial de tráfico", L"Traffic history"));
    const auto data = HistorySnapshot();

    {
        auto card = BeginCard(L"");
        wuxc::StackPanel tabs;
        tabs.Orientation(wuxc::Orientation::Horizontal);
        tabs.Spacing(8);
        tabs.Children().Append(TextButton(L2(L"Vista de lista", L"List view"), []() { s_hTab = 0; RebuildPage(); },
                                          s_hTab == 0 ? Look::Accent : Look::Standard));
        tabs.Children().Append(TextButton(L2(L"Vista de calendario", L"Calendar view"), []() { s_hTab = 1; RebuildPage(); },
                                          s_hTab == 1 ? Look::Accent : Look::Standard));
        tabs.Children().Append(GlyphBtn(L"\uE72C", L2(L"Actualizar", L"Refresh"), []() { RebuildPage(); }));
        AddRow(card, RowOf(L2(L"Vista", L"View"),
                           L2(L"Tráfico por periodos o por días en un calendario. Se guarda hasta diez años.",
                              L"Traffic by period, or by day on a calendar. Kept for up to ten years."),
                           tabs));
    }

    if (s_hTab == 0) HistoryListView(data);
    else HistoryCalendarView(data);

    auto card = BeginCard(L2(L"Datos", L"Data"));
    {
        wuxc::StackPanel sp;
        sp.Orientation(wuxc::Orientation::Horizontal);
        sp.Spacing(8);
        sp.Children().Append(TextButton(L2(L"Importar historial…", L"Import history…"), []() { DoImportHistory(); }));
        sp.Children().Append(TextButton(L2(L"Exportar historial…", L"Export history…"), []() { DoExportHistory(); }));
        AddRow(card, RowOf(L2(L"Importar / exportar", L"Import / export"), L"", sp));
    }
    AddNote(card, L2(L"Para traer el historial de la app original, importa su archivo traffic_history.json (normalmente en %APPDATA%\\TrafficMonitor). "
                     L"Si un día ya existe, se conserva el valor mayor, así que puedes importarlo más de una vez sin duplicar tráfico.",
                     L"To bring the history of the original app, import its traffic_history.json (usually in %APPDATA%\\TrafficMonitor). "
                     L"If a day already exists the larger value is kept, so you can import it more than once without double counting."));
}

// ---- Window ----

struct Category {
    const wchar_t* es;
    const wchar_t* en;
    const wchar_t* glyph;
    void (*build)();
};
static const Category kCats[] = {
    {L"Monitor", L"Monitor", L"\uE7F4", PageMonitor},
    {L"Aspecto", L"Appearance", L"\uE771", PageAppearance},
    {L"Colores", L"Colors", L"\uE790", PageColors},
    {L"General", L"General", L"\uE713", PageGeneral},
    {L"Historial", L"History", L"\uE81C", PageHistory},
};
static constexpr int kCatCount = 5;
static_assert(kCatCount > kPageHistory, "history page index");
[[clang::no_destroy]] static std::vector<wuxc::Border> s_pills;
[[clang::no_destroy]] static std::vector<wuxc::Button> s_navButtons;

static bool IsDirty() {
    std::lock_guard<std::mutex> lk(g_cfgMtx);
    return g_vals != g_saved;
}

static void SelectNav(int idx) {
    s_sel = idx;
    for (int i = 0; i < (int)s_pills.size(); i++) {
        s_pills[i].Opacity(i == idx ? 1.0 : 0.0);
        s_navButtons[i].Background(i == idx ? BrC(F().subtleHover) : Br(0, 0, 0, 0));
    }
}

static void FillPage() {
    s_pagePanel.Children().Clear();
    if (s_sel >= 0 && s_sel < kCatCount) kCats[s_sel].build();
}

static void RebuildPage() {
    if (!s_pagePanel) return;
    const double offset = s_pageScroll ? s_pageScroll.VerticalOffset() : 0.0;
    try {
        FillPage();
        if (s_pageScroll) {
            s_pageScroll.UpdateLayout();
            s_pageScroll.ChangeView(nullptr, winrt::box_value(offset).as<winrt::Windows::Foundation::IReference<double>>(), nullptr, true);
        }
    } catch (...) {
        Wh_Log(L"RebuildPage: exception ignored");
    }
}

// ---- Acrylic background of the panel (the same look as the FFmpeg Studio app) ----
static bool PanelAcrylicOn() { return ValGet(L"panelAcrylic") == L"1"; }

// Windows 11 22H2+ (build 22621) has real acrylic as a DWM system backdrop. The old
// SetWindowCompositionAttribute acrylic is flat/black there, so it is only the fallback.
// The DWM system backdrop and the XAML AcrylicBrush fall back to a flat solid color when the window
// is inactive. To avoid it the window is always reported as active (WM_NCACTIVATE below).
// (The legacy accent acrylic is NOT an option on Win11: the XAML island is opaque and shows black.)
static constexpr bool kKeepAcrylicWhenInactive = true;

static bool SystemBackdropSupported() {
    static int ok = -1;
    if (ok < 0) {
        ok = 0;
        using RtlGetVersionFn = LONG(WINAPI*)(OSVERSIONINFOW*);
        auto fn = reinterpret_cast<RtlGetVersionFn>(
            reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion")));
        OSVERSIONINFOW vi{};
        vi.dwOSVersionInfoSize = sizeof(vi);
        if (fn && fn(&vi) == 0 && vi.dwBuildNumber >= 22621) ok = 1;
    }
    return ok == 1;
}

// Own acrylic: a host-backdrop brush + a tint, drawn as composition visuals. XAML's AcrylicBrush turns
// itself into a flat color when the window is not active; these visuals have no such logic, so the
// blur stays when another window is in use.
namespace wuc = winrt::Windows::UI::Composition;
[[clang::no_destroy]] static wuc::ContainerVisual s_acrylicVisual{nullptr};
[[clang::no_destroy]] static wuc::CompositionColorBrush s_tintBrush{nullptr};

static void UpdateCustomAcrylic() {
    if (!s_acrylicVisual || !s_tintBrush) return;
    try {
        s_acrylicVisual.IsVisible(PanelAcrylicOn());
        const long pct = std::clamp(MInt(SnapshotVals(), L"panelAlpha", 50), 20L, 100L);
        const BYTE t = s_light ? 243 : 32;
        s_tintBrush.Color(Color{(BYTE)(pct * 255 / 100), t, t, t});
    } catch (...) {
        Wh_Log(L"UpdateCustomAcrylic failed");
    }
}

// First child of the root: an empty Border that carries the acrylic visuals under all the UI.
static wuxc::Border MakeBackdropBorder() {
    wuxc::Border b;
    b.IsHitTestVisible(false);
    s_acrylicVisual = nullptr;
    s_tintBrush = nullptr;
    if (!SystemBackdropSupported()) return b;
    try {
        auto vis = wux::Hosting::ElementCompositionPreview::GetElementVisual(b);
        auto comp = vis.Compositor();
        auto backdrop = comp.CreateSpriteVisual();
        backdrop.Brush(comp.CreateHostBackdropBrush());
        auto tint = comp.CreateSpriteVisual();
        s_tintBrush = comp.CreateColorBrush(Color{0, 0, 0, 0});
        tint.Brush(s_tintBrush);
        for (auto sv : {backdrop, tint}) {
            auto size = comp.CreateExpressionAnimation(L"host.Size");
            size.SetReferenceParameter(L"host", vis);
            sv.StartAnimation(L"Size", size);
        }
        auto container = comp.CreateContainerVisual();
        container.Children().InsertAtTop(backdrop);
        container.Children().InsertAtTop(tint);
        wux::Hosting::ElementCompositionPreview::SetElementChildVisual(b, container);
        s_acrylicVisual = container;
        UpdateCustomAcrylic();
    } catch (...) {
        Wh_Log(L"Custom acrylic failed, falling back to AcrylicBrush");
        s_acrylicVisual = nullptr;
        s_tintBrush = nullptr;
    }
    return b;
}

// Background of the XAML root. Windows 11: a XAML acrylic brush (BackgroundSource = HostBackdrop) that
// blurs what is behind the window (in-app "Backdrop" only blurs XAML content, which is nothing here);
// a transparent island would just be black. The opacity slider is the brush's tint opacity.
// Older Windows: transparent root over the legacy accent acrylic.
static wuxm::Brush PanelRootBrush() {
    if (PanelAcrylicOn() && s_acrylicVisual) return Br(0, 0, 0, 0);  // the blur is the backdrop visual below
    if (PanelAcrylicOn()) {
        if (!SystemBackdropSupported()) return Br(0, 0, 0, 0);
        const long pct = std::clamp(MInt(SnapshotVals(), L"panelAlpha", 50), 20L, 100L);
        const BYTE t = s_light ? 243 : 32;
        try {
            wuxm::AcrylicBrush ab;
            ab.BackgroundSource(wuxm::AcrylicBackgroundSource::HostBackdrop);
            ab.TintColor(Color{255, t, t, t});
            ab.TintOpacity(pct / 100.0);
            ab.TintLuminosityOpacity(winrt::box_value(std::clamp(pct / 100.0, 0.2, 1.0)).as<winrt::Windows::Foundation::IReference<double>>());
            ab.FallbackColor(Color{255, t, t, t});
            return ab;
        } catch (...) {
            Wh_Log(L"AcrylicBrush failed, using a solid color");
        }
    }
    return s_light ? Br(255, 243, 243, 243) : Br(255, 32, 32, 32);
}

static void BuildPanelUi() {
    s_light = IsSystemLight();
    s_tplBtn[0][0] = s_tplBtn[1][0] = nullptr;
    s_tplText = nullptr;
    RefreshAccent();
    s_pills.clear();
    s_navButtons.clear();

    wuxc::Grid root;
    s_root = root;
    auto backdropBorder = MakeBackdropBorder();
    root.Background(PanelRootBrush());
    root.RequestedTheme(s_light ? wux::ElementTheme::Light : wux::ElementTheme::Dark);
    {
        auto res = root.Resources();
        for (const wchar_t* k : {L"ToggleSwitchFillOn", L"ToggleSwitchFillOnPointerOver", L"ToggleSwitchFillOnPressed", L"ToggleSwitchStrokeOn",
                                 L"ToggleSwitchStrokeOnPointerOver", L"ToggleSwitchStrokeOnPressed", L"SystemControlHighlightAccentBrush",
                                 L"SystemAccentColorBrush", L"SliderTrackValueFill", L"SliderTrackValueFillPointerOver", L"SliderTrackValueFillPressed"})
            res.Insert(winrt::box_value(hstring{k}), s_accent);
    }

    wuxc::Grid shell;
    {
        wuxc::ColumnDefinition side;
        side.Width(wux::GridLength{240, wux::GridUnitType::Pixel});
        wuxc::ColumnDefinition page;
        page.Width(wux::GridLength{1, wux::GridUnitType::Star});
        shell.ColumnDefinitions().Append(side);
        shell.ColumnDefinitions().Append(page);
    }
    wuxc::Grid pane;
    pane.Padding(wux::Thickness{4, 8, 4, 0});
    for (auto h : {wux::GridUnitType::Star, wux::GridUnitType::Auto}) {
        wuxc::RowDefinition r;
        r.Height(wux::GridLength{1, h});
        pane.RowDefinitions().Append(r);
    }

    s_navItems = wuxc::StackPanel();
    for (int i = 0; i < kCatCount; i++) {
        wuxc::Border pill;
        pill.Width(3);
        pill.Height(16);
        pill.CornerRadius(wux::CornerRadius{1.5, 1.5, 1.5, 1.5});
        pill.Background(s_accent);
        pill.Opacity(0);
        pill.VerticalAlignment(wux::VerticalAlignment::Center);
        pill.HorizontalAlignment(wux::HorizontalAlignment::Left);
        wuxc::StackPanel content;
        content.Orientation(wuxc::Orientation::Horizontal);
        content.Spacing(14);
        content.Margin(wux::Thickness{10, 0, 0, 0});
        content.Children().Append(Glyph(kCats[i].glyph, 16));
        auto label = Text(TR(kCats[i].es, kCats[i].en), 14);
        label.VerticalAlignment(wux::VerticalAlignment::Center);
        content.Children().Append(label);
        wuxc::Grid cell;
        cell.Children().Append(pill);
        cell.Children().Append(content);
        wuxc::Button b;
        StyleButton(b, Look::Subtle);
        b.Content(cell);
        b.Height(34);
        b.Margin(wux::Thickness{4, 1, 4, 1});
        b.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        b.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
        b.Click([i](IInspectable const&, wux::RoutedEventArgs const&) {
            if (i == s_sel) return;
            SelectNav(i);
            FillPage();
            if (s_pageScroll) s_pageScroll.ChangeView(nullptr, winrt::box_value(0.0).as<winrt::Windows::Foundation::IReference<double>>(), nullptr, true);
        });
        s_pills.push_back(pill);
        s_navButtons.push_back(b);
        s_navItems.Children().Append(b);
    }
    pane.Children().Append(s_navItems);

    {
        wuxc::StackPanel footer;
        footer.Spacing(6);
        footer.Margin(wux::Thickness{12, 8, 12, 12});
        wuxc::Grid iconRow;
        iconRow.ColumnSpacing(6);
        auto addIcon = [&iconRow](wuxc::Button const& b) {
            wuxc::ColumnDefinition col;
            col.Width(wux::GridLength{1.0, wux::GridUnitType::Star});
            iconRow.ColumnDefinitions().Append(col);
            wuxc::Grid::SetColumn(b, (int)iconRow.Children().Size());
            iconRow.Children().Append(b);
        };
        addIcon(IconButton(L"\uE74E", L2(L"Exportar ajustes…", L"Export settings…"), []() { DoExport(); }));
        addIcon(IconButton(L"\uE8B5", L2(L"Importar ajustes…", L"Import settings…"), []() { DoImport(); }));
        addIcon(IconButton(L"\uE777", L2(L"Descartar cambios sin guardar", L"Discard unsaved changes"), []() {
            { std::lock_guard<std::mutex> lk(g_cfgMtx); g_vals = g_saved; }
            g_uiEs = ResolveUiEs(SnapshotVals());
            PushSettingsToTaskbar();
            RebuildAll();
        }));
        s_saveBtn = TextButton(L2(L"Guardar", L"Save"), []() {}, Look::Accent);
        s_saveBtn.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        s_saveBtn.Click([](IInspectable const&, wux::RoutedEventArgs const&) {
            const bool ok = SaveStore(SnapshotVals());
            auto label = [](const std::wstring& t) {
                if (!s_saveBtn) return;
                wuxc::TextBlock tb;
                tb.Text(hstring{t});
                s_saveBtn.Content(tb);
            };
            label(ok ? L"\u2713  " + L2(L"Guardado", L"Saved") : L2(L"No se pudo guardar", L"Couldn't save"));
            auto timer = std::make_shared<wux::DispatcherTimer>();
            timer->Interval(std::chrono::milliseconds(1400));
            timer->Tick([timer, label](IInspectable const&, IInspectable const&) {
                timer->Stop();
                label(L2(L"Guardar", L"Save"));
            });
            timer->Start();
        });
        footer.Children().Append(iconRow);
        footer.Children().Append(s_saveBtn);
        auto credits = Text(L2(L"Basado en TrafficMonitor · panel al estilo Fluent", L"Based on TrafficMonitor · Fluent-style panel"), 11, 0.55);
        credits.HorizontalAlignment(wux::HorizontalAlignment::Center);
        credits.TextAlignment(wux::TextAlignment::Center);
        footer.Children().Append(credits);
        wuxc::Grid::SetRow(footer, 1);
        pane.Children().Append(footer);
    }
    shell.Children().Append(pane);

    wuxc::ScrollViewer sv;
    s_pageScroll = sv;
    sv.HorizontalScrollMode(wuxc::ScrollMode::Disabled);
    sv.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
    s_pagePanel = wuxc::StackPanel();
    s_pagePanel.Margin(wux::Thickness{24, 10, 24, 20});
    sv.Content(s_pagePanel);
    wuxc::Grid::SetColumn(sv, 1);
    shell.Children().Append(sv);
    root.Children().Append(backdropBorder);
    root.Children().Append(shell);
    s_src.Content(root);

    if (s_sel < 0 || s_sel >= kCatCount) s_sel = 0;
    SelectNav(s_sel);
    FillPage();
}

static void RebuildAll() {
    const double offset = s_pageScroll ? s_pageScroll.VerticalOffset() : 0.0;
    try {
        BuildPanelUi();
        if (s_pageScroll) {
            s_pageScroll.UpdateLayout();
            s_pageScroll.ChangeView(nullptr, winrt::box_value(offset).as<winrt::Windows::Foundation::IReference<double>>(), nullptr, true);
        }
        if (s_hwnd) SetWindowTextW(s_hwnd, TR(L"Ajustes de TrafficMonitor", L"TrafficMonitor settings"));
    } catch (...) {
        Wh_Log(L"RebuildAll: exception ignored");
    }
}

static void ApplyDarkTitle(HWND h) {
    BOOL dark = !s_light;
    DwmSetWindowAttribute(h, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
}

// Real acrylic behind the whole window: SetWindowCompositionAttribute with
// ACCENT_ENABLE_ACRYLICBLURBEHIND and a tint (black in the dark theme, white in the light one).
// The window is erased with black, which DWM treats as transparent, and the XAML root is
// transparent, so the blur shows through. Not in user32's headers: looked up at run time.
struct AccentPolicy {
    int state;
    int flags;
    unsigned color;  // AABBGGRR
    int animation;
};
struct CompositionData {
    int attribute;
    void* data;
    size_t size;
};

static void RefreshPanelBackdrop() {
    HWND h = s_hwnd;
    if (!h) return;
    ApplyDarkTitle(h);
    using SetWcaFn = BOOL(WINAPI*)(HWND, CompositionData*);
    static SetWcaFn setWca = reinterpret_cast<SetWcaFn>(
        reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute")));
    const bool on = PanelAcrylicOn();
    if (SystemBackdropSupported()) {
        if (setWca) {  // make sure the legacy accent is off, the two do not mix
            AccentPolicy off{};
            CompositionData d0{19, &off, sizeof(off)};
            setWca(h, &d0);
        }
        int type = on ? 3 : 1;  // DWMSBT_TRANSIENTWINDOW (acrylic) / DWMSBT_NONE
        const HRESULT hrBd = DwmSetWindowAttribute(h, DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type));
        Wh_Log(L"Panel backdrop: acrylic=%d type=%d hr=0x%08X", on ? 1 : 0, type, (unsigned)hrBd);
        MARGINS mg = on ? MARGINS{-1, -1, -1, -1} : MARGINS{0, 0, 0, 0};
        DwmExtendFrameIntoClientArea(h, &mg);
        SetWindowPos(h, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    } else if (setWca) {
        AccentPolicy acc{};
        if (on) {
            const long pct = std::clamp(MInt(SnapshotVals(), L"panelAlpha", 50), 20L, 100L);
            const unsigned a = (unsigned)std::clamp(pct * 2, 1L, 255L);  // 50 % -> 100
            const unsigned t = s_light ? 255u : 0u;
            acc = {4, 0x20 | 0x40 | 0x80 | 0x100, (a << 24) | (t << 16) | (t << 8) | t, 0};
        }
        CompositionData d{19, &acc, sizeof(acc)};
        setWca(h, &d);
    }
    UpdateCustomAcrylic();
    try {
        if (s_root) s_root.Background(PanelRootBrush());
    } catch (...) {
    }
    InvalidateRect(h, nullptr, TRUE);
}

static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_SETTINGCHANGE:
            if (l && lstrcmpiW(reinterpret_cast<LPCWSTR>(l), L"ImmersiveColorSet") == 0 && IsSystemLight() != s_light) {
                try {
                    RebuildAll();
                    RefreshPanelBackdrop();
                } catch (...) {
                }
            }
            break;
        case WM_ERASEBKGND: {
            if (PanelAcrylicOn()) {  // black = transparent for DWM: the acrylic shows through
                RECT rcA;
                GetClientRect(h, &rcA);
                FillRect(reinterpret_cast<HDC>(w), &rcA, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
                return 1;
            }
            if (!s_eraseLight) s_eraseLight = CreateSolidBrush(RGB(243, 243, 243));
            if (!s_eraseDark) s_eraseDark = CreateSolidBrush(RGB(32, 32, 32));
            RECT rc;
            GetClientRect(h, &rc);
            FillRect(reinterpret_cast<HDC>(w), &rc, s_light ? s_eraseLight : s_eraseDark);
            return 1;
        }
        case WM_SIZE:
            if (s_island) MoveWindow(s_island, 0, 0, LOWORD(l), HIWORD(l), TRUE);
            return 0;
        case WM_NCACTIVATE:
            // Always "active" for DWM / XAML so the acrylic does not turn into a flat color
            // when another window has the focus.
            if (kKeepAcrylicWhenInactive && PanelAcrylicOn() && SystemBackdropSupported()) {
                const LRESULT r = DefWindowProcW(h, m, TRUE, l);
                if (!w) {  // really deactivated: tell the XAML island it is still active
                    if (s_island) SendMessageW(s_island, WM_ACTIVATE, MAKEWPARAM(WA_ACTIVE, 0), 0);
                }
                return r;
            }
            break;
        case WM_SETFOCUS:
            if (s_island) SetFocus(s_island);
            return 0;
        case WM_XS_REFRESH:
            try {
                const int p = s_reqPage.exchange(-1);
                if (p >= 0 && p < kCatCount) {
                    SelectNav(p);
                    FillPage();
                    if (s_pageScroll)
                        s_pageScroll.ChangeView(nullptr, winrt::box_value(0.0).as<winrt::Windows::Foundation::IReference<double>>(), nullptr, true);
                } else {
                    RebuildPage();
                }
            } catch (...) {}
            return 0;
        case WM_CLOSE:
            if (g_unloading) {
                DestroyWindow(h);
                return 0;
            }
            if (IsDirty()) {
                int choice = MessageBoxW(h, L2(L"Los cambios son una vista previa hasta que los guardes. Si cierras ahora se descartarán.\n\nPulsa «Guardar» primero para conservarlos.\n\n¿Cerrar y descartar los cambios?",
                                               L"Changes are a live preview until you save them. Closing now will discard them.\n\nClick \"Save\" first to keep them.\n\nClose and discard the changes?").c_str(),
                                         L2(L"Cambios sin guardar", L"Unsaved changes").c_str(), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
                if (choice != IDYES) return 0;
                {
                    std::lock_guard<std::mutex> lk(g_cfgMtx);
                    g_vals = g_saved;
                }
                g_uiEs = ResolveUiEs(SnapshotVals());
                PushSettingsToTaskbar();
            }
            try { if (s_applyTimer) s_applyTimer.Stop(); } catch (...) {}
            DestroyWindow(h);
            return 0;
        case WM_DESTROY:
            try { if (s_src) s_src.Close(); } catch (...) {}
            s_src = nullptr;
            s_root = nullptr;
            s_acrylicVisual = nullptr;
            s_tintBrush = nullptr;
            s_pagePanel = nullptr;
            s_pageScroll = nullptr;
            s_navItems = nullptr;
            s_saveBtn = nullptr;
            s_pills.clear();
            s_navButtons.clear();
            s_tplBtn[0][0] = s_tplBtn[1][0] = nullptr;
            s_tplBtn[0][1] = s_tplBtn[1][1] = nullptr;
            s_tplText = nullptr;
            s_accent = s_accentHover = s_accentPressed = s_accentFg = nullptr;
            s_island = nullptr;
            s_hwnd = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

// The app's own icon (icono.ico of the original TrafficMonitor), embedded as PNG frames.
// Windows builds an HICON straight from PNG data, so no image library is needed.
static const unsigned char kIcon16[] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0xF3, 0xFF, 0x61, 0x00, 0x00, 0x01, 0xFB, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0xA5, 0x93, 0x3D, 0x6B, 0x54,
    0x41, 0x14, 0x86, 0x9F, 0x33, 0x33, 0xF7, 0xEE, 0xB2, 0x77, 0x37, 0xDE, 0xC4, 0x10, 0x8C, 0xA2, 0x51, 0x84, 0x48, 0x1A, 0x5B, 0xAB, 0x54, 0xC1,
    0x5F, 0x20, 0xAE, 0x82, 0x95, 0x9D, 0x04, 0x04, 0xFD, 0x03, 0x01, 0x03, 0x76, 0x29, 0x45, 0x10, 0xFC, 0x07, 0x62, 0x21, 0x58, 0x5B, 0x24, 0x85,
    0xA4, 0xB7, 0x11, 0xD2, 0x6C, 0x24, 0x28, 0x92, 0xC8, 0xE6, 0x63, 0xBF, 0x6E, 0x66, 0xE6, 0x58, 0xEC, 0x62, 0x2E, 0x9B, 0x6C, 0xE5, 0x81, 0x29,
    0x0E, 0xCC, 0xFB, 0xCC, 0x39, 0xEF, 0x39, 0x23, 0xA8, 0xCA, 0xC3, 0xAD, 0xDD, 0x75, 0x57, 0xA9, 0xAE, 0x86, 0x62, 0x90, 0xA3, 0x2A, 0x80, 0x70,
    0x71, 0x28, 0x22, 0x6A, 0xD3, 0x4A, 0xDB, 0x0F, 0xFA, 0x6F, 0x3F, 0x2C, 0xDF, 0x58, 0x93, 0xE6, 0x56, 0xEB, 0x55, 0xDA, 0xC8, 0xD7, 0x4E, 0x4F,
    0x8E, 0xD0, 0x18, 0x27, 0xE8, 0xC6, 0x18, 0xC6, 0x92, 0xD4, 0xA7, 0x28, 0x0E, 0xDB, 0xEB, 0xD2, 0xDC, 0x6C, 0x75, 0xC4, 0xDA, 0xAA, 0x7A, 0x2F,
    0x88, 0x4C, 0x7A, 0x79, 0x8C, 0xA1, 0x2A, 0xCE, 0xA9, 0x86, 0xD0, 0x37, 0x40, 0x4D, 0x83, 0x37, 0x3A, 0x12, 0x6B, 0xE9, 0x5E, 0x99, 0x66, 0xA4,
    0x94, 0x8B, 0x88, 0x06, 0x6F, 0x80, 0x9A, 0x01, 0xD4, 0x88, 0xD0, 0x0B, 0x91, 0x17, 0x4B, 0xB3, 0x3C, 0xB8, 0x3E, 0xC5, 0x89, 0x8F, 0x24, 0x06,
    0x4E, 0xA3, 0xFE, 0x13, 0x77, 0xBD, 0xD2, 0x0B, 0xE7, 0xF0, 0x6A, 0x00, 0x51, 0x1D, 0xA6, 0x0B, 0x59, 0xCA, 0xD5, 0x5A, 0x02, 0x40, 0xC7, 0x2B,
    0xB3, 0x55, 0x07, 0xC0, 0xC1, 0x20, 0xF0, 0xE4, 0x56, 0xCE, 0xEA, 0xE2, 0x0C, 0xBD, 0x10, 0x31, 0x67, 0xA5, 0x89, 0x29, 0xB7, 0x56, 0x44, 0xC5,
    0x47, 0x68, 0x17, 0x81, 0xE5, 0xB9, 0x8C, 0x77, 0xF7, 0xAE, 0xF1, 0xF4, 0xF6, 0x34, 0x8F, 0x16, 0x72, 0x9A, 0x0B, 0x97, 0xF8, 0xD9, 0xF3, 0xE7,
    0xC6, 0xE3, 0xCA, 0x89, 0x05, 0x8A, 0x18, 0x79, 0x7C, 0x33, 0xE7, 0xF9, 0x9D, 0xCB, 0x7C, 0xDD, 0xEF, 0x72, 0x7F, 0xBE, 0x81, 0x15, 0x78, 0xBF,
    0xF3, 0x87, 0x8F, 0xBB, 0x87, 0xE4, 0xA9, 0x25, 0xEA, 0x04, 0x40, 0x40, 0x49, 0xAD, 0xB0, 0x72, 0xA5, 0xCE, 0xA7, 0x1F, 0x47, 0xBC, 0xF9, 0x7E,
    0xC0, 0xCA, 0x7C, 0x9D, 0xE9, 0xD4, 0xF2, 0x79, 0xEF, 0x88, 0x99, 0xD4, 0xE1, 0xB5, 0xEC, 0xC3, 0x18, 0xC0, 0x20, 0x14, 0x41, 0x79, 0xB6, 0xBD,
    0x47, 0xDD, 0x09, 0x35, 0x67, 0xF8, 0xF2, 0xEB, 0x84, 0xA0, 0x4A, 0xDD, 0x59, 0x8E, 0x7D, 0xA0, 0x62, 0x4C, 0xD9, 0x83, 0x21, 0x40, 0x04, 0xA2,
    0x42, 0x3F, 0x44, 0x16, 0xA7, 0x2A, 0x2C, 0xCF, 0x65, 0x34, 0x12, 0x21, 0xE8, 0x10, 0x2A, 0x02, 0x41, 0x15, 0x55, 0xD8, 0x39, 0x2E, 0xE8, 0xF8,
    0x33, 0x23, 0xA5, 0xB9, 0xD9, 0x8A, 0x46, 0x90, 0x5E, 0x50, 0xEE, 0xE6, 0x55, 0x5E, 0x2E, 0xCD, 0x32, 0x9D, 0x5A, 0x26, 0xC5, 0xEB, 0x6F, 0xBF,
    0xD9, 0xDE, 0xEF, 0x92, 0x39, 0x43, 0x54, 0x54, 0x9A, 0x9B, 0x2D, 0x45, 0x15, 0x11, 0x61, 0x10, 0x95, 0xCC, 0x19, 0xEA, 0xCE, 0x5C, 0x28, 0x56,
    0xE0, 0xB0, 0x08, 0xA3, 0x65, 0x53, 0x40, 0x70, 0x40, 0x57, 0x9C, 0xAB, 0x46, 0xEF, 0xA5, 0x6A, 0x45, 0x06, 0x41, 0xE9, 0xFA, 0xE1, 0xB8, 0x74,
    0x0C, 0x20, 0x80, 0x33, 0x32, 0xB6, 0xCA, 0xCA, 0x46, 0x92, 0x35, 0x8C, 0xB1, 0x56, 0xA2, 0x2A, 0x56, 0x20, 0x35, 0x42, 0x62, 0x84, 0x74, 0xEC,
    0x24, 0xA3, 0xC6, 0xC5, 0x5A, 0x49, 0xB2, 0x86, 0x41, 0xD9, 0x90, 0xFF, 0xFD, 0xCE, 0x7F, 0x01, 0x7C, 0xCE, 0xE5, 0xE8, 0x68, 0xE5, 0xC6, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
};
static const unsigned char kIcon24[] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xE0, 0x77, 0x3D, 0xF8, 0x00, 0x00, 0x03, 0x1F, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0xAD, 0x96, 0xCF, 0x6B, 0x5C,
    0x55, 0x14, 0xC7, 0x3F, 0xE7, 0xDE, 0xF7, 0x23, 0x33, 0x19, 0x9D, 0x99, 0x4E, 0xA3, 0x8D, 0x52, 0x49, 0x1B, 0x5A, 0x2C, 0x82, 0x50, 0x8C, 0xA0,
    0x6E, 0xFC, 0x1B, 0x5C, 0x34, 0xE2, 0x4A, 0x41, 0x14, 0x15, 0x34, 0xDD, 0xB9, 0x70, 0xE1, 0x52, 0xEC, 0xA2, 0xD1, 0x82, 0x88, 0x14, 0xD7, 0x0A,
    0xAE, 0x5C, 0x74, 0xE3, 0xAE, 0x2D, 0xE2, 0x52, 0x37, 0x62, 0x31, 0x88, 0x53, 0x62, 0x6B, 0xC9, 0xAF, 0x9A, 0x4E, 0x66, 0xF2, 0x66, 0xDE, 0xBB,
    0xF7, 0xB8, 0x78, 0x49, 0x66, 0xDE, 0x74, 0x92, 0x66, 0xB4, 0x07, 0x2E, 0xF7, 0xFD, 0xE0, 0x7E, 0xBF, 0xE7, 0x9C, 0x7B, 0xBE, 0xF7, 0x5C, 0x41,
    0x55, 0x10, 0xD1, 0x57, 0x7F, 0xBC, 0xB5, 0x20, 0xD6, 0xBE, 0xE3, 0xB3, 0x6C, 0x16, 0xEF, 0x02, 0x40, 0x18, 0xCF, 0x54, 0x8C, 0xCD, 0x24, 0x08,
    0xFE, 0x40, 0xDD, 0x97, 0xDF, 0xBE, 0xF8, 0xE4, 0x25, 0x54, 0x45, 0x50, 0x95, 0xF9, 0xEB, 0xCD, 0xCB, 0x71, 0xF5, 0xE8, 0x9B, 0x69, 0xBB, 0x85,
    0xCF, 0xD2, 0x31, 0x71, 0x8B, 0x66, 0x82, 0x90, 0x70, 0xF2, 0x11, 0x92, 0xBB, 0x6B, 0x5F, 0x7F, 0xF7, 0xF2, 0xCC, 0x5B, 0x32, 0x7F, 0xAD, 0xF9,
    0x7E, 0x5C, 0x6D, 0x5C, 0xEA, 0x6E, 0x6E, 0x64, 0xAA, 0xDE, 0x8A, 0x98, 0x71, 0x3D, 0x2F, 0x86, 0x81, 0x7A, 0x41, 0x7C, 0x5C, 0x3D, 0x12, 0x74,
    0x37, 0xD7, 0x3F, 0x08, 0x10, 0x73, 0x3E, 0xED, 0x6C, 0x79, 0x45, 0xCD, 0xFF, 0x05, 0x07, 0x10, 0xC4, 0x28, 0x4A, 0xDA, 0xD9, 0xF2, 0x88, 0x39,
    0x1F, 0xA8, 0xFA, 0x19, 0x9F, 0xF6, 0x8C, 0xEC, 0xA4, 0x5C, 0x00, 0x1D, 0x98, 0xFF, 0x2B, 0x89, 0x4F, 0x7B, 0x28, 0x3A, 0x63, 0x04, 0x31, 0xFD,
    0x1F, 0x90, 0x69, 0x0E, 0x9C, 0xA9, 0x1E, 0x8A, 0x40, 0x0E, 0x88, 0x59, 0x10, 0x53, 0x00, 0x77, 0x0A, 0xE5, 0x40, 0xF8, 0xF4, 0xEC, 0x31, 0xDE,
    0x3E, 0xD5, 0xA0, 0xEB, 0xB4, 0x00, 0x30, 0x9C, 0x40, 0x11, 0xE8, 0x3A, 0x45, 0x0F, 0xF0, 0x24, 0x18, 0xA0, 0x23, 0xF3, 0xCA, 0x54, 0x1C, 0x72,
    0xA2, 0x12, 0x51, 0xB2, 0x86, 0xC8, 0x08, 0xAA, 0xFD, 0x7A, 0xDD, 0xCE, 0x94, 0xD8, 0xCA, 0xDE, 0x7B, 0xE2, 0x94, 0x93, 0x95, 0x88, 0x95, 0x24,
    0xA3, 0xEB, 0xF4, 0x3E, 0x07, 0x00, 0xCC, 0xF0, 0x07, 0xAF, 0xF9, 0xE8, 0x7A, 0xDD, 0xF3, 0xDA, 0x29, 0xA4, 0xAA, 0x9C, 0xA9, 0xC6, 0x00, 0x38,
    0x55, 0x5A, 0x99, 0xE7, 0x95, 0xE3, 0x8F, 0x72, 0xF1, 0xB9, 0x69, 0x5E, 0x9B, 0xA9, 0xD1, 0x71, 0x1E, 0x7B, 0x18, 0x82, 0x5D, 0x50, 0xD9, 0x99,
    0x33, 0x9F, 0x03, 0xBE, 0x7B, 0xAA, 0xC1, 0x27, 0x67, 0x8F, 0xB1, 0xF0, 0xF4, 0x51, 0x12, 0xA7, 0x9C, 0x7B, 0xAA, 0xCA, 0xEB, 0x27, 0xEB, 0x00,
    0x2C, 0xB5, 0xBA, 0x04, 0x22, 0x23, 0xF7, 0x2C, 0x18, 0xF1, 0x2D, 0xCF, 0xD8, 0x4E, 0x7E, 0x27, 0xAC, 0xF0, 0xE1, 0x33, 0x8F, 0x31, 0xD7, 0x28,
    0x01, 0xF0, 0xD2, 0x54, 0x99, 0xCF, 0x9E, 0x7F, 0x82, 0x13, 0x93, 0x11, 0x00, 0x5F, 0xFC, 0xBE, 0xCE, 0x0F, 0xB7, 0xB7, 0xA8, 0x46, 0x06, 0x3F,
    0x82, 0x61, 0x64, 0x04, 0x00, 0xCE, 0x2B, 0xB5, 0xC8, 0xF2, 0xF1, 0xB3, 0x8F, 0x33, 0xD7, 0x28, 0xB1, 0x9A, 0x64, 0x7C, 0x7E, 0x63, 0x8D, 0x7F,
    0x7A, 0x8E, 0xD9, 0x4A, 0x84, 0x11, 0xF8, 0x6A, 0x69, 0x83, 0x2B, 0x7F, 0xB5, 0xA8, 0xED, 0x03, 0x7E, 0x60, 0x04, 0x1D, 0xA7, 0xBC, 0x30, 0x55,
    0xE6, 0x4C, 0x35, 0xE6, 0x66, 0xBB, 0xC7, 0x85, 0x5F, 0xD7, 0xB8, 0x71, 0x2F, 0xE1, 0x76, 0x27, 0xE3, 0x8D, 0xD9, 0x3A, 0xD7, 0x57, 0xDA, 0x7C,
    0xBF, 0x7C, 0x8F, 0x5A, 0x64, 0x70, 0x87, 0xAA, 0xA2, 0x21, 0x2B, 0x5B, 0xC3, 0x4F, 0xAB, 0x6D, 0xBC, 0x2A, 0x3F, 0x6F, 0x6C, 0xB3, 0x92, 0x38,
    0xA6, 0x4B, 0x21, 0x4B, 0xAD, 0x2E, 0x1F, 0xFD, 0x72, 0x07, 0x0F, 0x54, 0x1F, 0x00, 0x7E, 0x20, 0x81, 0x08, 0x6C, 0xF6, 0x3C, 0x57, 0x6E, 0xB5,
    0x28, 0x59, 0xA1, 0x64, 0x85, 0x9E, 0xD7, 0xBC, 0x74, 0xC9, 0x8B, 0xC0, 0x6B, 0x5F, 0xF5, 0xFB, 0x29, 0xBF, 0x4F, 0xA0, 0x85, 0x89, 0x92, 0x15,
    0x6A, 0x91, 0x25, 0xF1, 0x3E, 0x5F, 0xAC, 0xFD, 0x0D, 0x93, 0x81, 0x59, 0x00, 0x0F, 0xF4, 0xBC, 0x12, 0x9B, 0xFB, 0x2B, 0x69, 0x8F, 0x40, 0x81,
    0xC8, 0x08, 0x7F, 0x6F, 0xA7, 0x2C, 0x77, 0x52, 0x8E, 0x97, 0x43, 0x16, 0xE7, 0xA6, 0x49, 0x9C, 0xC7, 0x88, 0xEC, 0xAB, 0x56, 0xBF, 0xB3, 0xFA,
    0x9B, 0x3F, 0x37, 0xB9, 0xBA, 0xD2, 0x66, 0x32, 0x90, 0xC2, 0x86, 0x17, 0x52, 0x64, 0x24, 0x57, 0xE7, 0xC5, 0xDF, 0xD6, 0x78, 0xEF, 0xF4, 0x11,
    0x66, 0x2B, 0x31, 0xF5, 0xC8, 0x8E, 0x46, 0x1E, 0xB2, 0x7A, 0x6C, 0xF1, 0xAA, 0xC8, 0x50, 0x9F, 0x92, 0xF9, 0x6B, 0x37, 0x0B, 0xBE, 0x09, 0xB9,
    0x8A, 0x03, 0x11, 0xA6, 0x26, 0x2C, 0xE6, 0x41, 0x8D, 0x4D, 0x72, 0x21, 0xAE, 0x26, 0x6E, 0xA4, 0x92, 0x03, 0x05, 0x2F, 0x03, 0x7A, 0x50, 0x60,
    0xC2, 0xE6, 0x61, 0xDE, 0xD9, 0xCE, 0x0E, 0xE5, 0x3D, 0x40, 0x38, 0xE2, 0x20, 0x52, 0xD4, 0x07, 0x22, 0xD2, 0x34, 0x41, 0x38, 0xE3, 0xD2, 0x2E,
    0xBB, 0x47, 0xF7, 0x6E, 0x0E, 0xA3, 0x31, 0xFA, 0x8F, 0x16, 0x9E, 0xD5, 0xDB, 0x30, 0xC6, 0x67, 0x69, 0xD3, 0x08, 0x2C, 0x86, 0xE5, 0x8A, 0x11,
    0xC4, 0xAB, 0x16, 0xF5, 0xA8, 0x63, 0x8C, 0x41, 0x70, 0x41, 0x7C, 0x8E, 0xC9, 0xA2, 0xA0, 0x2A, 0xE7, 0xAE, 0x36, 0x2F, 0x4F, 0xD4, 0x1F, 0x66,
    0xD3, 0xAF, 0x90, 0xDC, 0x5D, 0xCF, 0x9B, 0x7E, 0xFF, 0xDA, 0xB2, 0xBC, 0x20, 0x36, 0x7C, 0xE8, 0xD7, 0x96, 0x7F, 0x01, 0xF4, 0x8E, 0x79, 0x7C,
    0x62, 0xE6, 0x93, 0xB7, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
};
static const unsigned char kIcon32[] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x73, 0x7A, 0x7A, 0xF4, 0x00, 0x00, 0x04, 0x26, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0xC5, 0x97, 0x5D, 0x68, 0x1C,
    0x55, 0x14, 0xC7, 0x7F, 0xE7, 0xCE, 0xCC, 0x4E, 0x76, 0x93, 0x6D, 0xD2, 0xB0, 0x2B, 0x49, 0xD4, 0x12, 0xFA, 0x21, 0x24, 0xD1, 0x97, 0xE6, 0xA1,
    0xA0, 0xE4, 0xCD, 0x8F, 0x37, 0x41, 0x11, 0xAD, 0x4F, 0x11, 0x1F, 0x0A, 0x2D, 0x08, 0x56, 0x10, 0x14, 0x44, 0x29, 0xF8, 0x24, 0x42, 0x11, 0x14,
    0x51, 0x0A, 0xA2, 0xBE, 0xA8, 0x15, 0x8B, 0xF8, 0x81, 0x82, 0x20, 0x28, 0x48, 0x51, 0x5A, 0xAD, 0xB4, 0x56, 0x2D, 0x08, 0x09, 0xCD, 0x6A, 0xDA,
    0xCD, 0xE7, 0x66, 0x37, 0xFB, 0x71, 0xE7, 0x1E, 0x1F, 0x66, 0x13, 0x34, 0xD9, 0x4D, 0x76, 0xDB, 0xB5, 0xFE, 0x61, 0x98, 0x65, 0x98, 0xB9, 0xFF,
    0xFF, 0x9E, 0xF3, 0x3F, 0xE7, 0xDE, 0x23, 0xA8, 0x0A, 0x22, 0x0A, 0x70, 0xF0, 0x74, 0xEE, 0x21, 0xD4, 0x1C, 0x51, 0x17, 0xED, 0xD7, 0xC8, 0xEE,
    0x00, 0x84, 0xCE, 0x40, 0xC5, 0xF3, 0x97, 0x8D, 0xEF, 0x9F, 0x51, 0x1B, 0xBD, 0xFE, 0xDE, 0x5D, 0x43, 0x1F, 0xC6, 0x4F, 0x55, 0xE4, 0x05, 0x55,
    0xF3, 0xCB, 0x05, 0x7C, 0xB3, 0x94, 0x7B, 0xD3, 0x4B, 0x84, 0x93, 0xEA, 0x1C, 0xAE, 0x5A, 0x41, 0xD5, 0x75, 0x88, 0x3B, 0x86, 0x88, 0xC1, 0x24,
    0x12, 0x88, 0xF1, 0x88, 0xAA, 0x95, 0xB7, 0x5D, 0xEF, 0xD0, 0xA1, 0xD1, 0x31, 0xAC, 0x39, 0x26, 0xE2, 0x64, 0x6E, 0xFA, 0x44, 0x57, 0x7F, 0x66,
    0xD2, 0x96, 0x56, 0x22, 0x5B, 0x2E, 0xB9, 0x4E, 0x93, 0x03, 0xA8, 0x3A, 0x6C, 0x79, 0xD5, 0xD9, 0xD2, 0x4A, 0xD4, 0xD5, 0x9F, 0x99, 0x94, 0xB9,
    0xE9, 0x13, 0xC7, 0x44, 0x9C, 0x1C, 0xFC, 0x76, 0xFA, 0x01, 0x93, 0x4C, 0x7D, 0x64, 0x4B, 0x45, 0x0B, 0xF8, 0x1D, 0x67, 0x6E, 0x0C, 0xEB, 0xA7,
    0xBA, 0xFD, 0xDA, 0x6A, 0xE9, 0x41, 0xE3, 0x94, 0xA3, 0x1A, 0x39, 0x55, 0x55, 0x73, 0x83, 0xC8, 0x51, 0x55, 0xA3, 0xCE, 0xA9, 0x28, 0x47, 0x7D,
    0x45, 0xF7, 0x47, 0xD5, 0x8A, 0x88, 0x48, 0xA7, 0x0C, 0xB7, 0x2D, 0x44, 0xC4, 0x44, 0x95, 0x0A, 0x8A, 0xEE, 0x37, 0x02, 0xDD, 0x34, 0xC8, 0xB9,
    0x6C, 0xB8, 0x77, 0x1C, 0xEA, 0x10, 0xE8, 0x6E, 0x18, 0x76, 0x01, 0xAA, 0x4E, 0xA1, 0x7E, 0xAF, 0xFF, 0xFC, 0x4F, 0xB0, 0x49, 0x80, 0x08, 0x54,
    0x9C, 0xB2, 0x37, 0x1D, 0xF2, 0xF2, 0xF8, 0x20, 0x47, 0x47, 0x32, 0xB4, 0x9B, 0x1C, 0x23, 0xE0, 0x49, 0x6B, 0xD1, 0xDB, 0x24, 0xC0, 0x10, 0x0B,
    0xB8, 0x33, 0x9B, 0xE2, 0xD6, 0x54, 0xC0, 0xC4, 0x4D, 0xDD, 0x0C, 0x25, 0x03, 0xAA, 0x4E, 0xFF, 0xB5, 0xA0, 0xD4, 0x89, 0x1A, 0x91, 0x17, 0xAD,
    0x63, 0xA9, 0xE6, 0x88, 0x74, 0x7B, 0x11, 0x4D, 0x53, 0x60, 0x55, 0x51, 0x05, 0xBB, 0x81, 0x78, 0x8D, 0xC4, 0x2A, 0xAC, 0x58, 0xB7, 0xFE, 0xFE,
    0xDA, 0xF3, 0x42, 0xCD, 0x71, 0xDF, 0x60, 0x9A, 0x43, 0x7B, 0xFB, 0x49, 0xF9, 0xB2, 0xAD, 0x88, 0xA6, 0x75, 0x2F, 0xC4, 0xE9, 0xD8, 0xF8, 0xB1,
    0x27, 0x50, 0xB4, 0x4A, 0x26, 0xF4, 0xD8, 0x9B, 0xEE, 0xE2, 0xDC, 0x42, 0x19, 0xAB, 0x4A, 0xE8, 0x09, 0x0B, 0x15, 0xC7, 0x23, 0xC3, 0xBD, 0x4C,
    0xEE, 0xDE, 0x09, 0xC4, 0xFE, 0x79, 0xEB, 0x8F, 0x05, 0xFA, 0x02, 0x43, 0xD4, 0xC4, 0x47, 0x6D, 0x35, 0x1E, 0xDF, 0x08, 0x4B, 0xD5, 0x88, 0xE1,
    0x9E, 0x04, 0xCF, 0x8C, 0x65, 0xB9, 0x39, 0x15, 0x70, 0x66, 0x7E, 0x95, 0x97, 0x2E, 0x5C, 0x25, 0x5F, 0x8E, 0x78, 0x74, 0xB8, 0x8F, 0xC9, 0xDD,
    0x3B, 0x89, 0x54, 0x51, 0xE0, 0xE2, 0x72, 0x85, 0x40, 0x84, 0xAD, 0x3C, 0xDC, 0xB2, 0x00, 0x4F, 0x60, 0xB1, 0x1A, 0x71, 0x47, 0x5F, 0x17, 0xCF,
    0xDE, 0x9E, 0x65, 0x47, 0xE0, 0x11, 0x29, 0x8C, 0xF7, 0x27, 0x79, 0x6A, 0x24, 0xC3, 0xF9, 0xC5, 0x32, 0x8F, 0xED, 0xD9, 0x89, 0x53, 0x30, 0x22,
    0x1C, 0xBF, 0x98, 0xE7, 0xFB, 0x7C, 0x89, 0x74, 0x60, 0xB6, 0xAC, 0xA2, 0x96, 0x04, 0x18, 0x81, 0xA5, 0xAA, 0xE3, 0xEE, 0xC1, 0x1E, 0x9E, 0x1C,
    0xC9, 0x10, 0xD6, 0xDD, 0xE7, 0x54, 0xF1, 0x44, 0x38, 0x90, 0x49, 0x71, 0x20, 0x93, 0xAA, 0x93, 0xC3, 0x2B, 0xBF, 0xE6, 0xF9, 0xEA, 0xCF, 0x15,
    0xFA, 0x12, 0xCD, 0x43, 0xBF, 0xBE, 0xF6, 0x76, 0xE4, 0x82, 0xB0, 0x1A, 0x39, 0xEE, 0x1D, 0xEA, 0xE1, 0xE9, 0xD1, 0x2C, 0x41, 0xBD, 0x26, 0x3F,
    0x9B, 0x29, 0xF0, 0xC4, 0x0F, 0x39, 0x66, 0xCB, 0xB6, 0x2E, 0x26, 0x26, 0x7F, 0xF5, 0xB7, 0x39, 0xBE, 0xCC, 0xB5, 0x46, 0xDE, 0x92, 0x00, 0x87,
    0x12, 0x1A, 0xC3, 0xE3, 0x7B, 0xFA, 0x31, 0x12, 0x93, 0xBC, 0x3F, 0xB5, 0xC8, 0x1B, 0x97, 0xE6, 0xC8, 0xAD, 0xD6, 0x78, 0xFE, 0xDC, 0x2C, 0x53,
    0xC5, 0x1A, 0x35, 0x55, 0x5E, 0xFB, 0x7D, 0x8E, 0xCF, 0x67, 0x0A, 0x2D, 0x93, 0x43, 0x0B, 0x29, 0x30, 0x08, 0x55, 0xE7, 0xF8, 0x74, 0x66, 0x99,
    0x7B, 0x06, 0x7A, 0xF8, 0xF8, 0x72, 0x81, 0x4F, 0x66, 0x96, 0xD9, 0x11, 0x18, 0x0C, 0xC2, 0x95, 0xB2, 0xE5, 0xB9, 0x9F, 0xFE, 0xA2, 0x27, 0x30,
    0xE4, 0x4A, 0x96, 0xDE, 0x36, 0xC8, 0x5B, 0x12, 0xA0, 0x28, 0x09, 0x63, 0xF8, 0x60, 0x6A, 0x89, 0x2F, 0x66, 0x0A, 0x94, 0x22, 0xA5, 0xB7, 0x6E,
    0xAC, 0x08, 0xA5, 0xCB, 0x13, 0xCA, 0x91, 0x52, 0xB2, 0x96, 0x94, 0x2F, 0x6D, 0xB7, 0xED, 0x96, 0xAB, 0x20, 0xE5, 0x1B, 0x44, 0x58, 0x77, 0xB5,
    0x27, 0x4A, 0xDC, 0x25, 0x94, 0xC0, 0x08, 0x82, 0xA0, 0x28, 0xDE, 0x86, 0xC6, 0xE1, 0x94, 0x6B, 0x2B, 0xC3, 0x78, 0xE9, 0xD8, 0x84, 0x22, 0x30,
    0x5F, 0x89, 0x48, 0x79, 0x42, 0xBB, 0x67, 0xA5, 0x2E, 0x4F, 0xB6, 0xEC, 0x05, 0x9B, 0x04, 0xAC, 0xFD, 0xAF, 0xCB, 0x25, 0x8B, 0x00, 0x0E, 0x38,
    0x7C, 0x5B, 0x3F, 0x3F, 0xCE, 0x97, 0xF1, 0x0D, 0x68, 0x1B, 0x21, 0xAE, 0x39, 0xE5, 0xEB, 0xD9, 0x22, 0xF9, 0x8A, 0x6D, 0x2A, 0x62, 0x93, 0x00,
    0xA7, 0x71, 0xB8, 0xBF, 0xBB, 0x5A, 0xE4, 0xFE, 0x5B, 0xD2, 0x0C, 0xF7, 0x24, 0xD8, 0x97, 0x0E, 0xD9, 0x97, 0x0E, 0x5B, 0x67, 0xFE, 0x07, 0xF6,
    0xA4, 0x43, 0x8E, 0xFD, 0x3C, 0x4B, 0x22, 0x90, 0x86, 0xE2, 0x1B, 0x96, 0xA1, 0x21, 0xEE, 0xE3, 0x2F, 0x9E, 0xBF, 0xC2, 0xE9, 0x7C, 0x89, 0xA2,
    0x75, 0x58, 0xA7, 0x58, 0x55, 0xA2, 0x16, 0xAF, 0x9A, 0x53, 0xAA, 0x4E, 0xB9, 0x54, 0xA8, 0x34, 0xDC, 0x35, 0xD7, 0x20, 0x0F, 0x7F, 0x33, 0xD5,
    0x30, 0xA8, 0x02, 0xD4, 0x54, 0xB1, 0x0E, 0x06, 0x92, 0x3E, 0x29, 0xAF, 0xBD, 0x23, 0xA3, 0x48, 0x9C, 0x82, 0xDC, 0x6A, 0x0D, 0x7F, 0x8B, 0x03,
    0x45, 0x53, 0x13, 0x2A, 0x10, 0x88, 0x90, 0xF0, 0x21, 0x5F, 0xB1, 0xD7, 0x74, 0x2A, 0x12, 0x20, 0x61, 0xB6, 0xD9, 0x8C, 0x14, 0x8A, 0x46, 0x4C,
    0x77, 0xA3, 0x59, 0x40, 0x89, 0x4D, 0x17, 0x34, 0xDA, 0x97, 0x5B, 0x44, 0x53, 0x72, 0x31, 0xA8, 0xBA, 0xA2, 0x11, 0xE4, 0xAC, 0x09, 0x43, 0xD5,
    0x2D, 0xA6, 0x11, 0xBD, 0x8E, 0xAB, 0xE1, 0x7A, 0xAA, 0xCE, 0x0B, 0x43, 0x15, 0xE4, 0xAC, 0x51, 0xE1, 0xB8, 0x18, 0x23, 0x22, 0xD2, 0xF9, 0x71,
    0xA8, 0x09, 0x44, 0xC4, 0x89, 0x31, 0xA2, 0xC2, 0x71, 0x73, 0x72, 0x62, 0xD7, 0xA9, 0x5A, 0xB1, 0xF0, 0x6E, 0x32, 0x3B, 0xE0, 0xA3, 0x1A, 0x6D,
    0x15, 0x89, 0xEB, 0x85, 0xAA, 0x3A, 0x54, 0xA3, 0x64, 0x76, 0xC0, 0xAF, 0x95, 0x56, 0xDE, 0x39, 0x39, 0xB1, 0xEB, 0xD4, 0xFF, 0x38, 0x9C, 0x2E,
    0x1C, 0x1A, 0x1D, 0x1B, 0xB3, 0xB2, 0x71, 0x3C, 0x17, 0xF1, 0x0E, 0x3B, 0x6B, 0xC7, 0x6F, 0xD4, 0x78, 0xFE, 0x37, 0xF4, 0xA5, 0x39, 0x59, 0x7A,
    0xCE, 0x2F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
};
static const unsigned char kIcon48[] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x30,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x57, 0x02, 0xF9, 0x87, 0x00, 0x00, 0x06, 0x7A, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0xD5, 0x9A, 0x4B, 0x6C, 0x5C,
    0x67, 0x15, 0xC7, 0x7F, 0xE7, 0xBB, 0x2F, 0x7B, 0x9E, 0x1E, 0xDB, 0xCD, 0x93, 0x38, 0x69, 0x5E, 0xA2, 0x09, 0xE9, 0x02, 0x81, 0x22, 0x10, 0x91,
    0x37, 0xA9, 0x50, 0x54, 0x09, 0x15, 0x68, 0xB2, 0x40, 0x08, 0x44, 0x5B, 0xD1, 0x0D, 0x62, 0x53, 0xB1, 0xA0, 0x14, 0x04, 0x64, 0x01, 0x14, 0x22,
    0x21, 0x2A, 0x16, 0xB4, 0xB0, 0x00, 0xA1, 0x4A, 0x60, 0x44, 0x01, 0xA9, 0x20, 0x21, 0x04, 0x4A, 0x23, 0x10, 0x8F, 0x2E, 0x68, 0x68, 0x42, 0x1B,
    0x59, 0x0A, 0x4E, 0x1A, 0xE2, 0xC4, 0xAF, 0xF1, 0x64, 0x66, 0x9C, 0x99, 0x7B, 0xEF, 0x77, 0x58, 0xDC, 0xB9, 0x13, 0xB7, 0x29, 0xF6, 0xCC, 0x78,
    0x1A, 0x3B, 0x7F, 0xE9, 0x6A, 0x9E, 0xD2, 0xF7, 0x3F, 0xE7, 0xFC, 0xCF, 0x39, 0xDF, 0xB9, 0xF7, 0x13, 0x52, 0xA8, 0x0A, 0x22, 0x0A, 0x70, 0xFC,
    0x2F, 0xD7, 0xF6, 0xBA, 0x86, 0x4F, 0x5B, 0x1B, 0x1F, 0xD3, 0x38, 0xDE, 0x8F, 0xDA, 0x1C, 0x20, 0xDC, 0x59, 0x28, 0x62, 0xAA, 0xE2, 0x38, 0x17,
    0x8C, 0x71, 0x7E, 0x17, 0xD9, 0xC6, 0x8F, 0x27, 0x3E, 0x38, 0x36, 0xF9, 0x56, 0xAE, 0xB2, 0xFC, 0x8B, 0xCF, 0xFE, 0xE0, 0x65, 0x6F, 0xF1, 0xD0,
    0xD6, 0xAF, 0x88, 0x98, 0xCF, 0x39, 0xBE, 0x3F, 0x64, 0xC3, 0x10, 0x1B, 0x86, 0xA8, 0xDA, 0x3B, 0xCC, 0x3D, 0x81, 0x88, 0xC1, 0x78, 0x1E, 0xC6,
    0xF3, 0x88, 0x9B, 0xCD, 0x32, 0xC2, 0x33, 0x85, 0x57, 0xAE, 0x9C, 0x7C, 0xF6, 0xF1, 0xF7, 0x85, 0x29, 0x67, 0x49, 0xDF, 0x7C, 0xE2, 0xCC, 0x54,
    0x29, 0x76, 0xFD, 0x17, 0x82, 0x5C, 0x61, 0xBC, 0xB1, 0x38, 0x8F, 0xC6, 0x71, 0xAC, 0x22, 0x22, 0x60, 0xD6, 0x85, 0x7D, 0x0B, 0x0A, 0x56, 0x54,
    0x55, 0x1C, 0xC7, 0x09, 0x86, 0x86, 0x69, 0xDC, 0xA8, 0x9C, 0x76, 0xA2, 0xE6, 0x47, 0x9F, 0x3F, 0xB2, 0x73, 0x01, 0x55, 0x91, 0xE3, 0x3F, 0x57,
    0x87, 0x03, 0xE7, 0x1C, 0x59, 0x28, 0xFE, 0x21, 0x28, 0x0C, 0x1D, 0x69, 0x94, 0xE7, 0x9A, 0x80, 0x87, 0xC8, 0x9D, 0x96, 0xCC, 0xCA, 0x50, 0x55,
    0x20, 0x0C, 0x86, 0x46, 0xFC, 0x46, 0xA5, 0x7C, 0x46, 0x4B, 0x8B, 0x47, 0x39, 0x7F, 0x30, 0x36, 0x13, 0x27, 0x24, 0x96, 0xF9, 0xFC, 0x53, 0x03,
    0xA5, 0x91, 0x84, 0xBC, 0x88, 0xBF, 0xE1, 0xC8, 0x03, 0x88, 0x08, 0x22, 0x7E, 0xA3, 0x3C, 0xD7, 0x1C, 0x28, 0x8D, 0x1C, 0x91, 0xF9, 0xFC, 0x53,
    0x13, 0x27, 0x24, 0x96, 0xE3, 0xA7, 0xAF, 0xDE, 0x2B, 0x4E, 0xF4, 0x2F, 0x11, 0x19, 0xD4, 0x28, 0x92, 0x0D, 0x49, 0x7E, 0x39, 0x54, 0x55, 0x5C,
    0x57, 0x55, 0xED, 0x92, 0xC6, 0xDE, 0x21, 0x23, 0x26, 0x7A, 0xD4, 0x1B, 0xCC, 0x66, 0x35, 0x8A, 0x74, 0xC3, 0x93, 0x07, 0x10, 0x11, 0x8D, 0x22,
    0xF5, 0x06, 0x73, 0x59, 0x31, 0xD1, 0xA3, 0x06, 0xD5, 0x07, 0x6D, 0xD8, 0x54, 0xBD, 0x1B, 0xC8, 0xB7, 0xA0, 0x22, 0x62, 0xC3, 0xA6, 0xA2, 0xFA,
    0xA0, 0xAB, 0xE8, 0x3E, 0x1B, 0x86, 0x22, 0x77, 0xBE, 0xCE, 0xF7, 0x0C, 0x01, 0x63, 0xC3, 0x10, 0x45, 0xF7, 0x19, 0x81, 0xEC, 0x7A, 0xD5, 0xF9,
    0xB5, 0x40, 0xD5, 0x22, 0x90, 0xED, 0xB8, 0xC6, 0x1B, 0xB9, 0xF5, 0xBA, 0x91, 0x42, 0xD5, 0xB1, 0x01, 0xB5, 0xC8, 0xA2, 0x0A, 0x4B, 0x91, 0x12,
    0xA9, 0x6E, 0x18, 0x23, 0xDC, 0xD5, 0xFE, 0x20, 0x40, 0xC3, 0x2A, 0x0F, 0xED, 0x28, 0x72, 0x74, 0x4B, 0x8E, 0x2B, 0xF5, 0x90, 0xE7, 0x26, 0xE7,
    0xA9, 0x84, 0x31, 0xAE, 0x08, 0x7A, 0x07, 0x48, 0xAE, 0x84, 0x15, 0x0D, 0x30, 0x02, 0x4B, 0xB1, 0xB2, 0x3F, 0x1F, 0xF0, 0xC8, 0x9E, 0x12, 0x00,
    0x63, 0x59, 0x8F, 0xC5, 0x30, 0xE6, 0x99, 0xD7, 0xE7, 0x28, 0x7A, 0x42, 0xBC, 0x46, 0x0B, 0x52, 0x49, 0x5A, 0xA5, 0x27, 0x67, 0xAC, 0x2A, 0xA1,
    0x58, 0x95, 0xE1, 0xC0, 0x41, 0x81, 0xA6, 0x55, 0xAC, 0xC2, 0xE8, 0x80, 0x83, 0x23, 0xBD, 0x2D, 0xF8, 0xA6, 0xC5, 0x25, 0x91, 0x64, 0x25, 0xB4,
    0x44, 0xDA, 0x5B, 0x6E, 0xAD, 0x6A, 0x80, 0x90, 0x18, 0x21, 0xAD, 0x05, 0x8D, 0xD0, 0x91, 0xD7, 0x8D, 0x80, 0xB3, 0x42, 0xC2, 0x1B, 0x49, 0xF2,
    0xEA, 0x50, 0x69, 0x80, 0x63, 0xDB, 0xF3, 0x14, 0x3D, 0x43, 0xD3, 0x76, 0x9F, 0x5B, 0xAB, 0xE6, 0x00, 0xDC, 0x4E, 0x62, 0xA5, 0x45, 0x84, 0x24,
    0x32, 0x37, 0x42, 0x8B, 0x02, 0x19, 0xC7, 0xE0, 0x9A, 0x44, 0x22, 0x29, 0x1C, 0x81, 0x72, 0xD3, 0xF2, 0xF0, 0xCE, 0x62, 0x5B, 0x9A, 0x57, 0xEA,
    0x21, 0x4F, 0xFE, 0xF3, 0x1A, 0xF5, 0xC8, 0x76, 0x15, 0xDD, 0xBE, 0x6E, 0x95, 0x8D, 0x40, 0xA4, 0x4A, 0xAC, 0xCA, 0xB1, 0x6D, 0x79, 0x1E, 0x1E,
    0x2B, 0x32, 0xE8, 0x0A, 0x8D, 0x58, 0xDB, 0x65, 0x38, 0x25, 0xFF, 0xB1, 0xB1, 0x02, 0x8F, 0xEC, 0x29, 0x61, 0x15, 0x42, 0xAB, 0x6C, 0xCF, 0x78,
    0xDC, 0x57, 0x0C, 0xB8, 0x69, 0x2D, 0xDD, 0xEC, 0x09, 0x3A, 0x8A, 0x40, 0x27, 0x70, 0x04, 0x1A, 0xB1, 0xE2, 0x18, 0xE1, 0x89, 0xFB, 0x46, 0x39,
    0x3C, 0x9A, 0x01, 0x60, 0x7C, 0x73, 0x96, 0xAF, 0x9F, 0xBD, 0xCE, 0x62, 0x18, 0x93, 0x75, 0x0D, 0x73, 0x8D, 0x98, 0x87, 0x76, 0x14, 0x78, 0x6C,
    0xEF, 0x70, 0x3B, 0x71, 0x3D, 0x23, 0xD4, 0x22, 0xCB, 0xC5, 0x6A, 0x93, 0xC0, 0x08, 0xDA, 0x45, 0x72, 0xF5, 0x25, 0x02, 0x8E, 0x40, 0x3D, 0x56,
    0xB2, 0x9E, 0xE1, 0x6B, 0xF7, 0x6F, 0xE6, 0xF0, 0x68, 0x86, 0x58, 0x21, 0x52, 0xD8, 0x9D, 0xF3, 0xF9, 0xEA, 0xFD, 0x9B, 0x18, 0xF2, 0x1D, 0xA6,
    0x97, 0x22, 0x3E, 0xF2, 0xAE, 0x02, 0x8F, 0xEF, 0x1B, 0x6E, 0x4B, 0xCA, 0x91, 0xA4, 0x38, 0x3C, 0x7D, 0x6E, 0x86, 0xE9, 0xA5, 0x10, 0xCF, 0x74,
    0x57, 0x9A, 0xD7, 0x1C, 0x01, 0x57, 0xE0, 0x46, 0x64, 0xD9, 0x36, 0xE8, 0xF1, 0xA5, 0x43, 0x9B, 0xD8, 0x91, 0xF1, 0x88, 0x5B, 0x15, 0x25, 0x29,
    0x00, 0xB0, 0x2B, 0xE7, 0xF3, 0xC5, 0x83, 0xF7, 0xF0, 0xD2, 0xF5, 0x1A, 0x8F, 0xED, 0x1D, 0x6E, 0x13, 0x34, 0x92, 0xC8, 0xE7, 0x1B, 0xAF, 0xCE,
    0xF0, 0xF2, 0xFC, 0x12, 0x45, 0xCF, 0x74, 0x5D, 0x96, 0xD7, 0x64, 0x80, 0x23, 0x42, 0xB9, 0x19, 0xF3, 0xEE, 0x42, 0xC0, 0x93, 0xEF, 0xD9, 0xC4,
    0x70, 0xE0, 0x60, 0x5B, 0xE4, 0x53, 0xCD, 0x6B, 0xEB, 0xDA, 0x5F, 0x08, 0xD8, 0x5F, 0x08, 0x92, 0xEF, 0xB4, 0x95, 0x2F, 0x56, 0xF9, 0xE6, 0xB9,
    0x19, 0xFE, 0x3E, 0x57, 0x67, 0xC8, 0x33, 0x44, 0x3D, 0xD4, 0xE5, 0x9E, 0x0D, 0x10, 0x84, 0x4A, 0x18, 0xF3, 0xFE, 0x91, 0x41, 0xBE, 0x70, 0xF0,
    0x1E, 0x32, 0x8E, 0x69, 0xCB, 0xC2, 0x08, 0x9C, 0xB9, 0x5E, 0x43, 0x80, 0x0F, 0x6D, 0xCA, 0x12, 0xB7, 0x08, 0xAB, 0x82, 0x48, 0x72, 0xC5, 0xAA,
    0x3C, 0x7D, 0x7E, 0x86, 0xBF, 0xCE, 0xD6, 0x29, 0xF6, 0x48, 0xBE, 0x67, 0x03, 0x0C, 0xC2, 0x52, 0x6C, 0x39, 0xBA, 0x25, 0xC7, 0x13, 0x07, 0x46,
    0x71, 0x44, 0xDE, 0x44, 0xFE, 0xF7, 0x57, 0xAB, 0x7C, 0xFF, 0xF5, 0x39, 0x8C, 0x24, 0x09, 0x7A, 0x78, 0x34, 0xD3, 0x6A, 0x54, 0x8A, 0x90, 0x24,
    0xE9, 0x77, 0xCE, 0xCF, 0xF2, 0xE7, 0x99, 0xDE, 0x3D, 0x7F, 0x8B, 0x4B, 0x0F, 0x50, 0x14, 0x57, 0x84, 0x4F, 0xEE, 0x2E, 0x61, 0x44, 0x08, 0xED,
    0xAD, 0x46, 0x37, 0x31, 0xB5, 0xC8, 0xF7, 0x5E, 0x9B, 0x65, 0xD0, 0x15, 0x7C, 0x23, 0x7C, 0xEB, 0x5C, 0xE2, 0x65, 0x57, 0x12, 0xC9, 0x01, 0x9C,
    0xFA, 0xF7, 0x0C, 0xA7, 0xAF, 0xD5, 0xD6, 0xE4, 0xF9, 0x14, 0x3D, 0x4A, 0x48, 0xB0, 0x28, 0xD5, 0x28, 0x46, 0x70, 0xF1, 0x5A, 0x82, 0x7F, 0x6E,
    0x72, 0x9E, 0x17, 0x2E, 0x57, 0x28, 0x7A, 0x26, 0x91, 0x0B, 0x09, 0xE9, 0x53, 0xE7, 0x67, 0xF9, 0xF8, 0x58, 0x91, 0x1D, 0x59, 0x8F, 0x3F, 0x4E,
    0x57, 0xF9, 0xDB, 0x6C, 0x9D, 0x21, 0xBF, 0xFB, 0x84, 0xED, 0x9B, 0x01, 0x69, 0x9F, 0x79, 0xF6, 0xC2, 0x3C, 0x9F, 0xD9, 0x53, 0x22, 0xE3, 0x1A,
    0x7E, 0x71, 0x69, 0x91, 0x3F, 0x4D, 0xD7, 0x6E, 0x23, 0x96, 0x76, 0xD5, 0x9F, 0x5E, 0x5C, 0x68, 0x45, 0x49, 0xC8, 0xF7, 0x50, 0x6D, 0xFE, 0x1F,
    0x7A, 0x32, 0xC0, 0xA2, 0x04, 0x46, 0xB8, 0x58, 0x6B, 0xF2, 0xE5, 0x57, 0xAE, 0xB5, 0x64, 0x64, 0x29, 0xBE, 0x8D, 0x57, 0xD3, 0x8F, 0x79, 0xEF,
    0x96, 0x5A, 0x6D, 0x9F, 0xC8, 0xC3, 0x1A, 0xAA, 0x90, 0x02, 0x81, 0x23, 0x58, 0xDB, 0xEA, 0xA6, 0xAE, 0x59, 0x91, 0x58, 0x3F, 0x49, 0x2F, 0x47,
    0xCF, 0x9D, 0x58, 0x48, 0xCA, 0x62, 0xCA, 0xEB, 0x9D, 0x22, 0xB8, 0x1A, 0x7A, 0xDA, 0x8D, 0x42, 0xD2, 0x41, 0xD3, 0xDA, 0xDE, 0x2F, 0xA4, 0x9B,
    0xB8, 0x6E, 0x9C, 0xD1, 0xC1, 0x48, 0x29, 0xD4, 0x5A, 0xB5, 0xCE, 0x90, 0xEC, 0x53, 0xF6, 0xE4, 0x02, 0x76, 0xE5, 0x7C, 0xDE, 0xA8, 0x87, 0x7D,
    0x1D, 0x2B, 0x6D, 0xAB, 0xD1, 0x0D, 0x74, 0xB1, 0x1F, 0x5A, 0xD1, 0x00, 0xAB, 0xE0, 0x1B, 0x61, 0xAA, 0xD6, 0xA4, 0xDC, 0x8C, 0x29, 0xFA, 0x0E,
    0xAA, 0x30, 0x12, 0x38, 0x7C, 0xFB, 0xBD, 0x5B, 0x99, 0xAA, 0x85, 0x7D, 0x1D, 0xEE, 0x97, 0x62, 0xCB, 0x2F, 0x2F, 0x2D, 0xF2, 0xEA, 0x62, 0x83,
    0x41, 0x47, 0x3A, 0x8A, 0xC4, 0xAA, 0x11, 0xF0, 0x0C, 0x2C, 0x34, 0x63, 0x7E, 0xF3, 0x46, 0x85, 0x4F, 0xED, 0x2E, 0x11, 0x03, 0x28, 0x64, 0x5D,
    0xC3, 0x81, 0x62, 0xB0, 0x76, 0xD6, 0x6F, 0xC1, 0xBD, 0x39, 0x9F, 0xCF, 0xFF, 0xE3, 0xBF, 0xDC, 0x8C, 0xB5, 0xA3, 0xC1, 0x66, 0x55, 0x03, 0xE2,
    0x16, 0xD9, 0x5F, 0x5D, 0xAE, 0xB0, 0x33, 0xEB, 0x33, 0xBE, 0x39, 0xDB, 0xFE, 0x2D, 0xF5, 0xD0, 0x5A, 0x25, 0x94, 0x4E, 0x71, 0x06, 0x28, 0xF9,
    0x0E, 0x59, 0xD7, 0x50, 0x8F, 0xA2, 0x76, 0xE7, 0x5E, 0x09, 0x1D, 0x27, 0xB1, 0x6B, 0x84, 0xEF, 0xBE, 0x36, 0xCB, 0x85, 0x4A, 0x83, 0x0F, 0x6F,
    0xCB, 0xB3, 0x3D, 0xE3, 0xE1, 0xF4, 0xF9, 0xE6, 0x90, 0x55, 0xF8, 0xF5, 0xE5, 0x0A, 0xD3, 0x37, 0xA3, 0x8E, 0x25, 0x24, 0x27, 0x5E, 0x9A, 0xEA,
    0xC8, 0x81, 0x29, 0xD7, 0x6A, 0x64, 0xC9, 0xBA, 0x86, 0x2D, 0x83, 0x2E, 0x19, 0xA7, 0x7F, 0x13, 0xA9, 0x08, 0xD4, 0x23, 0x65, 0xAA, 0xD6, 0xC4,
    0x33, 0xD2, 0x71, 0x6E, 0x75, 0xDC, 0xC8, 0x96, 0x77, 0x54, 0xAB, 0x70, 0xA9, 0x16, 0xF6, 0xB5, 0x84, 0x2A, 0xC9, 0x66, 0x30, 0x78, 0xA7, 0x27,
    0xB2, 0x34, 0xAC, 0x81, 0x91, 0xAE, 0x86, 0xEF, 0x4E, 0xB0, 0xBC, 0x31, 0x76, 0x0A, 0x17, 0xA1, 0x2A, 0x98, 0x5C, 0xB7, 0x77, 0xA8, 0x95, 0xFE,
    0x36, 0xB1, 0x6E, 0x21, 0x62, 0x50, 0x6C, 0xD5, 0xA0, 0x4C, 0x1A, 0xCF, 0x53, 0x85, 0xBB, 0xE6, 0x1E, 0xBB, 0x82, 0x35, 0x9E, 0xA7, 0x28, 0x93,
    0x06, 0x31, 0x2F, 0x1A, 0xCF, 0x17, 0xD1, 0xF5, 0xF4, 0x67, 0x77, 0x10, 0x55, 0x35, 0x9E, 0x2F, 0x88, 0xBC, 0x68, 0xD4, 0xC6, 0x3F, 0x0A, 0xEB,
    0xD5, 0x9A, 0xB8, 0xAE, 0x70, 0x37, 0x18, 0x91, 0x3C, 0xE4, 0x93, 0x84, 0xB3, 0xF7, 0x43, 0x33, 0x31, 0xBE, 0xEB, 0x22, 0xC8, 0xA9, 0xA0, 0x38,
    0x6C, 0x80, 0x70, 0xBD, 0xF9, 0x75, 0x80, 0x30, 0xE1, 0xAA, 0xA7, 0x7E, 0xF6, 0x81, 0xAD, 0xFF, 0xB9, 0xEB, 0x1F, 0x74, 0xDF, 0x7E, 0xD4, 0x20,
    0x5F, 0x18, 0x6F, 0x94, 0x37, 0xF2, 0x51, 0x83, 0xF2, 0x69, 0x27, 0x8A, 0xDA, 0x47, 0x0D, 0x0C, 0x22, 0x8A, 0xAA, 0x3C, 0x7F, 0x64, 0xE7, 0x42,
    0xF1, 0xEC, 0x95, 0x07, 0x9A, 0xD5, 0xCA, 0x49, 0x71, 0xDC, 0xB2, 0x97, 0xCB, 0x3B, 0xAE, 0x1F, 0x18, 0x91, 0xF5, 0xE3, 0x2F, 0x62, 0x70, 0xFD,
    0xC0, 0x78, 0xB9, 0xBC, 0x23, 0xAE, 0xB7, 0xD0, 0xAC, 0x56, 0x4E, 0x4E, 0x9E, 0xBD, 0xFA, 0x40, 0x4A, 0x3E, 0x39, 0xEC, 0xD1, 0x36, 0x75, 0xE3,
    0x1E, 0xB7, 0x71, 0x5C, 0xE7, 0xB7, 0x61, 0xD4, 0xF8, 0xC9, 0xDB, 0x1D, 0xB7, 0xF9, 0x1F, 0x06, 0x2D, 0x0E, 0xDA, 0x1B, 0x35, 0x09, 0xE0, 0x00,
    0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
};
static const unsigned char kIcon64[] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xAA, 0x69, 0x71, 0xDE, 0x00, 0x00, 0x09, 0x15, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0xE5, 0x9B, 0x6B, 0x8C, 0x9C,
    0x55, 0x1D, 0xC6, 0x7F, 0xFF, 0xF3, 0xBE, 0xF3, 0xEE, 0xCC, 0xEC, 0xEC, 0x74, 0x76, 0xB7, 0xED, 0xD2, 0x96, 0xCB, 0x16, 0x4A, 0x5B, 0x69, 0x69,
    0x89, 0xC1, 0x40, 0x91, 0x60, 0x1A, 0xA3, 0x10, 0x24, 0x8A, 0x91, 0x5A, 0x2B, 0xE1, 0x83, 0x12, 0x25, 0xA2, 0x31, 0x31, 0xA2, 0x1F, 0xC4, 0x64,
    0xBB, 0x5C, 0xF4, 0x8B, 0x7E, 0x41, 0xBE, 0x11, 0x45, 0xBF, 0xD8, 0x9B, 0xB4, 0x84, 0x68, 0x49, 0x30, 0x42, 0x34, 0x48, 0x13, 0x6E, 0x21, 0x14,
    0xD2, 0x16, 0x2D, 0x76, 0xBB, 0xB4, 0xA5, 0x2D, 0x7B, 0x9B, 0x9D, 0xDD, 0x99, 0x79, 0x2F, 0xE7, 0xEF, 0x87, 0x77, 0x66, 0xBA, 0xDB, 0x1B, 0xDD,
    0xDD, 0x99, 0x69, 0xB7, 0xFB, 0x24, 0xBB, 0x93, 0x79, 0x67, 0x66, 0xF7, 0x3C, 0xFF, 0xCB, 0xF3, 0x3E, 0xE7, 0x9C, 0x39, 0xC2, 0xE9, 0x50, 0x95,
    0x9E, 0xCD, 0x48, 0x6F, 0xAF, 0xD8, 0x1E, 0x55, 0xF3, 0x9F, 0x37, 0x4E, 0xDE, 0x69, 0x6D, 0x74, 0xAF, 0x46, 0x76, 0x9D, 0xDA, 0xA8, 0x1B, 0xD5,
    0x0C, 0x20, 0x67, 0x7C, 0xEE, 0x52, 0x81, 0xC8, 0xA8, 0x18, 0xE7, 0x90, 0xB8, 0xEE, 0x6B, 0xE0, 0x3E, 0xBF, 0x75, 0x77, 0xC7, 0x4B, 0xF4, 0x8A,
    0xED, 0xE9, 0x51, 0xD3, 0xBB, 0x19, 0x45, 0x44, 0x27, 0xBD, 0x7D, 0xE2, 0x93, 0x1E, 0x55, 0xD3, 0x2B, 0x62, 0x01, 0x36, 0xEE, 0x39, 0xB6, 0xD1,
    0x20, 0x8F, 0x28, 0x7A, 0xB3, 0x49, 0x24, 0xB0, 0x41, 0x80, 0x46, 0x21, 0x6A, 0x6D, 0x33, 0xE9, 0x4C, 0x19, 0x62, 0x0C, 0xE2, 0xB8, 0x54, 0xC7,
    0x8C, 0xC8, 0x1B, 0xAA, 0xFA, 0xDB, 0x6D, 0xEB, 0x16, 0x6D, 0x83, 0xC9, 0x1C, 0x61, 0x42, 0x00, 0xAA, 0x2F, 0x6C, 0x7A, 0xE5, 0xC8, 0x7C, 0x4D,
    0x9A, 0x67, 0xDC, 0x54, 0xFA, 0x5E, 0xEB, 0x97, 0x09, 0x8B, 0xE3, 0x8A, 0x60, 0x15, 0x11, 0x01, 0x73, 0x31, 0x48, 0x4D, 0x15, 0x0A, 0x56, 0x50,
    0x45, 0x31, 0x6E, 0xAA, 0x55, 0x8C, 0xE7, 0x11, 0x95, 0x4A, 0xBB, 0x28, 0x0E, 0x7D, 0x7F, 0xCB, 0xFA, 0x95, 0x9F, 0x4C, 0x0C, 0x82, 0xC0, 0x29,
    0xF2, 0x1B, 0xF7, 0x1C, 0xEB, 0x16, 0x91, 0x17, 0xDD, 0x64, 0x6A, 0xA5, 0x9F, 0x1F, 0x0A, 0x55, 0x31, 0x22, 0x32, 0x2B, 0x48, 0x9F, 0x0B, 0xAA,
    0x6A, 0x45, 0xB0, 0x5E, 0xB6, 0xDD, 0x8D, 0x4A, 0xC5, 0x7D, 0x56, 0xF5, 0xEE, 0x6D, 0xEB, 0x16, 0x1D, 0xAA, 0x72, 0x16, 0x7A, 0xD4, 0xC0, 0x66,
    0x1E, 0xB8, 0xFB, 0x7B, 0xED, 0x41, 0x64, 0xFE, 0xED, 0xB4, 0x24, 0x57, 0xF8, 0x85, 0x7C, 0x20, 0x22, 0x89, 0x8B, 0x3D, 0xF8, 0x7A, 0x42, 0x55,
    0x03, 0x2F, 0x93, 0x4D, 0x44, 0xE5, 0xD2, 0x81, 0xD6, 0x6C, 0xFA, 0xB6, 0x2B, 0x77, 0xCC, 0x1B, 0xEE, 0x05, 0xCC, 0x86, 0x55, 0x08, 0xBD, 0xBD,
    0xB6, 0x54, 0x0E, 0x9F, 0x75, 0xD3, 0x99, 0x15, 0xFE, 0xE8, 0xE5, 0x47, 0x1E, 0x40, 0x44, 0x12, 0xFE, 0x68, 0x3E, 0x70, 0xD3, 0x99, 0x15, 0xF9,
    0x81, 0xE1, 0x3F, 0xF6, 0xF6, 0x8A, 0xDD, 0xB0, 0x0A, 0x11, 0x80, 0x4D, 0xAF, 0x1E, 0xFD, 0x86, 0x93, 0xC9, 0xFC, 0xC5, 0xCF, 0x0F, 0x86, 0x20,
    0xEE, 0xC5, 0x1E, 0x6C, 0x63, 0xA1, 0xA1, 0x97, 0x6D, 0x77, 0xA3, 0x42, 0xFE, 0xBE, 0x2D, 0xB7, 0x5F, 0xF5, 0x9C, 0x7C, 0xE1, 0x15, 0x75, 0x17,
    0x9A, 0xC3, 0x6F, 0x26, 0x52, 0xE9, 0x35, 0x61, 0x69, 0xDC, 0x02, 0xCE, 0xC5, 0x1E, 0x62, 0x83, 0x11, 0xB9, 0xA9, 0xB4, 0x09, 0xC6, 0xC7, 0xDF,
    0x3D, 0x61, 0xAF, 0xBE, 0xD9, 0xED, 0xF2, 0xFA, 0xBF, 0xE8, 0x18, 0x6F, 0x6D, 0x50, 0x2C, 0xA8, 0x88, 0xB9, 0xDC, 0xC9, 0x03, 0x38, 0xC1, 0x78,
    0x41, 0x1D, 0x2F, 0xB9, 0xB6, 0xCB, 0xF6, 0x7F, 0xD1, 0x88, 0x95, 0xFB, 0x4C, 0xC2, 0x43, 0x26, 0xDC, 0x1B, 0x2F, 0x77, 0x88, 0x88, 0x75, 0x12,
    0x1E, 0x62, 0xE5, 0x3E, 0xA3, 0xAA, 0xEB, 0xA2, 0xD0, 0x87, 0x8A, 0x1E, 0xCC, 0x0D, 0x88, 0x44, 0xA1, 0x8F, 0xAA, 0xAE, 0x33, 0x88, 0x5E, 0xA3,
    0x61, 0x08, 0xB3, 0xC4, 0xE4, 0xD4, 0x09, 0x46, 0xC3, 0x10, 0x44, 0xAF, 0x71, 0x51, 0x32, 0xAA, 0x73, 0xA6, 0xFA, 0x6B, 0xA8, 0x58, 0xFA, 0xCC,
    0x5C, 0xCA, 0xFA, 0x59, 0x31, 0xAD, 0x00, 0x08, 0x60, 0x64, 0xF2, 0xE3, 0x6C, 0xC5, 0x94, 0x4D, 0x8F, 0x00, 0x91, 0x42, 0x29, 0xB4, 0xB4, 0x38,
    0x82, 0x1F, 0x29, 0xAE, 0x08, 0x9E, 0x11, 0xF4, 0x53, 0x3F, 0x7D, 0xE9, 0x61, 0x4A, 0x01, 0x88, 0xC9, 0x2B, 0x49, 0xC7, 0xF0, 0x9D, 0xEB, 0x3A,
    0x59, 0x91, 0x6D, 0xE1, 0x58, 0x31, 0xE0, 0xCF, 0x87, 0x86, 0xF9, 0xB8, 0x18, 0xE2, 0x39, 0x82, 0xCE, 0xB2, 0x28, 0x4C, 0x2D, 0x00, 0x02, 0x41,
    0x04, 0x3F, 0xF9, 0x4C, 0x27, 0xB7, 0x2D, 0x48, 0x03, 0x70, 0x5D, 0x9B, 0xC7, 0xD2, 0x8C, 0xC7, 0xCF, 0xDE, 0x3E, 0x46, 0xA8, 0x71, 0x4F, 0xCD,
    0xA6, 0x18, 0x5C, 0xB0, 0x06, 0x08, 0x10, 0x58, 0x65, 0x7E, 0xD2, 0xE1, 0xB3, 0x1D, 0x29, 0xAC, 0xC6, 0xAD, 0x10, 0x29, 0x2C, 0x49, 0x27, 0x58,
    0xD6, 0xD6, 0x42, 0x39, 0xB2, 0x0D, 0x75, 0x13, 0x8D, 0xD0, 0x9C, 0x29, 0x89, 0xA0, 0x02, 0x06, 0x41, 0x51, 0x8C, 0xC4, 0x83, 0xA9, 0x66, 0x3C,
    0xD1, 0x60, 0x0D, 0x30, 0x02, 0x81, 0x2A, 0xC5, 0x50, 0x89, 0x34, 0x7E, 0x5E, 0x97, 0xBF, 0x3B, 0xD5, 0x0F, 0x9C, 0x8D, 0xA4, 0x9C, 0xE3, 0x7A,
    0xBD, 0x60, 0x04, 0xC6, 0x42, 0x4B, 0xA7, 0xE7, 0xB2, 0x72, 0x5E, 0x0B, 0x29, 0x57, 0x28, 0x86, 0x5A, 0x97, 0x4A, 0xB8, 0x28, 0x53, 0xDF, 0x89,
    0x65, 0x6C, 0xF5, 0xFC, 0xC1, 0x73, 0x04, 0x46, 0x02, 0xCB, 0x3D, 0x4B, 0xB2, 0x3C, 0x70, 0x6D, 0x8E, 0x8C, 0x6B, 0x38, 0x5E, 0x0A, 0x79, 0x6A,
    0xFF, 0x27, 0xBC, 0x3F, 0x5C, 0x26, 0xE5, 0x0A, 0x76, 0x06, 0xD1, 0x6F, 0xBA, 0x11, 0xAA, 0x66, 0x33, 0x1F, 0x58, 0x46, 0x02, 0x4B, 0xA0, 0x7A,
    0xCE, 0x72, 0x76, 0x04, 0x86, 0x7D, 0xCB, 0x57, 0xAF, 0xCC, 0xF2, 0x83, 0xE5, 0x1D, 0x64, 0x5C, 0x43, 0xA4, 0xD0, 0x95, 0x74, 0x79, 0x78, 0x79,
    0x27, 0x9E, 0x33, 0x33, 0xF2, 0xD0, 0xC4, 0x0A, 0x90, 0xCA, 0xAF, 0x42, 0x60, 0xB9, 0x75, 0x41, 0x9A, 0x5B, 0xE7, 0xA7, 0x19, 0x0D, 0x2C, 0xBB,
    0x8F, 0x8E, 0x72, 0xBC, 0x18, 0xD2, 0xEA, 0x0A, 0xD1, 0x04, 0x32, 0x55, 0xF2, 0xF7, 0x5C, 0xD9, 0xC6, 0x43, 0xD7, 0x77, 0x60, 0x35, 0xBE, 0x0B,
    0x19, 0x89, 0xAB, 0x66, 0x41, 0xD2, 0xA5, 0xC3, 0x73, 0x38, 0x59, 0x0E, 0x49, 0xC8, 0xF4, 0xF5, 0xA7, 0x29, 0x01, 0xA8, 0x26, 0x78, 0x34, 0xB0,
    0x7C, 0xBB, 0x3B, 0xC7, 0xFD, 0x4B, 0x73, 0xB5, 0xD7, 0x3E, 0xBF, 0x20, 0xCD, 0x63, 0x7B, 0x4F, 0xD0, 0x37, 0xE6, 0xD7, 0x32, 0xEC, 0x08, 0x0C,
    0x07, 0x96, 0xBB, 0x97, 0xB4, 0xF1, 0xF0, 0xF2, 0xCE, 0x1A, 0xF9, 0xAA, 0x09, 0x73, 0x04, 0xFA, 0x0B, 0x01, 0x9F, 0xCC, 0x90, 0x3C, 0x34, 0xA1,
    0x05, 0x8C, 0x80, 0x05, 0x8A, 0x91, 0xF2, 0xF0, 0xF2, 0x4E, 0xEE, 0x5F, 0x9A, 0xAB, 0xDD, 0x42, 0xC3, 0x4A, 0x26, 0x37, 0xAF, 0xE9, 0xE2, 0xEA,
    0x56, 0x8F, 0x42, 0x68, 0xF1, 0x8C, 0x30, 0xEC, 0x5B, 0xEE, 0x5C, 0x94, 0xE1, 0x47, 0x2B, 0x3A, 0xD1, 0x09, 0xE4, 0xAD, 0x56, 0x35, 0x21, 0xE2,
    0x77, 0x07, 0x06, 0x88, 0x74, 0xE6, 0x93, 0xF8, 0x86, 0x06, 0xC0, 0x48, 0xEC, 0x1D, 0xAC, 0x2A, 0x8F, 0xDC, 0x30, 0x9F, 0xAF, 0x2C, 0x69, 0xAB,
    0xDD, 0xC2, 0x1C, 0x01, 0xB7, 0x52, 0xCE, 0x9D, 0x2D, 0x0E, 0xBD, 0x6B, 0xBA, 0xE8, 0x6E, 0xF5, 0x38, 0x52, 0x0C, 0xF8, 0xD2, 0xA2, 0x0C, 0x3F,
    0x5E, 0x39, 0x3F, 0xCE, 0xEC, 0x04, 0xF2, 0x46, 0xE2, 0x2A, 0x7A, 0xEC, 0xDD, 0x13, 0x1C, 0x2C, 0x94, 0x49, 0x5E, 0xCA, 0x1A, 0xE0, 0x08, 0x94,
    0x22, 0x25, 0xE5, 0x18, 0x7E, 0xBE, 0x6A, 0x01, 0x6B, 0xDB, 0x93, 0xB5, 0xF2, 0x85, 0x58, 0xF9, 0xAB, 0xC6, 0xA6, 0x1A, 0x84, 0x47, 0x6F, 0x5C,
    0xC8, 0x5F, 0x3F, 0xCA, 0xF3, 0xDD, 0x65, 0x1D, 0xB5, 0x5B, 0xEB, 0x44, 0xF2, 0x85, 0xD0, 0xF2, 0xD8, 0xDE, 0xE3, 0x7C, 0x90, 0x2F, 0xD3, 0x96,
    0x30, 0x93, 0x34, 0x63, 0xBA, 0x68, 0x48, 0x00, 0x9C, 0x8A, 0xD2, 0x2F, 0x48, 0xBA, 0xFC, 0x62, 0xF5, 0x42, 0xAE, 0xCD, 0x78, 0x93, 0xC8, 0x57,
    0x09, 0x55, 0x61, 0x24, 0x26, 0xDB, 0x95, 0x74, 0x79, 0x70, 0x59, 0x47, 0xED, 0xFA, 0x44, 0xF2, 0x63, 0xA1, 0xE5, 0xF1, 0x77, 0x4F, 0xB0, 0x7F,
    0xA4, 0x4C, 0x36, 0x61, 0x08, 0xEB, 0x64, 0x3C, 0xEA, 0xDE, 0x02, 0x8E, 0x08, 0xA3, 0x81, 0xA5, 0x3B, 0xE3, 0xF1, 0xE4, 0x4D, 0x57, 0x9C, 0x93,
    0xFC, 0xF1, 0x52, 0x08, 0xC4, 0xC4, 0xAB, 0x99, 0xD6, 0xCA, 0xEB, 0x55, 0x54, 0xDF, 0x5B, 0x8C, 0x2C, 0x4F, 0xEC, 0x3D, 0xC1, 0xFB, 0x23, 0xA5,
    0xBA, 0x92, 0x87, 0x3A, 0x07, 0xC0, 0x88, 0x50, 0x08, 0x2C, 0x6B, 0xDA, 0x93, 0x3C, 0xB1, 0xF6, 0x0A, 0xBA, 0x92, 0x6E, 0x4D, 0xB8, 0x80, 0x5A,
    0xFF, 0xBF, 0x35, 0x58, 0xE4, 0x87, 0xAF, 0x1F, 0xE1, 0x4F, 0x1F, 0x0E, 0xC5, 0xC4, 0x75, 0x72, 0x4B, 0x50, 0xB9, 0x66, 0x2A, 0x6D, 0xF4, 0xE4,
    0xDE, 0x93, 0xEC, 0x1D, 0xAE, 0x3F, 0x79, 0xA8, 0x63, 0x0B, 0x18, 0xA0, 0x18, 0x5A, 0x6E, 0x99, 0x9F, 0xE6, 0x97, 0x37, 0x2E, 0xC4, 0x33, 0x32,
    0xA9, 0xD4, 0xAB, 0x55, 0xF0, 0x8F, 0x8F, 0x0B, 0x3C, 0x7D, 0x60, 0x00, 0x57, 0x84, 0xED, 0x7D, 0x23, 0x18, 0xE0, 0x81, 0x6B, 0xDB, 0xE3, 0xCC,
    0x9F, 0x26, 0x78, 0xBE, 0x55, 0x7E, 0xF5, 0xDE, 0x09, 0xDE, 0x19, 0x2A, 0x92, 0xAD, 0x53, 0xCF, 0x9F, 0x8E, 0xBA, 0x6A, 0x80, 0x02, 0xDF, 0xEA,
    0xCE, 0xE1, 0x19, 0xA9, 0x11, 0x56, 0xE2, 0x6C, 0x3A, 0x02, 0x3B, 0x0F, 0x8F, 0xF0, 0xEC, 0xC1, 0x21, 0xD2, 0xAE, 0xC1, 0x08, 0x64, 0x8D, 0x61,
    0x4B, 0xDF, 0x08, 0x10, 0x07, 0x01, 0x4E, 0x91, 0x0F, 0xAC, 0xF2, 0xEB, 0xF7, 0x4E, 0xF0, 0xD6, 0x60, 0x91, 0x79, 0x0D, 0x22, 0x0F, 0x75, 0x0C,
    0x80, 0x02, 0xAE, 0x08, 0x6D, 0x15, 0x33, 0xA3, 0x0A, 0x11, 0xD4, 0x66, 0x8D, 0x7F, 0x38, 0x38, 0xC4, 0x73, 0x87, 0x47, 0x68, 0x4B, 0x98, 0x78,
    0xFF, 0xBA, 0x42, 0x68, 0x5E, 0xC2, 0xB0, 0xB5, 0x6F, 0x84, 0x50, 0x61, 0x53, 0x77, 0x8E, 0xA4, 0x23, 0x0C, 0xF9, 0x11, 0x4F, 0x1F, 0x18, 0xE0,
    0xF5, 0x81, 0x22, 0xB9, 0x06, 0x94, 0xFD, 0x44, 0xD4, 0xAF, 0x05, 0x04, 0x7C, 0x6B, 0x79, 0xF5, 0xE4, 0x18, 0x9B, 0xBA, 0x73, 0x35, 0xFB, 0x17,
    0xA9, 0xF2, 0xD4, 0xFE, 0x01, 0xFE, 0x7E, 0xAC, 0x40, 0xCE, 0x3B, 0x33, 0x93, 0x56, 0x21, 0x9B, 0x30, 0xEC, 0xEA, 0x1F, 0xE1, 0xCD, 0x81, 0x22,
    0x0B, 0x92, 0x0E, 0x87, 0xC7, 0x02, 0x06, 0xCA, 0x51, 0x43, 0x7A, 0xFE, 0x74, 0xD4, 0x2D, 0x00, 0x91, 0x42, 0xDA, 0x35, 0xEC, 0xEA, 0xCF, 0x13,
    0x29, 0xDC, 0x3A, 0x3F, 0x4D, 0x21, 0x88, 0xD8, 0xD9, 0x9F, 0xE7, 0xAD, 0xC1, 0xE2, 0x59, 0xC9, 0x57, 0x61, 0x15, 0x5A, 0x5D, 0xC3, 0xB1, 0x62,
    0x40, 0xFF, 0xB8, 0x8F, 0x67, 0x0C, 0xE9, 0x19, 0xCE, 0xF2, 0x2E, 0x14, 0x75, 0xF7, 0x01, 0x8E, 0xC0, 0xB6, 0x43, 0xC3, 0xEC, 0xEA, 0x1F, 0x21,
    0xAA, 0x6C, 0x37, 0x5C, 0x88, 0x80, 0x59, 0x05, 0xCF, 0x08, 0x2D, 0x12, 0xAF, 0x2B, 0x36, 0x83, 0x3C, 0x34, 0xC8, 0x08, 0x65, 0x12, 0x06, 0x55,
    0x48, 0xB8, 0xA7, 0x54, 0xFD, 0x42, 0x50, 0x15, 0xCC, 0x66, 0xA2, 0x21, 0x01, 0xA8, 0x11, 0xFE, 0x94, 0xC5, 0x8E, 0x4B, 0x01, 0x73, 0x7E, 0x67,
    0x68, 0x5A, 0x1B, 0x23, 0x67, 0x43, 0xD5, 0xB9, 0xC9, 0x79, 0xDE, 0xD3, 0x08, 0xCC, 0xB4, 0xC2, 0xA6, 0xBC, 0x31, 0x12, 0xAA, 0x4E, 0x12, 0xB4,
    0xEA, 0x57, 0x4A, 0xBA, 0x92, 0x0E, 0xA3, 0x81, 0xAD, 0x99, 0xA0, 0x66, 0xC0, 0x20, 0xB8, 0x66, 0x66, 0x82, 0x79, 0xC1, 0x01, 0x50, 0x4E, 0x4D,
    0x74, 0x06, 0xCB, 0x11, 0xAD, 0xAE, 0xA9, 0x65, 0x1D, 0x60, 0x63, 0x77, 0x8E, 0x01, 0x3F, 0xE2, 0x7F, 0x05, 0x3F, 0xBE, 0xA6, 0x8D, 0x5D, 0x26,
    0x17, 0x89, 0xAD, 0xF7, 0x48, 0x10, 0x91, 0x76, 0x4C, 0x73, 0x96, 0xC4, 0x9C, 0xCA, 0x9C, 0xFC, 0xED, 0xC1, 0x22, 0x57, 0xB7, 0x26, 0xB0, 0x9C,
    0x12, 0x91, 0x76, 0xCF, 0xE1, 0xD1, 0xD5, 0x0B, 0x29, 0x46, 0xD5, 0xF9, 0x5D, 0x63, 0xA1, 0x0A, 0x63, 0x91, 0x65, 0xE7, 0xE1, 0x3C, 0xBB, 0x8F,
    0x8C, 0x92, 0x72, 0xA7, 0xB7, 0x2D, 0x37, 0xA5, 0x00, 0x58, 0x85, 0xA4, 0x23, 0xBC, 0x78, 0x74, 0x94, 0xBB, 0x16, 0xB7, 0xD1, 0x52, 0xD9, 0x0B,
    0x14, 0x39, 0x45, 0x39, 0xE5, 0x34, 0x4F, 0x05, 0xD2, 0xAE, 0xE1, 0xA1, 0xEB, 0x3B, 0x38, 0x54, 0xF0, 0xD9, 0x97, 0x2F, 0x93, 0x9A, 0xC6, 0x0A,
    0xD1, 0x94, 0x77, 0x86, 0x5A, 0x8C, 0x70, 0x74, 0x3C, 0xE0, 0x99, 0xFF, 0x0E, 0xC6, 0x82, 0x27, 0xD4, 0x7A, 0xBE, 0x3A, 0xA7, 0x6F, 0xD6, 0x4F,
    0x60, 0xE3, 0xA4, 0xAC, 0xCA, 0x25, 0xF1, 0xED, 0xF4, 0x36, 0x4A, 0xA6, 0x7C, 0x17, 0x88, 0x34, 0x36, 0x3A, 0x2F, 0x1D, 0x1D, 0xC5, 0x15, 0x78,
    0x70, 0x59, 0x07, 0xDE, 0xC4, 0xE5, 0x9D, 0x66, 0x08, 0xA0, 0xC4, 0x2D, 0xE0, 0x9A, 0x38, 0xE8, 0xFD, 0x63, 0x3E, 0xAE, 0x4C, 0xEF, 0x5F, 0x4F,
    0xCB, 0x08, 0xD9, 0x4A, 0x10, 0xFE, 0x76, 0x64, 0x94, 0x0F, 0xF2, 0x3E, 0x5F, 0xBB, 0xAA, 0x8D, 0x9B, 0xDA, 0x53, 0xE4, 0x3C, 0xA7, 0x69, 0x5F,
    0xB5, 0x92, 0x4A, 0x10, 0x5E, 0x3C, 0x36, 0xCA, 0x1B, 0x03, 0x45, 0xD2, 0x8E, 0x99, 0xD6, 0xDD, 0x40, 0xBE, 0xF9, 0xAF, 0xBE, 0x69, 0xE7, 0xCC,
    0x91, 0x78, 0xB9, 0x3B, 0xB4, 0x4A, 0x67, 0x8B, 0xCB, 0x15, 0x29, 0x97, 0xB4, 0xDB, 0x04, 0x6F, 0x55, 0xD1, 0x9D, 0x21, 0x3F, 0xE2, 0xC3, 0x51,
    0x1F, 0xCF, 0x91, 0x69, 0xAB, 0xCE, 0x8C, 0xAC, 0x70, 0xA4, 0xD0, 0xE2, 0x08, 0x49, 0x47, 0x18, 0x0B, 0x2D, 0xFB, 0x47, 0xCA, 0xD8, 0x26, 0x9A,
    0x5F, 0x47, 0x84, 0xE4, 0x34, 0xD5, 0xBF, 0x8A, 0x19, 0xCF, 0x05, 0xAA, 0xEB, 0x79, 0x8E, 0x80, 0xEB, 0x36, 0xD7, 0x07, 0xD6, 0x63, 0xF2, 0x54,
    0xD7, 0x15, 0xA1, 0xD9, 0xF6, 0xF5, 0x18, 0x88, 0xDD, 0x64, 0x41, 0xCC, 0xDC, 0x9B, 0x13, 0x89, 0x31, 0x20, 0x14, 0x0C, 0x2A, 0x7D, 0xE2, 0xBA,
    0x10, 0xDB, 0xFA, 0xB9, 0x02, 0x2B, 0xAE, 0x0B, 0x2A, 0x7D, 0x46, 0x44, 0xF6, 0x38, 0xAE, 0xC7, 0xEC, 0x2C, 0xE0, 0xE9, 0x42, 0xD5, 0x71, 0x3D,
    0x44, 0x64, 0x8F, 0x41, 0xCC, 0x8E, 0x28, 0xF0, 0x51, 0xD5, 0xB9, 0xD3, 0x07, 0xAA, 0x26, 0x0A, 0x7C, 0x10, 0xB3, 0xC3, 0x7C, 0x1C, 0x2C, 0x7E,
    0xD9, 0x06, 0xFE, 0x3B, 0x89, 0x74, 0x06, 0xE2, 0x95, 0xEC, 0xCB, 0x1D, 0x91, 0x9B, 0xCE, 0x60, 0x03, 0xFF, 0x9D, 0x95, 0xC1, 0xE2, 0x97, 0xCD,
    0x3F, 0xD7, 0x4B, 0xE8, 0x18, 0xF3, 0x84, 0x49, 0x78, 0x32, 0x37, 0xDA, 0x40, 0xD5, 0x24, 0x5A, 0x44, 0x12, 0x89, 0xC7, 0x7B, 0xD7, 0x4B, 0x68,
    0x36, 0x6C, 0x57, 0x67, 0xCB, 0xED, 0x57, 0x3D, 0xE7, 0xE7, 0x47, 0x5E, 0xF0, 0xB2, 0x1D, 0xAE, 0x5A, 0x0D, 0x2E, 0xF6, 0x10, 0x1B, 0x05, 0xB5,
    0x1A, 0x78, 0xD9, 0x0E, 0xD7, 0xCF, 0x0F, 0xBE, 0xB0, 0x6D, 0xDD, 0xE2, 0x9D, 0x1B, 0xB6, 0xAB, 0x23, 0xF4, 0xA8, 0xE9, 0x01, 0x3E, 0xDA, 0x30,
    0x92, 0x1B, 0xCB, 0x8F, 0xBF, 0x76, 0xB9, 0x1F, 0x9B, 0x0B, 0xCB, 0xA5, 0x03, 0x36, 0x9B, 0xBE, 0xED, 0x86, 0xEA, 0xB1, 0x39, 0x7A, 0xC5, 0xB2,
    0x19, 0x7E, 0xBF, 0x3A, 0x37, 0xA8, 0x22, 0x77, 0xD9, 0xD0, 0xDF, 0xD7, 0x32, 0xAF, 0x3D, 0x01, 0x1A, 0xEA, 0x65, 0x70, 0x90, 0x20, 0xE6, 0xA0,
    0x61, 0xCB, 0xBC, 0xF6, 0x84, 0x0D, 0xFC, 0x7D, 0x94, 0xCB, 0x77, 0xED, 0x58, 0x9D, 0x1B, 0x64, 0x33, 0xD0, 0x2B, 0xD6, 0x00, 0xF4, 0x4A, 0x7C,
    0x50, 0x7A, 0xDB, 0xBA, 0x45, 0x87, 0x18, 0x1F, 0xBE, 0x23, 0x2A, 0x95, 0x76, 0x79, 0xD9, 0x76, 0x37, 0x91, 0x4A, 0x1B, 0x54, 0x15, 0x34, 0xD2,
    0x59, 0xE4, 0x13, 0xE2, 0xB1, 0x6A, 0x84, 0xAA, 0x26, 0x52, 0xAD, 0xC6, 0xCB, 0xB6, 0xBB, 0x41, 0x69, 0x7C, 0x17, 0xC5, 0xE1, 0x3B, 0xB6, 0xAD,
    0x5F, 0x7A, 0xE8, 0x8C, 0xA3, 0xB3, 0x55, 0x9C, 0x71, 0x78, 0x5A, 0xCC, 0x4F, 0x55, 0xED, 0xE7, 0x66, 0xFB, 0xE1, 0x69, 0x44, 0x7F, 0xB3, 0xF5,
    0x96, 0x45, 0xDB, 0xE1, 0x3C, 0x87, 0xA7, 0x6B, 0x38, 0xED, 0xF8, 0xFC, 0xC1, 0xB7, 0x07, 0xBF, 0x1C, 0x06, 0xFE, 0xD7, 0x67, 0xE7, 0xF1, 0x79,
    0x9E, 0xDF, 0xBA, 0x7B, 0xE1, 0x79, 0x8F, 0xCF, 0xFF, 0x1F, 0x51, 0x38, 0x81, 0x51, 0x03, 0x92, 0x95, 0x27, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
    0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
};

struct IconPng {
    int px;
    const unsigned char* data;
    size_t size;
};
static const IconPng kIcons[] = {{16, kIcon16, sizeof(kIcon16)}, {24, kIcon24, sizeof(kIcon24)},
                                 {32, kIcon32, sizeof(kIcon32)}, {48, kIcon48, sizeof(kIcon48)},
                                 {64, kIcon64, sizeof(kIcon64)}};

static HICON MakeAppIcon(int want) {
    if (want <= 0) want = 32;
    const IconPng* pick = &kIcons[4];
    for (const IconPng& ic : kIcons) {
        if (ic.px >= want) {
            pick = &ic;
            break;
        }
    }
    return CreateIconFromResourceEx(const_cast<PBYTE>(pick->data), (DWORD)pick->size, TRUE, 0x00030000,
                                    want, want, LR_DEFAULTCOLOR);
}

static DWORD WINAPI ThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    try {
        s_mgr = wuxh::WindowsXamlManager::InitializeForCurrentThread();
    } catch (...) {
        winrt::uninit_apartment();
        if (!g_unloading)
            MessageBoxW(nullptr, L"TrafficMonitor couldn't open its settings window (XAML Islands failed to start).",
                        L"TrafficMonitor", MB_OK | MB_ICONERROR);
        return 0;
    }
    if (g_unloading) {
        try { s_mgr.Close(); } catch (...) {}
        s_mgr = nullptr;
        winrt::uninit_apartment();
        return 0;
    }

    if (!s_module) s_module = ThisModule();
    if (!s_class[0]) _snwprintf(s_class, 64, L"TMXamlSettingsWnd_%p_%llx", (void*)s_module, (unsigned long long)GetTickCount64());
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = s_module;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = s_class;
    if (!s_classRegistered && RegisterClassExW(&wc)) s_classRegistered = true;

    HDC sdc = GetDC(nullptr);
    int dpi = sdc ? GetDeviceCaps(sdc, LOGPIXELSX) : 96;
    if (sdc) ReleaseDC(nullptr, sdc);
    float scale = (dpi > 0 ? dpi : 96) / 96.0f;
    int width = (int)(980 * scale), height = (int)(720 * scale);
    POINT pt{};
    GetCursorPos(&pt);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &mi);
    int x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - width) / 2;
    int y = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - height) / 2;

    s_light = IsSystemLight();
    // A XAML Island is opaque (black where it is transparent) unless the host window has no
    // redirection bitmap: then the island's transparent areas show the DWM backdrop (acrylic).
    const DWORD exStyle = WS_EX_APPWINDOW | (SystemBackdropSupported() ? WS_EX_NOREDIRECTIONBITMAP : 0);
    s_hwnd = CreateWindowExW(exStyle, s_class, TR(L"Ajustes de TrafficMonitor", L"TrafficMonitor settings"),
                             WS_OVERLAPPEDWINDOW, x, y, width, height, nullptr, nullptr, s_module, nullptr);
    if (!s_hwnd) {
        try { s_mgr.Close(); } catch (...) {}
        s_mgr = nullptr;
        winrt::uninit_apartment();
        return 0;
    }
    RefreshPanelBackdrop();

    // Own taskbar identity: without it Windows groups this window (it belongs to
    // explorer.exe) under the File Explorer button and shows it as "File Explorer".
    {
        winrt::com_ptr<IPropertyStore> ps;
        if (SUCCEEDED(SHGetPropertyStoreForWindow(s_hwnd, IID_PPV_ARGS(ps.put()))) && ps) {
            PROPVARIANT pv{};
            pv.vt = VT_LPWSTR;
            pv.pwszVal = const_cast<wchar_t*>(L"TrafficMonitorTaskbar.Settings");
            ps->SetValue(PKEY_AppUserModel_ID, pv);
            PROPVARIANT pin{};
            pin.vt = VT_BOOL;
            pin.boolVal = VARIANT_TRUE;
            ps->SetValue(PKEY_AppUserModel_PreventPinning, pin);
            ps->Commit();
        }
    }
    HICON iconBig = MakeAppIcon(GetSystemMetrics(SM_CXICON)), iconSmall = MakeAppIcon(GetSystemMetrics(SM_CXSMICON));
    if (iconBig) SendMessageW(s_hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(iconBig));
    if (iconSmall) SendMessageW(s_hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(iconSmall));

    try {
        s_src = wuxh::DesktopWindowXamlSource();
        auto native = s_src.as<IDesktopWindowXamlSourceNative>();
        native->AttachToWindow(s_hwnd);
        native->get_WindowHandle(&s_island);
        RECT rc{};
        GetClientRect(s_hwnd, &rc);
        if (s_island) SetWindowPos(s_island, nullptr, 0, 0, rc.right, rc.bottom, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);

        s_applyTimer = wux::DispatcherTimer();
        s_applyTimer.Interval(std::chrono::milliseconds(180));
        s_applyTimer.Tick([](IInspectable const&, IInspectable const&) {
            s_applyTimer.Stop();
            PushSettingsToTaskbar();
        });
        {
            const int p = s_reqPage.exchange(-1);
            s_sel = (p >= 0 && p < kCatCount) ? p : 0;
        }
        BuildPanelUi();
    } catch (...) {
        DestroyWindow(s_hwnd);
        if (iconBig) DestroyIcon(iconBig);
        if (iconSmall) DestroyIcon(iconSmall);
        try { s_mgr.Close(); } catch (...) {}
        s_mgr = nullptr;
        winrt::uninit_apartment();
        if (!g_unloading)
            MessageBoxW(nullptr, L"TrafficMonitor couldn't build its settings window.", L"TrafficMonitor", MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(s_hwnd, SW_SHOW);
    UpdateWindow(s_hwnd);
    SetForegroundWindow(s_hwnd);

    auto native2 = s_src ? s_src.try_as<IDesktopWindowXamlSourceNative2>() : nullptr;
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        BOOL processed = FALSE;
        if (native2) native2->PreTranslateMessage(&msg, &processed);
        if (!processed) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    native2 = nullptr;
    if (s_hwnd && IsWindow(s_hwnd)) DestroyWindow(s_hwnd);
    if (iconBig) DestroyIcon(iconBig);
    if (iconSmall) DestroyIcon(iconSmall);
    s_applyTimer = nullptr;
    try { if (s_mgr) s_mgr.Close(); } catch (...) {}
    s_mgr = nullptr;
    winrt::uninit_apartment();
    return 0;
}

// Runs on the taskbar's thread; the window lives on its own thread.
static void Show(int page) {
    std::lock_guard<std::mutex> lk(s_launchMtx);
    if (g_unloading) return;
    if (page >= 0) s_reqPage = page;
    if (s_hwnd && IsWindow(s_hwnd)) {
        PostMessageW(s_hwnd, WM_XS_REFRESH, 0, 0);
        ShowWindow(s_hwnd, IsIconic(s_hwnd) ? SW_RESTORE : SW_SHOW);
        SetForegroundWindow(s_hwnd);
        return;
    }
    if (s_thread) {
        if (WaitForSingleObject(s_thread, 0) != WAIT_OBJECT_0) return;  // still starting or closing
        CloseHandle(s_thread);
        s_thread = nullptr;
        s_threadId = 0;
    }
    s_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, &s_threadId);
}

static void NotifyRefresh() {
    HWND h = s_hwnd;
    if (h && IsWindow(h)) PostMessageW(h, WM_XS_REFRESH, 0, 0);
}

// Unload: closes the window and waits for its thread.
static void CloseWindow() {
    HANDLE thread = nullptr;
    DWORD tid = 0;
    {
        std::lock_guard<std::mutex> lk(s_launchMtx);
        thread = s_thread;
        tid = s_threadId;
        s_thread = nullptr;
        s_threadId = 0;
    }
    if (thread) {
        while (WaitForSingleObject(thread, 50) == WAIT_TIMEOUT) {
            HWND main = s_hwnd;
            EnumThreadWindows(tid, [](HWND wnd, LPARAM mainWnd) -> BOOL {
                if (wnd == reinterpret_cast<HWND>(mainWnd)) return TRUE;
                wchar_t cls[16]{};
                GetClassNameW(wnd, cls, ARRAYSIZE(cls));
                if (wcscmp(cls, L"#32770") == 0) {  // a message box
                    PostMessageW(wnd, WM_COMMAND, IDNO, 0);
                    PostMessageW(wnd, WM_COMMAND, IDCANCEL, 0);
                }
                return TRUE;
            }, reinterpret_cast<LPARAM>(main));
            if (main && IsWindow(main)) PostMessageW(main, WM_CLOSE, 0, 0);
            else PostThreadMessageW(tid, WM_QUIT, 0, 0);
        }
        CloseHandle(thread);
    }
    if (s_classRegistered && UnregisterClassW(s_class, s_module)) s_classRegistered = false;
    for (HBRUSH* b : {&s_eraseLight, &s_eraseDark})
        if (*b) {
            DeleteObject(*b);
            *b = nullptr;
        }
}

}  // namespace xs

// ===========================================================================
// Windhawk entry points
// ===========================================================================

BOOL Wh_ModInit() {
    Wh_Log(L"Wh_ModInit");
    g_unloading = false;
    LoadStore();
    g_cfg = BuildSettings(SnapshotVals());
    ApplyRuntimeSettings(g_cfg);

    if (HMODULE ip = LoadLibraryW(L"iphlpapi.dll")) {
        pGetIfTable2 = (GetIfTable2_t)GetProcAddress(ip, "GetIfTable2");
        pFreeMibTable = (FreeMibTable_t)GetProcAddress(ip, "FreeMibTable");
    }
    if (HMODULE pp = LoadLibraryW(L"powrprof.dll"))
        pCallNtPower = (CallNtPowerInformation_t)GetProcAddress(pp, "CallNtPowerInformation");

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"taskbar.dll symbols didn't load, the mod can't start. Usually Windhawk's "
               L"symbol download failed: exit Windhawk from the tray, reopen it and let "
               L"\"Loading symbols\" finish.");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_worker.emplace(WorkerProc);
    g_initThread.emplace([]() {
        // Retry until the taskbar window exists and XAML is ready (slow at Explorer startup).
        for (int i = 0; i < 120 && !g_unloading; i++) {
            HWND h = FindTaskbarWnd();
            if (h) {
                g_taskbarWnd = h;
                RunFromWindowThread(h, [](void*) { StartOnThread(); }, nullptr);
                if (g_timer) break;
            }
            Sleep(500);
        }
    });
}

void Wh_ModUninit() {
    g_unloading = true;
    xs::CloseWindow();  // the settings panel, if open
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_worker) {
        if (g_worker->joinable()) g_worker->join();
        g_worker.reset();
    }
    if (g_initThread) {
        if (g_initThread->joinable()) g_initThread->join();
        g_initThread.reset();
    }

    bool ran = false;
    if (g_taskbarWnd && IsWindow(g_taskbarWnd))
        ran = RunFromWindowThread(g_taskbarWnd, [](void*) { StopOnThread(); }, nullptr, true);
    if (!ran) {
        // No taskbar thread to release them on: leak rather than release them here.
        for (Tb* t : g_tbs) {
            winrt::detach_abi(t->elem);
            winrt::detach_abi(t->parent);
            winrt::detach_abi(t->rootContent);
            winrt::detach_abi(t->tip);
            new std::vector<ItemUi>(std::move(t->items));  // intentionally never freed
        }
        g_tbs.clear();
        winrt::detach_abi(g_timer);
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

// Windhawk's own page only has the "open panel" switch; everything else lives in the panel.
void Wh_ModSettingsChanged() {
    if (Wh_GetIntSetting(L"openPanel")) xs::Show();
}
