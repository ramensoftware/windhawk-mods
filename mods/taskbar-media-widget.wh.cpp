// ==WindhawkMod==
// @id taskbar-media-widget
// @name Taskbar Media Widget
// @description Compact media controls integrated into the Windows 10/11 taskbar.
// @version 0.3.1
// @author uMk0
// @github https://github.com/umk0
// @license MIT
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luser32 -lgdi32 -lmsimg32 -lshell32 -lshlwapi -lwindowscodecs -lwindowsapp -ldwmapi -lshcore -luuid -lksuser
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Media Widget

A compact media widget for the Windows 10 and Windows 11 taskbar. It reads
metadata and controls playback through Global System Media Transport Controls.

On Windows 11, the widget is inserted into the native tray XAML layout and
tracks the taskbar height automatically. On the classic Windows 10 taskbar,
including ExplorerPatcher configurations, it uses a DPI-aware Win32 surface
because that taskbar has no XAML tree.

The native Windows 11 bottom, top, left and right taskbar positions are
supported. Left and right taskbars use a compact vertical player that keeps
the artwork, playback time, transport controls and progress inside the bar.

## Screenshots

![Horizontal widget on the top taskbar](https://raw.githubusercontent.com/umk0/windhawk-mods/assets/taskbar-media-widget/horizontal-top.png)

![Compact widget on the left taskbar](https://raw.githubusercontent.com/umk0/windhawk-mods/assets/taskbar-media-widget/vertical.png)

Features:

- album art, title and artist
- always-visible previous / play-pause / next controls
- always-visible playback time
- seekable progress bar with a moving time tooltip
- title / artist marquee when text overflows
- artwork-derived progress and visualizer colors
- a real WASAPI loopback visualizer across the widget background
- mouse wheel app-volume control with a system-volume fallback
- compact idle launchers for configured music apps that skip pinned apps
- automatic layout updates when the taskbar edge or thickness changes

Unlike the existing taskbar media mods, this mod combines a native Windows 11
tray insertion with a Windows 10/classic-taskbar backend, keeps transport
controls visible, and includes the idle launcher and real audio visualizer in
the same compact surface.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- widgetWidth: 330
  $name: Widget width
- widgetHeight: 40
  $name: Widget height
- coverSize: 32
  $name: Cover size
- coverRadius: 7
  $name: Cover radius
- textWidth: 132
  $name: Text area width
- buttonSize: 28
  $name: Button size
- progressHeight: 3
  $name: Progress height
- progressHoverHeight: 6
  $name: Progress hover height
- accentColor: "#2EA8FF"
  $name: Accent color
- textColor: "#FFFFFF"
  $name: Title text color
- mutedTextColor: "#B9C0CC"
  $name: Artist/time text color
- followSystemTheme: true
  $name: Follow system theme
- themeMode: auto
  $name: Theme mode
  $description: Use light/dark if Windows reports the wrong taskbar theme.
  $options:
    - auto: Auto
    - light: Light
    - dark: Dark
- hideWhenNoMedia: true
  $name: Hide when no media
- idleApps: "Yandex Music,Spotify,Apple Music"
  $name: Idle apps
  $description: Comma-separated Start menu app names to show when no media session is active. Apps pinned to the taskbar are skipped when possible.
- marqueeSpeed: "0.55"
  $name: Marquee speed
- volumeStep: "0.02"
  $name: Wheel volume step
  $description: Volume change per mouse wheel notch. The widget tries app volume first and falls back to system volume.
- visualizerEnabled: true
  $name: Audio visualizer
  $description: Draw a subtle real WASAPI loopback spectrum behind the track text.
- visualizerOpacity: "0.32"
  $name: Visualizer opacity
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0A00
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#if !defined(NTDDI_VERSION) || NTDDI_VERSION < 0x0A000000
#undef NTDDI_VERSION
#define NTDDI_VERSION 0x0A000000
#endif

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

#include <shcore.h>
#include <windows.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <wincodec.h>
#include <windhawk_utils.h>
#include <windowsx.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Media::Control;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;
using namespace winrt::Windows::UI::Xaml::Shapes;

constexpr int kVisualizerMinBarCount = 18;
constexpr int kVisualizerMaxBarCount = 64;
constexpr int kVisualizerBarWidth = 3;
constexpr int kVisualizerBarGap = 2;
constexpr ULONGLONG kMissingMediaGraceMs = 2500;

enum class ThemeMode {
    Auto,
    Light,
    Dark,
};

enum class TaskbarEdge {
    Unknown,
    Bottom,
    Top,
    Left,
    Right,
};

bool IsVerticalTaskbarEdge(TaskbarEdge edge) {
    return edge == TaskbarEdge::Left || edge == TaskbarEdge::Right;
}

class ApartmentScope {
public:
    ApartmentScope() {
        init_apartment(apartment_type::multi_threaded);
    }

    ~ApartmentScope() {
        uninit_apartment();
    }

    ApartmentScope(ApartmentScope const&) = delete;
    ApartmentScope& operator=(ApartmentScope const&) = delete;
};

struct Settings {
    int widgetWidth = 330;
    int widgetHeight = 40;
    int coverSize = 32;
    int coverRadius = 7;
    int textWidth = 132;
    int buttonSize = 28;
    int progressHeight = 3;
    int progressHoverHeight = 6;
    COLORREF accentColor = RGB(46, 168, 255);
    COLORREF textColor = RGB(255, 255, 255);
    COLORREF mutedTextColor = RGB(185, 192, 204);
    bool followSystemTheme = true;
    ThemeMode themeMode = ThemeMode::Auto;
    bool hideWhenNoMedia = true;
    std::wstring idleApps;
    double marqueeSpeed = 0.55;
    double volumeStep = 0.02;
    bool visualizerEnabled = true;
    double visualizerOpacity = 0.32;
};

struct MediaState {
    bool hasSession = false;
    bool playOnlySession = false;
    bool isPlaying = false;
    bool canPrevious = false;
    bool canNext = false;
    bool canPlay = false;
    bool canPlayPause = false;
    bool canSeek = false;
    std::wstring sourceAppId;
    std::wstring title;
    std::wstring artist;
    int64_t timelineStartMs = 0;
    int64_t positionMs = 0;
    int64_t durationMs = 0;
    ULONGLONG updatedTick = 0;
    IRandomAccessStreamReference thumbnail{nullptr};
    std::vector<uint8_t> thumbnailBytes;
    bool hasAccentColor = false;
    COLORREF accentColor = RGB(46, 168, 255);
    std::wstring thumbnailKey;
};

struct TextScrollState {
    double offset = 0.0;
    bool forward = true;
    std::wstring lastText;
};

struct IdleApp {
    std::wstring name;
    std::wstring shortcutPath;
    std::vector<uint8_t> iconBytes;
};

Settings g_settings;
std::atomic<bool> g_unloading = false;
std::atomic<bool> g_workersStopping = false;
std::atomic<bool> g_uiDirty = false;
std::atomic<bool> g_taskbarHooksInstalled = false;
std::atomic<bool> g_modernWidgetAttached = false;
std::atomic<bool> g_classicBackendStarted = false;
std::atomic<TaskbarEdge> g_injectedTaskbarEdge = TaskbarEdge::Unknown;
std::atomic<int> g_injectedTaskbarThicknessPx = 0;
// Raw pointers are intentional: their destructors must not run during
// DLL_PROCESS_DETACH while Explorer is shutting down. Normal mod unload still
// joins and deletes every thread explicitly.
std::thread* g_mediaThread = nullptr;
std::thread* g_tickThread = nullptr;
std::thread* g_audioThread = nullptr;
std::thread* g_classicWidgetThread = nullptr;
std::mutex g_mediaMutex;
std::mutex g_visualizerMutex;
MediaState g_mediaState;
GlobalSystemMediaTransportControlsSession g_mediaSession{nullptr};
std::array<float, kVisualizerMaxBarCount> g_visualizerBands{};
ULONGLONG g_visualizerUpdatedTick = 0;
ULONGLONG g_missingMediaSinceTick = 0;

HWND g_taskbarWnd = nullptr;
Grid g_widgetGrid{nullptr};
FrameworkElement g_injectionParent{nullptr};
int g_widgetColumn = -1;
int g_widgetRow = -1;
HWND g_classicWidgetWnd = nullptr;
ULONGLONG g_backendDetectionStartedTick = 0;
bool g_verticalWidgetLayout = false;
double g_verticalWidgetWidth = 40.0;
double g_verticalSessionHeight = 144.0;

FrameworkElement g_coverContainer{nullptr};
FrameworkElement g_textContainer{nullptr};
FrameworkElement g_rightContainer{nullptr};
Grid g_visualizerGrid{nullptr};
Grid g_progressGrid{nullptr};
RowDefinition g_progressRow{nullptr};
StackPanel g_idlePanel{nullptr};
Image g_coverImage{nullptr};
TextBlock g_titleText{nullptr};
TextBlock g_artistText{nullptr};
TextBlock g_timeText{nullptr};
Border g_prevButton{nullptr};
Border g_playButton{nullptr};
Border g_nextButton{nullptr};
TextBlock g_prevButtonIcon{nullptr};
TextBlock g_playButtonIcon{nullptr};
TextBlock g_nextButtonIcon{nullptr};
Border g_progressHoverTip{nullptr};
TextBlock g_progressToolTipText{nullptr};
Border g_progressTrack{nullptr};
Border g_progressFill{nullptr};
winrt::Windows::UI::Xaml::Shapes::Ellipse g_progressThumb{nullptr};
TranslateTransform g_titleTransform{nullptr};
TranslateTransform g_artistTransform{nullptr};
Canvas g_titleCanvas{nullptr};
Canvas g_artistCanvas{nullptr};
TextScrollState g_titleScroll;
TextScrollState g_artistScroll;
bool g_progressHover = false;
std::wstring g_loadedThumbnailKey;
std::vector<IdleApp> g_idleApps;
std::vector<Border> g_visualizerBars;
int g_visualizerBarCount = kVisualizerMinBarCount;
std::wstring g_playButtonGlyph;
bool g_prevButtonHover = false;
bool g_playButtonHover = false;
bool g_nextButtonHover = false;
bool g_prevButtonEnabled = true;
bool g_playButtonEnabled = true;
bool g_nextButtonEnabled = true;

struct LayoutMetrics {
    int widgetHeight = 40;
    int contentHeight = 34;
    int coverSize = 28;
    int coverRadius = 7;
    int buttonSize = 28;
    int textWidth = 132;
    int progressHeight = 3;
    int progressHoverHeight = 6;
    int timeWidth = 50;
    int controlsWidth = 72;
    int rightWidth = 122;
    double titleFont = 12.0;
    double artistFont = 10.5;
    double timeFont = 9.5;
};

LayoutMetrics g_layout;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
using Std_Ref_Decref_t = void(WINAPI*)(void*);

CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
Std_Ref_Decref_t Std_Ref_Decref_Original = nullptr;
void* CTaskBand_ITaskListWndSite_vftable = nullptr;

using WindowThreadProc = void (*)(void*);

const GUID kCLSID_ShellLink = {0x00021401,
                               0x0000,
                               0x0000,
                               {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x46}};
const GUID kIID_IShellLinkW = {0x000214F9,
                               0x0000,
                               0x0000,
                               {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x46}};
const GUID kIID_IPersistFile = {0x0000010B,
                                0x0000,
                                0x0000,
                                {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x46}};

int ClampInt(int value, int minValue, int maxValue) {
    return value < minValue ? minValue : value > maxValue ? maxValue : value;
}

double ClampDouble(double value, double minValue, double maxValue) {
    return value < minValue ? minValue : value > maxValue ? maxValue : value;
}

COLORREF ParseColor(PCWSTR value, COLORREF fallback) {
    if (!value || value[0] != L'#' || wcslen(value) != 7) {
        return fallback;
    }

    wchar_t* end = nullptr;
    long raw = wcstol(value + 1, &end, 16);
    if (!end || *end != L'\0' || raw < 0 || raw > 0xFFFFFF) {
        return fallback;
    }

    return RGB((raw >> 16) & 0xFF, (raw >> 8) & 0xFF, raw & 0xFF);
}

ThemeMode ParseThemeMode(PCWSTR value) {
    if (!value) {
        return ThemeMode::Auto;
    }
    if (_wcsicmp(value, L"light") == 0) {
        return ThemeMode::Light;
    }
    if (_wcsicmp(value, L"dark") == 0) {
        return ThemeMode::Dark;
    }
    return ThemeMode::Auto;
}

std::wstring Trim(std::wstring value) {
    size_t start = 0;
    while (start < value.size() && iswspace(value[start])) {
        start++;
    }

    size_t end = value.size();
    while (end > start && iswspace(value[end - 1])) {
        end--;
    }

    return value.substr(start, end - start);
}

std::wstring ToLower(std::wstring value) {
    for (auto& ch : value) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return value;
}

std::vector<std::wstring> SplitCsv(std::wstring const& value) {
    std::vector<std::wstring> result;
    size_t start = 0;
    while (start <= value.size()) {
        size_t end = value.find(L',', start);
        std::wstring item = Trim(value.substr(
            start, end == std::wstring::npos ? std::wstring::npos : end - start));
        if (!item.empty()) {
            result.push_back(item);
        }
        if (end == std::wstring::npos) {
            break;
        }
        start = end + 1;
    }
    return result;
}

std::wstring ExpandEnv(PCWSTR value) {
    wchar_t buffer[MAX_PATH * 2]{};
    DWORD length = ExpandEnvironmentStringsW(value, buffer, ARRAYSIZE(buffer));
    if (length == 0 || length >= ARRAYSIZE(buffer)) {
        return {};
    }
    return buffer;
}

std::wstring FileStem(std::wstring path) {
    size_t slash = path.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        path = path.substr(slash + 1);
    }
    size_t dot = path.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        path = path.substr(0, dot);
    }
    return path;
}

bool AppNameMatches(std::wstring const& candidate,
                    std::wstring const& query) {
    std::wstring candidateLower = ToLower(candidate);
    std::wstring queryLower = ToLower(query);
    if (candidateLower.find(queryLower) != std::wstring::npos ||
        queryLower.find(candidateLower) != std::wstring::npos) {
        return true;
    }

    // Keep the public default language-neutral without losing localized Start
    // menu entries on Russian Windows installations.
    if (queryLower == L"yandex music") {
        return candidateLower.find(L"яндекс музыка") != std::wstring::npos;
    }
    return false;
}

void FindShortcutFiles(std::wstring const& directory,
                       std::vector<std::wstring>& files) {
    if (directory.empty()) {
        return;
    }

    WIN32_FIND_DATAW data{};
    std::wstring pattern = directory + L"\\*";
    HANDLE find = FindFirstFileW(pattern.c_str(), &data);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (wcscmp(data.cFileName, L".") == 0 ||
            wcscmp(data.cFileName, L"..") == 0) {
            continue;
        }

        std::wstring path = directory + L"\\" + data.cFileName;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            FindShortcutFiles(path, files);
        } else if (StrStrIW(data.cFileName, L".lnk")) {
            files.push_back(path);
        }
    } while (FindNextFileW(find, &data));

    FindClose(find);
}

bool IsPinnedTaskbarApp(std::wstring const& query) {
    std::wstring pinnedDir = ExpandEnv(
        L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar");
    std::vector<std::wstring> shortcuts;
    FindShortcutFiles(pinnedDir, shortcuts);

    for (auto const& shortcut : shortcuts) {
        if (AppNameMatches(FileStem(shortcut), query)) {
            return true;
        }
    }

    return false;
}

std::vector<uint8_t> HIconToPngBytes(HICON icon) {
    if (!icon) {
        return {};
    }

    try {
        com_ptr<IWICImagingFactory> factory;
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                    CLSCTX_INPROC_SERVER,
                                    IID_IWICImagingFactory,
                                    factory.put_void()))) {
            return {};
        }

        com_ptr<IWICBitmap> bitmap;
        if (FAILED(factory->CreateBitmapFromHICON(icon, bitmap.put()))) {
            return {};
        }

        HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, 0);
        if (!memory) {
            return {};
        }

        com_ptr<IStream> stream;
        if (FAILED(CreateStreamOnHGlobal(memory, TRUE, stream.put()))) {
            GlobalFree(memory);
            return {};
        }

        com_ptr<IWICBitmapEncoder> encoder;
        if (FAILED(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr,
                                          encoder.put())) ||
            FAILED(encoder->Initialize(stream.get(), WICBitmapEncoderNoCache))) {
            return {};
        }

        com_ptr<IWICBitmapFrameEncode> frame;
        if (FAILED(encoder->CreateNewFrame(frame.put(), nullptr)) ||
            FAILED(frame->Initialize(nullptr)) ||
            FAILED(frame->WriteSource(bitmap.get(), nullptr)) ||
            FAILED(frame->Commit()) || FAILED(encoder->Commit())) {
            return {};
        }

        LARGE_INTEGER start{};
        ULARGE_INTEGER end{};
        stream->Seek(start, STREAM_SEEK_END, &end);
        stream->Seek(start, STREAM_SEEK_SET, nullptr);

        std::vector<uint8_t> bytes(static_cast<size_t>(end.QuadPart));
        if (!bytes.empty()) {
            ULONG read = 0;
            stream->Read(bytes.data(), static_cast<ULONG>(bytes.size()), &read);
            bytes.resize(read);
        }
        return bytes;
    } catch (...) {
        return {};
    }
}

std::vector<uint8_t> ReadShortcutIconBytes(std::wstring const& shortcutPath) {
    std::wstring iconPath;
    int iconIndex = 0;
    std::wstring targetPath;

    try {
        com_ptr<IShellLinkW> shellLink;
        if (SUCCEEDED(CoCreateInstance(kCLSID_ShellLink, nullptr,
                                       CLSCTX_INPROC_SERVER, kIID_IShellLinkW,
                                       shellLink.put_void()))) {
            com_ptr<IPersistFile> persistFile;
            if (SUCCEEDED(shellLink->QueryInterface(kIID_IPersistFile,
                                                    persistFile.put_void())) &&
                SUCCEEDED(persistFile->Load(shortcutPath.c_str(), STGM_READ))) {
                wchar_t rawIconPath[MAX_PATH]{};
                if (SUCCEEDED(shellLink->GetIconLocation(rawIconPath,
                                                         ARRAYSIZE(rawIconPath),
                                                         &iconIndex)) &&
                    rawIconPath[0]) {
                    wchar_t expanded[MAX_PATH]{};
                    DWORD expandedLength = ExpandEnvironmentStringsW(
                        rawIconPath, expanded, ARRAYSIZE(expanded));
                    iconPath = expandedLength > 0 && expandedLength < ARRAYSIZE(expanded)
                                   ? expanded
                                   : rawIconPath;
                }

                wchar_t rawTargetPath[MAX_PATH]{};
                if (SUCCEEDED(shellLink->GetPath(rawTargetPath,
                                                 ARRAYSIZE(rawTargetPath),
                                                 nullptr, SLGP_RAWPATH)) &&
                    rawTargetPath[0]) {
                    targetPath = rawTargetPath;
                }
            }
        }
    } catch (...) {
    }

    HICON icon = nullptr;
    if (!iconPath.empty() && _wcsicmp(PathFindExtensionW(iconPath.c_str()), L".lnk") != 0) {
        ExtractIconExW(iconPath.c_str(), iconIndex, nullptr, &icon, 1);
    }

    if (!icon && !targetPath.empty()) {
        SHFILEINFOW info{};
        DWORD_PTR ok = SHGetFileInfoW(targetPath.c_str(), 0, &info, sizeof(info),
                                      SHGFI_ICON | SHGFI_LARGEICON);
        if (ok) {
            icon = info.hIcon;
        }
    }

    if (!icon) {
        SHFILEINFOW info{};
        DWORD_PTR ok = SHGetFileInfoW(shortcutPath.c_str(), 0, &info, sizeof(info),
                                      SHGFI_ICON | SHGFI_LARGEICON);
        if (ok) {
            icon = info.hIcon;
        }
    }

    std::vector<uint8_t> bytes = HIconToPngBytes(icon);
    if (icon) {
        DestroyIcon(icon);
    }
    return bytes;
}

void ScanIdleApps() {
    std::vector<std::wstring> shortcuts;
    FindShortcutFiles(ExpandEnv(L"%APPDATA%\\Microsoft\\Windows\\Start Menu\\Programs"),
                      shortcuts);
    FindShortcutFiles(ExpandEnv(L"%PROGRAMDATA%\\Microsoft\\Windows\\Start Menu\\Programs"),
                      shortcuts);

    std::vector<IdleApp> found;
    for (auto const& query : SplitCsv(g_settings.idleApps)) {
        if (IsPinnedTaskbarApp(query)) {
            continue;
        }

        for (auto const& shortcut : shortcuts) {
            std::wstring stem = FileStem(shortcut);
            if (AppNameMatches(stem, query)) {
                bool exists = false;
                for (auto const& app : found) {
                    if (_wcsicmp(app.shortcutPath.c_str(), shortcut.c_str()) == 0) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    found.push_back({stem, shortcut, ReadShortcutIconBytes(shortcut)});
                }
                break;
            }
        }
    }

    g_idleApps = std::move(found);
    Wh_Log(L"Idle apps found: %u", static_cast<unsigned>(g_idleApps.size()));
}

winrt::Windows::UI::Color XamlColor(COLORREF color, BYTE alpha = 0xFF) {
    return {alpha, GetRValue(color), GetGValue(color), GetBValue(color)};
}

SolidColorBrush MakeBrush(COLORREF color, BYTE alpha = 0xFF) {
    return SolidColorBrush(XamlColor(color, alpha));
}

double ColorLuminance(COLORREF color) {
    return (0.2126 * GetRValue(color) + 0.7152 * GetGValue(color) +
            0.0722 * GetBValue(color)) /
           255.0;
}

COLORREF ProgressAccentColor(MediaState const& state) {
    return state.hasAccentColor ? state.accentColor : g_settings.accentColor;
}

COLORREF ProgressThumbColor(COLORREF accent) {
    return ColorLuminance(accent) > 0.58 ? RGB(38, 42, 48) : RGB(245, 247, 250);
}

COLORREF VisualizerAccentColor(COLORREF accent) {
    double luminance = ColorLuminance(accent);
    if (luminance >= 0.48) {
        return accent;
    }

    double mix = ClampDouble((0.48 - luminance) * 1.8, 0.18, 0.55);
    int r = ClampInt(static_cast<int>(std::round(
                         GetRValue(accent) + (255 - GetRValue(accent)) * mix)),
                     0, 255);
    int g = ClampInt(static_cast<int>(std::round(
                         GetGValue(accent) + (255 - GetGValue(accent)) * mix)),
                     0, 255);
    int b = ClampInt(static_cast<int>(std::round(
                         GetBValue(accent) + (255 - GetBValue(accent)) * mix)),
                     0, 255);
    return RGB(r, g, b);
}

bool UseLightTheme();

COLORREF ButtonHoverColor() {
    return RGB(255, 255, 255);
}

BYTE ButtonHoverAlpha() {
    return UseLightTheme() ? 150 : 36;
}

void SetPointerCursor(winrt::Windows::UI::Core::CoreCursorType type) {
    try {
        auto window = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread();
        if (window) {
            window.PointerCursor(winrt::Windows::UI::Core::CoreCursor(type, 0));
        }
    } catch (...) {
    }

    SetCursor(LoadCursorW(nullptr, type == winrt::Windows::UI::Core::CoreCursorType::Hand
                                       ? IDC_HAND
                                       : IDC_ARROW));
}

void OpenCurrentMediaApp() {
    std::wstring appId;
    {
        std::lock_guard lock(g_mediaMutex);
        appId = g_mediaState.sourceAppId;
    }

    if (appId.empty()) {
        return;
    }

    std::thread([appId]() {
        std::wstring shellPath = L"shell:AppsFolder\\" + appId;
        HINSTANCE result = ShellExecuteW(nullptr, L"open", shellPath.c_str(),
                                         nullptr, nullptr, SW_SHOWNORMAL);
        if (reinterpret_cast<intptr_t>(result) <= 32) {
            Wh_Log(L"OpenCurrentMediaApp failed: appId=%s code=%Id",
                   appId.c_str(), reinterpret_cast<intptr_t>(result));
        }
    }).detach();
}

LayoutMetrics CalculateLayout(double availableHeight) {
    LayoutMetrics layout;

    int heightFromTaskbar =
        availableHeight > 0 ? static_cast<int>(std::round(availableHeight))
                            : g_settings.widgetHeight;
    layout.widgetHeight = ClampInt(heightFromTaskbar, 34, g_settings.widgetHeight);
    layout.progressHeight = ClampInt(g_settings.progressHeight, 2, 6);
    layout.progressHoverHeight =
        ClampInt(g_settings.progressHoverHeight, layout.progressHeight,
                 std::max(layout.progressHeight, layout.widgetHeight / 6));
    layout.contentHeight = std::max(24, layout.widgetHeight - layout.progressHoverHeight);
    if (layout.widgetHeight <= 38) {
        layout.titleFont = 11.0;
        layout.artistFont = 10.0;
        layout.timeFont = 9.0;
    }

    int textVisualHeight =
        static_cast<int>(std::round(layout.titleFont + layout.artistFont + 3.0));
    layout.coverSize =
        ClampInt(std::min({g_settings.coverSize, layout.contentHeight - 10,
                           textVisualHeight - 2}),
                 18, 34);
    layout.coverRadius = ClampInt(std::min(g_settings.coverRadius, layout.coverSize / 3),
                                  0, 14);
    layout.buttonSize =
        ClampInt(std::min(g_settings.buttonSize, layout.contentHeight - 4), 24, 30);
    layout.controlsWidth = layout.buttonSize * 3 + 6;
    layout.timeWidth = layout.widgetHeight <= 38 ? 52 : 58;
    layout.rightWidth = layout.controlsWidth + layout.timeWidth;
    layout.textWidth =
        ClampInt(g_settings.widgetWidth - layout.coverSize - layout.rightWidth - 18,
                 72, g_settings.textWidth);

    return layout;
}

void ResetWidgetElementReferences() {
    g_coverContainer = nullptr;
    g_textContainer = nullptr;
    g_rightContainer = nullptr;
    g_visualizerGrid = nullptr;
    g_progressGrid = nullptr;
    g_progressRow = nullptr;
    g_idlePanel = nullptr;
    g_coverImage = nullptr;
    g_titleText = nullptr;
    g_artistText = nullptr;
    g_timeText = nullptr;
    g_prevButton = nullptr;
    g_playButton = nullptr;
    g_nextButton = nullptr;
    g_prevButtonIcon = nullptr;
    g_playButtonIcon = nullptr;
    g_nextButtonIcon = nullptr;
    g_progressHoverTip = nullptr;
    g_progressToolTipText = nullptr;
    g_progressTrack = nullptr;
    g_progressFill = nullptr;
    g_progressThumb = nullptr;
    g_titleTransform = nullptr;
    g_artistTransform = nullptr;
    g_titleCanvas = nullptr;
    g_artistCanvas = nullptr;
    g_visualizerBars.clear();
    g_loadedThumbnailKey.clear();
    g_progressHover = false;
    g_playButtonGlyph.clear();
    g_prevButtonHover = false;
    g_playButtonHover = false;
    g_nextButtonHover = false;
    g_prevButtonEnabled = true;
    g_playButtonEnabled = true;
    g_nextButtonEnabled = true;
}

double VerticalIdleWidgetHeight() {
    size_t count = std::min<size_t>(g_idleApps.size(), 6);
    if (count == 0) {
        return g_verticalWidgetWidth;
    }

    return 8.0 + count * g_layout.buttonSize + (count - 1) * 4.0;
}

double IdleWidgetWidth() {
    size_t count = std::min<size_t>(g_idleApps.size(), 6);
    if (count == 0) {
        return 0;
    }

    double width = 8.0 + count * g_layout.buttonSize + (count - 1) * 4.0;
    return ClampDouble(width, static_cast<double>(g_layout.buttonSize + 8),
                       static_cast<double>(g_settings.widgetWidth));
}

int VisualizerBarCountForWidth(double width) {
    int available = std::max(1, static_cast<int>(std::round(width)) - 8);
    int pitch = kVisualizerBarWidth + kVisualizerBarGap;
    int count = (available + kVisualizerBarGap) / pitch;
    return ClampInt(count, kVisualizerMinBarCount, kVisualizerMaxBarCount);
}

bool RegistryUseLightTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr,
                     &value, &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

bool UseLightTheme() {
    if (g_settings.themeMode == ThemeMode::Light) {
        return true;
    }
    if (g_settings.themeMode == ThemeMode::Dark) {
        return false;
    }
    return g_settings.followSystemTheme && RegistryUseLightTheme();
}

COLORREF ActiveTextColor() {
    if (g_settings.themeMode == ThemeMode::Auto && !g_settings.followSystemTheme) {
        return g_settings.textColor;
    }
    return UseLightTheme() ? RGB(18, 24, 33) : RGB(255, 255, 255);
}

COLORREF ActiveMutedTextColor() {
    if (g_settings.themeMode == ThemeMode::Auto && !g_settings.followSystemTheme) {
        return g_settings.mutedTextColor;
    }
    return UseLightTheme() ? RGB(78, 88, 104) : RGB(185, 192, 204);
}

void LoadSettings() {
    g_settings.widgetWidth = ClampInt(Wh_GetIntSetting(L"widgetWidth"), 220, 560);
    g_settings.widgetHeight = ClampInt(Wh_GetIntSetting(L"widgetHeight"), 34, 60);
    g_settings.coverSize = ClampInt(Wh_GetIntSetting(L"coverSize"), 24, 48);
    g_settings.coverRadius = ClampInt(Wh_GetIntSetting(L"coverRadius"), 0, 18);
    g_settings.textWidth = ClampInt(Wh_GetIntSetting(L"textWidth"), 72, 260);
    g_settings.buttonSize = ClampInt(Wh_GetIntSetting(L"buttonSize"), 22, 42);
    g_settings.progressHeight = ClampInt(Wh_GetIntSetting(L"progressHeight"), 2, 8);
    g_settings.progressHoverHeight =
        ClampInt(Wh_GetIntSetting(L"progressHoverHeight"),
                 g_settings.progressHeight, 12);
    g_settings.followSystemTheme = Wh_GetIntSetting(L"followSystemTheme") != 0;
    g_settings.hideWhenNoMedia = Wh_GetIntSetting(L"hideWhenNoMedia") != 0;

    PCWSTR accentColor = Wh_GetStringSetting(L"accentColor");
    PCWSTR textColor = Wh_GetStringSetting(L"textColor");
    PCWSTR mutedTextColor = Wh_GetStringSetting(L"mutedTextColor");
    PCWSTR themeMode = Wh_GetStringSetting(L"themeMode");
    PCWSTR idleApps = Wh_GetStringSetting(L"idleApps");
    PCWSTR marqueeSpeed = Wh_GetStringSetting(L"marqueeSpeed");
    PCWSTR volumeStep = Wh_GetStringSetting(L"volumeStep");
    PCWSTR visualizerOpacity = Wh_GetStringSetting(L"visualizerOpacity");

    g_settings.accentColor = ParseColor(accentColor, RGB(46, 168, 255));
    g_settings.textColor = ParseColor(textColor, RGB(255, 255, 255));
    g_settings.mutedTextColor = ParseColor(mutedTextColor, RGB(185, 192, 204));
    g_settings.themeMode = ParseThemeMode(themeMode);
    g_settings.idleApps = idleApps ? idleApps : L"";
    g_settings.marqueeSpeed = marqueeSpeed ? _wtof(marqueeSpeed) : 0.55;
    g_settings.volumeStep = volumeStep ? _wtof(volumeStep) : 0.02;
    g_settings.visualizerEnabled = Wh_GetIntSetting(L"visualizerEnabled") != 0;
    g_settings.visualizerOpacity =
        visualizerOpacity && visualizerOpacity[0] ? _wtof(visualizerOpacity) : 0.32;
    g_settings.marqueeSpeed = ClampDouble(g_settings.marqueeSpeed, 0.1, 3.0);
    g_settings.volumeStep = ClampDouble(g_settings.volumeStep, 0.005, 0.1);
    g_settings.visualizerOpacity =
        ClampDouble(g_settings.visualizerOpacity, 0.12, 0.38);

    Wh_FreeStringSetting(accentColor);
    Wh_FreeStringSetting(textColor);
    Wh_FreeStringSetting(mutedTextColor);
    Wh_FreeStringSetting(themeMode);
    Wh_FreeStringSetting(idleApps);
    Wh_FreeStringSetting(marqueeSpeed);
    Wh_FreeStringSetting(volumeStep);
    Wh_FreeStringSetting(visualizerOpacity);
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows([](HWND hwnd, LPARAM param) CALLBACK -> BOOL {
        DWORD pid = 0;
        wchar_t className[32]{};
        if (GetWindowThreadProcessId(hwnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
            _wcsicmp(className, L"Shell_TrayWnd") == 0) {
            *reinterpret_cast<HWND*>(param) = hwnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

TaskbarEdge DetectTaskbarEdge(HWND taskbarWnd) {
    RECT taskbarRect{};
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!taskbarWnd || !GetWindowRect(taskbarWnd, &taskbarRect) ||
        !GetMonitorInfoW(MonitorFromWindow(taskbarWnd, MONITOR_DEFAULTTONEAREST),
                         &monitorInfo)) {
        return TaskbarEdge::Unknown;
    }

    int width = taskbarRect.right - taskbarRect.left;
    int height = taskbarRect.bottom - taskbarRect.top;
    if (height > width) {
        int leftDistance = std::abs(taskbarRect.left - monitorInfo.rcMonitor.left);
        int rightDistance =
            std::abs(monitorInfo.rcMonitor.right - taskbarRect.right);
        return leftDistance <= rightDistance ? TaskbarEdge::Left
                                             : TaskbarEdge::Right;
    }

    int topDistance = std::abs(taskbarRect.top - monitorInfo.rcMonitor.top);
    int bottomDistance =
        std::abs(monitorInfo.rcMonitor.bottom - taskbarRect.bottom);
    return topDistance < bottomDistance ? TaskbarEdge::Top
                                        : TaskbarEdge::Bottom;
}

int TaskbarThicknessPx(HWND taskbarWnd, TaskbarEdge edge) {
    RECT rect{};
    if (!taskbarWnd || !GetWindowRect(taskbarWnd, &rect)) {
        return 0;
    }
    return IsVerticalTaskbarEdge(edge) ? rect.right - rect.left
                                       : rect.bottom - rect.top;
}

bool IsCurrentProcessShellHost() {
    HWND shellWindow = GetShellWindow();
    if (!shellWindow) {
        // During early sign-in Explorer can load the mod before the desktop
        // window exists. In that case keep loading and let taskbar detection
        // finish asynchronously.
        return true;
    }

    DWORD shellProcessId = 0;
    GetWindowThreadProcessId(shellWindow, &shellProcessId);
    return shellProcessId == GetCurrentProcessId();
}

bool RunFromWindowThread(HWND hwnd, WindowThreadProc proc, void* param) {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_TaskbarMediaWidget_RunOnUiThread");
    struct Payload {
        WindowThreadProc proc;
        void* param;
    };

    DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) CALLBACK -> LRESULT {
            if (code == HC_ACTION) {
                auto cwp = reinterpret_cast<CWPSTRUCT const*>(lParam);
                static const UINT innerMessage =
                    RegisterWindowMessageW(L"Windhawk_TaskbarMediaWidget_RunOnUiThread");
                if (cwp->message == innerMessage) {
                    auto payload = reinterpret_cast<Payload*>(cwp->lParam);
                    payload->proc(payload->param);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    Payload payload{proc, param};
    SendMessageW(hwnd, message, 0, reinterpret_cast<LPARAM>(&payload));
    UnhookWindowsHookEx(hook);
    return true;
}

bool IsReadableMemoryRange(void const* address, size_t size) {
    if (!address || size == 0) {
        return false;
    }

    MEMORY_BASIC_INFORMATION memory{};
    if (!VirtualQuery(address, &memory, sizeof(memory)) ||
        memory.State != MEM_COMMIT ||
        (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }

    auto start = reinterpret_cast<uintptr_t>(address);
    auto regionStart = reinterpret_cast<uintptr_t>(memory.BaseAddress);
    auto regionEnd = regionStart + memory.RegionSize;
    return start >= regionStart && start <= regionEnd &&
           size <= regionEnd - start;
}

XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    if (!taskbarWnd || !CTaskBand_ITaskListWndSite_vftable ||
        !CTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original) {
        return nullptr;
    }

    HWND taskSwWnd =
        reinterpret_cast<HWND>(GetPropW(taskbarWnd, L"TaskbandHWND"));
    if (!taskSwWnd) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskSwWnd, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskListWndSite = taskBand;
    constexpr int kMaxSlotsToScan = 20;
    for (int i = 0;; i++) {
        if (!IsReadableMemoryRange(taskListWndSite, sizeof(void*))) {
            Wh_Log(L"GetTaskbarXamlRoot: unreadable CTaskBand slot %d", i);
            return nullptr;
        }
        if (*reinterpret_cast<void**>(taskListWndSite) ==
            CTaskBand_ITaskListWndSite_vftable) {
            break;
        }
        if (i == kMaxSlotsToScan) {
            Wh_Log(L"GetTaskbarXamlRoot: ITaskListWndSite not found");
            return nullptr;
        }
        taskListWndSite = reinterpret_cast<void**>(taskListWndSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskListWndSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0]) {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0;
    bool frameHeightPatternRecognized = false;
#if defined(_M_X64) || defined(__x86_64__)
    auto bytes =
        reinterpret_cast<BYTE const*>(TaskbarHost_FrameHeight_Original);
    if (IsReadableMemoryRange(bytes, 8) && bytes[0] == 0x48 &&
        bytes[1] == 0x83 && bytes[2] == 0xEC && bytes[4] == 0x48 &&
        bytes[5] == 0x83 && bytes[6] == 0xC1 && bytes[7] <= 0x7F) {
        taskbarElementIUnknownOffset = static_cast<size_t>(bytes[7]);
        frameHeightPatternRecognized = true;
    }
#endif

    BYTE* taskbarHost = static_cast<BYTE*>(taskbarHostSharedPtr[0]);
    if (!frameHeightPatternRecognized ||
        !IsReadableMemoryRange(taskbarHost + taskbarElementIUnknownOffset,
                               sizeof(::IUnknown*))) {
        Wh_Log(L"GetTaskbarXamlRoot: unsupported TaskbarHost layout");
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

    auto taskbarElementIUnknown =
        *reinterpret_cast<::IUnknown**>(taskbarHost +
                                        taskbarElementIUnknownOffset);
    if (!taskbarElementIUnknown) {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

    FrameworkElement taskbarElement{nullptr};
    HRESULT queryResult = taskbarElementIUnknown->QueryInterface(
        guid_of<FrameworkElement>(), put_abi(taskbarElement));
    auto result = SUCCEEDED(queryResult) && taskbarElement
                      ? taskbarElement.XamlRoot()
                      : nullptr;

    if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
        Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
    }
    return result;
}

FrameworkElement FindChildByName(FrameworkElement const& root,
                                 std::wstring_view name,
                                 int depth = 32) {
    if (!root || depth <= 0) {
        return nullptr;
    }

    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (child.Name() == name) {
            return child;
        }
        if (auto found = FindChildByName(child, name, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement const& root,
                                      wchar_t const* className,
                                      int depth = 32) {
    if (!root || depth <= 0) {
        return nullptr;
    }

    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (get_class_name(child) == className) {
            return child;
        }
        if (auto found = FindChildByClassName(child, className, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

FrameworkElement FindSystemTrayFramePanel(FrameworkElement const& root) {
    auto trayFrame = FindChildByClassName(root, L"SystemTray.SystemTrayFrame");
    auto trayPanel = trayFrame ? FindChildByName(trayFrame, L"SystemTrayFrameGrid")
                               : FindChildByName(root, L"SystemTrayFrameGrid");
    return trayPanel && trayPanel.try_as<Panel>() ? trayPanel : nullptr;
}

void ResetMarquee(TextScrollState& state, std::wstring const& text) {
    if (state.lastText != text) {
        state.lastText = text;
        state.offset = 0.0;
        state.forward = true;
    }
}

std::wstring FormatTime(int64_t milliseconds) {
    if (milliseconds < 0) {
        milliseconds = 0;
    }

    int64_t seconds = milliseconds / 1000;
    int64_t minutes = seconds / 60;
    seconds %= 60;

    wchar_t buffer[32]{};
    swprintf_s(buffer, L"%lld:%02lld", minutes, seconds);
    return buffer;
}

std::vector<uint8_t> ReadThumbnailBytes(
    IRandomAccessStreamReference const& thumbnail) {
    if (!thumbnail) {
        return {};
    }

    try {
        auto stream = thumbnail.OpenReadAsync().get();
        uint64_t size = stream.Size();
        if (size == 0 || size > 10 * 1024 * 1024) {
            return {};
        }

        Buffer buffer(static_cast<uint32_t>(size));
        auto readBuffer =
            stream.ReadAsync(buffer, static_cast<uint32_t>(size),
                             InputStreamOptions::None)
                .get();
        std::vector<uint8_t> bytes(readBuffer.Length());
        if (!bytes.empty()) {
            DataReader::FromBuffer(readBuffer).ReadBytes(bytes);
        }
        return bytes;
    } catch (hresult_error const& e) {
        Wh_Log(L"ReadThumbnailBytes failed: 0x%08X %s",
               static_cast<uint32_t>(e.code()), e.message().c_str());
    } catch (...) {
        Wh_Log(L"ReadThumbnailBytes failed");
    }
    return {};
}

COLORREF ExtractAccentColor(std::vector<uint8_t> const& imageBytes) {
    if (imageBytes.empty()) {
        return g_settings.accentColor;
    }

    try {
        IStream* rawStream =
            SHCreateMemStream(imageBytes.data(), static_cast<UINT>(imageBytes.size()));
        if (!rawStream) {
            return g_settings.accentColor;
        }

        com_ptr<IStream> stream;
        stream.attach(rawStream);

        com_ptr<IWICImagingFactory> factory;
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                    CLSCTX_INPROC_SERVER,
                                    IID_IWICImagingFactory,
                                    factory.put_void()))) {
            return g_settings.accentColor;
        }

        com_ptr<IWICBitmapDecoder> decoder;
        if (FAILED(factory->CreateDecoderFromStream(
                stream.get(), nullptr, WICDecodeMetadataCacheOnDemand,
                decoder.put()))) {
            return g_settings.accentColor;
        }

        com_ptr<IWICBitmapFrameDecode> frame;
        if (FAILED(decoder->GetFrame(0, frame.put()))) {
            return g_settings.accentColor;
        }

        com_ptr<IWICBitmapScaler> scaler;
        if (FAILED(factory->CreateBitmapScaler(scaler.put())) ||
            FAILED(scaler->Initialize(frame.get(), 16, 16,
                                      WICBitmapInterpolationModeFant))) {
            return g_settings.accentColor;
        }

        com_ptr<IWICFormatConverter> converter;
        if (FAILED(factory->CreateFormatConverter(converter.put())) ||
            FAILED(converter->Initialize(
                scaler.get(), GUID_WICPixelFormat32bppBGRA,
                WICBitmapDitherTypeNone, nullptr, 0.0,
                WICBitmapPaletteTypeCustom))) {
            return g_settings.accentColor;
        }

        std::vector<BYTE> pixels(16 * 16 * 4);
        if (FAILED(converter->CopyPixels(nullptr, 16 * 4,
                                         static_cast<UINT>(pixels.size()),
                                         pixels.data()))) {
            return g_settings.accentColor;
        }

        double red = 0.0;
        double green = 0.0;
        double blue = 0.0;
        double weightSum = 0.0;
        for (size_t i = 0; i + 3 < pixels.size(); i += 4) {
            BYTE b = pixels[i + 0];
            BYTE g = pixels[i + 1];
            BYTE r = pixels[i + 2];
            BYTE a = pixels[i + 3];
            if (a < 32) {
                continue;
            }

            BYTE maxChannel = std::max({r, g, b});
            BYTE minChannel = std::min({r, g, b});
            double saturation =
                maxChannel == 0 ? 0.0
                                : static_cast<double>(maxChannel - minChannel) /
                                      static_cast<double>(maxChannel);
            double weight = 0.45 + saturation;
            red += r * weight;
            green += g * weight;
            blue += b * weight;
            weightSum += weight;
        }

        if (weightSum <= 0.0) {
            return g_settings.accentColor;
        }

        int r = ClampInt(static_cast<int>(std::round(red / weightSum)), 35, 235);
        int g = ClampInt(static_cast<int>(std::round(green / weightSum)), 35, 235);
        int b = ClampInt(static_cast<int>(std::round(blue / weightSum)), 35, 235);
        return RGB(r, g, b);
    } catch (...) {
        return g_settings.accentColor;
    }
}

int64_t TimelineDurationMs(
    GlobalSystemMediaTransportControlsSessionTimelineProperties const& timeline) {
    auto duration = timeline.EndTime() - timeline.StartTime();
    return std::max<int64_t>(
        0, std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

int64_t TimelinePositionMs(
    GlobalSystemMediaTransportControlsSessionTimelineProperties const& timeline,
    bool isPlaying) {
    auto start = timeline.StartTime();
    auto end = timeline.EndTime();
    auto position = timeline.Position();

    if (timeline.LastUpdatedTime().time_since_epoch() !=
        DateTime::duration::zero()) {
        auto age = std::chrono::duration_cast<TimeSpan>(
            winrt::clock::now() - timeline.LastUpdatedTime());
        if (isPlaying && age > TimeSpan::zero()) {
            position += age;
        }
    }

    if (end > start) {
        position = std::clamp(position, start, end);
        position -= start;
    }

    return std::max<int64_t>(
        0, std::chrono::duration_cast<std::chrono::milliseconds>(position).count());
}

int64_t EffectivePosition(MediaState const& state) {
    int64_t position = state.positionMs;
    if (state.isPlaying) {
        position += static_cast<int64_t>(GetTickCount64() - state.updatedTick);
    }
    return state.durationMs > 0 ? std::clamp(position, int64_t{0}, state.durationMs)
                                : std::max<int64_t>(0, position);
}

TextBlock MakeIconText(PCWSTR glyph, double size) {
    TextBlock icon;
    icon.Text(glyph);
    icon.FontFamily(FontFamily(L"Segoe Fluent Icons"));
    icon.FontSize(size);
    icon.Foreground(MakeBrush(ActiveTextColor()));
    icon.HorizontalAlignment(HorizontalAlignment::Center);
    icon.VerticalAlignment(VerticalAlignment::Center);
    icon.UseLayoutRounding(true);
    return icon;
}

void UpdateButtonForeground(Border const& button,
                            TextBlock const& icon,
                            bool enabled) {
    if (!button || !icon) {
        return;
    }

    icon.Foreground(MakeBrush(ActiveTextColor()));
    button.Opacity(enabled ? 1.0 : 0.38);
}

void SetButtonHoverBackground(Border const& button, bool hovered) {
    if (!button) {
        return;
    }

    button.Background(hovered ? MakeBrush(ButtonHoverColor(), ButtonHoverAlpha())
                              : MakeBrush(RGB(0, 0, 0), 0));
}

void UpdateProgressHoverTipTheme() {
    if (!g_progressHoverTip || !g_progressToolTipText) {
        return;
    }

    if (UseLightTheme()) {
        g_progressHoverTip.Background(MakeBrush(RGB(255, 255, 255), 245));
        g_progressHoverTip.BorderBrush(MakeBrush(RGB(0, 0, 0), 24));
        g_progressToolTipText.Foreground(MakeBrush(RGB(24, 28, 36)));
    } else {
        g_progressHoverTip.Background(MakeBrush(RGB(20, 24, 30), 235));
        g_progressHoverTip.BorderBrush(MakeBrush(RGB(255, 255, 255), 22));
        g_progressToolTipText.Foreground(MakeBrush(RGB(255, 255, 255)));
    }
    g_progressHoverTip.BorderThickness({1, 1, 1, 1});
}

void SendMediaCommand(int command);

Border MakeButton(PCWSTR name,
                  PCWSTR glyph,
                  TextBlock& iconTarget,
                  bool* hoverState,
                  bool* enabledState,
                  int command) {
    Border button;
    button.Name(name);
    button.Width(g_layout.buttonSize);
    button.Height(g_layout.buttonSize);
    button.Margin({0, 0, 0, 0});
    double radius = static_cast<double>(g_layout.buttonSize) / 2.0;
    button.CornerRadius({radius, radius, radius, radius});
    button.Background(MakeBrush(RGB(0, 0, 0), 0));
    button.VerticalAlignment(VerticalAlignment::Center);
    button.HorizontalAlignment(HorizontalAlignment::Center);
    button.UseLayoutRounding(true);
    iconTarget = MakeIconText(glyph, 13.0);
    button.Child(iconTarget);
    button.PointerEntered([button, hoverState, enabledState](
                              IInspectable const&,
                              PointerRoutedEventArgs const&) mutable {
        if (enabledState && !*enabledState) {
            return;
        }
        if (hoverState) {
            *hoverState = true;
        }
        SetButtonHoverBackground(button, true);
    });
    button.PointerExited([button, hoverState](IInspectable const&,
                                              PointerRoutedEventArgs const&) mutable {
        if (hoverState) {
            *hoverState = false;
        }
        SetButtonHoverBackground(button, false);
    });
    button.PointerPressed([button, enabledState, command](IInspectable const&,
                                                         PointerRoutedEventArgs const& e) {
        if (!enabledState || !*enabledState) {
            return;
        }
        SetButtonHoverBackground(button, true);
        SendMediaCommand(command);
        e.Handled(true);
    });
    return button;
}

void LaunchIdleApp(std::wstring shortcutPath) {
    if (shortcutPath.empty()) {
        return;
    }

    std::thread([shortcutPath]() {
        HINSTANCE result = ShellExecuteW(nullptr, L"open", shortcutPath.c_str(),
                                         nullptr, nullptr, SW_SHOWNORMAL);
        if (reinterpret_cast<intptr_t>(result) <= 32) {
            Wh_Log(L"LaunchIdleApp failed: path=%s code=%Id",
                   shortcutPath.c_str(), reinterpret_cast<intptr_t>(result));
        }
    }).detach();
}

Button MakeIdleButton(IdleApp const& app) {
    Button button;
    button.Width(g_layout.buttonSize);
    button.Height(g_layout.buttonSize);
    button.Padding({0, 0, 0, 0});
    button.Margin({0, 0, 0, 0});
    button.BorderThickness({0, 0, 0, 0});
    button.CornerRadius({7, 7, 7, 7});
    button.Background(MakeBrush(RGB(0, 0, 0), 0));
    button.Foreground(MakeBrush(ActiveTextColor()));

    if (!app.iconBytes.empty()) {
        Image image;
        image.Width(std::max(16, g_layout.buttonSize - 8));
        image.Height(std::max(16, g_layout.buttonSize - 8));
        image.Stretch(Stretch::Uniform);

        IStream* rawStream = SHCreateMemStream(
            app.iconBytes.data(), static_cast<UINT>(app.iconBytes.size()));
        if (rawStream) {
            com_ptr<IStream> comStream;
            comStream.attach(rawStream);

            IRandomAccessStream randomAccessStream{nullptr};
            if (SUCCEEDED(::CreateRandomAccessStreamOverStream(
                    comStream.get(), BSOS_DEFAULT, guid_of<IRandomAccessStream>(),
                    put_abi(randomAccessStream)))) {
                BitmapImage bitmap;
                bitmap.DecodePixelWidth(std::max(32, g_layout.buttonSize * 2));
                image.Source(bitmap);
                bitmap.SetSourceAsync(randomAccessStream);
            }
        }

        button.Content(image);
    } else {
        TextBlock icon;
        std::wstring letter = app.name.empty() ? L"♪" : app.name.substr(0, 1);
        icon.Text(letter);
        icon.FontFamily(FontFamily(L"Segoe UI"));
        icon.FontSize(13.0);
        icon.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        icon.HorizontalAlignment(HorizontalAlignment::Center);
        icon.VerticalAlignment(VerticalAlignment::Center);
        button.Content(icon);
    }

    ToolTipService::SetToolTip(button, box_value(app.name));
    button.PointerEntered([button](IInspectable const&,
                                   PointerRoutedEventArgs const&) mutable {
        button.Background(MakeBrush(ButtonHoverColor(), UseLightTheme() ? 28 : 42));
    });
    button.PointerExited([button](IInspectable const&,
                                  PointerRoutedEventArgs const&) mutable {
        button.Background(MakeBrush(RGB(0, 0, 0), 0));
    });

    std::wstring shortcut = app.shortcutPath;
    button.Click([shortcut](IInspectable const&, RoutedEventArgs const&) {
        LaunchIdleApp(shortcut);
    });
    return button;
}

Canvas MakeMarqueeLine(TextBlock& textBlock,
                       TranslateTransform& transform,
                       PCWSTR name,
                       double fontSize,
                       COLORREF color,
                       bool bold) {
    Canvas canvas;
    canvas.Name(name);
    canvas.Height(fontSize + 3.0);
    canvas.Width(g_layout.textWidth);
    canvas.Clip(RectangleGeometry{});

    transform = TranslateTransform();
    textBlock = TextBlock();
    textBlock.RenderTransform(transform);
    textBlock.FontFamily(FontFamily(L"Segoe UI"));
    textBlock.FontSize(fontSize);
    textBlock.FontWeight(bold ? winrt::Windows::UI::Text::FontWeights::SemiBold()
                              : winrt::Windows::UI::Text::FontWeights::Normal());
    textBlock.TextWrapping(TextWrapping::NoWrap);
    textBlock.TextTrimming(TextTrimming::None);
    textBlock.Foreground(MakeBrush(color));
    Canvas::SetLeft(textBlock, 0);
    Canvas::SetTop(textBlock, 0);

    canvas.Children().Append(textBlock);
    canvas.SizeChanged([canvas](IInspectable const&, SizeChangedEventArgs const&) {
        if (auto geometry = canvas.Clip().try_as<RectangleGeometry>()) {
            geometry.Rect({0, 0, static_cast<float>(canvas.ActualWidth()),
                           static_cast<float>(canvas.ActualHeight())});
        }
    });

    return canvas;
}

void SendMediaCommand(int command) {
    std::thread([command]() {
        ApartmentScope apartment;
        GlobalSystemMediaTransportControlsSession session{nullptr};
        {
            std::lock_guard lock(g_mediaMutex);
            session = g_mediaSession;
        }
        if (!session) {
            return;
        }

        try {
            if (command == 1) {
                session.TrySkipPreviousAsync().get();
            } else if (command == 2) {
                auto controls = session.GetPlaybackInfo().Controls();
                if (controls.IsPlayPauseToggleEnabled()) {
                    session.TryTogglePlayPauseAsync().get();
                } else if (controls.IsPlayEnabled()) {
                    session.TryPlayAsync().get();
                }
            } else if (command == 3) {
                session.TrySkipNextAsync().get();
            }
            g_uiDirty = true;
        } catch (hresult_error const& e) {
            Wh_Log(L"SendMediaCommand failed: 0x%08X %s",
                   static_cast<uint32_t>(e.code()), e.message().c_str());
        } catch (...) {
            Wh_Log(L"SendMediaCommand failed");
        }
    }).detach();
}

bool AdjustAppVolumeByAumid(std::wstring const& aumid, float delta) {
    if (aumid.empty()) {
        return false;
    }

    com_ptr<IMMDeviceEnumerator> deviceEnumerator;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  __uuidof(IMMDeviceEnumerator),
                                  deviceEnumerator.put_void());
    if (FAILED(hr)) {
        return false;
    }

    com_ptr<IMMDevice> defaultDevice;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                   defaultDevice.put());
    if (FAILED(hr)) {
        return false;
    }

    com_ptr<IAudioSessionManager2> sessionManager;
    hr = defaultDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                                 nullptr, sessionManager.put_void());
    if (FAILED(hr)) {
        return false;
    }

    com_ptr<IAudioSessionEnumerator> sessionEnumerator;
    hr = sessionManager->GetSessionEnumerator(sessionEnumerator.put());
    if (FAILED(hr)) {
        return false;
    }

    int count = 0;
    if (FAILED(sessionEnumerator->GetCount(&count))) {
        return false;
    }

    bool changed = false;
    for (int i = 0; i < count; i++) {
        com_ptr<IAudioSessionControl> sessionControl;
        if (FAILED(sessionEnumerator->GetSession(i, sessionControl.put()))) {
            continue;
        }

        com_ptr<IAudioSessionControl2> sessionControl2;
        if (FAILED(sessionControl->QueryInterface(__uuidof(IAudioSessionControl2),
                                                  sessionControl2.put_void()))) {
            continue;
        }

        LPWSTR sessionId = nullptr;
        if (SUCCEEDED(sessionControl2->GetSessionIdentifier(&sessionId)) &&
            sessionId) {
            std::wstring id(sessionId);
            CoTaskMemFree(sessionId);
            if (id.find(aumid) == std::wstring::npos) {
                continue;
            }

            com_ptr<ISimpleAudioVolume> volume;
            if (SUCCEEDED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                         volume.put_void()))) {
                float current = 0.0f;
                if (SUCCEEDED(volume->GetMasterVolume(&current))) {
                    volume->SetMasterVolume(
                        ClampDouble(current + delta, 0.0, 1.0), nullptr);
                    changed = true;
                }
            }
        }
    }

    return changed;
}

void ChangeSystemVolume(bool increase) {
    HWND shellTrayWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!shellTrayWnd) {
        return;
    }
    SHORT command = increase ? APPCOMMAND_VOLUME_UP : APPCOMMAND_VOLUME_DOWN;
    PostMessageW(shellTrayWnd, WM_APPCOMMAND,
                 reinterpret_cast<WPARAM>(shellTrayWnd),
                 MAKELPARAM(0, command));
}

void ChangeVolumeFromWheel(int wheelDelta) {
    if (wheelDelta == 0) {
        return;
    }

    std::wstring sourceAppId;
    {
        std::lock_guard lock(g_mediaMutex);
        sourceAppId = g_mediaState.sourceAppId;
    }

    float delta = static_cast<float>(wheelDelta > 0 ? g_settings.volumeStep
                                                    : -g_settings.volumeStep);
    std::thread([sourceAppId, delta, wheelDelta]() {
        ApartmentScope apartment;
        if (!AdjustAppVolumeByAumid(sourceAppId, delta)) {
            ChangeSystemVolume(wheelDelta > 0);
        }
    }).detach();
}

bool AudioFormatIsFloat(WAVEFORMATEX const* format) {
    static const GUID kIeeeFloat = {0x00000003,
                                    0x0000,
                                    0x0010,
                                    {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38,
                                     0x9B, 0x71}};

    if (!format) {
        return false;
    }
    if (format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
        return true;
    }
    if (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
        format->cbSize >= sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)) {
        auto extensible = reinterpret_cast<WAVEFORMATEXTENSIBLE const*>(format);
        return IsEqualGUID(extensible->SubFormat, kIeeeFloat);
    }
    return false;
}

float ReadAudioSample(BYTE const* data,
                      WAVEFORMATEX const* format,
                      UINT32 frame,
                      WORD channel) {
    int bytesPerSample = std::max<WORD>(1, format->wBitsPerSample / 8);
    BYTE const* sample = data + frame * format->nBlockAlign +
                         channel * bytesPerSample;

    if (AudioFormatIsFloat(format) && bytesPerSample == 4) {
        float value = 0.0f;
        std::memcpy(&value, sample, sizeof(value));
        return ClampDouble(value, -1.0, 1.0);
    }

    if (format->wBitsPerSample == 16) {
        int16_t value = 0;
        std::memcpy(&value, sample, sizeof(value));
        return static_cast<float>(value / 32768.0);
    }
    if (format->wBitsPerSample == 24) {
        int32_t value =
            static_cast<int32_t>(sample[0]) |
            (static_cast<int32_t>(sample[1]) << 8) |
            (static_cast<int32_t>(sample[2]) << 16);
        if (value & 0x800000) {
            value |= ~0xFFFFFF;
        }
        return static_cast<float>(value / 8388608.0);
    }
    if (format->wBitsPerSample == 32) {
        int32_t value = 0;
        std::memcpy(&value, sample, sizeof(value));
        return static_cast<float>(value / 2147483648.0);
    }

    return 0.0f;
}

std::array<float, kVisualizerMaxBarCount> AnalyzeAudioPacket(
    BYTE const* data,
    UINT32 frames,
    DWORD flags,
    WAVEFORMATEX const* format) {
    std::array<float, kVisualizerMaxBarCount> bands{};
    if (!data || !format || frames == 0 ||
        (flags & AUDCLNT_BUFFERFLAGS_SILENT) != 0) {
        return bands;
    }

    static constexpr double kPi = 3.14159265358979323846;

    int channels = std::max<WORD>(1, format->nChannels);
    double sampleRate = std::max<DWORD>(1, format->nSamplesPerSec);
    UINT32 sampleCount = std::min<UINT32>(frames, 512);
    UINT32 step = std::max<UINT32>(1, frames / sampleCount);
    double minFrequency = 55.0;
    double maxFrequency = std::min(7600.0, sampleRate * 0.45);
    double frequencyRatio = maxFrequency / minFrequency;

    for (int band = 0; band < kVisualizerMaxBarCount; band++) {
        double t = static_cast<double>(band) /
                   static_cast<double>(kVisualizerMaxBarCount - 1);
        double frequency = minFrequency * std::pow(frequencyRatio, t);
        double coeff = 2.0 * std::cos(2.0 * kPi * frequency / sampleRate);
        double previous = 0.0;
        double previous2 = 0.0;

        for (UINT32 i = 0; i < sampleCount; i++) {
            UINT32 frame = std::min(frames - 1, i * step);
            double mono = 0.0;
            for (int channel = 0; channel < channels; channel++) {
                mono += ReadAudioSample(data, format, frame,
                                        static_cast<WORD>(channel));
            }
            mono /= channels;

            double current = mono + coeff * previous - previous2;
            previous2 = previous;
            previous = current;
        }

        double power = previous2 * previous2 + previous * previous -
                       coeff * previous * previous2;
        double magnitude = std::sqrt(std::max(0.0, power)) / sampleCount;
        double normalized = std::pow(ClampDouble(magnitude * 140.0, 0.0, 1.0),
                                     0.55);
        bands[band] = static_cast<float>(normalized);
    }

    return bands;
}

void StoreVisualizerBands(std::array<float, kVisualizerMaxBarCount> const& bands) {
    std::lock_guard lock(g_visualizerMutex);
    g_visualizerBands = bands;
    g_visualizerUpdatedTick = GetTickCount64();
}

void DecayVisualizerBands(float amount) {
    std::lock_guard lock(g_visualizerMutex);
    for (float& band : g_visualizerBands) {
        band *= amount;
        if (band < 0.015f) {
            band = 0.0f;
        }
    }
    g_visualizerUpdatedTick = GetTickCount64();
}

bool RunAudioCaptureSession() {
    com_ptr<IMMDeviceEnumerator> deviceEnumerator;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  __uuidof(IMMDeviceEnumerator),
                                  deviceEnumerator.put_void());
    if (FAILED(hr)) {
        return false;
    }

    com_ptr<IMMDevice> device;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                   device.put());
    if (FAILED(hr)) {
        return false;
    }

    com_ptr<IAudioClient> audioClient;
    hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                          audioClient.put_void());
    if (FAILED(hr)) {
        return false;
    }

    WAVEFORMATEX* mixFormat = nullptr;
    hr = audioClient->GetMixFormat(&mixFormat);
    if (FAILED(hr) || !mixFormat) {
        return false;
    }

    struct FormatGuard {
        WAVEFORMATEX* value = nullptr;
        ~FormatGuard() {
            if (value) {
                CoTaskMemFree(value);
            }
        }
    } formatGuard{mixFormat};

    hr = audioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK,
        1000000, 0, mixFormat, nullptr);
    if (FAILED(hr)) {
        return false;
    }

    com_ptr<IAudioCaptureClient> captureClient;
    hr = audioClient->GetService(__uuidof(IAudioCaptureClient),
                                 captureClient.put_void());
    if (FAILED(hr)) {
        return false;
    }

    hr = audioClient->Start();
    if (FAILED(hr)) {
        return false;
    }

    std::array<float, kVisualizerMaxBarCount> smoothed{};
    ULONGLONG lastPacketTick = GetTickCount64();

    while (!g_unloading && !g_workersStopping &&
           g_settings.visualizerEnabled) {
        UINT32 packetFrames = 0;
        hr = captureClient->GetNextPacketSize(&packetFrames);
        if (FAILED(hr)) {
            break;
        }

        bool hadPacket = false;
        while (packetFrames > 0 && !g_unloading) {
            BYTE* data = nullptr;
            UINT32 frames = 0;
            DWORD flags = 0;
            hr = captureClient->GetBuffer(&data, &frames, &flags, nullptr,
                                          nullptr);
            if (FAILED(hr)) {
                break;
            }

            auto bands = AnalyzeAudioPacket(data, frames, flags, mixFormat);
            for (int i = 0; i < kVisualizerMaxBarCount; i++) {
                float target = bands[i];
                float speed = target > smoothed[i] ? 0.45f : 0.18f;
                smoothed[i] += (target - smoothed[i]) * speed;
            }
            StoreVisualizerBands(smoothed);

            captureClient->ReleaseBuffer(frames);
            hadPacket = true;
            lastPacketTick = GetTickCount64();

            hr = captureClient->GetNextPacketSize(&packetFrames);
            if (FAILED(hr)) {
                break;
            }
        }
        if (FAILED(hr)) {
            break;
        }

        if (!hadPacket && GetTickCount64() - lastPacketTick > 180) {
            for (float& band : smoothed) {
                band *= 0.82f;
            }
            StoreVisualizerBands(smoothed);
        }

        Sleep(16);
    }

    audioClient->Stop();
    return true;
}

void AudioVisualizerThread() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    while (!g_unloading && !g_workersStopping) {
        if (!g_settings.visualizerEnabled) {
            DecayVisualizerBands(0.75f);
            Sleep(120);
            continue;
        }

        if (!RunAudioCaptureSession()) {
            DecayVisualizerBands(0.55f);
            Sleep(1000);
        }
    }
    DecayVisualizerBands(0.0f);
    CoUninitialize();
}

Grid BuildVerticalWidgetGrid(double availableWidth) {
    ResetWidgetElementReferences();
    g_verticalWidgetLayout = true;

    int width = ClampInt(
        availableWidth > 0 ? static_cast<int>(std::round(availableWidth)) : 40,
        32, 80);
    g_verticalWidgetWidth = width;
    g_layout = CalculateLayout(g_settings.widgetHeight);
    g_layout.coverSize =
        ClampInt(std::min(g_settings.coverSize, width - 8), 18, 34);
    g_layout.coverRadius =
        ClampInt(std::min(g_settings.coverRadius, g_layout.coverSize / 3), 0, 14);
    g_layout.buttonSize =
        ClampInt(std::min(g_settings.buttonSize, width - 4), 22, 30);
    g_layout.progressHeight = ClampInt(g_settings.progressHeight, 2, 5);
    g_layout.progressHoverHeight =
        ClampInt(g_settings.progressHoverHeight, g_layout.progressHeight, 7);
    g_layout.timeFont = 9.0;
    g_verticalSessionHeight =
        8.0 + g_layout.coverSize + 4.0 + g_layout.buttonSize * 3.0 + 4.0 +
        14.0 + g_layout.progressHoverHeight;
    g_layout.widgetHeight = static_cast<int>(std::round(g_verticalSessionHeight));

    Grid wrapper;
    wrapper.Name(L"TaskbarMediaWidget");
    wrapper.Width(g_verticalWidgetWidth);
    wrapper.Height(g_verticalSessionHeight);
    wrapper.Background(MakeBrush(RGB(0, 0, 0), 1));
    wrapper.HorizontalAlignment(HorizontalAlignment::Center);
    wrapper.VerticalAlignment(VerticalAlignment::Center);

    RowDefinition contentRow;
    contentRow.Height({1.0, GridUnitType::Star});
    g_progressRow = RowDefinition();
    g_progressRow.Height(
        {static_cast<double>(g_layout.progressHoverHeight), GridUnitType::Pixel});
    wrapper.RowDefinitions().Append(contentRow);
    wrapper.RowDefinitions().Append(g_progressRow);

    StackPanel sessionPanel;
    sessionPanel.Name(L"TaskbarMediaWidget_VerticalSession");
    sessionPanel.Orientation(Orientation::Vertical);
    sessionPanel.Spacing(1);
    sessionPanel.HorizontalAlignment(HorizontalAlignment::Center);
    sessionPanel.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetRow(sessionPanel, 0);
    wrapper.Children().Append(sessionPanel);
    g_rightContainer = sessionPanel;

    Border coverClip;
    coverClip.Width(g_layout.coverSize);
    coverClip.Height(g_layout.coverSize);
    coverClip.CornerRadius({static_cast<double>(g_layout.coverRadius),
                            static_cast<double>(g_layout.coverRadius),
                            static_cast<double>(g_layout.coverRadius),
                            static_cast<double>(g_layout.coverRadius)});
    coverClip.Background(MakeBrush(RGB(46, 62, 82), 120));
    coverClip.HorizontalAlignment(HorizontalAlignment::Center);
    coverClip.Margin({0, 1, 0, 2});
    ToolTipService::SetToolTip(coverClip, box_value(L"Open media app"));
    coverClip.PointerEntered([](IInspectable const&, PointerRoutedEventArgs const&) {
        SetPointerCursor(winrt::Windows::UI::Core::CoreCursorType::Hand);
    });
    coverClip.PointerExited([](IInspectable const&, PointerRoutedEventArgs const&) {
        SetPointerCursor(winrt::Windows::UI::Core::CoreCursorType::Arrow);
    });
    coverClip.PointerPressed([](IInspectable const&, PointerRoutedEventArgs const& e) {
        OpenCurrentMediaApp();
        e.Handled(true);
    });

    g_coverImage = Image();
    g_coverImage.Stretch(Stretch::UniformToFill);
    coverClip.Child(g_coverImage);
    sessionPanel.Children().Append(coverClip);
    g_coverContainer = coverClip;

    g_prevButton = MakeButton(L"TaskbarMediaWidget_Previous", L"\xE892",
                              g_prevButtonIcon, &g_prevButtonHover,
                              &g_prevButtonEnabled, 1);
    g_playButton = MakeButton(L"TaskbarMediaWidget_PlayPause", L"\xE768",
                              g_playButtonIcon, &g_playButtonHover,
                              &g_playButtonEnabled, 2);
    g_nextButton = MakeButton(L"TaskbarMediaWidget_Next", L"\xE893",
                              g_nextButtonIcon, &g_nextButtonHover,
                              &g_nextButtonEnabled, 3);
    sessionPanel.Children().Append(g_prevButton);
    sessionPanel.Children().Append(g_playButton);
    sessionPanel.Children().Append(g_nextButton);

    g_timeText = TextBlock();
    g_timeText.FontSize(g_layout.timeFont);
    g_timeText.FontFamily(FontFamily(L"Segoe UI"));
    g_timeText.Foreground(MakeBrush(ActiveMutedTextColor()));
    g_timeText.TextAlignment(TextAlignment::Center);
    g_timeText.HorizontalAlignment(HorizontalAlignment::Stretch);
    g_timeText.Margin({0, 1, 0, 0});
    sessionPanel.Children().Append(g_timeText);

    g_idlePanel = StackPanel();
    g_idlePanel.Orientation(Orientation::Vertical);
    g_idlePanel.Spacing(4);
    g_idlePanel.HorizontalAlignment(HorizontalAlignment::Center);
    g_idlePanel.VerticalAlignment(VerticalAlignment::Center);
    g_idlePanel.Margin({0, 4, 0, 4});
    g_idlePanel.Visibility(Visibility::Collapsed);
    for (size_t i = 0; i < g_idleApps.size() && i < 6; i++) {
        g_idlePanel.Children().Append(MakeIdleButton(g_idleApps[i]));
    }
    Grid::SetRow(g_idlePanel, 0);
    wrapper.Children().Append(g_idlePanel);

    g_progressGrid = Grid();
    g_progressGrid.Height(g_layout.progressHoverHeight);
    g_progressGrid.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetRow(g_progressGrid, 1);
    wrapper.Children().Append(g_progressGrid);

    Canvas progressOverlay;
    progressOverlay.IsHitTestVisible(false);
    Canvas::SetZIndex(progressOverlay, 20);
    Grid::SetRowSpan(progressOverlay, 2);
    wrapper.Children().Append(progressOverlay);

    g_progressHoverTip = Border();
    g_progressHoverTip.Width(std::max(30.0, g_verticalWidgetWidth - 2.0));
    g_progressHoverTip.Height(17);
    g_progressHoverTip.CornerRadius({4, 4, 4, 4});
    g_progressHoverTip.Visibility(Visibility::Collapsed);

    g_progressToolTipText = TextBlock();
    g_progressToolTipText.FontFamily(FontFamily(L"Segoe UI"));
    g_progressToolTipText.FontSize(9.0);
    g_progressToolTipText.Text(L"0:00");
    g_progressToolTipText.HorizontalAlignment(HorizontalAlignment::Center);
    g_progressToolTipText.VerticalAlignment(VerticalAlignment::Center);
    g_progressToolTipText.TextAlignment(TextAlignment::Center);
    g_progressHoverTip.Child(g_progressToolTipText);
    UpdateProgressHoverTipTheme();
    progressOverlay.Children().Append(g_progressHoverTip);

    g_progressTrack = Border();
    g_progressTrack.Height(g_layout.progressHeight);
    g_progressTrack.CornerRadius({4, 4, 4, 4});
    g_progressTrack.Background(MakeBrush(UseLightTheme() ? RGB(132, 142, 156)
                                                        : RGB(72, 82, 99),
                                         120));
    g_progressTrack.HorizontalAlignment(HorizontalAlignment::Stretch);
    g_progressTrack.VerticalAlignment(VerticalAlignment::Center);
    g_progressGrid.Children().Append(g_progressTrack);

    g_progressFill = Border();
    g_progressFill.Height(g_layout.progressHeight);
    g_progressFill.CornerRadius({4, 4, 4, 4});
    g_progressFill.Background(MakeBrush(g_settings.accentColor));
    g_progressFill.HorizontalAlignment(HorizontalAlignment::Left);
    g_progressFill.VerticalAlignment(VerticalAlignment::Center);
    g_progressGrid.Children().Append(g_progressFill);

    g_progressThumb = winrt::Windows::UI::Xaml::Shapes::Ellipse();
    g_progressThumb.Width(std::max(5, g_layout.progressHoverHeight));
    g_progressThumb.Height(std::max(5, g_layout.progressHoverHeight));
    g_progressThumb.Fill(MakeBrush(ProgressThumbColor(g_settings.accentColor)));
    g_progressThumb.Visibility(Visibility::Collapsed);
    g_progressThumb.HorizontalAlignment(HorizontalAlignment::Left);
    g_progressThumb.VerticalAlignment(VerticalAlignment::Center);
    g_progressGrid.Children().Append(g_progressThumb);

    auto seekAtPointer = [](PointerRoutedEventArgs const& e) {
        MediaState state;
        {
            std::lock_guard lock(g_mediaMutex);
            state = g_mediaState;
        }
        if (!state.canSeek || state.durationMs <= 0 || !g_progressTrack) {
            return;
        }

        auto point = e.GetCurrentPoint(g_progressTrack).Position();
        double width = std::max(1.0, g_progressTrack.ActualWidth());
        double ratio = ClampDouble(point.X / width, 0.0, 1.0);
        int64_t targetMs = static_cast<int64_t>(state.durationMs * ratio);
        int64_t absoluteTargetMs = state.timelineStartMs + targetMs;
        std::thread([absoluteTargetMs]() {
            ApartmentScope apartment;
            GlobalSystemMediaTransportControlsSession session{nullptr};
            {
                std::lock_guard lock(g_mediaMutex);
                session = g_mediaSession;
            }
            if (session) {
                try {
                    session.TryChangePlaybackPositionAsync(
                        absoluteTargetMs * 10000)
                        .get();
                    g_uiDirty = true;
                } catch (...) {
                }
            }
        }).detach();
    };

    g_progressGrid.PointerEntered([](IInspectable const&, PointerRoutedEventArgs const&) {
        g_progressHover = true;
        if (g_progressTrack) {
            g_progressTrack.Height(g_layout.progressHoverHeight);
        }
        if (g_progressFill) {
            g_progressFill.Height(g_layout.progressHoverHeight);
        }
        if (g_progressThumb) {
            g_progressThumb.Visibility(Visibility::Visible);
        }
        if (g_progressHoverTip) {
            g_progressHoverTip.Visibility(Visibility::Visible);
        }
    });
    g_progressGrid.PointerExited([](IInspectable const&, PointerRoutedEventArgs const&) {
        g_progressHover = false;
        if (g_progressTrack) {
            g_progressTrack.Height(g_layout.progressHeight);
        }
        if (g_progressFill) {
            g_progressFill.Height(g_layout.progressHeight);
        }
        if (g_progressThumb) {
            g_progressThumb.Visibility(Visibility::Collapsed);
        }
        if (g_progressHoverTip) {
            g_progressHoverTip.Visibility(Visibility::Collapsed);
        }
    });
    g_progressGrid.PointerMoved([](IInspectable const&, PointerRoutedEventArgs const& e) {
        if (!g_progressThumb || !g_progressHover) {
            return;
        }
        auto point = e.GetCurrentPoint(g_progressGrid).Position();
        double width = std::max(1.0, g_progressGrid.ActualWidth());
        double x = ClampDouble(point.X, 0.0, width);
        g_progressThumb.Margin({x - g_progressThumb.Width() / 2.0, 0, 0, 0});

        if (g_progressHoverTip && g_progressToolTipText) {
            MediaState state;
            {
                std::lock_guard lock(g_mediaMutex);
                state = g_mediaState;
            }
            if (state.durationMs > 0) {
                int64_t targetMs =
                    static_cast<int64_t>(state.durationMs * (x / width));
                g_progressToolTipText.Text(FormatTime(targetMs));
            }

            double tipWidth = g_progressHoverTip.Width();
            double tipHeight = g_progressHoverTip.Height();
            double left = ClampDouble(x - tipWidth / 2.0, 0.0,
                                      std::max(0.0, width - tipWidth));
            double top = std::max(0.0, g_verticalSessionHeight -
                                           g_layout.progressHoverHeight -
                                           tipHeight - 1.0);
            Canvas::SetLeft(g_progressHoverTip, left);
            Canvas::SetTop(g_progressHoverTip, top);
        }
    });
    g_progressGrid.PointerPressed([seekAtPointer](IInspectable const&,
                                                  PointerRoutedEventArgs const& e) {
        seekAtPointer(e);
        e.Handled(true);
    });

    wrapper.PointerWheelChanged([](IInspectable const&, PointerRoutedEventArgs const& e) {
        ChangeVolumeFromWheel(e.GetCurrentPoint(nullptr).Properties().MouseWheelDelta());
        e.Handled(true);
    });

    return wrapper;
}

Grid BuildWidgetGrid(double availableHeight) {
    ResetWidgetElementReferences();
    g_verticalWidgetLayout = false;
    g_layout = CalculateLayout(availableHeight);

    Grid wrapper;
    wrapper.Name(L"TaskbarMediaWidget");
    wrapper.Width(g_settings.widgetWidth);
    wrapper.Height(g_layout.widgetHeight);
    wrapper.MinHeight(g_layout.widgetHeight);
    wrapper.Background(MakeBrush(RGB(0, 0, 0), 1));
    wrapper.VerticalAlignment(VerticalAlignment::Center);
    RowDefinition contentRow;
    contentRow.Height({1.0, GridUnitType::Star});
    g_progressRow = RowDefinition();
    g_progressRow.Height(
        {static_cast<double>(g_layout.progressHoverHeight),
         GridUnitType::Pixel});
    wrapper.RowDefinitions().Append(contentRow);
    wrapper.RowDefinitions().Append(g_progressRow);

    g_visualizerBars.clear();
    g_visualizerBarCount =
        VisualizerBarCountForWidth(static_cast<double>(g_settings.widgetWidth));
    g_visualizerGrid = Grid();
    g_visualizerGrid.IsHitTestVisible(false);
    g_visualizerGrid.VerticalAlignment(VerticalAlignment::Stretch);
    g_visualizerGrid.HorizontalAlignment(HorizontalAlignment::Stretch);
    g_visualizerGrid.Margin({4, 3, 4, 3});
    g_visualizerGrid.Visibility(Visibility::Collapsed);
    Grid::SetRow(g_visualizerGrid, 0);
    Canvas::SetZIndex(g_visualizerGrid, 0);

    for (int i = 0; i < g_visualizerBarCount; i++) {
        ColumnDefinition column;
        column.Width({1.0, GridUnitType::Star});
        g_visualizerGrid.ColumnDefinitions().Append(column);

        Border bar;
        bar.Width(kVisualizerBarWidth);
        bar.Height(1);
        bar.CornerRadius({2, 2, 2, 2});
        bar.Margin({static_cast<double>(kVisualizerBarGap) / 2.0, 0,
                    static_cast<double>(kVisualizerBarGap) / 2.0, 0});
        bar.Background(MakeBrush(g_settings.accentColor, 51));
        bar.VerticalAlignment(VerticalAlignment::Bottom);
        bar.HorizontalAlignment(HorizontalAlignment::Center);
        Grid::SetColumn(bar, i);
        g_visualizerGrid.Children().Append(bar);
        g_visualizerBars.push_back(bar);
    }
    wrapper.Children().Append(g_visualizerGrid);

    Grid content;
    content.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetRow(content, 0);
    Canvas::SetZIndex(content, 10);
    wrapper.Children().Append(content);

    ColumnDefinition coverCol;
    coverCol.Width({g_layout.coverSize + 6.0, GridUnitType::Pixel});
    ColumnDefinition textCol;
    textCol.Width({static_cast<double>(g_layout.textWidth), GridUnitType::Pixel});
    ColumnDefinition rightCol;
    rightCol.Width({static_cast<double>(g_layout.rightWidth), GridUnitType::Pixel});
    content.ColumnDefinitions().Append(coverCol);
    content.ColumnDefinitions().Append(textCol);
    content.ColumnDefinitions().Append(rightCol);

    Border coverClip;
    coverClip.Width(g_layout.coverSize);
    coverClip.Height(g_layout.coverSize);
    coverClip.CornerRadius({static_cast<double>(g_layout.coverRadius),
                            static_cast<double>(g_layout.coverRadius),
                            static_cast<double>(g_layout.coverRadius),
                            static_cast<double>(g_layout.coverRadius)});
    coverClip.Background(MakeBrush(RGB(46, 62, 82), 120));
    coverClip.VerticalAlignment(VerticalAlignment::Center);
    coverClip.HorizontalAlignment(HorizontalAlignment::Center);
    coverClip.Margin({0, 3, 0, 0});
    coverClip.PointerEntered([](IInspectable const&, PointerRoutedEventArgs const&) {
        SetPointerCursor(winrt::Windows::UI::Core::CoreCursorType::Hand);
    });
    coverClip.PointerExited([](IInspectable const&, PointerRoutedEventArgs const&) {
        SetPointerCursor(winrt::Windows::UI::Core::CoreCursorType::Arrow);
    });
    coverClip.PointerPressed([](IInspectable const&, PointerRoutedEventArgs const& e) {
        OpenCurrentMediaApp();
        e.Handled(true);
    });

    g_coverImage = Image();
    g_coverImage.Stretch(Stretch::UniformToFill);
    coverClip.Child(g_coverImage);
    Grid::SetColumn(coverClip, 0);
    content.Children().Append(coverClip);
    g_coverContainer = coverClip;

    StackPanel textStack;
    textStack.Orientation(Orientation::Vertical);
    textStack.VerticalAlignment(VerticalAlignment::Center);
    textStack.Width(g_layout.textWidth);
    textStack.Spacing(-1);
    g_titleCanvas = MakeMarqueeLine(g_titleText, g_titleTransform,
                                    L"TaskbarMediaWidget_TitleCanvas",
                                    g_layout.titleFont,
                                    ActiveTextColor(), true);
    g_artistCanvas = MakeMarqueeLine(g_artistText, g_artistTransform,
                                     L"TaskbarMediaWidget_ArtistCanvas",
                                     g_layout.artistFont,
                                     ActiveMutedTextColor(), false);
    textStack.Children().Append(g_titleCanvas);
    textStack.Children().Append(g_artistCanvas);
    Grid::SetColumn(textStack, 1);
    content.Children().Append(textStack);
    g_textContainer = textStack;

    Grid rightPanel;
    rightPanel.Width(g_layout.rightWidth);
    rightPanel.VerticalAlignment(VerticalAlignment::Center);
    rightPanel.HorizontalAlignment(HorizontalAlignment::Right);
    rightPanel.Margin({0, 3, 0, 0});

    ColumnDefinition timeCol;
    timeCol.Width({static_cast<double>(g_layout.timeWidth), GridUnitType::Pixel});
    ColumnDefinition controlsCol;
    controlsCol.Width({static_cast<double>(g_layout.controlsWidth),
                       GridUnitType::Pixel});
    rightPanel.ColumnDefinitions().Append(timeCol);
    rightPanel.ColumnDefinitions().Append(controlsCol);

    StackPanel controls;
    controls.Orientation(Orientation::Horizontal);
    controls.Spacing(2);
    controls.VerticalAlignment(VerticalAlignment::Center);
    controls.HorizontalAlignment(HorizontalAlignment::Right);
    controls.Margin({0, 0, 0, 0});
    g_prevButton = MakeButton(L"TaskbarMediaWidget_Previous", L"\xE892",
                              g_prevButtonIcon, &g_prevButtonHover,
                              &g_prevButtonEnabled, 1);
    g_playButton = MakeButton(L"TaskbarMediaWidget_PlayPause", L"\xE768",
                              g_playButtonIcon, &g_playButtonHover,
                              &g_playButtonEnabled, 2);
    g_nextButton = MakeButton(L"TaskbarMediaWidget_Next", L"\xE893",
                              g_nextButtonIcon, &g_nextButtonHover,
                              &g_nextButtonEnabled, 3);
    controls.Children().Append(g_prevButton);
    controls.Children().Append(g_playButton);
    controls.Children().Append(g_nextButton);

    g_timeText = TextBlock();
    g_timeText.FontSize(g_layout.timeFont);
    g_timeText.FontFamily(FontFamily(L"Segoe UI"));
    g_timeText.Foreground(MakeBrush(ActiveMutedTextColor()));
    g_timeText.TextAlignment(TextAlignment::Right);
    g_timeText.VerticalAlignment(VerticalAlignment::Center);
    g_timeText.HorizontalAlignment(HorizontalAlignment::Stretch);
    g_timeText.Margin({0, 0, 7, 0});
    Grid::SetColumn(g_timeText, 0);
    rightPanel.Children().Append(g_timeText);
    Grid::SetColumn(controls, 1);
    rightPanel.Children().Append(controls);
    Grid::SetColumn(rightPanel, 2);
    content.Children().Append(rightPanel);
    g_rightContainer = rightPanel;

    g_idlePanel = StackPanel();
    g_idlePanel.Orientation(Orientation::Horizontal);
    g_idlePanel.Spacing(4);
    g_idlePanel.VerticalAlignment(VerticalAlignment::Center);
    g_idlePanel.HorizontalAlignment(HorizontalAlignment::Left);
    g_idlePanel.Margin({4, 0, 0, 0});
    g_idlePanel.Visibility(Visibility::Collapsed);
    for (size_t i = 0; i < g_idleApps.size() && i < 6; i++) {
        g_idlePanel.Children().Append(MakeIdleButton(g_idleApps[i]));
    }
    Grid::SetColumn(g_idlePanel, 0);
    Grid::SetColumnSpan(g_idlePanel, 3);
    content.Children().Append(g_idlePanel);

    g_progressGrid = Grid();
    g_progressGrid.Height(g_layout.progressHoverHeight);
    g_progressGrid.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetRow(g_progressGrid, 1);
    Grid::SetColumnSpan(g_progressGrid, 4);
    wrapper.Children().Append(g_progressGrid);

    Canvas progressOverlay;
    progressOverlay.IsHitTestVisible(false);
    Canvas::SetZIndex(progressOverlay, 20);
    Grid::SetRowSpan(progressOverlay, 2);
    wrapper.Children().Append(progressOverlay);

    g_progressHoverTip = Border();
    g_progressHoverTip.Width(42);
    g_progressHoverTip.Height(18);
    g_progressHoverTip.CornerRadius({4, 4, 4, 4});
    g_progressHoverTip.Visibility(Visibility::Collapsed);

    g_progressToolTipText = TextBlock();
    g_progressToolTipText.FontFamily(FontFamily(L"Segoe UI"));
    g_progressToolTipText.FontSize(10.0);
    g_progressToolTipText.Text(L"0:00");
    g_progressToolTipText.HorizontalAlignment(HorizontalAlignment::Center);
    g_progressToolTipText.VerticalAlignment(VerticalAlignment::Center);
    g_progressToolTipText.TextAlignment(TextAlignment::Center);
    g_progressHoverTip.Child(g_progressToolTipText);
    UpdateProgressHoverTipTheme();
    progressOverlay.Children().Append(g_progressHoverTip);

    g_progressTrack = Border();
    g_progressTrack.Height(g_layout.progressHeight);
    g_progressTrack.CornerRadius({4, 4, 4, 4});
    g_progressTrack.Background(MakeBrush(UseLightTheme() ? RGB(132, 142, 156)
                                                        : RGB(72, 82, 99),
                                         120));
    g_progressTrack.HorizontalAlignment(HorizontalAlignment::Stretch);
    g_progressTrack.VerticalAlignment(VerticalAlignment::Center);
    g_progressGrid.Children().Append(g_progressTrack);

    g_progressFill = Border();
    g_progressFill.Height(g_layout.progressHeight);
    g_progressFill.CornerRadius({4, 4, 4, 4});
    g_progressFill.Background(MakeBrush(g_settings.accentColor));
    g_progressFill.HorizontalAlignment(HorizontalAlignment::Left);
    g_progressFill.VerticalAlignment(VerticalAlignment::Center);
    g_progressGrid.Children().Append(g_progressFill);

    g_progressThumb = winrt::Windows::UI::Xaml::Shapes::Ellipse();
    g_progressThumb.Width(std::max(5, g_layout.progressHoverHeight));
    g_progressThumb.Height(std::max(5, g_layout.progressHoverHeight));
    g_progressThumb.Fill(MakeBrush(ProgressThumbColor(g_settings.accentColor)));
    g_progressThumb.Visibility(Visibility::Collapsed);
    g_progressThumb.HorizontalAlignment(HorizontalAlignment::Left);
    g_progressThumb.VerticalAlignment(VerticalAlignment::Center);
    g_progressGrid.Children().Append(g_progressThumb);

    auto seekAtPointer = [](PointerRoutedEventArgs const& e) {
        MediaState state;
        {
            std::lock_guard lock(g_mediaMutex);
            state = g_mediaState;
        }
        if (!state.canSeek || state.durationMs <= 0 || !g_progressTrack) {
            return;
        }

        auto point = e.GetCurrentPoint(g_progressTrack).Position();
        double width = std::max(1.0, g_progressTrack.ActualWidth());
        double ratio = ClampDouble(point.X / width, 0.0, 1.0);
        int64_t targetMs = static_cast<int64_t>(state.durationMs * ratio);
        int64_t absoluteTargetMs = state.timelineStartMs + targetMs;

        std::thread([absoluteTargetMs]() {
            ApartmentScope apartment;
            GlobalSystemMediaTransportControlsSession session{nullptr};
            {
                std::lock_guard lock(g_mediaMutex);
                session = g_mediaSession;
            }
            if (session) {
                try {
                    session.TryChangePlaybackPositionAsync(
                        absoluteTargetMs * 10000)
                        .get();
                    g_uiDirty = true;
                } catch (...) {
                }
            }
        }).detach();
    };

    g_progressGrid.PointerEntered([](IInspectable const&, PointerRoutedEventArgs const&) {
        g_progressHover = true;
        if (g_progressTrack) {
            g_progressTrack.Height(g_layout.progressHoverHeight);
        }
        if (g_progressFill) {
            g_progressFill.Height(g_layout.progressHoverHeight);
        }
        if (g_progressThumb) {
            g_progressThumb.Visibility(Visibility::Visible);
        }
        if (g_progressHoverTip) {
            g_progressHoverTip.Visibility(Visibility::Visible);
        }
    });
    g_progressGrid.PointerExited([](IInspectable const&, PointerRoutedEventArgs const&) {
        g_progressHover = false;
        if (g_progressTrack) {
            g_progressTrack.Height(g_layout.progressHeight);
        }
        if (g_progressFill) {
            g_progressFill.Height(g_layout.progressHeight);
        }
        if (g_progressThumb) {
            g_progressThumb.Visibility(Visibility::Collapsed);
        }
        if (g_progressHoverTip) {
            g_progressHoverTip.Visibility(Visibility::Collapsed);
        }
    });
    g_progressGrid.PointerMoved([](IInspectable const&, PointerRoutedEventArgs const& e) {
        if (!g_progressThumb || !g_progressHover) {
            return;
        }
        auto point = e.GetCurrentPoint(g_progressGrid).Position();
        double width = std::max(1.0, g_progressGrid.ActualWidth());
        double x = ClampDouble(point.X, 0.0, width);
        g_progressThumb.Margin({x - g_progressThumb.Width() / 2.0, 0, 0, 0});

        if (g_progressHoverTip && g_progressToolTipText) {
            MediaState state;
            {
                std::lock_guard lock(g_mediaMutex);
                state = g_mediaState;
            }
            if (state.durationMs > 0) {
                int64_t targetMs =
                    static_cast<int64_t>(state.durationMs * (x / width));
                g_progressToolTipText.Text(FormatTime(targetMs));
            }

            double tipWidth = g_progressHoverTip.Width();
            double tipHeight = g_progressHoverTip.Height();
            double left = ClampDouble(x - tipWidth / 2.0, 0.0,
                                      std::max(0.0, width - tipWidth));
            double top = std::max(0.0, static_cast<double>(g_layout.widgetHeight) -
                                           g_layout.progressHoverHeight -
                                           tipHeight - 1.0);
            Canvas::SetLeft(g_progressHoverTip, left);
            Canvas::SetTop(g_progressHoverTip, top);
        }
    });
    g_progressGrid.PointerPressed([seekAtPointer](IInspectable const&,
                                                PointerRoutedEventArgs const& e) {
        seekAtPointer(e);
        e.Handled(true);
    });

    wrapper.PointerWheelChanged([](IInspectable const&, PointerRoutedEventArgs const& e) {
        ChangeVolumeFromWheel(e.GetCurrentPoint(nullptr).Properties().MouseWheelDelta());
        e.Handled(true);
    });

    return wrapper;
}

void RemoveWidgetGrid() {
    if (!g_injectionParent) {
        g_widgetGrid = nullptr;
        g_widgetColumn = -1;
        g_widgetRow = -1;
        g_injectedTaskbarEdge = TaskbarEdge::Unknown;
        g_injectedTaskbarThicknessPx = 0;
        ResetWidgetElementReferences();
        g_modernWidgetAttached = false;
        return;
    }

    try {
        auto targetPanel = g_injectionParent.try_as<Panel>();
        auto targetGrid = g_injectionParent.try_as<Grid>();

        int removedColumn = -1;
        int removedRow = -1;
        if (targetPanel) {
            for (int i = static_cast<int>(targetPanel.Children().Size()) - 1;
                 i >= 0; i--) {
                auto child =
                    targetPanel.Children().GetAt(i).try_as<FrameworkElement>();
                if (child && child.Name() == L"TaskbarMediaWidget") {
                    if (targetGrid) {
                        removedColumn = Grid::GetColumn(child);
                        removedRow = Grid::GetRow(child);
                    }
                    targetPanel.Children().RemoveAt(i);
                }
            }
        }

        if (targetGrid && g_widgetColumn >= 0 && removedColumn >= 0 &&
            removedColumn < static_cast<int>(targetGrid.ColumnDefinitions().Size())) {
            for (uint32_t i = 0; i < targetGrid.Children().Size(); i++) {
                auto child = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                if (child && Grid::GetColumn(child) > removedColumn) {
                    Grid::SetColumn(child, Grid::GetColumn(child) - 1);
                }
            }
            targetGrid.ColumnDefinitions().RemoveAt(removedColumn);
        }
        if (targetGrid && g_widgetRow >= 0 && removedRow >= 0 &&
            removedRow < static_cast<int>(targetGrid.RowDefinitions().Size())) {
            for (uint32_t i = 0; i < targetGrid.Children().Size(); i++) {
                auto child = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                if (child && Grid::GetRow(child) > removedRow) {
                    Grid::SetRow(child, Grid::GetRow(child) - 1);
                }
            }
            targetGrid.RowDefinitions().RemoveAt(removedRow);
        }
    } catch (...) {
    }

    g_widgetGrid = nullptr;
    g_injectionParent = nullptr;
    g_widgetColumn = -1;
    g_widgetRow = -1;
    g_injectedTaskbarEdge = TaskbarEdge::Unknown;
    g_injectedTaskbarThicknessPx = 0;
    ResetWidgetElementReferences();
    g_modernWidgetAttached = false;
}

void UpdateTextMarquee(Canvas const& canvas,
                       TextBlock const& textBlock,
                       TranslateTransform const& transform,
                       TextScrollState& state) {
    if (!canvas || !textBlock || !transform) {
        return;
    }

    double viewWidth = canvas.ActualWidth();
    double textWidth = textBlock.ActualWidth();
    double overflow = textWidth - viewWidth;
    if (overflow <= 4.0) {
        state.offset = 0.0;
        state.forward = true;
        transform.X(0);
        return;
    }

    double step = g_settings.marqueeSpeed;
    state.offset += state.forward ? step : -step;
    if (state.offset >= overflow) {
        state.offset = overflow;
        state.forward = false;
    } else if (state.offset <= 0.0) {
        state.offset = 0.0;
        state.forward = true;
    }
    transform.X(-state.offset);
}

void UpdateVisualizerBars(MediaState const& state) {
    if (!g_visualizerGrid) {
        return;
    }

    bool visible = g_settings.visualizerEnabled && state.hasSession;
    g_visualizerGrid.Visibility(visible ? Visibility::Visible
                                        : Visibility::Collapsed);
    if (!visible) {
        return;
    }

    std::array<float, kVisualizerMaxBarCount> bands{};
    ULONGLONG updatedTick = 0;
    {
        std::lock_guard lock(g_visualizerMutex);
        bands = g_visualizerBands;
        updatedTick = g_visualizerUpdatedTick;
    }

    bool fresh = updatedTick != 0 && GetTickCount64() - updatedTick < 1000;
    bool active = state.isPlaying && fresh;
    COLORREF accent = VisualizerAccentColor(ProgressAccentColor(state));
    BYTE alpha = static_cast<BYTE>(
        ClampInt(static_cast<int>(std::round(g_settings.visualizerOpacity * 255.0)),
                 10, 170));
    double availableHeight = std::max(4.0, g_visualizerGrid.ActualHeight());
    double maxHeight = std::max(2.0, availableHeight - 5.0);

    for (size_t i = 0; i < g_visualizerBars.size(); i++) {
        double band = active ? ClampDouble(bands[i], 0.0, 1.0) : 0.0;
        double height = band > 0.015 ? 1.0 + band * maxHeight : 1.0;
        g_visualizerBars[i].Height(height);
        g_visualizerBars[i].Background(MakeBrush(accent, alpha));
    }
}

void RefreshProgress(MediaState const& state) {
    if (!g_progressTrack || !g_progressFill) {
        return;
    }

    double width = std::max(1.0, g_progressTrack.ActualWidth());
    double ratio = 0.0;
    if (state.durationMs > 0) {
        ratio = ClampDouble(static_cast<double>(EffectivePosition(state)) /
                                static_cast<double>(state.durationMs),
                            0.0, 1.0);
    }
    g_progressFill.Width(width * ratio);
}

void RefreshThumbnail(MediaState const& state) {
    if (!g_coverImage) {
        return;
    }

    if (state.thumbnailBytes.empty() || state.thumbnailKey.empty()) {
        g_coverImage.Source(nullptr);
        g_loadedThumbnailKey.clear();
        return;
    }

    if (state.thumbnailKey == g_loadedThumbnailKey) {
        return;
    }

    try {
        IStream* rawStream = SHCreateMemStream(
            state.thumbnailBytes.data(),
            static_cast<UINT>(state.thumbnailBytes.size()));
        if (!rawStream) {
            throw hresult_error(E_FAIL, L"SHCreateMemStream failed");
        }

        com_ptr<IStream> comStream;
        comStream.attach(rawStream);

        IRandomAccessStream randomAccessStream{nullptr};
        check_hresult(::CreateRandomAccessStreamOverStream(
            comStream.get(), BSOS_DEFAULT, guid_of<IRandomAccessStream>(),
            put_abi(randomAccessStream)));

        BitmapImage image;
        image.DecodePixelHeight(g_layout.coverSize > 0 ? g_layout.coverSize * 2
                                                       : g_settings.coverSize * 2);
        g_coverImage.Source(image);
        image.SetSourceAsync(randomAccessStream);
        g_loadedThumbnailKey = state.thumbnailKey;
    } catch (hresult_error const& e) {
        Wh_Log(L"RefreshThumbnail failed: 0x%08X %s",
               static_cast<uint32_t>(e.code()), e.message().c_str());
        g_coverImage.Source(nullptr);
        g_loadedThumbnailKey.clear();
    } catch (...) {
        Wh_Log(L"RefreshThumbnail failed");
        g_coverImage.Source(nullptr);
        g_loadedThumbnailKey.clear();
    }
}

void RefreshWidgetContents() {
    if (!g_widgetGrid) {
        return;
    }

    MediaState state;
    {
        std::lock_guard lock(g_mediaMutex);
        state = g_mediaState;
    }

    bool idleVisible = !state.hasSession && !g_idleApps.empty();
    bool visible = state.hasSession || idleVisible || !g_settings.hideWhenNoMedia;
    g_widgetGrid.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
    if (g_verticalWidgetLayout) {
        g_widgetGrid.Width(g_verticalWidgetWidth);
        g_widgetGrid.Height(idleVisible ? VerticalIdleWidgetHeight()
                                        : g_verticalSessionHeight);
    } else {
        g_widgetGrid.Width(idleVisible ? IdleWidgetWidth()
                                       : static_cast<double>(g_settings.widgetWidth));
    }
    if (g_progressRow) {
        g_progressRow.Height({idleVisible ? 0.0
                                          : static_cast<double>(
                                                g_layout.progressHoverHeight),
                              GridUnitType::Pixel});
    }
    if (!visible) {
        return;
    }

    if (g_idlePanel) {
        g_idlePanel.Visibility(idleVisible ? Visibility::Visible
                                           : Visibility::Collapsed);
    }
    if (g_coverContainer) {
        g_coverContainer.Visibility(state.hasSession ? Visibility::Visible
                                                     : Visibility::Collapsed);
    }
    if (g_textContainer) {
        g_textContainer.Visibility(state.hasSession || !idleVisible
                                       ? Visibility::Visible
                                       : Visibility::Collapsed);
    }
    if (g_rightContainer) {
        g_rightContainer.Visibility(state.hasSession ? Visibility::Visible
                                                     : Visibility::Collapsed);
    }
    if (g_progressGrid) {
        g_progressGrid.Visibility(state.hasSession ? Visibility::Visible
                                                   : Visibility::Collapsed);
    }
    if (g_visualizerGrid && !state.hasSession) {
        g_visualizerGrid.Visibility(Visibility::Collapsed);
    }

    if (idleVisible) {
        if (g_progressHoverTip) {
            g_progressHoverTip.Visibility(Visibility::Collapsed);
        }
        return;
    }

    std::wstring title = state.title.empty() ? L"No media" : state.title;
    std::wstring artist = state.artist;
    if (g_verticalWidgetLayout && g_coverContainer) {
        std::wstring tooltip = title;
        if (!artist.empty()) {
            tooltip += L"\n" + artist;
        }
        ToolTipService::SetToolTip(g_coverContainer, box_value(tooltip));
    }
    COLORREF accent = ProgressAccentColor(state);
    if (g_progressFill) {
        g_progressFill.Background(MakeBrush(accent));
    }
    if (g_progressThumb) {
        g_progressThumb.Fill(MakeBrush(ProgressThumbColor(accent)));
    }
    UpdateProgressHoverTipTheme();

    if (g_titleText) {
        ResetMarquee(g_titleScroll, title);
        g_titleText.Text(title);
        g_titleText.Foreground(MakeBrush(ActiveTextColor()));
    }
    if (g_artistText) {
        ResetMarquee(g_artistScroll, artist);
        g_artistText.Text(artist);
        g_artistText.Foreground(MakeBrush(ActiveMutedTextColor()));
    }

    if (g_timeText) {
        std::wstring time = FormatTime(EffectivePosition(state));
        if (!g_verticalWidgetLayout) {
            time += L" / " + FormatTime(state.durationMs);
        }
        g_timeText.Text(time);
        g_timeText.Foreground(MakeBrush(ActiveMutedTextColor()));
    }

    if (g_playButton) {
        std::wstring glyph = state.isPlaying ? L"\xE769" : L"\xE768";
        if (g_playButtonGlyph != glyph) {
            if (g_playButtonIcon) {
                g_playButtonIcon.Text(glyph);
                g_playButtonIcon.FontSize(14.0);
            }
            g_playButtonGlyph = glyph;
        }
        g_playButtonEnabled = state.canPlayPause || state.canPlay;
        if (!g_playButtonEnabled) {
            g_playButtonHover = false;
            SetButtonHoverBackground(g_playButton, false);
        }
        UpdateButtonForeground(g_playButton, g_playButtonIcon,
                               g_playButtonEnabled);
    }
    if (g_prevButton) {
        g_prevButtonEnabled = state.canPrevious;
        if (!g_prevButtonEnabled) {
            g_prevButtonHover = false;
            SetButtonHoverBackground(g_prevButton, false);
        }
        UpdateButtonForeground(g_prevButton, g_prevButtonIcon,
                               g_prevButtonEnabled);
    }
    if (g_nextButton) {
        g_nextButtonEnabled = state.canNext;
        if (!g_nextButtonEnabled) {
            g_nextButtonHover = false;
            SetButtonHoverBackground(g_nextButton, false);
        }
        UpdateButtonForeground(g_nextButton, g_nextButtonIcon,
                               g_nextButtonEnabled);
    }

    RefreshThumbnail(state);
    RefreshProgress(state);
    UpdateVisualizerBars(state);
}

void TickWidgetUi() {
    if (!g_widgetGrid) {
        return;
    }

    MediaState state;
    {
        std::lock_guard lock(g_mediaMutex);
        state = g_mediaState;
    }

    if (g_timeText && state.hasSession) {
        std::wstring time = FormatTime(EffectivePosition(state));
        if (!g_verticalWidgetLayout) {
            time += L" / " + FormatTime(state.durationMs);
        }
        g_timeText.Text(time);
    }
    RefreshProgress(state);
    UpdateVisualizerBars(state);
    UpdateTextMarquee(g_titleCanvas, g_titleText, g_titleTransform, g_titleScroll);
    UpdateTextMarquee(g_artistCanvas, g_artistText, g_artistTransform,
                      g_artistScroll);
}

bool InjectWidgetGrid() {
    g_modernWidgetAttached = false;
    HWND hwnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!hwnd) {
        return false;
    }
    g_taskbarWnd = hwnd;

    try {
        auto xamlRoot = GetTaskbarXamlRoot(hwnd);
        if (!xamlRoot) {
            return false;
        }

        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root) {
            return false;
        }

        auto targetElement = FindSystemTrayFramePanel(root);
        auto targetPanel = targetElement ? targetElement.try_as<Panel>() : nullptr;
        if (!targetPanel) {
            return false;
        }

        RemoveWidgetGrid();

        TaskbarEdge edge = DetectTaskbarEdge(hwnd);
        bool vertical = IsVerticalTaskbarEdge(edge);
        Grid widget = vertical ? BuildVerticalWidgetGrid(targetElement.ActualWidth())
                               : BuildWidgetGrid(targetElement.ActualHeight());
        widget.Margin(vertical ? Thickness{0, 4, 0, 4}
                               : Thickness{4, 0, 4, 0});

        int insertCol = -1;
        int insertRow = -1;
        if (auto targetGrid = targetElement.try_as<Grid>()) {
            if (vertical) {
                RowDefinition row;
                row.Height({1.0, GridUnitType::Auto});
                insertRow = 0;
                targetGrid.RowDefinitions().InsertAt(insertRow, row);
                for (uint32_t i = 0; i < targetGrid.Children().Size(); i++) {
                    auto child =
                        targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                    if (child && Grid::GetRow(child) >= insertRow) {
                        Grid::SetRow(child, Grid::GetRow(child) + 1);
                    }
                }

                Grid::SetRow(widget, insertRow);
            } else {
                ColumnDefinition column;
                column.Width({1.0, GridUnitType::Auto});
                insertCol = 0;
                targetGrid.ColumnDefinitions().InsertAt(insertCol, column);
                for (uint32_t i = 0; i < targetGrid.Children().Size(); i++) {
                    auto child =
                        targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                    if (child && Grid::GetColumn(child) >= insertCol) {
                        Grid::SetColumn(child, Grid::GetColumn(child) + 1);
                    }
                }

                Grid::SetColumn(widget, insertCol);
            }
            targetGrid.Children().Append(widget);
        } else {
            // Newer taskbar builds use a StackPanel whose orientation follows
            // the native taskbar edge.
            targetPanel.Children().InsertAt(0, widget);
        }

        g_widgetGrid = widget;
        g_injectionParent = targetElement;
        g_widgetColumn = insertCol;
        g_widgetRow = insertRow;
        g_injectedTaskbarEdge = edge;
        g_injectedTaskbarThicknessPx = TaskbarThicknessPx(hwnd, edge);
        g_modernWidgetAttached = true;
        RefreshWidgetContents();
        return true;
    } catch (hresult_error const& e) {
        Wh_Log(L"InjectWidgetGrid failed: 0x%08X %s",
               static_cast<uint32_t>(e.code()), e.message().c_str());
    } catch (...) {
        Wh_Log(L"InjectWidgetGrid failed");
    }
    return false;
}

bool HasMeaningfulMedia(hstring const& title,
                        hstring const& artist,
                        TimeSpan start,
                        TimeSpan end,
                        GlobalSystemMediaTransportControlsSessionPlaybackControls const& controls,
                        IRandomAccessStreamReference const& thumbnail) {
    std::wstring titleText = Trim(std::wstring(title.c_str()));
    std::wstring artistText = Trim(std::wstring(artist.c_str()));
    if (titleText.empty()) {
        return false;
    }

    bool hasDuration =
        end > start && std::chrono::duration_cast<std::chrono::seconds>(end - start).count() > 2;
    return !artistText.empty() || hasDuration || static_cast<bool>(thumbnail);
}

void PollMedia() {
    MediaState nextState;
    GlobalSystemMediaTransportControlsSession nextSession{nullptr};

    try {
        auto manager =
            GlobalSystemMediaTransportControlsSessionManager::RequestAsync()
                .get();
        auto currentSession = manager.GetCurrentSession();
        if (currentSession) {
            nextSession = currentSession;
        } else {
            auto sessions = manager.GetSessions();
            if (sessions.Size() > 0) {
                nextSession = sessions.GetAt(0);
            }
        }

        if (nextSession) {
            auto playbackInfo = nextSession.GetPlaybackInfo();
            auto controls = playbackInfo.Controls();
            auto timeline = nextSession.GetTimelineProperties();
            auto properties = nextSession.TryGetMediaPropertiesAsync().get();
            bool meaningful = HasMeaningfulMedia(
                properties.Title(), properties.Artist(), timeline.StartTime(),
                timeline.EndTime(), controls, properties.Thumbnail());

            if (!meaningful) {
                nextSession = nullptr;
            } else {
                nextState.hasSession = true;
                nextState.sourceAppId = nextSession.SourceAppUserModelId();
                nextState.title = properties.Title();
                nextState.artist = properties.Artist();
                nextState.isPlaying =
                    playbackInfo.PlaybackStatus() ==
                    GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                nextState.canPrevious = controls.IsPreviousEnabled();
                nextState.canNext = controls.IsNextEnabled();
                nextState.canPlay = controls.IsPlayEnabled();
                nextState.canPlayPause =
                    controls.IsPlayPauseToggleEnabled();
                nextState.canSeek = controls.IsPlaybackPositionEnabled();
                nextState.timelineStartMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        timeline.StartTime())
                        .count();
                nextState.positionMs =
                    TimelinePositionMs(timeline, nextState.isPlaying);
                nextState.durationMs = TimelineDurationMs(timeline);
                nextState.updatedTick = GetTickCount64();
                nextState.thumbnail = properties.Thumbnail();
                nextState.thumbnailKey =
                    nextState.sourceAppId + L"|" + nextState.title + L"|" +
                    nextState.artist;
                {
                    std::lock_guard lock(g_mediaMutex);
                    if (g_mediaState.thumbnailKey == nextState.thumbnailKey) {
                        nextState.thumbnailBytes = g_mediaState.thumbnailBytes;
                        nextState.hasAccentColor = g_mediaState.hasAccentColor;
                        nextState.accentColor = g_mediaState.accentColor;
                    }
                }
                if (nextState.thumbnailBytes.empty()) {
                    nextState.thumbnailBytes =
                        ReadThumbnailBytes(properties.Thumbnail());
                }
                if (!nextState.thumbnailBytes.empty() &&
                    !nextState.hasAccentColor) {
                    nextState.accentColor =
                        ExtractAccentColor(nextState.thumbnailBytes);
                    nextState.hasAccentColor = true;
                }
                if (properties.Thumbnail() && nextState.thumbnailBytes.empty()) {
                    Wh_Log(L"Thumbnail exists but no bytes were read: source=%s title=%s",
                           nextState.sourceAppId.c_str(),
                           nextState.title.c_str());
                }
            }
        }
    } catch (hresult_error const& e) {
        Wh_Log(L"PollMedia failed: 0x%08X %s",
               static_cast<uint32_t>(e.code()), e.message().c_str());
    } catch (...) {
        Wh_Log(L"PollMedia failed");
    }

    {
        std::lock_guard lock(g_mediaMutex);
        ULONGLONG now = GetTickCount64();
        if (nextState.hasSession) {
            g_missingMediaSinceTick = 0;
        } else if (g_mediaState.hasSession) {
            if (g_missingMediaSinceTick == 0) {
                g_missingMediaSinceTick = now;
            }

            if (now - g_missingMediaSinceTick < kMissingMediaGraceMs) {
                nextState = g_mediaState;
                nextSession = g_mediaSession;
            }
        } else {
            g_missingMediaSinceTick = 0;
        }

        g_mediaState = nextState;
        g_mediaSession = nextSession;
    }
    g_uiDirty = true;
}

// Windows 10's taskbar is a classic Win32 surface. Keeping this backend in the
// same module lets the media/session code stay shared while only the rendering
// and taskbar attachment differ from the Windows 11 XAML path.
struct ClassicDecodedImage {
    std::wstring key;
    UINT width = 0;
    UINT height = 0;
    std::vector<BYTE> pixels;
};

ClassicDecodedImage g_classicCoverImage;
std::vector<ClassicDecodedImage> g_classicIdleImages;
int g_classicHotButton = 0;
bool g_classicProgressHover = false;
int g_classicProgressHoverX = 0;

int ClassicScale(HWND hwnd, int value) {
    HDC dc = GetDC(hwnd);
    int dpi = dc ? GetDeviceCaps(dc, LOGPIXELSX) : 96;
    if (dc) {
        ReleaseDC(hwnd, dc);
    }
    return MulDiv(value, dpi > 0 ? dpi : 96, 96);
}

COLORREF BlendColor(COLORREF background, COLORREF foreground, double amount) {
    amount = ClampDouble(amount, 0.0, 1.0);
    auto channel = [amount](BYTE a, BYTE b) {
        return static_cast<BYTE>(ClampInt(
            static_cast<int>(std::round(a + (b - a) * amount)), 0, 255));
    };
    return RGB(channel(GetRValue(background), GetRValue(foreground)),
               channel(GetGValue(background), GetGValue(foreground)),
               channel(GetBValue(background), GetBValue(foreground)));
}

bool DecodeClassicImage(std::vector<uint8_t> const& bytes,
                        std::wstring const& key,
                        int decodeSize,
                        ClassicDecodedImage& output) {
    if (bytes.empty() || key.empty() || decodeSize <= 0) {
        output = {};
        return false;
    }
    if (output.key == key && !output.pixels.empty()) {
        return true;
    }

    IStream* rawStream =
        SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size()));
    if (!rawStream) {
        output = {};
        return false;
    }
    com_ptr<IStream> stream;
    stream.attach(rawStream);

    com_ptr<IWICImagingFactory> factory;
    com_ptr<IWICBitmapDecoder> decoder;
    com_ptr<IWICBitmapFrameDecode> frame;
    com_ptr<IWICBitmapScaler> scaler;
    com_ptr<IWICFormatConverter> converter;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
                                factory.put_void())) ||
        FAILED(factory->CreateDecoderFromStream(
            stream.get(), nullptr, WICDecodeMetadataCacheOnLoad,
            decoder.put())) ||
        FAILED(decoder->GetFrame(0, frame.put())) ||
        FAILED(factory->CreateBitmapScaler(scaler.put())) ||
        FAILED(scaler->Initialize(frame.get(), decodeSize, decodeSize,
                                  WICBitmapInterpolationModeFant)) ||
        FAILED(factory->CreateFormatConverter(converter.put())) ||
        FAILED(converter->Initialize(
            scaler.get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0.0,
            WICBitmapPaletteTypeCustom))) {
        output = {};
        return false;
    }

    ClassicDecodedImage decoded;
    decoded.key = key;
    decoded.width = static_cast<UINT>(decodeSize);
    decoded.height = static_cast<UINT>(decodeSize);
    decoded.pixels.resize(static_cast<size_t>(decodeSize) * decodeSize * 4);
    if (FAILED(converter->CopyPixels(
            nullptr, decodeSize * 4, static_cast<UINT>(decoded.pixels.size()),
            decoded.pixels.data()))) {
        output = {};
        return false;
    }

    output = std::move(decoded);
    return true;
}

void DrawClassicImage(HDC dc,
                      ClassicDecodedImage const& image,
                      RECT destination,
                      int radius) {
    if (image.pixels.empty() || image.width == 0 || image.height == 0) {
        return;
    }

    BITMAPINFO bitmapInfo{};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = static_cast<LONG>(image.width);
    bitmapInfo.bmiHeader.biHeight = -static_cast<LONG>(image.height);
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap =
        CreateDIBSection(dc, &bitmapInfo, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        return;
    }
    memcpy(bits, image.pixels.data(), image.pixels.size());

    HDC imageDc = CreateCompatibleDC(dc);
    HGDIOBJ oldBitmap = SelectObject(imageDc, bitmap);
    int saved = SaveDC(dc);
    HRGN clip = CreateRoundRectRgn(destination.left, destination.top,
                                   destination.right + 1,
                                   destination.bottom + 1, radius, radius);
    SelectClipRgn(dc, clip);
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(dc, destination.left, destination.top,
               destination.right - destination.left,
               destination.bottom - destination.top, imageDc, 0, 0,
               static_cast<int>(image.width), static_cast<int>(image.height),
               blend);
    RestoreDC(dc, saved);
    DeleteObject(clip);
    SelectObject(imageDc, oldBitmap);
    DeleteDC(imageDc);
    DeleteObject(bitmap);
}

void FillClassicRoundRect(HDC dc, RECT rect, int radius, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(brush);
}

RECT ClassicProgressRect(HWND hwnd) {
    RECT client{};
    GetClientRect(hwnd, &client);
    int normalHeight = std::max(2, ClassicScale(hwnd, g_settings.progressHeight));
    int hoverHeight = std::max(
        normalHeight,
        ClassicScale(hwnd, g_settings.progressHoverHeight));
    int height = g_classicProgressHover ? hoverHeight : normalHeight;
    return {ClassicScale(hwnd, 5), client.bottom - height,
            client.right - ClassicScale(hwnd, 5), client.bottom};
}

RECT ClassicProgressHitRect(HWND hwnd) {
    RECT rect = ClassicProgressRect(hwnd);
    rect.top = std::max<LONG>(0, rect.top - ClassicScale(hwnd, 8));
    return rect;
}

RECT ClassicCoverRect(HWND hwnd) {
    RECT client{};
    GetClientRect(hwnd, &client);
    int progress = ClassicScale(hwnd, g_settings.progressHoverHeight);
    int contentHeight =
        static_cast<int>(std::max<LONG>(1, client.bottom - progress));
    int size = std::min(ClassicScale(hwnd, g_settings.coverSize),
                        std::max(ClassicScale(hwnd, 18),
                                 contentHeight - ClassicScale(hwnd, 8)));
    int left = ClassicScale(hwnd, 6);
    int top = (contentHeight - size) / 2;
    return {left, top, left + size, top + size};
}

RECT ClassicButtonRect(HWND hwnd, int command) {
    RECT client{};
    GetClientRect(hwnd, &client);
    int progress = ClassicScale(hwnd, g_settings.progressHoverHeight);
    int contentHeight =
        static_cast<int>(std::max<LONG>(1, client.bottom - progress));
    int size = std::min(ClassicScale(hwnd, g_settings.buttonSize),
                        std::max(ClassicScale(hwnd, 22), contentHeight - 2));
    int gap = ClassicScale(hwnd, 2);
    int right = client.right - ClassicScale(hwnd, 3);
    int indexFromRight = command == 3 ? 0 : command == 2 ? 1 : 2;
    int x = right - size - indexFromRight * (size + gap);
    int y = (contentHeight - size) / 2;
    return {x, y, x + size, y + size};
}

bool PointInClassicRect(RECT const& rect, POINT point) {
    return point.x >= rect.left && point.x < rect.right &&
           point.y >= rect.top && point.y < rect.bottom;
}

void SeekClassicWidget(double ratio) {
    ratio = ClampDouble(ratio, 0.0, 1.0);
    MediaState state;
    {
        std::lock_guard lock(g_mediaMutex);
        state = g_mediaState;
    }
    if (!state.canSeek || state.durationMs <= 0) {
        return;
    }

    int64_t targetMs = static_cast<int64_t>(state.durationMs * ratio);
    int64_t absoluteTargetMs = state.timelineStartMs + targetMs;
    std::thread([absoluteTargetMs]() {
        ApartmentScope apartment;
        GlobalSystemMediaTransportControlsSession session{nullptr};
        {
            std::lock_guard lock(g_mediaMutex);
            session = g_mediaSession;
        }
        if (!session) {
            return;
        }
        try {
            session.TryChangePlaybackPositionAsync(absoluteTargetMs * 10000)
                .get();
            g_uiDirty = true;
        } catch (...) {
        }
    }).detach();
}

RECT ClassicIdleIconRect(HWND hwnd, size_t index, size_t count) {
    RECT client{};
    GetClientRect(hwnd, &client);
    int progress = ClassicScale(hwnd, g_settings.progressHoverHeight);
    int contentHeight =
        static_cast<int>(std::max<LONG>(1, client.bottom - progress));
    int iconSize = std::min(ClassicScale(hwnd, 28), contentHeight - 6);
    int gap = ClassicScale(hwnd, 4);
    int totalWidth = static_cast<int>(count) * iconSize +
                     static_cast<int>(count > 0 ? count - 1 : 0) * gap;
    int left = static_cast<int>(std::max<LONG>(
        ClassicScale(hwnd, 4), (client.right - totalWidth) / 2));
    int top = (contentHeight - iconSize) / 2;
    int x = left + static_cast<int>(index) * (iconSize + gap);
    return {x, top, x + iconSize, top + iconSize};
}

int ClassicDesiredWidth(HWND hwnd, MediaState const& state) {
    if (state.hasSession || g_idleApps.empty()) {
        return ClassicScale(hwnd, g_settings.widgetWidth);
    }

    size_t count = std::min<size_t>(g_idleApps.size(), 6);
    int iconSize = ClassicScale(hwnd, 28);
    int gap = ClassicScale(hwnd, 4);
    int width = ClassicScale(hwnd, 8) + static_cast<int>(count) * iconSize +
                static_cast<int>(count > 0 ? count - 1 : 0) * gap;
    return std::min(width, ClassicScale(hwnd, g_settings.widgetWidth));
}

void DrawClassicMarquee(HDC dc,
                        RECT rect,
                        HFONT font,
                        std::wstring const& text,
                        COLORREF color) {
    if (text.empty() || rect.right <= rect.left) {
        return;
    }

    HGDIOBJ oldFont = SelectObject(dc, font);
    SetTextColor(dc, color);
    SetBkMode(dc, TRANSPARENT);
    SIZE size{};
    GetTextExtentPoint32W(dc, text.c_str(), static_cast<int>(text.size()), &size);

    int saved = SaveDC(dc);
    IntersectClipRect(dc, rect.left, rect.top, rect.right, rect.bottom);
    int x = rect.left;
    int overflow = size.cx - (rect.right - rect.left);
    if (overflow > ClassicScale(g_classicWidgetWnd, 4)) {
        double travel = static_cast<double>(overflow + ClassicScale(
                                                       g_classicWidgetWnd, 12));
        double cycle = travel * 2.0;
        double phase = fmod(GetTickCount64() * g_settings.marqueeSpeed * 0.035,
                            cycle);
        double offset = phase <= travel ? phase : cycle - phase;
        x -= static_cast<int>(offset);
    }

    RECT drawRect{x, rect.top, x + std::max(size.cx, rect.right - rect.left),
                  rect.bottom};
    DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &drawRect,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    RestoreDC(dc, saved);
    SelectObject(dc, oldFont);
}

void PaintClassicWidget(HWND hwnd, HDC targetDc) {
    RECT client{};
    GetClientRect(hwnd, &client);
    int width = client.right;
    int height = client.bottom;
    if (width <= 0 || height <= 0) {
        return;
    }

    HDC dc = CreateCompatibleDC(targetDc);
    HBITMAP buffer = CreateCompatibleBitmap(targetDc, width, height);
    HGDIOBJ oldBitmap = SelectObject(dc, buffer);

    bool light = UseLightTheme();
    COLORREF background = light ? RGB(243, 243, 243) : RGB(31, 31, 31);
    COLORREF textColor = ActiveTextColor();
    COLORREF mutedColor = ActiveMutedTextColor();
    HBRUSH backgroundBrush = CreateSolidBrush(background);
    FillRect(dc, &client, backgroundBrush);
    DeleteObject(backgroundBrush);

    MediaState state;
    {
        std::lock_guard lock(g_mediaMutex);
        state = g_mediaState;
    }

    int progressReserve = ClassicScale(hwnd, g_settings.progressHoverHeight);
    int contentHeight = std::max(1, height - progressReserve);
    if (state.hasSession && g_settings.visualizerEnabled) {
        std::array<float, kVisualizerMaxBarCount> bands{};
        {
            std::lock_guard lock(g_visualizerMutex);
            bands = g_visualizerBands;
        }
        int barWidth = std::max(1, ClassicScale(hwnd, kVisualizerBarWidth));
        int gap = std::max(1, ClassicScale(hwnd, kVisualizerBarGap));
        int count = ClampInt((width - gap) / (barWidth + gap),
                             kVisualizerMinBarCount, kVisualizerMaxBarCount);
        COLORREF accent = VisualizerAccentColor(ProgressAccentColor(state));
        COLORREF barColor =
            BlendColor(background, accent, g_settings.visualizerOpacity);
        HBRUSH barBrush = CreateSolidBrush(barColor);
        for (int i = 0; i < count; i++) {
            int source = i * kVisualizerMaxBarCount / count;
            int barHeight = std::max(
                1, static_cast<int>(bands[source] * (contentHeight - 4)));
            int x = gap + i * (barWidth + gap);
            RECT bar{x, contentHeight - barHeight, x + barWidth,
                     contentHeight};
            FillRect(dc, &bar, barBrush);
        }
        DeleteObject(barBrush);
    }

    if (!state.hasSession) {
        size_t count = std::min<size_t>(g_idleApps.size(), 6);
        if (g_classicIdleImages.size() != count) {
            g_classicIdleImages.clear();
            g_classicIdleImages.resize(count);
        }
        HFONT fallbackFont = CreateFontW(
            -ClassicScale(hwnd, 13), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE,
            FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        for (size_t i = 0; i < count; i++) {
            RECT iconRect = ClassicIdleIconRect(hwnd, i, count);
            if (!g_idleApps[i].iconBytes.empty() &&
                DecodeClassicImage(g_idleApps[i].iconBytes,
                                   g_idleApps[i].shortcutPath,
                                   iconRect.right - iconRect.left,
                                   g_classicIdleImages[i])) {
                DrawClassicImage(dc, g_classicIdleImages[i], iconRect,
                                 ClassicScale(hwnd, 7));
            } else {
                std::wstring letter = g_idleApps[i].name.empty()
                                          ? L"\x266A"
                                          : g_idleApps[i].name.substr(0, 1);
                HGDIOBJ oldFont = SelectObject(dc, fallbackFont);
                SetTextColor(dc, textColor);
                SetBkMode(dc, TRANSPARENT);
                DrawTextW(dc, letter.c_str(), static_cast<int>(letter.size()),
                          &iconRect,
                          DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                SelectObject(dc, oldFont);
            }
        }
        DeleteObject(fallbackFont);
    } else {
        RECT coverRect = ClassicCoverRect(hwnd);
        int coverRadius = ClassicScale(hwnd, g_settings.coverRadius * 2);
        if (DecodeClassicImage(state.thumbnailBytes, state.thumbnailKey,
                               coverRect.right - coverRect.left,
                               g_classicCoverImage)) {
            DrawClassicImage(dc, g_classicCoverImage, coverRect, coverRadius);
        } else {
            FillClassicRoundRect(dc, coverRect, coverRadius,
                                 light ? RGB(214, 219, 226)
                                       : RGB(53, 61, 72));
        }

        RECT prevRect = ClassicButtonRect(hwnd, 1);
        RECT playRect = ClassicButtonRect(hwnd, 2);
        RECT nextRect = ClassicButtonRect(hwnd, 3);
        RECT buttonRects[] = {prevRect, playRect, nextRect};
        for (int i = 0; i < 3; i++) {
            if (g_classicHotButton == i + 1) {
                COLORREF hover = light ? RGB(255, 255, 255)
                                       : RGB(62, 62, 62);
                FillClassicRoundRect(dc, buttonRects[i],
                                     buttonRects[i].right - buttonRects[i].left,
                                     hover);
            }
        }

        int textLeft = coverRect.right + ClassicScale(hwnd, 6);
        int textRight = prevRect.left - ClassicScale(hwnd, 58);
        textRight = std::max(textLeft + ClassicScale(hwnd, 20), textRight);
        int lineHeight = std::max(12, contentHeight / 2);
        RECT titleRect{textLeft, 1, textRight, lineHeight + 1};
        RECT artistRect{textLeft, lineHeight - 1, textRight,
                        contentHeight - 1};
        HFONT titleFont = CreateFontW(
            -ClassicScale(hwnd, 11), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE,
            FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HFONT artistFont = CreateFontW(
            -ClassicScale(hwnd, 10), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        DrawClassicMarquee(dc, titleRect, titleFont, state.title, textColor);
        DrawClassicMarquee(dc, artistRect, artistFont, state.artist, mutedColor);

        RECT timeRect{prevRect.left - ClassicScale(hwnd, 58), 0,
                      prevRect.left - ClassicScale(hwnd, 4), contentHeight};
        std::wstring time = FormatTime(EffectivePosition(state)) + L" / " +
                            FormatTime(state.durationMs);
        HFONT timeFont = CreateFontW(
            -ClassicScale(hwnd, 9), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HGDIOBJ oldFont = SelectObject(dc, timeFont);
        SetTextColor(dc, mutedColor);
        SetBkMode(dc, TRANSPARENT);
        DrawTextW(dc, time.c_str(), static_cast<int>(time.size()), &timeRect,
                  DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(dc, oldFont);

        HFONT iconFont = CreateFontW(
            -ClassicScale(hwnd, 13), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe Fluent Icons");
        HGDIOBJ previousFont = SelectObject(dc, iconFont);
        SetBkMode(dc, TRANSPARENT);
        PCWSTR glyphs[] = {L"\xE892", state.isPlaying ? L"\xE769" : L"\xE768",
                           L"\xE893"};
        bool enabled[] = {state.canPrevious,
                          state.canPlayPause || state.canPlay, state.canNext};
        for (int i = 0; i < 3; i++) {
            SetTextColor(dc, enabled[i]
                                 ? textColor
                                 : BlendColor(background, textColor, 0.38));
            DrawTextW(dc, glyphs[i], 1, &buttonRects[i],
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }
        SelectObject(dc, previousFont);
        DeleteObject(iconFont);

        RECT progressRect = ClassicProgressRect(hwnd);
        COLORREF trackColor = light ? RGB(177, 184, 194) : RGB(74, 82, 94);
        FillClassicRoundRect(dc, progressRect,
                             progressRect.bottom - progressRect.top,
                             trackColor);
        double ratio = state.durationMs > 0
                           ? ClampDouble(static_cast<double>(
                                             EffectivePosition(state)) /
                                             state.durationMs,
                                         0.0, 1.0)
                           : 0.0;
        RECT fillRect = progressRect;
        fillRect.right = fillRect.left + static_cast<int>(
                                             (fillRect.right - fillRect.left) *
                                             ratio);
        if (fillRect.right > fillRect.left) {
            FillClassicRoundRect(dc, fillRect,
                                 progressRect.bottom - progressRect.top,
                                 ProgressAccentColor(state));
        }

        if (g_classicProgressHover) {
            int x = ClampInt(g_classicProgressHoverX, progressRect.left,
                             progressRect.right);
            int radius = static_cast<int>(std::max<LONG>(
                2, (progressRect.bottom - progressRect.top) / 2));
            HBRUSH thumb =
                CreateSolidBrush(ProgressThumbColor(ProgressAccentColor(state)));
            HGDIOBJ oldBrush = SelectObject(dc, thumb);
            HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
            ::Ellipse(dc, x - radius,
                      (progressRect.top + progressRect.bottom) / 2 - radius,
                      x + radius + 1,
                      (progressRect.top + progressRect.bottom) / 2 + radius + 1);
            SelectObject(dc, oldPen);
            SelectObject(dc, oldBrush);
            DeleteObject(thumb);

            double hoverRatio = static_cast<double>(x - progressRect.left) /
                                std::max<LONG>(
                                    1, progressRect.right - progressRect.left);
            std::wstring tooltip =
                FormatTime(static_cast<int64_t>(state.durationMs * hoverRatio));
            int tipWidth = ClassicScale(hwnd, 42);
            int tipHeight = ClassicScale(hwnd, 18);
            int tipLeft = ClampInt(x - tipWidth / 2, 0, width - tipWidth);
            LONG tipTop =
                std::max<LONG>(0, progressRect.top - tipHeight - 1);
            RECT tipRect{tipLeft, tipTop, tipLeft + tipWidth,
                         tipTop + tipHeight};
            FillClassicRoundRect(dc, tipRect, ClassicScale(hwnd, 5),
                                 light ? RGB(255, 255, 255)
                                       : RGB(24, 27, 32));
            HGDIOBJ oldTipFont = SelectObject(dc, artistFont);
            SetTextColor(dc, textColor);
            DrawTextW(dc, tooltip.c_str(), static_cast<int>(tooltip.size()),
                      &tipRect,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            SelectObject(dc, oldTipFont);
        }

        DeleteObject(titleFont);
        DeleteObject(artistFont);
        DeleteObject(timeFont);
    }

    BitBlt(targetDc, 0, 0, width, height, dc, 0, 0, SRCCOPY);
    SelectObject(dc, oldBitmap);
    DeleteObject(buffer);
    DeleteDC(dc);
}

bool HasClassicTaskbarHierarchy(HWND taskbarWnd) {
    if (!taskbarWnd) {
        return false;
    }
    return FindWindowExW(taskbarWnd, nullptr, L"ReBarWindow32", nullptr) ||
           FindWindowExW(taskbarWnd, nullptr, L"TrayNotifyWnd", nullptr);
}

bool IsFullscreenWindowOnTaskbarMonitor(HWND taskbarWnd) {
    HWND foreground = GetForegroundWindow();
    if (!foreground || foreground == taskbarWnd ||
        foreground == g_classicWidgetWnd || !IsWindowVisible(foreground)) {
        return false;
    }

    RECT foregroundRect{};
    RECT monitorRect{};
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    HMONITOR monitor = MonitorFromWindow(taskbarWnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo) ||
        !GetWindowRect(GetAncestor(foreground, GA_ROOT), &foregroundRect)) {
        return false;
    }
    monitorRect = monitorInfo.rcMonitor;
    constexpr int kTolerance = 2;
    return foregroundRect.left <= monitorRect.left + kTolerance &&
           foregroundRect.top <= monitorRect.top + kTolerance &&
           foregroundRect.right >= monitorRect.right - kTolerance &&
           foregroundRect.bottom >= monitorRect.bottom - kTolerance;
}

bool ClassicBackendWanted(HWND taskbarWnd) {
    if (g_modernWidgetAttached.load()) {
        return false;
    }
    if (HasClassicTaskbarHierarchy(taskbarWnd) ||
        !g_taskbarHooksInstalled.load()) {
        return true;
    }
    return GetTickCount64() - g_backendDetectionStartedTick >= 8000;
}

void RepositionClassicWidget() {
    HWND hwnd = g_classicWidgetWnd;
    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hwnd || !taskbarWnd || !ClassicBackendWanted(taskbarWnd)) {
        if (hwnd) {
            ShowWindow(hwnd, SW_HIDE);
        }
        return;
    }

    MediaState state;
    {
        std::lock_guard lock(g_mediaMutex);
        state = g_mediaState;
    }
    bool idleVisible = !state.hasSession && !g_idleApps.empty();
    bool shouldShow = state.hasSession || idleVisible ||
                      !g_settings.hideWhenNoMedia;
    if (!shouldShow || IsFullscreenWindowOnTaskbarMonitor(taskbarWnd)) {
        ShowWindow(hwnd, SW_HIDE);
        return;
    }

    RECT taskbarRect{};
    if (!GetWindowRect(taskbarWnd, &taskbarRect)) {
        ShowWindow(hwnd, SW_HIDE);
        return;
    }

    int taskbarWidth = taskbarRect.right - taskbarRect.left;
    int taskbarHeight = taskbarRect.bottom - taskbarRect.top;
    if (taskbarWidth < taskbarHeight) {
        // Classic vertical taskbars don't have enough horizontal room for the
        // full player. Keep it adjacent to the taskbar instead of covering it.
        int width = ClassicDesiredWidth(hwnd, state);
        int height = ClassicScale(hwnd, g_settings.widgetHeight);
        int x = taskbarRect.left == 0 ? taskbarRect.right + 2
                                     : taskbarRect.left - width - 2;
        int y = taskbarRect.top + 4;
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        return;
    }

    int height = std::min(ClassicScale(hwnd, g_settings.widgetHeight),
                          std::max(30, taskbarHeight - 4));
    int width = ClassicDesiredWidth(hwnd, state);
    int x = taskbarRect.right - width - 4;
    HWND trayNotify =
        FindWindowExW(taskbarWnd, nullptr, L"TrayNotifyWnd", nullptr);
    RECT trayRect{};
    if (trayNotify && GetWindowRect(trayNotify, &trayRect) &&
        trayRect.left - width - 4 >= taskbarRect.left) {
        x = trayRect.left - width - 4;
    }
    int y = taskbarRect.top + (taskbarHeight - height) / 2;

    static int previousWidth = 0;
    static int previousHeight = 0;
    if (width != previousWidth || height != previousHeight) {
        HRGN region = CreateRoundRectRgn(0, 0, width + 1, height + 1,
                                         ClassicScale(hwnd, 8),
                                         ClassicScale(hwnd, 8));
        if (!SetWindowRgn(hwnd, region, FALSE)) {
            DeleteObject(region);
        }
        previousWidth = width;
        previousHeight = height;
    }

    SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

int ClassicHotButtonAt(HWND hwnd, POINT point) {
    for (int command = 1; command <= 3; command++) {
        if (PointInClassicRect(ClassicButtonRect(hwnd, command), point)) {
            return command;
        }
    }
    return 0;
}

LRESULT CALLBACK ClassicWidgetWndProc(HWND hwnd,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            SetTimer(hwnd, 1, 1000, nullptr);
            SetTimer(hwnd, 2, 500, nullptr);
            return 0;

        case WM_TIMER:
            if (wParam == 1) {
                MediaState state;
                {
                    std::lock_guard lock(g_mediaMutex);
                    state = g_mediaState;
                }
                bool animate = state.hasSession && state.isPlaying;
                SetTimer(hwnd, 1, animate ? 33 : 1000, nullptr);
                InvalidateRect(hwnd, nullptr, FALSE);
            } else if (wParam == 2) {
                RepositionClassicWidget();
            }
            return 0;

        case WM_SETTINGCHANGE:
        case WM_DISPLAYCHANGE:
            RepositionClassicWidget();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                POINT point{};
                GetCursorPos(&point);
                ScreenToClient(hwnd, &point);
                MediaState state;
                {
                    std::lock_guard lock(g_mediaMutex);
                    state = g_mediaState;
                }
                bool interactive = false;
                if (state.hasSession) {
                    interactive = ClassicHotButtonAt(hwnd, point) != 0 ||
                                  PointInClassicRect(ClassicCoverRect(hwnd),
                                                     point) ||
                                  PointInClassicRect(
                                      ClassicProgressHitRect(hwnd), point);
                } else {
                    size_t count = std::min<size_t>(g_idleApps.size(), 6);
                    for (size_t i = 0; i < count; i++) {
                        interactive = interactive || PointInClassicRect(
                                                           ClassicIdleIconRect(
                                                               hwnd, i, count),
                                                           point);
                    }
                }
                SetCursor(LoadCursorW(nullptr,
                                      interactive ? IDC_HAND : IDC_ARROW));
                return TRUE;
            }
            break;

        case WM_MOUSEMOVE: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            MediaState state;
            {
                std::lock_guard lock(g_mediaMutex);
                state = g_mediaState;
            }
            g_classicHotButton = state.hasSession
                                     ? ClassicHotButtonAt(hwnd, point)
                                     : 0;
            g_classicProgressHover =
                state.hasSession && state.canSeek && state.durationMs > 0 &&
                PointInClassicRect(ClassicProgressHitRect(hwnd), point);
            if (g_classicProgressHover) {
                g_classicProgressHoverX = point.x;
            }
            TRACKMOUSEEVENT tracking{sizeof(tracking), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tracking);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case WM_MOUSELEAVE:
            g_classicHotButton = 0;
            g_classicProgressHover = false;
            g_classicProgressHoverX = 0;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_MOUSEWHEEL:
            ChangeVolumeFromWheel(GET_WHEEL_DELTA_WPARAM(wParam));
            return 0;

        case WM_LBUTTONDOWN: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            MediaState state;
            {
                std::lock_guard lock(g_mediaMutex);
                state = g_mediaState;
            }
            if (!state.hasSession) {
                size_t count = std::min<size_t>(g_idleApps.size(), 6);
                for (size_t i = 0; i < count; i++) {
                    if (PointInClassicRect(
                            ClassicIdleIconRect(hwnd, i, count), point)) {
                        LaunchIdleApp(g_idleApps[i].shortcutPath);
                        return 0;
                    }
                }
                return 0;
            }

            if (PointInClassicRect(ClassicProgressHitRect(hwnd), point) &&
                state.canSeek && state.durationMs > 0) {
                RECT progress = ClassicProgressRect(hwnd);
                SeekClassicWidget(
                    static_cast<double>(point.x - progress.left) /
                    std::max<LONG>(1, progress.right - progress.left));
            } else if (PointInClassicRect(ClassicCoverRect(hwnd), point)) {
                OpenCurrentMediaApp();
            } else {
                int command = ClassicHotButtonAt(hwnd, point);
                bool enabled = command == 1   ? state.canPrevious
                               : command == 2 ? state.canPlayPause || state.canPlay
                               : command == 3 ? state.canNext
                                              : false;
                if (enabled) {
                    SendMediaCommand(command);
                }
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT paint{};
            HDC dc = BeginPaint(hwnd, &paint);
            PaintClassicWidget(hwnd, dc);
            EndPaint(hwnd, &paint);
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            KillTimer(hwnd, 2);
            g_classicWidgetWnd = nullptr;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void ClassicWidgetThread() {
    ApartmentScope apartment;
    HMODULE module = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<PCWSTR>(&ClassicWidgetWndProc), &module);
    HINSTANCE instance = module ? module : GetModuleHandleW(nullptr);
    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = ClassicWidgetWndProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = L"WindhawkTaskbarMediaWidgetClassic";
    if (!RegisterClassW(&windowClass) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"Classic backend: RegisterClass failed: %u", GetLastError());
        return;
    }

    g_classicWidgetWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
        windowClass.lpszClassName, L"Taskbar Media Widget", WS_POPUP, 0, 0,
        g_settings.widgetWidth, g_settings.widgetHeight, nullptr, nullptr,
        instance, nullptr);
    if (!g_classicWidgetWnd) {
        Wh_Log(L"Classic backend: CreateWindowEx failed: %u", GetLastError());
        UnregisterClassW(windowClass.lpszClassName, instance);
        return;
    }

    g_classicBackendStarted = true;
    RepositionClassicWidget();
    MSG message{};
    while (!g_unloading && !g_workersStopping &&
           GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    if (g_classicWidgetWnd) {
        DestroyWindow(g_classicWidgetWnd);
    }
    UnregisterClassW(windowClass.lpszClassName, instance);
    g_classicBackendStarted = false;
}

void MediaThread() {
    ApartmentScope apartment;
    while (!g_unloading && !g_workersStopping) {
        PollMedia();
        for (int i = 0;
             i < 10 && !g_unloading && !g_workersStopping; i++) {
            Sleep(100);
        }
    }
}

bool HookTaskbarDllSymbols();

void UiTickThread() {
    ULONGLONG lastHookAttempt = 0;
    ULONGLONG lastInjectAttempt = 0;

    while (!g_unloading && !g_workersStopping) {
        ULONGLONG now = GetTickCount64();
        int nextTickDelay = 250;

        if (!g_taskbarHooksInstalled.load() && now - lastHookAttempt > 1000) {
            HookTaskbarDllSymbols();
            lastHookAttempt = now;
        }

        HWND hwnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
        if (hwnd) {
            g_taskbarWnd = hwnd;
            TaskbarEdge currentEdge = DetectTaskbarEdge(hwnd);
            int currentThickness = TaskbarThicknessPx(hwnd, currentEdge);
            if (g_modernWidgetAttached.load() &&
                currentEdge != TaskbarEdge::Unknown &&
                (currentEdge != g_injectedTaskbarEdge.load() ||
                 (currentThickness > 0 &&
                  currentThickness != g_injectedTaskbarThicknessPx.load()))) {
                RunFromWindowThread(
                    hwnd,
                    [](void*) {
                        RemoveWidgetGrid();
                        InjectWidgetGrid();
                    },
                    nullptr);
                lastInjectAttempt = now;
            }
            if (!g_modernWidgetAttached.load() &&
                now - lastInjectAttempt > 1000) {
                RunFromWindowThread(hwnd, [](void*) { InjectWidgetGrid(); },
                                    nullptr);
                lastInjectAttempt = now;
            }

            if (g_modernWidgetAttached.load() && g_uiDirty.exchange(false)) {
                RunFromWindowThread(hwnd, [](void*) { RefreshWidgetContents(); },
                                    nullptr);
            } else if (g_modernWidgetAttached.load()) {
                RunFromWindowThread(hwnd, [](void*) { TickWidgetUi(); },
                                    nullptr);
            }

            if (g_modernWidgetAttached.load()) {
                MediaState state;
                {
                    std::lock_guard lock(g_mediaMutex);
                    state = g_mediaState;
                }
                nextTickDelay = state.hasSession && state.isPlaying ? 33 : 1000;
            }
        }
        for (int slept = 0;
             slept < nextTickDelay && !g_unloading && !g_workersStopping;
             slept += 25) {
            Sleep(std::min(25, nextTickDelay - slept));
        }
    }
}

void ApplyWidgetOnTaskbarThread() {
    HWND hwnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!hwnd) {
        return;
    }
    g_taskbarWnd = hwnd;
    RunFromWindowThread(hwnd, [](void*) { InjectWidgetGrid(); }, nullptr);
}

using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (g_unloading) {
        return;
    }

    g_modernWidgetAttached = false;
    g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    ApplyWidgetOnTaskbarThread();
}

bool HookTaskbarDllSymbols() {
    if (g_taskbarHooksInstalled.load()) {
        return true;
    }

    HMODULE taskbarDll =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbarDll) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &Std_Ref_Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook},
    };

    if (!WindhawkUtils::HookSymbols(taskbarDll, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        return false;
    }

    g_taskbarHooksInstalled = true;
    return true;
}

BOOL Wh_ModInit() {
    if (!IsCurrentProcessShellHost()) {
        Wh_Log(L"Skipping a secondary Explorer process");
        return FALSE;
    }

    g_unloading = false;
    g_workersStopping = false;
    g_backendDetectionStartedTick = GetTickCount64();
    LoadSettings();
    ScanIdleApps();
    HookTaskbarDllSymbols();
    return TRUE;
}

void JoinAndDeleteThread(std::thread*& thread) {
    if (!thread) {
        return;
    }
    if (thread->joinable()) {
        thread->join();
    }
    delete thread;
    thread = nullptr;
}

void StartWorkerThreads() {
    g_workersStopping = false;
    g_mediaThread = new std::thread(MediaThread);
    g_audioThread = new std::thread(AudioVisualizerThread);
    g_tickThread = new std::thread(UiTickThread);
    g_classicWidgetThread = new std::thread(ClassicWidgetThread);
}

void StopWorkerThreads() {
    g_workersStopping = true;
    if (g_classicWidgetWnd) {
        PostMessageW(g_classicWidgetWnd, WM_CLOSE, 0, 0);
    }

    JoinAndDeleteThread(g_mediaThread);
    JoinAndDeleteThread(g_audioThread);
    JoinAndDeleteThread(g_tickThread);
    JoinAndDeleteThread(g_classicWidgetThread);
}

void Wh_ModAfterInit() {
    g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    ApplyWidgetOnTaskbarThread();
    StartWorkerThreads();
}

void Wh_ModUninit() {
    g_unloading = true;
    StopWorkerThreads();

    HWND hwnd = g_taskbarWnd;
    if (hwnd) {
        RunFromWindowThread(hwnd, [](void*) { RemoveWidgetGrid(); }, nullptr);
    }
}

void Wh_ModSettingsChanged() {
    StopWorkerThreads();

    HWND hwnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (hwnd) {
        g_taskbarWnd = hwnd;
        RunFromWindowThread(hwnd, [](void*) { RemoveWidgetGrid(); }, nullptr);
    }

    LoadSettings();
    ScanIdleApps();

    if (hwnd) {
        RunFromWindowThread(hwnd, [](void*) { InjectWidgetGrid(); }, nullptr);
    }
    StartWorkerThreads();
}
