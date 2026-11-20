// ==WindhawkMod==
// @id              fullscreen-start
// @name            Fullscreen Start (Launchpad style)
// @description     Replaces the Start menu with a fullscreen grid of all apps, with search and an adjustable blurred background
// @version         1.0.0
// @author          Rafael Mendes
// @github          https://github.com/RafaelMendes1407
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lshell32 -lshcore -ldwmapi -ld2d1 -ldwrite -lole32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fullscreen Start (Launchpad style)

![Fullscreen Start](https://raw.githubusercontent.com/RafaelMendes1407/windhawk-mods/screenshots/fullscreen-start.png)

Replaces the Windows Start menu with a screen in the style of the macOS
Launchpad: all installed apps in a grid, with search at the top and a blurred
background.

## Features
- **Fullscreen**: takes the whole work area of the monitor where the mouse
  is, without covering the taskbar, other side bars or the Dock of the
  **Standalone Dock (macOS-like)** mod.
- **All apps**: the same ones as the "All apps" list of Start (programs and
  Store apps), in alphabetical order, with icon and name.
- **Search**: just type. The list is filtered instantly, ignoring case and
  accents; names that start with the text come first.
- **Adjustable blurred background**: configurable strength, opacity and
  color.

## How to open
Everything that would open the Windows Start menu opens this one instead:
- the **Windows** key and **Ctrl+Esc**;
- the **Start button** of the taskbar, or of the Dock of the Standalone Dock
  mod.

The mod hooks the function Explorer uses to show Start
(`XamlLauncher::ShowStartView` in `twinui.pcshell.dll`), so the original
Start doesn't open at all. Windhawk downloads the symbols of that file the
first time the mod loads.

## Keyboard and mouse
- Type to search; **Backspace** deletes, **Ctrl+Backspace** deletes the last
  word, **Ctrl+V** pastes.
- **Arrows** select an app, **Enter** opens the selected one (while
  searching, the first result is already selected).
- **Esc** clears the search; with an empty search, it closes.
- The mouse wheel or **Page Up/Page Down** scroll the list.
- Clicking outside the icons or switching windows closes it.

## Blur
- **Adjustable** (default): the mod takes a screenshot the moment it opens
  and blurs it with the chosen strength. The background stays still while
  Start is open.
- **Acrylic** and **Windows blur**: the effect is applied by DWM composition
  and follows what changes behind it. Windows doesn't allow changing the
  strength of that blur, so the strength setting doesn't apply to them.
- **No blur**: only the color on top.

Changes to the settings apply the next time Start opens.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- blurMode: custom
  $name: Blur type
  $options:
  - custom: Adjustable (uses the strength below)
  - acrylic: Windows acrylic (DWM)
  - blur: Windows blur (DWM)
  - none: No blur
- blurAmount: 60
  $name: Blur strength (0 to 100)
  $description: Only for the adjustable blur. The Windows one has a fixed strength
- backgroundOpacity: 30
  $name: Background color opacity (%)
  $description: 0 leaves only the blur; 100 covers everything with the color
- tintColor: "#000000"
  $name: Background color
  $description: In hexadecimal, like #1E1E2E
- gridIconSize: 72
  $name: Icon size (pixels at 100% scaling)
- gridColumns: 0
  $name: Grid columns
  $description: 0 picks based on the screen size
- winKey: true
  $name: Open with the Windows key
  $description: Also Ctrl+Esc. When off, they open the original Start
- startButton: true
  $name: Open with the Start button
  $description: Of the taskbar and of the Standalone Dock mod, and the other ways Start opens (not the keyboard). When off, they open the original Start
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
// Otherwise the Win32 macro renames ID2D1RenderTarget::DrawText.
#undef DrawText
#include <d2d1_1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

constexpr WCHAR kWindowClass[] = L"WindhawkFullscreenStart";
constexpr WCHAR kBackdropClass[] = L"WindhawkFullscreenStartBackdrop";
constexpr WCHAR kAppsFolder[] = L"shell:AppsFolder\\";

// From the Standalone Dock mod: the edge strip it takes.
constexpr WCHAR kDockClass[] = L"WindhawkMacDock";
constexpr WCHAR kDockPlateClass[] = L"WindhawkMacDockPlate";
constexpr WCHAR kDockEdgeProp[] = L"WindhawkMacDock.Edge";
constexpr WCHAR kDockReservedProp[] = L"WindhawkMacDock.Reserved";

constexpr UINT WM_APP_TOGGLE = WM_APP + 1;
constexpr UINT WM_APP_APPS = WM_APP + 2;  // lParam: new AppList*.
constexpr UINT WM_APP_RELOAD = WM_APP + 3;
constexpr UINT WM_APP_QUIT = WM_APP + 4;

constexpr UINT_PTR kTimerFrame = 1;
constexpr UINT_PTR kTimerFirstLoad = 4;

constexpr GUID kBhidEnumItems = {
    0x94f60519,
    0x2850,
    0x4924,
    {0xaa, 0x5a, 0xd1, 0x5e, 0x84, 0x86, 0x80, 0x39}};

double Now() {
    static LARGE_INTEGER frequency = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
}

std::wstring ToLower(std::wstring text) {
    if (!text.empty()) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE, text.data(),
                      (int)text.size(), text.data(), (int)text.size(),
                      nullptr, nullptr, 0);
    }
    return text;
}

bool EndsWith(const std::wstring& text, const std::wstring& suffix) {
    return text.size() >= suffix.size() &&
           text.compare(text.size() - suffix.size(), suffix.size(), suffix) ==
               0;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class BlurMode { Custom, Acrylic, Blur, None };

struct Settings {
    BlurMode blurMode = BlurMode::Custom;
    int blurAmount = 60;
    float opacity = 0.3f;
    D2D1_COLOR_F tint{0, 0, 0, 1};
    int iconSize = 72;
    int columns = 0;
};

Settings g_settings;
// Also read in the ShowStartView hook, on Explorer's threads.
std::atomic<bool> g_winKeyEnabled{true};
std::atomic<bool> g_startButtonEnabled{true};

std::wstring GetStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

D2D1_COLOR_F ParseColor(std::wstring text, D2D1_COLOR_F fallback) {
    text.erase(std::remove_if(text.begin(), text.end(), iswspace), text.end());
    if (!text.empty() && text[0] == L'#') {
        text.erase(0, 1);
    }
    if (text.size() != 6 ||
        !std::all_of(text.begin(), text.end(), iswxdigit)) {
        return fallback;
    }
    UINT rgb = wcstoul(text.c_str(), nullptr, 16);
    return D2D1::ColorF(((rgb >> 16) & 0xFF) / 255.0f,
                        ((rgb >> 8) & 0xFF) / 255.0f, (rgb & 0xFF) / 255.0f);
}

void LoadSettings() {
    Settings s;
    std::wstring mode = GetStringSetting(L"blurMode");
    s.blurMode = mode == L"acrylic" ? BlurMode::Acrylic
                 : mode == L"blur"  ? BlurMode::Blur
                 : mode == L"none"  ? BlurMode::None
                                    : BlurMode::Custom;
    s.blurAmount = std::clamp(Wh_GetIntSetting(L"blurAmount"), 0, 100);
    s.opacity =
        std::clamp(Wh_GetIntSetting(L"backgroundOpacity"), 0, 100) / 100.0f;
    s.tint = ParseColor(GetStringSetting(L"tintColor"), s.tint);
    s.iconSize = std::clamp(Wh_GetIntSetting(L"gridIconSize"), 24, 192);
    s.columns = std::clamp(Wh_GetIntSetting(L"gridColumns"), 0, 30);
    g_settings = s;
    g_winKeyEnabled = Wh_GetIntSetting(L"winKey") != 0;
    g_startButtonEnabled = Wh_GetIntSetting(L"startButton") != 0;
}

////////////////////////////////////////////////////////////////////////////////
// App list
//
// Comes from the virtual folder "shell:AppsFolder", the same one as the
// "All apps" list of Start: programs with a Start menu shortcut and Store
// apps. It's read on a separate thread, with the icons, and handed ready to the window.

struct App {
    std::wstring name;
    std::wstring target;  // shell:AppsFolder\..., opened with ShellExecute.
    std::vector<UINT32> pixels;  // Square icon, premultiplied BGRA.
    int iconSize = 0;
    ComPtr<ID2D1Bitmap> bitmap;
    UINT generation = 0;
};
using AppList = std::vector<std::shared_ptr<App>>;

// The globals that hold COM objects (D2D bitmaps, factories, text formats) are
// released by the Start thread before it ends (no_destroy): when Explorer
// exits, Wh_ModUninit isn't called, the thread is already gone, and static
// destructors would release them on the exiting thread under the loader lock.
[[clang::no_destroy]] AppList g_apps;
bool g_appsLoaded;
double g_appsTime;
int g_appsIconPixels;

HANDLE g_worker;
std::atomic<bool> g_workerStop;
std::atomic<HWND> g_wnd;

// Normalizes the alpha and enlarges small icons centered in a large image
// (old exes without a 256 px icon), otherwise they would look tiny.
void FinishIcon(std::vector<UINT32> pixels, int width, int height, App& app) {
    if (std::none_of(pixels.begin(), pixels.end(),
                     [](UINT32 p) { return (p >> 24) != 0; })) {
        for (auto& p : pixels) {
            p |= 0xFF000000;
        }
    }
    bool premultiplied =
        std::none_of(pixels.begin(), pixels.end(), [](UINT32 p) {
            UINT a = p >> 24;
            return ((p >> 16) & 0xFF) > a || ((p >> 8) & 0xFF) > a ||
                   (p & 0xFF) > a;
        });
    if (!premultiplied) {
        for (auto& p : pixels) {
            UINT a = p >> 24;
            UINT r = ((p >> 16) & 0xFF) * a / 255;
            UINT g = ((p >> 8) & 0xFF) * a / 255;
            UINT b = (p & 0xFF) * a / 255;
            p = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }

    int left = width, top = height, right = -1, bottom = -1;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if ((pixels[y * width + x] >> 24) > 8) {
                left = std::min(left, x);
                right = std::max(right, x);
                top = std::min(top, y);
                bottom = std::max(bottom, y);
            }
        }
    }
    if (right < 0) {
        return;
    }
    int full = std::max(width, height);
    int content = std::max(right - left + 1, bottom - top + 1);
    int side, cx, cy;
    if (content < full * 0.6f) {
        side = std::min(full, (int)std::ceil(content * 1.08f));
        cx = (left + right + 1) / 2;
        cy = (top + bottom + 1) / 2;
    } else {
        side = full;
        cx = width / 2;
        cy = height / 2;
    }

    app.iconSize = side;
    app.pixels.assign((size_t)side * side, 0);
    int x0 = cx - side / 2, y0 = cy - side / 2;
    for (int y = 0; y < side; y++) {
        int sy = y0 + y;
        if (sy < 0 || sy >= height) {
            continue;
        }
        for (int x = 0; x < side; x++) {
            int sx = x0 + x;
            if (sx >= 0 && sx < width) {
                app.pixels[(size_t)y * side + x] = pixels[sy * width + sx];
            }
        }
    }
}

void LoadAppIcon(IShellItem* item, int size, App& app) {
    ComPtr<IShellItemImageFactory> factory;
    HBITMAP bitmap = nullptr;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&factory))) ||
        FAILED(factory->GetImage(SIZE{size, size},
                                 SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                                 &bitmap))) {
        return;
    }
    BITMAP bm{};
    if (GetObjectW(bitmap, sizeof(bm), &bm) && bm.bmWidth > 0) {
        int width = bm.bmWidth, height = std::abs(bm.bmHeight);
        std::vector<UINT32> pixels((size_t)width * height);
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        HDC dc = GetDC(nullptr);
        int lines = GetDIBits(dc, bitmap, 0, height, pixels.data(), &bi,
                              DIB_RGB_COLORS);
        ReleaseDC(nullptr, dc);
        if (lines) {
            FinishIcon(std::move(pixels), width, height, app);
        }
    }
    DeleteObject(bitmap);
}

// Uninstallers, documents and website links that some programs put in the
// Start menu.
bool ShouldSkip(const std::wstring& name, const std::wstring& parsing) {
    std::wstring lowerName = ToLower(name);
    std::wstring lowerTarget = ToLower(parsing);
    if (lowerName.empty() ||
        lowerTarget.find(L"\\unins0") != std::wstring::npos ||
        lowerTarget.find(L"\\uninst.exe") != std::wstring::npos ||
        lowerTarget.find(L"\\uninstall.exe") != std::wstring::npos ||
        lowerTarget.rfind(L"http://", 0) == 0 ||
        lowerTarget.rfind(L"https://", 0) == 0) {
        return true;
    }
    for (PCWSTR ext : {L".chm", L".txt", L".pdf", L".rtf", L".htm", L".html"}) {
        if (EndsWith(lowerTarget, ext)) {
            return true;
        }
    }
    return false;
}

bool NameLess(const std::wstring& a, const std::wstring& b) {
    return CompareStringEx(LOCALE_NAME_USER_DEFAULT,
                           LINGUISTIC_IGNORECASE | SORT_DIGITSASNUMBERS,
                           a.c_str(), (int)a.size(), b.c_str(), (int)b.size(),
                           nullptr, nullptr, 0) == CSTR_LESS_THAN;
}

AppList* EnumerateApps(int iconPixels) {
    auto list = std::make_unique<AppList>();
    ComPtr<IShellItem> folder;
    ComPtr<IEnumShellItems> items;
    if (FAILED(SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr,
                                           IID_PPV_ARGS(&folder))) ||
        FAILED(folder->BindToHandler(nullptr, kBhidEnumItems,
                                     IID_PPV_ARGS(&items)))) {
        return list.release();
    }
    ComPtr<IShellItem> item;
    while (!g_workerStop &&
           items->Next(1, item.ReleaseAndGetAddressOf(), nullptr) == S_OK) {
        PWSTR name = nullptr, parsing = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) &&
            SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING,
                                           &parsing)) &&
            !ShouldSkip(name, parsing)) {
            auto app = std::make_shared<App>();
            app->name = name;
            app->target = std::wstring(kAppsFolder) + parsing;
            LoadAppIcon(item.Get(), iconPixels, *app);
            list->push_back(std::move(app));
        }
        CoTaskMemFree(name);
        CoTaskMemFree(parsing);
    }

    std::stable_sort(list->begin(), list->end(),
                     [](const auto& a, const auto& b) {
                         return NameLess(a->name, b->name);
                     });
    // The same app may show up twice (user and all-users shortcut).
    list->erase(std::unique(list->begin(), list->end(),
                            [](const auto& a, const auto& b) {
                                return a->name == b->name &&
                                       ToLower(a->target) == ToLower(b->target);
                            }),
                list->end());
    return list.release();
}

DWORD WINAPI LoadAppsThread(void* param) {
    int iconPixels = (int)(INT_PTR)param;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    AppList* list = EnumerateApps(iconPixels);
    CoUninitialize();
    HWND wnd = g_wnd;
    if (g_workerStop || !wnd ||
        !PostMessageW(wnd, WM_APP_APPS, iconPixels, (LPARAM)list)) {
        delete list;
    }
    return 0;
}

// Icons are read at the size of the monitor with the largest scaling.
int IconPixels() {
    UINT maxDpi = 96;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM param) -> BOOL {
            UINT dpiX = 96, dpiY = 96;
            if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX,
                                           &dpiY))) {
                *(UINT*)param = std::max(*(UINT*)param, dpiX);
            }
            return TRUE;
        },
        (LPARAM)&maxDpi);
    return std::min(256, (int)std::ceil(g_settings.iconSize * maxDpi / 96.0));
}

void StartLoadingApps() {
    if (g_worker) {
        if (WaitForSingleObject(g_worker, 0) == WAIT_TIMEOUT) {
            return;  // Already reading.
        }
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    g_appsTime = Now();
    g_worker = CreateThread(nullptr, 0, LoadAppsThread,
                            (void*)(INT_PTR)IconPixels(), 0, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// Search

std::wstring g_query;
std::vector<int> g_visible;  // Indices in g_apps that pass the search.

// 0: the name starts with the text; 1: a word of the name starts with it;
// 2: it's in the middle; -1: it isn't there. Ignoring case and accents.
int MatchRank(const std::wstring& name, const std::wstring& word) {
    int pos = FindNLSStringEx(
        LOCALE_NAME_USER_DEFAULT,
        FIND_FROMSTART | LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC,
        name.c_str(), (int)name.size(), word.c_str(), (int)word.size(),
        nullptr, nullptr, nullptr, 0);
    if (pos < 0) {
        return -1;
    }
    if (pos == 0) {
        return 0;
    }
    return iswalnum(name[pos - 1]) ? 2 : 1;
}

void UpdateFilter() {
    std::vector<std::wstring> words;
    size_t start = 0;
    while (start < g_query.size()) {
        size_t end = g_query.find(L' ', start);
        if (end == std::wstring::npos) {
            end = g_query.size();
        }
        if (end > start) {
            words.push_back(g_query.substr(start, end - start));
        }
        start = end + 1;
    }

    std::vector<std::pair<int, int>> ranked;  // (position, index)
    for (int i = 0; i < (int)g_apps.size(); i++) {
        int rank = 0;
        for (size_t w = 0; w < words.size() && rank >= 0; w++) {
            int r = MatchRank(g_apps[i]->name, words[w]);
            rank = r < 0 ? -1 : (w == 0 ? r : rank);
        }
        if (rank >= 0) {
            ranked.emplace_back(rank, i);
        }
    }
    std::stable_sort(ranked.begin(), ranked.end(),
                     [](const auto& a, const auto& b) {
                         return a.first < b.first;
                     });
    g_visible.clear();
    for (const auto& r : ranked) {
        g_visible.push_back(r.second);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Available area
//
// The monitor's work area already excludes the visible taskbar and the side
// bars that reserve space. The Standalone Dock usually doesn't reserve it (it
// hides by itself), so its strip is removed separately.

RECT AvailableArea(HMONITOR monitor) {
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(monitor, &mi);
    struct Search {
        HMONITOR monitor;
        RECT monitorRect;
        RECT area;
    } search{monitor, mi.rcMonitor, mi.rcWork};

    EnumWindows(
        [](HWND hwnd, LPARAM param) -> BOOL {
            auto* s = (Search*)param;
            WCHAR className[64];
            if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) ||
                wcscmp(className, kDockClass) ||
                MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) != s->monitor) {
                return TRUE;
            }
            int edge = (int)(INT_PTR)GetPropW(hwnd, kDockEdgeProp);
            int reserved = (int)(INT_PTR)GetPropW(hwnd, kDockReservedProp);
            const RECT& m = s->monitorRect;
            RECT& a = s->area;
            switch (edge) {
                case 1:
                    a.bottom = std::min<LONG>(a.bottom, m.bottom - reserved);
                    break;
                case 2:
                    a.left = std::max<LONG>(a.left, m.left + reserved);
                    break;
                case 3:
                    a.right = std::min<LONG>(a.right, m.right - reserved);
                    break;
            }
            return TRUE;
        },
        (LPARAM)&search);

    // A giant Dock can't reduce the area to zero.
    RECT& a = search.area;
    if (a.right - a.left < 200 || a.bottom - a.top < 200) {
        a = mi.rcWork;
    }
    return a;
}

// Puts the Dock windows above Start, which leaves the Dock's strip free.
// The background ones first: the Dock window is owned by one of them and stays above.
void RaiseDockWindows() {
    // Collects before moving: raising a window changes the order FindWindowEx
    // walks, and with two Docks it kept alternating between them endlessly.
    std::vector<HWND> windows;
    for (PCWSTR className : {kDockPlateClass, kDockClass}) {
        HWND hwnd = nullptr;
        while ((hwnd = FindWindowExW(nullptr, hwnd, className, nullptr))) {
            if (IsWindowVisible(hwnd)) {
                windows.push_back(hwnd);
            }
        }
    }
    // Without waiting for the Dock thread: the request goes to its queue.
    for (HWND hwnd : windows) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Drawing

[[clang::no_destroy]] ComPtr<ID2D1Factory> g_d2d;
[[clang::no_destroy]] ComPtr<IDWriteFactory> g_dwrite;
[[clang::no_destroy]] ComPtr<ID2D1DCRenderTarget> g_target;
[[clang::no_destroy]] ComPtr<ID2D1SolidColorBrush> g_brush;
UINT g_targetGeneration = 1;

struct Surface {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ old = nullptr;
    int width = 0, height = 0;
};
Surface g_surface;

void FreeSurface(Surface& s) {
    if (s.dc) {
        SelectObject(s.dc, s.old);
        DeleteObject(s.bitmap);
        DeleteDC(s.dc);
    }
    s = {};
}

bool EnsureSurface(Surface& s, int width, int height) {
    if (s.dc && s.width == width && s.height == height) {
        return true;
    }
    FreeSurface(s);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    s.dc = CreateCompatibleDC(nullptr);
    s.bitmap = CreateDIBSection(s.dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!s.bitmap) {
        DeleteDC(s.dc);
        s = {};
        return false;
    }
    s.old = SelectObject(s.dc, s.bitmap);
    s.width = width;
    s.height = height;
    return true;
}

bool EnsureTarget() {
    if (g_target) {
        return true;
    }
    auto props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96);
    if (FAILED(g_d2d->CreateDCRenderTarget(&props, &g_target))) {
        return false;
    }
    g_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    g_target->CreateSolidColorBrush(D2D1::ColorF(0, 0), &g_brush);
    g_targetGeneration++;
    return true;
}

void ResetTarget() {
    g_brush.Reset();
    g_target.Reset();
}

ID2D1Bitmap* GetBitmap(App& app) {
    if (app.pixels.empty()) {
        return nullptr;
    }
    if (app.generation != g_targetGeneration) {
        app.generation = g_targetGeneration;
        app.bitmap.Reset();
        g_target->CreateBitmap(
            D2D1::SizeU(app.iconSize, app.iconSize), app.pixels.data(),
            app.iconSize * 4,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96, 96),
            &app.bitmap);
    }
    return app.bitmap.Get();
}

D2D1_COLOR_F Alpha(D2D1_COLOR_F c, float a) {
    c.a = a;
    return c;
}

void Fill(const D2D1_RECT_F& r, D2D1_COLOR_F color, float radius = 0) {
    g_brush->SetColor(color);
    if (radius > 0) {
        g_target->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius),
                                       g_brush.Get());
    } else {
        g_target->FillRectangle(r, g_brush.Get());
    }
}

////////////////////////////////////////////////////////////////////////////////
// Background
//
// Adjustable: a screenshot taken before the window appears, downscaled and
// blurred (three box blur passes, which approximate a gaussian blur) and
// stretched back with linear interpolation. Downscaling first makes a
// strong blur cheap.

struct Backdrop {
    std::vector<UINT32> pixels;
    int width = 0, height = 0;  // Downscaled size.
    float sourceWidth = 0, sourceHeight = 0;  // Part used, in downscaled pixels.
    ComPtr<ID2D1Bitmap> bitmap;
    UINT generation = 0;
};
[[clang::no_destroy]] Backdrop g_backdropImage;

void BoxBlurPass(const UINT32* src,
                 UINT32* dst,
                 int lines,
                 int length,
                 int lineStep,
                 int step,
                 int radius) {
    int window = 2 * radius + 1;
    for (int line = 0; line < lines; line++) {
        const UINT32* s = src + (size_t)line * lineStep;
        UINT32* d = dst + (size_t)line * lineStep;
        auto at = [&](int i) {
            return s[(size_t)std::clamp(i, 0, length - 1) * step];
        };
        int sum[3] = {};
        for (int i = -radius; i <= radius; i++) {
            UINT32 p = at(i);
            sum[0] += p & 0xFF;
            sum[1] += (p >> 8) & 0xFF;
            sum[2] += (p >> 16) & 0xFF;
        }
        for (int i = 0; i < length; i++) {
            d[(size_t)i * step] = 0xFF000000 | ((sum[2] / window) << 16) |
                                  ((sum[1] / window) << 8) | (sum[0] / window);
            UINT32 in = at(i + radius + 1), out = at(i - radius);
            sum[0] += (int)(in & 0xFF) - (int)(out & 0xFF);
            sum[1] += (int)((in >> 8) & 0xFF) - (int)((out >> 8) & 0xFF);
            sum[2] += (int)((in >> 16) & 0xFF) - (int)((out >> 16) & 0xFF);
        }
    }
}

void CaptureBackdrop(const RECT& rc, float sigma) {
    g_backdropImage = {};
    int width = rc.right - rc.left, height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0) {
        return;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    HBITMAP bitmap =
        CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(dc);
        ReleaseDC(nullptr, screen);
        return;
    }
    HGDIOBJ old = SelectObject(dc, bitmap);
    // Without CAPTUREBLT, which makes the cursor flicker; the screen composed
    // by DWM already includes layered windows.
    BitBlt(dc, 0, 0, width, height, screen, rc.left, rc.top, SRCCOPY);
    GdiFlush();
    ReleaseDC(nullptr, screen);

    // Downscale by averaging f x f blocks.
    int f = sigma >= 3 ? std::clamp((int)(sigma / 3), 1, 16) : 1;
    int sw = (width + f - 1) / f, sh = (height + f - 1) / f;
    std::vector<UINT32> small((size_t)sw * sh);
    const UINT32* full = (const UINT32*)bits;
    for (int y = 0; y < sh; y++) {
        for (int x = 0; x < sw; x++) {
            int sum[3] = {}, count = 0;
            for (int yy = y * f; yy < std::min(height, (y + 1) * f); yy++) {
                for (int xx = x * f; xx < std::min(width, (x + 1) * f); xx++) {
                    UINT32 p = full[(size_t)yy * width + xx];
                    sum[0] += p & 0xFF;
                    sum[1] += (p >> 8) & 0xFF;
                    sum[2] += (p >> 16) & 0xFF;
                    count++;
                }
            }
            small[(size_t)y * sw + x] = 0xFF000000 |
                                        ((sum[2] / count) << 16) |
                                        ((sum[1] / count) << 8) |
                                        (sum[0] / count);
        }
    }
    SelectObject(dc, old);
    DeleteObject(bitmap);
    DeleteDC(dc);

    // Three box blurs with this width add up to the desired deviation.
    float s = sigma / f;
    int radius = (int)std::lround((std::sqrt(4 * s * s + 1) - 1) / 2);
    if (radius > 0) {
        std::vector<UINT32> temp(small.size());
        for (int pass = 0; pass < 3; pass++) {
            BoxBlurPass(small.data(), temp.data(), sh, sw, sw, 1, radius);
            BoxBlurPass(temp.data(), small.data(), sw, sh, 1, sw, radius);
        }
    }

    g_backdropImage.pixels = std::move(small);
    g_backdropImage.width = sw;
    g_backdropImage.height = sh;
    g_backdropImage.sourceWidth = (float)width / f;
    g_backdropImage.sourceHeight = (float)height / f;
}

ID2D1Bitmap* GetBackdropBitmap() {
    Backdrop& b = g_backdropImage;
    if (b.pixels.empty()) {
        return nullptr;
    }
    if (b.generation != g_targetGeneration) {
        b.generation = g_targetGeneration;
        b.bitmap.Reset();
        g_target->CreateBitmap(
            D2D1::SizeU(b.width, b.height), b.pixels.data(), b.width * 4,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96, 96),
            &b.bitmap);
    }
    return b.bitmap.Get();
}

// Acrylic and Windows blur: a regular window with the DWM effect sits right
// below the layered window where everything is drawn, like in the Dock.

struct AccentPolicy {
    INT accentState;
    INT accentFlags;
    INT gradientColor;
    INT animationId;
};

struct WindowCompositionAttribData {
    DWORD attrib;
    PVOID data;
    SIZE_T size;
};

constexpr DWORD WCA_ACCENT_POLICY = 19;
constexpr INT ACCENT_DISABLED = 0;
constexpr INT ACCENT_ENABLE_BLURBEHIND = 3;
constexpr INT ACCENT_ENABLE_ACRYLICBLURBEHIND = 4;
constexpr DWORD kDwmUseImmersiveDarkMode = 20;
constexpr DWORD kDwmWindowCornerPreference = 33;
constexpr DWORD kDwmBorderColor = 34;

HWND g_backdrop;

void SetAccent(HWND hwnd, INT state, DWORD abgr) {
    using SetWindowCompositionAttribute_t =
        BOOL(WINAPI*)(HWND, WindowCompositionAttribData*);
    static auto setWindowCompositionAttribute =
        (SetWindowCompositionAttribute_t)GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (!setWindowCompositionAttribute) {
        return;
    }
    AccentPolicy policy{state, state ? 2 : 0, (INT)abgr, 0};
    WindowCompositionAttribData data{WCA_ACCENT_POLICY, &policy,
                                     sizeof(policy)};
    setWindowCompositionAttribute(hwnd, &data);
}

void ApplyBackdropEffect() {
    // Makes DWM respect the window's alpha channel, without blur.
    DWM_BLURBEHIND blur{};
    blur.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    blur.fEnable = TRUE;
    blur.hRgnBlur = CreateRectRgn(0, 0, -1, -1);
    DwmEnableBlurBehindWindow(g_backdrop, &blur);
    DeleteObject(blur.hRgnBlur);

    BOOL dark = TRUE;
    DwmSetWindowAttribute(g_backdrop, kDwmUseImmersiveDarkMode, &dark,
                          sizeof(dark));
    UINT corner = 1;  // DWMWCP_DONOTROUND
    DwmSetWindowAttribute(g_backdrop, kDwmWindowCornerPreference, &corner,
                          sizeof(corner));
    COLORREF border = 0xFFFFFFFE;  // DWMWA_COLOR_NONE
    DwmSetWindowAttribute(g_backdrop, kDwmBorderColor, &border,
                          sizeof(border));

    // The color is drawn on the top window, with the chosen opacity; acrylic
    // only requires a nonzero alpha.
    D2D1_COLOR_F c = g_settings.tint;
    DWORD abgr = (1u << 24) | ((DWORD)(c.b * 255) << 16) |
                 ((DWORD)(c.g * 255) << 8) | (DWORD)(c.r * 255);
    switch (g_settings.blurMode) {
        case BlurMode::Acrylic:
            SetAccent(g_backdrop, ACCENT_ENABLE_ACRYLICBLURBEHIND, abgr);
            break;
        case BlurMode::Blur:
            SetAccent(g_backdrop, ACCENT_ENABLE_BLURBEHIND, 0);
            break;
        default:
            SetAccent(g_backdrop, ACCENT_DISABLED, 0);
            break;
    }
}

bool UsesDwmBackdrop() {
    return g_settings.blurMode == BlurMode::Acrylic ||
           g_settings.blurMode == BlurMode::Blur;
}

////////////////////////////////////////////////////////////////////////////////
// Layout

struct Layout {
    RECT area{};  // Window position on screen.
    int width = 0, height = 0;
    float scale = 1;
    float icon = 72, labelHeight = 20, cellWidth = 140, cellHeight = 140;
    float cellPad = 6;
    int columns = 1;
    float gridLeft = 0;
    float viewTop = 0, viewBottom = 0;  // Visible range, in whole rows.
    int visibleRows = 1;
    float contentHeight = 0;
    D2D1_RECT_F search{};
};

Layout g_layout;
float g_scroll, g_scrollTarget;
int g_wheelDelta;  // Wheel accumulated until it completes a row (touchpads).

[[clang::no_destroy]] ComPtr<IDWriteTextFormat> g_labelFormat, g_searchFormat,
    g_messageFormat;
float g_formatScale;

void EnsureFormats(float scale) {
    if (g_labelFormat && g_formatScale == scale) {
        return;
    }
    g_formatScale = scale;
    auto make = [&](float size, DWRITE_FONT_WEIGHT weight,
                    DWRITE_TEXT_ALIGNMENT align,
                    ComPtr<IDWriteTextFormat>& format) {
        format.Reset();
        g_dwrite->CreateTextFormat(L"Segoe UI Variable Text", nullptr, weight,
                                   DWRITE_FONT_STYLE_NORMAL,
                                   DWRITE_FONT_STRETCH_NORMAL, size * scale,
                                   L"", &format);
        if (!format) {
            return;
        }
        format->SetTextAlignment(align);
        format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        ComPtr<IDWriteInlineObject> ellipsis;
        g_dwrite->CreateEllipsisTrimmingSign(format.Get(), &ellipsis);
        format->SetTrimming(&trimming, ellipsis.Get());
    };
    make(12.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
         g_labelFormat);
    make(15, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING,
         g_searchFormat);
    make(16, DWRITE_FONT_WEIGHT_SEMI_LIGHT, DWRITE_TEXT_ALIGNMENT_CENTER,
         g_messageFormat);
}

float MaxScroll() {
    return std::max(0.0f, g_layout.contentHeight -
                              (g_layout.viewBottom - g_layout.viewTop));
}

void ComputeLayout() {
    Layout& L = g_layout;
    float s = L.scale;
    float W = (float)L.width, H = (float)L.height;

    float searchWidth = std::min(320 * s, W - 40 * s);
    float searchTop = std::round(std::max(28 * s, H * 0.045f));
    L.search = D2D1::RectF(std::round((W - searchWidth) / 2), searchTop,
                           std::round((W + searchWidth) / 2),
                           searchTop + std::round(36 * s));

    L.icon = std::round(g_settings.iconSize * s);
    L.labelHeight = std::round(20 * s);
    L.cellPad = std::round(6 * s);
    L.cellHeight = std::round(L.icon + L.labelHeight + 46 * s);
    L.viewTop = L.search.bottom + std::round(28 * s);

    float side = std::max(48 * s, W * 0.07f);
    float available = std::max(L.icon * 2, W - 2 * side);
    float minCell = std::max(L.icon * 1.8f, L.icon + 64 * s);
    L.columns = g_settings.columns > 0
                    ? g_settings.columns
                    : std::clamp((int)(available / minCell), 2, 14);
    // With few columns on a wide screen, the icons don't spread out too much.
    L.cellWidth = std::floor(
        std::min(available / L.columns, std::max(minCell, L.icon * 2.6f)));
    L.gridLeft = std::round((W - L.cellWidth * L.columns) / 2);

    // Only whole rows are visible; scrolling moves row by row.
    L.visibleRows =
        std::max(1, (int)((H - std::round(16 * s) - L.viewTop) / L.cellHeight));
    L.viewBottom = L.viewTop + L.visibleRows * L.cellHeight;

    int rows = ((int)g_visible.size() + L.columns - 1) / L.columns;
    L.contentHeight = rows * L.cellHeight;
    g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll());
    g_scroll = std::clamp(g_scroll, 0.0f, MaxScroll());
}

struct Cell {
    D2D1_RECT_F highlight, icon, label;
};

Cell CellAt(int index) {
    const Layout& L = g_layout;
    int row = index / L.columns, col = index % L.columns;
    float x = L.gridLeft + col * L.cellWidth;
    float y = L.viewTop + row * L.cellHeight - std::round(g_scroll);
    Cell c;
    c.highlight = D2D1::RectF(x + L.cellPad, y, x + L.cellWidth - L.cellPad,
                              y + L.cellHeight - L.cellPad * 2);
    float iconLeft = std::round(x + (L.cellWidth - L.icon) / 2);
    float iconTop = std::round(y + 10 * L.scale);
    c.icon = D2D1::RectF(iconLeft, iconTop, iconLeft + L.icon, iconTop + L.icon);
    float labelTop = c.icon.bottom + std::round(8 * L.scale);
    c.label = D2D1::RectF(x + L.cellPad + 4 * L.scale, labelTop,
                          x + L.cellWidth - L.cellPad - 4 * L.scale,
                          labelTop + L.labelHeight);
    return c;
}

bool InRect(const D2D1_RECT_F& r, float x, float y) {
    return x >= r.left && x < r.right && y >= r.top && y < r.bottom;
}

// Position in the visible list under the point (window coordinates), or -1.
int ItemAt(int px, int py) {
    const Layout& L = g_layout;
    float x = (float)px, y = (float)py;
    if (y < L.viewTop || y >= L.viewBottom || x < L.gridLeft ||
        x >= L.gridLeft + L.cellWidth * L.columns) {
        return -1;
    }
    int col = (int)((x - L.gridLeft) / L.cellWidth);
    int row = (int)((y - L.viewTop + std::round(g_scroll)) / L.cellHeight);
    int index = row * L.columns + col;
    if (row < 0 || index < 0 || index >= (int)g_visible.size() ||
        !InRect(CellAt(index).highlight, x, y)) {
        return -1;
    }
    return index;
}

////////////////////////////////////////////////////////////////////////////////
// Screen state and drawing

bool g_shown;        // The windows are on screen (opening, open or closing).
bool g_closing;
float g_appear;      // 0 closed, 1 open.
double g_lastFrame;
double g_lastClosed;  // When it closed by losing focus.
bool g_frameTimer;
int g_hover = -1, g_selected = -1, g_pressed = -1;
bool g_pressedEmpty;
bool g_trackingMouse;

float Ease(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return 1 - (1 - t) * (1 - t) * (1 - t);
}

void DrawSearchBox() {
    const Layout& L = g_layout;
    float s = L.scale;
    const D2D1_RECT_F& r = L.search;
    float radius = (r.bottom - r.top) / 2;
    Fill(r, D2D1::ColorF(1, 1, 1, 0.14f), radius);
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.22f));
    g_target->DrawRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(r.left + 0.5f, r.top + 0.5f,
                                      r.right - 0.5f, r.bottom - 0.5f),
                          radius, radius),
        g_brush.Get(), 1);

    // Magnifying glass.
    float cx = r.left + 18 * s, cy = (r.top + r.bottom) / 2 - 1 * s;
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.75f));
    g_target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 5.5f * s, 5.5f * s),
                          g_brush.Get(), 1.6f * s);
    g_target->DrawLine(D2D1::Point2F(cx + 4 * s, cy + 4 * s),
                       D2D1::Point2F(cx + 8.5f * s, cy + 8.5f * s),
                       g_brush.Get(), 1.6f * s);

    D2D1_RECT_F text = D2D1::RectF(r.left + 34 * s, r.top, r.right - 14 * s,
                                   r.bottom);
    if (!g_searchFormat) {
        return;
    }
    if (g_query.empty()) {
        g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.55f));
        g_target->DrawText(L"Search", 6, g_searchFormat.Get(), text,
                           g_brush.Get());
    }

    // The typed text, with its end always visible, and the caret after it.
    ComPtr<IDWriteTextLayout> layout;
    g_dwrite->CreateTextLayout(g_query.c_str(), (UINT32)g_query.size(),
                               g_searchFormat.Get(), 10000,
                               text.bottom - text.top, &layout);
    if (!layout) {
        return;
    }
    layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    DWRITE_TEXT_METRICS metrics{};
    layout->GetMetrics(&metrics);
    float textWidth = metrics.widthIncludingTrailingWhitespace;
    float room = text.right - text.left - 3 * s;
    float shift = std::max(0.0f, textWidth - room);
    g_target->PushAxisAlignedClip(text, D2D1_ANTIALIAS_MODE_ALIASED);
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f));
    g_target->DrawTextLayout(D2D1::Point2F(text.left - shift, text.top),
                             layout.Get(), g_brush.Get());
    float caretX = std::round(text.left + textWidth - shift) + 1;
    float caretH = 18 * s;
    float mid = (text.top + text.bottom) / 2;
    Fill(D2D1::RectF(caretX, mid - caretH / 2, caretX + std::max(1.0f, s),
                     mid + caretH / 2),
         D2D1::ColorF(1, 1, 1, 0.9f));
    g_target->PopAxisAlignedClip();
}

void DrawMessage(PCWSTR text) {
    const Layout& L = g_layout;
    if (!g_messageFormat) {
        return;
    }
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.7f));
    g_target->DrawText(text, (UINT32)wcslen(text), g_messageFormat.Get(),
                       D2D1::RectF(0, L.viewTop, (float)L.width,
                                   L.viewTop + 120 * L.scale),
                       g_brush.Get());
}

void DrawGrid() {
    const Layout& L = g_layout;
    float s = L.scale;
    if (!g_appsLoaded && g_apps.empty()) {
        DrawMessage(L"Loading apps…");
        return;
    }
    if (g_visible.empty()) {
        DrawMessage(g_query.empty() ? L"No apps" : L"No apps found");
        return;
    }

    g_target->PushAxisAlignedClip(
        D2D1::RectF(0, L.viewTop, (float)L.width, L.viewBottom),
        D2D1_ANTIALIAS_MODE_ALIASED);
    int firstRow = std::max(0, (int)(g_scroll / L.cellHeight));
    int lastRow =
        (int)((g_scroll + L.viewBottom - L.viewTop) / L.cellHeight) + 1;
    int first = firstRow * L.columns;
    int last = std::min((int)g_visible.size(), (lastRow + 1) * L.columns);
    for (int i = first; i < last; i++) {
        App& app = *g_apps[g_visible[i]];
        Cell c = CellAt(i);
        if (i == g_selected || i == g_hover) {
            Fill(c.highlight,
                 D2D1::ColorF(1, 1, 1, i == g_selected ? 0.18f : 0.10f),
                 12 * s);
        }
        float opacity = i == g_pressed ? 0.6f : 1.0f;
        if (ID2D1Bitmap* bitmap = GetBitmap(app)) {
            g_target->DrawBitmap(bitmap, c.icon, opacity,
                                 D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        } else {
            Fill(c.icon, D2D1::ColorF(1, 1, 1, 0.12f * opacity), 14 * s);
        }
        if (g_labelFormat) {
            // Light shadow so the name reads well over any background.
            D2D1_RECT_F shadow = c.label;
            shadow.top += 1 * s;
            shadow.bottom += 1 * s;
            g_brush->SetColor(D2D1::ColorF(0, 0, 0, 0.45f));
            g_target->DrawText(app.name.c_str(), (UINT32)app.name.size(),
                               g_labelFormat.Get(), shadow, g_brush.Get());
            g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f));
            g_target->DrawText(app.name.c_str(), (UINT32)app.name.size(),
                               g_labelFormat.Get(), c.label, g_brush.Get());
        }
    }
    g_target->PopAxisAlignedClip();

    // Subtle scroll bar, only when there's something to scroll.
    float maxScroll = MaxScroll();
    if (maxScroll > 0) {
        float track = L.viewBottom - L.viewTop;
        float thumb = std::max(40 * s, track * track / L.contentHeight);
        float top = L.viewTop + (track - thumb) * (g_scroll / maxScroll);
        float right = (float)L.width - 10 * s;
        Fill(D2D1::RectF(right - 4 * s, top, right, top + thumb),
             D2D1::ColorF(1, 1, 1, 0.28f), 2 * s);
    }
}

void Render() {
    if (!g_shown || !EnsureTarget()) {
        return;
    }
    const Layout& L = g_layout;
    if (!EnsureSurface(g_surface, L.width, L.height)) {
        return;
    }
    RECT bind{0, 0, L.width, L.height};
    if (FAILED(g_target->BindDC(g_surface.dc, &bind))) {
        return;
    }
    float W = (float)L.width, H = (float)L.height;
    g_target->BeginDraw();
    g_target->SetTransform(D2D1::Matrix3x2F::Identity());
    // Minimum alpha instead of zero: fully transparent pixels of a layered
    // window let the click pass through to what's behind.
    g_target->Clear(D2D1::ColorF(0, 0, 0, 1 / 255.0f));
    if (g_settings.blurMode == BlurMode::Custom) {
        if (ID2D1Bitmap* bitmap = GetBackdropBitmap()) {
            g_target->DrawBitmap(
                bitmap, D2D1::RectF(0, 0, W, H), 1,
                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                D2D1::RectF(0, 0, g_backdropImage.sourceWidth,
                            g_backdropImage.sourceHeight));
        }
    }
    Fill(D2D1::RectF(0, 0, W, H), Alpha(g_settings.tint, g_settings.opacity));

    // When opening, the grid arrives from slightly larger, like in Launchpad.
    float zoom = 1.06f - 0.06f * Ease(g_appear);
    g_target->SetTransform(
        D2D1::Matrix3x2F::Scale(zoom, zoom, D2D1::Point2F(W / 2, H / 2)));
    DrawSearchBox();
    DrawGrid();
    g_target->SetTransform(D2D1::Matrix3x2F::Identity());

    if (g_target->EndDraw() == D2DERR_RECREATE_TARGET) {
        ResetTarget();
        return;
    }
    POINT dst{L.area.left, L.area.top}, src{0, 0};
    SIZE size{L.width, L.height};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, (BYTE)std::lround(255 * Ease(g_appear)),
                        AC_SRC_ALPHA};
    UpdateLayeredWindow(g_wnd, nullptr, &dst, &size, g_surface.dc, &src, 0,
                        &blend, ULW_ALPHA);
}

void StartFrames() {
    if (!g_frameTimer) {
        g_frameTimer = true;
        g_lastFrame = Now();
        SetTimer(g_wnd, kTimerFrame, 10, nullptr);
    }
}

void HideNow() {
    HWND wnd = g_wnd;
    ShowWindow(wnd, SW_HIDE);
    ShowWindow(g_backdrop, SW_HIDE);
    g_shown = false;
    g_closing = false;
    g_appear = 0;
    g_backdropImage = {};
    FreeSurface(g_surface);
    g_trackingMouse = false;
}

void OnFrame() {
    double now = Now();
    float dt = (float)std::clamp(now - g_lastFrame, 0.0, 0.05);
    g_lastFrame = now;
    bool animating = false;

    float target = g_closing ? 0.0f : 1.0f;
    if (g_appear != target) {
        float speed = g_closing ? 1 / 0.14f : 1 / 0.2f;
        g_appear = g_closing ? std::max(0.0f, g_appear - dt * speed)
                             : std::min(1.0f, g_appear + dt * speed);
        animating = g_appear != target;
    }
    if (g_scroll != g_scrollTarget) {
        g_scroll += (g_scrollTarget - g_scroll) * std::min(1.0f, dt * 16);
        if (std::abs(g_scrollTarget - g_scroll) < 0.5f) {
            g_scroll = g_scrollTarget;
        }
        animating |= g_scroll != g_scrollTarget;
    }

    if (g_closing && g_appear <= 0) {
        HideNow();
        animating = false;
    } else {
        Render();
    }
    if (!animating) {
        KillTimer(g_wnd, kTimerFrame);
        g_frameTimer = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Opening and closing

// SetForegroundWindow succeeds when the calling process received the last
// input event; an empty SendInput provides it.
void ForceForeground(HWND hwnd) {
    INPUT input{};
    SendInput(1, &input, sizeof(INPUT));
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
}

void PlaceWindows() {
    const RECT& a = g_layout.area;
    // Always on top: Windows sometimes leaves "always on top" windows (the
    // Dock included) below regular ones, and positioning below the Dock
    // put Start behind the windows.
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    RaiseDockWindows();
    if (UsesDwmBackdrop()) {
        SetWindowPos(g_backdrop, g_wnd, a.left, a.top, a.right - a.left,
                     a.bottom - a.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
}

void Open() {
    if (g_shown) {
        if (g_closing) {  // Reopened in the middle of closing.
            g_closing = false;
            ForceForeground(g_wnd);
            PlaceWindows();
            StartFrames();
        }
        return;
    }

    POINT pt;
    GetCursorPos(&pt);
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

    Layout& L = g_layout;
    L.area = AvailableArea(monitor);
    L.width = L.area.right - L.area.left;
    L.height = L.area.bottom - L.area.top;
    L.scale = dpiX / 96.0f;
    EnsureFormats(L.scale);

    g_query.clear();
    g_hover = g_selected = g_pressed = -1;
    g_scroll = g_scrollTarget = 0;
    g_wheelDelta = 0;
    UpdateFilter();
    ComputeLayout();

    // The background screenshot is taken before any of the mod's windows appear.
    if (g_settings.blurMode == BlurMode::Custom) {
        CaptureBackdrop(L.area, g_settings.blurAmount / 100.0f * 60 * L.scale);
    }
    if (UsesDwmBackdrop()) {
        ApplyBackdropEffect();
    }

    g_shown = true;
    g_closing = false;
    g_appear = 0;
    Render();
    ShowWindow(g_wnd, SW_SHOW);
    ForceForeground(g_wnd);
    PlaceWindows();
    StartFrames();

    if (!g_appsLoaded || Now() - g_appsTime > 60 ||
        g_appsIconPixels != IconPixels()) {
        StartLoadingApps();
    }
}

void Close() {
    if (!g_shown || g_closing) {
        return;
    }
    g_closing = true;
    g_pressed = -1;
    if (GetCapture() == g_wnd) {
        ReleaseCapture();
    }
    StartFrames();
}

void Toggle() {
    if (g_shown && !g_closing) {
        Close();
    } else {
        Open();
    }
}

void LaunchApp(int index) {
    if (index < 0 || index >= (int)g_visible.size()) {
        return;
    }
    const App& app = *g_apps[g_visible[index]];
    SHELLEXECUTEINFOW sei{sizeof(sei)};
    sei.fMask = SEE_MASK_ASYNCOK;
    sei.lpFile = app.target.c_str();
    sei.nShow = SW_SHOWNORMAL;
    AllowSetForegroundWindow(ASFW_ANY);
    ShellExecuteExW(&sei);
    Close();
}

void EnsureVisible(int index) {
    if (index < 0) {
        return;
    }
    const Layout& L = g_layout;
    int row = index / L.columns;
    int firstRow = (int)std::lround(g_scrollTarget / L.cellHeight);
    if (row < firstRow) {
        g_scrollTarget = row * L.cellHeight;
    } else if (row >= firstRow + L.visibleRows) {
        g_scrollTarget = (row - L.visibleRows + 1) * L.cellHeight;
    }
    g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll());
}

void ScrollRows(int rows) {
    float cell = g_layout.cellHeight;
    float target = (std::lround(g_scrollTarget / cell) + rows) * cell;
    g_scrollTarget = std::clamp(target, 0.0f, MaxScroll());
    StartFrames();
}

void QueryChanged() {
    UpdateFilter();
    g_hover = -1;
    g_selected = g_query.empty() || g_visible.empty() ? -1 : 0;
    g_scroll = g_scrollTarget = 0;
    ComputeLayout();
    Render();
}

void PasteText() {
    if (!OpenClipboard(g_wnd)) {
        return;
    }
    if (HANDLE data = GetClipboardData(CF_UNICODETEXT)) {
        if (auto* text = (PCWSTR)GlobalLock(data)) {
            std::wstring line = text;
            GlobalUnlock(data);
            line = line.substr(0, line.find_first_of(L"\r\n"));
            g_query += line.substr(0, 200);
        }
    }
    CloseClipboard();
    QueryChanged();
}

void OnChar(WCHAR ch) {
    switch (ch) {
        case 8:  // Backspace
            if (!g_query.empty()) {
                g_query.pop_back();
                if (!g_query.empty() && IS_HIGH_SURROGATE(g_query.back())) {
                    g_query.pop_back();
                }
                QueryChanged();
            }
            return;
        case 0x7F: {  // Ctrl+Backspace: the last word.
            size_t end = g_query.find_last_not_of(L' ');
            if (end == std::wstring::npos) {
                g_query.clear();
            } else {
                size_t space = g_query.find_last_of(L' ', end);
                g_query.resize(space == std::wstring::npos ? 0 : space + 1);
            }
            QueryChanged();
            return;
        }
        case 0x16:  // Ctrl+V
            PasteText();
            return;
    }
    if (ch >= 32 && g_query.size() < 200) {
        g_query += ch;
        QueryChanged();
    }
}

void MoveSelection(int delta) {
    if (g_visible.empty()) {
        return;
    }
    int count = (int)g_visible.size();
    if (g_selected < 0) {
        g_selected = 0;
    } else {
        int next = g_selected + delta;
        if (next >= 0 && next < count) {
            g_selected = next;
        } else if (delta > 0 && delta == g_layout.columns) {
            g_selected = count - 1;  // Down on the last incomplete row.
        }
    }
    EnsureVisible(g_selected);
    Render();
    StartFrames();
}

void OnKeyDown(WPARAM key) {
    switch (key) {
        case VK_ESCAPE:
            if (!g_query.empty()) {
                g_query.clear();
                QueryChanged();
            } else {
                Close();
            }
            break;
        case VK_RETURN:
            if (g_selected >= 0) {
                LaunchApp(g_selected);
            } else if (!g_query.empty() && !g_visible.empty()) {
                LaunchApp(0);
            }
            break;
        case VK_LEFT:
            MoveSelection(-1);
            break;
        case VK_RIGHT:
            MoveSelection(1);
            break;
        case VK_UP:
            MoveSelection(-g_layout.columns);
            break;
        case VK_DOWN:
            MoveSelection(g_layout.columns);
            break;
        case VK_PRIOR:
            ScrollRows(-g_layout.visibleRows);
            break;
        case VK_NEXT:
            ScrollRows(g_layout.visibleRows);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Opening instead of the Windows Start
//
// Every way of opening Start (Windows key, Ctrl+Esc, the taskbar's Start
// button, WM_SYSCOMMAND/SC_TASKLIST from the Dock) goes through
// XamlLauncher::ShowStartView in twinui.pcshell.dll. The hook asks the mod's
// window to toggle and doesn't call the original, so the Windows Start never
// opens.

// Observed on Windows 11 build 26300: the Windows key and Ctrl+Esc arrive as
// method 1 with flag 8; WM_SYSCOMMAND/SC_TASKLIST (the Dock's Start button)
// also arrives as method 1, without that flag.
constexpr int kShowMethodKeyboard = 1;
constexpr int kShowFlagKeyboard = 0x8;

bool IsKeyboardShow(int showMethod, int showFlags) {
    return showMethod == kShowMethodKeyboard && (showFlags & kShowFlagKeyboard);
}

bool OwnsTaskbar() {
    DWORD pid = 0;
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    return tray && GetWindowThreadProcessId(tray, &pid) &&
           pid == GetCurrentProcessId();
}

using XamlLauncher_ShowStartView_t = HRESULT(WINAPI*)(void* pThis,
                                                      int showMethod,
                                                      int showFlags);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original;
HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis,
                                               int showMethod,
                                               int showFlags) {
    Wh_Log(L"> method %d, flags %d", showMethod, showFlags);
    bool enabled = IsKeyboardShow(showMethod, showFlags)
                       ? g_winKeyEnabled
                       : g_startButtonEnabled;
    HWND wnd = g_wnd;
    if (!enabled || !wnd) {
        return XamlLauncher_ShowStartView_Original(pThis, showMethod,
                                                   showFlags);
    }
    PostMessageW(wnd, WM_APP_TOGGLE, 0, 0);
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// Windows


void OnAppsLoaded(AppList* list, int iconPixels) {
    g_apps = std::move(*list);
    delete list;
    g_appsLoaded = true;
    g_appsIconPixels = iconPixels;
    if (g_shown) {
        UpdateFilter();
        int count = (int)g_visible.size();
        if (g_selected >= count) {
            g_selected = count ? 0 : -1;
        }
        g_hover = g_pressed = -1;
        ComputeLayout();
        Render();
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_APP_TOGGLE:
            // Clicking the taskbar's Start button while this one is open first
            // takes the focus (which closes it) and then asks for Start: that
            // click was meant to close it.
            if ((!g_shown || g_closing) && Now() - g_lastClosed < 0.5) {
                return 0;
            }
            Toggle();
            return 0;

        case WM_APP_APPS:
            OnAppsLoaded((AppList*)lParam, (int)wParam);
            return 0;

        case WM_APP_RELOAD:
            LoadSettings();
            if (g_shown) {
                HideNow();
            }
            if (g_appsIconPixels != IconPixels()) {
                StartLoadingApps();
            }
            return 0;

        case WM_APP_QUIT:
            DestroyWindow(hwnd);
            return 0;

        case WM_TIMER:
            switch (wParam) {
                case kTimerFrame:
                    OnFrame();
                    break;
                case kTimerFirstLoad:
                    // Only in the process that owns the taskbar: short-lived
                    // explorer.exe processes (explorer.exe /select,...) get
                    // the mod too, and exit before they'd need the list.
                    if (OwnsTaskbar()) {
                        KillTimer(hwnd, kTimerFirstLoad);
                        if (!g_appsLoaded) {
                            StartLoadingApps();
                        }
                    }
                    break;
            }
            return 0;

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && g_shown && !g_closing) {
                g_lastClosed = Now();
                Close();
            }
            return 0;

        case WM_MOUSEMOVE: {
            if (!g_trackingMouse) {
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                g_trackingMouse = TrackMouseEvent(&tme);
            }
            int hover = ItemAt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            if (hover != g_hover) {
                g_hover = hover;
                Render();
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            g_trackingMouse = false;
            if (g_hover >= 0) {
                g_hover = -1;
                Render();
            }
            return 0;

        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            SetCapture(hwnd);
            g_pressed = ItemAt(x, y);
            g_pressedEmpty = g_pressed < 0 &&
                             !InRect(g_layout.search, (float)x, (float)y);
            Render();
            return 0;
        }

        case WM_LBUTTONUP: {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (GetCapture() == hwnd) {
                ReleaseCapture();
            }
            int item = ItemAt(x, y);
            int pressed = g_pressed;
            g_pressed = -1;
            if (pressed >= 0 && item == pressed) {
                LaunchApp(item);
            } else if (g_pressedEmpty && item < 0 &&
                       !InRect(g_layout.search, (float)x, (float)y)) {
                Close();  // Clicking on empty space closes, like in Launchpad.
            } else {
                Render();
            }
            g_pressedEmpty = false;
            return 0;
        }

        case WM_MOUSEWHEEL:
            g_wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
            if (std::abs(g_wheelDelta) >= WHEEL_DELTA) {
                ScrollRows(-g_wheelDelta / WHEEL_DELTA);
                g_wheelDelta %= WHEEL_DELTA;
            }
            return 0;

        case WM_KEYDOWN:
            OnKeyDown(wParam);
            return 0;

        case WM_CHAR:
            OnChar((WCHAR)wParam);
            return 0;

        case WM_CLOSE:  // Alt+F4
            Close();
            return 0;

        case WM_DPICHANGED:
            return 0;  // Position and scaling are computed when opening.

        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            if (g_shown && (msg == WM_DISPLAYCHANGE || wParam == SPI_SETWORKAREA)) {
                HideNow();
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK BackdropWndProc(HWND hwnd,
                                 UINT msg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_ERASEBKGND: {
            // Black with zero alpha: only the DWM effect shows.
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect((HDC)wParam, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            return 1;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            FillRect(dc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HANDLE g_thread;
HANDLE g_readyEvent;

DWORD WINAPI StartThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    OleInitialize(nullptr);
    LoadSettings();

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, g_d2d.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        (IUnknown**)g_dwrite.GetAddressOf());

    HINSTANCE instance = (HINSTANCE)&__ImageBase;
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kWindowClass;
    RegisterClassExW(&wc);

    WNDCLASSEXW bc{sizeof(bc)};
    bc.lpfnWndProc = BackdropWndProc;
    bc.hInstance = instance;
    bc.lpszClassName = kBackdropClass;
    RegisterClassExW(&bc);

    g_backdrop = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE |
            WS_EX_TRANSPARENT,
        kBackdropClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance,
        nullptr);
    HWND wnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
                               kWindowClass, L"Start", WS_POPUP, 0, 0, 0, 0,
                               nullptr, nullptr, instance, nullptr);
    bool ok = g_d2d && g_dwrite && g_backdrop && wnd;
    if (ok) {
        g_wnd = wnd;
        // The app list is read a bit later, so it doesn't weigh on explorer's
        // startup.
        SetTimer(wnd, kTimerFirstLoad, 3000, nullptr);
    }
    SetEvent(g_readyEvent);

    if (ok) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    } else {
        Wh_Log(L"Failed to create Start");
        if (wnd) {
            DestroyWindow(wnd);
        }
    }
    g_wnd = nullptr;

    g_workerStop = true;
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    // A list delivered after the window stopped handling messages.
    MSG msg;
    while (PeekMessageW(&msg, nullptr, WM_APP_APPS, WM_APP_APPS, PM_REMOVE)) {
        delete (AppList*)msg.lParam;
    }

    if (g_backdrop) {
        DestroyWindow(g_backdrop);
        g_backdrop = nullptr;
    }
    AppList().swap(g_apps);  // Also frees the memory (no_destroy).
    g_backdropImage = {};
    FreeSurface(g_surface);
    g_labelFormat.Reset();
    g_searchFormat.Reset();
    g_messageFormat.Reset();
    ResetTarget();
    g_dwrite.Reset();
    g_d2d.Reset();
    UnregisterClassW(kWindowClass, instance);
    UnregisterClassW(kBackdropClass, instance);
    OleUninitialize();
    return 0;
}

// With "Launch folder windows in a separate process", folder windows run in
// their own explorer.exe, which never shows Start.
bool IsFolderWindowsProcess() {
    std::wstring commandLine = ToLower(GetCommandLineW());
    return commandLine.find(L"/factory") != std::wstring::npos ||
           commandLine.find(L"-embedding") != std::wstring::npos ||
           commandLine.find(L"/separate") != std::wstring::npos;
}

bool HookShowStartView() {
    HMODULE module = LoadLibraryExW(L"twinui.pcshell.dll", nullptr,
                                    LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return false;
    }
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
            &XamlLauncher_ShowStartView_Original,
            XamlLauncher_ShowStartView_Hook,
        },
    };
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    return true;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (IsFolderWindowsProcess()) {
        Wh_Log(L"Folder windows process; Start stays in the taskbar's one");
        return FALSE;
    }
    LoadSettings();
    if (!HookShowStartView()) {
        return FALSE;
    }
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, StartThread, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (!g_thread) {
        return;
    }
    WaitForSingleObject(g_readyEvent, INFINITE);
    if (HWND wnd = g_wnd) {
        PostMessageW(wnd, WM_APP_QUIT, 0, 0);
    }
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (HWND wnd = g_wnd) {
        PostMessageW(wnd, WM_APP_RELOAD, 0, 0);
    }
}
