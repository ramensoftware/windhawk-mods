// ==WindhawkMod==
// @id              applaunchdock-launchpad
// @name            AppLaunchDock/Launchpad
// @description     Right-edge dock that auto-hides, accepts dragged apps, expands to a centered all-apps view, and copies the live taskbar color
// @version         0.2.0
// @author          ThatShadowDev
// @github          https://github.com/ThatShadowDev
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 -lgdiplus -lole32 -loleaut32 -lshell32 -lshlwapi -lgdi32 -ldwmapi -luuid -luser32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# App-Launch-Dock (taskbar-themed, auto-hide)

A dock on the right edge of your primary screen.

* **Auto-hide:** a thin activation bar sits on the right edge. Hover it and the dock slides out.
  Move away and it slides back. Turn auto-hide off in settings to keep it always visible.
* **Drag & drop:** drag apps, shortcuts, files or Start-menu apps onto the dock to pin them.
  When auto-hide is on, push the cursor against the right edge while dragging to reveal it.
* **All apps:** once the dock is out, click the grid button at the top. The dock expands to the
  center of the screen with every installed app (including Store apps). Just start typing to
  search, scroll with the mouse wheel, click to launch. Esc or clicking elsewhere closes it.
* **Right-click** any icon to open, pin/unpin, or open its file location.
* **Taskbar styling:** the dock samples the real taskbar's pixels every 2 seconds, so it follows
  Taskbar Styler themes, accent tints and light/dark mode. Text color adapts to the background.

Pinned items are shortcuts in a folder (default: the classic Quick Launch folder).
(I should note that this mod was inspired by another of its type by cheliks1123, so credit to them. I wanted to try and re-code that with more features so it could better match the rest of my customizations.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- folder: ""
  $name: Shortcuts folder
  $description: Leave empty to use %APPDATA%\Microsoft\Internet Explorer\Quick Launch
- iconSize: 36
  $name: Dock icon size
- autohide: true
  $name: Auto-hide the dock
- showActivationBar: true
  $name: Show the activation bar on the right edge
- barHeightPercent: 40
  $name: Activation bar height (% of screen height)
- showDelayMs: 150
  $name: Hover delay before the dock appears (ms)
- hideDelayMs: 600
  $name: Delay before the dock hides (ms)
- edgeMargin: 10
  $name: Gap between the dock and the screen edge
- cornerRadius: 12
  $name: Corner radius (0 = square)
- blurStrength: 40
  $name: Background blur strength (0 = off, 100 = max)
- opacity: 75
  $name: Background opacity (%)
- tintColor: ""
  $name: Custom color override, e.g. #202020
  $description: Leave empty to copy the live taskbar color
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <objidl.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <gdiplus.h>

#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace Gdiplus;


// small helpers


template <class T>
struct CP {
    T* p = nullptr;
    CP() {}
    ~CP() { if (p) p->Release(); }
    T** operator&() { return &p; }
    T* operator->() { return p; }
    explicit operator bool() const { return p != nullptr; }
    CP(const CP&) = delete;
    CP& operator=(const CP&) = delete;
};

static int Clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
static int Mini(int a, int b) { return a < b ? a : b; }
static int Maxi(int a, int b) { return a > b ? a : b; }

static double g_scale = 1.0;
static int S(double v) { return (int)(v * g_scale + 0.5); }


// settings / globals


struct Cfg {
    std::wstring folder;
    int iconSize = 36;
    bool autohide = true;
    bool showBar = true;
    int barPct = 40;
    int showMs = 150;
    int hideMs = 600;
    int margin = 10;
    int radius = 12;
    int blur = 40;
    int opacity = 75;
    std::wstring tint;
} g_cfg;

static float ItemRadius() { float r = S(g_cfg.radius) * 0.6f; return r < 3.0f ? 3.0f : r; }

enum State { ST_HIDDEN, ST_DOCK, ST_EXPANDED };

struct AppItem {
    std::wstring name;
    std::wstring lnk;                 // pinned item: shortcut file path
    std::shared_ptr<ITEMIDLIST> pidl; // all-apps item: absolute PIDL
    std::wstring key;                 // icon cache key
};

struct IconEntry {
    std::vector<BYTE> pix;
    std::unique_ptr<Bitmap> bmp;
    std::unique_ptr<Bitmap> scaled;  // pre-scaled copy drawn 1:1 (crisp + fast)
    int scaledSize = 0;
    bool tried = false;
};

struct RectD { double x = 0, y = 0, w = 0, h = 0; };

static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;
static volatile bool g_quit = false;

static HWND g_hwnd = nullptr;
static HWND g_bar = nullptr;
static const UINT WM_APP_SETTINGS = WM_APP + 1;

static State g_state = ST_HIDDEN;
static RectD g_cur, g_tgt;
static bool g_first = true;
static bool g_visible = false;
static bool g_barShown = false;
static bool g_dirty = true;
static bool g_barDirty = true;
static bool g_tracking = false;
static bool g_menuOpen = false;
static bool g_dropHover = false;
static bool g_dropOK = false;
static bool g_viewExpanded = false;

static DWORD g_lastInside = 0;
static DWORD g_edgeSince = 0;
static DWORD g_lastSlow = 0;
static DWORD g_lastBlink = 0;
static DWORD g_allLoaded = 0;

static RECT g_mon = {0, 0, 1920, 1080};

static int g_hover = -1;
static std::wstring g_query;
static int g_scroll = 0;

static BYTE g_r = 32, g_g = 32, g_b = 32;
static bool g_dark = true;
static Color g_accent(255, 0, 120, 215);

static int g_dockW = 60, g_dockH = 200, g_centerW = 900, g_centerH = 600;

static std::unique_ptr<Bitmap> g_bgBmp;
static std::vector<BYTE> g_bgPix;
static const int g_bgScale = 4;
static bool g_affinityOK = false;
static bool g_wasActive = false;
static DWORD g_lastCap = 0;
static HHOOK g_kbHook = nullptr;

static std::vector<AppItem> g_pinned;
static std::vector<AppItem> g_all;
static std::vector<int> g_filtered;
static std::wstring g_pinnedSig;
static std::map<std::wstring, std::unique_ptr<IconEntry>> g_icons;

static FontFamily* g_ff = nullptr;
static std::map<int, Font*> g_fonts;

static void LoadSettings() {
    PCWSTR f = Wh_GetStringSetting(L"folder");
    std::wstring folder = f ? f : L"";
    Wh_FreeStringSetting(f);
    if (folder.empty())
        folder = L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch";
    WCHAR ex[MAX_PATH];
    ExpandEnvironmentStringsW(folder.c_str(), ex, MAX_PATH);
    g_cfg.folder = ex;

    g_cfg.iconSize = Clampi(Wh_GetIntSetting(L"iconSize"), 16, 96);
    g_cfg.autohide = Wh_GetIntSetting(L"autohide") != 0;
    g_cfg.showBar = Wh_GetIntSetting(L"showActivationBar") != 0;
    g_cfg.barPct = Clampi(Wh_GetIntSetting(L"barHeightPercent"), 5, 100);
    g_cfg.showMs = Clampi(Wh_GetIntSetting(L"showDelayMs"), 0, 3000);
    g_cfg.hideMs = Clampi(Wh_GetIntSetting(L"hideDelayMs"), 0, 5000);
    g_cfg.margin = Clampi(Wh_GetIntSetting(L"edgeMargin"), 0, 100);
    g_cfg.radius = Clampi(Wh_GetIntSetting(L"cornerRadius"), 0, 48);
    g_cfg.blur = Clampi(Wh_GetIntSetting(L"blurStrength"), 0, 100);
    g_cfg.opacity = Clampi(Wh_GetIntSetting(L"opacity"), 5, 100);
    PCWSTR tc = Wh_GetStringSetting(L"tintColor");
    g_cfg.tint = tc ? tc : L"";
    Wh_FreeStringSetting(tc);
}


// colors (sampled from the real taskbar)


static Color BG(BYTE a) { return Color(a, g_r, g_g, g_b); }
static Color FG(BYTE a) { return g_dark ? Color(a, 255, 255, 255) : Color(a, 0, 0, 0); }

static void SetBgColor(BYTE r, BYTE g, BYTE b) {
    if (r == g_r && g == g_g && b == g_b) return;
    g_r = r; g_g = g; g_b = b;
    double lum = 0.299 * r + 0.587 * g + 0.114 * b;
    g_dark = lum < 140;
    g_dirty = true;
    g_barDirty = true;
}

static bool ParseTint(const std::wstring& t, int& r, int& g, int& b) {
    std::wstring h = t;
    if (!h.empty() && h[0] == L'#') h = h.substr(1);
    if (h.size() != 6) return false;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(h.c_str(), &end, 16);
    if (!end || *end) return false;
    r = (v >> 16) & 0xFF; g = (v >> 8) & 0xFF; b = v & 0xFF;
    return true;
}

static int MedianOf(std::vector<int>& v) {
    for (size_t i = 1; i < v.size(); i++)
        for (size_t j = i; j > 0 && v[j] < v[j - 1]; j--) std::swap(v[j], v[j - 1]);
    return v[v.size() / 2];
}

static void SampleTaskbarColor() {
    bool ok = false;
    int cr, cg, cb;
    if (ParseTint(g_cfg.tint, cr, cg, cb)) {
        SetBgColor((BYTE)cr, (BYTE)cg, (BYTE)cb);
        ok = true;
    }
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!ok && tray) {
        RECT rc;
        if (GetWindowRect(tray, &rc) && rc.right > rc.left) {
            HDC dc = GetDC(nullptr);
            if (dc) {
                // median of several points along the left edge band of the taskbar
                std::vector<int> rs, gs, bs;
                int h = rc.bottom - rc.top;
                for (int i = 1; i <= 4; i++) {
                    for (int dx = 2; dx <= 8; dx += 6) {
                        COLORREF c = GetPixel(dc, rc.left + dx, rc.top + h * i / 5);
                        if (c != CLR_INVALID) {
                            rs.push_back(GetRValue(c));
                            gs.push_back(GetGValue(c));
                            bs.push_back(GetBValue(c));
                        }
                    }
                }
                if (!rs.empty()) {
                    SetBgColor((BYTE)MedianOf(rs), (BYTE)MedianOf(gs), (BYTE)MedianOf(bs));
                    ok = true;
                }
                ReleaseDC(nullptr, dc);
            }
        }
    }
    if (!ok) {
        DWORD light = 0, sz = sizeof(light);
        bool isLight = false;
        if (RegGetValueW(HKEY_CURRENT_USER,
                         L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                         L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &light,
                         &sz) == ERROR_SUCCESS)
            isLight = light != 0;
        if (isLight) SetBgColor(243, 243, 243); else SetBgColor(32, 32, 32);
    }

    DWORD col = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&col, &opaque)))
        g_accent = Color(255, (col >> 16) & 0xFF, (col >> 8) & 0xFF, col & 0xFF);
}


// icons


static IShellItem* ItemForApp(const AppItem& a) {
    IShellItem* item = nullptr;
    if (!a.lnk.empty()) {
        CP<IShellLinkW> sl;
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                       IID_PPV_ARGS(&sl)))) {
            CP<IPersistFile> pf;
            if (SUCCEEDED(sl->QueryInterface(IID_PPV_ARGS(&pf))) &&
                SUCCEEDED(pf->Load(a.lnk.c_str(), STGM_READ))) {
                LPITEMIDLIST pidl = nullptr;
                if (SUCCEEDED(sl->GetIDList(&pidl)) && pidl) {
                    SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item));
                    CoTaskMemFree(pidl);
                }
                if (!item) {
                    WCHAR p[MAX_PATH] = {};
                    if (SUCCEEDED(sl->GetPath(p, MAX_PATH, nullptr, 0)) && p[0])
                        SHCreateItemFromParsingName(p, nullptr, IID_PPV_ARGS(&item));
                }
            }
        }
        if (!item) SHCreateItemFromParsingName(a.lnk.c_str(), nullptr, IID_PPV_ARGS(&item));
    } else if (a.pidl) {
        SHCreateItemFromIDList(a.pidl.get(), IID_PPV_ARGS(&item));
    }
    return item;
}

static int g_iconBudget = 6;

static Bitmap* GetIcon(const AppItem& a) {
    auto& slot = g_icons[a.key];
    if (!slot) slot.reset(new IconEntry);
    IconEntry& e = *slot;
    if (e.tried) return e.bmp.get();
    if (g_iconBudget <= 0) {  // spread icon loading over several frames to avoid stutter
        g_dirty = true;
        return nullptr;
    }
    g_iconBudget--;
    e.tried = true;

    IShellItem* item = ItemForApp(a);
    if (!item) return nullptr;
    CP<IShellItemImageFactory> fac;
    HRESULT hr = item->QueryInterface(IID_PPV_ARGS(&fac));
    item->Release();
    if (FAILED(hr)) return nullptr;

    SIZE sz = {128, 128};
    HBITMAP hbm = nullptr;
    if (FAILED(fac->GetImage(sz, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &hbm)) || !hbm)
        return nullptr;

    BITMAP bm = {};
    GetObjectW(hbm, sizeof(bm), &bm);
    if (bm.bmWidth <= 0 || bm.bmHeight <= 0) {
        DeleteObject(hbm);
        return nullptr;
    }
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = bm.bmWidth;
    bi.bmiHeader.biHeight = -bm.bmHeight;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    e.pix.assign((size_t)bm.bmWidth * bm.bmHeight * 4, 0);
    HDC dc = GetDC(nullptr);
    GetDIBits(dc, hbm, 0, bm.bmHeight, e.pix.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    DeleteObject(hbm);

    e.bmp.reset(new Bitmap(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4, PixelFormat32bppPARGB,
                           e.pix.data()));
    return e.bmp.get();
}


// pinned folder + all apps


static std::wstring StripExt(const std::wstring& n) {
    size_t d = n.find_last_of(L'.');
    return d == std::wstring::npos ? n : n.substr(0, d);
}

static std::wstring LowerExt(const std::wstring& n) {
    size_t d = n.find_last_of(L'.');
    if (d == std::wstring::npos) return L"";
    std::wstring e = n.substr(d);
    for (auto& c : e) c = (wchar_t)towlower(c);
    return e;
}

static std::vector<AppItem> ScanPinned(std::wstring& sig) {
    struct Raw { AppItem it; FILETIME ct; };
    std::vector<Raw> raw;
    sig.clear();

    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((g_cfg.folder + L"\\*").c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN)) continue;
            if (_wcsicmp(fd.cFileName, L"desktop.ini") == 0) continue;
            Raw r;
            r.it.name = StripExt(fd.cFileName);
            r.it.lnk = g_cfg.folder + L"\\" + fd.cFileName;
            r.it.key = r.it.lnk;
            r.ct = fd.ftCreationTime;
            raw.push_back(r);
            sig += fd.cFileName;
            sig += std::to_wstring(fd.ftLastWriteTime.dwLowDateTime);
            sig += L'|';
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    // oldest first so newly pinned apps land at the end
    for (size_t i = 1; i < raw.size(); i++)
        for (size_t j = i; j > 0 && CompareFileTime(&raw[j].ct, &raw[j - 1].ct) < 0; j--)
            std::swap(raw[j], raw[j - 1]);

    std::vector<AppItem> out;
    for (auto& r : raw) out.push_back(r.it);
    return out;
}

static bool ContainsCI(const std::wstring& h, const std::wstring& n) {
    if (n.empty()) return true;
    if (n.size() > h.size()) return false;
    for (size_t i = 0; i + n.size() <= h.size(); i++) {
        size_t k = 0;
        while (k < n.size() && towlower(h[i + k]) == towlower(n[k])) k++;
        if (k == n.size()) return true;
    }
    return false;
}

static void RebuildFiltered() {
    g_filtered.clear();
    for (size_t i = 0; i < g_all.size(); i++)
        if (ContainsCI(g_all[i].name, g_query)) g_filtered.push_back((int)i);
}

static void LoadAllApps() {
    g_all.clear();
    CP<IShellItem> folder;
    if (FAILED(SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_PPV_ARGS(&folder))))
        return;
    CP<IEnumShellItems> en;
    if (FAILED(folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&en)))) return;

    IShellItem* it = nullptr;
    ULONG fetched = 0;
    while (en->Next(1, &it, &fetched) == S_OK && fetched == 1) {
        PWSTR nm = nullptr;
        if (SUCCEEDED(it->GetDisplayName(SIGDN_NORMALDISPLAY, &nm)) && nm) {
            LPITEMIDLIST pidl = nullptr;
            if (SUCCEEDED(SHGetIDListFromObject(it, (PIDLIST_ABSOLUTE*)&pidl)) && pidl) {
                AppItem a;
                a.name = nm;
                a.pidl = std::shared_ptr<ITEMIDLIST>(pidl, [](ITEMIDLIST* p) { CoTaskMemFree(p); });
                a.key = L"app:" + a.name;
                g_all.push_back(a);
            }
            CoTaskMemFree(nm);
        }
        it->Release();
        it = nullptr;
    }
    for (size_t i = 1; i < g_all.size(); i++)
        for (size_t j = i; j > 0 && _wcsicmp(g_all[j].name.c_str(), g_all[j - 1].name.c_str()) < 0; j--)
            std::swap(g_all[j], g_all[j - 1]);
    g_allLoaded = GetTickCount();
}


// launching / pinning


static void LaunchItem(const AppItem& a) {
    if (!a.lnk.empty()) {
        ShellExecuteW(nullptr, L"open", a.lnk.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    } else if (a.pidl) {
        SHELLEXECUTEINFOW sei = {};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_IDLIST | SEE_MASK_FLAG_NO_UI;
        sei.lpIDList = (void*)a.pidl.get();
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
    }
}

static std::wstring UniquePath(std::wstring name, const std::wstring& ext) {
    for (auto& c : name)
        if (wcschr(L"\\/:*?\"<>|", c)) c = L'_';
    SHCreateDirectoryExW(nullptr, g_cfg.folder.c_str(), nullptr);
    std::wstring p = g_cfg.folder + L"\\" + name + ext;
    for (int i = 2; PathFileExistsW(p.c_str()) && i < 1000; i++)
        p = g_cfg.folder + L"\\" + name + L" (" + std::to_wstring(i) + L")" + ext;
    return p;
}

static bool CreateShortcutFile(const std::wstring& name, LPCITEMIDLIST pidl, const wchar_t* path) {
    CP<IShellLinkW> sl;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&sl))))
        return false;
    if (pidl) {
        sl->SetIDList(pidl);
    } else {
        sl->SetPath(path);
        WCHAR dir[MAX_PATH];
        wcsncpy(dir, path, MAX_PATH - 1);
        dir[MAX_PATH - 1] = 0;
        PathRemoveFileSpecW(dir);
        sl->SetWorkingDirectory(dir);
    }
    CP<IPersistFile> pf;
    if (FAILED(sl->QueryInterface(IID_PPV_ARGS(&pf)))) return false;
    return SUCCEEDED(pf->Save(UniquePath(name, L".lnk").c_str(), TRUE));
}

static void PinPath(const std::wstring& path) {
    const WCHAR* fn = PathFindFileNameW(path.c_str());
    std::wstring ext = LowerExt(fn);
    if (ext == L".lnk" || ext == L".url" || ext == L".appref-ms") {
        CopyFileW(path.c_str(), UniquePath(StripExt(fn), ext).c_str(), FALSE);
    } else {
        CreateShortcutFile(StripExt(fn), nullptr, path.c_str());
    }
}

static void PinPidl(LPCITEMIDLIST pidl) {
    WCHAR p[MAX_PATH] = {};
    if (SHGetPathFromIDListW(pidl, p) && p[0] && PathFileExistsW(p)) {
        PinPath(p);
        return;
    }
    PWSTR nm = nullptr;
    std::wstring name = L"App";
    if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_NORMALDISPLAY, &nm)) && nm) {
        name = nm;
        CoTaskMemFree(nm);
    }
    CreateShortcutFile(name, pidl, nullptr);
}

static bool Droppable(IDataObject* d) {
    static UINT cfIds = RegisterClipboardFormatW(CFSTR_SHELLIDLIST);
    FORMATETC f1 = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    FORMATETC f2 = {(CLIPFORMAT)cfIds, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    return d->QueryGetData(&f1) == S_OK || d->QueryGetData(&f2) == S_OK;
}

static void PinDataObject(IDataObject* d) {
    FORMATETC f = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM sm = {};
    if (SUCCEEDED(d->GetData(&f, &sm))) {
        HDROP hd = (HDROP)GlobalLock(sm.hGlobal);
        if (hd) {
            UINT n = DragQueryFileW(hd, 0xFFFFFFFF, nullptr, 0);
            for (UINT i = 0; i < n; i++) {
                WCHAR p[MAX_PATH];
                if (DragQueryFileW(hd, i, p, MAX_PATH)) PinPath(p);
            }
            GlobalUnlock(sm.hGlobal);
        }
        ReleaseStgMedium(&sm);
    } else {
        static UINT cfIds = RegisterClipboardFormatW(CFSTR_SHELLIDLIST);
        FORMATETC f2 = {(CLIPFORMAT)cfIds, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        STGMEDIUM sm2 = {};
        if (SUCCEEDED(d->GetData(&f2, &sm2))) {
            CIDA* c = (CIDA*)GlobalLock(sm2.hGlobal);
            if (c) {
                LPCITEMIDLIST parent = (LPCITEMIDLIST)((BYTE*)c + c->aoffset[0]);
                for (UINT i = 0; i < c->cidl; i++) {
                    LPCITEMIDLIST child = (LPCITEMIDLIST)((BYTE*)c + c->aoffset[i + 1]);
                    LPITEMIDLIST abs = ILCombine((PCIDLIST_ABSOLUTE)parent, (PCUIDLIST_RELATIVE)child);
                    if (abs) {
                        PinPidl(abs);
                        ILFree(abs);
                    }
                }
                GlobalUnlock(sm2.hGlobal);
            }
            ReleaseStgMedium(&sm2);
        }
    }
    g_pinnedSig.clear();  // force rescan
    g_lastSlow = 0;
}

static int FindPinnedByName(const std::wstring& name) {
    for (size_t i = 0; i < g_pinned.size(); i++)
        if (_wcsicmp(g_pinned[i].name.c_str(), name.c_str()) == 0) return (int)i;
    return -1;
}


// drawing primitives


static Font* GetFont(int px, bool bold) {
    int key = px * 2 + (bold ? 1 : 0);
    auto it = g_fonts.find(key);
    if (it != g_fonts.end()) return it->second;
    Font* f = new Font(g_ff, (REAL)px, bold ? FontStyleBold : FontStyleRegular, UnitPixel);
    g_fonts[key] = f;
    return f;
}

static void RoundPath(GraphicsPath& p, RectF r, float rad) {
    float m = (r.Width < r.Height ? r.Width : r.Height) / 2;
    if (rad > m) rad = m;
    float d = rad * 2;
    if (d <= 0) { p.AddRectangle(r); return; }
    p.AddArc(r.X, r.Y, d, d, 180, 90);
    p.AddArc(r.X + r.Width - d, r.Y, d, d, 270, 90);
    p.AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0, 90);
    p.AddArc(r.X, r.Y + r.Height - d, d, d, 90, 90);
    p.CloseFigure();
}

static void FillRound(Graphics& g, RectF r, float rad, Color c) {
    GraphicsPath p;
    RoundPath(p, r, rad);
    SolidBrush b(c);
    g.FillPath(&b, &p);
}

static void StrokeRound(Graphics& g, RectF r, float rad, Color c, float w = 1.0f) {
    GraphicsPath p;
    RoundPath(p, r, rad);
    Pen pen(c, w);
    g.DrawPath(&pen, &p);
}

static void Txt(Graphics& g, const std::wstring& s, RectF r, int px, Color c,
                StringAlignment ha = StringAlignmentCenter,
                StringAlignment va = StringAlignmentCenter, bool wrap = false, bool bold = false) {
    StringFormat sf;
    sf.SetAlignment(ha);
    sf.SetLineAlignment(va);
    sf.SetTrimming(wrap ? StringTrimmingEllipsisWord : StringTrimmingEllipsisCharacter);
    if (!wrap) sf.SetFormatFlags(StringFormatFlagsNoWrap);
    SolidBrush b(c);
    g.DrawString(s.c_str(), -1, GetFont(px, bold), r, &sf, &b);
}

static void DrawIcon(Graphics& g, const AppItem& a, RectF r) {
    int sz = (int)(r.Width + 0.5f);
    Bitmap* bmp = GetIcon(a);
    if (bmp && sz > 0) {
        IconEntry& e = *g_icons[a.key];
        if (!e.scaled || e.scaledSize != sz) {
            e.scaled.reset(new Bitmap(sz, sz, PixelFormat32bppPARGB));
            Graphics gg(e.scaled.get());
            gg.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            gg.SetPixelOffsetMode(PixelOffsetModeHalf);
            gg.SetCompositingMode(CompositingModeSourceCopy);
            ImageAttributes ia;
            ia.SetWrapMode(WrapModeTileFlipXY);
            gg.DrawImage(bmp, Rect(0, 0, sz, sz), 0, 0, (INT)bmp->GetWidth(), (INT)bmp->GetHeight(),
                         UnitPixel, &ia);
            e.scaledSize = sz;
        }
        g.DrawImage(e.scaled.get(), (INT)std::lround(r.X), (INT)std::lround(r.Y), sz, sz);
    } else {
        FillRound(g, r, 6, FG(40));
    }
}


// layout


static void RefreshMonitor() {
    HMONITOR m = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoW(m, &mi)) {
        if (memcmp(&mi.rcMonitor, &g_mon, sizeof(RECT)) != 0) {
            g_mon = mi.rcMonitor;
            g_barDirty = true;
            g_dirty = true;
        }
    }
}

static int DockCell() { return S(g_cfg.iconSize) + S(14); }
static int DockPad() { return S(8); }
static int DockSep() { return S(10); }

static bool DockHasPlaceholder() { return g_pinned.empty() || g_dropHover; }

static RectF DockRect(int idx) {
    float cell = (float)DockCell(), pad = (float)DockPad();
    float y = pad + (idx == 0 ? 0 : cell + DockSep() + (idx - 1) * cell);
    return RectF(pad, y, cell, cell);
}

static void ComputeTarget() {
    int monW = g_mon.right - g_mon.left, monH = g_mon.bottom - g_mon.top;
    int cell = DockCell(), pad = DockPad();
    int cells = 1 + (int)g_pinned.size() + (DockHasPlaceholder() ? 1 : 0);
    g_dockW = cell + pad * 2;
    g_dockH = pad * 2 + DockSep() + cell * cells;
    int maxH = monH - S(80);
    if (g_dockH > maxH) g_dockH = maxH;

    g_centerW = Mini(S(1000), monW - S(200));
    g_centerH = Mini(S(680), monH - S(140));

    int dockY = g_mon.top + (monH - g_dockH) / 2;
    if (g_state == ST_HIDDEN) {
        g_tgt = {(double)g_mon.right - 1, (double)dockY, 1, (double)g_dockH};
    } else if (g_state == ST_DOCK) {
        g_tgt = {(double)(g_mon.right - g_cfg.margin - g_dockW), (double)dockY, (double)g_dockW,
                 (double)g_dockH};
    } else {
        g_tgt = {(double)(g_mon.left + (monW - g_centerW) / 2),
                 (double)(g_mon.top + (monH - g_centerH) / 2), (double)g_centerW, (double)g_centerH};
    }
}

struct ExpLayout {
    float pad, searchH, cellW, cellH, gridTop, gridH, gridX;
    int cols;
};

static ExpLayout GetExp(int W, int H) {
    ExpLayout L;
    L.pad = (float)S(24);
    L.searchH = (float)S(40);
    L.cellW = (float)S(112);
    L.cellH = (float)S(108);
    L.gridTop = L.pad + L.searchH + S(16);
    L.gridH = H - L.gridTop - L.pad / 2;
    L.cols = Maxi(1, (int)((W - 2 * L.pad) / L.cellW));
    L.gridX = (W - L.cols * L.cellW) / 2;
    return L;
}

static int MaxScroll(const ExpLayout& L) {
    int rows = ((int)g_filtered.size() + L.cols - 1) / L.cols;
    int content = (int)(rows * L.cellH);
    return Maxi(0, content - (int)L.gridH);
}


// rendering


static void Present(HWND hwnd, Bitmap& bmp, int x, int y) {
    int W = (int)bmp.GetWidth(), H = (int)bmp.GetHeight();
    HDC scr = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(scr);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = W;
    bi.bmiHeader.biHeight = -H;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (dib && bits) {
        HGDIOBJ old = SelectObject(mem, dib);
        BitmapData bd;
        Rect rc(0, 0, W, H);
        if (bmp.LockBits(&rc, ImageLockModeRead, PixelFormat32bppPARGB, &bd) == Ok) {
            for (int row = 0; row < H; row++)
                memcpy((BYTE*)bits + (size_t)row * W * 4, (BYTE*)bd.Scan0 + (size_t)row * bd.Stride,
                       (size_t)W * 4);
            bmp.UnlockBits(&bd);
        }
        POINT dst = {x, y};
        SIZE sz = {W, H};
        POINT src = {0, 0};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, scr, &dst, &sz, mem, &src, 0, &bf, ULW_ALPHA);
        SelectObject(mem, old);
    }
    if (dib) DeleteObject(dib);
    DeleteDC(mem);
    ReleaseDC(nullptr, scr);
}

static void BoxBlurPass(const BYTE* src, BYTE* dst, int w, int h, int r, bool horiz) {
    int len = horiz ? w : h, lines = horiz ? h : w;
    int sa = horiz ? 4 : w * 4;
    int sl = horiz ? w * 4 : 4;
    int win = 2 * r + 1;
    for (int l = 0; l < lines; l++) {
        const BYTE* s = src + (size_t)l * sl;
        BYTE* d = dst + (size_t)l * sl;
        for (int c = 0; c < 3; c++) {
            int sum = 0;
            for (int i = -r; i <= r; i++) sum += s[(size_t)Clampi(i, 0, len - 1) * sa + c];
            for (int i = 0; i < len; i++) {
                d[(size_t)i * sa + c] = (BYTE)(sum / win);
                sum += s[(size_t)Clampi(i + r + 1, 0, len - 1) * sa + c] -
                       s[(size_t)Clampi(i - r, 0, len - 1) * sa + c];
            }
        }
        for (int i = 0; i < len; i++) d[(size_t)i * sa + 3] = 255;
    }
}

// Grabs a downscaled copy of the whole monitor and blurs it.
static void CaptureBackdrop() {
    g_lastCap = GetTickCount();
    if (g_cfg.blur <= 0) {
        g_bgBmp.reset();
        return;
    }
    int monW = g_mon.right - g_mon.left, monH = g_mon.bottom - g_mon.top;
    int w = Maxi(1, monW / g_bgScale), h = Maxi(1, monH / g_bgScale);

    HDC scr = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(scr);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (dib && bits) {
        HGDIOBJ old = SelectObject(mem, dib);
        SetStretchBltMode(mem, COLORONCOLOR);
        SetBrushOrgEx(mem, 0, 0, nullptr);
        StretchBlt(mem, 0, 0, w, h, scr, g_mon.left, g_mon.top, monW, monH, SRCCOPY | CAPTUREBLT);
        GdiFlush();
        g_bgBmp.reset();
        g_bgPix.assign((BYTE*)bits, (BYTE*)bits + (size_t)w * h * 4);
        SelectObject(mem, old);

        int r = Maxi(1, (int)(g_cfg.blur * 0.16 * g_scale + 0.5));
        std::vector<BYTE> tmp(g_bgPix.size());
        for (int pass = 0; pass < 3; pass++) {
            BoxBlurPass(g_bgPix.data(), tmp.data(), w, h, r, true);
            BoxBlurPass(tmp.data(), g_bgPix.data(), w, h, r, false);
        }
        g_bgBmp.reset(new Bitmap(w, h, w * 4, PixelFormat32bppRGB, g_bgPix.data()));
    }
    if (dib) DeleteObject(dib);
    DeleteDC(mem);
    ReleaseDC(nullptr, scr);
}

static void DrawDock(Graphics& g, int W, int H) {
    float tx = W < g_dockW ? (float)(W - g_dockW) : 0.0f;
    g.TranslateTransform(tx, 0);

    int n = (int)g_pinned.size();
    for (int idx = 0; idx <= n + 1; idx++) {
        bool placeholder = idx == n + 1;
        if (placeholder && !DockHasPlaceholder()) break;
        RectF r = DockRect(idx);
        if (r.Y + r.Height > H) break;

        if (g_hover == idx && !placeholder) FillRound(g, r, ItemRadius(), FG(30));

        if (idx == 0) {
            float sq = (float)S(5), gap = (float)S(3);
            float total = 3 * sq + 2 * gap;
            float ox = r.X + (r.Width - total) / 2, oy = r.Y + (r.Height - total) / 2;
            for (int a = 0; a < 3; a++)
                for (int b = 0; b < 3; b++)
                    FillRound(g, RectF(ox + b * (sq + gap), oy + a * (sq + gap), sq, sq), 1.5f,
                              FG(220));
            float sy = r.Y + r.Height + DockSep() / 2.0f;
            Pen p(FG(50), 1.0f);
            g.DrawLine(&p, r.X + S(6), sy, r.X + r.Width - S(6), sy);
        } else if (placeholder) {
            Color c = g_dropHover ? g_accent : FG(90);
            StrokeRound(g, RectF(r.X + 3, r.Y + 3, r.Width - 6, r.Height - 6), (float)S(6), c, 1.5f);
            Txt(g, L"+", r, S(g_cfg.iconSize * 0.6), c);
        } else {
            float is = (float)S(g_cfg.iconSize);
            DrawIcon(g, g_pinned[idx - 1],
                     RectF(r.X + (r.Width - is) / 2, r.Y + (r.Height - is) / 2, is, is));
        }
    }
    g.ResetTransform();
}

static void DrawExpanded(Graphics& g, int W, int H) {
    ExpLayout L = GetExp(W, H);

    RectF sb(L.pad, L.pad, W - 2 * L.pad, L.searchH);
    FillRound(g, sb, ItemRadius(), FG(22));
    StrokeRound(g, sb, ItemRadius(), FG(45));
    RectF st(sb.X + S(14), sb.Y, sb.Width - S(28), sb.Height);
    if (g_query.empty()) {
        Txt(g, L"Type to search all apps", st, S(15), FG(130), StringAlignmentNear);
    } else {
        std::wstring q = g_query;
        if ((GetTickCount() / 500) % 2 == 0) q += L"|";
        Txt(g, q, st, S(15), FG(240), StringAlignmentNear);
    }

    g.SetClip(RectF(0, L.gridTop, (REAL)W, L.gridH));
    int maxScroll = MaxScroll(L);
    if (g_scroll > maxScroll) g_scroll = maxScroll;
    if (g_scroll < 0) g_scroll = 0;

    for (size_t i = 0; i < g_filtered.size(); i++) {
        int row = (int)i / L.cols, col = (int)i % L.cols;
        float x = L.gridX + col * L.cellW;
        float y = L.gridTop + row * L.cellH - g_scroll;
        if (y + L.cellH < L.gridTop || y > L.gridTop + L.gridH) continue;
        const AppItem& a = g_all[g_filtered[i]];

        if (g_hover == (int)i)
            FillRound(g, RectF(x + 3, y + 3, L.cellW - 6, L.cellH - 6), ItemRadius(), FG(30));

        float is = (float)S(44);
        DrawIcon(g, a, RectF(x + (L.cellW - is) / 2, y + S(12), is, is));
        Txt(g, a.name, RectF(x + S(6), y + S(12) + is + S(6), L.cellW - S(12), (REAL)S(34)), S(12),
            FG(230), StringAlignmentCenter, StringAlignmentNear, true);
    }
    if (g_filtered.empty())
        Txt(g, g_all.empty() ? L"Loading apps..." : L"No apps found",
            RectF(0, L.gridTop, (REAL)W, (REAL)S(60)), S(14), FG(130));
    g.ResetClip();
}

static void Render() {
    int W = (int)std::lround(g_cur.w), H = (int)std::lround(g_cur.h);
    if (W < 1 || H < 1) return;
    g_iconBudget = 6;

    Bitmap bmp(W, H, PixelFormat32bppPARGB);
    {
        Graphics g(&bmp);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintAntiAlias);
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(PixelOffsetModeHalf);

        RectF full(0.5f, 0.5f, (REAL)(W - 1), (REAL)(H - 1));
        float R = (float)S(g_cfg.radius);

        if (g_cfg.blur > 0 && g_bgBmp) {
            // A texture brush keeps the rounded edge anti-aliased (a clip region would be jagged).
            TextureBrush tb(g_bgBmp.get(), WrapModeClamp);
            float d = (float)g_bgScale;
            Matrix mx(d, 0, 0, d, (REAL)(g_mon.left - g_cur.x), (REAL)(g_mon.top - g_cur.y));
            tb.SetTransform(&mx);
            GraphicsPath bp;
            RoundPath(bp, full, R);
            g.SetInterpolationMode(InterpolationModeBilinear);
            g.FillPath(&tb, &bp);
            g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        }
        FillRound(g, full, R, BG((BYTE)(g_cfg.opacity * 255 / 100)));
        StrokeRound(g, full, R, g_dropHover ? g_accent : FG(45), g_dropHover ? 2.0f : 1.0f);

        double t = (g_cur.w - g_dockW) / (double)(g_centerW - g_dockW);
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        if (t < 0.12) {
            g_viewExpanded = false;
            DrawDock(g, W, H);
        } else if (t > 0.88) {
            g_viewExpanded = true;
            DrawExpanded(g, W, H);
        }
    }
    Present(g_hwnd, bmp, (int)std::lround(g_cur.x), (int)std::lround(g_cur.y));
}

static void UpdateBar() {
    int monH = g_mon.bottom - g_mon.top;
    int bw = S(5);
    int bh = Maxi(S(40), monH * g_cfg.barPct / 100);
    Bitmap bmp(bw, bh, PixelFormat32bppPARGB);
    {
        Graphics g(&bmp);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        RectF r(0.5f, 0.5f, (REAL)(bw + bw), (REAL)(bh - 1));
        FillRound(g, r, (float)bw, BG(240));
        StrokeRound(g, r, (float)bw, FG(80));
    }
    Present(g_bar, bmp, g_mon.right - bw, g_mon.top + (monH - bh) / 2);
}


// state machine


static void SetNoActivate(bool on) {
    LONG_PTR ex = GetWindowLongPtrW(g_hwnd, GWL_EXSTYLE);
    LONG_PTR n = on ? (ex | WS_EX_NOACTIVATE) : (ex & ~(LONG_PTR)WS_EX_NOACTIVATE);
    if (n != ex) {
        SetWindowLongPtrW(g_hwnd, GWL_EXSTYLE, n);
        SetWindowPos(g_hwnd, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}

static LRESULT CALLBACK KbProc(int code, WPARAM w, LPARAM l);

static void Expand() {
    if (g_all.empty()) LoadAllApps();
    g_query.clear();
    g_scroll = 0;
    g_hover = -1;
    RebuildFiltered();
    CaptureBackdrop();
    g_state = ST_EXPANDED;
    g_wasActive = false;
    SetNoActivate(false);

    // Make sure the window really gets keyboard focus.
    HWND fg = GetForegroundWindow();
    DWORD fgTid = fg ? GetWindowThreadProcessId(fg, nullptr) : 0;
    DWORD me = GetCurrentThreadId();
    if (fgTid && fgTid != me) AttachThreadInput(me, fgTid, TRUE);
    BringWindowToTop(g_hwnd);
    SetForegroundWindow(g_hwnd);
    SetFocus(g_hwnd);
    if (fgTid && fgTid != me) AttachThreadInput(me, fgTid, FALSE);

    // Fallback so typing always reaches the search box, even if focus was refused.
    if (!g_kbHook)
        g_kbHook = SetWindowsHookExW(WH_KEYBOARD_LL, KbProc, GetModuleHandleW(nullptr), 0);
    g_dirty = true;
}

static void Collapse(bool launched) {
    if (g_kbHook) {
        UnhookWindowsHookEx(g_kbHook);
        g_kbHook = nullptr;
    }
    g_query.clear();
    g_hover = -1;
    g_state = (g_cfg.autohide && launched) ? ST_HIDDEN : ST_DOCK;
    g_lastInside = GetTickCount();
    SetNoActivate(true);
    g_dirty = true;
}

static void HandleChar(wchar_t c) {
    if (g_state != ST_EXPANDED) return;
    if (c == 27) {
        if (g_query.empty()) { Collapse(false); return; }
        g_query.clear();
    } else if (c == 8) {
        if (!g_query.empty()) g_query.pop_back();
    } else if (c == 13) {
        if (!g_filtered.empty()) {
            LaunchItem(g_all[g_filtered[0]]);
            Collapse(true);
        }
        return;
    } else if (c >= 32 && c != 127) {
        g_query += c;
    } else {
        return;
    }
    RebuildFiltered();
    g_scroll = 0;
    g_hover = -1;
    g_dirty = true;
}

static LRESULT CALLBACK KbProc(int code, WPARAM w, LPARAM l) {
    if (code == HC_ACTION && w == WM_KEYDOWN && g_state == ST_EXPANDED && !g_menuOpen) {
        KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)l;
        bool mod = (GetAsyncKeyState(VK_CONTROL) & 0x8000) || (GetAsyncKeyState(VK_MENU) & 0x8000) ||
                   (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);
        if (!mod && !(k->flags & LLKHF_INJECTED)) {
            wchar_t ch = 0;
            if (k->vkCode == VK_ESCAPE) ch = 27;
            else if (k->vkCode == VK_BACK) ch = 8;
            else if (k->vkCode == VK_RETURN) ch = 13;
            else {
                BYTE st[256] = {};
                GetKeyboardState(st);
                st[VK_SHIFT] = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? 0x80 : 0;
                st[VK_CAPITAL] = (BYTE)(GetKeyState(VK_CAPITAL) & 1);
                wchar_t buf[8] = {};
                if (ToUnicode(k->vkCode, k->scanCode, st, buf, 8, 4) == 1 && buf[0] >= 32)
                    ch = buf[0];
            }
            if (ch) {
                HandleChar(ch);
                return 1;  // swallow so it doesn't also type into another app
            }
        }
    }
    return CallNextHookEx(nullptr, code, w, l);
}

static int HitDock(int x, int y) {
    float tx = g_cur.w < g_dockW ? (float)(g_cur.w - g_dockW) : 0.0f;
    float fx = x - tx, fy = (float)y;
    int n = (int)g_pinned.size();
    for (int idx = 0; idx <= n; idx++) {
        RectF r = DockRect(idx);
        if (fx >= r.X && fx < r.X + r.Width && fy >= r.Y && fy < r.Y + r.Height) return idx;
    }
    return -1;
}

static int HitExp(int x, int y) {
    ExpLayout L = GetExp((int)std::lround(g_cur.w), (int)std::lround(g_cur.h));
    if (y < L.gridTop || y >= L.gridTop + L.gridH) return -1;
    float gx = x - L.gridX;
    if (gx < 0 || gx >= L.cols * L.cellW) return -1;
    int col = (int)(gx / L.cellW);
    int row = (int)((y - L.gridTop + g_scroll) / L.cellH);
    int i = row * L.cols + col;
    return (i >= 0 && i < (int)g_filtered.size()) ? i : -1;
}

static int HitTest(int x, int y) {
    if (g_state == ST_EXPANDED && g_viewExpanded) return HitExp(x, y);
    if (g_state != ST_EXPANDED) return HitDock(x, y);
    return -1;
}

static void OnClick(int hit) {
    if (hit < 0) return;
    if (g_state == ST_EXPANDED) {
        LaunchItem(g_all[g_filtered[hit]]);
        Collapse(true);
    } else if (hit == 0) {
        Expand();
    } else if (hit - 1 < (int)g_pinned.size()) {
        LaunchItem(g_pinned[hit - 1]);
    }
}

enum { CMD_OPEN = 1, CMD_PIN, CMD_UNPIN, CMD_LOCATION };

static void OnContext(int hit) {
    if (hit < 0) return;
    if (g_state != ST_EXPANDED && hit == 0) return;

    const AppItem* item = nullptr;
    int pinnedIdx = -1;
    if (g_state == ST_EXPANDED) {
        item = &g_all[g_filtered[hit]];
        pinnedIdx = FindPinnedByName(item->name);
    } else {
        pinnedIdx = hit - 1;
        item = &g_pinned[pinnedIdx];
    }

    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, CMD_OPEN, L"Open");
    if (pinnedIdx >= 0) {
        AppendMenuW(m, MF_STRING, CMD_LOCATION, L"Open file location");
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING, CMD_UNPIN, L"Remove from dock");
    } else {
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(m, MF_STRING, CMD_PIN, L"Pin to dock");
    }

    POINT pt;
    GetCursorPos(&pt);
    g_menuOpen = true;
    SetForegroundWindow(g_hwnd);
    int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_hwnd, nullptr);
    PostMessageW(g_hwnd, WM_NULL, 0, 0);
    DestroyMenu(m);
    g_menuOpen = false;
    g_lastInside = GetTickCount();

    AppItem copy = *item;
    switch (cmd) {
        case CMD_OPEN:
            LaunchItem(copy);
            if (g_state == ST_EXPANDED) Collapse(true);
            break;
        case CMD_LOCATION:
            ShellExecuteW(nullptr, L"open", L"explorer.exe",
                          (L"/select,\"" + g_pinned[pinnedIdx].lnk + L"\"").c_str(), nullptr,
                          SW_SHOWNORMAL);
            break;
        case CMD_UNPIN:
            DeleteFileW(g_pinned[pinnedIdx].lnk.c_str());
            g_lastSlow = 0;
            g_pinnedSig.clear();
            break;
        case CMD_PIN:
            if (copy.pidl) CreateShortcutFile(copy.name, copy.pidl.get(), nullptr);
            g_lastSlow = 0;
            g_pinnedSig.clear();
            break;
    }
    g_dirty = true;
}

static void SlowUpdate() {
    SampleTaskbarColor();
    RefreshMonitor();
    std::wstring sig;
    auto list = ScanPinned(sig);
    if (sig != g_pinnedSig) {
        g_pinnedSig = sig;
        g_pinned = list;
        g_hover = -1;
        g_dirty = true;
    }
}

static void Tick() {
    DWORD now = GetTickCount();
    POINT pt;
    GetCursorPos(&pt);

    if (g_lastSlow == 0 || now - g_lastSlow > 2000) {
        g_lastSlow = now;
        SlowUpdate();
    }

    // Load the all-apps list ahead of time so opening the launchpad is instant.
    static DWORD s_start = now, s_lastLoad = 0;
    if (g_state != ST_EXPANDED && !g_menuOpen && !g_dropHover && now - s_start > 3000 &&
        (s_lastLoad == 0 || now - s_lastLoad > 120000)) {
        s_lastLoad = now;
        LoadAllApps();
    }

    int monH = g_mon.bottom - g_mon.top;
    int bh = Maxi(S(40), monH * g_cfg.barPct / 100);
    int barTop = g_mon.top + (monH - bh) / 2;
    bool inEdge = pt.x >= g_mon.right - 2 && pt.y >= barTop && pt.y <= barTop + bh;

    if (g_state == ST_HIDDEN) {
        if (!g_cfg.autohide) {
            g_state = ST_DOCK;
            g_dirty = true;
        } else if (inEdge) {
            if (!g_edgeSince) g_edgeSince = now;
            if (now - g_edgeSince >= (DWORD)g_cfg.showMs) {
                g_state = ST_DOCK;
                g_lastInside = now;
                g_edgeSince = 0;
                g_dirty = true;
            }
        } else {
            g_edgeSince = 0;
        }
    } else if (g_state == ST_DOCK) {
        if (g_cfg.autohide && !g_menuOpen && !g_dropHover) {
            RECT r = {(LONG)g_cur.x - S(12), (LONG)g_cur.y - S(12), g_mon.right,
                      (LONG)(g_cur.y + g_cur.h) + S(12)};
            if (inEdge || PtInRect(&r, pt)) g_lastInside = now;
            else if (now - g_lastInside > (DWORD)g_cfg.hideMs) {
                g_state = ST_HIDDEN;
                g_hover = -1;
                g_dirty = true;
            }
        }
    } else {
        if (now / 500 != g_lastBlink) {
            g_lastBlink = now / 500;
            g_dirty = true;
        }
        // click anywhere outside the panel closes it
        if (!g_menuOpen && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            RECT r = {(LONG)g_tgt.x, (LONG)g_tgt.y, (LONG)(g_tgt.x + g_tgt.w), (LONG)(g_tgt.y + g_tgt.h)};
            if (!PtInRect(&r, pt)) Collapse(false);
        }
    }

    ComputeTarget();
    if (g_first) {
        g_cur = g_tgt;
        g_first = false;
    }
    bool moved = false;
    auto step = [&](double& c, double t) {
        double d = t - c;
        if (fabs(d) < 0.6) {
            if (c != t) { c = t; moved = true; }
        } else {
            c += d * 0.28;
            moved = true;
        }
    };
    step(g_cur.x, g_tgt.x);
    step(g_cur.y, g_tgt.y);
    step(g_cur.w, g_tgt.w);
    step(g_cur.h, g_tgt.h);

    bool wantVisible = !(g_state == ST_HIDDEN && g_cur.w <= 3);
    if (wantVisible && !g_visible) {
        CaptureBackdrop();
        ShowWindow(g_hwnd, SW_SHOWNA);
        SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        g_visible = true;
        g_dirty = true;
    } else if (!wantVisible && g_visible) {
        ShowWindow(g_hwnd, SW_HIDE);
        g_visible = false;
    }

    bool wantBar = g_cfg.autohide && g_cfg.showBar && g_state == ST_HIDDEN;
    if (wantBar && !g_barShown) {
        UpdateBar();
        ShowWindow(g_bar, SW_SHOWNA);
        SetWindowPos(g_bar, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        g_barShown = true;
        g_barDirty = false;
    } else if (!wantBar && g_barShown) {
        ShowWindow(g_bar, SW_HIDE);
        g_barShown = false;
    } else if (g_barShown && g_barDirty) {
        UpdateBar();
        g_barDirty = false;
    }

    if (g_affinityOK && g_visible && !moved && g_cfg.blur > 0 && now - g_lastCap > 1500) {
        CaptureBackdrop();
        g_dirty = true;
    }

    if (g_visible && (moved || g_dirty)) {
        g_dirty = false;
        Render();
    }
}


// drag & drop target


class DropTgt : public IDropTarget {
    LONG m_ref = 1;

   public:
    STDMETHODIMP QueryInterface(REFIID iid, void** pp) override {
        if (iid == IID_IUnknown || iid == IID_IDropTarget) {
            *pp = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *pp = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_ref); }
    STDMETHODIMP_(ULONG) Release() override { return InterlockedDecrement(&m_ref); }

    STDMETHODIMP DragEnter(IDataObject* d, DWORD, POINTL, DWORD* e) override {
        g_dropOK = Droppable(d);
        g_dropHover = g_dropOK;
        g_lastInside = GetTickCount();
        g_dirty = true;
        *e = g_dropOK ? DROPEFFECT_LINK : DROPEFFECT_NONE;
        return S_OK;
    }
    STDMETHODIMP DragOver(DWORD, POINTL, DWORD* e) override {
        g_lastInside = GetTickCount();
        *e = g_dropOK ? DROPEFFECT_LINK : DROPEFFECT_NONE;
        return S_OK;
    }
    STDMETHODIMP DragLeave() override {
        g_dropHover = false;
        g_dirty = true;
        return S_OK;
    }
    STDMETHODIMP Drop(IDataObject* d, DWORD, POINTL, DWORD* e) override {
        g_dropHover = false;
        g_dirty = true;
        if (g_dropOK) PinDataObject(d);
        *e = DROPEFFECT_LINK;
        return S_OK;
    }
};
static DropTgt g_drop;


// window procedures + thread


static LRESULT CALLBACK BarProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_NCHITTEST) return HTTRANSPARENT;
    return DefWindowProcW(h, m, w, l);
}

static LRESULT CALLBACK DockProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_TIMER:
            Tick();
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return g_state == ST_EXPANDED ? MA_ACTIVATE : MA_NOACTIVATE;
        case WM_MOUSEMOVE: {
            int hv = HitTest(GET_X_LPARAM(l), GET_Y_LPARAM(l));
            if (hv != g_hover) {
                g_hover = hv;
                g_dirty = true;
            }
            if (!g_tracking) {
                TRACKMOUSEEVENT t = {sizeof(t), TME_LEAVE, h, 0};
                TrackMouseEvent(&t);
                g_tracking = true;
            }
            g_lastInside = GetTickCount();
            return 0;
        }
        case WM_MOUSELEAVE:
            g_tracking = false;
            g_hover = -1;
            g_dirty = true;
            return 0;
        case WM_LBUTTONUP:
            OnClick(HitTest(GET_X_LPARAM(l), GET_Y_LPARAM(l)));
            return 0;
        case WM_RBUTTONUP:
            OnContext(HitTest(GET_X_LPARAM(l), GET_Y_LPARAM(l)));
            return 0;
        case WM_MOUSEWHEEL:
            if (g_state == ST_EXPANDED) {
                g_scroll -= GET_WHEEL_DELTA_WPARAM(w) * S(60) / WHEEL_DELTA;
                if (g_scroll < 0) g_scroll = 0;
                g_hover = -1;
                g_dirty = true;
            }
            return 0;
        case WM_CHAR:
            HandleChar((wchar_t)w);
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(w) != WA_INACTIVE) g_wasActive = true;
            else if (g_wasActive && g_state == ST_EXPANDED && !g_menuOpen) Collapse(false);
            return 0;
        case WM_DISPLAYCHANGE:
            RefreshMonitor();
            return 0;
        case WM_APP_SETTINGS:
            g_pinnedSig.clear();
            g_lastSlow = 0;
            g_barDirty = true;
            g_dirty = true;
            CaptureBackdrop();
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static DWORD WINAPI UiThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // Only run inside the explorer.exe process that owns the taskbar.
    bool found = false;
    for (int i = 0; i < 120 && !g_quit; i++) {
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray) {
            DWORD pid = 0;
            GetWindowThreadProcessId(tray, &pid);
            found = pid == GetCurrentProcessId();
            break;
        }
        Sleep(500);
    }
    if (!found || g_quit) return 0;

    OleInitialize(nullptr);
    GdiplusStartupInput gsi;
    ULONG_PTR gdipToken = 0;
    GdiplusStartup(&gdipToken, &gsi, nullptr);
    g_ff = new FontFamily(L"Segoe UI");

    HDC sdc = GetDC(nullptr);
    g_scale = GetDeviceCaps(sdc, LOGPIXELSX) / 96.0;
    ReleaseDC(nullptr, sdc);

    HINSTANCE hi = GetModuleHandleW(nullptr);
    WNDCLASSW wc = {};
    wc.hInstance = hi;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpfnWndProc = DockProc;
    wc.lpszClassName = L"WH_QuickLaunchDock";
    RegisterClassW(&wc);
    wc.lpfnWndProc = BarProc;
    wc.lpszClassName = L"WH_QuickLaunchDockBar";
    RegisterClassW(&wc);

    DWORD ex = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    g_hwnd = CreateWindowExW(ex, L"WH_QuickLaunchDock", L"Quick Launch Dock", WS_POPUP, 0, 0, 10, 10,
                             nullptr, nullptr, hi, nullptr);
    g_bar = CreateWindowExW(ex, L"WH_QuickLaunchDockBar", L"", WS_POPUP, 0, 0, 10, 10, nullptr,
                            nullptr, hi, nullptr);

    if (g_hwnd && g_bar) {
        // Lets us re-capture the backdrop live without our own window showing up in it.
        g_affinityOK = SetWindowDisplayAffinity(g_hwnd, 0x00000011 /*WDA_EXCLUDEFROMCAPTURE*/) != 0;
        RegisterDragDrop(g_hwnd, &g_drop);
        RefreshMonitor();
        SampleTaskbarColor();
        SetTimer(g_hwnd, 1, 10, nullptr);

        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        KillTimer(g_hwnd, 1);
        RevokeDragDrop(g_hwnd);
    }

    if (g_hwnd) DestroyWindow(g_hwnd);
    if (g_bar) DestroyWindow(g_bar);
    g_hwnd = g_bar = nullptr;
    UnregisterClassW(L"WH_QuickLaunchDock", hi);
    UnregisterClassW(L"WH_QuickLaunchDockBar", hi);

    if (g_kbHook) {
        UnhookWindowsHookEx(g_kbHook);
        g_kbHook = nullptr;
    }
    g_bgBmp.reset();
    g_icons.clear();
    g_pinned.clear();
    g_all.clear();
    for (auto& kv : g_fonts) delete kv.second;
    g_fonts.clear();
    delete g_ff;
    g_ff = nullptr;
    GdiplusShutdown(gdipToken);
    OleUninitialize();
    return 0;
}


// Windhawk entry points


BOOL Wh_ModInit() {
    LoadSettings();
    g_quit = false;
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, &g_threadId);
    return g_thread != nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP_SETTINGS, 0, 0);
}

void Wh_ModUninit() {
    g_quit = true;
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}
