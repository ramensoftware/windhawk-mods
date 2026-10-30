// ==WindhawkMod==
// @id taskbar-media-widget
// @name Taskbar Media Widget
// @description Compact media controls integrated into the Windows 10/11 taskbar.
// @version 0.3.5
// @author uMk0
// @github https://github.com/umk0
// @license MIT
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lole32 -lruntimeobject -luser32 -lgdi32 -lmsimg32 -lshell32 -lshlwapi -lwindowscodecs -lwindowsapp -lshcore -luuid -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Media Widget

A compact media widget for the Windows 10 and Windows 11 taskbar. It reads
metadata and controls playback through Global System Media Transport Controls.
The [media API](https://learn.microsoft.com/en-us/uwp/api/windows.media.control.globalsystemmediatransportcontrolssessionmanager)
requires Windows 10 version 1809 or newer, including Windows 11.

On Windows 11, the widget is inserted into the native tray XAML layout and
tracks the taskbar height automatically. On the classic 64-bit Windows 10
taskbar, including ExplorerPatcher configurations, it uses a DPI-aware Win32
child window because that taskbar has no XAML tree. It reserves space beside
the native task buttons and restores that space when hidden or unloaded.

Widgets are created on the primary and secondary taskbars enabled in Windows.
Each taskbar has independent layout and UI state; media polling and audio
capture remain shared. On Windows 11 an unavailable XAML host is retried rather
than replaced by a floating desktop overlay.

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
- per-monitor taskbar instances and DPI-aware classic layouts

Taskbar Fluent Media Player and Island Media Controls already provide rich
Windows 11 playback widgets, including audio visualization. This mod focuses
on a different compatibility and layout scope: the same UI also supports the
64-bit Windows 10/classic taskbar, native top/left/right taskbar positions, and
an idle state that becomes a compact launcher for unpinned music apps. Those
features require a second Win32 rendering backend and a distinct launcher
state rather than a small option in either Windows 11-only layout.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- widgetWidth: 330
  $name: Widget width
- widgetHeight: 40
  $name: Widget height
- coverSize: 32
  $name: Maximum cover size
  $description: Preferred size in DIPs, limited by the taskbar and text height.
- coverRadius: 7
  $name: Cover radius
- textWidth: 132
  $name: Text area width
- buttonSize: 28
  $name: Maximum button size
  $description: Preferred size in DIPs, reduced when the taskbar has less space.
- progressHeight: 3
  $name: Progress height
- progressHoverHeight: 6
  $name: Progress hover height
- accentColor: "#2EA8FF"
  $name: Accent color
  $description: Used when artwork colors are disabled or unavailable.
- useArtworkColor: true
  $name: Use artwork colors
- textColor: "#FFFFFF"
  $name: Title text color
  $description: Used with Theme mode Auto and Follow system theme disabled.
- mutedTextColor: "#B9C0CC"
  $name: Artist/time text color
  $description: Used with Theme mode Auto and Follow system theme disabled.
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
- marqueeWhenPaused: true
  $name: Scroll long titles while paused
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
#if !defined(NTDDI_VERSION) || NTDDI_VERSION < 0x0A000002
#undef NTDDI_VERSION
#define NTDDI_VERSION 0x0A000002
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
#include <commctrl.h>
#include <cassert>

// Windhawk pre-includes windows.h with an older SDK target before this file.
extern "C" WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
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
    bool useArtworkColor = true;
    bool marqueeWhenPaused = true;
    double visualizerOpacity = 0.32;
};

struct MediaState {
    bool hasSession = false;
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
    std::shared_ptr<const std::vector<uint8_t>> thumbnailBytes;
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
std::atomic<uint64_t> g_uiRevision = 1;
std::atomic<bool> g_systemLightTheme = false;
std::atomic<bool> g_idleAppsChanged = false;
std::atomic<bool> g_taskbarHooksInstalled = false;
// Raw pointers are intentional: their destructors must not run during
// DLL_PROCESS_DETACH while Explorer is shutting down. Normal mod unload still
// joins and deletes every thread explicitly.
std::thread* g_mediaThread = nullptr;
std::thread* g_tickThread = nullptr;
std::thread* g_audioThread = nullptr;
std::thread* g_commandThread = nullptr;
std::thread* g_idleScanThread = nullptr;
std::mutex g_mediaMutex;
std::mutex g_visualizerMutex;
std::mutex g_idleAppsMutex;
MediaState g_mediaState;
[[clang::no_destroy]] GlobalSystemMediaTransportControlsSession g_mediaSession{
    nullptr};
std::array<float, kVisualizerMaxBarCount> g_visualizerBands{};
ULONGLONG g_visualizerUpdatedTick = 0;
ULONGLONG g_missingMediaSinceTick = 0;

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

struct ClassicDecodedImage {
    std::wstring key;
    UINT width = 0;
    UINT height = 0;
    std::vector<BYTE> pixels;
};


struct WidgetUiState {
    HWND taskbarWnd = nullptr;
    Grid widgetGrid{nullptr};
    FrameworkElement injectionParent{nullptr};
    ColumnDefinition insertedColumn{nullptr};
    RowDefinition insertedRow{nullptr};
    HWND classicWidgetWnd = nullptr;
    bool verticalWidgetLayout = false;
    double verticalWidgetWidth = 40.0;
    double verticalSessionHeight = 144.0;
    FrameworkElement coverContainer{nullptr};
    FrameworkElement textContainer{nullptr};
    FrameworkElement rightContainer{nullptr};
    Grid visualizerGrid{nullptr};
    Grid progressGrid{nullptr};
    RowDefinition progressRow{nullptr};
    StackPanel idlePanel{nullptr};
    Image coverImage{nullptr};
    TextBlock titleText{nullptr};
    TextBlock artistText{nullptr};
    TextBlock timeText{nullptr};
    Border prevButton{nullptr};
    Border playButton{nullptr};
    Border nextButton{nullptr};
    TextBlock prevButtonIcon{nullptr};
    TextBlock playButtonIcon{nullptr};
    TextBlock nextButtonIcon{nullptr};
    Border progressHoverTip{nullptr};
    TextBlock progressToolTipText{nullptr};
    Border progressTrack{nullptr};
    Border progressFill{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Ellipse
        progressThumb{nullptr};
    TranslateTransform titleTransform{nullptr};
    TranslateTransform artistTransform{nullptr};
    Canvas titleCanvas{nullptr};
    Canvas artistCanvas{nullptr};
    DispatcherTimer uiTimer{nullptr};
    event_token uiTimerToken{};
    bool uiTimerHasToken = false;
    int uiTimerIntervalMs = 0;
    std::unique_ptr<std::vector<std::function<void()>>>
        widgetEventRevokers;
    TextScrollState titleScroll;
    TextScrollState artistScroll;
    bool progressHover = false;
    std::wstring loadedThumbnailKey;
    std::wstring displayedTime;
    int visualizerBarCount = kVisualizerMinBarCount;
    std::wstring playButtonGlyph;
    bool prevButtonHover = false;
    bool playButtonHover = false;
    bool nextButtonHover = false;
    bool prevButtonEnabled = true;
    bool playButtonEnabled = true;
    bool nextButtonEnabled = true;
    bool modernWidgetAttached = false;
    bool modernWidgetVisible = false;
    bool classicWidgetVisible = false;
    TaskbarEdge injectedTaskbarEdge = TaskbarEdge::Unknown;
    int injectedTaskbarThicknessPx = 0;
    LayoutMetrics layout;
    ClassicDecodedImage classicCoverImage;
    std::vector<ClassicDecodedImage> classicIdleImages;
    int classicHotButton = 0;
    bool classicProgressHover = false;
    int classicProgressHoverX = 0;
    bool classicTextOverflow = false;
    uint64_t seenRevision = 0;
    double progressWidth = -1;
    bool hostSubclassed = false;
    std::atomic<bool> destroyed = false;
    HWND nativeTaskList = nullptr;
    RECT nativeArea{};
    bool haveNativeArea = false;
    bool changingNativeArea = false;
    int reservedExtent = 0;
    UINT dpi = 96;
};

// The registry is owned by UiTickThread. Each instance's UI fields and COM
// references are used and released only on that taskbar's window thread.
thread_local WidgetUiState* g_currentWidget = nullptr;
WidgetUiState& Ui() {
    assert(g_currentWidget);
    return *g_currentWidget;
}

class WidgetScope {
    WidgetUiState* previous_;
public:
    explicit WidgetScope(WidgetUiState& state)
        : previous_(std::exchange(g_currentWidget, &state)) {}
    ~WidgetScope() { g_currentWidget = previous_; }
    WidgetScope(WidgetScope const&) = delete;
    WidgetScope& operator=(WidgetScope const&) = delete;
};

template<class Handler>
auto BindWidgetHandler(Handler handler) {
    auto* state = &Ui();
    return [state, handler = std::move(handler)](auto const& sender, auto const& args) {
        if (g_unloading || g_workersStopping || state->destroyed) return;
        WidgetScope scope(*state);
        handler(sender, args);
    };
}

std::vector<IdleApp> g_idleApps;
std::atomic<unsigned> g_visibleWidgetCount = 0;

HANDLE g_audioWakeEvent = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_mediaWakeEvent = nullptr;
HANDLE g_taskbarWakeEvent = nullptr;
std::atomic<bool> g_taskbarDiscoveryRequested = true;
HINSTANCE g_widgetModule = nullptr;
constexpr PCWSTR kClassicWidgetClass = L"WindhawkTaskbarMediaWidgetClassic";

enum class CommandType {
    OpenMediaApp,
    LaunchShortcut,
    MediaControl,
    ChangeVolume,
    Seek,
};

struct CommandRequest {
    CommandType type;
    std::wstring text;
    int command = 0;
    float volumeDelta = 0;
    int64_t positionMs = 0;
};

std::mutex g_commandMutex;
std::condition_variable g_commandCondition;
std::deque<CommandRequest> g_commandQueue;

void WakeAudioThread() {
    if (g_audioWakeEvent) {
        SetEvent(g_audioWakeEvent);
    }
}

void SetWidgetVisible(bool& previous, bool visible) {
    if (std::exchange(previous, visible) != visible) {
        if (visible) ++g_visibleWidgetCount;
        else --g_visibleWidgetCount;
        WakeAudioThread();
    }
}

void SetModernWidgetVisible(bool visible) {
    SetWidgetVisible(Ui().modernWidgetVisible, visible);
}

void SetClassicWidgetVisible(bool visible) {
    SetWidgetVisible(Ui().classicWidgetVisible, visible);
}


using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
using Std_Ref_Decref_t = void(WINAPI*)(void*);

CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
CTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;
void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
Std_Ref_Decref_t Std_Ref_Decref_Original = nullptr;
void* CTaskBand_ITaskListWndSite_vftable = nullptr;

using WindowThreadProc = void (*)(void*);


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
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr,
                                       CLSCTX_INPROC_SERVER, __uuidof(IShellLinkW),
                                       shellLink.put_void()))) {
            com_ptr<IPersistFile> persistFile;
            if (SUCCEEDED(shellLink->QueryInterface(__uuidof(IPersistFile),
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

    size_t count = found.size();
    {
        std::lock_guard lock(g_idleAppsMutex);
        g_idleApps = std::move(found);
    }
    Wh_Log(L"Idle apps found: %u", static_cast<unsigned>(count));
    g_idleAppsChanged = true;
    ++g_uiRevision;
}

std::vector<IdleApp> IdleAppsSnapshot() {
    std::lock_guard lock(g_idleAppsMutex);
    return g_idleApps;
}

size_t IdleAppCount() {
    std::lock_guard lock(g_idleAppsMutex);
    return g_idleApps.size();
}

void QueueCommand(CommandRequest request) {
    if (g_unloading || g_workersStopping) {
        return;
    }
    {
        std::lock_guard lock(g_commandMutex);
        g_commandQueue.push_back(std::move(request));
    }
    g_commandCondition.notify_one();
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
    return g_settings.useArtworkColor && state.hasAccentColor
               ? state.accentColor : g_settings.accentColor;
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

    QueueCommand({CommandType::OpenMediaApp, std::move(appId)});
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
    layout.timeWidth = layout.widgetHeight <= 38 ? 52 : 58;
    int buttonWidthLimit = (g_settings.widgetWidth - layout.coverSize -
                            layout.timeWidth - 18 - 48 - 6) / 3;
    layout.buttonSize = ClampInt(
        std::min({g_settings.buttonSize, layout.contentHeight - 4, buttonWidthLimit}),
        22, 42);
    layout.controlsWidth = layout.buttonSize * 3 + 6;
    layout.rightWidth = layout.controlsWidth + layout.timeWidth;
    layout.textWidth = std::min(g_settings.textWidth,
        std::max(1, g_settings.widgetWidth - layout.coverSize - layout.rightWidth - 18));

    return layout;
}

template <typename T>
void TrackWidgetEvent(T revoker) {
    if (!Ui().widgetEventRevokers) {
        Ui().widgetEventRevokers =
            std::make_unique<std::vector<std::function<void()>>>();
    }
    auto owned = std::make_shared<T>(std::move(revoker));
    Ui().widgetEventRevokers->emplace_back([owned] { owned->revoke(); });
}

void RevokeWidgetEvents() {
    // Detached XAML elements can outlive the DLL. Remove their delegates on
    // the taskbar thread before detaching the visual tree.
    if (Ui().widgetEventRevokers) {
        for (auto& revoke : *Ui().widgetEventRevokers) {
            revoke();
        }
        Ui().widgetEventRevokers.reset();
    }
}

void ResetWidgetElementReferences() {
    RevokeWidgetEvents();
    Ui().coverContainer = nullptr;
    Ui().textContainer = nullptr;
    Ui().rightContainer = nullptr;
    Ui().visualizerGrid = nullptr;
    Ui().progressGrid = nullptr;
    Ui().progressRow = nullptr;
    Ui().idlePanel = nullptr;
    Ui().coverImage = nullptr;
    Ui().titleText = nullptr;
    Ui().artistText = nullptr;
    Ui().timeText = nullptr;
    Ui().displayedTime.clear();
    Ui().prevButton = nullptr;
    Ui().playButton = nullptr;
    Ui().nextButton = nullptr;
    Ui().prevButtonIcon = nullptr;
    Ui().playButtonIcon = nullptr;
    Ui().nextButtonIcon = nullptr;
    Ui().progressHoverTip = nullptr;
    Ui().progressToolTipText = nullptr;
    Ui().progressTrack = nullptr;
    Ui().progressFill = nullptr;
    Ui().progressThumb = nullptr;
    Ui().progressWidth = -1;
    Ui().titleTransform = nullptr;
    Ui().artistTransform = nullptr;
    Ui().titleCanvas = nullptr;
    Ui().artistCanvas = nullptr;
    Ui().loadedThumbnailKey.clear();
    Ui().progressHover = false;
    Ui().playButtonGlyph.clear();
    Ui().prevButtonHover = false;
    Ui().playButtonHover = false;
    Ui().nextButtonHover = false;
    Ui().prevButtonEnabled = true;
    Ui().playButtonEnabled = true;
    Ui().nextButtonEnabled = true;
}

double VerticalIdleWidgetHeight() {
    size_t count = std::min<size_t>(IdleAppCount(), 6);
    if (count == 0) {
        return Ui().verticalWidgetWidth;
    }

    return 8.0 + count * Ui().layout.buttonSize + (count - 1) * 4.0;
}

double IdleWidgetWidth() {
    size_t count = std::min<size_t>(IdleAppCount(), 6);
    if (count == 0) {
        return 0;
    }

    double width = 8.0 + count * Ui().layout.buttonSize + (count - 1) * 4.0;
    return ClampDouble(width, static_cast<double>(Ui().layout.buttonSize + 8),
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
    return g_settings.followSystemTheme && g_systemLightTheme.load();
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

double ParseNumberSetting(PCWSTR text, double fallback) {
    wchar_t* end = nullptr;
    double value = std::wcstod(text, &end);
    if (end == text || !std::isfinite(value)) return fallback;
    while (std::iswspace(*end)) ++end;
    return *end ? fallback : value;
}

void LoadSettings() {
    g_settings.widgetWidth = ClampInt(Wh_GetIntSetting(L"widgetWidth"), 220, 560);
    g_settings.widgetHeight = ClampInt(Wh_GetIntSetting(L"widgetHeight"), 34, 60);
    g_settings.coverSize = ClampInt(Wh_GetIntSetting(L"coverSize"), 18, 48);
    g_settings.coverRadius = ClampInt(Wh_GetIntSetting(L"coverRadius"), 0, 18);
    g_settings.textWidth = ClampInt(Wh_GetIntSetting(L"textWidth"), 72, 260);
    g_settings.buttonSize = ClampInt(Wh_GetIntSetting(L"buttonSize"), 22, 42);
    g_settings.progressHeight = ClampInt(Wh_GetIntSetting(L"progressHeight"), 2, 8);
    g_settings.progressHoverHeight =
        ClampInt(Wh_GetIntSetting(L"progressHoverHeight"),
                 g_settings.progressHeight, 12);
    g_settings.followSystemTheme = Wh_GetIntSetting(L"followSystemTheme") != 0;
    g_settings.hideWhenNoMedia = Wh_GetIntSetting(L"hideWhenNoMedia") != 0;

    auto accentColor = WindhawkUtils::StringSetting::make(L"accentColor");
    auto textColor = WindhawkUtils::StringSetting::make(L"textColor");
    auto mutedTextColor = WindhawkUtils::StringSetting::make(L"mutedTextColor");
    auto themeMode = WindhawkUtils::StringSetting::make(L"themeMode");
    auto idleApps = WindhawkUtils::StringSetting::make(L"idleApps");
    auto marqueeSpeed = WindhawkUtils::StringSetting::make(L"marqueeSpeed");
    auto volumeStep = WindhawkUtils::StringSetting::make(L"volumeStep");
    auto visualizerOpacity = WindhawkUtils::StringSetting::make(L"visualizerOpacity");

    g_settings.accentColor = ParseColor(accentColor, RGB(46, 168, 255));
    g_settings.textColor = ParseColor(textColor, RGB(255, 255, 255));
    g_settings.mutedTextColor = ParseColor(mutedTextColor, RGB(185, 192, 204));
    g_settings.themeMode = ParseThemeMode(themeMode);
    g_settings.idleApps = idleApps.get();
    g_settings.marqueeSpeed = ParseNumberSetting(marqueeSpeed, 0.55);
    g_settings.volumeStep = ParseNumberSetting(volumeStep, 0.02);
    g_settings.visualizerEnabled = Wh_GetIntSetting(L"visualizerEnabled") != 0;
    g_settings.useArtworkColor = Wh_GetIntSetting(L"useArtworkColor") != 0;
    g_settings.marqueeWhenPaused = Wh_GetIntSetting(L"marqueeWhenPaused") != 0;
    g_settings.visualizerOpacity =
        ParseNumberSetting(visualizerOpacity, 0.32);
    g_settings.marqueeSpeed = ClampDouble(g_settings.marqueeSpeed, 0.1, 3.0);
    g_settings.volumeStep = ClampDouble(g_settings.volumeStep, 0.005, 0.1);
    g_settings.visualizerOpacity =
        ClampDouble(g_settings.visualizerOpacity, 0.12, 0.38);

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
        bool executed = false;
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
                    if (!payload->executed) {
                        payload->executed = true;
                        payload->proc(payload->param);
                    }
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
    return payload.executed;
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
    wchar_t className[64]{};
    GetClassNameW(taskbarWnd, className, ARRAYSIZE(className));
    bool secondary = _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
    void* expectedVtable = secondary ? CSecondaryTaskBand_ITaskListWndSite_vftable
                                    : CTaskBand_ITaskListWndSite_vftable;
    auto getHost = secondary ? CSecondaryTaskBand_GetTaskbarHost_Original
                             : CTaskBand_GetTaskbarHost_Original;
    if (!taskbarWnd || !expectedVtable || !getHost || !TaskbarHost_FrameHeight_Original) {
        return nullptr;
    }
    HWND taskSwWnd = secondary
        ? FindWindowExW(taskbarWnd, nullptr, L"WorkerW", nullptr)
        : reinterpret_cast<HWND>(GetPropW(taskbarWnd, L"TaskbandHWND"));
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
        if (*reinterpret_cast<void**>(taskListWndSite) == expectedVtable) {
            break;
        }
        if (i == kMaxSlotsToScan) {
            Wh_Log(L"GetTaskbarXamlRoot: ITaskListWndSite not found");
            return nullptr;
        }
        taskListWndSite = reinterpret_cast<void**>(taskListWndSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getHost(taskListWndSite, taskbarHostSharedPtr);
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
#elif defined(_M_ARM64) || defined(__aarch64__)
    auto instructions =
        reinterpret_cast<DWORD const*>(TaskbarHost_FrameHeight_Original);
    if (IsReadableMemoryRange(instructions, sizeof(DWORD) * 4) &&
        instructions[0] == 0xD503237F &&
        (instructions[1] & 0xFFC07FFF) == 0xA9807BFD &&
        instructions[2] == 0x910003FD &&
        (instructions[3] & 0xFFF00FE0) == 0xF8400C00) {
        taskbarElementIUnknownOffset =
            static_cast<size_t>((instructions[3] >> 12) & 0xFF);
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
        // Do not invent playback indefinitely if the media broker stops replying.
        position += static_cast<int64_t>(
            std::min<ULONGLONG>(GetTickCount64() - state.updatedTick, 2000));
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
    if (!Ui().progressHoverTip || !Ui().progressToolTipText) {
        return;
    }

    if (UseLightTheme()) {
        Ui().progressHoverTip.Background(MakeBrush(RGB(255, 255, 255), 245));
        Ui().progressHoverTip.BorderBrush(MakeBrush(RGB(0, 0, 0), 24));
        Ui().progressToolTipText.Foreground(MakeBrush(RGB(24, 28, 36)));
    } else {
        Ui().progressHoverTip.Background(MakeBrush(RGB(20, 24, 30), 235));
        Ui().progressHoverTip.BorderBrush(MakeBrush(RGB(255, 255, 255), 22));
        Ui().progressToolTipText.Foreground(MakeBrush(RGB(255, 255, 255)));
    }
    Ui().progressHoverTip.BorderThickness({1, 1, 1, 1});
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
    button.Width(Ui().layout.buttonSize);
    button.Height(Ui().layout.buttonSize);
    button.Margin({0, 0, 0, 0});
    double radius = static_cast<double>(Ui().layout.buttonSize) / 2.0;
    button.CornerRadius({radius, radius, radius, radius});
    button.Background(MakeBrush(RGB(0, 0, 0), 0));
    button.VerticalAlignment(VerticalAlignment::Center);
    button.HorizontalAlignment(HorizontalAlignment::Center);
    button.UseLayoutRounding(true);
    iconTarget = MakeIconText(glyph, 13.0);
    button.Child(iconTarget);
    TrackWidgetEvent(button.PointerEntered(auto_revoke, BindWidgetHandler([hoverState, enabledState](
                              IInspectable const& sender,
                              PointerRoutedEventArgs const&) {
        if (enabledState && !*enabledState) {
            return;
        }
        if (hoverState) {
            *hoverState = true;
        }
        SetButtonHoverBackground(sender.as<Border>(), true);
    })));
    TrackWidgetEvent(button.PointerExited(auto_revoke, BindWidgetHandler([hoverState](IInspectable const& sender,
                                      PointerRoutedEventArgs const&) {
        if (hoverState) {
            *hoverState = false;
        }
        SetButtonHoverBackground(sender.as<Border>(), false);
    })));
    TrackWidgetEvent(button.PointerPressed(auto_revoke, BindWidgetHandler([enabledState, command](IInspectable const& sender,
                                                 PointerRoutedEventArgs const& e) {
        if (!enabledState || !*enabledState) {
            return;
        }
        SetButtonHoverBackground(sender.as<Border>(), true);
        SendMediaCommand(command);
        e.Handled(true);
    })));
    return button;
}

void LaunchIdleApp(std::wstring shortcutPath) {
    if (shortcutPath.empty()) {
        return;
    }

    QueueCommand({CommandType::LaunchShortcut, std::move(shortcutPath)});
}

Button MakeIdleButton(IdleApp const& app) {
    Button button;
    button.Width(Ui().layout.buttonSize);
    button.Height(Ui().layout.buttonSize);
    button.Padding({0, 0, 0, 0});
    button.Margin({0, 0, 0, 0});
    button.BorderThickness({0, 0, 0, 0});
    button.CornerRadius({7, 7, 7, 7});
    button.Background(MakeBrush(RGB(0, 0, 0), 0));
    button.Foreground(MakeBrush(ActiveTextColor()));

    if (!app.iconBytes.empty()) {
        Image image;
        image.Width(std::max(16, Ui().layout.buttonSize - 8));
        image.Height(std::max(16, Ui().layout.buttonSize - 8));
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
                bitmap.DecodePixelWidth(std::max(32, Ui().layout.buttonSize * 2));
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
    TrackWidgetEvent(button.PointerEntered(auto_revoke, BindWidgetHandler([](IInspectable const& sender,
                             PointerRoutedEventArgs const&) {
        sender.as<Button>().Background(
            MakeBrush(ButtonHoverColor(), UseLightTheme() ? 28 : 42));
    })));
    TrackWidgetEvent(button.PointerExited(auto_revoke, BindWidgetHandler([](IInspectable const& sender,
                            PointerRoutedEventArgs const&) {
        sender.as<Button>().Background(MakeBrush(RGB(0, 0, 0), 0));
    })));

    std::wstring shortcut = app.shortcutPath;
    TrackWidgetEvent(button.Click(auto_revoke, BindWidgetHandler([shortcut](IInspectable const&, RoutedEventArgs const&) {
        LaunchIdleApp(shortcut);
    })));
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
    canvas.Width(Ui().layout.textWidth);
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
    TrackWidgetEvent(canvas.SizeChanged(auto_revoke, BindWidgetHandler([](IInspectable const& sender, SizeChangedEventArgs const&) {
        auto canvas = sender.as<Canvas>();
        if (auto geometry = canvas.Clip().try_as<RectangleGeometry>()) {
            geometry.Rect({0, 0, static_cast<float>(canvas.ActualWidth()),
                           static_cast<float>(canvas.ActualHeight())});
        }
    })));

    return canvas;
}

void SendMediaCommand(int command) {
    QueueCommand({CommandType::MediaControl, {}, command});
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
            if (ToLower(id).find(ToLower(aumid)) == std::wstring::npos) {
                continue;
            }

            com_ptr<ISimpleAudioVolume> volume;
            if (SUCCEEDED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                         volume.put_void()))) {
                float current = 0.0f;
                if (SUCCEEDED(volume->GetMasterVolume(&current))) {
                    changed |= SUCCEEDED(volume->SetMasterVolume(
                        ClampDouble(current + delta, 0.0, 1.0), nullptr));
                }
            }
        }
    }

    return changed;
}

void ChangeSystemVolume(float delta) {
    com_ptr<IMMDeviceEnumerator> enumerator;
    check_hresult(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
        CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), enumerator.put_void()));
    com_ptr<IMMDevice> device;
    check_hresult(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, device.put()));
    com_ptr<IAudioEndpointVolume> volume;
    check_hresult(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL,
                                  nullptr, volume.put_void()));
    float current = 0;
    check_hresult(volume->GetMasterVolumeLevelScalar(&current));
    check_hresult(volume->SetMasterVolumeLevelScalar(
        static_cast<float>(ClampDouble(current + delta, 0.0, 1.0)), nullptr));
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

    float delta = static_cast<float>(g_settings.volumeStep * wheelDelta / WHEEL_DELTA);
    CommandRequest request{};
    request.type = CommandType::ChangeVolume;
    request.text = std::move(sourceAppId);
    request.volumeDelta = delta;
    QueueCommand(std::move(request));
}

void SeekMedia(int64_t absoluteTargetMs) {
    CommandRequest request{};
    request.type = CommandType::Seek;
    request.positionMs = absoluteTargetMs;
    QueueCommand(std::move(request));
}

template <typename T>
bool WaitForMediaOperation(T const& operation) {
    // wait_for installs a Completed delegate which can survive cancellation
    // and call into an unloaded mod. Poll status without registering callbacks.
    ULONGLONG deadline = GetTickCount64() + 3000;
    for (;;) {
        auto status = operation.Status();
        if (status == AsyncStatus::Completed) {
            return operation.GetResults();
        }
        if (status != AsyncStatus::Started) {
            operation.GetResults();
            return false;
        }
        if (GetTickCount64() >= deadline ||
            WaitForSingleObject(g_stopEvent, 20) != WAIT_TIMEOUT) {
            operation.Cancel();
            return false;
        }
    }
}

bool IsExecutableAppId(std::wstring const& appId) {
    return appId.size() >= 5 &&
           _wcsicmp(appId.c_str() + appId.size() - 4, L".exe") == 0 &&
           appId.find_first_of(L"\\/:\"") == std::wstring::npos;
}

bool ActivateExeMediaWindow(std::wstring const& appId) {
    // Some desktop players expose only an executable name, not an AUMID.
    // Match an existing process window; never feed an arbitrary session ID
    // to ShellExecute as an executable path.
    if (!IsExecutableAppId(appId)) return false;
    struct Search { std::wstring const* name; HWND window = nullptr; } search{&appId};
    EnumWindows([](HWND hwnd, LPARAM parameter) CALLBACK -> BOOL {
        auto& search = *reinterpret_cast<Search*>(parameter);
        if (GetWindow(hwnd, GW_OWNER) ||
            (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)) return TRUE;
        bool visible = IsWindowVisible(hwnd);
        if (!visible && !GetWindowTextLengthW(hwnd)) return TRUE;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!process) return TRUE;
        wchar_t path[32768];
        DWORD size = ARRAYSIZE(path);
        bool match = QueryFullProcessImageNameW(process, 0, path, &size) &&
            _wcsicmp(PathFindFileNameW(path), search.name->c_str()) == 0;
        CloseHandle(process);
        if (!match) return TRUE;
        if (visible || !search.window) search.window = hwnd;
        return visible ? FALSE : TRUE;
    }, reinterpret_cast<LPARAM>(&search));
    if (!search.window) return false;
    if (IsIconic(search.window) || !IsWindowVisible(search.window)) {
        ShowWindowAsync(search.window, SW_RESTORE);
    }
    SetForegroundWindow(search.window);
    return true;
}

void ExecuteShellOpen(std::wstring const& target, bool appsFolder) {
    if (appsFolder && ActivateExeMediaWindow(target)) return;
    std::wstring path = appsFolder ? L"shell:AppsFolder\\" + target : target;
    HINSTANCE result = ShellExecuteW(nullptr, L"open", path.c_str(), nullptr,
                                     nullptr, SW_SHOWNORMAL);
    if (reinterpret_cast<intptr_t>(result) <= 32) {
        Wh_Log(L"Shell open failed: target=%s code=%Id", target.c_str(),
               reinterpret_cast<intptr_t>(result));
    }
}

void ExecuteMediaControl(int command) {
    GlobalSystemMediaTransportControlsSession session{nullptr};
    {
        std::lock_guard lock(g_mediaMutex);
        session = g_mediaSession;
    }
    if (!session) {
        return;
    }

    if (command == 1) {
        WaitForMediaOperation(session.TrySkipPreviousAsync());
    } else if (command == 2) {
        auto controls = session.GetPlaybackInfo().Controls();
        if (controls.IsPlayPauseToggleEnabled()) {
            WaitForMediaOperation(session.TryTogglePlayPauseAsync());
        } else if (controls.IsPlayEnabled()) {
            WaitForMediaOperation(session.TryPlayAsync());
        }
    } else if (command == 3) {
        WaitForMediaOperation(session.TrySkipNextAsync());
    }
    ++g_uiRevision;
    SetEvent(g_mediaWakeEvent);
}

void ExecuteSeek(int64_t positionMs) {
    GlobalSystemMediaTransportControlsSession session{nullptr};
    {
        std::lock_guard lock(g_mediaMutex);
        session = g_mediaSession;
    }
    if (session && WaitForMediaOperation(
                       session.TryChangePlaybackPositionAsync(positionMs * 10000))) {
        ++g_uiRevision;
        SetEvent(g_mediaWakeEvent);
    }
}

void CommandThread() {
    ApartmentScope apartment;
    for (;;) {
        CommandRequest request{};
        request.type = CommandType::MediaControl;
        {
            std::unique_lock lock(g_commandMutex);
            g_commandCondition.wait(lock, [] {
                return g_unloading || g_workersStopping ||
                       !g_commandQueue.empty();
            });
            if ((g_unloading || g_workersStopping) && g_commandQueue.empty()) {
                break;
            }
            request = std::move(g_commandQueue.front());
            g_commandQueue.pop_front();
        }

        try {
            switch (request.type) {
                case CommandType::OpenMediaApp:
                    ExecuteShellOpen(request.text, true);
                    break;
                case CommandType::LaunchShortcut:
                    ExecuteShellOpen(request.text, false);
                    break;
                case CommandType::MediaControl:
                    ExecuteMediaControl(request.command);
                    break;
                case CommandType::ChangeVolume:
                    if (!AdjustAppVolumeByAumid(request.text,
                                                request.volumeDelta)) {
                        ChangeSystemVolume(request.volumeDelta);
                    }
                    break;
                case CommandType::Seek:
                    ExecuteSeek(request.positionMs);
                    break;
            }
        } catch (hresult_error const& e) {
            Wh_Log(L"Command worker failed: 0x%08X %s",
                   static_cast<uint32_t>(e.code()), e.message().c_str());
        } catch (...) {
            Wh_Log(L"Command worker failed");
        }
    }
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

bool VisualizerShouldCapture() {
    if (!g_settings.visualizerEnabled || g_unloading || g_workersStopping) {
        return false;
    }
    if (g_visibleWidgetCount.load() == 0) {
        return false;
    }
    std::lock_guard lock(g_mediaMutex);
    return g_mediaState.hasSession && g_mediaState.isPlaying;
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

    while (VisualizerShouldCapture()) {
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
        if (!VisualizerShouldCapture()) {
            DecayVisualizerBands(0.0f);
            WaitForSingleObject(g_audioWakeEvent, INFINITE);
            continue;
        }

        if (!RunAudioCaptureSession()) {
            DecayVisualizerBands(0.55f);
            WaitForSingleObject(g_audioWakeEvent, 1000);
        }
    }
    DecayVisualizerBands(0.0f);
    CoUninitialize();
}

void RefreshProgress(MediaState const& state);

void AttachProgressHandlers() {
    TrackWidgetEvent(Ui().progressTrack.SizeChanged(auto_revoke, BindWidgetHandler(
        [](IInspectable const&, SizeChangedEventArgs const&) {
            MediaState state;
            {
                std::lock_guard lock(g_mediaMutex);
                state = g_mediaState;
            }
            RefreshProgress(state);
        })));
    auto seekAtPointer = [](PointerRoutedEventArgs const& e) {
        MediaState state;
        {
            std::lock_guard lock(g_mediaMutex);
            state = g_mediaState;
        }
        if (!state.canSeek || state.durationMs <= 0 || !Ui().progressTrack) {
            return;
        }

        auto point = e.GetCurrentPoint(Ui().progressTrack).Position();
        double width = std::max(1.0, Ui().progressTrack.ActualWidth());
        double ratio = ClampDouble(point.X / width, 0.0, 1.0);
        int64_t targetMs = static_cast<int64_t>(state.durationMs * ratio);
        int64_t absoluteTargetMs = state.timelineStartMs + targetMs;
        SeekMedia(absoluteTargetMs);
    };

    TrackWidgetEvent(Ui().progressGrid.PointerEntered(auto_revoke, BindWidgetHandler([](IInspectable const&, PointerRoutedEventArgs const&) {
        Ui().progressHover = true;
        if (Ui().progressTrack) {
            Ui().progressTrack.Height(Ui().layout.progressHoverHeight);
        }
        if (Ui().progressFill) {
            Ui().progressFill.Height(Ui().layout.progressHoverHeight);
        }
        if (Ui().progressThumb) {
            Ui().progressThumb.Visibility(Visibility::Visible);
        }
        if (Ui().progressHoverTip) {
            Ui().progressHoverTip.Visibility(Visibility::Visible);
        }
    })));
    TrackWidgetEvent(Ui().progressGrid.PointerExited(auto_revoke, BindWidgetHandler([](IInspectable const&, PointerRoutedEventArgs const&) {
        Ui().progressHover = false;
        if (Ui().progressTrack) {
            Ui().progressTrack.Height(Ui().layout.progressHeight);
        }
        if (Ui().progressFill) {
            Ui().progressFill.Height(Ui().layout.progressHeight);
        }
        if (Ui().progressThumb) {
            Ui().progressThumb.Visibility(Visibility::Collapsed);
        }
        if (Ui().progressHoverTip) {
            Ui().progressHoverTip.Visibility(Visibility::Collapsed);
        }
    })));
    TrackWidgetEvent(Ui().progressGrid.PointerMoved(auto_revoke, BindWidgetHandler([](IInspectable const&, PointerRoutedEventArgs const& e) {
        if (!Ui().progressThumb || !Ui().progressHover) {
            return;
        }
        auto point = e.GetCurrentPoint(Ui().progressGrid).Position();
        double width = std::max(1.0, Ui().progressGrid.ActualWidth());
        double x = ClampDouble(point.X, 0.0, width);
        Ui().progressThumb.Margin({x - Ui().progressThumb.Width() / 2.0, 0, 0, 0});

        if (Ui().progressHoverTip && Ui().progressToolTipText) {
            MediaState state;
            {
                std::lock_guard lock(g_mediaMutex);
                state = g_mediaState;
            }
            if (state.durationMs > 0) {
                int64_t targetMs =
                    static_cast<int64_t>(state.durationMs * (x / width));
                Ui().progressToolTipText.Text(FormatTime(targetMs));
            }

            double tipWidth = Ui().progressHoverTip.Width();
            double tipHeight = Ui().progressHoverTip.Height();
            double left = ClampDouble(x - tipWidth / 2.0, 0.0,
                                      std::max(0.0, width - tipWidth));
            double top = std::max(0.0, static_cast<double>(Ui().layout.widgetHeight) -
                                           Ui().layout.progressHoverHeight -
                                           tipHeight - 1.0);
            Canvas::SetLeft(Ui().progressHoverTip, left);
            Canvas::SetTop(Ui().progressHoverTip, top);
        }
    })));
    TrackWidgetEvent(Ui().progressGrid.PointerPressed(auto_revoke, BindWidgetHandler([seekAtPointer](IInspectable const&,
                                                  PointerRoutedEventArgs const& e) {
        seekAtPointer(e);
        e.Handled(true);
    })));

}

Grid BuildVerticalWidgetGrid(double availableWidth) {
    ResetWidgetElementReferences();
    Ui().verticalWidgetLayout = true;

    int width = ClampInt(
        availableWidth > 0 ? static_cast<int>(std::round(availableWidth)) : 40,
        32, 80);
    Ui().verticalWidgetWidth = width;
    Ui().layout = CalculateLayout(g_settings.widgetHeight);
    Ui().layout.coverSize =
        ClampInt(std::min(g_settings.coverSize, width - 8), 18, 48);
    Ui().layout.coverRadius =
        ClampInt(std::min(g_settings.coverRadius, Ui().layout.coverSize / 3), 0, 14);
    Ui().layout.buttonSize =
        ClampInt(std::min(g_settings.buttonSize, width - 4), 22, 42);
    Ui().layout.progressHeight = ClampInt(g_settings.progressHeight, 2, 5);
    Ui().layout.progressHoverHeight =
        ClampInt(g_settings.progressHoverHeight, Ui().layout.progressHeight, 7);
    Ui().layout.timeFont = 9.0;
    Ui().verticalSessionHeight =
        8.0 + Ui().layout.coverSize + 4.0 + Ui().layout.buttonSize * 3.0 + 4.0 +
        14.0 + Ui().layout.progressHoverHeight;
    Ui().layout.widgetHeight = static_cast<int>(std::round(Ui().verticalSessionHeight));

    Grid wrapper;
    wrapper.Name(L"TaskbarMediaWidget");
    wrapper.Width(Ui().verticalWidgetWidth);
    wrapper.Height(Ui().verticalSessionHeight);
    wrapper.Background(MakeBrush(RGB(0, 0, 0), 1));
    wrapper.HorizontalAlignment(HorizontalAlignment::Center);
    wrapper.VerticalAlignment(VerticalAlignment::Center);

    RowDefinition contentRow;
    contentRow.Height({1.0, GridUnitType::Star});
    Ui().progressRow = RowDefinition();
    Ui().progressRow.Height(
        {static_cast<double>(Ui().layout.progressHoverHeight), GridUnitType::Pixel});
    wrapper.RowDefinitions().Append(contentRow);
    wrapper.RowDefinitions().Append(Ui().progressRow);

    StackPanel sessionPanel;
    sessionPanel.Name(L"TaskbarMediaWidget_VerticalSession");
    sessionPanel.Orientation(Orientation::Vertical);
    sessionPanel.Spacing(1);
    sessionPanel.HorizontalAlignment(HorizontalAlignment::Center);
    sessionPanel.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetRow(sessionPanel, 0);
    wrapper.Children().Append(sessionPanel);
    Ui().rightContainer = sessionPanel;

    Border coverClip;
    coverClip.Width(Ui().layout.coverSize);
    coverClip.Height(Ui().layout.coverSize);
    coverClip.CornerRadius({static_cast<double>(Ui().layout.coverRadius),
                            static_cast<double>(Ui().layout.coverRadius),
                            static_cast<double>(Ui().layout.coverRadius),
                            static_cast<double>(Ui().layout.coverRadius)});
    coverClip.Background(MakeBrush(RGB(46, 62, 82), 120));
    coverClip.HorizontalAlignment(HorizontalAlignment::Center);
    coverClip.Margin({0, 1, 0, 2});
    ToolTipService::SetToolTip(coverClip, box_value(L"Open media app"));
    TrackWidgetEvent(coverClip.PointerEntered(auto_revoke, BindWidgetHandler([](IInspectable const&, PointerRoutedEventArgs const&) {
        SetPointerCursor(winrt::Windows::UI::Core::CoreCursorType::Hand);
    })));
    TrackWidgetEvent(coverClip.PointerExited(auto_revoke, BindWidgetHandler([](IInspectable const&, PointerRoutedEventArgs const&) {
        SetPointerCursor(winrt::Windows::UI::Core::CoreCursorType::Arrow);
    })));
    TrackWidgetEvent(coverClip.PointerPressed(auto_revoke, BindWidgetHandler([](IInspectable const&, PointerRoutedEventArgs const& e) {
        OpenCurrentMediaApp();
        e.Handled(true);
    })));

    Ui().coverImage = Image();
    Ui().coverImage.Stretch(Stretch::UniformToFill);
    coverClip.Child(Ui().coverImage);
    sessionPanel.Children().Append(coverClip);
    Ui().coverContainer = coverClip;

    Ui().prevButton = MakeButton(L"TaskbarMediaWidget_Previous", L"\xE892",
                              Ui().prevButtonIcon, &Ui().prevButtonHover,
                              &Ui().prevButtonEnabled, 1);
    Ui().playButton = MakeButton(L"TaskbarMediaWidget_PlayPause", L"\xE768",
                              Ui().playButtonIcon, &Ui().playButtonHover,
                              &Ui().playButtonEnabled, 2);
    Ui().nextButton = MakeButton(L"TaskbarMediaWidget_Next", L"\xE893",
                              Ui().nextButtonIcon, &Ui().nextButtonHover,
                              &Ui().nextButtonEnabled, 3);
    sessionPanel.Children().Append(Ui().prevButton);
    sessionPanel.Children().Append(Ui().playButton);
    sessionPanel.Children().Append(Ui().nextButton);

    Ui().timeText = TextBlock();
    Ui().timeText.FontSize(Ui().layout.timeFont);
    Ui().timeText.FontFamily(FontFamily(L"Segoe UI"));
    Ui().timeText.Foreground(MakeBrush(ActiveMutedTextColor()));
    Ui().timeText.TextAlignment(TextAlignment::Center);
    Ui().timeText.HorizontalAlignment(HorizontalAlignment::Stretch);
    Ui().timeText.Margin({0, 1, 0, 0});
    sessionPanel.Children().Append(Ui().timeText);

    Ui().idlePanel = StackPanel();
    Ui().idlePanel.Orientation(Orientation::Vertical);
    Ui().idlePanel.Spacing(4);
    Ui().idlePanel.HorizontalAlignment(HorizontalAlignment::Center);
    Ui().idlePanel.VerticalAlignment(VerticalAlignment::Center);
    Ui().idlePanel.Margin({0, 4, 0, 4});
    Ui().idlePanel.Visibility(Visibility::Collapsed);
    auto idleApps = IdleAppsSnapshot();
    for (size_t i = 0; i < idleApps.size() && i < 6; i++) {
        Ui().idlePanel.Children().Append(MakeIdleButton(idleApps[i]));
    }
    Grid::SetRow(Ui().idlePanel, 0);
    wrapper.Children().Append(Ui().idlePanel);

    Ui().progressGrid = Grid();
    Ui().progressGrid.Height(Ui().layout.progressHoverHeight);
    Ui().progressGrid.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetRow(Ui().progressGrid, 1);
    wrapper.Children().Append(Ui().progressGrid);

    Canvas progressOverlay;
    progressOverlay.IsHitTestVisible(false);
    Canvas::SetZIndex(progressOverlay, 20);
    Grid::SetRowSpan(progressOverlay, 2);
    wrapper.Children().Append(progressOverlay);

    Ui().progressHoverTip = Border();
    Ui().progressHoverTip.Width(std::max(30.0, Ui().verticalWidgetWidth - 2.0));
    Ui().progressHoverTip.Height(17);
    Ui().progressHoverTip.CornerRadius({4, 4, 4, 4});
    Ui().progressHoverTip.Visibility(Visibility::Collapsed);

    Ui().progressToolTipText = TextBlock();
    Ui().progressToolTipText.FontFamily(FontFamily(L"Segoe UI"));
    Ui().progressToolTipText.FontSize(9.0);
    Ui().progressToolTipText.Text(L"0:00");
    Ui().progressToolTipText.HorizontalAlignment(HorizontalAlignment::Center);
    Ui().progressToolTipText.VerticalAlignment(VerticalAlignment::Center);
    Ui().progressToolTipText.TextAlignment(TextAlignment::Center);
    Ui().progressHoverTip.Child(Ui().progressToolTipText);
    UpdateProgressHoverTipTheme();
    progressOverlay.Children().Append(Ui().progressHoverTip);

    Ui().progressTrack = Border();
    Ui().progressTrack.Height(Ui().layout.progressHeight);
    Ui().progressTrack.CornerRadius({4, 4, 4, 4});
    Ui().progressTrack.Background(MakeBrush(UseLightTheme() ? RGB(132, 142, 156)
                                                        : RGB(72, 82, 99),
                                         120));
    Ui().progressTrack.HorizontalAlignment(HorizontalAlignment::Stretch);
    Ui().progressTrack.VerticalAlignment(VerticalAlignment::Center);
    Ui().progressGrid.Children().Append(Ui().progressTrack);

    Ui().progressFill = Border();
    Ui().progressFill.Height(Ui().layout.progressHeight);
    Ui().progressFill.CornerRadius({4, 4, 4, 4});
    Ui().progressFill.Background(MakeBrush(g_settings.accentColor));
    Ui().progressFill.HorizontalAlignment(HorizontalAlignment::Left);
    Ui().progressFill.VerticalAlignment(VerticalAlignment::Center);
    Ui().progressGrid.Children().Append(Ui().progressFill);

    Ui().progressThumb = winrt::Windows::UI::Xaml::Shapes::Ellipse();
    Ui().progressThumb.Width(std::max(5, Ui().layout.progressHoverHeight));
    Ui().progressThumb.Height(std::max(5, Ui().layout.progressHoverHeight));
    Ui().progressThumb.Fill(MakeBrush(ProgressThumbColor(g_settings.accentColor)));
    Ui().progressThumb.Visibility(Visibility::Collapsed);
    Ui().progressThumb.HorizontalAlignment(HorizontalAlignment::Left);
    Ui().progressThumb.VerticalAlignment(VerticalAlignment::Center);
    Ui().progressGrid.Children().Append(Ui().progressThumb);

    AttachProgressHandlers();

    TrackWidgetEvent(wrapper.PointerWheelChanged(auto_revoke, BindWidgetHandler([](IInspectable const&, PointerRoutedEventArgs const& e) {
        ChangeVolumeFromWheel(e.GetCurrentPoint(nullptr).Properties().MouseWheelDelta());
        e.Handled(true);
    })));

    return wrapper;
}

Grid BuildWidgetGrid(double availableHeight) {
    ResetWidgetElementReferences();
    Ui().verticalWidgetLayout = false;
    Ui().layout = CalculateLayout(availableHeight);

    Grid wrapper;
    wrapper.Name(L"TaskbarMediaWidget");
    wrapper.Width(g_settings.widgetWidth);
    wrapper.Height(Ui().layout.widgetHeight);
    wrapper.MinHeight(Ui().layout.widgetHeight);
    wrapper.Background(MakeBrush(RGB(0, 0, 0), 1));
    wrapper.VerticalAlignment(VerticalAlignment::Center);
    RowDefinition contentRow;
    contentRow.Height({1.0, GridUnitType::Star});
    Ui().progressRow = RowDefinition();
    Ui().progressRow.Height(
        {static_cast<double>(Ui().layout.progressHoverHeight),
         GridUnitType::Pixel});
    wrapper.RowDefinitions().Append(contentRow);
    wrapper.RowDefinitions().Append(Ui().progressRow);

    Ui().visualizerBarCount =
        VisualizerBarCountForWidth(static_cast<double>(g_settings.widgetWidth));
    Ui().visualizerGrid = Grid();
    Ui().visualizerGrid.IsHitTestVisible(false);
    Ui().visualizerGrid.VerticalAlignment(VerticalAlignment::Stretch);
    Ui().visualizerGrid.HorizontalAlignment(HorizontalAlignment::Stretch);
    Ui().visualizerGrid.Margin({4, 3, 4, 3});
    Ui().visualizerGrid.Visibility(Visibility::Collapsed);
    Grid::SetRow(Ui().visualizerGrid, 0);
    Canvas::SetZIndex(Ui().visualizerGrid, 0);

    for (int i = 0; i < Ui().visualizerBarCount; i++) {
        ColumnDefinition column;
        column.Width({1.0, GridUnitType::Star});
        Ui().visualizerGrid.ColumnDefinitions().Append(column);

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
        Ui().visualizerGrid.Children().Append(bar);
    }
    wrapper.Children().Append(Ui().visualizerGrid);

    Grid content;
    content.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetRow(content, 0);
    Canvas::SetZIndex(content, 10);
    wrapper.Children().Append(content);

    ColumnDefinition coverCol;
    coverCol.Width({Ui().layout.coverSize + 6.0, GridUnitType::Pixel});
    ColumnDefinition textCol;
    textCol.Width({static_cast<double>(Ui().layout.textWidth), GridUnitType::Pixel});
    ColumnDefinition rightCol;
    rightCol.Width({static_cast<double>(Ui().layout.rightWidth), GridUnitType::Pixel});
    content.ColumnDefinitions().Append(coverCol);
    content.ColumnDefinitions().Append(textCol);
    content.ColumnDefinitions().Append(rightCol);

    Border coverClip;
    coverClip.Width(Ui().layout.coverSize);
    coverClip.Height(Ui().layout.coverSize);
    coverClip.CornerRadius({static_cast<double>(Ui().layout.coverRadius),
                            static_cast<double>(Ui().layout.coverRadius),
                            static_cast<double>(Ui().layout.coverRadius),
                            static_cast<double>(Ui().layout.coverRadius)});
    coverClip.Background(MakeBrush(RGB(46, 62, 82), 120));
    coverClip.VerticalAlignment(VerticalAlignment::Center);
    coverClip.HorizontalAlignment(HorizontalAlignment::Center);
    coverClip.Margin({0, 3, 0, 0});
    TrackWidgetEvent(coverClip.PointerEntered(auto_revoke, BindWidgetHandler([](IInspectable const&, PointerRoutedEventArgs const&) {
        SetPointerCursor(winrt::Windows::UI::Core::CoreCursorType::Hand);
    })));
    TrackWidgetEvent(coverClip.PointerExited(auto_revoke, BindWidgetHandler([](IInspectable const&, PointerRoutedEventArgs const&) {
        SetPointerCursor(winrt::Windows::UI::Core::CoreCursorType::Arrow);
    })));
    TrackWidgetEvent(coverClip.PointerPressed(auto_revoke, BindWidgetHandler([](IInspectable const&, PointerRoutedEventArgs const& e) {
        OpenCurrentMediaApp();
        e.Handled(true);
    })));

    Ui().coverImage = Image();
    Ui().coverImage.Stretch(Stretch::UniformToFill);
    coverClip.Child(Ui().coverImage);
    Grid::SetColumn(coverClip, 0);
    content.Children().Append(coverClip);
    Ui().coverContainer = coverClip;

    StackPanel textStack;
    textStack.Orientation(Orientation::Vertical);
    textStack.VerticalAlignment(VerticalAlignment::Center);
    textStack.Width(Ui().layout.textWidth);
    textStack.Spacing(-1);
    Ui().titleCanvas = MakeMarqueeLine(Ui().titleText, Ui().titleTransform,
                                    L"TaskbarMediaWidget_TitleCanvas",
                                    Ui().layout.titleFont,
                                    ActiveTextColor(), true);
    Ui().artistCanvas = MakeMarqueeLine(Ui().artistText, Ui().artistTransform,
                                     L"TaskbarMediaWidget_ArtistCanvas",
                                     Ui().layout.artistFont,
                                     ActiveMutedTextColor(), false);
    textStack.Children().Append(Ui().titleCanvas);
    textStack.Children().Append(Ui().artistCanvas);
    Grid::SetColumn(textStack, 1);
    content.Children().Append(textStack);
    Ui().textContainer = textStack;

    Grid rightPanel;
    rightPanel.Width(Ui().layout.rightWidth);
    rightPanel.VerticalAlignment(VerticalAlignment::Center);
    rightPanel.HorizontalAlignment(HorizontalAlignment::Right);
    rightPanel.Margin({0, 3, 0, 0});

    ColumnDefinition timeCol;
    timeCol.Width({static_cast<double>(Ui().layout.timeWidth), GridUnitType::Pixel});
    ColumnDefinition controlsCol;
    controlsCol.Width({static_cast<double>(Ui().layout.controlsWidth),
                       GridUnitType::Pixel});
    rightPanel.ColumnDefinitions().Append(timeCol);
    rightPanel.ColumnDefinitions().Append(controlsCol);

    StackPanel controls;
    controls.Orientation(Orientation::Horizontal);
    controls.Spacing(2);
    controls.VerticalAlignment(VerticalAlignment::Center);
    controls.HorizontalAlignment(HorizontalAlignment::Right);
    controls.Margin({0, 0, 0, 0});
    Ui().prevButton = MakeButton(L"TaskbarMediaWidget_Previous", L"\xE892",
                              Ui().prevButtonIcon, &Ui().prevButtonHover,
                              &Ui().prevButtonEnabled, 1);
    Ui().playButton = MakeButton(L"TaskbarMediaWidget_PlayPause", L"\xE768",
                              Ui().playButtonIcon, &Ui().playButtonHover,
                              &Ui().playButtonEnabled, 2);
    Ui().nextButton = MakeButton(L"TaskbarMediaWidget_Next", L"\xE893",
                              Ui().nextButtonIcon, &Ui().nextButtonHover,
                              &Ui().nextButtonEnabled, 3);
    controls.Children().Append(Ui().prevButton);
    controls.Children().Append(Ui().playButton);
    controls.Children().Append(Ui().nextButton);

    Ui().timeText = TextBlock();
    Ui().timeText.FontSize(Ui().layout.timeFont);
    Ui().timeText.FontFamily(FontFamily(L"Segoe UI"));
    Ui().timeText.Foreground(MakeBrush(ActiveMutedTextColor()));
    Ui().timeText.TextAlignment(TextAlignment::Right);
    Ui().timeText.VerticalAlignment(VerticalAlignment::Center);
    Ui().timeText.HorizontalAlignment(HorizontalAlignment::Stretch);
    Ui().timeText.Margin({0, 0, 7, 0});
    Grid::SetColumn(Ui().timeText, 0);
    rightPanel.Children().Append(Ui().timeText);
    Grid::SetColumn(controls, 1);
    rightPanel.Children().Append(controls);
    Grid::SetColumn(rightPanel, 2);
    content.Children().Append(rightPanel);
    Ui().rightContainer = rightPanel;

    Ui().idlePanel = StackPanel();
    Ui().idlePanel.Orientation(Orientation::Horizontal);
    Ui().idlePanel.Spacing(4);
    Ui().idlePanel.VerticalAlignment(VerticalAlignment::Center);
    Ui().idlePanel.HorizontalAlignment(HorizontalAlignment::Left);
    Ui().idlePanel.Margin({4, 0, 0, 0});
    Ui().idlePanel.Visibility(Visibility::Collapsed);
    auto idleApps = IdleAppsSnapshot();
    for (size_t i = 0; i < idleApps.size() && i < 6; i++) {
        Ui().idlePanel.Children().Append(MakeIdleButton(idleApps[i]));
    }
    Grid::SetColumn(Ui().idlePanel, 0);
    Grid::SetColumnSpan(Ui().idlePanel, 3);
    content.Children().Append(Ui().idlePanel);

    Ui().progressGrid = Grid();
    Ui().progressGrid.Height(Ui().layout.progressHoverHeight);
    Ui().progressGrid.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetRow(Ui().progressGrid, 1);
    Grid::SetColumnSpan(Ui().progressGrid, 4);
    wrapper.Children().Append(Ui().progressGrid);

    Canvas progressOverlay;
    progressOverlay.IsHitTestVisible(false);
    Canvas::SetZIndex(progressOverlay, 20);
    Grid::SetRowSpan(progressOverlay, 2);
    wrapper.Children().Append(progressOverlay);

    Ui().progressHoverTip = Border();
    Ui().progressHoverTip.Width(42);
    Ui().progressHoverTip.Height(18);
    Ui().progressHoverTip.CornerRadius({4, 4, 4, 4});
    Ui().progressHoverTip.Visibility(Visibility::Collapsed);

    Ui().progressToolTipText = TextBlock();
    Ui().progressToolTipText.FontFamily(FontFamily(L"Segoe UI"));
    Ui().progressToolTipText.FontSize(10.0);
    Ui().progressToolTipText.Text(L"0:00");
    Ui().progressToolTipText.HorizontalAlignment(HorizontalAlignment::Center);
    Ui().progressToolTipText.VerticalAlignment(VerticalAlignment::Center);
    Ui().progressToolTipText.TextAlignment(TextAlignment::Center);
    Ui().progressHoverTip.Child(Ui().progressToolTipText);
    UpdateProgressHoverTipTheme();
    progressOverlay.Children().Append(Ui().progressHoverTip);

    Ui().progressTrack = Border();
    Ui().progressTrack.Height(Ui().layout.progressHeight);
    Ui().progressTrack.CornerRadius({4, 4, 4, 4});
    Ui().progressTrack.Background(MakeBrush(UseLightTheme() ? RGB(132, 142, 156)
                                                        : RGB(72, 82, 99),
                                         120));
    Ui().progressTrack.HorizontalAlignment(HorizontalAlignment::Stretch);
    Ui().progressTrack.VerticalAlignment(VerticalAlignment::Center);
    Ui().progressGrid.Children().Append(Ui().progressTrack);

    Ui().progressFill = Border();
    Ui().progressFill.Height(Ui().layout.progressHeight);
    Ui().progressFill.CornerRadius({4, 4, 4, 4});
    Ui().progressFill.Background(MakeBrush(g_settings.accentColor));
    Ui().progressFill.HorizontalAlignment(HorizontalAlignment::Left);
    Ui().progressFill.VerticalAlignment(VerticalAlignment::Center);
    Ui().progressGrid.Children().Append(Ui().progressFill);

    Ui().progressThumb = winrt::Windows::UI::Xaml::Shapes::Ellipse();
    Ui().progressThumb.Width(std::max(5, Ui().layout.progressHoverHeight));
    Ui().progressThumb.Height(std::max(5, Ui().layout.progressHoverHeight));
    Ui().progressThumb.Fill(MakeBrush(ProgressThumbColor(g_settings.accentColor)));
    Ui().progressThumb.Visibility(Visibility::Collapsed);
    Ui().progressThumb.HorizontalAlignment(HorizontalAlignment::Left);
    Ui().progressThumb.VerticalAlignment(VerticalAlignment::Center);
    Ui().progressGrid.Children().Append(Ui().progressThumb);

    AttachProgressHandlers();

    TrackWidgetEvent(wrapper.PointerWheelChanged(auto_revoke, BindWidgetHandler([](IInspectable const&, PointerRoutedEventArgs const& e) {
        ChangeVolumeFromWheel(e.GetCurrentPoint(nullptr).Properties().MouseWheelDelta());
        e.Handled(true);
    })));

    return wrapper;
}

void RefreshWidgetContents();
void TickWidgetUi();

void StopWidgetUiTimer() {
    if (!Ui().uiTimer) {
        return;
    }
    auto timer = std::exchange(Ui().uiTimer, nullptr);
    bool hasToken = std::exchange(Ui().uiTimerHasToken, false);
    Ui().uiTimerIntervalMs = 0;
    // Revoke independently of Stop: a closed XAML host can reject Stop while
    // the event source still holds its delegate.
    try {
        if (hasToken) timer.Tick(Ui().uiTimerToken);
    } catch (...) {
    }
    try {
        timer.Stop();
    } catch (...) {
    }
}

void RemoveWidgetGrid() {
    StopWidgetUiTimer();
    RevokeWidgetEvents();
    SetModernWidgetVisible(false);
    if (!Ui().injectionParent) {
        Ui().widgetGrid = nullptr;
        Ui().insertedColumn = nullptr;
        Ui().insertedRow = nullptr;
        Ui().injectedTaskbarEdge = TaskbarEdge::Unknown;
        Ui().injectedTaskbarThicknessPx = 0;
        ResetWidgetElementReferences();
        Ui().modernWidgetAttached = false;
        return;
    }

    try {
        auto targetPanel = Ui().injectionParent.try_as<Panel>();
        auto targetGrid = Ui().injectionParent.try_as<Grid>();

        uint32_t index = 0;
        if (targetPanel && Ui().widgetGrid &&
            targetPanel.Children().IndexOf(Ui().widgetGrid, index)) {
            targetPanel.Children().RemoveAt(index);
        }

        if (targetGrid && Ui().insertedColumn &&
            targetGrid.ColumnDefinitions().IndexOf(Ui().insertedColumn, index)) {
            for (uint32_t i = 0; i < targetGrid.Children().Size(); i++) {
                auto child = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                if (child && Grid::GetColumn(child) > static_cast<int>(index)) {
                    Grid::SetColumn(child, Grid::GetColumn(child) - 1);
                }
            }
            targetGrid.ColumnDefinitions().RemoveAt(index);
        }
        if (targetGrid && Ui().insertedRow &&
            targetGrid.RowDefinitions().IndexOf(Ui().insertedRow, index)) {
            for (uint32_t i = 0; i < targetGrid.Children().Size(); i++) {
                auto child = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                if (child && Grid::GetRow(child) > static_cast<int>(index)) {
                    Grid::SetRow(child, Grid::GetRow(child) - 1);
                }
            }
            targetGrid.RowDefinitions().RemoveAt(index);
        }
    } catch (...) {
    }

    Ui().widgetGrid = nullptr;
    Ui().injectionParent = nullptr;
    Ui().insertedColumn = nullptr;
    Ui().insertedRow = nullptr;
    Ui().injectedTaskbarEdge = TaskbarEdge::Unknown;
    Ui().injectedTaskbarThicknessPx = 0;
    ResetWidgetElementReferences();
    Ui().modernWidgetAttached = false;
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
    if (!Ui().visualizerGrid) {
        return;
    }

    bool visible = g_settings.visualizerEnabled && state.hasSession;
    Ui().visualizerGrid.Visibility(visible ? Visibility::Visible
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
    double availableHeight = std::max(4.0, Ui().visualizerGrid.ActualHeight());
    double maxHeight = std::max(2.0, availableHeight - 5.0);

    auto brush = MakeBrush(accent, alpha);
    uint32_t barCount = Ui().visualizerGrid.Children().Size();
    for (uint32_t i = 0; i < barCount; i++) {
        auto bar = Ui().visualizerGrid.Children().GetAt(i).try_as<Border>();
        if (!bar) {
            continue;
        }
        size_t bandIndex = static_cast<size_t>(i) * kVisualizerMaxBarCount /
                           std::max<uint32_t>(1, barCount);
        double band = active ? ClampDouble(bands[bandIndex], 0.0, 1.0) : 0.0;
        double height = band > 0.015 ? 1.0 + band * maxHeight : 1.0;
        bar.Height(height);
        bar.Background(brush);
    }
}

void RefreshProgress(MediaState const& state) {
    if (!Ui().progressTrack || !Ui().progressFill) {
        return;
    }

    double width = std::max(1.0, Ui().progressTrack.ActualWidth());
    double ratio = 0.0;
    if (state.durationMs > 0) {
        ratio = ClampDouble(static_cast<double>(EffectivePosition(state)) /
                                static_cast<double>(state.durationMs),
                            0.0, 1.0);
    }
    double scale = Ui().progressTrack.XamlRoot()
                       ? Ui().progressTrack.XamlRoot().RasterizationScale() : 1.0;
    scale = std::max(1.0, scale);
    double pixels = std::round(width * ratio * scale);
    double fillWidth = std::min(width, pixels / scale);
    if (fillWidth != Ui().progressWidth) {
        Ui().progressFill.Width(fillWidth);
        Ui().progressWidth = fillWidth;
    }
}

void RefreshThumbnail(MediaState const& state) {
    if (!Ui().coverImage) {
        return;
    }

    if (!state.thumbnailBytes || state.thumbnailBytes->empty() ||
        state.thumbnailKey.empty()) {
        Ui().coverImage.Source(nullptr);
        Ui().loadedThumbnailKey.clear();
        return;
    }

    if (state.thumbnailKey == Ui().loadedThumbnailKey) {
        return;
    }

    try {
        IStream* rawStream = SHCreateMemStream(
            state.thumbnailBytes->data(),
            static_cast<UINT>(state.thumbnailBytes->size()));
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
        image.DecodePixelHeight(Ui().layout.coverSize > 0 ? Ui().layout.coverSize * 2
                                                       : g_settings.coverSize * 2);
        Ui().coverImage.Source(image);
        image.SetSourceAsync(randomAccessStream);
        Ui().loadedThumbnailKey = state.thumbnailKey;
    } catch (hresult_error const& e) {
        Wh_Log(L"RefreshThumbnail failed: 0x%08X %s",
               static_cast<uint32_t>(e.code()), e.message().c_str());
        Ui().coverImage.Source(nullptr);
        Ui().loadedThumbnailKey.clear();
    } catch (...) {
        Wh_Log(L"RefreshThumbnail failed");
        Ui().coverImage.Source(nullptr);
        Ui().loadedThumbnailKey.clear();
    }
}

void RefreshTimeText(MediaState const& state) {
    if (!Ui().timeText) {
        return;
    }
    std::wstring time = FormatTime(EffectivePosition(state));
    if (!Ui().verticalWidgetLayout) {
        time += L" / " + FormatTime(state.durationMs);
    }
    if (time != Ui().displayedTime) {
        Ui().timeText.Text(time);
        Ui().displayedTime = std::move(time);
    }
}

void RefreshWidgetContents() {
    if (!Ui().widgetGrid) {
        return;
    }

    MediaState state;
    {
        std::lock_guard lock(g_mediaMutex);
        state = g_mediaState;
    }

    bool idleVisible = !state.hasSession && IdleAppCount() != 0;
    bool visible = state.hasSession || idleVisible || !g_settings.hideWhenNoMedia;
    Ui().widgetGrid.Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
    SetModernWidgetVisible(visible);
    if (Ui().verticalWidgetLayout) {
        Ui().widgetGrid.Width(Ui().verticalWidgetWidth);
        Ui().widgetGrid.Height(idleVisible ? VerticalIdleWidgetHeight()
                                        : Ui().verticalSessionHeight);
    } else {
        Ui().widgetGrid.Width(idleVisible ? IdleWidgetWidth()
                                       : static_cast<double>(g_settings.widgetWidth));
    }
    if (Ui().progressRow) {
        Ui().progressRow.Height({idleVisible ? 0.0
                                          : static_cast<double>(
                                                Ui().layout.progressHoverHeight),
                              GridUnitType::Pixel});
    }
    if (!visible) {
        return;
    }

    if (Ui().idlePanel) {
        Ui().idlePanel.Visibility(idleVisible ? Visibility::Visible
                                           : Visibility::Collapsed);
    }
    if (Ui().coverContainer) {
        Ui().coverContainer.Visibility(state.hasSession ? Visibility::Visible
                                                     : Visibility::Collapsed);
    }
    if (Ui().textContainer) {
        Ui().textContainer.Visibility(state.hasSession || !idleVisible
                                       ? Visibility::Visible
                                       : Visibility::Collapsed);
    }
    if (Ui().rightContainer) {
        Ui().rightContainer.Visibility(state.hasSession ? Visibility::Visible
                                                     : Visibility::Collapsed);
    }
    if (Ui().progressGrid) {
        Ui().progressGrid.Visibility(state.hasSession ? Visibility::Visible
                                                   : Visibility::Collapsed);
    }
    if (Ui().visualizerGrid && !state.hasSession) {
        Ui().visualizerGrid.Visibility(Visibility::Collapsed);
    }

    if (idleVisible) {
        if (Ui().progressHoverTip) {
            Ui().progressHoverTip.Visibility(Visibility::Collapsed);
        }
        return;
    }

    std::wstring title = state.title.empty() ? L"No media" : state.title;
    std::wstring artist = state.artist;
    if (Ui().verticalWidgetLayout && Ui().coverContainer) {
        std::wstring tooltip = title;
        if (!artist.empty()) {
            tooltip += L"\n" + artist;
        }
        ToolTipService::SetToolTip(Ui().coverContainer, box_value(tooltip));
    }
    COLORREF accent = ProgressAccentColor(state);
    if (Ui().progressFill) {
        Ui().progressFill.Background(MakeBrush(accent));
    }
    if (Ui().progressTrack) {
        Ui().progressTrack.Background(MakeBrush(
            UseLightTheme() ? RGB(132, 142, 156) : RGB(72, 82, 99), 120));
    }
    if (Ui().progressThumb) {
        Ui().progressThumb.Fill(MakeBrush(ProgressThumbColor(accent)));
    }
    UpdateProgressHoverTipTheme();

    if (Ui().titleText) {
        ResetMarquee(Ui().titleScroll, title);
        Ui().titleText.Text(title);
        Ui().titleText.Foreground(MakeBrush(ActiveTextColor()));
    }
    if (Ui().artistText) {
        ResetMarquee(Ui().artistScroll, artist);
        Ui().artistText.Text(artist);
        Ui().artistText.Foreground(MakeBrush(ActiveMutedTextColor()));
    }

    if (Ui().timeText) {
        RefreshTimeText(state);
        Ui().timeText.Foreground(MakeBrush(ActiveMutedTextColor()));
    }

    if (Ui().playButton) {
        std::wstring glyph = state.isPlaying ? L"\xE769" : L"\xE768";
        if (Ui().playButtonGlyph != glyph) {
            if (Ui().playButtonIcon) {
                Ui().playButtonIcon.Text(glyph);
                Ui().playButtonIcon.FontSize(14.0);
            }
            Ui().playButtonGlyph = glyph;
        }
        Ui().playButtonEnabled = state.canPlayPause || state.canPlay;
        if (!Ui().playButtonEnabled) {
            Ui().playButtonHover = false;
            SetButtonHoverBackground(Ui().playButton, false);
        }
        UpdateButtonForeground(Ui().playButton, Ui().playButtonIcon,
                               Ui().playButtonEnabled);
        SetButtonHoverBackground(Ui().playButton, Ui().playButtonHover);
    }
    if (Ui().prevButton) {
        Ui().prevButtonEnabled = state.canPrevious;
        if (!Ui().prevButtonEnabled) {
            Ui().prevButtonHover = false;
            SetButtonHoverBackground(Ui().prevButton, false);
        }
        UpdateButtonForeground(Ui().prevButton, Ui().prevButtonIcon,
                               Ui().prevButtonEnabled);
        SetButtonHoverBackground(Ui().prevButton, Ui().prevButtonHover);
    }
    if (Ui().nextButton) {
        Ui().nextButtonEnabled = state.canNext;
        if (!Ui().nextButtonEnabled) {
            Ui().nextButtonHover = false;
            SetButtonHoverBackground(Ui().nextButton, false);
        }
        UpdateButtonForeground(Ui().nextButton, Ui().nextButtonIcon,
                               Ui().nextButtonEnabled);
        SetButtonHoverBackground(Ui().nextButton, Ui().nextButtonHover);
    }

    RefreshThumbnail(state);
    RefreshProgress(state);
    UpdateVisualizerBars(state);
}

void TickWidgetUi() {
    if (!Ui().widgetGrid) {
        return;
    }

    MediaState state;
    {
        std::lock_guard lock(g_mediaMutex);
        state = g_mediaState;
    }

    if (!state.hasSession) {
        return;
    }
    if (state.isPlaying) {
        RefreshTimeText(state);
        RefreshProgress(state);
        if (g_settings.visualizerEnabled) UpdateVisualizerBars(state);
    }
    UpdateTextMarquee(Ui().titleCanvas, Ui().titleText, Ui().titleTransform, Ui().titleScroll);
    UpdateTextMarquee(Ui().artistCanvas, Ui().artistText, Ui().artistTransform,
                      Ui().artistScroll);
}

bool WidgetNeedsFastTicks(MediaState const& state) {
    if (!state.hasSession) {
        return false;
    }
    if (state.isPlaying && g_settings.visualizerEnabled) {
        return true;
    }
    if (!state.isPlaying && !g_settings.marqueeWhenPaused) {
        return false;
    }
    auto textOverflows = [](Canvas const& canvas, TextBlock const& text) {
        return canvas && text && text.ActualWidth() > canvas.ActualWidth() + 4.0;
    };
    return textOverflows(Ui().titleCanvas, Ui().titleText) ||
           textOverflows(Ui().artistCanvas, Ui().artistText);
}

void WidgetUiTimerTick(IInspectable const&, IInspectable const&) {
    if (g_unloading || g_workersStopping || !Ui().modernWidgetAttached) {
        return;
    }

    MediaState state;
    {
        std::lock_guard lock(g_mediaMutex);
        state = g_mediaState;
    }
    bool fastTicks = WidgetNeedsFastTicks(state);
    uint64_t revision = g_uiRevision.load();
    if (Ui().seenRevision != revision) {
        Ui().seenRevision = revision;
        RefreshWidgetContents();
    } else if (fastTicks || (state.hasSession && state.isPlaying)) {
        TickWidgetUi();
    }
    // Keep the idle check cheap, but pick up a pause/resume without adding a
    // second of UI latency on top of the media polling interval.
    int interval = fastTicks ? 33 : 100;
    if (Ui().uiTimer && interval != Ui().uiTimerIntervalMs) {
        Ui().uiTimer.Interval(TimeSpan{std::chrono::milliseconds(interval)});
        Ui().uiTimerIntervalMs = interval;
    }
}

void StartWidgetUiTimer() {
    StopWidgetUiTimer();
    try {
        Ui().uiTimer = DispatcherTimer();
        Ui().uiTimerIntervalMs = 33;
        Ui().uiTimer.Interval(TimeSpan{std::chrono::milliseconds(33)});
        Ui().uiTimerToken = Ui().uiTimer.Tick(BindWidgetHandler(&WidgetUiTimerTick));
        Ui().uiTimerHasToken = true;
        Ui().uiTimer.Start();
    } catch (...) {
        StopWidgetUiTimer();
    }
}

bool InjectWidgetGrid() {
    Ui().modernWidgetAttached = false;
    HWND hwnd = Ui().taskbarWnd;
    if (!hwnd) {
        return false;
    }
    Ui().taskbarWnd = hwnd;

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
        Ui().widgetGrid = widget;
        Ui().injectionParent = targetElement;

        int insertCol = -1;
        int insertRow = -1;
        if (auto targetGrid = targetElement.try_as<Grid>()) {
            if (vertical) {
                RowDefinition row;
                row.Height({1.0, GridUnitType::Auto});
                insertRow = 0;
                targetGrid.RowDefinitions().InsertAt(insertRow, row);
                Ui().insertedRow = row;
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
                Ui().insertedColumn = column;
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

        Ui().injectedTaskbarEdge = edge;
        Ui().injectedTaskbarThicknessPx = TaskbarThicknessPx(hwnd, edge);
        Ui().modernWidgetAttached = true;
        Ui().seenRevision = g_uiRevision.load();
        RefreshWidgetContents();
        StartWidgetUiTimer();
        return true;
    } catch (hresult_error const& e) {
        Wh_Log(L"InjectWidgetGrid failed: 0x%08X %s",
               static_cast<uint32_t>(e.code()), e.message().c_str());
    } catch (...) {
        Wh_Log(L"InjectWidgetGrid failed");
    }
    RemoveWidgetGrid();
    return false;
}

bool HasMeaningfulMedia(hstring const& title,
                        hstring const& artist,
                        int64_t durationMs,
                        IRandomAccessStreamReference const& thumbnail) {
    std::wstring titleText = Trim(std::wstring(title.c_str()));
    std::wstring artistText = Trim(std::wstring(artist.c_str()));
    if (titleText.empty()) {
        return false;
    }

    bool hasDuration = durationMs >= 3000;
    return !artistText.empty() || hasDuration || static_cast<bool>(thumbnail);
}

template <typename T>
void CancelPending(T& operation) {
    if (operation) {
        try {
            if (operation.Status() == AsyncStatus::Started) {
                operation.Cancel();
            }
        } catch (...) {
        }
        operation = nullptr;
    }
}

// These proxies and pending operations belong to the media thread. Never wait
// for metadata or artwork before publishing playback and timeline changes.
struct MediaPollContext {
    ULONGLONG managerStartedTick = 0;
    ULONGLONG managerRetryTick = 0;
    ULONGLONG metadataStartedTick = 0;
    ULONGLONG artworkStartedTick = 0;
    ULONGLONG nextPlaybackTick = 0;
    ULONGLONG nextMetadataTick = 0;
    ULONGLONG artworkRetryTick = 0;
    unsigned int consecutiveFailures = 0;
    MediaState playbackState;
    bool managerDelayLogged = false;
    GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
    IAsyncOperation<GlobalSystemMediaTransportControlsSessionManager> managerRequest{nullptr};
    GlobalSystemMediaTransportControlsSession session{nullptr};
    IAsyncOperation<GlobalSystemMediaTransportControlsSessionMediaProperties> metadataRequest{nullptr};
    GlobalSystemMediaTransportControlsSessionMediaProperties properties{nullptr};
    IAsyncOperation<IRandomAccessStreamWithContentType> artworkOpen{nullptr};
    IAsyncOperationWithProgress<IBuffer, uint32_t> artworkRead{nullptr};
    IRandomAccessStreamWithContentType artworkStream{nullptr};
    std::wstring artworkKey;
    std::shared_ptr<const std::vector<uint8_t>> artworkBytes;
    bool artworkRequested = false;
    bool hasAccent = false;
    COLORREF accent = 0;

    void ResetArtwork() {
        CancelPending(artworkRead);
        CancelPending(artworkOpen);
        artworkStream = nullptr;
        artworkBytes.reset();
        artworkRequested = false;
        hasAccent = false;
        artworkKey.clear();
        artworkRetryTick = 0;
    }

    void ResetSession() {
        CancelPending(metadataRequest);
        properties = nullptr;
        session = nullptr;
        playbackState = {};
        nextMetadataTick = 0;
        ResetArtwork();
    }

    void RecordFailure() {
        if (++consecutiveFailures < 3) {
            return;
        }
        // Keep the last displayed track while replacing a broken broker proxy.
        ResetSession();
        CancelPending(managerRequest);
        manager = nullptr;
        managerRetryTick = GetTickCount64() + 3000;
        consecutiveFailures = 0;
        Wh_Log(L"Repeated media IPC errors; reconnecting in 3 s");
    }

    ~MediaPollContext() {
        ResetSession();
        CancelPending(managerRequest);
    }
};

void PollArtwork(MediaPollContext& context, std::wstring const& key) {
    if (context.artworkKey != key) {
        context.ResetArtwork();
        context.artworkKey = key;
    }
    try {
        if (!context.artworkRequested &&
            GetTickCount64() >= context.artworkRetryTick &&
            context.properties.Thumbnail()) {
            context.artworkStartedTick = GetTickCount64();
            context.artworkOpen = context.properties.Thumbnail().OpenReadAsync();
            context.artworkRequested = true;
        }
        if (context.artworkOpen &&
            context.artworkOpen.Status() != AsyncStatus::Started) {
            auto operation = std::exchange(context.artworkOpen, nullptr);
            context.artworkStream = operation.GetResults();
            uint64_t size = context.artworkStream.Size();
            if (size > 0 && size <= 10 * 1024 * 1024) {
                context.artworkStartedTick = GetTickCount64();
                context.artworkRead = context.artworkStream.ReadAsync(
                    Buffer(static_cast<uint32_t>(size)),
                    static_cast<uint32_t>(size), InputStreamOptions::None);
            } else {
                context.artworkStream = nullptr;
            }
        }
        if (context.artworkRead &&
            context.artworkRead.Status() != AsyncStatus::Started) {
            auto operation = std::exchange(context.artworkRead, nullptr);
            auto buffer = operation.GetResults();
            auto bytes = std::make_shared<std::vector<uint8_t>>(buffer.Length());
            if (!bytes->empty()) {
                DataReader::FromBuffer(buffer).ReadBytes(*bytes);
                context.accent = ExtractAccentColor(*bytes);
                context.hasAccent = true;
            }
            context.artworkBytes = std::move(bytes);
            context.artworkStream = nullptr;
        }
    } catch (hresult_error const& e) {
        Wh_Log(L"Artwork request failed: 0x%08X", static_cast<uint32_t>(e.code()));
        context.ResetArtwork();
        context.artworkKey = key;
        context.artworkRetryTick = GetTickCount64() + 1000;
    }
}

bool MediaPresentationChanged(MediaState const& previous,
                              MediaState const& next) {
    return previous.hasSession != next.hasSession ||
           previous.isPlaying != next.isPlaying ||
           previous.canPrevious != next.canPrevious ||
           previous.canNext != next.canNext ||
           previous.canPlay != next.canPlay ||
           previous.canPlayPause != next.canPlayPause ||
           previous.canSeek != next.canSeek ||
           previous.sourceAppId != next.sourceAppId ||
           previous.title != next.title || previous.artist != next.artist ||
           previous.timelineStartMs != next.timelineStartMs ||
           previous.durationMs != next.durationMs ||
           previous.thumbnailBytes != next.thumbnailBytes ||
           previous.thumbnailKey != next.thumbnailKey ||
           previous.hasAccentColor != next.hasAccentColor ||
           previous.accentColor != next.accentColor ||
           (next.hasSession &&
            std::abs(EffectivePosition(previous) - EffectivePosition(next)) >
                (next.isPlaying ? 250 : 0));
}

DWORD PendingMediaPollDelay(ULONGLONG started, ULONGLONG now) {
    // Only newly started operations get the fast path. A stalled broker must
    // not keep Explorer polling twenty times a second indefinitely.
    return now - started < 500 ? 50 : 1000;
}

DWORD MediaPollDelay(MediaPollContext const& context, ULONGLONG now) {
    DWORD delay = 1000;
    if (context.manager && context.nextPlaybackTick > now) {
        delay = static_cast<DWORD>(
            std::min<ULONGLONG>(delay, context.nextPlaybackTick - now));
    }
    if (context.session && !context.metadataRequest &&
        context.nextMetadataTick > now) {
        delay = static_cast<DWORD>(
            std::min<ULONGLONG>(delay, context.nextMetadataTick - now));
    }
    if (context.managerRequest) {
        delay = std::min(delay, PendingMediaPollDelay(context.managerStartedTick, now));
    }
    if (context.metadataRequest) {
        delay = std::min(delay, PendingMediaPollDelay(context.metadataStartedTick, now));
    }
    if (context.artworkOpen || context.artworkRead) {
        delay = std::min(delay, PendingMediaPollDelay(context.artworkStartedTick, now));
    }
    return delay;
}

void PollMedia(MediaPollContext& context, bool forcePlayback = false) {
    MediaState nextState;
    GlobalSystemMediaTransportControlsSession nextSession{nullptr};

    try {
        if (!context.manager) {
            if (!context.managerRequest) {
                if (GetTickCount64() < context.managerRetryTick) {
                    return;
                }
                context.managerStartedTick = GetTickCount64();
                context.managerDelayLogged = false;
                context.managerRequest =
                    GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
            }
            if (context.managerRequest.Status() == AsyncStatus::Started) {
                if (!context.managerDelayLogged &&
                    GetTickCount64() - context.managerStartedTick >= 3000) {
                    Wh_Log(L"Media manager still pending after %llu ms",
                           GetTickCount64() - context.managerStartedTick);
                    context.managerDelayLogged = true;
                }
                return;
            }
            auto request = std::exchange(context.managerRequest, nullptr);
            try {
                context.manager = request.GetResults();
            } catch (hresult_error const& e) {
                context.managerRetryTick = GetTickCount64() + 3000;
                Wh_Log(L"Media manager request failed after %llu ms: 0x%08X; retry in 3 s",
                       GetTickCount64() - context.managerStartedTick,
                       static_cast<uint32_t>(e.code()));
                return;
            }
            Wh_Log(L"Media manager ready after %llu ms",
                   GetTickCount64() - context.managerStartedTick);
        }
        // Short async-completion checks reuse the last playback sample instead
        // of issuing another set of synchronous broker calls.
        if (forcePlayback || GetTickCount64() >= context.nextPlaybackTick) {
            context.nextPlaybackTick = GetTickCount64() + 1000;
            nextSession = context.manager.GetCurrentSession();
            if (!nextSession) {
                auto sessions = context.manager.GetSessions();
                if (sessions.Size() > 0) {
                    nextSession = sessions.GetAt(0);
                }
            }

            if (nextSession != context.session) {
                context.ResetSession();
                context.session = nextSession;
            }
            MediaState playback;
            if (nextSession) {
                auto info = nextSession.GetPlaybackInfo();
                auto controls = info.Controls();
                auto timeline = nextSession.GetTimelineProperties();
                playback.hasSession = true;
                playback.sourceAppId = nextSession.SourceAppUserModelId();
                playback.isPlaying = info.PlaybackStatus() ==
                    GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                playback.canPrevious = controls.IsPreviousEnabled();
                playback.canNext = controls.IsNextEnabled();
                playback.canPlay = controls.IsPlayEnabled();
                playback.canPlayPause = controls.IsPlayPauseToggleEnabled();
                playback.canSeek = controls.IsPlaybackPositionEnabled();
                playback.timelineStartMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        timeline.StartTime()).count();
                playback.positionMs = TimelinePositionMs(timeline, playback.isPlaying);
                playback.durationMs = TimelineDurationMs(timeline);
                playback.updatedTick = GetTickCount64();
            }
            context.playbackState = std::move(playback);
            context.consecutiveFailures = 0;
        }
        nextSession = context.session;
        if (nextSession) {
            try {
                if (!context.metadataRequest &&
                    GetTickCount64() >= context.nextMetadataTick) {
                    context.metadataStartedTick = GetTickCount64();
                    context.nextMetadataTick = context.metadataStartedTick + 1000;
                    context.metadataRequest = nextSession.TryGetMediaPropertiesAsync();
                }
                if (context.metadataRequest &&
                    context.metadataRequest.Status() != AsyncStatus::Started) {
                    auto request = std::exchange(context.metadataRequest, nullptr);
                    context.properties = request.GetResults();
                    ULONGLONG elapsed = GetTickCount64() - context.metadataStartedTick;
                    if (elapsed >= 3000) {
                        Wh_Log(L"Media metadata ready after %llu ms", elapsed);
                    }
                }
            } catch (hresult_error const& e) {
                Wh_Log(L"Metadata request failed: 0x%08X",
                       static_cast<uint32_t>(e.code()));
            }
            // A pending request is not a missing session. Keep the displayed
            // track until metadata arrives instead of flashing the idle UI.
            if (!context.properties) {
                return;
            }

            auto properties = context.properties;
            bool meaningful = HasMeaningfulMedia(
                properties.Title(), properties.Artist(),
                context.playbackState.durationMs, properties.Thumbnail());

            if (!meaningful) {
                nextSession = nullptr;
            } else {
                nextState = context.playbackState;
                nextState.title = properties.Title();
                nextState.artist = properties.Artist();
                nextState.thumbnailKey =
                    nextState.sourceAppId + L"|" + nextState.title + L"|" +
                    nextState.artist;
                PollArtwork(context, nextState.thumbnailKey);
                nextState.thumbnailBytes = context.artworkBytes;
                nextState.hasAccentColor = context.hasAccent;
                nextState.accentColor = context.accent;
            }
        }
    } catch (hresult_error const& e) {
        Wh_Log(L"PollMedia failed: 0x%08X %s",
               static_cast<uint32_t>(e.code()), e.message().c_str());
        // A transient IPC failure must not turn a playing track into idle.
        context.RecordFailure();
        return;
    } catch (...) {
        Wh_Log(L"PollMedia failed");
        context.RecordFailure();
        return;
    }

    bool presentationChanged;
    bool audioChanged;
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
        presentationChanged = MediaPresentationChanged(g_mediaState, nextState);
        audioChanged = g_mediaState.hasSession != nextState.hasSession ||
                       g_mediaState.isPlaying != nextState.isPlaying;
        // Refresh the timeline anchor even when nothing needs repainting.
        // Otherwise EffectivePosition's stale-sample cap would stop the clock.
        if (presentationChanged) {
            g_mediaState = std::move(nextState);
        } else {
            g_mediaState.positionMs = nextState.positionMs;
            g_mediaState.updatedTick = nextState.updatedTick;
        }
        if (g_mediaSession != nextSession) {
            g_mediaSession = nextSession;
        }
    }
    if (presentationChanged) {
        ++g_uiRevision;
    }
    if (audioChanged) {
        WakeAudioThread();
    }
}
// Windows 10's taskbar is a classic Win32 surface. Keeping this backend in the
// same module lets the media/session code stay shared while only the rendering
// and taskbar attachment differ from the Windows 11 XAML path.
int ClassicScale(HWND hwnd, int value) {
    UINT dpi = GetDpiForWindow(hwnd);
    return MulDiv(value, dpi ? dpi : 96, 96);
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
    int height = Ui().classicProgressHover ? hoverHeight : normalHeight;
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
    if (Ui().verticalWidgetLayout) {
        size = std::max(1, std::min(ClassicScale(hwnd, g_settings.coverSize),
                        static_cast<int>(client.right) - ClassicScale(hwnd, 8)));
        left = (client.right - size) / 2;
        top = ClassicScale(hwnd, 4);
    }
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
    if (Ui().verticalWidgetLayout) {
        size = std::max(1, std::min(ClassicScale(hwnd, g_settings.buttonSize),
                                   static_cast<int>(client.right) - ClassicScale(hwnd, 4)));
        int x = (client.right - size) / 2;
        int y = ClassicCoverRect(hwnd).bottom + ClassicScale(hwnd, 4) + (command - 1) * size;
        return {x, y, x + size, y + size};
    }
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
    SeekMedia(absoluteTargetMs);
}

RECT ClassicIdleIconRect(HWND hwnd, size_t index, size_t count) {
    RECT client{};
    GetClientRect(hwnd, &client);
    int progress = ClassicScale(hwnd, g_settings.progressHoverHeight);
    int contentHeight =
        static_cast<int>(std::max<LONG>(1, client.bottom - progress));
    int iconSize = std::min(ClassicScale(hwnd, 28), contentHeight - 6);
    if (Ui().verticalWidgetLayout) {
        iconSize = std::max(1, std::min(ClassicScale(hwnd, 28),
                                       static_cast<int>(client.right) - ClassicScale(hwnd, 8)));
        int left = (client.right - iconSize) / 2;
        int top = ClassicScale(hwnd, 4) + static_cast<int>(index) * (iconSize + ClassicScale(hwnd, 4));
        return {left, top, left + iconSize, top + iconSize};
    }
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
    size_t idleCount = IdleAppCount();
    if (state.hasSession || idleCount == 0) {
        return ClassicScale(hwnd, g_settings.widgetWidth);
    }

    size_t count = std::min<size_t>(idleCount, 6);
    int iconSize = ClassicScale(hwnd, 28);
    int gap = ClassicScale(hwnd, 4);
    int width = ClassicScale(hwnd, 8) + static_cast<int>(count) * iconSize +
                static_cast<int>(count > 0 ? count - 1 : 0) * gap;
    return std::min(width, ClassicScale(hwnd, g_settings.widgetWidth));
}

bool DrawClassicMarquee(HDC dc,
                        RECT rect,
                        HFONT font,
                        std::wstring const& text,
                        COLORREF color) {
    if (text.empty() || rect.right <= rect.left) {
        return false;
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
    bool allowMarquee;
    {
        std::lock_guard lock(g_mediaMutex);
        allowMarquee = g_mediaState.isPlaying || g_settings.marqueeWhenPaused;
    }
    bool scrolls = allowMarquee && overflow > ClassicScale(Ui().classicWidgetWnd, 4);
    if (scrolls) {
        double travel = static_cast<double>(overflow + ClassicScale(
                                                       Ui().classicWidgetWnd, 12));
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
    return scrolls;
}

void PaintClassicWidget(HWND hwnd, HDC targetDc) {
    Ui().classicTextOverflow = false;
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
                             1, kVisualizerMaxBarCount);
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
        auto idleApps = IdleAppsSnapshot();
        size_t count = std::min<size_t>(idleApps.size(), 6);
        if (Ui().classicIdleImages.size() != count) {
            Ui().classicIdleImages.clear();
            Ui().classicIdleImages.resize(count);
        }
        HFONT fallbackFont = CreateFontW(
            -ClassicScale(hwnd, 13), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE,
            FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        for (size_t i = 0; i < count; i++) {
            RECT iconRect = ClassicIdleIconRect(hwnd, i, count);
            if (!idleApps[i].iconBytes.empty() &&
                DecodeClassicImage(idleApps[i].iconBytes,
                                   idleApps[i].shortcutPath,
                                   iconRect.right - iconRect.left,
                                   Ui().classicIdleImages[i])) {
                DrawClassicImage(dc, Ui().classicIdleImages[i], iconRect,
                                 ClassicScale(hwnd, 7));
            } else {
                std::wstring letter = idleApps[i].name.empty()
                                          ? L"\x266A"
                                          : idleApps[i].name.substr(0, 1);
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
        if (state.thumbnailBytes &&
            DecodeClassicImage(*state.thumbnailBytes, state.thumbnailKey,
                               coverRect.right - coverRect.left,
                               Ui().classicCoverImage)) {
            DrawClassicImage(dc, Ui().classicCoverImage, coverRect, coverRadius);
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
            if (Ui().classicHotButton == i + 1) {
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
        bool titleScrolls = !Ui().verticalWidgetLayout &&
            DrawClassicMarquee(dc, titleRect, titleFont, state.title, textColor);
        bool artistScrolls = !Ui().verticalWidgetLayout &&
            DrawClassicMarquee(dc, artistRect, artistFont, state.artist, mutedColor);
        Ui().classicTextOverflow = titleScrolls || artistScrolls;

        RECT timeRect{prevRect.left - ClassicScale(hwnd, 58), 0,
                      prevRect.left - ClassicScale(hwnd, 4), contentHeight};
        std::wstring time = FormatTime(EffectivePosition(state));
        if (Ui().verticalWidgetLayout) {
            timeRect = {0, nextRect.bottom, client.right, contentHeight};
        } else {
            time += L" / " + FormatTime(state.durationMs);
        }
        HFONT timeFont = CreateFontW(
            -ClassicScale(hwnd, 9), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HGDIOBJ oldFont = SelectObject(dc, timeFont);
        SetTextColor(dc, mutedColor);
        SetBkMode(dc, TRANSPARENT);
        DrawTextW(dc, time.c_str(), static_cast<int>(time.size()), &timeRect,
                  (Ui().verticalWidgetLayout ? DT_CENTER : DT_RIGHT) |
                      DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(dc, oldFont);

        HFONT iconFont = CreateFontW(
            -ClassicScale(hwnd, 13), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe MDL2 Assets");
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

        if (Ui().classicProgressHover) {
            int x = ClampInt(Ui().classicProgressHoverX, progressRect.left,
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
            int tipWidth = std::min(width, ClassicScale(hwnd, 42));
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

RECT WindowRectInParent(HWND window) {
    RECT rect{};
    GetWindowRect(window, &rect);
    MapWindowPoints(nullptr, GetParent(window), reinterpret_cast<POINT*>(&rect), 2);
    return rect;
}

bool SameRect(RECT const& a, RECT const& b) {
    return a.left == b.left && a.top == b.top &&
           a.right == b.right && a.bottom == b.bottom;
}

void PlaceClassicWidget() {
    auto& ui = Ui();
    if (!ui.classicWidgetWnd || !ui.haveNativeArea) return;
    RECT area = ui.nativeArea;
    bool vertical = IsVerticalTaskbarEdge(DetectTaskbarEdge(ui.taskbarWnd));
    ui.verticalWidgetLayout = vertical;
    RECT reserved = area;
    if (vertical) reserved.top = reserved.bottom - ui.reservedExtent;
    else {
        reserved.left = reserved.right - ui.reservedExtent;
        int height = std::min<int>(reserved.bottom - reserved.top,
                                   ClassicScale(ui.classicWidgetWnd, g_settings.widgetHeight));
        reserved.top += (reserved.bottom - reserved.top - height) / 2;
        reserved.bottom = reserved.top + height;
    }
    // The reserved area belongs to a direct child of the taskbar.
    int width = reserved.right - reserved.left;
    int height = reserved.bottom - reserved.top;
    if (!ui.reservedExtent || width <= 0 || height <= 0) {
        ShowWindow(ui.classicWidgetWnd, SW_HIDE);
        SetClassicWidgetVisible(false);
        return;
    }
    if (!SameRect(WindowRectInParent(ui.classicWidgetWnd), reserved)) {
        SetWindowPos(ui.classicWidgetWnd, nullptr, reserved.left, reserved.top,
                     width, height, SWP_NOZORDER | SWP_NOACTIVATE);
    }
    if (!IsWindowVisible(ui.classicWidgetWnd)) ShowWindow(ui.classicWidgetWnd, SW_SHOWNOACTIVATE);
    SetClassicWidgetVisible(true);
}

LRESULT CALLBACK NativeTaskListSubclass(HWND hwnd, UINT message, WPARAM wp,
                                        LPARAM lp, UINT_PTR id, DWORD_PTR data) {
    auto& ui = *reinterpret_cast<WidgetUiState*>(data);
    WidgetScope scope(ui);
    if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd, NativeTaskListSubclass, id);
        ui.nativeTaskList = nullptr;
        ui.haveNativeArea = false;
        ui.reservedExtent = 0;
        if (ui.classicWidgetWnd) ShowWindow(ui.classicWidgetWnd, SW_HIDE);
        SetClassicWidgetVisible(false);
        return DefSubclassProc(hwnd, message, wp, lp);
    }
    LRESULT result = DefSubclassProc(hwnd, message, wp, lp);
    if (message == WM_WINDOWPOSCHANGING && !ui.changingNativeArea) {
        auto* pos = reinterpret_cast<WINDOWPOS*>(lp);
        RECT area = ui.haveNativeArea ? ui.nativeArea : WindowRectInParent(hwnd);
        int width = area.right - area.left;
        int height = area.bottom - area.top;
        if (!(pos->flags & SWP_NOMOVE)) {
            area.left = pos->x;
            area.top = pos->y;
        }
        if (!(pos->flags & SWP_NOSIZE)) {
            width = pos->cx;
            height = pos->cy;
        }
        area.right = area.left + width;
        area.bottom = area.top + height;
        ui.nativeArea = area;
        ui.haveNativeArea = true;
        bool vertical = IsVerticalTaskbarEdge(DetectTaskbarEdge(ui.taskbarWnd));
        int extent = vertical ? height : width;
        // Leave usable native task-button space, including on a small monitor.
        if (extent < ui.reservedExtent + ClassicScale(hwnd, 96)) {
            ui.reservedExtent = 0;
        }
        if (!(pos->flags & SWP_NOSIZE)) {
            if (vertical) pos->cy = std::max(1, height - ui.reservedExtent);
            else pos->cx = std::max(1, width - ui.reservedExtent);
        }
    }
    if (message == WM_WINDOWPOSCHANGED && !ui.changingNativeArea) {
        PlaceClassicWidget();
    }
    return result;
}

void DetachClassicTaskList() {
    auto& ui = Ui();
    HWND native = ui.nativeTaskList;
    ui.nativeTaskList = nullptr;
    ui.reservedExtent = 0;
    if (native && IsWindow(native)) {
        RemoveWindowSubclass(native, NativeTaskListSubclass,
                             reinterpret_cast<UINT_PTR>(&ui));
        if (ui.haveNativeArea) {
            RECT area = ui.nativeArea;
            SetWindowPos(native, nullptr, area.left, area.top,
                         area.right - area.left, area.bottom - area.top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
    ui.haveNativeArea = false;
    if (ui.classicWidgetWnd) ShowWindow(ui.classicWidgetWnd, SW_HIDE);
    SetClassicWidgetVisible(false);
}

std::vector<HWND> FindTaskbarWindows();

HWND FindClassicTaskListHost(HWND taskbar) {
    HWND rebar = FindWindowExW(taskbar, nullptr, L"ReBarWindow32", nullptr);
    if (rebar && FindWindowExW(rebar, nullptr, L"MSTaskSwWClass", nullptr)) {
        return rebar;
    }
    // A secondary WorkerW is not sufficient evidence of the classic shell.
    // Confirm its primary taskbar is classic before reserving native space.
    HWND primaryRebar = nullptr;
    for (HWND candidate : FindTaskbarWindows()) {
        wchar_t name[64]{};
        GetClassNameW(candidate, name, ARRAYSIZE(name));
        if (!_wcsicmp(name, L"Shell_TrayWnd")) {
            primaryRebar = FindWindowExW(candidate, nullptr, L"ReBarWindow32", nullptr);
            break;
        }
    }
    if (!primaryRebar ||
        !FindWindowExW(primaryRebar, nullptr, L"MSTaskSwWClass", nullptr)) {
        return nullptr;
    }
    HWND worker = FindWindowExW(taskbar, nullptr, L"WorkerW", nullptr);
    if (worker && FindWindowExW(worker, nullptr, L"MSTaskListWClass", nullptr)) {
        return worker;
    }
    return nullptr;
}

void RepositionClassicWidget() {
    auto& ui = Ui();
    if (!ui.classicWidgetWnd || !ui.nativeTaskList || !ui.haveNativeArea) return;
    MediaState state;
    {
        std::lock_guard lock(g_mediaMutex);
        state = g_mediaState;
    }
    size_t idleCount = std::min<size_t>(IdleAppCount(), 6);
    bool visible = state.hasSession || idleCount || !g_settings.hideWhenNoMedia;
    bool vertical = IsVerticalTaskbarEdge(DetectTaskbarEdge(ui.taskbarWnd));
    ui.verticalWidgetLayout = vertical;
    int desired = 0;
    if (visible) {
        if (vertical) {
            int width = ui.nativeArea.right - ui.nativeArea.left;
            int cover = std::min(ClassicScale(ui.classicWidgetWnd, g_settings.coverSize),
                                 width - ClassicScale(ui.classicWidgetWnd, 8));
            int button = std::min(ClassicScale(ui.classicWidgetWnd, g_settings.buttonSize),
                                  width - ClassicScale(ui.classicWidgetWnd, 4));
            desired = state.hasSession
                ? cover + 3 * button + ClassicScale(ui.classicWidgetWnd,
                    30 + g_settings.progressHoverHeight)
                : ClassicScale(ui.classicWidgetWnd, static_cast<int>(8 + idleCount * 32));
        } else {
            desired = ClassicDesiredWidth(ui.classicWidgetWnd, state);
        }
    }
    int extent = vertical ? ui.nativeArea.bottom - ui.nativeArea.top
                          : ui.nativeArea.right - ui.nativeArea.left;
    if (extent < desired + ClassicScale(ui.taskbarWnd, 96)) desired = 0;
    ui.reservedExtent = desired;
    RECT area = ui.nativeArea;
    if (vertical) area.bottom -= desired;
    else area.right -= desired;
    if (!SameRect(WindowRectInParent(ui.nativeTaskList), area)) {
        ui.changingNativeArea = true;
        SetWindowPos(ui.nativeTaskList, nullptr, area.left, area.top,
                     area.right - area.left, area.bottom - area.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        ui.changingNativeArea = false;
    }
    PlaceClassicWidget();
}

int ClassicHotButtonAt(HWND hwnd, POINT point) {
    for (int command = 1; command <= 3; command++) {
        if (PointInClassicRect(ClassicButtonRect(hwnd, command), point)) {
            return command;
        }
    }
    return 0;
}

int ClassicPaintIntervalMs(bool visible, bool hasSession, bool isPlaying) {
    bool animate = visible && hasSession &&
                   ((isPlaying && g_settings.visualizerEnabled) ||
                    (Ui().classicTextOverflow &&
                     (isPlaying || g_settings.marqueeWhenPaused)));
    return animate ? 33 : 1000;
}

LRESULT CALLBACK ClassicWidgetWndProc(HWND hwnd,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    auto* state = reinterpret_cast<WidgetUiState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        state = static_cast<WidgetUiState*>(
            reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
    }
    if (!state) return DefWindowProcW(hwnd, message, wParam, lParam);
    WidgetScope scope(*state);
    switch (message) {
        case WM_CREATE:
            Ui().classicWidgetWnd = hwnd;
            Ui().classicTextOverflow = false;
            SetTimer(hwnd, 1, 1000, nullptr);
            return 0;

        case WM_TIMER:
            if (wParam == 1) {
                bool visible = IsWindowVisible(hwnd);
                int interval = 1000;
                if (visible) {
                    std::lock_guard lock(g_mediaMutex);
                    interval = ClassicPaintIntervalMs(
                        visible, g_mediaState.hasSession, g_mediaState.isPlaying);
                }
                SetTimer(hwnd, 1, interval, nullptr);
                if (visible) {
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
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
                    size_t count = std::min<size_t>(IdleAppCount(), 6);
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
            Ui().classicHotButton = state.hasSession
                                     ? ClassicHotButtonAt(hwnd, point)
                                     : 0;
            Ui().classicProgressHover =
                state.hasSession && state.canSeek && state.durationMs > 0 &&
                PointInClassicRect(ClassicProgressHitRect(hwnd), point);
            if (Ui().classicProgressHover) {
                Ui().classicProgressHoverX = point.x;
            }
            TRACKMOUSEEVENT tracking{sizeof(tracking), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tracking);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case WM_MOUSELEAVE:
            Ui().classicHotButton = 0;
            Ui().classicProgressHover = false;
            Ui().classicProgressHoverX = 0;
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
                auto idleApps = IdleAppsSnapshot();
                size_t count = std::min<size_t>(idleApps.size(), 6);
                for (size_t i = 0; i < count; i++) {
                    if (PointInClassicRect(
                            ClassicIdleIconRect(hwnd, i, count), point)) {
                        LaunchIdleApp(idleApps[i].shortcutPath);
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
            DetachClassicTaskList();
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            KillTimer(hwnd, 2);
            SetClassicWidgetVisible(false);
            Ui().classicWidgetWnd = nullptr;
            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool AttachClassicWidget() {
    auto& ui = Ui();
    HWND native = FindClassicTaskListHost(ui.taskbarWnd);
    if (!native) return false;
    if (!ui.classicWidgetWnd) {
        ui.classicWidgetWnd = CreateWindowExW(
            WS_EX_NOACTIVATE, kClassicWidgetClass, L"Taskbar Media Widget",
            WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 1, 1, ui.taskbarWnd, nullptr,
            g_widgetModule, &ui);
        if (!ui.classicWidgetWnd) return false;
    }
    if (ui.nativeTaskList != native) {
        DetachClassicTaskList();
        ui.nativeArea = WindowRectInParent(native);
        ui.haveNativeArea = true;
        if (!SetWindowSubclass(native, NativeTaskListSubclass,
                               reinterpret_cast<UINT_PTR>(&ui),
                               reinterpret_cast<DWORD_PTR>(&ui))) {
            ui.haveNativeArea = false;
            return false;
        }
        ui.nativeTaskList = native;
    }
    RepositionClassicWidget();
    return true;
}

void MediaThread() {
    ApartmentScope apartment;
    MediaPollContext context;
    HANDLE events[] = {g_stopEvent, g_mediaWakeEvent};
    bool forcePlayback = false;
    while (!g_unloading && !g_workersStopping) {
        PollMedia(context, forcePlayback);
        DWORD delay = MediaPollDelay(context, GetTickCount64());
        DWORD result = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, delay);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) {
            break;
        }
        forcePlayback = result == WAIT_OBJECT_0 + 1;
    }
    {
        std::lock_guard lock(g_mediaMutex);
        g_mediaSession = nullptr;
    }
}

std::vector<HWND> FindTaskbarWindows() {
    std::vector<HWND> windows;
    EnumWindows([](HWND hwnd, LPARAM parameter) CALLBACK -> BOOL {
        DWORD pid = 0;
        wchar_t name[64]{};
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId() && GetClassNameW(hwnd, name, ARRAYSIZE(name)) &&
            (!_wcsicmp(name, L"Shell_TrayWnd") ||
             !_wcsicmp(name, L"Shell_SecondaryTrayWnd"))) {
            reinterpret_cast<std::vector<HWND>*>(parameter)->push_back(hwnd);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));
    return windows;
}

void CleanupWidgetInstance() {
    RemoveWidgetGrid();
    DetachClassicTaskList();
    if (Ui().classicWidgetWnd) DestroyWindow(Ui().classicWidgetWnd);
    Ui().classicCoverImage = {};
    Ui().classicIdleImages.clear();
}

LRESULT CALLBACK TaskbarWidgetSubclass(HWND hwnd, UINT message, WPARAM wp,
                                       LPARAM lp, UINT_PTR id, DWORD_PTR data) {
    auto& ui = *reinterpret_cast<WidgetUiState*>(data);
    WidgetScope scope(ui);
    if (message == WM_NCDESTROY) {
        CleanupWidgetInstance();
        RemoveWindowSubclass(hwnd, TaskbarWidgetSubclass, id);
        ui.hostSubclassed = false;
        ui.destroyed = true;
        g_taskbarDiscoveryRequested = true;
        if (g_taskbarWakeEvent) SetEvent(g_taskbarWakeEvent);
    } else if (message == WM_DISPLAYCHANGE || message == WM_DPICHANGED ||
               message == WM_SETTINGCHANGE || message == WM_THEMECHANGED) {
        g_taskbarDiscoveryRequested = true;
        if (g_taskbarWakeEvent) SetEvent(g_taskbarWakeEvent);
    }
    return DefSubclassProc(hwnd, message, wp, lp);
}

void TickWidgetInstance(WidgetUiState& ui, bool rebuildIdle) {
    WidgetScope scope(ui);
    if (ui.destroyed) return;
    if (!ui.hostSubclassed) {
        if (!SetWindowSubclass(ui.taskbarWnd, TaskbarWidgetSubclass,
                               reinterpret_cast<UINT_PTR>(&ui),
                               reinterpret_cast<DWORD_PTR>(&ui))) return;
        ui.hostSubclassed = true;
    }
    TaskbarEdge edge = DetectTaskbarEdge(ui.taskbarWnd);
    int thickness = TaskbarThicknessPx(ui.taskbarWnd, edge);
    UINT dpi = GetDpiForWindow(ui.taskbarWnd);
    bool layoutChanged = edge != ui.injectedTaskbarEdge ||
                         thickness != ui.injectedTaskbarThicknessPx || dpi != ui.dpi;
    if (ui.modernWidgetAttached && (layoutChanged || rebuildIdle)) RemoveWidgetGrid();
    if (ui.modernWidgetAttached && !ui.widgetGrid.IsLoaded()) RemoveWidgetGrid();
    if (!ui.modernWidgetAttached && !ui.nativeTaskList) {
        if (g_taskbarHooksInstalled && InjectWidgetGrid()) {
            if (ui.classicWidgetWnd) DestroyWindow(ui.classicWidgetWnd);
        } else {
            // Native controls identify the classic backend, not a failed XAML
            // lookup or a timeout during Windows 11 taskbar reconstruction.
            AttachClassicWidget();
        }
    }
    ui.injectedTaskbarEdge = edge;
    ui.injectedTaskbarThicknessPx = thickness;
    ui.dpi = dpi;
    if (ui.nativeTaskList) {
        RepositionClassicWidget();
        uint64_t revision = g_uiRevision.load();
        if (ui.seenRevision != revision || layoutChanged || rebuildIdle) {
            ui.seenRevision = revision;
            if (ui.classicWidgetWnd) InvalidateRect(ui.classicWidgetWnd, nullptr, FALSE);
        }
    }
}

void UiTickThread() {
    // The worker owns the registry, but every UI/COM access is dispatched to
    // the corresponding taskbar thread. The media and audio workers are shared.
    std::vector<std::unique_ptr<WidgetUiState>> widgets;
    int discoveryPasses = 5;
    HANDLE events[] = {g_stopEvent, g_taskbarWakeEvent};
    while (!g_unloading && !g_workersStopping) {
        bool light = RegistryUseLightTheme();
        if (g_systemLightTheme.exchange(light) != light) ++g_uiRevision;
        bool rebuildIdle = g_idleAppsChanged.exchange(false);
        if (g_taskbarDiscoveryRequested.exchange(false)) discoveryPasses = 5;
        // Keep retired instances until worker shutdown: an outer routed event
        // can still be unwinding after a nested taskbar-destruction message.
        bool anyLive = std::any_of(widgets.begin(), widgets.end(),
                                  [](auto const& ui) { return !ui->destroyed; });
        if (discoveryPasses > 0 || !anyLive) {
            if (discoveryPasses > 0) --discoveryPasses;
            for (HWND hwnd : FindTaskbarWindows()) {
                if (std::any_of(widgets.begin(), widgets.end(),
                                [hwnd](auto const& ui) {
                                    return !ui->destroyed && ui->taskbarWnd == hwnd;
                                })) continue;
                auto ui = std::make_unique<WidgetUiState>();
                ui->taskbarWnd = hwnd;
                widgets.push_back(std::move(ui));
            }
        }
        for (auto& ui : widgets) {
            if (g_workersStopping || g_unloading) break;
            if (ui->destroyed) continue;
            struct Request { WidgetUiState* ui; bool rebuildIdle; } request{ui.get(), rebuildIdle};
            RunFromWindowThread(ui->taskbarWnd, [](void* parameter) {
                auto& request = *static_cast<Request*>(parameter);
                if (g_workersStopping || g_unloading) return;
                try {
                    TickWidgetInstance(*request.ui, request.rebuildIdle);
                } catch (hresult_error const& error) {
                    Wh_Log(L"Taskbar widget update failed: 0x%08X", static_cast<uint32_t>(error.code()));
                } catch (...) {
                    Wh_Log(L"Taskbar widget update failed");
                }
            }, &request);
        }
        DWORD result = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, 1000);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) break;
    }
    for (auto& ui : widgets) {
        if (ui->destroyed) continue;
        RunFromWindowThread(ui->taskbarWnd, [](void* parameter) {
            auto& ui = *static_cast<WidgetUiState*>(parameter);
            WidgetScope scope(ui);
            CleanupWidgetInstance();
            if (ui.hostSubclassed) {
                RemoveWindowSubclass(ui.taskbarWnd, TaskbarWidgetSubclass,
                                     reinterpret_cast<UINT_PTR>(&ui));
                ui.hostSubclassed = false;
            }
            ui.destroyed = true;
        }, ui.get());
    }
}

using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (g_unloading) {
        return;
    }

    g_taskbarDiscoveryRequested = true;
    if (g_taskbarWakeEvent) SetEvent(g_taskbarWakeEvent);
}

decltype(&CreateWindowExW) CreateWindowExW_Original = nullptr;

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, PCWSTR className,
                                 PCWSTR title, DWORD style, int x, int y,
                                 int width, int height, HWND parent, HMENU menu,
                                 HINSTANCE instance, void* parameter) {
    HWND hwnd = CreateWindowExW_Original(exStyle, className, title, style,
        x, y, width, height, parent, menu, instance, parameter);
    if (hwnd && !g_unloading && reinterpret_cast<ULONG_PTR>(className) > 0xFFFF &&
        (!_wcsicmp(className, L"Shell_TrayWnd") ||
         !_wcsicmp(className, L"Shell_SecondaryTrayWnd"))) {
        // A monitor's taskbar can be created after the display-change message.
        g_taskbarDiscoveryRequested = true;
        if (g_taskbarWakeEvent) SetEvent(g_taskbarWakeEvent);
    }
    return hwnd;
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
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable, nullptr, true},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original, nullptr, true},
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
        FreeLibrary(taskbarDll);
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

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                       &CreateWindowExW_Original)) {
        return FALSE;
    }

    g_unloading = false;
    g_workersStopping = false;
    LoadSettings();
    g_systemLightTheme = RegistryUseLightTheme();
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<PCWSTR>(&ClassicWidgetWndProc), &g_widgetModule);
    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = ClassicWidgetWndProc;
    windowClass.hInstance = g_widgetModule;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_HAND);
    windowClass.lpszClassName = kClassicWidgetClass;
    if (!RegisterClassW(&windowClass)) return FALSE;
    // Worker events live across settings reloads. UI callbacks may signal the
    // audio event until the widget has been removed on the taskbar thread.
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_audioWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_mediaWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_taskbarWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_audioWakeEvent || !g_mediaWakeEvent || !g_taskbarWakeEvent) {
        Wh_Log(L"Failed to create worker events: %u", GetLastError());
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        if (g_audioWakeEvent) {
            CloseHandle(g_audioWakeEvent);
            g_audioWakeEvent = nullptr;
        }
        if (g_mediaWakeEvent) {
            CloseHandle(g_mediaWakeEvent);
            g_mediaWakeEvent = nullptr;
        }
        if (g_taskbarWakeEvent) {
            CloseHandle(g_taskbarWakeEvent);
            g_taskbarWakeEvent = nullptr;
        }
        UnregisterClassW(kClassicWidgetClass, g_widgetModule);
        return FALSE;
    }
    HookTaskbarDllSymbols();
    return TRUE;
}

void JoinAndDeleteThread(std::thread*& thread) {
    // Windhawk calls unload/settings handlers on its engine thread.
    if (!thread) return;
    if (thread->joinable()) thread->join();
    delete thread;
    thread = nullptr;
}

void StartWorkerThreads() {
    ResetEvent(g_stopEvent);
    ResetEvent(g_audioWakeEvent);
    ResetEvent(g_mediaWakeEvent);
    g_workersStopping = false;
    {
        std::lock_guard lock(g_commandMutex);
        g_commandQueue.clear();
    }
    g_commandThread = new std::thread(CommandThread);
    g_mediaThread = new std::thread(MediaThread);
    g_idleScanThread = new std::thread([] {
        try {
            ApartmentScope apartment;
            ScanIdleApps();
        } catch (...) {
            Wh_Log(L"Idle app scan failed");
        }
    });
    if (g_settings.visualizerEnabled) {
        g_audioThread = new std::thread(AudioVisualizerThread);
    }
    g_taskbarDiscoveryRequested = true;
    g_tickThread = new std::thread(UiTickThread);
}

void StopWorkerThreads() {
    g_workersStopping = true;
    SetEvent(g_stopEvent);
    {
        std::lock_guard lock(g_commandMutex);
        g_commandQueue.clear();
    }
    g_commandCondition.notify_all();
    if (g_audioWakeEvent) {
        SetEvent(g_audioWakeEvent);
    }

    JoinAndDeleteThread(g_commandThread);
    JoinAndDeleteThread(g_mediaThread);
    JoinAndDeleteThread(g_idleScanThread);
    JoinAndDeleteThread(g_audioThread);
    JoinAndDeleteThread(g_tickThread);
    {
        std::lock_guard lock(g_commandMutex);
        g_commandQueue.clear();
    }
}

void Wh_ModAfterInit() {
    StartWorkerThreads();
}

void Wh_ModUninit() {
    g_unloading = true;
    StopWorkerThreads();
    UnregisterClassW(kClassicWidgetClass, g_widgetModule);
    CloseHandle(g_audioWakeEvent);
    g_audioWakeEvent = nullptr;
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
    CloseHandle(g_mediaWakeEvent);
    g_mediaWakeEvent = nullptr;
    CloseHandle(g_taskbarWakeEvent);
    g_taskbarWakeEvent = nullptr;
}

void Wh_ModSettingsChanged() {
    StopWorkerThreads();
    LoadSettings();
    g_systemLightTheme = RegistryUseLightTheme();
    ++g_uiRevision;
    StartWorkerThreads();
}
