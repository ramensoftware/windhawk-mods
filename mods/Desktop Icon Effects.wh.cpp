// ==WindhawkMod==
// @id              desktop-icon-effects
// @name            Desktop Icon Effects
// @description     Black-white, colorize and transparency effects for desktop icons, plus hide names / shortcut arrow
// @version         1.3
// @author          NoMoreDg
// @github          https://github.com/NoMoreDg/Windhawk-Mods
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32 -lmsimg32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icon Effects
Just a simple effect mod for the desktop icons.

## Features
- Colorize the icon
- Black-White color space icon colors
- Make them transparent

## Source code
[View the source code on GitHub](https://github.com/NoMoreDg/Windhawk-Mods)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- HideNames: false
  $name: Hide icon names
  $description: Shows only the icons, without their text labels
- HideShortcutArrow: false
  $name: Hide shortcut arrow
  $description: Removes the small arrow at the bottom left of shortcut icons
- Grayscale: false
  $name: 1. Black-white colorspace
  $description: Turns icons black and white
- Colorize: false
  $name: 2. Colorizer
  $description: Tints icons with the color below (runs after black-white)
- ColorizeColor: "#0078D7"
  $name: Colorizer color (hex)
  $description: Only the hue of this color is used
- ColorizeSaturation: 100
  $name: Colorizer saturation (0-100)
- ColorizeContrast: 100
  $name: Colorizer contrast (0-200)
  $description: 100 = unchanged, lower = flatter, higher = more contrast
- Transparency: false
  $name: 3. Transparency
  $description: Makes icons see-through (runs last)
- TransparencyAmount: 50
  $name: Transparency amount (0-100)
  $description: 0 = fully visible, 100 = invisible
- RefreshDelay: s2
  $name: Delay before re-applying after resize / refresh
  $description: The effect pauses while you resize or refresh the desktop and comes back after this delay. Longer = smoother resizing.
  $options:
  - s05: 0.5 seconds
  - s1: 1 second
  - s2: 2 seconds
  - s3: 3 seconds
  - s5: 5 seconds
- WatchdogInterval: s10
  $name: Background re-attach check
  $description: How often the mod checks that it is still attached to the desktop. Longer = less performance impact.
  $options:
  - off: Off
  - s2: Every 2 seconds
  - s5: Every 5 seconds
  - s10: Every 10 seconds
  - s30: Every 30 seconds
  - s60: Every 60 seconds
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <vector>
#include <map>
#include <tuple>
#include <utility>
#include <algorithm>
#include <cmath>
#include <cwchar>

struct Settings {
    bool gray = false, colorize = false, transp = false;
    bool hideNames = false, hideArrow = false;
    float hue = 0, sat = 1, contrast = 1, opacity = 1;
    int settleMs = 2000;
    int watchMs = 10000;
    bool HasEffects() const { return gray || colorize || transp; }
    bool NeedsWork() const { return HasEffects() || hideArrow; }
};

static Settings g_s;
static CRITICAL_SECTION g_cs;
static UINT g_runMsg = 0;
static std::vector<HWND> g_tracked;
static HANDLE g_stopEvt = nullptr;
static HANDLE g_watchdog = nullptr;
static const UINT_PTR kSettleTimer = 0xD1E5;
static wchar_t g_emptyText[2] = L"";

struct Cached {
    HBITMAP bmp = nullptr;
    int w = 0, h = 0;
    std::vector<BYTE> alpha;   // original alpha, used to rebuild the selection highlight
};
typedef std::tuple<UINT_PTR, int, int, int, int> CacheKey;  // himl, image, overlay, cx, cy
static std::map<CacheKey, Cached> g_cache;

struct SubData {
    bool active = false;
    bool settling = false;
    bool sizeKnown = false;
    HWND lastList = nullptr;
    HIMAGELIST lastHiml = nullptr;
    int lastCx = 0, lastCy = 0;
    LRESULT lastView = -1;
    HWND classList = nullptr;
    bool isDesktop = false;

    HDC bgDC = nullptr;
    HBITMAP bgBmp = nullptr;
    HGDIOBJ bgOld = nullptr;
    void* bgBits = nullptr;
    int bgW = 0, bgH = 0;
    RECT rc = {};
    bool hasBg = false;
    bool hl = false;

    void FreeBg() { hasBg = false; hl = false; }
    void DestroyBg() {
        if (bgDC) { SelectObject(bgDC, bgOld); DeleteDC(bgDC); bgDC = nullptr; }
        if (bgBmp) { DeleteObject(bgBmp); bgBmp = nullptr; }
        bgBits = nullptr;
        bgW = bgH = 0;
        hasBg = false;
        hl = false;
    }
};

// ---------------------------------------------------------------- color math
static float Hue2Rgb(float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.f / 6) return p + (q - p) * 6 * t;
    if (t < 0.5f) return q;
    if (t < 2.f / 3) return p + (q - p) * (2.f / 3 - t) * 6;
    return p;
}

static void HslToRgb(float h, float s, float l, float* r, float* g, float* b) {
    if (s <= 0) { *r = *g = *b = l; return; }
    float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
    float p = 2 * l - q;
    *r = Hue2Rgb(p, q, h + 1.f / 3);
    *g = Hue2Rgb(p, q, h);
    *b = Hue2Rgb(p, q, h - 1.f / 3);
}

static float HueOf(float r, float g, float b) {
    float mx = std::max(r, std::max(g, b)), mn = std::min(r, std::min(g, b));
    float d = mx - mn;
    if (d <= 0) return 0;
    float h;
    if (mx == r) h = fmodf((g - b) / d, 6.f);
    else if (mx == g) h = (b - r) / d + 2;
    else h = (r - g) / d + 4;
    h /= 6;
    if (h < 0) h += 1;
    return h;
}

// Input: straight-alpha BGRA. Output: premultiplied BGRA.
static void ProcessBuffer(BYTE* p, int n, const Settings& s) {
    BYTE lutR[256], lutG[256], lutB[256];
    if (s.colorize) {
        for (int i = 0; i < 256; i++) {
            float l = i / 255.f;
            l = std::clamp((l - 0.5f) * s.contrast + 0.5f, 0.f, 1.f);
            float r, g, b;
            HslToRgb(s.hue, s.sat, l, &r, &g, &b);
            lutR[i] = (BYTE)(std::clamp(r, 0.f, 1.f) * 255 + 0.5f);
            lutG[i] = (BYTE)(std::clamp(g, 0.f, 1.f) * 255 + 0.5f);
            lutB[i] = (BYTE)(std::clamp(b, 0.f, 1.f) * 255 + 0.5f);
        }
    }
    int op = (int)(s.opacity * 256 + 0.5f);

    for (int i = 0; i < n; i++) {
        BYTE* q = p + i * 4;
        int a = q[3];
        if (a == 0) { q[0] = q[1] = q[2] = 0; continue; }
        int b = q[0], g = q[1], r = q[2];

        // 1. black-white
        if (s.gray) {
            int l = (r * 77 + g * 150 + b * 29) >> 8;
            r = g = b = l;
        }
        // 2. colorize
        if (s.colorize) {
            int l = (r * 77 + g * 150 + b * 29) >> 8;
            r = lutR[l]; g = lutG[l]; b = lutB[l];
        }
        // 3. transparency
        if (s.transp) a = (a * op) >> 8;

        q[0] = (BYTE)((b * a + 127) / 255);
        q[1] = (BYTE)((g * a + 127) / 255);
        q[2] = (BYTE)((r * a + 127) / 255);
        q[3] = (BYTE)a;
    }
}

// ---------------------------------------------------------------- settings / cache
static void ClearCacheLocked() {
    for (auto& kv : g_cache)
        if (kv.second.bmp) DeleteObject(kv.second.bmp);
    g_cache.clear();
}

static void ClearCache() {
    EnterCriticalSection(&g_cs);
    ClearCacheLocked();
    LeaveCriticalSection(&g_cs);
}

static int KeyToMs(PCWSTR k, int def) {
    struct E { const wchar_t* key; int ms; };
    static const E t[] = {{L"off", 0}, {L"s05", 500}, {L"s1", 1000}, {L"s2", 2000}, {L"s3", 3000},
                          {L"s5", 5000}, {L"s10", 10000}, {L"s30", 30000}, {L"s60", 60000}};
    if (k)
        for (const E& e : t)
            if (!wcscmp(k, e.key)) return e.ms;
    return def;
}

static void LoadSettings() {
    Settings s;
    s.hideNames = Wh_GetIntSetting(L"HideNames") != 0;
    s.hideArrow = Wh_GetIntSetting(L"HideShortcutArrow") != 0;
    s.gray = Wh_GetIntSetting(L"Grayscale") != 0;
    s.colorize = Wh_GetIntSetting(L"Colorize") != 0;
    s.transp = Wh_GetIntSetting(L"Transparency") != 0;

    PCWSTR c = Wh_GetStringSetting(L"ColorizeColor");
    const wchar_t* p = c ? c : L"0078D7";
    if (*p == L'#') p++;
    unsigned long v = wcstoul(p, nullptr, 16) & 0xFFFFFF;
    Wh_FreeStringSetting(c);
    s.hue = HueOf(((v >> 16) & 0xFF) / 255.f, ((v >> 8) & 0xFF) / 255.f, (v & 0xFF) / 255.f);

    s.sat = std::clamp(Wh_GetIntSetting(L"ColorizeSaturation"), 0, 100) / 100.f;
    s.contrast = std::clamp(Wh_GetIntSetting(L"ColorizeContrast"), 0, 200) / 100.f;
    s.opacity = 1.f - std::clamp(Wh_GetIntSetting(L"TransparencyAmount"), 0, 100) / 100.f;

    PCWSTR rd = Wh_GetStringSetting(L"RefreshDelay");
    s.settleMs = std::max(300, KeyToMs(rd, 2000));
    Wh_FreeStringSetting(rd);
    PCWSTR wd = Wh_GetStringSetting(L"WatchdogInterval");
    s.watchMs = KeyToMs(wd, 10000);
    Wh_FreeStringSetting(wd);

    EnterCriticalSection(&g_cs);
    g_s = s;
    ClearCacheLocked();
    LeaveCriticalSection(&g_cs);
}

static Settings GetSettings() {
    EnterCriticalSection(&g_cs);
    Settings s = g_s;
    LeaveCriticalSection(&g_cs);
    return s;
}

// Overlay index of the shortcut arrow in the system overlay list.
static int LinkOverlayIndex() {
    static int idx = 0;
    if (idx > 0) return idx;
    typedef int (WINAPI* Fn)(LPCWSTR, int);
    HMODULE h = GetModuleHandleW(L"shell32.dll");
    Fn f = h ? (Fn)GetProcAddress(h, "SHGetIconOverlayIndexW") : nullptr;
    int r = f ? f(nullptr, 0x0FFFFFFE /* IDO_SHGIOI_LINK */) : 0;
    idx = r > 0 ? r : 2;
    return idx;
}

// ---------------------------------------------------------------- icon building
static bool BuildProcessed(HICON icon, const Settings& s, Cached* out) {
    ICONINFO ii;
    if (!GetIconInfo(icon, &ii)) return false;
    bool ok = false;
    if (ii.hbmColor) {
        BITMAP bm = {};
        GetObjectW(ii.hbmColor, sizeof(bm), &bm);
        int w = bm.bmWidth, h = bm.bmHeight;
        if (w > 0 && h > 0 && w <= 1024 && h <= 1024) {
            std::vector<BYTE> buf((size_t)w * h * 4);
            BITMAPINFO bi = {};
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = w;
            bi.bmiHeader.biHeight = -h;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;

            HDC dc = GetDC(nullptr);
            if (GetDIBits(dc, ii.hbmColor, 0, h, buf.data(), &bi, DIB_RGB_COLORS)) {
                bool hasAlpha = false;
                for (size_t i = 0; i < (size_t)w * h; i++)
                    if (buf[i * 4 + 3]) { hasAlpha = true; break; }
                if (!hasAlpha && ii.hbmMask) {
                    std::vector<BYTE> mb((size_t)w * h * 4);
                    if (GetDIBits(dc, ii.hbmMask, 0, h, mb.data(), &bi, DIB_RGB_COLORS))
                        for (size_t i = 0; i < (size_t)w * h; i++)
                            buf[i * 4 + 3] = mb[i * 4] ? 0 : 255;
                }
                out->alpha.resize((size_t)w * h);
                for (size_t i = 0; i < (size_t)w * h; i++) out->alpha[i] = buf[i * 4 + 3];

                ProcessBuffer(buf.data(), w * h, s);

                void* bits = nullptr;
                HBITMAP dib = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
                if (dib && bits) {
                    memcpy(bits, buf.data(), buf.size());
                    out->bmp = dib; out->w = w; out->h = h;
                    ok = true;
                }
            }
            ReleaseDC(nullptr, dc);
        }
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return ok;
}

// ---------------------------------------------------------------- painting
static HIMAGELIST GetListImageList(HWND hList, int* cx, int* cy, LRESULT* viewOut) {
    LRESULT view = SendMessageW(hList, LVM_GETVIEW, 0, 0);
    if (viewOut) *viewOut = view;
    int which = (view == LV_VIEW_ICON || view == LV_VIEW_TILE) ? LVSIL_NORMAL : LVSIL_SMALL;
    HIMAGELIST himl = (HIMAGELIST)SendMessageW(hList, LVM_GETIMAGELIST, which, 0);
    *cx = *cy = 0;
    if (himl) ImageList_GetIconSize(himl, cx, cy);
    return himl;
}

static bool CaptureBg(SubData* sd, HWND hList, HDC hdc, int idx) {
    RECT rc = {};
    rc.left = LVIR_ICON;
    if (!SendMessageW(hList, LVM_GETITEMRECT, idx, (LPARAM)&rc)) return false;
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0 || w > 1024 || h > 1024) return false;

    if (!sd->bgDC || sd->bgW != w || sd->bgH != h) {
        sd->DestroyBg();
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        HBITMAP bmp = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        HDC mem = CreateCompatibleDC(hdc);
        if (!bmp || !bits || !mem) {
            if (bmp) DeleteObject(bmp);
            if (mem) DeleteDC(mem);
            return false;
        }
        sd->bgOld = SelectObject(mem, bmp);
        sd->bgDC = mem;
        sd->bgBmp = bmp;
        sd->bgBits = bits;
        sd->bgW = w;
        sd->bgH = h;
    }
    BitBlt(sd->bgDC, 0, 0, w, h, hdc, rc.left, rc.top, SRCCOPY);
    sd->rc = rc;
    sd->hasBg = true;
    return true;
}

// Restores the area under the icon. For selected/hovered items the selection
// highlight is rebuilt from the pixels the original icon left empty.
static void RestoreBackground(SubData* sd, HDC hdc, const Cached& c, int ox, int oy) {
    int rw = sd->rc.right - sd->rc.left, rh = sd->rc.bottom - sd->rc.top;
    bool done = false;

    if (sd->hl && sd->bgBits) {
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = rw;
        bi.bmiHeader.biHeight = -rh;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* tbits = nullptr;
        HBITMAP tb = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &tbits, nullptr, 0);
        HDC tdc = CreateCompatibleDC(hdc);
        if (tb && tbits && tdc) {
            HGDIOBJ old = SelectObject(tdc, tb);
            BitBlt(tdc, 0, 0, rw, rh, hdc, sd->rc.left, sd->rc.top, SRCCOPY);
            BYTE* C = (BYTE*)tbits;
            const BYTE* B = (const BYTE*)sd->bgBits;

            auto isEmpty = [&](int x, int y) {
                int ix = x - ox, iy = y - oy;
                if (ix < 0 || iy < 0 || ix >= c.w || iy >= c.h) return true;
                return c.alpha[(size_t)iy * c.w + ix] == 0;
            };

            long long sumC[4] = {}, sumB[4] = {};
            long long n = 0;
            for (int y = 0; y < rh; y++)
                for (int x = 0; x < rw; x++)
                    if (isEmpty(x, y)) {
                        size_t o = ((size_t)y * rw + x) * 4;
                        for (int k = 0; k < 4; k++) { sumC[k] += C[o + k]; sumB[k] += B[o + k]; }
                        n++;
                    }
            if (n > 0) {
                int d[4];
                for (int k = 0; k < 4; k++) d[k] = (int)((sumC[k] - sumB[k]) / n);
                for (int y = 0; y < rh; y++)
                    for (int x = 0; x < rw; x++)
                        if (!isEmpty(x, y)) {
                            size_t o = ((size_t)y * rw + x) * 4;
                            for (int k = 0; k < 4; k++)
                                C[o + k] = (BYTE)std::clamp((int)B[o + k] + d[k], 0, 255);
                        }
                BitBlt(hdc, sd->rc.left, sd->rc.top, rw, rh, tdc, 0, 0, SRCCOPY);
                done = true;
            }
            SelectObject(tdc, old);
        }
        if (tdc) DeleteDC(tdc);
        if (tb) DeleteObject(tb);
    }
    if (!done)
        BitBlt(hdc, sd->rc.left, sd->rc.top, rw, rh, sd->bgDC, 0, 0, SRCCOPY);
}

static void ApplyIcon(SubData* sd, HWND hList, HDC hdc, int idx, const Settings& s) {
    int cx, cy;
    HIMAGELIST himl = GetListImageList(hList, &cx, &cy, nullptr);
    if (!himl) return;

    LVITEMW it = {};
    it.mask = LVIF_IMAGE | LVIF_STATE;
    it.iItem = idx;
    it.stateMask = LVIS_OVERLAYMASK | LVIS_CUT;
    if (!SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&it)) return;
    if (it.iImage < 0 || (it.state & LVIS_CUT)) return;
    int overlay = (it.state & LVIS_OVERLAYMASK) >> 8;

    // drop the shortcut arrow (other overlays are kept)
    bool arrowHidden = false;
    if (s.hideArrow && overlay > 0 && overlay == LinkOverlayIndex()) {
        overlay = 0;
        arrowHidden = true;
    }
    // nothing to change for this icon
    if (!s.HasEffects() && !arrowHidden) return;

    int rw = sd->rc.right - sd->rc.left, rh = sd->rc.bottom - sd->rc.top;

    EnterCriticalSection(&g_cs);
    CacheKey key((UINT_PTR)himl, it.iImage, overlay, cx, cy);
    auto f = g_cache.find(key);
    if (f == g_cache.end()) {
        HICON icon = ImageList_GetIcon(himl, it.iImage, ILD_NORMAL | INDEXTOOVERLAYMASK(overlay));
        if (!icon) { LeaveCriticalSection(&g_cs); return; }
        Cached nc;
        bool ok = BuildProcessed(icon, s, &nc);
        DestroyIcon(icon);
        if (!ok) { LeaveCriticalSection(&g_cs); return; }
        if (g_cache.size() > 1024) ClearCacheLocked();
        f = g_cache.emplace(key, std::move(nc)).first;
    }
    const Cached& c = f->second;

    if (c.w <= rw + 2 && c.h <= rh + 2) {
        int ox = (rw - c.w) / 2, oy = (rh - c.h) / 2;
        HDC mdc = CreateCompatibleDC(hdc);
        HGDIOBJ old = SelectObject(mdc, c.bmp);
        RestoreBackground(sd, hdc, c, ox, oy);
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        AlphaBlend(hdc, sd->rc.left + ox, sd->rc.top + oy, c.w, c.h, mdc, 0, 0, c.w, c.h, bf);
        SelectObject(mdc, old);
        DeleteDC(mdc);
    }
    LeaveCriticalSection(&g_cs);
}

static bool IsListView(SubData* sd, HWND h) {
    if (h == sd->lastList) return true;
    wchar_t cls[32] = {};
    GetClassNameW(h, cls, 32);
    if (!wcscmp(cls, L"SysListView32")) { sd->lastList = h; return true; }
    return false;
}

// Only the desktop icon view is affected (top-level window is Progman or WorkerW).
static bool IsDesktopList(HWND hList) {
    HWND root = GetAncestor(hList, GA_ROOT);
    if (!root) return false;
    wchar_t cls[64] = {};
    GetClassNameW(root, cls, 64);
    return !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW");
}

static bool IsDesktopCached(SubData* sd, HWND hList) {
    if (sd->classList != hList) {
        sd->classList = hList;
        sd->isDesktop = IsDesktopList(hList);
    }
    return sd->isDesktop;
}

static void Untrack(HWND h) {
    EnterCriticalSection(&g_cs);
    g_tracked.erase(std::remove(g_tracked.begin(), g_tracked.end(), h), g_tracked.end());
    LeaveCriticalSection(&g_cs);
}

// Pause the effect, then re-apply it once things have been quiet for the chosen delay.
static void StartSettle(SubData* sd, HWND hWnd, int ms) {
    sd->settling = true;
    SetTimer(hWnd, kSettleTimer, (UINT)ms, nullptr);
}

static LRESULT CALLBACK ParentSubclassProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp,
                                           UINT_PTR id, DWORD_PTR ref) {
    SubData* sd = (SubData*)ref;

    if (msg == WM_NCDESTROY) {
        KillTimer(hWnd, kSettleTimer);
        RemoveWindowSubclass(hWnd, ParentSubclassProc, id);
        Untrack(hWnd);
        sd->DestroyBg();
        delete sd;
        return DefSubclassProc(hWnd, msg, wp, lp);
    }

    if (msg == WM_TIMER && wp == kSettleTimer) {
        KillTimer(hWnd, kSettleTimer);
        sd->settling = false;
        ClearCache();
        RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
        return 0;
    }

    if (msg == WM_NOTIFY && lp) {
        NMHDR* nh = (NMHDR*)lp;
        if (IsListView(sd, nh->hwndFrom)) {

            // icon names: return empty text for every label on the desktop
            if (nh->code == (UINT)LVN_GETDISPINFOW) {
                LRESULT r = DefSubclassProc(hWnd, msg, wp, lp);
                Settings s = GetSettings();
                if (s.hideNames && IsDesktopCached(sd, nh->hwndFrom)) {
                    NMLVDISPINFOW* di = (NMLVDISPINFOW*)lp;
                    if (di->item.mask & LVIF_TEXT) di->item.pszText = g_emptyText;
                }
                return r;
            }

            // desktop reloaded (refresh) -> wait for it to settle, then re-apply
            if (nh->code == (UINT)LVN_DELETEALLITEMS) {
                Settings s = GetSettings();
                if (s.NeedsWork() && IsDesktopCached(sd, nh->hwndFrom)) {
                    ClearCache();
                    StartSettle(sd, hWnd, s.settleMs);
                }
            }

            if (nh->code == (UINT)NM_CUSTOMDRAW) {
                LRESULT r = DefSubclassProc(hWnd, msg, wp, lp);
                NMLVCUSTOMDRAW* cd = (NMLVCUSTOMDRAW*)lp;
                HWND hList = nh->hwndFrom;

                switch (cd->nmcd.dwDrawStage) {
                    case CDDS_PREPAINT: {
                        sd->FreeBg();
                        sd->active = false;
                        Settings s = GetSettings();
                        sd->classList = hList;
                        sd->isDesktop = IsDesktopList(hList);
                        if (s.NeedsWork() && sd->isDesktop) {
                            int cx, cy;
                            LRESULT view;
                            HIMAGELIST himl = GetListImageList(hList, &cx, &cy, &view);
                            if (!sd->sizeKnown) {
                                sd->sizeKnown = true;
                                sd->lastHiml = himl; sd->lastCx = cx; sd->lastCy = cy; sd->lastView = view;
                            } else if (himl != sd->lastHiml || cx != sd->lastCx ||
                                       cy != sd->lastCy || view != sd->lastView) {
                                // resized / view changed -> don't rebuild on every step
                                sd->lastHiml = himl; sd->lastCx = cx; sd->lastCy = cy; sd->lastView = view;
                                ClearCache();
                                StartSettle(sd, hWnd, s.settleMs);
                            }
                            sd->active = !sd->settling;
                            if (sd->active && !(r & CDRF_SKIPDEFAULT)) r |= CDRF_NOTIFYITEMDRAW;
                        }
                        break;
                    }
                    case CDDS_ITEMPREPAINT: {
                        sd->FreeBg();
                        if (sd->active && !(r & CDRF_SKIPDEFAULT)) {
                            UINT st = cd->nmcd.uItemState;
                            bool hl = (st & (CDIS_SELECTED | CDIS_HOT | CDIS_MARKED)) != 0;
                            if (CaptureBg(sd, hList, cd->nmcd.hdc, (int)cd->nmcd.dwItemSpec)) {
                                sd->hl = hl;
                                r |= CDRF_NOTIFYPOSTPAINT;
                            }
                        }
                        break;
                    }
                    case CDDS_ITEMPOSTPAINT: {
                        if (sd->hasBg) {
                            Settings s = GetSettings();
                            ApplyIcon(sd, hList, cd->nmcd.hdc, (int)cd->nmcd.dwItemSpec, s);
                            sd->FreeBg();
                        }
                        break;
                    }
                }
                return r;
            }
        }
    }
    return DefSubclassProc(hWnd, msg, wp, lp);
}

// ---------------------------------------------------------------- run on window's thread
struct RunParam { void (WINAPI* proc)(PVOID); PVOID arg; };

static LRESULT CALLBACK RunHookProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lp;
        if (cwp->message == g_runMsg) {
            RunParam* p = (RunParam*)cwp->lParam;
            p->proc(p->arg);
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

static bool RunFromWindowThread(HWND hWnd, void (WINAPI* proc)(PVOID), PVOID arg) {
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) { proc(arg); return true; }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunHookProc, nullptr, tid);
    if (!hook) return false;
    RunParam p = {proc, arg};
    SendMessageW(hWnd, g_runMsg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

static bool IsTracked(HWND h) {
    EnterCriticalSection(&g_cs);
    bool r = std::find(g_tracked.begin(), g_tracked.end(), h) != g_tracked.end();
    LeaveCriticalSection(&g_cs);
    return r;
}

static void WINAPI DoSubclass(PVOID arg) {
    HWND parent = (HWND)arg;
    DWORD_PTR dummy;
    if (GetWindowSubclass(parent, ParentSubclassProc, 1, &dummy)) return;
    SubData* sd = new SubData();
    if (!SetWindowSubclass(parent, ParentSubclassProc, 1, (DWORD_PTR)sd)) { delete sd; return; }
    EnterCriticalSection(&g_cs);
    g_tracked.push_back(parent);
    LeaveCriticalSection(&g_cs);
    RedrawWindow(parent, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}

static void WINAPI DoUnsubclass(PVOID arg) {
    HWND parent = (HWND)arg;
    DWORD_PTR ref = 0;
    if (GetWindowSubclass(parent, ParentSubclassProc, 1, &ref)) {
        KillTimer(parent, kSettleTimer);
        RemoveWindowSubclass(parent, ParentSubclassProc, 1);
        SubData* sd = (SubData*)ref;
        sd->DestroyBg();
        delete sd;
    }
    Untrack(parent);
}

// ---------------------------------------------------------------- hooks and scanning
using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;

static HWND WINAPI CreateWindowExW_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style,
                                        int x, int y, int w, int h, HWND parent, HMENU menu,
                                        HINSTANCE inst, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(ex, cls, name, style, x, y, w, h, parent, menu, inst, param);
    if (hwnd && (style & WS_CHILD)) {
        wchar_t cn[32] = {};
        GetClassNameW(hwnd, cn, 32);
        if (!wcscmp(cn, L"SysListView32")) {
            HWND p = GetParent(hwnd);
            if (p) DoSubclass(p);   // whether it is the desktop is decided at paint time
        }
    }
    return hwnd;
}

static BOOL CALLBACK EnumChildProc(HWND h, LPARAM) {
    wchar_t cls[32] = {};
    GetClassNameW(h, cls, 32);
    if (!wcscmp(cls, L"SysListView32")) {
        HWND p = GetParent(h);
        if (p && !IsTracked(p)) RunFromWindowThread(p, DoSubclass, p);
    }
    return TRUE;
}

static BOOL CALLBACK EnumTopProc(HWND h, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId()) EnumChildWindows(h, EnumChildProc, 0);
    return TRUE;
}

// Safety net: re-attaches if the desktop icon view was recreated without the hook noticing.
static DWORD WINAPI WatchdogThread(LPVOID) {
    for (;;) {
        int ms = GetSettings().watchMs;
        DWORD wait = ms > 0 ? (DWORD)ms : 2000;
        if (WaitForSingleObject(g_stopEvt, wait) != WAIT_TIMEOUT) break;
        if (ms > 0) EnumWindows(EnumTopProc, 0);
    }
    return 0;
}

static void RedrawAll() {
    std::vector<HWND> copy;
    EnterCriticalSection(&g_cs);
    copy = g_tracked;
    LeaveCriticalSection(&g_cs);
    for (HWND h : copy)
        if (IsWindow(h))
            RedrawWindow(h, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}

// Makes the desktop list re-request every item's text (needed when "Hide icon names" is toggled).
static void RefreshDesktopLabels() {
    std::vector<HWND> copy;
    EnterCriticalSection(&g_cs);
    copy = g_tracked;
    LeaveCriticalSection(&g_cs);
    for (HWND parent : copy) {
        if (!IsWindow(parent)) continue;
        HWND list = FindWindowExW(parent, nullptr, L"SysListView32", nullptr);
        if (!list || !IsDesktopList(list)) continue;
        DWORD_PTR n = 0;
        if (!SendMessageTimeoutW(list, LVM_GETITEMCOUNT, 0, 0, SMTO_ABORTIFHUNG, 300, &n)) continue;
        for (int i = 0; i < (int)n; i++) {
            DWORD_PTR res;
            SendMessageTimeoutW(list, LVM_UPDATE, i, 0, SMTO_ABORTIFHUNG, 200, &res);
        }
    }
}

BOOL Wh_ModInit() {
    InitializeCriticalSection(&g_cs);
    g_runMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_desktop-icon-effects");
    LoadSettings();
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);
    EnumWindows(EnumTopProc, 0);
    if (GetSettings().hideNames) RefreshDesktopLabels();
    g_stopEvt = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_watchdog = CreateThread(nullptr, 0, WatchdogThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    bool hadNamesHidden = GetSettings().hideNames;
    LoadSettings();
    if (GetSettings().hideNames != hadNamesHidden) RefreshDesktopLabels();
    RedrawAll();
}

void Wh_ModUninit() {
    if (g_watchdog) {
        SetEvent(g_stopEvt);
        WaitForSingleObject(g_watchdog, 3000);
        CloseHandle(g_watchdog);
        g_watchdog = nullptr;
    }
    if (g_stopEvt) { CloseHandle(g_stopEvt); g_stopEvt = nullptr; }

    bool hadNamesHidden = GetSettings().hideNames;

    std::vector<HWND> copy;
    EnterCriticalSection(&g_cs);
    copy = g_tracked;
    LeaveCriticalSection(&g_cs);
    for (HWND h : copy)
        if (IsWindow(h)) RunFromWindowThread(h, DoUnsubclass, h);
    if (hadNamesHidden) {
        // names come back as soon as our hook is gone; make the list ask for them again
        for (HWND parent : copy) {
            if (!IsWindow(parent)) continue;
            HWND list = FindWindowExW(parent, nullptr, L"SysListView32", nullptr);
            if (!list || !IsDesktopList(list)) continue;
            DWORD_PTR n = 0;
            if (!SendMessageTimeoutW(list, LVM_GETITEMCOUNT, 0, 0, SMTO_ABORTIFHUNG, 300, &n)) continue;
            for (int i = 0; i < (int)n; i++) {
                DWORD_PTR res;
                SendMessageTimeoutW(list, LVM_UPDATE, i, 0, SMTO_ABORTIFHUNG, 200, &res);
            }
        }
    }
    for (HWND h : copy)
        if (IsWindow(h))
            RedrawWindow(h, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    ClearCache();
    DeleteCriticalSection(&g_cs);
}