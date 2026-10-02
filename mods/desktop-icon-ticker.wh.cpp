// ==WindhawkMod==
// @id              desktop-icon-ticker
// @name            Desktop Icon Ticker
// @description     Show live scrolling or static text (time, date, CPU, RAM, disk, battery, Recycle Bin...) on top of desktop icons
// @version         0.5
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32 -lshell32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icon Ticker
![Desktop Icon Ticker overview](https://i.imgur.com/54urXw9.png)

Draws live text on top of desktop icons: a scrolling ticker inside the
"This PC" monitor, a badge on the Recycle Bin, free space on a drive
shortcut, and so on.

![Desktop Icon Ticker](https://i.imgur.com/xrokfYJ.png)
[Watch the overview video in full quality](https://i.imgur.com/vqLcdSN.mp4)

Each entry in **Icons** targets one desktop icon. Pick a system icon
(This PC, Recycle Bin, User's Files, Network, Control Panel) and it is
found automatically in any Windows language, even if it was renamed.
For any other icon (shortcuts, files, folders) choose "Custom label"
and type the label exactly as shown on the desktop.

## Placeholders

| Placeholder   | Shows                                   |
|---------------|-----------------------------------------|
| `{time}`      | Time (hh:mm, system format)             |
| `{timesec}`   | Time with seconds                       |
| `{date}`      | Short date                              |
| `{datelong}`  | Long date                               |
| `{day}`       | Day of the week                         |
| `{cpu}`       | CPU usage %                             |
| `{ram}`       | RAM usage %                             |
| `{ramfree}`   | Free RAM                                |
| `{disk:C}`    | Free space on drive C (any letter)      |
| `{battery}`   | Battery % ("+" when charging, "AC" if none) |
| `{uptime}`    | Time since boot                         |
| `{bin}`       | Number of items in the Recycle Bin      |
| `{binsize}`   | Size of the Recycle Bin                 |

Example: `{time} • CPU {cpu} • C: {disk:C} free`

## Tips

- "Text area" decides where the text goes inside the icon. "Monitor
  screen" fits the Windows 11 This PC icon; use "Custom" plus the
  offsets for anything else.
- "Hide when text equals" lets you hide a badge, e.g. `0` for an empty
  Recycle Bin.
- The text follows the icon if you move it, no need to lock positions.
- Windows 11 hides the This PC icon by default. Enable it in Settings >
  Personalization > Themes > Desktop icon settings.
- Animation pauses automatically while a maximized or full-screen window
  covers the monitor the icon is on, to save CPU and battery.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- frameInterval: 30
  $name: Animation frame interval (ms)
  $description: Lower is smoother but uses more CPU
- refreshInterval: 1000
  $name: Info refresh interval (ms)
  $description: How often time, CPU, RAM etc. are updated
- rules:
  - - target: thispc
      $name: Icon
      $options:
      - thispc: This PC
      - recyclebin: Recycle Bin
      - userfiles: User's Files
      - network: Network
      - controlpanel: Control Panel
      - custom: Custom label
    - itemName: ""
      $name: Custom label
      $description: Only used when Icon is "Custom label". Exact label shown on the desktop
    - text: "{time} • {day} {date} • CPU {cpu} • RAM {ram} • C: {disk:C} free"
      $name: Text
      $description: Supports placeholders, see the mod description
    - mode: scroll
      $name: Mode
      $options:
      - scroll: Scrolling
      - static: Static (centered)
    - speed: 1
      $name: Scroll speed (px per frame)
    - area: monitor
      $name: Text area
      $options:
      - monitor: Monitor screen (This PC icon)
      - full: Whole icon
      - top: Top strip
      - bottom: Bottom strip
      - custom: Custom
    - color: "FFFFFF"
      $name: Text color (RRGGBB)
    - bgColor: ""
      $name: Background color (RRGGBB, empty = none)
    - fontSize: 8
      $name: Font size (pt)
    - bold: true
      $name: Bold
    - hideWhen: ""
      $name: Hide when text equals
      $description: For example "0" to hide a Recycle Bin counter when it's empty
    - offsetX: 0
      $name: Offset X (px)
    - offsetY: 0
      $name: Offset Y (px)
    - customLeft: 0
      $name: Custom area - left (%)
    - customTop: 0
      $name: Custom area - top (%)
    - customWidth: 100
      $name: Custom area - width (%)
    - customHeight: 100
      $name: Custom area - height (%)
  - - target: recyclebin
    - itemName: ""
    - text: "{bin}"
    - mode: static
    - speed: 1
    - area: bottom
    - color: "FFFFFF"
    - bgColor: "D13438"
    - fontSize: 7
    - bold: true
    - hideWhen: "0"
    - offsetX: 0
    - offsetY: 0
    - customLeft: 0
    - customTop: 0
    - customWidth: 100
    - customHeight: 100
  $name: Icons
  $description: One entry per desktop icon
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <atomic>
#include <cwchar>
#include <cwctype>
#include <map>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------
// Settings and state
//
// Threads:
//   - desktop thread (explorer): custom draw, paints the text
//   - animation thread: invalidates scrolling text
//   - info thread: resolves icon names, expands placeholders (may block on
//     slow drives, so it is kept away from the animation)
// The two worker threads are only started in the explorer.exe process that
// owns the desktop, when its list view is found (CreateWindowExW hook or
// Wh_ModAfterInit). Other explorer.exe processes only carry the hook.
// Everything in g_rules / g_states / g_generation is guarded by g_lock.
// ---------------------------------------------------------------------------

struct Rule {
    std::wstring itemName;   // custom label typed by the user
    std::wstring clsid;      // system icon id, empty for custom labels
    std::wstring matchName;  // label actually matched (resolved at runtime)
    std::wstring tmpl;
    std::wstring hideWhen;
    COLORREF color = RGB(255, 255, 255);
    bool hasBg = false;
    COLORREF bg = 0;
    int fontSize = 8;
    bool bold = true;
    bool scroll = true;
    int speed = 1;
    int l = 0, t = 0, w = 100, h = 100;
    int offX = 0, offY = 0;
};

struct RuleState {
    std::wstring text;
    RECT lastRect{};      // area to invalidate (text area + icon), client coords
    bool hasRect = false;
    int missed = 0;       // invalidations since the last actual paint
    HFONT font = nullptr;
    UINT fontDpi = 0;
};

std::vector<Rule> g_rules;
std::vector<RuleState> g_states;
unsigned g_generation = 0;
int g_frameInterval = 30;
int g_refreshInterval = 1000;
SRWLOCK g_lock = SRWLOCK_INIT;

std::atomic<HWND> g_defView{nullptr};
std::atomic<HWND> g_listView{nullptr};
std::atomic<unsigned> g_tick{0};
std::atomic<bool> g_needFullRedraw{true};

std::atomic<bool> g_animIdle{false};      // animation thread sleeps indefinitely
std::atomic<unsigned> g_paintCount{0};    // incremented on every rule paint
std::atomic<bool> g_unloading{false};

HANDLE g_stopEvent = nullptr;
HANDLE g_infoWake = nullptr;   // auto-reset
HANDLE g_animWake = nullptr;   // auto-reset
HANDLE g_animThread = nullptr;
HANDLE g_infoThread = nullptr;
SRWLOCK g_startLock = SRWLOCK_INIT;  // guards thread creation vs. unload
bool g_threadsStarted = false;

static void WakeAnim() {
    if (g_animWake) SetEvent(g_animWake);
}

static int Clamp(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// Accepts exactly "RRGGBB" or "#RRGGBB" (spaces around are ignored).
static bool ParseColor(const std::wstring& str, COLORREF* out) {
    size_t a = str.find_first_not_of(L" \t");
    if (a == std::wstring::npos) return false;
    size_t b = str.find_last_not_of(L" \t");
    std::wstring s = str.substr(a, b - a + 1);
    if (!s.empty() && s[0] == L'#') s.erase(0, 1);
    if (s.size() != 6) return false;
    for (wchar_t c : s) {
        if (!iswxdigit(c)) return false;
    }
    unsigned long v = wcstoul(s.c_str(), nullptr, 16);
    *out = RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
    return true;
}

static std::wstring GetStr(PCWSTR name, int i) {
    PCWSTR v = Wh_GetStringSetting(name, i);
    std::wstring r = v ? v : L"";
    Wh_FreeStringSetting(v);
    return r;
}

static void FreeFontsLocked() {
    for (auto& st : g_states) {
        if (st.font) {
            DeleteObject(st.font);
            st.font = nullptr;
        }
    }
}

static void LoadSettings() {
    std::vector<Rule> rules;

    for (int i = 0; i < 32; i++) {
        std::wstring target = GetStr(L"rules[%d].target", i);
        std::wstring name = GetStr(L"rules[%d].itemName", i);
        if (target.empty() && name.empty()) break;

        Rule r;
        r.itemName = name;
        if (target == L"thispc") r.clsid = L"{20D04FE0-3AEA-1069-A2D8-08002B30309D}";
        else if (target == L"recyclebin") r.clsid = L"{645FF040-5081-101B-9F08-00AA002F954E}";
        else if (target == L"userfiles") r.clsid = L"{59031a47-3f72-44a7-89c5-5595fe6b30ee}";
        else if (target == L"network") r.clsid = L"{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}";
        else if (target == L"controlpanel") r.clsid = L"{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}";

        if (r.clsid.empty()) {
            if (name.empty()) {
                Wh_Log(L"Rule %d: custom label is empty, skipped", i);
                continue;
            }
            r.matchName = name;
        }

        r.tmpl = GetStr(L"rules[%d].text", i);
        r.hideWhen = GetStr(L"rules[%d].hideWhen", i);

        COLORREF c;
        std::wstring colorStr = GetStr(L"rules[%d].color", i);
        if (ParseColor(colorStr, &c)) {
            r.color = c;
        } else if (!colorStr.empty()) {
            Wh_Log(L"Rule %d: invalid color \"%s\", using white", i, colorStr.c_str());
        }
        std::wstring bgStr = GetStr(L"rules[%d].bgColor", i);
        r.hasBg = ParseColor(bgStr, &r.bg);
        if (!r.hasBg && !bgStr.empty()) {
            Wh_Log(L"Rule %d: invalid background \"%s\", ignored", i, bgStr.c_str());
        }

        r.fontSize = Clamp(Wh_GetIntSetting(L"rules[%d].fontSize", i), 4, 72);
        r.bold = Wh_GetIntSetting(L"rules[%d].bold", i) != 0;
        r.scroll = GetStr(L"rules[%d].mode", i) != L"static";
        r.speed = Clamp(Wh_GetIntSetting(L"rules[%d].speed", i), 1, 20);
        r.offX = Clamp(Wh_GetIntSetting(L"rules[%d].offsetX", i), -64, 64);
        r.offY = Clamp(Wh_GetIntSetting(L"rules[%d].offsetY", i), -64, 64);

        std::wstring area = GetStr(L"rules[%d].area", i);
        if (area == L"monitor") {
            r.l = 8; r.t = 12; r.w = 84; r.h = 56;
        } else if (area == L"top") {
            r.l = 0; r.t = 0; r.w = 100; r.h = 32;
        } else if (area == L"bottom") {
            r.l = 0; r.t = 68; r.w = 100; r.h = 32;
        } else if (area == L"custom") {
            r.l = Clamp(Wh_GetIntSetting(L"rules[%d].customLeft", i), -50, 150);
            r.t = Clamp(Wh_GetIntSetting(L"rules[%d].customTop", i), -50, 150);
            r.w = Clamp(Wh_GetIntSetting(L"rules[%d].customWidth", i), 1, 200);
            r.h = Clamp(Wh_GetIntSetting(L"rules[%d].customHeight", i), 1, 200);
        } else {
            r.l = 0; r.t = 0; r.w = 100; r.h = 100;
        }

        rules.push_back(std::move(r));
    }

    int frame = Clamp(Wh_GetIntSetting(L"frameInterval"), 10, 500);
    int refresh = Clamp(Wh_GetIntSetting(L"refreshInterval"), 250, 60000);
    int count = (int)rules.size();

    AcquireSRWLockExclusive(&g_lock);
    FreeFontsLocked();
    g_rules = std::move(rules);
    g_states.assign(g_rules.size(), RuleState{});
    g_frameInterval = frame;
    g_refreshInterval = refresh;
    g_generation++;
    ReleaseSRWLockExclusive(&g_lock);

    g_needFullRedraw = true;
    if (g_infoWake) SetEvent(g_infoWake);
    WakeAnim();
    Wh_Log(L"Loaded %d rule(s)", count);
}

// ---------------------------------------------------------------------------
// System info and placeholders (info thread only)
// ---------------------------------------------------------------------------

static ULONGLONG FtToU64(const FILETIME& ft) {
    return ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}

static int g_cpu = 0;
static ULONGLONG g_prevIdle = 0, g_prevTotal = 0;
static std::map<wchar_t, std::pair<ULONGLONG, std::wstring>> g_diskCache;
static ULONGLONG g_binTime = 0;
static long long g_binCount = 0, g_binSize = 0;

static void UpdateCpu() {
    FILETIME idle, kernel, user;
    if (!GetSystemTimes(&idle, &kernel, &user)) return;
    ULONGLONG i = FtToU64(idle);
    ULONGLONG total = FtToU64(kernel) + FtToU64(user);
    if (g_prevTotal && total > g_prevTotal && i >= g_prevIdle) {
        ULONGLONG dT = total - g_prevTotal;
        ULONGLONG dI = i - g_prevIdle;
        g_cpu = dI >= dT ? 0 : Clamp((int)(100 * (dT - dI) / dT), 0, 100);
    }
    g_prevIdle = i;
    g_prevTotal = total;
}

static std::wstring FormatBytes(ULONGLONG b) {
    wchar_t buf[64];
    double gb = b / 1073741824.0;
    if (gb >= 100) swprintf(buf, 64, L"%.0f GB", gb);
    else if (gb >= 1) swprintf(buf, 64, L"%.1f GB", gb);
    else swprintf(buf, 64, L"%.0f MB", b / 1048576.0);
    return buf;
}

static void UpdateBin(ULONGLONG now) {
    if (g_binTime && now - g_binTime < 3000) return;
    g_binTime = now;
    SHQUERYRBINFO qi{};
    qi.cbSize = sizeof(qi);
    if (SUCCEEDED(SHQueryRecycleBinW(nullptr, &qi))) {
        g_binCount = qi.i64NumItems;
        g_binSize = qi.i64Size;
    }
}

static std::wstring DiskFree(wchar_t letter, ULONGLONG now) {
    letter = (wchar_t)towupper(letter);
    if (letter < L'A' || letter > L'Z') return L"?";
    auto it = g_diskCache.find(letter);
    if (it != g_diskCache.end() && now - it->second.first < 10000) {
        return it->second.second;
    }
    wchar_t root[] = {letter, L':', L'\\', 0};
    ULARGE_INTEGER freeAvail{};
    std::wstring v = L"?";
    if (GetDiskFreeSpaceExW(root, &freeAvail, nullptr, nullptr)) {
        v = FormatBytes(freeAvail.QuadPart);
    }
    g_diskCache[letter] = {now, v};
    return v;
}

static bool Resolve(const std::wstring& tok, const std::wstring& arg,
                    ULONGLONG now, std::wstring* out) {
    wchar_t buf[128];

    if (tok == L"time" || tok == L"timesec") {
        DWORD flags = tok == L"time" ? TIME_NOSECONDS : 0;
        if (GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, flags, nullptr, nullptr, buf, 128)) {
            *out = buf;
        }
        return true;
    }
    if (tok == L"date" || tok == L"datelong" || tok == L"day") {
        DWORD flags = tok == L"date" ? DATE_SHORTDATE
                    : tok == L"datelong" ? DATE_LONGDATE : 0;
        PCWSTR fmt = tok == L"day" ? L"dddd" : nullptr;
        if (GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, flags, nullptr, fmt, buf, 128, nullptr)) {
            *out = buf;
        }
        return true;
    }
    if (tok == L"cpu") {
        swprintf(buf, 128, L"%d%%", g_cpu);
        *out = buf;
        return true;
    }
    if (tok == L"ram" || tok == L"ramfree") {
        MEMORYSTATUSEX ms{};
        ms.dwLength = sizeof(ms);
        if (GlobalMemoryStatusEx(&ms)) {
            if (tok == L"ram") {
                swprintf(buf, 128, L"%u%%", (unsigned)ms.dwMemoryLoad);
                *out = buf;
            } else {
                *out = FormatBytes(ms.ullAvailPhys);
            }
        }
        return true;
    }
    if (tok == L"disk") {
        *out = DiskFree(arg.empty() ? L'C' : arg[0], now);
        return true;
    }
    if (tok == L"battery") {
        SYSTEM_POWER_STATUS ps{};
        if (GetSystemPowerStatus(&ps) && ps.BatteryLifePercent <= 100 &&
            !(ps.BatteryFlag & 128)) {
            swprintf(buf, 128, L"%d%%%s", (int)ps.BatteryLifePercent,
                     (ps.BatteryFlag & 8) ? L"+" : L"");
            *out = buf;
        } else {
            *out = L"AC";
        }
        return true;
    }
    if (tok == L"uptime") {
        unsigned long long m = GetTickCount64() / 60000;
        unsigned long long d = m / 1440, h = (m / 60) % 24, mm = m % 60;
        if (d) swprintf(buf, 128, L"%llud %lluh", d, h);
        else if (h) swprintf(buf, 128, L"%lluh %llum", h, mm);
        else swprintf(buf, 128, L"%llum", mm);
        *out = buf;
        return true;
    }
    if (tok == L"bin" || tok == L"binsize") {
        UpdateBin(now);
        if (tok == L"bin") {
            swprintf(buf, 128, L"%lld", g_binCount);
            *out = buf;
        } else {
            *out = FormatBytes((ULONGLONG)g_binSize);
        }
        return true;
    }
    return false;
}

static std::wstring Expand(const std::wstring& t, ULONGLONG now) {
    std::wstring out;
    size_t i = 0;
    while (i < t.size()) {
        if (t[i] == L'{') {
            size_t j = t.find(L'}', i);
            if (j != std::wstring::npos) {
                std::wstring tok = t.substr(i + 1, j - i - 1);
                std::wstring arg;
                size_t c = tok.find(L':');
                if (c != std::wstring::npos) {
                    arg = tok.substr(c + 1);
                    tok = tok.substr(0, c);
                }
                std::wstring v;
                if (Resolve(tok, arg, now, &v)) {
                    out += v;
                    i = j + 1;
                    continue;
                }
            }
        }
        out += t[i++];
    }
    return out;
}

static std::wstring ResolveShellName(const std::wstring& clsid) {
    std::wstring result;
    std::wstring path = L"::" + clsid;
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr))) {
        PWSTR name = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_NORMALDISPLAY, &name))) {
            result = name;
            CoTaskMemFree(name);
        }
        CoTaskMemFree(pidl);
    }
    return result;
}

// ---------------------------------------------------------------------------
// Drawing (desktop thread)
// ---------------------------------------------------------------------------

static std::wstring GetItemLabel(HWND lv, int index) {
    WCHAR buf[260] = {};
    LVITEMW it{};
    it.iSubItem = 0;
    it.pszText = buf;
    it.cchTextMax = ARRAYSIZE(buf);
    SendMessageW(lv, LVM_GETITEMTEXTW, index, (LPARAM)&it);
    return buf;
}

static int FindRuleLocked(const std::wstring& label) {
    if (label.empty()) return -1;
    for (size_t i = 0; i < g_rules.size(); i++) {
        const std::wstring& m = g_rules[i].matchName;
        if (!m.empty() && _wcsicmp(m.c_str(), label.c_str()) == 0) {
            return (int)i;
        }
    }
    return -1;
}

static bool GetAreaRect(HWND lv, int index, const Rule& r, RECT* area, RECT* icon) {
    RECT rc{};
    rc.left = LVIR_ICON;
    if (!SendMessageW(lv, LVM_GETITEMRECT, index, (LPARAM)&rc)) return false;

    int rw = rc.right - rc.left;
    int rh = rc.bottom - rc.top;
    if (rw <= 0 || rh <= 0) return false;

    int cx = rw < rh ? rw : rh;
    int cy = cx;
    HIMAGELIST il = (HIMAGELIST)SendMessageW(lv, LVM_GETIMAGELIST, LVSIL_NORMAL, 0);
    if (il) ImageList_GetIconSize(il, &cx, &cy);
    // The image list can be bigger than what is actually drawn.
    if (cx > rw) cx = rw;
    if (cy > rh) cy = rh;

    int ix = rc.left + (rw - cx) / 2 + r.offX;
    int iy = rc.top + (rh - cy) / 2 + r.offY;

    area->left = ix + cx * r.l / 100;
    area->top = iy + cy * r.t / 100;
    area->right = area->left + cx * r.w / 100;
    area->bottom = area->top + cy * r.h / 100;
    *icon = rc;
    return area->right > area->left && area->bottom > area->top;
}

static void FillRounded(HDC hdc, const RECT& rc, COLORREF color, int radius) {
    HBRUSH br = CreateSolidBrush(color);
    HGDIOBJ oldB = SelectObject(hdc, br);
    HGDIOBJ oldP = SelectObject(hdc, GetStockObject(NULL_PEN));
    RoundRect(hdc, rc.left, rc.top, rc.right + 1, rc.bottom + 1, radius, radius);
    SelectObject(hdc, oldP);
    SelectObject(hdc, oldB);
    DeleteObject(br);
}

static void DrawRuleLocked(HWND lv, HDC hdc, int index, int ri) {
    const Rule& r = g_rules[ri];
    RuleState& st = g_states[ri];

    RECT area, icon;
    if (!GetAreaRect(lv, index, r, &area, &icon)) return;

    // Invalidate the icon together with the text area: if only a part
    // outside the item were invalidated, the list view would not repaint
    // the item and custom draw would never run again.
    UnionRect(&st.lastRect, &area, &icon);
    st.hasRect = true;
    st.missed = 0;
    g_paintCount++;

    if (st.text.empty() || st.text == r.hideWhen) return;

    UINT dpi = GetDpiForWindow(lv);
    if (!dpi) dpi = 96;
    if (!st.font || st.fontDpi != dpi) {
        if (st.font) DeleteObject(st.font);
        st.font = CreateFontW(-MulDiv(r.fontSize, dpi, 72), 0, 0, 0,
                              r.bold ? FW_BOLD : FW_NORMAL, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                              DEFAULT_PITCH, L"Segoe UI");
        st.fontDpi = dpi;
    }
    if (!st.font) return;

    int saved = SaveDC(hdc);
    IntersectClipRect(hdc, area.left, area.top, area.right, area.bottom);
    SelectObject(hdc, st.font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, r.color);

    SIZE sz{};
    GetTextExtentPoint32W(hdc, st.text.c_str(), (int)st.text.size(), &sz);

    int aw = area.right - area.left;
    int ah = area.bottom - area.top;
    int y = area.top + (ah - sz.cy) / 2;
    int radius = MulDiv(4, dpi, 96);
    int x;

    if (r.scroll) {
        if (r.hasBg) FillRounded(hdc, area, r.bg, radius);
        unsigned period = (unsigned)(sz.cx + aw);
        if (period < 1) period = 1;
        // 64-bit math: tick * speed cannot overflow.
        unsigned long long step = (unsigned long long)g_tick.load() * (unsigned)r.speed;
        x = area.right - (int)(step % period);
    } else {
        // Centered if it fits, otherwise left-aligned so the start is readable.
        x = sz.cx <= aw ? area.left + (aw - sz.cx) / 2 : area.left;
        if (r.hasBg) {
            int pad = MulDiv(3, dpi, 96);
            RECT b{x - pad, y, x + sz.cx + pad, y + sz.cy};
            FillRounded(hdc, b, r.bg, radius);
        }
    }

    TextOutW(hdc, x, y, st.text.c_str(), (int)st.text.size());
    RestoreDC(hdc, saved);
}

LRESULT CALLBACK DefViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_NOTIFY) {
        auto* hdr = (NMHDR*)lParam;
        HWND lv = g_listView;
        if (hdr && lv && hdr->code == NM_CUSTOMDRAW && hdr->hwndFrom == lv) {
            auto* cd = (NMLVCUSTOMDRAW*)lParam;
            DWORD stage = cd->nmcd.dwDrawStage;
            int index = (int)cd->nmcd.dwItemSpec;

            LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            if (stage == CDDS_PREPAINT) {
                res |= CDRF_NOTIFYITEMDRAW;
            } else if (stage == CDDS_ITEMPREPAINT) {
                std::wstring label = GetItemLabel(lv, index);
                AcquireSRWLockShared(&g_lock);
                bool found = FindRuleLocked(label) >= 0;
                ReleaseSRWLockShared(&g_lock);
                if (found) res |= CDRF_NOTIFYPOSTPAINT;
            } else if (stage == CDDS_ITEMPOSTPAINT) {
                std::wstring label = GetItemLabel(lv, index);
                AcquireSRWLockExclusive(&g_lock);
                int ri = FindRuleLocked(label);
                if (ri >= 0) DrawRuleLocked(lv, cd->nmcd.hdc, index, ri);
                ReleaseSRWLockExclusive(&g_lock);
                // Our icon is painting again (shown, re-added, desktop icons
                // turned back on): wake the animation if it went to sleep.
                if (ri >= 0 && g_animIdle.exchange(false)) WakeAnim();
            }
            return res;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Desktop discovery
// ---------------------------------------------------------------------------

DWORD WINAPI AnimThread(LPVOID);
DWORD WINAPI InfoThread(LPVOID);

static bool HasClass(HWND h, PCWSTR cls) {
    WCHAR buf[64] = {};
    return h && GetClassNameW(h, buf, ARRAYSIZE(buf)) && _wcsicmp(buf, cls) == 0;
}

// SysListView32 -> SHELLDLL_DefView -> Progman / WorkerW
static bool IsDesktopListView(HWND lv) {
    if (!HasClass(lv, L"SysListView32")) return false;
    HWND dv = GetParent(lv);
    if (!HasClass(dv, L"SHELLDLL_DefView")) return false;
    HWND top = GetParent(dv);
    return HasClass(top, L"Progman") || HasClass(top, L"WorkerW");
}

// Used once in Wh_ModAfterInit, for a desktop that already exists.
static HWND FindExistingDesktopListView() {
    HWND dv = nullptr;
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (progman) dv = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    if (!dv) {
        HWND w = nullptr;
        while ((w = FindWindowExW(nullptr, w, L"WorkerW", nullptr)) != nullptr) {
            dv = FindWindowExW(w, nullptr, L"SHELLDLL_DefView", nullptr);
            if (dv) break;
        }
    }
    if (!dv) return nullptr;

    // Windhawk loads the mod in every explorer.exe; only the shell process
    // owns the desktop.
    DWORD pid = 0;
    GetWindowThreadProcessId(dv, &pid);
    if (pid != GetCurrentProcessId()) return nullptr;

    HWND lv = FindWindowExW(dv, nullptr, L"SysListView32", nullptr);
    return IsDesktopListView(lv) ? lv : nullptr;
}

static void StartThreads() {
    AcquireSRWLockExclusive(&g_startLock);
    if (!g_unloading && !g_threadsStarted) {
        g_threadsStarted = true;
        g_infoThread = CreateThread(nullptr, 0, InfoThread, nullptr, 0, nullptr);
        g_animThread = CreateThread(nullptr, 0, AnimThread, nullptr, 0, nullptr);
        if (!g_infoThread || !g_animThread) Wh_Log(L"CreateThread failed");
    }
    ReleaseSRWLockExclusive(&g_startLock);
}

// Called from the CreateWindowExW hook (desktop thread) or from
// Wh_ModAfterInit (Windhawk thread). No lock is held across the subclass
// call; subclassing the same window twice with the same proc and id only
// updates it, so a race between the two callers is harmless.
static void OnDesktopListView(HWND lv) {
    if (g_unloading) return;

    HWND dv = GetParent(lv);
    if (g_defView.load() != dv) {
        if (!WindhawkUtils::SetWindowSubclassFromAnyThread(dv, DefViewSubclassProc, 0)) {
            Wh_Log(L"Subclass failed");
            return;
        }
        g_defView = dv;
    }
    g_listView = lv;

    // Rects from a previous list view are meaningless now.
    AcquireSRWLockExclusive(&g_lock);
    for (auto& st : g_states) st.hasRect = false;
    ReleaseSRWLockExclusive(&g_lock);

    g_needFullRedraw = true;
    StartThreads();
    if (g_infoWake) SetEvent(g_infoWake);
    WakeAnim();
    Wh_Log(L"Desktop hooked");
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName, DWORD dwStyle, int X,
                                 int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu,
                                 HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    // Cheap filter first: this hook sees every window explorer creates.
    if (hWnd && hWndParent && lpClassName && !IS_INTRESOURCE(lpClassName) &&
        _wcsicmp(lpClassName, L"SysListView32") == 0 &&
        IsDesktopListView(hWnd)) {
        OnDesktopListView(hWnd);
    }
    return hWnd;
}

// Monitor whose work area is fully covered by the foreground window
// (maximized or full screen), or nullptr.
static HMONITOR GetCoveredMonitor() {
    HWND fg = GetForegroundWindow();
    if (!fg || IsIconic(fg) || !IsWindowVisible(fg)) return nullptr;

    WCHAR cls[64] = {};
    GetClassNameW(fg, cls, ARRAYSIZE(cls));
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") ||
        !wcscmp(cls, L"Shell_TrayWnd") || !wcscmp(cls, L"Shell_SecondaryTrayWnd")) {
        return nullptr;
    }

    RECT wr;
    if (!GetWindowRect(fg, &wr)) return nullptr;
    HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONULL);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!mon || !GetMonitorInfoW(mon, &mi)) return nullptr;

    const RECT& w = mi.rcWork;
    if (wr.left <= w.left && wr.top <= w.top && wr.right >= w.right && wr.bottom >= w.bottom) {
        return mon;
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Threads
// ---------------------------------------------------------------------------

// Wait policy:
//   - something is actually scrolling           -> frame interval
//   - scrolling is paused because the icon's monitor is covered, or the
//     item stopped painting (occasional probe)  -> 250 ms
//   - nothing to animate (no scrolling rules, icon not on the desktop,
//     desktop icons hidden, desktop gone)        -> sleep until woken by a
//     paint of a rule's icon, a settings change or a re-hook
DWORD WINAPI AnimThread(LPVOID) {
    HANDLE handles[2] = {g_stopEvent, g_animWake};
    unsigned loops = 0;

    for (;;) {
        unsigned paintsAtStart = g_paintCount.load();
        DWORD wait = INFINITE;

        HWND lv = g_listView;
        HWND dv = g_defView;
        bool hooked = lv && dv && IsWindow(lv) && IsWindow(dv) && GetParent(lv) == dv;
        if (!hooked) {
            // Drop stale handles; a new desktop is reported by the hook.
            if (lv && (!IsWindow(lv) || GetParent(lv) != dv)) g_listView.compare_exchange_strong(lv, nullptr);
            if (dv && !IsWindow(dv)) g_defView.compare_exchange_strong(dv, nullptr);
        } else {
            if (g_needFullRedraw.exchange(false)) InvalidateRect(lv, nullptr, TRUE);

            AcquireSRWLockShared(&g_lock);
            int frame = g_frameInterval;
            ReleaseSRWLockShared(&g_lock);

            loops++;
            bool animating = false;
            bool polling = false;
            std::vector<RECT> dirty;

            if (IsWindowVisible(lv)) {
                HMONITOR covered = GetCoveredMonitor();

                AcquireSRWLockExclusive(&g_lock);
                for (size_t i = 0; i < g_rules.size(); i++) {
                    RuleState& st = g_states[i];
                    if (!g_rules[i].scroll || !st.hasRect) continue;

                    if (covered) {
                        RECT sr = st.lastRect;
                        MapWindowPoints(lv, nullptr, (POINT*)&sr, 2);
                        if (MonitorFromRect(&sr, MONITOR_DEFAULTTONEAREST) == covered) {
                            polling = true;  // need to notice when it's uncovered
                            continue;
                        }
                    }

                    if (st.missed > 100) {
                        // The item stopped painting (deleted, hidden): probe
                        // now and then instead of every frame.
                        polling = true;
                        if (loops % 64 != 0) continue;
                    } else {
                        animating = true;
                    }
                    if (st.missed < 1000000) st.missed++;
                    dirty.push_back(st.lastRect);
                }
                ReleaseSRWLockExclusive(&g_lock);
            }

            if (animating) g_tick++;
            for (auto& rc : dirty) InvalidateRect(lv, &rc, TRUE);

            wait = animating ? (DWORD)frame : (polling ? 250 : INFINITE);
        }

        if (wait == INFINITE) {
            // Announce the sleep, then make sure no paint slipped in since
            // this iteration started; otherwise its wake-up would be lost.
            g_animIdle = true;
            if (g_paintCount.load() != paintsAtStart) {
                g_animIdle = false;
                wait = 0;
            }
        }

        DWORD w = WaitForMultipleObjects(2, handles, FALSE, wait);
        g_animIdle = false;
        if (w == WAIT_OBJECT_0) break;
    }
    return 0;
}

DWORD WINAPI InfoThread(LPVOID) {
    // Never show "insert a disk" dialogs for empty card readers etc.
    SetThreadErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX, nullptr);
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    unsigned lastGen = ~0u;
    ULONGLONG lastResolve = 0;
    HANDLE handles[2] = {g_stopEvent, g_infoWake};
    DWORD wait = 0;

    for (;;) {
        DWORD w = WaitForMultipleObjects(2, handles, FALSE, wait);
        if (w == WAIT_OBJECT_0) break;

        // Nothing to draw on: stay idle until the desktop is (re)hooked,
        // which signals g_infoWake.
        if (!g_listView.load()) {
            wait = INFINITE;
            continue;
        }

        unsigned gen;
        int refreshMs;
        std::vector<std::wstring> clsids, tmpls;
        AcquireSRWLockShared(&g_lock);
        gen = g_generation;
        refreshMs = g_refreshInterval;
        for (auto& r : g_rules) {
            clsids.push_back(r.clsid);
            tmpls.push_back(r.tmpl);
        }
        ReleaseSRWLockShared(&g_lock);

        ULONGLONG now = GetTickCount64();
        bool resolve = gen != lastGen || now - lastResolve >= 10000;
        lastGen = gen;

        // Slow work (shell, disks, Recycle Bin) happens without the lock.
        std::vector<std::wstring> names;
        if (resolve) {
            lastResolve = now;
            for (auto& c : clsids) names.push_back(c.empty() ? L"" : ResolveShellName(c));
        }
        UpdateCpu();
        std::vector<std::wstring> texts;
        for (auto& t : tmpls) texts.push_back(Expand(t, now));

        bool namesChanged = false;
        std::vector<RECT> dirty;

        AcquireSRWLockExclusive(&g_lock);
        if (g_generation == gen && g_rules.size() == tmpls.size()) {
            for (size_t i = 0; i < g_rules.size(); i++) {
                Rule& r = g_rules[i];
                RuleState& st = g_states[i];
                if (resolve && !r.clsid.empty()) {
                    if (names[i].empty()) {
                        Wh_Log(L"Could not resolve %s", r.clsid.c_str());
                    } else if (r.matchName != names[i]) {
                        r.matchName = names[i];
                        namesChanged = true;
                        Wh_Log(L"Icon %s -> \"%s\"", r.clsid.c_str(), names[i].c_str());
                    }
                }
                if (st.text != texts[i]) {
                    st.text = texts[i];
                    if (st.hasRect) dirty.push_back(st.lastRect);
                }
            }
        }
        ReleaseSRWLockExclusive(&g_lock);

        // A full redraw only when the set of matched icons changes, never
        // just because a value changed: a missing icon must not cause the
        // whole desktop to repaint every second.
        if (namesChanged) {
            g_needFullRedraw = true;
            WakeAnim();
        }
        HWND lv = g_listView;
        if (lv) {
            for (auto& rc : dirty) InvalidateRect(lv, &rc, TRUE);
        }

        // Wake up on the next refresh boundary, so {time} flips on time.
        SYSTEMTIME st;
        GetLocalTime(&st);
        DWORD msOfMinute = st.wSecond * 1000u + st.wMilliseconds;
        wait = (DWORD)refreshMs - (msOfMinute % (DWORD)refreshMs) + 15;
        if (wait > 10000) wait = 10000;
    }

    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

// Waits for a thread while still serving messages sent to this thread,
// so a worker blocked in SendMessage to us cannot deadlock the unload.
static void WaitPumping(HANDLE h) {
    if (!h) return;
    for (;;) {
        DWORD r = MsgWaitForMultipleObjects(1, &h, FALSE, INFINITE, QS_SENDMESSAGE);
        if (r != WAIT_OBJECT_0 + 1) break;
        MSG msg;
        PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
    }
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

static void Cleanup() {
    // No thread may be started after this point.
    AcquireSRWLockExclusive(&g_startLock);
    g_unloading = true;
    ReleaseSRWLockExclusive(&g_startLock);

    if (g_stopEvent) SetEvent(g_stopEvent);
    WaitPumping(g_animThread);
    WaitPumping(g_infoThread);
    if (g_animThread) { CloseHandle(g_animThread); g_animThread = nullptr; }
    if (g_infoThread) { CloseHandle(g_infoThread); g_infoThread = nullptr; }

    HWND dv = g_defView;
    HWND lv = g_listView;
    if (dv && IsWindow(dv)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(dv, DefViewSubclassProc);
    }
    g_defView = nullptr;
    g_listView = nullptr;
    if (lv && IsWindow(lv)) InvalidateRect(lv, nullptr, TRUE);

    AcquireSRWLockExclusive(&g_lock);
    FreeFontsLocked();
    ReleaseSRWLockExclusive(&g_lock);

    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
    if (g_infoWake) { CloseHandle(g_infoWake); g_infoWake = nullptr; }
    if (g_animWake) { CloseHandle(g_animWake); g_animWake = nullptr; }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_infoWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_animWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_infoWake || !g_animWake) {
        Cleanup();
        return FALSE;
    }

    LoadSettings();

    // The desktop list view is reported when it is created; worker threads
    // start only then, so non-shell explorer.exe processes stay idle.
    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                            (void**)&CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    // The desktop usually exists already when the mod is enabled.
    HWND lv = FindExistingDesktopListView();
    if (lv) OnDesktopListView(lv);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    Cleanup();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
