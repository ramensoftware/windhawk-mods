// ==WindhawkMod==
// @id              custom-win-p-layouts
// @name            Custom Win+P Layouts
// @description     Replace the Win+P flyout with your own screen combinations for any multi-monitor setup
// @version         1.0.0
// @author          Hans
// @github          https://github.com/harafuhub
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32 -lshell32 -ldwmapi -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Win+P Layouts

The Windows **Win+P** flyout only knows "PC screen only", "Duplicate",
"Extend" and "Second screen only". With three or more screens you cannot
pick *which* screens should be on. This mod replaces the flyout with a list
of screen combinations: an automatic one, or your own.

## Using it

- **Win+P** opens the flyout. The row with the coloured bar is the layout
  active now. Rows for screens that are not connected are hidden.
- **Hold Win and tap P** to move through the rows. Stop tapping (or let go of
  Win) and the highlighted layout is applied after a moment. This also works
  when a screen is dark, so you can switch blind.
- Clicking a row, or arrow keys + Enter, also work. **Esc** closes.
- **Win+Shift+P** and the *Windows Project menu* row open the original flyout
  (for Duplicate etc.).

## Make the list your own

Every combination of your screens is a row. **Right-click a row** (or press
the Menu key) to:

- **Rename** the row, e.g. "Desk" instead of "Built-in screen + DELL U2720Q"
- **Rename a screen**, e.g. "Lower" instead of "Screen". The new name shows
  in every row you have not renamed yourself
- **Hide** it, if you never use that combination
- **Move** it up or down
- choose its **main display** (taskbar and Start menu)

**Show hidden rows** in the same menu brings hidden rows back. Your changes
are kept when screens are unplugged and plugged in again. With 4 or more
screens, only the common combinations are shown at first.

Windows remembers position and resolution for each combination. If a
combination opens with an odd arrangement, fix it once in Display settings.

## Optional

- **Ask to keep the new layout**: the old layout returns after 15 seconds
  unless you click Keep.
- On the sign-in screen and in apps running as administrator, Windows may
  show the original flyout.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- askToKeep: false
  $name: Ask to keep the new layout
  $description: When on, the old layout returns after 15 seconds unless you click Keep
- applyDelay: 1200
  $name: Auto-apply delay (ms)
  $description: How long after the last Win+P tap the highlighted layout is applied
- hideWindowsEntry: false
  $name: Hide the "Windows Project menu" row
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>

#ifdef DUO_STANDALONE
// Test tool: settings come from environment variables, e.g. the setting
// "askToKeep" is read from WH_askToKeep.
static std::wstring StubSetting(PCWSTR fmt, va_list args) {
    wchar_t key[128], value[1024];
    vswprintf(key, 128, fmt, args);
    std::wstring env = L"WH_";
    for (const wchar_t* c = key; *c; ++c) env += iswalnum(*c) ? *c : L'_';
    DWORD n = GetEnvironmentVariableW(env.c_str(), value, 1024);
    return n && n < 1024 ? value : L"";
}
static int Wh_GetIntSetting(PCWSTR fmt, ...) {
    va_list a; va_start(a, fmt);
    std::wstring v = StubSetting(fmt, a);
    va_end(a);
    return v == L"true" ? 1 : _wtoi(v.c_str());
}
static void Wh_Log(PCWSTR, ...) {}
// Saved values go to files in %TEMP%, e.g. custom-win-p-layouts-rows.txt.
static std::wstring StubFile(PCWSTR name) {
    wchar_t dir[MAX_PATH];
    GetTempPathW(MAX_PATH, dir);
    return std::wstring(dir) + L"custom-win-p-layouts-" + name + L".txt";
}
static size_t Wh_GetStringValue(PCWSTR name, PWSTR buffer, size_t chars) {
    buffer[0] = 0;
    FILE* f = _wfopen(StubFile(name).c_str(), L"rb");
    if (!f) return 0;
    size_t n = fread(buffer, sizeof(wchar_t), chars - 1, f);
    fclose(f);
    buffer[n] = 0;
    return n;
}
static BOOL Wh_SetStringValue(PCWSTR name, PCWSTR value) {
    FILE* f = _wfopen(StubFile(name).c_str(), L"wb");
    if (!f) return FALSE;
    fwrite(value, sizeof(wchar_t), wcslen(value), f);
    fclose(f);
    return TRUE;
}
#endif

namespace duo {

// ---------------------------------------------------------------- display backend

using Path = DISPLAYCONFIG_PATH_INFO;
using Mode = DISPLAYCONFIG_MODE_INFO;
constexpr int kMaxTargets = 6;

struct Target {
    LUID adapter{};
    UINT32 id{};
    std::wstring monitor;  // the monitor's own name (may be empty)
    std::wstring name;     // name shown in the flyout
    bool internal{};
    bool active{};
    bool primary{};        // the main display right now
};

std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t"), b = s.find_last_not_of(L" \t");
    return a == std::wstring::npos ? L"" : s.substr(a, b - a + 1);
}

// Saved values: lines of tab-separated fields.
std::vector<std::vector<std::wstring>> ReadLines(PCWSTR value) {
    std::vector<wchar_t> buf(65536);
    buf[0] = 0;
    Wh_GetStringValue(value, buf.data(), buf.size());
    std::wstring s = buf.data();
    std::vector<std::vector<std::wstring>> lines;
    size_t pos = 0;
    while (pos < s.size()) {
        size_t end = s.find(L'\n', pos);
        if (end == std::wstring::npos) end = s.size();
        std::wstring line = s.substr(pos, end - pos);
        pos = end + 1;
        std::vector<std::wstring> f;
        size_t a = 0;
        for (;;) {
            size_t b = line.find(L'\t', a);
            f.push_back(line.substr(a, b == std::wstring::npos ? std::wstring::npos : b - a));
            if (b == std::wstring::npos) break;
            a = b + 1;
        }
        lines.push_back(f);
    }
    return lines;
}

std::wstring Field(std::wstring s) {  // no tabs or line breaks inside a field
    std::replace(s.begin(), s.end(), L'\t', L' ');
    std::replace(s.begin(), s.end(), L'\n', L' ');
    std::replace(s.begin(), s.end(), L'\r', L' ');
    return s;
}

// Names the user gave screens in the flyout, saved as "<screen ID>\t<name>".
void SaveScreenName(UINT32 id, const std::wstring& name) {
    std::wstring s;
    for (const auto& f : ReadLines(L"screens"))
        if (f.size() >= 2 && wcstoul(f[0].c_str(), nullptr, 10) != id && !f[1].empty())
            s += f[0] + L"\t" + f[1] + L"\n";
    if (!name.empty()) s += std::to_wstring(id) + L"\t" + Field(name) + L"\n";
    if (!Wh_SetStringValue(L"screens", s.c_str())) Wh_Log(L"Saving the screen names failed");
}

struct Snapshot {
    std::vector<Path> paths;
    std::vector<Mode> modes;
};

bool SameLuid(const LUID& a, const LUID& b) {
    return a.LowPart == b.LowPart && a.HighPart == b.HighPart;
}
bool IsTarget(const Path& p, const Target& t) {
    return SameLuid(p.targetInfo.adapterId, t.adapter) && p.targetInfo.id == t.id;
}

LONG Query(UINT32 flags, Snapshot& out) {
    for (int retry = 0; retry < 5; ++retry) {
        UINT32 np = 0, nm = 0;
        LONG e = GetDisplayConfigBufferSizes(flags, &np, &nm);
        if (e) return e;
        Snapshot s;
        s.paths.resize(np);
        s.modes.resize(nm);
        e = QueryDisplayConfig(flags, &np, s.paths.data(), &nm, s.modes.data(), nullptr);
        if (e == ERROR_INSUFFICIENT_BUFFER) continue;
        if (e) return e;
        s.paths.resize(np);
        s.modes.resize(nm);
        out = std::move(s);
        return ERROR_SUCCESS;
    }
    return ERROR_RETRY;
}

bool IsInternalTech(DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY t) {
    return t == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
           t == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
           t == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED ||
           t == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS;
}

std::vector<Target> Enumerate() {
    std::vector<Target> list;
    Snapshot all;
    if (LONG e = Query(QDC_ALL_PATHS, all)) {
        Wh_Log(L"QueryDisplayConfig failed: %ld", e);
        return list;
    }
    for (const Path& p : all.paths) {
        if (!p.targetInfo.targetAvailable) continue;
        Target* t = nullptr;
        for (auto& x : list)
            if (IsTarget(p, x)) t = &x;
        if (!t) {
            Target n;
            n.adapter = p.targetInfo.adapterId;
            n.id = p.targetInfo.id;
            n.internal = IsInternalTech(p.targetInfo.outputTechnology);
            DISPLAYCONFIG_TARGET_DEVICE_NAME dn{};
            dn.header = {DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME, sizeof(dn), n.adapter, n.id};
            if (!DisplayConfigGetDeviceInfo(&dn.header)) n.monitor = dn.monitorFriendlyDeviceName;
            list.push_back(n);
            t = &list.back();
        }
        if (p.flags & DISPLAYCONFIG_PATH_ACTIVE) {
            t->active = true;
            UINT32 m = p.sourceInfo.modeInfoIdx;
            if (m < all.modes.size() && all.modes[m].infoType == DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE) {
                POINTL pos = all.modes[m].sourceMode.position;
                if (pos.x == 0 && pos.y == 0) t->primary = true;
            }
        }
    }
    // Built-in panels first, then the others in the order Windows reports them.
    std::stable_partition(list.begin(), list.end(), [](const Target& t) { return t.internal; });
    if (list.size() > kMaxTargets) list.resize(kMaxTargets);

    // Names: the monitor's own name. Identical monitors (or several unnamed
    // ones) get a number.
    for (size_t k = 0; k < list.size(); ++k) {
        Target& t = list[k];
        t.name = !t.monitor.empty() ? t.monitor : t.internal ? L"Built-in screen" : L"Screen";
        int same = 1;
        for (size_t j = 0; j < k; ++j)
            if (list[j].name == t.name || list[j].name.starts_with(t.name + L" ")) ++same;
        if (same > 1) t.name += L" " + std::to_wstring(same);
    }
    // Names given with "Rename screen" win.
    for (const auto& f : ReadLines(L"screens"))
        if (f.size() >= 2 && !f[1].empty())
            for (auto& t : list)
                if (t.id == wcstoul(f[0].c_str(), nullptr, 10)) t.name = f[1];
    return list;
}

unsigned ActiveMask(const std::vector<Target>& t) {
    unsigned m = 0;
    for (size_t i = 0; i < t.size(); ++i) if (t[i].active) m |= 1u << i;
    return m;
}

// Build a topology that turns on exactly the targets in `mask`.
LONG ApplyMask(const std::vector<Target>& targets, unsigned mask) {
    Snapshot all;
    if (LONG e = Query(QDC_ALL_PATHS, all)) return e;
    std::vector<Path> chosen;
    std::vector<bool> done(targets.size(), false);
    // Screens that are on already keep their source.
    for (size_t i = 0; i < targets.size(); ++i) {
        if (!(mask & (1u << i))) continue;
        for (const Path& p : all.paths)
            if (IsTarget(p, targets[i]) && (p.flags & DISPLAYCONFIG_PATH_ACTIVE)) {
                chosen.push_back(p); done[i] = true; break;
            }
    }
    // New screens get a free source on their adapter.
    for (size_t i = 0; i < targets.size(); ++i) {
        if (!(mask & (1u << i)) || done[i]) continue;
        for (const Path& p : all.paths) {
            if (!IsTarget(p, targets[i])) continue;
            bool used = false;
            for (const Path& c : chosen)
                if (SameLuid(c.sourceInfo.adapterId, p.sourceInfo.adapterId) &&
                    c.sourceInfo.id == p.sourceInfo.id) used = true;
            if (!used) { chosen.push_back(p); done[i] = true; break; }
        }
        if (!done[i]) return ERROR_NOT_FOUND;
    }
    for (Path& c : chosen) {
        c.flags = DISPLAYCONFIG_PATH_ACTIVE;
        c.sourceInfo.modeInfoIdx = DISPLAYCONFIG_PATH_MODE_IDX_INVALID;
        c.targetInfo.modeInfoIdx = DISPLAYCONFIG_PATH_MODE_IDX_INVALID;
    }
    UINT32 n = static_cast<UINT32>(chosen.size());
    // 1) Reuse the arrangement Windows remembers for this combination.
    LONG e = SetDisplayConfig(n, chosen.data(), 0, nullptr,
                              SDC_APPLY | SDC_TOPOLOGY_SUPPLIED | SDC_ALLOW_PATH_ORDER_CHANGES);
    if (e == ERROR_SUCCESS) return e;
    Wh_Log(L"Topology from database failed (%ld), letting Windows pick modes", e);
    // 2) First time this combination is used: let Windows choose modes, and remember it.
    std::vector<Path> copy = chosen;
    e = SetDisplayConfig(n, copy.data(), 0, nullptr,
                         SDC_APPLY | SDC_USE_SUPPLIED_DISPLAY_CONFIG | SDC_ALLOW_CHANGES |
                         SDC_SAVE_TO_DATABASE);
    if (e) Wh_Log(L"SetDisplayConfig failed: %ld", e);
    return e;
}

// Make `t` the main display: Windows treats the screen at (0, 0) as the main
// one, so shift every screen so that `t` lands there.
LONG SetPrimary(const Target& t) {
    Snapshot s;
    if (LONG e = Query(QDC_ONLY_ACTIVE_PATHS, s)) return e;
    POINTL origin{};
    bool found = false;
    for (const Path& p : s.paths) {
        UINT32 m = p.sourceInfo.modeInfoIdx;
        if (IsTarget(p, t) && m < s.modes.size() &&
            s.modes[m].infoType == DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE) {
            origin = s.modes[m].sourceMode.position;
            found = true;
            break;
        }
    }
    if (!found) return ERROR_NOT_FOUND;
    if (origin.x == 0 && origin.y == 0) return ERROR_SUCCESS;
    for (Mode& m : s.modes)
        if (m.infoType == DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE) {
            m.sourceMode.position.x -= origin.x;
            m.sourceMode.position.y -= origin.y;
        }
    LONG e = SetDisplayConfig(static_cast<UINT32>(s.paths.size()), s.paths.data(),
                              static_cast<UINT32>(s.modes.size()), s.modes.data(),
                              SDC_APPLY | SDC_USE_SUPPLIED_DISPLAY_CONFIG | SDC_ALLOW_CHANGES |
                              SDC_SAVE_TO_DATABASE);
    if (e) Wh_Log(L"Setting the main display failed: %ld", e);
    return e;
}

LONG ApplyLayout(const std::vector<Target>& targets, unsigned mask, int primary) {
    if (LONG e = ApplyMask(targets, mask)) return e;
    if (primary < 0 || primary >= static_cast<int>(targets.size())) return ERROR_SUCCESS;
    return SetPrimary(targets[primary]);
}

LONG Restore(Snapshot s) {
    LONG e = s.paths.empty() ? ERROR_INVALID_DATA
        : SetDisplayConfig(static_cast<UINT32>(s.paths.size()), s.paths.data(),
                           static_cast<UINT32>(s.modes.size()), s.modes.data(),
                           SDC_APPLY | SDC_USE_SUPPLIED_DISPLAY_CONFIG | SDC_ALLOW_CHANGES |
                           SDC_SAVE_TO_DATABASE);
    if (e) {
        Wh_Log(L"Exact restore failed (%ld), using saved layout", e);
        e = SetDisplayConfig(0, nullptr, 0, nullptr, SDC_APPLY | SDC_USE_DATABASE_CURRENT);
    }
    return e;
}

// ---------------------------------------------------------------- UI state

enum Kind { kCombo, kSettings, kNative };
struct Option {
    Kind kind;
    unsigned mask;
    std::wstring label, sub;
    int primary = -1;    // index into targets of the main display, -1 = leave as is
    std::wstring key;    // the saved row this option shows
    bool hidden = false;
};

constexpr wchar_t kClass[] = L"CustomWinPLayoutsFlyout_100";
constexpr UINT kShow = WM_APP + 17, kWinUp = WM_APP + 18;
constexpr UINT_PTR kCountdownTimer = 1, kCommitTimer = 2, kAnimTimer = 3;
bool cycled = false;  // selection moved with Win+P since the flyout opened

// Slide animation: up + fade in when opening, down + fade out when closing.
constexpr double kInMs = 190, kOutMs = 130;
constexpr int kSlide = 40;   // distance in DIPs
double animP = 0;            // 0 = hidden, 1 = fully shown
int animDir = 0;             // +1 opening, -1 closing, 0 idle
LONGLONG animLast = 0;
int restX = 0, restY = 0;    // final position computed by Place()

int ApplyDelay() {
    int ms = Wh_GetIntSetting(L"applyDelay");
    return ms <= 0 ? 1200 : std::min(ms, 10000);
}
constexpr int kKeep = -10, kRevert = -11;
constexpr int kWidth = 380, kHeader = 56, kRow = 56, kPad = 8, kFooter = 104;
constexpr ULONGLONG kConfirmMs = 15000;

HINSTANCE instance{};
HANDLE uiThread{}, hookThread{}, stopEvent{}, readyEvent{};
DWORD uiId{}, hookId{};
std::atomic<HWND> hookPanel{nullptr};
bool preview = false;

HWND panel{};
std::vector<Target> targets;
std::vector<Option> options;
int sel = 0, hot = -1;
bool pending = false;
ULONGLONG deadline = 0;
Snapshot before;
std::wstring status;
int dpi = 96;
HFONT fTitle{}, fLabel{}, fSub{}, fIcon{};
bool dark = true;
COLORREF accent = RGB(0, 120, 215);

int Px(int n) { return MulDiv(n, dpi, 96); }

void LoadTheme() {
    DWORD v = 0, size = sizeof(v);
    dark = RegGetValueW(HKEY_CURRENT_USER,
               L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
               L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &size) != ERROR_SUCCESS ||
           v == 0;
    DWORD c = 0; BOOL opaque = FALSE;
    size = sizeof(c);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"AccentColor",
                     RRF_RT_REG_DWORD, nullptr, &c, &size) == ERROR_SUCCESS)
        accent = RGB(c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF);  // stored as ABGR
    else if (SUCCEEDED(DwmGetColorizationColor(&c, &opaque)))
        accent = RGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
}

void MakeFonts() {
    for (HFONT* f : {&fTitle, &fLabel, &fSub, &fIcon}) if (*f) { DeleteObject(*f); *f = nullptr; }
    auto mk = [](int size, int weight, const wchar_t* face) {
        return CreateFontW(-Px(size), 0, 0, 0, weight, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 0, face);
    };
    fTitle = mk(20, FW_SEMIBOLD, L"Segoe UI Variable Display");
    fLabel = mk(14, FW_NORMAL, L"Segoe UI Variable Text");
    fSub = mk(12, FW_NORMAL, L"Segoe UI Variable Text");
    fIcon = mk(16, FW_NORMAL, L"Segoe Fluent Icons");
}

// ---------------------------------------------------------------- saved rows

// Every combination of screens is a row. What the user changed in the flyout
// (order, name, hidden, main display) is saved with Windhawk, one line per row:
// "<screen IDs joined by +>\t<hidden 0/1>\t<main display ID or 0>\t<name>".
// Rows whose screens are unplugged stay saved.
struct Row {
    std::wstring key, label;
    bool hidden = false;
    UINT32 primary = 0;
};
std::vector<Row> rows;      // in flyout order
bool showHidden = false;    // "Show hidden rows" is on (until the flyout closes)

std::wstring KeyOf(unsigned mask) {
    std::vector<UINT32> ids;
    for (size_t i = 0; i < targets.size(); ++i)
        if (mask & (1u << i)) ids.push_back(targets[i].id);
    std::sort(ids.begin(), ids.end());
    std::wstring k;
    for (UINT32 id : ids) k += (k.empty() ? L"" : L"+") + std::to_wstring(id);
    return k;
}

// The screens of a row as a mask over `targets`, or 0 if one is not connected.
unsigned MaskOf(const std::wstring& key) {
    unsigned m = 0;
    size_t pos = 0;
    while (pos < key.size()) {
        size_t end = key.find(L'+', pos);
        if (end == std::wstring::npos) end = key.size();
        UINT32 id = wcstoul(key.substr(pos, end - pos).c_str(), nullptr, 10);
        pos = end + 1;
        int found = -1;
        for (size_t i = 0; i < targets.size(); ++i)
            if (targets[i].id == id) found = static_cast<int>(i);
        if (found < 0) return 0;
        m |= 1u << found;
    }
    return m;
}

Row* FindRow(const std::wstring& key) {
    for (auto& r : rows) if (r.key == key) return &r;
    return nullptr;
}

void LoadRows() {
    rows.clear();
    for (const auto& f : ReadLines(L"rows")) {
        if (f.size() < 4 || f[0].empty() || FindRow(f[0])) continue;
        rows.push_back({f[0], f[3], f[1] == L"1",
                        static_cast<UINT32>(wcstoul(f[2].c_str(), nullptr, 10))});
    }
}

void SaveRows() {
    std::wstring s;
    for (const auto& r : rows)
        s += r.key + L"\t" + (r.hidden ? L"1" : L"0") + L"\t" + std::to_wstring(r.primary) +
             L"\t" + Field(r.label) + L"\n";
    if (!Wh_SetStringValue(L"rows", s.c_str())) Wh_Log(L"Saving the rows failed");
}

// The row matches what is on screen now: same screens and, if the row names a
// main display, that one is the main display.
bool IsCurrent(const Option& o) {
    if (o.kind != kCombo || o.mask != ActiveMask(targets)) return false;
    return o.primary < 0 || o.primary >= static_cast<int>(targets.size()) ||
           targets[o.primary].primary;
}

void BuildOptions() {
    targets = Enumerate();
    LoadRows();
    options.clear();
    int n = static_cast<int>(targets.size());

    // Combinations not seen before go to the end. With 4+ screens, only each
    // screen alone, the first screen with each other one, and all screens
    // start visible; the rest start hidden.
    unsigned all = n ? (1u << n) - 1 : 0;
    for (int size = 1; size <= n; ++size)
        for (unsigned m = 1; m <= all; ++m) {
            if (__builtin_popcount(m) != size || FindRow(KeyOf(m))) continue;
            bool common = n <= 3 || size == 1 || m == all || (size == 2 && (m & 1));
            rows.push_back({KeyOf(m), L"", !common, 0});
        }

    for (const Row& row : rows) {
        unsigned m = MaskOf(row.key);
        if (!m || (row.hidden && !showHidden)) continue;
        int size = __builtin_popcount(m);
        int primary = -1;
        for (int i = 0; i < n; ++i)
            if ((m & (1u << i)) && row.primary && targets[i].id == row.primary && size > 1)
                primary = i;
        Option o{kCombo, m, row.label, L"", primary, row.key, row.hidden};
        std::wstring names;
        for (int i = 0; i < n; ++i)
            if (m & (1u << i)) names += (names.empty() ? L"" : L" + ") + targets[i].name;
        if (o.label.empty()) o.label = size == 1 ? names + L" only" : names;
        if (size == 1) o.sub = L"ID " + std::to_wstring(targets[__builtin_ctz(m)].id);
        else o.sub = size == n ? L"Extend across all screens" : L"Extend";
        if (o.label != names && o.label != names + L" only") o.sub = names;
        if (primary >= 0) o.sub += L"  ·  main: " + targets[primary].name;
        if (row.hidden) o.sub = L"Hidden  ·  " + o.sub;
        options.push_back(o);
    }
    options.push_back({kSettings, 0, L"Display settings", L"Arrange, scale and resolution"});
    if (!Wh_GetIntSetting(L"hideWindowsEntry"))
        options.push_back({kNative, 0, L"Windows Project menu", L"Duplicate and other built-in options"});
}

int ListTop() { return Px(kHeader); }
RECT RowRect(int i) {
    RECT r{0, 0, 0, 0};
    GetClientRect(panel, &r);
    return {Px(kPad), ListTop() + i * Px(kRow), r.right - Px(kPad), ListTop() + (i + 1) * Px(kRow)};
}
int FooterTop() { return ListTop() + static_cast<int>(options.size()) * Px(kRow) + Px(kPad); }
RECT ButtonRect(int which) {  // 0 = Keep, 1 = Revert
    RECT c{}; GetClientRect(panel, &c);
    int w = (c.right - Px(kPad) * 3 - Px(8)) / 2;
    int x = Px(kPad) * 2 + which * (w + Px(8)) - Px(kPad);
    int y = FooterTop() + Px(44);
    return {x + Px(kPad), y, x + Px(kPad) + w, y + Px(36)};
}
bool HasFooter() { return pending || !status.empty(); }
int PanelHeight() {
    return FooterTop() + (HasFooter() ? Px(kFooter) : Px(4));
}

int HitTest(POINT pt) {
    if (pending) {
        RECT k = ButtonRect(0), r = ButtonRect(1);
        if (PtInRect(&k, pt)) return kKeep;
        if (PtInRect(&r, pt)) return kRevert;
        return -1;
    }
    for (int i = 0; i < static_cast<int>(options.size()); ++i) {
        RECT r = RowRect(i);
        if (PtInRect(&r, pt)) return i;
    }
    return -1;
}

void Place() {
    POINT pt{}; GetCursorPos(&pt);
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    UINT dx = 96, dy = 96;
    if (SUCCEEDED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dx, &dy)) && static_cast<int>(dx) != dpi) {
        dpi = static_cast<int>(dx);
        MakeFonts();
    }
    MONITORINFO mi{}; mi.cbSize = sizeof(mi);
    GetMonitorInfoW(mon, &mi);
    // Size first (client area depends on width), then compute height.
    int w = Px(kWidth);
    SetWindowPos(panel, nullptr, 0, 0, w, Px(200), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    int h = PanelHeight();
    int maxH = (mi.rcWork.bottom - mi.rcWork.top) - Px(24);
    if (h > maxH) h = maxH;
    restX = mi.rcWork.right - w - Px(12);
    restY = mi.rcWork.bottom - h - Px(12);
    int y = animDir ? restY + Px(kSlide) : restY;  // mid-animation: next frame fixes it
    SetWindowPos(panel, HWND_TOPMOST, restX, y, w, h, SWP_NOACTIVATE);
    InvalidateRect(panel, nullptr, FALSE);
}

// ---------------------------------------------------------------- animation

LONGLONG NowMs() {
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return c.QuadPart * 1000 / f.QuadPart;
}

bool AnimationsOn() {
    BOOL on = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &on, 0);
    return on != FALSE;
}

void SetLayered(bool on) {
    LONG_PTR ex = GetWindowLongPtrW(panel, GWL_EXSTYLE);
    LONG_PTR want = on ? (ex | WS_EX_LAYERED) : (ex & ~static_cast<LONG_PTR>(WS_EX_LAYERED));
    if (want == ex) return;
    SetWindowLongPtrW(panel, GWL_EXSTYLE, want);
    if (!on) RedrawWindow(panel, nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
}

void ApplyAnim() {
    double e = 1 - (1 - animP) * (1 - animP) * (1 - animP);  // ease-out cubic
    int y = restY + static_cast<int>((1 - e) * Px(kSlide) + 0.5);
    SetWindowPos(panel, nullptr, restX, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    SetLayeredWindowAttributes(panel, 0, static_cast<BYTE>(255 * e + 0.5), LWA_ALPHA);
}

void FinishAnim() {
    KillTimer(panel, kAnimTimer);
    int dir = animDir;
    animDir = 0;
    if (dir > 0) {
        animP = 1;
        SetWindowPos(panel, nullptr, restX, restY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        SetLayered(false);
    } else {
        animP = 0;
        ShowWindow(panel, SW_HIDE);
        SetLayered(false);
        if (preview) DestroyWindow(panel);
    }
}

// dir +1: slide in (shows the window), -1: slide out (hides it at the end).
// Reversing mid-way continues from the current position.
void StartAnim(int dir) {
    bool visible = IsWindowVisible(panel);
    if (dir < 0 && (!visible || animDir < 0)) return;
    if (dir > 0 && visible && animDir == 0) return;  // already fully shown
    if (!visible) animP = 0;
    animDir = dir;
    if (!AnimationsOn()) {
        if (dir > 0) ShowWindow(panel, SW_SHOW);
        FinishAnim();
        return;
    }
    SetLayered(true);
    ApplyAnim();
    if (!visible) ShowWindow(panel, SW_SHOW);
    animLast = NowMs();
    SetTimer(panel, kAnimTimer, 8, nullptr);
}

void AnimTick() {
    LONGLONG now = NowMs();
    double dt = static_cast<double>(now - animLast);
    animLast = now;
    animP += animDir * dt / (animDir > 0 ? kInMs : kOutMs);
    if (animP >= 1 || animP <= 0) FinishAnim();
    else ApplyAnim();
}

void Appear() { StartAnim(+1); }
void Dismiss() { StartAnim(-1); }
bool Shown() { return IsWindowVisible(panel) && animDir >= 0; }

void Refresh() {
    bool keep = sel >= 0 && sel < static_cast<int>(options.size()) && options[sel].kind == kCombo;
    Option was = keep ? options[sel] : Option{};
    BuildOptions();
    sel = 0;
    int count = static_cast<int>(options.size());
    for (int i = count - 1; i >= 0; --i)
        if (keep ? options[i].kind == kCombo && options[i].mask == was.mask &&
                       options[i].primary == was.primary
                 : IsCurrent(options[i]))
            sel = i;
    if (IsWindowVisible(panel)) Place();
}

void ForceForeground(HWND w) {
    if (SetForegroundWindow(w)) return;
    HWND fg = GetForegroundWindow();
    DWORD fgThread = fg ? GetWindowThreadProcessId(fg, nullptr) : 0, me = GetCurrentThreadId();
    if (fgThread && fgThread != me) {
        AttachThreadInput(me, fgThread, TRUE);
        SetForegroundWindow(w);
        AttachThreadInput(me, fgThread, FALSE);
    }
}

// ---------------------------------------------------------------- editing rows

constexpr UINT kRenameDone = WM_APP + 19;  // wParam: 1 = keep the new name
HWND editBox{};
WNDPROC editProc{};
HBRUSH editBrush{};
std::wstring editKey, editDefault;
int editScreen = -1;  // >= 0: renaming this screen (index into targets) instead of the row

int IconWidth() {
    int n = std::max(static_cast<int>(targets.size()), 2);
    return Px(18) * n + Px(4) * (n - 1);
}

LRESULT CALLBACK EditProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_KEYDOWN && (wp == VK_RETURN || wp == VK_ESCAPE)) {
        PostMessageW(panel, kRenameDone, wp == VK_RETURN, 0);
        return 0;
    }
    if (msg == WM_CHAR && (wp == L'\r' || wp == 27)) return 0;  // no beep
    if (msg == WM_KILLFOCUS) PostMessageW(panel, kRenameDone, 1, 0);
    return CallWindowProcW(editProc, h, msg, wp, lp);
}

void EndRename(bool keep) {
    if (!editBox) return;
    wchar_t text[256]{};
    GetWindowTextW(editBox, text, 256);
    HWND box = editBox;
    editBox = nullptr;  // destroying it sends WM_KILLFOCUS -> ignored now
    DestroyWindow(box);
    if (editBrush) { DeleteObject(editBrush); editBrush = nullptr; }
    SetFocus(panel);
    if (keep && editScreen >= 0 && editScreen < static_cast<int>(targets.size())) {
        // A screen's name shows in every row that has not been renamed.
        // Empty = back to the monitor's own name.
        std::wstring name = Trim(text);
        if (name != targets[editScreen].name) SaveScreenName(targets[editScreen].id, name);
        Refresh();
    } else if (keep) {
        std::wstring label = Trim(text);
        if (label == editDefault) label.clear();  // unchanged: keep following the screen names
        if (Row* r = FindRow(editKey)) {
            r->label = label;
            SaveRows();
        }
        Refresh();
    }
    InvalidateRect(panel, nullptr, FALSE);
}

// Rename row `i`, or with `screen` >= 0 that screen (shown in place of row i's name).
void BeginRename(int i, int screen = -1) {
    EndRename(false);
    const Option& o = options[i];
    const Row* row = FindRow(o.key);
    if (!row) return;
    editKey = o.key;
    editScreen = screen;
    editDefault = row->label.empty() ? o.label : L"";
    const std::wstring& text = screen >= 0 ? targets[screen].name : o.label;
    RECT r = RowRect(i);
    int left = r.left + Px(14) + IconWidth() + Px(12);
    editBrush = CreateSolidBrush(dark ? RGB(56, 56, 56) : RGB(222, 222, 222));
    editBox = CreateWindowExW(0, L"EDIT", text.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                              left, r.top + Px(9), r.right - Px(10) - left, Px(22), panel,
                              nullptr, instance, nullptr);
    if (!editBox) return;
    SendMessageW(editBox, WM_SETFONT, reinterpret_cast<WPARAM>(fLabel), FALSE);
    SendMessageW(editBox, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, 0);
    SendMessageW(editBox, EM_LIMITTEXT, 100, 0);
    editProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(editBox, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(EditProc)));
    SendMessageW(editBox, EM_SETSEL, 0, -1);
    SetFocus(editBox);
}

enum MenuId { kMenuRename = 1, kMenuHide, kMenuUp, kMenuDown, kMenuShowHidden, kMenuReset,
              kMenuMainAuto, kMenuMain = 100, kMenuScreen = 200 };

// Right-click menu for row `i` (or for the empty area / settings rows).
void ShowMenu(int i, POINT at) {
    KillTimer(panel, kCommitTimer);
    cycled = false;
    EndRename(true);
    int count = static_cast<int>(options.size());
    bool row = i >= 0 && i < count && options[i].kind == kCombo;
    HMENU menu = CreatePopupMenu();
    if (row) {
        sel = i;
        InvalidateRect(panel, nullptr, FALSE);
        const Option& o = options[i];
        bool up = i > 0 && options[i - 1].kind == kCombo;
        bool down = i + 1 < count && options[i + 1].kind == kCombo;
        AppendMenuW(menu, MF_STRING, kMenuRename, L"Rename row");
        if (__builtin_popcount(o.mask) == 1) {
            AppendMenuW(menu, MF_STRING, kMenuScreen + __builtin_ctz(o.mask), L"Rename screen");
        } else {
            HMENU sub = CreatePopupMenu();
            for (int t = 0; t < static_cast<int>(targets.size()); ++t)
                if (o.mask & (1u << t))
                    AppendMenuW(sub, MF_STRING, kMenuScreen + t, targets[t].name.c_str());
            AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(sub), L"Rename screen");
        }
        if (__builtin_popcount(o.mask) > 1) {
            HMENU sub = CreatePopupMenu();
            AppendMenuW(sub, MF_STRING | (o.primary < 0 ? MF_CHECKED : 0), kMenuMainAuto,
                        L"Leave as it is");
            for (int t = 0; t < static_cast<int>(targets.size()); ++t)
                if (o.mask & (1u << t))
                    AppendMenuW(sub, MF_STRING | (o.primary == t ? MF_CHECKED : 0), kMenuMain + t,
                                targets[t].name.c_str());
            AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(sub), L"Main display");
        }
        AppendMenuW(menu, MF_STRING | (up ? 0 : MF_GRAYED), kMenuUp, L"Move up");
        AppendMenuW(menu, MF_STRING | (down ? 0 : MF_GRAYED), kMenuDown, L"Move down");
        AppendMenuW(menu, MF_STRING, kMenuHide, o.hidden ? L"Show this row" : L"Hide this row");
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    }
    AppendMenuW(menu, MF_STRING | (showHidden ? MF_CHECKED : 0), kMenuShowHidden,
                L"Show hidden rows");
    AppendMenuW(menu, MF_STRING, kMenuReset, L"Reset all rows");
    int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, at.x, at.y, 0, panel, nullptr);
    DestroyMenu(menu);  // also destroys the submenu

    if (cmd == kMenuShowHidden) { showHidden = !showHidden; Refresh(); return; }
    if (cmd == kMenuReset) {  // rows and screen names
        rows.clear();
        SaveRows();
        Wh_SetStringValue(L"screens", L"");
        Refresh();
        return;
    }
    if (!row || !cmd) return;
    if (cmd == kMenuRename) { BeginRename(i); return; }
    if (cmd >= kMenuScreen) { BeginRename(i, cmd - kMenuScreen); return; }
    Row* r = FindRow(options[i].key);
    if (!r) return;
    if (cmd == kMenuHide) r->hidden = !r->hidden;
    else if (cmd == kMenuMainAuto) r->primary = 0;
    else if (cmd >= kMenuMain) r->primary = targets[cmd - kMenuMain].id;
    else if (cmd == kMenuUp || cmd == kMenuDown) {
        Row* other = FindRow(options[cmd == kMenuUp ? i - 1 : i + 1].key);
        if (!other) return;
        std::swap(*r, *other);
    }
    SaveRows();
    Refresh();
    InvalidateRect(panel, nullptr, FALSE);
}

void Open() {
    LoadTheme();
    EndRename(false);
    status.clear();
    hot = -1;
    sel = -1;
    cycled = false;
    showHidden = false;
    KillTimer(panel, kCommitTimer);
    Refresh();
    if (sel < 0) sel = 0;
    Place();
    Appear();
    ForceForeground(panel);
    SetFocus(panel);
}

void Hide() {
    if (pending) return;
    EndRename(true);
    KillTimer(panel, kCommitTimer);
    cycled = false;
    Dismiss();  // the preview window closes itself once it has slid out
}

void SendNativeWinP() {
    INPUT in[4]{};
    WORD keys[4] = {VK_LWIN, 'P', 'P', VK_LWIN};
    for (int i = 0; i < 4; ++i) {
        in[i].type = INPUT_KEYBOARD;
        in[i].ki.wVk = keys[i];
        if (i >= 2) in[i].ki.dwFlags = KEYEVENTF_KEYUP;
    }
    SendInput(4, in, sizeof(INPUT));
}

void EndPending(bool keep) {
    if (!pending) return;
    KillTimer(panel, kCountdownTimer);
    pending = false;
    if (keep) {
        status.clear();
        Dismiss();
        return;
    }
    LONG e = Restore(before);
    status = e ? L"Could not restore the previous layout (error " + std::to_wstring(e) +
                     L"). Use Display settings."
               : L"Previous layout restored.";
    Refresh();
    Place();
}

void Activate(int i) {
    if (pending) {
        if (i == kKeep) EndPending(true);
        else if (i == kRevert) EndPending(false);
        return;
    }
    if (i < 0 || i >= static_cast<int>(options.size())) return;
    KillTimer(panel, kCommitTimer);
    cycled = false;
    const Option& o = options[i];
    if (o.kind == kSettings) {
        Hide();
        ShellExecuteW(nullptr, L"open", L"ms-settings:display", nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }
    if (o.kind == kNative) {
        Hide();
        SendNativeWinP();  // injected input is not intercepted by our hook
        return;
    }
    if (IsCurrent(o)) { Hide(); return; }
    if (preview) {
        status = L"Preview mode: would switch to \"" + o.label + L"\".";
        Place();
        return;
    }
    if (LONG e = Query(QDC_ONLY_ACTIVE_PATHS, before)) {
        status = L"Could not read the current layout (error " + std::to_wstring(e) + L").";
        Place();
        return;
    }
    LONG e = ApplyLayout(targets, o.mask, o.primary);
    if (e) {
        status = L"Windows refused this combination (error " + std::to_wstring(e) +
                 L"). The graphics card may not support it, or a screen is switched off.";
        Refresh();
        Place();
        return;
    }
    status.clear();
    if (!Wh_GetIntSetting(L"askToKeep")) { Refresh(); Dismiss(); return; }
    pending = true;
    deadline = GetTickCount64() + kConfirmMs;
    SetTimer(panel, kCountdownTimer, 250, nullptr);
    Refresh();
    Place();
    Appear();
    ForceForeground(panel);
}

// ---------------------------------------------------------------- painting

void Fill(HDC dc, RECT r, COLORREF c, int radius = 0) {
    HBRUSH b = CreateSolidBrush(c);
    if (radius) {
        HPEN pen = CreatePen(PS_SOLID, 1, c);
        HGDIOBJ ob = SelectObject(dc, b), op = SelectObject(dc, pen);
        RoundRect(dc, r.left, r.top, r.right, r.bottom, radius, radius);
        SelectObject(dc, ob); SelectObject(dc, op);
        DeleteObject(pen);
    } else {
        FillRect(dc, &r, b);
    }
    DeleteObject(b);
}

void Outline(HDC dc, RECT r, COLORREF c) {
    HPEN pen = CreatePen(PS_SOLID, std::max(1, Px(1)), c);
    HGDIOBJ op = SelectObject(dc, pen), ob = SelectObject(dc, GetStockObject(NULL_BRUSH));
    Rectangle(dc, r.left, r.top, r.right, r.bottom);
    SelectObject(dc, op); SelectObject(dc, ob);
    DeleteObject(pen);
}

void Text(HDC dc, const std::wstring& s, RECT r, HFONT f, COLORREF c, UINT flags) {
    SelectObject(dc, f);
    SetTextColor(dc, c);
    DrawTextW(dc, s.c_str(), -1, &r, flags | DT_NOPREFIX);
}

void Paint(HDC dc, RECT client) {
    COLORREF bg = dark ? RGB(32, 32, 32) : RGB(243, 243, 243);
    COLORREF hover = dark ? RGB(45, 45, 45) : RGB(232, 232, 232);
    COLORREF selc = dark ? RGB(56, 56, 56) : RGB(222, 222, 222);
    COLORREF fg = dark ? RGB(255, 255, 255) : RGB(0, 0, 0);
    COLORREF sub = dark ? RGB(170, 170, 170) : RGB(96, 96, 96);
    COLORREF off = dark ? RGB(110, 110, 110) : RGB(150, 150, 150);
    Fill(dc, client, bg);
    SetBkMode(dc, TRANSPARENT);

    RECT title{Px(20), 0, client.right - Px(20), Px(kHeader)};
    Text(dc, L"Project", title, fTitle, fg, DT_SINGLELINE | DT_VCENTER);
    if (!pending)
        Text(dc, showHidden ? L"Showing hidden rows" : L"Right-click to edit", title, fSub, sub,
             DT_SINGLELINE | DT_VCENTER | DT_RIGHT);

    int n = static_cast<int>(targets.size());
    for (int i = 0; i < static_cast<int>(options.size()); ++i) {
        const Option& o = options[i];
        RECT r = RowRect(i);
        if (i == sel) Fill(dc, r, selc, Px(8));
        else if (i == hot && !pending) Fill(dc, r, hover, Px(8));
        if (IsCurrent(o)) {
            RECT bar{r.left + Px(2), r.top + Px(16), r.left + Px(5), r.bottom - Px(16)};
            Fill(dc, bar, accent, Px(3));
        }
        int x = r.left + Px(14);
        int cy = (r.top + r.bottom) / 2;
        int iconW = IconWidth();
        if (o.kind == kCombo) {
            for (int t = 0; t < n; ++t) {
                RECT box{x + t * Px(22), cy - Px(6), x + t * Px(22) + Px(18), cy + Px(6)};
                if (o.mask & (1u << t)) Fill(dc, box, o.hidden ? off : accent);
                else Outline(dc, box, off);
            }
        } else {
            RECT ir{x, r.top, x + iconW, r.bottom};
            Text(dc, o.kind == kSettings ? L"" : L"", ir, fIcon, fg,
                 DT_SINGLELINE | DT_VCENTER | DT_LEFT);
        }
        COLORREF lc = pending || o.hidden ? sub : fg;
        RECT lr{x + iconW + Px(14), r.top + Px(8), r.right - Px(10), cy + Px(2)};
        Text(dc, o.label, lr, fLabel, lc, DT_SINGLELINE | DT_BOTTOM | DT_END_ELLIPSIS);
        RECT sr{lr.left, cy + Px(2), lr.right, r.bottom - Px(6)};
        std::wstring s = o.sub;
        if (IsCurrent(o)) s = L"Current  ·  " + s;
        Text(dc, s, sr, fSub, sub, DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS);
    }

    if (!HasFooter()) return;
    int top = FooterTop();
    RECT line{Px(kPad), top, client.right - Px(kPad), top + Px(1)};
    Fill(dc, line, dark ? RGB(60, 60, 60) : RGB(210, 210, 210));
    RECT msg{Px(20), top + Px(6), client.right - Px(20), top + Px(42)};
    if (pending) {
        ULONGLONG now = GetTickCount64();
        int left = now >= deadline ? 0 : static_cast<int>((deadline - now + 999) / 1000);
        Text(dc, L"Keep this layout? Reverting in " + std::to_wstring(left) + L" s", msg, fLabel,
             fg, DT_SINGLELINE | DT_VCENTER);
        RECT k = ButtonRect(0), rv = ButtonRect(1);
        Fill(dc, k, accent, Px(8));
        Text(dc, L"Keep", k, fLabel, RGB(255, 255, 255), DT_SINGLELINE | DT_VCENTER | DT_CENTER);
        Fill(dc, rv, hot == kRevert ? selc : hover, Px(8));
        Text(dc, L"Revert", rv, fLabel, fg, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
    } else {
        msg.bottom = top + Px(kFooter) - Px(8);
        Text(dc, status, msg, fSub, sub, DT_WORDBREAK);
    }
}

// ---------------------------------------------------------------- window

LRESULT CALLBACK Proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case kShow:
        EndRename(false);
        if (Shown() && !pending && !options.empty()) {
            // Win+P again: move to the next layout (settings and hidden rows
            // are skipped), then apply it once the user stops tapping.
            int count = static_cast<int>(options.size());
            for (int step = 1; step <= count; ++step) {
                int next = (sel + step) % count;
                if (options[next].kind == kCombo && !options[next].hidden) { sel = next; break; }
            }
            cycled = true;
            SetTimer(hwnd, kCommitTimer, ApplyDelay(), nullptr);
            InvalidateRect(hwnd, nullptr, FALSE);
        } else if (!pending) {
            Open();
        }
        return 0;
    case kWinUp:
        // Letting go of Win after cycling applies a little sooner.
        if (cycled && Shown() && !pending)
            SetTimer(hwnd, kCommitTimer, std::min(ApplyDelay(), 400), nullptr);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        RECT c; GetClientRect(hwnd, &c);
        HDC mem = CreateCompatibleDC(dc);
        HBITMAP bmp = CreateCompatibleBitmap(dc, c.right, c.bottom);
        HGDIOBJ old = SelectObject(mem, bmp);
        Paint(mem, c);
        BitBlt(dc, 0, 0, c.right, c.bottom, mem, 0, 0, SRCCOPY);
        SelectObject(mem, old);
        DeleteObject(bmp);
        DeleteDC(mem);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND: return 1;
    case WM_MOUSEMOVE: {
        int h = HitTest({GET_X_LPARAM(lp), GET_Y_LPARAM(lp)});
        if (h != hot) { hot = h; InvalidateRect(hwnd, nullptr, FALSE); }
        TRACKMOUSEEVENT t{sizeof(t), TME_LEAVE, hwnd, 0};
        TrackMouseEvent(&t);
        return 0;
    }
    case WM_MOUSELEAVE: hot = -1; InvalidateRect(hwnd, nullptr, FALSE); return 0;
    case WM_LBUTTONUP: Activate(HitTest({GET_X_LPARAM(lp), GET_Y_LPARAM(lp)})); return 0;
    case WM_CONTEXTMENU: {  // right-click, Menu key or Shift+F10
        if (pending || animDir < 0) return 0;
        POINT at{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
        int i = sel;
        if (lp == -1) {
            RECT r = RowRect(std::max(sel, 0));
            at = {r.left + Px(40), r.bottom};
            ClientToScreen(hwnd, &at);
        } else {
            POINT c = at;
            ScreenToClient(hwnd, &c);
            i = HitTest(c);
        }
        ShowMenu(i, at);
        return 0;
    }
    case kRenameDone: EndRename(wp != 0); return 0;
    case WM_CTLCOLOREDIT:
        SetTextColor(reinterpret_cast<HDC>(wp), dark ? RGB(255, 255, 255) : RGB(0, 0, 0));
        SetBkColor(reinterpret_cast<HDC>(wp), dark ? RGB(56, 56, 56) : RGB(222, 222, 222));
        return reinterpret_cast<LRESULT>(editBrush);
    case WM_KEYDOWN: {
        int count = static_cast<int>(options.size());
        KillTimer(hwnd, kCommitTimer);  // choosing by hand: no auto-apply
        cycled = false;
        if (pending) {
            if (wp == VK_RETURN) EndPending(true);
            else if (wp == VK_ESCAPE) EndPending(false);
            return 0;
        }
        if (wp == VK_ESCAPE) Hide();
        else if (wp == VK_DOWN || wp == VK_TAB) sel = (sel + 1) % count;
        else if (wp == VK_UP) sel = (sel + count - 1) % count;
        else if (wp == VK_HOME) sel = 0;
        else if (wp == VK_END) sel = count - 1;
        else if (wp == VK_RETURN || wp == VK_SPACE) Activate(sel);
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    case WM_ACTIVATE:
        if (LOWORD(wp) == WA_INACTIVE && !pending && !preview) Hide();
        return 0;
    case WM_TIMER:
        if (wp == kAnimTimer) { AnimTick(); return 0; }
        if (wp == kCommitTimer) {
            KillTimer(hwnd, kCommitTimer);
            if (cycled && !pending) Activate(sel);
            return 0;
        }
        if (pending && GetTickCount64() >= deadline) EndPending(false);
        else InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    case WM_DISPLAYCHANGE:
        if (IsWindowVisible(hwnd)) Refresh();
        return 0;
    case WM_DPICHANGED:
        if (HIWORD(wp) != dpi && IsWindowVisible(hwnd)) Place();
        return 0;
    case WM_SETTINGCHANGE:
        LoadTheme();
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    case WM_CLOSE:
        if (pending) EndPending(false);
        if (preview && !IsWindowVisible(hwnd)) { DestroyWindow(hwnd); return 0; }
        Dismiss();
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------- Win+P hook

// Pure state machine: tested without sending keys to the desktop.
struct Keys {
    bool down[256]{};
    bool consumedP{};
    bool armed{};    // flyout opened with Win+P, waiting for Win release
    bool winUp{};    // set by Event(): Win was just released after Win+P
    bool Event(DWORD vk, bool pressed, bool injected, bool& open) {
        open = false;
        winUp = false;
        if (injected || vk >= 256) return false;
        bool repeat = down[vk] && pressed;
        down[vk] = pressed;
        if ((vk == VK_LWIN || vk == VK_RWIN) && !pressed && armed &&
            !down[VK_LWIN] && !down[VK_RWIN]) {
            armed = false;
            winUp = true;  // not swallowed: Windows must see the release
            return false;
        }
        if (vk != 'P') return false;
        if (consumedP && !pressed) { consumedP = false; return true; }
        bool win = down[VK_LWIN] || down[VK_RWIN];
        bool mods = down[VK_LSHIFT] || down[VK_RSHIFT] || down[VK_SHIFT] || down[VK_LCONTROL] ||
                    down[VK_RCONTROL] || down[VK_CONTROL] || down[VK_LMENU] || down[VK_RMENU] ||
                    down[VK_MENU];
        if (pressed && win && !mods) {
            consumedP = true;
            armed = true;
            open = !repeat;  // auto-repeat is swallowed but does not cycle
            return true;
        }
        return false;
    }
} keys;

LRESULT CALLBACK Keyboard(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && hookPanel.load()) {
        auto* k = reinterpret_cast<KBDLLHOOKSTRUCT*>(lp);
        bool open{};
        bool pressed = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
        if (keys.Event(k->vkCode, pressed, (k->flags & LLKHF_INJECTED) != 0, open)) {
            if (open) {
                // Without this, releasing Win alone would open Start.
                INPUT mask[2]{};
                for (auto& in : mask) { in.type = INPUT_KEYBOARD; in.ki.wVk = 0xE8; }
                mask[1].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(2, mask, sizeof(INPUT));
                PostMessageW(hookPanel.load(), kShow, 0, 0);
            }
            return 1;
        }
        if (keys.winUp) PostMessageW(hookPanel.load(), kWinUp, 0, 0);
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

DWORD WINAPI HookLoop(void*) {
    MSG msg{};
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
    for (int k = 0; k < 256; ++k) keys.down[k] = (GetAsyncKeyState(k) & 0x8000) != 0;
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, Keyboard, instance, 0);
    SetEvent(readyEvent);
    if (!hook) { Wh_Log(L"SetWindowsHookEx failed: %lu", GetLastError()); return 1; }
    Wh_Log(L"Win+P hook installed");
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    UnhookWindowsHookEx(hook);
    return 0;
}

DWORD WINAPI UiLoop(void*) {
    if (!preview) {
        // Only the Explorer instance that owns the desktop shell shows the flyout.
        DWORD owner{};
        for (int retry = 0; retry < 300; ++retry) {
            HWND shell = GetShellWindow();
            if (shell) GetWindowThreadProcessId(shell, &owner);
            if (owner) break;
            if (WaitForSingleObject(stopEvent, 100) == WAIT_OBJECT_0) return 0;
        }
        if (owner != GetCurrentProcessId()) {
            Wh_Log(L"Not the shell Explorer process, idle");
            return 0;
        }
    }
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    WNDCLASSW wc{};
    wc.style = CS_DROPSHADOW;
    wc.lpfnWndProc = Proc;
    wc.hInstance = instance;
    wc.lpszClassName = kClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&wc)) { Wh_Log(L"RegisterClass failed: %lu", GetLastError()); return 1; }
    panel = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kClass, L"Project", WS_POPUP | WS_CLIPCHILDREN, 0, 0,
                            kWidth, 400, nullptr, nullptr, instance, nullptr);
    if (!panel) { UnregisterClassW(kClass, instance); return 1; }
    DWORD corner = 2;  // DWMWCP_ROUND
    DwmSetWindowAttribute(panel, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &corner, sizeof(corner));
    dpi = static_cast<int>(GetDpiForWindow(panel));
    MakeFonts();
    LoadTheme();
    if (!preview) {
        hookPanel.store(panel);
        hookThread = CreateThread(nullptr, 0, HookLoop, nullptr, 0, &hookId);
    } else {
        Open();
    }
    MSG msg{};
    for (;;) {
        DWORD wait = MsgWaitForMultipleObjects(1, &stopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) break;
        bool quit = false;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { quit = true; break; }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (quit) break;
    }
    if (hookThread) {
        WaitForSingleObject(readyEvent, INFINITE);
        PostThreadMessageW(hookId, WM_QUIT, 0, 0);
        WaitForSingleObject(hookThread, INFINITE);
        CloseHandle(hookThread);
        hookThread = nullptr;
    }
    hookPanel.store(nullptr);
    if (pending) { KillTimer(panel, 1); pending = false; Restore(before); }
    if (IsWindow(panel)) DestroyWindow(panel);
    panel = nullptr;
    for (HFONT* f : {&fTitle, &fLabel, &fSub, &fIcon}) if (*f) { DeleteObject(*f); *f = nullptr; }
    UnregisterClassW(kClass, instance);
    return 0;
}

bool Start(bool isPreview) {
    preview = isPreview;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&Proc), &instance);
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent || !readyEvent) return false;
    uiThread = CreateThread(nullptr, 0, UiLoop, nullptr, 0, &uiId);
    return uiThread != nullptr;
}

void Stop() {
    if (stopEvent) SetEvent(stopEvent);
    if (uiThread) { WaitForSingleObject(uiThread, INFINITE); CloseHandle(uiThread); uiThread = nullptr; }
    if (stopEvent) { CloseHandle(stopEvent); stopEvent = nullptr; }
    if (readyEvent) { CloseHandle(readyEvent); readyEvent = nullptr; }
}

}  // namespace duo

#ifndef DUO_STANDALONE
BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    if (duo::Start(false)) return TRUE;
    duo::Stop();
    return FALSE;
}
void Wh_ModUninit() { duo::Stop(); }
#else
int wmain(int argc, wchar_t** argv) {
    using namespace duo;
    if (argc > 1 && !wcscmp(argv[1], L"--self-test")) {
        int failures = 0; Keys k; bool show{};
        auto check = [&](bool b) { if (!b) ++failures; };
        check(!k.Event('P', true, false, show)); k.Event('P', false, false, show);  // plain P
        k.Event(VK_LWIN, true, false, show);
        check(k.Event('P', true, false, show) && show);    // Win+P opens
        check(k.Event('P', true, false, show) && !show);   // auto-repeat swallowed
        check(k.Event('P', false, false, show) && !show);  // P up swallowed
        check(k.Event('P', true, false, show) && show);    // P again cycles
        check(!k.Event(VK_LWIN, false, false, show) && k.winUp);  // Win up passes + signals
        k.Event(VK_LWIN, true, false, show);
        check(!k.Event(VK_LWIN, false, false, show) && !k.winUp); // plain Win tap: no signal
        k.Event(VK_LWIN, true, false, show); k.Event('P', true, false, show);
        k.Event(VK_LWIN, false, false, show);
        check(k.Event('P', false, false, show) && !show);  // P up after Win up swallowed
        k.Event(VK_RWIN, true, false, show); k.Event(VK_LSHIFT, true, false, show);
        check(!k.Event('P', true, false, show)); k.Event('P', false, false, show);  // Win+Shift+P
        k.Event(VK_LSHIFT, false, false, show);
        check(!k.Event('P', true, true, show));  // injected passes
        printf("Key filter self-test: %s (%d failures)\n", failures ? "FAIL" : "PASS", failures);
        return failures ? 1 : 0;
    }
    if (argc > 1 && !wcscmp(argv[1], L"--list")) {
        showHidden = true;
        BuildOptions();
        for (auto& t : targets)
            wprintf(L"screen id %u  %ls  [model: %ls]%ls%ls%ls\n", t.id, t.name.c_str(),
                    t.monitor.empty() ? L"none" : t.monitor.c_str(),
                    t.internal ? L"  [built-in]" : L"", t.active ? L"  [on]" : L"",
                    t.primary ? L"  [main]" : L"");
        for (auto& o : options)
            wprintf(L"  %ls %ls  (%ls)\n", IsCurrent(o) ? L"*" : L" ",
                    o.label.c_str(), o.sub.c_str());
        return 0;
    }
    if (argc > 1 && !wcscmp(argv[1], L"--preview")) {
        if (!Start(true)) { Stop(); return 1; }
        WaitForSingleObject(uiThread, 120000);
        Stop();
        return 0;
    }
    puts("Use --self-test, --list (read-only) or --preview (no switching).");
    return 0;
}
#endif
