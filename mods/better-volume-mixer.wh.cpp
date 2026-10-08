// ==WindhawkMod==
// @id           better-volume-mixer
// @name         Better Volume Mixer
// @description  Quickly control master and per-app volume from the system tray
// @version      1.5.0
// @author       0Allu
// @github       https://github.com/0Allu
// @homepage     https://github.com/0Allu/better-volume-mixer
// @donateUrl    https://ko-fi.com/0allu
// @include      windhawk.exe
// @compilerOptions -lole32 -lshell32 -lgdi32 -luser32 -ldwmapi -ladvapi32 -lmsimg32 -loleaut32 -lgdiplus
// @license      MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Better Volume Mixer

A compact volume mixer for Windows 11. Control master volume, individual apps,
and playback devices directly from the system tray.

![Better Volume Mixer demo](https://raw.githubusercontent.com/0Allu/better-volume-mixer/main/assets/better-volume-mixer.png)

Click the speaker icon in the system tray to open or close the mixer. If it is
hidden, open the tray overflow menu and drag the icon onto the taskbar.

## Features

* Master and per-app volume controls
* Quick playback device switching
* Mute buttons and middle-click mute
* Muted master-volume indicator in the mixer and tray
* Middle-click the tray icon to toggle master mute
* Mouse-wheel volume adjustment
* Exact volume entry
* Keyboard controls
* Pin apps to the top of the mixer
* Right-click an app to copy its process name or hide it temporarily
* Open the classic Sound control panel from the tray menu
* Custom app names and default volumes
* Hide selected applications
* Full-name tooltips for shortened app and device names
* Compact mode
* Light, dark, and system themes
* Windows 11 and Windows 10 visual styles
* Background transparency, blur, and animations
* Configurable apps per page

Temporary hides last until the mod restarts. To undo them sooner, right-click
the tray icon and choose **Restore temporarily hidden sources**. Hiding a source
does not mute it.

Most appearance and behavior options can be changed from the mod settings.

## Hide the Windows volume icon

To avoid having two volume icons, install
[Taskbar tray system icon tweaks](https://windhawk.net/mods/taskbar-tray-system-icon-tweaks)
and enable **Hide volume icon** in its settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- compactMode: false
  $name: Compact mode
  $description: Use a smaller, denser layout with shorter rows and a narrower window, so more apps fit at once. Off by default.
- theme: system
  $name: Theme
  $description: Choose the mixer appearance or follow the current Windows theme.
  $options:
  - system: Follow Windows
  - dark: Dark
  - light: Light
- visualStyle: windows11
  $name: Visual style
  $description: Windows 11 has rounded corners and the Windows 11 slider. Windows 10 has square corners, neutral gray colors, the flat Windows 10 slider with its bar-shaped handle, and no gap above the taskbar. Either style works with every theme.
  $options:
  - windows11: Windows 11
  - windows10: Windows 10
- backgroundOpacity: 85
  $name: Background opacity
  $description: Background opacity from 0 to 100 percent. Text and controls remain opaque. Blur is applied where supported by Windows.
- backgroundEffect: blur
  $name: Background effect
  $description: What shows through the mixer while background opacity is below 100. Frosted glass blurs the background and evens out its brightness, so text stays readable over anything; lower the background opacity to make it more glassy. Blur only blurs it, and bright backgrounds can wash out the text at low opacity.
  $options:
  - glass: Frosted glass
  - blur: Blur
  - none: No blur
- windowBorder: true
  $name: Window border
  $description: Show the thin outline that Windows draws around the mixer. Turn off for a borderless look.
- animations: true
  $name: Enable animations
  $description: Animate opening and closing, hover effects, and volume changes. The mixer opens and closes like the taskbar flyouts of the chosen visual style. The Windows animation preference is also respected.
- volumeStep: 5
  $name: Mouse-wheel volume step
  $description: Percentage points per mouse-wheel step over an app or master-volume row. Valid range is 1 to 20; default is 5.
- keyboardControls: false
  $name: Keyboard controls
  $description: Up/Down selects a row, Left/Right changes volume by the mouse-wheel step, and M toggles mute. Works while the mixer is focused. Off by default.
- exactVolumeEntry: false
  $name: Exact volume entry
  $description: Click a percentage to enter a whole number from 0 to 100. Enter applies it; Escape or clicking elsewhere cancels. Off by default.
- closeWhenFocusIsLost: true
  $name: Close when focus is lost
  $description: Automatically close the mixer when you click somewhere else.
- maxVisibleApps: "7"
  $name: Applications per page
  $description: Maximum app rows per page, limited by available display space. Master volume does not count toward this limit.
  $options:
  - "1": "1"
  - "2": "2"
  - "3": "3"
  - "4": "4"
  - "5": "5"
  - "6": "6"
  - "7": "7"
  - "8": "8"
  - "9": "9"
  - "10": "10"
  - "11": "11"
  - "12": "12"
  - "13": "13"
  - "14": "14"
  - "15": "15"
- showAllSessions: false
  $name: Show all audio sessions
  $description: Show every audio session separately for each playback device, including unused or idle ones, instead of one row per app. Most users can leave this off.
- customApps:
  - - app: ""
      $name: App name
      $description: Enter the executable name (for example Spotify.exe or Spotify), its full path, or the original name shown in the mixer. Matching ignores capitalization. Leave blank to ignore this entry.
    - displayName: ""
      $name: Custom display name
      $description: Leave blank to keep the original name.
    - defaultVolume: -1
      $name: Default volume percentage
      $description: Use 0 to 100, or -1 to leave volume unchanged. Applied once when a session is detected, even with the mixer closed, and again if you change this default. Mute status is preserved.
  $name: Custom app configuration
  $description: Add an entry for each app to customize. The first matching entry wins. Up to 256 entries are supported.
- hiddenApps: [""]
  $name: Hidden applications
  $description: Add executable names (for example Spotify.exe or Spotify), full paths, or original mixer names to hide. Matching ignores capitalization. Blank entries are ignored. Hiding does not mute the app or disable its configured default volume. Up to 256 entries are supported.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <dwmapi.h>
#include <propsys.h>
#include <objidl.h>
#include <gdiplus.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Some Windhawk/MinGW headers only forward-declare this Core Audio interface.
// Define the standard COM method table, in ABI order, when it is missing.
// Keep this at global scope so it completes the header's forward declaration.
#ifndef __IAudioMeterInformation_INTERFACE_DEFINED__
#define __IAudioMeterInformation_INTERFACE_DEFINED__
MIDL_INTERFACE("c02216f6-8c67-4b5b-9d00-d008e73e0064")
IAudioMeterInformation : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* peak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(
        UINT count, float* peaks) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* mask) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IAudioMeterInformation, 0xc02216f6, 0x8c67, 0x4b5b,
                0x9d, 0x00, 0xd0, 0x08, 0xe7, 0x3e, 0x00, 0x64)
#endif
#endif

namespace {

constexpr PROPERTYKEY kDeviceFriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd,
     {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14};

#ifndef NOTIFYICON_VERSION_4
#define NOTIFYICON_VERSION_4 4
#endif
#ifndef NIF_GUID
#define NIF_GUID 0x00000020
#endif
#ifndef NIF_SHOWTIP
#define NIF_SHOWTIP 0x00000080
#endif
#ifndef NIN_SELECT
#define NIN_SELECT (WM_USER + 0)
#endif
#ifndef NINF_KEY
#define NINF_KEY 0x1
#endif
#ifndef NIN_KEYSELECT
#define NIN_KEYSELECT (NIN_SELECT | NINF_KEY)
#endif

constexpr UINT WM_APP_TRAY = WM_APP + 1;
constexpr UINT WM_APP_RELOAD_SETTINGS = WM_APP + 2;
constexpr UINT WM_APP_SHOW_MIXER = WM_APP + 3;
constexpr UINT WM_APP_FINISH_VOLUME_ENTRY = WM_APP + 4;
constexpr UINT WM_APP_AUDIO_CHANGED = WM_APP + 5;
constexpr UINT WM_APP_MASTER_STATE_CHANGED = WM_APP + 6;
constexpr UINT WM_APP_DEFAULT_OUTPUT_CHANGED = WM_APP + 7;
constexpr UINT_PTR TIMER_METERS = 1;
constexpr UINT_PTR TIMER_REFRESH = 2;
constexpr UINT_PTR TIMER_CHECK_FOCUS = 3;
constexpr UINT_PTR TIMER_NAME_TOOLTIP = 4;
constexpr UINT_PTR TIMER_TRIM = 5;
constexpr UINT NAME_TOOLTIP_DELAY = 1000;
constexpr UINT TRAY_ICON_ID = 0x4D58;
// Generated once for this mod. Never regenerate it on startup or on updates.
constexpr GUID MIXER_TRAY_GUID = {
    0x6bb7d04a, 0xc78a, 0x4a7a,
    {0xac, 0x7a, 0x81, 0x7b, 0x68, 0x0f, 0xfd, 0x51}};

constexpr UINT MENU_OPEN = 1;
constexpr UINT MENU_REFRESH = 2;
constexpr UINT MENU_SOUND_SETTINGS = 3;
constexpr UINT MENU_WINDOWS_MIXER = 4;
constexpr UINT MENU_CLASSIC_SOUND = 5;
constexpr UINT MENU_RESTORE_HIDDEN = 6;
constexpr UINT MENU_SOURCE_COPY_PROCESS = 7;
constexpr UINT MENU_SOURCE_HIDE = 8;

constexpr int DRAG_NONE = -2;
constexpr int DRAG_MASTER = -1;

struct CustomApp {
    std::wstring app;
    std::wstring displayName;
    int defaultVolume = -1;
};

struct Settings {
    std::wstring theme = L"system";
    bool windows10Style = false;
    bool showAllSessions = false;
    bool compactMode = false;
    bool windowBorder = true;
    bool closeWhenFocusIsLost = true;
    bool animations = true;
    int backgroundOpacity = 85;
    std::wstring backgroundEffect = L"blur";
    int maxVisibleApps = 7;
    int volumeStep = 5;
    bool keyboardControls = false;
    bool exactVolumeEntry = false;
    std::vector<CustomApp> customApps;
    std::vector<std::wstring> hiddenApps;
};

struct Theme {
    COLORREF background;
    COLORREF panel;
    COLORREF text;
    COLORREF secondaryText;
    COLORREF accent;
    COLORREF muted;
    COLORREF divider;
    COLORREF thumb;
    COLORREF sliderFill;
};

struct AppSession {
    DWORD processId = 0;
    std::wstring name;
    std::wstring executablePath;
    std::wstring pinKey;
    // Per-output pin keys saved before rows were merged across outputs.
    std::vector<std::wstring> legacyPinKeys;
    bool pinned = false;
    bool active = false;
    // Set while scanning: whether any session qualifies the row to be shown,
    // whether a hide rule matched, and which session named and leveled it.
    bool listed = false;
    bool hidden = false;
    int namePriority = -1;
    int levelPriority = -1;
    float volume = 1.0f;
    bool muted = false;
    HICON icon = nullptr;
    std::vector<ISimpleAudioVolume*> volumeControls;
    std::vector<IAudioMeterInformation*> meters;

    AppSession() = default;
    AppSession(const AppSession&) = delete;
    AppSession& operator=(const AppSession&) = delete;

    AppSession(AppSession&& other) noexcept {
        *this = std::move(other);
    }

    AppSession& operator=(AppSession&& other) noexcept {
        if (this != &other) {
            Reset();
            processId = other.processId;
            name = std::move(other.name);
            executablePath = std::move(other.executablePath);
            pinKey = std::move(other.pinKey);
            legacyPinKeys = std::move(other.legacyPinKeys);
            pinned = other.pinned;
            active = other.active;
            listed = other.listed;
            hidden = other.hidden;
            namePriority = other.namePriority;
            levelPriority = other.levelPriority;
            volume = other.volume;
            muted = other.muted;
            icon = other.icon;
            volumeControls = std::move(other.volumeControls);
            meters = std::move(other.meters);
            other.icon = nullptr;
            other.volumeControls.clear();
            other.meters.clear();
        }
        return *this;
    }

    ~AppSession() {
        Reset();
    }

    void Reset() {
        for (auto* control : volumeControls) {
            control->Release();
        }
        volumeControls.clear();
        for (auto* meter : meters) {
            meter->Release();
        }
        meters.clear();
        if (icon) {
            DestroyIcon(icon);
            icon = nullptr;
        }
    }
};

class EndpointVolumeNotification final : public IAudioEndpointVolumeCallback {
public:
    explicit EndpointVolumeNotification(HWND window) : m_window(window) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (iid != __uuidof(IUnknown) &&
            iid != __uuidof(IAudioEndpointVolumeCallback)) {
            return E_NOINTERFACE;
        }
        *object = static_cast<IAudioEndpointVolumeCallback*>(this);
        AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return static_cast<ULONG>(InterlockedIncrement(&m_references));
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG remaining = static_cast<ULONG>(InterlockedDecrement(&m_references));
        if (!remaining) delete this;
        return remaining;
    }

    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override {
        HWND window = m_window.load();
        if (window) PostMessageW(window, WM_APP_MASTER_STATE_CHANGED, 0, 0);
        return S_OK;
    }

    void SetWindow(HWND window) { m_window.store(window); }

private:
    ~EndpointVolumeNotification() = default;
    LONG m_references = 1;
    std::atomic<HWND> m_window{nullptr};
};

Settings g_settings;
Theme g_theme;
bool g_lightTheme;

HANDLE g_thread;
HANDLE g_windowReadyEvent;
std::atomic<HWND> g_hWnd;
UINT g_taskbarCreatedMessage;

HICON g_trayIcon;
HICON g_masterIcon;
std::unordered_map<std::wstring, HICON> g_iconCache;
HFONT g_titleFont;
HFONT g_bodyFont;
HFONT g_smallFont;
HFONT g_symbolFont;
ULONG_PTR g_gdiplusToken;
Gdiplus::Font* g_titleTypography;
Gdiplus::Font* g_bodyTypography;
Gdiplus::Font* g_smallTypography;

// Scale of the thumb's accent dot in each pointer state, as used by the
// native Windows 11 slider.
constexpr float THUMB_DOT_REST = 0.86f;
constexpr float THUMB_DOT_HOVER = 1.167f;
constexpr float THUMB_DOT_PRESSED = 0.71f;

enum class ThumbState { Rest, Hover, Pressed };

struct RowVisual {
    float hover = 0.0f;
    float volume = 0.0f;
    float peak = 0.0f;
    float thumbDot = THUMB_DOT_REST;
    ThumbState thumbState = ThumbState::Rest;
    bool initialized = false;
};
std::unordered_map<std::wstring, RowVisual> g_rowVisuals;
bool g_motionEnabled;
bool g_closeHovered;
int g_hoverPageButton = -1;
float g_closeHoverAmount;
float g_frameSeconds = 0.016f;
// Animation times are AnimationClock() values, in microseconds.
ULONGLONG g_lastPaintTime;
ULONGLONG g_openTime;  // Start of the opening animation, while it runs.
ULONGLONG g_closeTime; // Start of the closing animation, while it runs.
bool g_openWarmup; // The first frame of the opening animation is still due.
bool g_closeQueued; // A close was asked for while the flyout was opening.
ULONGLONG g_frameRenderTime; // When the translucent frame was drawn; 0: none.
ULONGLONG g_pacedSince;
// The flyout opens and closes by sliding towards the taskbar edge its tray
// icon is on. Distances are in pixels, positions in screen coordinates.
RECT g_restRect{};     // Where the open flyout rests.
RECT g_slideClip{};    // The flyout is cut off outside this area as it slides.
int g_slideTravel;     // How far from its resting place a closed flyout is.
bool g_slideFromAbove; // The taskbar is above the flyout rather than below.
// Where the running animation set off from and what the last frame showed,
// so that an interrupted animation carries on from where the flyout is.
float g_slideStart;
float g_fadeStart = 1.0f;
float g_slideOffset;
float g_fadeOpacity = 1.0f;
int g_paintMatte = -1; // -1: opaque, 0/1: black/white alpha-recovery pass.
bool g_transparencyFailed;
float g_masterPaintVolume;
bool g_masterPaintMuted;
bool g_frameAnimating;
bool g_frameHasAudio;
UINT g_paintTimerInterval;

IAudioEndpointVolume* g_endpointVolume;
EndpointVolumeNotification* g_endpointVolumeCallback;
bool g_masterIconMuted;
std::wstring g_endpointName;
// COM interfaces and icons must never be released by a process-exit destructor.
// Refresh clears the contents; the UI thread resets storage on controlled exit.
[[clang::no_destroy]] std::optional<std::vector<AppSession>> g_apps{std::in_place};
// Session instance IDs are unique across app instances. Do not remember PIDs:
// they can be reused, and one process can have sessions on multiple outputs.
std::unordered_set<std::wstring> g_activatedSessions;
std::unordered_set<std::wstring> g_liveSessions;
// Session instance IDs survive UI refreshes and distinguish restarted apps.
// Access is shared by the UI and Core Audio notification threads.
SRWLOCK g_defaultVolumeLock = SRWLOCK_INIT;
std::unordered_map<std::wstring, int> g_appliedDefaultVolumes;
std::unordered_set<std::wstring> g_liveDefaultSessions;

HANDLE g_audioNotificationThread;
HANDLE g_audioNotificationStopEvent;
HANDLE g_audioNotificationRebuildEvent;
HANDLE g_audioNotificationSessionEvent;
std::atomic<bool> g_audioUiRefreshPosted;

// Lifetime IMMNotificationClient used on the UI thread. It exists for the
// window's whole lifetime so default-output changes are always observed, even
// while the mixer is hidden. It posts WM_APP_DEFAULT_OUTPUT_CHANGED.
IMMDeviceEnumerator* g_uiDeviceEnumerator = nullptr;
class EndpointNotification;

int g_dpi = 96;
int g_scrollRow;
int g_availableAppRows = 15;
int g_hoverRow = DRAG_NONE;
bool g_hoverThumb; // The pointer is over the slider thumb of g_hoverRow.
POINT g_pointer{}; // Last pointer position, in client coordinates.
int g_dragRow = DRAG_NONE;
bool g_mouseTracking;
bool g_audioAvailable;
bool g_trayVersion4;
bool g_trayUsesGuid = true;
bool g_showRequestPending;
bool g_closeRequestPending;
bool g_trayPressKnown;
bool g_trayPressWasVisible;
bool g_showingMixer;
bool g_outputMenuOpen;
bool g_sourceMenuOpen;
bool g_outputHovered;
bool g_pendingDefaultOutputChange;
std::unordered_set<std::wstring> g_temporarilyHiddenSources;
HWND g_nameTooltip;
std::wstring g_nameTooltipText;
RECT g_nameTooltipRect{};
bool g_nameTooltipClipped;
ULONGLONG g_nameTooltipSince;

void CancelNameTooltip(HWND window);

int g_selectedRow = DRAG_MASTER;
DWORD g_selectedProcessId;
std::wstring g_selectedPinKey;
HWND g_volumeEntry;
HWND g_volumeEdit;
int g_volumeEntryRow = DRAG_NONE;
ULONG_PTR g_volumeEntryGeneration;
constexpr PCWSTR VOLUME_ENTRY_CLASS = L"WindhawkBetterVolumeMixerEntry";
constexpr PCWSTR NAME_TOOLTIP_CLASS = L"WindhawkBetterVolumeMixerTooltip";

void SelectRow(int row) {
    g_selectedRow = row;
    g_selectedPinKey.clear();
    if (row >= 0 && row < static_cast<int>(g_apps->size())) {
        g_selectedProcessId = (*g_apps)[row].processId;
        g_selectedPinKey = (*g_apps)[row].pinKey;
    }
}

void RestoreSelectedRow() {
    if (g_selectedRow < 0) return;
    for (size_t i = 0; i < g_apps->size(); ++i) {
        const auto& app = (*g_apps)[i];
        if (app.processId == g_selectedProcessId && app.pinKey == g_selectedPinKey) {
            g_selectedRow = static_cast<int>(i);
            return;
        }
    }
    SelectRow(DRAG_MASTER);
}


int Scale(int value) {
    return MulDiv(value, g_dpi, 96);
}

bool IsWindows10Style() {
    return g_settings.windows10Style;
}

// Corner radius for the current visual style: the Windows 10 style is square.
int Radius(int value) {
    return IsWindows10Style() ? 0 : Scale(value);
}

int ClampInt(int value, int minimum, int maximum) {
    return value < minimum ? minimum : (value > maximum ? maximum : value);
}

float ClampVolume(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

std::wstring TrimSetting(const wchar_t* value) {
    std::wstring text = value ? value : L"";
    size_t first = text.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return {};
    return text.substr(first, text.find_last_not_of(L" \t\r\n") - first + 1);
}

bool HasDefaultVolumeRules() {
    return std::any_of(g_settings.customApps.begin(), g_settings.customApps.end(),
                      [](const CustomApp& rule) { return rule.defaultVolume >= 0; });
}

void LoadSettings() {
    WindhawkUtils::StringSetting theme =
        WindhawkUtils::StringSetting::make(L"theme");
    g_settings.theme = theme.get();
    if (g_settings.theme != L"dark" && g_settings.theme != L"light") {
        g_settings.theme = L"system";
    }
    WindhawkUtils::StringSetting style =
        WindhawkUtils::StringSetting::make(L"visualStyle");
    g_settings.windows10Style = wcscmp(style.get(), L"windows10") == 0;

    g_settings.showAllSessions =
        Wh_GetIntSetting(L"showAllSessions") != 0;
    g_settings.closeWhenFocusIsLost =
        Wh_GetIntSetting(L"closeWhenFocusIsLost") != 0;
    g_settings.animations = Wh_GetIntSetting(L"animations") != 0;
    g_settings.backgroundOpacity = ClampInt(Wh_GetIntSetting(L"backgroundOpacity"), 0, 100);
    WindhawkUtils::StringSetting effect =
        WindhawkUtils::StringSetting::make(L"backgroundEffect");
    g_settings.backgroundEffect = effect.get();
    if (g_settings.backgroundEffect != L"glass" &&
        g_settings.backgroundEffect != L"none") {
        g_settings.backgroundEffect = L"blur";
    }
    WindhawkUtils::StringSetting pageSize =
        WindhawkUtils::StringSetting::make(L"maxVisibleApps");
    g_settings.maxVisibleApps = ClampInt(_wtoi(pageSize.get()), 1, 15);
    g_settings.compactMode = Wh_GetIntSetting(L"compactMode") != 0;
    g_settings.windowBorder = Wh_GetIntSetting(L"windowBorder") != 0;
    g_settings.keyboardControls = Wh_GetIntSetting(L"keyboardControls") != 0;
    g_settings.exactVolumeEntry = Wh_GetIntSetting(L"exactVolumeEntry") != 0;
    g_settings.volumeStep =
        ClampInt(Wh_GetIntSetting(L"volumeStep"), 1, 20);

    g_settings.customApps.clear();
    // The settings API has no array length query. Scan a bounded range so a
    // blank entry does not hide valid entries placed after it.
    for (int i = 0; i < 256; ++i) {
        auto app = WindhawkUtils::StringSetting::make(L"customApps[%d].app", i);
        CustomApp rule;
        rule.app = TrimSetting(app.get());
        if (rule.app.empty()) continue;
        auto name = WindhawkUtils::StringSetting::make(L"customApps[%d].displayName", i);
        rule.displayName = TrimSetting(name.get());
        int volume = Wh_GetIntSetting(L"customApps[%d].defaultVolume", i);
        rule.defaultVolume = volume >= 0 && volume <= 100 ? volume : -1;
        g_settings.customApps.push_back(std::move(rule));
    }
    g_settings.hiddenApps.clear();
    for (int i = 0; i < 256; ++i) {
        auto app = WindhawkUtils::StringSetting::make(L"hiddenApps[%d]", i);
        std::wstring name = TrimSetting(app.get());
        if (!name.empty()) g_settings.hiddenApps.push_back(std::move(name));
    }
}

enum class AccentState : int {
    Disabled = 0,
    BlurBehind = 3,
    AcrylicBlurBehind = 4,
};

struct AccentPolicy {
    AccentState state;
    DWORD flags;
    DWORD gradientColor;
    DWORD animationId;
};

struct WindowCompositionAttributeData {
    int attribute;
    void* data;
    SIZE_T size;
};

void ApplyBackdropBlur(HWND hWnd) {
    using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(
        HWND, const WindowCompositionAttributeData*);

    static auto setWindowCompositionAttribute =
        reinterpret_cast<SetWindowCompositionAttribute_t>(
            GetProcAddress(GetModuleHandleW(L"user32.dll"),
                           "SetWindowCompositionAttribute"));

    if (!setWindowCompositionAttribute) {
        return;
    }

    bool transparent = g_settings.backgroundOpacity < 100 && !g_transparencyFailed;
    bool blur = transparent && g_settings.backgroundEffect != L"none";
    AccentPolicy policy{};
    WindowCompositionAttributeData data{19, &policy, sizeof(policy)};  // WCA_ACCENT_POLICY.

    if (blur && g_settings.backgroundEffect == L"glass") {
        // Acrylic blurs the backdrop and also pulls its brightness towards
        // the tint, so a bright or busy background cannot wash out the text.
        // The tint is nearly clear (ABGR, alpha 1; 0 would disable the
        // effect): the mixer paints its own translucent background on top,
        // which is what backgroundOpacity controls.
        policy.state = AccentState::AcrylicBlurBehind;
        policy.flags = 2;  // Use gradientColor as the tint.
        policy.gradientColor = 0x01000000 | (g_theme.background & 0x00FFFFFF);
        if (setWindowCompositionAttribute(hWnd, &data)) {
            return;
        }
        Wh_Log(L"Mixer: frosted glass backdrop unavailable; using plain blur");
        policy = {};
    }

    // Keep the plain blur untinted, because the mixer already paints its own
    // theme-aware translucent background on top.
    policy.state = blur ? AccentState::BlurBehind : AccentState::Disabled;
    if (!setWindowCompositionAttribute(hWnd, &data) && blur) {
        Wh_Log(L"Mixer: backdrop blur unavailable; using transparency without blur");
    }
}

// Windows 11 rounds the window on request and outlines it with a thin border
// unless told not to.
void ApplyWindowFrame(HWND hWnd) {
    // DWMWA_WINDOW_CORNER_PREFERENCE: DWMWCP_DONOTROUND or DWMWCP_ROUND.
    DWORD corner = IsWindows10Style() ? 1 : 2;
    DwmSetWindowAttribute(hWnd, 33, &corner, sizeof(corner));
    // DWMWA_BORDER_COLOR: DWMWA_COLOR_DEFAULT or DWMWA_COLOR_NONE.
    COLORREF color = g_settings.windowBorder ? 0xFFFFFFFF : 0xFFFFFFFE;
    DwmSetWindowAttribute(hWnd, 34, &color, sizeof(color));
}

void ApplyTransparencyStyle(HWND hWnd) {
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    bool transparent = g_settings.backgroundOpacity < 100 && !g_transparencyFailed;
    LONG_PTR next = transparent ? style | WS_EX_LAYERED : style & ~WS_EX_LAYERED;
    if (style != next) {
        SetLastError(0);
        if (!SetWindowLongPtrW(hWnd, GWL_EXSTYLE, next) && GetLastError()) {
            g_transparencyFailed = true;
            Wh_Log(L"Mixer: could not enable background transparency");
        }
        SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
    ApplyBackdropBlur(hWnd);
}

void SetPaintTimer(HWND hWnd, UINT interval) {
    if (g_paintTimerInterval != interval &&
        SetTimer(hWnd, TIMER_METERS, interval, nullptr)) {
        g_paintTimerInterval = interval;
    }
}

UINT ChoosePaintInterval(bool motion, bool animating, bool audio) {
    if (motion && animating) return 16;
    if (audio) return motion ? 33 : 80;
    return 200;
}

void UpdatePaintTimer(HWND hWnd) {
    if (!IsWindowVisible(hWnd)) return;
    SetPaintTimer(hWnd, ChoosePaintInterval(g_motionEnabled,
        g_frameAnimating || g_openTime || g_closeTime || g_dragRow != DRAG_NONE,
        g_frameHasAudio));
}

// Microseconds from a steady high-resolution clock. GetTickCount64 advances
// in steps of about 16 ms, which is too coarse to time animation frames.
ULONGLONG AnimationClock() {
    static const LONGLONG frequency = [] {
        LARGE_INTEGER value;
        QueryPerformanceFrequency(&value);
        return value.QuadPart;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<ULONGLONG>(counter.QuadPart / frequency * 1000000 +
                                  counter.QuadPart % frequency * 1000000 / frequency);
}

// Call after presenting a frame. Timer messages arrive on a coarse and uneven
// 15 or 31 ms cadence, which makes short animations stutter. While something
// is animating, wait for the compositor to show this frame and then request
// the next one straight away, so frames follow the display's refresh rate.
void PaceAnimation(HWND hWnd) {
    if (g_openWarmup) {
        // The first frame after opening is slow, because caches are cold, and
        // was presented fully transparent. Start the animation only now.
        g_openWarmup = false;
        if (g_openTime) g_openTime = AnimationClock();
    }
    if (!g_motionEnabled || !(g_frameAnimating || g_openTime || g_closeTime) ||
        !IsWindowVisible(hWnd)) {
        g_pacedSince = 0;
        return;
    }
    // Paint messages outrank timer messages. Should an animation ever fail
    // to settle, fall back to the paint timer rather than starve the others.
    ULONGLONG now = AnimationClock();
    if (!g_pacedSince) g_pacedSince = now;
    if (now - g_pacedSince < 2000000 && SUCCEEDED(DwmFlush())) {
        InvalidateRect(hWnd, nullptr, FALSE);
    }
}

// Windows 11's entrance curve, cubic-bezier(0, 0, 0, 1): quick off the mark,
// then a long settle. Its exit curve, cubic-bezier(1, 0, 1, 1), is the same
// motion run backwards.
float EntranceEase(float progress) {
    float t = std::cbrt(progress);
    return t * t * (3.0f - 2.0f * t);
}

struct FlyoutFrame {
    float offset; // Distance from the resting place, towards the taskbar.
    float opacity;
    bool finished;
};

// The taskbar flyouts of Windows 11 slide all the way out from the taskbar's
// edge and back again, without fading. Those of Windows 10 fade in while they
// slide a short distance, and close without an animation (see HideMixer).
FlyoutFrame FlyoutFrameAt(ULONGLONG now) {
    bool closing = g_closeTime != 0;
    bool windows10 = IsWindows10Style();
    ULONGLONG start = closing ? g_closeTime : g_openTime;
    float offset = closing ? static_cast<float>(g_slideTravel) : 0.0f;
    if (!start || !g_motionEnabled) return {offset, 1.0f, true};
    float duration = closing ? 167000.0f : windows10 ? 370000.0f : 250000.0f;
    float progress = now > start
        ? ClampVolume(static_cast<float>(now - start) / duration) : 0.0f;
    float eased = closing ? 1.0f - EntranceEase(1.0f - progress)
        : windows10 ? 1.0f - std::pow(1.0f - progress, 5.0f)
        : EntranceEase(progress);
    return {g_slideStart + (offset - g_slideStart) * eased,
            g_fadeStart + (1.0f - g_fadeStart) * eased, progress >= 1.0f};
}

// The part of the flyout that shows when it is `offset` pixels away from its
// resting place, and where that part starts within the flyout.
RECT FlyoutBounds(float offset, POINT* origin) {
    int shift = static_cast<int>(std::lround(offset));
    RECT whole = g_restRect;
    OffsetRect(&whole, 0, g_slideFromAbove ? -shift : shift);
    RECT shown = whole;
    if (!IntersectRect(&shown, &whole, &g_slideClip)) shown = whole;
    *origin = {shown.left - whole.left, shown.top - whole.top};
    return shown;
}

// Moves an opaque flyout's window to where this frame of the animation has
// it, and returns where the window starts within the flyout. A translucent
// flyout is moved along with its contents instead; see PaintTransparentMixer.
POINT PlaceFlyoutFrame(HWND hWnd, const FlyoutFrame& frame) {
    POINT origin{};
    RECT bounds = FlyoutBounds(frame.offset, &origin);
    SetWindowPos(hWnd, nullptr, bounds.left, bounds.top, bounds.right - bounds.left,
                 bounds.bottom - bounds.top, SWP_NOZORDER | SWP_NOACTIVATE);
    g_slideOffset = frame.offset;
    g_fadeOpacity = frame.opacity;
    return origin;
}

// Puts the window in the flyout's resting place, whole.
void RestFlyoutWindow(HWND hWnd) {
    UINT flags = SWP_NOOWNERZORDER | SWP_NOACTIVATE;
    if (IsRectEmpty(&g_restRect)) flags |= SWP_NOMOVE | SWP_NOSIZE;
    SetWindowPos(hWnd, HWND_TOPMOST, g_restRect.left, g_restRect.top,
                 g_restRect.right - g_restRect.left,
                 g_restRect.bottom - g_restRect.top, flags);
}

// Ends the opening animation with the flyout in place.
void SettleFlyout(HWND hWnd) {
    g_openTime = 0;
    g_openWarmup = false;
    g_slideOffset = 0.0f;
    g_fadeOpacity = 1.0f;
    RestFlyoutWindow(hWnd);
    InvalidateRect(hWnd, nullptr, FALSE);
}

void CompleteHide(HWND hWnd);
void HideMixer(HWND hWnd);

// Jumps to the end of whichever of the two animations is running.
void FinishFlyoutMotion(HWND hWnd) {
    if (g_closeTime) {
        CompleteHide(hWnd);
    } else if (g_openTime) {
        SettleFlyout(hWnd);
    }
}

// Like the taskbar's own flyouts, the mixer finishes opening before it
// closes. Call after a frame, to carry out a close that had to wait.
void RunQueuedClose(HWND hWnd) {
    if (g_closeQueued && !g_openTime) {
        g_closeQueued = false;
        HideMixer(hWnd);
    }
}

// The flyout's whole size. Its window can be cut short while it slides.
RECT MixerContentRect(HWND hWnd) {
    RECT rect{};
    if (g_openTime || g_closeTime) {
        rect.right = g_restRect.right - g_restRect.left;
        rect.bottom = g_restRect.bottom - g_restRect.top;
    } else {
        GetClientRect(hWnd, &rect);
    }
    return rect;
}

// Open as far as the user is concerned: on screen and not on its way out.
bool MixerOpen(HWND hWnd) {
    return IsWindowVisible(hWnd) && !g_closeTime;
}

void UpdateMotionPreference(HWND hWnd) {
    BOOL enabled = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0);
    g_motionEnabled = g_settings.animations && enabled;
    if (!g_motionEnabled) {
        FinishFlyoutMotion(hWnd);
    }
    if (IsWindowVisible(hWnd)) {
        SetPaintTimer(hWnd, g_motionEnabled ? 16 : 80);
    }
}

bool WindowsUsesLightTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    LSTATUS result = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return result == ERROR_SUCCESS && value != 0;
}

// AccentPalette holds eight RGBA entries, ordered from the lightest shade to
// the darkest, with the base accent color at index 3. Windows 11 controls use
// a lighter shade (1) on dark surfaces and a darker one (4) on light surfaces.
bool ReadAccentShade(int index, COLORREF* color) {
    BYTE palette[32];
    DWORD size = sizeof(palette);
    LSTATUS result = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
        L"AccentPalette", RRF_RT_REG_BINARY, nullptr, palette, &size);
    if (result != ERROR_SUCCESS || size < sizeof(palette)) return false;
    const BYTE* shade = palette + index * 4;
    *color = RGB(shade[0], shade[1], shade[2]);
    return true;
}

void ClearTextCache();

void UpdateTheme() {
    ClearTextCache();
    bool light = g_settings.theme == L"light" ||
                 (g_settings.theme == L"system" && WindowsUsesLightTheme());
    g_lightTheme = light;

    DWORD colorization = 0;
    BOOL opaque = FALSE;
    COLORREF accent = RGB(0, 120, 212);
    if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque))) {
        accent = RGB((colorization >> 16) & 0xFF,
                     (colorization >> 8) & 0xFF,
                     colorization & 0xFF);
    }
    // Prefer the shade the native controls use; keep the DWM color otherwise.
    ReadAccentShade(light ? 4 : 1, &accent);
    // Windows 10 sliders use the base accent color in both themes.
    COLORREF sliderFill = accent;
    if (IsWindows10Style()) ReadAccentShade(3, &sliderFill);

    if (IsWindows10Style()) {
        // The neutral grays of the Windows 10 flyouts.
        if (light) {
            g_theme = {RGB(230, 230, 230), RGB(242, 242, 242),
                       RGB(0, 0, 0), RGB(95, 95, 95),
                       accent, RGB(120, 120, 120), RGB(204, 204, 204),
                       RGB(255, 255, 255), sliderFill};
        } else {
            g_theme = {RGB(31, 31, 31), RGB(43, 43, 43),
                       RGB(255, 255, 255), RGB(170, 170, 170),
                       accent, RGB(140, 140, 140), RGB(60, 60, 60),
                       RGB(69, 69, 69), sliderFill};
        }
        return;
    }

    // The slider thumb uses the native Windows 11 solid control color.
    if (light) {
        g_theme = {RGB(247, 248, 250), RGB(255, 255, 255),
                   RGB(27, 31, 38), RGB(98, 105, 117),
                   accent, RGB(115, 122, 134), RGB(228, 231, 236),
                   RGB(255, 255, 255), sliderFill};
    } else {
        g_theme = {RGB(27, 29, 34), RGB(38, 41, 48),
                   RGB(242, 244, 248), RGB(159, 168, 183),
                   accent, RGB(138, 146, 159), RGB(48, 53, 62),
                   RGB(69, 69, 69), sliderFill};
    }
}

void DeleteFonts() {
    ClearTextCache();
    delete g_titleTypography;
    delete g_bodyTypography;
    delete g_smallTypography;
    g_titleTypography = g_bodyTypography = g_smallTypography = nullptr;
    if (g_symbolFont) {
        DeleteObject(g_symbolFont);
        g_symbolFont = nullptr;
    }
    if (g_titleFont) {
        DeleteObject(g_titleFont);
        g_titleFont = nullptr;
    }
    if (g_bodyFont) {
        DeleteObject(g_bodyFont);
        g_bodyFont = nullptr;
    }
    if (g_smallFont) {
        DeleteObject(g_smallFont);
        g_smallFont = nullptr;
    }
}

Gdiplus::Font* CreateTypographyFont(PCWSTR preferredFamily, float pixels) {
    Gdiplus::FontFamily family(preferredFamily);
    Gdiplus::Font* font = nullptr;
    if (family.GetLastStatus() == Gdiplus::Ok &&
        family.IsStyleAvailable(Gdiplus::FontStyleRegular)) {
        font = new Gdiplus::Font(&family, pixels, Gdiplus::FontStyleRegular,
                                 Gdiplus::UnitPixel);
        if (font->GetLastStatus() == Gdiplus::Ok) return font;
        delete font;
    }
    font = new Gdiplus::Font(L"Segoe UI", pixels, Gdiplus::FontStyleRegular,
                             Gdiplus::UnitPixel);
    if (font->GetLastStatus() == Gdiplus::Ok) return font;
    delete font;
    return nullptr; // DrawLabel retains a GDI fallback.
}

int CALLBACK FontFound(const LOGFONTW*, const TEXTMETRICW*, DWORD, LPARAM found) {
    *reinterpret_cast<bool*>(found) = true;
    return 0;
}

bool FontInstalled(PCWSTR face) {
    LOGFONTW request{};
    request.lfCharSet = DEFAULT_CHARSET;
    lstrcpynW(request.lfFaceName, face, LF_FACESIZE);
    bool found = false;
    HDC dc = GetDC(nullptr);
    EnumFontFamiliesExW(dc, &request, FontFound, reinterpret_cast<LPARAM>(&found), 0);
    ReleaseDC(nullptr, dc);
    return found;
}

void CreateFonts(HWND hWnd) {
    DeleteFonts();
    g_dpi = static_cast<int>(GetDpiForWindow(hWnd));
    if (!g_dpi) {
        g_dpi = 96;
    }

    // Windows 10 does not come with the Windows 11 icon font. Its own has the
    // same icons at the same code points; without it, they would be missing.
    bool fluentIcons = !IsWindows10Style() && FontInstalled(L"Segoe Fluent Icons");

    g_titleFont = CreateFontW(
        -MulDiv(15, g_dpi, 72), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_bodyFont = CreateFontW(
        -MulDiv(10, g_dpi, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_smallFont = CreateFontW(
        -MulDiv(9, g_dpi, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_symbolFont = CreateFontW(
        -MulDiv(13, g_dpi, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        fluentIcons ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets");
    if (g_gdiplusToken) {
        float density = g_dpi / 96.0f;
        PCWSTR textFamily = IsWindows10Style() ? L"Segoe UI" : L"Segoe UI Variable Text";
        g_titleTypography = CreateTypographyFont(L"Segoe UI Semibold", 20.0f * density);
        g_bodyTypography = CreateTypographyFont(textFamily, 14.0f * density);
        g_smallTypography = CreateTypographyFont(textFamily, 12.0f * density);
    }
}

std::wstring BaseNameWithoutExtension(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    std::wstring name = slash == std::wstring::npos ? path : path.substr(slash + 1);
    size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        name.resize(dot);
    }
    return name;
}

std::wstring GetProcessPath(DWORD processId) {
    if (!processId) {
        return {};
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                 processId);
    if (!process) {
        return {};
    }

    std::vector<WCHAR> buffer(32768);
    DWORD size = static_cast<DWORD>(buffer.size());
    std::wstring result;
    if (QueryFullProcessImageNameW(process, 0, buffer.data(), &size)) {
        result.assign(buffer.data(), size);
    }
    CloseHandle(process);
    return result;
}

bool IsAudioEngineSession(const std::wstring& processPath, bool systemSounds) {
    if (systemSounds || processPath.empty()) {
        return false;
    }
    size_t slash = processPath.find_last_of(L"\\/");
    const wchar_t* filename = processPath.c_str() +
        (slash == std::wstring::npos ? 0 : slash + 1);
    return _wcsicmp(filename, L"audiodg.exe") == 0;
}

void ClearIconCache() {
    for (const auto& entry : g_iconCache) {
        if (entry.second) {
            DestroyIcon(entry.second);
        }
    }
    g_iconCache.clear();
}

HICON ExtractSizedIcon(const std::wstring& path, int pixels) {
    if (path.empty()) {
        return nullptr;
    }

    HICON icon = nullptr;
    HRESULT hr = SHDefExtractIconW(path.c_str(), 0, 0, &icon, nullptr,
                                   MAKELONG(pixels, 0));
    if (SUCCEEDED(hr) && icon) {
        return icon;
    }
    if (icon) {
        DestroyIcon(icon);
    }

    SHFILEINFOW info = {};
    if (SHGetFileInfoW(path.c_str(), FILE_ATTRIBUTE_NORMAL, &info, sizeof(info),
                       SHGFI_ICON | SHGFI_LARGEICON)) {
        return info.hIcon;
    }
    return nullptr;
}

HICON GetExecutableIcon(const std::wstring& path) {
    if (path.empty()) {
        return nullptr;
    }
    int pixels = Scale(24);
    std::wstring key = std::to_wstring(pixels) + L":" + path;
    auto found = g_iconCache.find(key);
    if (found != g_iconCache.end()) {
        return found->second ? CopyIcon(found->second) : nullptr;
    }
    if (g_iconCache.size() >= 256) {
        ClearIconCache();
    }
    HICON icon = ExtractSizedIcon(path, pixels);
    g_iconCache.emplace(std::move(key), icon);
    return icon ? CopyIcon(icon) : nullptr;
}

std::wstring GetSessionDisplayName(IAudioSessionControl* control,
                                   DWORD processId,
                                   bool systemSounds,
                                   std::wstring* processPath) {
    if (systemSounds) {
        return L"System sounds";
    }

    LPWSTR displayName = nullptr;
    if (SUCCEEDED(control->GetDisplayName(&displayName)) && displayName) {
        std::wstring value = displayName;
        CoTaskMemFree(displayName);
        if (!value.empty() && value.front() != L'@') {
            return value;
        }
    }

    if (processPath->empty()) {
        *processPath = GetProcessPath(processId);
    }
    std::wstring name = BaseNameWithoutExtension(*processPath);
    return name.empty() ? L"Unknown application" : name;
}

std::wstring ReadEndpointName(IMMDevice* device) {
    IPropertyStore* store = nullptr;
    if (FAILED(device->OpenPropertyStore(STGM_READ, &store)) || !store) {
        return L"Default output";
    }

    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring result = L"Default output";
    if (SUCCEEDED(store->GetValue(kDeviceFriendlyName, &value)) &&
        value.vt == VT_LPWSTR && value.pwszVal) {
        result = value.pwszVal;
    }
    PropVariantClear(&value);
    store->Release();
    return result;
}

void ReleaseDefaultEndpointVolume() {
    if (g_endpointVolumeCallback) {
        g_endpointVolumeCallback->SetWindow(nullptr);
        if (g_endpointVolume) {
            g_endpointVolume->UnregisterControlChangeNotify(g_endpointVolumeCallback);
        }
        g_endpointVolumeCallback->Release();
        g_endpointVolumeCallback = nullptr;
    }
    if (g_endpointVolume) {
        g_endpointVolume->Release();
        g_endpointVolume = nullptr;
    }
}

// Binds the given endpoint, replacing any previous binding. Registers the
// IAudioEndpointVolumeCallback against the supplied window so that mute and
// volume changes propagate even when the mixer is hidden.
bool BindDefaultEndpointVolumeFromDevice(IMMDevice* device, HWND hWnd) {
    if (!device) return false;

    g_endpointName = ReadEndpointName(device);

    HRESULT hr = device->Activate(
        __uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr,
        reinterpret_cast<void**>(&g_endpointVolume));
    if (FAILED(hr) || !g_endpointVolume) {
        g_endpointVolume = nullptr;
        g_endpointName.clear();
        return false;
    }

    auto* callback = new EndpointVolumeNotification(hWnd);
    if (SUCCEEDED(g_endpointVolume->RegisterControlChangeNotify(callback))) {
        g_endpointVolumeCallback = callback;
    } else {
        callback->Release();
    }
    return true;
}

// Resolves the current eRender/eMultimedia endpoint and binds it, releasing
// any previous binding first. Safe to call while the mixer is hidden.
bool BindDefaultEndpointVolume(HWND hWnd) {
    ReleaseDefaultEndpointVolume();

    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator),
        reinterpret_cast<void**>(&enumerator));
    if (FAILED(hr) || !enumerator) return false;

    IMMDevice* device = nullptr;
    hr = enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);
    enumerator->Release();
    if (FAILED(hr) || !device) return false;

    bool result = BindDefaultEndpointVolumeFromDevice(device, hWnd);
    device->Release();
    return result;
}

void ReleaseAudioData() {
    g_apps->clear();
    ReleaseDefaultEndpointVolume();
    g_endpointName.clear();
    g_audioAvailable = false;
}

bool ShouldIncludeSession(AudioSessionState state, const std::wstring& id,
                          bool isDefault) {
    if (state == AudioSessionStateExpired) {
        return false;
    }
    if (!id.empty()) {
        g_liveSessions.insert(id);
        if (state == AudioSessionStateActive) {
            g_activatedSessions.insert(id);
        }
    }
    // Include the full default-output list. Only additional outputs need
    // playback history, preventing their unused idle sessions from flooding it.
    // Playback state also works for muted apps, allowing users to unmute them.
    // If the ID query failed, show active sessions without remembering them.
    return isDefault || g_settings.showAllSessions ||
           state == AudioSessionStateActive ||
           (!id.empty() && g_activatedSessions.count(id) != 0);
}

void PrunePlaybackHistory() {
    for (auto it = g_activatedSessions.begin(); it != g_activatedSessions.end();) {
        if (!g_liveSessions.count(*it)) {
            it = g_activatedSessions.erase(it);
        } else {
            ++it;
        }
    }
    g_liveSessions.clear();
}

std::wstring SourcePinKey(std::wstring path, const std::wstring& name,
                          const std::wstring& endpoint, bool systemSounds) {
    for (auto& c : path) c = static_cast<wchar_t>(std::towlower(c));
    std::wstring identity = systemSounds ? L"system" :
        (path.empty() ? L"name:" + name : L"exe:" + path);
    identity += L"|output:" + endpoint;
    unsigned long long hash = 14695981039346656037ULL;
    for (wchar_t c : identity) {
        hash ^= static_cast<unsigned short>(c);
        hash *= 1099511628211ULL;
    }
    return L"sourcePin_v1_" + std::to_wstring(hash);
}

bool AppSortOrder(const AppSession& left, const AppSession& right) {
    if (left.pinned != right.pinned) return left.pinned > right.pinned;
    if (!left.pinned && left.active != right.active) return left.active > right.active;
    int nameOrder = _wcsicmp(left.name.c_str(), right.name.c_str());
    return nameOrder ? nameOrder < 0 : left.pinKey < right.pinKey;
}

void ToggleSourcePin(HWND window, int row) {
    CancelNameTooltip(window);
    if (row < 0 || row >= static_cast<int>(g_apps->size())) return;
    std::wstring key = (*g_apps)[row].pinKey;
    bool pinned = !(*g_apps)[row].pinned;
    BOOL saved = pinned ? Wh_SetIntValue(key.c_str(), 1)
                        : Wh_DeleteValue(key.c_str());
    if (!pinned) {
        // A merged row can still be pinned through its older per-output keys.
        for (const auto& legacy : (*g_apps)[row].legacyPinKeys) {
            saved |= Wh_DeleteValue(legacy.c_str());
        }
        (*g_apps)[row].legacyPinKeys.clear();
    }
    if (!saved) {
        Wh_Log(L"Mixer: could not save source pin");
        return;
    }
    for (auto& app : *g_apps) if (app.pinKey == key) app.pinned = pinned;
    std::stable_sort(g_apps->begin(), g_apps->end(), AppSortOrder);
    g_scrollRow = 0;
    SelectRow(DRAG_MASTER);
    g_hoverRow = DRAG_NONE;
    InvalidateRect(window, nullptr, FALSE);
}

bool MatchesAppName(const std::wstring& target, const std::wstring& path,
                    const std::wstring& name) {
    if (target.empty()) return false;
    if (_wcsicmp(target.c_str(), name.c_str()) == 0) return true;
    if (path.empty()) return false;
    size_t slash = path.find_last_of(L"\\/");
    const wchar_t* filename = path.c_str() + (slash == std::wstring::npos ? 0 : slash + 1);
    return _wcsicmp(target.c_str(), path.c_str()) == 0 ||
           _wcsicmp(target.c_str(), filename) == 0 ||
           _wcsicmp(target.c_str(), BaseNameWithoutExtension(path).c_str()) == 0;
}

const CustomApp* FindCustomAppInRules(const std::vector<CustomApp>& rules,
                                           const std::wstring& path,
                                           const std::wstring& name) {
    for (const auto& rule : rules) {
        if (MatchesAppName(rule.app, path, name)) return &rule;
    }
    return nullptr;
}

const CustomApp* FindCustomApp(const std::wstring& path, const std::wstring& name) {
    return FindCustomAppInRules(g_settings.customApps, path, name);
}

bool IsAppHidden(const std::wstring& path, const std::wstring& name) {
    return std::any_of(g_settings.hiddenApps.begin(), g_settings.hiddenApps.end(),
        [&](const std::wstring& target) { return MatchesAppName(target, path, name); });
}

void ForgetAppliedDefaultVolume(const std::wstring& instanceId) {
    if (instanceId.empty()) return;
    AcquireSRWLockExclusive(&g_defaultVolumeLock);
    g_appliedDefaultVolumes.erase(instanceId);
    ReleaseSRWLockExclusive(&g_defaultVolumeLock);
}

bool ApplyTrackedDefaultVolume(const std::wstring& instanceId, int percent,
                               ISimpleAudioVolume* volume) {
    if (instanceId.empty() || !volume || percent < 0 || percent > 100) return false;

    AcquireSRWLockShared(&g_defaultVolumeLock);
    auto previous = g_appliedDefaultVolumes.find(instanceId);
    bool alreadyApplied = previous != g_appliedDefaultVolumes.end() &&
                          previous->second == percent;
    ReleaseSRWLockShared(&g_defaultVolumeLock);
    if (alreadyApplied) return false;

    HRESULT result = volume->SetMasterVolume(percent / 100.0f, nullptr);
    if (FAILED(result)) return false;

    AcquireSRWLockExclusive(&g_defaultVolumeLock);
    g_appliedDefaultVolumes[instanceId] = percent;
    ReleaseSRWLockExclusive(&g_defaultVolumeLock);
    return true;
}

void QueueAudioUiRefresh(HWND window) {
    bool expected = false;
    if (!g_audioUiRefreshPosted.compare_exchange_strong(expected, true)) return;
    if (!window || !PostMessageW(window, WM_APP_AUDIO_CHANGED, 0, 0)) {
        g_audioUiRefreshPosted.store(false);
    }
}

void ApplyDefaultRuleToSession(IAudioSessionControl* control,
                               const std::vector<CustomApp>& rules) {
    if (!control) return;

    IAudioSessionControl2* control2 = nullptr;
    ISimpleAudioVolume* volume = nullptr;
    if (FAILED(control->QueryInterface(__uuidof(IAudioSessionControl2),
                                       reinterpret_cast<void**>(&control2))) ||
        FAILED(control->QueryInterface(__uuidof(ISimpleAudioVolume),
                                       reinterpret_cast<void**>(&volume))) ||
        !control2 || !volume) {
        if (control2) control2->Release();
        if (volume) volume->Release();
        return;
    }

    LPWSTR rawInstanceId = nullptr;
    std::wstring instanceId;
    if (SUCCEEDED(control2->GetSessionInstanceIdentifier(&rawInstanceId)) &&
        rawInstanceId) {
        instanceId = rawInstanceId;
    }
    if (rawInstanceId) CoTaskMemFree(rawInstanceId);

    DWORD processId = 0;
    HRESULT processResult = control2->GetProcessId(&processId);
    bool systemSounds = control2->IsSystemSoundsSession() == S_OK;
    std::wstring processPath = SUCCEEDED(processResult)
        ? GetProcessPath(processId) : std::wstring();
    if (!IsAudioEngineSession(processPath, systemSounds)) {
        std::wstring displayName = GetSessionDisplayName(
            control, processId, systemSounds, &processPath);
        const CustomApp* rule = FindCustomAppInRules(rules, processPath, displayName);
        if (rule && rule->defaultVolume >= 0) {
            ApplyTrackedDefaultVolume(instanceId, rule->defaultVolume, volume);
        } else {
            ForgetAppliedDefaultVolume(instanceId);
        }
    }

    volume->Release();
    control2->Release();
}

struct AudioNotificationContext {
    HWND window;
    HANDLE stopEvent;
    HANDLE rebuildEvent;
    HANDLE sessionEvent;
    SRWLOCK pendingLock = SRWLOCK_INIT;
    std::vector<IAudioSessionControl*> pendingSessions;
    std::vector<CustomApp> rules;
};

class AudioSessionNotification final : public IAudioSessionNotification {
public:
    explicit AudioSessionNotification(AudioNotificationContext* context)
        : m_context(context) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (iid == __uuidof(IUnknown) || iid == __uuidof(IAudioSessionNotification)) {
            *object = static_cast<IAudioSessionNotification*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return static_cast<ULONG>(InterlockedIncrement(&m_references));
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG remaining = static_cast<ULONG>(InterlockedDecrement(&m_references));
        if (!remaining) delete this;
        return remaining;
    }

    HRESULT STDMETHODCALLTYPE OnSessionCreated(IAudioSessionControl* newSession) override {
        if (!newSession || !m_context) return S_OK;

        newSession->AddRef();
        AcquireSRWLockExclusive(&m_context->pendingLock);
        m_context->pendingSessions.push_back(newSession);
        ReleaseSRWLockExclusive(&m_context->pendingLock);
        SetEvent(m_context->sessionEvent);
        return S_OK;
    }

private:
    ~AudioSessionNotification() = default;
    LONG m_references = 1;
    AudioNotificationContext* m_context;
};

// Generic IMMNotificationClient with two optional sinks: an event for the
// background audio-notification thread, and a window for the UI thread.
// Construct with either; the other sink stays unused.
class EndpointNotification final : public IMMNotificationClient {
public:
    explicit EndpointNotification(HANDLE rebuildEvent)
        : m_rebuildEvent(rebuildEvent) {}

    void SetWindow(HWND window) { m_window.store(window); }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override {
        if (!object) return E_POINTER;
        *object = nullptr;
        if (iid == __uuidof(IUnknown) || iid == __uuidof(IMMNotificationClient)) {
            *object = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return static_cast<ULONG>(InterlockedIncrement(&m_references));
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG remaining = static_cast<ULONG>(InterlockedDecrement(&m_references));
        if (!remaining) delete this;
        return remaining;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override {
        SignalRebuild();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override {
        SignalRebuild();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override {
        SignalRebuild();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole,
                                                     LPCWSTR) override {
        if (flow == eRender || flow == eAll) SignalRebuild();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override {
        return S_OK;
    }

private:
    ~EndpointNotification() = default;

    void SignalRebuild() {
        if (m_rebuildEvent) SetEvent(m_rebuildEvent);
        HWND window = m_window.load();
        if (window) PostMessageW(window, WM_APP_DEFAULT_OUTPUT_CHANGED, 0, 0);
    }

    LONG m_references = 1;
    HANDLE m_rebuildEvent = nullptr;
    std::atomic<HWND> m_window{nullptr};
};

EndpointNotification* g_uiEndpointNotification = nullptr;

struct AudioSessionWatcher {
    IAudioSessionManager2* manager = nullptr;
    AudioSessionNotification* notification = nullptr;

    ~AudioSessionWatcher() {
        if (manager && notification) {
            manager->UnregisterSessionNotification(notification);
        }
        if (notification) notification->Release();
        if (manager) manager->Release();
    }
};

void PrimeSessionManager(IAudioSessionManager2* manager,
                         const std::vector<CustomApp>& rules) {
    IAudioSessionEnumerator* sessions = nullptr;
    if (FAILED(manager->GetSessionEnumerator(&sessions)) || !sessions) return;

    int count = 0;
    if (SUCCEEDED(sessions->GetCount(&count))) {
        for (int i = 0; i < count; ++i) {
            IAudioSessionControl* control = nullptr;
            if (SUCCEEDED(sessions->GetSession(i, &control)) && control) {
                ApplyDefaultRuleToSession(control, rules);
                control->Release();
            }
        }
    }
    sessions->Release();
}

void RebuildAudioSessionWatchers(
        IMMDeviceEnumerator* enumerator, AudioNotificationContext* context,
        std::vector<std::unique_ptr<AudioSessionWatcher>>& watchers) {
    watchers.clear();

    IMMDeviceCollection* devices = nullptr;
    HRESULT hr = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices);
    if (FAILED(hr) || !devices) {
        Wh_Log(L"Mixer: notification endpoint enumeration failed: 0x%08lX",
               static_cast<unsigned long>(hr));
        return;
    }

    UINT count = 0;
    if (FAILED(devices->GetCount(&count))) count = 0;
    for (UINT i = 0; i < count; ++i) {
        IMMDevice* device = nullptr;
        if (FAILED(devices->Item(i, &device)) || !device) continue;

        IAudioSessionManager2* manager = nullptr;
        hr = device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER,
                              nullptr, reinterpret_cast<void**>(&manager));
        device->Release();
        if (FAILED(hr) || !manager) continue;

        auto* notification = new AudioSessionNotification(context);
        hr = manager->RegisterSessionNotification(notification);
        if (FAILED(hr)) {
            notification->Release();
            manager->Release();
            continue;
        }

        auto watcher = std::make_unique<AudioSessionWatcher>();
        watcher->manager = manager;
        watcher->notification = notification;
        PrimeSessionManager(manager, context->rules);
        watchers.push_back(std::move(watcher));
    }
    devices->Release();
}

void DrainPendingAudioSessions(AudioNotificationContext* context, bool applyRules) {
    std::vector<IAudioSessionControl*> pending;
    AcquireSRWLockExclusive(&context->pendingLock);
    pending.swap(context->pendingSessions);
    ReleaseSRWLockExclusive(&context->pendingLock);

    for (IAudioSessionControl* control : pending) {
        if (applyRules) {
            ApplyDefaultRuleToSession(control, context->rules);
        }
        control->Release();
    }

    if (applyRules && !pending.empty()) {
        QueueAudioUiRefresh(context->window);
    }
}

DWORD WINAPI AudioNotificationThreadProc(LPVOID parameter) {
    std::unique_ptr<AudioNotificationContext> context(
        static_cast<AudioNotificationContext*>(parameter));
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(comResult)) {
        Wh_Log(L"Mixer: audio notification COM initialization failed: 0x%08lX",
               static_cast<unsigned long>(comResult));
        return 1;
    }

    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  __uuidof(IMMDeviceEnumerator),
                                  reinterpret_cast<void**>(&enumerator));
    if (FAILED(hr) || !enumerator) {
        Wh_Log(L"Mixer: audio notification device enumerator failed: 0x%08lX",
               static_cast<unsigned long>(hr));
        CoUninitialize();
        return 1;
    }

    auto* endpointNotification = new EndpointNotification(context->rebuildEvent);
    bool endpointRegistered = SUCCEEDED(
        enumerator->RegisterEndpointNotificationCallback(endpointNotification));
    if (!endpointRegistered) {
        Wh_Log(L"Mixer: endpoint notification registration failed");
    }

    std::vector<std::unique_ptr<AudioSessionWatcher>> watchers;
    RebuildAudioSessionWatchers(enumerator, context.get(), watchers);

    HANDLE events[] = {
        context->stopEvent, context->rebuildEvent, context->sessionEvent};
    while (true) {
        DWORD wait = WaitForMultipleObjects(3, events, FALSE, INFINITE);
        if (wait == WAIT_OBJECT_0) break;
        if (wait == WAIT_OBJECT_0 + 1) {
            RebuildAudioSessionWatchers(enumerator, context.get(), watchers);
            QueueAudioUiRefresh(context->window);
            continue;
        }
        if (wait == WAIT_OBJECT_0 + 2) {
            DrainPendingAudioSessions(context.get(), true);
            continue;
        }
        break;
    }

    watchers.clear();
    DrainPendingAudioSessions(context.get(), false);
    if (endpointRegistered) {
        enumerator->UnregisterEndpointNotificationCallback(endpointNotification);
    }
    endpointNotification->Release();
    enumerator->Release();
    CoUninitialize();
    return 0;
}

bool StopDefaultVolumeNotifications() {
    if (!g_audioNotificationThread) return true;

    if (g_audioNotificationStopEvent) SetEvent(g_audioNotificationStopEvent);
    DWORD wait = WaitForSingleObject(g_audioNotificationThread, 5000);
    if (wait != WAIT_OBJECT_0) {
        Wh_Log(L"Mixer: audio notification thread did not stop in time");
        return false;
    }

    CloseHandle(g_audioNotificationThread);
    g_audioNotificationThread = nullptr;
    if (g_audioNotificationStopEvent) {
        CloseHandle(g_audioNotificationStopEvent);
        g_audioNotificationStopEvent = nullptr;
    }
    if (g_audioNotificationRebuildEvent) {
        CloseHandle(g_audioNotificationRebuildEvent);
        g_audioNotificationRebuildEvent = nullptr;
    }
    if (g_audioNotificationSessionEvent) {
        CloseHandle(g_audioNotificationSessionEvent);
        g_audioNotificationSessionEvent = nullptr;
    }
    g_audioUiRefreshPosted.store(false);
    return true;
}

void StartDefaultVolumeNotifications(HWND window) {
    if (!HasDefaultVolumeRules() || g_audioNotificationThread) return;

    g_audioNotificationStopEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_audioNotificationRebuildEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_audioNotificationSessionEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_audioNotificationStopEvent || !g_audioNotificationRebuildEvent ||
        !g_audioNotificationSessionEvent) {
        Wh_Log(L"Mixer: could not create audio notification events");
        if (g_audioNotificationStopEvent) CloseHandle(g_audioNotificationStopEvent);
        if (g_audioNotificationRebuildEvent) CloseHandle(g_audioNotificationRebuildEvent);
        if (g_audioNotificationSessionEvent) CloseHandle(g_audioNotificationSessionEvent);
        g_audioNotificationStopEvent = nullptr;
        g_audioNotificationRebuildEvent = nullptr;
        g_audioNotificationSessionEvent = nullptr;
        return;
    }

    auto* context = new AudioNotificationContext;
    context->window = window;
    context->stopEvent = g_audioNotificationStopEvent;
    context->rebuildEvent = g_audioNotificationRebuildEvent;
    context->sessionEvent = g_audioNotificationSessionEvent;
    context->rules = g_settings.customApps;
    g_audioNotificationThread = CreateThread(
        nullptr, 0, AudioNotificationThreadProc, context, 0, nullptr);
    if (!g_audioNotificationThread) {
        Wh_Log(L"Mixer: could not start audio notification thread, error %lu",
               GetLastError());
        delete context;
        CloseHandle(g_audioNotificationStopEvent);
        CloseHandle(g_audioNotificationRebuildEvent);
        CloseHandle(g_audioNotificationSessionEvent);
        g_audioNotificationStopEvent = nullptr;
        g_audioNotificationRebuildEvent = nullptr;
        g_audioNotificationSessionEvent = nullptr;
    }
}

void ApplyDefaultVolume(const CustomApp* rule, const std::wstring& instanceId,
                        ISimpleAudioVolume* volume) {
    if (instanceId.empty()) return;
    if (!rule || rule->defaultVolume < 0) {
        ForgetAppliedDefaultVolume(instanceId);
        return;
    }
    ApplyTrackedDefaultVolume(instanceId, rule->defaultVolume, volume);
}

void PruneDefaultVolumes(bool completeScan) {
    // An unavailable device/service is not evidence that a session ended.
    if (!completeScan) return;
    AcquireSRWLockExclusive(&g_defaultVolumeLock);
    for (auto it = g_appliedDefaultVolumes.begin(); it != g_appliedDefaultVolumes.end();) {
        if (!g_liveDefaultSessions.count(it->first)) it = g_appliedDefaultVolumes.erase(it);
        else ++it;
    }
    ReleaseSRWLockExclusive(&g_defaultVolumeLock);
}

bool AppendDeviceSessions(IMMDevice* device, bool isDefault, bool diagnostics,
                          bool buildRows,
                          std::unordered_map<std::wstring, size_t>& groups) {
    std::wstring endpointName = ReadEndpointName(device);
    std::wstring endpointId = endpointName;
    LPWSTR rawEndpointId = nullptr;
    if (SUCCEEDED(device->GetId(&rawEndpointId)) && rawEndpointId) endpointId = rawEndpointId;
    if (rawEndpointId) CoTaskMemFree(rawEndpointId);
    IAudioSessionManager2* manager = nullptr;
    HRESULT hr = device->Activate(__uuidof(IAudioSessionManager2),
                                  CLSCTX_INPROC_SERVER, nullptr,
                                  reinterpret_cast<void**>(&manager));
    if (FAILED(hr) || !manager) {
        if (diagnostics) {
            Wh_Log(L"Mixer: session manager failed for '%s': 0x%08lX",
                   endpointName.c_str(), static_cast<unsigned long>(hr));
        }
        return false;
    }

    IAudioSessionEnumerator* sessions = nullptr;
    hr = manager->GetSessionEnumerator(&sessions);
    manager->Release();
    if (FAILED(hr) || !sessions) {
        if (diagnostics) {
            Wh_Log(L"Mixer: session enumeration failed for '%s': 0x%08lX",
                   endpointName.c_str(), static_cast<unsigned long>(hr));
        }
        return false;
    }

    int count = 0;
    hr = sessions->GetCount(&count);
    bool complete = SUCCEEDED(hr);
    if (diagnostics) {
        Wh_Log(L"Mixer: output '%s', default=%d, sessions=%d, result=0x%08lX",
               endpointName.c_str(), isDefault, count,
               static_cast<unsigned long>(hr));
    }
    for (int i = 0; i < count; i++) {
        IAudioSessionControl* control = nullptr;
        if (FAILED(sessions->GetSession(i, &control)) || !control) {
            complete = false;
            continue;
        }

        AudioSessionState state = AudioSessionStateExpired;
        if (FAILED(control->GetState(&state))) complete = false;
        if (state == AudioSessionStateExpired) {
            control->Release();
            continue;
        }

        IAudioSessionControl2* control2 = nullptr;
        ISimpleAudioVolume* volume = nullptr;
        if (FAILED(control->QueryInterface(__uuidof(IAudioSessionControl2),
                                           reinterpret_cast<void**>(&control2))) ||
            FAILED(control->QueryInterface(__uuidof(ISimpleAudioVolume),
                                           reinterpret_cast<void**>(&volume))) ||
            !control2 || !volume) {
            complete = false;
            if (control2) {
                control2->Release();
            }
            if (volume) {
                volume->Release();
            }
            control->Release();
            continue;
        }

        DWORD processId = 0;
        HRESULT processResult = control2->GetProcessId(&processId);
        bool systemSounds = control2->IsSystemSoundsSession() == S_OK;
        std::wstring processPath = SUCCEEDED(processResult)
            ? GetProcessPath(processId) : std::wstring();
        if (IsAudioEngineSession(processPath, systemSounds)) {
            if (diagnostics) {
                Wh_Log(L"Mixer: hiding audio engine session, PID=%lu, output='%s'",
                       processId, endpointName.c_str());
            }
            volume->Release();
            control2->Release();
            control->Release();
            continue;
        }

        std::wstring displayName = GetSessionDisplayName(control, processId,
                                                         systemSounds, &processPath);
        // A name the session supplied beats the executable-name fallback.
        int namePriority =
            displayName != BaseNameWithoutExtension(processPath) ? 1 : 0;
        // Like the Windows volume mixer, show one row per app: sessions of
        // the same executable are merged, including across outputs, where
        // audio routers leave idle duplicates behind. System sounds, sessions
        // without a readable path, and everything when showAllSessions is on,
        // stay scoped to their own output.
        bool scoped = g_settings.showAllSessions || systemSounds ||
                      processPath.empty();
        std::wstring outputPinKey =
            SourcePinKey(processPath, displayName, endpointId, systemSounds);
        std::wstring pinKey = scoped
            ? outputPinKey : SourcePinKey(processPath, displayName, L"", false);
        bool pinned = buildRows && Wh_GetIntValue(pinKey.c_str(), 0) != 0;
        bool legacyPinned = buildRows && !scoped &&
                            Wh_GetIntValue(outputPinKey.c_str(), 0) != 0;
        LPWSTR rawInstanceId = nullptr;
        std::wstring instanceId;
        if (SUCCEEDED(control2->GetSessionInstanceIdentifier(&rawInstanceId)) &&
            rawInstanceId) {
            instanceId = rawInstanceId;
        }
        if (rawInstanceId) {
            CoTaskMemFree(rawInstanceId);
        }
        if (instanceId.empty()) complete = false;
        else g_liveDefaultSessions.insert(instanceId);
        bool hidden = IsAppHidden(processPath, displayName) ||
                      g_temporarilyHiddenSources.count(pinKey) != 0;
        const CustomApp* custom = FindCustomApp(processPath, displayName);
        ApplyDefaultVolume(custom, instanceId, volume);
        // Build identity before applying the alias, so renaming keeps pins and
        // keyboard selection attached to the original source.
        if (custom && !custom->displayName.empty()) {
            displayName = custom->displayName;
            namePriority = 2;
        }
        bool include = ShouldIncludeSession(state, instanceId, isDefault);
        if (!buildRows) {
            volume->Release();
            control2->Release();
            control->Release();
            continue;
        }

        // The pin key already identifies the executable, per output when the
        // row is scoped. Sessions without a readable path are only grouped by
        // process, and are never merged into System sounds.
        std::wstring groupKey = pinKey;
        if (!systemSounds && processPath.empty()) {
            groupKey = (SUCCEEDED(processResult) && processId
                ? L"pid:" + std::to_wstring(processId)
                : L"session:" + std::to_wstring(i)) + L"|" + endpointId;
        }

        // Every session joins its row, so the slider also reaches sessions
        // that would not be listed on their own. Rows nothing qualifies are
        // dropped once all outputs are scanned.
        auto existing = groups.find(groupKey);
        if (existing == groups.end()) {
            AppSession created;
            created.processId = processId;
            created.executablePath = processPath;
            created.pinKey = pinKey;
            if (diagnostics) {
                Wh_Log(L"Mixer: app '%s', PID=%lu, active=%d, output='%s'",
                       displayName.c_str(), processId,
                       state == AudioSessionStateActive, endpointName.c_str());
            }
            existing = groups.emplace(groupKey, g_apps->size()).first;
            g_apps->push_back(std::move(created));
        }
        AppSession& app = (*g_apps)[existing->second];
        app.listed |= include || pinned || legacyPinned;
        app.hidden |= hidden;
        app.pinned |= pinned || legacyPinned;
        if (legacyPinned) app.legacyPinKeys.push_back(std::move(outputPinKey));
        app.active |= state == AudioSessionStateActive;
        if (namePriority > app.namePriority) {
            app.namePriority = namePriority;
            app.name = displayName;
            if (scoped && !isDefault) {
                app.name += L" [" + endpointName + L"]";
            }
        }
        // Show the level of the session that is most likely audible.
        int levelPriority = state == AudioSessionStateActive ? 2
                            : isDefault ? 1 : 0;
        if (levelPriority > app.levelPriority) {
            app.levelPriority = levelPriority;
            volume->GetMasterVolume(&app.volume);
            BOOL muted = FALSE;
            volume->GetMute(&muted);
            app.muted = muted != FALSE;
        }
        app.volumeControls.push_back(volume);

        IAudioMeterInformation* meter = nullptr;
        if (SUCCEEDED(control->QueryInterface(__uuidof(IAudioMeterInformation),
                                              reinterpret_cast<void**>(&meter))) &&
            meter) {
            app.meters.push_back(meter);
        }

        control2->Release();
        control->Release();
    }
    sessions->Release();
    return complete;
}

void ClampPageOffset();

void RefreshMasterIcons(HWND hWnd, bool force = false);

void RefreshAudioSessions(HWND hWnd, bool diagnostics = false) {
    if (g_dragRow != DRAG_NONE || g_volumeEntry || g_sourceMenuOpen) {
        return;
    }

    bool buildRows = IsWindowVisible(hWnd) || diagnostics;
    g_liveSessions.clear();
    g_liveDefaultSessions.clear();
    ReleaseAudioData();
    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  __uuidof(IMMDeviceEnumerator),
                                  reinterpret_cast<void**>(&enumerator));
    if (FAILED(hr) || !enumerator) {
        if (diagnostics) {
            Wh_Log(L"Mixer: device enumeration failed: 0x%08lX",
                   static_cast<unsigned long>(hr));
        }
        RestoreSelectedRow();
        ClampPageOffset();
        InvalidateRect(hWnd, nullptr, FALSE);
        return;
    }

    std::wstring defaultId;
    IMMDevice* defaultDevice = nullptr;
    if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(
            eRender, eMultimedia, &defaultDevice)) && defaultDevice) {
        LPWSTR id = nullptr;
        if (SUCCEEDED(defaultDevice->GetId(&id)) && id) {
            defaultId = id;
            CoTaskMemFree(id);
        }
        // The helper releases any previous binding (via ReleaseAudioData
        // above) and re-registers the endpoint callback against this window.
        BindDefaultEndpointVolumeFromDevice(defaultDevice, hWnd);
    }

    bool completeScan = true;
    std::unordered_map<std::wstring, size_t> groups;
    IMMDeviceCollection* devices = nullptr;
    hr = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices);
    if (SUCCEEDED(hr) && devices) {
        UINT count = 0;
        if (FAILED(devices->GetCount(&count))) completeScan = false;
        if (diagnostics) {
            Wh_Log(L"Mixer: scanning %u active playback outputs", count);
        }
        for (UINT i = 0; i < count; ++i) {
            IMMDevice* device = nullptr;
            if (FAILED(devices->Item(i, &device)) || !device) {
                completeScan = false;
                continue;
            }
            LPWSTR id = nullptr;
            bool isDefault = false;
            if (SUCCEEDED(device->GetId(&id)) && id) {
                isDefault = !defaultId.empty() && defaultId == id;
                CoTaskMemFree(id);
            }
            if (!AppendDeviceSessions(device, isDefault, diagnostics, buildRows,
                                      groups)) {
                completeScan = false;
            }
            device->Release();
        }
        devices->Release();
    } else {
        completeScan = false;
        if (diagnostics) {
            Wh_Log(L"Mixer: playback collection failed: 0x%08lX",
                   static_cast<unsigned long>(hr));
        }
        // Preserve default-device functionality if collection enumeration fails.
        if (defaultDevice) {
            AppendDeviceSessions(defaultDevice, true, diagnostics, buildRows,
                                 groups);
        }
    }
    if (defaultDevice) {
        defaultDevice->Release();
    }
    enumerator->Release();

    PruneDefaultVolumes(completeScan);
    g_liveDefaultSessions.clear();
    PrunePlaybackHistory();

    std::erase_if(*g_apps, [](const AppSession& app) {
        return !app.listed || app.hidden;
    });
    for (auto& app : *g_apps) {
        app.icon = GetExecutableIcon(app.executablePath);
    }
    std::stable_sort(g_apps->begin(), g_apps->end(), AppSortOrder);

    RestoreSelectedRow();
    g_audioAvailable = g_endpointVolume != nullptr || !g_apps->empty();
    ClampPageOffset();
    RefreshMasterIcons(hWnd);
    InvalidateRect(hWnd, nullptr, FALSE);
}

bool EnsureDefaultEndpointVolume() {
    if (g_endpointVolume) {
        return true;
    }
    // Fallback path: no live binding yet. Bind through the helper so the
    // callback is registered against the current window.
    return BindDefaultEndpointVolume(g_hWnd.load());
}

float GetMasterVolume() {
    float value = 0.0f;
    if (EnsureDefaultEndpointVolume()) {
        g_endpointVolume->GetMasterVolumeLevelScalar(&value);
    }
    return value;
}

bool GetMasterMuted() {
    BOOL muted = FALSE;
    if (EnsureDefaultEndpointVolume()) {
        g_endpointVolume->GetMute(&muted);
    }
    return muted != FALSE;
}

float GetAppPeak(const AppSession& app) {
    float peak = 0.0f;
    for (auto* meter : app.meters) {
        float value = 0.0f;
        if (SUCCEEDED(meter->GetPeakValue(&value))) {
            peak = std::max(peak, value);
        }
    }
    return peak;
}

void SetRowVolume(int row, float value) {
    value = ClampVolume(value);
    if (row == DRAG_MASTER) {
        if (g_endpointVolume) {
            g_endpointVolume->SetMasterVolumeLevelScalar(value, nullptr);
            if (value > 0.0f) {
                g_endpointVolume->SetMute(FALSE, nullptr);
            }
            RefreshMasterIcons(g_hWnd.load(), false);
        }
        return;
    }

    if (row < 0 || row >= static_cast<int>(g_apps->size())) {
        return;
    }
    AppSession& app = (*g_apps)[row];
    for (auto* control : app.volumeControls) {
        control->SetMasterVolume(value, nullptr);
        if (value > 0.0f) {
            control->SetMute(FALSE, nullptr);
        }
    }
    app.volume = value;
    if (value > 0.0f) {
        app.muted = false;
    }
}

void ToggleRowMute(int row) {
    if (row == DRAG_MASTER) {
        if (EnsureDefaultEndpointVolume()) {
            bool muted = GetMasterMuted();
            g_endpointVolume->SetMute(!muted, nullptr);
            RefreshMasterIcons(g_hWnd.load(), false);
            InvalidateRect(g_hWnd.load(), nullptr, FALSE);
        }
        return;
    }

    if (row < 0 || row >= static_cast<int>(g_apps->size())) {
        return;
    }
    AppSession& app = (*g_apps)[row];
    bool newMuted = !app.muted;
    for (auto* control : app.volumeControls) {
        control->SetMute(newMuted, nullptr);
    }
    app.muted = newMuted;
}

bool IsCompact() {
    return g_settings.compactMode;
}

int HeaderHeight() {
    return Scale(IsCompact() ? 44 : 82);
}

int RowHeight() {
    // Compact Windows 10 rows are taller to fit the bar-shaped thumb.
    if (IsCompact()) return Scale(IsWindows10Style() ? 56 : 48);
    return Scale(62);
}

int SectionGap() {
    return Scale(IsCompact() ? 20 : 26);
}

int FooterHeight(bool paged) {
    if (IsCompact()) {
        return Scale(paged ? 42 : 6);
    }
    return Scale(paged ? 48 : 10);
}

int RowTop(int visibleRow) {
    return HeaderHeight() + visibleRow * RowHeight() +
           (visibleRow > 0 ? SectionGap() : 0);
}

int PopupWidth() {
    return Scale(IsCompact() ? 360 : 400);
}

RECT TitleRect(int width) {
    return {Scale(24), Scale(11), width - Scale(60), Scale(39)};
}

RECT CloseButtonRect(int width) {
    if (IsCompact()) {
        return {width - Scale(44), Scale(8), width - Scale(16), Scale(36)};
    }
    return {width - Scale(52), Scale(9), width - Scale(20), Scale(41)};
}

RECT OutputPickerRect(int width) {
    if (IsCompact()) {
        return {Scale(12), Scale(8), width - Scale(52), Scale(36)};
    }
    return {Scale(12), Scale(44), width - Scale(12), Scale(74)};
}

RECT DeviceNameRect(int width) {
    RECT picker = OutputPickerRect(width);
    return {picker.left + Scale(38), picker.top,
            picker.right - Scale(34), picker.bottom};
}

int PageSize() {
    return std::min(g_settings.maxVisibleApps, g_availableAppRows);
}

int AppRowsForHeight(int availableHeight) {
    int fixedHeight = HeaderHeight() + RowHeight() + SectionGap() +
                      FooterHeight(true);
    return ClampInt((availableHeight - fixedHeight) / RowHeight(), 1, 15);
}

int PageCount() {
    return std::max(1, (static_cast<int>(g_apps->size()) +
                       PageSize() - 1) / PageSize());
}

void ClampPageOffset() {
    int page = ClampInt(g_scrollRow / PageSize(), 0, PageCount() - 1);
    g_scrollRow = page * PageSize();
    if (g_settings.keyboardControls && g_selectedRow >= 0) {
        g_scrollRow = (g_selectedRow / PageSize()) * PageSize();
    }
}

void ChangePage(HWND window, int delta) {
    CancelNameTooltip(window);
    if (g_dragRow != DRAG_NONE || g_volumeEntry || PageCount() <= 1) return;
    int pages = PageCount();
    int page = (g_scrollRow / PageSize() + delta % pages + pages) % pages;
    g_scrollRow = page * PageSize();
    SelectRow(g_scrollRow);
    g_hoverRow = DRAG_NONE;
    InvalidateRect(window, nullptr, FALSE);
}

int VisibleAppCount() {
    return std::min(std::max(0, static_cast<int>(g_apps->size()) - g_scrollRow),
                    PageSize());
}

int PopupHeight() {
    int slots = std::max(1, std::min(static_cast<int>(g_apps->size()),
                                   PageSize()));
    return HeaderHeight() + RowHeight() * (1 + slots) + SectionGap() +
           FooterHeight(PageCount() > 1);
}

RECT PageButtonRect(bool next, const RECT& client) {
    int left = next ? client.right - Scale(120) : Scale(20);
    if (IsCompact()) {
        return {left, client.bottom - Scale(40), left + Scale(100),
                client.bottom - Scale(12)};
    }
    return {left, client.bottom - Scale(44), left + Scale(100),
            client.bottom - Scale(12)};
}

RECT IconTileRectForRow(int visibleRow) {
    int top = RowTop(visibleRow);
    if (IsCompact()) {
        int offset = Scale(IsWindows10Style() ? 12 : 8);
        return {Scale(22), top + offset, Scale(52), top + offset + Scale(30)};
    }
    return {Scale(22), top + Scale(12), Scale(58), top + Scale(48)};
}

RECT NameRectForRow(int visibleRow, int clientWidth) {
    int top = RowTop(visibleRow);
    if (IsCompact()) {
        return {Scale(62), top + Scale(2), clientWidth - Scale(132),
                top + Scale(26)};
    }
    return {Scale(68), top + Scale(7), clientWidth - Scale(132),
            top + Scale(31)};
}

RECT PercentRectForRow(int visibleRow, int clientWidth) {
    int top = RowTop(visibleRow);
    if (IsCompact()) {
        if (visibleRow == 0) {
            return {clientWidth - Scale(100), top + Scale(2),
                    clientWidth - Scale(50), top + Scale(26)};
        }
        return {clientWidth - Scale(124), top + Scale(2),
                clientWidth - Scale(76), top + Scale(26)};
    }
    return {clientWidth - Scale(124), top + Scale(7),
            clientWidth - Scale(78), top + Scale(31)};
}

RECT SliderRectForRow(int visibleRow, int clientWidth) {
    int top = RowTop(visibleRow);
    if (IsCompact()) {
        int offset = Scale(IsWindows10Style() ? 36 : 30);
        return {Scale(62), top + offset, clientWidth - Scale(28),
                top + offset + Scale(8)};
    }
    return {Scale(68), top + Scale(38), clientWidth - Scale(78),
            top + Scale(46)};
}

RECT MuteRectForRow(int visibleRow, int clientWidth) {
    int top = RowTop(visibleRow);
    if (IsCompact()) {
        return {clientWidth - Scale(46), top + Scale(2),
                clientWidth - Scale(22), top + Scale(26)};
    }
    return {clientWidth - Scale(58), top + Scale(visibleRow ? 28 : 12),
            clientWidth - Scale(22), top + Scale(visibleRow ? 58 : 48)};
}

RECT PinRectForRow(int visibleRow, int clientWidth) {
    int top = RowTop(visibleRow);
    if (IsCompact()) {
        return {clientWidth - Scale(72), top + Scale(2),
                clientWidth - Scale(48), top + Scale(26)};
    }
    return {clientWidth - Scale(56), top + Scale(1),
            clientWidth - Scale(24), top + Scale(28)};
}

// Outer thumb size in pixels: the round Windows 11 thumb, border included, or
// the bar-shaped Windows 10 one.
float SliderThumbWidth() {
    return (IsWindows10Style() ? 8.0f : 20.0f) * g_dpi / 96.0f;
}

float SliderThumbHeight() {
    return (IsWindows10Style() ? 24.0f : 20.0f) * g_dpi / 96.0f;
}

// Distance from either end of the rail to the thumb's center at 0% and 100%.
// As in the native sliders, the thumb stays within the rail instead of being
// centered on its ends. The solid surface of the Windows 11 thumb still
// overlaps each end by a pixel, so no sliver of the rail shows through its
// translucent border.
float SliderThumbInset() {
    float half = SliderThumbWidth() * 0.5f;
    return IsWindows10Style() ? half : half - 2.0f * g_dpi / 96.0f;
}

// Horizontal center of the thumb for a volume in the 0 to 1 range.
float SliderThumbCenter(const RECT& slider, float volume) {
    float inset = SliderThumbInset();
    float travel = std::max(
        0.0f, static_cast<float>(slider.right - slider.left) - 2.0f * inset);
    return slider.left + inset + travel * ClampVolume(volume);
}

bool SliderThumbContains(const RECT& slider, float volume, POINT point) {
    float dx = point.x + 0.5f - SliderThumbCenter(slider, volume);
    float dy = point.y + 0.5f - (slider.top + slider.bottom) * 0.5f;
    if (IsWindows10Style()) {
        // The bar is narrow, so allow a little slack on either side.
        return std::abs(dx) <= SliderThumbWidth() * 0.5f + 2.0f * g_dpi / 96.0f &&
               std::abs(dy) <= SliderThumbHeight() * 0.5f;
    }
    float radius = SliderThumbWidth() * 0.5f;
    return dx * dx + dy * dy <= radius * radius;
}

RECT SliderHitRect(int visibleRow, int clientWidth) {
    RECT rect = SliderRectForRow(visibleRow, clientWidth);
    int vertical = IsCompact() ? 8 : 12;
    InflateRect(&rect, Scale(10), Scale(vertical));
    return rect;
}

int RowFromPoint(POINT point) {
    if (point.y < HeaderHeight()) {
        return DRAG_NONE;
    }
    int relative = point.y - HeaderHeight();
    int visibleRow = 0;
    if (relative >= RowHeight()) {
        if (relative < RowHeight() + SectionGap()) {
            return DRAG_NONE;
        }
        visibleRow = 1 + (relative - RowHeight() - SectionGap()) / RowHeight();
    }
    if (visibleRow > VisibleAppCount()) {
        return DRAG_NONE;
    }
    if (visibleRow == 0) {
        return DRAG_MASTER;
    }
    int appIndex = g_scrollRow + visibleRow - 1;
    return appIndex >= 0 && appIndex < static_cast<int>(g_apps->size())
               ? appIndex
               : DRAG_NONE;
}

int VisibleRowForDataRow(int row) {
    return row == DRAG_MASTER ? 0 : row - g_scrollRow + 1;
}

// A separate owned popup keeps the native text editor visible over the
// UpdateLayeredWindow surface. Audio enumeration is paused until it closes.
bool ParseVolumePercent(const std::wstring& text, int& value) {
    if (text.empty() || text.size() > 3) return false;
    int parsed = 0;
    for (wchar_t c : text) {
        if (c < L'0' || c > L'9') return false;
        parsed = parsed * 10 + (c - L'0');
    }
    if (parsed > 100) return false;
    value = parsed;
    return true;
}

bool FinishVolumeEntry(HWND owner, bool commit, bool returnFocus) {
    if (!g_volumeEntry) return true;
    if (commit) {
        WCHAR text[32]{};
        GetWindowTextW(g_volumeEdit, text, ARRAYSIZE(text));
        int percent;
        if (!ParseVolumePercent(text, percent)) {
            MessageBeep(MB_ICONWARNING);
            SendMessageW(g_volumeEdit, EM_SETSEL, 0, -1);
            return false;
        }
        SetRowVolume(g_volumeEntryRow, percent / 100.0f);
    }
    HWND entry = g_volumeEntry;
    g_volumeEntry = nullptr;
    g_volumeEdit = nullptr;
    g_volumeEntryRow = DRAG_NONE;
    ++g_volumeEntryGeneration;
    DestroyWindow(entry);
    if (returnFocus && IsWindowVisible(owner)) SetFocus(owner);
    InvalidateRect(owner, nullptr, FALSE);
    return true;
}

LRESULT CALLBACK VolumeEntryProc(HWND window, UINT message, WPARAM wParam,
                                 LPARAM lParam) {
    HWND owner = GetWindow(window, GW_OWNER);
    switch (message) {
        case WM_SETFOCUS:
            if (g_volumeEdit) SetFocus(g_volumeEdit);
            return 0;
        case WM_COMMAND:
            if (HIWORD(wParam) != EN_KILLFOCUS) break;
            PostMessageW(owner, WM_APP_FINISH_VOLUME_ENTRY, 0,
                         static_cast<LPARAM>(g_volumeEntryGeneration));
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                PostMessageW(owner, WM_APP_FINISH_VOLUME_ENTRY, 0,
                             static_cast<LPARAM>(g_volumeEntryGeneration));
                SetTimer(owner, TIMER_CHECK_FOCUS, 150, nullptr);
            }
            break;
        case WM_CLOSE:
            PostMessageW(owner, WM_APP_FINISH_VOLUME_ENTRY, 0,
                         static_cast<LPARAM>(g_volumeEntryGeneration));
            return 0;
        case WM_CTLCOLOREDIT: {
            HDC dc = reinterpret_cast<HDC>(wParam);
            SetTextColor(dc, g_theme.text);
            SetBkColor(dc, g_theme.panel);
            SetDCBrushColor(dc, g_theme.panel);
            return reinterpret_cast<LRESULT>(GetStockObject(DC_BRUSH));
        }
        case WM_ERASEBKGND: {
            RECT rect{};
            GetClientRect(window, &rect);
            HDC dc = reinterpret_cast<HDC>(wParam);
            SetDCBrushColor(dc, g_theme.panel);
            FillRect(dc, &rect, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
            return 1;
        }
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

void BeginVolumeEntry(HWND owner, int row) {
    if (!g_settings.exactVolumeEntry || g_dragRow != DRAG_NONE ||
        (row == DRAG_MASTER && !g_endpointVolume)) return;
    FinishVolumeEntry(owner, false, false);
    RECT client{};
    GetClientRect(owner, &client);
    RECT rect = PercentRectForRow(VisibleRowForDataRow(row), client.right);
    POINT origin{rect.left, rect.top};
    ClientToScreen(owner, &origin);
    HINSTANCE instance = GetModuleHandleW(nullptr);
    g_volumeEntryRow = row;
    ++g_volumeEntryGeneration;
    g_volumeEntry = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        VOLUME_ENTRY_CLASS, L"Volume (0-100%)", WS_POPUP | WS_BORDER,
        origin.x, origin.y, rect.right - rect.left, rect.bottom - rect.top,
        owner, nullptr, instance, nullptr);
    if (!g_volumeEntry) {
        g_volumeEntryRow = DRAG_NONE;
        return;
    }
    GetClientRect(g_volumeEntry, &client);
    g_volumeEdit = CreateWindowExW(0, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER | ES_AUTOHSCROLL,
        0, 0, client.right, client.bottom,
        g_volumeEntry, nullptr, instance, nullptr);
    if (!g_volumeEdit) {
        FinishVolumeEntry(owner, false, false);
        return;
    }
    SendMessageW(g_volumeEdit, EM_SETLIMITTEXT, 16, 0);
    SendMessageW(g_volumeEdit, WM_SETFONT, reinterpret_cast<WPARAM>(g_bodyFont), TRUE);
    float volume = row == DRAG_MASTER ? GetMasterVolume() : (*g_apps)[row].volume;
    std::wstring text = std::to_wstring(static_cast<int>(volume * 100.0f + 0.5f));
    SetWindowTextW(g_volumeEdit, text.c_str());
    SendMessageW(g_volumeEdit, EM_SETSEL, 0, -1);
    ShowWindow(g_volumeEntry, SW_SHOW);
    SetFocus(g_volumeEdit);
}

void MoveKeyboardSelection(HWND window, int direction) {
    CancelNameTooltip(window);
    int row = g_selectedRow;
    if (row == DRAG_MASTER && direction > 0 && !g_apps->empty()) {
        row = g_scrollRow;
    } else {
        row = ClampInt(row + direction, DRAG_MASTER,
                       static_cast<int>(g_apps->size()) - 1);
    }
    SelectRow(row);
    ClampPageOffset();
    InvalidateRect(window, nullptr, FALSE);
}

bool HandleRowKey(HWND window, WPARAM key, LPARAM flags) {
    if (!g_settings.keyboardControls || g_volumeEntry || g_dragRow != DRAG_NONE ||
        (GetKeyState(VK_CONTROL) & 0x8000) || (GetKeyState(VK_MENU) & 0x8000)) {
        return false;
    }
    if (key == VK_UP || key == VK_DOWN) {
        MoveKeyboardSelection(window, key == VK_DOWN ? 1 : -1);
        return true;
    }
    if (key == VK_LEFT || key == VK_RIGHT) {
        float volume = g_selectedRow == DRAG_MASTER ? GetMasterVolume()
            : (*g_apps)[g_selectedRow].volume;
        SetRowVolume(g_selectedRow, volume +
            (key == VK_RIGHT ? 1 : -1) * g_settings.volumeStep / 100.0f);
        InvalidateRect(window, nullptr, FALSE);
        return true;
    }
    if (key == 'M') {
        if (!(flags & (1LL << 30))) ToggleRowMute(g_selectedRow);
        InvalidateRect(window, nullptr, FALSE);
        return true;
    }
    return false;
}

void ApplyVolumeFromPoint(HWND hWnd, int row, int x) {
    RECT client;
    GetClientRect(hWnd, &client);
    RECT slider = SliderRectForRow(VisibleRowForDataRow(row), client.right);
    float inset = SliderThumbInset();
    float travel = static_cast<float>(slider.right - slider.left) - 2.0f * inset;
    if (travel <= 0.0f) return;
    float value = (static_cast<float>(x - slider.left) - inset) / travel;
    SetRowVolume(row, value);
    InvalidateRect(hWnd, nullptr, FALSE);
}

void FillSolidRect(HDC dc, const RECT& rect, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(dc, &rect, brush);
    DeleteObject(brush);
}

void DrawRoundedRect(HDC dc, const RECT& rect, int radius, COLORREF color) {
    if (radius <= 0) {
        // Unlike RoundRect with a null pen, this fills the last row and column.
        FillSolidRect(dc, rect, color);
        return;
    }
    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_NULL, 0, color);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

struct CachedLabel {
    std::wstring text;
    HFONT font = nullptr;
    UINT format = 0;
    COLORREF color = 0;
    int width = 0, height = 0;
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    ~CachedLabel() {
        if (dc && previous) SelectObject(dc, previous);
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
    }
};
struct LabelCache {
    std::vector<std::unique_ptr<CachedLabel>> labels;
    size_t bytes = 0;
};
struct TextCaches {
    LabelCache general;
    LabelCache percentages;
};
std::optional<TextCaches> g_textCache;
constexpr int TEXT_SUPERSAMPLE = 6;

void ClearTextCache() {
    GdiFlush();
    g_textCache.reset();
}

// Average the supersampled coverage mask, then construct premultiplied BGRA.
// The same coverage is used against both mattes, without colored ClearType fringes.
DWORD TextCoveragePixel(const BYTE* scan, int stride, int x, int y, COLORREF color) {
    unsigned sum = 0;
    for (int dy = 0; dy < TEXT_SUPERSAMPLE; ++dy) {
        const BYTE* row = scan + static_cast<std::ptrdiff_t>(
            y * TEXT_SUPERSAMPLE + dy) * stride;
        for (int dx = 0; dx < TEXT_SUPERSAMPLE; ++dx) {
            sum += row[(x * TEXT_SUPERSAMPLE + dx) * 4];
        }
    }
    constexpr unsigned samples = TEXT_SUPERSAMPLE * TEXT_SUPERSAMPLE;
    DWORD alpha = (sum + samples / 2) / samples;
    return (alpha << 24) |
        (((GetRValue(color) * alpha + 127) / 255) << 16) |
        (((GetGValue(color) * alpha + 127) / 255) << 8) |
        ((GetBValue(color) * alpha + 127) / 255);
}

bool DrawSmoothLabel(HDC dc, const std::wstring& text, const RECT& rect,
                     HFONT font, Gdiplus::Font* typography, COLORREF color, UINT format,
                     bool percentage) {
    int width = rect.right - rect.left, height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0 || text.empty()) return true;
    if (width > 4096 || height > 1024 || static_cast<size_t>(width) * height > 262144)
        return false;
    auto blend = [&](const CachedLabel& label) {
        BLENDFUNCTION operation{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        return AlphaBlend(dc, rect.left, rect.top, width, height,
                          label.dc, 0, 0, width, height, operation) != FALSE;
    };
    if (!g_textCache) g_textCache.emplace();
    LabelCache& cache = percentage ? g_textCache->percentages : g_textCache->general;
    for (const auto& label : cache.labels) {
        if (label->font == font && label->format == format && label->color == color &&
            label->width == width && label->height == height && label->text == text)
            return blend(*label);
    }

    Gdiplus::Bitmap mask(width * TEXT_SUPERSAMPLE, height * TEXT_SUPERSAMPLE,
                         PixelFormat32bppARGB);
    if (mask.GetLastStatus() != Gdiplus::Ok) return false;
    {
        Gdiplus::Graphics graphics(&mask);
        graphics.SetPageUnit(Gdiplus::UnitPixel);
        graphics.Clear(Gdiplus::Color(255, 0, 0, 0));
        graphics.ScaleTransform(static_cast<float>(TEXT_SUPERSAMPLE),
                                static_cast<float>(TEXT_SUPERSAMPLE));
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        Gdiplus::StringFormat layout(Gdiplus::StringFormat::GenericTypographic());
        layout.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox |
            ((format & DT_SINGLELINE) ? Gdiplus::StringFormatFlagsNoWrap : 0));
        layout.SetAlignment((format & DT_CENTER) ? Gdiplus::StringAlignmentCenter :
            (format & DT_RIGHT) ? Gdiplus::StringAlignmentFar : Gdiplus::StringAlignmentNear);
        layout.SetLineAlignment((format & DT_VCENTER) ? Gdiplus::StringAlignmentCenter :
            Gdiplus::StringAlignmentNear);
        layout.SetTrimming((format & DT_END_ELLIPSIS) ? Gdiplus::StringTrimmingEllipsisCharacter :
            Gdiplus::StringTrimmingNone);
        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
        Gdiplus::RectF bounds(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
        if (graphics.DrawString(text.c_str(), static_cast<int>(text.size()), typography,
                                bounds, &layout, &brush) != Gdiplus::Ok) return false;
        graphics.Flush(Gdiplus::FlushIntentionSync);
    }

    auto label = std::make_unique<CachedLabel>();
    label->text = text; label->font = font; label->format = format; label->color = color;
    label->width = width; label->height = height;
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    label->bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    label->dc = CreateCompatibleDC(nullptr);
    if (!label->bitmap || !bits || !label->dc) return false;
    HGDIOBJ previous = SelectObject(label->dc, label->bitmap);
    if (!previous || previous == HGDI_ERROR) return false;
    label->previous = previous;

    Gdiplus::Rect area(0, 0, width * TEXT_SUPERSAMPLE,
                       height * TEXT_SUPERSAMPLE);
    Gdiplus::BitmapData data{};
    if (mask.LockBits(&area, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB,
                      &data) != Gdiplus::Ok) return false;
    auto* pixels = static_cast<DWORD*>(bits);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            pixels[static_cast<size_t>(y) * width + x] = TextCoveragePixel(
                static_cast<const BYTE*>(data.Scan0), data.Stride, x, y, color);
        }
    }
    mask.UnlockBits(&data);

    size_t bytes = static_cast<size_t>(width) * height * sizeof(DWORD);
    GdiFlush();
    while (!cache.labels.empty() &&
           (cache.labels.size() >= 128 || cache.bytes + bytes > 8 * 1024 * 1024)) {
        const auto& oldest = cache.labels.front();
        cache.bytes -= static_cast<size_t>(oldest->width) * oldest->height * sizeof(DWORD);
        cache.labels.erase(cache.labels.begin());
    }
    cache.bytes += bytes;
    cache.labels.push_back(std::move(label));
    return blend(*cache.labels.back());
}

void DrawLabel(HDC dc, const std::wstring& text, RECT rect, HFONT font,
               COLORREF color, UINT format, bool percentage = false) {
    Gdiplus::Font* typography = font == g_titleFont ? g_titleTypography :
        font == g_bodyFont ? g_bodyTypography :
        font == g_smallFont ? g_smallTypography : nullptr;
    if (typography && typography->GetLastStatus() == Gdiplus::Ok &&
        DrawSmoothLabel(dc, text, rect, font, typography, color, format, percentage)) return;
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetTextColor(dc, color);
    SetBkMode(dc, TRANSPARENT);
    DrawTextW(dc, text.c_str(), -1, &rect, format | DT_NOPREFIX);
    SelectObject(dc, oldFont);
}

void CancelNameTooltip(HWND window) {
    KillTimer(window, TIMER_NAME_TOOLTIP);
    if (g_nameTooltip && IsWindowVisible(g_nameTooltip)) {
        ShowWindow(g_nameTooltip, SW_HIDE);
    }
    g_nameTooltipText.clear();
    g_nameTooltipClipped = false;
    g_nameTooltipSince = 0;
}

bool NameIsClipped(HWND window, const std::wstring& text, const RECT& rect) {
    int width = rect.right - rect.left;
    if (text.empty() || width <= 0) return false;

    if (g_bodyTypography && g_bodyTypography->GetLastStatus() == Gdiplus::Ok) {
        Gdiplus::Bitmap bitmap(1, 1, PixelFormat32bppARGB);
        Gdiplus::Graphics graphics(&bitmap);
        graphics.SetPageUnit(Gdiplus::UnitPixel);
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

        Gdiplus::StringFormat layout(Gdiplus::StringFormat::GenericTypographic());
        layout.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox |
                              Gdiplus::StringFormatFlagsNoWrap);
        layout.SetTrimming(Gdiplus::StringTrimmingNone);

        Gdiplus::RectF measured{};
        if (graphics.MeasureString(
                text.c_str(), static_cast<int>(text.size()), g_bodyTypography,
                Gdiplus::PointF(0.0f, 0.0f), &layout, &measured) == Gdiplus::Ok) {
            return measured.Width > static_cast<float>(width);
        }
    }

    HDC dc = GetDC(window);
    if (!dc) return false;
    HGDIOBJ oldFont = SelectObject(dc, g_bodyFont);
    RECT measured{};
    DrawTextW(dc, text.c_str(), -1, &measured,
              DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(dc, oldFont);
    ReleaseDC(window, dc);
    return measured.right - measured.left > width;
}

void UpdateNameTooltip(HWND window, POINT point) {
    if (!IsWindowVisible(window) || g_outputMenuOpen || g_volumeEntry ||
        g_dragRow != DRAG_NONE) {
        CancelNameTooltip(window);
        return;
    }

    RECT client{};
    GetClientRect(window, &client);
    RECT rect = DeviceNameRect(client.right);
    std::wstring text;

    if (PtInRect(&rect, point)) {
        text = g_endpointName.empty() ? L"No output device" : g_endpointName;
    } else if (PtInRect(&client, point)) {
        int row = RowFromPoint(point);
        if (row != DRAG_NONE) {
            rect = NameRectForRow(VisibleRowForDataRow(row), client.right);
            if (PtInRect(&rect, point)) {
                text = row == DRAG_MASTER
                    ? (g_endpointVolume ? L"Master volume" : L"Output unavailable")
                    : (*g_apps)[row].name;
            }
        }
    }

    if (text == g_nameTooltipText && EqualRect(&rect, &g_nameTooltipRect)) return;

    CancelNameTooltip(window);
    g_nameTooltipText = std::move(text);
    g_nameTooltipRect = rect;
    g_nameTooltipClipped = NameIsClipped(window, g_nameTooltipText, rect);

    if (g_nameTooltipClipped) {
        g_nameTooltipSince = GetTickCount64();
        SetTimer(window, TIMER_NAME_TOOLTIP, NAME_TOOLTIP_DELAY, nullptr);
    }
}

void PaintNameTooltip(HWND window, HDC dc) {
    RECT client{};
    GetClientRect(window, &client);

    HBRUSH background = CreateSolidBrush(g_theme.panel);
    FillRect(dc, &client, background);
    DeleteObject(background);

    RECT textRect = client;
    InflateRect(&textRect, -Scale(8), -Scale(3));

    DrawLabel(dc, g_nameTooltipText, textRect, g_bodyFont, g_theme.text,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

SIZE MeasureNameTooltipText() {
    SIZE result{0, 0};
    if (g_nameTooltipText.empty()) return result;

    if (g_bodyTypography && g_bodyTypography->GetLastStatus() == Gdiplus::Ok) {
        Gdiplus::Bitmap bitmap(1, 1, PixelFormat32bppARGB);
        Gdiplus::Graphics graphics(&bitmap);
        graphics.SetPageUnit(Gdiplus::UnitPixel);
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

        Gdiplus::StringFormat layout(Gdiplus::StringFormat::GenericTypographic());
        layout.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox |
                              Gdiplus::StringFormatFlagsNoWrap);
        layout.SetTrimming(Gdiplus::StringTrimmingNone);

        Gdiplus::RectF measured{};
        if (graphics.MeasureString(
                g_nameTooltipText.c_str(),
                static_cast<int>(g_nameTooltipText.size()),
                g_bodyTypography,
                Gdiplus::PointF(0.0f, 0.0f),
                &layout,
                &measured) == Gdiplus::Ok) {
            result.cx = static_cast<LONG>(std::ceil(measured.Width));
            result.cy = static_cast<LONG>(std::ceil(measured.Height)) + Scale(2);
            return result;
        }
    }

    HWND owner = g_hWnd.load();
    HDC dc = owner ? GetDC(owner) : nullptr;
    if (!dc) return result;

    HGDIOBJ oldFont = SelectObject(dc, g_bodyFont);
    RECT measured{};
    DrawTextW(dc, g_nameTooltipText.c_str(), -1, &measured,
              DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(dc, oldFont);
    ReleaseDC(owner, dc);

    result.cx = measured.right - measured.left;
    result.cy = measured.bottom - measured.top;
    return result;
}

void ApplyNameTooltipWindowStyle() {
    if (!g_nameTooltip) return;

    LONG_PTR exStyle = GetWindowLongPtrW(g_nameTooltip, GWL_EXSTYLE);

    if (g_settings.backgroundOpacity < 100) {
        if (!(exStyle & WS_EX_LAYERED)) {
            exStyle |= WS_EX_LAYERED;
            SetWindowLongPtrW(g_nameTooltip, GWL_EXSTYLE, exStyle);
        }

        int tooltipOpacity = std::max(85, g_settings.backgroundOpacity);
        BYTE alpha = static_cast<BYTE>(MulDiv(255, tooltipOpacity, 100));
        SetLayeredWindowAttributes(g_nameTooltip, 0, alpha, LWA_ALPHA);
    } else if (exStyle & WS_EX_LAYERED) {
        exStyle &= ~WS_EX_LAYERED;
        SetWindowLongPtrW(g_nameTooltip, GWL_EXSTYLE, exStyle);
    }

    RECT bounds{};
    if (GetWindowRect(g_nameTooltip, &bounds)) {
        int width = bounds.right - bounds.left;
        int height = bounds.bottom - bounds.top;
        int radius = Radius(8);

        HRGN region = CreateRoundRectRgn(
            0, 0, width + 1, height + 1, radius, radius);

        if (region && !SetWindowRgn(g_nameTooltip, region, TRUE)) {
            DeleteObject(region);
        }
    }
}

void PositionNameTooltip(POINT cursor) {
    if (!g_nameTooltip || g_nameTooltipText.empty()) return;

    SIZE measured = MeasureNameTooltipText();

    int width = std::max(
        Scale(28), static_cast<int>(measured.cx) + Scale(16));
    int height = std::max(
        Scale(26), static_cast<int>(measured.cy) + Scale(6));

    int x = cursor.x + Scale(12);
    int y = cursor.y + Scale(20);

    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{sizeof(monitorInfo)};

    if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
        const RECT& work = monitorInfo.rcWork;
        int workWidth = work.right - work.left;

        width = std::min(width, std::max(Scale(40), workWidth - Scale(16)));

        if (x + width > work.right - Scale(8)) {
            x = work.right - Scale(8) - width;
        }
        if (x < work.left + Scale(8)) {
            x = work.left + Scale(8);
        }

        if (y + height > work.bottom - Scale(8)) {
            y = cursor.y - height - Scale(12);
        }
        if (y < work.top + Scale(8)) {
            y = work.top + Scale(8);
        }
    }

    SetWindowPos(
        g_nameTooltip, HWND_TOPMOST, x, y, width, height,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);

    ApplyNameTooltipWindowStyle();
    InvalidateRect(g_nameTooltip, nullptr, FALSE);
    UpdateWindow(g_nameTooltip);
}

LRESULT CALLBACK NameTooltipProc(HWND window, UINT message,
                                 WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT paint{};
            HDC dc = BeginPaint(window, &paint);
            PaintNameTooltip(window, dc);
            EndPaint(window, &paint);
            return 0;
        }

        case WM_PRINTCLIENT:
            if (reinterpret_cast<HDC>(wParam)) {
                PaintNameTooltip(window, reinterpret_cast<HDC>(wParam));
            }
            return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

void ShowNameTooltip(HWND window) {
    KillTimer(window, TIMER_NAME_TOOLTIP);

    POINT point{};
    if (!GetCursorPos(&point) || WindowFromPoint(point) != window) {
        CancelNameTooltip(window);
        return;
    }

    POINT clientPoint = point;
    ScreenToClient(window, &clientPoint);
    UpdateNameTooltip(window, clientPoint);

    if (!g_nameTooltipClipped) return;

    ULONGLONG elapsed = GetTickCount64() - g_nameTooltipSince;
    if (elapsed < NAME_TOOLTIP_DELAY) {
        SetTimer(window, TIMER_NAME_TOOLTIP,
                 NAME_TOOLTIP_DELAY - static_cast<UINT>(elapsed), nullptr);
        return;
    }

    if (!g_nameTooltip) {
        g_nameTooltip = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            NAME_TOOLTIP_CLASS, L"", WS_POPUP,
            0, 0, 1, 1, window, nullptr, GetModuleHandleW(nullptr), nullptr);

        if (!g_nameTooltip) return;
    }

    PositionNameTooltip(point);
}

COLORREF MixColor(COLORREF from, COLORREF to, float amount) {
    amount = ClampVolume(amount);
    return RGB(
        static_cast<int>(GetRValue(from) + (GetRValue(to) - GetRValue(from)) * amount),
        static_cast<int>(GetGValue(from) + (GetGValue(to) - GetGValue(from)) * amount),
        static_cast<int>(GetBValue(from) + (GetBValue(to) - GetBValue(from)) * amount));
}

COLORREF IconColor() {
    return g_lightTheme
        ? MixColor(g_theme.text, g_theme.secondaryText, 0.35f)
        : g_theme.secondaryText;
}

// Share of the text color laid over a hovered surface. A highlight that reads
// well on an opaque background all but disappears on a translucent one, so it
// grows stronger as the background opacity drops.
float HoverStrength() {
    float opacity = g_settings.backgroundOpacity < 100 && !g_transparencyFailed
        ? g_settings.backgroundOpacity / 100.0f : 1.0f;
    float opaque = g_lightTheme ? 0.055f : 0.05f;
    float clear = g_lightTheme ? 0.11f : 0.085f;
    return clear + (opaque - clear) * opacity;
}

// Resolves a surface color for the current paint pass: the base color at the
// background opacity, with `highlight` (0 for none, 1 for a full hover) laid
// over it at a strength that does not fade with the background.
COLORREF SurfaceColor(COLORREF base, float highlight) {
    if (g_paintMatte >= 0) {
        base = MixColor(g_paintMatte ? RGB(255, 255, 255) : RGB(0, 0, 0),
                        base, g_settings.backgroundOpacity / 100.0f);
    }
    return highlight > 0.0f
        ? MixColor(base, g_theme.text, HoverStrength() * highlight) : base;
}

void DrawSurface(HDC dc, const RECT& rect, int radius, COLORREF color,
                 float highlight = 0.0f) {
    DrawRoundedRect(dc, rect, radius, SurfaceColor(color, highlight));
}

float EaseTowards(float current, float target, float speed) {
    if (!g_motionEnabled) {
        return target;
    }
    float next = current + (target - current) *
        (1.0f - std::exp(-speed * g_frameSeconds));
    return std::abs(target - next) < 0.001f ? target : next;
}

void DrawSlider(HDC dc, const RECT& rect, float volume, float peak,
                bool muted, float thumbDot, ThumbState thumbState) {
    std::optional<Gdiplus::Graphics> graphics;
    if (g_gdiplusToken) {
        graphics.emplace(dc);
        if (graphics->GetLastStatus() != Gdiplus::Ok ||
            graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias) != Gdiplus::Ok ||
            graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf) != Gdiplus::Ok) {
            graphics.reset();
        }
    }

    auto capsule = [&](float x, float y, float width, float height, COLORREF color,
                       BYTE alpha = 255) {
        if (width <= 0.0f || height <= 0.0f) return;
        float diameter = std::min(width, height);
        if (!graphics && alpha < 255) {
            // GDI cannot blend, so approximate against the mixer background.
            color = MixColor(g_theme.background, color, alpha / 255.0f);
        }
        if (graphics) {
            Gdiplus::GraphicsPath path;
            path.AddArc(x, y, diameter, diameter, 180.0f, 90.0f);
            path.AddArc(x + width - diameter, y, diameter, diameter, 270.0f, 90.0f);
            path.AddArc(x + width - diameter, y + height - diameter,
                        diameter, diameter, 0.0f, 90.0f);
            path.AddArc(x, y + height - diameter, diameter, diameter, 90.0f, 90.0f);
            path.CloseFigure();
            Gdiplus::SolidBrush brush(Gdiplus::Color(
                alpha, GetRValue(color), GetGValue(color), GetBValue(color)));
            graphics->FillPath(&brush, &path);
        } else {
            RECT bounds = {static_cast<LONG>(std::lround(x)),
                           static_cast<LONG>(std::lround(y)),
                           static_cast<LONG>(std::lround(x + width)),
                           static_cast<LONG>(std::lround(y + height))};
            DrawRoundedRect(dc, bounds, std::max(1, static_cast<int>(std::lround(diameter))),
                            color);
        }
    };

    float density = g_dpi / 96.0f;
    float centerY = (rect.top + rect.bottom) * 0.5f;
    float left = static_cast<float>(rect.left);
    float width = static_cast<float>(rect.right - rect.left);
    if (width <= 0.0f) return;
    bool windows10 = IsWindows10Style();
    float thumbDiameter = SliderThumbWidth();
    float thumbHeight = SliderThumbHeight();
    float thumbRadius = thumbDiameter * 0.5f;
    float trackHeight = (windows10 ? 2.0f : 4.0f) * density;
    float trackTop = centerY - trackHeight * 0.5f;
    float thumbX = SliderThumbCenter(rect, volume);

    // The native rail is a translucent overlay rather than a fixed gray, so
    // it adapts to whatever is behind it. A muted row dims both parts of the
    // rail, like a disabled native slider, with the filled part still distinct.
    // The Windows 10 rail is fainter and brightens under the pointer.
    COLORREF overlay = g_lightTheme ? RGB(0, 0, 0) : RGB(255, 255, 255);
    BYTE railAlpha = muted ? (g_lightTheme ? 0x51 : 0x3F)
                           : (g_lightTheme ? 0x72 : 0x8B);
    if (windows10) {
        railAlpha = muted ? 0x33 : thumbState == ThumbState::Hover ? 0x99 : 0x66;
    }
    COLORREF activeTrack = muted ? g_theme.muted : g_theme.sliderFill;
    capsule(left, trackTop, width, trackHeight, overlay, railAlpha);
    capsule(left, trackTop, thumbX - left, trackHeight, activeTrack);

    if (!muted && peak > 0.01f) {
        // Compact rows have no room below the thumb, so their meter runs
        // directly under the rail and passes behind the thumb.
        float meterTop = IsCompact() ? trackTop + trackHeight + density
                                     : centerY + thumbHeight * 0.5f + density;
        capsule(left, meterTop, width * ClampVolume(peak),
                (IsCompact() ? 2.0f : 3.0f) * density,
                MixColor(g_theme.sliderFill, g_theme.text, 0.15f));
    }

    if (windows10) {
        // The Windows 10 thumb is a flat bar in the accent color. It turns
        // near-white (near-black in the light theme) under the pointer, and
        // gray while it is held.
        COLORREF thumb = activeTrack;
        if (!muted && thumbState == ThumbState::Hover) {
            thumb = g_lightTheme ? RGB(23, 23, 23) : RGB(242, 242, 242);
        } else if (!muted && thumbState == ThumbState::Pressed) {
            thumb = g_lightTheme ? RGB(204, 204, 204) : RGB(118, 118, 118);
        }
        capsule(thumbX - thumbRadius, centerY - thumbHeight * 0.5f,
                thumbDiameter, thumbHeight, thumb);
        return;
    }

    // The outer thumb is a solid control surface, so it stays opaque at any
    // background opacity. Only the accent dot reacts to the pointer. Its 1px
    // border is a faint overlay that is slightly stronger along one edge.
    float surfaceDiameter = thumbDiameter - 2.0f * density;
    if (graphics) {
        float top = centerY - thumbRadius;
        BYTE red = GetRValue(overlay), green = GetGValue(overlay),
             blue = GetBValue(overlay);
        Gdiplus::LinearGradientBrush border(
            Gdiplus::PointF(0.0f, top - 1.0f),
            Gdiplus::PointF(0.0f, top + thumbDiameter + 1.0f),
            Gdiplus::Color(g_lightTheme ? 0x0F : 0x18, red, green, blue),
            Gdiplus::Color(g_lightTheme ? 0x29 : 0x12, red, green, blue));
        Gdiplus::GraphicsPath ring; // Alternate fill leaves the middle empty.
        ring.AddEllipse(thumbX - thumbRadius, top, thumbDiameter, thumbDiameter);
        ring.AddEllipse(thumbX - surfaceDiameter * 0.5f,
                        centerY - surfaceDiameter * 0.5f,
                        surfaceDiameter, surfaceDiameter);
        graphics->FillPath(&border, &ring);
    }
    capsule(thumbX - surfaceDiameter * 0.5f, centerY - surfaceDiameter * 0.5f,
            surfaceDiameter, surfaceDiameter, g_theme.thumb);
    float dotDiameter = thumbDiameter * 0.6f * thumbDot;
    capsule(thumbX - dotDiameter * 0.5f, centerY - dotDiameter * 0.5f,
            dotDiameter, dotDiameter, activeTrack);
}

// Same level thresholds as the speaker icon in the Windows volume flyout.
PCWSTR VolumeGlyph(float volume, bool muted) {
    if (muted) return L"\uE74F";
    int percent = static_cast<int>(volume * 100.0f + 0.5f);
    return percent == 0 ? L"\uE992" : percent < 33 ? L"\uE993" :
           percent < 66 ? L"\uE994" : L"\uE995";
}

// rowColor and rowHighlight describe the row surface behind the button.
void DrawMuteButton(HDC dc, int visibleRow, int clientWidth, bool muted,
                    float volume, COLORREF rowColor, float rowHighlight) {
    RECT button = MuteRectForRow(visibleRow, clientWidth);
    if (muted) {
        DrawSurface(dc, button, Radius(8), rowColor, rowHighlight + 1.0f);
    }
    DrawLabel(dc, VolumeGlyph(volume, muted), button, g_symbolFont,
              muted ? g_theme.accent : IconColor(),
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void DrawAudioRow(HDC dc, int visibleRow, int dataRow, int clientWidth) {
    int top = RowTop(visibleRow);
    bool master = dataRow == DRAG_MASTER;
    std::wstring name;
    float volume;
    float peak = 0.0f;
    bool muted;
    HICON icon = nullptr;

    if (master) {
        name = g_endpointVolume ? L"Master volume" : L"Output unavailable";
        if (g_paintMatte != 1) {
            g_masterPaintVolume = GetMasterVolume();
            g_masterPaintMuted = GetMasterMuted();
        }
        volume = g_masterPaintVolume;
        muted = g_masterPaintMuted;
        icon = g_masterIcon;
    } else {
        const AppSession& app = (*g_apps)[dataRow];
        name = app.name;
        volume = app.volume;
        muted = app.muted;
        if (g_paintMatte != 1) peak = GetAppPeak(app);
        icon = app.icon;
    }

    std::wstring visualKey = master ? L"master" :
        L"app:" + std::to_wstring((*g_apps)[dataRow].processId) + L":" + name;
    RowVisual& visual = g_rowVisuals[visualKey];
    if (!visual.initialized) {
        visual.volume = volume;
        visual.initialized = true;
    }
    if (g_paintMatte != 1) {
        float targetHover = g_hoverRow == dataRow ? 1.0f : 0.0f;
        // The accent dot grows while the pointer is over the thumb and shrinks
        // while the thumb is held. Test against the thumb as it is drawn, so
        // the state stays right when the volume changes under a still pointer.
        visual.thumbState = ThumbState::Rest;
        if (g_dragRow == dataRow) {
            visual.thumbState = ThumbState::Pressed;
        } else if (g_dragRow == DRAG_NONE && g_hoverRow == dataRow &&
                   SliderThumbContains(SliderRectForRow(visibleRow, clientWidth),
                                       visual.volume, g_pointer)) {
            visual.thumbState = ThumbState::Hover;
        }
        float targetThumbDot =
            visual.thumbState == ThumbState::Pressed ? THUMB_DOT_PRESSED :
            visual.thumbState == ThumbState::Hover ? THUMB_DOT_HOVER : THUMB_DOT_REST;
        g_frameAnimating |= std::abs(visual.hover - targetHover) >= 0.001f ||
                            std::abs(visual.volume - volume) >= 0.001f ||
                            std::abs(visual.thumbDot - targetThumbDot) >= 0.001f;
        visual.thumbDot = EaseTowards(visual.thumbDot, targetThumbDot, 18.0f);
        g_frameHasAudio |= peak > 0.005f || visual.peak > 0.005f;
        visual.hover = EaseTowards(visual.hover, g_hoverRow == dataRow ? 1.0f : 0.0f, 18.0f);
        visual.volume = g_dragRow == dataRow ? volume : EaseTowards(visual.volume, volume, 22.0f);
        visual.peak = EaseTowards(visual.peak, peak, peak > visual.peak ? 35.0f : 8.0f);
    }
    RECT rowRect = {Scale(12), top, clientWidth - Scale(12), top + RowHeight() - Scale(2)};
    COLORREF base = master ? g_theme.panel : g_theme.background;
    DrawSurface(dc, rowRect, Radius(16), base, visual.hover);

    if (g_settings.keyboardControls && g_selectedRow == dataRow) {
        RECT marker{Scale(14), top + Scale(12), Scale(17),
                    top + RowHeight() - Scale(14)};
        DrawRoundedRect(dc, marker, Radius(3), g_theme.accent);
    }

    RECT iconTile = IconTileRectForRow(visibleRow);
    DrawSurface(dc, iconTile, Radius(10),
                    master ? g_theme.background : g_theme.panel);
    if (icon) {
        int iconSize = Scale(24);
        DrawIconEx(dc,
                   iconTile.left + (iconTile.right - iconTile.left - iconSize) / 2,
                   iconTile.top + (iconTile.bottom - iconTile.top - iconSize) / 2,
                   icon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
    } else {
        DrawLabel(dc, L"\uE8D6", iconTile, g_symbolFont, IconColor(),
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    RECT nameRect = NameRectForRow(visibleRow, clientWidth);
    DrawLabel(dc, name, nameRect, g_bodyFont, g_theme.text,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    WCHAR percent[16];
    swprintf_s(percent, L"%d%%",
               static_cast<int>(volume * 100.0f + 0.5f));
    RECT percentRect = PercentRectForRow(visibleRow, clientWidth);
    // The percentages have a cache of their own: there are only 101 of them,
    // so even a slider being dragged soon finds its label already drawn.
    DrawLabel(dc, percent, percentRect, g_bodyFont,
              muted ? g_theme.secondaryText : g_theme.text,
              DT_RIGHT | DT_VCENTER | DT_SINGLELINE, /*percentage=*/true);

    // In compact mode the thumb can reach the mute button's highlight, so the
    // slider is drawn on top of it.
    DrawMuteButton(dc, visibleRow, clientWidth, muted, volume, base, visual.hover);
    DrawSlider(dc, SliderRectForRow(visibleRow, clientWidth), visual.volume,
               visual.peak, muted, visual.thumbDot, visual.thumbState);
    if (!master) {
        RECT pin = PinRectForRow(visibleRow, clientWidth);
        bool pinned = (*g_apps)[dataRow].pinned;
        DrawLabel(dc, pinned ? L"\uE840" : L"\uE718", pin, g_symbolFont,
                  pinned ? g_theme.accent : IconColor(),
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void RenderMixerContents(HDC dc, const RECT& client) {
    if (g_paintMatte >= 0) {
        FillSolidRect(dc, client, g_paintMatte ? RGB(255, 255, 255) : RGB(0, 0, 0));
        DrawSurface(dc, client, Radius(20), g_theme.background);
    } else {
        FillSolidRect(dc, client, g_theme.background);
    }

    if (!IsCompact()) {
        DrawLabel(dc, L"Volume mixer", TitleRect(client.right), g_titleFont,
                  g_theme.text, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }

    RECT pickerRect = OutputPickerRect(client.right);
    DrawSurface(dc, pickerRect, Radius(16),
                MixColor(g_theme.panel, RGB(0, 0, 0), 0.15f),
                g_outputHovered || g_outputMenuOpen ? 1.0f : 0.0f);
    RECT deviceIconRect = {pickerRect.left + Scale(8), pickerRect.top,
                           pickerRect.left + Scale(34), pickerRect.bottom};
    DrawLabel(dc, L"\uE7F5", deviceIconRect, g_symbolFont, IconColor(),
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    RECT deviceNameRect = DeviceNameRect(client.right);
    DrawLabel(dc, g_endpointName.empty() ? L"No output device" : g_endpointName,
              deviceNameRect, g_bodyFont, g_theme.text,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    RECT arrowRect = {pickerRect.right - Scale(30), pickerRect.top,
                      pickerRect.right - Scale(8), pickerRect.bottom};
    DrawLabel(dc, L"\uE70D", arrowRect, g_symbolFont, IconColor(),
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    RECT closeRect = CloseButtonRect(client.right);
    if (g_paintMatte != 1) {
        g_frameAnimating |= std::abs(g_closeHoverAmount -
                                     (g_closeHovered ? 1.0f : 0.0f)) >= 0.001f;
        g_closeHoverAmount = EaseTowards(g_closeHoverAmount, g_closeHovered ? 1.0f : 0.0f, 18.0f);
    }
    DrawSurface(dc, closeRect, Radius(8), g_theme.background, g_closeHoverAmount);
    DrawLabel(dc, L"\uE8BB", closeRect, g_symbolFont, IconColor(),
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    if (!g_audioAvailable) {
        RECT message = {Scale(24), HeaderHeight() + Scale(30),
                        client.right - Scale(24), client.bottom - Scale(20)};
        DrawLabel(dc, L"No controllable playback sessions or default output.", message,
                  g_bodyFont, g_theme.secondaryText,
                  DT_CENTER | DT_VCENTER | DT_WORDBREAK);
    } else {
        DrawAudioRow(dc, 0, DRAG_MASTER, client.right);

        RECT section = {Scale(24),
                        HeaderHeight() + RowHeight() + Scale(IsCompact() ? 3 : 5),
                        client.right - Scale(24),
                        RowTop(1) - Scale(IsCompact() ? 2 : 3)};
        DrawLabel(dc, L"Applications", section, g_smallFont, g_theme.secondaryText,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        DrawLabel(dc, std::to_wstring(g_apps->size()), section, g_smallFont,
                  g_theme.secondaryText, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        if (g_apps->empty()) {
            RECT empty = {Scale(24), RowTop(1) + Scale(8),
                          client.right - Scale(24), client.bottom - Scale(12)};
            DrawLabel(dc, g_settings.showAllSessions
                              ? L"No applications have a playback session."
                              : L"No app sessions found. Start playback to add one.",
                      empty, g_bodyFont, g_theme.secondaryText,
                      DT_CENTER | DT_VCENTER | DT_WORDBREAK);
        } else {
            int count = VisibleAppCount();
            for (int i = 0; i < count; i++) {
                DrawAudioRow(dc, i + 1, g_scrollRow + i, client.right);
            }
        }
    }

    if (PageCount() > 1) {
        for (bool next : {false, true}) {
            RECT button = PageButtonRect(next, client);
            bool hovered = g_hoverPageButton == (next ? 1 : 0);
            DrawRoundedRect(dc, button, Radius(8), hovered
                ? MixColor(g_theme.panel, g_theme.text, HoverStrength())
                : g_theme.panel);
            DrawLabel(dc, next ? L"Next" : L"Previous", button, g_smallFont,
                      g_theme.text, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        RECT previousButton = PageButtonRect(false, client);
        RECT counter = {Scale(128), previousButton.top,
                        client.right - Scale(128), previousButton.bottom};
        std::wstring text = L"Page " + std::to_wstring(g_scrollRow / PageSize() + 1) +
                            L" of " + std::to_wstring(PageCount());
        DrawLabel(dc, text, counter, g_smallFont, g_theme.secondaryText,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

struct PixelBuffer {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    DWORD* pixels = nullptr;
    int width = 0, height = 0;
    void Reset() {
        if (dc && previous) SelectObject(dc, previous);
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
        dc = nullptr; bitmap = nullptr; previous = nullptr; pixels = nullptr;
        width = height = 0;
    }
    bool Ensure(int w, int h) {
        if (dc && width == w && height == h) return true;
        Reset();
        if (w <= 0 || h <= 0) return false;
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = w;
        info.bmiHeader.biHeight = -h;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        dc = CreateCompatibleDC(nullptr);
        if (!bitmap || !bits || !dc) { Reset(); return false; }
        previous = SelectObject(dc, bitmap);
        if (!previous || previous == HGDI_ERROR) { previous = nullptr; Reset(); return false; }
        pixels = static_cast<DWORD*>(bits);
        width = w; height = h;
        return true;
    }
};
PixelBuffer g_blackFrame, g_whiteFrame;

DWORD RecoverPixelAlpha(DWORD black, DWORD white) {
    // Only surfaces are drawn differently against the two mattes; opaque
    // foreground content (text, icons, dividers, fully-covered pixels of
    // antialiased edges) comes out byte-identical either way. That case is
    // bit-for-bit equivalent to the general formula below (equal channels
    // give zero transparency, so alpha is 255 and each output channel is
    // just min(blackChannel, 255) == blackChannel), so skip straight to the
    // answer instead of doing three shifts, two subtractions and two clamps
    // per pixel for content that is going to end up fully opaque anyway.
    if ((black & 0x00FFFFFFu) == (white & 0x00FFFFFFu)) {
        return 0xFF000000u | (black & 0x00FFFFFFu);
    }
    int transparency = 0;
    for (int shift : {0, 8, 16}) {
        int difference = static_cast<int>((white >> shift) & 255) -
                         static_cast<int>((black >> shift) & 255);
        transparency = std::max(transparency, difference);
    }
    DWORD alpha = 255 - ClampInt(transparency, 0, 255);
    DWORD result = alpha << 24;
    for (int shift : {0, 8, 16}) {
        result |= std::min<DWORD>((black >> shift) & 255UL, alpha) << shift;
    }
    return result;
}

// Drawing the mixer is what a frame costs; moving a drawn frame is nearly
// free. A closing flyout therefore keeps the picture it has, and an opening
// one redraws only as often as the level meters update.
bool CanReuseTransparentFrame(const RECT& client, ULONGLONG now) {
    return g_frameRenderTime && !g_openWarmup && g_blackFrame.dc &&
        g_blackFrame.width == client.right && g_blackFrame.height == client.bottom &&
        (g_closeTime || (g_openTime && now - g_frameRenderTime < 33000));
}

bool PaintTransparentMixer(HWND window, const RECT& client, ULONGLONG now,
                           bool reuse = false) {
    if (!reuse) {
        g_frameRenderTime = 0;
        if (!g_blackFrame.Ensure(client.right, client.bottom) ||
            !g_whiteFrame.Ensure(client.right, client.bottom)) return false;
        // GDI does not preserve alpha. Draw the same scene against black and
        // white mattes to recover per-pixel coverage, including antialiased
        // text/icons. Only surfaces use backgroundOpacity; foreground
        // primitives stay opaque.
        g_paintMatte = 0;
        RenderMixerContents(g_blackFrame.dc, client);
        g_paintMatte = 1;
        RenderMixerContents(g_whiteFrame.dc, client);
        g_paintMatte = -1;
        GdiFlush();
        size_t count = static_cast<size_t>(client.right) * client.bottom;
        for (size_t i = 0; i < count; ++i) {
            g_blackFrame.pixels[i] = RecoverPixelAlpha(g_blackFrame.pixels[i],
                                                       g_whiteFrame.pixels[i]);
        }
        g_frameRenderTime = now;
    }
    POINT origin{};
    SIZE size{client.right, client.bottom};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    if (!g_openTime && !g_closeTime) {
        return UpdateLayeredWindow(window, nullptr, nullptr, &size, g_blackFrame.dc,
                                   &origin, 0, &blend, ULW_ALPHA) != FALSE;
    }
    // Opening or closing: the frame also moves the window and fades it.
    FlyoutFrame frame = FlyoutFrameAt(now);
    RECT bounds = FlyoutBounds(frame.offset, &origin);
    POINT position{bounds.left, bounds.top};
    size = {bounds.right - bounds.left, bounds.bottom - bounds.top};
    // The first frame after opening stays invisible; see PaceAnimation.
    blend.SourceConstantAlpha = g_openWarmup
        ? 0 : static_cast<BYTE>(255.0f * ClampVolume(frame.opacity));
    BOOL result = UpdateLayeredWindow(window, nullptr, &position, &size, g_blackFrame.dc,
                                     &origin, 0, &blend, ULW_ALPHA);
    g_slideOffset = frame.offset;
    g_fadeOpacity = frame.opacity;
    if (g_openTime && frame.finished && !g_openWarmup) SettleFlyout(window);
    return result != FALSE;
}

void PaintMixer(HWND hWnd) {
    ULONGLONG now = AnimationClock();
    if (g_openWarmup && g_openTime) g_openTime = now;
    if (g_closeTime && FlyoutFrameAt(now).finished) {
        ValidateRect(hWnd, nullptr);
        CompleteHide(hWnd);
        return;
    }
    bool transparent = g_settings.backgroundOpacity < 100 && !g_transparencyFailed;
    if (transparent) {
        RECT content = MixerContentRect(hWnd);
        if (CanReuseTransparentFrame(content, now) &&
            PaintTransparentMixer(hWnd, content, now, true)) {
            ValidateRect(hWnd, nullptr);
            RunQueuedClose(hWnd);
            PaceAnimation(hWnd);
            return;
        }
    }
    g_frameAnimating = false;
    g_frameHasAudio = false;
    g_frameSeconds = g_lastPaintTime
        ? std::min(0.05f, static_cast<float>(now - g_lastPaintTime) / 1000000.0f)
        : 0.016f;
    g_lastPaintTime = now;
    if (g_rowVisuals.size() > 256) g_rowVisuals.clear();
    POINT origin{};
    bool arrived = false;
    if (!transparent && (g_openTime || g_closeTime)) {
        // An opaque flyout is an ordinary window: it slides by being moved,
        // and it cannot fade.
        FlyoutFrame frame = FlyoutFrameAt(now);
        origin = PlaceFlyoutFrame(hWnd, frame);
        arrived = g_openTime && frame.finished && !g_openWarmup;
    }
    PAINTSTRUCT paint;
    HDC windowDc = BeginPaint(hWnd, &paint);
    RECT client = MixerContentRect(hWnd);
    if (client.right <= 0 || client.bottom <= 0) { EndPaint(hWnd, &paint); return; }
    if (transparent) {
        if (PaintTransparentMixer(hWnd, client, now)) {
            UpdatePaintTimer(hWnd);
            EndPaint(hWnd, &paint);
            RunQueuedClose(hWnd);
            PaceAnimation(hWnd);
            return;
        }
        Wh_Log(L"Mixer: transparent rendering failed, using opaque background, error %lu", GetLastError());
        g_transparencyFailed = true;
        ApplyTransparencyStyle(hWnd);
    }
    g_paintMatte = -1;
    HDC dc = CreateCompatibleDC(windowDc);
    HBITMAP bitmap = CreateCompatibleBitmap(windowDc, client.right, client.bottom);
    if (!dc || !bitmap) {
        if (dc) DeleteDC(dc);
        if (bitmap) DeleteObject(bitmap);
        EndPaint(hWnd, &paint);
        return;
    }
    HGDIOBJ oldBitmap = SelectObject(dc, bitmap);
    RenderMixerContents(dc, client);
    BitBlt(windowDc, 0, 0, client.right - origin.x, client.bottom - origin.y, dc,
           origin.x, origin.y, SRCCOPY);
    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
    UpdatePaintTimer(hWnd);
    EndPaint(hWnd, &paint);
    if (arrived) SettleFlyout(hWnd);
    RunQueuedClose(hWnd);
    PaceAnimation(hWnd);
}

void ResizeMixer(HWND hWnd) {
    // A sliding flyout takes its size from PositionMixer with its next frame.
    if (g_openTime || g_closeTime) return;
    RECT windowRect;
    GetWindowRect(hWnd, &windowRect);
    SetWindowPos(hWnd, nullptr, windowRect.left, windowRect.top, PopupWidth(),
                 PopupHeight(), SWP_NOZORDER | SWP_NOACTIVATE);
}

bool GetTrayIconRect(RECT* rect) {
    NOTIFYICONIDENTIFIER identifier = {};
    identifier.cbSize = sizeof(identifier);
    identifier.hWnd = g_hWnd.load();
    identifier.uID = TRAY_ICON_ID;
    if (g_trayUsesGuid) identifier.guidItem = MIXER_TRAY_GUID;
    return SUCCEEDED(Shell_NotifyIconGetRect(&identifier, rect));
}

void PositionMixer(HWND hWnd) {
    RECT anchor = {};
    if (!GetTrayIconRect(&anchor)) {
        POINT cursor = {};
        GetCursorPos(&cursor);
        anchor = {cursor.x, cursor.y, cursor.x + 1, cursor.y + 1};
    }

    HMONITOR monitor = MonitorFromRect(&anchor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info = {sizeof(info)};
    GetMonitorInfoW(monitor, &info);

    // The taskbar flyouts of Windows 10 sit directly on the taskbar.
    int gap = IsWindows10Style() ? 0 : Scale(8);
    g_availableAppRows = AppRowsForHeight(info.rcWork.bottom - info.rcWork.top - 2 * gap);
    ClampPageOffset();
    int width = PopupWidth();
    int height = PopupHeight();
    int x = (anchor.left + anchor.right - width) / 2;
    int y = anchor.top - height - gap;

    if (y < info.rcWork.top) {
        y = anchor.bottom + gap;
    }
    x = ClampInt(x, info.rcWork.left, std::max(info.rcWork.left, info.rcWork.right - width));
    y = ClampInt(y, info.rcWork.top, std::max(info.rcWork.top, info.rcWork.bottom - height));

    // The flyout slides out from the taskbar's edge and is cut off there, so
    // that nothing of it shows over or through the taskbar. Where the tray
    // icon is not in a taskbar that takes up screen space (the taskbar hides
    // itself, or the icon is in the overflow), that edge is the icon's own.
    g_restRect = {x, y, x + width, y + height};
    g_slideFromAbove = y + height / 2 > (anchor.top + anchor.bottom) / 2;
    int edge = g_slideFromAbove
        ? std::max<int>(info.rcWork.top, std::min<int>(y, anchor.bottom))
        : std::min<int>(info.rcWork.bottom, std::max<int>(y + height, anchor.top));
    g_slideClip = {LONG_MIN / 2, LONG_MIN / 2, LONG_MAX / 2, LONG_MAX / 2};
    (g_slideFromAbove ? g_slideClip.top : g_slideClip.bottom) = edge;
    // A closed flyout is past that edge: all of it in the Windows 11 style, a
    // short way in the Windows 10 style. Windows cannot round the corners of
    // a window only a few pixels tall, so a strip of the flyout stays put.
    int hidden = g_slideFromAbove ? y + height - edge : edge - y;
    int travel = IsWindows10Style() ? Scale(50) : hidden;
    g_slideTravel = std::max(std::min(travel, hidden - Scale(20)), 0);
    // While the flyout slides, each frame puts the window where it belongs.
    if (g_openTime || g_closeTime) return;

    RECT current = {};
    if (!GetWindowRect(hWnd, &current) || current.left != x || current.top != y ||
        current.right - current.left != width || current.bottom - current.top != height) {
        SetWindowPos(hWnd, HWND_TOPMOST, x, y, width, height,
                     SWP_NOOWNERZORDER | SWP_NOACTIVATE);
    }
}

void ShowMixer(HWND hWnd) {
    if (g_showingMixer) {
        return;
    }
    g_showingMixer = true;
    KillTimer(hWnd, TIMER_CHECK_FOCUS);
    g_closeQueued = false;
    bool opening = !MixerOpen(hWnd);
    bool turning = false;
    if (opening) {
        SelectRow(DRAG_MASTER);
        RefreshAudioSessions(hWnd, true);
        UpdateMotionPreference(hWnd);
        // A flyout that is still closing turns around where it is.
        turning = g_closeTime != 0;
        g_closeTime = 0;
        g_openTime = g_motionEnabled ? AnimationClock() : 0;
        g_openWarmup = g_openTime && !turning;
        KillTimer(hWnd, TIMER_TRIM);
        g_pacedSince = 0;
        g_lastPaintTime = 0;
        g_hoverRow = DRAG_NONE;
        g_closeHovered = false;
        g_closeHoverAmount = 0;
        g_rowVisuals.clear();
    }
    PositionMixer(hWnd);
    if (opening && g_openTime) {
        g_slideStart = turning ? g_slideOffset : static_cast<float>(g_slideTravel);
        g_fadeStart = turning ? g_fadeOpacity : IsWindows10Style() ? 0.8f : 1.0f;
    }
    if (opening && g_settings.backgroundOpacity < 100 && !g_transparencyFailed) {
        // A hidden layered window keeps its last frame, and its backdrop
        // effect appears the moment the window is shown. Present the first
        // frame before showing the window, which is fully transparent when
        // animating, so neither can flash in the final position ahead of the
        // opening animation.
        RECT client = MixerContentRect(hWnd);
        if (client.right > 0 && client.bottom > 0) {
            PaintTransparentMixer(hWnd, client, g_openTime);
        }
    } else if (opening && g_openTime) {
        PlaceFlyoutFrame(hWnd, FlyoutFrameAt(g_openTime));
    }
    ShowWindow(hWnd, SW_SHOWNORMAL);
    SetTimer(hWnd, TIMER_REFRESH, 1000, nullptr);
    SetPaintTimer(hWnd, g_motionEnabled ? 16 : 80);
    BOOL foreground = SetForegroundWindow(hWnd);
    if (foreground) {
        SetFocus(hWnd);
    } else if (g_settings.closeWhenFocusIsLost) {
        SetTimer(hWnd, TIMER_CHECK_FOCUS, 150, nullptr);
    }
    InvalidateRect(hWnd, nullptr, FALSE);
    UpdateWindow(hWnd);
    g_showingMixer = false;
    Wh_Log(L"Mixer: popup shown, foreground request=%d", foreground);
}

void QueueMixerAction(HWND hWnd, bool close) {
    KillTimer(hWnd, TIMER_CHECK_FOCUS);
    if (!g_showRequestPending) {
        g_showRequestPending = true;
        g_closeRequestPending = close;
        // Return to the shell callback before showing/activating the popup.
        if (!PostMessageW(hWnd, WM_APP_SHOW_MIXER, 0, 0)) {
            g_showRequestPending = false;
            Wh_Log(L"Mixer: could not queue popup, error %lu", GetLastError());
        }
    }
}

void QueueShowMixer(HWND hWnd) {
    QueueMixerAction(hWnd, false);
}

void RememberTrayPress(HWND hWnd) {
    g_trayPressKnown = true;
    g_trayPressWasVisible = MixerOpen(hWnd);
}

void QueueTrayToggle(HWND hWnd, bool keyboard) {
    // The taskbar can take focus and dismiss the popup before mouse-up.
    // Use its visibility at mouse-down, so a closing click cannot reopen it.
    bool close = !keyboard && g_trayPressKnown
        ? g_trayPressWasVisible : MixerOpen(hWnd);
    g_trayPressKnown = false;
    // Like the taskbar's own flyouts, the mixer finishes opening or closing
    // before the icon toggles it again.
    if (g_openTime || g_closeTime) {
        if (g_openTime) {
            // Pressing the icon took the focus away. Take it back, or the
            // mixer would close for having lost it.
            KillTimer(hWnd, TIMER_CHECK_FOCUS);
            if (SetForegroundWindow(hWnd)) {
                SetFocus(hWnd);
            } else if (g_settings.closeWhenFocusIsLost) {
                SetTimer(hWnd, TIMER_CHECK_FOCUS, 150, nullptr);
            }
        }
        return;
    }
    QueueMixerAction(hWnd, close);
}

// Frees what only an open mixer needs: its frame buffers and drawn text.
void TrimMixerMemory() {
    g_blackFrame.Reset();
    g_whiteFrame.Reset();
    ClearTextCache();
}

// Takes the flyout off screen, once any closing animation has run.
void CompleteHide(HWND hWnd) {
    KillTimer(hWnd, TIMER_METERS);
    g_paintTimerInterval = 0;
    g_openTime = 0;
    g_closeTime = 0;
    g_openWarmup = false;
    g_closeQueued = false;
    ShowWindow(hWnd, SW_HIDE);
    RestFlyoutWindow(hWnd);
    // Reopening soon is much cheaper with the buffers and text still at
    // hand, so keep them for a little while.
    if (!SetTimer(hWnd, TIMER_TRIM, 10000, nullptr)) TrimMixerMemory();
}

void HideMixer(HWND hWnd) {
    FinishVolumeEntry(hWnd, false, false);
    CancelNameTooltip(hWnd);
    g_showRequestPending = false;
    KillTimer(hWnd, TIMER_CHECK_FOCUS);
    KillTimer(hWnd, TIMER_REFRESH);
    g_dragRow = DRAG_NONE;
    if (GetCapture() == hWnd) {
        ReleaseCapture();
    }
    if (g_closeTime) return; // Already on its way out.
    // Nothing of a flyout that has yet to show its first frame can be seen,
    // and the taskbar flyouts of Windows 10 close without an animation.
    if (!g_motionEnabled || !IsWindowVisible(hWnd) || g_openWarmup ||
        IsWindows10Style()) {
        CompleteHide(hWnd);
        return;
    }
    // A flyout that is still opening closes once it has arrived.
    if (g_openTime) {
        g_closeQueued = true;
        return;
    }
    g_slideStart = 0.0f;
    g_fadeStart = 1.0f;
    g_closeTime = AnimationClock();
    g_pacedSince = 0;
    SetPaintTimer(hWnd, 16);
    InvalidateRect(hWnd, nullptr, FALSE);
}

enum class TrayAction { None, Open, Menu, ToggleMute };

TrayAction GetTrayAction(UINT event, bool version4) {
    if (event == WM_MBUTTONUP) {
        return TrayAction::ToggleMute;
    }
    if (version4) {
        if (event == NIN_SELECT || event == NIN_KEYSELECT) {
            return TrayAction::Open;
        }
        if (event == WM_CONTEXTMENU) {
            return TrayAction::Menu;
        }
    } else {
        if (event == WM_LBUTTONUP) {
            return TrayAction::Open;
        }
        if (event == WM_RBUTTONUP) {
            return TrayAction::Menu;
        }
    }
    return TrayAction::None;
}

void AddTrayIcon(HWND hWnd) {
    g_trayVersion4 = false;
    g_trayUsesGuid = true;
    NOTIFYICONDATAW data = {};
    data.cbSize = sizeof(data);
    data.hWnd = hWnd;
    data.uID = TRAY_ICON_ID;
    data.guidItem = MIXER_TRAY_GUID;
    data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID | NIF_SHOWTIP;
    data.uCallbackMessage = WM_APP_TRAY;
    data.hIcon = g_trayIcon;
    wcscpy_s(data.szTip, L"Better Volume Mixer");
    if (!Shell_NotifyIconW(NIM_ADD, &data)) {
        // A GUID can remain associated with an earlier Windhawk executable path.
        // Keep the same identity mode for all later operations on this icon.
        data.uFlags &= ~NIF_GUID;
        data.guidItem = {};
        g_trayUsesGuid = false;
        if (!Shell_NotifyIconW(NIM_ADD, &data)) {
            Wh_Log(L"Mixer: tray icon registration failed");
            return;
        }
        Wh_Log(L"Mixer: tray icon registered using window/ID fallback");
    }
    Wh_Log(L"Mixer: tray icon registered");

    data.uVersion = NOTIFYICON_VERSION_4;
    g_trayVersion4 = Shell_NotifyIconW(NIM_SETVERSION, &data) != FALSE;
    if (!g_trayVersion4) {
        Wh_Log(L"Mixer: tray icon version negotiation failed");
    }
    Wh_Log(L"Mixer: tray callback mode=%s", g_trayVersion4 ? L"v4" : L"legacy");
}

void RemoveTrayIcon(HWND hWnd) {
    NOTIFYICONDATAW data = {};
    data.cbSize = sizeof(data);
    data.hWnd = hWnd;
    data.uID = TRAY_ICON_ID;
    data.uFlags = g_trayUsesGuid ? NIF_GUID : 0;
    if (g_trayUsesGuid) data.guidItem = MIXER_TRAY_GUID;
    Shell_NotifyIconW(NIM_DELETE, &data);
}

bool SpeakerShapeContains(float x, float y, bool includeWaves = true) {
    constexpr float points[][2] = {
        {1.5f, 5.5f}, {4.5f, 5.5f}, {8.0f, 2.5f},
        {8.0f, 13.5f}, {4.5f, 10.5f}, {1.5f, 10.5f}};
    bool inside = false;
    for (int i = 0, j = 5; i < 6; j = i++) {
        if ((points[i][1] > y) != (points[j][1] > y) &&
            x < (points[j][0] - points[i][0]) * (y - points[i][1]) /
                    (points[j][1] - points[i][1]) + points[i][0]) {
            inside = !inside;
        }
    }
    if (inside || !includeWaves) return inside;
    float dx = x - 6.8f, dy = y - 8.0f;
    float radius = std::sqrt(dx * dx + dy * dy);
    bool inner = dx > 2.2f && std::abs(dy) < 2.8f &&
                 std::abs(radius - 3.8f) < 0.65f;
    bool outer = dx > 3.8f && std::abs(dy) < 5.6f &&
                 std::abs(radius - 6.6f) < 0.65f;
    return inner || outer;
}

bool SpeakerMuteMarkContains(float x, float y) {
    auto segmentDistance = [](float px, float py, float ax, float ay,
                              float bx, float by) {
        float vx = bx - ax;
        float vy = by - ay;
        float wx = px - ax;
        float wy = py - ay;
        float lengthSquared = vx * vx + vy * vy;
        float t = lengthSquared > 0.0f
            ? std::max(0.0f, std::min(1.0f,
                  (wx * vx + wy * vy) / lengthSquared))
            : 0.0f;
        float dx = px - (ax + t * vx);
        float dy = py - (ay + t * vy);
        return std::sqrt(dx * dx + dy * dy);
    };

    constexpr float thickness = 0.8f;
    return segmentDistance(x, y, 10.0f, 6.0f, 14.0f, 10.0f) < thickness ||
           segmentDistance(x, y, 10.0f, 10.0f, 14.0f, 6.0f) < thickness;
}

HICON CreateSpeakerIcon(int pixels, COLORREF color, float shapeScale = 1.0f,
                        bool softenEdges = false, bool muted = false) {
    pixels = ClampInt(pixels, 16, 256);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = pixels;
    info.bmiHeader.biHeight = -pixels;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) DeleteObject(bitmap);
        return nullptr;
    }
    auto output = static_cast<DWORD*>(bits);
    const int samples = softenEdges ? 12 : 8;
    const float sampleSpan = softenEdges ? 1.35f : 1.0f;
    for (int y = 0; y < pixels; ++y) {
        for (int x = 0; x < pixels; ++x) {
            int covered = 0;
            for (int sy = 0; sy < samples; ++sy) {
                for (int sx = 0; sx < samples; ++sx) {
                    float offsetX = ((sx + 0.5f) / samples - 0.5f) * sampleSpan;
                    float offsetY = ((sy + 0.5f) / samples - 0.5f) * sampleSpan;
                    float designX = (x + 0.5f + offsetX) * 16.0f / pixels;
                    float designY = (y + 0.5f + offsetY) * 16.0f / pixels;
                    float shapeX = 8.0f + (designX - 8.0f) / shapeScale;
                    float shapeY = 8.0f + (designY - 8.0f) / shapeScale;
                    bool inside = SpeakerShapeContains(shapeX, shapeY, !muted);
                    if (muted && SpeakerMuteMarkContains(shapeX, shapeY)) {
                        inside = true;
                    }
                    covered += inside;
                }
            }
            DWORD alpha = (covered * 255 + samples * samples / 2) / (samples * samples);
            output[y * pixels + x] = (alpha << 24) |
                ((GetRValue(color) * alpha / 255) << 16) |
                ((GetGValue(color) * alpha / 255) << 8) |
                (GetBValue(color) * alpha / 255);
        }
    }
    std::vector<BYTE> maskBits(((pixels + 15) / 16) * 2 * pixels, 0);
    HBITMAP mask = CreateBitmap(pixels, pixels, 1, 1, maskBits.data());
    HICON icon = nullptr;
    if (mask) {
        ICONINFO iconInfo{};
        iconInfo.fIcon = TRUE;
        iconInfo.hbmColor = bitmap;
        iconInfo.hbmMask = mask;
        icon = CreateIconIndirect(&iconInfo);
        DeleteObject(mask);
    }
    DeleteObject(bitmap);
    return icon;
}

HICON LoadMixerIcon(int pixels, bool tray = false, bool muted = false) {
    COLORREF color = g_theme.text;
    if (tray) {
        DWORD light = 0, size = sizeof(light);
        RegGetValueW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size);
        color = light ? RGB(30, 32, 36) : RGB(248, 249, 252);
    }
    if (HICON icon = CreateSpeakerIcon(pixels, color, tray ? 1.12f : 1.0f, !tray, muted)) {
        return icon;
    }
    WCHAR systemDirectory[MAX_PATH];
    UINT length = GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
    if (length && length < ARRAYSIZE(systemDirectory)) {
        std::wstring path(systemDirectory, length);
        path += L"\\SndVol.exe";
        if (HICON icon = ExtractSizedIcon(path, pixels)) {
            return icon;
        }
    }
    return static_cast<HICON>(LoadImageW(nullptr, IDI_APPLICATION, IMAGE_ICON,
                                         pixels, pixels, 0));
}

void RefreshIconSizes(HWND hWnd) {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    UINT trayDpi = taskbar ? GetDpiForWindow(taskbar) : 96;
    if (!trayDpi) {
        trayDpi = 96;
    }
    bool muted = GetMasterMuted();
    int trayPixels = GetSystemMetricsForDpi(SM_CXSMICON, trayDpi);
    HICON tray = LoadMixerIcon(std::max(16, trayPixels), true, muted);
    if (tray) {
        if (g_hWnd.load() == hWnd) {
            NOTIFYICONDATAW data = {};
            data.cbSize = sizeof(data);
            data.hWnd = hWnd;
            data.uID = TRAY_ICON_ID;
            if (g_trayUsesGuid) data.guidItem = MIXER_TRAY_GUID;
            data.uFlags = NIF_ICON | (g_trayUsesGuid ? NIF_GUID : 0);
            data.hIcon = tray;
            if (!Shell_NotifyIconW(NIM_MODIFY, &data)) {
                Wh_Log(L"Mixer: tray DPI icon update failed");
            }
        }
        if (g_trayIcon) {
            DestroyIcon(g_trayIcon);
        }
        g_trayIcon = tray;
    }
    HICON master = LoadMixerIcon(Scale(24), false, muted);
    if (master) {
        if (g_masterIcon) {
            DestroyIcon(g_masterIcon);
        }
        g_masterIcon = master;
    }
    g_masterIconMuted = muted;
    for (auto& app : *g_apps) {
        if (app.icon) {
            DestroyIcon(app.icon);
        }
        app.icon = GetExecutableIcon(app.executablePath);
    }
}

void RefreshMasterIcons(HWND hWnd, bool force) {
    bool muted = GetMasterMuted();
    if (!force && muted == g_masterIconMuted) {
        return;
    }

    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    UINT trayDpi = taskbar ? GetDpiForWindow(taskbar) : 96;
    if (!trayDpi) trayDpi = 96;
    int trayPixels = GetSystemMetricsForDpi(SM_CXSMICON, trayDpi);
    HICON tray = LoadMixerIcon(std::max(16, trayPixels), true, muted);
    if (tray) {
        if (g_hWnd.load() == hWnd) {
            NOTIFYICONDATAW data = {};
            data.cbSize = sizeof(data);
            data.hWnd = hWnd;
            data.uID = TRAY_ICON_ID;
            if (g_trayUsesGuid) data.guidItem = MIXER_TRAY_GUID;
            data.uFlags = NIF_ICON | (g_trayUsesGuid ? NIF_GUID : 0);
            data.hIcon = tray;
            Shell_NotifyIconW(NIM_MODIFY, &data);
        }
        if (g_trayIcon) DestroyIcon(g_trayIcon);
        g_trayIcon = tray;
    }

    HICON master = LoadMixerIcon(Scale(24), false, muted);
    if (master) {
        if (g_masterIcon) DestroyIcon(g_masterIcon);
        g_masterIcon = master;
    }
    g_masterIconMuted = muted;
}

std::wstring ProcessNameFromPath(const std::wstring& path) {
    size_t separator = path.find_last_of(L"\\/");
    return separator == std::wstring::npos ? path : path.substr(separator + 1);
}

bool CopyProcessName(HWND owner, const std::wstring& name) {
    if (name.empty()) return false;
    SIZE_T bytes = (name.size() + 1) * sizeof(wchar_t);
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!memory) return false;

    auto* text = static_cast<wchar_t*>(GlobalLock(memory));
    if (!text) {
        GlobalFree(memory);
        return false;
    }

    std::wmemcpy(text, name.c_str(), name.size() + 1);
    GlobalUnlock(memory);

    if (!OpenClipboard(owner)) {
        GlobalFree(memory);
        return false;
    }

    bool copied = EmptyClipboard() &&
                  SetClipboardData(CF_UNICODETEXT, memory);
    CloseClipboard();

    if (!copied) GlobalFree(memory);
    return copied;
}

void RefreshSourceList(HWND window) {
    CancelNameTooltip(window);
    g_hoverRow = DRAG_NONE;
    RefreshAudioSessions(window);
    g_rowVisuals.clear();
    ResizeMixer(window);
    if (IsWindowVisible(window)) PositionMixer(window);
    InvalidateRect(window, nullptr, FALSE);
}

void ShowSourceMenu(HWND window, int row, POINT screenPoint) {
    if (g_sourceMenuOpen || g_outputMenuOpen || g_dragRow != DRAG_NONE ||
        row < 0 || row >= static_cast<int>(g_apps->size())) {
        return;
    }

    const std::wstring key = (*g_apps)[row].pinKey;
    const std::wstring processName =
        ProcessNameFromPath((*g_apps)[row].executablePath);

    FinishVolumeEntry(window, false, false);
    CancelNameTooltip(window);
    SelectRow(row);

    HMENU menu = CreatePopupMenu();
    if (!menu) return;

    AppendMenuW(menu,
                MF_STRING | (processName.empty() ? MF_GRAYED : 0),
                MENU_SOURCE_COPY_PROCESS,
                L"Copy process name");
    AppendMenuW(menu, MF_STRING, MENU_SOURCE_HIDE, L"Hide temporarily");

    g_sourceMenuOpen = true;
    KillTimer(window, TIMER_CHECK_FOCUS);
    SetForegroundWindow(window);

    UINT command = TrackPopupMenu(
        menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON,
        screenPoint.x, screenPoint.y, 0, window, nullptr);

    DestroyMenu(menu);

    if (IsWindow(window)) {
        if (command == MENU_SOURCE_COPY_PROCESS && !processName.empty()) {
            if (!CopyProcessName(window, processName)) {
                Wh_Log(L"Mixer: could not copy the process name to the clipboard");
                MessageBoxW(window,
                            L"Could not copy the process name. Please try again.",
                            L"Better Volume Mixer",
                            MB_OK | MB_ICONWARNING);
            }
        } else if (command == MENU_SOURCE_HIDE) {
            g_temporarilyHiddenSources.insert(key);
        }
    }

    g_sourceMenuOpen = false;
    if (g_pendingDefaultOutputChange) {
        g_pendingDefaultOutputChange = false;
        PostMessageW(window, WM_APP_DEFAULT_OUTPUT_CHANGED, 0, 0);
    }
    if (!IsWindow(window)) return;

    if (command == MENU_SOURCE_HIDE) {
        RefreshSourceList(window);
    }

    PostMessageW(window, WM_NULL, 0, 0);
    InvalidateRect(window, nullptr, FALSE);

    if (g_settings.closeWhenFocusIsLost && IsWindowVisible(window)) {
        SetTimer(window, TIMER_CHECK_FOCUS, 150, nullptr);
    }
}

void OpenClassicSound(HWND owner) {
    wchar_t directory[MAX_PATH];
    UINT length = GetSystemDirectoryW(directory, ARRAYSIZE(directory));
    if (!length || length >= ARRAYSIZE(directory)) {
        Wh_Log(L"Mixer: could not locate the Windows system directory");
        return;
    }

    std::wstring control =
        std::wstring(directory, length) + L"\\control.exe";
    HINSTANCE result = ShellExecuteW(
        owner, L"open", control.c_str(), L"/name Microsoft.Sound",
        nullptr, SW_SHOWNORMAL);

    if (reinterpret_cast<INT_PTR>(result) <= 32) {
        Wh_Log(L"Mixer: could not open the classic Sound control panel");
        MessageBoxW(owner,
                    L"Windows could not open the Sound control panel.",
                    L"Better Volume Mixer",
                    MB_OK | MB_ICONWARNING);
    }
}

void OpenSystemPage(PCWSTR uri) {
    ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL);
}

void ShowTrayMenu(HWND hWnd, int x, int y) {
    if (g_sourceMenuOpen || g_outputMenuOpen) return;

    HMENU menu = CreatePopupMenu();
    if (!menu) return;

    AppendMenuW(menu, MF_STRING,
                MENU_REFRESH, L"Refresh applications");

    if (!g_temporarilyHiddenSources.empty()) {
        AppendMenuW(menu, MF_STRING,
                    MENU_RESTORE_HIDDEN,
                    L"Restore temporarily hidden sources");
    }

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING,
                MENU_SOUND_SETTINGS, L"Sound settings");
    AppendMenuW(menu, MF_STRING,
                MENU_WINDOWS_MIXER, L"Windows volume mixer");
    AppendMenuW(menu, MF_STRING,
                MENU_CLASSIC_SOUND,
                L"Sound control panel");

    SetForegroundWindow(hWnd);
    UINT command = TrackPopupMenu(
        menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON,
        x, y, 0, hWnd, nullptr);

    PostMessageW(hWnd, WM_NULL, 0, 0);
    DestroyMenu(menu);

    if (g_pendingDefaultOutputChange) {
        g_pendingDefaultOutputChange = false;
        PostMessageW(hWnd, WM_APP_DEFAULT_OUTPUT_CHANGED, 0, 0);
    }

    switch (command) {
        case MENU_OPEN:
            QueueShowMixer(hWnd);
            break;

        case MENU_REFRESH:
            RefreshAudioSessions(hWnd, true);
            if (IsWindowVisible(hWnd)) {
                ResizeMixer(hWnd);
                PositionMixer(hWnd);
            }
            break;

        case MENU_SOUND_SETTINGS:
            OpenSystemPage(L"ms-settings:sound");
            break;

        case MENU_CLASSIC_SOUND:
            OpenClassicSound(hWnd);
            break;

        case MENU_WINDOWS_MIXER:
            OpenSystemPage(L"ms-settings:apps-volume");
            break;

        case MENU_RESTORE_HIDDEN:
            g_temporarilyHiddenSources.clear();
            RefreshSourceList(hWnd);
            break;
    }
}

// Default-output switching uses the undocumented Windows shell policy interface.
static const CLSID kCLSID_PolicyConfigClient = {
    0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}};
static const IID kIID_IPolicyConfig = {
    0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}};

struct DeviceShareModeOpaque;

// The undocumented interface used by the sound applet to change the default
// endpoint. Only SetDefaultEndpoint is called, the other methods are declared
// just to keep the vtable layout correct.
struct IPolicyConfig : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, WAVEFORMATEX*, WAVEFORMATEX*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PINT64, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, DeviceShareModeOpaque*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, DeviceShareModeOpaque*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR deviceId, ERole role) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

struct OutputDevice {
    std::wstring id;
    std::wstring name;
};

HRESULT SetPlaybackOutput(const std::wstring& id) {
    IPolicyConfig* policy = nullptr;
    HRESULT result = CoCreateInstance(kCLSID_PolicyConfigClient, nullptr, CLSCTX_INPROC_SERVER,
                                      kIID_IPolicyConfig, reinterpret_cast<void**>(&policy));
    if (SUCCEEDED(result)) {
        // Change ordinary playback only. Preserve the communications output.
        result = policy->SetDefaultEndpoint(id.c_str(), eMultimedia);
        if (SUCCEEDED(result)) result = policy->SetDefaultEndpoint(id.c_str(), eConsole);
        policy->Release();
    }
    return result;
}

LRESULT CALLBACK OutputMenuMessageFilter(int code, WPARAM wParam, LPARAM lParam) {
    if (code == MSGF_MENU && g_outputMenuOpen) {
        const MSG* message = reinterpret_cast<const MSG*>(lParam);
        HWND window = g_hWnd.load();
        if (window && (message->message == WM_LBUTTONDOWN ||
                       message->message == WM_LBUTTONDBLCLK) &&
            WindowFromPoint(message->pt) == window) {
            RECT client{};
            GetClientRect(window, &client);
            RECT picker = OutputPickerRect(client.right);
            POINT point = message->pt;
            ScreenToClient(window, &point);
            if (PtInRect(&picker, point)) {
                EndMenu();
                return TRUE; // This click only closes the list; never replay it.
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void ShowOutputPicker(HWND window) {
    CancelNameTooltip(window);
    if (g_outputMenuOpen || g_dragRow != DRAG_NONE) return;
    g_outputMenuOpen = true;
    KillTimer(window, TIMER_CHECK_FOCUS);
    std::vector<OutputDevice> outputs;
    std::wstring currentId;
    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT result = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
        CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator),
        reinterpret_cast<void**>(&enumerator));
    if (SUCCEEDED(result)) {
        IMMDevice* current = nullptr;
        if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &current))) {
            LPWSTR id = nullptr;
            if (SUCCEEDED(current->GetId(&id)) && id) {
                currentId = id; CoTaskMemFree(id);
            }
            current->Release();
        }
        IMMDeviceCollection* collection = nullptr;
        result = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &collection);
        if (SUCCEEDED(result)) {
            UINT count = 0; collection->GetCount(&count);
            for (UINT i = 0; i < count; ++i) {
                IMMDevice* device = nullptr;
                if (FAILED(collection->Item(i, &device))) continue;
                LPWSTR id = nullptr;
                if (SUCCEEDED(device->GetId(&id)) && id) {
                    outputs.push_back({id, ReadEndpointName(device)});
                    CoTaskMemFree(id);
                }
                device->Release();
            }
            collection->Release();
        }
        enumerator->Release();
    }
    std::sort(outputs.begin(), outputs.end(), [](const auto& a, const auto& b) {
        int order = _wcsicmp(a.name.c_str(), b.name.c_str());
        return order ? order < 0 : a.id < b.id;
    });
    HMENU menu = CreatePopupMenu();
    UINT selected = 0;
    if (menu) {
        for (size_t i = 0; i < outputs.size(); ++i) {
            std::wstring label;
            for (wchar_t c : outputs[i].name) {
                label += c;
                if (c == L'&') label += L'&';
            }
            AppendMenuW(menu, MF_STRING | (outputs[i].id == currentId ? MF_CHECKED : 0),
                        i + 1, label.c_str());
        }
        if (outputs.empty()) AppendMenuW(menu, MF_STRING | MF_GRAYED, 0,
            FAILED(result) ? L"Could not load audio outputs" : L"No available audio outputs");
        RECT client{}; GetClientRect(window, &client);
        RECT picker = OutputPickerRect(client.right);
        POINT anchor{picker.left, picker.bottom}; ClientToScreen(window, &anchor);
        SetForegroundWindow(window);
        HHOOK filter = SetWindowsHookExW(WH_MSGFILTER, OutputMenuMessageFilter,
                                         nullptr, GetCurrentThreadId());
        if (!filter) Wh_Log(L"Mixer: output-menu click filter failed, error %lu", GetLastError());
        selected = TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_LEFTALIGN |
                                    TPM_TOPALIGN | TPM_RIGHTBUTTON, anchor.x, anchor.y,
                                    window, nullptr);
        if (filter) UnhookWindowsHookEx(filter);
        DestroyMenu(menu);
    }
    if (!IsWindow(window)) {
        g_outputMenuOpen = false;
        return;
    }
    if (selected && selected <= outputs.size()) {
        result = SetPlaybackOutput(outputs[selected - 1].id);
        if (FAILED(result)) {
            Wh_Log(L"Mixer: output switch failed: 0x%08lX", static_cast<unsigned long>(result));
            MessageBoxW(window, L"Windows could not fully switch the audio output. The device may have disconnected."
                        L"\n\nYou can also select an output in Windows Sound settings.",
                        L"Better Volume Mixer", MB_OK | MB_ICONWARNING);
        }
        SelectRow(DRAG_MASTER);
        RefreshAudioSessions(window, true);
        g_scrollRow = 0;
        g_rowVisuals.clear();
        PositionMixer(window);
    }
    g_outputMenuOpen = false;
    if (g_pendingDefaultOutputChange) {
        g_pendingDefaultOutputChange = false;
        PostMessageW(window, WM_APP_DEFAULT_OUTPUT_CHANGED, 0, 0);
    }
    InvalidateRect(window, nullptr, FALSE);
    if (g_settings.closeWhenFocusIsLost && IsWindowVisible(window))
        SetTimer(window, TIMER_CHECK_FOCUS, 150, nullptr);
}

void HandlePointerMove(HWND hWnd, POINT point) {
    UpdateNameTooltip(hWnd, point);
    RECT client;
    GetClientRect(hWnd, &client);
    RECT closeRect = CloseButtonRect(client.right);
    RECT pickerRect = OutputPickerRect(client.right);
    bool outputHovered = PtInRect(&pickerRect, point) != FALSE;
    if (outputHovered != g_outputHovered) {
        g_outputHovered = outputHovered;
        InvalidateRect(hWnd, nullptr, FALSE);
    }
    bool closeHovered = PtInRect(&closeRect, point) != FALSE;
    int pageHovered = -1;
    if (PageCount() > 1) {
        for (bool next : {false, true}) {
            RECT button = PageButtonRect(next, client);
            if (PtInRect(&button, point)) pageHovered = next ? 1 : 0;
        }
    }
    if (pageHovered != g_hoverPageButton) {
        g_hoverPageButton = pageHovered;
        InvalidateRect(hWnd, nullptr, FALSE);
    }
    if (closeHovered != g_closeHovered) {
        g_closeHovered = closeHovered;
        InvalidateRect(hWnd, nullptr, FALSE);
    }
    int row = RowFromPoint(point);
    bool thumbHovered = false;
    if (row != DRAG_NONE) {
        RECT slider = SliderRectForRow(VisibleRowForDataRow(row), client.right);
        float volume = row == DRAG_MASTER ? g_masterPaintVolume
                                          : (*g_apps)[row].volume;
        thumbHovered = SliderThumbContains(slider, volume, point);
    }
    g_pointer = point;
    if (row != g_hoverRow || thumbHovered != g_hoverThumb) {
        g_hoverRow = row;
        g_hoverThumb = thumbHovered;
        InvalidateRect(hWnd, nullptr, FALSE);
    }

    if (!g_mouseTracking) {
        TRACKMOUSEEVENT tracking = {sizeof(tracking), TME_LEAVE, hWnd, 0};
        TrackMouseEvent(&tracking);
        g_mouseTracking = true;
    }

    if (g_dragRow != DRAG_NONE && (GetKeyState(VK_LBUTTON) & 0x8000)) {
        ApplyVolumeFromPoint(hWnd, g_dragRow, point.x);
    }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam) {
    if (message == g_taskbarCreatedMessage && g_taskbarCreatedMessage) {
        AddTrayIcon(hWnd);
        return 0;
    }

    // A flyout on its way out no longer reacts to input. One that is still
    // on its way in is put in place first, and takes a click where the
    // pointer then is over it.
    if (g_closeTime) {
        if ((message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) ||
            (message >= WM_KEYFIRST && message <= WM_KEYLAST) ||
            message == WM_CONTEXTMENU) {
            return 0;
        }
    } else if (g_openTime && (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN ||
                              message == WM_MOUSEWHEEL || message == WM_CONTEXTMENU)) {
        SettleFlyout(hWnd);
        if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN) {
            DWORD position = GetMessagePos();
            POINT point = {GET_X_LPARAM(position), GET_Y_LPARAM(position)};
            ScreenToClient(hWnd, &point);
            lParam = MAKELPARAM(point.x, point.y);
        }
    }

    switch (message) {
        case WM_CREATE: {
            Gdiplus::GdiplusStartupInput startup;
            if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &startup, nullptr) != Gdiplus::Ok) {
                g_gdiplusToken = 0;
                Wh_Log(L"Mixer: typography initialization failed; using GDI text");
            }
            CreateFonts(hWnd);
            UpdateTheme();

            // Bind the master endpoint before anything queries its mute state.
            // This registers the IAudioEndpointVolumeCallback immediately so
            // the tray icon and middle-click mute stay accurate even while the
            // mixer is hidden.
            BindDefaultEndpointVolume(hWnd);

            RefreshIconSizes(hWnd);
            ApplyTransparencyStyle(hWnd);
            ApplyWindowFrame(hWnd);
            if (HasDefaultVolumeRules()) {
                RefreshAudioSessions(hWnd);
                StartDefaultVolumeNotifications(hWnd);
            }
            RefreshMasterIcons(hWnd, true);

            // Lifetime watcher: react to default-output changes even when the
            // mixer is hidden, so tray mute controls always target the current
            // device. CoCreateInstance here runs on the UI thread and the
            // notifications are marshalled through the window message below.
            IMMDeviceEnumerator* uiEnumerator = nullptr;
            if (SUCCEEDED(CoCreateInstance(
                    __uuidof(MMDeviceEnumerator), nullptr,
                    CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator),
                    reinterpret_cast<void**>(&uiEnumerator))) &&
                uiEnumerator) {
                auto* uiNotification = new EndpointNotification(nullptr);
                uiNotification->SetWindow(hWnd);
                if (SUCCEEDED(uiEnumerator->RegisterEndpointNotificationCallback(
                        uiNotification))) {
                    g_uiDeviceEnumerator = uiEnumerator;
                    g_uiEndpointNotification = uiNotification;
                } else {
                    Wh_Log(L"Mixer: default output notification registration failed");
                    uiNotification->Release();
                    uiEnumerator->Release();
                }
            }
            return 0;
        }

        case WM_APP_TRAY: {
            UINT event = g_trayVersion4 ? LOWORD(lParam)
                                        : static_cast<UINT>(lParam);
            if (event == WM_LBUTTONDOWN || event == WM_LBUTTONDBLCLK) {
                RememberTrayPress(hWnd);
            }
            TrayAction action = GetTrayAction(event, g_trayVersion4);
            if (action != TrayAction::None) {
                Wh_Log(L"Mixer: tray action event=0x%X", event);
            }
            if (action == TrayAction::ToggleMute) {
                ToggleRowMute(DRAG_MASTER);
                InvalidateRect(hWnd, nullptr, FALSE);
            } else if (action == TrayAction::Open) {
                QueueTrayToggle(hWnd, event == NIN_KEYSELECT);
            } else if (action == TrayAction::Menu) {
                g_trayPressKnown = false;
                // WM_CONTEXTMENU coordinates in the tray callback are not
                // guaranteed. Anchor keyboard menus at the icon, others at
                // the cursor, rather than decoding an undefined wParam.
                POINT cursor = {};
                GetCursorPos(&cursor);
                RECT iconRect = {};
                if (GetKeyState(VK_RBUTTON) >= 0 && GetTrayIconRect(&iconRect)) {
                    cursor = {iconRect.left, iconRect.bottom};
                }
                ShowTrayMenu(hWnd, cursor.x, cursor.y);
            }
            return 0;
        }

        case WM_APP_SHOW_MIXER:
            if (g_showRequestPending) {
                g_showRequestPending = false;
                if (g_closeRequestPending) {
                    HideMixer(hWnd);
                } else {
                    ShowMixer(hWnd);
                }
            }
            return 0;

        case WM_APP_FINISH_VOLUME_ENTRY:
            if (g_volumeEntry && static_cast<ULONG_PTR>(lParam) == g_volumeEntryGeneration) {
                FinishVolumeEntry(hWnd, wParam != 0, wParam != 0);
            }
            return 0;

        case WM_APP_AUDIO_CHANGED:
            g_audioUiRefreshPosted.store(false);
            if (MixerOpen(hWnd)) {
                RefreshAudioSessions(hWnd);
                PositionMixer(hWnd);
                POINT point{};
                if (GetCursorPos(&point)) {
                    ScreenToClient(hWnd, &point);
                    UpdateNameTooltip(hWnd, point);
                }
            }
            return 0;

        case WM_APP_MASTER_STATE_CHANGED:
            RefreshMasterIcons(hWnd);
            return 0;

        case WM_APP_DEFAULT_OUTPUT_CHANGED: {
            // A popup menu's modal loop delivers this message while the menu
            // is on screen. Shell_NotifyIconW (via RefreshMasterIcons) talks
            // to the taskbar and can demote the popup inside the topmost
            // band; the STA COM calls in BindDefaultEndpointVolume re-enter
            // the menu's message pump. Defer the whole update until the menu
            // closes and re-post the message from the menu paths below.
            if (g_sourceMenuOpen || g_outputMenuOpen) {
                g_pendingDefaultOutputChange = true;
                return 0;
            }
            bool canRefresh =
                MixerOpen(hWnd) && g_dragRow == DRAG_NONE &&
                !g_volumeEntry;
            if (canRefresh) {
                RefreshAudioSessions(hWnd);
                PositionMixer(hWnd);
            } else {
                BindDefaultEndpointVolume(hWnd);
            }
            RefreshMasterIcons(hWnd, true);
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }

        case WM_APP_RELOAD_SETTINGS:
            FinishFlyoutMotion(hWnd);
            FinishVolumeEntry(hWnd, false, false);
            CancelNameTooltip(hWnd);
            SelectRow(DRAG_MASTER);
            StopDefaultVolumeNotifications();
            LoadSettings();
            CreateFonts(hWnd);  // The visual style selects the font families.
            if (IsWindowVisible(hWnd)) {
                SetTimer(hWnd, TIMER_REFRESH, 1000, nullptr);
            } else {
                KillTimer(hWnd, TIMER_REFRESH);
            }
            StartDefaultVolumeNotifications(hWnd);
            g_transparencyFailed = false;
            // The frosted glass backdrop is tinted with the theme color.
            UpdateTheme();
            ApplyTransparencyStyle(hWnd);
            ApplyWindowFrame(hWnd);
            UpdateMotionPreference(hWnd);
            RefreshIconSizes(hWnd);
            RefreshAudioSessions(hWnd);
            ResizeMixer(hWnd);
            if (IsWindowVisible(hWnd)) {
                PositionMixer(hWnd);
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;

        case WM_TIMER:
            if (wParam == TIMER_NAME_TOOLTIP) {
                ShowNameTooltip(hWnd);
                return 0;
            }
            if (wParam == TIMER_TRIM) {
                KillTimer(hWnd, TIMER_TRIM);
                if (!IsWindowVisible(hWnd)) TrimMixerMemory();
                return 0;
            }
            if (wParam == TIMER_CHECK_FOCUS) {
                KillTimer(hWnd, TIMER_CHECK_FOCUS);
                HWND foreground = GetForegroundWindow();
                bool mixerOwnsFocus = foreground == hWnd ||
                    (foreground && GetAncestor(foreground, GA_ROOTOWNER) == hWnd);
                if (!g_showingMixer && !g_showRequestPending && !g_outputMenuOpen &&
                    !g_sourceMenuOpen && g_settings.closeWhenFocusIsLost && MixerOpen(hWnd) &&
                    !mixerOwnsFocus && GetCapture() != hWnd) {
                    Wh_Log(L"Mixer: closing after confirmed focus loss");
                    HideMixer(hWnd);
                }
                return 0;
            }
            if (wParam == TIMER_METERS && IsWindowVisible(hWnd)) {
                // Paint messages drive the opening and closing animations.
                // Should they ever stop coming, do not leave the flyout
                // stranded half way.
                ULONGLONG sliding = g_closeTime ? g_closeTime : g_openTime;
                if (sliding && AnimationClock() - sliding > 1000000) {
                    FinishFlyoutMotion(hWnd);
                }
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (wParam == TIMER_REFRESH) {
                // KillTimer doesn't remove a timer message already in the queue.
                if (MixerOpen(hWnd)) {
                    RefreshAudioSessions(hWnd);
                    PositionMixer(hWnd);
                    POINT point{};
                    if (GetCursorPos(&point)) {
                        ScreenToClient(hWnd, &point);
                        UpdateNameTooltip(hWnd, point);
                    }
                }
                return 0;
            }
            break;

        case WM_PAINT:
            if (IsWindowVisible(hWnd)) {
                PaintMixer(hWnd);
            } else {
                ValidateRect(hWnd, nullptr);
            }
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEMOVE: {
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            HandlePointerMove(hWnd, point);
            return 0;
        }

        case WM_MOUSELEAVE:
            CancelNameTooltip(hWnd);
            g_outputHovered = false;
            g_mouseTracking = false;
            g_hoverRow = DRAG_NONE;
            g_hoverThumb = false;
            g_closeHovered = false;
            g_hoverPageButton = -1;
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;

        case WM_LBUTTONDOWN: {
            CancelNameTooltip(hWnd);
            FinishVolumeEntry(hWnd, false, false);
            SetFocus(hWnd);
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            RECT client;
            GetClientRect(hWnd, &client);

            RECT closeRect = CloseButtonRect(client.right);
            if (PtInRect(&closeRect, point)) {
                HideMixer(hWnd);
                return 0;
            }

            RECT pickerRect = OutputPickerRect(client.right);
            if (PtInRect(&pickerRect, point)) {
                ShowOutputPicker(hWnd);
                return 0;
            }
            if (PageCount() > 1) {
                for (bool next : {false, true}) {
                    RECT button = PageButtonRect(next, client);
                    if (PtInRect(&button, point)) {
                        ChangePage(hWnd, next ? 1 : -1);
                        return 0;
                    }
                }
            }

            int row = RowFromPoint(point);
            if (row == DRAG_NONE) {
                return 0;
            }
            SelectRow(row);
            InvalidateRect(hWnd, nullptr, FALSE);
            int visibleRow = VisibleRowForDataRow(row);
            RECT percentRect = PercentRectForRow(visibleRow, client.right);
            if (g_settings.exactVolumeEntry && PtInRect(&percentRect, point)) {
                BeginVolumeEntry(hWnd, row);
                return 0;
            }
            RECT pinRect = PinRectForRow(visibleRow, client.right);
            if (row >= 0 && PtInRect(&pinRect, point)) {
                ToggleSourcePin(hWnd, row);
                return 0;
            }
            RECT muteRect = MuteRectForRow(visibleRow, client.right);
            if (PtInRect(&muteRect, point)) {
                ToggleRowMute(row);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }

            RECT sliderRect = SliderHitRect(visibleRow, client.right);
            if (PtInRect(&sliderRect, point)) {
                g_dragRow = row;
                SetCapture(hWnd);
                ApplyVolumeFromPoint(hWnd, row, point.x);
            }
            return 0;
        }

        case WM_CONTEXTMENU: {
            if (reinterpret_cast<HWND>(wParam) != hWnd) break;

            POINT screenPoint{
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            };
            int row;

            if (screenPoint.x == -1 && screenPoint.y == -1) {
                row = g_selectedRow;
                if (row < g_scrollRow ||
                    row >= g_scrollRow + VisibleAppCount()) {
                    return 0;
                }

                RECT client{};
                GetClientRect(hWnd, &client);
                RECT name =
                    NameRectForRow(VisibleRowForDataRow(row), client.right);
                screenPoint = {name.left, name.bottom};
                ClientToScreen(hWnd, &screenPoint);
            } else {
                POINT clientPoint = screenPoint;
                ScreenToClient(hWnd, &clientPoint);

                RECT client{};
                GetClientRect(hWnd, &client);
                if (!PtInRect(&client, clientPoint)) return 0;

                row = RowFromPoint(clientPoint);
            }

            ShowSourceMenu(hWnd, row, screenPoint);
            return 0;
        }

        case WM_MBUTTONDOWN: {
            CancelNameTooltip(hWnd);
            if (g_dragRow != DRAG_NONE) return 0;
            FinishVolumeEntry(hWnd, false, false);
            int row = RowFromPoint({GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
            if (row != DRAG_NONE) {
                SelectRow(row);
                ToggleRowMute(row);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONUP:
            if (g_dragRow != DRAG_NONE) {
                g_dragRow = DRAG_NONE;
                ReleaseCapture();
            }
            return 0;

        case WM_CAPTURECHANGED:
            g_dragRow = DRAG_NONE;
            return 0;

        case WM_MOUSEWHEEL: {
            if (g_volumeEntry || g_dragRow != DRAG_NONE) return 0;
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hWnd, &point);
            RECT client;
            GetClientRect(hWnd, &client);
            if (!PtInRect(&client, point)) return 0;
            int row = RowFromPoint(point);

            int notches = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            if (row != DRAG_NONE && notches) {
                float current = row == DRAG_MASTER ? GetMasterVolume()
                                                   : (*g_apps)[row].volume;
                SetRowVolume(row, current + notches *
                                              g_settings.volumeStep / 100.0f);
                InvalidateRect(hWnd, nullptr, FALSE);
            } else if (!g_apps->empty() && notches) {
                ChangePage(hWnd, -notches);
            }
            return 0;
        }

        case WM_KEYDOWN:
            if (HandleRowKey(hWnd, wParam, lParam)) return 0;
            if (wParam == VK_ESCAPE) {
                HideMixer(hWnd);
                return 0;
            }
            if (wParam == VK_PRIOR || wParam == VK_NEXT) {
                ChangePage(hWnd, wParam == VK_NEXT ? 1 : -1);
                return 0;
            }
            break;

        case WM_ACTIVATE:
            if (LOWORD(wParam) != WA_INACTIVE) {
                KillTimer(hWnd, TIMER_CHECK_FOCUS);
            } else if (!g_showingMixer && !g_showRequestPending && !g_outputMenuOpen && !g_sourceMenuOpen &&
                g_settings.closeWhenFocusIsLost && MixerOpen(hWnd)) {
                // Capture intent even if activation arrives before the tray's
                // mouse-down callback. Focus dismissal remains independent.
                POINT cursor = {};
                RECT iconRect = {};
                if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) &&
                    GetCursorPos(&cursor) && GetTrayIconRect(&iconRect) &&
                    PtInRect(&iconRect, cursor)) {
                    RememberTrayPress(hWnd);
                }
                // Do not hide inside a synchronous activation transition.
                // Recheck the actual foreground window after the shell settles.
                SetTimer(hWnd, TIMER_CHECK_FOCUS, 150, nullptr);
            }
            break;

        case WM_DWMCOLORIZATIONCOLORCHANGED:
            // The slider and other accent-colored controls use g_theme.accent.
            // Refresh it immediately when Windows changes the system accent,
            // even when the mixer itself is forced to the light or dark theme.
            UpdateTheme();
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case WM_SETTINGCHANGE: {
            UpdateMotionPreference(hWnd);
            bool immersiveColorChanged =
                lParam && wcscmp(reinterpret_cast<PCWSTR>(lParam),
                                 L"ImmersiveColorSet") == 0;
            if (g_settings.theme == L"system" || immersiveColorChanged) {
                UpdateTheme();
                ApplyBackdropBlur(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            if (immersiveColorChanged) {
                RefreshIconSizes(hWnd);
            }
            break;
        }

        case WM_DISPLAYCHANGE:
            FinishVolumeEntry(hWnd, false, false);
            CancelNameTooltip(hWnd);
            RefreshIconSizes(hWnd);
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case WM_DPICHANGED: {
            FinishFlyoutMotion(hWnd);
            FinishVolumeEntry(hWnd, false, false);
            CancelNameTooltip(hWnd);
            g_dpi = HIWORD(wParam);
            CreateFonts(hWnd);
            RefreshIconSizes(hWnd);
            RECT* suggested = reinterpret_cast<RECT*>(lParam);
            SetWindowPos(hWnd, nullptr, suggested->left, suggested->top,
                         PopupWidth(), PopupHeight(),
                         SWP_NOZORDER | SWP_NOACTIVATE);
            PositionMixer(hWnd);
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }

        case WM_DESTROY:
            FinishVolumeEntry(hWnd, false, false);
            CancelNameTooltip(hWnd);
            StopDefaultVolumeNotifications();

            if (g_uiDeviceEnumerator && g_uiEndpointNotification) {
                g_uiDeviceEnumerator->UnregisterEndpointNotificationCallback(
                    g_uiEndpointNotification);
            }
            if (g_uiEndpointNotification) {
                g_uiEndpointNotification->Release();
                g_uiEndpointNotification = nullptr;
            }
            if (g_uiDeviceEnumerator) {
                g_uiDeviceEnumerator->Release();
                g_uiDeviceEnumerator = nullptr;
            }

            if (g_nameTooltip) {
                DestroyWindow(g_nameTooltip);
                g_nameTooltip = nullptr;
            }
            g_blackFrame.Reset();
            g_whiteFrame.Reset();
            KillTimer(hWnd, TIMER_METERS);
            KillTimer(hWnd, TIMER_REFRESH);
            KillTimer(hWnd, TIMER_CHECK_FOCUS);
            KillTimer(hWnd, TIMER_TRIM);
            g_showRequestPending = false;
            g_openTime = 0;
            g_closeTime = 0;
            g_closeQueued = false;
            RemoveTrayIcon(hWnd);
            ReleaseAudioData();
            g_activatedSessions.clear();
            g_liveSessions.clear();
            g_temporarilyHiddenSources.clear();
            AcquireSRWLockExclusive(&g_defaultVolumeLock);
            g_appliedDefaultVolumes.clear();
            ReleaseSRWLockExclusive(&g_defaultVolumeLock);
            g_liveDefaultSessions.clear();
            g_rowVisuals.clear();
            ClearIconCache();
            if (g_masterIcon) {
                DestroyIcon(g_masterIcon);
                g_masterIcon = nullptr;
            }
            DeleteFonts();
            if (g_gdiplusToken) {
                Gdiplus::GdiplusShutdown(g_gdiplusToken);
                g_gdiplusToken = 0;
            }
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

constexpr PCWSTR WINDOW_CLASS_NAME = L"WindhawkBetterTaskbarVolumeMixer";

DWORD WINAPI ThreadProc(LPVOID) {
    Wh_Log(L"Mixer: UI thread started, PID=%lu", GetCurrentProcessId());
    if (!SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
        Wh_Log(L"Mixer: per-monitor DPI setup failed, error %lu", GetLastError());
    }
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    Wh_Log(L"Mixer: COM initialization result=0x%08lX",
           static_cast<unsigned long>(comResult));
    if (FAILED(comResult)) {
        SetEvent(g_windowReadyEvent);
        return 1;
    }

    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    // No CS_DROPSHADOW: that shadow stays behind when the flyout slides open,
    // and Windows 11 gives a rounded window a shadow of its own.
    windowClass.style = CS_DBLCLKS;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassExW failed, error %u", GetLastError());
        SetEvent(g_windowReadyEvent);
        if (g_trayIcon) {
            DestroyIcon(g_trayIcon);
            g_trayIcon = nullptr;
        }
        if (SUCCEEDED(comResult)) {
            CoUninitialize();
        }
        return 1;
    }

    WNDCLASSEXW entryClass{};
    entryClass.cbSize = sizeof(entryClass);
    entryClass.lpfnWndProc = VolumeEntryProc;
    entryClass.hInstance = windowClass.hInstance;
    entryClass.hCursor = LoadCursorW(nullptr, IDC_IBEAM);
    entryClass.lpszClassName = VOLUME_ENTRY_CLASS;
    if (!RegisterClassExW(&entryClass)) {
        UnregisterClassW(WINDOW_CLASS_NAME, windowClass.hInstance);
        SetEvent(g_windowReadyEvent);
        CoUninitialize();
        return 1;
    }

    WNDCLASSEXW tooltipClass{};
    tooltipClass.cbSize = sizeof(tooltipClass);
    tooltipClass.lpfnWndProc = NameTooltipProc;
    tooltipClass.hInstance = windowClass.hInstance;
    tooltipClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    tooltipClass.lpszClassName = NAME_TOOLTIP_CLASS;
    if (!RegisterClassExW(&tooltipClass)) {
        UnregisterClassW(VOLUME_ENTRY_CLASS, windowClass.hInstance);
        UnregisterClassW(WINDOW_CLASS_NAME, windowClass.hInstance);
        SetEvent(g_windowReadyEvent);
        CoUninitialize();
        return 1;
    }

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST, WINDOW_CLASS_NAME, L"Volume mixer",
        WS_POPUP, 0, 0, 420, 400, nullptr, nullptr, windowClass.hInstance,
        nullptr);
    if (!hWnd) {
        Wh_Log(L"CreateWindowExW failed, error %u", GetLastError());
        UnregisterClassW(NAME_TOOLTIP_CLASS, windowClass.hInstance);
        UnregisterClassW(VOLUME_ENTRY_CLASS, windowClass.hInstance);
        UnregisterClassW(WINDOW_CLASS_NAME, windowClass.hInstance);
        SetEvent(g_windowReadyEvent);
        if (g_trayIcon) {
            DestroyIcon(g_trayIcon);
            g_trayIcon = nullptr;
        }
        if (SUCCEEDED(comResult)) {
            CoUninitialize();
        }
        return 1;
    }

    g_hWnd.store(hWnd);
    Wh_Log(L"Mixer: window created, HWND=%p", static_cast<void*>(hWnd));
    AddTrayIcon(hWnd);
    SetEvent(g_windowReadyEvent);

    MSG message;
    BOOL result;
    while ((result = GetMessageW(&message, nullptr, 0, 0)) != 0) {
        if (result == -1) {
            Wh_Log(L"Mixer: GetMessage failed, error %lu", GetLastError());
            break;
        }
        if (message.hwnd == g_volumeEdit && message.message == WM_KEYDOWN) {
            if (message.wParam == VK_RETURN || message.wParam == VK_ESCAPE) {
                FinishVolumeEntry(hWnd, message.wParam == VK_RETURN, true);
                continue;
            }
            if (message.wParam == 'A' && (GetKeyState(VK_CONTROL) & 0x8000)) {
                SendMessageW(g_volumeEdit, EM_SETSEL, 0, -1);
                continue;
            }
        }
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (IsWindow(hWnd)) DestroyWindow(hWnd);
    g_hWnd.store(nullptr);
    g_apps.reset();
    UnregisterClassW(NAME_TOOLTIP_CLASS, windowClass.hInstance);
    UnregisterClassW(VOLUME_ENTRY_CLASS, windowClass.hInstance);
    UnregisterClassW(WINDOW_CLASS_NAME, windowClass.hInstance);
    if (g_trayIcon) {
        DestroyIcon(g_trayIcon);
        g_trayIcon = nullptr;
    }

    if (SUCCEEDED(comResult)) {
        CoUninitialize();
    }
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L"Mixer: initializing tool");
    LoadSettings();

    g_windowReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_windowReadyEvent) {
        Wh_Log(L"Mixer: CreateEvent failed, error %lu", GetLastError());
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"Mixer: CreateThread failed, error %lu", GetLastError());
        CloseHandle(g_windowReadyEvent);
        g_windowReadyEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_windowReadyEvent) {
        WaitForSingleObject(g_windowReadyEvent, 5000);
    }
    if (HWND hWnd = g_hWnd.load()) {
        PostMessageW(hWnd, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_windowReadyEvent) {
        WaitForSingleObject(g_windowReadyEvent, 5000);
    }
    if (HWND hWnd = g_hWnd.load()) {
        PostMessageW(hWnd, WM_CLOSE, 0, 0);
    }
    if (g_thread) {
        DWORD result = WaitForSingleObject(g_thread, 5000);
        if (result != WAIT_OBJECT_0) {
            // The caller immediately exits this dedicated tool process. Keep
            // handles alive until then: the UI thread might still use them.
            Wh_Log(L"Mixer: UI shutdown did not complete; exiting the dedicated tool process");
            return;
        }
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_windowReadyEvent) {
        CloseHandle(g_windowReadyEvent);
        g_windowReadyEvent = nullptr;
    }
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
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
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
