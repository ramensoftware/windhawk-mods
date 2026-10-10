// ==WindhawkMod==
// @id                  windows-11-control-center-buttons
// @name                Windows 11 Control Center Buttons
// @description         Replaces the gear button in the Windows 11 Control Center footer with fully customizable buttons.
// @version             1.0.0
// @author              Salyts
// @github              https://github.com/Salyts
// @donateUrl           https://ko-fi.com/salyts
// @homepage            https://github.com/Salyts/Windows-11-Control-Center-Buttons
// @license             MIT
// @include             ShellHost.exe
// @include             explorer.exe
// @architecture        x86-64
// @compilerOptions     -lole32 -loleaut32 -lruntimeobject -lshlwapi -lshell32 -luuid -luser32 -lwtsapi32 -lpowrprof -lgdi32 -lgdiplus -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Control Center Buttons

• **[Report a bug or suggest a feature](https://github.com/Salyts/Windows-11-Control-Center-Buttons/issues)** · **[Discussion on Discord](https://discord.com/channels/923944342991818753/1558419938820227123)** · **[Support](https://ko-fi.com/salyts)**

Removes the stock **Settings (gear)** button from the bottom bar of the Windows 11
Control Center panel and puts your own buttons there instead.

`explorer.exe` is included only to launch actions: the panel host sends the click to a
tiny hidden window inside Explorer, which runs the command.

| Before | After |
|--------|---------|
| ![before](https://i.imgur.com/ILoCB2R.png) | ![after](https://i.imgur.com/R7ET89k.png) |

## Button fields

| Field | Description |
|---|---|
| **Preset** | Ready-made button. Set `Custom` to define your own. |
| **Name** | Tooltip. Empty on presets = default name. |
| **Icon** | Segoe Fluent glyph (`E713` or `\uE713`), image path (PNG/ICO/JPG/BMP/WEBP) or `.exe`/`.dll` path to take the icon from. |
| **Action** | Used for `Custom`. One or several commands, run in order. |
| **Left-click submenu** | Menu opened on left click (replaces Action). |
| **Right-click submenu** | Menu opened on right click. |

## Action formats

| Prefix | Example | Description |
|--------|---------|-------------|
| `" "` | `"C:\Program Files\Windhawk\windhawk.exe"` | Opens a file or folder by absolute path. |
| `~` | `~Downloads` or `~windhawk.exe` | Opens a folder or file by name. |
| `cmd:` | `cmd:control` | Runs a command through `cmd.exe`. |
| `shell:` | `shell:shutdown /r /f /t 0` | Runs through `powershell.exe`. |
| `press:` | `press:Win+E` or `press:0x5B;0x45` | Keyboard key press using [Win32 key code](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes). |
| `web:` | `web:https://windhawk.net/` | Opens a URL in the default browser. |
| `ms-settings:` | `ms-settings:bluetooth` | Opens a Windows Settings page. |

### Modifier signs (prepend to any action)
| Sign | Example | Description |
|------|---------|-------------|
| `-` | `-"C:\Program Files\app.exe"` or `-shell:shutdown /r /f /t 0` | Runs as administrator. |
| `*` | `*cmd:tasklist` or `*shell:Get-Process` | Execution with a terminal window. (only for `cmd:` and `shell:` prefixes). |

Signs can be combined: `-*cmd:tasklist` runs cmd in a visible window as admin.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- preset_language: en
  $name: Preset language
  $description: Language for the tooltip names of preset buttons.
  $options:
    - en: English
    - ru: Russian (Русский)

- alignment: right
  $name: Button alignment
  $description: Horizontal alignment of the custom button container within the Control Center footer.
  $options:
    - left: Left
    - center: Center
    - right: Right

- invert_buttons: true
  $name: Invert button order
  $description: Reverses the order in which custom buttons appear. Useful when alignment is set to Right.

- invert_icons_submenus: false
  $name: Invert icons in submenus
  $description: Reverses the order in which icons are displayed in submenus.

- hide_original_settings: true
  $name: Replace the original Settings button
  $description: Hides the stock gear button in the Control Center footer. Turn off to keep it and show the custom buttons next to it.

- close_flyout: true
  $name: Close Control Center after click
  $description: Automatically closes the Control Center panel after clicking any custom button (except buttons that open a submenu).

- button_spacing: 2
  $name: Spacing between buttons (px)
  $description: Horizontal margin between each button.

- container_margin_left: 8
  $name: Container left margin (px)
  $description: Left margin of the custom button container. Negative values pull it outward.

- container_margin_right: 8
  $name: Container right margin (px)
  $description: Right margin of the custom button container. Negative values pull it outward.

- buttons:
    - - Preset: menu_shutdown
        $name: Preset
        $description: "Choose a built-in preset button, or select Custom to define your own Name / Icon / Action."
        $options: 
          - custom: Custom
          - settings: Settings
          - explorer: Explorer
          - documents: Documents
          - downloads: Downloads
          - music: Music
          - pictures: Pictures
          - videos: Videos
          - network: Network
          - personal_folder: Personal Folder
          - shutdown: Shut down
          - restart: Restart
          - sign_out: Sign out
          - sleep: Sleep
          - hibernate: Hibernate
          - lock: Lock
          - menu_shutdown: Power Menu
      - Name: ""
        $name: Name
        $description: "Tooltip shown on hover. Leave empty on preset buttons to use the preset default."
      - Icon: ""
        $name: Icon
        $description: "Segoe Fluent glyph (e.g. E7E8 or \\uE7E8), image path (e.g. C:\\Icons\\app.png), or app path (e.g. C:\\Program Files\\app.exe)."
      - Action: [""]
        $name: Action
        $description: "Only used when Preset = Custom. See mod description for supported formats. If there is a Left-click submenu, this option does not work."
      - submenu:
          - - name: ""
              $name: Item name
            - icon: ""
              $name: Item icon
            - action: [""]
              $name: Item action
            - submenu:
              - - name: ""
                  $name: Item name
                - icon: ""
                  $name: Item icon
                - action: [""]
                  $name: Item action
                - submenu:
                  - - name: ""
                      $name: Item name
                    - icon: ""
                      $name: Item icon
                    - action: [""]
                      $name: Item action
                  $name: Level 3 submenu
              $name: Level 2 submenu
        $name: Left-click submenu
        $description: "Each item can have nested submenus up to 3 levels deep total."
      - rightClickSubmenu:
          - - name: ""
              $name: Item name
            - icon: ""
              $name: Item icon
            - action: [""]
              $name: Item action
            - submenu:
              - - name: ""
                  $name: Item name
                - icon: ""
                  $name: Item icon
                - action: [""]
                  $name: Item action
                - submenu:
                  - - name: ""
                      $name: Item name
                    - icon: ""
                      $name: Item icon
                    - action: [""]
                      $name: Item action
                  $name: Level 3 submenu
              $name: Level 2 submenu
        $name: Right-click submenu
        $description: "Each item can have nested submenus up to 3 levels deep total."
    - - Preset: settings
    - - Preset: explorer
  $name: Buttons
  $description: "Only used when Preset = Custom. See mod description for supported formats."
*/
// ==/WindhawkModSettings==
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <thread>
#include <condition_variable>
#include <map>
#include <optional>
#include <chrono>
#include <memory>
#include <string_view>

#include <windows.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <powrprof.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shcore.h>
#include <unordered_map>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

#ifdef _MSC_VER
#pragma comment(lib, "gdiplus.lib")
#endif
#include <gdiplus.h>

#include <windhawk_utils.h>

namespace wf   = winrt::Windows::Foundation;
namespace wu   = winrt::Windows::UI;
namespace wux  = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxcp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace wuxa = winrt::Windows::UI::Xaml::Automation;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxmi = winrt::Windows::UI::Xaml::Media::Imaging;
namespace wss  = winrt::Windows::Storage::Streams;

static const wchar_t* CONTAINER_TAG      = L"WH_QSFB_Container";
static const wchar_t* PROXY_WINDOW_CLASS = L"WH_QSFB_Proxy_Class";
static const wchar_t* PROXY_WINDOW_NAME  = L"WH_QSFB_Proxy_Window";
static constexpr ULONG_PTR kCopyDataMagic = 0x51534642;
static const wchar_t* FALLBACK_ICON      = L"\uE783";

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_closeFlyout{true};
static std::atomic<bool> g_invertIconsSubmenus{false};
static std::atomic<bool> g_stop{false};

static std::mutex g_settingsMutex;

static ULONG_PTR g_gdiplusToken = 0;
static std::mutex g_gdipMutex;

struct Settings {
    wux::HorizontalAlignment alignment            = wux::HorizontalAlignment::Right;
    bool                     invertButtons        = true;
    bool                     invertIconsSubmenus  = false;
    bool                     closeFlyout          = true;
    bool                     hideOriginalSettings = true;
    int                      buttonSpacing        = 0;
    int                      containerMarginLeft  = 0;
    int                      containerMarginRight = 0;
    bool                     langRussian          = false;
};

struct ActionItem {
    std::wstring name;
    std::wstring icon;
    std::vector<std::wstring> actionArray;
    std::vector<ActionItem> submenu;
    std::vector<ActionItem> rightClickSubmenu;
    int actionId = -1;
};

static Settings                g_settings;
static std::vector<ActionItem> g_buttons;
static std::vector<std::vector<std::wstring>> g_actionRegistry;

static bool g_isExplorer = false;

static HWND   g_proxyWindow   = NULL;
static HANDLE g_proxyThread   = NULL;
static DWORD  g_proxyThreadId = 0;
static HANDLE g_proxyReady    = NULL;

using WorkerClock = std::chrono::steady_clock;

[[clang::no_destroy]] static std::optional<std::thread> g_worker;
static std::mutex                                       g_queueMutex;
static std::condition_variable                          g_queueCv;
static std::multimap<WorkerClock::time_point, std::function<void()>> g_queue;

static void Enqueue(std::function<void()> fn, DWORD delayMs = 0) {
    {
        std::lock_guard<std::mutex> lk(g_queueMutex);
        if (g_stop.load()) return;
        g_queue.emplace(WorkerClock::now() + std::chrono::milliseconds(delayMs), std::move(fn));
    }
    g_queueCv.notify_one();
}

static void WorkerMain() {
    bool com = SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));

    std::unique_lock<std::mutex> lk(g_queueMutex);
    while (!g_stop.load()) {
        if (g_queue.empty()) {
            g_queueCv.wait(lk);
            continue;
        }
        auto first = g_queue.begin();
        if (first->first > WorkerClock::now()) {
            g_queueCv.wait_until(lk, first->first);
            continue;
        }
        std::function<void()> fn = std::move(first->second);
        g_queue.erase(first);
        lk.unlock();
        try { fn(); } catch (...) {}
        lk.lock();
    }
    g_queue.clear();
    lk.unlock();

    if (com) CoUninitialize();
}

static void StartWorker() {
    g_stop.store(false);
    if (!g_worker) g_worker.emplace(WorkerMain);
}

static void StopWorker() {
    {
        std::lock_guard<std::mutex> lk(g_queueMutex);
        g_stop.store(true);
    }
    g_queueCv.notify_all();
    if (g_worker) {
        g_worker->join();
        g_worker.reset();
    }
}

static std::wstring Trim(std::wstring s) {
    auto isSpace = [](wchar_t c) { return !!iswspace(c); };
    while (!s.empty() && isSpace(s.front())) s.erase(s.begin());
    while (!s.empty() && isSpace(s.back()))  s.pop_back();
    return s;
}

static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) c = static_cast<wchar_t>(towlower(c));
    return s;
}

static bool StartsWithCI(const std::wstring& s, const std::wstring& prefix) {
    return s.size() >= prefix.size() &&
           _wcsnicmp(s.c_str(), prefix.c_str(), prefix.size()) == 0;
}

static std::wstring StripOuterQuotes(const std::wstring& s) {
    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"')
        return s.substr(1, s.size() - 2);
    return s;
}

static void SafeFreeString(PCWSTR s) {
    if (s) Wh_FreeStringSetting(s);
}

static bool IsImagePath(const std::wstring& s) {
    if (s.size() < 4) return false;
    if (s.size() >= 2 && s[1] == L':')                    return true;
    if (s.size() >= 2 && s[0] == L'\\' && s[1] == L'\\') return true;
    std::wstring lo = ToLower(s);
    for (const wchar_t* ext : { L".png", L".ico", L".jpg", L".jpeg", L".bmp", L".webp" }) {
        size_t elen = wcslen(ext);
        if (lo.size() >= elen && lo.compare(lo.size() - elen, elen, ext) == 0)
            return true;
    }
    return false;
}

static bool IsExePath(const std::wstring& s) {
    if (s.size() < 5) return false;
    std::wstring lo = ToLower(s);
    for (const wchar_t* ext : { L".exe", L".dll" }) {
        size_t elen = wcslen(ext);
        if (lo.size() >= elen && lo.compare(lo.size() - elen, elen, ext) == 0)
            return true;
    }
    return false;
}

static std::wstring MakeFileUri(const std::wstring& path) {
    std::wstring result;
    result.reserve(path.size() + 8);
    result = L"file:///";
    for (wchar_t c : path) {
        if (c == L'\\') {
            result += L'/';
        } else if (c == L' ') {
            result += L"%20";
        } else {
            result += c;
        }
    }
    return result;
}

static std::wstring DecodeEscapes(const std::wstring& in) {
    std::wstring out;
    out.reserve(in.size());
    for (size_t i = 0; i < in.size(); ++i) {
        if (in[i] == L'\\' && i + 1 < in.size()) {
            wchar_t next = in[i + 1];
            if (next == L'u' && i + 5 < in.size()) {
                unsigned v = 0; bool ok = true;
                for (size_t j = i + 2; j < i + 6; ++j) {
                    wchar_t c = in[j]; v <<= 4;
                    if      (c >= L'0' && c <= L'9') v |= static_cast<unsigned>(c - L'0');
                    else if (c >= L'a' && c <= L'f') v |= static_cast<unsigned>(10 + c - L'a');
                    else if (c >= L'A' && c <= L'F') v |= static_cast<unsigned>(10 + c - L'A');
                    else { ok = false; break; }
                }
                if (ok) { out.push_back(static_cast<wchar_t>(v)); i += 5; continue; }
            }
            switch (next) {
                case L'\\': out.push_back(L'\\'); ++i; continue;
                case L'"':  out.push_back(L'"');  ++i; continue;
                case L'n':  out.push_back(L'\n'); ++i; continue;
                case L'r':  out.push_back(L'\r'); ++i; continue;
                case L't':  out.push_back(L'\t'); ++i; continue;
                default: break;
            }
        }
        out.push_back(in[i]);
    }
    return out;
}

static std::wstring NormalizeIconString(const std::wstring& raw) {
    std::wstring s = Trim(raw);
    if (s.empty()) return s;

    s = StripOuterQuotes(s);
    
    if (IsImagePath(s) || IsExePath(s)) return s;

    if (s.size() >= 2 && s[0] == L'/' && s[1] == L'u')
        return L"\\" + s.substr(1);

    if (s.size() == 4 && std::all_of(s.begin(), s.end(), [](wchar_t c) {
        return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F');
    }))
        return L"\\u" + s;

    if (s.size() == 6 && s[0] == L'\\' && s[1] == L'u' &&
        std::all_of(s.begin() + 2, s.end(), [](wchar_t c) {
            return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F');
        }))
        return s;

    return s;
}

static void EnsureGdiplus() {
    std::lock_guard<std::mutex> lk(g_gdipMutex);
    if (g_gdiplusToken == 0) {
        Gdiplus::GdiplusStartupInput input;
        Gdiplus::GdiplusStartup(&g_gdiplusToken, &input, nullptr);
    }
}

static HICON LoadIconFromExe(const std::wstring& path, int size) {
    HICON hLarge = nullptr, hSmall = nullptr;
    if (ExtractIconExW(path.c_str(), 0, &hLarge, &hSmall, 1) == 0)
        return nullptr;

    if (size > 16) {
        if (hSmall) DestroyIcon(hSmall);
        return hLarge;
    } else {
        if (hLarge) DestroyIcon(hLarge);
        return hSmall;
    }
}

static wuxmi::BitmapSource CreateBitmapSourceFromIcon(HICON hIcon) {
    if (!hIcon) return nullptr;

    ICONINFO info{};
    if (!GetIconInfo(hIcon, &info)) return nullptr;

    struct BitmapGuard {
        HBITMAP color, mask;
        ~BitmapGuard() {
            if (color) DeleteObject(color);
            if (mask)  DeleteObject(mask);
        }
    } bmpGuard{ info.hbmColor, info.hbmMask };

    BITMAP bm{};
    GetObject(info.hbmColor ? info.hbmColor : info.hbmMask, sizeof(bm), &bm);

    HDC screenDC = GetDC(nullptr);
    HDC memDC    = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = bm.bmWidth;
    bmi.bmiHeader.biHeight      = -bm.bmHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

    wuxmi::BitmapSource result = nullptr;
    if (dib && bits) {
        HBITMAP oldBmp = static_cast<HBITMAP>(SelectObject(memDC, dib));
        DrawIconEx(memDC, 0, 0, hIcon, bm.bmWidth, bm.bmHeight, 0, nullptr, DI_NORMAL);
        SelectObject(memDC, oldBmp);

        try {
            wss::InMemoryRandomAccessStream stream;

            EnsureGdiplus();

            Gdiplus::Bitmap srcBmp(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4,
                                   PixelFormat32bppARGB, static_cast<BYTE*>(bits));

            if (srcBmp.GetLastStatus() == Gdiplus::Ok) {
                IStream* pStream = nullptr;
                if (SUCCEEDED(CreateStreamOverRandomAccessStream(
                    winrt::get_unknown(stream), IID_PPV_ARGS(&pStream)))) {

                    CLSID pngClsid;
                    CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngClsid);

                    if (srcBmp.Save(pStream, &pngClsid, nullptr) == Gdiplus::Ok) {
                        stream.Seek(0);

                        wuxmi::BitmapImage bmpImage;
                        bmpImage.SetSource(stream);
                        result = bmpImage;
                    }
                    pStream->Release();
                }
            }
        } catch (...) {
            Wh_Log(L"Exception creating in-memory bitmap from icon");
        }

        DeleteObject(dib);
    }

    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
    return result;
}

static bool SaveIconToPng(HICON hIcon, const std::wstring& pngPath) {
    if (!hIcon) return false;

    std::wstring tmpPath = pngPath + L"." + std::to_wstring(GetCurrentProcessId()) + L".tmp";

    ICONINFO info{};
    if (!GetIconInfo(hIcon, &info)) return false;

    struct BitmapGuard {
        HBITMAP color, mask;
        ~BitmapGuard() {
            if (color) DeleteObject(color);
            if (mask)  DeleteObject(mask);
        }
    } bmpGuard{ info.hbmColor, info.hbmMask };

    BITMAP bm{};
    GetObject(info.hbmColor ? info.hbmColor : info.hbmMask, sizeof(bm), &bm);

    HDC screenDC = GetDC(nullptr);
    HDC memDC    = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = bm.bmWidth;
    bmi.bmiHeader.biHeight      = -bm.bmHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

    bool result = false;
    if (dib && bits) {
        HBITMAP oldBmp = static_cast<HBITMAP>(SelectObject(memDC, dib));
        DrawIconEx(memDC, 0, 0, hIcon, bm.bmWidth, bm.bmHeight, 0, nullptr, DI_NORMAL);
        SelectObject(memDC, oldBmp);

        EnsureGdiplus();

        Gdiplus::Bitmap srcBmp(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4,
                               PixelFormat32bppARGB, static_cast<BYTE*>(bits));
        if (srcBmp.GetLastStatus() == Gdiplus::Ok) {
            CLSID pngClsid;
            CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngClsid);
            if (srcBmp.Save(tmpPath.c_str(), &pngClsid, nullptr) == Gdiplus::Ok) {
                result = MoveFileExW(tmpPath.c_str(), pngPath.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
                if (!result && GetFileAttributesW(pngPath.c_str()) != INVALID_FILE_ATTRIBUTES)
                    result = true;
            }
            DeleteFileW(tmpPath.c_str());
        }
        DeleteObject(dib);
    }

    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);

    return result;
}

static wuxmi::BitmapSource ResolveExeIconInMemory(const std::wstring& iconStr, int size) {
    if (iconStr.empty() || !IsExePath(iconStr)) return nullptr;
    if (GetFileAttributesW(iconStr.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Executable not found for icon: %s", iconStr.c_str());
        return nullptr;
    }
    HICON hIcon = LoadIconFromExe(iconStr, size);
    if (!hIcon) return nullptr;
    auto bmpSource = CreateBitmapSourceFromIcon(hIcon);
    DestroyIcon(hIcon);
    return bmpSource;
}

static std::wstring GetCachedIconPng(const std::wstring& exePath, int size) {
    WIN32_FILE_ATTRIBUTE_DATA fad{};
    if (!GetFileAttributesExW(exePath.c_str(), GetFileExInfoStandard, &fad)) return L"";

    WCHAR dir[MAX_PATH]{};
    if (!Wh_GetModStoragePath(dir, ARRAYSIZE(dir))) return L"";
    CreateDirectoryW(dir, nullptr);

    unsigned long long h = 1469598103934665603ULL;
    auto mix = [&h](const void* p, size_t n) {
        auto b = static_cast<const unsigned char*>(p);
        for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ULL; }
    };
    std::wstring key = ToLower(exePath);
    mix(key.data(), key.size() * sizeof(wchar_t));
    mix(&fad.ftLastWriteTime, sizeof(FILETIME));
    mix(&size, sizeof(size));

    wchar_t name[40];
    swprintf_s(name, L"icon_%016llx.png", h);
    std::wstring png = std::wstring(dir) + L"\\" + name;

    if (GetFileAttributesW(png.c_str()) != INVALID_FILE_ATTRIBUTES) return png;

    HICON hIcon = LoadIconFromExe(exePath, size);
    if (!hIcon) return L"";
    bool ok = SaveIconToPng(hIcon, png);
    DestroyIcon(hIcon);
    return ok ? png : L"";
}

static std::wstring ResolveExeIcon(const std::wstring& iconStr, int size) {
    if (iconStr.empty() || !IsExePath(iconStr)) return iconStr;
    if (GetFileAttributesW(iconStr.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Executable not found for icon: %s", iconStr.c_str());
        return L"";
    }
    return GetCachedIconPng(iconStr, size);
}

static std::wstring GlyphOrEmpty(const std::wstring& s) {
    return (!s.empty() && !IsImagePath(s) && !IsExePath(s)) ? s : std::wstring{};
}

static wux::UIElement MakeButtonIcon(const std::wstring& iconStr) {
    if (IsExePath(iconStr)) {
        auto bmpSource = ResolveExeIconInMemory(iconStr, 32);
        if (bmpSource) {
            try {
                wuxc::Image img;
                img.Width(16); img.Height(16);
                img.Stretch(wuxm::Stretch::Uniform);
                img.Source(bmpSource);
                return img;
            } catch (...) {
                Wh_Log(L"Exception creating button icon from in-memory bitmap: %s", iconStr.c_str());
            }
        }
    }

    std::wstring resolved = ResolveExeIcon(iconStr, 32);

    if (!resolved.empty() && IsImagePath(resolved)) {
        if (GetFileAttributesW(resolved.c_str()) != INVALID_FILE_ATTRIBUTES) {
            try {
                wuxmi::BitmapImage bmp;
                bmp.DecodePixelWidth(32);
                bmp.DecodePixelHeight(32);
                bmp.UriSource(winrt::Windows::Foundation::Uri(MakeFileUri(resolved)));
                wuxc::Image img;
                img.Width(16); img.Height(16);
                img.Stretch(wuxm::Stretch::Uniform);
                img.Source(bmp);
                return img;
            } catch (...) {
                Wh_Log(L"Exception creating button image icon: %s", resolved.c_str());
            }
        } else {
            Wh_Log(L"Icon file not found: %s", resolved.c_str());
        }
    }

    wuxc::FontIcon fi;
    fi.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    fi.FontSize(16);
    std::wstring glyph = GlyphOrEmpty(resolved.empty() ? iconStr : resolved);
    fi.Glyph(!glyph.empty() ? glyph : FALLBACK_ICON);
    return fi;
}

static wuxc::IconElement MakeMenuIcon(const std::wstring& iconStr) {
    if (IsImagePath(iconStr) && !IsExePath(iconStr)) {
        if (GetFileAttributesW(iconStr.c_str()) != INVALID_FILE_ATTRIBUTES) {
            try {
                wuxc::BitmapIcon bi;
                bi.UriSource(winrt::Windows::Foundation::Uri(MakeFileUri(iconStr)));
                bi.ShowAsMonochrome(false);
                return bi;
            } catch (...) {
                Wh_Log(L"Exception creating menu BitmapIcon: %s", iconStr.c_str());
            }
        } else {
            Wh_Log(L"Menu icon file not found: %s", iconStr.c_str());
        }
    }

    if (IsExePath(iconStr)) {
        if (GetFileAttributesW(iconStr.c_str()) != INVALID_FILE_ATTRIBUTES) {
            std::wstring pngPath = GetCachedIconPng(iconStr, 16);

            if (!pngPath.empty()) {
                try {
                    wuxc::BitmapIcon bi;
                    bi.UriSource(winrt::Windows::Foundation::Uri(MakeFileUri(pngPath)));
                    bi.ShowAsMonochrome(false);
                    return bi;
                } catch (...) {
                    Wh_Log(L"Exception creating menu icon from exe: %s", iconStr.c_str());
                }
            }
        }
    }

    wuxc::FontIcon fi;
    fi.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    fi.FontSize(16);
    std::wstring glyph = GlyphOrEmpty(iconStr);
    fi.Glyph(!glyph.empty() ? glyph : FALLBACK_ICON);
    return fi;
}


struct PresetDef {
    const wchar_t* key;
    const wchar_t* nameEn;
    const wchar_t* nameRu;
    const wchar_t* icon;
    const wchar_t* action;
};

static const PresetDef kPresets[] = {
    { L"settings",        L"Settings",        L"Параметры",         L"\uE713", L"__preset:settings"        },
    { L"explorer",        L"Explorer",        L"Проводник",         L"\uEC50", L"__preset:explorer"        },
    { L"documents",       L"Documents",       L"Документы",         L"\uE8A5", L"__preset:documents"       },
    { L"downloads",       L"Downloads",       L"Загрузки",          L"\uE896", L"__preset:downloads"       },
    { L"music",           L"Music",           L"Музыка",            L"\uEC4F", L"__preset:music"           },
    { L"pictures",        L"Pictures",        L"Изображения",       L"\uE91B", L"__preset:pictures"        },
    { L"videos",          L"Videos",          L"Видео",             L"\uE714", L"__preset:videos"          },
    { L"network",         L"Network",         L"Сеть",              L"\uEC27", L"__preset:network"         },
    { L"personal_folder", L"Personal Folder", L"Личная папка",      L"\uEC25", L"__preset:personal_folder" },
    { L"shutdown",        L"Shut down",       L"Завершение работы", L"\uE7E8", L"__preset:shutdown"        },
    { L"restart",         L"Restart",         L"Перезагрузка",      L"\uE777", L"__preset:restart"         },
    { L"sign_out",        L"Sign out",        L"Выйти",             L"\uF3B1", L"__preset:sign_out"        },
    { L"sleep",           L"Sleep",           L"Спящий режим",      L"\uE708", L"__preset:sleep"           },
    { L"hibernate",       L"Hibernate",       L"Гибернация",        L"\uE823", L"__preset:hibernate"       },
    { L"lock",            L"Lock",            L"Блокировка",        L"\uE72E", L"__preset:lock"            },
    { L"menu_shutdown",   L"Power",           L"Выключение",        L"\uE7E8", nullptr                     },
};
static constexpr int kPresetCount = static_cast<int>(std::size(kPresets));

static const PresetDef* FindPreset(const std::wstring& key) {
    std::wstring k = ToLower(Trim(key));
    if (k.empty() || k == L"custom") return nullptr;
    for (int i = 0; i < kPresetCount; ++i)
        if (k == kPresets[i].key) return &kPresets[i];
    Wh_Log(L"Unknown preset key: '%s'", key.c_str());
    return nullptr;
}

static std::wstring PresetName(const PresetDef& pd, bool russian) {
    return russian ? pd.nameRu : pd.nameEn;
}

static bool GetKnownFolderPath(const wchar_t* id, std::wstring& out) {
    std::wstring n = ToLower(id);
    KNOWNFOLDERID fid{};
    if      (n == L"downloads")                     fid = FOLDERID_Downloads;
    else if (n == L"documents" || n == L"personal") fid = FOLDERID_Documents;
    else if (n == L"music")                         fid = FOLDERID_Music;
    else if (n == L"pictures")                      fid = FOLDERID_Pictures;
    else if (n == L"videos")                        fid = FOLDERID_Videos;
    else if (n == L"desktop")                       fid = FOLDERID_Desktop;
    else if (n == L"profile" || n == L"home")       fid = FOLDERID_Profile;
    else return false;

    PWSTR raw = nullptr;
    bool ok = SUCCEEDED(SHGetKnownFolderPath(fid, 0, nullptr, &raw)) && raw;
    if (ok) out = raw;
    CoTaskMemFree(raw);
    return ok;
}

static void OpenKnownFolder(const wchar_t* name) {
    std::wstring path;
    if (GetKnownFolderPath(name, path))
        ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static bool EnableShutdownPrivilege() {
    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;
    TOKEN_PRIVILEGES tkp{};
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    bool ok = LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME,
                                    &tkp.Privileges[0].Luid) &&
              AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, nullptr, nullptr);
    CloseHandle(hToken);
    return ok;
}

static bool PerformPresetAction(const std::wstring& action) {
    if (!StartsWithCI(action, L"__preset:")) return false;
    std::wstring key = ToLower(action.substr(9));

    if      (key == L"settings")        ShellExecuteW(nullptr, L"open", L"ms-settings:", nullptr, nullptr, SW_SHOWNORMAL);
    else if (key == L"explorer")        ShellExecuteW(nullptr, L"open", L"explorer.exe", nullptr, nullptr, SW_SHOWNORMAL);
    else if (key == L"documents")       OpenKnownFolder(L"documents");
    else if (key == L"downloads")       OpenKnownFolder(L"downloads");
    else if (key == L"music")           OpenKnownFolder(L"music");
    else if (key == L"pictures")        OpenKnownFolder(L"pictures");
    else if (key == L"videos")          OpenKnownFolder(L"videos");
    else if (key == L"network")         ShellExecuteW(nullptr, L"open", L"shell:NetworkPlacesFolder", nullptr, nullptr, SW_SHOWNORMAL);
    else if (key == L"personal_folder") OpenKnownFolder(L"profile");
    else if (key == L"shutdown")        { EnableShutdownPrivilege(); ExitWindowsEx(EWX_SHUTDOWN, SHTDN_REASON_MAJOR_OTHER); }
    else if (key == L"restart")         { EnableShutdownPrivilege(); ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_OTHER); }
    else if (key == L"sign_out")        { EnableShutdownPrivilege(); ExitWindowsEx(EWX_LOGOFF, 0); }
    else if (key == L"sleep")           SetSuspendState(FALSE, FALSE, FALSE);
    else if (key == L"hibernate")       SetSuspendState(TRUE,  FALSE, FALSE);
    else if (key == L"lock")            LockWorkStation();
    else return false;

    return true;
}

static bool SearchRecursive(const std::wstring& root, const std::wstring& name,
                             int depth, std::wstring& out) {
    if (depth < 0) return false;
    std::wstring mask = root;
    if (!mask.empty() && mask.back() != L'\\') mask += L'\\';
    mask += L'*';

    WIN32_FIND_DATAW fd{};
    HANDLE h = FindFirstFileW(mask.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return false;

    bool found = false;
    do {
        if (g_stop.load()) break;
        if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L"..")) continue;

        std::wstring full = root;
        if (!full.empty() && full.back() != L'\\') full += L'\\';
        full += fd.cFileName;

        if (!_wcsicmp(fd.cFileName, name.c_str())) {
            out = full; found = true; break;
        }
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
            if (SearchRecursive(full, name, depth - 1, out)) { found = true; break; }
        }
    } while (FindNextFileW(h, &fd));

    FindClose(h);
    return found;
}

static bool SearchByName(const std::wstring& name, std::wstring& out) {
    wchar_t buf[MAX_PATH * 4]{};
    if (SearchPathW(nullptr, name.c_str(), nullptr, ARRAYSIZE(buf), buf, nullptr)) {
        out = buf; return true;
    }

    std::vector<std::wstring> roots;
    auto addEnvDir = [&](const wchar_t* var) {
        wchar_t tmp[MAX_PATH * 2]{};
        if (GetEnvironmentVariableW(var, tmp, ARRAYSIZE(tmp))) roots.emplace_back(tmp);
    };
    addEnvDir(L"ProgramFiles"); addEnvDir(L"ProgramFiles(x86)"); addEnvDir(L"ProgramW6432");
    addEnvDir(L"SystemRoot");   addEnvDir(L"LOCALAPPDATA");       addEnvDir(L"APPDATA");
    addEnvDir(L"USERPROFILE");
    for (const wchar_t* folder : { L"desktop", L"documents", L"downloads",
                                    L"music",   L"pictures",  L"videos" }) {
        std::wstring p;
        if (GetKnownFolderPath(folder, p)) roots.push_back(std::move(p));
    }

    for (const auto& r : roots) {
        if (g_stop.load()) return false;
        if (SearchRecursive(r, name, 5, out)) return true;
    }
    return false;
}

static WORD ResolveKeyToken(const std::wstring& tok) {
    if (tok.empty()) return 0;
    std::wstring lo = ToLower(tok);

    struct { const wchar_t* name; WORD vk; } named[] = {
        { L"ctrl",       VK_CONTROL  }, { L"alt",        VK_MENU    },
        { L"shift",      VK_SHIFT    }, { L"win",        VK_LWIN    },
        { L"enter",      VK_RETURN   }, { L"esc",        VK_ESCAPE  },
        { L"escape",     VK_ESCAPE   }, { L"tab",        VK_TAB     },
        { L"space",      VK_SPACE    }, { L"backspace",  VK_BACK    },
        { L"delete",     VK_DELETE   }, { L"del",        VK_DELETE  },
        { L"insert",     VK_INSERT   }, { L"ins",        VK_INSERT  },
        { L"home",       VK_HOME     }, { L"end",        VK_END     },
        { L"pageup",     VK_PRIOR    }, { L"pgup",       VK_PRIOR   },
        { L"pagedown",   VK_NEXT     }, { L"pgdn",       VK_NEXT    },
        { L"left",       VK_LEFT     }, { L"right",      VK_RIGHT   },
        { L"up",         VK_UP       }, { L"down",       VK_DOWN    },
        { L"printscreen",VK_SNAPSHOT }, { L"pause",      VK_PAUSE   },
        { L"capslock",   VK_CAPITAL  }, { L"numlock",    VK_NUMLOCK },
        { L"scrolllock", VK_SCROLL   }, { L"sleep",      VK_SLEEP   },
    };
    for (auto& e : named)
        if (lo == e.name) return e.vk;

    if (lo.size() >= 2 && lo[0] == L'f') {
        wchar_t* end = nullptr;
        long n = wcstol(lo.c_str() + 1, &end, 10);
        if (end && *end == L'\0' && n >= 1 && n <= 24)
            return static_cast<WORD>(VK_F1 + n - 1);
    }

    if (tok.size() >= 2 && tok[0] == L'0' && (tok[1] == L'x' || tok[1] == L'X')) {
        long v = wcstol(tok.c_str(), nullptr, 16);
        if (v > 0 && v <= 0xFF) return static_cast<WORD>(v);
        Wh_Log(L"press: hex VK out of range: %s", tok.c_str());
        return 0;
    }

    if (iswdigit(tok[0])) {
        long v = wcstol(tok.c_str(), nullptr, 10);
        if (v > 0 && v <= 0xFF) return static_cast<WORD>(v);
        Wh_Log(L"press: decimal VK out of range: %s", tok.c_str());
        return 0;
    }

    if (tok.size() == 1) {
        wchar_t c = towupper(tok[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9'))
            return static_cast<WORD>(c);
    }

    Wh_Log(L"press: unrecognised token '%s'", tok.c_str());
    return 0;
}

static void SendVirtualKeypress(const std::wstring& keysStr) {
    std::vector<WORD> keys;
    std::wstring token;

    auto flush = [&]() {
        token = Trim(token);
        if (!token.empty()) {
            WORD vk = ResolveKeyToken(token);
            if (vk) keys.push_back(vk);
            token.clear();
        }
    };

    for (size_t i = 0; i < keysStr.size(); ++i) {
        wchar_t c = keysStr[i];
        if (c == L'+' || c == L';') {
            flush();
        } else {
            token += c;
        }
    }
    flush();

    if (keys.empty()) {
        Wh_Log(L"press: no valid codes in '%s'", keysStr.c_str());
        return;
    }

    for (WORD vk : keys)      { keybd_event(static_cast<BYTE>(vk), 0, 0, 0); Sleep(10); }
    Sleep(100);
    for (size_t i = keys.size(); i > 0; --i)
        { keybd_event(static_cast<BYTE>(keys[i - 1]), 0, KEYEVENTF_KEYUP, 0); Sleep(10); }
}

struct ParsedAction {
    std::wstring body;
    bool         runas;
    bool         showWindow;
};

static ParsedAction ParseActionSigns(const std::wstring& raw) {
    ParsedAction r{ raw, false, false };
    size_t i = 0;
    while (i < raw.size()) {
        if      (raw[i] == L'-') { r.runas      = true; ++i; }
        else if (raw[i] == L'*') { r.showWindow = true; ++i; }
        else break;
    }
    r.body = raw.substr(i);
    return r;
}

static bool ExecuteProcess(const std::wstring& cmd, bool useCmdExe, bool showWindow) {
    std::wstring line = useCmdExe ? (L"cmd.exe /C " + cmd) : cmd;
    std::vector<wchar_t> buf(line.begin(), line.end());
    buf.push_back(L'\0');

    STARTUPINFOW si{}; si.cb = sizeof(si);
    si.dwFlags     = STARTF_USESHOWWINDOW;
    si.wShowWindow = showWindow ? SW_NORMAL : SW_HIDE;

    PROCESS_INFORMATION pi{};
    DWORD flags = showWindow ? 0 : CREATE_NO_WINDOW;
    if (!CreateProcessW(nullptr, buf.data(), nullptr, nullptr,
                        FALSE, flags, nullptr, nullptr, &si, &pi))
        return false;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

static void ExecuteSingleCommand(const std::wstring& cmd) {
    auto [action, runas, showWindow] = ParseActionSigns(cmd);
    const LPCWSTR verb = runas ? L"runas" : L"open";

    if (StartsWithCI(action, L"press:")) {
        Sleep(150);
        SendVirtualKeypress(action.substr(6));
        return;
    }

    if (StartsWithCI(action, L"cmd:")) {
        std::wstring command = Trim(action.substr(4));
        if (showWindow)
            ShellExecuteW(nullptr, verb, L"cmd.exe",
                          (L"/K " + command).c_str(), nullptr, SW_NORMAL);
        else
            ShellExecuteW(nullptr, verb, L"cmd.exe",
                          (L"/C " + command).c_str(), nullptr, SW_HIDE);
        return;
    }

    if (StartsWithCI(action, L"shell:")) {
        std::wstring ps = Trim(action.substr(6));
        std::wstring args = L"-NoProfile -ExecutionPolicy Bypass -Command " + ps;
        if (showWindow) args = L"-NoExit " + args;
        ShellExecuteW(nullptr, verb, L"powershell.exe",
                      args.c_str(), nullptr, showWindow ? SW_NORMAL : SW_HIDE);
        return;
    }

    if (StartsWithCI(action, L"ms-settings:")) {
        ShellExecuteW(nullptr, verb, action.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }

    if (StartsWithCI(action, L"web:")) {
        ShellExecuteW(nullptr, verb, Trim(action.substr(4)).c_str(),
                      nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }

    if (!action.empty() && action.front() == L'"') {
        ShellExecuteW(nullptr, verb, StripOuterQuotes(action).c_str(),
                      nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }

    if (!action.empty() && action.front() == L'~') {
        std::wstring tgt = Trim(action.substr(1));
        if (tgt.empty()) { Wh_Log(L"~search: empty target"); return; }
        std::wstring resolved;
        if (GetKnownFolderPath(tgt.c_str(), resolved) ||
            SearchByName(tgt, resolved))
            ShellExecuteW(nullptr, verb, resolved.c_str(),
                          nullptr, nullptr, SW_SHOWNORMAL);
        else
            Wh_Log(L"~search: '%s' not found", tgt.c_str());
        return;
    }

    ExecuteProcess(action, false, showWindow);
}

static void ExecuteActionText(const std::vector<std::wstring>& commands) {
    std::vector<std::wstring> cmds;
    cmds.reserve(commands.size());
    for (const auto& c : commands) {
        std::wstring t = Trim(c);
        if (!t.empty()) cmds.push_back(std::move(t));
    }

    for (size_t i = 0; i < cmds.size() && !g_stop.load(); ++i) {
        if (!PerformPresetAction(cmds[i]))
            ExecuteSingleCommand(cmds[i]);
        if (i + 1 < cmds.size())
            Sleep(50);
    }
}

static void SendEscapeKey() {
    INPUT in[2]{};
    in[0].type = in[1].type = INPUT_KEYBOARD;
    in[0].ki.wVk = in[1].ki.wVk = VK_ESCAPE;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

static void ScheduleEscapeKeypress(DWORD delayMs = 100) {
    Enqueue([] { SendEscapeKey(); }, delayMs);
}

static void CloseFlyoutAfterClick() {
    if (g_closeFlyout.load(std::memory_order_relaxed))
        ScheduleEscapeKeypress(100);
}

static void CloseFlyoutForPress() {
    ScheduleEscapeKeypress(100);
}

static void RegisterActions(std::vector<ActionItem>& items,
                            std::vector<std::vector<std::wstring>>& reg) {
    for (auto& it : items) {
        if (!it.actionArray.empty()) {
            it.actionId = static_cast<int>(reg.size());
            reg.push_back(it.actionArray);
        }
        RegisterActions(it.submenu, reg);
        RegisterActions(it.rightClickSubmenu, reg);
    }
}

static void ExecuteRegistered(int id) {
    std::vector<std::wstring> acts;
    {
        std::lock_guard<std::mutex> lk(g_settingsMutex);
        if (id >= 0 && id < static_cast<int>(g_actionRegistry.size()))
            acts = g_actionRegistry[id];
    }
    if (!acts.empty()) ExecuteActionText(acts);
}

static void RunRegisteredAsync(int id) {
    Enqueue([id] { ExecuteRegistered(id); });
}

static LRESULT CALLBACK ProxyWndProc(HWND hWnd, UINT msg,
                                     WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COPYDATA) {
        const auto* cds = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
        if (cds && cds->dwData == kCopyDataMagic && cds->cbData == sizeof(int) &&
            cds->lpData && !g_unloading) {
            RunRegisteredAsync(*static_cast<const int*>(cds->lpData));
            return TRUE;
        }
        return FALSE;
    }
    switch (msg) {
        case WM_CLOSE:   DestroyWindow(hWnd); return 0;
        case WM_DESTROY: g_proxyWindow = NULL; PostQuitMessage(0); return 0;
        default: return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}

static HINSTANCE GetModHandle() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&GetModHandle), &module);
    return module;
}

static DWORD WINAPI ProxyWindowThread(LPVOID) {
    HINSTANCE inst = GetModHandle();

    WNDCLASSEXW wcex{};
    wcex.cbSize        = sizeof(wcex);
    wcex.lpfnWndProc   = ProxyWndProc;
    wcex.hInstance     = inst;
    wcex.lpszClassName = PROXY_WINDOW_CLASS;

    if (!RegisterClassExW(&wcex)) {
        Wh_Log(L"Proxy: RegisterClassEx failed %lu", GetLastError());
        SetEvent(g_proxyReady);
        return 1;
    }

    g_proxyWindow = CreateWindowExW(
        0, PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME, 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr, inst, nullptr);
    if (!g_proxyWindow) {
        Wh_Log(L"Proxy: CreateWindowEx failed %lu", GetLastError());
        UnregisterClassW(PROXY_WINDOW_CLASS, inst);
        SetEvent(g_proxyReady);
        return 1;
    }

    ChangeWindowMessageFilterEx(g_proxyWindow, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
    SetEvent(g_proxyReady);

    MSG m{};
    while (GetMessageW(&m, nullptr, 0, 0)) {
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }

    UnregisterClassW(PROXY_WINDOW_CLASS, inst);
    g_proxyWindow = NULL;
    return 0;
}

static void StartProxyThread() {
    if (g_proxyThread) return;

    g_proxyReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_proxyReady) return;

    g_proxyThread = CreateThread(nullptr, 0, ProxyWindowThread, nullptr, 0, &g_proxyThreadId);
    if (!g_proxyThread) {
        Wh_Log(L"Proxy: CreateThread failed %lu", GetLastError());
        CloseHandle(g_proxyReady);
        g_proxyReady = NULL;
        return;
    }

    HANDLE waitFor[] = { g_proxyReady, g_proxyThread };
    WaitForMultipleObjects(ARRAYSIZE(waitFor), waitFor, FALSE, INFINITE);
}

static void StopProxyThread() {
    if (g_proxyWindow)
        PostMessageW(g_proxyWindow, WM_CLOSE, 0, 0);
    if (g_proxyThread) {
        WaitForSingleObject(g_proxyThread, INFINITE);
        CloseHandle(g_proxyThread);
        g_proxyThread = NULL;
    }
    if (g_proxyReady) {
        CloseHandle(g_proxyReady);
        g_proxyReady = NULL;
    }
}

static void SendActionToProxy(int id) {
    if (id < 0) return;
    HWND hw = FindWindowExW(HWND_MESSAGE, nullptr, PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME);
    if (hw) {
        COPYDATASTRUCT cds{ kCopyDataMagic, sizeof(int), &id };
        DWORD_PTR res = 0;
        if (SendMessageTimeoutW(hw, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&cds),
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &res) && res)
            return;
        Wh_Log(L"Proxy did not accept action %d; running locally", id);
    }
    RunRegisteredAsync(id);
}

static void LoadSettings() {
    Settings s;

    PCWSTR lang = Wh_GetStringSetting(L"preset_language");
    s.langRussian = lang && _wcsicmp(lang, L"ru") == 0;
    SafeFreeString(lang);

    PCWSTR align = Wh_GetStringSetting(L"alignment");
    if      (align && _wcsicmp(align, L"left")   == 0) s.alignment = wux::HorizontalAlignment::Left;
    else if (align && _wcsicmp(align, L"center") == 0) s.alignment = wux::HorizontalAlignment::Center;
    else                                                s.alignment = wux::HorizontalAlignment::Right;
    SafeFreeString(align);

    s.invertButtons          = Wh_GetIntSetting(L"invert_buttons")          != 0;
    s.invertIconsSubmenus    = Wh_GetIntSetting(L"invert_icons_submenus")   != 0;
    s.closeFlyout            = Wh_GetIntSetting(L"close_flyout")            != 0;
    s.hideOriginalSettings   = Wh_GetIntSetting(L"hide_original_settings")  != 0;
    s.buttonSpacing          = Wh_GetIntSetting(L"button_spacing");
    s.containerMarginLeft    = Wh_GetIntSetting(L"container_margin_left");
    s.containerMarginRight   = Wh_GetIntSetting(L"container_margin_right");

    g_closeFlyout.store(s.closeFlyout, std::memory_order_relaxed);
    g_invertIconsSubmenus.store(s.invertIconsSubmenus, std::memory_order_relaxed);

    std::lock_guard<std::mutex> lk(g_settingsMutex);
    g_settings = s;
}

static std::vector<ActionItem> LoadSubmenuRecursive(const std::wstring& basePath, int depth = 0) {
    std::vector<ActionItem> items;
    if (depth >= 3) return items;

    for (int j = 0; j < 64; ++j) {
        std::wstring namePath   = basePath + L"[" + std::to_wstring(j) + L"].name";
        std::wstring iconPath   = basePath + L"[" + std::to_wstring(j) + L"].icon";
        std::wstring actionPath = basePath + L"[" + std::to_wstring(j) + L"].action";

        PCWSTR n  = Wh_GetStringSetting(namePath.c_str());
        PCWSTR ic = Wh_GetStringSetting(iconPath.c_str());

        std::wstring name   = n ? n : L"";
        std::wstring icon   = DecodeEscapes(NormalizeIconString(ic ? ic : L""));

        SafeFreeString(n); SafeFreeString(ic);

        if (Trim(name).empty()) break;

        std::vector<std::wstring> actionArray;
        for (int k = 0; k < 64; ++k) {
            std::wstring cmdPath = actionPath + L"[" + std::to_wstring(k) + L"]";
            PCWSTR cmd = Wh_GetStringSetting(cmdPath.c_str());
            if (!cmd || wcslen(cmd) == 0) {
                SafeFreeString(cmd);
                break;
            }
            std::wstring cmdStr = cmd;
            SafeFreeString(cmd);
            cmdStr = Trim(cmdStr);
            if (cmdStr.empty()) break;

            actionArray.push_back(std::move(cmdStr));
        }

        ActionItem item;
        item.name        = Trim(name);
        item.icon        = Trim(icon);
        item.actionArray = std::move(actionArray);

        std::wstring submenuPath = basePath + L"[" + std::to_wstring(j) + L"].submenu";
        item.submenu = LoadSubmenuRecursive(submenuPath, depth + 1);

        std::wstring rightClickPath = basePath + L"[" + std::to_wstring(j) + L"].rightClickSubmenu";
        item.rightClickSubmenu = LoadSubmenuRecursive(rightClickPath, depth + 1);

        items.push_back(std::move(item));
    }
    return items;
}

static std::vector<ActionItem> LoadSubmenu(int btnIdx) {
    std::wstring basePath = L"buttons[" + std::to_wstring(btnIdx) + L"].submenu";
    return LoadSubmenuRecursive(basePath, 0);
}

static std::vector<ActionItem> LoadRightClickSubmenu(int btnIdx) {
    std::wstring basePath = L"buttons[" + std::to_wstring(btnIdx) + L"].rightClickSubmenu";
    return LoadSubmenuRecursive(basePath, 0);
}

static void BuildButtons() {
    bool ruLang;
    { std::lock_guard<std::mutex> lk(g_settingsMutex); ruLang = g_settings.langRussian; }

    std::vector<ActionItem> newButtons;

    for (int i = 0; i < 128; ++i) {
        PCWSTR presetRaw = Wh_GetStringSetting(L"buttons[%d].Preset", i);
        PCWSTR nRaw      = Wh_GetStringSetting(L"buttons[%d].Name",   i);
        PCWSTR iRaw      = Wh_GetStringSetting(L"buttons[%d].Icon",   i);

        std::wstring preset    = presetRaw ? presetRaw : L"";
        std::wstring rawName   = nRaw ? nRaw : L"";
        std::wstring rawIcon   = DecodeEscapes(NormalizeIconString(iRaw ? iRaw : L""));

        SafeFreeString(presetRaw); SafeFreeString(nRaw);
        SafeFreeString(iRaw);

        std::vector<std::wstring> actionArray;
        for (int k = 0; k < 64; ++k) {
            WCHAR actionPath[128];
            swprintf_s(actionPath, L"buttons[%d].Action[%d]", i, k);
            PCWSTR cmd = Wh_GetStringSetting(actionPath);
            if (!cmd || wcslen(cmd) == 0) {
                SafeFreeString(cmd);
                break;
            }
            std::wstring cmdStr = cmd;
            SafeFreeString(cmd);
            cmdStr = Trim(cmdStr);
            if (cmdStr.empty()) break;
            actionArray.push_back(std::move(cmdStr));
        }

        if (actionArray.empty()) {
            PCWSTR oldAction = Wh_GetStringSetting(L"buttons[%d].Action", i);
            if (oldAction && wcslen(oldAction) > 0) {
                std::wstring old = Trim(oldAction);
                if (!old.empty()) actionArray.push_back(std::move(old));
            }
            SafeFreeString(oldAction);
        }

        std::wstring presetKey = ToLower(Trim(preset));
        bool isCustom = (presetKey == L"custom" || presetKey.empty());

        if (isCustom) {
            bool allEmpty = Trim(rawName).empty() &&
                            Trim(rawIcon).empty() &&
                            actionArray.empty();
            if (presetKey.empty() && allEmpty) break;
            if (allEmpty) continue;
        }

        const PresetDef* pd = FindPreset(preset);
        auto userSub = LoadSubmenu(i);
        auto userRightSub = LoadRightClickSubmenu(i);

        ActionItem item;

        if (pd) {
            item.name = Trim(rawName).empty() ? PresetName(*pd, ruLang) : Trim(rawName);
            item.icon = Trim(rawIcon).empty()  ? pd->icon               : Trim(rawIcon);

            if (pd->action) {
                item.actionArray = { pd->action };
            } else {
                for (const wchar_t* key : { L"lock", L"sleep", L"shutdown", L"restart" }) {
                    const PresetDef* sp = FindPreset(key);
                    if (!sp) continue;
                    ActionItem si;
                    si.name        = PresetName(*sp, ruLang);
                    si.icon        = sp->icon;
                    si.actionArray = { sp->action };
                    item.submenu.push_back(std::move(si));
                }
            }
        } else {
            item.name   = Trim(rawName);
            item.icon   = Trim(rawIcon).empty() ? FALLBACK_ICON : Trim(rawIcon);
            item.actionArray = std::move(actionArray);
            if (!userSub.empty()) item.submenu = std::move(userSub);
            if (!userRightSub.empty()) item.rightClickSubmenu = std::move(userRightSub);
        }

        newButtons.push_back(std::move(item));
    }

    std::vector<std::vector<std::wstring>> reg;
    RegisterActions(newButtons, reg);

    std::lock_guard<std::mutex> lk(g_settingsMutex);
    g_buttons = std::move(newButtons);
    g_actionRegistry = std::move(reg);
}

static bool IsExplorerProcess() {
    WCHAR path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    return ToLower(path).find(L"explorer.exe") != std::wstring::npos;
}

static std::wstring ToW(winrt::hstring const& h) { return std::wstring(h.c_str(), h.size()); }

static std::wstring ClassOf(wux::DependencyObject const& o) {
    try { return ToW(winrt::get_class_name(o)); } catch (...) { return L""; }
}

static std::wstring LabelOf(wux::DependencyObject const& o) {
    std::wstring l = ClassOf(o);
    if (auto fe = o.try_as<wux::FrameworkElement>()) {
        std::wstring n = ToW(fe.Name());
        if (!n.empty()) l += L"#" + n;
    }
    return l;
}

static void DumpTree(wux::DependencyObject const& root, int depth, int maxDepth) {
    if (!root || depth > maxDepth) return;
    std::wstring indent(static_cast<size_t>(depth) * 2, L' ');
    Wh_Log(L"%s%s", indent.c_str(), LabelOf(root).c_str());
    int n = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; ++i)
        DumpTree(wuxm::VisualTreeHelper::GetChild(root, i), depth + 1, maxDepth);
}

static wux::DependencyObject FindByLabel(wux::DependencyObject const& root,
                                         std::wstring_view label, int maxDepth) {
    if (!root || maxDepth < 0) return nullptr;
    if (LabelOf(root) == label) return root;
    int n = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; ++i)
        if (auto f = FindByLabel(wuxm::VisualTreeHelper::GetChild(root, i), label, maxDepth - 1))
            return f;
    return nullptr;
}

static wux::DependencyObject FindByElementName(wux::DependencyObject const& root,
                                               std::wstring_view name, int maxDepth) {
    if (!root || maxDepth < 0) return nullptr;
    if (auto fe = root.try_as<wux::FrameworkElement>())
        if (std::wstring_view{fe.Name()} == name) return root;
    int n = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; ++i)
        if (auto f = FindByElementName(wuxm::VisualTreeHelper::GetChild(root, i), name, maxDepth - 1))
            return f;
    return nullptr;
}

static bool HasContainerTag(wux::DependencyObject const& o) {
    auto fe = o.try_as<wux::FrameworkElement>();
    if (!fe) return false;
    auto tag = fe.Tag();
    if (!tag) return false;
    return winrt::unbox_value_or<winrt::hstring>(tag, L"") == CONTAINER_TAG;
}

static wuxc::StackPanel FindOurContainer(wux::DependencyObject const& root, int maxDepth) {
    if (!root || maxDepth < 0) return nullptr;
    if (HasContainerTag(root))
        if (auto sp = root.try_as<wuxc::StackPanel>()) return sp;
    int n = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; ++i)
        if (auto f = FindOurContainer(wuxm::VisualTreeHelper::GetChild(root, i), maxDepth - 1))
            return f;
    return nullptr;
}

static bool IsGearText(winrt::hstring const& s) {
    return s.size() >= 1 && s.c_str()[0] == L'\uE713';
}

static bool HasGearGlyph(wux::DependencyObject const& o, int depth) {
    if (!o || depth < 0) return false;
    try {
        if (auto fi = o.try_as<wuxc::FontIcon>()) {
            if (IsGearText(fi.Glyph())) return true;
        } else if (auto tb = o.try_as<wuxc::TextBlock>()) {
            if (IsGearText(tb.Text())) return true;
        } else if (auto si = o.try_as<wuxc::SymbolIcon>()) {
            if (si.Symbol() == wuxc::Symbol::Setting) return true;
        }

        if (auto cc = o.try_as<wuxc::ContentControl>()) {
            auto content = cc.Content();
            if (content)
                if (auto d = content.try_as<wux::DependencyObject>())
                    if (HasGearGlyph(d, depth - 1)) return true;
        }
    } catch (...) {}
    int n = wuxm::VisualTreeHelper::GetChildrenCount(o);
    for (int i = 0; i < n; ++i)
        if (HasGearGlyph(wuxm::VisualTreeHelper::GetChild(o, i), depth - 1)) return true;
    return false;
}

static bool IsButtonLike(wux::DependencyObject const& o) {
    if (o.try_as<wuxcp::ButtonBase>()) return true;
    return ClassOf(o).find(L"Button") != std::wstring::npos;
}

static void CollectButtons(wux::DependencyObject const& o,
                           std::vector<wux::FrameworkElement>& out, int depth) {
    if (!o || depth < 0 || HasContainerTag(o)) return;
    if (IsButtonLike(o))
        if (auto fe = o.try_as<wux::FrameworkElement>()) out.push_back(fe);
    int n = wuxm::VisualTreeHelper::GetChildrenCount(o);
    for (int i = 0; i < n; ++i)
        CollectButtons(wuxm::VisualTreeHelper::GetChild(o, i), out, depth - 1);
}

static int SettingsScore(wux::FrameworkElement const& b) {
    try {
        if (HasGearGlyph(b, 8)) return 3;
        std::wstring name = ToLower(ToW(b.Name()));
        std::wstring aid  = ToLower(ToW(wuxa::AutomationProperties::GetAutomationId(b)));
        if (name.find(L"settingsbutton") != std::wstring::npos ||
            aid.find(L"settingsbutton")  != std::wstring::npos) return 2;
        if (name.find(L"setting") != std::wstring::npos ||
            aid.find(L"setting")  != std::wstring::npos) return 1;
    } catch (...) {}
    return 0;
}

static wux::FrameworkElement FindGearButton(wux::FrameworkElement const& footer) {
    std::vector<wux::FrameworkElement> buttons;
    CollectButtons(footer, buttons, 14);
    if (buttons.empty()) return nullptr;

    wux::FrameworkElement best{nullptr};
    int bestScore = 0;
    for (auto const& b : buttons) {
        int s = SettingsScore(b);
        if (s > bestScore) { best = b; bestScore = s; }
    }
    if (best) return best;

    if (buttons.size() == 1) return buttons[0];

    double bestX = -1e9;
    for (auto const& b : buttons) {
        try {
            if (b.ActualWidth() <= 0) continue;
            double x = b.TransformToVisual(footer).TransformPoint({0, 0}).X;
            if (x > bestX) { bestX = x; best = b; }
        } catch (...) {}
    }
    return best;
}

[[clang::no_destroy]] static std::vector<std::shared_ptr<void>> g_revokers;
[[clang::no_destroy]] static std::vector<std::shared_ptr<void>> g_menuRevokers;
[[clang::no_destroy]] static wux::DispatcherTimer::Tick_revoker g_earlyTick;
[[clang::no_destroy]] static wux::DispatcherTimer::Tick_revoker g_retryTick;

template <class R>
static void Keep(std::vector<std::shared_ptr<void>>& bag, R&& r) {
    bag.push_back(std::make_shared<std::decay_t<R>>(std::forward<R>(r)));
}

template <class TParent>
static void FillMenu(TParent const& parent, const std::vector<ActionItem>& items, bool invertIcons) {
    for (const auto& item : items) {
        std::wstring icon = item.icon.empty() ? std::wstring(FALLBACK_ICON) : item.icon;
        if (!item.submenu.empty()) {
            wuxc::MenuFlyoutSubItem sub;
            sub.Text(item.name);
            sub.Icon(MakeMenuIcon(icon));
            if (invertIcons) sub.FlowDirection(wux::FlowDirection::RightToLeft);
            FillMenu(sub, item.submenu, invertIcons);
            parent.Items().Append(sub);
        } else {
            wuxc::MenuFlyoutItem mi;
            mi.Text(item.name);
            mi.Icon(MakeMenuIcon(icon));
            if (invertIcons) mi.FlowDirection(wux::FlowDirection::RightToLeft);
            int  id      = item.actionId;
            bool isPress = !item.actionArray.empty() && StartsWithCI(item.actionArray[0], L"press:");
            Keep(g_menuRevokers, mi.Click(winrt::auto_revoke, [id, isPress](auto&&, auto&&) {
                if (isPress) CloseFlyoutForPress(); else CloseFlyoutAfterClick();
                SendActionToProxy(id);
            }));
            parent.Items().Append(mi);
        }
    }
}

static void ShowMenu(winrt::weak_ref<wuxc::Button> const& weakBtn,
                     const std::vector<ActionItem>& items, bool invertIcons) {
    auto btn = weakBtn.get();
    if (!btn || items.empty()) return;
    g_menuRevokers.clear();
    wuxc::MenuFlyout flyout;
    FillMenu(flyout, items, invertIcons);
    flyout.ShowAt(btn);
}

static void AttachRightClickMenu(wuxc::Button const& btn, std::vector<ActionItem> rightSub,
                                 bool invertIcons) {
    btn.IsRightTapEnabled(false);
    btn.IsHoldingEnabled(false);
    btn.AllowFocusOnInteraction(false);

    Keep(g_revokers, btn.ContextRequested(winrt::auto_revoke,
        [](auto&&, auto&& args) { args.Handled(true); }));

    auto weak = winrt::make_weak(btn);
    Keep(g_revokers, btn.PointerPressed(winrt::auto_revoke,
        [rightSub, weak, invertIcons](auto&&, auto&& args) {
            auto props = args.GetCurrentPoint(nullptr).Properties();
            if (props.IsRightButtonPressed()) {
                args.Handled(true);
                ShowMenu(weak, rightSub, invertIcons);
            }
        }));
    Keep(g_revokers, btn.RightTapped(winrt::auto_revoke,
        [](auto&&, auto&& args) { args.Handled(true); }));
    Keep(g_revokers, btn.Holding(winrt::auto_revoke,
        [](auto&&, auto&& args) { args.Handled(true); }));
    Keep(g_revokers, btn.PointerReleased(winrt::auto_revoke,
        [](auto&&, auto&& args) {
            auto props = args.GetCurrentPoint(nullptr).Properties();
            if (props.PointerUpdateKind() == winrt::Windows::UI::Input::PointerUpdateKind::RightButtonReleased)
                args.Handled(true);
        }));
}

static wuxc::StackPanel BuildContainer(const Settings& cur, std::vector<ActionItem> btns) {
    if (cur.invertButtons) std::reverse(btns.begin(), btns.end());

    wuxc::StackPanel container;
    container.Tag(winrt::box_value(CONTAINER_TAG));
    container.Orientation(wuxc::Orientation::Horizontal);
    container.VerticalAlignment(wux::VerticalAlignment::Center);
    container.HorizontalAlignment(cur.alignment);
    container.Margin({ static_cast<double>(cur.containerMarginLeft), 0,
                       static_cast<double>(cur.containerMarginRight), 0 });

    for (size_t i = 0; i < btns.size(); ++i) {
        const auto& def = btns[i];

        wuxc::Button btn;
        btn.Width(36); btn.Height(36);
        double rightMargin = (i + 1 < btns.size()) ? static_cast<double>(cur.buttonSpacing) : 0.0;
        btn.Margin({ 0, 0, rightMargin, 0 });
        btn.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
        btn.BorderThickness({ 0, 0, 0, 0 });
        btn.CornerRadius({ 4, 4, 4, 4 });
        btn.Padding({ 0, 0, 0, 0 });
        btn.Content(MakeButtonIcon(def.icon.empty() ? std::wstring(FALLBACK_ICON) : def.icon));
        wuxc::ToolTipService::SetToolTip(btn, winrt::box_value(def.name));

        bool invertIcons = cur.invertIconsSubmenus;

        if (!def.submenu.empty()) {
            auto weak = winrt::make_weak(btn);
            auto sub  = def.submenu;
            Keep(g_revokers, btn.Click(winrt::auto_revoke,
                [sub, weak, invertIcons](auto&&, auto&&) { ShowMenu(weak, sub, invertIcons); }));
        } else {
            int  id      = def.actionId;
            bool isPress = !def.actionArray.empty() && StartsWithCI(def.actionArray[0], L"press:");
            Keep(g_revokers, btn.Click(winrt::auto_revoke, [id, isPress](auto&&, auto&&) {
                if (isPress) CloseFlyoutForPress(); else CloseFlyoutAfterClick();
                SendActionToProxy(id);
            }));
        }

        if (!def.rightClickSubmenu.empty())
            AttachRightClickMenu(btn, def.rightClickSubmenu, invertIcons);

        container.Children().Append(btn);
    }
    return container;
}

struct Injection {
    winrt::weak_ref<wux::FrameworkElement> gear;
    wux::Visibility                        gearVisibility = wux::Visibility::Visible;
    bool                                   gearHidden     = false;
    winrt::weak_ref<wuxc::Panel>           panel;
    winrt::weak_ref<wuxc::StackPanel>      container;
};

[[clang::no_destroy]] static std::vector<Injection> g_injections;
static std::atomic<DWORD> g_xamlThreadId{0};
[[clang::no_destroy]] static wux::DispatcherTimer g_earlyInject{nullptr};
[[clang::no_destroy]] static wux::DispatcherTimer g_retryTimer{nullptr};
static int g_retryAttempts = 0;
static constexpr int kMaxRetryAttempts = 30;

static void StopRetry() {
    g_retryTick.revoke();
    if (g_retryTimer) {
        g_retryTimer.Stop();
        g_retryTimer = nullptr;
    }
    g_retryAttempts = 0;
}

static bool g_dumpedFooter = false;
static bool g_dumpedTree   = false;

static Injection* FindInjection(wuxc::StackPanel const& container) {
    for (auto& inj : g_injections)
        if (inj.container.get() == container) return &inj;
    return nullptr;
}

static void HideGear(Injection& inj, wux::FrameworkElement const& gear) {
    if (gear.Visibility() != wux::Visibility::Collapsed) {
        inj.gearVisibility = gear.Visibility();
        inj.gear = winrt::make_weak(gear);
        inj.gearHidden = true;
        gear.Visibility(wux::Visibility::Collapsed);
    }
}

static bool InjectInto(wux::FrameworkElement const& view) {
    Settings cur;
    std::vector<ActionItem> btns;
    {
        std::lock_guard<std::mutex> lk(g_settingsMutex);
        cur  = g_settings;
        btns = g_buttons;
    }
    if (btns.empty()) return true;

    wux::FrameworkElement footer{nullptr};
    if (auto l1 = FindByLabel(view, L"Windows.UI.Xaml.Controls.Grid#L1Grid", 8))
        if (auto f = FindByLabel(l1, L"Windows.UI.Xaml.Controls.Grid#FooterGrid", 6))
            footer = f.try_as<wux::FrameworkElement>();
    if (!footer)
        if (auto f = FindByElementName(view, L"FooterGrid", 14))
            footer = f.try_as<wux::FrameworkElement>();
    if (!footer) {
        if (!g_dumpedTree) {
            g_dumpedTree = true;
            Wh_Log(L"FooterGrid not found. Tree follows:");
            DumpTree(view, 0, 7);
        }
        return false;
    }

    auto gear = FindGearButton(footer);
    auto container = FindOurContainer(footer, 14);

    if (!gear && !container) {
        if (!g_dumpedFooter) {
            g_dumpedFooter = true;
            Wh_Log(L"Settings button not found in the footer. Footer tree follows:");
            DumpTree(footer, 0, 10);
        }
        return false;
    }

    if (container) {
        if (gear && cur.hideOriginalSettings)
            if (Injection* inj = FindInjection(container)) HideGear(*inj, gear);
        return true;
    }

    wux::FrameworkElement child = gear;
    wuxc::Panel panel{nullptr};
    for (int up = 0; up < 8 && child; ++up) {
        auto parent = wuxm::VisualTreeHelper::GetParent(child);
        if (!parent) break;
        if (auto p = parent.try_as<wuxc::Panel>()) { panel = p; break; }
        child = parent.try_as<wux::FrameworkElement>();
    }
    if (!panel || !child) {
        Wh_Log(L"Settings button has no panel parent; not injecting");
        return false;
    }

    container = BuildContainer(cur, btns);

    wuxc::Panel host = panel;
    bool spanFooter = false;
    wux::FrameworkElement anchor = child;
    {
        if (auto footerGrid = footer.try_as<wuxc::Grid>()) {
            wux::FrameworkElement top = gear;
            for (int up = 0; up < 12 && top; ++up) {
                auto parent = wuxm::VisualTreeHelper::GetParent(top);
                if (!parent) { top = nullptr; break; }
                if (parent == footer) break;
                top = parent.try_as<wux::FrameworkElement>();
            }
            if (top) {
                host = footerGrid;
                anchor = top;
                spanFooter = true;
            }
        }
    }

    if (spanFooter) {
        auto footerGrid = host.as<wuxc::Grid>();
        int cols = static_cast<int>(footerGrid.ColumnDefinitions().Size());
        wuxc::Grid::SetRow(container, wuxc::Grid::GetRow(anchor));
        wuxc::Grid::SetRowSpan(container, wuxc::Grid::GetRowSpan(anchor));
        wuxc::Grid::SetColumn(container, 0);
        wuxc::Grid::SetColumnSpan(container, cols > 0 ? cols : 1);

        if (!cur.hideOriginalSettings && cur.alignment == wux::HorizontalAlignment::Right) {
            double gearW = gear.ActualWidth();
            if (!(gearW > 0)) {
                gearW = gear.Width();
                if (!(gearW > 0)) gearW = 40;
            }
            auto gm = gear.Margin();
            auto cm = container.Margin();
            cm.Right += gearW + gm.Left + gm.Right;
            container.Margin(cm);
        }
        host.Children().Append(container);
    } else {
        if (auto grid = panel.try_as<wuxc::Grid>()) {
            wuxc::Grid::SetRow(container, wuxc::Grid::GetRow(child));
            wuxc::Grid::SetRowSpan(container, wuxc::Grid::GetRowSpan(child));
            wuxc::Grid::SetColumn(container, wuxc::Grid::GetColumn(child));
            wuxc::Grid::SetColumnSpan(container, wuxc::Grid::GetColumnSpan(child));
        }
        uint32_t idx = 0;
        if (panel.Children().IndexOf(child, idx))
            panel.Children().InsertAt(idx + 1, container);
        else
            panel.Children().Append(container);
    }

    Injection inj;
    inj.panel     = winrt::make_weak(host);
    inj.container = winrt::make_weak(container);
    inj.gear      = winrt::make_weak(gear);
    if (cur.hideOriginalSettings) HideGear(inj, gear);
    g_injections.push_back(std::move(inj));

    Wh_Log(L"Injected %zu button(s); gear = %s", btns.size(), LabelOf(gear).c_str());
    return true;
}

static bool TryInject(wux::FrameworkElement const& view) {
    static bool busy = false;
    if (g_unloading || busy) return false;
    busy = true;
    bool ok = false;
    try {
        ok = InjectInto(view);
    } catch (...) {
        Wh_Log(L"Injection failed: %08X", static_cast<unsigned>(winrt::to_hresult().value));
    }
    busy = false;
    return ok;
}

static void AttachInjector(wux::FrameworkElement const& view) {
    g_xamlThreadId.store(GetCurrentThreadId());
    if (TryInject(view)) { StopRetry(); return; }
    if (g_retryTimer) return;

    auto timer = wux::DispatcherTimer();
    timer.Interval(std::chrono::milliseconds(100));
    g_retryTick = timer.Tick(winrt::auto_revoke, [weak = winrt::make_weak(view)](wf::IInspectable const& sender,
                                               wf::IInspectable const&) {
        auto v = weak.get();
        if (g_unloading || !v || TryInject(v) || ++g_retryAttempts >= kMaxRetryAttempts) {
            if (auto self = sender.try_as<wux::DispatcherTimer>()) self.Stop();
            g_retryTimer = nullptr;
            g_retryAttempts = 0;
        }
    });
    timer.Start();
    g_retryTimer = timer;
}

static void RemoveInjections() {
    StopRetry();
    g_earlyTick.revoke();
    g_revokers.clear();
    g_menuRevokers.clear();
    if (g_earlyInject) {
        g_earlyInject.Stop();
        g_earlyInject = nullptr;
    }

    std::vector<Injection> injections;
    injections.swap(g_injections);

    for (auto& inj : injections) {
        try {
            auto panel     = inj.panel.get();
            auto container = inj.container.get();
            if (panel && container) {
                uint32_t idx = 0;
                if (panel.Children().IndexOf(container, idx))
                    panel.Children().RemoveAt(idx);
                container.Children().Clear();
            }
            if (inj.gearHidden)
                if (auto gear = inj.gear.get()) gear.Visibility(inj.gearVisibility);
        } catch (...) {
            Wh_Log(L"RemoveInjections error: %08X", static_cast<unsigned>(winrt::to_hresult().value));
        }
    }
    Wh_Log(L"Removed %zu injection(s)", injections.size());
}

static BOOL CALLBACK FirstThreadWindow(HWND hwnd, LPARAM lParam) {
    *reinterpret_cast<HWND*>(lParam) = hwnd;
    return FALSE;
}

static bool RunOnXamlThread(const std::function<void()>& fn) {
    static const UINT kRunMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    DWORD threadId = g_xamlThreadId.load();
    if (!threadId || !kRunMsg) return false;

    if (threadId == GetCurrentThreadId()) {
        try { fn(); } catch (...) { return false; }
        return true;
    }

    HWND target = nullptr;
    EnumThreadWindows(threadId, FirstThreadWindow, reinterpret_cast<LPARAM>(&target));
    if (!target) return false;

    struct RunParam { const std::function<void()>* fn; bool ran; };

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == kRunMsg && cwp->lParam) {
                    auto* p = reinterpret_cast<RunParam*>(cwp->lParam);
                    if (!p->ran) {
                        p->ran = true;
                        try { (*p->fn)(); } catch (...) {}
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) return false;

    RunParam param{ &fn, false };
    SendMessageW(target, kRunMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return param.ran;
}

static std::atomic<bool> g_discoveryHooked{false};

using OnGotFocus_t = int(WINAPI*)(void* pThis, void* args);
static OnGotFocus_t ControlCenterView_OnGotFocus_Original;

using Connect_t = int(WINAPI*)(void* pThis, int connectionId, void* target);
static Connect_t ControlCenterView_Connect_Original;

static wux::FrameworkElement FindContainingView(wux::DependencyObject const& from) {
    wux::DependencyObject node = from;
    for (int up = 0; up < 12 && node; ++up) {
        try {
            if (std::wstring_view{winrt::get_class_name(node)} == L"ControlCenter.ControlCenterView")
                return node.try_as<wux::FrameworkElement>();
        } catch (...) {}
        node = wuxm::VisualTreeHelper::GetParent(node);
    }
    return nullptr;
}

static int WINAPI ControlCenterView_Connect_Hook(void* pThis, int connectionId, void* target) {
    int ret = ControlCenterView_Connect_Original(pThis, connectionId, target);
    if (g_unloading || !target || g_earlyInject) return ret;

    try {
        wux::FrameworkElement child{nullptr};
        static_cast<::IUnknown*>(target)->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(child));
        if (!child) return ret;

        g_xamlThreadId.store(GetCurrentThreadId());

        auto timer = wux::DispatcherTimer();
        timer.Interval(std::chrono::milliseconds(1));
        g_earlyTick = timer.Tick(winrt::auto_revoke, [weak = winrt::make_weak(child)](wf::IInspectable const& sender,
                                                    wf::IInspectable const&) {
            if (auto self = sender.try_as<wux::DispatcherTimer>()) self.Stop();
            g_earlyInject = nullptr;
            if (g_unloading) return;
            try {
                auto element = weak.get();
                if (!element) return;
                if (auto view = FindContainingView(element)) AttachInjector(view);
            } catch (...) {}
        });
        timer.Start();
        g_earlyInject = timer;
    } catch (...) {}
    return ret;
}

static int WINAPI ControlCenterView_OnGotFocus_Hook(void* pThis, void* args) {
    int ret = ControlCenterView_OnGotFocus_Original(pThis, args);
    if (g_unloading) return ret;
    try {
        wux::FrameworkElement view{nullptr};
        static_cast<::IUnknown*>(pThis)->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(view));
        if (view) AttachInjector(view);
    } catch (...) {
        Wh_Log(L"OnGotFocus hook error");
    }
    return ret;
}

static void InstallDiscoveryHooks(HMODULE controlCenter, bool applyNow) {
    if (g_discoveryHooked.exchange(true)) return;

    WindhawkUtils::SYMBOL_HOOK controlCenterDllHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
            &ControlCenterView_OnGotFocus_Original,
            ControlCenterView_OnGotFocus_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Markup::IComponentConnector>::Connect(int,void *))"},
            &ControlCenterView_Connect_Original,
            ControlCenterView_Connect_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(controlCenter, controlCenterDllHooks, ARRAYSIZE(controlCenterDllHooks))) {
        Wh_Log(L"Could not hook ControlCenterView; the footer will not be changed");
        return;
    }
    if (applyNow && !Wh_ApplyHookOperations()) {
        Wh_Log(L"Hooks registered but could not be armed");
        return;
    }
    Wh_Log(L"ControlCenterView hooked");
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original;

static bool IsControlCenterDll(LPCWSTR path) {
    const wchar_t* name = path;
    for (const wchar_t* p = path; *p; ++p)
        if (*p == L'\\' || *p == L'/') name = p + 1;
    return _wcsicmp(name, L"ControlCenter.dll") == 0 || _wcsicmp(name, L"ControlCenter") == 0;
}

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR path, HANDLE file, DWORD flags) {
    HMODULE result = LoadLibraryExW_Original(path, file, flags);
    if (result && path && !g_discoveryHooked.load() && IsControlCenterDll(path)) {
        if (HMODULE module = GetModuleHandleW(L"ControlCenter.dll"))
            InstallDiscoveryHooks(module, /*applyNow=*/true);
    }
    return result;
}

static void StartControlCenterWatch() {
    if (HMODULE module = GetModuleHandleW(L"ControlCenter.dll")) {
        InstallDiscoveryHooks(module, /*applyNow=*/false);
        return;
    }
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto target = kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
                                   GetProcAddress(kernelBase, "LoadLibraryExW"))
                             : nullptr;
    if (!target) {
        Wh_Log(L"No kernelbase!LoadLibraryExW; the footer will not be changed");
        return;
    }
    WindhawkUtils::SetFunctionHook(target, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Wh_ModInit");
    g_isExplorer = IsExplorerProcess();

    LoadSettings();
    BuildButtons();

    StartWorker();

    if (g_isExplorer) {
        StartProxyThread();
        return TRUE;
    }

    StartControlCenterWatch();
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isExplorer || g_discoveryHooked.load()) return;
    if (HMODULE module = GetModuleHandleW(L"ControlCenter.dll"))
        InstallDiscoveryHooks(module, /*applyNow=*/true);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Wh_ModUninit");
    g_unloading = true;

    if (g_isExplorer) {
        StopProxyThread();
    } else if (g_xamlThreadId.load() != 0) {
        bool restored = false;
        for (int attempt = 0; attempt < 25 && !restored; ++attempt) {
            restored = RunOnXamlThread(&RemoveInjections);
            if (!restored) Sleep(200);
        }
        if (!restored)
            Wh_Log(L"Could not reach the XAML thread; the footer may stay modified until ShellHost restarts");
        g_xamlThreadId.store(0);
    }

    StopWorker();
}
