// ==WindhawkMod==
// @id              nocturne-glucose-sparkline
// @name            Nocturne Glucose Sparkline on Taskbar
// @description     Renders a glucose sparkline with predicted glucose, loop status (IOB/COB) and server alerts on the Windows 11 taskbar, auto-positioned to track the centred app icons. Reads a local V4 summary JSON written by the Nocturne desktop companion.
// @version         0.1.0
// @author          Rhys Gray
// @github          https://github.com/ryceg
// @homepage        https://getnocturne.dev
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         AGPL-3.0-or-later
// ==/WindhawkMod==

// Source code is published under the GNU Affero General Public License v3.0 or later.

// ==WindhawkModReadme==
/*
# Nocturne Glucose Sparkline on Taskbar

Renders a glucose sparkline directly on the Windows 11 taskbar: recent actual
readings as a solid line, the predicted-glucose continuation as a dashed line,
plus the current value, trend arrow and an optional loop-status line (IOB/COB).
The widget auto-positions just left of the centred app icons and slides as apps
open/close.

**Data source:** the mod reads a local JSON file written by either (or both) of
two Nocturne clients that poll the server: the desktop companion (default
`%LOCALAPPDATA%\Nocturne\glucose.json`) and the packaged Windows 11 widget
(which writes the same logical path; Windows may redirect that write into the
widget's package container). The mod reads every candidate path and renders
whichever source is freshest, so it keeps working if only one is running. The file is the raw `V4SummaryResponse` from the
Nocturne `GET /api/v4/summary` endpoint (sgv, history, iob, cob, predictions,
alarm), with all glucose values in mg/dL; the mod converts to the configured
display unit. There is no network access from `explorer.exe` — the mod only ever
reads those files. If none are present or all are stale, the sparkline dims; when
the server alert engine reports an active alarm the card pulses a coloured
border.

## How the positioning works
The mod hooks
`winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged`
in `Taskbar.View.dll`, walks `TaskbarFrame -> RootGrid`, and adds its tile to a
single shared `TaskbarWidgetHost` StackPanel (find-or-created). The framework lays
the tiles out side by side, so multiple taskbar mods can coexist without overlap;
only the host is positioned, slid left of the centred app cluster via a
RenderTransform on every layout pass. A background reader thread parses the
summary JSON and pushes values onto the XAML UI thread via the `CoreDispatcher`.

Positioning pattern adapted from m417z's taskbar mods (taskbar-labels /
taskbar-icon-size) via the claude-code-usage mod; the shared host is what lets the
two coexist. Windows 11 only.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dataPath: ""
  $name: Summary JSON path
  $description: Path to a V4 summary JSON (the raw V4SummaryResponse from GET /api/v4/summary). Set this to force a single source. Blank = auto. The desktop companion's %LOCALAPPDATA%\Nocturne\glucose.json plus the packaged Win11 widget's file, whichever is freshest. Supports %ENV% and ~.
- pollSeconds: 15
  $name: Poll interval (seconds)
  $description: How often the mod re-reads the summary JSON.
- staleAfterSeconds: 600
  $name: Stale after (seconds)
  $description: Dim the sparkline when the latest reading (current.mills) is older than this many seconds.
- unit: mmol
  $name: Display unit
  $options:
  - mmol: mmol/L
  - mgdl: mg/dL
  $description: The summary's glucose values are always mg/dL; the mod converts to this unit for display.
- rangeLow: 3.9
  $name: Target range low (display unit)
  $description: Lower bound of the target-range band and in-range colouring, in the display unit selected above.
- rangeHigh: 10.0
  $name: Target range high (display unit)
  $description: Upper bound of the target-range band and in-range colouring, in the display unit selected above.
- style:
  - colorInRange: "36C76A"
    $name: In-range line color (hex RRGGBB)
  - colorHigh: "E6B800"
    $name: High line color (hex RRGGBB)
  - colorLow: "E0533D"
    $name: Low line color (hex RRGGBB)
  - colorPredicted: "8AA0B4"
    $name: Predicted line color (hex RRGGBB)
  - showBand: true
    $name: Show target-range band
  - showCurrentValue: true
    $name: Show current value
  - showTrendArrow: true
    $name: Show trend arrow
  - showIobCob: true
    $name: Show loop status (IOB/COB)
    $description: Show an "IOB 1.2U · COB 14g" line from the summary's iob/cob. Hidden when both are zero or absent.
  - sparkWidth: 120
    $name: Sparkline width (px)
  - sparkHeight: 26
    $name: Sparkline height (px)
  - lineThickness: 2
    $name: Line thickness (px)
  - fontSize: 13
    $name: Font size (px)
  - autoTextColor: true
    $name: Auto text color (follow taskbar theme)
    $description: When on, value text is black on a light taskbar and white on a dark one. Turn off to use the fixed Text color below.
  - textColor: "FFFFFF"
    $name: Text color (when auto is off)
  $name: Style
- alert:
  - pulseOutOfRange: true
    $name: Pulse a border on an active alarm
    $description: When the summary reports an active (un-silenced) server alarm, the whole card gets a pulsing coloured outline (low alarms use the Low colour, everything else the High colour). If the summary has no alarm field at all, falls back to pulsing when the current value is below the range low or above the range high.
  - borderThickness: 2
    $name: Alert border thickness (px)
  $name: Alert
- rank: 20
  $name: Tile order
  $description: Left-to-right order among taskbar widgets sharing the host (lower = further left). The widget auto-tracks the app icons; this only sets ordering relative to other participating mods.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <limits>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#undef GetCurrentTime  // collides with a winrt method name

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;
namespace Animation = winrt::Windows::UI::Xaml::Media::Animation;
namespace Shapes = winrt::Windows::UI::Xaml::Shapes;
using winrt::Windows::Foundation::Point;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
struct Settings {
    std::vector<std::wstring> dataPaths;  // candidate summary JSON paths, tried freshest-wins
    int pollSeconds = 15;
    int staleAfterSeconds = 600;
    std::wstring unit = L"mmol/L";  // display unit: "mmol/L" or "mg/dL"
    double rangeLow = 3.9, rangeHigh = 10.0;  // target range, in the display unit
    COLORREF colorInRange = 0, colorHigh = 0, colorLow = 0, colorPredicted = 0;
    bool showBand = true, showCurrentValue = true, showTrendArrow = true, showIobCob = true;
    int sparkWidth = 120, sparkHeight = 26, lineThickness = 2, fontSize = 13;
    bool autoTextColor = true;
    COLORREF textColor = 0;
    bool pulseOutOfRange = true;
    int borderThickness = 2;
    int rank = 20;  // left-to-right order among tiles in the shared host (lower = leftmost)
} g_settings;

// ---------------------------------------------------------------------------
// Parsed glucose state -> UI snapshot
// ---------------------------------------------------------------------------
struct GPoint { double tMs = 0; double v = 0; };  // v is in the display unit
// A flat, thread-safe snapshot handed to the UI thread for rendering. All glucose
// values (current/readings/predicted/delta) are already converted to the display unit.
struct GlucoseSnapshot {
    bool valid = false;     // a read was attempted
    bool ok = false;        // file parsed and has at least one reading
    double current = 0; std::wstring trend; double delta = 0; bool hasDelta = false;
    std::vector<GPoint> readings, predicted;
    double currentMills = 0;          // current.mills (latest reading time), drives staleness
    double iob = 0, cob = 0;          // loop status (units / grams)
    bool hasIob = false, hasCob = false;
    bool hasAlarmField = false;       // whether the summary carried an "alarm" object at all
    bool alarmActive = false;         // alarm present AND level>0 AND !isSilenced
    bool alarmLow = false;            // alarm "type" indicates a low (colours the pulse)
};

std::mutex g_widgetMutex;
// g_dispatcher / g_borderStoryboard hold XAML/COM objects with non-trivial
// destructors. They are intentionally heap-leaked (held by reference) so their
// destructors never run during DLL_PROCESS_DETACH (process shutdown — explorer
// restart, sign-out, reboot — where Wh_ModUninit is NOT called): releasing an STA
// proxy after its apartment is gone can hang under the loader lock. Normal teardown
// releases them on the UI thread in Wh_ModBeforeUninit while the apartment is alive,
// so the leak only ever happens at real process exit, where the OS reclaims it.
winrt::Windows::UI::Core::CoreDispatcher& g_dispatcher =
    *new winrt::Windows::UI::Core::CoreDispatcher{nullptr};
winrt::weak_ref<FrameworkElement> g_widgetRoot;
winrt::weak_ref<FrameworkElement> g_rootGrid;  // captured for synchronous tile removal on disable
GlucoseSnapshot g_lastSnapshot;  // replayed when the widget is (re)inserted

// Heap pointer with a trivial destructor. At process shutdown Wh_ModUninit is NOT
// called and globals are destroyed during DLL_PROCESS_DETACH; a value std::thread
// would still be joinable() there and ~std::thread() would call std::terminate(),
// aborting the host. Leaking the pointer is safe (the OS reclaims the thread); the
// real join + delete happens in Wh_ModUninit while the process is alive.
std::thread* g_readerThread = nullptr;
std::atomic<bool> g_running{false};
std::atomic<bool> g_taskbarViewHooked{false};
std::atomic<bool> g_insertPending{false};  // a deferred insertion is queued

// The out-of-range alert pulse storyboard. Created on the UI thread and kept alive so
// refreshes only Begin/Stop it (UI-thread only). Heap-leaked (see g_dispatcher above).
Animation::Storyboard& g_borderStoryboard =
    *new Animation::Storyboard{nullptr};
winrt::weak_ref<Media::SolidColorBrush> g_borderBrushTarget;

// Re-entrancy guard: set while we position the host from *inside* the taskbar
// layout pass, so the RenderTransform write can't recurse into the hook. UI-thread only.
std::atomic<bool> g_inLayoutApply{false};

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------
static COLORREF ParseHexColor(PCWSTR s, COLORREF fallback) {
    if (!s) return fallback;
    unsigned r = 0, g = 0, b = 0;
    if (swscanf(s, L"%2x%2x%2x", &r, &g, &b) == 3) return RGB(r, g, b);
    return fallback;
}

static std::wstring ExpandEnv(const std::wstring& in) {
    DWORD n = ExpandEnvironmentStringsW(in.c_str(), nullptr, 0);
    if (!n) return in;
    std::wstring out(n, L'\0');
    ExpandEnvironmentStringsW(in.c_str(), out.data(), n);
    if (!out.empty() && out.back() == L'\0') out.pop_back();
    return out;
}

// Expand %VAR% / leading ~ and normalise to backslashes.
static std::wstring ExpandConfigPath(const std::wstring& in) {
    std::wstring s = in;
    if (!s.empty() && s[0] == L'~' && (s.size() == 1 || s[1] == L'/' || s[1] == L'\\')) {
        wchar_t up[MAX_PATH]; DWORD n = GetEnvironmentVariableW(L"USERPROFILE", up, MAX_PATH);
        if (n) s = std::wstring(up, n) + s.substr(1);
    }
    s = ExpandEnv(s);
    for (auto& c : s) if (c == L'/') c = L'\\';
    return s;
}

// Append the packaged Win11 widget's glucose file(s) to `out`. The widget writes to
// %LOCALAPPDATA%\Nocturne\glucose.json; depending on the OS/packaging, Windows may redirect that
// AppData write into the widget's package container (%LOCALAPPDATA%\Packages\<PFN>\LocalCache\...).
// The PFN suffix is a publisher hash, so we glob the identity prefix and probe the known redirect
// subpaths; missing ones simply fail to read later. If the write is NOT redirected, the widget
// shares the companion's real path (covered by the companion candidate) and these globs find
// nothing — harmless. Either way the freshest readable source wins.
static void AddWidgetCandidatePaths(std::vector<std::wstring>& out) {
    wchar_t lad[MAX_PATH];
    DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", lad, MAX_PATH);
    if (!n || n >= MAX_PATH) return;
    std::wstring packages = std::wstring(lad, n) + L"\\Packages";
    std::wstring search = packages + L"\\Nightscout.Nocturne.Widget_*";

    WIN32_FIND_DATAW fd{};
    HANDLE h = FindFirstFileW(search.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
        if (fd.cFileName[0] == L'.') continue;
        std::wstring base = packages + L"\\" + fd.cFileName;
        // Desktop-bridge AppData redirect targets, most-likely first.
        static const wchar_t* kSubs[] = {
            L"\\LocalCache\\Local\\Nocturne\\glucose.json",
            L"\\LocalCache\\Roaming\\Nocturne\\glucose.json",
            L"\\LocalCache\\Nocturne\\glucose.json",
            L"\\LocalState\\Nocturne\\glucose.json",
        };
        for (auto* s : kSubs) out.push_back(base + s);
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

static int64_t NowMsEpoch() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER u; u.LowPart = ft.dwLowDateTime; u.HighPart = ft.dwHighDateTime;
    return (int64_t)((u.QuadPart - 116444736000000000ULL) / 10000ULL);
}

// Convert a canonical mg/dL value to the configured display unit. mmol/L is
// rounded to 1 dp; mg/dL to a whole number.
static double ToDisplayUnit(double mgdl) {
    if (g_settings.unit == L"mg/dL") return std::round(mgdl);
    return std::round(mgdl / 18.0182 * 10.0) / 10.0;
}

static std::string Narrow(const std::wstring& w) {
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), s.data(), n, nullptr, nullptr);
    return s;
}
static std::wstring Widen(const std::string& s) {
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), w.data(), n);
    return w;
}

static bool ReadFileUtf8(const std::wstring& path, std::string& out) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER sz{};
    GetFileSizeEx(h, &sz);
    out.resize((size_t)sz.QuadPart);
    DWORD read = 0;
    BOOL ok = ReadFile(h, out.data(), (DWORD)out.size(), &read, nullptr);
    CloseHandle(h);
    out.resize(read);
    // Tolerate a UTF-8 BOM.
    if (out.size() >= 3 && (unsigned char)out[0] == 0xEF &&
        (unsigned char)out[1] == 0xBB && (unsigned char)out[2] == 0xBF)
        out.erase(0, 3);
    return ok == TRUE;
}

// ---------------------------------------------------------------------------
// Minimal JSON parser (parse-only). Adapted from the claude-code-usage mod.
// ---------------------------------------------------------------------------
struct JsonValue {
    enum Type { Null, Bool, Num, Str, Arr, Obj } type = Null;
    bool b = false;
    double num = 0;
    std::string str;
    std::vector<JsonValue> arr;
    std::vector<std::pair<std::string, JsonValue>> obj;

    const JsonValue* get(const std::string& k) const {
        for (auto& kv : obj) if (kv.first == k) return &kv.second;
        return nullptr;
    }
    bool isNum() const { return type == Num; }
    bool isStr() const { return type == Str; }
};

struct JsonParser {
    const std::string& s; size_t i = 0; bool ok = true;
    explicit JsonParser(const std::string& src) : s(src) {}
    void ws() { while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r')) i++; }
    JsonValue parse() { ws(); JsonValue v = value(); ws(); return v; }
    JsonValue value() {
        ws();
        if (i >= s.size()) { ok = false; return {}; }
        char c = s[i];
        if (c == '{') return object();
        if (c == '[') return array();
        if (c == '"') { JsonValue j; j.type = JsonValue::Str; j.str = string(); return j; }
        if (c == 't' || c == 'f') return boolean();
        if (c == 'n') { i += 4; JsonValue j; return j; }
        return number();
    }
    JsonValue object() {
        JsonValue j; j.type = JsonValue::Obj; i++; ws();
        if (i < s.size() && s[i] == '}') { i++; return j; }
        while (i < s.size()) {
            ws();
            std::string key = string(); ws();
            if (i < s.size() && s[i] == ':') i++; else { ok = false; break; }
            j.obj.emplace_back(std::move(key), value()); ws();
            if (i < s.size() && s[i] == ',') { i++; continue; }
            if (i < s.size() && s[i] == '}') { i++; break; }
            ok = false; break;
        }
        return j;
    }
    JsonValue array() {
        JsonValue j; j.type = JsonValue::Arr; i++; ws();
        if (i < s.size() && s[i] == ']') { i++; return j; }
        while (i < s.size()) {
            j.arr.push_back(value()); ws();
            if (i < s.size() && s[i] == ',') { i++; continue; }
            if (i < s.size() && s[i] == ']') { i++; break; }
            ok = false; break;
        }
        return j;
    }
    JsonValue boolean() {
        JsonValue j; j.type = JsonValue::Bool;
        if (s.compare(i, 4, "true") == 0) { j.b = true; i += 4; }
        else { j.b = false; i += 5; }
        return j;
    }
    JsonValue number() {
        size_t start = i;
        while (i < s.size() && (isdigit((unsigned char)s[i]) || s[i] == '-' || s[i] == '+' ||
                                s[i] == '.' || s[i] == 'e' || s[i] == 'E')) i++;
        JsonValue j; j.type = JsonValue::Num;
        j.num = strtod(s.substr(start, i - start).c_str(), nullptr);
        return j;
    }
    std::string string() {
        std::string out;
        if (i >= s.size() || s[i] != '"') { ok = false; return out; }
        i++;
        while (i < s.size()) {
            char c = s[i++];
            if (c == '"') return out;
            if (c == '\\' && i < s.size()) {
                char e = s[i++];
                switch (e) {
                    case '"': out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/': out += '/'; break;
                    case 'b': out += '\b'; break;
                    case 'f': out += '\f'; break;
                    case 'n': out += '\n'; break;
                    case 'r': out += '\r'; break;
                    case 't': out += '\t'; break;
                    case 'u': {
                        if (i + 4 > s.size()) { ok = false; return out; }
                        unsigned cp = (unsigned)strtoul(s.substr(i, 4).c_str(), nullptr, 16);
                        i += 4;
                        if (cp < 0x80) out += (char)cp;
                        else if (cp < 0x800) { out += (char)(0xC0 | (cp >> 6)); out += (char)(0x80 | (cp & 0x3F)); }
                        else { out += (char)(0xE0 | (cp >> 12)); out += (char)(0x80 | ((cp >> 6) & 0x3F)); out += (char)(0x80 | (cp & 0x3F)); }
                        break;
                    }
                    default: out += e; break;
                }
            } else {
                out += c;
            }
        }
        ok = false;
        return out;
    }
};

static JsonValue JsonParse(const std::string& s, bool* ok = nullptr) {
    JsonParser p(s);
    JsonValue v = p.parse();
    if (ok) *ok = p.ok;
    return v;
}

// Parse one V4 history array ([{ "sgv": mgdl, "mills": epochMs, ... }]) into
// GPoints, converting sgv to the display unit. Order is not guaranteed by the
// summary, so callers sort by tMs ascending afterwards.
static void ParseHistory(const JsonValue* arr, std::vector<GPoint>& out) {
    if (!arr || arr->type != JsonValue::Arr) return;
    for (const auto& e : arr->arr) {
        if (e.type != JsonValue::Obj) continue;
        GPoint p;
        if (auto* m = e.get("mills"); m && m->isNum()) p.tMs = m->num;
        if (auto* v = e.get("sgv"); v && v->isNum()) p.v = ToDisplayUnit(v->num);
        if (p.tMs > 0) out.push_back(p);
    }
    std::sort(out.begin(), out.end(), [](const GPoint& a, const GPoint& b) { return a.tMs < b.tMs; });
}

// Read + parse one V4SummaryResponse JSON file into a snapshot. Never throws; ok=false
// on any miss. All glucose values are converted from canonical mg/dL to the
// display unit here so the UI thread renders them verbatim.
static GlucoseSnapshot ReadOneGlucoseFile(const std::wstring& path) {
    GlucoseSnapshot s;
    s.valid = true;
    std::string raw;
    if (!ReadFileUtf8(path, raw)) {
        return s;
    }
    bool ok = false;
    JsonValue d = JsonParse(raw, &ok);
    if (!ok || d.type != JsonValue::Obj) { Wh_Log(L"[read] parse error"); return s; }

    if (auto* p = d.get("iob"); p && p->isNum()) { s.iob = p->num; s.hasIob = true; }
    if (auto* p = d.get("cob"); p && p->isNum()) { s.cob = p->num; s.hasCob = true; }

    if (auto* cur = d.get("current"); cur && cur->type == JsonValue::Obj) {
        if (auto* p = cur->get("sgv"); p && p->isNum()) s.current = ToDisplayUnit(p->num);
        if (auto* p = cur->get("direction"); p && p->isStr()) s.trend = Widen(p->str);
        if (auto* p = cur->get("delta"); p && p->isNum()) { s.delta = ToDisplayUnit(p->num); s.hasDelta = true; }
        if (auto* p = cur->get("mills"); p && p->isNum()) s.currentMills = p->num;
    }

    if (auto* a = d.get("alarm"); a && a->type == JsonValue::Obj) {
        s.hasAlarmField = true;
        int level = 0; bool silenced = false; std::string type;
        if (auto* p = a->get("level"); p && p->isNum()) level = (int)llround(p->num);
        if (auto* p = a->get("isSilenced"); p && p->type == JsonValue::Bool) silenced = p->b;
        if (auto* p = a->get("type"); p && p->isStr()) type = p->str;
        s.alarmActive = level > 0 && !silenced;
        s.alarmLow = type.find("low") != std::string::npos || type.find("Low") != std::string::npos;
    } else if (auto* a = d.get("alarm"); a && a->type != JsonValue::Null) {
        // present but not an object (defensive) — still counts as "has a field".
        s.hasAlarmField = a != nullptr;
    }

    ParseHistory(d.get("history"), s.readings);

    // predictions: { values:[mgdl...], startMills, intervalMills }. Point i is at
    // startMills + i*intervalMills, value values[i] (converted to the display unit).
    if (auto* pr = d.get("predictions"); pr && pr->type == JsonValue::Obj) {
        const JsonValue* vals = pr->get("values");
        double startMills = 0, intervalMills = 0;
        if (auto* p = pr->get("startMills"); p && p->isNum()) startMills = p->num;
        if (auto* p = pr->get("intervalMills"); p && p->isNum()) intervalMills = p->num;
        if (vals && vals->type == JsonValue::Arr) {
            for (size_t i = 0; i < vals->arr.size(); i++) {
                if (!vals->arr[i].isNum()) continue;
                GPoint p;
                p.tMs = startMills + (double)i * intervalMills;
                p.v = ToDisplayUnit(vals->arr[i].num);
                s.predicted.push_back(p);
            }
        }
    }

    s.ok = !s.readings.empty();
    return s;
}

// Effective freshness of a snapshot for source selection: current.mills when present, else the
// last reading's time. Lets us pick the most up-to-date source when several are readable.
static double SnapshotFreshness(const GlucoseSnapshot& s) {
    if (s.currentMills > 0) return s.currentMills;
    return s.readings.empty() ? 0.0 : s.readings.back().tMs;
}

// Read every candidate path (companion + packaged widget) and return the freshest parsed snapshot,
// so whichever source is live and most current wins and a dead/stale source can't mask it. Returns
// a valid-but-not-ok snapshot (UI dims) when none are readable.
static GlucoseSnapshot ReadGlucose() {
    GlucoseSnapshot best;
    bool haveBest = false;
    for (const auto& path : g_settings.dataPaths) {
        GlucoseSnapshot s = ReadOneGlucoseFile(path);
        if (!s.ok) continue;
        if (!haveBest || SnapshotFreshness(s) > SnapshotFreshness(best)) {
            best = std::move(s);
            haveBest = true;
        }
    }
    if (!haveBest) {
        Wh_Log(L"[read] no readable summary JSON among %zu candidate(s)", g_settings.dataPaths.size());
        best.valid = true;
    }
    return best;
}

// ---------------------------------------------------------------------------
// XAML: visual-tree helpers, widget construction, UI-thread updates
// ---------------------------------------------------------------------------
static FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (child && child.Name() == name) return child;
    }
    return nullptr;
}

// ============================================================================
// Shared TaskbarWidgetHost protocol — v1
// ----------------------------------------------------------------------------
// Independent taskbar mods otherwise each absolute-position a widget into RootGrid
// and collide. Instead, every participating mod adds its tile as a child of ONE
// shared horizontal StackPanel ("TaskbarWidgetHost"); the framework lays the tiles
// out side by side (overlap is impossible) and only the host is positioned, left of
// the app cluster. Any mod find-or-creates the host and drives its position
// idempotently (all compute the same value), so there is no owner and either mod may
// unload safely. Tiles are kept ordered by an integer rank (then insertion), so the
// left-to-right order is stable regardless of which mod loads first.
//
// This block MUST stay byte-identical across every participating mod. Bump the
// protocol version if the host's name / shape / positioning contract changes.
// ============================================================================
namespace TaskbarHost {

constexpr int kProtocol = 1;
constexpr double kGap = 16.0;      // space between the host's right edge and the apps
constexpr double kMinX = 8.0;      // never slide left of this (clears the Widgets corner)
constexpr double kSpacing = 8.0;   // gap between tiles inside the host
inline PCWSTR HostName() { return L"TaskbarWidgetHost"; }

// UI thread, read-only. Returns the host if it exists, else nullptr — does NOT create
// it, so it is safe inside the layout pass (where tree mutation throws 0x800F1000).
inline Controls::StackPanel Find(FrameworkElement rootGrid) {
    Controls::Panel panel{nullptr};
    try { panel = rootGrid.as<Controls::Panel>(); } catch (...) { return nullptr; }
    auto children = panel.Children();
    for (uint32_t i = 0; i < children.Size(); i++) {
        auto fe = children.GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == HostName())
            return fe.try_as<Controls::StackPanel>();
    }
    return nullptr;
}

// UI thread. Returns the shared host, creating it as a child of `rootGrid` if absent.
inline Controls::StackPanel FindOrCreate(FrameworkElement rootGrid) {
    if (auto existing = Find(rootGrid)) return existing;
    Controls::Panel panel{nullptr};
    try { panel = rootGrid.as<Controls::Panel>(); } catch (...) { return nullptr; }
    Controls::StackPanel host;
    host.Name(HostName());
    host.Orientation(Controls::Orientation::Horizontal);
    host.Spacing(kSpacing);
    host.HorizontalAlignment(HorizontalAlignment::Left);
    host.VerticalAlignment(VerticalAlignment::Center);
    try { panel.Children().Append(host); } catch (...) { return nullptr; }
    return host;
}

// UI thread (read-only-safe inside the layout pass: only RenderTransform is touched).
// Slide the host so its right edge sits `kGap` left of the app cluster. `appLeft` is
// the app-cluster left edge in rootGrid coords; NaN / zero width means "not ready".
inline void Reposition(Controls::StackPanel host, double appLeft) {
    if (!host) return;
    double w = host.ActualWidth();
    if (std::isnan(appLeft) || w <= 0.0) return;
    double tx = appLeft - w - kGap;
    if (tx < kMinX) tx = kMinX;
    static double lastTx = std::numeric_limits<double>::quiet_NaN();
    if (!std::isnan(lastTx) && std::abs(tx - lastTx) < 0.5) return;  // skip no-op writes
    lastTx = tx;
    Media::TranslateTransform tt;
    tt.X(tx);
    host.RenderTransform(tt);
}

// UI thread. Add `tile` to the host if not already present (matched by Name), keeping
// children sorted by `rank` so tile order is stable regardless of load order.
inline void AddTile(Controls::StackPanel host, FrameworkElement tile, int rank) {
    if (!host || !tile) return;
    auto children = host.Children();
    for (uint32_t i = 0; i < children.Size(); i++) {
        auto fe = children.GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == tile.Name()) return;  // already present
    }
    tile.Tag(winrt::box_value(rank));
    uint32_t idx = children.Size();
    for (uint32_t i = 0; i < children.Size(); i++) {
        auto fe = children.GetAt(i).try_as<FrameworkElement>();
        int o = fe ? winrt::unbox_value_or<int>(fe.Tag(), 0) : 0;
        if (o > rank) { idx = i; break; }
    }
    children.InsertAt(idx, tile);
}

// UI thread. Remove a tile by name; remove the now-empty host too.
inline void RemoveTile(FrameworkElement rootGrid, PCWSTR tileName) {
    Controls::Panel panel{nullptr};
    try { panel = rootGrid.as<Controls::Panel>(); } catch (...) { return; }
    auto rc = panel.Children();
    for (uint32_t i = 0; i < rc.Size(); i++) {
        auto fe = rc.GetAt(i).try_as<FrameworkElement>();
        if (!fe || fe.Name() != HostName()) continue;
        auto host = fe.try_as<Controls::StackPanel>();
        if (!host) return;
        auto hc = host.Children();
        for (uint32_t j = hc.Size(); j-- > 0;) {
            auto t = hc.GetAt(j).try_as<FrameworkElement>();
            if (t && t.Name() == tileName) hc.RemoveAt(j);
        }
        if (host.Children().Size() == 0) rc.RemoveAt(i);
        return;
    }
}

}  // namespace TaskbarHost

// Read-only: compute the left edge (in rootGrid coords) of the first realized
// app button inside TaskbarFrameRepeater. The repeater centres its items and
// grows leftward as apps open, so this is the boundary the widget must stay
// clear of in "auto" mode. Returns NaN if nothing usable is found.
// MUST stay read-only (TransformToVisual / VisualTreeHelper / bounds only): it
// runs from the layout hook where mutating the tree throws 0x800F1000.
static double ComputeAppLeftEdge(FrameworkElement rootGrid, bool dump) {
    if (!rootGrid) return std::numeric_limits<double>::quiet_NaN();
    auto repeater = FindChildByName(rootGrid, L"TaskbarFrameRepeater");
    if (!repeater) {
        if (dump) Wh_Log(L"[repeater] TaskbarFrameRepeater not found");
        return std::numeric_limits<double>::quiet_NaN();
    }
    double tbWidth = rootGrid.ActualWidth();
    double minX = std::numeric_limits<double>::quiet_NaN();
    int count = Media::VisualTreeHelper::GetChildrenCount(repeater);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
        if (!child) continue;
        bool vis = child.Visibility() == Visibility::Visible;
        double w = child.ActualWidth();
        double x = std::numeric_limits<double>::quiet_NaN();
        try {
            x = child.TransformToVisual(rootGrid).TransformPoint(Point{0, 0}).X;
        } catch (...) {
            // TransformToVisual can throw if the child isn't in the live tree.
        }
        auto cls = winrt::get_class_name(child);
        // The element we anchor left-of is the leftmost of the CENTRED cluster.
        // Exclude the Widgets/weather entry point (pinned far-left, x~0), the
        // off-screen virtualized items (observed at x=-10044) and anything past
        // the right edge, plus the full-width background / zero-width items.
        if (cls == L"Taskbar.AugmentedEntryPointButton") continue;
        if (std::isnan(x) || x < 1.0 || (tbWidth > 0.0 && x > tbWidth)) continue;
        if (!vis || w <= 4.0 || w >= 320.0) continue;
        if (std::isnan(minX) || x < minX) minX = x;
    }
    if (dump) Wh_Log(L"[repeater] -> appLeftEdge=%.0f", minX);
    return minX;
}

static Media::SolidColorBrush MakeBrush(COLORREF c) {
    winrt::Windows::UI::Color col{};
    col.A = 255; col.R = GetRValue(c); col.G = GetGValue(c); col.B = GetBValue(c);
    return Media::SolidColorBrush{col};
}

// True when the taskbar uses the light theme (so we need dark text).
static bool IsTaskbarLight() {
    DWORD val = 0, sz = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &val, &sz) == ERROR_SUCCESS)
        return val != 0;
    return false;  // default to dark taskbar
}
static COLORREF EffectiveTextColor() {
    if (g_settings.autoTextColor)
        return IsTaskbarLight() ? RGB(0x1A, 0x1A, 0x1A) : RGB(0xFF, 0xFF, 0xFF);
    return g_settings.textColor;
}

// In-range / high / low colour for the current value and the actual line.
static COLORREF StateColor(double v, double lo, double hi) {
    if (lo > 0 && v < lo) return g_settings.colorLow;
    if (hi > 0 && v > hi) return g_settings.colorHigh;
    return g_settings.colorInRange;
}

// Format an already-converted value for the configured display unit.
static std::wstring FormatValue(double v) {
    wchar_t b[32];
    if (g_settings.unit == L"mg/dL") _snwprintf(b, 32, L"%.0f", v);
    else _snwprintf(b, 32, L"%.1f", v);
    b[31] = L'\0';
    return b;
}

// Signed delta for the configured unit (e.g. "+0.3" / "-2"); rides next to the trend arrow.
static std::wstring FormatDelta(double v) {
    wchar_t b[32];
    if (g_settings.unit == L"mg/dL") _snwprintf(b, 32, L"%+.0f", v);
    else _snwprintf(b, 32, L"%+.1f", v);
    b[31] = L'\0';
    return b;
}

// Unicode trend glyph for the Dexcom-style direction strings.
static std::wstring TrendArrow(const std::wstring& t) {
    if (t == L"DoubleUp") return L"\x21C8";
    if (t == L"SingleUp") return L"\x2191";
    if (t == L"FortyFiveUp") return L"\x2197";
    if (t == L"Flat") return L"\x2192";
    if (t == L"FortyFiveDown") return L"\x2198";
    if (t == L"SingleDown") return L"\x2193";
    if (t == L"DoubleDown") return L"\x21CA";
    return L"";
}

// Named elements (CurrentValue / TrendArrow / IobCob / RangeBand / SparkActual / SparkPredicted /
// CurrentDot) are filled later via FindName(). The root Name NocturneGlucoseTile is what
// removal-by-name keys off.
static std::wstring BuildWidgetXaml() {
    auto bc = g_settings.colorLow;  // alert border colour; recoloured at apply time
    auto bandC = g_settings.colorInRange;
    int fs = g_settings.fontSize;
    int sub = fs - 3; if (sub < 9) sub = 9;  // loop-status line is a touch smaller
    wchar_t buf[2816];
    _snwprintf(buf, ARRAYSIZE(buf),
        LR"(<Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
              xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
              Name="NocturneGlucoseTile" VerticalAlignment="Center">
            <Border Name="CardBorder" BorderThickness="0" CornerRadius="6" Padding="6,2">
              <Border.BorderBrush><SolidColorBrush Color="#%02X%02X%02X" Opacity="0"/></Border.BorderBrush>
              <Grid Name="CardInner">
                <Grid.ColumnDefinitions><ColumnDefinition Width="Auto"/><ColumnDefinition Width="Auto"/></Grid.ColumnDefinitions>
                <StackPanel Grid.Column="0" Orientation="Vertical" VerticalAlignment="Center" Margin="0,0,8,0">
                  <StackPanel Orientation="Horizontal" Spacing="2" VerticalAlignment="Center">
                    <TextBlock Name="CurrentValue" Text="" FontSize="%d" FontWeight="SemiBold" VerticalAlignment="Center"/>
                    <TextBlock Name="TrendArrow" Text="" FontSize="%d" VerticalAlignment="Center" Margin="2,0,0,0"/>
                  </StackPanel>
                  <TextBlock Name="IobCob" Text="" FontSize="%d" Opacity="0.85" VerticalAlignment="Center"/>
                </StackPanel>
                <Canvas Grid.Column="1" Name="SparkCanvas" Width="%d" Height="%d" VerticalAlignment="Center">
                  <Rectangle Name="RangeBand" Opacity="0.14">
                    <Rectangle.Fill><SolidColorBrush Color="#%02X%02X%02X"/></Rectangle.Fill>
                  </Rectangle>
                  <Polyline Name="SparkActual" StrokeThickness="%d" StrokeLineJoin="Round"/>
                  <Polyline Name="SparkPredicted" StrokeThickness="%d" StrokeLineJoin="Round" StrokeDashArray="2,2" Opacity="0.9"/>
                  <Ellipse Name="CurrentDot" Width="5" Height="5"/>
                </Canvas>
              </Grid>
            </Border>
          </Grid>)",
        GetRValue(bc), GetGValue(bc), GetBValue(bc),
        fs, fs, sub,
        g_settings.sparkWidth, g_settings.sparkHeight,
        GetRValue(bandC), GetGValue(bandC), GetBValue(bandC),
        g_settings.lineThickness, g_settings.lineThickness);
    buf[ARRAYSIZE(buf) - 1] = L'\0';
    return buf;
}

// UI-thread: pulse a coloured outline around the card when out of range. Animates
// the border BRUSH's Opacity (animating the Border's Opacity would fade its
// children too). Idempotent: re-pulsing the same brush leaves it running.
static void UpdateAlertBorder(FrameworkElement root, bool pulse, COLORREF color) {
    auto border = root.FindName(L"CardBorder").try_as<Controls::Border>();
    if (!border) return;
    int t = g_settings.borderThickness;
    border.BorderThickness(pulse ? Thickness{(double)t, (double)t, (double)t, (double)t}
                                 : Thickness{0, 0, 0, 0});
    auto brush = border.BorderBrush().try_as<Media::SolidColorBrush>();
    if (!brush) { brush = Media::SolidColorBrush{}; border.BorderBrush(brush); }
    winrt::Windows::UI::Color col{};
    col.A = 255; col.R = GetRValue(color); col.G = GetGValue(color); col.B = GetBValue(color);
    brush.Color(col);

    if (!pulse) {
        if (g_borderStoryboard) { try { g_borderStoryboard.Stop(); } catch (...) {} }
        g_borderBrushTarget = {};
        brush.Opacity(0.0);
        return;
    }
    if (g_borderStoryboard && g_borderBrushTarget.get() == brush) return;  // already pulsing it
    if (g_borderStoryboard) { try { g_borderStoryboard.Stop(); } catch (...) {} }
    brush.Opacity(1.0);

    Animation::Storyboard sb;
    Animation::RepeatBehavior forever{};
    forever.Type = Animation::RepeatBehaviorType::Forever;
    Animation::DoubleAnimation da;
    da.To(0.15);
    da.Duration(Duration{std::chrono::milliseconds(800)});
    da.AutoReverse(true);
    da.RepeatBehavior(forever);
    Animation::Storyboard::SetTarget(da, brush);
    Animation::Storyboard::SetTargetProperty(da, L"Opacity");
    sb.Children().Append(da);
    g_borderStoryboard = sb;
    g_borderBrushTarget = winrt::make_weak(brush);
    try { g_borderStoryboard.Begin(); } catch (...) {
        Wh_Log(L"[alert] border Begin failed %08X", winrt::to_hresult());
    }
}

// Map a value to a Y coordinate in the canvas (inverted: high value = low Y).
struct Scale { double tMin, tSpan, vMin, vSpan; double w, h; };
static double MapX(const Scale& sc, double t) {
    return (t - sc.tMin) / sc.tSpan * sc.w;
}
static double MapY(const Scale& sc, double v) {
    return sc.h - (v - sc.vMin) / sc.vSpan * sc.h;
}

// UI-thread: push one snapshot's values into the live widget.
static void ApplyWidget(const GlucoseSnapshot& s) {
    FrameworkElement root{nullptr};
    {
        std::lock_guard<std::mutex> lk(g_widgetMutex);
        root = g_widgetRoot.get();
    }
    if (!root) return;
    try {
        // Position is owned by the shared TaskbarHost; the tile only sizes to its content.

        // Dim the whole card when the latest reading is old or unread.
        double ageMs = s.currentMills > 0 ? (double)NowMsEpoch() - s.currentMills : 1e18;
        bool stale = !s.ok || ageMs > (double)g_settings.staleAfterSeconds * 1000.0;
        root.Opacity(stale ? 0.45 : 1.0);

        COLORREF tc = EffectiveTextColor();
        double rLo = g_settings.rangeLow, rHi = g_settings.rangeHigh;

        if (auto cv = root.FindName(L"CurrentValue").try_as<Controls::TextBlock>()) {
            cv.Visibility(g_settings.showCurrentValue ? Visibility::Visible : Visibility::Collapsed);
            cv.Text(s.ok ? FormatValue(s.current) : L"--");
            cv.Foreground(MakeBrush(s.ok ? StateColor(s.current, rLo, rHi) : tc));
        }
        if (auto ta = root.FindName(L"TrendArrow").try_as<Controls::TextBlock>()) {
            bool show = g_settings.showTrendArrow && s.ok && !s.trend.empty();
            ta.Visibility(show ? Visibility::Visible : Visibility::Collapsed);
            // Arrow + signed delta since the previous reading (e.g. "→  -0.2"). The summary
            // sometimes omits delta (null) — show only the arrow then, not a misleading "+0.0".
            std::wstring at = show ? TrendArrow(s.trend) : L"";
            if (show && s.hasDelta) at += L"  " + FormatDelta(s.delta);
            ta.Text(at);
            ta.Foreground(MakeBrush(tc));
        }

        // Loop status (IOB/COB). Hidden when disabled, no data, or both zero.
        if (auto io = root.FindName(L"IobCob").try_as<Controls::TextBlock>()) {
            bool haveIob = s.hasIob && s.iob > 0.0;
            bool haveCob = s.hasCob && s.cob > 0.0;
            bool show = g_settings.showIobCob && s.ok && (haveIob || haveCob);
            if (show) {
                std::wstring txt;
                wchar_t b[48];
                if (haveIob) { _snwprintf(b, ARRAYSIZE(b), L"IOB %.1fU", s.iob); b[ARRAYSIZE(b) - 1] = L'\0'; txt = b; }
                if (haveCob) {
                    _snwprintf(b, ARRAYSIZE(b), L"COB %.0fg", s.cob); b[ARRAYSIZE(b) - 1] = L'\0';
                    if (!txt.empty()) txt += L" \x00B7 ";  // middle dot
                    txt += b;
                }
                io.Text(txt);
                io.Foreground(MakeBrush(tc));
            }
            io.Visibility(show ? Visibility::Visible : Visibility::Collapsed);
        }

        // Sparkline geometry. Actual + predicted share one value scale so they align; X spans
        // the first reading to the last predicted point.
        double w = (double)g_settings.sparkWidth, h = (double)g_settings.sparkHeight;
        auto setLine = [&](PCWSTR name, const Media::PointCollection& pc) {
            if (auto pl = root.FindName(winrt::hstring{name}).try_as<Shapes::Polyline>())
                pl.Points(pc);
        };

        if (!s.ok) {
            setLine(L"SparkActual", Media::PointCollection{});
            setLine(L"SparkPredicted", Media::PointCollection{});
            if (auto band = root.FindName(L"RangeBand").try_as<Shapes::Rectangle>())
                band.Visibility(Visibility::Collapsed);
            if (auto dot = root.FindName(L"CurrentDot").try_as<Shapes::Ellipse>())
                dot.Visibility(Visibility::Collapsed);
            UpdateAlertBorder(root, false, g_settings.colorLow);
            return;
        }

        double tMin = s.readings.front().tMs;
        double tMax = s.predicted.empty() ? s.readings.back().tMs : s.predicted.back().tMs;
        double vMin = std::numeric_limits<double>::max(), vMax = std::numeric_limits<double>::lowest();
        auto consider = [&](double v) { vMin = std::min(vMin, v); vMax = std::max(vMax, v); };
        for (auto& p : s.readings) consider(p.v);
        for (auto& p : s.predicted) consider(p.v);
        if (rLo > 0) consider(rLo);
        if (rHi > 0) consider(rHi);
        // Pad the value range ~8% so the line doesn't touch the edges.
        double pad = (vMax - vMin) * 0.08; if (pad < 0.1) pad = 0.1;
        Scale sc{tMin, std::max(tMax - tMin, 1.0), vMin - pad, std::max((vMax + pad) - (vMin - pad), 0.1), w, h};

        // Target-range band behind the line (from settings — the summary has no range).
        if (auto band = root.FindName(L"RangeBand").try_as<Shapes::Rectangle>()) {
            bool show = g_settings.showBand && rLo > 0 && rHi > 0;
            band.Visibility(show ? Visibility::Visible : Visibility::Collapsed);
            if (show) {
                double yHi = MapY(sc, rHi), yLo = MapY(sc, rLo);
                band.Width(w);
                band.Height(std::max(yLo - yHi, 0.0));
                Controls::Canvas::SetLeft(band, 0.0);
                Controls::Canvas::SetTop(band, yHi);
            }
        }

        Media::PointCollection actual;
        for (auto& p : s.readings) actual.Append(Point{(float)MapX(sc, p.tMs), (float)MapY(sc, p.v)});
        setLine(L"SparkActual", actual);
        if (auto pl = root.FindName(L"SparkActual").try_as<Shapes::Polyline>())
            pl.Stroke(MakeBrush(StateColor(s.current, rLo, rHi)));

        // Predicted line: prepend the last actual point so it connects visually.
        Media::PointCollection pred;
        if (!s.predicted.empty()) {
            const GPoint& last = s.readings.back();
            pred.Append(Point{(float)MapX(sc, last.tMs), (float)MapY(sc, last.v)});
            for (auto& p : s.predicted) pred.Append(Point{(float)MapX(sc, p.tMs), (float)MapY(sc, p.v)});
        }
        setLine(L"SparkPredicted", pred);
        if (auto pl = root.FindName(L"SparkPredicted").try_as<Shapes::Polyline>())
            pl.Stroke(MakeBrush(g_settings.colorPredicted));

        if (auto dot = root.FindName(L"CurrentDot").try_as<Shapes::Ellipse>()) {
            dot.Visibility(Visibility::Visible);
            dot.Fill(MakeBrush(StateColor(s.current, rLo, rHi)));
            const GPoint& last = s.readings.back();
            Controls::Canvas::SetLeft(dot, MapX(sc, last.tMs) - 2.5);
            Controls::Canvas::SetTop(dot, MapY(sc, last.v) - 2.5);
        }

        // Alert pulse. Driven by the server alert engine: pulse when the summary
        // reports an active (un-silenced) alarm, coloured low vs high by the alarm
        // type. If the summary carried no alarm field at all, fall back to a local
        // range comparison so the card still reacts.
        bool pulseLow, doPulse;
        if (s.hasAlarmField) {
            doPulse = s.alarmActive;
            pulseLow = s.alarmLow;
        } else {
            bool low = rLo > 0 && s.current < rLo;
            bool high = rHi > 0 && s.current > rHi;
            doPulse = low || high;
            pulseLow = low;
        }
        bool pulse = g_settings.pulseOutOfRange && doPulse && !stale;
        UpdateAlertBorder(root, pulse, pulseLow ? g_settings.colorLow : g_settings.colorHigh);
    } catch (...) {
        Wh_Log(L"[ApplyWidget] error %08X", winrt::to_hresult());
    }
}

// Any thread: marshal a snapshot onto the XAML UI thread.
static void PostWidgetUpdate(const GlucoseSnapshot& s) {
    winrt::Windows::UI::Core::CoreDispatcher disp{nullptr};
    {
        std::lock_guard<std::mutex> lk(g_widgetMutex);
        g_lastSnapshot = s;
        disp = g_dispatcher;
    }
    if (!disp) return;
    disp.TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                     [s]() { ApplyWidget(s); });
}

// UI-thread, inside the layout pass: slide the shared TaskbarHost to track the app
// cluster as apps open/close. Find-only (never creates the host here — tree mutation
// inside the layout pass throws 0x800F1000); creation happens in the deferred insert.
// Only the host's RenderTransform changes, so this can't trigger heavy re-layout.
static void PositionHost(FrameworkElement rootGrid, double appLeft) {
    TaskbarHost::Reposition(TaskbarHost::Find(rootGrid), appLeft);
}

// UI-thread (from the layout hook): ensure our tile exists in the shared host.
static void InsertWidgetIfNeeded(FrameworkElement rootGrid) {
    static std::atomic<int> tries{0};
    bool log = tries.fetch_add(1) < 4;

    auto host = TaskbarHost::FindOrCreate(rootGrid);
    if (!host) {
        if (log) Wh_Log(L"[inject] could not find/create the TaskbarWidgetHost");
        return;
    }

    // Clear a stale tile orphaned by a previous instance (recompile), plus the legacy
    // pre-host root if upgrading from an older build. Done inline (not via RemoveTile,
    // which would also drop the host we are about to fill).
    try {
        auto hc = host.Children();
        for (uint32_t i = hc.Size(); i-- > 0;) {
            auto fe = hc.GetAt(i).try_as<FrameworkElement>();
            if (fe && fe.Name() == L"NocturneGlucoseTile") hc.RemoveAt(i);
        }
        auto rc = rootGrid.as<Controls::Panel>().Children();
        for (uint32_t i = rc.Size(); i-- > 0;) {
            auto fe = rc.GetAt(i).try_as<FrameworkElement>();
            if (fe && fe.Name() == L"NocturneGlucoseRoot") rc.RemoveAt(i);
        }
    } catch (...) {}

    FrameworkElement tile{nullptr};
    try {
        tile = Markup::XamlReader::Load(winrt::hstring{BuildWidgetXaml()}).as<FrameworkElement>();
    } catch (...) {
        if (log) Wh_Log(L"[inject] XamlReader::Load failed %08X", winrt::to_hresult());
        return;
    }
    TaskbarHost::AddTile(host, tile, g_settings.rank);

    GlucoseSnapshot replay;
    try {
        std::lock_guard<std::mutex> lk(g_widgetMutex);
        g_widgetRoot = winrt::make_weak(tile);
        g_rootGrid = winrt::make_weak(rootGrid);
        g_dispatcher = tile.Dispatcher();
        replay = g_lastSnapshot;
    } catch (...) {
        if (log) Wh_Log(L"[inject] dispatcher capture failed %08X", winrt::to_hresult());
        return;
    }
    Wh_Log(L"[inject] tile added to TaskbarWidgetHost (rank=%d)", g_settings.rank);
    if (replay.valid) ApplyWidget(replay);
}

// ---------------------------------------------------------------------------
// TaskbarFrame hook (Taskbar.View.dll)
// ---------------------------------------------------------------------------
using TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t = void(WINAPI*)(void* pThis);
TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t
    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original;
void WINAPI TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook(void* pThis) {
    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original(pThis);
    static std::atomic<int> seen{0};
    bool log = seen.fetch_add(1) < 3;
    try {
        // NOTE: layout/ABI-specific. This offset (+3 pointer slots) is taken
        // verbatim from taskbar-labels and is the prime suspect if injection
        // silently no-ops on a given build.
        void* abi = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown unk{nullptr};
        winrt::copy_from_abi(unk, abi);
        auto taskbarFrame = unk.try_as<FrameworkElement>();
        if (!taskbarFrame) {
            if (log) Wh_Log(L"[hook] fired but no FrameworkElement (ABI offset wrong?)");
            return;
        }
        auto rootGrid = FindChildByName(taskbarFrame, L"RootGrid");
        if (!rootGrid) rootGrid = FindChildByName(taskbarFrame, L"TaskbarFrameBorder");
        if (!rootGrid) {
            if (log) Wh_Log(L"[hook] RootGrid not found");
            return;
        }
        // Recompute the app-cluster left edge on every layout change (read-only —
        // safe inside the hook) and slide the shared host to track it. DO NOT run a
        // full ApplyWidget here: mutating text / brushes / Storyboards re-dirties
        // layout and re-enters this very hook (unbounded re-entrant layout). Only the
        // host's translate changes; the guard makes any accidental re-entry a no-op.
        double appLeft = ComputeAppLeftEdge(rootGrid, log);
        if (!g_inLayoutApply.exchange(true)) {
            PositionHost(rootGrid, appLeft);
            g_inLayoutApply.store(false);
        }

        {
            std::lock_guard<std::mutex> lk(g_widgetMutex);
            if (g_widgetRoot.get()) return;  // already own a live widget
        }
        // Schedule an insertion (deferred to avoid mutating the tree during
        // layout -> 0x800F1000). g_insertPending coalesces the frequent callbacks.
        if (!g_insertPending.exchange(true)) {
            rootGrid.Dispatcher().TryRunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                [rootGrid]() {
                    InsertWidgetIfNeeded(rootGrid);
                    g_insertPending.store(false);
                });
        }
    } catch (...) {
        if (log) Wh_Log(L"[hook] error %08X", winrt::to_hresult());
    }
}

static HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandle(L"ExplorerExtensions.dll");
    return module;
}

static bool HookTaskbarViewSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged(void))"},
            &TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original,
            TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook,
        },
    };
    if (!WindhawkUtils::HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"HookSymbols(Taskbar.View.dll) failed");
        return false;
    }
    return true;
}

// Hook LoadLibraryExW so we also catch a taskbar reload (explorer restart).
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module && !g_taskbarViewHooked.load() &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewHooked.exchange(true)) {
        if (HookTaskbarViewSymbols(module)) Wh_ApplyHookOperations();
    }
    return module;
}

// ---------------------------------------------------------------------------
// Reader thread: read the summary JSON -> snapshot -> UI
// ---------------------------------------------------------------------------
static void ReaderLoop() {
    while (g_running.load()) {
        GlucoseSnapshot s = ReadGlucose();
        if (s.ok)
            Wh_Log(L"[render] %s%s readings=%zu predicted=%zu alarm=%d",
                   FormatValue(s.current).c_str(), g_settings.unit.c_str(),
                   s.readings.size(), s.predicted.size(), s.alarmActive ? 1 : 0);
        PostWidgetUpdate(s);
        for (int i = 0; i < g_settings.pollSeconds * 10 && g_running.load(); i++)
            Sleep(100);
    }
}

// ---------------------------------------------------------------------------
// Settings + lifecycle
// ---------------------------------------------------------------------------
void LoadSettings() {
    g_settings.dataPaths.clear();
    PCWSTR dataPath = Wh_GetStringSetting(L"dataPath");
    if (dataPath && *dataPath) {
        // Explicit override: this path is the sole source (single-source, back-compat).
        g_settings.dataPaths.push_back(ExpandConfigPath(dataPath));
    } else {
        // Auto: the desktop companion's file, then the packaged widget's redirected file(s).
        g_settings.dataPaths.push_back(ExpandConfigPath(L"%LOCALAPPDATA%\\Nocturne\\glucose.json"));
        AddWidgetCandidatePaths(g_settings.dataPaths);
    }
    Wh_FreeStringSetting(dataPath);

    int poll = Wh_GetIntSetting(L"pollSeconds");
    g_settings.pollSeconds = poll < 1 ? 1 : poll;
    int stale = Wh_GetIntSetting(L"staleAfterSeconds");
    g_settings.staleAfterSeconds = stale < 1 ? 1 : stale;

    // Display unit: the $options key is "mmol" or "mgdl"; map to the format string.
    PCWSTR unit = Wh_GetStringSetting(L"unit");
    g_settings.unit = (unit && wcscmp(unit, L"mgdl") == 0) ? L"mg/dL" : L"mmol/L";
    Wh_FreeStringSetting(unit);

    // Target range, expressed in the display unit (the summary carries no range).
    auto getDouble = [](PCWSTR key, double def) {
        PCWSTR s = Wh_GetStringSetting(key);
        double v = def;
        if (s && *s) { wchar_t* end = nullptr; double parsed = wcstod(s, &end); if (end != s) v = parsed; }
        Wh_FreeStringSetting(s);
        return v;
    };
    g_settings.rangeLow = getDouble(L"rangeLow", 3.9);
    g_settings.rangeHigh = getDouble(L"rangeHigh", 10.0);

    auto color = [](PCWSTR key, COLORREF def) {
        PCWSTR s = Wh_GetStringSetting(key);
        COLORREF c = ParseHexColor(s, def);
        Wh_FreeStringSetting(s);
        return c;
    };
    g_settings.colorInRange = color(L"style.colorInRange", RGB(0x36, 0xC7, 0x6A));
    g_settings.colorHigh = color(L"style.colorHigh", RGB(0xE6, 0xB8, 0x00));
    g_settings.colorLow = color(L"style.colorLow", RGB(0xE0, 0x53, 0x3D));
    g_settings.colorPredicted = color(L"style.colorPredicted", RGB(0x8A, 0xA0, 0xB4));
    g_settings.showBand = Wh_GetIntSetting(L"style.showBand");
    g_settings.showCurrentValue = Wh_GetIntSetting(L"style.showCurrentValue");
    g_settings.showTrendArrow = Wh_GetIntSetting(L"style.showTrendArrow");
    g_settings.showIobCob = Wh_GetIntSetting(L"style.showIobCob");
    g_settings.sparkWidth = Wh_GetIntSetting(L"style.sparkWidth");
    g_settings.sparkHeight = Wh_GetIntSetting(L"style.sparkHeight");
    g_settings.lineThickness = Wh_GetIntSetting(L"style.lineThickness");
    g_settings.fontSize = Wh_GetIntSetting(L"style.fontSize");
    g_settings.autoTextColor = Wh_GetIntSetting(L"style.autoTextColor");
    g_settings.textColor = color(L"style.textColor", RGB(0xFF, 0xFF, 0xFF));

    g_settings.pulseOutOfRange = Wh_GetIntSetting(L"alert.pulseOutOfRange");
    g_settings.borderThickness = Wh_GetIntSetting(L"alert.borderThickness");

    g_settings.rank = Wh_GetIntSetting(L"rank");
}

static void StartReader() {
    g_running.store(true);
    g_readerThread = new std::thread(ReaderLoop);
}
static void StopReader() {
    g_running.store(false);
    if (g_readerThread) {
        if (g_readerThread->joinable()) g_readerThread->join();
        delete g_readerThread;
        g_readerThread = nullptr;
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init nocturne-glucose-sparkline");
    LoadSettings();
    Wh_Log(L"[init] %zu data path candidate(s):", g_settings.dataPaths.size());
    for (const auto& p : g_settings.dataPaths) Wh_Log(L"[init]   %s", p.c_str());

    if (HMODULE tv = GetTaskbarViewModuleHandle()) {
        if (HookTaskbarViewSymbols(tv)) {
            g_taskbarViewHooked.store(true);
            Wh_Log(L"[init] taskbar symbols hooked OK");
        }
    } else {
        Wh_Log(L"[init] Taskbar.View.dll / ExplorerExtensions.dll NOT found");
    }
    HMODULE kbase = GetModuleHandle(L"kernelbase.dll");
    auto pLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(kbase, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);

    StartReader();
    return TRUE;
}

// Runs while the hooks and the XAML UI thread are still live (unlike Wh_ModUninit at
// process shutdown), so this is where we revert our visible effect. Remove our tile
// synchronously: dispatch the tree mutation onto the UI thread and WAIT for it, so the
// tile is gone before the DLL unloads — no fire-and-forget use-after-free. The same
// dispatch releases the UI/COM globals on their apartment thread.
void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit nocturne-glucose-sparkline");

    winrt::Windows::UI::Core::CoreDispatcher disp{nullptr};
    winrt::weak_ref<FrameworkElement> rootRef;
    {
        std::lock_guard<std::mutex> lk(g_widgetMutex);
        disp = g_dispatcher;
        rootRef = g_rootGrid;
    }
    if (!disp) return;  // never inserted a tile; nothing live to tear down

    HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    disp.TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                     [rootRef, done]() {
                         if (auto rg = rootRef.get())
                             TaskbarHost::RemoveTile(rg, L"NocturneGlucoseTile");
                         // Release UI/COM globals on the apartment thread.
                         if (g_borderStoryboard) { try { g_borderStoryboard.Stop(); } catch (...) {} }
                         g_borderStoryboard = nullptr;
                         g_borderBrushTarget = {};
                         std::lock_guard<std::mutex> lk(g_widgetMutex);
                         g_widgetRoot = {};
                         g_rootGrid = {};
                         g_dispatcher = nullptr;
                         if (done) SetEvent(done);
                     });
    // Bounded wait so a wedged UI thread can't hang unload indefinitely.
    if (done) { WaitForSingleObject(done, 3000); CloseHandle(done); }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit nocturne-glucose-sparkline");
    StopReader();  // join + delete the worker while the process is alive
    // The tile and the UI/COM globals are torn down in Wh_ModBeforeUninit on the UI
    // thread (apartment still valid). At process shutdown neither runs and the
    // heap-leaked globals are reclaimed by the OS (see their declarations).
}

// Reload on a settings change. fontSize, lineThickness and the sparkline width/height
// are baked into the tile's XAML at construction (BuildWidgetXaml) and can't be
// re-applied in place, so we rebuild via the reload path: it re-runs Wh_ModInit and
// InsertWidgetIfNeeded reconstructs the tile with the new geometry.
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"SettingsChanged");
    *bReload = TRUE;
    return TRUE;
}
