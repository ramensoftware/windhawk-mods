// ==WindhawkMod==
// @id              macos-launchpad-plus
// @name            MacOS Launchpad Plus
// @description     Replaces the Start menu with a floating macOS-style grid of apps, with folders, pages, search, custom icons and power menu.
// @version         1.2.0
// @author          Thiago Silva
// @github          https://github.com/brideinflame-rgb
// @homepage        https://github.com/brideinflame-rgb/macos-launchpad
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lshell32 -lshcore -ld2d1 -ldwrite -lole32 -luuid -lwindowscodecs
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MacOS Launchpad Plus

Replaces the Windows Start menu with a floating app grid inspired by the macOS Launchpad.

## Credits
Based on the original **Fullscreen Start (Launchpad style)** mod (`fullscreen-start`) by **Rafael Mendes**:
[github.com/RafaelMendes1407](https://github.com/RafaelMendes1407).
All credit for the original concept and implementation goes to him.

Maintained by **Thiago Silva**:
[github.com/brideinflame-rgb/macos-launchpad](https://github.com/brideinflame-rgb/macos-launchpad)

## Features
- Opens with the Windows key and/or the Start button (each can be toggled)
- Window or fullscreen mode (fullscreen respects the work area and the space reserved by Standalone Dock)
- Search as a top bar or a centered macOS-style box
- Search also finds apps inside folders
- Vertical scrolling or horizontal pages with page indicators, mouse wheel and swipe
- Persistent folders with open/close animations, rename and undo
- Create a folder by dragging an app onto another app, or from the context menu
- Drag apps into folders; drag an app out of an open folder to remove it
- Multi-select with Ctrl+click or the context menu
- Pin to taskbar from the context menu or by dragging an icon onto the taskbar
- Custom icons per app (PNG, JPG, JPEG, BMP, ICO, GIF, WebP), persistent
- Power button (Lock, Sleep, Restart, Shut down, Sign out) left or right of the search bar
- Opening/closing animations: Pop, Fade, Slide, CRT, with adjustable speed
- Blur Mode: Adjustable (blurred snapshot of the desktop) or None
- Colors, opacity, borders, rounded corners and blur behind the open folder

## Controls
- **Type** to search, **Backspace** deletes, **Ctrl+Backspace** deletes a word, **Ctrl+V** pastes
- **Arrows** + **Enter** to navigate and launch; **Page Up/Down** to scroll or change page
- **Esc**: closes menu > clears selection > clears search > closes folder > closes Launchpad
- Clicking empty space clears the selection or closes; clicking outside an open folder closes it
- Losing focus closes the Launchpad

## Context menus
- **App**: Select/Deselect, New folder with this app (or with the selection), Pin to taskbar, Change icon, Reset icon
- **App found inside a folder (search)**: Pin to taskbar, Change icon, Reset icon
- **Folder**: Rename, Undo folder
- **Empty space**: New folder, Select all, Clear selection
- **Inside a folder**: Change/Reset icon of the app, Rename, Undo folder

## Notes and limitations
- The blur is a static snapshot taken when the Launchpad opens
- Search is a case-insensitive substring match (accents are not ignored); results are sorted alphabetically
- Apps found inside folders can be launched, pinned and have their icon changed, but not selected or regrouped
- Power actions run without confirmation
- Pinning uses the shell `taskbarpin` verb and may silently fail if Windows doesn't offer it
- Uninstalled apps are removed from folders automatically
- Changing settings closes the Launchpad if it is open

## Changelog
### 1.2.0
- Search now includes apps inside folders
- "Blur type" renamed to "Blur Mode"; only Adjustable and None remain (Acrylic and Windows blur removed)
- Fixed credits (original mod: Fullscreen Start (Launchpad style))
### 1.1.0
- New setting: power button position (left or right of the search bar)
### 1.0.0
- First release, based on Rafael Mendes' Fullscreen Start (Launchpad style)
- Custom icons per app
- Optional power menu button
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- blurMode: custom
  $name: Blur Mode
  $options:
  - custom: Adjustable
  - none: None (no blur)
- blurAmount: 60
  $name: Blur strength (0-100)
  $description: Used only when Blur Mode is Adjustable
- backgroundOpacity: 60
  $name: Background opacity (%)
- tintColor: "#1e1e1e"
  $name: Background color
- displayMode: window
  $name: Display mode
  $options:
  - window: Window
  - fullscreen: Fullscreen
- windowWidth: 80
  $name: Window width (%, window mode only)
- windowHeight: 75
  $name: Window height (%, window mode only)
- searchStyle: bar
  $name: Search style
  $options:
  - bar: Top bar
  - box: Centered box (macOS)
- cornerRadius: 28
  $name: Window corner radius (px)
- folderRadius: 26
  $name: Open folder corner radius (px)
- folderBlur: 40
  $name: Folder blur (0-100)
- animationSpeed: 100
  $name: Animation speed (%, 25-400)
- borderWidth: 1
  $name: Border width (px, 0 = none)
- borderColor: "#ffffff"
  $name: Border color
- borderOpacity: 18
  $name: Border opacity (%)
- gridIconSize: 64
  $name: Icon size (px)
- gridColumns: 0
  $name: Columns (0 = auto)
- pageMode: vertical
  $name: Page navigation
  $options:
  - vertical: Vertical (scroll)
  - horizontal: Horizontal (pages)
- dragToPin: true
  $name: Drag icon to taskbar to pin
- openAnimation: zoom
  $name: Opening animation
  $options:
  - zoom: Pop in (surgir)
  - fade: Fade
  - slide: Rise from taskbar
  - crt: CRT
- closeAnimation: zoom
  $name: Closing animation
  $options:
  - zoom: Pop out (surgir)
  - fade: Fade
  - slide: Sink into taskbar
  - crt: CRT
- winKey: true
  $name: Open with Windows key
- startButton: true
  $name: Open with Start button
- showPowerButton: true
  $name: Show power button
  $description: Small power icon next to the search bar (lock, sleep, restart, shut down, sign out)
- powerButtonPosition: left
  $name: Power button position
  $description: Side of the search bar where the power button is placed
  $options:
  - left: Left of the search bar
  - right: Right of the search bar
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellscalingapi.h>
#undef DrawText
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Microsoft::WRL::ComPtr;
extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

constexpr WCHAR kWindowClass[] = L"WindhawkMacLaunchpad";
constexpr WCHAR kAppsFolder[] = L"shell:AppsFolder\\";
constexpr WCHAR kFoldersKey[] = L"folders";
constexpr WCHAR kIconsKey[] = L"customIcons";

constexpr UINT WM_APP_TOGGLE = WM_APP + 1;
constexpr UINT WM_APP_APPS = WM_APP + 2;
constexpr UINT WM_APP_RELOAD = WM_APP + 3;
constexpr UINT WM_APP_QUIT = WM_APP + 4;

constexpr UINT_PTR kTimerFrame = 1;
constexpr UINT_PTR kTimerFirstLoad = 4;

constexpr float kFolderOpenSeconds = 0.34f;
constexpr float kFolderCloseSeconds = 0.24f;

// Codificação de g_layoutItems (tela principal):
//   0..999                -> índice em g_apps
//   1000..999999          -> pasta (1000 + índice da pasta)
//   >= kFolderAppBase     -> app dentro de pasta (resultado de busca): base + pasta*10000 + índice
constexpr int kFolderAppBase = 1000000;

enum { kMenuRename = 1, kMenuUndoFolder, kMenuNewFolder, kMenuSelect, kMenuSelectAll, kMenuClearSelection, kMenuPin,
       kMenuChangeIcon, kMenuResetIcon,
       kMenuLock, kMenuSleep, kMenuRestart, kMenuShutdown, kMenuSignOut };

constexpr GUID kBhidEnumItems = {0x94f60519, 0x2850, 0x4924, {0xaa, 0x5a, 0xd1, 0x5e, 0x84, 0x86, 0x80, 0x39}};
constexpr GUID kBhidSFUIObject = {0x3981e225, 0xf559, 0x11d3, {0x8e, 0x3a, 0x00, 0xc0, 0x4f, 0x68, 0x37, 0xd5}};
constexpr GUID kClsidGaussianBlur = {0x1feb6d69, 0x2fe6, 0x4ac9, {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

double Now() {
    static LARGE_INTEGER frequency = [] { LARGE_INTEGER f; QueryPerformanceFrequency(&f); return f; }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
}

std::wstring ToLower(std::wstring text) {
    if (!text.empty()) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE, text.data(), (int)text.size(), text.data(), (int)text.size(), nullptr, nullptr, 0);
    }
    return text;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class BlurMode { Custom, None };
enum class Anim { Zoom, Fade, Slide, Crt };

Anim ParseAnim(const std::wstring& v) { return v == L"fade" ? Anim::Fade : v == L"slide" ? Anim::Slide : v == L"crt" ? Anim::Crt : Anim::Zoom; }
float AnimSeconds(Anim a, bool closing) {
    switch (a) {
        case Anim::Fade:  return closing ? 0.18f : 0.24f;
        case Anim::Slide: return closing ? 0.26f : 0.36f;
        case Anim::Crt:   return closing ? 0.40f : 0.44f;
        default:          return closing ? 0.16f : 0.30f;
    }
}

struct Settings {
    BlurMode blurMode = BlurMode::Custom;
    int blurAmount = 60;
    float opacity = 0.6f;
    D2D1_COLOR_F tint{0.118f, 0.118f, 0.118f, 1.0f};
    int windowWidth = 80;
    int windowHeight = 75;
    int iconSize = 64;
    int columns = 0;
    bool fullscreen = false;
    bool searchBox = false;
    int cornerRadius = 28;
    int folderRadius = 26;
    int folderBlur = 40;
    float animSpeed = 1.0f;
    int borderWidth = 1;
    D2D1_COLOR_F borderColor{1.0f, 1.0f, 1.0f, 1.0f};
    float borderOpacity = 0.18f;
    bool horizontal = false;
    bool dragToPin = true;
    bool showPower = true;
    bool powerRight = false;
    Anim openAnim = Anim::Zoom, closeAnim = Anim::Zoom;
};

Settings g_settings;
std::atomic<bool> g_winKeyEnabled{true};
std::atomic<bool> g_startButtonEnabled{true};

std::wstring GetStringSetting(PCWSTR name) { return WindhawkUtils::StringSetting::make(name).get(); }

D2D1_COLOR_F ParseColor(std::wstring text, D2D1_COLOR_F fallback) {
    text.erase(std::remove_if(text.begin(), text.end(), iswspace), text.end());
    if (!text.empty() && text[0] == L'#') text.erase(0, 1);
    if (text.size() != 6 || !std::all_of(text.begin(), text.end(), iswxdigit)) return fallback;
    UINT rgb = wcstoul(text.c_str(), nullptr, 16);
    return D2D1::ColorF(((rgb >> 16) & 0xFF) / 255.0f, ((rgb >> 8) & 0xFF) / 255.0f, (rgb & 0xFF) / 255.0f);
}

void LoadSettings() {
    Settings s;
    // Valores antigos ("acrylic", "blur") caem em Adjustable
    s.blurMode = GetStringSetting(L"blurMode") == L"none" ? BlurMode::None : BlurMode::Custom;
    s.blurAmount = std::clamp(Wh_GetIntSetting(L"blurAmount"), 0, 100);
    s.opacity = std::clamp(Wh_GetIntSetting(L"backgroundOpacity"), 0, 100) / 100.0f;
    s.tint = ParseColor(GetStringSetting(L"tintColor"), s.tint);
    s.windowWidth = std::clamp(Wh_GetIntSetting(L"windowWidth"), 30, 100);
    s.windowHeight = std::clamp(Wh_GetIntSetting(L"windowHeight"), 30, 100);
    s.iconSize = std::clamp(Wh_GetIntSetting(L"gridIconSize"), 24, 192);
    s.columns = std::clamp(Wh_GetIntSetting(L"gridColumns"), 0, 30);
    s.fullscreen = GetStringSetting(L"displayMode") == L"fullscreen";
    s.searchBox = GetStringSetting(L"searchStyle") == L"box";
    s.cornerRadius = std::clamp(Wh_GetIntSetting(L"cornerRadius"), 0, 60);
    s.folderRadius = std::clamp(Wh_GetIntSetting(L"folderRadius"), 0, 60);
    s.folderBlur = std::clamp(Wh_GetIntSetting(L"folderBlur"), 0, 100);
    s.animSpeed = std::clamp(Wh_GetIntSetting(L"animationSpeed"), 25, 400) / 100.0f;
    s.borderWidth = std::clamp(Wh_GetIntSetting(L"borderWidth"), 0, 8);
    s.borderColor = ParseColor(GetStringSetting(L"borderColor"), s.borderColor);
    s.borderOpacity = std::clamp(Wh_GetIntSetting(L"borderOpacity"), 0, 100) / 100.0f;
    s.horizontal = GetStringSetting(L"pageMode") == L"horizontal";
    s.dragToPin = Wh_GetIntSetting(L"dragToPin") != 0;
    s.showPower = Wh_GetIntSetting(L"showPowerButton") != 0;
    s.powerRight = GetStringSetting(L"powerButtonPosition") == L"right";
    s.openAnim = ParseAnim(GetStringSetting(L"openAnimation"));
    s.closeAnim = ParseAnim(GetStringSetting(L"closeAnimation"));
    g_settings = s;
    g_winKeyEnabled = Wh_GetIntSetting(L"winKey") != 0;
    g_startButtonEnabled = Wh_GetIntSetting(L"startButton") != 0;
}

////////////////////////////////////////////////////////////////////////////////
// App list & Folders

struct App {
    std::wstring name;
    std::wstring target;
    std::vector<UINT32> pixels;
    int iconSize = 0;
    ComPtr<ID2D1Bitmap> bitmap;
    UINT generation = 0;
};

struct Folder {
    std::wstring name = L"Folder";
    std::wstring sortKey;
    std::vector<std::shared_ptr<App>> apps;
};

using AppList = std::vector<std::shared_ptr<App>>;
[[clang::no_destroy]] AppList g_apps;
[[clang::no_destroy]] std::vector<Folder> g_folders;
[[clang::no_destroy]] std::vector<int> g_layoutItems;

bool g_appsLoaded;
double g_appsTime;
int g_appsIconPixels;
HANDLE g_worker;
std::atomic<bool> g_workerStop;
std::atomic<HWND> g_wnd;

void FinishIcon(std::vector<UINT32> pixels, int width, int height, App& app) {
    if (std::none_of(pixels.begin(), pixels.end(), [](UINT32 p) { return (p >> 24) != 0; })) {
        for (auto& p : pixels) p |= 0xFF000000;
    }
    bool premultiplied = std::none_of(pixels.begin(), pixels.end(), [](UINT32 p) {
        UINT a = p >> 24;
        return ((p >> 16) & 0xFF) > a || ((p >> 8) & 0xFF) > a || (p & 0xFF) > a;
    });
    if (!premultiplied) {
        for (auto& p : pixels) {
            UINT a = p >> 24, r = ((p >> 16) & 0xFF) * a / 255, g = ((p >> 8) & 0xFF) * a / 255, b = (p & 0xFF) * a / 255;
            p = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
    int left = width, top = height, right = -1, bottom = -1;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if ((pixels[y * width + x] >> 24) > 8) {
                left = std::min(left, x); right = std::max(right, x); top = std::min(top, y); bottom = std::max(bottom, y);
            }
        }
    }
    if (right < 0) return;
    int full = std::max(width, height), content = std::max(right - left + 1, bottom - top + 1);
    int side, cx, cy;
    if (content < full * 0.6f) {
        side = std::min(full, (int)std::ceil(content * 1.08f));
        cx = (left + right + 1) / 2; cy = (top + bottom + 1) / 2;
    } else { side = full; cx = width / 2; cy = height / 2; }
    app.iconSize = side;
    app.pixels.assign((size_t)side * side, 0);
    int x0 = cx - side / 2, y0 = cy - side / 2;
    for (int y = 0; y < side; y++) {
        int sy = y0 + y;
        if (sy < 0 || sy >= height) continue;
        for (int x = 0; x < side; x++) {
            int sx = x0 + x;
            if (sx >= 0 && sx < width) app.pixels[(size_t)y * side + x] = pixels[sy * width + sx];
        }
    }
}

void LoadAppIcon(IShellItem* item, int size, App& app) {
    ComPtr<IShellItemImageFactory> factory;
    HBITMAP bitmap = nullptr;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&factory))) || FAILED(factory->GetImage(SIZE{size, size}, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &bitmap))) return;
    BITMAP bm{};
    if (GetObjectW(bitmap, sizeof(bm), &bm) && bm.bmWidth > 0) {
        int width = bm.bmWidth, height = std::abs(bm.bmHeight);
        std::vector<UINT32> pixels((size_t)width * height);
        BITMAPINFO bi{}; bi.bmiHeader.biSize = sizeof(bi.bmiHeader); bi.bmiHeader.biWidth = width; bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
        HDC dc = GetDC(nullptr);
        int lines = GetDIBits(dc, bitmap, 0, height, pixels.data(), &bi, DIB_RGB_COLORS);
        ReleaseDC(nullptr, dc);
        if (lines) FinishIcon(std::move(pixels), width, height, app);
    }
    DeleteObject(bitmap);
}

bool ShouldSkip(const std::wstring& name, const std::wstring& parsing) {
    std::wstring lowerName = ToLower(name), lowerTarget = ToLower(parsing);
    if (lowerName.empty() || lowerTarget.find(L"\\unins0") != std::wstring::npos || lowerTarget.find(L"\\uninst.exe") != std::wstring::npos || lowerTarget.find(L"\\uninstall.exe") != std::wstring::npos || lowerTarget.rfind(L"http", 0) == 0) return true;
    for (PCWSTR ext : {L".chm", L".txt", L".pdf", L".rtf", L".htm", L".html"}) {
        if (lowerTarget.size() >= wcslen(ext) && lowerTarget.compare(lowerTarget.size() - wcslen(ext), wcslen(ext), ext) == 0) return true;
    }
    return false;
}

bool NameLess(const std::wstring& a, const std::wstring& b) {
    return CompareStringEx(LOCALE_NAME_USER_DEFAULT, LINGUISTIC_IGNORECASE | SORT_DIGITSASNUMBERS, a.c_str(), -1, b.c_str(), -1, nullptr, nullptr, 0) == CSTR_LESS_THAN;
}

AppList* EnumerateApps(int iconPixels) {
    auto list = std::make_unique<AppList>();
    ComPtr<IShellItem> folder; ComPtr<IEnumShellItems> items;
    if (FAILED(SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_PPV_ARGS(&folder))) || FAILED(folder->BindToHandler(nullptr, kBhidEnumItems, IID_PPV_ARGS(&items)))) return list.release();
    ComPtr<IShellItem> item;
    while (!g_workerStop && items->Next(1, item.ReleaseAndGetAddressOf(), nullptr) == S_OK) {
        PWSTR name = nullptr, parsing = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &parsing)) && !ShouldSkip(name, parsing)) {
            auto app = std::make_shared<App>();
            app->name = name; app->target = std::wstring(kAppsFolder) + parsing;
            LoadAppIcon(item.Get(), iconPixels, *app);
            list->push_back(std::move(app));
        }
        CoTaskMemFree(name); CoTaskMemFree(parsing);
    }
    std::stable_sort(list->begin(), list->end(), [](const auto& a, const auto& b) { return NameLess(a->name, b->name); });
    return list.release();
}

DWORD WINAPI LoadAppsThread(void* param) {
    int iconPixels = (int)(INT_PTR)param;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    AppList* list = EnumerateApps(iconPixels);
    CoUninitialize();
    HWND wnd = g_wnd;
    if (g_workerStop || !wnd || !PostMessageW(wnd, WM_APP_APPS, iconPixels, (LPARAM)list)) delete list;
    return 0;
}

int IconPixels() {
    UINT maxDpi = 96;
    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR monitor, HDC, LPRECT, LPARAM param) -> BOOL {
        UINT dpiX = 96, dpiY = 96;
        if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) *(UINT*)param = std::max(*(UINT*)param, dpiX);
        return TRUE;
    }, (LPARAM)&maxDpi);
    return std::min(256, (int)std::ceil(g_settings.iconSize * maxDpi / 96.0));
}

void StartLoadingApps() {
    if (g_worker) {
        if (WaitForSingleObject(g_worker, 0) == WAIT_TIMEOUT) return;
        CloseHandle(g_worker); g_worker = nullptr;
    }
    g_appsTime = Now();
    g_worker = CreateThread(nullptr, 0, LoadAppsThread, (void*)(INT_PTR)IconPixels(), 0, nullptr);
}

std::wstring g_query;
int g_openFolderIndex = -1;

bool IsFolderApp(int it) { return it >= kFolderAppBase; }
bool IsFolderItem(int it) { return it >= 1000 && it < kFolderAppBase; }

// Resolve um item da tela principal para App (app solto ou app dentro de pasta)
App* ResolveApp(int it) {
    if (it < 0) return nullptr;
    if (it < 1000) return it < (int)g_apps.size() ? g_apps[it].get() : nullptr;
    if (!IsFolderApp(it)) return nullptr;
    int f = (it - kFolderAppBase) / 10000, a = (it - kFolderAppBase) % 10000;
    if (f >= (int)g_folders.size() || a >= (int)g_folders[f].apps.size()) return nullptr;
    return g_folders[f].apps[a].get();
}

void BuildMainItems(std::vector<int>& out) {
    out.clear();
    if (!g_query.empty()) {
        std::wstring q = ToLower(g_query);
        for (int i = 0; i < (int)g_apps.size() && i < 1000; i++)
            if (ToLower(g_apps[i]->name).find(q) != std::wstring::npos) out.push_back(i);
        for (int f = 0; f < (int)g_folders.size(); f++)
            for (int a = 0; a < (int)g_folders[f].apps.size() && a < 10000; a++)
                if (ToLower(g_folders[f].apps[a]->name).find(q) != std::wstring::npos)
                    out.push_back(kFolderAppBase + f * 10000 + a);
        std::stable_sort(out.begin(), out.end(), [](int x, int y) { return NameLess(ResolveApp(x)->name, ResolveApp(y)->name); });
        return;
    }
    std::vector<int> folderOrder;
    for (int j = 0; j < (int)g_folders.size(); j++) folderOrder.push_back(j);
    std::stable_sort(folderOrder.begin(), folderOrder.end(), [](int a, int b) { return NameLess(g_folders[a].sortKey, g_folders[b].sortKey); });
    size_t next = 0;
    for (int i = 0; i < (int)g_apps.size(); i++) {
        while (next < folderOrder.size() && !NameLess(g_apps[i]->name, g_folders[folderOrder[next]].sortKey)) {
            out.push_back(1000 + folderOrder[next++]);
        }
        out.push_back(i);
    }
    while (next < folderOrder.size()) out.push_back(1000 + folderOrder[next++]);
}

void UndoFolder(int index) {
    if (index < 0 || index >= (int)g_folders.size()) return;
    for (const auto& app : g_folders[index].apps) g_apps.push_back(app);
    g_folders.erase(g_folders.begin() + index);
    std::stable_sort(g_apps.begin(), g_apps.end(), [](const auto& a, const auto& b) { return NameLess(a->name, b->name); });
}

////////////////////////////////////////////////////////////////////////////////
// Persistência das pastas

std::vector<std::wstring> Split(const std::wstring& s, WCHAR sep) {
    std::vector<std::wstring> parts;
    size_t start = 0;
    for (;;) {
        size_t pos = s.find(sep, start);
        if (pos == std::wstring::npos) { parts.push_back(s.substr(start)); break; }
        parts.push_back(s.substr(start, pos - start));
        start = pos + 1;
    }
    return parts;
}

std::wstring Clean(std::wstring s) {
    for (auto& c : s) if (c == L'\t' || c == L'\n' || c == L'\r') c = L' ';
    return s;
}

void SaveFolders() {
    std::wstring data;
    for (const auto& f : g_folders) {
        data += Clean(f.name); data += L'\t'; data += Clean(f.sortKey);
        for (const auto& a : f.apps) { data += L'\t'; data += Clean(a->target); }
        data += L'\n';
    }
    Wh_SetStringValue(kFoldersKey, data.c_str());
}

void LoadFolders() {
    std::vector<WCHAR> buf(1 << 18, 0);
    Wh_GetStringValue(kFoldersKey, buf.data(), buf.size());
    buf.back() = 0;
    g_folders.clear();
    for (const auto& line : Split(buf.data(), L'\n')) {
        auto fields = Split(line, L'\t');
        if (fields.size() < 2) continue;
        Folder folder;
        folder.name = fields[0]; folder.sortKey = fields[1];
        for (size_t i = 2; i < fields.size(); i++) {
            if (fields[i].empty()) continue;
            auto app = std::make_shared<App>();
            app->name = app->target = fields[i];
            folder.apps.push_back(app);
        }
        g_folders.push_back(std::move(folder));
    }
}

void ReconcileFolders(AppList& fresh) {
    std::unordered_map<std::wstring, std::shared_ptr<App>> byTarget;
    for (const auto& app : fresh) byTarget[app->target] = app;
    std::unordered_set<std::wstring> inFolders;
    for (auto& folder : g_folders) {
        std::vector<std::shared_ptr<App>> updated;
        for (const auto& old : folder.apps) {
            auto it = byTarget.find(old->target);
            if (it != byTarget.end() && inFolders.insert(old->target).second) updated.push_back(it->second);
        }
        folder.apps = std::move(updated);
    }
    if (inFolders.empty()) return;
    fresh.erase(std::remove_if(fresh.begin(), fresh.end(), [&](const std::shared_ptr<App>& app) { return inFolders.count(app->target) != 0; }), fresh.end());
}

////////////////////////////////////////////////////////////////////////////////
// Ícones personalizados

[[clang::no_destroy]] std::unordered_map<std::wstring, std::wstring> g_customIcons; // target -> caminho
bool g_dialogOpen;

void SaveCustomIcons() {
    std::wstring data;
    for (auto& [target, path] : g_customIcons) { data += Clean(target); data += L'\t'; data += Clean(path); data += L'\n'; }
    Wh_SetStringValue(kIconsKey, data.c_str());
}

void LoadCustomIcons() {
    std::vector<WCHAR> buf(1 << 17, 0);
    Wh_GetStringValue(kIconsKey, buf.data(), buf.size());
    buf.back() = 0;
    g_customIcons.clear();
    for (const auto& line : Split(buf.data(), L'\n')) {
        auto f = Split(line, L'\t');
        if (f.size() == 2 && !f[0].empty() && !f[1].empty()) g_customIcons[f[0]] = f[1];
    }
}

bool LoadImageFile(const std::wstring& path, int size, App& app) {
    ComPtr<IWICImagingFactory> wic; ComPtr<IWICBitmapDecoder> dec; ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICBitmapScaler> scaler; ComPtr<IWICFormatConverter> conv;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic))) ||
        FAILED(wic->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &dec))) return false;
    UINT frames = 0, best = 0, bestArea = 0; dec->GetFrameCount(&frames);
    for (UINT i = 0; i < frames; i++) {
        ComPtr<IWICBitmapFrameDecode> f; UINT w = 0, h = 0;
        if (SUCCEEDED(dec->GetFrame(i, &f)) && SUCCEEDED(f->GetSize(&w, &h)) && w * h > bestArea) { bestArea = w * h; best = i; }
    }
    if (FAILED(dec->GetFrame(best, &frame))) return false;
    UINT w = 0, h = 0; frame->GetSize(&w, &h);
    if (!w || !h) return false;
    float k = (float)size / std::max(w, h);
    UINT sw = std::clamp((UINT)std::lround(w * k), 1u, (UINT)size), sh = std::clamp((UINT)std::lround(h * k), 1u, (UINT)size);
    if (FAILED(wic->CreateBitmapScaler(&scaler)) ||
        FAILED(scaler->Initialize(frame.Get(), sw, sh, (WICBitmapInterpolationMode)4)) ||
        FAILED(wic->CreateFormatConverter(&conv)) ||
        FAILED(conv->Initialize(scaler.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom))) return false;
    std::vector<UINT32> tmp((size_t)sw * sh);
    if (FAILED(conv->CopyPixels(nullptr, sw * 4, (UINT)tmp.size() * 4, (BYTE*)tmp.data()))) return false;
    app.pixels.assign((size_t)size * size, 0);
    UINT ox = (size - sw) / 2, oy = (size - sh) / 2;
    for (UINT y = 0; y < sh; y++) std::copy_n(&tmp[(size_t)y * sw], sw, &app.pixels[(size_t)(y + oy) * size + ox]);
    app.iconSize = size; app.bitmap.Reset(); app.generation = 0;
    return true;
}

void ApplyCustomIcons(AppList& list, int size) {
    for (auto& app : list) {
        auto it = g_customIcons.find(app->target);
        if (it != g_customIcons.end()) LoadImageFile(it->second, size, *app);
    }
}

int CurrentIconPixels() { return g_appsIconPixels ? g_appsIconPixels : IconPixels(); }

void ResetAppIcon(App& app) {
    app.pixels.clear(); app.bitmap.Reset(); app.generation = 0;
    ComPtr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemFromParsingName(app.target.c_str(), nullptr, IID_PPV_ARGS(&item))))
        LoadAppIcon(item.Get(), CurrentIconPixels(), app);
}

bool PickImage(HWND owner, std::wstring& out) {
    ComPtr<IFileOpenDialog> dlg;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg)))) return false;
    COMDLG_FILTERSPEC types[] = { {L"Images", L"*.png;*.jpg;*.jpeg;*.bmp;*.ico;*.gif;*.webp"}, {L"All files", L"*.*"} };
    dlg->SetFileTypes(ARRAYSIZE(types), types);
    dlg->SetTitle(L"Choose icon image");
    g_dialogOpen = true;
    SetWindowPos(owner, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    HRESULT hr = dlg->Show(owner);
    SetWindowPos(owner, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    g_dialogOpen = false;
    ComPtr<IShellItem> res; PWSTR path = nullptr;
    if (FAILED(hr) || FAILED(dlg->GetResult(&res)) || FAILED(res->GetDisplayName(SIGDN_FILESYSPATH, &path))) return false;
    out = path; CoTaskMemFree(path);
    return true;
}

void RebuildLayout() {
    if (g_openFolderIndex >= 0 && g_openFolderIndex < (int)g_folders.size()) {
        g_layoutItems.clear();
        for (int i = 0; i < (int)g_folders[g_openFolderIndex].apps.size(); i++) g_layoutItems.push_back(i);
        return;
    }
    BuildMainItems(g_layoutItems);
}

////////////////////////////////////////////////////////////////////////////////
// Drawing

[[clang::no_destroy]] ComPtr<ID2D1Factory> g_d2d;
[[clang::no_destroy]] ComPtr<IDWriteFactory> g_dwrite;
[[clang::no_destroy]] ComPtr<ID2D1DCRenderTarget> g_target;
[[clang::no_destroy]] ComPtr<ID2D1SolidColorBrush> g_brush;
UINT g_targetGeneration = 1;
[[clang::no_destroy]] ComPtr<ID2D1Bitmap1> g_layerBitmap;
UINT g_layerGeneration;

struct Surface { HDC dc = nullptr; HBITMAP bitmap = nullptr; HGDIOBJ old = nullptr; int width = 0, height = 0; };
Surface g_surface;

void FreeSurface(Surface& s) {
    if (s.dc) { SelectObject(s.dc, s.old); DeleteObject(s.bitmap); DeleteDC(s.dc); }
    s = {};
}

bool EnsureSurface(Surface& s, int width, int height) {
    if (s.dc && s.width == width && s.height == height) return true;
    FreeSurface(s);
    BITMAPINFO bi{}; bi.bmiHeader.biSize = sizeof(bi.bmiHeader); bi.bmiHeader.biWidth = width; bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    s.dc = CreateCompatibleDC(nullptr);
    s.bitmap = CreateDIBSection(s.dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!s.bitmap) { DeleteDC(s.dc); s = {}; return false; }
    s.old = SelectObject(s.dc, s.bitmap); s.width = width; s.height = height;
    return true;
}

bool EnsureTarget() {
    if (g_target) return true;
    auto props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
    if (FAILED(g_d2d->CreateDCRenderTarget(&props, &g_target))) return false;
    g_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    g_target->CreateSolidColorBrush(D2D1::ColorF(0, 0), &g_brush);
    g_targetGeneration++;
    return true;
}

void ResetTarget() { g_layerBitmap.Reset(); g_brush.Reset(); g_target.Reset(); }

ID2D1Bitmap1* EnsureLayerBitmap(ID2D1DeviceContext* dc, int w, int h) {
    if (g_layerBitmap && g_layerGeneration == g_targetGeneration) {
        D2D1_SIZE_U sz = g_layerBitmap->GetPixelSize();
        if ((int)sz.width == w && (int)sz.height == h) return g_layerBitmap.Get();
    }
    g_layerBitmap.Reset(); g_layerGeneration = g_targetGeneration;
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
    dc->CreateBitmap(D2D1::SizeU(w, h), nullptr, 0, &props, &g_layerBitmap);
    return g_layerBitmap.Get();
}

ID2D1Bitmap* GetBitmap(App& app) {
    if (app.pixels.empty()) return nullptr;
    if (app.generation != g_targetGeneration) {
        app.generation = g_targetGeneration; app.bitmap.Reset();
        g_target->CreateBitmap(D2D1::SizeU(app.iconSize, app.iconSize), app.pixels.data(), app.iconSize * 4,
            D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96), &app.bitmap);
    }
    return app.bitmap.Get();
}

D2D1_COLOR_F Alpha(D2D1_COLOR_F c, float a) { c.a = a; return c; }
void Fill(const D2D1_RECT_F& r, D2D1_COLOR_F color, float radius = 0) {
    g_brush->SetColor(color);
    if (radius > 0) g_target->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), g_brush.Get());
    else g_target->FillRectangle(r, g_brush.Get());
}

////////////////////////////////////////////////////////////////////////////////
// Background (Blur Mode: Adjustable)

struct Backdrop { std::vector<UINT32> pixels; int width = 0, height = 0; float sourceWidth = 0, sourceHeight = 0; ComPtr<ID2D1Bitmap> bitmap; UINT generation = 0; };
[[clang::no_destroy]] Backdrop g_backdropImage;

void BoxBlurPass(const UINT32* src, UINT32* dst, int lines, int length, int lineStep, int step, int radius) {
    int window = 2 * radius + 1;
    for (int line = 0; line < lines; line++) {
        const UINT32* s = src + (size_t)line * lineStep; UINT32* d = dst + (size_t)line * lineStep;
        auto at = [&](int i) { return s[(size_t)std::clamp(i, 0, length - 1) * step]; };
        int sum[3] = {};
        for (int i = -radius; i <= radius; i++) { UINT32 p = at(i); sum[0] += p & 0xFF; sum[1] += (p >> 8) & 0xFF; sum[2] += (p >> 16) & 0xFF; }
        for (int i = 0; i < length; i++) {
            d[(size_t)i * step] = 0xFF000000 | ((sum[2] / window) << 16) | ((sum[1] / window) << 8) | (sum[0] / window);
            UINT32 in = at(i + radius + 1), out = at(i - radius);
            sum[0] += (int)(in & 0xFF) - (int)(out & 0xFF); sum[1] += (int)((in >> 8) & 0xFF) - (int)((out >> 8) & 0xFF); sum[2] += (int)((in >> 16) & 0xFF) - (int)((out >> 16) & 0xFF);
        }
    }
}

void CaptureBackdrop(const RECT& rc, float sigma) {
    g_backdropImage = {};
    int width = rc.right - rc.left, height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0) return;
    BITMAPINFO bi{}; bi.bmiHeader.biSize = sizeof(bi.bmiHeader); bi.bmiHeader.biWidth = width; bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr; HDC screen = GetDC(nullptr); HDC dc = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) { DeleteDC(dc); ReleaseDC(nullptr, screen); return; }
    HGDIOBJ old = SelectObject(dc, bitmap);
    BitBlt(dc, 0, 0, width, height, screen, rc.left, rc.top, SRCCOPY); GdiFlush(); ReleaseDC(nullptr, screen);
    int f = sigma >= 3 ? std::clamp((int)(sigma / 3), 1, 16) : 1;
    int sw = (width + f - 1) / f, sh = (height + f - 1) / f;
    std::vector<UINT32> small((size_t)sw * sh); const UINT32* full = (const UINT32*)bits;
    for (int y = 0; y < sh; y++) {
        for (int x = 0; x < sw; x++) {
            int sum[3] = {}, count = 0;
            for (int yy = y * f; yy < std::min(height, (y + 1) * f); yy++) {
                for (int xx = x * f; xx < std::min(width, (x + 1) * f); xx++) {
                    UINT32 p = full[(size_t)yy * width + xx]; sum[0] += p & 0xFF; sum[1] += (p >> 8) & 0xFF; sum[2] += (p >> 16) & 0xFF; count++;
                }
            }
            small[(size_t)y * sw + x] = 0xFF000000 | ((sum[2] / count) << 16) | ((sum[1] / count) << 8) | (sum[0] / count);
        }
    }
    SelectObject(dc, old); DeleteObject(bitmap); DeleteDC(dc);
    float s = sigma / f; int radius = (int)std::lround((std::sqrt(4 * s * s + 1) - 1) / 2);
    if (radius > 0) {
        std::vector<UINT32> temp(small.size());
        for (int pass = 0; pass < 3; pass++) {
            BoxBlurPass(small.data(), temp.data(), sh, sw, sw, 1, radius); BoxBlurPass(temp.data(), small.data(), sw, sh, 1, sw, radius);
        }
    }
    g_backdropImage.pixels = std::move(small); g_backdropImage.width = sw; g_backdropImage.height = sh;
    g_backdropImage.sourceWidth = (float)width / f; g_backdropImage.sourceHeight = (float)height / f;
}

ID2D1Bitmap* GetBackdropBitmap() {
    Backdrop& b = g_backdropImage;
    if (b.pixels.empty()) return nullptr;
    if (b.generation != g_targetGeneration) {
        b.generation = g_targetGeneration; b.bitmap.Reset();
        g_target->CreateBitmap(D2D1::SizeU(b.width, b.height), b.pixels.data(), b.width * 4,
            D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96), &b.bitmap);
    }
    return b.bitmap.Get();
}

////////////////////////////////////////////////////////////////////////////////
// Tela cheia: área disponível descontando o Dock

constexpr WCHAR kDockClass[] = L"WindhawkMacDock";
constexpr WCHAR kDockPlateClass[] = L"WindhawkMacDockPlate";
constexpr WCHAR kDockEdgeProp[] = L"WindhawkMacDock.Edge";
constexpr WCHAR kDockReservedProp[] = L"WindhawkMacDock.Reserved";

RECT AvailableArea(HMONITOR monitor) {
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(monitor, &mi);
    struct Search { HMONITOR monitor; RECT monitorRect; RECT area; } search{monitor, mi.rcMonitor, mi.rcWork};
    EnumWindows([](HWND hwnd, LPARAM param) -> BOOL {
        auto* s = (Search*)param;
        WCHAR className[64];
        if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) || wcscmp(className, kDockClass) ||
            MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) != s->monitor) return TRUE;
        int edge = (int)(INT_PTR)GetPropW(hwnd, kDockEdgeProp);
        int reserved = (int)(INT_PTR)GetPropW(hwnd, kDockReservedProp);
        const RECT& m = s->monitorRect; RECT& a = s->area;
        switch (edge) {
            case 1: a.bottom = std::min<LONG>(a.bottom, m.bottom - reserved); break;
            case 2: a.left = std::max<LONG>(a.left, m.left + reserved); break;
            case 3: a.right = std::min<LONG>(a.right, m.right - reserved); break;
        }
        return TRUE;
    }, (LPARAM)&search);
    RECT& a = search.area;
    if (a.right - a.left < 200 || a.bottom - a.top < 200) a = mi.rcWork;
    return a;
}

void RaiseDockWindows() {
    std::vector<HWND> windows;
    for (PCWSTR className : {kDockPlateClass, kDockClass}) {
        HWND hwnd = nullptr;
        while ((hwnd = FindWindowExW(nullptr, hwnd, className, nullptr))) {
            if (IsWindowVisible(hwnd)) windows.push_back(hwnd);
        }
    }
    for (HWND hwnd : windows) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Layout

struct Layout {
    RECT area{}; int width = 0, height = 0; float scale = 1;
    float icon = 72, labelHeight = 20, cellWidth = 140, cellHeight = 140; float cellPad = 6;
    int columns = 1; float gridLeft = 0; float viewTop = 0, viewBottom = 0; int visibleRows = 1;
    float contentHeight = 0; D2D1_RECT_F search{}, btnBack{}, panel{}, btnPower{};
    bool horizontal = false; float pageWidth = 1; int perPage = 1, pages = 1;
    D2D1_RECT_F dots{}; float dotsH = 20, dotGap = 16;
    int extraBottom = 0;
};

Layout g_layout;
float g_scroll, g_scrollTarget;
int g_wheelDelta;
float g_dotsHoverAnim; bool g_dotsHover; int g_dotHover = -1;
bool g_swiping; float g_swipeStartScroll; double g_lastFlip;

Layout g_mainLayout;
float g_mainScroll;
[[clang::no_destroy]] std::vector<int> g_mainItems;

D2D1_RECT_F g_folderOrigin{};
float g_folderAnim;
bool g_folderClosing;

bool g_renaming, g_renameSelectAll;
[[clang::no_destroy]] std::wstring g_renameText;

[[clang::no_destroy]] ComPtr<IDWriteTextFormat> g_labelFormat, g_searchFormat, g_messageFormat, g_titleFormat, g_menuFormat;
float g_formatScale;

void EnsureFormats(float scale) {
    if (g_labelFormat && g_formatScale == scale) return;
    g_formatScale = scale;
    auto make = [&](float size, DWRITE_FONT_WEIGHT weight, DWRITE_TEXT_ALIGNMENT align, ComPtr<IDWriteTextFormat>& format) {
        format.Reset();
        g_dwrite->CreateTextFormat(L"Segoe UI Variable Text", nullptr, weight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size * scale, L"", &format);
        if (!format) return;
        format->SetTextAlignment(align); format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        ComPtr<IDWriteInlineObject> ellipsis; g_dwrite->CreateEllipsisTrimmingSign(format.Get(), &ellipsis);
        format->SetTrimming(&trimming, ellipsis.Get());
    };
    make(12.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER, g_labelFormat);
    make(14.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, g_searchFormat);
    make(16.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER, g_titleFormat);
    make(16, DWRITE_FONT_WEIGHT_SEMI_LIGHT, DWRITE_TEXT_ALIGNMENT_CENTER, g_messageFormat);
    make(13.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, g_menuFormat);
}

float MaxScroll() {
    const Layout& L = g_layout;
    if (L.horizontal) return std::max(0, L.pages - 1) * L.pageWidth;
    return std::max(0.0f, L.contentHeight - (L.viewBottom - L.viewTop));
}

void ComputeLayout() {
    Layout& L = g_layout; float s = L.scale; float W = (float)L.width, H = (float)L.height;
    float padding = 40 * s;
    bool inFolder = g_openFolderIndex >= 0;
    float bottomLimit;

    if (inFolder) {
        float margin = 24 * s;
        L.panel = D2D1::RectF(margin, 72 * s, W - margin, H - margin);
        L.btnBack = D2D1::RectF(padding, 24 * s, padding + 32 * s, 24 * s + 32 * s);
        L.search = D2D1::RectF(0, 0, 0, 0);
        L.btnPower = D2D1::RectF(0, 0, 0, 0);
        L.viewTop = L.panel.top + 16 * s;
        bottomLimit = L.panel.bottom - 16 * s;
    } else {
        L.panel = D2D1::RectF(0, 0, 0, 0);
        L.btnBack = D2D1::RectF(0, 0, 0, 0);
        bool power = g_settings.showPower, right = g_settings.powerRight;
        float pw = std::round(36 * s);
        float powerLeft = 0;
        if (g_settings.searchBox) {
            float gap = std::round(8 * s);
            float reserve = power ? 2 * (pw + gap) : 0;
            float sw = std::max(120 * s, std::min(320 * s, W - 2 * padding - reserve));
            float top = std::round(std::max(28 * s, H * 0.045f));
            L.search = D2D1::RectF(std::round((W - sw) / 2), top, std::round((W + sw) / 2), top + std::round(36 * s));
            powerLeft = right ? L.search.right + gap : L.search.left - gap - pw;
        } else {
            float reserve = power ? pw + 12 * s : 0;
            float left = power && !right ? padding + reserve : padding;
            float rightEdge = power && right ? W - padding - reserve : W - padding;
            L.search = D2D1::RectF(left, 24 * s, rightEdge, 24 * s + 40 * s);
            powerLeft = right ? W - padding - pw : padding;
        }
        float cy = (L.search.top + L.search.bottom) / 2;
        powerLeft = std::round(powerLeft);
        L.btnPower = power
            ? D2D1::RectF(powerLeft, std::round(cy - pw / 2), powerLeft + pw, std::round(cy - pw / 2) + pw)
            : D2D1::RectF(0, 0, 0, 0);
        L.viewTop = L.search.bottom + std::round(30 * s);
        bottomLimit = H - padding;
    }

    L.icon = std::round(g_settings.iconSize * s);
    L.labelHeight = std::round(18 * s);
    L.cellPad = std::round(6 * s);
    L.cellHeight = std::round(L.icon + L.labelHeight + 45 * s);

    float availableWidth = inFolder ? (L.panel.right - L.panel.left) - 40 * s : W - 2 * padding;
    float available = std::max(L.icon * 2, availableWidth);
    float minCell = std::max(L.icon * 1.8f, L.icon + 64 * s);

    L.columns = g_settings.columns > 0 ? g_settings.columns : std::clamp((int)(available / minCell), 2, 14);
    L.cellWidth = std::floor(std::min(available / L.columns, std::max(minCell, L.icon * 2.6f)));

    L.gridLeft = inFolder ? std::round(L.panel.left + 20 * s) : std::round((W - L.cellWidth * L.columns) / 2);

    L.horizontal = g_settings.horizontal;
    L.dotsH = std::round(std::clamp(std::min(W, H) * 0.028f, 16 * s, 28 * s));
    if (L.horizontal) bottomLimit -= L.dotsH + 18 * s;
    L.visibleRows = std::max(1, (int)((bottomLimit - L.viewTop) / L.cellHeight));
    L.viewBottom = L.viewTop + L.visibleRows * L.cellHeight;

    int count = (int)g_layoutItems.size();
    L.pageWidth = inFolder ? (L.panel.right - L.panel.left) : W;
    L.perPage = std::max(1, L.visibleRows * L.columns);
    L.pages = std::max(1, (count + L.perPage - 1) / L.perPage);
    L.contentHeight = L.horizontal ? 0 : ((count + L.columns - 1) / L.columns) * L.cellHeight;

    L.dots = D2D1::RectF(0, 0, 0, 0);
    if (L.horizontal && L.pages > 1) {
        float h = L.dotsH, pad = h * 0.45f, gap = h * 0.8f, maxW = L.pageWidth * 0.6f;
        if (L.pages * gap + 2 * pad > maxW) gap = std::max(h * 0.35f, (maxW - 2 * pad) / L.pages);
        L.dotGap = gap;
        float w = L.pages * gap + 2 * pad;
        float cx = inFolder ? (L.panel.left + L.panel.right) / 2 : W / 2;
        float top = std::round(L.viewBottom + 9 * s);
        L.dots = D2D1::RectF(std::round(cx - w / 2), top, std::round(cx + w / 2), top + h);
    }
    g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll()); g_scroll = std::clamp(g_scroll, 0.0f, MaxScroll());
}

struct Cell { D2D1_RECT_F highlight, icon, label; };
Cell CellAtIn(const Layout& L, float scroll, int index) {
    float x, y;
    if (L.horizontal) {
        int page = index / L.perPage, local = index % L.perPage;
        x = L.gridLeft + page * L.pageWidth + (local % L.columns) * L.cellWidth - std::round(scroll);
        y = L.viewTop + (local / L.columns) * L.cellHeight;
    } else {
        x = L.gridLeft + (index % L.columns) * L.cellWidth;
        y = L.viewTop + (index / L.columns) * L.cellHeight - std::round(scroll);
    }
    Cell c;
    c.highlight = D2D1::RectF(x + L.cellPad, y, x + L.cellWidth - L.cellPad, y + L.cellHeight - L.cellPad * 2);
    float iconLeft = std::round(x + (L.cellWidth - L.icon) / 2), iconTop = std::round(y + 8 * L.scale);
    c.icon = D2D1::RectF(iconLeft, iconTop, iconLeft + L.icon, iconTop + L.icon);
    float labelTop = c.icon.bottom + std::round(8 * L.scale);
    c.label = D2D1::RectF(x + L.cellPad + 4 * L.scale, labelTop, x + L.cellWidth - L.cellPad - 4 * L.scale, labelTop + L.labelHeight);
    return c;
}
Cell CellAt(int index) { return CellAtIn(g_layout, g_scroll, index); }

bool InRect(const D2D1_RECT_F& r, float x, float y) { return x >= r.left && x < r.right && y >= r.top && y < r.bottom; }

D2D1_RECT_F TitleRect(const Layout& L) {
    float w = 360 * L.scale, cx = L.width / 2.0f;
    return D2D1::RectF(cx - w / 2, L.btnBack.top, cx + w / 2, L.btnBack.bottom);
}

int ItemAt(int px, int py) {
    const Layout& L = g_layout; float x = (float)px, y = (float)py;
    if (y < L.viewTop || y >= L.viewBottom) return -1;
    int index;
    if (L.horizontal) {
        float vx = x + std::round(g_scroll) - L.gridLeft;
        int page = (int)std::floor(vx / L.pageWidth);
        float lx = vx - page * L.pageWidth;
        if (page < 0 || lx < 0 || lx >= L.cellWidth * L.columns) return -1;
        index = page * L.perPage + (int)((y - L.viewTop) / L.cellHeight) * L.columns + (int)(lx / L.cellWidth);
    } else {
        if (x < L.gridLeft || x >= L.gridLeft + L.cellWidth * L.columns) return -1;
        int row = (int)((y - L.viewTop + std::round(g_scroll)) / L.cellHeight);
        if (row < 0) return -1;
        index = row * L.columns + (int)((x - L.gridLeft) / L.cellWidth);
    }
    if (index < 0 || index >= (int)g_layoutItems.size() || !InRect(CellAt(index).highlight, x, y)) return -1;
    return index;
}

////////////////////////////////////////////////////////////////////////////////
// States and Drawing

bool g_shown, g_closing; float g_appear; double g_lastFrame, g_lastClosed; bool g_frameTimer;
int g_hover = -1, g_selected = -1, g_pressed = -1; bool g_pressedEmpty, g_trackingMouse;
float g_dragX = -1, g_dragY = -1; int g_dragItem = -1; bool g_isDragging = false;
bool g_powerHover;

float Ease(float t) { t = std::clamp(t, 0.0f, 1.0f); return 1 - (1 - t) * (1 - t) * (1 - t); }
float EaseInOut(float t) { t = std::clamp(t, 0.0f, 1.0f); return t < 0.5f ? 4 * t * t * t : 1 - std::pow(-2 * t + 2, 3.0f) / 2; }
float Lerp(float a, float b, float t) { return a + (b - a) * t; }
D2D1_RECT_F LerpRect(const D2D1_RECT_F& a, const D2D1_RECT_F& b, float t) {
    return D2D1::RectF(Lerp(a.left, b.left, t), Lerp(a.top, b.top, t), Lerp(a.right, b.right, t), Lerp(a.bottom, b.bottom, t));
}

bool FolderSettled() { return g_openFolderIndex < 0 || (!g_folderClosing && g_folderAnim >= 1.0f); }

// ---------- Animações de abrir/fechar ----------
float g_slideDistance;
struct AnimFx { D2D1::Matrix3x2F m = D2D1::Matrix3x2F::Identity(); float alpha = 1, flash = 0; };

float BackOut(float t) { t = std::clamp(t, 0.0f, 1.0f); const float c1 = 1.4f, c3 = c1 + 1; float u = t - 1; return 1 + c3 * u * u * u + c1 * u * u; }

AnimFx ComputeAnim() {
    const Layout& L = g_layout; float W = (float)L.width, H = (float)L.height, s = L.scale;
    D2D1_POINT_2F c = D2D1::Point2F(W / 2, H / 2);
    float p = std::clamp(g_appear, 0.0f, 1.0f);
    AnimFx fx;
    switch (g_closing ? g_settings.closeAnim : g_settings.openAnim) {
        case Anim::Fade: fx.alpha = p * p * (3 - 2 * p); break;
        case Anim::Zoom: {
            float e = g_closing ? Ease(p) : BackOut(p), k = 0.86f + 0.14f * e;
            fx.m = D2D1::Matrix3x2F::Scale(k, k, c); fx.alpha = std::min(1.0f, p * 2.2f); break;
        }
        case Anim::Slide: {
            float e = Ease(p), k = 0.92f + 0.08f * e;
            fx.m = D2D1::Matrix3x2F::Scale(k, k, D2D1::Point2F(W / 2, H)) * D2D1::Matrix3x2F::Translation(0, (1 - e) * g_slideDistance);
            fx.alpha = std::min(1.0f, p * 2.5f); break;
        }
        case Anim::Crt: {
            float minY = std::max(2 * s / H, 0.003f), sx, sy;
            if (p < 0.45f) { sx = Lerp(0.02f, 1.0f, Ease(p / 0.45f)); sy = minY; }
            else { sx = 1; sy = Lerp(minY, 1.0f, Ease((p - 0.45f) / 0.55f)); }
            fx.m = D2D1::Matrix3x2F::Scale(sx, sy, c);
            fx.flash = 0.85f * (1 - Ease(p));
            fx.alpha = std::min(1.0f, p * 12); break;
        }
    }
    return fx;
}

// ---------- Menu de contexto ----------
struct MenuItem { std::wstring text; int id = 0; bool separator = false; bool danger = false; };
struct PopupMenu {
    bool open = false, closing = false; float anim = 0; D2D1_RECT_F rect{}; D2D1_POINT_2F origin{};
    std::vector<MenuItem> items; int hover = -1, folder = -1, appIdx = -1, folderApp = -1; float itemH = 32, sepH = 9;
};
[[clang::no_destroy]] PopupMenu g_menu;

float MenuItemTop(int i) {
    float y = g_menu.rect.top + 6 * g_layout.scale;
    for (int k = 0; k < i; k++) y += g_menu.items[k].separator ? g_menu.sepH : g_menu.itemH;
    return y;
}

int MenuHit(float x, float y) {
    if (!g_menu.open || g_menu.closing || !InRect(g_menu.rect, x, y)) return -1;
    for (int i = 0; i < (int)g_menu.items.size(); i++) {
        if (g_menu.items[i].separator) continue;
        float t = MenuItemTop(i);
        if (y >= t && y < t + g_menu.itemH) return i;
    }
    return -1;
}

void DrawPopupMenu(const D2D1::Matrix3x2F& base) {
    if (!g_menu.open) return;
    const Layout& L = g_layout; float s = L.scale, e = Ease(g_menu.anim), k = 0.92f + 0.08f * e;
    if (e <= 0.001f) return;
    g_target->SetTransform(D2D1::Matrix3x2F::Scale(k, k, g_menu.origin) * base);
    D2D1_RECT_F r = g_menu.rect; float rad = 14 * s;
    for (int i = 1; i <= 3; i++) {
        float g = i * 3 * s;
        Fill(D2D1::RectF(r.left - g, r.top - g + 5 * s, r.right + g, r.bottom + g + 5 * s), D2D1::ColorF(0, 0, 0, 0.06f * e), rad + g);
    }
    ComPtr<ID2D1RoundedRectangleGeometry> geo;
    g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, rad, rad), &geo);
    if (!geo) return;
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), geo.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), e), nullptr);
    if (g_settings.blurMode == BlurMode::Custom) {
        if (ID2D1Bitmap* bmp = GetBackdropBitmap())
            g_target->DrawBitmap(bmp, D2D1::RectF(0, 0, (float)L.width, (float)L.height), 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                D2D1::RectF(0, 0, g_backdropImage.sourceWidth, g_backdropImage.sourceHeight));
    }
    Fill(r, Alpha(g_settings.tint, 0.70f));
    Fill(r, D2D1::ColorF(1, 1, 1, 0.07f));
    for (int i = 0; i < (int)g_menu.items.size(); i++) {
        const MenuItem& it = g_menu.items[i]; float top = MenuItemTop(i);
        if (it.separator) {
            float y = std::round(top + g_menu.sepH / 2) + 0.5f;
            g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.12f));
            g_target->DrawLine(D2D1::Point2F(r.left + 12 * s, y), D2D1::Point2F(r.right - 12 * s, y), g_brush.Get(), 1.0f);
            continue;
        }
        D2D1_RECT_F row = D2D1::RectF(r.left + 6 * s, top, r.right - 6 * s, top + g_menu.itemH);
        if (i == g_menu.hover) Fill(row, it.danger ? D2D1::ColorF(1, 0.3f, 0.3f, 0.22f) : D2D1::ColorF(1, 1, 1, 0.14f), 8 * s);
        if (g_menuFormat) {
            g_brush->SetColor(it.danger ? D2D1::ColorF(1, 0.55f, 0.55f, 0.95f) : D2D1::ColorF(1, 1, 1, 0.92f));
            g_target->DrawText(it.text.c_str(), (UINT32)it.text.size(), g_menuFormat.Get(),
                D2D1::RectF(row.left + 10 * s, row.top, row.right - 10 * s, row.bottom), g_brush.Get());
        }
    }
    g_target->PopLayer();
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.18f * e));
    g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + 0.5f, r.top + 0.5f, r.right - 0.5f, r.bottom - 0.5f), rad, rad), g_brush.Get(), 1.0f);
}

// ---------- Seleção múltipla ----------
[[clang::no_destroy]] std::unordered_set<App*> g_selection;

// App da tela principal (solto ou dentro de pasta via busca)
App* AppOfMain(int idx) {
    if (g_openFolderIndex >= 0 || idx < 0 || idx >= (int)g_layoutItems.size()) return nullptr;
    int it = g_layoutItems[idx];
    return IsFolderItem(it) ? nullptr : ResolveApp(it);
}
// Apenas apps soltos podem ser selecionados/agrupados
App* SelectableApp(int idx) {
    App* a = AppOfMain(idx);
    return a && g_layoutItems[idx] < 1000 ? a : nullptr;
}
bool IsSelected(int idx) { App* a = SelectableApp(idx); return a && g_selection.count(a); }
bool GroupDrag() { return g_isDragging && g_dragItem >= 0 && g_selection.size() > 1 && IsSelected(g_dragItem); }

void DrawCheckBadge(const D2D1_RECT_F& icon, float a) {
    float s = g_layout.scale, r = 9 * s; D2D1_POINT_2F c = D2D1::Point2F(icon.right - 4 * s, icon.top + 4 * s);
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f * a)); g_target->FillEllipse(D2D1::Ellipse(c, r + 1.5f * s, r + 1.5f * s), g_brush.Get());
    g_brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, a)); g_target->FillEllipse(D2D1::Ellipse(c, r, r), g_brush.Get());
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, a));
    g_target->DrawLine(D2D1::Point2F(c.x - 4 * s, c.y), D2D1::Point2F(c.x - 1 * s, c.y + 3 * s), g_brush.Get(), 1.8f * s);
    g_target->DrawLine(D2D1::Point2F(c.x - 1 * s, c.y + 3 * s), D2D1::Point2F(c.x + 4.5f * s, c.y - 3.5f * s), g_brush.Get(), 1.8f * s);
}

void DrawGroupDrag(const D2D1_RECT_F& r, int item) {
    float s = g_layout.scale; App* lead = g_apps[item].get(); std::vector<App*> back;
    for (auto& a : g_apps) if (a.get() != lead && g_selection.count(a.get())) { back.push_back(a.get()); if (back.size() == 2) break; }
    for (int k = (int)back.size(); k >= 1; k--) {
        float o = k * 7 * s;
        if (ID2D1Bitmap* b = GetBitmap(*back[k - 1]))
            g_target->DrawBitmap(b, D2D1::RectF(r.left + o, r.top - o, r.right + o, r.bottom - o), 0.5f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }
    if (ID2D1Bitmap* b = GetBitmap(*lead)) g_target->DrawBitmap(b, r, 0.9f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    float br = 11 * s; D2D1_POINT_2F c = D2D1::Point2F(r.right - 2 * s, r.top + 2 * s);
    g_brush->SetColor(D2D1::ColorF(1.0f, 0.27f, 0.23f, 0.95f)); g_target->FillEllipse(D2D1::Ellipse(c, br, br), g_brush.Get());
    std::wstring t = std::to_wstring(g_selection.size());
    if (g_labelFormat) {
        g_brush->SetColor(D2D1::ColorF(1, 1, 1));
        g_target->DrawText(t.c_str(), (UINT32)t.size(), g_labelFormat.Get(), D2D1::RectF(c.x - br, c.y - br, c.x + br, c.y + br), g_brush.Get());
    }
}

// Paginação

void VisibleRange(const Layout& L, float scroll, int count, int& first, int& last) {
    if (L.horizontal) {
        int p = std::max(0, (int)std::floor(scroll / L.pageWidth));
        first = p * L.perPage; last = std::min(count, (p + 2) * L.perPage);
    } else {
        int firstRow = std::max(0, (int)(scroll / L.cellHeight));
        int lastRow = (int)((scroll + L.viewBottom - L.viewTop) / L.cellHeight) + 1;
        first = firstRow * L.columns; last = std::min(count, (lastRow + 1) * L.columns);
    }
}

float PageFade(const Layout& L, float scroll, int index) {
    if (!L.horizontal) return 1.0f;
    float off = std::abs(index / L.perPage - scroll / L.pageWidth);
    return 1.0f - 0.55f * std::min(1.0f, off);
}

int DotAt(float x, float y) {
    const Layout& L = g_layout;
    if (!L.horizontal || L.pages < 2) return -1;
    float m = 6 * L.scale;
    if (x < L.dots.left || x >= L.dots.right || y < L.dots.top - m || y >= L.dots.bottom + m) return -1;
    return std::clamp((int)((x - L.dots.left - L.dotsH * 0.45f) / L.dotGap), 0, L.pages - 1);
}

void DrawPageDots(const Layout& L, float scroll, float a) {
    if (L.pages < 2 || a <= 0.001f) return;
    float s = L.scale, h = L.dotsH, gap = L.dotGap, hv = EaseInOut(g_dotsHoverAnim);
    float grow = h * 0.10f * hv;
    D2D1_RECT_F r = D2D1::RectF(L.dots.left - grow, L.dots.top - grow, L.dots.right + grow, L.dots.bottom + grow);
    float rad = (r.bottom - r.top) / 2, cy = (r.top + r.bottom) / 2;

    Fill(D2D1::RectF(r.left, r.top + 2 * s, r.right, r.bottom + 2 * s), D2D1::ColorF(0, 0, 0, 0.16f * a), rad);
    Fill(r, D2D1::ColorF(1, 1, 1, (0.09f + 0.05f * hv) * a), rad);
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, (0.14f + 0.10f * hv) * a));
    g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + 0.5f, r.top + 0.5f, r.right - 0.5f, r.bottom - 0.5f), rad - 0.5f, rad - 0.5f), g_brush.Get(), 1.0f);

    float dr = std::max(2.0f * s, std::min(h * 0.13f, gap * 0.3f));
    float x0 = L.dots.left + h * 0.45f + gap * 0.5f;
    for (int i = 0; i < L.pages; i++) {
        float boost = i == g_dotHover ? hv : 0.0f, rr = dr * (1.0f + 0.3f * boost);
        g_brush->SetColor(D2D1::ColorF(1, 1, 1, (0.30f + 0.25f * boost) * a));
        g_target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x0 + i * gap, cy), rr, rr), g_brush.Get());
    }

    float pos = std::clamp(scroll / L.pageWidth, 0.0f, (float)(L.pages - 1));
    float stretch = std::sin((pos - std::floor(pos)) * 3.14159265f);
    float ax = x0 + pos * gap, aw = dr * 3.2f + gap * 0.45f * stretch;
    Fill(D2D1::RectF(ax - aw / 2, cy - dr, ax + aw / 2, cy + dr), D2D1::ColorF(1, 1, 1, 0.92f * a), dr);
}

void DrawSearchBar(const Layout& L, float a) {
    if (a <= 0.001f) return;
    float s = L.scale;
    const D2D1_RECT_F& r = L.search;
    bool box = g_settings.searchBox;
    if (box) {
        float rad = (r.bottom - r.top) / 2;
        Fill(r, D2D1::ColorF(1, 1, 1, 0.14f * a), rad);
        g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.22f * a));
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + 0.5f, r.top + 0.5f, r.right - 0.5f, r.bottom - 0.5f), rad, rad), g_brush.Get(), 1.0f);
    }
    float cx = r.left + (box ? 18 : 12) * s, cy = (r.top + r.bottom) / 2;
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.6f * a));
    g_target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 5.5f * s, 5.5f * s), g_brush.Get(), 1.2f * s);
    g_target->DrawLine(D2D1::Point2F(cx + 4.0f * s, cy + 4.0f * s), D2D1::Point2F(cx + 8.0f * s, cy + 8.0f * s), g_brush.Get(), 1.2f * s);

    D2D1_RECT_F text = D2D1::RectF(r.left + (box ? 34 : 30) * s, r.top, r.right - 20 * s, r.bottom);
    if (!g_searchFormat) return;

    if (g_query.empty()) {
        g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.4f * a));
        g_target->DrawText(L"Search Applications", 19, g_searchFormat.Get(), text, g_brush.Get());
    }

    ComPtr<IDWriteTextLayout> layout;
    g_dwrite->CreateTextLayout(g_query.c_str(), (UINT32)g_query.size(), g_searchFormat.Get(), 10000, text.bottom - text.top, &layout);
    if (!layout) return;
    layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING); DWRITE_TEXT_METRICS metrics{}; layout->GetMetrics(&metrics);
    float textWidth = metrics.widthIncludingTrailingWhitespace, room = text.right - text.left - 3 * s, shift = std::max(0.0f, textWidth - room);
    g_target->PushAxisAlignedClip(text, D2D1_ANTIALIAS_MODE_ALIASED);
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f * a)); g_target->DrawTextLayout(D2D1::Point2F(text.left - shift, text.top), layout.Get(), g_brush.Get());

    if (g_query.empty() || (Now() - g_lastFrame > 0.5 && (int)(Now() * 2) % 2 == 0)) {
        float caretX = std::round(text.left + textWidth - shift) + 1, caretH = 20 * s, mid = (text.top + text.bottom) / 2;
        Fill(D2D1::RectF(caretX, mid - caretH / 2, caretX + std::max(1.0f, s), mid + caretH / 2), D2D1::ColorF(1, 1, 1, 0.6f * a));
    }
    g_target->PopAxisAlignedClip();
}

// ---------- Botão de energia ----------
void DrawPowerButton(const Layout& L, float a) {
    if (!g_settings.showPower || a <= 0.001f || L.btnPower.right <= L.btnPower.left) return;
    float s = L.scale; const D2D1_RECT_F& r = L.btnPower;
    float rad = (r.right - r.left) / 2;
    D2D1_POINT_2F c = D2D1::Point2F((r.left + r.right) / 2, (r.top + r.bottom) / 2);

    Fill(r, D2D1::ColorF(1, 1, 1, (g_powerHover ? 0.20f : 0.10f) * a), rad);
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.18f * a));
    g_target->DrawEllipse(D2D1::Ellipse(c, rad - 0.5f, rad - 0.5f), g_brush.Get(), 1.0f);

    float ir = 8 * s, gap = 0.70f;
    D2D1_POINT_2F p0 = D2D1::Point2F(c.x - ir * std::sin(gap), c.y - ir * std::cos(gap));
    D2D1_POINT_2F p1 = D2D1::Point2F(c.x + ir * std::sin(gap), c.y - ir * std::cos(gap));
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.9f * a));
    ComPtr<ID2D1PathGeometry> path; ComPtr<ID2D1GeometrySink> sink;
    if (SUCCEEDED(g_d2d->CreatePathGeometry(&path)) && SUCCEEDED(path->Open(&sink))) {
        sink->BeginFigure(p0, D2D1_FIGURE_BEGIN_HOLLOW);
        sink->AddArc(D2D1::ArcSegment(p1, D2D1::SizeF(ir, ir), 0, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();
        g_target->DrawGeometry(path.Get(), g_brush.Get(), 1.8f * s);
    }
    g_target->DrawLine(D2D1::Point2F(c.x, c.y - ir * 1.2f), D2D1::Point2F(c.x, c.y - ir * 0.15f), g_brush.Get(), 1.8f * s);
}

void EnableShutdownPrivilege() {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) return;
    TOKEN_PRIVILEGES tp{}; tp.PrivilegeCount = 1; tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid))
        AdjustTokenPrivileges(token, FALSE, &tp, 0, nullptr, nullptr);
    CloseHandle(token);
}

void DoPowerAction(int id) {
    switch (id) {
        case kMenuLock: LockWorkStation(); break;
        case kMenuSleep: {
            using SetSuspendState_t = BOOLEAN(WINAPI*)(BOOLEAN, BOOLEAN, BOOLEAN);
            if (HMODULE h = LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)) {
                if (auto fn = (SetSuspendState_t)GetProcAddress(h, "SetSuspendState")) fn(FALSE, FALSE, FALSE);
                FreeLibrary(h);
            }
            break;
        }
        case kMenuRestart:  EnableShutdownPrivilege(); ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_FLAG_PLANNED); break;
        case kMenuShutdown: EnableShutdownPrivilege(); ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF, SHTDN_REASON_FLAG_PLANNED); break;
        case kMenuSignOut:  ExitWindowsEx(EWX_LOGOFF, 0); break;
    }
}

void DrawFolderHeader(const Layout& L, float a) {
    if (a <= 0.001f) return;
    float s = L.scale;
    float cx = L.btnBack.left + 16 * s, cy = (L.btnBack.top + L.btnBack.bottom) / 2;
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.6f * a));
    g_target->DrawLine(D2D1::Point2F(cx - 4 * s, cy), D2D1::Point2F(cx + 6 * s, cy), g_brush.Get(), 2.0f * s);
    g_target->DrawLine(D2D1::Point2F(cx - 4 * s, cy), D2D1::Point2F(cx + 2 * s, cy - 6 * s), g_brush.Get(), 2.0f * s);
    g_target->DrawLine(D2D1::Point2F(cx - 4 * s, cy), D2D1::Point2F(cx + 2 * s, cy + 6 * s), g_brush.Get(), 2.0f * s);

    if (g_titleFormat && g_openFolderIndex >= 0 && g_openFolderIndex < (int)g_folders.size()) {
        D2D1_RECT_F row = D2D1::RectF(0, L.btnBack.top, (float)L.width, L.btnBack.bottom);
        if (g_renaming) {
            float cx2 = L.width / 2.0f, textWidth = 0;
            ComPtr<IDWriteTextLayout> layout;
            g_dwrite->CreateTextLayout(g_renameText.c_str(), (UINT32)g_renameText.size(), g_titleFormat.Get(), 10000, row.bottom - row.top, &layout);
            if (layout) { DWRITE_TEXT_METRICS metrics{}; layout->GetMetrics(&metrics); textWidth = metrics.widthIncludingTrailingWhitespace; }
            float half = std::max(textWidth / 2 + 18 * s, 90 * s);
            D2D1_RECT_F box = D2D1::RectF(cx2 - half, row.top, cx2 + half, row.bottom);
            Fill(box, D2D1::ColorF(1, 1, 1, 0.14f * a), 8 * s);
            g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.30f * a));
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(box, 8 * s, 8 * s), g_brush.Get(), 1.0f);
            if (g_renameSelectAll && textWidth > 0) {
                Fill(D2D1::RectF(cx2 - textWidth / 2 - 2 * s, row.top + 6 * s, cx2 + textWidth / 2 + 2 * s, row.bottom - 6 * s), D2D1::ColorF(0.33f, 0.55f, 1.0f, 0.55f * a), 3 * s);
            }
            g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f * a));
            g_target->DrawText(g_renameText.c_str(), (UINT32)g_renameText.size(), g_titleFormat.Get(), row, g_brush.Get());
            if (!g_renameSelectAll) {
                float caretX = std::round(cx2 + textWidth / 2) + 1, caretH = 18 * s, mid = (row.top + row.bottom) / 2;
                Fill(D2D1::RectF(caretX, mid - caretH / 2, caretX + std::max(1.0f, s), mid + caretH / 2), D2D1::ColorF(1, 1, 1, 0.8f * a));
            }
        } else {
            const std::wstring& name = g_folders[g_openFolderIndex].name;
            g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f * a));
            g_target->DrawText(name.c_str(), (UINT32)name.size(), g_titleFormat.Get(), row, g_brush.Get());
        }
    }
}

void DrawMessage(PCWSTR text) {
    const Layout& L = g_layout;
    if (!g_messageFormat) return;
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.6f));
    g_target->DrawText(text, (UINT32)wcslen(text), g_messageFormat.Get(), D2D1::RectF(0, L.viewTop, (float)L.width, L.viewTop + 120 * L.scale), g_brush.Get());
}

D2D1_RECT_F FolderMiniSlot(const D2D1_RECT_F& r, int index, float pad) {
    int k = std::min(index, 3);
    float cellW = (r.right - r.left - pad * 3) / 2;
    float cellH = (r.bottom - r.top - pad * 3) / 2;
    float left = r.left + pad + (k % 2) * (cellW + pad);
    float top = r.top + pad + (k / 2) * (cellH + pad);
    return D2D1::RectF(left, top, left + cellW, top + cellH);
}

void DrawFolderIcon(const Folder& folder, const D2D1_RECT_F& r, float opacity = 1.0f) {
    float s = g_layout.scale;
    float pad = 10 * s;
    Fill(r, D2D1::ColorF(1, 1, 1, 0.2f * opacity), 16 * s);
    for (int i = 0; i < 4 && i < (int)folder.apps.size(); i++) {
        D2D1_RECT_F slot = FolderMiniSlot(r, i, pad);
        App& app = *folder.apps[i];
        if (ID2D1Bitmap* bitmap = GetBitmap(app)) {
            g_target->DrawBitmap(bitmap, slot, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }
    }
}

void DrawMainGrid(const Layout& L, const std::vector<int>& items, float scroll, float alpha, bool interactive, int hiddenFolderItem, float hiddenFade) {
    float s = L.scale;
    if (interactive) {
        if (!g_appsLoaded && g_apps.empty()) { DrawMessage(L"Loading apps..."); return; }
        if (items.empty()) { DrawMessage(g_query.empty() ? L"No apps" : L"No apps found"); return; }
    } else if (items.empty()) {
        return;
    }
    g_target->PushAxisAlignedClip(D2D1::RectF(0, L.viewTop, (float)L.width, L.viewBottom), D2D1_ANTIALIAS_MODE_ALIASED);
    int first, last; VisibleRange(L, scroll, (int)items.size(), first, last);

    for (int i = first; i < last; i++) {
        int item = items[i];
        if (item < 0) continue;
        if (IsFolderItem(item) ? item - 1000 >= (int)g_folders.size() : !ResolveApp(item)) continue;
        Cell c = CellAtIn(L, scroll, i);

        bool sel = interactive && IsSelected(i);
        if (sel) Fill(c.highlight, D2D1::ColorF(0.04f, 0.52f, 1.0f, 0.22f), 12 * s);
        else if (interactive && (i == g_selected || i == g_hover))
            Fill(c.highlight, D2D1::ColorF(1, 1, 1, i == g_selected ? 0.18f : 0.10f), 12 * s);
        float dragDim = (interactive && ((i == g_pressed && g_isDragging) || i == g_dragItem || (sel && GroupDrag()))) ? 0.4f : 1.0f;
        float opacity = dragDim * alpha * PageFade(L, scroll, i);

        if (IsFolderItem(item)) {
            const Folder& f = g_folders[item - 1000];
            float labelOpacity = opacity;
            if (item == hiddenFolderItem) labelOpacity *= 1.0f - hiddenFade;
            else DrawFolderIcon(f, c.icon, opacity);
            if (g_labelFormat) {
                g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f * labelOpacity));
                g_target->DrawText(f.name.c_str(), (UINT32)f.name.size(), g_labelFormat.Get(), c.label, g_brush.Get());
            }
        } else {
            App& app = *ResolveApp(item);
            if (ID2D1Bitmap* bitmap = GetBitmap(app)) {
                g_target->DrawBitmap(bitmap, c.icon, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            } else {
                Fill(c.icon, D2D1::ColorF(1, 1, 1, 0.12f * opacity), 14 * s);
            }
            if (sel) DrawCheckBadge(c.icon, opacity);
            if (g_labelFormat) {
                D2D1_RECT_F shadow = c.label; shadow.top += 1 * s; shadow.bottom += 1 * s;
                g_brush->SetColor(D2D1::ColorF(0, 0, 0, 0.45f * opacity)); g_target->DrawText(app.name.c_str(), (UINT32)app.name.size(), g_labelFormat.Get(), shadow, g_brush.Get());
                g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f * opacity)); g_target->DrawText(app.name.c_str(), (UINT32)app.name.size(), g_labelFormat.Get(), c.label, g_brush.Get());
            }
        }
    }

    if (interactive && g_isDragging && g_dragItem >= 0 && g_dragItem < (int)items.size() && g_dragX >= 0) {
        int item = items[g_dragItem];
        D2D1_RECT_F dragRect = D2D1::RectF(g_dragX - L.icon / 2, g_dragY - L.icon / 2, g_dragX + L.icon / 2, g_dragY + L.icon / 2);
        if (IsFolderItem(item)) {
            if (item - 1000 < (int)g_folders.size()) DrawFolderIcon(g_folders[item - 1000], dragRect, 0.8f);
        } else if (App* da = ResolveApp(item)) {
            if (GroupDrag() && item < 1000) DrawGroupDrag(dragRect, item);
            else if (ID2D1Bitmap* bitmap = GetBitmap(*da))
                g_target->DrawBitmap(bitmap, dragRect, 0.8f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }
    }

    g_target->PopAxisAlignedClip();

    if (interactive) {
        if (L.horizontal) DrawPageDots(L, scroll, 1.0f);
        else if (float maxScroll = MaxScroll(); maxScroll > 0) {
            float track = L.viewBottom - L.viewTop, thumb = std::max(40 * s, track * track / L.contentHeight);
            float top = L.viewTop + (track - thumb) * (g_scroll / maxScroll), right = (float)L.width - 12 * s;
            Fill(D2D1::RectF(right - 4 * s, top, right, top + thumb), D2D1::ColorF(1, 1, 1, 0.25f), 2 * s);
        }
    }
}

void DrawFolderPanel(float t, const D2D1_RECT_F& from, float fromPad, bool settled) {
    const Layout& L = g_layout; float s = L.scale;
    if (g_openFolderIndex < 0 || g_openFolderIndex >= (int)g_folders.size()) return;
    const Folder& folder = g_folders[g_openFolderIndex];

    D2D1_RECT_F rect = LerpRect(from, L.panel, t);
    float radius = Lerp(16 * s, g_settings.folderRadius * s, t);

    Fill(rect, D2D1::ColorF(1, 1, 1, Lerp(0.2f, 0.04f, t)), radius);
    if (g_settings.borderWidth > 0) {
        g_brush->SetColor(Alpha(g_settings.borderColor, g_settings.borderOpacity * t));
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), g_brush.Get(), g_settings.borderWidth * s);
    }

    D2D1_RECT_F clip = D2D1::RectF(rect.left, Lerp(rect.top, L.viewTop, t), rect.right, Lerp(rect.bottom, L.viewBottom, t));
    g_target->PushAxisAlignedClip(clip, D2D1_ANTIALIAS_MODE_ALIASED);

    int count = (int)g_layoutItems.size();
    int first = 0, last = count;
    if (settled) VisibleRange(L, g_scroll, count, first, last);

    float labelFade = std::clamp((t - 0.5f) / 0.5f, 0.0f, 1.0f);
    for (int i = first; i < last; i++) {
        int appIdx = g_layoutItems[i];
        if (appIdx < 0 || appIdx >= (int)folder.apps.size()) continue;
        App& app = *folder.apps[appIdx];
        Cell c = CellAtIn(L, g_scroll, i);

        if (settled && (i == g_selected || i == g_hover)) {
            Fill(c.highlight, D2D1::ColorF(1, 1, 1, i == g_selected ? 0.18f : 0.10f), 12 * s);
        }

        D2D1_RECT_F slot = FolderMiniSlot(from, appIdx, fromPad);
        float delay = std::min(appIdx, 8) * 0.035f;
        float localT = std::clamp((t - delay) / std::max(0.01f, 1.0f - delay), 0.0f, 1.0f);
        localT = EaseInOut(localT);

        D2D1_RECT_F icon = LerpRect(slot, c.icon, localT);
        float iconScale = 0.72f + 0.28f * localT;
        float centerX = (icon.left + icon.right) * 0.5f, centerY = (icon.top + icon.bottom) * 0.5f;
        float halfWidth = (icon.right - icon.left) * 0.5f * iconScale, halfHeight = (icon.bottom - icon.top) * 0.5f * iconScale;
        icon = D2D1::RectF(centerX - halfWidth, centerY - halfHeight, centerX + halfWidth, centerY + halfHeight);

        float dragDim = (settled && i == g_pressed && g_isDragging) ? 0.4f : 1.0f;
        float opacity = dragDim * localT * PageFade(L, g_scroll, i);

        if (ID2D1Bitmap* bitmap = GetBitmap(app)) {
            g_target->DrawBitmap(bitmap, icon, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }

        if (g_labelFormat && labelFade > 0.0f) {
            D2D1_RECT_F label = c.label;
            if (!settled) {
                float cx = (icon.left + icon.right) / 2, w = c.label.right - c.label.left;
                float top = icon.bottom + std::round(8 * s * localT);
                label = D2D1::RectF(cx - w / 2, top, cx + w / 2, top + L.labelHeight);
            }
            float lo = labelFade * opacity;
            D2D1_RECT_F shadow = label; shadow.top += 1 * s; shadow.bottom += 1 * s;
            g_brush->SetColor(D2D1::ColorF(0, 0, 0, 0.45f * lo)); g_target->DrawText(app.name.c_str(), (UINT32)app.name.size(), g_labelFormat.Get(), shadow, g_brush.Get());
            g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f * lo)); g_target->DrawText(app.name.c_str(), (UINT32)app.name.size(), g_labelFormat.Get(), label, g_brush.Get());
        }
    }

    g_target->PopAxisAlignedClip();

    if (settled && g_isDragging && g_dragItem >= 0 && g_dragItem < (int)g_layoutItems.size() && g_dragX >= 0) {
        int idx = g_layoutItems[g_dragItem];
        if (idx >= 0 && idx < (int)folder.apps.size()) {
            if (ID2D1Bitmap* bmp = GetBitmap(*folder.apps[idx])) {
                D2D1_RECT_F r = D2D1::RectF(g_dragX - L.icon / 2, g_dragY - L.icon / 2, g_dragX + L.icon / 2, g_dragY + L.icon / 2);
                g_target->DrawBitmap(bmp, r, 0.8f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
        }
    }

    if (settled) {
        if (L.horizontal) DrawPageDots(L, g_scroll, 1.0f);
        else if (float maxScroll = MaxScroll(); maxScroll > 0) {
            float track = L.viewBottom - L.viewTop, thumb = std::max(40 * s, track * track / L.contentHeight);
            float top = L.viewTop + (track - thumb) * (g_scroll / maxScroll), right = L.panel.right - 8 * s;
            Fill(D2D1::RectF(right - 4 * s, top, right, top + thumb), D2D1::ColorF(1, 1, 1, 0.25f), 2 * s);
        }
    }
}

void Render() {
    if (!g_shown || !EnsureTarget()) return;
    const Layout& L = g_layout;
    if (!EnsureSurface(g_surface, L.width, L.height + L.extraBottom)) return;
    RECT bind{0, 0, L.width, L.height + L.extraBottom};
    if (FAILED(g_target->BindDC(g_surface.dc, &bind))) return;
    float W = (float)L.width, H = (float)L.height;
    float radius = g_settings.fullscreen ? 0.0f : g_settings.cornerRadius * L.scale;

    g_target->BeginDraw(); g_target->SetTransform(D2D1::Matrix3x2F::Identity());
    g_target->Clear(D2D1::ColorF(0, 0, 0, 0));
    if (g_settings.fullscreen) Fill(D2D1::RectF(0, 0, W, H), D2D1::ColorF(0, 0, 0, 1 / 255.0f));
    AnimFx fx = ComputeAnim();
    g_target->SetTransform(fx.m);

    if (g_settings.blurMode == BlurMode::Custom) {
        if (ID2D1Bitmap* bitmap = GetBackdropBitmap()) {
            ComPtr<ID2D1RoundedRectangleGeometry> clipGeo;
            g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(0, 0, W, H), radius, radius), &clipGeo);
            g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clipGeo.Get()), nullptr);
            g_target->DrawBitmap(bitmap, D2D1::RectF(0, 0, W, H), 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(0, 0, g_backdropImage.sourceWidth, g_backdropImage.sourceHeight));
            g_target->PopLayer();
        }
    }

    g_brush->SetColor(Alpha(g_settings.tint, g_settings.opacity));
    g_target->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(0, 0, W, H), radius, radius), g_brush.Get());

    if (!g_settings.fullscreen && g_settings.borderWidth > 0) {
        float bw = g_settings.borderWidth * L.scale, h = bw / 2, rr = std::max(0.0f, radius - h);
        g_brush->SetColor(Alpha(g_settings.borderColor, g_settings.borderOpacity));
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(h, h, W - h, H - h), rr, rr), g_brush.Get(), bw);
    }

    D2D1::Matrix3x2F zoomMatrix = fx.m;

    if (g_openFolderIndex >= 0 && g_openFolderIndex < (int)g_folders.size()) {
        float t = EaseInOut(g_folderAnim);
        bool settled = FolderSettled();
        float s = L.scale;
        float bgScale = 1.0f - 0.05f * EaseInOut(t);
        D2D1_POINT_2F center = D2D1::Point2F(W / 2, H / 2);
        D2D1::Matrix3x2F bgMatrix = D2D1::Matrix3x2F::Scale(bgScale, bgScale, center) * zoomMatrix;
        float bgFade = 1.0f - 0.80f * EaseInOut(t);
        auto drawBackground = [&] {
            g_target->SetTransform(bgMatrix);
            DrawSearchBar(g_mainLayout, 1.0f - 0.85f * t);
            DrawPowerButton(g_mainLayout, 1.0f - t);
            DrawMainGrid(g_mainLayout, g_mainItems, g_mainScroll, bgFade, false, 1000 + g_openFolderIndex, t);
        };
        D2D1_RECT_F from = D2D1::RectF(
            center.x + (g_folderOrigin.left - center.x) * bgScale, center.y + (g_folderOrigin.top - center.y) * bgScale,
            center.x + (g_folderOrigin.right - center.x) * bgScale, center.y + (g_folderOrigin.bottom - center.y) * bgScale);

        float sigma = g_settings.folderBlur * 0.3f * s * t;
        bool blurred = false;
        ComPtr<ID2D1DeviceContext> dc;
        if (sigma > 0.5f && SUCCEEDED(g_target.As(&dc))) {
            if (ID2D1Bitmap1* layer = EnsureLayerBitmap(dc.Get(), L.width, L.height)) {
                ComPtr<ID2D1Image> old; dc->GetTarget(&old);
                dc->SetTarget(layer); dc->Clear(D2D1::ColorF(0, 0, 0, 0));
                drawBackground();
                dc->SetTarget(old.Get());
                g_target->SetTransform(D2D1::Matrix3x2F::Identity());

                D2D1_RECT_F panel = LerpRect(from, L.panel, t);
                float pr = Lerp(16 * s, g_settings.folderRadius * s, t);
                ComPtr<ID2D1RoundedRectangleGeometry> panelGeo;
                ComPtr<ID2D1RectangleGeometry> fullGeo;
                ComPtr<ID2D1PathGeometry> outside; ComPtr<ID2D1GeometrySink> sink;
                ComPtr<ID2D1Effect> blur;
                g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(panel, pr, pr), &panelGeo);
                g_d2d->CreateRectangleGeometry(D2D1::RectF(-W, -H, 2 * W, 2 * H), &fullGeo);
                if (panelGeo && fullGeo && SUCCEEDED(g_d2d->CreatePathGeometry(&outside)) && SUCCEEDED(outside->Open(&sink)) &&
                    SUCCEEDED(fullGeo->CombineWithGeometry(panelGeo.Get(), D2D1_COMBINE_MODE_EXCLUDE, nullptr, sink.Get())) &&
                    SUCCEEDED(sink->Close()) && SUCCEEDED(dc->CreateEffect(kClsidGaussianBlur, &blur))) {
                    blur->SetInput(0, layer);
                    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, sigma);
                    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
                    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), outside.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, zoomMatrix), nullptr);
                    g_target->DrawBitmap(layer, D2D1::RectF(0, 0, W, H));
                    g_target->PopLayer();
                    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), panelGeo.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, zoomMatrix), nullptr);
                    dc->DrawImage(blur.Get());
                    g_target->PopLayer();
                    blurred = true;
                }
            }
        }
        if (!blurred) drawBackground();

        g_target->SetTransform(zoomMatrix);
        DrawFolderPanel(t, from, 10 * s * bgScale, settled);
        DrawFolderHeader(L, std::clamp((t - 0.35f) / 0.65f, 0.0f, 1.0f));
    } else {
        g_target->SetTransform(zoomMatrix);
        DrawSearchBar(L, 1.0f);
        DrawPowerButton(L, 1.0f);
        DrawMainGrid(L, g_layoutItems, g_scroll, 1.0f, true, -1, 0.0f);
    }
    DrawPopupMenu(fx.m);
    if (fx.flash > 0.001f) { g_target->SetTransform(fx.m); Fill(D2D1::RectF(0, 0, W, H), D2D1::ColorF(1, 1, 1, fx.flash), radius); }
    g_target->SetTransform(D2D1::Matrix3x2F::Identity());

    if (g_target->EndDraw() == D2DERR_RECREATE_TARGET) { ResetTarget(); return; }
    POINT dst{L.area.left, L.area.top}, src{0, 0}; SIZE size{L.width, L.height + L.extraBottom};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, (BYTE)std::lround(255 * std::clamp(fx.alpha, 0.0f, 1.0f)), AC_SRC_ALPHA};
    UpdateLayeredWindow(g_wnd, nullptr, &dst, &size, g_surface.dc, &src, 0, &blend, ULW_ALPHA);
}

void StartFrames() {
    if (!g_frameTimer) { g_frameTimer = true; g_lastFrame = Now(); SetTimer(g_wnd, kTimerFrame, 10, nullptr); }
}

void BeginRename() {
    if (g_openFolderIndex < 0 || g_openFolderIndex >= (int)g_folders.size()) return;
    g_renaming = true; g_renameSelectAll = true;
    g_renameText = g_folders[g_openFolderIndex].name;
    Render();
}

void CommitRename() {
    if (!g_renaming) return;
    g_renaming = false;
    if (g_openFolderIndex >= 0 && g_openFolderIndex < (int)g_folders.size()) {
        std::wstring name = g_renameText;
        name.erase(0, name.find_first_not_of(L' '));
        name.erase(name.find_last_not_of(L' ') + 1);
        if (!name.empty()) { g_folders[g_openFolderIndex].name = name; SaveFolders(); }
    }
    Render();
}

void CancelRename() {
    if (!g_renaming) return;
    g_renaming = false; Render();
}

void OpenFolder(int folderIndex) {
    if (folderIndex < 0 || folderIndex >= (int)g_folders.size() || g_openFolderIndex >= 0) return;

    g_mainLayout = g_layout; g_mainScroll = g_scroll; g_mainItems = g_layoutItems;

    int layoutIdx = -1;
    for (int i = 0; i < (int)g_mainItems.size(); i++) {
        if (g_mainItems[i] == 1000 + folderIndex) { layoutIdx = i; break; }
    }
    if (layoutIdx >= 0) {
        g_folderOrigin = CellAtIn(g_mainLayout, g_mainScroll, layoutIdx).icon;
    } else {
        float cx = g_mainLayout.width / 2.0f, cy = g_mainLayout.height / 2.0f, h = g_mainLayout.icon / 2;
        g_folderOrigin = D2D1::RectF(cx - h, cy - h, cx + h, cy + h);
    }

    g_openFolderIndex = folderIndex;
    g_folderAnim = 0.0f; g_folderClosing = false;
    g_query.clear(); g_hover = g_selected = g_pressed = -1; g_scroll = g_scrollTarget = 0;
    g_dotsHover = false; g_dotHover = -1; g_powerHover = false;
    RebuildLayout(); ComputeLayout(); Render(); StartFrames();
}

void FinishFolderClose() {
    g_openFolderIndex = -1; g_folderClosing = false; g_folderAnim = 0.0f; g_renaming = false;
    g_hover = g_selected = g_pressed = -1; g_dotsHover = false; g_dotHover = -1;
    g_scroll = g_scrollTarget = g_mainScroll;
    RebuildLayout(); ComputeLayout();
}

void CloseFolder(bool animate = true) {
    if (g_openFolderIndex < 0) return;
    CommitRename();
    if (!animate) { FinishFolderClose(); return; }
    if (g_folderClosing) return;
    g_folderClosing = true; g_hover = g_selected = g_pressed = -1;
    StartFrames();
}

void HideNow() {
    ShowWindow(g_wnd, SW_HIDE);
    g_openFolderIndex = -1; g_folderAnim = 0.0f; g_folderClosing = false; g_renaming = false; g_mainItems.clear();
    g_shown = false; g_closing = false; g_appear = 0; g_backdropImage = {}; FreeSurface(g_surface); g_trackingMouse = false;
    g_menu.open = false; g_selection.clear(); g_powerHover = false;
}

void OnFrame() {
    double now = Now(); float dt = (float)std::clamp(now - g_lastFrame, 0.0, 0.05); g_lastFrame = now;
    bool animating = false;
    float target = g_closing ? 0.0f : 1.0f;
    if (g_appear != target) {
        float speed = g_settings.animSpeed / AnimSeconds(g_closing ? g_settings.closeAnim : g_settings.openAnim, g_closing);
        g_appear = g_closing ? std::max(0.0f, g_appear - dt * speed) : std::min(1.0f, g_appear + dt * speed);
        animating = g_appear != target;
    }
    if (g_scroll != g_scrollTarget && !g_swiping) {
        float k = (g_layout.horizontal ? 10.0f : 16.0f) * g_settings.animSpeed;
        g_scroll += (g_scrollTarget - g_scroll) * std::min(1.0f, dt * k);
        if (std::abs(g_scrollTarget - g_scroll) < 0.5f) g_scroll = g_scrollTarget;
        animating |= g_scroll != g_scrollTarget;
    }
    {
        float hvTarget = g_dotsHover ? 1.0f : 0.0f, st = dt * 7 * g_settings.animSpeed;
        if (g_dotsHoverAnim != hvTarget) {
            g_dotsHoverAnim = hvTarget > g_dotsHoverAnim ? std::min(hvTarget, g_dotsHoverAnim + st) : std::max(hvTarget, g_dotsHoverAnim - st);
            animating |= g_dotsHoverAnim != hvTarget;
        }
    }
    if (g_menu.open) {
        float tgt = g_menu.closing ? 0.0f : 1.0f, st = dt * g_settings.animSpeed / (g_menu.closing ? 0.10f : 0.15f);
        g_menu.anim = g_menu.closing ? std::max(0.0f, g_menu.anim - st) : std::min(1.0f, g_menu.anim + st);
        if (g_menu.closing && g_menu.anim <= 0) g_menu.open = false;
        animating |= g_menu.open && g_menu.anim != tgt;
    }
    if (g_openFolderIndex >= 0) {
        float folderTarget = g_folderClosing ? 0.0f : 1.0f;
        if (g_folderAnim != folderTarget) {
            float step = dt * g_settings.animSpeed / (g_folderClosing ? kFolderCloseSeconds : kFolderOpenSeconds);
            g_folderAnim = g_folderClosing ? std::max(0.0f, g_folderAnim - step) : std::min(1.0f, g_folderAnim + step);
            animating |= g_folderAnim != folderTarget;
        }
        if (g_folderClosing && g_folderAnim <= 0.0f) FinishFolderClose();
    }
    if (g_closing && g_appear <= 0) { HideNow(); animating = false; }
    else Render();
    if (!animating) { KillTimer(g_wnd, kTimerFrame); g_frameTimer = false; }
}

////////////////////////////////////////////////////////////////////////////////
// Actions

void ForceForeground(HWND hwnd) { INPUT input{}; SendInput(1, &input, sizeof(INPUT)); SetForegroundWindow(hwnd); SetFocus(hwnd); }

void PlaceWindows() {
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (g_settings.fullscreen) RaiseDockWindows();
}

void Open() {
    if (g_shown) { if (g_closing) { g_closing = false; ForceForeground(g_wnd); PlaceWindows(); StartFrames(); } return; }
    POINT pt; GetCursorPos(&pt); HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    UINT dpiX = 96, dpiY = 96; GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    Layout& L = g_layout;
    if (g_settings.fullscreen) {
        L.area = AvailableArea(monitor);
    } else {
        MONITORINFO mi{sizeof(mi)}; GetMonitorInfoW(monitor, &mi);
        int workWidth = mi.rcWork.right - mi.rcWork.left, workHeight = mi.rcWork.bottom - mi.rcWork.top;
        int winWidth = (workWidth * g_settings.windowWidth) / 100, winHeight = (workHeight * g_settings.windowHeight) / 100;
        L.area.left = mi.rcWork.left + (workWidth - winWidth) / 2; L.area.top = mi.rcWork.top + (workHeight - winHeight) / 2;
        L.area.right = L.area.left + winWidth; L.area.bottom = L.area.top + winHeight;
    }
    L.width = L.area.right - L.area.left; L.height = L.area.bottom - L.area.top;
    L.scale = dpiX / 96.0f; EnsureFormats(L.scale);
    {
        MONITORINFO mi{sizeof(mi)}; GetMonitorInfoW(monitor, &mi);
        g_slideDistance = (float)(mi.rcMonitor.bottom - L.area.top);
        bool slide = g_settings.openAnim == Anim::Slide || g_settings.closeAnim == Anim::Slide;
        L.extraBottom = slide ? (int)std::max<LONG>(0, mi.rcMonitor.bottom - L.area.bottom) : 0;
    }
    g_menu.open = false; g_selection.clear();
    g_query.clear(); g_openFolderIndex = -1; g_folderAnim = 0.0f; g_folderClosing = false; g_renaming = false;
    g_hover = g_selected = g_pressed = -1; g_scroll = g_scrollTarget = 0; g_wheelDelta = 0; g_dragItem = -1; g_isDragging = false;
    g_dotsHover = false; g_dotHover = -1; g_dotsHoverAnim = 0; g_swiping = false; g_powerHover = false;
    RebuildLayout(); ComputeLayout();
    if (g_settings.blurMode == BlurMode::Custom) CaptureBackdrop(L.area, g_settings.blurAmount / 100.0f * 60 * L.scale);
    g_shown = true; g_closing = false; g_appear = 0; Render(); ShowWindow(g_wnd, SW_SHOW);
    ForceForeground(g_wnd); PlaceWindows(); StartFrames();
    if (!g_appsLoaded || Now() - g_appsTime > 60 || g_appsIconPixels != IconPixels()) StartLoadingApps();
}

void Close() {
    if (!g_shown || g_closing) return;
    CommitRename();
    g_closing = true; g_pressed = -1; g_swiping = false;
    if (GetCapture() == g_wnd) ReleaseCapture();
    StartFrames();
}

void Toggle() { if (g_shown && !g_closing) Close(); else Open(); }

void LaunchApp(int item) {
    if (item < 0) return;
    const App* ap = nullptr;
    if (g_openFolderIndex >= 0) {
        if (g_openFolderIndex >= (int)g_folders.size() || item >= (int)g_folders[g_openFolderIndex].apps.size()) return;
        ap = g_folders[g_openFolderIndex].apps[item].get();
    } else {
        if (IsFolderItem(item)) { OpenFolder(item - 1000); return; }
        ap = ResolveApp(item);
    }
    if (!ap) return;
    SHELLEXECUTEINFOW sei{sizeof(sei)}; sei.fMask = SEE_MASK_ASYNCOK; sei.lpFile = ap->target.c_str(); sei.nShow = SW_SHOWNORMAL;
    AllowSetForegroundWindow(ASFW_ANY); ShellExecuteExW(&sei); Close();
}

int CurrentPage() { return g_layout.horizontal ? (int)std::lround(g_scrollTarget / g_layout.pageWidth) : 0; }

void GoToPage(int page) {
    g_scrollTarget = std::clamp(page, 0, g_layout.pages - 1) * g_layout.pageWidth; StartFrames();
}

void EnsureVisible(int index) {
    if (index < 0) return;
    const Layout& L = g_layout;
    if (L.horizontal) { g_scrollTarget = std::clamp((index / L.perPage) * L.pageWidth, 0.0f, MaxScroll()); return; }
    int row = index / L.columns, firstRow = (int)std::lround(g_scrollTarget / L.cellHeight);
    if (row < firstRow) g_scrollTarget = row * L.cellHeight;
    else if (row >= firstRow + L.visibleRows) g_scrollTarget = (row - L.visibleRows + 1) * L.cellHeight;
    g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll());
}

void ScrollRows(int rows) {
    if (g_layout.horizontal) { GoToPage(CurrentPage() + (rows > 0) - (rows < 0)); return; }
    float cell = g_layout.cellHeight, target = (std::lround(g_scrollTarget / cell) + rows) * cell;
    g_scrollTarget = std::clamp(target, 0.0f, MaxScroll()); StartFrames();
}

void QueryChanged() {
    if (g_openFolderIndex >= 0) FinishFolderClose();
    RebuildLayout(); g_hover = -1; g_selected = g_query.empty() || g_layoutItems.empty() ? -1 : 0;
    g_scroll = g_scrollTarget = 0; ComputeLayout(); Render();
}

std::wstring ReadClipboardLine() {
    std::wstring line;
    if (!OpenClipboard(g_wnd)) return line;
    if (HANDLE data = GetClipboardData(CF_UNICODETEXT)) {
        if (auto* text = (PCWSTR)GlobalLock(data)) {
            line = text; GlobalUnlock(data);
            line = line.substr(0, line.find_first_of(L"\r\n"));
        }
    }
    CloseClipboard();
    return line;
}

void PasteText() { g_query += ReadClipboardLine().substr(0, 200); QueryChanged(); }

void OnRenameChar(WCHAR ch) {
    auto dropSelection = [] { if (g_renameSelectAll) { g_renameText.clear(); g_renameSelectAll = false; } };
    switch (ch) {
        case 8:
            if (g_renameSelectAll) dropSelection();
            else if (!g_renameText.empty()) {
                g_renameText.pop_back();
                if (!g_renameText.empty() && IS_HIGH_SURROGATE(g_renameText.back())) g_renameText.pop_back();
            }
            break;
        case 0x7F: g_renameText.clear(); g_renameSelectAll = false; break;
        case 0x16: { std::wstring line = ReadClipboardLine(); if (!line.empty()) { dropSelection(); g_renameText += line; } break; }
        default: if (ch >= 32) { dropSelection(); g_renameText += ch; } break;
    }
    if (g_renameText.size() > 60) g_renameText.resize(60);
    Render();
}

void OnChar(WCHAR ch) {
    if (g_menu.open) return;
    if (g_renaming) { OnRenameChar(ch); return; }
    bool edits = ch == 8 || ch == 0x7F || ch == 0x16 || ch >= 32;
    if (edits && g_openFolderIndex >= 0) { FinishFolderClose(); Render(); }
    switch (ch) {
        case 8: if (!g_query.empty()) { g_query.pop_back(); if (!g_query.empty() && IS_HIGH_SURROGATE(g_query.back())) g_query.pop_back(); QueryChanged(); } return;
        case 0x7F: { size_t end = g_query.find_last_not_of(L' '); if (end == std::wstring::npos) g_query.clear(); else { size_t space = g_query.find_last_of(L' ', end); g_query.resize(space == std::wstring::npos ? 0 : space + 1); } QueryChanged(); return; }
        case 0x16: PasteText(); return;
    }
    if (ch >= 32 && g_query.size() < 200) { g_query += ch; QueryChanged(); }
}

void MoveSelection(int delta) {
    if (g_layoutItems.empty()) return;
    int count = (int)g_layoutItems.size();
    if (g_selected < 0) g_selected = 0;
    else {
        int next = g_selected + delta;
        if (next >= 0 && next < count) g_selected = next;
        else if (delta > 0 && delta == g_layout.columns) g_selected = count - 1;
    }
    EnsureVisible(g_selected); Render(); StartFrames();
}

void CloseMenu();
void MenuStep(int d);
void ExecuteMenu(int id);

void OnKeyDown(WPARAM key) {
    if (g_menu.open && !g_menu.closing) {
        if (key == VK_ESCAPE) CloseMenu();
        else if (key == VK_DOWN) MenuStep(1);
        else if (key == VK_UP) MenuStep(-1);
        else if (key == VK_RETURN && g_menu.hover >= 0) ExecuteMenu(g_menu.items[g_menu.hover].id);
        return;
    }
    if (key == VK_ESCAPE && !g_selection.empty() && !g_renaming) { g_selection.clear(); Render(); return; }
    if (g_renaming) {
        if (key == VK_ESCAPE) CancelRename();
        else if (key == VK_RETURN) CommitRename();
        return;
    }
    if (key != VK_ESCAPE && !FolderSettled()) return;
    switch (key) {
        case VK_ESCAPE:
            if (!g_query.empty()) { g_query.clear(); QueryChanged(); }
            else if (g_openFolderIndex >= 0) CloseFolder();
            else Close();
            break;
        case VK_RETURN: if (g_selected >= 0 && g_selected < (int)g_layoutItems.size()) LaunchApp(g_layoutItems[g_selected]); break;
        case VK_LEFT: MoveSelection(-1); break;
        case VK_RIGHT: MoveSelection(1); break;
        case VK_UP: MoveSelection(-g_layout.columns); break;
        case VK_DOWN: MoveSelection(g_layout.columns); break;
        case VK_PRIOR: ScrollRows(-g_layout.visibleRows); break;
        case VK_NEXT: ScrollRows(g_layout.visibleRows); break;
    }
}

void MoveAppOutOfFolder(int appIdx) {
    int fi = g_openFolderIndex;
    if (fi < 0 || fi >= (int)g_folders.size()) return;
    auto& apps = g_folders[fi].apps;
    if (appIdx < 0 || appIdx >= (int)apps.size()) return;

    g_apps.push_back(apps[appIdx]);
    apps.erase(apps.begin() + appIdx);
    std::stable_sort(g_apps.begin(), g_apps.end(), [](const auto& a, const auto& b) { return NameLess(a->name, b->name); });

    if (apps.empty()) {
        FinishFolderClose();
        g_folders.erase(g_folders.begin() + fi);
    } else {
        g_hover = g_selected = -1;
        BuildMainItems(g_mainItems);
    }
    SaveFolders();
    RebuildLayout(); ComputeLayout(); Render();
}

// Fixar na barra de tarefas

bool IsOverTaskbar(float cx, float cy) {
    POINT pt{g_layout.area.left + (LONG)cx, g_layout.area.top + (LONG)cy};
    for (PCWSTR cls : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd", kDockClass}) {
        HWND h = nullptr;
        while ((h = FindWindowExW(nullptr, h, cls, nullptr))) {
            RECT rc;
            if (IsWindowVisible(h) && GetWindowRect(h, &rc) && PtInRect(&rc, pt)) return true;
        }
    }
    return false;
}

bool PinToTaskbar(const std::wstring& target) {
    ComPtr<IShellItem> item; ComPtr<IContextMenu> cm;
    if (FAILED(SHCreateItemFromParsingName(target.c_str(), nullptr, IID_PPV_ARGS(&item))) ||
        FAILED(item->BindToHandler(nullptr, kBhidSFUIObject, IID_PPV_ARGS(&cm)))) return false;
    HMENU menu = CreatePopupMenu(); if (!menu) return false;
    constexpr UINT kFirst = 1; bool ok = false;
    auto invoke = [&](LPCSTR verbA, LPCWSTR verbW) {
        CMINVOKECOMMANDINFOEX ici{sizeof(ici)};
        ici.fMask = CMIC_MASK_UNICODE; ici.hwnd = g_wnd; ici.nShow = SW_SHOWNORMAL;
        ici.lpVerb = verbA; ici.lpVerbW = verbW;
        return SUCCEEDED(cm->InvokeCommand((CMINVOKECOMMANDINFO*)&ici));
    };
    if (SUCCEEDED(cm->QueryContextMenu(menu, 0, kFirst, 0x7FFF, CMF_NORMAL))) {
        for (int i = 0, n = GetMenuItemCount(menu); i < n && !ok; i++) {
            UINT id = GetMenuItemID(menu, i);
            if (id == (UINT)-1 || id < kFirst) continue;
            WCHAR verb[128] = {};
            if (FAILED(cm->GetCommandString(id - kFirst, GCS_VERBW, nullptr, (LPSTR)verb, ARRAYSIZE(verb)))) continue;
            std::wstring v = ToLower(verb);
            if (v == L"taskbarpin" || v == L"pintotaskbar") ok = invoke(MAKEINTRESOURCEA(id - kFirst), MAKEINTRESOURCEW(id - kFirst));
        }
    }
    if (!ok) ok = invoke("taskbarpin", L"taskbarpin");
    DestroyMenu(menu);
    return ok;
}

std::vector<std::shared_ptr<App>> TakeApps(const std::unordered_set<App*>& set) {
    std::vector<std::shared_ptr<App>> out;
    for (auto& a : g_apps) if (set.count(a.get())) out.push_back(a);
    g_apps.erase(std::remove_if(g_apps.begin(), g_apps.end(), [&](const std::shared_ptr<App>& a) { return set.count(a.get()) != 0; }), g_apps.end());
    return out;
}

void CreateFolderFromSelection() {
    Folder f; f.name = L"New Folder";
    auto taken = TakeApps(g_selection); g_selection.clear();
    f.sortKey = taken.empty() ? f.name : taken[0]->name;
    f.apps = std::move(taken);
    g_folders.push_back(std::move(f)); SaveFolders();
    g_hover = g_selected = -1; RebuildLayout(); ComputeLayout();
    OpenFolder((int)g_folders.size() - 1); BeginRename();
}

void EndDrag() {
    if (g_dragItem < 0) return;
    int fromIdx = g_dragItem;
    if (fromIdx >= (int)g_layoutItems.size()) { g_dragItem = -1; return; }

    if (g_settings.dragToPin && IsOverTaskbar(g_dragX, g_dragY)) {
        int item = g_layoutItems[fromIdx]; g_dragItem = -1;
        App* app = nullptr;
        if (g_openFolderIndex >= 0) {
            if (g_openFolderIndex < (int)g_folders.size() && item >= 0 && item < (int)g_folders[g_openFolderIndex].apps.size())
                app = g_folders[g_openFolderIndex].apps[item].get();
        } else if (!IsFolderItem(item)) app = ResolveApp(item);
        if (app && g_openFolderIndex < 0 && item < 1000 && g_selection.size() > 1 && g_selection.count(app)) {
            for (auto& a : g_apps) if (g_selection.count(a.get())) PinToTaskbar(a->target);
        } else if (app) PinToTaskbar(app->target);
        Render(); return;
    }

    if (g_openFolderIndex >= 0) {
        g_dragItem = -1;
        if (FolderSettled() && !InRect(g_layout.panel, g_dragX, g_dragY))
            MoveAppOutOfFolder(g_layoutItems[fromIdx]);
        return;
    }

    int toIdx = ItemAt((int)g_dragX, (int)g_dragY);
    int fromItem = g_layoutItems[fromIdx];
    g_dragItem = -1;
    if (toIdx < 0 || toIdx == fromIdx || fromItem >= 1000) return;
    int toItem = g_layoutItems[toIdx];
    if (IsFolderApp(toItem)) return; // resultado de busca dentro de pasta: não agrupa
    App* dragged = g_apps[fromItem].get();
    std::unordered_set<App*> moving;
    if (g_selection.count(dragged)) moving = g_selection; else moving.insert(dragged);

    if (toItem < 1000) {
        std::shared_ptr<App> target = g_apps[toItem];
        moving.erase(target.get());
        if (moving.empty()) return;
        auto taken = TakeApps(moving);
        g_apps.erase(std::find(g_apps.begin(), g_apps.end(), target));
        Folder f; f.name = f.sortKey = target->name;
        f.apps.push_back(target);
        f.apps.insert(f.apps.end(), taken.begin(), taken.end());
        g_folders.push_back(std::move(f));
    } else {
        auto taken = TakeApps(moving);
        auto& dst = g_folders[toItem - 1000].apps;
        dst.insert(dst.end(), taken.begin(), taken.end());
    }
    g_selection.clear();
    SaveFolders();
    RebuildLayout(); ComputeLayout(); Render();
}

////////////////////////////////////////////////////////////////////////////////
// System Hooks & Window Proc

bool OwnsTaskbar() {
    DWORD pid = 0; HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    return tray && GetWindowThreadProcessId(tray, &pid) && pid == GetCurrentProcessId();
}

using XamlLauncher_ShowStartView_t = HRESULT(WINAPI*)(void*, int, int);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original;

HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis, int showMethod, int showFlags) {
    bool enabled = (showMethod == 1 && (showFlags & 0x8)) ? g_winKeyEnabled : g_startButtonEnabled;
    HWND wnd = g_wnd; if (!enabled || !wnd) return XamlLauncher_ShowStartView_Original(pThis, showMethod, showFlags);
    PostMessageW(wnd, WM_APP_TOGGLE, 0, 0); return S_OK;
}

void OnAppsLoaded(AppList* list, int iconPixels) {
    g_menu.open = false; g_selection.clear();
    if (!list->empty()) { ApplyCustomIcons(*list, iconPixels); ReconcileFolders(*list); }
    g_apps = std::move(*list); delete list;
    g_appsLoaded = true; g_appsIconPixels = iconPixels;
    if (g_shown) {
        if (g_openFolderIndex >= 0) BuildMainItems(g_mainItems);
        RebuildLayout(); ComputeLayout(); Render();
    }
}

void UndoFolderAt(int index) {
    if (index < 0 || index >= (int)g_folders.size()) return;
    if (g_openFolderIndex >= 0) FinishFolderClose();
    UndoFolder(index);
    SaveFolders();
    g_hover = g_selected = -1;
    RebuildLayout(); ComputeLayout(); Render();
}

void CloseMenu() { if (g_menu.open && !g_menu.closing) { g_menu.closing = true; g_menu.hover = -1; StartFrames(); } }

// alignRight: o menu cresce para a esquerda a partir de x (usado com o botão à direita)
void OpenMenu(float x, float y, std::vector<MenuItem> items, bool alignRight = false) {
    if (items.empty()) return;
    const Layout& L = g_layout; float s = L.scale;
    g_menu.items = std::move(items); g_menu.itemH = std::round(32 * s); g_menu.sepH = std::round(9 * s);
    float w = 0, h = 12 * s;
    for (auto& it : g_menu.items) {
        h += it.separator ? g_menu.sepH : g_menu.itemH;
        if (it.separator || !g_menuFormat) continue;
        ComPtr<IDWriteTextLayout> tl;
        g_dwrite->CreateTextLayout(it.text.c_str(), (UINT32)it.text.size(), g_menuFormat.Get(), 1000, 40, &tl);
        if (tl) { DWRITE_TEXT_METRICS m{}; tl->GetMetrics(&m); w = std::max(w, m.widthIncludingTrailingWhitespace); }
    }
    w = std::round(std::max(190 * s, w + 44 * s));
    float m = 8 * s, left = alignRight ? x - w : x, top = y;
    if (!alignRight && left + w > L.width - m) left = x - w;
    if (alignRight && left < m) left = x;
    if (top + h > L.height - m) top = y - h;
    left = std::clamp(left, m, std::max(m, L.width - w - m)); top = std::clamp(top, m, std::max(m, L.height - h - m));
    g_menu.rect = D2D1::RectF(left, top, left + w, top + h); g_menu.origin = D2D1::Point2F(x, y);
    g_menu.open = true; g_menu.closing = false; g_menu.anim = 0; g_menu.hover = -1;
    StartFrames(); Render();
}

void MenuStep(int d) {
    int n = (int)g_menu.items.size(); if (!n) return;
    int i = g_menu.hover < 0 ? (d > 0 ? -1 : 0) : g_menu.hover;
    for (int k = 0; k < n; k++) { i = ((i + d) % n + n) % n; if (!g_menu.items[i].separator) { g_menu.hover = i; break; } }
    Render();
}

App* MenuApp() {
    if (g_menu.appIdx >= 0) return AppOfMain(g_menu.appIdx);
    if (g_openFolderIndex >= 0 && g_openFolderIndex < (int)g_folders.size() &&
        g_menu.folderApp >= 0 && g_menu.folderApp < (int)g_layoutItems.size()) {
        int i = g_layoutItems[g_menu.folderApp]; auto& v = g_folders[g_openFolderIndex].apps;
        if (i >= 0 && i < (int)v.size()) return v[i].get();
    }
    return nullptr;
}

void ExecuteMenu(int id) {
    int folder = g_menu.folder; App* app = MenuApp();
    CloseMenu();
    switch (id) {
        case kMenuRename:
            if (folder >= 0 && folder < (int)g_folders.size()) { if (g_openFolderIndex < 0) OpenFolder(folder); BeginRename(); }
            break;
        case kMenuUndoFolder: UndoFolderAt(folder); break;
        case kMenuSelect: if (app && !g_selection.erase(app)) g_selection.insert(app); Render(); break;
        case kMenuSelectAll:
            for (int it : g_layoutItems) if (it < 1000 && it < (int)g_apps.size()) g_selection.insert(g_apps[it].get());
            Render(); break;
        case kMenuClearSelection: g_selection.clear(); Render(); break;
        case kMenuNewFolder:
            if (app && !g_selection.count(app)) { g_selection.clear(); g_selection.insert(app); }
            CreateFolderFromSelection(); break;
        case kMenuPin:
            if (app && g_selection.count(app) && g_selection.size() > 1) { for (auto& a : g_apps) if (g_selection.count(a.get())) PinToTaskbar(a->target); }
            else if (app) PinToTaskbar(app->target);
            break;
        case kMenuChangeIcon: {
            std::wstring path;
            if (app && PickImage(g_wnd, path)) {
                if (LoadImageFile(path, CurrentIconPixels(), *app)) { g_customIcons[app->target] = path; SaveCustomIcons(); }
                else MessageBoxW(g_wnd, L"Could not load this image.", L"Launchpad", MB_ICONWARNING);
            }
            if (g_shown) { ForceForeground(g_wnd); Render(); }
            break;
        }
        case kMenuResetIcon:
            if (app) { g_customIcons.erase(app->target); SaveCustomIcons(); ResetAppIcon(*app); Render(); }
            break;
        case kMenuLock: case kMenuSleep: case kMenuRestart: case kMenuShutdown: case kMenuSignOut:
            HideNow();
            DoPowerAction(id);
            break;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_APP_TOGGLE:
            if ((!g_shown || g_closing) && Now() - g_lastClosed < 0.5) return 0;
            Toggle(); return 0;
        case WM_APP_APPS: OnAppsLoaded((AppList*)lParam, (int)wParam); return 0;
        case WM_APP_RELOAD: LoadSettings(); if (g_shown) HideNow(); return 0;
        case WM_APP_QUIT: DestroyWindow(hwnd); return 0;
        case WM_TIMER:
            if (wParam == kTimerFrame) OnFrame();
            else if (wParam == kTimerFirstLoad && OwnsTaskbar()) { KillTimer(hwnd, kTimerFirstLoad); if (!g_appsLoaded) StartLoadingApps(); }
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && g_shown && !g_closing && !g_dialogOpen) { g_lastClosed = Now(); Close(); }
            return 0;
        case WM_MOUSEMOVE: {
            if (!g_trackingMouse) { TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0}; g_trackingMouse = TrackMouseEvent(&tme); }
            if (g_menu.open && !g_menu.closing) {
                int h = MenuHit((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
                if (h != g_menu.hover) { g_menu.hover = h; Render(); }
                return 0;
            }
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            int dot = (FolderSettled() && !g_swiping && g_pressed < 0) ? DotAt((float)x, (float)y) : -1;
            if ((dot >= 0) != g_dotsHover || dot != g_dotHover) { g_dotsHover = dot >= 0; g_dotHover = dot; StartFrames(); }
            if (g_pressed >= 0) {
                if (!g_isDragging && (std::abs(x - (int)g_dragX) > 5 || std::abs(y - (int)g_dragY) > 5)) { g_isDragging = true; g_dragItem = g_pressed; }
                if (g_isDragging) {
                    g_dragX = (float)x; g_dragY = (float)y; Render();
                    float edge = 24 * g_layout.scale;
                    if (g_layout.horizontal && Now() - g_lastFlip > 0.7 && y >= g_layout.viewTop && y < g_layout.viewBottom &&
                        (x < edge || x > g_layout.width - edge)) { g_lastFlip = Now(); GoToPage(CurrentPage() + (x < edge ? -1 : 1)); }
                }
            } else if (g_pressedEmpty && g_layout.horizontal && GetCapture() == hwnd) {
                float dx = x - g_dragX;
                if (!g_swiping && std::abs(dx) > 6 * g_layout.scale) g_swiping = true;
                if (g_swiping) {
                    float v = g_swipeStartScroll - dx, m = MaxScroll();
                    if (v < 0) v *= 0.35f; else if (v > m) v = m + (v - m) * 0.35f;
                    g_scroll = g_scrollTarget = v; Render();
                }
            } else if (FolderSettled()) {
                bool ph = g_openFolderIndex < 0 && g_settings.showPower && InRect(g_layout.btnPower, (float)x, (float)y);
                if (ph != g_powerHover) { g_powerHover = ph; Render(); }
                int hover = ItemAt(x, y);
                if (hover != g_hover) { g_hover = hover; Render(); }
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            g_trackingMouse = false; g_dotsHover = false; g_dotHover = -1; StartFrames();
            if (g_powerHover) { g_powerHover = false; Render(); }
            if (g_menu.open && g_menu.hover >= 0) { g_menu.hover = -1; Render(); }
            if (g_hover >= 0) { g_hover = -1; Render(); } return 0;
        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (g_menu.open && !g_menu.closing) { if (!InRect(g_menu.rect, (float)x, (float)y)) CloseMenu(); return 0; }
            if (g_openFolderIndex < 0 && g_settings.showPower && InRect(g_layout.btnPower, (float)x, (float)y)) {
                g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
                bool right = g_settings.powerRight;
                OpenMenu(right ? g_layout.btnPower.right : g_layout.btnPower.left, g_layout.btnPower.bottom + 6 * g_layout.scale, {
                    {L"Lock", kMenuLock},
                    {L"Sleep", kMenuSleep},
                    {L"", 0, true},
                    {L"Restart", kMenuRestart},
                    {L"Shut down", kMenuShutdown, false, true},
                    {L"", 0, true},
                    {L"Sign out", kMenuSignOut},
                }, right);
                return 0;
            }
            if (g_openFolderIndex < 0 && (wParam & MK_CONTROL)) {
                if (App* a = SelectableApp(ItemAt(x, y))) { if (!g_selection.erase(a)) g_selection.insert(a); Render(); return 0; }
            }
            if (g_renaming) {
                if (InRect(TitleRect(g_layout), (float)x, (float)y)) return 0;
                CommitRename();
            }
            if (g_openFolderIndex >= 0) {
                if (InRect(g_layout.btnBack, (float)x, (float)y)) { CloseFolder(); return 0; }
                if (!FolderSettled()) return 0;
                if (InRect(TitleRect(g_layout), (float)x, (float)y)) { BeginRename(); return 0; }
            }
            if (FolderSettled()) { int dot = DotAt((float)x, (float)y); if (dot >= 0) { GoToPage(dot); return 0; } }
            SetCapture(hwnd); g_pressed = ItemAt(x, y); g_dragX = (float)x; g_dragY = (float)y;
            g_isDragging = false; g_swiping = false; g_swipeStartScroll = g_scrollTarget;
            g_pressedEmpty = g_pressed < 0 && !InRect(g_layout.search, (float)x, (float)y); Render(); return 0;
        }
        case WM_LBUTTONUP: {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (g_menu.open && !g_menu.closing) { int h = MenuHit((float)x, (float)y); if (h >= 0) ExecuteMenu(g_menu.items[h].id); return 0; }
            if (GetCapture() == hwnd) ReleaseCapture();
            if (g_swiping) {
                float dx = x - g_dragX, pw = g_layout.pageWidth;
                int base = (int)std::lround(g_swipeStartScroll / pw);
                g_swiping = false;
                GoToPage(std::abs(dx) > pw * 0.12f ? base + (dx < 0 ? 1 : -1) : base);
            } else if (g_pressed >= 0) {
                if (g_isDragging) { g_dragX = (float)x; g_dragY = (float)y; EndDrag(); }
                else if (ItemAt(x, y) == g_pressed) LaunchApp(g_layoutItems[g_pressed]);
            } else if (g_pressedEmpty && ItemAt(x, y) < 0) {
                if (g_openFolderIndex >= 0) {
                    if (!InRect(g_layout.panel, (float)x, (float)y)) CloseFolder();
                } else if (!InRect(g_layout.search, (float)x, (float)y)) { if (!g_selection.empty()) g_selection.clear(); else Close(); }
            }
            g_pressed = -1; g_dragItem = -1; g_isDragging = false; g_pressedEmpty = false; Render(); return 0;
        }
        case WM_RBUTTONUP: {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (!FolderSettled()) return 0;
            CommitRename();
            g_menu.open = false; g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
            std::vector<MenuItem> items;
            int n = (int)g_selection.size();
            std::wstring withN = L"New folder with " + std::to_wstring(n) + L" items";
            if (g_openFolderIndex >= 0) {
                if (!InRect(g_layout.panel, (float)x, (float)y) && !InRect(TitleRect(g_layout), (float)x, (float)y)) return 0;
                g_menu.folder = g_openFolderIndex;
            } else {
                int index = ItemAt(x, y);
                if (index >= 0 && IsFolderItem(g_layoutItems[index])) g_menu.folder = g_layoutItems[index] - 1000;
                else if (App* a = AppOfMain(index)) {
                    g_menu.appIdx = index;
                    if (SelectableApp(index)) {
                        bool sel = g_selection.count(a) != 0;
                        items.push_back({sel ? L"Deselect" : L"Select", kMenuSelect});
                        items.push_back({sel && n > 1 ? withN : std::wstring(L"New folder with this app"), kMenuNewFolder});
                        items.push_back({L"", 0, true});
                        items.push_back({sel && n > 1 ? L"Pin selected to taskbar" : L"Pin to taskbar", kMenuPin});
                    } else {
                        items.push_back({L"Pin to taskbar", kMenuPin});
                    }
                    items.push_back({L"", 0, true});
                    items.push_back({L"Change icon...", kMenuChangeIcon});
                    if (g_customIcons.count(a->target)) items.push_back({L"Reset icon", kMenuResetIcon});
                } else {
                    items.push_back({n ? withN : std::wstring(L"New folder"), kMenuNewFolder});
                    items.push_back({L"Select all", kMenuSelectAll});
                    if (n) items.push_back({L"Clear selection", kMenuClearSelection});
                }
            }
            if (g_menu.folder >= 0) {
                items.clear();
                if (g_openFolderIndex >= 0) {
                    int idx = ItemAt(x, y);
                    if (idx >= 0) {
                        g_menu.folderApp = idx;
                        if (App* fa = MenuApp()) {
                            items.push_back({L"Change icon...", kMenuChangeIcon});
                            if (g_customIcons.count(fa->target)) items.push_back({L"Reset icon", kMenuResetIcon});
                            items.push_back({L"", 0, true});
                        }
                    }
                }
                items.push_back({L"Rename", kMenuRename});
                items.push_back({L"", 0, true});
                items.push_back({L"Undo folder", kMenuUndoFolder, false, true});
            }
            OpenMenu((float)x, (float)y, std::move(items));
            return 0;
        }
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL: {
            if (g_menu.open) return 0;
            if (!FolderSettled()) return 0;
            bool h = msg == WM_MOUSEHWHEEL;
            if (h && !g_layout.horizontal) return 0;
            if (g_layout.horizontal && Now() - g_lastFlip < 0.28) { g_wheelDelta = 0; return 0; }
            g_wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
            if (std::abs(g_wheelDelta) >= WHEEL_DELTA) {
                int steps = g_wheelDelta / WHEEL_DELTA; g_wheelDelta %= WHEEL_DELTA;
                ScrollRows(h ? steps : -steps);
                if (g_layout.horizontal) g_lastFlip = Now();
            }
            return 0;
        }
        case WM_KEYDOWN: OnKeyDown(wParam); return 0;
        case WM_CHAR: OnChar((WCHAR)wParam); return 0;
        case WM_CLOSE: Close(); return 0;
        case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HANDLE g_thread;
HANDLE g_readyEvent;

DWORD WINAPI StartThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2); OleInitialize(nullptr); LoadSettings(); LoadFolders(); LoadCustomIcons();
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, g_d2d.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)g_dwrite.GetAddressOf());
    HINSTANCE instance = (HINSTANCE)&__ImageBase;

    WNDCLASSEXW wc{sizeof(wc)}; wc.lpfnWndProc = WndProc; wc.hInstance = instance; wc.hCursor = LoadCursorW(nullptr, IDC_ARROW); wc.lpszClassName = kWindowClass; RegisterClassExW(&wc);

    HWND wnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED, kWindowClass, L"Start", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance, nullptr);
    bool ok = g_d2d && g_dwrite && wnd;
    if (ok) { g_wnd = wnd; SetTimer(wnd, kTimerFirstLoad, 3000, nullptr); }
    SetEvent(g_readyEvent);
    if (ok) {
        MSG msg; while (GetMessageW(&msg, nullptr, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    } else { if (wnd) DestroyWindow(wnd); }
    g_wnd = nullptr; g_workerStop = true;
    if (g_worker) { WaitForSingleObject(g_worker, INFINITE); CloseHandle(g_worker); g_worker = nullptr; }
    MSG msg; while (PeekMessageW(&msg, nullptr, WM_APP_APPS, WM_APP_APPS, PM_REMOVE)) delete (AppList*)msg.lParam;
    g_selection.clear(); g_menu = PopupMenu{};
    AppList().swap(g_apps); g_folders.clear(); g_customIcons.clear(); g_mainItems.clear(); g_backdropImage = {}; FreeSurface(g_surface);
    g_labelFormat.Reset(); g_searchFormat.Reset(); g_messageFormat.Reset(); g_titleFormat.Reset(); g_menuFormat.Reset();
    ResetTarget(); g_dwrite.Reset(); g_d2d.Reset();
    UnregisterClassW(kWindowClass, instance); OleUninitialize(); return 0;
}

bool IsFolderWindowsProcess() {
    std::wstring cmd = ToLower(GetCommandLineW());
    return cmd.find(L"/factory") != std::wstring::npos || cmd.find(L"-embedding") != std::wstring::npos || cmd.find(L"/separate") != std::wstring::npos;
}

bool HookShowStartView() {
    HMODULE module = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;
    WindhawkUtils::SYMBOL_HOOK hooks[] = { { {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"}, &XamlLauncher_ShowStartView_Original, XamlLauncher_ShowStartView_Hook } };
    return WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

} // namespace

BOOL Wh_ModInit() {
    if (IsFolderWindowsProcess()) return FALSE;
    LoadSettings(); if (!HookShowStartView()) return FALSE;
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, StartThread, nullptr, 0, nullptr);
    if (!g_thread) { CloseHandle(g_readyEvent); g_readyEvent = nullptr; return FALSE; }
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_thread) return;
    WaitForSingleObject(g_readyEvent, INFINITE);
    if (HWND wnd = g_wnd) PostMessageW(wnd, WM_APP_QUIT, 0, 0);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread); g_thread = nullptr; CloseHandle(g_readyEvent); g_readyEvent = nullptr;
}

void Wh_ModSettingsChanged() { if (HWND wnd = g_wnd) PostMessageW(wnd, WM_APP_RELOAD, 0, 0); }
