// ==WindhawkMod==
// @id              internet-speed-on-taskbar
// @name            Internet Speed On Taskbar
// @description     Native-style live upload/download speed inside the taskbar, left or next to the tray. Works alongside other taskbar mods.
// @version         1.2
// @author          krish
// @github		https://github.com/Webspace-com
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -liphlpapi -lgdi32 -luser32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Internet Speed On Taskbar
Live upload (↑) and download (↓) speed inside the Windows taskbar.

## Works with other mods
* Hooks nothing. It only adds its own child window to the taskbar, so it
  can't conflict with mods that hook taskbar functions.
* Because it is a child of the taskbar, it follows any mod that moves,
  animates, fades, auto-hides or resizes the taskbar (e.g. Taskbar Auto-Hide
  Instant Show, taskbar height/position mods, ExplorerPatcher).


## Placement
* **Right:** just left of the notification area (tray, quick settings,
  clock).
* **Left:** after Widgets (Windows 11) or after Start/Search/Task View
  (Windows 10 / ExplorerPatcher; task buttons shift over to make room).
* Windows 11 with left-aligned icons: Left would cover Start, so Right is
  used.

## Adapts to
Per-monitor DPI, Windows 11 Text size, light/dark/accent taskbar, high
contrast, explorer restarts, monitors added/removed, vertical taskbars
(Windows 10). Counts physical adapters only (VPNs aren't double counted);
use Adapter filter to pick one.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: right
  $name: Position
  $options:
  - right: Right (next to quick settings / tray)
  - left: Left (after Widgets or Start)
- gap: 6
  $name: Gap (px)
  $description: Space between the meter and its anchor, at 100% scaling
- monitors: primary
  $name: Show on
  $options:
  - primary: Main taskbar only
  - all: All taskbars
- lines: auto
  $name: Layout
  $options:
  - auto: Auto (two lines if they fit)
  - two: Two lines
  - one: One line
- fontSize: 9
  $name: Font size (pt)
  $description: 9 matches the taskbar clock
- bold: false
  $name: Semibold text
- textColor: auto
  $name: Text color
  $description: auto follows light/dark taskbar, or a hex color like #FFFFFF
- upArrowColor: "#87CEFA"
  $name: Upload arrow color
  $description: Hex color (default light sky blue), or auto to match the text
- downArrowColor: "#90EE90"
  $name: Download arrow color
  $description: Hex color (default light green), or auto to match the text
- units: bytes
  $name: Units
  $options:
  - bytes: Bytes (KB/s, MB/s)
  - bits: Bits (Kbps, Mbps)
- interval: 1000
  $name: Update interval (ms)
- adapter: ""
  $name: Adapter filter
  $description: Empty = all physical adapters. Or part of an adapter name, e.g. Wi-Fi
- clickOpensTaskManager: false
  $name: Click opens Task Manager
  $description: When off, clicks pass through to the taskbar
*/
// ==/WindhawkModSettings==

#include <winsock2.h>
#include <ws2ipdef.h>
#include <windows.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <shellapi.h>
#include <uiautomation.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cwctype>
#include <string>
#include <vector>

// =====================================================================
// Internet speed meter
// ---------------------------------------------------------------------
// Design notes (why it is built this way):
//  * The meter is a real child window of the taskbar (Shell_TrayWnd /
//    Shell_SecondaryTrayWnd), created on the taskbar's own UI thread. So it
//    moves, slides, fades and hides together with the taskbar (auto-hide,
//    this mod's animations, fullscreen apps) with zero extra work.
//  * Network counters are read on a separate worker thread, so a slow
//    driver query can never stall the taskbar.
//  * Placement is anchored to real taskbar parts: the notification area
//    (TrayNotifyWnd), the Win10 Start/Search/Task View buttons and, on
//    Windows 11, the Widgets button via UI Automation. Fixed offsets are
//    only a fallback (e.g. Win11 reports an empty tray rect at startup).
//  * Size follows the taskbar's DPI (per monitor), Windows 11 text scaling
//    (Accessibility > Text size), light/dark/accent and high contrast.
//  * Per-adapter deltas (keyed by LUID) avoid fake spikes when Wi-Fi or a
//    VPN reconnects.
// =====================================================================

namespace ns {

enum class Pos { Right, Left };
enum class Lines { Auto, Two, One };

struct Config {
    Pos pos = Pos::Right;
    int gap = 6;
    bool allMonitors = false;
    Lines lines = Lines::Auto;
    int fontSize = 9;
    bool bold = false;
    bool autoColor = true;
    COLORREF color = RGB(255, 255, 255);
    bool upAuto = false, downAuto = false;
    COLORREF upColor = RGB(0x87, 0xCE, 0xFA);    // light sky blue
    COLORREF downColor = RGB(0x90, 0xEE, 0x90);  // light green
    bool bits = false;
    int interval = 1000;
    std::wstring adapter;  // lowercase substring filter, empty = auto
    bool clickTaskMgr = false;
};

SRWLOCK g_cfgLock = SRWLOCK_INIT;
Config g_cfg;

Config GetConfig() {
    AcquireSRWLockShared(&g_cfgLock);
    Config c = g_cfg;
    ReleaseSRWLockShared(&g_cfgLock);
    return c;
}

std::atomic<double> g_down{0}, g_up{0};
std::atomic<bool> g_lightTaskbar{false}, g_highContrast{false},
    g_leftAligned{false};
std::atomic<int> g_textScale{100};
std::atomic<bool> g_widgetsValid{false};
std::atomic<int> g_widgetsL{0}, g_widgetsR{0};  // relative to taskbar left
std::atomic<HWND> g_widgetsTaskbar{nullptr};

constexpr UINT WM_NS_TICK = WM_APP + 0x51;
constexpr UINT WM_NS_RELAYOUT = WM_APP + 0x52;
constexpr UINT WM_NS_SETTINGS = WM_APP + 0x53;
constexpr UINT_PTR kLayoutTimer = 1;
constexpr UINT_PTR kSampleTimer = 1;
const wchar_t kMeterClass[] = L"WhNetSpeedMeter_" WH_MOD_ID;
const wchar_t kHostClass[] = L"WhNetSpeedHost_" WH_MOD_ID;

extern "C" IMAGE_DOS_HEADER __ImageBase;
inline HINSTANCE ModInstance() { return (HINSTANCE)&__ImageBase; }

template <typename T>
T Clamp(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }

std::wstring Lower(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}

// ---------------- settings & system state ----------------

void LoadConfig() {
    Config c;

    PCWSTR s = Wh_GetStringSetting(L"position");
    c.pos = (s && wcscmp(s, L"left") == 0) ? Pos::Left : Pos::Right;
    Wh_FreeStringSetting(s);

    c.gap = Clamp(Wh_GetIntSetting(L"gap"), 0, 1000);

    s = Wh_GetStringSetting(L"monitors");
    c.allMonitors = s && wcscmp(s, L"all") == 0;
    Wh_FreeStringSetting(s);

    s = Wh_GetStringSetting(L"lines");
    c.lines = !s ? Lines::Auto
              : wcscmp(s, L"two") == 0 ? Lines::Two
              : wcscmp(s, L"one") == 0 ? Lines::One
                                        : Lines::Auto;
    Wh_FreeStringSetting(s);

    int fs = Wh_GetIntSetting(L"fontSize");
    c.fontSize = fs <= 0 ? 9 : Clamp(fs, 6, 24);
    c.bold = Wh_GetIntSetting(L"bold") != 0;

    s = Wh_GetStringSetting(L"textColor");
    c.autoColor = true;
    if (s && *s && _wcsicmp(s, L"auto") != 0) {
        const wchar_t* p = (s[0] == L'#') ? s + 1 : s;
        if (wcslen(p) == 6) {
            unsigned long v = wcstoul(p, nullptr, 16);
            c.color = RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
            c.autoColor = false;
        }
    }
    Wh_FreeStringSetting(s);

    auto readColor = [](PCWSTR name, bool* isAuto, COLORREF* out) {
        PCWSTR v = Wh_GetStringSetting(name);
        *isAuto = true;
        if (v && *v && _wcsicmp(v, L"auto") != 0) {
            const wchar_t* p = (v[0] == L'#') ? v + 1 : v;
            if (wcslen(p) == 6) {
                unsigned long x = wcstoul(p, nullptr, 16);
                *out = RGB((x >> 16) & 0xFF, (x >> 8) & 0xFF, x & 0xFF);
                *isAuto = false;
            }
        }
        Wh_FreeStringSetting(v);
    };
    readColor(L"upArrowColor", &c.upAuto, &c.upColor);
    readColor(L"downArrowColor", &c.downAuto, &c.downColor);

    s = Wh_GetStringSetting(L"units");
    c.bits = s && wcscmp(s, L"bits") == 0;
    Wh_FreeStringSetting(s);

    c.interval = Clamp(Wh_GetIntSetting(L"interval"), 250, 10000);

    s = Wh_GetStringSetting(L"adapter");
    c.adapter = s ? Lower(s) : L"";
    Wh_FreeStringSetting(s);

    c.clickTaskMgr = Wh_GetIntSetting(L"clickOpensTaskManager") != 0;

    AcquireSRWLockExclusive(&g_cfgLock);
    g_cfg = c;
    ReleaseSRWLockExclusive(&g_cfgLock);
}

DWORD RegDword(PCWSTR path, PCWSTR name, DWORD def) {
    DWORD v = 0, size = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, path, name, RRF_RT_REG_DWORD, nullptr,
                     &v, &size) == ERROR_SUCCESS)
        return v;
    return def;
}

void ReadSystemState() {
    const wchar_t* personalize =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    // Taskbar follows the *system* (not app) mode; accent color forces dark.
    g_lightTaskbar = RegDword(personalize, L"SystemUsesLightTheme", 0) != 0 &&
                     RegDword(personalize, L"ColorPrevalence", 0) == 0;

    HIGHCONTRASTW hc = {sizeof(hc)};
    g_highContrast =
        SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0) &&
        (hc.dwFlags & HCF_HIGHCONTRASTON);

    g_textScale = Clamp<int>(
        (int)RegDword(L"Software\\Microsoft\\Accessibility", L"TextScaleFactor",
                      100),
        100, 225);

    g_leftAligned =
        RegDword(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
                 L"Advanced",
                 L"TaskbarAl", 1) == 0;
}

// ---------------- network sampling (worker thread) ----------------

struct IfPrev {
    ULONG64 luid;
    ULONG64 in, out;
};
std::vector<IfPrev> g_prev;
ULONG64 g_prevTick = 0;

void Sample(const Config& c) {
    PMIB_IF_TABLE2 table = nullptr;
    if (GetIfTable2(&table) != NO_ERROR || !table) return;

    ULONG64 now = GetTickCount64();
    ULONG64 dIn = 0, dOut = 0;
    std::vector<IfPrev> cur;

    for (ULONG i = 0; i < table->NumEntries; i++) {
        const MIB_IF_ROW2& r = table->Table[i];
        if (r.Type == IF_TYPE_SOFTWARE_LOOPBACK) continue;
        if (r.OperStatus != IfOperStatusUp) continue;

        if (c.adapter.empty()) {
            // Auto: physical adapters only, so VPN/virtual/filter layers
            // don't double count the same bytes.
            if (!r.InterfaceAndOperStatusFlags.HardwareInterface) continue;
            if (r.InterfaceAndOperStatusFlags.FilterInterface) continue;
        } else {
            std::wstring alias = Lower(r.Alias), desc = Lower(r.Description);
            if (alias.find(c.adapter) == std::wstring::npos &&
                desc.find(c.adapter) == std::wstring::npos)
                continue;
        }

        cur.push_back({r.InterfaceLuid.Value, r.InOctets, r.OutOctets});
        for (const auto& p : g_prev) {
            if (p.luid == r.InterfaceLuid.Value) {
                if (r.InOctets >= p.in) dIn += r.InOctets - p.in;
                if (r.OutOctets >= p.out) dOut += r.OutOctets - p.out;
                break;
            }
        }
    }
    FreeMibTable(table);

    if (g_prevTick && now > g_prevTick) {
        double secs = (now - g_prevTick) / 1000.0;
        g_down = dIn / secs;
        g_up = dOut / secs;
    }
    g_prev.swap(cur);
    g_prevTick = now;
}

const wchar_t* kByteUnits[] = {L"B/s", L"KB/s", L"MB/s", L"GB/s"};
const wchar_t* kBitUnits[] = {L"bps", L"Kbps", L"Mbps", L"Gbps"};

std::wstring FormatSpeed(double bytesPerSec, bool bits) {
    double v = bits ? bytesPerSec * 8 : bytesPerSec;
    const wchar_t** units = bits ? kBitUnits : kByteUnits;
    double step = bits ? 1000.0 : 1024.0;
    int u = 0;
    while (v >= step * 0.995 && u < 3) {
        v /= step;
        u++;
    }
    wchar_t buf[32];
    if (u == 0 || v >= 99.95)
        swprintf(buf, 32, L"%.0f %ls", v, units[u]);
    else
        swprintf(buf, 32, L"%.1f %ls", v, units[u]);
    return buf;
}

// ---------------- Windows 11 Widgets button (UI Automation) ----------------

IUIAutomation* g_uia = nullptr;
ULONG64 g_lastUia = 0;

void UpdateWidgets() {
    ULONG64 now = GetTickCount64();
    if (g_lastUia && now - g_lastUia < 2000) return;
    g_lastUia = now;

    HWND tb = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tb || !FindWindowExW(tb, nullptr,
                              L"Windows.UI.Composition.DesktopWindowContentBridge",
                              nullptr)) {
        g_widgetsValid = false;
        return;
    }
    if (!g_uia &&
        FAILED(CoCreateInstance(CLSID_CUIAutomation, nullptr,
                                CLSCTX_INPROC_SERVER, IID_IUIAutomation,
                                (void**)&g_uia))) {
        g_uia = nullptr;
        return;
    }

    bool ok = false;
    RECT r = {};
    IUIAutomationElement* root = nullptr;
    if (SUCCEEDED(g_uia->ElementFromHandle(tb, &root)) && root) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_BSTR;
        v.bstrVal = SysAllocString(L"WidgetsButton");
        IUIAutomationCondition* cond = nullptr;
        g_uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, v, &cond);
        VariantClear(&v);
        if (cond) {
            IUIAutomationElement* el = nullptr;
            if (SUCCEEDED(root->FindFirst(TreeScope_Descendants, cond, &el)) &&
                el) {
                BOOL off = TRUE;
                if (SUCCEEDED(el->get_CurrentBoundingRectangle(&r)) &&
                    SUCCEEDED(el->get_CurrentIsOffscreen(&off)) && !off &&
                    r.right > r.left)
                    ok = true;
                el->Release();
            }
            cond->Release();
        }
        root->Release();
    }

    RECT tr;
    GetWindowRect(tb, &tr);
    g_widgetsL = r.left - tr.left;
    g_widgetsR = r.right - tr.left;
    g_widgetsTaskbar = tb;
    g_widgetsValid = ok;
}

// ---------------- the meter window (taskbar UI thread) ----------------

struct StyleKey {
    UINT dpi = 0;
    int bandH = 0, availW = 0, pt = 0, scale = 0;
    bool bold = false, bits = false;
    Lines lines = Lines::Auto;
    bool operator==(const StyleKey& o) const {
        return dpi == o.dpi && bandH == o.bandH && availW == o.availW &&
               pt == o.pt && scale == o.scale && bold == o.bold &&
               bits == o.bits && lines == o.lines;
    }
};

struct Meter {
    HWND hwnd = nullptr;
    HWND taskbar = nullptr;
    bool primary = true;

    StyleKey key;
    bool styleValid = false;
    HFONT font = nullptr;
    bool twoLines = true;
    int w = 0, h = 0, arrowW = 0, valueW = 0, lineH = 0, pad = 0;

    HDC memDC = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ oldBmp = nullptr;
    void* pixels = nullptr;
    int bmpW = 0, bmpH = 0;

    HWND rebar = nullptr;
    bool rebarTouched = false;
    bool rebarHoriz = true;
    Pos rebarPos = Pos::Right;
    int rebarOrigEdge = 0;  // client coords

    RECT lastRc = {};
};

PCWSTR FontFace() {
    static int has = -1;
    if (has < 0) {
        LOGFONTW lf = {};
        lf.lfCharSet = DEFAULT_CHARSET;
        wcscpy_s(lf.lfFaceName, L"Segoe UI Variable Text");
        int found = 0;
        HDC dc = GetDC(nullptr);
        EnumFontFamiliesExW(
            dc, &lf,
            [](const LOGFONTW*, const TEXTMETRICW*, DWORD, LPARAM lp) -> int {
                *(int*)lp = 1;
                return 0;
            },
            (LPARAM)&found, 0);
        ReleaseDC(nullptr, dc);
        has = found;
    }
    return has ? L"Segoe UI Variable Text" : L"Segoe UI";
}

void SetFont(Meter* m, int pt, UINT dpi, int scale, bool bold, bool bits) {
    if (m->font) {
        SelectObject(m->memDC, GetStockObject(SYSTEM_FONT));
        DeleteObject(m->font);
    }
    int px = MulDiv(MulDiv(pt, scale, 100), (int)dpi, 72);
    m->font = CreateFontW(-px, 0, 0, 0, bold ? FW_SEMIBOLD : FW_NORMAL, FALSE,
                          FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          DEFAULT_PITCH, FontFace());
    SelectObject(m->memDC, m->font);

    TEXTMETRICW tm;
    GetTextMetricsW(m->memDC, &tm);
    m->lineH = tm.tmHeight;

    SIZE sz;
    GetTextExtentPoint32W(m->memDC, L"\u2193 ", 2, &sz);
    m->arrowW = sz.cx;

    const wchar_t** units = bits ? kBitUnits : kByteUnits;
    int mx = 0;
    for (int i = 0; i < 4; i++) {
        std::wstring t = std::wstring(L"888.8 ") + units[i];
        GetTextExtentPoint32W(m->memDC, t.c_str(), (int)t.size(), &sz);
        mx = (std::max)(mx, (int)sz.cx);
    }
    m->valueW = mx;
}

// bandH: visible taskbar height (horizontal); availW: width (vertical)
void EnsureStyle(Meter* m, const Config& c, UINT dpi, int bandH, int availW,
                 bool win11) {
    StyleKey k;
    k.dpi = dpi;
    k.bandH = bandH;
    k.availW = availW;
    k.pt = c.fontSize;
    k.scale = win11 ? g_textScale.load() : 100;  // Win10 clock ignores it
    k.bold = c.bold;
    k.bits = c.bits;
    k.lines = c.lines;
    if (m->styleValid && k == m->key) return;

    m->pad = MulDiv(2, (int)dpi, 96);
    int gapPx = MulDiv(2, (int)dpi, 96);

    if (availW > 0) {
        // Vertical taskbar: always two lines, shrink until the width fits.
        for (int pt = c.fontSize;; pt--) {
            SetFont(m, pt, dpi, k.scale, c.bold, c.bits);
            if (m->arrowW + m->valueW <= availW || pt <= 6) break;
        }
        m->twoLines = true;
    } else {
        bool two = false;
        if (c.lines != Lines::One) {
            int minPt = c.lines == Lines::Two ? 6 : (std::max)(6, c.fontSize - 1);
            for (int pt = c.fontSize; pt >= minPt; pt--) {
                SetFont(m, pt, dpi, k.scale, c.bold, c.bits);
                if (2 * m->lineH <= bandH - 2 * m->pad) {
                    two = true;
                    break;
                }
            }
            if (c.lines == Lines::Two) two = true;  // forced, smallest font
        }
        if (!two) SetFont(m, c.fontSize, dpi, k.scale, c.bold, c.bits);
        m->twoLines = two;
    }

    if (m->twoLines) {
        m->w = m->arrowW + m->valueW + gapPx;
        m->h = 2 * m->lineH;
    } else {
        m->w = 2 * (m->arrowW + m->valueW) + MulDiv(12, (int)dpi, 96);
        m->h = m->lineH;
    }
    m->key = k;
    m->styleValid = true;
}

COLORREF TextColor(const Config& c) {
    if (g_highContrast) return GetSysColor(COLOR_WINDOWTEXT);
    if (!c.autoColor) return c.color;
    return g_lightTaskbar ? RGB(0, 0, 0) : RGB(255, 255, 255);
}

void Render(Meter* m, const Config& c) {
    if (!m->font || m->w <= 0 || m->h <= 0) return;

    if (!m->bmp || m->bmpW != m->w || m->bmpH != m->h) {
        if (m->bmp) {
            SelectObject(m->memDC, m->oldBmp);
            DeleteObject(m->bmp);
            m->bmp = nullptr;
        }
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = m->w;
        bi.bmiHeader.biHeight = -m->h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        m->bmp = CreateDIBSection(m->memDC, &bi, DIB_RGB_COLORS, &m->pixels,
                                  nullptr, 0);
        if (!m->bmp) return;
        m->oldBmp = SelectObject(m->memDC, m->bmp);
        m->bmpW = m->w;
        m->bmpH = m->h;
    }

    ZeroMemory(m->pixels, (size_t)m->w * m->h * 4);
    SelectObject(m->memDC, m->font);
    SetBkMode(m->memDC, TRANSPARENT);
    SetTextColor(m->memDC, RGB(255, 255, 255));  // coverage mask

    std::wstring up = FormatSpeed(g_up, c.bits);
    std::wstring down = FormatSpeed(g_down, c.bits);
    UINT f = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;

    auto drawPair = [&](RECT rc, const wchar_t* arrow, const std::wstring& v) {
        DrawTextW(m->memDC, arrow, -1, &rc, f | DT_LEFT);
        DrawTextW(m->memDC, v.c_str(), -1, &rc, f | DT_RIGHT);
    };

    if (m->twoLines) {
        int top = (m->h - 2 * m->lineH) / 2;
        drawPair({0, top, m->w, top + m->lineH}, L"\u2191", up);
        drawPair({0, top + m->lineH, m->w, top + 2 * m->lineH}, L"\u2193", down);
    } else {
        int half = m->arrowW + m->valueW;
        drawPair({0, 0, half, m->h}, L"\u2193", down);
        drawPair({m->w - half, 0, m->w, m->h}, L"\u2191", up);
    }
    GdiFlush();

    // Grayscale coverage -> premultiplied BGRA. Arrows get their own color.
    COLORREF textCol = TextColor(c);
    bool hc = g_highContrast;
    COLORREF upCol = (hc || c.upAuto) ? textCol : c.upColor;
    COLORREF downCol = (hc || c.downAuto) ? textCol : c.downColor;

    // Arrow cells: [x0, x1) x [y0, y1)
    struct Cell { int x0, x1, y0, y1; COLORREF col; } cells[2];
    if (m->twoLines) {
        int top = (m->h - 2 * m->lineH) / 2;
        cells[0] = {0, m->arrowW, top, top + m->lineH, upCol};
        cells[1] = {0, m->arrowW, top + m->lineH, top + 2 * m->lineH, downCol};
    } else {
        int half = m->arrowW + m->valueW;
        cells[0] = {0, m->arrowW, 0, m->h, downCol};
        cells[1] = {m->w - half, m->w - half + m->arrowW, 0, m->h, upCol};
    }

    BYTE* p = (BYTE*)m->pixels;
    for (int py = 0; py < m->h; py++) {
        for (int px = 0; px < m->w; px++, p += 4) {
            BYTE a = (std::max)({p[0], p[1], p[2]});
            COLORREF col = textCol;
            for (const auto& cl : cells)
                if (px >= cl.x0 && px < cl.x1 && py >= cl.y0 && py < cl.y1)
                    col = cl.col;
            p[0] = (BYTE)(GetBValue(col) * a / 255);
            p[1] = (BYTE)(GetGValue(col) * a / 255);
            p[2] = (BYTE)(GetRValue(col) * a / 255);
            p[3] = a;
        }
    }

    POINT src = {0, 0};
    SIZE size = {m->w, m->h};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(m->hwnd, nullptr, nullptr, &size, m->memDC, &src, 0,
                        &bf, ULW_ALPHA);
}

bool VisibleChildRect(HWND tb, HWND child, RECT* out) {
    if (!child || !IsWindowVisible(child)) return false;
    GetWindowRect(child, out);
    RECT tr, i;
    GetWindowRect(tb, &tr);
    return out->right > out->left && out->bottom > out->top &&
           IntersectRect(&i, out, &tr);
}

// Win10 / ExplorerPatcher: edge right after Start, Search and Task View.
int Win10LeftAnchor(HWND tb, bool horiz, HWND self) {
    int edge = INT_MIN;
    for (HWND ch = GetWindow(tb, GW_CHILD); ch; ch = GetWindow(ch, GW_HWNDNEXT)) {
        if (ch == self) continue;
        wchar_t cls[64];
        if (!GetClassNameW(ch, cls, 64)) continue;
        if (wcscmp(cls, L"Start") && wcscmp(cls, L"TrayButton") &&
            wcscmp(cls, L"TrayDummySearchControl"))
            continue;
        RECT r;
        if (VisibleChildRect(tb, ch, &r))
            edge = (std::max)(edge, (int)(horiz ? r.right : r.bottom));
    }
    return edge;
}

RECT ClientRectOf(HWND parent, HWND child) {
    RECT r;
    GetWindowRect(child, &r);
    MapWindowPoints(HWND_DESKTOP, parent, (POINT*)&r, 2);
    return r;
}

// Win10 / ExplorerPatcher: shrink the task-button band so buttons never sit
// under the meter (explorer re-lays it out at times; we re-apply).
void AdjustRebar(Meter* m, bool horiz, Pos pos, RECT meterClient, int gap) {
    if (!m->rebar || !IsWindow(m->rebar))
        m->rebar = FindWindowExW(m->taskbar, nullptr, L"ReBarWindow32", nullptr);
    if (!m->rebar || !IsWindowVisible(m->rebar)) return;

    RECT r = ClientRectOf(m->taskbar, m->rebar);
    RECT nr = r;
    if (horiz && pos == Pos::Right && r.right > meterClient.left - gap)
        nr.right = meterClient.left - gap;
    else if (horiz && pos == Pos::Left && r.left < meterClient.right + gap)
        nr.left = meterClient.right + gap;
    else if (!horiz && pos == Pos::Right && r.bottom > meterClient.top - gap)
        nr.bottom = meterClient.top - gap;
    else if (!horiz && pos == Pos::Left && r.top < meterClient.bottom + gap)
        nr.top = meterClient.bottom + gap;
    else
        return;
    if (nr.right - nr.left < 1 || nr.bottom - nr.top < 1) return;

    if (!m->rebarTouched || m->rebarHoriz != horiz || m->rebarPos != pos) {
        m->rebarOrigEdge = horiz ? (pos == Pos::Right ? r.right : r.left)
                                 : (pos == Pos::Right ? r.bottom : r.top);
        m->rebarHoriz = horiz;
        m->rebarPos = pos;
        m->rebarTouched = true;
    }
    MoveWindow(m->rebar, nr.left, nr.top, nr.right - nr.left,
               nr.bottom - nr.top, TRUE);
}

void RestoreRebar(Meter* m) {
    if (!m->rebarTouched || !m->rebar || !IsWindow(m->rebar)) return;
    RECT r = ClientRectOf(m->taskbar, m->rebar);
    if (m->rebarHoriz) {
        if (m->rebarPos == Pos::Right) r.right = (std::max)(r.right, (LONG)m->rebarOrigEdge);
        else r.left = (std::min)(r.left, (LONG)m->rebarOrigEdge);
    } else {
        if (m->rebarPos == Pos::Right) r.bottom = (std::max)(r.bottom, (LONG)m->rebarOrigEdge);
        else r.top = (std::min)(r.top, (LONG)m->rebarOrigEdge);
    }
    MoveWindow(m->rebar, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE);
    m->rebarTouched = false;
}

// Returns true when the meter's size changed (needs a re-render).
bool Layout(Meter* m, const Config& c, bool force) {
    HWND tb = m->taskbar;
    if (!IsWindow(tb)) return false;

    RECT tr;
    GetWindowRect(tb, &tr);
    int tbW = tr.right - tr.left, tbH = tr.bottom - tr.top;
    if (tbW <= 0 || tbH <= 0) return false;

    bool horiz = tbW >= tbH;
    bool win11 = FindWindowExW(tb, nullptr,
                               L"Windows.UI.Composition.DesktopWindowContentBridge",
                               nullptr) != nullptr;
    UINT dpi = GetDpiForWindow(tb);
    if (!dpi) dpi = 96;
    auto S = [&](int v) { return MulDiv(v, (int)dpi, 96); };

    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromWindow(tb, MONITOR_DEFAULTTONEAREST), &mi);

    RECT tray = {};
    bool trayOk = VisibleChildRect(
        tb, FindWindowExW(tb, nullptr, L"TrayNotifyWnd", nullptr), &tray);

    Pos pos = c.pos;
    // Win11 with left-aligned icons: Start sits at the left edge.
    if (win11 && g_leftAligned && pos == Pos::Left) pos = Pos::Right;

    bool widgetsHere = win11 && g_widgetsValid && g_widgetsTaskbar.load() == tb;
    int gap = S(c.gap);
    int prevW = m->w, prevH = m->h;
    int x, y;

    if (horiz) {
        int bandT, bandB;
        if (trayOk) {
            bandT = tray.top;
            bandB = tray.bottom;
        } else if (win11) {
            // Win11 Shell_TrayWnd can include invisible padding at high DPI.
            int vis = (std::min)(tbH, S(48));
            bool bottomEdge = (tr.top + tr.bottom) / 2 >
                              (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
            bandT = bottomEdge ? tr.bottom - vis : tr.top;
            bandB = bandT + vis;
        } else {
            bandT = tr.top;
            bandB = tr.bottom;
        }
        EnsureStyle(m, c, dpi, bandB - bandT, 0, win11);

        if (pos == Pos::Right) {
            int anchor = trayOk ? tray.left
                                : tr.right - S(win11 ? (m->primary ? 300 : 120)
                                                     : (m->primary ? 250 : 100));
            if (widgetsHere) {  // Widgets sits right when icons are left-aligned
                int wl = tr.left + g_widgetsL;
                if (wl > tr.left + tbW / 2 && wl < anchor) anchor = wl;
            }
            x = anchor - gap - m->w;
        } else {
            int anchor = tr.left;
            if (win11) {
                if (widgetsHere) {
                    int wr = tr.left + g_widgetsR;
                    if (wr < tr.left + tbW / 2) anchor = wr;
                }
            } else {
                int a = Win10LeftAnchor(tb, true, m->hwnd);
                if (a != INT_MIN) anchor = a;
            }
            x = anchor + gap;
        }
        y = bandT + ((bandB - bandT) - m->h) / 2;
        x = Clamp<int>(x, tr.left, (std::max)((int)tr.left, (int)tr.right - m->w));
    } else {
        EnsureStyle(m, c, dpi, 0, tbW - S(6), win11);
        x = tr.left + (tbW - m->w) / 2;
        if (pos == Pos::Right) {
            int anchor = trayOk ? tray.top : tr.bottom - S(250);
            y = anchor - gap - m->h;
        } else {
            int a = win11 ? INT_MIN : Win10LeftAnchor(tb, false, m->hwnd);
            y = (a != INT_MIN ? a : (int)tr.top) + gap;
        }
        y = Clamp<int>(y, tr.top, (std::max)((int)tr.top, (int)tr.bottom - m->h));
    }

    POINT pt = {x, y};
    ScreenToClient(tb, &pt);
    RECT want = {pt.x, pt.y, pt.x + m->w, pt.y + m->h};

    if (!win11 && m->primary) AdjustRebar(m, horiz, pos, want, S(4));

    bool onTop = GetWindow(tb, GW_CHILD) == m->hwnd;
    if (force || !onTop || !EqualRect(&want, &m->lastRc)) {
        SetWindowPos(m->hwnd, HWND_TOP, want.left, want.top, m->w, m->h,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        m->lastRc = want;
    }
    return force || prevW != m->w || prevH != m->h;
}

void FreeMeter(Meter* m) {
    if (m->memDC) {
        if (m->bmp) {
            SelectObject(m->memDC, m->oldBmp);
            DeleteObject(m->bmp);
        }
        SelectObject(m->memDC, GetStockObject(SYSTEM_FONT));
        if (m->font) DeleteObject(m->font);
        DeleteDC(m->memDC);
    }
    delete m;
}

LRESULT CALLBACK MeterProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Meter* m = (Meter*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (msg) {
        case WM_NCCREATE: {
            m = (Meter*)((CREATESTRUCTW*)lp)->lpCreateParams;
            m->hwnd = hwnd;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)m);
            break;
        }
        case WM_NS_TICK:
            if (m) {
                Config c = GetConfig();
                Layout(m, c, false);
                Render(m, c);
            }
            return 0;
        case WM_NS_RELAYOUT:
            if (m) {
                Config c = GetConfig();
                m->styleValid = false;
                Layout(m, c, true);
                Render(m, c);
            }
            return 0;
        case WM_TIMER:
            if (m && wp == kLayoutTimer) {
                Config c = GetConfig();
                if (Layout(m, c, false)) Render(m, c);
            }
            return 0;
        case WM_NCHITTEST:
            return GetConfig().clickTaskMgr ? HTCLIENT : HTTRANSPARENT;
        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_ARROW));
            return TRUE;
        case WM_LBUTTONUP:
            ShellExecuteW(nullptr, L"open", L"taskmgr.exe", nullptr, nullptr,
                          SW_SHOWNORMAL);
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kLayoutTimer);
            if (m) RestoreRebar(m);
            return 0;
        case WM_NCDESTROY:
            if (m) {
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
                FreeMeter(m);
            }
            break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// Run code on the thread that owns hWnd (the taskbar UI thread).
using RunProc_t = void(WINAPI*)(void*);
bool RunFromWindowThread(HWND hWnd, RunProc_t proc, void* param) {
    static const UINT runMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param {
        RunProc_t proc;
        void* param;
    };
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hWnd, &pid);
    if (!tid || pid != GetCurrentProcessId()) return false;
    if (tid == GetCurrentThreadId()) {
        proc(param);
        return true;
    }
    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wp, LPARAM lp) -> LRESULT {
            if (code == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lp;
                if (cwp->message == runMsg) {
                    Param* p = (Param*)cwp->lParam;
                    p->proc(p->param);
                }
            }
            return CallNextHookEx(nullptr, code, wp, lp);
        },
        nullptr, tid);
    if (!hook) return false;
    Param p = {proc, param};
    SendMessageW(hWnd, runMsg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

struct CreateParam {
    HWND taskbar;
    bool primary;
    HWND result;
};

void WINAPI CreateMeterOnTaskbarThread(void* param) {
    auto* cp = (CreateParam*)param;
    Meter* m = new Meter;
    m->taskbar = cp->taskbar;
    m->primary = cp->primary;
    m->memDC = CreateCompatibleDC(nullptr);

    HWND h = CreateWindowExW(WS_EX_LAYERED | WS_EX_NOACTIVATE, kMeterClass,
                             L"", WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 1, 1,
                             cp->taskbar, nullptr, ModInstance(), m);
    if (!h) {
        Wh_Log(L"NetSpeed: CreateWindowEx failed: %u", GetLastError());
        return;  // if WM_NCCREATE ran, WM_NCDESTROY already freed m
    }
    SetTimer(h, kLayoutTimer, 250, nullptr);
    Config c = GetConfig();
    Layout(m, c, true);
    Render(m, c);
    ShowWindow(h, SW_SHOWNA);
    cp->result = h;
}

// ---------------- host / worker thread ----------------

HANDLE g_worker = nullptr;
HANDLE g_workerReady = nullptr;
std::atomic<HWND> g_host{nullptr};
UINT g_taskbarCreatedMsg = 0;
struct MeterRef {
    HWND taskbar;
    HWND meter;
};
std::vector<MeterRef> g_meters;  // worker thread only

void Broadcast(UINT msg) {
    for (auto& r : g_meters) PostMessageW(r.meter, msg, 0, 0);
}

void DestroyAllMeters() {
    for (auto& r : g_meters)
        if (IsWindow(r.meter)) SendMessageW(r.meter, WM_CLOSE, 0, 0);
    g_meters.clear();
}

void EnsureMeters(const Config& c) {
    std::vector<std::pair<HWND, bool>> want;
    if (HWND p = FindWindowW(L"Shell_TrayWnd", nullptr)) want.push_back({p, true});
    if (c.allMonitors) {
        HWND s = nullptr;
        while ((s = FindWindowExW(nullptr, s, L"Shell_SecondaryTrayWnd", nullptr)))
            want.push_back({s, false});
    }

    for (auto it = g_meters.begin(); it != g_meters.end();) {
        bool wanted = false;
        for (auto& w : want) wanted |= (w.first == it->taskbar);
        if (!IsWindow(it->meter) || GetParent(it->meter) != it->taskbar) {
            it = g_meters.erase(it);
        } else if (!wanted) {
            SendMessageW(it->meter, WM_CLOSE, 0, 0);
            it = g_meters.erase(it);
        } else {
            ++it;
        }
    }

    for (auto& w : want) {
        bool have = false;
        for (auto& r : g_meters) have |= (r.taskbar == w.first);
        if (have) continue;
        CreateParam cp = {w.first, w.second, nullptr};
        if (RunFromWindowThread(w.first, CreateMeterOnTaskbarThread, &cp) &&
            cp.result)
            g_meters.push_back({w.first, cp.result});
    }
}

void Tick() {
    Config c = GetConfig();
    Sample(c);
    if (c.pos == Pos::Left || g_leftAligned) UpdateWidgets();
    EnsureMeters(c);
    Broadcast(WM_NS_TICK);
}

LRESULT CALLBACK HostProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {  // explorer rebuilt taskbar
        g_lastUia = 0;
        EnsureMeters(GetConfig());
        Broadcast(WM_NS_RELAYOUT);
        return 0;
    }
    switch (msg) {
        case WM_TIMER:
            if (wp == kSampleTimer) Tick();
            return 0;
        case WM_SETTINGCHANGE:
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE:
            ReadSystemState();
            g_lastUia = 0;
            Broadcast(WM_NS_RELAYOUT);
            break;
        case WM_NS_SETTINGS: {
            LoadConfig();
            Config c = GetConfig();
            SetTimer(hwnd, kSampleTimer, c.interval, nullptr);
            g_lastUia = 0;
            EnsureMeters(c);
            Broadcast(WM_NS_RELAYOUT);
            return 0;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kSampleTimer);
            DestroyAllMeters();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

DWORD WINAPI WorkerProc(LPVOID) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSW wc = {};
    wc.lpfnWndProc = HostProc;
    wc.hInstance = ModInstance();
    wc.lpszClassName = kHostClass;
    RegisterClassW(&wc);

    // Hidden top-level window: receives theme/DPI/display/taskbar broadcasts.
    HWND host = CreateWindowExW(WS_EX_TOOLWINDOW, kHostClass, L"", WS_POPUP, 0,
                                0, 0, 0, nullptr, nullptr, ModInstance(), nullptr);
    g_host = host;
    SetEvent(g_workerReady);

    if (host) {
        ReadSystemState();
        Tick();
        SetTimer(host, kSampleTimer, GetConfig().interval, nullptr);
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    g_host = nullptr;
    if (g_uia) {
        g_uia->Release();
        g_uia = nullptr;
    }
    UnregisterClassW(kHostClass, ModInstance());
    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

void Start() {
    if (g_worker) return;
    WNDCLASSW wc = {};
    wc.lpfnWndProc = MeterProc;
    wc.hInstance = ModInstance();
    wc.lpszClassName = kMeterClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    g_prev.clear();
    g_prevTick = 0;
    g_workerReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_worker = CreateThread(nullptr, 0, WorkerProc, nullptr, 0, nullptr);
    if (g_worker) WaitForSingleObject(g_workerReady, 5000);
}

void Stop() {
    if (!g_worker) return;
    if (HWND h = g_host) PostMessageW(h, WM_CLOSE, 0, 0);
    WaitForSingleObject(g_worker, 15000);
    CloseHandle(g_worker);
    g_worker = nullptr;
    if (g_workerReady) {
        CloseHandle(g_workerReady);
        g_workerReady = nullptr;
    }
    UnregisterClassW(kMeterClass, ModInstance());
}

}  // namespace ns
// ===================== end internet speed meter =====================


BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    ns::LoadConfig();
    ns::Start();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    ns::Stop();
}

void Wh_ModSettingsChanged() {
    ns::LoadConfig();
    if (HWND h = ns::g_host) {
        PostMessageW(h, ns::WM_NS_SETTINGS, 0, 0);
    }
}
