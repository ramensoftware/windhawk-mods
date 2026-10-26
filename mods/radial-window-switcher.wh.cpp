// ==WindhawkMod==
// @id              radial-window-switcher
// @name            Radial Window Switcher
// @name:ru-RU      Круговой переключатель окон
// @description     A macOS-style radial Alt+Tab: app icons in a ring, a live window preview and smooth animations
// @description:ru-RU Круговой Alt+Tab в стиле macOS: иконки приложений по кругу, живое превью окна и плавные анимации
// @version         1.0.0
// @author          kotikdetka
// @github          https://github.com/kotikdetka
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -lgdiplus -lgdi32 -ldwmapi -lversion -lshcore -lshell32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Radial Window Switcher

![Radial Window Switcher](https://raw.githubusercontent.com/kotikdetka/radial-window-switcher/6fe413c441dce347eaf6f80ea5c2bb8c68d17b27/photo_2026-10-26_18-49-46.jpg)

Replaces the standard Alt+Tab with a radial switcher: the icons of open
windows are arranged in a ring in the center of the screen, the name of the
selected app is shown in the middle, and a live preview of the selected window
appears next to its sector.

## Usage

- Hold **Alt** and press **Tab** to open the ring.
- While holding Alt, press **Tab** for the next window, **Shift+Tab** for the
  previous one. Arrow keys also work.
- Release **Alt** to switch to the selected window.
- Press **Esc** to close the ring without switching.
- A quick Alt+Tab switches to the previous window, just like the standard one.

## Features

- Smooth animations: the ring spins in, sectors pop out one by one, the
  selection glows and a pointer glides to the selected app.
- Live window previews (DWM thumbnails), like the standard Alt+Tab.
- The selected sector uses the Windows accent color.
- Per-monitor DPI aware; the ring opens on the monitor with the cursor.
- Can use Alt+\` or Alt+Q instead of Alt+Tab, keeping the standard switcher.

## Notes

- The switcher runs in its own Windhawk process. Only a small part runs in
  Explorer: it catches Explorer's Alt+Tab hotkey and hands it to the
  switcher, so the standard switcher doesn't open as well.
- Minimized windows have no preview: Windows doesn't render them.
- Alt+\` and Alt+Q can't be caught while an elevated (administrator) window is
  in the foreground. Alt+Tab works there too, through Explorer.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- trigger: altTab
  $name: Shortcut
  $name:ru-RU: Сочетание клавиш
  $description: Hold Alt and press the second key to cycle through windows
  $description:ru-RU: Держите Alt и нажимайте вторую клавишу, чтобы листать окна
  $options:
  - altTab: Alt+Tab (replace the standard switcher)
  - altBacktick: Alt+` (key above Tab)
  - altQ: Alt+Q
  $options:ru-RU:
  - altTab: Alt+Tab (заменить стандартный)
  - altBacktick: Alt+` (клавиша Ё)
  - altQ: Alt+Q
- activateOnRelease: true
  $name: Switch when Alt is released
  $name:ru-RU: Переключать при отпускании Alt
  $description: If disabled, the ring stays open until Enter is pressed
  $description:ru-RU: Если выключено, меню остаётся открытым до нажатия Enter
- showPreview: true
  $name: Show live window preview
  $name:ru-RU: Показывать живое превью окна
- previewSize: 380
  $name: Preview width (px)
  $name:ru-RU: Ширина превью (px)
- radius: 230
  $name: Ring radius (px)
  $name:ru-RU: Радиус круга (px)
- iconSize: 64
  $name: Icon size (px)
  $name:ru-RU: Размер иконок (px)
- maxItems: 12
  $name: Maximum number of windows
  $name:ru-RU: Максимум окон в круге
- dimOpacity: 70
  $name: Background dimming (0-200)
  $name:ru-RU: Затемнение фона (0-200)
- useAccentColor: true
  $name: Highlight the selection with the Windows accent color
  $name:ru-RU: Подсвечивать выбранный сектор акцентным цветом Windows
- animate: true
  $name: Animations
  $name:ru-RU: Анимации
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <windows.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <tlhelp32.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwchar>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace gp = Gdiplus;

// ============================================================================
// Settings
// ============================================================================

enum class Trigger { AltBacktick, AltTab, AltQ };

struct Settings {
    Trigger trigger = Trigger::AltTab;
    bool activateOnRelease = true;
    bool showPreview = true;
    int previewSize = 380;
    int radius = 230;
    int iconSize = 64;
    int maxItems = 12;
    int dimOpacity = 70;
    bool useAccentColor = true;
    bool animate = true;
};

static Settings g_settings;
// The trigger key: matched by virtual-key code (Tab, Q), or by scan code for
// the key above Tab, whose virtual-key code depends on the layout.
static std::atomic<DWORD> g_triggerVk{VK_TAB};
static std::atomic<DWORD> g_triggerScan{0};
static std::atomic<bool> g_activateOnRelease{true};

static void LoadSettings() {
    Settings s;
    const auto trig = WindhawkUtils::StringSetting::make(L"trigger");
    if (wcscmp(trig, L"altBacktick") == 0) {
        s.trigger = Trigger::AltBacktick;
    } else if (wcscmp(trig, L"altQ") == 0) {
        s.trigger = Trigger::AltQ;
    }
    s.activateOnRelease = Wh_GetIntSetting(L"activateOnRelease") != 0;
    s.showPreview = Wh_GetIntSetting(L"showPreview") != 0;
    s.previewSize = std::clamp(Wh_GetIntSetting(L"previewSize"), 160, 900);
    s.radius = std::clamp(Wh_GetIntSetting(L"radius"), 140, 520);
    s.iconSize = std::clamp(Wh_GetIntSetting(L"iconSize"), 24, 160);
    s.maxItems = std::clamp(Wh_GetIntSetting(L"maxItems"), 2, 24);
    s.dimOpacity = std::clamp(Wh_GetIntSetting(L"dimOpacity"), 0, 200);
    s.useAccentColor = Wh_GetIntSetting(L"useAccentColor") != 0;
    s.animate = Wh_GetIntSetting(L"animate") != 0;
    g_settings = s;
    g_activateOnRelease = s.activateOnRelease;

    switch (s.trigger) {
        case Trigger::AltQ:
            g_triggerVk = 'Q';
            g_triggerScan = 0;
            break;
        case Trigger::AltBacktick:
            g_triggerVk = 0;
            g_triggerScan = 0x29;  // the key above Tab
            break;
        default:
            g_triggerVk = VK_TAB;
            g_triggerScan = 0;
            break;
    }
}

// ============================================================================
// Constants & globals
// ============================================================================

constexpr ULONG_PTR kInjectedMagic = 0x52414454;  // marks our own SendInput
constexpr UINT WM_RS_SHOW = WM_APP + 1;
constexpr UINT WM_RS_NEXT = WM_APP + 2;
constexpr UINT WM_RS_PREV = WM_APP + 3;
constexpr UINT WM_RS_ACTIVATE = WM_APP + 4;
constexpr UINT WM_RS_CANCEL = WM_APP + 5;
constexpr UINT WM_RS_RELOAD = WM_APP + 6;
constexpr UINT WM_RS_QUIT = WM_APP + 7;
constexpr UINT WM_RS_REHOOK = WM_APP + 8;
constexpr UINT WM_RS_PREWARM = WM_APP + 9;
constexpr UINT_PTR kTimerId = 1;
constexpr float kPi = 3.14159265358979f;
constexpr wchar_t kClassName[] = L"RadialWindowSwitcher_Windhawk";
constexpr wchar_t kPreviewClassName[] = L"RadialWindowSwitcherPreview_Windhawk";
// The switcher window's title tells Explorer whether it can take Alt+Tab
// over right now. FindWindow compares titles without sending messages.
constexpr wchar_t kTitleIdle[] = L"Radial Window Switcher";
constexpr wchar_t kTitleReady[] = L"Radial Window Switcher (ready)";
// Posted by Explorer to the switcher window when it gets the Alt+Tab hotkey.
// wParam: 1 for Shift+Alt+Tab.
constexpr wchar_t kHotkeyMessageName[] = L"RadialWindowSwitcher_Hotkey";
// Explorer's thread that receives the Alt+Tab hotkey.
constexpr wchar_t kAltTabThreadName[] = L"Immersive Shell";
static UINT g_hotkeyMessage = 0;

static HINSTANCE g_hInst = nullptr;
static HANDLE g_thread = nullptr;
static HANDLE g_readyEvent = nullptr;
static std::atomic<HWND> g_hwnd{nullptr};
static HHOOK g_hook = nullptr;
static HWND g_previewHwnd = nullptr;
static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_hookReadyEvent = nullptr;

// Shared between the UI thread and the keyboard-hook thread.
static std::atomic<bool> g_visible{false};
static std::atomic<bool> g_pending{false};  // show requested, not processed yet
static std::atomic<bool> g_altHeld{false};  // physical Alt state, set by the hook
static bool g_dirty = false;
static ULONGLONG g_animStart = 0;

// ============================================================================
// Icons / app info
// ============================================================================

struct IconImage {
    std::vector<BYTE> pixels;
    int w = 0, h = 0;
    std::unique_ptr<gp::Bitmap> bmp;
};

struct AppInfo {
    std::wstring name;
    std::shared_ptr<IconImage> icon;
};

struct Item {
    HWND hwnd = nullptr;
    std::wstring name;
    std::wstring title;
    std::shared_ptr<IconImage> icon;
};

static std::unordered_map<std::wstring, AppInfo> g_appCache;
static std::shared_ptr<IconImage> g_defaultIcon;
static std::vector<Item> g_items;
static int g_sel = 0;

static std::shared_ptr<IconImage> IconFromHicon(HICON hIcon) {
    ICONINFO ii{};
    if (!hIcon || !GetIconInfo(hIcon, &ii)) {
        return nullptr;
    }
    std::shared_ptr<IconImage> img;
    if (ii.hbmColor) {
        BITMAP bm{};
        GetObjectW(ii.hbmColor, sizeof(bm), &bm);
        int w = bm.bmWidth, h = bm.bmHeight;
        if (w > 0 && h > 0) {
            img = std::make_shared<IconImage>();
            img->w = w;
            img->h = h;
            img->pixels.resize((size_t)w * h * 4);

            BITMAPINFO bi{};
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = w;
            bi.bmiHeader.biHeight = -h;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;

            HDC dc = GetDC(nullptr);
            GetDIBits(dc, ii.hbmColor, 0, h, img->pixels.data(), &bi,
                      DIB_RGB_COLORS);

            bool hasAlpha = false;
            for (size_t i = 0; i < (size_t)w * h; i++) {
                if (img->pixels[i * 4 + 3]) {
                    hasAlpha = true;
                    break;
                }
            }
            if (!hasAlpha && ii.hbmMask) {
                std::vector<BYTE> mask((size_t)w * h * 4);
                GetDIBits(dc, ii.hbmMask, 0, h, mask.data(), &bi,
                          DIB_RGB_COLORS);
                for (size_t i = 0; i < (size_t)w * h; i++) {
                    img->pixels[i * 4 + 3] = mask[i * 4] ? 0 : 255;
                }
            }
            ReleaseDC(nullptr, dc);

            img->bmp = std::make_unique<gp::Bitmap>(
                w, h, w * 4, PixelFormat32bppARGB, img->pixels.data());
        }
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return img;
}

static std::shared_ptr<IconImage> LoadExeIcon(const std::wstring& path,
                                              int size) {
    using PrivateExtractIconsW_t =
        UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
    static auto pExtract = (PrivateExtractIconsW_t)(void*)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "PrivateExtractIconsW");
    if (!pExtract) {
        return nullptr;
    }
    HICON hIcon = nullptr;
    UINT id = 0;
    UINT n = pExtract(path.c_str(), 0, size, size, &hIcon, &id, 1, 0);
    if (n == 0 || n == 0xFFFFFFFF || !hIcon) {
        return nullptr;
    }
    auto img = IconFromHicon(hIcon);
    DestroyIcon(hIcon);
    return img;
}

static HICON GetWindowIconHandle(HWND hwnd) {
    DWORD_PTR r = 0;
    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_BIG, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &r) &&
        r) {
        return (HICON)r;
    }
    r = GetClassLongPtrW(hwnd, GCLP_HICON);
    if (r) {
        return (HICON)r;
    }
    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_SMALL2, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &r) &&
        r) {
        return (HICON)r;
    }
    return nullptr;
}

static std::wstring GetProcessPath(DWORD pid) {
    std::wstring result;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (h) {
        wchar_t buf[MAX_PATH * 2];
        DWORD size = ARRAYSIZE(buf);
        if (QueryFullProcessImageNameW(h, 0, buf, &size)) {
            result.assign(buf, size);
        }
        CloseHandle(h);
    }
    return result;
}

static std::wstring GetFileDescription(const std::wstring& path) {
    DWORD dummy = 0;
    DWORD size = GetFileVersionInfoSizeW(path.c_str(), &dummy);
    if (!size) {
        return L"";
    }
    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoW(path.c_str(), 0, size, data.data())) {
        return L"";
    }
    struct LangCodePage {
        WORD lang;
        WORD codePage;
    }* tr = nullptr;
    UINT cb = 0;
    if (VerQueryValueW(data.data(), L"\\VarFileInfo\\Translation",
                       (LPVOID*)&tr, &cb) &&
        cb >= sizeof(LangCodePage)) {
        wchar_t sub[64];
        _snwprintf(sub, ARRAYSIZE(sub),
                   L"\\StringFileInfo\\%04x%04x\\FileDescription", tr[0].lang,
                   tr[0].codePage);
        LPWSTR val = nullptr;
        UINT len = 0;
        if (VerQueryValueW(data.data(), sub, (LPVOID*)&val, &len) && val &&
            len > 1) {
            return val;
        }
    }
    return L"";
}

static std::wstring FileStem(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    std::wstring name = slash == std::wstring::npos ? path : path.substr(slash + 1);
    size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        name.resize(dot);
    }
    return name;
}

static bool EndsWithI(const std::wstring& s, const wchar_t* suffix) {
    size_t n = wcslen(suffix);
    return s.size() >= n && _wcsicmp(s.c_str() + s.size() - n, suffix) == 0;
}

static AppInfo GetAppInfo(const std::wstring& path, int iconPx) {
    std::wstring key = path + L"|" + std::to_wstring(iconPx);
    auto it = g_appCache.find(key);
    if (it != g_appCache.end()) {
        return it->second;
    }
    if (g_appCache.size() > 256) {
        g_appCache.clear();
    }
    AppInfo info;
    // In a 32-bit tool process, reads of System32 files would be redirected
    // to SysWOW64, where 64-bit-only system apps don't exist.
    PVOID wow64State = nullptr;
    BOOL redirectionOff = Wow64DisableWow64FsRedirection(&wow64State);
    info.name = GetFileDescription(path);
    info.icon = LoadExeIcon(path, iconPx);
    if (redirectionOff) {
        Wow64RevertWow64FsRedirection(wow64State);
    }
    if (info.name.empty()) {
        info.name = FileStem(path);
    }
    g_appCache.emplace(key, info);
    return info;
}

// ============================================================================
// Window enumeration
// ============================================================================

static bool IsSwitchableWindow(HWND h) {
    if (h == g_hwnd || h == g_previewHwnd || !IsWindowVisible(h)) {
        return false;
    }
    LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) {
        return false;
    }
    if ((ex & WS_EX_NOACTIVATE) && !(ex & WS_EX_APPWINDOW)) {
        return false;
    }
    if (GetWindow(h, GW_OWNER) && !(ex & WS_EX_APPWINDOW)) {
        return false;
    }
    int cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return false;
    }
    wchar_t title[2];
    if (GetWindowTextW(h, title, ARRAYSIZE(title)) == 0) {
        return false;
    }
    wchar_t cls[64];
    GetClassNameW(h, cls, ARRAYSIZE(cls));
    static const wchar_t* kSkip[] = {
        L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
        L"Windows.UI.Core.CoreWindow"};
    for (auto s : kSkip) {
        if (wcscmp(cls, s) == 0) {
            return false;
        }
    }
    return true;
}

static BOOL CALLBACK EnumWindowsProc(HWND h, LPARAM lp) {
    auto* list = reinterpret_cast<std::vector<HWND>*>(lp);
    if (IsSwitchableWindow(h)) {
        list->push_back(h);
    }
    return list->size() < (size_t)g_settings.maxItems;
}

static std::wstring GetTitle(HWND h) {
    wchar_t buf[512];
    int n = GetWindowTextW(h, buf, ARRAYSIZE(buf));
    return std::wstring(buf, std::max(n, 0));
}

static void BuildItems(int iconPx) {
    g_items.clear();
    std::vector<HWND> list;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&list));

    if (!g_defaultIcon) {
        g_defaultIcon = IconFromHicon(LoadIconW(nullptr, IDI_APPLICATION));
    }

    for (HWND h : list) {
        Item it;
        it.hwnd = h;
        it.title = GetTitle(h);

        DWORD pid = 0;
        GetWindowThreadProcessId(h, &pid);
        std::wstring path = GetProcessPath(pid);
        bool uwp = false;
        if (EndsWithI(path, L"\\ApplicationFrameHost.exe")) {
            uwp = true;
            path.clear();
            HWND core = FindWindowExW(h, nullptr,
                                      L"Windows.UI.Core.CoreWindow", nullptr);
            if (core) {
                DWORD corePid = 0;
                GetWindowThreadProcessId(core, &corePid);
                path = GetProcessPath(corePid);
            }
        }

        if (!path.empty()) {
            AppInfo app = GetAppInfo(path, iconPx);
            it.name = app.name;
            it.icon = app.icon;
        }
        if (uwp || it.name.empty()) {
            it.name = it.title;
        }
        if (!it.icon) {
            it.icon = IconFromHicon(GetWindowIconHandle(h));
        }
        if (!it.icon) {
            it.icon = g_defaultIcon;
        }
        g_items.push_back(std::move(it));
    }
}

// ============================================================================
// Layout & drawing
// ============================================================================

struct Layout {
    RECT mon{};
    RECT work{};
    int width = 0, height = 0;
    float cx = 0, cy = 0;  // circle center, window-local
    float scale = 1;
    float outer = 0, inner = 0, icon = 0;
};
static Layout g_lay;

static HDC g_memDC = nullptr;
static HBITMAP g_dib = nullptr;
static HGDIOBJ g_oldBmp = nullptr;
static void* g_bits = nullptr;
static int g_dibW = 0, g_dibH = 0;

static void FreeDib() {
    if (g_memDC) {
        if (g_oldBmp) SelectObject(g_memDC, g_oldBmp);
        DeleteDC(g_memDC);
    }
    if (g_dib) DeleteObject(g_dib);
    g_memDC = nullptr;
    g_dib = nullptr;
    g_oldBmp = nullptr;
    g_bits = nullptr;
    g_dibW = g_dibH = 0;
}

static bool EnsureDib(int w, int h) {
    if (g_dib && g_dibW == w && g_dibH == h) {
        return true;
    }
    FreeDib();
    HDC screen = GetDC(nullptr);
    g_memDC = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    g_dib = CreateDIBSection(g_memDC, &bi, DIB_RGB_COLORS, &g_bits, nullptr, 0);
    if (!g_dib) {
        FreeDib();
        return false;
    }
    g_oldBmp = SelectObject(g_memDC, g_dib);
    g_dibW = w;
    g_dibH = h;
    return true;
}

static float GetMonitorScale(HMONITOR mon) {
    UINT x = 96, y = 96;
    if (FAILED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &x, &y))) {
        x = 96;
    }
    return x / 96.f;
}

// The menu always opens in the center of the monitor with the cursor.
static void ComputeLayout() {
    Layout& L = g_lay;
    POINT cur{};
    GetCursorPos(&cur);
    HMONITOR mon = MonitorFromPoint(cur, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(mon, &mi);

    L.mon = mi.rcMonitor;
    L.work = mi.rcWork;
    L.width = L.mon.right - L.mon.left;
    L.height = L.mon.bottom - L.mon.top;
    L.scale = GetMonitorScale(mon);

    RECT w = mi.rcWork;
    float maxOuter = std::min(w.right - w.left, w.bottom - w.top) / 2.f -
                     24 * L.scale;
    L.outer = std::min(g_settings.radius * L.scale, maxOuter);
    L.inner = L.outer * 0.46f;
    L.icon = g_settings.iconSize * L.scale;

    L.cx = (w.left + w.right) / 2.f - L.mon.left;
    L.cy = (w.top + w.bottom) / 2.f - L.mon.top;
}

static gp::PointF Polar(float r, float deg) {
    float a = deg * kPi / 180.f;
    return gp::PointF(r * cosf(a), r * sinf(a));
}

static gp::PointF Lerp(const gp::PointF& a, const gp::PointF& b, float t) {
    return gp::PointF(a.X + (b.X - a.X) * t, a.Y + (b.Y - a.Y) * t);
}

static float Dist(const gp::PointF& a, const gp::PointF& b) {
    return hypotf(a.X - b.X, a.Y - b.Y);
}

static float RadToDeg(float r) { return r * 180.f / kPi; }

// Annular sector with a constant-width gap to its neighbours and rounded corners.
static void BuildSectorPath(gp::GraphicsPath& p, float ri, float ro, float a0,
                            float a1, float gap, float corner) {
    float go = RadToDeg(gap / 2 / ro);
    float gi = RadToDeg(gap / 2 / ri);
    float so = a0 + go, eo = a1 - go, si = a0 + gi, ei = a1 - gi;
    if (eo <= so || ei <= si) {
        so = a0, eo = a1, si = a0, ei = a1;
    }

    gp::PointF Pos = Polar(ro, so), Poe = Polar(ro, eo);
    gp::PointF Pis = Polar(ri, si), Pie = Polar(ri, ei);
    float edge = std::max(Dist(Pos, Pis), 1.f);
    float c = std::min(corner, edge * 0.45f);
    float co = std::min(RadToDeg(c / ro), (eo - so) * 0.45f);
    float ci = std::min(RadToDeg(c / ri), (ei - si) * 0.45f);
    float te = c / edge;
    const float k = 0.55f;

    auto roundCorner = [&](const gp::PointF& from, const gp::PointF& cornerPt,
                           const gp::PointF& to) {
        p.AddBezier(from, Lerp(from, cornerPt, k), Lerp(to, cornerPt, k), to);
    };

    gp::PointF e1 = Lerp(Pos, Pis, te);
    roundCorner(e1, Pos, Polar(ro, so + co));
    p.AddArc(-ro, -ro, 2 * ro, 2 * ro, so + co, (eo - co) - (so + co));
    gp::PointF e2 = Lerp(Poe, Pie, te);
    roundCorner(Polar(ro, eo - co), Poe, e2);
    gp::PointF e3 = Lerp(Pie, Poe, te);
    p.AddLine(e2, e3);
    roundCorner(e3, Pie, Polar(ri, ei - ci));
    p.AddArc(-ri, -ri, 2 * ri, 2 * ri, ei - ci, -((ei - ci) - (si + ci)));
    gp::PointF e4 = Lerp(Pis, Pos, te);
    roundCorner(Polar(ri, si + ci), Pis, e4);
    p.CloseFigure();
}

static void AddRoundRect(gp::GraphicsPath& p, const gp::RectF& r, float rad) {
    rad = std::min(rad, std::min(r.Width, r.Height) / 2);
    float d = rad * 2;
    p.AddArc(r.X, r.Y, d, d, 180, 90);
    p.AddArc(r.X + r.Width - d, r.Y, d, d, 270, 90);
    p.AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0, 90);
    p.AddArc(r.X, r.Y + r.Height - d, d, d, 90, 90);
    p.CloseFigure();
}

static float Clamp01(float v) { return std::clamp(v, 0.f, 1.f); }

static float EaseOutCubic(float t) {
    t = Clamp01(t);
    return 1.f - powf(1.f - t, 3.f);
}

static float EaseOutBack(float t) {
    t = Clamp01(t);
    const float c1 = 1.70158f, c3 = c1 + 1.f;
    return 1.f + c3 * powf(t - 1.f, 3.f) + c1 * powf(t - 1.f, 2.f);
}

static gp::Color MixColor(const gp::Color& a, const gp::Color& b, float t,
                          float alphaMul = 1.f) {
    auto mix = [t](BYTE x, BYTE y) { return (float)x + ((float)y - (float)x) * t; };
    auto toByte = [](float v) {
        return (BYTE)std::lround(std::clamp(v, 0.f, 255.f));
    };
    return gp::Color(toByte(mix(a.GetA(), b.GetA()) * alphaMul),
                     toByte(mix(a.GetR(), b.GetR())),
                     toByte(mix(a.GetG(), b.GetG())),
                     toByte(mix(a.GetB(), b.GetB())));
}

static float NormalizeAngle(float a) {
    a = fmodf(a + 180.f, 360.f);
    if (a < 0) a += 360.f;
    return a - 180.f;
}

// Milliseconds since `since`, or "long ago" when animations are off.
static float AnimMs(ULONGLONG since) {
    if (!g_settings.animate) {
        return 100000.f;
    }
    return (float)(GetTickCount64() - since);
}

// Read once per frame in Render() rather than per sector.
static DWORD g_accentColor = 0;
static bool g_hasAccentColor = false;

static void RefreshAccentColor() {
    BOOL opaque = FALSE;
    g_hasAccentColor = g_settings.useAccentColor &&
                       SUCCEEDED(DwmGetColorizationColor(&g_accentColor, &opaque));
}

static gp::Color GetAccentColor(BYTE alpha) {
    if (g_hasAccentColor) {
        DWORD c = g_accentColor;
        return gp::Color(alpha, (c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
    }
    return gp::Color((BYTE)std::min(255, alpha * 3 / 4), 255, 255, 255);
}

// ---------------------------------------------------------------------------
// Animation state
// ---------------------------------------------------------------------------

static std::vector<float> g_hover;  // 0..1 highlight per sector
static float g_pointerAngle = -90.f;
static int g_textIndex = -1;
static ULONGLONG g_textChangeAt = 0;
static bool g_needFrame = false;

// ---------------------------------------------------------------------------
// Live window preview (DWM thumbnail, like the system Alt+Tab)
// ---------------------------------------------------------------------------

static HTHUMBNAIL g_thumb = nullptr;
static HWND g_thumbSource = nullptr;
static bool g_previewShown = false;
static ULONGLONG g_previewAt = 0;
static RECT g_previewFinal{};
static float g_previewDirX = 0, g_previewDirY = 0;
static gp::RectF g_previewDrawRect;  // window-local, for the glow frame

static void ClosePreview() {
    if (g_thumb) {
        DwmUnregisterThumbnail(g_thumb);
        g_thumb = nullptr;
    }
    g_thumbSource = nullptr;
    g_previewShown = false;
    if (g_previewHwnd) {
        ShowWindow(g_previewHwnd, SW_HIDE);
    }
}

// Places the preview at the right animation frame. Returns true while moving.
static bool UpdatePreviewAnim() {
    if (!g_previewShown || !g_thumb || !g_previewHwnd) {
        return false;
    }
    float s = g_lay.scale;
    float t = Clamp01(AnimMs(g_previewAt) / 240.f);
    float e = EaseOutCubic(t);
    float sc = 0.78f + 0.22f * EaseOutBack(t);

    const RECT& f = g_previewFinal;
    float fw = (float)(f.right - f.left), fh = (float)(f.bottom - f.top);
    float slide = (1.f - e) * 40.f * s;
    float cx = (f.left + f.right) / 2.f - g_previewDirX * slide;
    float cy = (f.top + f.bottom) / 2.f - g_previewDirY * slide;
    int w = std::max(1, (int)std::lround(fw * sc));
    int h = std::max(1, (int)std::lround(fh * sc));
    int x = (int)std::lround(cx - w / 2.f);
    int y = (int)std::lround(cy - h / 2.f);

    SetWindowPos(g_previewHwnd, nullptr, x, y, w, h,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    DWM_THUMBNAIL_PROPERTIES props{};
    props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE |
                    DWM_TNP_OPACITY | DWM_TNP_SOURCECLIENTAREAONLY;
    props.rcDestination = RECT{0, 0, w, h};
    props.fVisible = TRUE;
    props.opacity = (BYTE)std::lround(255 * (0.35f + 0.65f * e));
    props.fSourceClientAreaOnly = FALSE;
    DwmUpdateThumbnailProperties(g_thumb, &props);

    g_previewDrawRect = gp::RectF((float)(x - g_lay.mon.left),
                                  (float)(y - g_lay.mon.top), (float)w,
                                  (float)h);
    return t < 1.f;
}

static void OpenPreview(int index) {
    if (index < 0 || index >= (int)g_items.size()) {
        ClosePreview();
        return;
    }
    HWND src = g_items[index].hwnd;
    if (src == g_thumbSource && g_thumb) {
        return;
    }
    ClosePreview();
    if (!g_settings.showPreview || !g_previewHwnd || !IsWindow(src) ||
        IsIconic(src)) {
        return;  // minimized windows have nothing to show
    }
    if (FAILED(DwmRegisterThumbnail(g_previewHwnd, src, &g_thumb))) {
        g_thumb = nullptr;
        return;
    }
    SIZE srcSize{};
    if (FAILED(DwmQueryThumbnailSourceSize(g_thumb, &srcSize)) ||
        srcSize.cx <= 0 || srcSize.cy <= 0) {
        ClosePreview();
        return;
    }
    g_thumbSource = src;

    float s = g_lay.scale;
    float maxW = g_settings.previewSize * s;
    float maxH = maxW * 0.62f;
    float k = std::min({maxW / srcSize.cx, maxH / srcSize.cy, 1.f});
    float w = srcSize.cx * k, h = srcSize.cy * k;

    // Put the preview outside the ring, in the direction of the sector.
    int n = (int)g_items.size();
    float a = (-90.f + index * 360.f / n) * kPi / 180.f;
    float dx = cosf(a), dy = sinf(a);
    float ring = g_lay.outer + 26 * s;
    float dist = ring + fabsf(dx) * w / 2 + fabsf(dy) * h / 2;
    float cx = g_lay.mon.left + g_lay.cx + dx * dist;
    float cy = g_lay.mon.top + g_lay.cy + dy * dist;

    const RECT& wk = g_lay.work;
    float m = 10 * s;
    float x = std::clamp(cx - w / 2, wk.left + m, std::max(wk.left + m, wk.right - m - w));
    float y = std::clamp(cy - h / 2, wk.top + m, std::max(wk.top + m, wk.bottom - m - h));

    g_previewFinal = RECT{(LONG)std::lround(x), (LONG)std::lround(y),
                          (LONG)std::lround(x + w), (LONG)std::lround(y + h)};
    g_previewDirX = dx;
    g_previewDirY = dy;
    g_previewAt = GetTickCount64();
    g_previewShown = true;

    UpdatePreviewAnim();
    ShowWindow(g_previewHwnd, SW_SHOWNOACTIVATE);
}

// ---------------------------------------------------------------------------
// Rendering. Returns true while anything is still animating.
// ---------------------------------------------------------------------------

static void DrawIcon(gp::Graphics& g, const IconImage& icon, const gp::RectF& r,
                     float alpha) {
    if (alpha >= 0.999f) {
        g.DrawImage(icon.bmp.get(), r);
        return;
    }
    gp::ColorMatrix cm = {{{1, 0, 0, 0, 0},
                           {0, 1, 0, 0, 0},
                           {0, 0, 1, 0, 0},
                           {0, 0, 0, alpha, 0},
                           {0, 0, 0, 0, 1}}};
    gp::ImageAttributes ia;
    ia.SetColorMatrix(&cm);
    g.DrawImage(icon.bmp.get(), r, 0, 0, (float)icon.w, (float)icon.h,
                gp::UnitPixel, &ia);
}

static bool Render() {
    const Layout& L = g_lay;
    if (!g_bits || !g_hwnd) {
        return false;
    }
    RefreshAccentColor();
    const float s = L.scale;
    const float ms = AnimMs(g_animStart);
    const int n = (int)g_items.size();
    const float step = n > 0 ? 360.f / n : 360.f;
    bool animating = ms < 700.f;

    // Smoothly follow the selection.
    if ((int)g_hover.size() != n) {
        g_hover.assign(n, 0.f);
    }
    for (int i = 0; i < n; i++) {
        float target = (i == g_sel) ? 1.f : 0.f;
        float& h = g_hover[i];
        if (!g_settings.animate) {
            h = target;
        } else {
            h += (target - h) * 0.3f;
            if (fabsf(target - h) < 0.005f) {
                h = target;
            } else {
                animating = true;
            }
        }
    }
    if (n > 0) {
        float target = -90.f + g_sel * step;
        float diff = NormalizeAngle(target - g_pointerAngle);
        if (!g_settings.animate || fabsf(diff) < 0.2f) {
            g_pointerAngle = target;
        } else {
            g_pointerAngle += diff * 0.3f;
            animating = true;
        }
    }
    if (g_textIndex != g_sel) {
        g_textIndex = g_sel;
        g_textChangeAt = GetTickCount64();
    }
    float textT = Clamp01(AnimMs(g_textChangeAt) / 200.f);
    if (textT < 1.f) {
        animating = true;
    }

    float fade = EaseOutCubic(ms / 160.f);

    {
        gp::Bitmap canvas(L.width, L.height, L.width * 4,
                          PixelFormat32bppPARGB, (BYTE*)g_bits);
        gp::Graphics g(&canvas);
        g.Clear(gp::Color((BYTE)g_settings.dimOpacity, 0, 0, 0));
        g.SetSmoothingMode(gp::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(gp::PixelOffsetModeHalf);
        g.SetInterpolationMode(gp::InterpolationModeHighQualityBicubic);
        g.SetCompositingQuality(gp::CompositingQualityHighQuality);
        g.SetTextRenderingHint(gp::TextRenderingHintAntiAlias);

        // Soft shadow + frame behind the live preview window.
        if (g_previewShown) {
            float rad = 10 * s;
            for (int k = 4; k >= 1; k--) {
                gp::RectF r = g_previewDrawRect;
                r.Inflate(k * 4 * s, k * 4 * s);
                r.Offset(0, 4 * s);
                gp::GraphicsPath sp;
                AddRoundRect(sp, r, rad + k * 4 * s);
                gp::SolidBrush sb(gp::Color(22, 0, 0, 0));
                g.FillPath(&sb, &sp);
            }
            gp::RectF fr = g_previewDrawRect;
            fr.Inflate(3 * s, 3 * s);
            gp::GraphicsPath fp;
            AddRoundRect(fp, fr, rad + 3 * s);
            gp::Pen framePen(GetAccentColor(200), 2.f * s);
            g.DrawPath(&framePen, &fp);
        }

        g.TranslateTransform(L.cx, L.cy);
        float ringRot = -40.f * (1.f - EaseOutCubic(ms / 450.f));

        float gap = 8 * s, corner = 12 * s;
        gp::Color normalFill(44, 255, 255, 255), normalEdge(70, 255, 255, 255);
        gp::Color selFill = GetAccentColor(185), selEdge(170, 255, 255, 255);

        if (n > 0) {
            float midR = (L.inner + L.outer) / 2;
            float maxIcon = std::min(L.icon, (L.outer - L.inner) * 0.6f);
            if (n > 1) {
                maxIcon = std::min(maxIcon,
                                   2 * midR * sinf(step * kPi / 360.f) * 0.7f);
            }
            float stagger = std::min(28.f, 220.f / n);

            // Unselected sectors first, the highlighted one on top.
            for (int pass = 0; pass < 2; pass++) {
                for (int i = 0; i < n; i++) {
                    bool front = g_hover[i] > 0.5f;
                    if (front != (pass == 1)) continue;

                    float local = Clamp01((ms - 40.f - i * stagger) / 260.f);
                    if (local <= 0.f) continue;
                    float pop = EaseOutBack(local);
                    float appear = EaseOutCubic(local);
                    float h = g_hover[i];

                    gp::GraphicsState st = g.Save();
                    float k = 0.55f + 0.45f * pop;
                    g.ScaleTransform(k, k);

                    float a = -90.f + i * step + ringRot;
                    float ro = L.outer + 12 * s * h;
                    float ri = L.inner + 2 * s * h;

                    gp::GraphicsPath path;
                    if (n == 1) {
                        path.AddEllipse(-ro, -ro, 2 * ro, 2 * ro);
                        path.AddEllipse(-ri, -ri, 2 * ri, 2 * ri);
                    } else {
                        BuildSectorPath(path, ri, ro, a - step / 2,
                                        a + step / 2, gap, corner);
                    }

                    if (h > 0.01f) {
                        gp::Pen glow(GetAccentColor((BYTE)std::lround(70 * h * appear)),
                                     10.f * s);
                        glow.SetLineJoin(gp::LineJoinRound);
                        g.DrawPath(&glow, &path);
                    }
                    gp::SolidBrush brush(MixColor(normalFill, selFill, h, appear));
                    g.FillPath(&brush, &path);
                    gp::Pen pen(MixColor(normalEdge, selEdge, h, appear), 1.2f * s);
                    g.DrawPath(&pen, &path);

                    const Item& item = g_items[i];
                    if (item.icon && item.icon->bmp) {
                        gp::PointF c = Polar((ri + ro) / 2, a);
                        float sz = maxIcon * (1.f + 0.2f * h);
                        DrawIcon(g, *item.icon,
                                 gp::RectF(c.X - sz / 2, c.Y - sz / 2, sz, sz),
                                 appear);
                    }
                    g.Restore(st);
                }
            }
        }

        // Center disc.
        float cpop = EaseOutBack((ms - 60.f) / 300.f);
        if (cpop > 0.f) {
            float cr = (L.inner - 14 * s) * (0.6f + 0.4f * cpop);
            gp::SolidBrush discBrush(gp::Color(160, 20, 20, 24));
            g.FillEllipse(&discBrush, -cr, -cr, 2 * cr, 2 * cr);
            gp::Pen discPen(gp::Color(70, 255, 255, 255), 1.2f * s);
            g.DrawEllipse(&discPen, -cr, -cr, 2 * cr, 2 * cr);

            gp::Pen gloss(gp::Color(140, 255, 255, 255), 3 * s);
            gloss.SetStartCap(gp::LineCapRound);
            gloss.SetEndCap(gp::LineCapRound);
            float gr = cr - 10 * s;
            g.DrawArc(&gloss, -gr, -gr, 2 * gr, 2 * gr, 205.f, 35.f);

            // Pointer arc that glides toward the selected sector.
            if (n > 0) {
                gp::Pen pointer(GetAccentColor(255), 4 * s);
                pointer.SetStartCap(gp::LineCapRound);
                pointer.SetEndCap(gp::LineCapRound);
                float pr = cr - 1 * s;
                float sweep = std::min(46.f, step * 0.7f);
                g.DrawArc(&pointer, -pr, -pr, 2 * pr, 2 * pr,
                          g_pointerAngle + ringRot - sweep / 2, sweep);
            }

            // Application name only.
            std::wstring name;
            if (n > 0 && g_sel >= 0 && g_sel < n) {
                name = g_items[g_sel].name;
            } else {
                name = L"No open windows";
            }
            float te = EaseOutCubic(textT);
            float tw = cr * 1.6f;
            float yOff = (1.f - te) * 10 * s;
            gp::Font font(L"Segoe UI", 21 * s * (0.6f + 0.4f * cpop),
                          gp::FontStyleBold, gp::UnitPixel);
            gp::SolidBrush textBrush(
                gp::Color((BYTE)std::lround(255 * te), 255, 255, 255));
            gp::StringFormat sf;
            sf.SetAlignment(gp::StringAlignmentCenter);
            sf.SetLineAlignment(gp::StringAlignmentCenter);
            sf.SetTrimming(gp::StringTrimmingEllipsisCharacter);
            g.DrawString(name.c_str(), (INT)name.size(), &font,
                         gp::RectF(-tw / 2, -cr * 0.6f + yOff, tw, cr * 1.2f),
                         &sf, &textBrush);
        }
        g.Flush(gp::FlushIntentionSync);
    }

    BLENDFUNCTION bf{AC_SRC_OVER, 0, (BYTE)std::lround(255 * fade), AC_SRC_ALPHA};
    POINT dst{L.mon.left, L.mon.top};
    SIZE size{L.width, L.height};
    POINT src{0, 0};
    UpdateLayeredWindow(g_hwnd, nullptr, &dst, &size, g_memDC, &src, 0, &bf,
                        ULW_ALPHA);
    return animating;
}

static void Frame() {
    bool a = UpdatePreviewAnim();
    bool b = Render();
    g_needFrame = a || b;
    g_dirty = false;
}

// ============================================================================
// Switching logic
// ============================================================================

static void SendDummyKey() {
    // An unassigned key between Alt down/up stops apps from opening their
    // menu bar on Alt release, and grants us foreground rights.
    INPUT in[2]{};
    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wVk = 0xE8;
    in[0].ki.dwExtraInfo = kInjectedMagic;
    in[1] = in[0];
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

static void ForceForeground(HWND h) {
    if (IsIconic(h)) {
        ShowWindowAsync(h, SW_RESTORE);
    }
    SendDummyKey();  // makes this process the source of the last input
    SetForegroundWindow(h);
}

static void HideSwitcher() {
    g_visible = false;
    g_pending = false;
    ClosePreview();
    if (g_hwnd) {
        KillTimer(g_hwnd, kTimerId);
        ShowWindow(g_hwnd, SW_HIDE);
    }
    FreeDib();
}

static void ActivateSelected() {
    HWND target = nullptr;
    if (g_sel >= 0 && g_sel < (int)g_items.size()) {
        target = g_items[g_sel].hwnd;
    }
    HideSwitcher();
    if (target && IsWindow(target)) {
        ForceForeground(target);
    }
}

static void SetSelection(int idx) {
    int n = (int)g_items.size();
    if (n == 0) {
        return;
    }
    idx = ((idx % n) + n) % n;
    if (idx == g_sel) {
        return;
    }
    g_sel = idx;
    OpenPreview(idx);
    g_dirty = true;
}

static void ShowSwitcher(bool reverse) {
    SendDummyKey();  // stops the Alt-release from opening app menu bars
    // g_pending stays true until g_visible is set, so the hook keeps
    // swallowing Tab while icons are being collected.
    ComputeLayout();
    BuildItems((int)std::lround(g_lay.icon * 1.25f));

    int n = (int)g_items.size();
    g_sel = 0;
    if (n > 1) {
        HWND fg = GetForegroundWindow();
        HWND fgRoot = fg ? GetAncestor(fg, GA_ROOTOWNER) : nullptr;
        bool firstIsActive =
            g_items[0].hwnd == fg || g_items[0].hwnd == fgRoot;
        if (reverse) {
            g_sel = n - 1;
        } else {
            g_sel = firstIsActive ? 1 : 0;
        }
    }

    if (!EnsureDib(g_lay.width, g_lay.height)) {
        Wh_Log(L"Failed to allocate DIB");
        g_pending = false;
        return;
    }

    g_visible = true;
    g_pending = false;
    g_animStart = GetTickCount64();
    g_hover.assign(n, 0.f);
    g_pointerAngle = n > 0 ? -90.f + g_sel * 360.f / n : -90.f;
    g_textIndex = -1;
    ClosePreview();

    Frame();
    SetWindowPos(g_hwnd, HWND_TOPMOST, g_lay.mon.left, g_lay.mon.top,
                 g_lay.width, g_lay.height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    OpenPreview(g_sel);
    SetTimer(g_hwnd, kTimerId, 10, nullptr);
}

static void MoveSelection(int delta) {
    if (!g_visible || g_items.empty()) {
        return;
    }
    SetSelection(g_sel + delta);
    Frame();
}

static void OnTimer() {
    if (!g_visible) {
        return;
    }

    // Safety net: if Alt was released and we somehow missed the key-up.
    // Ask Windows for the real key state: the keyboard hook can't see keys
    // while an elevated window (e.g. Windhawk itself) is in the foreground.
    // Requires Alt to be up on several checks in a row (~60 ms), so a single
    // stale reading can't close the menu.
    static int altUpTicks = 0;
    bool altDown = (GetAsyncKeyState(VK_MENU) & 0x8000) ||
                   (GetAsyncKeyState(VK_LMENU) & 0x8000) ||
                   (GetAsyncKeyState(VK_RMENU) & 0x8000);
    altUpTicks = altDown ? 0 : altUpTicks + 1;
    if (g_settings.activateOnRelease &&
        GetTickCount64() - g_animStart > 250 && altUpTicks >= 6) {
        altUpTicks = 0;
        ActivateSelected();
        return;
    }

    if (g_needFrame || g_dirty) {
        Frame();
    }
}

// ============================================================================
// Keyboard hook (tool process)
// ============================================================================

static bool KeyDown(int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; }

static LRESULT CALLBACK KeyboardProc(int code, WPARAM wp, LPARAM lp) {
    HWND hwnd = g_hwnd;
    if (code != HC_ACTION || !hwnd) {
        return CallNextHookEx(nullptr, code, wp, lp);
    }
    auto* k = reinterpret_cast<KBDLLHOOKSTRUCT*>(lp);
    if (k->dwExtraInfo == kInjectedMagic) {
        return CallNextHookEx(nullptr, code, wp, lp);
    }

    bool down = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
    bool up = wp == WM_KEYUP || wp == WM_SYSKEYUP;
    DWORD vk = k->vkCode;
    const DWORD triggerVk = g_triggerVk;
    bool isTrigger = triggerVk ? vk == triggerVk
                               : (k->scanCode == g_triggerScan &&
                                  !(k->flags & LLKHF_EXTENDED));
    bool isAlt = vk == VK_MENU || vk == VK_LMENU || vk == VK_RMENU;
    bool isModifier = isAlt || vk == VK_SHIFT || vk == VK_LSHIFT ||
                      vk == VK_RSHIFT || vk == VK_CONTROL ||
                      vk == VK_LCONTROL || vk == VK_RCONTROL ||
                      vk == VK_LWIN || vk == VK_RWIN;
    if (isAlt) {
        if (down) {
            g_altHeld = true;
        } else if (up) {
            g_altHeld = false;
        }
    }

    bool active = g_visible || g_pending;

    if (!active) {
        // Keep this path instant: no logging, no SendInput, no window
        // lookups.
        if (down && isTrigger) {
            bool alt = g_altHeld || (k->flags & LLKHF_ALTDOWN) ||
                       KeyDown(VK_MENU);
            bool otherMods = KeyDown(VK_CONTROL) || KeyDown(VK_LWIN) ||
                             KeyDown(VK_RWIN);
            if (alt && !otherMods) {
                g_pending = true;
                PostMessageW(hwnd, WM_RS_SHOW, KeyDown(VK_SHIFT) ? 1 : 0, 0);
                return 1;
            }
        }
        return CallNextHookEx(nullptr, code, wp, lp);
    }

    // Menu is open (or about to be).
    if (isAlt) {
        if (up && g_activateOnRelease) {
            PostMessageW(hwnd, WM_RS_ACTIVATE, 0, 0);
        }
        return CallNextHookEx(nullptr, code, wp, lp);
    }
    if (isModifier) {
        return CallNextHookEx(nullptr, code, wp, lp);
    }
    if (down) {
        if (isTrigger) {
            PostMessageW(hwnd, KeyDown(VK_SHIFT) ? WM_RS_PREV : WM_RS_NEXT, 0,
                         0);
        } else if (vk == VK_ESCAPE) {
            PostMessageW(hwnd, WM_RS_CANCEL, 0, 0);
        } else if (vk == VK_RIGHT || vk == VK_DOWN) {
            PostMessageW(hwnd, WM_RS_NEXT, 0, 0);
        } else if (vk == VK_LEFT || vk == VK_UP) {
            PostMessageW(hwnd, WM_RS_PREV, 0, 0);
        } else if (vk == VK_RETURN || vk == VK_SPACE) {
            PostMessageW(hwnd, WM_RS_ACTIVATE, 0, 0);
        }
    }
    return 1;  // swallow everything else while the menu is open
}

// Installs a fresh hook first, then removes the old one, so there is no gap.
// Called once at startup, and again only when Windows seems to have dropped
// the hook (see OnForwardedHotkey): a new hook goes ahead of the ones other
// programs installed after it, so it isn't re-installed needlessly.
static void InstallKeyboardHook() {
    HHOOK fresh = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, g_hInst, 0);
    if (!fresh) {
        Wh_Log(L"SetWindowsHookEx(WH_KEYBOARD_LL) failed: %u", GetLastError());
        return;
    }
    HHOOK old = g_hook;
    g_hook = fresh;
    if (old) {
        UnhookWindowsHookEx(old);
    }
}

// The hook runs on its own thread that does nothing else, so it always answers
// quickly and Windows doesn't drop it for exceeding LowLevelHooksTimeout.
static DWORD WINAPI HookThreadProc(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // create queue
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    InstallKeyboardHook();
    SetEvent(g_hookReadyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_RS_REHOOK) {
            Wh_Log(L"Installing the keyboard hook again");
            InstallKeyboardHook();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = nullptr;
    }
    return 0;
}

static DWORD GetIntegrityLevel(HANDLE process) {
    HANDLE token = nullptr;
    if (!OpenProcessToken(process, TOKEN_QUERY, &token)) {
        return 0;
    }
    DWORD level = 0;
    BYTE buffer[sizeof(TOKEN_MANDATORY_LABEL) + SECURITY_MAX_SID_SIZE];
    DWORD size = 0;
    if (GetTokenInformation(token, TokenIntegrityLevel, buffer, sizeof(buffer),
                            &size)) {
        auto* label = reinterpret_cast<TOKEN_MANDATORY_LABEL*>(buffer);
        PSID sid = label->Label.Sid;
        level = *GetSidSubAuthority(sid, *GetSidSubAuthorityCount(sid) - 1);
    }
    CloseHandle(token);
    return level;
}

// True if the window belongs to a process this one can't see the keys of
// (e.g. an elevated app).
static bool IsAboveThisProcess(HWND hwnd) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return true;
    }
    DWORD level = GetIntegrityLevel(process);
    CloseHandle(process);
    static const DWORD ownLevel = GetIntegrityLevel(GetCurrentProcess());
    return !level || level > ownLevel;
}

// Explorer received the Alt+Tab hotkey and handed it over: the keyboard hook
// didn't see the keys, usually because an elevated window is in front.
static void OnForwardedHotkey(bool shift) {
    if (g_visible || g_pending) {
        MoveSelection(shift ? -1 : 1);
        return;
    }
    // Otherwise the hook should have seen the keys: Windows may have dropped
    // it, which would leave every Alt+Tab to this path from now on.
    HWND foreground = GetForegroundWindow();
    if (foreground && !IsAboveThisProcess(foreground)) {
        PostThreadMessageW(g_hookThreadId, WM_RS_REHOOK, 0, 0);
    }
    g_pending = true;
    ShowSwitcher(shift);
}

static void PublishState() {
    if (g_hwnd) {
        SetWindowTextW(g_hwnd, g_settings.trigger == Trigger::AltTab
                                   ? kTitleReady
                                   : kTitleIdle);
    }
}

// ============================================================================
// Window & thread (tool process)
// ============================================================================

static LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_hotkeyMessage && g_hotkeyMessage) {
        if (g_settings.trigger == Trigger::AltTab) {
            OnForwardedHotkey(wp != 0);
        }
        return 0;
    }
    switch (msg) {
        case WM_RS_SHOW:
            if (g_visible) {
                MoveSelection(wp ? -1 : 1);
            } else {
                ShowSwitcher(wp != 0);
            }
            return 0;
        case WM_RS_NEXT:
            MoveSelection(1);
            return 0;
        case WM_RS_PREV:
            MoveSelection(-1);
            return 0;
        case WM_RS_ACTIVATE:
            if (g_visible) {
                ActivateSelected();
            }
            return 0;
        case WM_RS_CANCEL:
            if (g_visible) {
                HideSwitcher();
            }
            return 0;
        case WM_RS_RELOAD:
            if (g_visible) HideSwitcher();
            LoadSettings();
            g_appCache.clear();
            PublishState();
            return 0;
        case WM_RS_PREWARM:
            if (!g_visible && !g_pending) {
                ComputeLayout();
                BuildItems((int)std::lround(g_lay.icon * 1.25f));
                g_items.clear();
            }
            return 0;
        case WM_RS_QUIT:
            HideSwitcher();
            DestroyWindow(h);
            return 0;
        case WM_TIMER:
            if (wp == kTimerId) {
                OnTimer();
            }
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_DESTROY:
            g_hwnd = nullptr;
            g_previewHwnd = nullptr;  // owned window, destroyed with us
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

// Host window for the live DWM thumbnail. Ignores the mouse entirely.
static LRESULT CALLBACK PreviewProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_ERASEBKGND: {
            RECT rc;
            GetClientRect(h, &rc);
            FillRect((HDC)wp, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            return 1;
        }
    }
    return DefWindowProcW(h, msg, wp, lp);
}

static DWORD WINAPI ThreadProc(LPVOID) {
    gp::GdiplusStartupInput gdipInput;
    ULONG_PTR gdipToken = 0;
    gp::GdiplusStartup(&gdipToken, &gdipInput, nullptr);

    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kClassName;
    RegisterClassExW(&wc);

    WNDCLASSEXW pwc{sizeof(pwc)};
    pwc.lpfnWndProc = PreviewProc;
    pwc.hInstance = g_hInst;
    pwc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    pwc.lpszClassName = kPreviewClassName;
    RegisterClassExW(&pwc);

    // WS_EX_TRANSPARENT: the menu never catches mouse clicks.
    g_hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
                                 WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                             kClassName, kTitleIdle, WS_POPUP, 0, 0, 1, 1,
                             nullptr, nullptr, g_hInst, nullptr);

    if (g_hwnd) {
        // Explorer runs at a higher integrity level than this process may, so
        // let its forwarded message through UIPI.
        ChangeWindowMessageFilterEx(g_hwnd, g_hotkeyMessage, MSGFLT_ALLOW,
                                    nullptr);

        // Owned by the menu window, so it always stays above it.
        g_previewHwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                WS_EX_TRANSPARENT,
            kPreviewClassName, L"", WS_POPUP, 0, 0, 1, 1, g_hwnd, nullptr,
            g_hInst, nullptr);
        if (g_previewHwnd) {
            DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
            DwmSetWindowAttribute(g_previewHwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                                  &corner, sizeof(corner));
        }

        // Load icons and app names in advance so the first Alt+Tab is fast.
        PostMessageW(g_hwnd, WM_RS_PREWARM, 0, 0);

        g_hookReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0,
                                    &g_hookThreadId);
        if (g_hookThread) {
            // Always set by HookThreadProc right after installing the hook.
            WaitForSingleObject(g_hookReadyEvent, INFINITE);
        }
        PublishState();
    } else {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
    }
    SetEvent(g_readyEvent);

    if (g_hwnd) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (g_hookThread) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }
    if (g_hookReadyEvent) {
        CloseHandle(g_hookReadyEvent);
        g_hookReadyEvent = nullptr;
    }
    if (g_hwnd) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }
    FreeDib();
    g_items.clear();
    g_appCache.clear();
    g_defaultIcon.reset();
    UnregisterClassW(kClassName, g_hInst);
    UnregisterClassW(kPreviewClassName, g_hInst);
    gp::GdiplusShutdown(gdipToken);
    return 0;
}

BOOL WhTool_ModInit() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&ThreadProc), &g_hInst);

    LoadSettings();

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (HWND hwnd = g_hwnd) {
        PostMessageW(hwnd, WM_RS_RELOAD, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_thread) {
        WaitForSingleObject(g_readyEvent, INFINITE);  // always set by ThreadProc
        if (HWND hwnd = g_hwnd) {
            PostMessageW(hwnd, WM_RS_QUIT, 0, 0);
        }
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}

// ============================================================================
// Explorer's Alt+Tab hotkey (explorer.exe)
// ============================================================================
//
// The standard switcher is started by a hotkey that Explorer registers for
// itself. The keyboard hook in the tool process normally swallows Tab before
// that, but low-level hooks don't see keys while an elevated window is in
// front. Hotkeys are delivered in every case, so Explorer's "Immersive Shell"
// thread (the one that receives the hotkey) is watched, and the hotkey is
// handed to the switcher instead of opening the standard one.
//
// This is the only part of the mod that runs inside Explorer.

static bool g_isExplorer = false;
static HHOOK g_messageHook = nullptr;
static HANDLE g_messageHookThread = nullptr;  // to know when it ends
static std::atomic<int> g_messageHookCalls{0};
static HANDLE g_explorerStopEvent = nullptr;
static HANDLE g_explorerThread = nullptr;

// Hands the hotkey to the switcher. Returns false if the switcher isn't
// running or doesn't replace Alt+Tab; the standard switcher then opens.
static bool ForwardHotkey(bool shift) {
    HWND switcher = FindWindowW(kClassName, kTitleReady);
    if (!switcher) {
        return false;
    }
    // Explorer has just received the hotkey, so it may pass the foreground
    // on. The switcher needs it to activate the chosen window.
    DWORD processId = 0;
    GetWindowThreadProcessId(switcher, &processId);
    AllowSetForegroundWindow(processId);
    return PostMessageW(switcher, g_hotkeyMessage, shift ? 1 : 0, 0) != FALSE;
}

static LRESULT CALLBACK GetMessageProc(int code, WPARAM wParam, LPARAM lParam) {
    g_messageHookCalls++;

    auto* msg = reinterpret_cast<MSG*>(lParam);
    if (code == HC_ACTION && wParam == PM_REMOVE && msg &&
        msg->message == WM_HOTKEY && HIWORD(msg->lParam) == VK_TAB) {
        const UINT mods = LOWORD(msg->lParam);
        if ((mods & MOD_ALT) && !(mods & (MOD_CONTROL | MOD_WIN)) &&
            ForwardHotkey((mods & MOD_SHIFT) != 0)) {
            // Explorer gets an empty message instead of the hotkey.
            msg->message = WM_NULL;
        }
    }

    const LRESULT result = CallNextHookEx(nullptr, code, wParam, lParam);
    g_messageHookCalls--;
    return result;
}

// The ID of Explorer's thread that receives the hotkey, or 0.
static DWORD FindAltTabThread() {
    DWORD found = 0;
    const DWORD processId = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    THREADENTRY32 entry{sizeof(entry)};
    for (BOOL more = Thread32First(snapshot, &entry); more && !found;
         more = Thread32Next(snapshot, &entry)) {
        if (entry.th32OwnerProcessID != processId) {
            continue;
        }
        HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE,
                                   entry.th32ThreadID);
        if (!thread) {
            continue;
        }
        PWSTR name = nullptr;
        if (SUCCEEDED(GetThreadDescription(thread, &name)) && name &&
            wcscmp(name, kAltTabThreadName) == 0) {
            found = entry.th32ThreadID;
        }
        if (name) {
            LocalFree(name);
        }
        CloseHandle(thread);
    }
    CloseHandle(snapshot);
    return found;
}

static bool HookAltTabThread() {
    const DWORD threadId = FindAltTabThread();
    if (!threadId) {
        return false;
    }
    HANDLE thread = OpenThread(SYNCHRONIZE, FALSE, threadId);
    if (!thread) {
        return false;
    }
    // A thread of this process: no module (SetWindowsHookEx docs).
    HHOOK hook =
        SetWindowsHookExW(WH_GETMESSAGE, GetMessageProc, nullptr, threadId);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx(WH_GETMESSAGE) failed: %u", GetLastError());
        CloseHandle(thread);
        return false;
    }
    Wh_Log(L"Hooked Explorer's thread %u", threadId);
    g_messageHook = hook;
    g_messageHookThread = thread;
    return true;
}

static void UnhookAltTabThread() {
    if (g_messageHook) {
        UnhookWindowsHookEx(g_messageHook);
        g_messageHook = nullptr;
    }
    if (g_messageHookThread) {
        CloseHandle(g_messageHookThread);
        g_messageHookThread = nullptr;
    }
    // Let calls already running on Explorer's thread leave the module before
    // it gets unloaded. No new calls start after unhooking, so this ends.
    while (g_messageHookCalls > 0) {
        Sleep(1);
    }
}

static DWORD WINAPI ExplorerThreadProc(LPVOID) {
    // Only the shell process (the one with the taskbar) receives the hotkey.
    // Other Explorer processes (separate folder windows) have nothing to do.
    for (;;) {
        if (HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr)) {
            DWORD processId = 0;
            GetWindowThreadProcessId(taskbar, &processId);
            if (processId != GetCurrentProcessId()) {
                return 0;
            }
            break;
        }
        // Explorer is still starting up.
        if (WaitForSingleObject(g_explorerStopEvent, 1000) != WAIT_TIMEOUT) {
            return 0;
        }
    }

    // Hooks the thread, and again if it's ever created anew. Until it's
    // found, it's looked for again, less and less often.
    DWORD interval = 500;
    for (;;) {
        if (!g_messageHook) {
            HookAltTabThread();
        }
        const HANDLE events[] = {g_explorerStopEvent, g_messageHookThread};
        const DWORD result =
            WaitForMultipleObjects(g_messageHook ? 2 : 1, events, FALSE,
                                   g_messageHook ? INFINITE : interval);
        if (result == WAIT_TIMEOUT) {
            interval = std::min<DWORD>(interval * 2, 30000);
        } else if (result == WAIT_OBJECT_0 + 1) {
            // The thread ended (its ID can be reused).
            UnhookAltTabThread();
            interval = 500;
        } else {
            break;
        }
    }

    UnhookAltTabThread();
    return 0;
}

static void StartExplorerPart() {
    // Nothing to block if the switcher doesn't replace Alt+Tab.
    if (g_settings.trigger != Trigger::AltTab) {
        return;
    }
    g_explorerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_explorerThread =
        CreateThread(nullptr, 0, ExplorerThreadProc, nullptr, 0, nullptr);
}

static void StopExplorerPart() {
    if (g_explorerThread) {
        SetEvent(g_explorerStopEvent);
        WaitForSingleObject(g_explorerThread, INFINITE);
        CloseHandle(g_explorerThread);
        g_explorerThread = nullptr;
    }
    if (g_explorerStopEvent) {
        CloseHandle(g_explorerStopEvent);
        g_explorerStopEvent = nullptr;
    }
}

static bool IsExplorerProcess() {
    WCHAR path[MAX_PATH];
    const DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length == ARRAYSIZE(path)) {
        return false;
    }
    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"explorer.exe") == 0;
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
//
// This mod also targets explorer.exe: there, each callback runs the Explorer
// part instead (the lines marked "explorer.exe part").

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    g_hotkeyMessage = RegisterWindowMessageW(kHotkeyMessageName);

    // explorer.exe part.
    if (IsExplorerProcess()) {
        g_isExplorer = true;
        LoadSettings();
        StartExplorerPart();
        return TRUE;
    }

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
    // explorer.exe part.
    if (g_isExplorer) {
        StopExplorerPart();
        LoadSettings();
        StartExplorerPart();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    // explorer.exe part.
    if (g_isExplorer) {
        StopExplorerPart();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
