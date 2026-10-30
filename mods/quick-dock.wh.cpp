// ==WindhawkMod==
// @id              quick-dock
// @name            Quick Dock
// @description     Taskbar island that shows a folder grid on hover, with file thumbnails and the Explorer context menu
// @version         3.9.0
// @author          RHproject
// @github          https://github.com/RHH-cmyk
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32 -lshell32 -lole32 -luuid -lmsimg32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- folder: ""
  $name: Folder
  $description: Leave empty to use the Desktop folder
- label: Quick Dock
  $name: Island label
  $description: Text shown on the taskbar island
- pillAnchor: left
  $name: Island position
  $options:
    - left: Left side (uses the offset below)
    - tray: Left of system tray (free space only)
- offsetX: 16
  $name: Position from taskbar left (px)
  $description: Used when Island position is "Left side"
- trayGap: 8
  $name: Gap from system tray (px)
  $description: Used when Island position is "Left of system tray"
- pillWidth: 0
  $name: Island width (px)
  $description: 0 = automatic, follows the length of the label text
- pillHeight: 30
  $name: Island height (px)
  $description: Automatically limited so it never goes beyond the taskbar height
- fontSize: 12
  $name: Font size (px)
- panelCols: 4
  $name: Columns in folder panel
  $description: Number of columns. Lower = narrower panel
- islandColor: dark
  $name: Island color
  $options:
    - dark: Dark
    - black: Black
    - gray: Gray
    - blue: Blue
    - purple: Purple
    - red: Red
    - green: Green
    - light: Light
- panelColor: dark
  $name: Panel color
  $options:
    - dark: Dark
    - black: Black
    - gray: Gray
    - blue: Blue
    - purple: Purple
    - red: Red
    - green: Green
- outlineColor: gray
  $name: Outline color
  $options:
    - gray: Gray
    - white: White
    - black: Black
    - blue: Blue
    - purple: Purple
    - red: Red
    - green: Green
    - gold: Gold
- outlineWidth: 1
  $name: Outline thickness (px)
  $description: 0 = no outline
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <objbase.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <wchar.h>
#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>

struct Entry {
    std::wstring name;
    std::wstring path;
    bool dir;
    HICON icon;
    HBITMAP thumb;
    bool thumbTried;
};

static HWND g_island = nullptr;
static HWND g_panel = nullptr;
static HANDLE g_thread = nullptr;
static std::wstring g_folder;
static std::wstring g_label = L"Quick Dock";
static std::wstring g_anchor = L"left";
static std::wstring g_clsIsland;
static std::wstring g_clsPanel;
static int g_offsetX = 16;
static int g_trayGap = 8;
static int g_pillWidth = 0;
static int g_pillHeight = 30;
static int g_fontSize = 12;
static int g_panelCols = 4;
static int g_outlineW = 1;
static COLORREF g_islandColor = RGB(30, 30, 36);
static COLORREF g_islandText = RGB(255, 255, 255);
static COLORREF g_panelColor = RGB(30, 30, 36);
static COLORREF g_outlineColor = RGB(58, 58, 58);
static std::vector<Entry> g_entries;
static bool g_expanded = false;
static bool g_suppress = false;
static bool g_menuOpen = false;
static IContextMenu2* g_cm2 = nullptr;
static IContextMenu3* g_cm3 = nullptr;
static float g_anim = 0.0f;
static float g_animTarget = 0.0f;
static double g_s = 1.0;
static RECT g_tb{};
static int g_tbH = 40;
static int g_hover = -1;
static int g_scroll = 0;
static int g_tick = 0;
static int g_leaveTicks = 0;

static int g_cols = 3;
static int g_visRows = 1;
static int g_panelW = 0;
static int g_panelH = 0;

static RECT g_islandRc{};
static RECT g_panelRc{};
static int g_islandRegionW = -1, g_islandRegionH = -1;
static int g_panelRegionW = -1, g_panelRegionH = -1;

static const UINT_PTR TIMER_MAIN = 1;
static const UINT_PTR TIMER_ANIM = 2;
static const UINT_PTR TIMER_HOVER = 3;
static const UINT WM_APP_SETTINGS = WM_APP + 1;

static const COLORREF COL_TILE_HOVER = RGB(60, 60, 70);
static const COLORREF COL_TEXT = RGB(240, 240, 240);
static const COLORREF COL_TEXT_DIM = RGB(160, 160, 170);
static const COLORREF COL_DIR = RGB(120, 190, 255);

// ---------- Layout (semua dalam px, sudah di-scale DPI) ----------
static int S(double v) { return (int)(v * g_s); }
static int Header() { return S(28); }
static int Pad() { return S(10); }
static int TileW() { return S(76); }
static int TileH() { return S(84); }
static int ThumbSz() { return S(36); }
static int Gap() { return S(8); }

static HFONT MakeFont(int px, int weight) {
    return CreateFontW(-px, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

static int MeasureLabelWidth() {
    HDC dc = GetDC(nullptr);
    HFONT f = MakeFont(S(g_fontSize), FW_SEMIBOLD);
    HGDIOBJ old = SelectObject(dc, f);
    SIZE sz{};
    GetTextExtentPoint32W(dc, g_label.c_str(), (int)g_label.size(), &sz);
    SelectObject(dc, old);
    DeleteObject(f);
    ReleaseDC(nullptr, dc);
    return sz.cx + S(28);
}

static int PillW() {
    if (g_pillWidth > 0) return S(g_pillWidth);
    return std::max(MeasureLabelWidth(), S(60));
}

static int PillH() {
    int maxH = g_tbH - S(6);
    int h = S(g_pillHeight);
    if (h > maxH) h = maxH;
    if (h < S(12)) h = S(12);
    return h;
}

static float EaseOutCubic(float t) {
    float u = 1.0f - t;
    return 1.0f - u * u * u;
}

// ---------- Color presets (dipilih dari dropdown) ----------
static COLORREF ColorByName(const std::wstring& n, COLORREF fallback) {
    if (n == L"dark")   return RGB(30, 30, 36);
    if (n == L"black")  return RGB(12, 12, 12);
    if (n == L"gray")   return RGB(58, 58, 58);
    if (n == L"white")  return RGB(240, 240, 240);
    if (n == L"blue")   return RGB(28, 52, 92);
    if (n == L"purple") return RGB(58, 36, 92);
    if (n == L"red")    return RGB(92, 30, 36);
    if (n == L"green")  return RGB(28, 80, 60);
    if (n == L"gold")   return RGB(190, 150, 40);
    if (n == L"light")  return RGB(235, 235, 240);
    return fallback;
}

// ---------- Settings & folder ----------
static std::wstring GetFolder() {
    if (!g_folder.empty()) return g_folder;
    wchar_t buf[MAX_PATH];
    if (SHGetFolderPathW(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, 0, buf) == S_OK)
        return buf;
    return L"";
}

static std::wstring ReadString(PCWSTR key, const wchar_t* fallback) {
    PCWSTR v = Wh_GetStringSetting(key);
    std::wstring out = (v && v[0]) ? v : fallback;
    Wh_FreeStringSetting(v);
    return out;
}

static void LoadSettings() {
    PCWSTR f = Wh_GetStringSetting(L"folder");
    g_folder = f ? f : L"";
    Wh_FreeStringSetting(f);

    PCWSTR l = Wh_GetStringSetting(L"label");
    g_label = (l && l[0]) ? l : L"Quick Dock";
    Wh_FreeStringSetting(l);

    g_anchor = ReadString(L"pillAnchor", L"left");

    g_offsetX = Wh_GetIntSetting(L"offsetX");

    g_trayGap = Wh_GetIntSetting(L"trayGap");
    if (g_trayGap < 0) g_trayGap = 0;
    if (g_trayGap > 200) g_trayGap = 200;

    g_pillWidth = Wh_GetIntSetting(L"pillWidth");
    if (g_pillWidth < 0) g_pillWidth = 0;

    g_pillHeight = Wh_GetIntSetting(L"pillHeight");
    if (g_pillHeight < 12) g_pillHeight = 12;

    g_fontSize = Wh_GetIntSetting(L"fontSize");
    if (g_fontSize < 8) g_fontSize = 8;

    g_panelCols = Wh_GetIntSetting(L"panelCols");
    if (g_panelCols < 1) g_panelCols = 1;
    if (g_panelCols > 8) g_panelCols = 8;

    g_outlineW = Wh_GetIntSetting(L"outlineWidth");
    if (g_outlineW < 0) g_outlineW = 0;
    if (g_outlineW > 6) g_outlineW = 6;

    std::wstring ic = ReadString(L"islandColor", L"dark");
    g_islandColor = ColorByName(ic, RGB(30, 30, 36));
    int lum = (GetRValue(g_islandColor) * 299 + GetGValue(g_islandColor) * 587 +
               GetBValue(g_islandColor) * 114) / 1000;
    g_islandText = lum > 150 ? RGB(20, 20, 20) : RGB(255, 255, 255);

    std::wstring pc = ReadString(L"panelColor", L"dark");
    g_panelColor = ColorByName(pc, RGB(30, 30, 36));

    std::wstring oc = ReadString(L"outlineColor", L"gray");
    g_outlineColor = ColorByName(oc, RGB(58, 58, 58));
}

static void ClearEntries() {
    for (auto& e : g_entries) {
        if (e.icon) DestroyIcon(e.icon);
        if (e.thumb) DeleteObject(e.thumb);
    }
    g_entries.clear();
}

static void RefreshEntries() {
    std::map<std::wstring, Entry> pool;
    for (auto& e : g_entries) pool[e.path] = e;
    g_entries.clear();

    std::wstring dir = GetFolder();
    if (!dir.empty()) {
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW((dir + L"\\*").c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) continue;
                Entry e;
                e.name = fd.cFileName;
                e.path = dir + L"\\" + fd.cFileName;
                e.dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                e.icon = nullptr;
                e.thumb = nullptr;
                e.thumbTried = false;

                auto it = pool.find(e.path);
                if (it != pool.end()) {
                    e.icon = it->second.icon;
                    e.thumb = it->second.thumb;
                    e.thumbTried = it->second.thumbTried;
                    pool.erase(it);
                }
                g_entries.push_back(e);
            } while (FindNextFileW(h, &fd));
            FindClose(h);
        }
    }

    for (auto& kv : pool) {
        if (kv.second.icon) DestroyIcon(kv.second.icon);
        if (kv.second.thumb) DeleteObject(kv.second.thumb);
    }

    std::sort(g_entries.begin(), g_entries.end(), [](const Entry& a, const Entry& b) {
        if (a.dir != b.dir) return a.dir;
        return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
    });
}

static void LoadThumb(Entry& e) {
    if (e.thumbTried) return;
    e.thumbTried = true;

    IShellItem* item = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName(e.path.c_str(), nullptr, IID_PPV_ARGS(&item)))) {
        IShellItemImageFactory* factory = nullptr;
        if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory)))) {
            SIZE sz = {ThumbSz(), ThumbSz()};
            factory->GetImage(sz, SIIGBF_BIGGERSIZEOK | SIIGBF_RESIZETOFIT, &e.thumb);
            factory->Release();
        }
        item->Release();
    }

    if (!e.thumb) {
        SHFILEINFOW sfi{};
        if (SHGetFileInfoW(e.path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON) && sfi.hIcon)
            e.icon = sfi.hIcon;
    }
}

static void ComputePanelSize() {
    int n = (int)g_entries.size();
    g_cols = (n == 0) ? g_panelCols : std::min(n, g_panelCols);
    int rowsNeeded = std::max(1, (n + g_cols - 1) / g_cols);

    int avail = g_tb.top - Gap() - S(60);
    int maxRows = std::max(1, (avail - Header() - Pad()) / TileH());
    g_visRows = std::min(rowsNeeded, maxRows);

    g_panelW = Pad() * 2 + g_cols * TileW();
    g_panelH = Header() + g_visRows * TileH() + Pad();

    int rows = (n + g_cols - 1) / g_cols;
    int maxScroll = std::max(0, rows - g_visRows);
    g_scroll = std::max(0, std::min(g_scroll, maxScroll));
}

static int HitTile(int x, int y) {
    if (x < Pad() || x >= g_panelW - Pad()) return -1;
    if (y < Header() || y >= Header() + g_visRows * TileH()) return -1;
    int col = (x - Pad()) / TileW();
    int row = (y - Header()) / TileH();
    if (col >= g_cols) return -1;
    int idx = (g_scroll + row) * g_cols + col;
    return idx < (int)g_entries.size() ? idx : -1;
}

// ---------- Region (bentuk window) ----------
static int ClampRadius(int radius, int w, int h) {
    return std::min(radius, std::min(w, h) / 2);
}

static void ApplyRegion(HWND h, int w, int hgt, int radius, int& cacheW, int& cacheH) {
    if (w == cacheW && hgt == cacheH) return;
    int r = ClampRadius(radius, w, hgt);
    HRGN rgn = CreateRoundRectRgn(0, 0, w + 1, hgt + 1, r * 2, r * 2);
    SetWindowRgn(h, rgn, TRUE);
    cacheW = w;
    cacheH = hgt;
}

// ---------- Posisi di taskbar ----------
// Kiri system tray (ikon jam, Wi-Fi, volume) dan kanan daftar aplikasi.
static int TrayLeft(HWND tb) {
    HWND tray = FindWindowExW(tb, nullptr, L"TrayNotifyWnd", nullptr);
    RECT r{};
    if (tray && GetWindowRect(tray, &r)) return r.left;
    return 0;
}

static int TaskListRight(HWND tb) {
    HWND rebar = FindWindowExW(tb, nullptr, L"ReBarWindow32", nullptr);
    HWND list = rebar ? FindWindowExW(rebar, nullptr, L"MSTaskListWClass", nullptr) : nullptr;
    RECT r{};
    if (list && GetWindowRect(list, &r)) return r.right;
    return 0;
}

// Mode "tray": island diletakkan di kiri system tray, hanya jika ruangnya cukup
// dan tidak menimpa daftar aplikasi. Kalau tidak cukup, kembali ke posisi kiri.
static int ComputeIslandX(HWND tb, int pw) {
    int leftX = g_tb.left + g_offsetX;
    if (g_anchor != L"tray") return leftX;

    int trayL = TrayLeft(tb);
    if (trayL <= 0) return leftX;

    int x = trayL - S(g_trayGap) - pw;
    int listR = TaskListRight(tb);
    if (x < listR + S(4) || x < g_tb.left) return leftX;
    return x;
}

// ---------- Placement ----------
static void PlacePanel() {
    float t = EaseOutCubic(g_anim);
    int curH = (int)(g_panelH * t);
    if (curH <= 2 || g_panelW <= 0) {
        if (IsWindowVisible(g_panel)) ShowWindow(g_panel, SW_HIDE);
        g_panelRc = RECT{};
        return;
    }

    // Panel rata dengan island: kiri untuk mode kiri, kanan untuk mode tray
    int x;
    if (g_anchor == L"tray" && g_islandRc.right > 0)
        x = g_islandRc.right - g_panelW;
    else
        x = g_tb.left + g_offsetX;
    if (x < 0) x = 0;

    int bottom = g_tb.top - Gap();
    RECT want{x, bottom - curH, x + g_panelW, bottom};

    bool same = want.left == g_panelRc.left && want.top == g_panelRc.top &&
                want.right == g_panelRc.right && want.bottom == g_panelRc.bottom;
    if (same && IsWindowVisible(g_panel)) return;

    SetWindowPos(g_panel, nullptr, want.left, want.top, g_panelW, curH,
                 SWP_NOACTIVATE | SWP_NOZORDER);
    if (!IsWindowVisible(g_panel)) ShowWindow(g_panel, SW_SHOWNOACTIVATE);
    ApplyRegion(g_panel, g_panelW, curH, S(16), g_panelRegionW, g_panelRegionH);
    g_panelRc = want;
    InvalidateRect(g_panel, nullptr, FALSE);
}

static void PlaceAll() {
    HWND tb = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tb) return;
    RECT newTb{};
    if (!GetWindowRect(tb, &newTb)) return;

    UINT dpi = GetDpiForWindow(tb);
    g_s = dpi ? dpi / 96.0 : 1.0;
    g_tb = newTb;
    g_tbH = g_tb.bottom - g_tb.top;

    int pw = PillW();
    int ph = PillH();
    int x = ComputeIslandX(tb, pw);
    int y = g_tb.top + (g_tbH - ph) / 2;
    RECT wantIsland{x, y, x + pw, y + ph};
    if (wantIsland.left != g_islandRc.left || wantIsland.top != g_islandRc.top ||
        wantIsland.right != g_islandRc.right || wantIsland.bottom != g_islandRc.bottom) {
        SetWindowPos(g_island, nullptr, x, y, pw, ph, SWP_NOACTIVATE | SWP_NOZORDER);
        ApplyRegion(g_island, pw, ph, ph / 2, g_islandRegionW, g_islandRegionH);
        g_islandRc = wantIsland;
        InvalidateRect(g_island, nullptr, FALSE);
    }

    ComputePanelSize();
    PlacePanel();
}

static void SetExpanded(bool on) {
    g_expanded = on;
    g_hover = -1;
    if (on) {
        g_scroll = 0;
        RefreshEntries();
        ComputePanelSize();
    }
    g_animTarget = on ? 1.0f : 0.0f;
    SetTimer(g_island, TIMER_ANIM, 15, nullptr);
    InvalidateRect(g_island, nullptr, FALSE);
}

static void UpdateHover() {
    if (g_menuOpen) return;

    POINT p;
    GetCursorPos(&p);

    RECT ir{};
    GetWindowRect(g_island, &ir);

    bool panelVisible = IsWindowVisible(g_panel) != FALSE;
    RECT pr{};
    if (panelVisible) GetWindowRect(g_panel, &pr);

    bool inside = PtInRect(&ir, p) || (panelVisible && PtInRect(&pr, p));

    if (g_suppress) {
        if (!inside) g_suppress = false;
        return;
    }

    if (inside) {
        g_leaveTicks = 0;
        if (!g_expanded) SetExpanded(true);
    } else if (g_expanded) {
        if (++g_leaveTicks >= 6) {
            g_leaveTicks = 0;
            SetExpanded(false);
        }
    }
}

// ---------- Menu konteks Explorer ----------
static void ShowShellContextMenu(HWND owner, const std::wstring& path, POINT screenPt, bool extended) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr)) || !pidl)
        return;

    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    if (FAILED(SHBindToParent(pidl, IID_PPV_ARGS(&parent), &child))) {
        CoTaskMemFree(pidl);
        return;
    }

    IContextMenu* cm = nullptr;
    if (SUCCEEDED(parent->GetUIObjectOf(owner, 1, &child, IID_IContextMenu, nullptr,
                                        (void**)&cm)) && cm) {
        HMENU menu = CreatePopupMenu();
        UINT flags = CMF_NORMAL | CMF_CANRENAME;
        if (extended) flags |= CMF_EXTENDEDVERBS;
        cm->QueryContextMenu(menu, 0, 1, 0x7FFF, flags);

        cm->QueryInterface(IID_PPV_ARGS(&g_cm2));
        cm->QueryInterface(IID_PPV_ARGS(&g_cm3));

        SetForegroundWindow(owner);
        g_menuOpen = true;
        int cmd = TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                   screenPt.x, screenPt.y, owner, nullptr);
        g_menuOpen = false;

        if (cmd > 0) {
            CMINVOKECOMMANDINFOEX ici{};
            ici.cbSize = sizeof(ici);
            ici.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE;
            ici.hwnd = owner;
            ici.lpVerb = MAKEINTRESOURCEA(cmd - 1);
            ici.lpVerbW = MAKEINTRESOURCEW(cmd - 1);
            ici.nShow = SW_SHOWNORMAL;
            ici.ptInvoke = screenPt;
            cm->InvokeCommand((CMINVOKECOMMANDINFO*)&ici);
        }

        if (g_cm3) { g_cm3->Release(); g_cm3 = nullptr; }
        if (g_cm2) { g_cm2->Release(); g_cm2 = nullptr; }
        DestroyMenu(menu);
        cm->Release();
    }

    parent->Release();
    CoTaskMemFree(pidl);
}

// ---------- Painting ----------
static void DrawBox(HDC hdc, const RECT& rc, COLORREF fill, COLORREF line, int lineW, int radius) {
    HBRUSH base = CreateSolidBrush(fill);
    FillRect(hdc, &rc, base);
    DeleteObject(base);

    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    int r = ClampRadius(radius, w, h);

    HBRUSH brush = CreateSolidBrush(fill);
    HGDIOBJ oldBrush = SelectObject(hdc, brush);
    HPEN pen = nullptr;
    HGDIOBJ oldPen = nullptr;
    int inset = 0;
    if (lineW > 0) {
        pen = CreatePen(PS_SOLID, lineW, line);
        oldPen = SelectObject(hdc, pen);
        inset = (lineW + 1) / 2;
    } else {
        oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
    }

    RoundRect(hdc, rc.left + inset, rc.top + inset, rc.right - inset, rc.bottom - inset,
              r * 2, r * 2);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    if (pen) DeleteObject(pen);
    DeleteObject(brush);
}

static void DrawIsland(HDC hdc, const RECT& rc) {
    DrawBox(hdc, rc, g_islandColor, g_outlineColor, g_outlineW, (rc.bottom - rc.top) / 2);

    HFONT f = MakeFont(S(g_fontSize), FW_SEMIBOLD);
    HGDIOBJ old = SelectObject(hdc, f);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, g_islandText);

    RECT tr = rc;
    DrawTextW(hdc, g_label.c_str(), -1, &tr,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

    SelectObject(hdc, old);
    DeleteObject(f);
}

static void PaintIsland(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w > 0 && h > 0) {
        HDC mem = CreateCompatibleDC(hdc);
        HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
        HGDIOBJ ob = SelectObject(mem, bmp);
        RECT local{0, 0, w, h};
        DrawIsland(mem, local);
        BitBlt(hdc, 0, 0, w, h, mem, 0, 0, SRCCOPY);
        SelectObject(mem, ob);
        DeleteObject(bmp);
        DeleteDC(mem);
    }
    EndPaint(hwnd, &ps);
}

static void DrawPanel(HDC hdc, const RECT& rc) {
    DrawBox(hdc, rc, g_panelColor, g_outlineColor, g_outlineW, S(16));

    SetBkMode(hdc, TRANSPARENT);
    HFONT fSmall = MakeFont(S(11), FW_NORMAL);
    HFONT fHead = MakeFont(S(10), FW_NORMAL);
    HGDIOBJ old = SelectObject(hdc, fHead);

    RECT hr{Pad(), 0, g_panelW - Pad(), Header()};
    SetTextColor(hdc, COL_TEXT_DIM);
    std::wstring folder = GetFolder();
    DrawTextW(hdc, folder.c_str(), -1, &hr,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS | DT_NOPREFIX);

    SelectObject(hdc, fSmall);

    if (g_entries.empty()) {
        RECT er{Pad(), Header() + Pad(), g_panelW - Pad(), Header() + S(40)};
        SetTextColor(hdc, RGB(200, 200, 200));
        DrawTextW(hdc, L"Folder is empty or not found", -1, &er, DT_LEFT | DT_NOPREFIX);
    }

    int thumb = ThumbSz();
    for (int r = 0; r < g_visRows; r++) {
        for (int c = 0; c < g_cols; c++) {
            int idx = (g_scroll + r) * g_cols + c;
            if (idx >= (int)g_entries.size()) break;
            Entry& e = g_entries[idx];

            int tx = Pad() + c * TileW();
            int ty = Header() + r * TileH();

            if (idx == g_hover) {
                HBRUSH hb = CreateSolidBrush(COL_TILE_HOVER);
                RECT hbR{tx + S(2), ty + S(2), tx + TileW() - S(2), ty + TileH() - S(2)};
                FillRect(hdc, &hbR, hb);
                DeleteObject(hb);
            }

            LoadThumb(e);
            int ix = tx + (TileW() - thumb) / 2;
            int iy = ty + S(6);
            if (e.thumb) {
                BITMAP bm{};
                GetObject(e.thumb, sizeof(bm), &bm);
                HDC mem = CreateCompatibleDC(hdc);
                HGDIOBJ oldBm = SelectObject(mem, e.thumb);
                BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                AlphaBlend(hdc, ix, iy, thumb, thumb, mem, 0, 0, bm.bmWidth, bm.bmHeight, bf);
                SelectObject(mem, oldBm);
                DeleteDC(mem);
            } else if (e.icon) {
                DrawIconEx(hdc, ix, iy, e.icon, thumb, thumb, 0, nullptr, DI_NORMAL);
            }

            RECT nr{tx + S(3), iy + thumb + S(4), tx + TileW() - S(3), ty + TileH() - S(2)};
            SetTextColor(hdc, e.dir ? COL_DIR : COL_TEXT);
            DrawTextW(hdc, e.name.c_str(), -1, &nr,
                      DT_CENTER | DT_TOP | DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX);
        }
    }

    SelectObject(hdc, old);
    DeleteObject(fSmall);
    DeleteObject(fHead);
}

static void PaintPanel(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w > 0 && h > 0) {
        HDC mem = CreateCompatibleDC(hdc);
        HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
        HGDIOBJ ob = SelectObject(mem, bmp);
        RECT local{0, 0, w, h};
        DrawPanel(mem, local);
        BitBlt(hdc, 0, 0, w, h, mem, 0, 0, SRCCOPY);
        SelectObject(mem, ob);
        DeleteObject(bmp);
        DeleteDC(mem);
    }
    EndPaint(hwnd, &ps);
}

// ---------- Window procedures ----------
static LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        PaintPanel(hwnd);
        return 0;

    case WM_MOUSEMOVE: {
        int idx = HitTile(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        if (idx != g_hover) {
            g_hover = idx;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }

    case WM_LBUTTONUP: {
        int idx = HitTile(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        if (idx >= 0) {
            ShellExecuteW(nullptr, L"open", g_entries[idx].path.c_str(),
                          nullptr, nullptr, SW_SHOWNORMAL);
            g_suppress = true;
            SetExpanded(false);
        }
        return 0;
    }

    case WM_RBUTTONUP: {
        int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
        int idx = HitTile(x, y);
        if (idx >= 0) {
            POINT pt{x, y};
            ClientToScreen(hwnd, &pt);
            bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            ShowShellContextMenu(hwnd, g_entries[idx].path, pt, shift);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }

    case WM_INITMENUPOPUP:
    case WM_MEASUREITEM:
    case WM_DRAWITEM: {
        if (g_cm3) {
            LRESULT lr = 0;
            if (SUCCEEDED(g_cm3->HandleMenuMsg2(msg, wp, lp, &lr))) return lr;
        } else if (g_cm2) {
            if (SUCCEEDED(g_cm2->HandleMenuMsg(msg, wp, lp))) return 0;
        }
        break;
    }

    case WM_MENUCHAR: {
        if (g_cm3) {
            LRESULT lr = 0;
            if (SUCCEEDED(g_cm3->HandleMenuMsg2(msg, wp, lp, &lr))) return lr;
        }
        break;
    }

    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wp);
        int rows = ((int)g_entries.size() + g_cols - 1) / g_cols;
        int maxScroll = std::max(0, rows - g_visRows);
        g_scroll = std::max(0, std::min(g_scroll - delta / WHEEL_DELTA, maxScroll));
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK IslandProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        PaintIsland(hwnd);
        return 0;

    case WM_TIMER:
        if (wp == TIMER_ANIM) {
            float diff = g_animTarget - g_anim;
            if (std::fabs(diff) < 0.01f) {
                g_anim = g_animTarget;
                KillTimer(hwnd, TIMER_ANIM);
            } else {
                g_anim += diff * 0.25f;
            }
            PlacePanel();
            return 0;
        }

        if (wp == TIMER_HOVER) {
            UpdateHover();
            return 0;
        }

        if (wp == TIMER_MAIN) {
            if (++g_tick % 6 == 0) {
                if (g_expanded && !g_menuOpen) {
                    RefreshEntries();
                    ComputePanelSize();
                    PlacePanel();
                }
                PlaceAll();
            }
        }
        return 0;

    case WM_APP_SETTINGS:
        LoadSettings();
        g_islandRc = RECT{};
        g_panelRc = RECT{};
        RefreshEntries();
        PlaceAll();
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_MAIN);
        KillTimer(hwnd, TIMER_ANIM);
        KillTimer(hwnd, TIMER_HOVER);
        if (g_panel) DestroyWindow(g_panel);
        ClearEntries();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------- Thread & mod lifecycle ----------
static DWORD WINAPI UiThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    HINSTANCE hi = GetModuleHandleW(nullptr);
    std::wstring suffix = std::to_wstring(GetTickCount64());
    g_clsIsland = L"QDIsland" + suffix;
    g_clsPanel = L"QDPanel" + suffix;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = hi;
    wc.hCursor = LoadCursorW(nullptr, IDC_HAND);

    wc.lpfnWndProc = IslandProc;
    wc.lpszClassName = g_clsIsland.c_str();
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW (island) failed (error %lu)", GetLastError());
        CoUninitialize();
        return 1;
    }
    wc.lpfnWndProc = PanelProc;
    wc.lpszClassName = g_clsPanel.c_str();
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW (panel) failed (error %lu)", GetLastError());
        CoUninitialize();
        return 1;
    }

    HWND tb = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tb) {
        Wh_Log(L"Taskbar window not found");
        CoUninitialize();
        return 1;
    }

    g_island = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, g_clsIsland.c_str(),
                               L"", WS_POPUP, 0, 0, 1, 1, tb, nullptr, hi, nullptr);
    g_panel = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, g_clsPanel.c_str(),
                              L"", WS_POPUP, 0, 0, 1, 1, tb, nullptr, hi, nullptr);
    if (!g_island || !g_panel) {
        Wh_Log(L"CreateWindowExW failed (error %lu)", GetLastError());
        CoUninitialize();
        return 1;
    }

    RefreshEntries();
    PlaceAll();
    ShowWindow(g_island, SW_SHOWNOACTIVATE);
    SetTimer(g_island, TIMER_MAIN, 500, nullptr);
    SetTimer(g_island, TIMER_HOVER, 60, nullptr);
    Wh_Log(L"Quick Dock created, folder: %s", GetFolder().c_str());

    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0) > 0) {
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }

    UnregisterClassW(g_clsPanel.c_str(), hi);
    UnregisterClassW(g_clsIsland.c_str(), hi);
    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed (error %lu)", GetLastError());
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (g_island) PostMessageW(g_island, WM_APP_SETTINGS, 0, 0);
}

void Wh_ModUninit() {
    if (g_island) PostMessageW(g_island, WM_CLOSE, 0, 0);
    if (g_thread) {
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    g_island = nullptr;
    g_panel = nullptr;
}